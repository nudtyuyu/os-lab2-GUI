/*
 *  /kernel/timer.h
 *
 *  (C) 2022  Healer
 */



#ifndef _TIMER_H
#define _TIMER_H


typedef struct
{
        long usr_jiffies;
        int type;
        long init_jiffies;
        int pid;
        struct user_timer *next;

}user_timer;

typedef struct
{
        user_timer *head;
        user_timer *rear;
        int num;
}user_timer_list;




#endif