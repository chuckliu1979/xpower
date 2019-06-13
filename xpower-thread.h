/*
** Copyright (C) 2011 XXX all rights reserved.
**
** Copy thread and mutex controls from Objective C. 
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XPOWER_THREAD_H_
#define _XPOWER_THREAD_H_

#ifdef __cplusplus
extern "C" {
#endif /** __cplusplus */

extern int _xthread_exit_status; /* global exit status */

/* thread priorities */
#define _XTHREAD_INTERACTIVE_PRIORITY 2
#define _XTHREAD_BACKGROUND_PRIORITY  1
#define _XTHREAD_LOW_PRIORITY         0

/* a thread */
typedef void *_xthread_t;

/* this structure represents a single mutual exclusion lock */
struct _xmutex {
    volatile _xthread_t owner;/* id of thread that owns */
    volatile int depth;       /* # of acquires */
    void *backend;            /* specific to backend */
};
typedef struct _xmutex *_xmutex_t;
extern _xmutex_t _xthread_runtime_mutex;

/* this structure represents a single condition mutex */
struct _xcondition {
    void *backend;  /* specific to backend */
};
typedef struct _xcondition *_xcondition_t;

/* frontend mutex functions */
_xmutex_t _xmutex_allocate(void);
int _xmutex_deallocate(_xmutex_t mutex);
int _xmutex_lock(_xmutex_t mutex);
int _xmutex_unlock(_xmutex_t mutex);
int _xmutex_trylock(_xmutex_t mutex);

/* frontend condition mutex functions */
_xcondition_t _xcondition_allocate(void);
int _xcondition_deallocate(_xcondition_t condition);
int _xcondition_wait(_xcondition_t condition, _xmutex_t mutex);
int _xcondition_signal(_xcondition_t condition);
int _xcondition_broadcast(_xcondition_t condition);

/* frontend thread functions */
_xthread_t _xthread_id(void);
int _xthread_exit(void);
int _xthread_set_priority(int priority);
int _xthread_get_priority(void);
int _xthread_set_data(void *value);
void _xthread_add(void);
void _xthread_remove(void);
void _xthread_yield(void);
void *_xthread_get_data(void);
void _xthread_init(void);

/*! 
 * use this to set the hook function that will be called when the
 * runtime initially becomes multi threaded.
 * the hook function is only called once, meaning only when the 2nd
 * thread is spawned, not for each and every thread.
 *
 * it returns the previous hook function or NULL if there is none.
 * a program outside of the runtime could set this to some function
 * so it can be informed; for example, the GNUstep Base Library sets
 * it so it can implement the NSBecommingMultiThreaded notification.
 *
 */
typedef void (*_xthread_callback)(void);
_xthread_callback _xthread_set_callback(_xthread_callback func);

/* backend initialization functions */
int __xinit_thread_system(void);
int __xclose_thread_system(void);

/* backend mutex functions */
int __xmutex_allocate(_xmutex_t mutex);
int __xmutex_deallocate(_xmutex_t mutex);
int __xmutex_lock(_xmutex_t mutex);
int __xmutex_trylock(_xmutex_t mutex);
int __xmutex_unlock(_xmutex_t mutex);

/* backend condition mutex functions */
int __xcondition_allocate(_xcondition_t condition);
int __xcondition_deallocate(_xcondition_t condition);
int __xcondition_wait(_xcondition_t condition, _xmutex_t mutex);
int __xcondition_broadcast(_xcondition_t condition);
int __xcondition_signal(_xcondition_t condition);

/* backend thread functions */
_xthread_t __xthread_detach(void (*func)(void *arg), void *arg);
_xthread_t __xthread_id(void);
int __xthread_set_priority(int priority);
int __xthread_get_priority(void);
int __xthread_exit(void);
int __xthread_set_data(void *value);
void __xthread_yield(void);
void *__xthread_get_data(void);

#ifdef __cplusplus
}
#endif /** __cplusplus */

#endif/*_XPOWER_THREAD_H_*/
