.section ".text.boot"

.globl _start
_start:
	// setup the stack at 1kb less than the interrupt stack.
	mov sp, #0x7C00

	// clear BSS
	ldr r4, =__bss_start
	ldr r9, =__bss_end
	mov r5, #0
	mov r6, #0
	mov r7, #0
	mov r8, #0
	b 2f
1:
	// store multiple (r5-r8) at r4
	stmia r4!, {r5-r8}
2:
	cmp r4, r9
	blo 1b

	// run the kernel
	// /0 has a firmware code, r1 has the machine ID, and r2 has ATAGS
	bl kmain
3:
	// halt the CPU
	wfe
	b 3b
