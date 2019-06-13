/*
** Copyright (C) 2011 XXX all rights reserved.
**
** Copy thread and mutex controls from Objective C. 
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#include "../xpower-thread.h"

#define INCL_DOSSEMAPHORES
#define INCL_DOSPROCESS

#include <os2.h>
#include <stdlib.h>

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
    int thread_id = 0;
    
    if ((thread_id = _beginthread (func,NULL,32768,arg)) < 0)
        thread_id = 0;
     
    return (_xthread_t)thread_id;
}

/* 
 * set the current thread's priority
 */
int __xthread_set_priority(int priority){
    ULONG sys_class = 0;
    ULONG sys_priority = 0;
    
    /* _XTHREAD_INTERACTIVE_PRIORITY -> PRTYC_FOREGROUNDSERVER
     * _XTHREAD_BACKGROUND_PRIORITY  -> PRTYC_REGULAR
     * _XTHREAD_LOW_PRIORITY         -> PRTYC_IDLETIME 
     */
     
    switch (priority){
    case _XTHREAD_INTERACTIVE_PRIORITY:sys_class = PRTYC_REGULAR;sys_priority = 10;break;
    default:
    case _XTHREAD_BACKGROUND_PRIORITY :sys_class = PRTYC_IDLETIME;sys_priority = 25;break;
    case _XTHREAD_LOW_PRIORITY        :sys_class = PRTYC_IDLETIME;sys_priority = 0;break;
    }
    
    /* change priority */
    if (!DosSetPriority (PRTYS_THREAD,sys_class,sys_priority,*_threadid))
        return 0;
    else
        return -1;
}

/* return the current thread's priority */
int __xthread_get_priority(void){
    PTIB ptib;
    PPIB ppib;
    
    /* get information about current thread */
    DosGetInfoBlocks (&ptib,&ppib);
    
    switch (ptib->tib_ptib2->tib2_ulpri){
    case PRTYC_IDLETIME:
    case PRTYC_REGULAR:
    case PRTYC_TIMECRITICAL:
    case PRTYC_FOREGROUNDSERVER:
    default:return _XTHREAD_INTERACTIVE_PRIORITY;
    }
    
    return -1;
}

/* yield our process time to another thread */
void __xthread_yield(void){
    DosSleep(0);
}

/* terminate the current thread */
int __xthread_exit(void){
    /* terminate the thread, NEVER use DosExit() */
    _endthread();
    /* failed if we reached here */
    return -1;
}

/* returns an integer value which uniquely describes a thread */
_xthread_t __xthread_id(void){
    return (_xthread_t)*_threadid;;
}

/* sets the thread's local storage pointer. */
int __xthread_set_data(void *value){
    *_threadstore() = value;
    return 0;
}

/* returns the thread's local storage pointer */
void *__xthread_get_data(void){
    return *_threadstore();
}

/* allocate a mutex */
int __xmutex_allocate(_xmutex_t mutex){
    if (DosCreateMutexSem (NULL, (HMTX)(&(mutex->backend)),0L,0) > 0)
        return -1;
    else
        return 0;
}

/* deallocate a mutex */
int __xmutex_deallocate(_xmutex_t mutex){
    DosCloseMutexSem((HMTX)(mutex->backend));
    return 0;
}

/* grab a lock on a mutex */
int __xmutex_lock(_xmutex_t mutex){
    if (DosRequestMutexSem((HMTX)(mutex->backend),-1L) != 0)
        return -1;
    else
        return 0;
}

/* try to grab a lock on a mutex */
int __xmutex_trylock(_xmutex_t mutex){
    if (DosRequestMutexSem((HMTX)(mutex->backend),0L) != 0)
        return -1;
    else
        return 0;
}

/* unlock the mutex */
int __xmutex_unlock(_xmutex_t mutex){
    if (DosReleaseMutexSem((HMTX)(mutex->backend)) != 0)
        return -1;
    else
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

