/*
** Copyright (C) 2011 XXX all rights reserved.
**
** Copy thread and mutex controls from Objective C. 
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#include "../xpower-thread.h"

/* thread local storage for a single thread */
static void *__xthread_storage = NULL;

/* initialize the threads subsystem */
int __xinit_thread_system(void){
    /* no thread support available */
    return -1;
}

/* close the threads subsystem */
int __xclose_thread_system(void){
    /* no thread support available */
    return -1;
}

/* create a new thread of execution */
_xthread_t __xthread_detach(void (*func)(void *arg), void *arg){
    /* no thread support available */   
    return NULL;
}

/* 
 * set the current thread's priority
 * be aware that the default schedpolicy often disallows thread priorities 
 */
int __xthread_set_priority(int priority){
    /* no thread support available */  
    return -1;
}

/* return the current thread's priority */
int __xthread_get_priority(void){
    return _XTHREAD_INTERACTIVE_PRIORITY;
}

/* yield our process time to another thread */
void __xthread_yield(void){
    return;
}

/* terminate the current thread */
int __xthread_exit(void){
    /* no thread support available */ 
    /* should we really exit the program */
    /* exit(&_xthread_exit_status); */
    return -1;
}

/* returns an integer value which uniquely describes a thread */
_xthread_t __xthread_id(void){
    /* no thread support available, use 1 */ 
    return (_xthread_t)1;
}

/* sets the thread's local storage pointer. */
int __xthread_set_data(void *value){
    __xthread_storage = value;
    return 0;
}

/* returns the thread's local storage pointer */
void *__xthread_get_data(void){
    return __xthread_storage;
}

/* allocate a mutex */
int __xmutex_allocate(_xmutex_t mutex){
    return 0;
}

/* deallocate a mutex */
int __xmutex_deallocate(_xmutex_t mutex){
    return 0;
}

/* grab a lock on a mutex */
int __xmutex_lock(_xmutex_t mutex){
    /* there can only be one thread, so we always get the lock */
    return 0;
}

/* try to grab a lock on a mutex */
int __xmutex_trylock(_xmutex_t mutex){
    /* there can only be one thread, so we always get the lock */
    return 0;
}

/* unlock the mutex */
int __xmutex_unlock(_xmutex_t mutex){
    return 0;
}

/* allocate a condition */
int __xcondition_allocate(_xcondition_t condition){
    return 0;
}

/* deallocate a condition */
int __xcondition_deallocate(_xcondition_t condition){
    return 0;
}

/* wait on the condition */
int __xcondition_wait(_xcondition_t condition, _xmutex_t mutex){
    return 0;
}

/* wake up all threads waiting on this condition */
int __xcondition_broadcast(_xcondition_t condition){
    return 0;
}

/* wake up one thread waiting on this condition */
int __xcondition_signal(_xcondition_t condition){
    return 0;
}

