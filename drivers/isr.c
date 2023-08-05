#include "drivers/isr.h"
#include "drivers/irq.h"
#include "drivers/uart1.h"
#include "kernel/io.h"
#include <stdint.h>

static int initialized = 0;

// adapted from https://developer.arm.com/documentation/dui0056/d/handling-processor-exceptions/installing-an-exception-handler/installing-the-handlers-from-c?lang=en
static uint32_t install_vector(uint32_t address, volatile uint32_t *vector) {
  uint32_t new_vector = 0xE59FF000 | (address - (uint32_t)vector - 0x8);
  uint32_t old_vector = *vector;
  *vector = new_vector;
  io_printf(&uart1, "\tInstalled handler pointer 0x%x at vector 0x%x.\n", address, vector);
  return old_vector;
}

// ISRs implemented in kernel/isr.S call the C handlers below
extern void isr_reset();
extern void isr_undefined();
extern void isr_software();
extern void isr_prefetch();
extern void isr_data();
extern void isr_irq();
extern void isr_fiq();

/* START WARNING: Do not use floats here. It could clobber the interruptee's
 * float registers! */

void isr_undefined_handler() {
  io_printf(&uart1, "Exception: Undefined instruction\n");
}

void isr_software_handler(uint32_t lr, uint32_t sp) {
  uint32_t code = *(uint32_t *)(lr - 0x4) & 0x00FFFFFF;
  io_printf(&uart1, "Code: %u\n", code);
  io_printf(&uart1, "SP: %u\n", sp);
}

void isr_prefetch_handler() {
  io_printf(&uart1, "Exception: Prefetch Abort or breakpoint\n");
}

void isr_data_handler() {
  io_printf(&uart1, "Exception: Prefetch Abort\n");
}

void isr_irq_handler() {
  irq_handle();
}

void isr_fiq_handler() {
  io_printf(&uart1, "Exception: FIQ\n");
}

/* END WARNING */

void isr_init() {
  if (initialized) {
    return;
  }
  io_printf(&uart1, "drivers/isr initializing...\n");

  // fill out the handler table
  *HANDLER_RESET = (uint32_t)isr_reset;
  *HANDLER_UNDEFINED = (uint32_t)isr_undefined;
  *HANDLER_SOFTWARE = (uint32_t)isr_software;
  *HANDLER_PREFETCH = (uint32_t)isr_prefetch;
  *HANDLER_DATA = (uint32_t)isr_data;
  *HANDLER_IRQ = (uint32_t)isr_irq;
  *HANDLER_FIQ = (uint32_t)isr_fiq;

  // load the exception vector table with LoaD Relative instructions to the
  // handler table.
  install_vector((uint32_t)HANDLER_RESET, EXCEPTION_RESET);
  install_vector((uint32_t)HANDLER_UNDEFINED, EXCEPTION_UNDEFINED);
  install_vector((uint32_t)HANDLER_SOFTWARE, EXCEPTION_SOFTWARE);
  install_vector((uint32_t)HANDLER_PREFETCH, EXCEPTION_PREFETCH);
  install_vector((uint32_t)HANDLER_DATA, EXCEPTION_DATA);
  install_vector((uint32_t)HANDLER_IRQ, EXCEPTION_IRQ);
  install_vector((uint32_t)HANDLER_FIQ, EXCEPTION_FIQ);

  initialized = 1;
  io_printf(&uart1, "\t...done.\n");
}
