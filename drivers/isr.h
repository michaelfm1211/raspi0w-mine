#pragma once

#include <stdint.h>

// These are the addresses the CPU will jump to on an interrupt. Don't use these
// for adding handlers. Put those in the HANDLER table.
#define EXCEPTION_RESET (uint32_t *)0x0
#define EXCEPTION_UNDEFINED (uint32_t *)0x4
#define EXCEPTION_SOFTWARE (uint32_t *)0x8
#define EXCEPTION_PREFETCH (uint32_t *)0xC
#define EXCEPTION_DATA (uint32_t *)0x10
#define EXCEPTION_RESERVED (uint32_t *)0x14
#define EXCEPTION_IRQ (uint32_t *)0x18
#define EXCEPTION_FIQ (uint32_t *)0x1C

// These are the pointers to the addresses that will be jumped to by the
// vectors. These are the addresses you should use for adding handlers.
#define HANDLER_RESET (uint32_t *)0x20
#define HANDLER_UNDEFINED (uint32_t *)0x24
#define HANDLER_SOFTWARE (uint32_t *)0x28
#define HANDLER_PREFETCH (uint32_t *)0x2C
#define HANDLER_DATA (uint32_t *)0x30
#define HANDLER_RESERVED (uint32_t *)0x34
#define HANDLER_IRQ (uint32_t *)0x38
#define HANDLER_FIQ (uint32_t *)0x3C

void isr_init(void);
