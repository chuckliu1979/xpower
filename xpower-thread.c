/*
** Copyright (C) 2011 XXX all rights reserved.
**
** Copy thread and mutex controls from Objective C. 
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#include <stdlib.h>
#include "xpower-thread.h"

int _xthread_exit_status = 0;       /* global exit status */
int _xthread_runtime_threads_alive; /* number of threads which are alive */
int _xthread_is_multi_threaded = 0; /* flag which lets us know if we ever became multi threaded */

_xmutex_t _xthread_runtime_mutex = NULL;

/* the hook function called when the runtime becomes multi threaded */
_xthread_callback _xthread_became_multi_threaded = NULL;

_xthread_callback _xthread_set_callback(_xthread_callback func){
    _xthread_callback temp = _xthread_became_multi_threaded;
    _xthread_became_multi_threaded = func;
    return temp;
}

/*!
 * set the current thread's priority 
 */
int _xthread_set_priority(int priority){
    /* call the backend */
    return __xthread_set_priority(priority);
}

/*!
 * return the current thread's priority
 */
int _xthread_get_priority(void){
    /* call the backend */
    return __xthread_get_priority();
}

/*!
 * yield our process time to another thread. Any BUSY waiting that is done
 * by a thread should use this function to make sure that other threads can
 * make progress even on a lazy uniprocessor system.
 */
void _xthread_yield(void){
    /* call the backend */
    __xthread_yield();
}

/*!
 * terminate the current tread. doesn't return.
 * actually, if it failed returns -1.
 */
int _xthread_exit(void){
    /* decrement our counter of the number of threads alive */
    _xmutex_lock(_xthread_runtime_mutex);
    _xthread_runtime_threads_alive--;
    _xmutex_unlock(_xthread_runtime_mutex);
    
    /* call the backend to terminate the thread */
    return __xthread_exit();
}

/*!
 * returns an integer value which uniquely describes a thread. must not be
 * NULL which is reserved as a marker for "no thread".
 */
_xthread_t _xthread_id(void){
    /* call the backend */
    return __xthread_id();
}

/*!
 * sets the thread's local storage pointer.
 * returns 0 if successful or -1 if failed.
 */
int _xthread_set_data(void *value){
    /* call the backend */
    return __xthread_set_data(value);
}

/*!
 * returns the thread's local storage pointer or NULL on failure.
 */
void *_xthread_get_data(void){
    /* call the backend */
    return __xthread_get_data();
}

/*!
 * allocate a mutex. return the mutex pointer if successful or NULL if the
 * allocation failed for any reason.
 */
_xmutex_t _xmutex_allocate(void){
    _xmutex_t mutex;
    /* allocate the mutex structure */
    if (!(mutex = (_xmutex_t)malloc(sizeof(struct _xmutex))))
        return NULL;
    /* call backend to create the mutex */
    if (__xmutex_allocate(mutex)){
        /* failed !*/
        free(mutex);
        return NULL;
    }
    /* initialize mutex */
    mutex->owner = NULL;
    mutex->depth = 0;
    return mutex;
}

/*!
 * deallocate a mutex. Note that this includes an implicit mutex_lock to
 * insure that no one else is using the lock.  It is legal to deallocate
 * a lock if we have a lock on it, but illegal to deallocate a lock held
 * by anyone else.
 * returns the number of locks on the thread. (1 for deallocate).
 */
int _xmutex_deallocate(_xmutex_t mutex){
    int depth;
    /* valid mutex? */
    if (!mutex)
        return -1;
    /* acquire lock on mutex */
    depth = _xmutex_lock(mutex);
    /* call backend to destroy mutex */
    if (__xmutex_deallocate(mutex))
        return -1;
    free(mutex);
    return depth;
}

/*!
 * grab a lock on a mutex.if this thread already has a lock on this mutex
 * then we increment the lock count.  If another thread has a lock on the
 * mutex we block and wait for the thread to release the lock.
 * returns the lock count on the mutex held by this thread.
 */
int _xmutex_lock(_xmutex_t mutex){
    _xthread_t thread_id;
    int status;
    
    /* valid mutex? */
    if (!mutex)
        return -1;
    /* if we already own the lock then increment depth */
    thread_id = __xthread_id();
    if (mutex->owner == thread_id)
        return ++mutex->depth;
    /* call the backend to lock the mutex */
    status = __xmutex_lock(mutex);
    /* failed? */
    if (status)
        return status;
    /* successfully locked the thread */
    mutex->owner = thread_id;
    return mutex->depth = 1;
}

/*!
 * try to grab a lock on a mutex.if this thread already has a lock on
 * this mutex then we increment the lock count and return it.if another
 * thread has a lock on the mutex returns -1.
 */
int _xmutex_trylock(_xmutex_t mutex){
    _xthread_t thread_id;
    int status;
    
    /* valid mutex? */
    if (!mutex)
        return -1;
    /* if we already own the lock then increment depth */
    thread_id = __xthread_id();
    if (mutex->owner == thread_id)
        return ++mutex->depth;
    /* call the backend to lock the mutex */
    status = __xmutex_trylock(mutex);
    /* failed? */
    if (status)
        return status;
    /* successfully locked the thread */
    mutex->owner = thread_id;
    return mutex->depth = 1;
}

/*!
 * unlocks the mutex by one level.
 * decrements the lock count on this mutex by one.
 * if the lock count reaches zero, release the lock on the mutex.
 * returns the lock count on the mutex.
 * it is an error to attempt to unlock a mutex which this thread
 * doesn't hold in which case return -1 and the mutex is unaffected.
 */
int _xmutex_unlock(_xmutex_t mutex){
    _xthread_t thread_id;
    int status;
    
    /* valid mutex? */
    if (!mutex)
        return -1;
    
    /* if another thread owns the lock then abort */
    thread_id = __xthread_id ();
    if (mutex->owner != thread_id)
        return -1;
    
    /* decrement depth and return */
    if (mutex->depth > 1)
        return --mutex->depth;
    
    /* depth down to zero so we are no longer the owner */
    mutex->depth = 0;
    mutex->owner = NULL;
    
    /* have the backend unlock the mutex */
    status = __xmutex_unlock (mutex);
    
    /* failed? */
    if (status)
        return status;
    
    return 0;
}

/*!
 * allocate a condition.return the condition pointer if successful or NULL
 * if the allocation failed for any reason.
 */
_xcondition_t _xcondition_allocate(void){
    _xcondition_t condition;
       
    /* allocate the condition mutex structure */
    if (!(condition = (_xcondition_t)malloc(sizeof(struct _xcondition))))
        return NULL;
    
    /* call the backend to create the condition mutex */
    if (__xcondition_allocate (condition)){
        /* failed! */
        free(condition);
        return NULL;
    }
    
    /* success! */
    return condition;
}

/*!
 * deallocate a condition. Note that this includes an implicit
 * condition_broadcast to insure that waiting threads have the opportunity
 * to wake.it is legal to dealloc a condition only if no other
 * thread is/will be using it.here we do NOT check for other threads
 * waiting but just wake them up.
 */
int _xcondition_deallocate(_xcondition_t condition){
    /* broadcast the condition */
    if (_xcondition_broadcast(condition))
        return -1;
    
    /* call the backend to destroy */
    if (__xcondition_deallocate(condition))
        return -1;
    
    /* free the condition mutex structure */
    free(condition);
    
    return 0;
}

/*!
 * wait on the condition unlocking the mutex until _xcondition_signal()
 * or _xcondition_broadcast() are called for the same condition. the
 * given mutex *must* have the depth set to 1 so that it can be unlocked
 * here, so that someone else can lock it and signal/broadcast the condition.
 * the mutex is used to lock access to the shared data that make up the
 * "condition" predicate.
 */
int _xcondition_wait(_xcondition_t condition, _xmutex_t mutex){
    _xthread_t thread_id;
    
    /* valid arguments? */
    if (!mutex || !condition)
        return -1;
    
    /* make sure we are owner of mutex */
    thread_id = __xthread_id();
    if (mutex->owner != thread_id)
        return -1;
    
    /* cannot be locked more than once */
    if (mutex->depth > 1)
        return -1;
    
    /* virtually unlock the mutex */
    mutex->depth = 0;
    mutex->owner = (_xthread_t)NULL;
    
    /* call the backend to wait */
    __xcondition_wait(condition, mutex);
    
    /* make ourselves owner of the mutex */
    mutex->owner = thread_id;
    mutex->depth = 1;
    
    return 0;
}

/*!
 * wake up all threads waiting on this condition.it is recommended that
 * the called would lock the same mutex as the threads in _xcondition_wait
 * before changing the "condition predicate" and make this call and unlock it
 * right away after this call.
 */
int _xcondition_signal(_xcondition_t condition){
    /* valid condition mutex? */
    if (!condition)
        return -1;
    
    return __xcondition_broadcast(condition);
}

/*!
 * wake up one thread waiting on this condition.it is recommended that
 * the called would lock the same mutex as the threads in _xcondition_wait
 * before changing the "condition predicate" and make this call and unlock it
 * right away after this call.
 */
int _xcondition_broadcast(_xcondition_t condition){
    /* valid condition mutex? */
    if (!condition)
        return -1;
    
    return __xcondition_signal(condition);
}

/*!
 * make the x thread system aware that a thread which is managed
 * (started, stopped) by external code could access x facilities
 * from now on.this is used when you are interfacing with some
 * external non-x-based environment/system - you must call
 * _xthread_add() before an alien thread makes any calls to
 * x.do not cause the _xthread_became_multi_threaded hook to
 * be executed. 
 */
void _xthread_add(void){
    _xmutex_lock(_xthread_runtime_mutex);
    _xthread_is_multi_threaded = 1;
    _xthread_runtime_threads_alive++;
    _xmutex_unlock(_xthread_runtime_mutex);
}

/*!
 * make the x thread system aware that a thread managed (started,
 * stopped) by some external code will no longer access x and thus
 * can be forgotten by the x thread system.call _xthread_remove()
 * when your alien thread is done with making calls to x.
 */
void _xthread_remove(void){
    _xmutex_lock(_xthread_runtime_mutex);
    _xthread_runtime_threads_alive--;
    _xmutex_unlock(_xthread_runtime_mutex);
}

/*!
 * initializes the thread system global data 
 */
void _xthread_init(void){
    /* have we processed any initialize previously? this flag is used to
       indicate that some global data structures need to be built
     */
    static int _xprevious_initialized = 0;
    /* on the first call of this routine, initialize some data structures */
    if (!_xprevious_initialized){
        /* initialize thread-safe system */
        __xinit_thread_system();
        _xthread_runtime_threads_alive = 1;
        _xthread_runtime_mutex = _xmutex_allocate();
        _xprevious_initialized = 1;
    }
}

