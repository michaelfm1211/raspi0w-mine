/*
 * THIS DOES NOT WORK, unsure why
 */

#pragma once

#include <stdint.h>

// registers
#define SYSTIMER_CS (volatile uint32_t *)0x20003000
#define SYSTIMER_CLO (volatile uint32_t *)0x20003004
#define SYSTIMER_CHI (volatile uint32_t *)0x20003008
#define SYSTIMER_C0 (volatile uint32_t *)0x2000300C
#define SYSTIMER_C1 (volatile uint32_t *)0x20003010
#define SYSTIMER_C2 (volatile uint32_t *)0x20003014
#define SYSTIMER_C3 (volatile uint32_t *)0x20003018

// match bits on SYSTIMER_CS
#define SYSTIMER_CS_M0 (1 << 0)
#define SYSTIMER_CS_M1 (1 << 1)
#define SYSTIMER_CS_M2 (1 << 2)

// IRQs
#define SYSTIMER_IRQ1 0
#define SYSTIMER_IRQ2 1
#define SYSTIMER_IRQ3 2

// timer runs at 62.5 kHz
// how often the timer should fire. should fire every 1ms
#define SYSTIMER_INTERVAL 1000

extern uint32_t systimer_intervals_passed;

void systimer_init(void);
