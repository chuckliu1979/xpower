/*
** Copyright (C) 1993 David E. Stewart & Zbigniew Leyk, all rights reserved.
**
**           Meschach Library
** 
** This Meschach Library is provided "as is" without any express 
** or implied warranty of any kind with respect to this software. 
** In particular the authors shall not be liable for any direct, 
** indirect, special, incidental or consequential damages arising 
** in any way from use of the software.
** 
** Everyone is granted permission to copy, modify and redistribute this
** Meschach Library, provided:
**  1.  All copies contain this copyright notice.
**  2.  All modified copies shall carry a notice stating who
**      made the last modification and the date of such modification.
**  3.  No charge is made for this software or works derived from it.  
**      This clause shall not be construed as constraining other software
**      distributed on the same medium as this software, nor is a
**      distribution fee considered a charge.
**
** Modified by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XPOWER_CONFIG_H_
#define _XPOWER_CONFIG_H_

#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

#include "xpower-decl.h"

#ifdef __cplusplus
extern "C" {
#endif

/* debug */
#ifdef _XDEBUG
_XEXPORT extern FILE *_xsetfp(FILE *);
_XEXPORT extern void *_xmalloc(size_t size);
_XEXPORT extern void *_xrealloc(void *, size_t);
_XEXPORT extern void *_xcalloc(size_t, size_t);
_XEXPORT extern void _xfree(void *);
#else
  #define _xmalloc  malloc
  #define _xrealloc realloc
  #define _xcalloc  calloc
  #define _xfree    free
#endif/*_XDEBUG*/

/* boolean values */
#define _XTRUE  1
#define _XFALSE 0

/* error types */
#define _XE_UNKNOWN     0
#define _XE_SIZES       1
#define _XE_BOUNDS      2
#define _XE_MEM         3
#define _XE_SING        4
#define _XE_POSDEF      5
#define _XE_FORMAT      6
#define _XE_INPUT       7
#define _XE_NULL        8
#define _XE_SQUARE      9
#define _XE_RANGE       10
#define _XE_INSITU2     11
#define _XE_INSITU      12
#define _XE_ITER        13
#define _XE_CONV        14
#define _XE_START       15
#define _XE_SIGNAL      16
#define _XE_INTERN      17
#define _XE_EOF         18
#define _XE_SHARED_VECS 19
#define _XE_NEG         20
#define _XE_OVERWRITE   21
#define _XE_BREAKDOWN   22

/* warning types */
#define _XWARN_UNKNOWN     0
#define _XWARN_WRONG_TYPE  1
#define _XWARN_NO_MARK     2
#define _XWARN_RES_LESS_0  3
#define _XWARN_SHARED_VEC  4

/* error flags */
#define _XEF_EXIT   0 /* exit on error */
#define _XEF_ABORT  1 /* abort (dump core) on error */
#define _XEF_JUMP   2 /* jump on error */
#define _XEF_SILENT 3 /* jump, but don't print message */

/* max. # of error lists */
#define _XERR_LIST_MAX_LEN 10

/* setjmp cache */
_XEXPORT extern jmp_buf _xrestart;

_XEXPORT extern int _xev_err(const char*,int,int,const char*,int);
_XEXPORT extern int _xset_err_flag(int);
_XEXPORT extern int _xcount_errs(int);
_XEXPORT extern int _xerr_list_attach(int,int,char**,int);
_XEXPORT extern int _xerr_is_list_attached(int);
_XEXPORT extern int _xerr_list_free(int);

#define _xerror(err_num,fn_name)   _xev_err(__FILE__,err_num,__LINE__,fn_name,0)
#define _xwarning(err_num,fn_name) _xev_err(__FILE__,err_num,__LINE__,fn_name,1)

#define _xerrflag_exit()   _xset_err_flag(_XEF_EXIT)
#define _xerrflag_abort()  _xset_err_flag(_XEF_ABORT)
#define _xerrflag_jump()   if (! setjmp(_xrestart)) _xset_err_flag(_XEF_JUMP)
#define _xerrflag_silent() if (! setjmp(_xrestart)) _xset_err_flag(_XEF_SILENT)

extern int isatty(int);
/*
#ifndef _MSC_VER
#define _fileno fileno
#endif*/

/* standard copy & zero functions */
#ifndef _XBCOPY
  #define _xmem_copy(from,to,size) memmove((to),(from),(size))
  #define _xmem_zero(where,size)   memset((where),'\0',(size))
#else
  #define _xmem_copy(from,to,size) bcopy((char *)(to),(char *)(from),(int)(size))
  #define _xmem_zero(where,size)   bzero((char *)(where),(int)(size))
#endif

/* error catching macros */
/* execute err_part if error errnum is raised while executing ok_part */
#define _xcatchone(errnum,ok_part,err_part)              \
    {                                                    \
        jmp_buf _save; int _err_num, _old_flag;          \
        _old_flag = _xset_err_flag(_XEF_SILENT);         \
        _xmem_copy(_xrestart,_save,sizeof(jmp_buf));     \
        if ((_err_num=setjmp(_xrestart)) == 0){          \
            ok_part;                                     \
            _xset_err_flag(_old_flag);                   \
            _xmem_copy(_save,_xrestart,sizeof(jmp_buf)); \
        }                                                \
        else if (_err_num == errnum){                    \
            _xset_err_flag(_old_flag);                   \
            _xmem_copy(_save,_xrestart,sizeof(jmp_buf)); \
            err_part;                                    \
        }                                                \
        else {                                           \
            _xset_err_flag(_old_flag);                   \
            _xmem_copy(_save,_xrestart,sizeof(jmp_buf)); \
            _xerror(_err_num,"_xcatchone");              \
        }                                                \
    } 

/* execute err_part if any error raised while executing ok_part */
#define _xcatchall(ok_part,err_part)                     \
    {                                                    \
        jmp_buf _save; int _err_num, _old_flag;          \
        _old_flag = _xset_err_flag(_XEF_SILENT);         \
        _xmem_copy(_xrestart,_save,sizeof(jmp_buf));     \
        if ((_err_num=setjmp(_xrestart)) == 0){          \
            ok_part;                                     \
            _xset_err_flag(_old_flag);                   \
            _xmem_copy(_save,_xrestart,sizeof(jmp_buf)); \
        }                                                \
        else {                                           \
            _xset_err_flag(_old_flag);                   \
            _xmem_copy(_save,_xrestart,sizeof(jmp_buf)); \
            err_part;                                    \
        }                                                \
    }

/* print message if error raised while executing ok_part,
   then re-raise error to trace calls 
 */
#define _xtracecatch(ok_part,function)                    \
    {                                                     \
        jmp_buf _save; int _err_num, _old_flag;           \
        _old_flag = _xset_err_flag(_XEF_JUMP);            \
        _xmem_copy(_xrestart,_save,sizeof(jmp_buf));      \
        if ((_err_num=setjmp(_xrestart)) == 0){           \
            ok_part;                                      \
            _xset_err_flag(_old_flag);                    \
            _xmem_copy(_save,_xrestart,sizeof(jmp_buf));} \
        else {                                            \
            _xset_err_flag(_old_flag);                    \
            _xmem_copy(_save,_xrestart,sizeof(jmp_buf));  \
            _xerror(_err_num,function);                   \
        }                                                 \
    }

/* floating point precision */
#define _XFLOAT       1
#define _XDOUBLE      2
#define _XLONG_DOUBLE 3
#define _XREAL_DBL
/* if nothing is defined, choose single precision */
#ifndef _XREAL_FLT
  #ifndef _XREAL_DBL
    #define _XREAL_FLT 1
  #endif
#endif

/* single precision */
#ifdef _XREAL_FLT
  #define _xreal float
  #define _xlongreal float
  #define _XREAL _XFLOAT
  #define _XLONGREAL _XFLOAT
#endif

/* double precision */
#ifdef _XREAL_DBL
  #define _xreal double
  #define _xlongreal double
  #define _XREAL _XDOUBLE
  #define _XLONGREAL _XDOUBLE
#endif

/* machine epsilon or unit roundoff error */
/* This is correct on most IEEE Real precision systems */
#ifdef DBL_EPSILON
  #if _XREAL == _XDOUBLE
    #define _XMACHEPS DBL_EPSILON
  #elif _XREAL == _XFLOAT
    #define _XMACHEPS FLT_EPSILON
  #elif _XREAL == _XLONGDOUBLE
    #define _XMACHEPS LDBL_EPSILON
  #endif
#endif

#define _XF_MACHEPS 1.19209e-07
#define _XD_MACHEPS 2.22045e-16

#ifndef _XMACHEPS
  #if _XREAL == _XDOUBLE
    #define _XMACHEPS _XD_MACHEPS
  #elif _XREAL == _XFLOAT  
    #define _XMACHEPS _XF_MACHEPS
  #elif _XREAL == _XLONGDOUBLE
    #define _XMACHEPS _XD_MACHEPS
  #endif
#endif

#define _XMAXLINE  81
#define _XMAX_INT  2147483647
#define _XMAX_RAND ((double)(_XMAX_INT))

#ifndef _XHUGE_VAL
#define _XHUGE_VAL HUGE
#endif

/* for hash table Note: the hash size should be a prime, or at very least odd */
#define _XMEM_HASHSIZE      509
#define _XMEM_HASHSIZE_FILE "xpower-config.h"

/* default: memory information is off, set it to 1 if you want it all the time */
#define _XMEM_SWITCH_ON_DEF 0

/* available standard types */
#define _XTYPE_NULL     (-1)
#define _XTYPE_MAT      0
#define _XTYPE_BAND     1
#define _XTYPE_PERM     2
#define _XTYPE_VEC      3
#define _XTYPE_IVEC     4
#define _XTYPE_ITER     5
#define _XTYPE_SPROW    6
#define _XTYPE_SPMAT    7
#define _XTYPE_ZVEC     8
#define _XTYPE_ZMAT     9

/* structure for memory information */
typedef struct __xmem_array {
    int bytes;       /* # of allocated bytes for each type (summary) */
    int numvar;      /* # of allocated variables for each type */
} _xmem_array;

/* internal type */
typedef struct __xmem_connect {
    char **type_names;       /* array of names of types (strings) */
    int (**free_funcs)();    /* array of functions for releasing types */
    unsigned int ntypes;     /* max number of types */
    _xmem_array *info_sum;   /* local array for keeping track of memory */
} _xmem_connect;

/* max number of lists of types */
#define _XMEM_CONNECT_MAX_LISTS 5

_XEXPORT extern int _xmem_info_is_on(void);
_XEXPORT extern int _xmem_info_on(int);
_XEXPORT extern int _xmem_info_bytes(int,int);
_XEXPORT extern int _xmem_info_numvar(int,int);
_XEXPORT extern void _xmem_info_file(FILE*,int);
_XEXPORT extern void _xmem_bytes_list(int,int,int,int);
_XEXPORT extern void _xmem_numvar_list(int,int,int);

/* memory macros */
#define _xmem_info()                        _xmem_info_file(stdout,0)
#define _xmem_bytes(type,old_size,new_size) _xmem_bytes_list(type,old_size,new_size,0)
#define _xmem_numvar(type,num)              _xmem_numvar_list(type,num,0)
#define _xnew(type)                         ((type *)_xcalloc((size_t)1,(size_t)sizeof(type)))
#define _xnew_a(num,type)                   ((type *)_xcalloc((size_t)(num),(size_t)sizeof(type)))
#define _xmemcopy(from,to,n_items,type)     _xmem_copy((char *)(from),(char *)(to),(unsigned)(n_items)*sizeof(type))
#define _xrenew(var,num,type)               ((var) = (type *)((var) ?                               \
                                            _xrealloc((char *)(var),(size_t)((num)*sizeof(type))) : \
                                            _xcalloc((size_t)(num),(size_t)sizeof(type))))

#define _xmax(a,b) ((a) > (b) ? (a) : (b))
#define _xmin(a,b) ((a) > (b) ? (b) : (a))

/* miscellaneous functions */
_XEXPORT extern double _xmrand();
_XEXPORT extern void _xsmrand(int);
_XEXPORT extern void _xmrandlist(_xreal *, int);
_XEXPORT extern int _xskipjunk(FILE *);

#define _xcube(x)   ((double)(x)*(x)*(x))
#define _xsquare(x) ((double)(x)*(x))

/* other I/O */
_XEXPORT extern int _xyn_dflt(int);
_XEXPORT extern int _xfy_or_n(FILE *, const char *);
_XEXPORT extern int _xfin_int(FILE *, const char *, int, int);
_XEXPORT extern double _xfin_double(FILE *, const char *, double, double);

/* general purpose input routine; skips comments # ... \n */
#define _xfinput(fp,prompt,fmt,var) ((isatty(_fileno(fp)) ? fprintf(stderr,prompt) : \
                                                            _xskipjunk(fp)), fscanf(fp,fmt,var))
#define _xinput(prompt,fmt,var)     _xfinput(stdin,prompt,fmt,var)
#define _xfprompter(fp,prompt)      (isatty(_fileno(fp)) ? fprintf(stderr,prompt) : _xskipjunk(fp))
#define _xprompter(prompt)          _xfprompter(stdin,prompt)
#define _xy_or_n(s)                 _xfy_or_n(stdin,s)
#define _xin_int(s,lo,hi)           _xfin_int(stdin,s,lo,hi)
#define _xin_double(s,lo,hi)        _xfin_double(stdin,s,lo,hi)

#define _XTHREAD_SAFE 1 /* thread safe */
#ifdef _XTHREAD_SAFE
  #include "xpower-thread.h"
  #define _XSTATIC
  #define _xmem_stat_free(mark)
  #define _xmem_stat_reg(var,type)
  #define _XMEM_STAT_REG(var,type)
#else
_XEXPORT extern int _xmem_stat_reg_list(void**,int,int,char*,int);
_XEXPORT extern int _xmem_stat_mark(int);
_XEXPORT extern int _xmem_stat_free_list(int,int);
_XEXPORT extern int _xmem_stat_show_mark(void);
_XEXPORT extern int _xmem_free_vars(int);
_XEXPORT extern int _xmem_is_list_attached(int);
_XEXPORT extern int _xmem_stat_reg_vars(int,int,char*,int,...);
_XEXPORT extern int _xmem_attach_list(int,int,char**,int(**)(void*),_xmem_array*);
_XEXPORT extern void _xmem_stat_dump(FILE*,int);
_XEXPORT extern void _xmem_dump_list(FILE*fp,int);
  #define _XSTATIC static
  #define _xmem_stat_free(mark)    _xmem_stat_free_list(mark,0)
  #define _xmem_stat_reg(var,type) _xmem_stat_reg_list((void **)var,type,0,__FILE__,__LINE__)
  #define _XMEM_STAT_REG(var,type) _xmem_stat_reg_list((void **)&(var),type,0,__FILE__,__LINE__)
  #define _xthread_init()
  #define _xmutex_lock(x) 
  #define _xmutex_unlock(x) 
  #define _xmutex_trylock(x)
#endif/*_XTHREADSAFE*/

#ifndef _XFLEX_ARRAY
/* see if our compiler is known to support flexible array members */
  #if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L) && (!defined(__SUNPRO_C) || (__SUNPRO_C > 0x580))
    #define _XFLEX_ARRAY /* empty */
    #define _XFLEX_ARRAY_SIZE 0
  #elif defined(_MSC_VER)
    #define _XFLEX_ARRAY /* empty */
    #define _XFLEX_ARRAY_SIZE 0
  #elif defined(__GNUC__)
    #if (__GNUC__ >= 3)
      #define _XFLEX_ARRAY /* empty */
      #define _XFLEX_ARRAY_SIZE 0
    #else
      #define _XFLEX_ARRAY 0 /* older GNU extension */
      #define _XFLEX_ARRAY_SIZE 0
    #endif
  #endif
#endif/*_XFLEX_ARRAY*/
/* otherwise, default to safer but a bit wasteful traditional style */
#ifndef _XFLEX_ARRAY
  #define _XFLEX_ARRAY 1
  #define _XFLEX_ARRAY_SIZE 1
#endif

#ifdef _MSC_VER
  #define inline __inline
  #define __inline__ __inline
  #define __attribute__(x)
#endif

#ifdef __GNUC__
  #define _XTYPEOF(x) (__typeof__(x))
  #define _XNORETURN __attribute__((__noreturn__))
  #define _XNORETURN_PTR __attribute__((__noreturn__))
#elif defined(_MSC_VER)
  #define _XTYPEOF(x) 
  #define _XNORETURN __declspec(noreturn)
  #define _XNORETURN_PTR
#else
  #define _XTYPEOF(x)
  #define _XNORETURN
  #define _XNORETURN_PTR
  #ifndef __attribute__
    #define __attribute__(x)
  #endif
#endif

#ifndef offsetof
#define offsetof(s,m) ((size_t)&((s *)0)->m)
#endif

/*! cast a member of a structure out to the containing structure */
#ifndef _xcontainer_of
  #ifdef __GNUC__
    #define _xcontainer_of(ptr,type,member) ({                                                  \
                                             const typeof(((type *)0)->member) *__mptr = (ptr); \
                                             (type *)((char *)__mptr - offsetof(type,member));})
  #else
    #define _xcontainer_of(ptr,type,member) (type *)((char *)ptr - offsetof(type,member))
  #endif
#endif

#ifdef __cplusplus
}
#endif

#endif/*_XPOWER_CONFIG_H_*/
