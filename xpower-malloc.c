/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file define various trivial helper wrappers around 
** standard functions, useful for debug.
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#include "xpower-config.h"

/*
 * these memory routines are used to find any memory issues or memory
 * optimization. For some reasons, the PAS need allocate memory space
 * dynamically, it is important to guarantee the memory has been freed
 * correctly.
 *
 */

#ifdef _XDEBUG
static FILE *_xfp = NULL;
#define _XFP (_xfp ? _xfp : stdout)

/*!
 * set the output destination for memory debug routines 
 */
FILE *_xsetfp(FILE *fp){
    FILE *tmp = _xfp;
    _xfp = fp;
    return (tmp);
}

/*!
 * allocate memory from heap, print the address & length allocated
 */
void *_xmalloc(size_t size){
    void *ret = malloc(size);
    (void) fprintf(_XFP,"MEMFUN##### malloc : [0x%08lx] size=%d\n",(long)ret,size);

    return (ret);
}

/*!
 * re-allocate memory from heap, print the new address & length allocated,
 * print the old address if success
 */
void *_xrealloc(void *ptr, size_t size){
    void *ret = realloc(ptr,size);
    (void) fprintf(_XFP,"MEMFUN##### realloc: [0x%08lx] size=%d\n",(long)ret,size);
    if (ret && ptr)(void) fprintf(_XFP,"MEMFUN##### free   : [0x%08lx]\n",(long)ptr);

    return (ret);
}

/*!
 * allocate & clean memory from heap, print the new address & length allocated
 */
void *_xcalloc(size_t nmem, size_t size){
    void *ret = calloc(nmem,size);
    (void) fprintf(_XFP,"MEMFUN##### calloc : [0x%08lx] size=%d\n",(long)ret,nmem*size);

    return (ret);
}

/*!
 * deallocate memory to heap, print the address that has been deallocated if it 
 * is not NULL
 */
void _xfree(void *ptr){
    free(ptr);
    if (ptr)(void) fprintf(_XFP,"MEMFUN##### free   : [0x%08lx]\n",(long)ptr);
}
#endif/*_XDEBUG*/
