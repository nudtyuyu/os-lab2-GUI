/*
 *  linux/lib/sleep.c
 *
 *  (C) 2022  Healer
 */

#define __LIBRARY__
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <linux/sched.h>



//_syscall1(unsigned int,sleep,unsigned int,seconds);

unsigned int sys_sleep(unsigned int seconds);

void sys_pipe2()
{}
     


unsigned int sys_sleep(unsigned int seconds)
{
        
        int old_blocked = current->blocked;
        current->blocked = current->blocked & (1<<(SIGALRM-1));
        sys_alarm(seconds);
        sys_pause();
        if(sys_signal(SIGALRM,SIG_IGN)==-1)
        {
                return -1;
        }
        
        current->blocked = old_blocked;
        return 0;
}

