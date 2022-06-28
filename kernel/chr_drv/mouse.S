
/*
 *  linux/kernel/mouse.s
 *
 *  (C) 2022  Healer
 */

#include <linux/config.h>

.text
.globl mouse_interrupt



mouse_interrupt:
        pushl %eax
	pushl %ebx
	pushl %ecx
	pushl %edx
	push %ds
	movl $0x10,%eax
	mov %ax,%ds
	xor %al,%al		/* %eax is scan code */
	inb $0x60,%al
	call readmouse
	addl $4,%esp
	pop %es
	pop %ds
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax
	iret