/*
** Copyright (C) 2011 XXX all rights reserved.
**
** Copy thread and mutex controls from Objective C. 
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#include <mach/mach.h>
#include <mach/cthreads.h>
#include "../xpower-thread.h"

/*
 * obtain the maximum thread priority that can set for t.under the
 * mach threading model, it is possible for the developer to adjust the
 * maximum priority downward only-- cannot be raised without superuser
 * privileges.once lowered, it cannot be raised.
 */
static int __xmach_get_max_thread_priority(cthread_t t, int *base){
    thread_t threadP;
    kern_return_t error;
    struct thread_sched_info info;
    unsigned int info_count=THREAD_SCHED_INFO_COUNT;
       
    if (t == NULL)
        return -1;
    
    threadP = cthread_thread(t); /* get thread underlying */
    error = thread_info(threadP, THREAD_SCHED_INFO,
                       (thread_info_t) &info, &info_count);
    
    if (error != KERN_SUCCESS)
        return -1;
    
    if (base != NULL)
        *base = info.base_priority;
    
    return info.max_priority;
}

/* initialize the threads subsystem */
int __xinit_thread_system(void){
    return 0;
}

/* close the threads subsystem */
int __xclose_thread_system(void){
    return 0;
}

/* create a new thread of execution */
_xthread_t __xthread_detach(void (*func)(void *arg), void *arg){
    _xthread_t thread_id;
    cthread_t new_thread_handle;
    
    /* create thread */
    new_thread_handle = cthread_fork((cthread_fn_t)func, arg);
    if (new_thread_handle){
        /* this is not terribly portable */
        thread_id = *(_xthread_t *)&new_thread_handle;
        cthread_detach(new_thread_handle);
    }
    else
        thread_id = NULL;
     
    return thread_id;
}

/* 
 * set the current thread's priority
 */
int __xthread_set_priority(int priority){
    _xthread_t *t = _xthread_id();
    cthread_t cT = (cthread_t)t;
    int maxPriority = __xmach_get_max_thread_priority(cT, NULL);
    int sys_priority = 0;
    
    if (maxPriority == -1)
        return -1;
    
    switch (priority){
    case _XTHREAD_INTERACTIVE_PRIORITY:sys_priority = maxPriority;break;
    case _XTHREAD_BACKGROUND_PRIORITY :sys_priority = (maxPriority*2)/3;break;
    case _XTHREAD_LOW_PRIORITY        :sys_priority = maxPriority/3;break;
    default:return -1;
    }
    
    if (sys_priority == 0)
        return -1;
    
    /* change the priority */
    if (cthread_priority(cT, sys_priority, 0) == KERN_SUCCESS)
        return 0;
    else
        return -1;
}

/* return the current thread's priority */
int __xthread_get_priority(void){
    _xthread_t *t = _xthread_id();
    cthread_t cT = (cthread_t)t; /* see _xthread_id() */
    int basePriority;
    int maxPriority;
    int sys_priority = 0;
    int interactiveT, backgroundT, lowT; /* thresholds */
    
    maxPriority = __xmach_get_max_thread_priority(cT, &basePriority);
    if (maxPriority == -1)
        return -1;
    
    if (basePriority > ((maxPriority*2)/3))
        return _XTHREAD_INTERACTIVE_PRIORITY;
    if (basePriority > (maxPriority/3))
        return _XTHREAD_BACKGROUND_PRIORITY;
    
    return _XTHREAD_LOW_PRIORITY;
}

/* yield our process time to another thread */
void __xthread_yield(void){
    cthread_yield();
}

/* terminate the current thread */
int __xthread_exit(void){
    /* exit the thread */ 
    cthread_exit(&_xthread_exit_status);
    /* failed if we reached here */
    return -1;
}

/* returns an integer value which uniquely describes a thread */
_xthread_t __xthread_id(void){
    cthread_t self = cthread_self(); 
    return *(_xthread_t *)&self;
}

/* sets the thread's local storage pointer. */
int __xthread_set_data(void *value){
    cthread_set_data(cthread_self(), (any_t)value);
    return 0;
}

/* returns the thread's local storage pointer */
void *__xthread_get_data(void){
    return (void *)cthread_data(cthread_self());
}

/* allocate a mutex */
int __xmutex_allocate(_xmutex_t mutex){
    int err = 0;
    mutex->backend = malloc(sizeof(struct mutex));
    err = mutex_init((mutex_t)(mutex->backend));
    
    if (err != 0){
        free (mutex->backend);
        return -1;
    }
    else
        return 0;
}

/* deallocate a mutex */
int __xmutex_deallocate(_xmutex_t mutex){
    mutex_clear((mutex_t)(mutex->backend));
    
    free (mutex->backend);
    mutex->backend = NULL;
    return 0;
}

/* grab a lock on a mutex */
int __xmutex_lock(_xmutex_t mutex){
    mutex_lock((mutex_t)(mutex->backend));
    return 0;
}

/* try to grab a lock on a mutex */
int __xmutex_trylock(_xmutex_t mutex){
    if (mutex_try_lock((mutex_t)(mutex->backend)) == 0)
        return -1;
    else
        return 0;
}

/* unlock the mutex */
int __xmutex_unlock(_xmutex_t mutex){
    mutex_unlock((mutex_t)(mutex->backend));
    return 0;
}

/* allocate a condition */
int __xcondition_allocate(_xcondition_t condition){
    condition->backend = malloc(sizeof(struct condition));
    condition_init((condition_t)(condition->backend));    
    return 0;
}

/* deallocate a condition */
int __xcondition_deallocate(_xcondition_t condition){
    condition_clear((condition_t)(condition->backend));
    free (condition->backend);
    condition->backend = NULL;
    return 0;
}

/* wait on the condition */
int __xcondition_wait(_xcondition_t condition, _xmutex_t mutex){
    condition_wait((condition_t)(condition->backend),
                   (mutex_t)(mutex->backend));
    return 0;
}

/* wake up all threads waiting on this condition */
int __xcondition_broadcast(_xcondition_t condition){
    condition_broadcast((condition_t)(condition->backend));
    return 0;
}

/* wake up one thread waiting on this condition */
int __xcondition_signal(_xcondition_t condition){
    condition_signal((condition_t)(condition->backend));
    return 0;
}

