#include "drivers/mailbox.h"
#include "drivers/uart1.h"
#include "kernel/io.h"
#include <stdint.h>

volatile uint32_t mailbox_buffer[36] __attribute__((aligned(16)));

int mailbox_call(uint8_t channel) {
  uint32_t r = ((uint32_t)mailbox_buffer & ~0xF) | channel;
  while (*MAILBOX_STATUS1 & MAILBOX_STATUS_FULL) {
  }
  *MAILBOX_WRITE1 = r;

  while (1) {
    while (*MAILBOX_STATUS0 & MAILBOX_STATUS_EMPTY) {
    }
    if (*MAILBOX_READ0 == r) {
      return mailbox_buffer[1] == MAILBOX_RESPONSE;
    }
  }
  return 0;
}
