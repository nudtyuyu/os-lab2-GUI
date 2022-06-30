

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
        if(MsgL->head->next==NULL)
        {
                return -1;
        }
        else
        {
                if(MsgL->rear==MsgL->head->next)
                {
                        MsgL->rear = MsgL;
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
        sti();
        int i;
        for(i=0;i<sizeof(message);i++)
        {
                put_fs_byte(*(char *)tmp+i,(char*)msg+i);
        }
        return 0;
}


int sys_post_message(message *msg)
{
        if(MsgL==NULL)
        {
               MsgL=(MsgList *)malloc(sizeof(MsgList));
               MsgList_Init(MsgL); 
        }
        if(msg==NULL)
        {
                printk("msg is NULL\n");
                return -1;
        }
        cli();
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