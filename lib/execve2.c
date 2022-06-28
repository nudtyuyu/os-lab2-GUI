/*
 *  linux/lib/execve2.c
 *
 *  (C) 2022  Healer
 */

#define __LIBRARY__
#include <unistd.h>

_syscall3(int,execve2,const char *,file,char **,argv,char **,envp)