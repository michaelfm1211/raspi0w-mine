#pragma once

#include <stdint.h>

// NOTE: IRQ_BASIC_PENDING is actually a 21-bit register.
#define IRQ_BASIC_PENDING (volatile uint32_t *)0x2000B200
#define IRQ_PENDING_1 (volatile uint32_t *)0x2000B204
#define IRQ_PENDING_2 (volatile uint32_t *)0x2000B208
#define IRQ_FIQ_CTRL (volatile uint8_t *)0x2000B20C
#define IRQ_ENABLE_1 (volatile uint32_t *)0x2000B210
#define IRQ_ENABLE_2 (volatile uint32_t *)0x2000B214
#define IRQ_ENABLE_BASIC (volatile uint8_t *)0x2000B218
#define IRQ_DISABLE_1 (volatile uint32_t *)0x2000B21C
#define IRQ_DISABLE_2 (volatile uint32_t *)0x2000B220
#define IRQ_DISABLE_BASIC (volatile uint8_t *)0x2000B224

// defined in drivers/irq.c
extern void (*irq_handlers[64])(void);

void irq_init(void);
void irq_handle(void);
