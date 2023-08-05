#include "drivers/bluetooth.h"
#include "drivers/gpio.h"
#include "drivers/uart1.h"
#include "kernel/io.h"
#include <stdint.h>

extern uint8_t _binary_firmware_BCM43430A1_hcd_start[];
extern uint32_t _binary_firmware_BCM43430A1_hcd_size;

static int initialized = 0;

static inline void wait_cycles(register uint32_t r) {
  while (r--) {
    asm volatile("nop");
  }
}

static void enable_uart0(void) {
  // set GPIO30-33 to ALT5
  register uint32_t r = *GPFSEL3;
  r |= (7 << 0) | (7 << 3) | (7 << 6) | (7 << 9);
  *GPFSEL3 = r;

  // pull GPIO30-33 down
  *GPPUD = 1;
  wait_cycles(150);
  *GPPUDCLK0 = (1 << 30) | (1 << 31);
  *GPPUDCLK1 = (1 << 0) | (1 << 1);
  wait_cycles(150);
  *GPPUD = 0;
  *GPPUDCLK0 = 0;

  *BLUETOOTH_UART0_IMSC = 0x00; // disable all interrupts (temporarily)
  *BLUETOOTH_UART0_ICR = 0x7ff; // clear all interrupts
  *BLUETOOTH_UART0_IBRD = 0x1a; // set baud rate to 115200 (integer part)
  *BLUETOOTH_UART0_FBRD = 0x03; // set baud rate to 115200 (fractional part)
  *BLUETOOTH_UART0_IFLS = 0x08; // trigger interrupt when FIFO is 1/4 full
  *BLUETOOTH_UART0_LCRH = 0x70; // 8N1 and enable FIFOs
  *BLUETOOTH_UART0_CR = 0xB01;  // enable UART0, receive and transmit
  *BLUETOOTH_UART0_IMSC =
      0x430; // enable overrun, receive, and transmit interrupts

  // clear the receive FIFO
  while (!(*BLUETOOTH_UART0_FR & 0x10)) {
    *BLUETOOTH_UART0_DR;
  }

  // gaunteed to be at least 100ms delay
  wait_cycles(25000000);
}

static inline void uart0_send(uint8_t byte) {
  while (*BLUETOOTH_UART0_FR & 0x20) {
  }
  *(volatile uint8_t *)BLUETOOTH_UART0_DR = byte;
}

static inline uint8_t uart0_read(void) {
  while (*BLUETOOTH_UART0_FR & 0x10) {
  }
  return *(volatile uint8_t *)BLUETOOTH_UART0_DR;
}

static int reset(void) {
  uint8_t data[] = {};
  return bluetooth_hci_command(BLUETOOTH_HCI_CMD_RESET, data, 0, 0, 0);
}

static int load_firmware(void) {
  uint8_t data[] = {};
  if (bluetooth_hci_command(BLUETOOTH_HCI_CMD_LOAD_FIRMWARE, data, 0, 0, 0)) {
    // failed to send initial load firmware command, error code 1
    return 1;
  }

  wait_cycles(12500000);

  uint8_t *ptr = (uint8_t *)_binary_firmware_BCM43430A1_hcd_start;
  uint32_t size = (uint32_t)&_binary_firmware_BCM43430A1_hcd_size;
  uint32_t sent = 0;
  while (sent < size) {
    uint16_t opcode = (*ptr & 0xFF) | (*(ptr + 1) << 8); // convert endianness
    uint8_t len = *(uint8_t *)(ptr + 2);
    ptr += 3;
    if (bluetooth_hci_command(opcode, ptr, len, 0, 0)) {
      // failed to send firmware command, error code 2
      return 2;
    }

    ptr += len;
    sent += len + 3;
    io_printf(&uart1, "\tUploaded %d/%d bytes of firmware\r", sent, size);
  }
  io_printf(&uart1, "\n");

  // gaunteed to be at least 100ms delay
  wait_cycles(25000000);
  return 0;
}

void bluetooth_init(void) {
  int error;

  if (initialized) {
    return;
  }
  io_printf(&uart1, "drivers/bluetooth initializing...\n");
  
  enable_uart0();
  io_printf(&uart1, "\tUART0 initialized.\n");

  error = reset();
  if (error == 0) {
    io_printf(&uart1, "\tReset Bluetooth hardware\n");
  } else {
    io_printf(&uart1, "\tFailed to reset Bluetooth hardware. Error: %d\n",
        error);
    return;
  }

  uint8_t *firm = (uint8_t *)_binary_firmware_BCM43430A1_hcd_start;
  uint32_t size = (uint32_t)&_binary_firmware_BCM43430A1_hcd_size;
  io_printf(&uart1, "\tFound Bluetooth firmware at 0x%x with size %u\n", firm, size);

  error = load_firmware();
  if (error == 0) {
    io_printf(&uart1, "\tLoaded Bluetooth firmware\n");
  } else {
    io_printf(&uart1, "\tFailed to load Bluetooth firmware. Error: %d\n",
        error);
    return;
  }

  // clear the receive FIFO. unsure why this is needed, but won't work without
  // it.
  while (!(*BLUETOOTH_UART0_FR & 0x10)) {
    *BLUETOOTH_UART0_DR;
  }

  initialized = 1;
  io_printf(&uart1, "\t... done.\n");
}

int bluetooth_hci_command(uint16_t opcode, uint8_t *data, uint32_t input_len,
    uint8_t *output, uint32_t output_len) {
  // send the HCI command packet
  uart0_send(BLUETOOTH_HCI_PKT_CMD);
  uart0_send(opcode & 0xFF); // lower byte
  uart0_send(opcode >> 8);   // upper byte
  uart0_send(input_len);

  for (uint32_t i = 0; i < input_len; i++) {
    uart0_send(data[i]);
  }

  // wait for a command complete event
  if (uart0_read() != BLUETOOTH_HCI_PKT_EV) {
    // wrong event, error code 1
    return 1;
  }

  uint8_t ev_code = uart0_read();
  if (ev_code == BLUETOOTH_HCI_EV_CMD_STATUS) {
    if (uart0_read() != 0x4) {
      // wrong number of parameters, should be 4, error code 2
      return 2;
    }
    uint8_t status = uart0_read();
    if (status != 0) {
      // we got an real error from the chip, print and return error code 3
      io_printf(&uart1, "Bluetooth HCI Command Error: Got status %x\n", status);
      return 3;
    }
    if (uart0_read() == 0) {
      // the controller isn't allowing us to send commands, error code 8
      return 4;
    }
    if (uart0_read() != (opcode & 0xFF)) {
      // mismatching lower half opcode, error code 5
      return 5;
    }
    if (uart0_read() != (opcode >> 8)) {
      // mismatching upper half opcode, error code 6
      return 6;
    }
  } else if (ev_code == BLUETOOTH_HCI_EV_CMD_COMPLETE) {
    // we got a legit response from the chip, let's make sure it's the right
    if (uart0_read() != 4 + output_len) {
      // wrong number of parameters, should be 4, error code 7
      return 7;
    }
    if (uart0_read() == 0) {
      // the controller isn't allowing us to send commands, error code 8
      return 8;
    }
    if (uart0_read() != (opcode & 0xFF)) {
      // mismatching lower half opcode, error code 9
      return 9;
    }
    if (uart0_read() != (opcode >> 8)) {
      // mismatching upper half opcode, error code 10
      return 10;
    }
    if (uart0_read() != 0) {
      // status should be 0, error code 11
      return 11;
    }

    // the rest of the packet is the return data, store it in output
    for (uint32_t i = 0; i < output_len; i++) {
      output[i] = uart0_read();
    }
  } else {
    // wrong event, error code 12
    return 12;
  }

  // success
  return 0;
}
