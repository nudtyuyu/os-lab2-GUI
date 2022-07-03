#define __LIBRARY__
#include <unistd.h>
#include <message.h>
#include <adv.h>
#include <stdlib.h>
#include <graphics.h>

_syscall0(void,init_graphics);
_syscall1(int,get_message,message*,msg);
_syscall1(void,timer_create,int,milliseconds);
_syscall0(void,exit_graphics);
_syscall2(void,repaint,int,count,object*,obj);
_syscall1(int,sleep,int,seconds);



int main()
{
       object *objects = (object*)malloc(20*sizeof(object));
       message *msg = (message *)malloc(sizeof(message)); 
       int i=0;
       int ps_y = 100;
       int objects_count;
       int remain;
       
       objects[0].posx = P_X;
       objects[0].posy = ps_y;
       objects[0].width = P_WIDTH;
       objects[0].height = P_HEIGHT;
       objects[0].color = 12;
       
       objects[1].posx = 0;
       objects[1].posy = ps_y+P_HEIGHT+2;
       objects[1].width =  vga_width;
       objects[1].height = vga_height - objects[1].posy;
       objects[1].color = 20;

       objects[2].posy = ps_y+P_HEIGHT+2;
       objects[2].posx = vga_width;
       objects[2].width = 0;
       objects[2].height = vga_height - objects[2].posy;
       objects[2].color = 20;
       objects_count =3;
       remain = objects_count-3;
       init_graphics();
       repaint(objects_count,objects);
       timer_create(8000);
       srand(0);
       GAME:    while(1)
               {
                        if(objects[0].posx>objects[1].posx+objects[1].width+GAP_WIDTH/3 && objects[0].posx<objects[2].posx-GAP_WIDTH/2)
                        {
                                goto GAMEOVER;
                        }
                        for(i=0;i<sizeof(message);i++)
                        {
                                *(char*)msg=0;
                        }
                        get_message(msg);
                        switch (msg->mid)
                        {
                                case USER_TIMER_UP:
                                        objects_count = (remain>0)?objects_count:4+rand()%19;
                                        objects_count = (objects_count>20)?20:objects_count;
                                        remain = objects_count;
                                        objects[0].posx +=P_SPEED;
                                        objects[1].width -=P_SPEED*2;
                                        objects[2].posx = objects[1].width + GAP_WIDTH;
                                        objects[2].width += P_SPEED*2; 
                                        if(objects[1].width<0)   
                                                objects[1].width =vga_width;
                                        if(objects[2].width>vga_width) 
                                        {
                                                objects[2].posx = vga_width;
                                                objects[2].width = 0;
                                        }
                                        for(i=3;i<objects_count;i++)
                                        {
                                                objects[i].posx = objects[0].posx + 2*P_SPEED*(i-2) + COIN_GAP;
                                                objects[i].posy = objects[0].posy - P_JUMP*2;
                                                objects[i].width = 5;
                                                objects[i].height = 5;
                                                objects[i].color = 30;
                                        }
                                        repaint(objects_count,objects);
                                        sleep(1);
                                        break;
                                case MSG_MOUSE_CLICK_LEFT:
                                        objects[0].posx +=P_SPEED;
                                        objects[0].posy -=P_JUMP*2;
                                        objects[1].width -=P_SPEED*2;
                                        objects[2].posx = objects[1].width + GAP_WIDTH;
                                        objects[2].width += P_SPEED*2; 
                                        if(objects[1].width<0)   
                                                objects[1].width =vga_width;
                                        if(objects[2].width>vga_width) 
                                        {
                                                objects[2].posx = vga_width;
                                                objects[2].width = 0;
                                        }
                                        
                                        if(objects_count>3)
                                        {
                                                objects[objects_count-1].color = 3;
                                                remain--;
                                                objects_count--;
                                        }
                                        for(i=3;i<objects_count;i++)
                                        {
                                                objects[i].posx = objects[0].posx + 2*P_SPEED*(i-2) + COIN_GAP;
                                                
                                                objects[i].width = 5;
                                                objects[i].height = 5;
                                                objects[i].color = 30;
                                        }
                                        
                                         
                                        repaint(objects_count,objects);
                                        objects[0].posx +=P_SPEED;
                                        objects[0].posy +=P_JUMP*2;
                                        objects[1].width -=P_SPEED*2;
                                        objects[2].posx = objects[1].width + GAP_WIDTH;
                                        objects[2].width += P_SPEED*2;
                                        if(objects[1].width<0)   
                                                objects[1].width =vga_width;
                                        if(objects[2].width>vga_width) 
                                        {
                                                objects[2].posx = vga_width;
                                                objects[2].width = 0;
                                        }
                                        
                                        for(i=3;i<objects_count;i++)
                                        {
                                                objects[i].posx = objects[0].posx + 2*P_SPEED*(i-2) + COIN_GAP;
                                                
                                                objects[i].width = 5;
                                                objects[i].height = 5;
                                                objects[i].color = 30;
                                        }
                                        
                                        sleep(1);
                                        repaint(objects_count,objects);
                                        sleep(1);
                                        break;
                                case MSG_MOUSE_CLICK_RIGHT:
                                        exit_graphics();
                                        return 0;
                                        break;
                                
                                default:
                                        
                                        break;
                        }

               }
         
GAMEOVER:       objects[0].posx=0;
                objects[0].posy=0;
                objects[0].width=vga_width;
                objects[0].height = vga_height;
                objects[0].color = 9;
                repaint(1,objects);
                sleep(1);
        while(1)
        {
                for(i=0;i<sizeof(message);i++)
                {
                        *(char*)msg=0;
                }
                get_message(msg);
                switch(msg->mid)
                {
                        case MSG_MOUSE_CLICK_LEFT:
                               objects[0].posx = P_X;
                               objects[0].posy = ps_y;
                               objects[0].width = P_WIDTH;
                               objects[0].height = P_HEIGHT;
                               objects[0].color = 12;
                               
                               objects[1].posx = 0;
                               objects[1].posy = ps_y+P_HEIGHT+2;
                               objects[1].width =  vga_width;
                               objects[1].height = vga_height - objects[1].posy;
                               objects[1].color = 20;

                               objects[2].posy = ps_y+P_HEIGHT+2;
                               objects[2].posx = vga_width;
                               objects[2].width = 0;
                               objects[2].height = vga_height - objects[2].posy;
                               objects[2].color = 20;
                               objects_count =3;
                               remain = objects_count-3;
                               repaint(3,objects);
                               sleep(1);
                               goto GAME;
                        case MSG_MOUSE_CLICK_RIGHT:
                                exit_graphics();
                                return 0;
                                break;
                        default:
                                break;

                }
                
        }


}



