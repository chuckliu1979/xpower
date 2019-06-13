/*
** Copyright (C) 2011 XXX all rights reserved.
**
** Copy thread and mutex controls from Objective C. 
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#include <stdlib.h>
#include <sys/types.h>
#include <sys/sysmp.h>
#include <sys/prctl.h>
#include <ulocks.h>
#include "../xpower-thread.h"
 
/* key structure for maintaining thread specific storage */
static void *__xshared_arena_handle = NULL;
 
/* initialize the threads subsystem */
int __xinit_thread_system(void){
    /* name of IRIX arena */
    char arena_name[64];
 
    /* construct a temporary name for arena */
    sprintf(arena_name, "/usr/tmp/x_%05u", (unsigned)getpid());
 
    /* up to 256 threads. arena only for threads */
    usconfig(CONF_INITUSERS, 256);
    usconfig(CONF_ARENATYPE, US_SHAREDONLY);
 
    /* initialize the arena */
    if (!(__xshared_arena_handle = usinit(arena_name)))/* failed */
        return -1;
 
    return 0;
}
 
/* close the threads subsystem. */
int __xclose_thread_system(void){
    return 0;
}
 
/* create a new thread of execution */
_xthread_t __xthread_detach(void (*func)(void *arg), void *arg){
    _xthread_t thread_id;
    int sys_id;
 
    if ((sys_id = sproc((void *)func, PR_SALL, arg)) >= 0)
        thread_id = (_xthread_t)sys_id;
    else
        thread_id = NULL;
   
    return thread_id;
}
 
/* set the current thread's priority. */
int __xthread_set_priority(int priority){
    /* not implemented yet */
    return -1;
}
 
/* return the current thread's priority. */
int __xthread_get_priority(void){
    /* not implemented yet */
    return _XTHREAD_INTERACTIVE_PRIORITY;
}
 
/* yield our process time to another thread */
void __xthread_yield(void){
    sginap(0);
}
 
/* terminate the current thread */
int __xthread_exit(void){
    /** IRIX only has exit */
    exit(_xthread_exit_status);
    /* failed if we reached here */
    return -1;
}
 
/* returns an integer value which uniquely describes a thread */
_xthread_t __xthread_id(void){
    /* threads are processes */
    return (_xthread_t)get_pid();
}
 
/* sets the thread's local storage pointer */
int __xthread_set_data(void *value){
    *((void **)&PRDA->usr_prda) = value;
    return 0;
}
 
/* returns the thread's local storage pointer */
void * __xthread_get_data(void){
    return *((void **)&PRDA->usr_prda);
}
 
/* allocate a mutex */
int __xmutex_allocate(_xmutex_t mutex){
    if (!((ulock_t)(mutex->backend) = usnewlock(__xshared_arena_handle)))
        return -1;
    else
        return 0;
}
 
/* deallocate a mutex */
int __xmutex_deallocate(_xmutex_t mutex){
    usfreelock((ulock_t)(mutex->backend), __xshared_arena_handle);
    return 0;
}
 
/* grab a lock on a mutex */
int __xmutex_lock(_xmutex_t mutex){
    if (ussetlock((ulock_t)(mutex->backend)) == 0)
        return -1;
    else
        return 0;
}
 
/* try to grab a lock on a mutex */
int __xmutex_trylock(_xmutex_t mutex){
    if (ustestlock((ulock_t)(mutex->backend)) == 0)
        return -1;
    else
        return 0;
}
 
/* unlock the mutex */
int __xmutex_unlock(_xmutex_t mutex){
    usunsetlock((ulock_t)(mutex->backend));
    return 0;
}
 
/* allocate a condition */
int __xcondition_allocate(_xcondition_t condition){
    /* unimplemented */
    return -1;
}
 
/* deallocate a condition */
int __xcondition_deallocate(_xcondition_t condition){
    /* unimplemented */
    return -1;
}
 
/* wait on the condition */
int __xcondition_wait(_xcondition_t condition, _xmutex_t mutex){
    /* unimplemented */
    return -1;
}
 
/* wake up all threads waiting on this condition */
int __xcondition_broadcast(_xcondition_t condition){
    /* unimplemented */
    return -1;
}
 
/* wake up one thread waiting on this condition */
int __xcondition_signal(_xcondition_t condition){
    /* unimplemented */
    return -1;
}

