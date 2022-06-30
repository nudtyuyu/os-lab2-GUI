

#include <message.h>
#include <linux/sched.h>
#include <linux/tty.h>
#include <asm/segment.h>
#include <asm/system.h>
#include <asm/io.h>
#include <unistd.h>
#include <timer.h>

user_timer_list *UL=NULL;

void UserTimer_Init(user_timer_list *L)
{
        L->head = (user_timer*)malloc(sizeof(user_timer));
        L->rear = L->head;
}

void sys_timer_create(long milliseconds)
{
        long usr_jiffies = milliseconds/10;

        user_timer * timer =(user_timer*)malloc(sizeof(user_timer));
        timer->usr_jiffies = usr_jiffies;
        timer->init_jiffies = usr_jiffies;
        timer->pid=-1;
        insert_user_timer(timer);
}

void insert_user_timer(user_timer * timer)
{
        if(UL==NULL)
        {
                UL=(user_timer_list*)malloc(sizeof(user_timer_list));
                UserTimer_Init(UL);
        }
        if(UL->head == UL->rear)
        {
                UL->head->next = timer;
                UL->rear = timer;
                timer->next= NULL;

        }
        else
        {
                UL->rear->next = timer;
                UL->rear = timer;
                timer->next = NULL;
        }
        UL->num++;

}

void scan_user_timer_list()
{
        if(UL==NULL)
                return;
        user_timer *tmp = UL->head->next;
        while(tmp)
        {
                tmp->usr_jiffies--;
                if(tmp->usr_jiffies==0)
                {
                        message *msg = (message*)malloc(sizeof(message));
                        msg->mid = USER_TIMER_UP;
                        msg->pid = -1;
                        msg->next = NULL;
                        sys_post_message(msg);
                }
                tmp=tmp->next;
        }
        UL->rear->usr_jiffies--;
}