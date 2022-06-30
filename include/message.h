/*
 *  /kernel/graphics.c
 *
 *  (C) 2022  Healer
 */



#ifndef _MSG_H
#define _MSG_H

#define MSG_MOUSE_CLICK_LEFT 1
#define MSG_MOUSE_CLICK_RIGHT 2
#define USER_TIMER_UP 3
typedef struct
{
        int mid;
        int pid;
        struct message *next;

}message;

typedef struct MList
{
        message *head;
        message *rear;
        int num;
}MsgList;

int sys_get_message(message *msg);
int sys_post_message(message *msg);


#endif
