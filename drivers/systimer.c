/*
 * THIS DOES NOT WORK, unsure why
 */

#include "drivers/systimer.h"
#include "drivers/irq.h"
#include "drivers/uart1.h"
#include "kernel/io.h"

uint32_t systimer_intervals_passed;

static int initialized = 0;

static void handle_irq(void) {
  *SYSTIMER_C0 = *SYSTIMER_CLO + SYSTIMER_INTERVAL;
  *SYSTIMER_CS = SYSTIMER_CS_M0;
  systimer_intervals_passed++;
}

void systimer_init(void) {
  if (initialized) {
    return;
  }
  io_printf(&uart1, "drivers/systimer initializing...\n");

  irq_handlers[SYSTIMER_IRQ1] = handle_irq;
  *IRQ_ENABLE_1 |= 1 << SYSTIMER_IRQ1;
  io_printf(&uart1, "\tRegistered IRQ handler 0x%x for IRQ %u\n", handle_irq, SYSTIMER_IRQ1);

  *SYSTIMER_C0 = *SYSTIMER_CLO + SYSTIMER_INTERVAL;
  systimer_intervals_passed = 0;

  initialized = 1;
  io_printf(&uart1, "\t... done.\n");
}
