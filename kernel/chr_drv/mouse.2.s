# 1 "mouse.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "mouse.S"








# 1 "../../include/linux/config.h" 1





















# 36 "../../include/linux/config.h"

# 47 "../../include/linux/config.h"

# 9 "mouse.S" 2

.text
.globl mouse_interrupt


size	= 1024		

head = 4
tail = 8
proc_list = 12
buf = 16

mode:	.byte 0		
leds:	.byte 2		
e0:	.byte 0



mouse_interrupt:
	
        pushl %eax
	pushl %ebx
	pushl %ecx
	pushl %edx
	push %ds
	push %es
	movl $0x10,%eax
	mov %ax,%ds
	mov %ax,%es
	xor %al,%al
        xor %eax,%eax	
	inb $0x60,%al
        pushl %eax
	call readmouse

        movb $0x20,%al
        outb %al,$0xA0
        outb %al,$0x20

	addl $4,%esp
	pop %es
	pop %ds
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax
	iret