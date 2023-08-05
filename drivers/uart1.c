/*
 * UART1 (mini UART) driver.
 *
 * On the RPI0W, this is the primary UART, while UART0 (the PL011) is the
 * secondary UART and is connected to bluetooth.
 */

#include "drivers/uart1.h"
#include "drivers/gpio.h"
#include "kernel/io.h"
#include <stdint.h>

static int initialized = 0;
struct io_interface uart1;

static inline void wait_150_cycles() {
  register uint32_t r = 150;
  while (r--) {
    asm volatile("nop");
  }
}

static void uart1_write_byte(uint8_t byte) {
  if (byte == '\n') {
    // convert \n to \r\n
    uart1_write_byte('\r');
  }

  // wait until transmitter is empty
  while (!(*AUX_MU_LSR_REG & 0x20)) {
  }

  *AUX_MU_IO_REG = byte;
}

static uint8_t uart1_read_byte() {
  // wait until data is ready
  while (!(*AUX_MU_LSR_REG & 0x01)) {
  }

  return *AUX_MU_IO_REG;
}

void uart1_init(void) {
  if (initialized) {
    return;
  }

  // setup UART1
  *AUX_ENABLES |= 1; // enable UART1, AUX mini UART
  *AUX_MU_CNTL_REG = 0;
  // see https://github.com/dwelch67/raspberrypi-zero/tree/master/uart01
  // for why AUX_MU_LCR_REG is set to 3. The documentation is wrong.
  *AUX_MU_LCR_REG = 3;    // turn on 8-bit mode
  *AUX_MU_MCR_REG = 0;    // no flow control
  *AUX_MU_IIR_REG = 0;    // turn off interrupts
  *AUX_MU_IER_REG = 0xc6; // clear and enable FIFOs
  *AUX_MU_BAUD_REG = 270; // 115200 baud

  // map UART1 to GPIO pins
  register uint32_t r = *GPFSEL1;
  r &= ~((7 << 12) | (7 << 15)); // GPIO 14 and 15
  r |= (2 << 12) | (2 << 15);    // ALT5
  *GPFSEL1 = r;

  *GPPUD = 0;
  wait_150_cycles();
  *GPPUDCLK0 = (1 << 14) | (1 << 15);
  wait_150_cycles();
  *GPPUDCLK0 = 0;

  // clear the FIFO
  while (*AUX_MU_LSR_REG & 0x01) {
    uart1_read_byte();
  }

  // enable TX and RX
  *AUX_MU_CNTL_REG = 3;

  uart1 = (struct io_interface){
      .name = "UART1",
      .write = uart1_write_byte,
      .read = uart1_read_byte,
  };
  initialized = 1;

  io_printf(&uart1, "\n==========================\n");
  io_printf(&uart1, "=== raspi0w-mine UART1 ===\n");
  io_printf(&uart1, "==========================\n\n");
}
