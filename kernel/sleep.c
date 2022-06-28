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
void handler(int sig)
{}
/*unsigned int sys_sleep(unsigned int seconds)
{
        //int err = errno;
        struct sigaction act,action;
        int newmask;
        char * from;
        char *to;
        struct sigaction tmp;
        int signum = SIGALRM;
        int i;
        act.sa_handler = handler;
        act.sa_mask =0;
        act.sa_flags =0;
        sys_sigaction(SIGALRM,SIG_IGN);
        /*if (signum<1 || signum>32 || signum==SIGKILL)
		return -1;
	tmp = current->sigaction[signum-1];
        from = (char*)(&tmp);
        to =   (char*) (&action);
        //verify_area(action, sizeof(struct sigaction));
	for (i=0 ; i< sizeof(struct sigaction) ; i++) {
		*(to++) = *(from++);
	}
        from = (char*)(&act);
        to = (char*) (signum-1+current->sigaction);
	for (i=0 ; i< sizeof(struct sigaction) ; i++)
		*((to)++) = *((from)++);
	
	if (current->sigaction[signum-1].sa_flags & SA_NOMASK)
		current->sigaction[signum-1].sa_mask = 0;
	else
		current->sigaction[signum-1].sa_mask |= (1<<(signum-1));
        //sigemptyset(&newmask);
        //newmask&=0;
        //sigaddset(&newmask,SIGALRM);
        //newmask &= (1<<(SIGALRM-1));
        //int old = sys_ssetmask(newmask);
        //sigprocmask(SIG_BLOCK,&newmask,&oldmask);

        sys_alarm(seconds);

        sys_pause();

        int unslept = sys_alarm(0);
        /*
	if (signum<1 || signum>32 || signum==SIGKILL)
		return -1;
	tmp = current->sigaction[signum-1];
        from = (char*)(&action);
        to = (char*) (signum-1+current->sigaction);
	for (i=0 ; i< sizeof(struct sigaction) ; i++)
		*((to)++) = *((from)++);
	
	if (current->sigaction[signum-1].sa_flags & SA_NOMASK)
		current->sigaction[signum-1].sa_mask = 0;
	else
		current->sigaction[signum-1].sa_mask |= (1<<(signum-1));
        current->sigaction[signum-1].sa_handler = handler;
	//return 0;
        //sys_sigaction(SIGALRM,SIG_IGN,NULL);
        //sys_ssetmask(old);
        //errno = err;
        return (unslept);       
}*/

/*int sys_sleep(unsigned int seconds)
{
        unsigned int remaining, slept;
        long before, after;


        if (seconds == 0)
        return 0;
        before = sys_time((long *) NULL);
        remaining = sys_alarm(seconds);
        after = sys_time((long *) NULL);

        sys_pause();

        sys_signal(SIGALRM, handler);

        slept = (after - before) / 1000;
        if (remaining > slept)
        sys_alarm(remaining - slept);

        return slept;

}*/

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

