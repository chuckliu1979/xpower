/*
** Copyright (C) 2011 XXX all rights reserved.
**
** Copy thread and mutex controls from Objective C. 
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#include "../xpower-thread.h"

#include <thread.h>
#include <synch.h>
#include <errno.h>

/* key structure for maintaining thread specific storage */
static thread_key_t __xthread_storage;

/* initialize the threads subsystem */
int __xinit_thread_system(void){
    /* initialize the thread storage key */
    if (thr_keycreate(&__xthread_storage, NULL) == 0)
        return 0;
    else
        return -1;
}

/* close the threads subsystem */
int __xclose_thread_system(void){
    return 0;
}

/* create a new thread of execution */
_xthread_t __xthread_detach(void (*func)(void *arg), void *arg){
    _xthread_t thread_id;
    thread_t new_thread_id = 0;
    
    if (thr_create(NULL, 0, (void *)func, arg,
                   THR_DETACHED | THR_NEW_LWP,
                   &new_thread_id) == 0)
        thread_id = *(_xthread_t *)&new_thread_id;
    else
        thread_id = NULL;
     
    return thread_id;
}

/* 
 * set the current thread's priority
 * be aware that the default schedpolicy often disallows thread priorities 
 */
int __xthread_set_priority(int priority){
    int sys_priority = 0;
    
    switch (priority){
    case _XTHREAD_INTERACTIVE_PRIORITY:sys_priority = 300;break;
    default:
    case _XTHREAD_BACKGROUND_PRIORITY :sys_priority = 200;break;
    case _XTHREAD_LOW_PRIORITY        :sys_priority = 1000;break;
    }
    
    /* change priority */
    if (thr_setprio(thr_self(), sys_priority) == 0)
        return 0;
    else
        return -1;
}

/* return the current thread's priority */
int __xthread_get_priority(void){
    int sys_priority;
                                                      
    if (thr_getprio(thr_self(), &sys_priority) == 0){
        if (sys_priority >= 250)
            return _XTHREAD_INTERACTIVE_PRIORITY;
        else if (sys_priority >= 150)
            return _XTHREAD_BACKGROUND_PRIORITY;
        return _XTHREAD_LOW_PRIORITY;
    }
    
    /* couldn't get priority */
    return -1;
}

/* yield our process time to another thread */
void __xthread_yield(void){
    thr_yield();
}

/* terminate the current thread */
int __xthread_exit(void){
    /* exit the thread */
    thr_exit(&_xthread_exit_status);
    /* failed if we reached here */
    return -1;
}

/* returns an integer value which uniquely describes a thread */
_xthread_t __xthread_id(void){
    return (_xthread_t)thr_self();
}

/* sets the thread's local storage pointer. */
int __xthread_set_data(void *value){
    if (thr_setspecific(__xthread_storage, value) == 0)
        return 0;
    else
        return -1;
}

/* returns the thread's local storage pointer */
void *__xthread_get_data(void){
    void *value = NULL;
    
    if (thr_getspecific(__xthread_storage, &value) == 0)
      return value;
    
    return NULL;
}

/* allocate a mutex */
int __xmutex_allocate(_xmutex_t mutex){
    if (mutex_init( (mutex_t *)(&(mutex->backend)), USYNC_THREAD, 0))
        return -1;
    else
        return 0;
}

/* deallocate a mutex */
int __xmutex_deallocate(_xmutex_t mutex){
    mutex_destroy((mutex_t *)(&(mutex->backend)));
    return 0;
}

/* grab a lock on a mutex */
int __xmutex_lock(_xmutex_t mutex){
    if (mutex_lock((mutex_t *)(&(mutex->backend))) != 0)
        return -1;
    else
        return 0;
}

/* try to grab a lock on a mutex */
int __xmutex_trylock(_xmutex_t mutex){
    if (mutex_trylock((mutex_t *)(&(mutex->backend))) != 0)
        return -1;
    else
        return 0;
}

/* unlock the mutex */
int __xmutex_unlock(_xmutex_t mutex){
    if (mutex_unlock((mutex_t *)(&(mutex->backend))) != 0)
        return -1;
    else
        return 0;
}

/* allocate a condition */
int __xcondition_allocate(_xcondition_t condition){
    return cond_init((cond_t *)(&(condition->backend)), USYNC_THREAD, NULL);
}

/* deallocate a condition */
int __xcondition_deallocate(_xcondition_t condition){
    return cond_destroy((cond_t *)(&(condition->backend)));
}

/* wait on the condition */
int __xcondition_wait(_xcondition_t condition, _xmutex_t mutex){
    return cond_wait((cond_t *)(&(condition->backend)),
                     (mutex_t *)(&(mutex->backend)));
}

/* wake up all threads waiting on this condition */
int __xcondition_broadcast(_xcondition_t condition){
    return cond_broadcast((cond_t *)(&(condition->backend)));
}

/* wake up one thread waiting on this condition */
int __xcondition_signal(_xcondition_t condition){
    return cond_signal((cond_t *)(&(condition->backend)));
}

