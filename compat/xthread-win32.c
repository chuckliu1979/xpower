/*
** Copyright (C) 2011 XXX all rights reserved.
**
** Copy thread and mutex controls from Objective C. 
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#include "../xpower-thread.h"
#include <windows.h>

/* key structure for maintaining thread specific storage */
static DWORD __xdata_tls = (DWORD)-1;

/* initialize the threads subsystem */
int __xinit_thread_system(void){
    /* initialize the thread storage key */
    if ((__xdata_tls = TlsAlloc()) != (DWORD)-1)
        return 0;
    else
        return -1;
}

/* close the threads subsystem */
int __xclose_thread_system(void){
    if (__xdata_tls != (DWORD)-1)
        TlsFree(__xdata_tls);
    return 0;
}

/* create a new thread of execution */
_xthread_t __xthread_detach(void (*func)(void *arg), void *arg){
    DWORD thread_id = 0;
    HANDLE win32_handle;

    if (!(win32_handle = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)func,arg,0,&thread_id)))
        thread_id = 0;
        
    return (_xthread_t)(size_t)thread_id;
}

/* set the current thread's priority */
int __xthread_set_priority(int priority){
    int sys_priority = 0;
    
    switch (priority){
    case _XTHREAD_INTERACTIVE_PRIORITY: sys_priority = THREAD_PRIORITY_NORMAL;break;
    default:
    case _XTHREAD_BACKGROUND_PRIORITY : sys_priority = THREAD_PRIORITY_BELOW_NORMAL;break;
    case _XTHREAD_LOW_PRIORITY        : sys_priority = THREAD_PRIORITY_LOWEST; break;
    }
    
    /* change priority */
    if (SetThreadPriority(GetCurrentThread(), sys_priority))
        return 0;
    else
        return -1;
}

/* return the current thread's priority */
int __xthread_get_priority(void){
    int sys_priority;
    
    sys_priority = GetThreadPriority(GetCurrentThread());
     
    switch (sys_priority){
    case THREAD_PRIORITY_HIGHEST:
    case THREAD_PRIORITY_TIME_CRITICAL:
    case THREAD_PRIORITY_ABOVE_NORMAL:
    case THREAD_PRIORITY_NORMAL: return _XTHREAD_INTERACTIVE_PRIORITY;
    default:
    case THREAD_PRIORITY_BELOW_NORMAL: return _XTHREAD_BACKGROUND_PRIORITY;  
    case THREAD_PRIORITY_IDLE:
    case THREAD_PRIORITY_LOWEST: return _XTHREAD_LOW_PRIORITY;
    }
    
    /* couldn't get priority. */
    return -1;
}

/* yield our process time to another thread */
void __xthread_yield(void){
    Sleep(0);
}

/* terminate the current thread */
int __xthread_exit(void){
    /* exit the thread */
    ExitThread(_xthread_exit_status);
    /* failed if we reached here */
    return -1;
}

/* returns an integer value which uniquely describes a thread */
_xthread_t __xthread_id(void){
    return (_xthread_t)(size_t)GetCurrentThreadId();
}

/* sets the thread's local storage pointer. */
int __xthread_set_data(void *value){
    if (TlsSetValue(__xdata_tls, value))
        return 0;
    else
        return -1;
}

/* returns the thread's local storage pointer */
void *__xthread_get_data(void){
    return TlsGetValue(__xdata_tls); /* return thread data.      */
}

/* allocate a mutex */
int __xmutex_allocate(_xmutex_t mutex){
    if ((mutex->backend = (void *)CreateMutex(NULL, 0, NULL)) == NULL)
        return -1;
    else
        return 0;
}

/* deallocate a mutex */
int __xmutex_deallocate(_xmutex_t mutex){
    CloseHandle((HANDLE)(mutex->backend));
    return 0;
}

/* grab a lock on a mutex */
int __xmutex_lock(_xmutex_t mutex){
    int status;

    status = WaitForSingleObject((HANDLE)(mutex->backend), INFINITE);
    if (status != WAIT_OBJECT_0 && status != WAIT_ABANDONED)
        return -1;
    else
        return 0;
}

/* try to grab a lock on a mutex */
int __xmutex_trylock(_xmutex_t mutex){
    int status;
    
    status = WaitForSingleObject((HANDLE)(mutex->backend), 0);
    if (status != WAIT_OBJECT_0 && status != WAIT_ABANDONED)
        return -1;
    else
        return 0;
}

/* unlock the mutex */
int __xmutex_unlock(_xmutex_t mutex){
    if (ReleaseMutex((HANDLE)(mutex->backend)) == 0)
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

