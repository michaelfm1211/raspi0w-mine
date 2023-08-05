#include "drivers/irq.h"
#include "drivers/uart1.h"
#include "kernel/io.h"
#include <stdint.h>

void (*irq_handlers[64])(void);

static int initialized = 0;

void irq_init(void) {
  if (initialized) {
    return;
  }
  io_printf(&uart1, "drivers/irq initializing...\n");

  asm volatile("cpsie i");

  initialized = 1;
  io_printf(&uart1, "\t... done.\n");
}

void irq_handle(void) {
  // Ensure that IRQs are pending
  if (!*IRQ_PENDING_1 && !*IRQ_PENDING_2){
    /* io_printf(&uart1, "No IRQs are pending. IRQ registers: 0x%x, 0x%x\n", */
    /*     *IRQ_PENDING_1, *IRQ_PENDING_2); */
    return;
  }

  if (*IRQ_PENDING_1) {
    // there may be multiple IRQs pending, so do each of them
    int j = 1;
    for (uint32_t i = 0; i < 32; i++) {
      if (*IRQ_PENDING_1 & j) {
        if (!irq_handlers[i]) {
          io_printf(&uart1, "No handler for IRQ %d\n", j);
          continue;
        }
        irq_handlers[i]();
      }
      j <<= 1;
    }
  }
  if (*IRQ_PENDING_2) {
    // there may be multiple IRQs pending, so do each of them
    int j = 0;
    for (uint32_t i = 1; i < 32; i++) {
      if (*IRQ_PENDING_1 & j) {
        if (!irq_handlers[i]) {
          io_printf(&uart1, "No handler for IRQ %d\n", j);
          continue;
        }
        irq_handlers[i]();
      }
      j <<= 1;
    }
  }
}
