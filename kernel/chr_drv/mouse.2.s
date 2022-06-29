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
	cmpb $0xe0,%al
	je set_e0
	cmpb $0xe1,%al
	je set_e1
	call readmouse
        popl %eax
	movb $0,e0
e0_e1:	inb $0x61,%al
	jmp 1f
1:	jmp 1f
1:	orb $0x80,%al
	jmp 1f
1:	jmp 1f
1:	outb %al,$0x61
	jmp 1f
1:	jmp 1f
1:	andb $0x7F,%al
	outb %al,$0x61
        movb $0x20,%al
        outb %al,$0xA0
        outb %al,$0x20
	pushl $0
	call do_tty_interrupt
	addl $4,%esp
	pop %es
	pop %ds
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax
	iret
set_e0:	movb $1,e0
	jmp e0_e1
set_e1:	movb $2,e0
	jmp e0_e1
