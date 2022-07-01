/*
*   /kernel/chr_drv/readmouse.c
*   Copyright(2022)  Healers
*/



#define __LIBRARY__
#include <linux/sched.h>
#include <linux/tty.h>
#include <asm/segment.h>
#include <asm/system.h>
#include <asm/io.h>
#include <message.h>

#define vga_graph_memstart 0xA0000
#define vga_graph_memsize 64000
#define cursor_side 6
#define vga_width 320
#define vga_height 200



static unsigned char mouse_input_count=0;
static unsigned char mouse_left_down;
static unsigned char mouse_right_down;
//static unsigned char mouse_left_move;
static unsigned char x_overflow;
static unsigned char y_overflow;
static int x_sign =0;
static int y_sign =0;
int x_position = 100;
int y_position = 100;
void readmouse(int mousecode)
{
        // static unsigned char mouse_input_count=0;
        // static unsigned char mouse_left_down;
        // static unsigned char mouse_right_down;
        //static unsigned char mouse_left_move;
        //static unsigned char mouse_right_move;
        //static unsigned char mouse_up_move;
        //static unsigned char mouse_down_move;
        int i,j;
        char *ptr = (char *)vga_graph_memstart;
        
        if(mousecode == 0xFA)
        {
                mouse_input_count =1;
                return 0;
        }
        switch(mouse_input_count)
        {
                case 1:
                        mouse_left_down = (mousecode & 0x1) ==0x1;
                        mouse_right_down = (mousecode & 0x2) ==0x2;
                        x_sign = (mousecode & 0x10)?0xffffff00:0;
                        y_sign = (mousecode & 0x20)?0xffffff00:0;
                        x_overflow = (mousecode & 0x40);
                        y_overflow = (mousecode & 0x80);
                        // cli();
                        // for(i=0;i<vga_graph_memsize;i++)
                        // {
                        //         *ptr++=3;
                        // }
                        // sti();
                        if(mouse_left_down)
                        {
                               message *msg = malloc(sizeof(message));
                               msg->mid = MSG_MOUSE_CLICK_LEFT;
                               msg ->pid =-1;
                               sys_post_message(msg);

                        }
                        if(mouse_right_down)
                        {
                               message *msg = malloc(sizeof(message));
                               msg->mid = MSG_MOUSE_CLICK_RIGHT;
                               msg ->pid =-1;
                               sys_post_message(msg);
                        }
                        mouse_input_count++;
                        break;
                case 2:
                        if(x_overflow && !x_sign)
                        {
                                x_position = 0+3*cursor_side;
                                mouse_input_count++;
                                break;
                        }
                        else if(x_overflow && x_sign)
                        {
                                x_position = vga_width-3*cursor_side;
                                mouse_input_count++;
                                break;
                        }
                        
                                

                        x_position += (int)(x_sign | mousecode);
                        if(mouse_left_down || mouse_right_down)
                        {
                               x_position -= (int)(x_sign | mousecode);
                        }
                        if(x_position<0) x_position=20;
                        if(x_position>vga_width) x_position = vga_width-20;
                        mouse_input_count++;
                        break;
                case 3:
                        
                        //cli();
                         
                        y_position -= (int)( y_sign | mousecode);
                        if(mouse_left_down || mouse_right_down)
                        {
                                y_position += (int)( y_sign | mousecode);
                        }  
                        if(y_position>vga_height) y_position = vga_height-10;
                        if(y_position<0) y_position=10;
                        if(y_overflow && !y_sign)
                        {
                                y_position = 0+2*cursor_side;
                                
                        }
                        else if(y_overflow && y_sign)
                        {
                                y_position = vga_height-2*cursor_side;
                                
                        }
                        //if(y_position>vga_width) y_position = vga_width;
                        mouse_input_count=1;
                        int MAX_X = (x_position+cursor_side>vga_width)?vga_width-15:x_position+cursor_side;
                        int MAX_Y = (y_position+cursor_side>vga_height)?vga_height-10:y_position+cursor_side;
                        // if(x_sign)
                        //         printk("left\n");
                        // else if(!x_sign)
                        //         printk("right\n");
                        // if(y_sign)
                        //         printk("down\n");
                        // else if(!y_sign)
                        //         printk("up\n");
                        // if(mouse_left_down)
                        //         printk("left key\n");
                        // else if(mouse_right_down)
                        //         printk("right key\n");
                        
                        
                        //printk("do-read-mouse\n");
                        
                        // for(i = x_position-cursor_side;i<=MAX_X;i++)
                        // {
                        //         for(j=y_position-cursor_side;j<=MAX_Y;j++)
                        //         {
                                        

                        //                 ptr = (char*)(vga_graph_memstart + j*vga_width + i);
                        //                 *ptr=12;
                        //         }
                        // }
                        //sti();
                        
                        break;  
                // case 4:
                //         printk("3D!\n");
                //         mouse_input_count =1;
                //         break;

        }

        
        
        
        
}
