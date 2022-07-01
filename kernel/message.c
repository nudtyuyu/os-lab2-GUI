

#include <message.h>
#include <linux/sched.h>
#include <linux/tty.h>
#include <asm/segment.h>
#include <asm/system.h>
#include <asm/io.h>
#include <unistd.h>
MsgList * MsgL=NULL;


void MsgList_Init(MsgList*ML)
{
        //ML = (MsgList*)malloc(sizeof(MsgList));
        ML->head = (message*)malloc(sizeof(message));
        ML->rear = ML->head;
        ML->head->next = NULL;
        ML->num =0;
}


int sys_get_message(message *msg)
{
        if(msg==NULL)
        {
                msg = (message*)malloc(sizeof(message));
        }
        message * tmp;
        cli();
        if(MsgL==NULL || MsgL->head->next==NULL)
        {
                sti();
                return -1;
        }
        else
        {
                if(MsgL->rear==MsgL->head->next)
                {
                        MsgL->rear = MsgL->head;
                        tmp = MsgL->head->next;
                        MsgL->head->next = NULL;
                }
                else
                {
                        tmp = MsgL->head->next;
                        MsgL->head->next = tmp->next;
                }
                MsgL->num--;
        }
        
        int i;
        for(i=0;i<sizeof(message);i++)
        {
                put_fs_byte(*((char *)tmp+i),(char*)msg+i);
        }
        sti();
        return 0;
}


int sys_post_message(message *msg)
{
        cli();
        if(MsgL==NULL)
        {
               MsgL=(MsgList *)malloc(sizeof(MsgList));
               MsgList_Init(MsgL); 
        }
        if(msg==NULL)
        {
                printk("msg is NULL\n");
                sti();
                return -1;
        }
        
        
        if(MsgL->rear == MsgL->head)
        {
                MsgL->head->next = msg;
                MsgL->rear = msg;
                msg->next = NULL;
        }
        else
        {
                MsgL->rear->next = msg;
                MsgL->rear = msg;
                msg->next = NULL;
        }
        MsgL->num++;
        sti();
        return 0;
}