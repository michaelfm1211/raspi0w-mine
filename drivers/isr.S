.section .text

.globl isr_reset
isr_reset:	
	// this REALLY shouldn't happen because firmware has already handled it.
	// if somehow we end up here, give a sign and die
	mov r0, #0x10 // give a sign that we're in reset
	b .

.globl isr_undefined
isr_undefined:
	mov sp, 0x8000
	stmfd sp!, {r0-r12, lr}
	bl isr_undefined_handler
	ldmfd sp!, {r0-r12, pc}^

.globl isr_software
isr_software:
	mov sp, 0x8000
	stmfd sp!, {r0-r12, lr}
	mov r0, lr
	mov r1, sp
	bl isr_software_handler
	ldmfd sp!, {r0-r12, pc}^

.globl isr_prefetch
isr_prefetch:
	mov sp, 0x8000
	sub lr, lr, #4
	stmfd sp!, {r0-r12, lr}
	bl isr_prefetch_handler
	ldmfd sp!, {r0-r12, pc}^

.globl isr_data
isr_data:
	mov sp, 0x8000
	sub lr, lr, #8
	stmfd sp!, {r0-r12, lr}
	bl isr_data_handler
	ldmfd sp!, {r0-r12, pc}^

.globl isr_irq
isr_irq:
	mov sp, 0x8000
	sub lr, lr, #4
	stmfd sp!, {r0-r12, lr}
	bl isr_irq_handler
	ldmfd sp!, {r0-r12, pc}^

.globl isr_fiq
isr_fiq:
	mov sp, 0x8000
	sub lr, lr, #4
	stmfd sp!, {r0-r12, lr}
	bl isr_fiq_handler
	ldmfd sp!, {r0-r12, pc}^
