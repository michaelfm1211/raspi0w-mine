#include "drivers/bluetooth.h"
#include "drivers/irq.h"
#include "drivers/isr.h"
#include "drivers/mailbox.h"
#include "drivers/systimer.h"
#include "drivers/uart1.h"
#include "kernel/io.h"
#include "lib/string.h"
#include <stdint.h>

extern uint32_t __start;
extern uint32_t __end;

static void print_mac_addr() {
  memset((void *)mailbox_buffer, 0, sizeof(mailbox_buffer));
  mailbox_buffer[0] = 8 * sizeof(uint32_t); // buffer size
  mailbox_buffer[1] = MAILBOX_REQUEST;      // a request
  // <tag get board mac address>
  mailbox_buffer[2] = MAILBOX_TAG_BOARD_MAC; // tag id
  mailbox_buffer[3] = 4 * 2;                 // tag value buffer size
  mailbox_buffer[4] = 0;                     // tag request code
  mailbox_buffer[5] = 0;                     // tag value buffer
  mailbox_buffer[6] = 0;                     // tag value buffer
  // </tag get board model>
  // <tag end>
  mailbox_buffer[7] = MAILBOX_TAG_END; // tag value buffer
  // </tag end>

  if (mailbox_call(MAILBOX_CHANNEL_TAGS_ARM2VC)) {
    uint8_t *mac = (uint8_t *)(&mailbox_buffer[5]);
    io_printf(&uart1, "MAC address: %x:%x:%x:%x:%x:%x\n", mac[0], mac[1],
              mac[2], mac[3], mac[4], mac[5]);
  } else {
    io_printf(&uart1, "Error getting MAC address: mailbox call failed\n");
  }
}

static void print_diagnostics() {
  io_printf(&uart1, "=== raspi0w-mine Diagnostics ===\n");
  io_printf(&uart1, "Kernel start: 0x%x\n", &__start);
  io_printf(&uart1, "Kernel end: 0x%x\n", &__end);
  io_printf(&uart1, "Kernel size: %u bytes\n", &__end - &__start);

  uint32_t cpsr;
  asm volatile("mrs %0, cpsr" : "=r"(cpsr));
  io_printf(&uart1, "CPSR: 0x%x\n", cpsr);
  const char *modes[] = {"User",   "FIQ",    "IRQ",        "Supervisor",
                         "0b0100", "0b0101", "Monitor",    "Abort",
                         "0b1000", "0b1001", "Hypervisor", "Undefined",
                         "0b1100", "0b1101", "0b1110",     "System"};
  io_printf(&uart1, "Mode: %d (%s)\n", cpsr & 0xF, modes[cpsr & 0xF]);
  io_printf(&uart1, "=== End of Diagnostics ===\n");
}

static void eddystone_beacon() {
  int err;

  // set event mask to receive all events
  uint8_t event_mask[8] = {0};
  event_mask[0] = 0xff;
  err = bluetooth_hci_command(BLUETOOTH_HCI_CMD_LE_SET_EVENT_MASK, event_mask,
      8, 0, 0);
  if (err) {
    io_printf(&uart1, "Error setting event mask. Code: %u\n", err);
    return;
  }

  // set advertising parameters
  uint16_t interval_min = 100 / 0.625;
  uint16_t interval_max = 100 / 0.625;
  uint8_t adv_params[15] = {
      interval_min & 0xff,       // interval_min low
      interval_min >> 8,         // interval_min high
      interval_max & 0xff,       // interval_max low
      interval_max >> 8,         // interval_max high
      BLUETOOTH_ADV_NONCONN_IND, // advertising type
      0,                         // own address type (Public Device Address)
      0,                         // peer address type (N/A for ADV_NONCONN_IND)
      0,
      0,
      0,
      0,
      0,
      0,   // peer address (N/A for ADV_NONCONN_IND)
      0x7, // advertising channel map (all channels)
      0,   // filter policy (disable the whitelist)
  };
  err = bluetooth_hci_command(BLUETOOTH_HCI_CMD_LE_SET_ADV_PARAMS, adv_params,
      15, 0, 0);
  if (err) {
    io_printf(&uart1, "Error setting advertising parameters. Code: %u\n", err);
    return;
  }

  // set advertising data
  // generated with https://yencarnacion.github.io/eddystone-url-calculator/
  uint8_t adv_data[32] = {
      0x15, // length of payload
      0x02, // length of next secion
      0x01, // section type (flags)
      0x06, // flag for low energy advertising mode
      0x03, // length of next section
      0x03, // section type (complete list of 16-bit UUIDs)
      0xaa, // Eddystone UUID (low)
      0xfe, // Eddystone UUID (high)
      0x0d, // length of next section
      0x16, // section type (service data)
      0xaa, // Eddystone UUID (low)
      0xfe, // Eddystone UUID (high)
      0x10, // Frame type (URL)
      0x00, // TX power used to calculate distance to beacon
      0x03, // URL Scheme Prefix (https://)
      0x67, // ASCII host name ("google")
      0x6f, 0x6f, 0x67, 0x6c, 0x65,
      0x07,                                       // .com/
      0,    0,    0,    0,    0,    0, 0, 0, 0, 0 // padding
  };
  err = bluetooth_hci_command(BLUETOOTH_HCI_CMD_LE_SET_ADV_DATA, adv_data, 32,
      0, 0);
  if (err) {
    io_printf(&uart1, "Error setting advertising data. Code: %u\n", err);
    return;
  }

  // enable advertising
  uint8_t adv_enable[1] = {1};
  err = bluetooth_hci_command(BLUETOOTH_HCI_CMD_LE_SET_ADV_ENABLE, adv_enable,
      1, 0, 0);
  if (err) {
    io_printf(&uart1, "Error enabling advertising. Code: %u\n", err);
    return;
  }

  io_printf(&uart1, "Enabled Eddystone beacon\n");
}

static void shell() {
  io_printf(&uart1, "=== raspi0w-mine Shell ===\n");

  int freewrite = 0;
  while (1) {
    uint8_t byte = uart1.read();

    if (freewrite == 1) {
      // FREEWRITE MODE
      if (byte == 27) {
        freewrite = 0;
        uart1.write('\n');
        continue;
      } else if (byte == '\r') {
        byte = '\n';
      } else if (byte == 0x7F) {
        uart1.write('\b');
        uart1.write(' ');
        byte = '\b';
      }
      uart1.write(byte);
    } else {
      // NOT FREEWRITE
      if (byte == 't') {
        io_printf(&uart1, "System time: %u\n", systimer_intervals_passed);
      } else if (byte == 'f') {
        freewrite = 1;
      } else if (byte == 'd') {
        print_diagnostics();
      } else if (byte == 'm') {
        print_mac_addr();
      } else if (byte == 'b') {
        uint8_t data[] = {};
        uint8_t bdaddr[6];
        int err = bluetooth_hci_command(BLUETOOTH_HCI_CMD_GET_BDADDR, data, 0,
                                        bdaddr, 6);
        if (!err) {
          io_printf(&uart1, "Bluetooth device address: %x:%x:%x:%x:%x:%x\n",
                    bdaddr[0], bdaddr[1], bdaddr[2], bdaddr[3], bdaddr[4],
                    bdaddr[5]);
        } else {
          io_printf(&uart1,
                    "Error getting Bluetooth device address. Code: %d\n", err);
        }
      } else if (byte == 'e') {
        eddystone_beacon();
      } else if (byte == 'w') {
        // wait for at least 250,000,000 cycles.
        io_printf(&uart1,
                  "Waiting for 250,000,000 cycles (if built without debug)\n");
        io_printf(&uart1, "Hopefully this should take 1 seciond.\n");
        for (int i = 0; i < 250000000; i++) {
          asm volatile("nop");
        }
        io_printf(&uart1, "Done\n");
      } else if (byte == 'h') {
        io_printf(&uart1, "Commands:\n");
        io_printf(&uart1, "\tt: Display the current system time (nonfunctional).\n");
        io_printf(&uart1, "\tf: Enter freewrite mode.\n");
        io_printf(&uart1, "\td: Print diagnostics.\n");
        io_printf(&uart1, "\tm: Print MAC address.\n");
        io_printf(&uart1, "\tb: Print Bluetooth device address.\n");
        io_printf(&uart1, "\te: Start Eddystone beacon.\n");
        io_printf(&uart1, "\ts: Stop Eddystone beacon.\n");
        io_printf(&uart1,
                  "\tw: Wait 250 million cycles to test clock speed.\n");
        io_printf(&uart1, "\th: Show this help command.\n");
      } else {
        io_printf(&uart1, "Unknown command: %c. Use h for help.\n", byte);
      }
    }
  }
}

void kmain(void) {
  // initialize drivers
  uart1_init();
  isr_init();
  irq_init();
  /* systimer_init(); */
  bluetooth_init();

  print_diagnostics();

  shell();

  return;
}
