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


/*Display mouse*/
#define vga_graph_memstart 0xA0000
#define vga_graph_memsize 64000
#define cursor_side 6
#define vga_width 320
#define vga_height 200

extern int x_position;
extern int y_position;

void sys_init_graphics()
{
        outb(0x05,0x3CE);
        outb(0x40,0x3CF);//设置256色，且取出方式为移动拼装
        outb(0x06,0x3CE);
        outb(0x05,0x3CF);//设置显存的地址区域，禁止字符模式
        outb(0x04,0x3C4);
        outb(0x08,0x3C5);//设置将4个显存片连在一起

        outb(0x01,0x3D4);//End Horizontal Display
        outb(0x4F,0x3D5);//设置End Horizontal Display 为79
        outb(0x03,0x3D4);//End Horizontal Blanking
        outb(0x82,0x3D5);//设置Display Enable Skew为0

        outb(0x07,0x3D4);//设置Vertical Display End 第8，9位为1，0
        outb(0x1F,0x3D5);
        outb(0x12,0x3D4);//Vertical Display End的低7位
        outb(0x8F,0x3D5);//低7位为0x8F
        //outb(0x17,0x3D4);
        //outb(0xA3,0x3D5);//设置SLDIV=1，每两个时钟周期扫描线加一

        outb(0x14,0x3D4);
        outb(0x40,0x3D5);//DW=1(第6位置1)
        outb(0x13,0x3D4);
        outb(0x28,0x3D5);//设置offset=40

        outb(0x0C,0x3D4);//高8位
        outb(0x0,0x3D5);
        outb(0x0D,0x3D4);//低8位
        outb(0x0,0x3D5);//将Start Address 设置为0xA0000(偏移为0x0)    

        /*Display the mouse*/
                        
        int i,j;
        char *ptr = (char *)vga_graph_memstart;
        for(i=0;i<vga_graph_memsize;i++)
        {
                *ptr++=3;
        }
        for(i = x_position-cursor_side;i<=x_position+cursor_side;i++)
        {
                for(j=y_position-cursor_side;j<=y_position+cursor_side;j++)
                {
                         ptr = (char*)(vga_graph_memstart + j*vga_width + i);
                        *ptr=12;
                }
        }

}