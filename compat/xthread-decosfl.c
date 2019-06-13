/*
** Copyright (C) 2011 XXX all rights reserved.
**
** Copy thread and mutex controls from Objective C. 
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#include <pthread.h>
#include "../xpower-thread.h"
 
/* key structure for maintaining thread specific storage */
static pthread_key_t __xthread_storage;
 
/* initialize the threads subsystem */
int __xinit_thread_system(void){
    /* initialize the thread storage key */
    return pthread_keycreate(&__xthread_storage, NULL);
}
 
/* close the threads subsystem */
int __xclose_thread_system(void){
    /* destroy the thread storage key */
    /* not implemented yet */
    /* return pthread_key_delete(&__xthread_storage); */
    return 0;
}
 
/* create a new thread of execution */
_xthread_t __xthread_detach(void (*func)(void *arg), void *arg){
    _xthread_t thread_id;
    pthread_t new_thread_handle;
 
    if (pthread_create(&new_thread_handle, pthread_attr_default,(void *)func, arg) == 0){
        /* ??? may not work! (64bit) */
        thread_id = *(_xthread_t *)&new_thread_handle;
        pthread_detach(&new_thread_handle); /* fully detach thread */
    }
    else
        thread_id = NULL;
   
    return thread_id;
}
 
/* set the current thread's priority */
int __xthread_set_priority(int priority){
    int sys_priority = 0;
 
    switch (priority){
    case _XTHREAD_INTERACTIVE_PRIORITY:sys_priority = (PRI_FG_MIN_NP + PRI_FG_MAX_NP)/2;break;
    default:
    case _XTHREAD_BACKGROUND_PRIORITY :sys_priority = (PRI_BG_MIN_NP + PRI_BG_MAX_NP)/2;break;
    case _XTHREAD_LOW_PRIORITY        :sys_priority = (PRI_BG_MIN_NP + PRI_BG_MAX_NP)/2;break;
    }
     
    /* change the priority */
    if (pthread_setprio(pthread_self(), sys_priority) >= 0)
        return 0;
    else
        /* failed */
        return -1;
}
 
/* return the current thread's priority */
int __xthread_get_priority(void){
    int sys_priority;
     
    if ((sys_priority = pthread_getprio(pthread_self())) >= 0){
        if (sys_priority >= PRI_FG_MIN_NP && sys_priority <= PRI_FG_MAX_NP)
            return _XTHREAD_INTERACTIVE_PRIORITY;
        if (sys_priority >= PRI_BG_MIN_NP && sys_priority <= PRI_BG_MAX_NP)
            return _XTHREAD_BACKGROUND_PRIORITY;
        return _XTHREAD_LOW_PRIORITY;
    }
 
    /* failed */
    return -1;
}
 
/* yield our process time to another thread */
void __xthread_yield(void){
    pthread_yield();
}
 
/* terminate the current thread */
int __xthread_exit(void){
    /* exit the thread */
    pthread_exit(&_xthread_exit_status);
    /* failed if we reached here */
    return -1;
}
 
/* returns an integer value which uniquely describes a thread */
_xthread_t __xthread_id(void){
    pthread_t self = pthread_self();
    return (_xthread_t)pthread_getunique_np(&self);
}
 
/* sets the thread's local storage pointer */
int __xthread_set_data(void *value){
    return pthread_setspecific(__xthread_storage, value);
}
 
/* returns the thread's local storage pointer */
void *__xthread_get_data(void){
    void *value = NULL;
    if (!(pthread_getspecific(__xthread_storage, &value)))
        return value;
 
    return NULL;
}
 
/* allocate a mutex */
int __xmutex_allocate(_xmutex_t mutex){
    if (pthread_mutex_init((pthread_mutex_t *)(&(mutex->backend)),
        pthread_mutexattr_default))
        return -1;
    else
        return 0;
}
 
/* deallocate a mutex */
int __xmutex_deallocate(_xmutex_t mutex){
    if (pthread_mutex_destroy((pthread_mutex_t *)(&(mutex->backend))))
        return -1;
    else
        return 0;
}
 
/* grab a lock on a mutex */
int __xmutex_lock(_xmutex_t mutex){
    return pthread_mutex_lock((pthread_mutex_t *)(&(mutex->backend)));
}
 
/* try to grab a lock on a mutex */
int __xmutex_trylock(_xmutex_t mutex){
    if (pthread_mutex_trylock((pthread_mutex_t *)(&(mutex->backend))) != 1)
        return -1;
    else
        return 0;
}
 
/* unlock the mutex */
int __xmutex_unlock(_xmutex_t mutex){
    return pthread_mutex_unlock((pthread_mutex_t *)(&(mutex->backend)));
}
 
/* allocate a condition */
int __xcondition_allocate(_xcondition_t condition){
    /* unimplemented. */
    return -1;
    /*
    if (pthread_cond_init((pthread_cond_t *)(&(condition->backend)), NULL))
        return -1;
    else
        return 0;
     */
}
 
/* deallocate a condition */
int __xcondition_deallocate(_xcondition_t condition){
    /* unimplemented. */
    return -1;
    /* return pthread_cond_destroy((pthread_cond_t *)(&(condition->backend)));*/
}
 
/* wait on the condition */
int __xcondition_wait(_xcondition_t condition, _xmutex_t mutex){
    /* unimplemented. */
    return -1;
    /* return pthread_cond_wait((pthread_cond_t *)(&(condition->backend)),
                                (pthread_mutex_t *)(&(mutex->backend)));
     */
}
 
/* wake up all threads waiting on this condition */
int __xcondition_broadcast(_xcondition_t condition){
    /* unimplemented. */
    return -1;
    /* return pthread_cond_broadcast((pthread_cond_t *)(&(condition->backend))); */
}
 
/* wake up one thread waiting on this condition */
int __xcondition_signal(_xcondition_t condition){
    /* unimplemented. */
    return -1;
    /* return pthread_cond_signal((pthread_cond_t *)(&(condition->backend))); */
}

