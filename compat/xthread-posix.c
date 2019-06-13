/*
** Copyright (C) 2011 XXX all rights reserved.
**
** Copy thread and mutex controls from Objective C. 
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#include "../xpower-thread.h"
#include <pthread.h>

/* key structure for maintaining thread specific storage */
static pthread_key_t __xthread_storage;
static pthread_attr_t __xthread_attribs;

/* initialize the threads subsystem */
int __xinit_thread_system(void){
    /* initialize the thread storage key */
    if (pthread_key_create(&__xthread_storage, NULL) == 0){
        /*
         * the normal default detach state for threads is PTHREAD_CREATE_JOINABLE
         * which causes threads to not die when you think they should
         */
        if (pthread_attr_init(&__xthread_attribs) == 0){
            if (pthread_attr_setdetachstate(&__xthread_attribs,PTHREAD_CREATE_DETACHED) == 0)
                return 0;
        }
    }
    
    return -1;
}

/* close the threads subsystem */
int __xclose_thread_system(void){
    if (pthread_key_delete(__xthread_storage) == 0){
        if (pthread_attr_destroy(&__xthread_attribs) == 0)
            return 0;
    }

    return -1;
}

/* create a new thread of execution */
_xthread_t __xthread_detach(void (*func)(void *arg), void *arg){
    _xthread_t thread_id = 0;
    pthread_t new_thread_handle;
    
    if (!(pthread_create(&new_thread_handle, &__xthread_attribs,(void *)func, arg)))
        thread_id = *(_xthread_t *)&new_thread_handle;
    else
        thread_id = NULL;
        
    return thread_id;
}

/* 
 * set the current thread's priority
 * be aware that the default schedpolicy often disallows thread priorities 
 */
int __xthread_set_priority(int priority){
    pthread_t thread_id = pthread_self();
    int policy;
    struct sched_param params;
    int priority_min, priority_max;
    
    if (pthread_getschedparam(thread_id, &policy, &params) == 0){
        if ((priority_max = sched_get_priority_max(policy)) != 0)
            return -1;
        if ((priority_min = sched_get_priority_min(policy)) != 0)
            return -1;
        if (priority > priority_max)
            priority = priority_max;
        else if (priority < priority_min)
            priority = priority_min;
        params.sched_priority = priority;
        /*
         * the solaris 7 and several other man pages incorrectly state that
         * this should be a pointer to policy but pthread.h is universally
         * at odds with this.
         */
        if (pthread_setschedparam(thread_id, policy, &params) == 0)
            return 0;
    }
      
    return -1;
}

/* return the current thread's priority */
int __xthread_get_priority(void){
    int policy;
    struct sched_param params;
    
    if (pthread_getschedparam(pthread_self(), &policy, &params) == 0)
        return params.sched_priority;
    else
        return -1;
}

/* yield our process time to another thread */
void __xthread_yield(void){
    sched_yield();
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
    return *(_xthread_t *)&self;
}

/* sets the thread's local storage pointer. */
int __xthread_set_data(void *value){
    if (pthread_setspecific(__xthread_storage, value) == 0)
      return 0;
    else
      return -1;
}

/* returns the thread's local storage pointer */
void *__xthread_get_data(void){
    return pthread_getspecific(__xthread_storage);
}

/* allocate a mutex */
int __xmutex_allocate(_xmutex_t mutex){
    mutex->backend = malloc(sizeof(pthread_mutex_t));
    
    if (pthread_mutex_init((pthread_mutex_t *)mutex->backend, NULL)){
        free(mutex->backend);
        mutex->backend = NULL;
        return -1;
    }
    
    return 0;
}

/* deallocate a mutex */
int __xmutex_deallocate(_xmutex_t mutex){
    int count = 1;
    
    /*
     * Posix threads specifically require that the thread be unlocked for
     * pthread_mutex_destroy to work.
     */
    
    while (count){
        if ((count = pthread_mutex_unlock((pthread_mutex_t*)mutex->backend)) < 0)
            return -1;
    }
    
    if (pthread_mutex_destroy((pthread_mutex_t *)mutex->backend))
        return -1;
    
    free(mutex->backend);
    mutex->backend = NULL;
    return 0;
}

/* grab a lock on a mutex */
int __xmutex_lock(_xmutex_t mutex){
    if (pthread_mutex_lock((pthread_mutex_t *)mutex->backend) == 0)
        return 0;
    else
        return -1;
}

/* try to grab a lock on a mutex */
int __xmutex_trylock(_xmutex_t mutex){
    if (pthread_mutex_trylock((pthread_mutex_t *)mutex->backend) == 0)
        return 0;
    else
        return -1;
}

/* unlock the mutex */
int __xmutex_unlock(_xmutex_t mutex){
    if (pthread_mutex_unlock((pthread_mutex_t *)mutex->backend) == 0)
        return 0;
    else
        return -1;
}

/* allocate a condition */
int __xcondition_allocate(_xcondition_t condition){
    condition->backend = malloc(sizeof(pthread_cond_t));
    
    if (pthread_cond_init((pthread_cond_t *)condition->backend, NULL)){
        free(condition->backend);
        condition->backend = NULL;
        return -1;
    }
    
    return 0;
}

/* deallocate a condition */
int __xcondition_deallocate(_xcondition_t condition){
    if (pthread_cond_destroy((pthread_cond_t *)condition->backend))
        return -1;
    
    free(condition->backend);
    condition->backend = NULL;
    return 0;
}

/* wait on the condition */
int __xcondition_wait(_xcondition_t condition, _xmutex_t mutex){
    if (pthread_cond_wait((pthread_cond_t *)condition->backend,
                          (pthread_mutex_t *)mutex->backend) == 0)
        return 0;
    else
        return -1;
}

/* wake up all threads waiting on this condition */
int __xcondition_broadcast(_xcondition_t condition){
    if (pthread_cond_broadcast((pthread_cond_t *)condition->backend) == 0)
        return 0;
    else
        return -1;
}

/* wake up one thread waiting on this condition */
int __xcondition_signal(_xcondition_t condition){
    if (pthread_cond_signal((pthread_cond_t *)condition->backend) == 0)
        return 0;
    else
        return -1;
}

