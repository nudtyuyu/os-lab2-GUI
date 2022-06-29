/*
 *  linux/lib/getcwd.c
 *
 *  (C) 2022  Healer
 */

#define __LIBRARY__

#include <linux/sched.h>
#include <linux/tty.h>
#include <asm/segment.h>
#include <asm/system.h>
#include <asm/io.h>
#include <unistd.h>

void sys_init_graphics()
{
        outb(0x05,0x3CE);
        outb(0x40,0x3CF);//设置256色，且取出方式为移动拼装
        outb(0x06,0x3CE);
        outb(0x05,0x3CF);//设置显存的地址区域，禁止字符模式
        outb(0x04,0x3C4);
        outb(0x08,0x3C5);//设置将4个显存片连在一起
}