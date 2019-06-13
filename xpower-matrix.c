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

#include <math.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <signal.h>
#include "xpower-matrix.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#pragma warning(disable:4996 4244 4018)
#endif

const int _x_version = 0x01020b00;
const char *_xversion(){
    return ("Based on Meshach matrix library version 1.2b:\n"
            "Meshach matrix library version 1.2b\n"
            "Changes since 1.2a:\n"
            "\t Fixed bug in _xmat_schur() for 2x2 blocks with real e-vals\n"
            "\t Fixed bug in _xmat_schur() reading beyond end of array\n"
            "\t Fixed some installation bugs\n"
            "\t Fixed bugs & improved efficiency in _xspmat_ilufactor()\n"
            "\t _xperm_inv() doesn't crash inverting non-permutations\n"
            "\t Fixed bug in _xvec_ifft()\n");
}

static char *_xerr_mesg[] = {   
    "unknown error",                                      /* 0 */
    "sizes of objects don't match",                       /* 1 */
    "index out of bounds",                                /* 2 */
    "can't allocate memory",                              /* 3 */
    "singular matrix",                                    /* 4 */
    "matrix not positive definite",                       /* 5 */
    "incorrect format input",                             /* 6 */
    "bad input file/device",                              /* 7 */
    "null objects passed",                                /* 8 */
    "matrix not square",                                  /* 9 */
    "object out of range",                                /* 10 */
    "can't do operation in situ for non-square matrix",   /* 11 */
    "can't do operation in situ",                         /* 12 */
    "excessive number of iterations",                     /* 13 */
    "convergence criterion failed",                       /* 14 */
    "bad starting value",                                 /* 15 */
    "floating exception",                                 /* 16 */
    "internal inconsistency (data structure)",            /* 17 */
    "unexpected end-of-file",                             /* 18 */
    "shared vectors (cannot release them)",               /* 19 */  
    "negative argument",                                  /* 20 */
    "cannot overwrite object",                            /* 21 */
    "breakdown in iterative method"                       /* 22 */
};
#define _XMAXERR (sizeof(_xerr_mesg)/sizeof(char *))

static char *_xwarn_mesg[] = {
    "unknown warning",                             /* 0 */
    "wrong type number (use macro _XTYPE_*)",      /* 1 */
    "no corresponding mem_stat_mark",              /* 2 */
    "computed norm of a residual is less than 0",  /* 3 */
    "resizing a shared vector"                     /* 4 */
};
#define _XMAXWARN (sizeof(_xwarn_mesg)/sizeof(char *))

#define _XMAX_ERRS 100
#define _XE_SIGNAL 16

/* other local variables */
static int _xerr_flag = _XEF_EXIT, _xnum_errs = 0, _xcnt_errs = 1;

/* array of pointers to lists of errors */
typedef struct __xerr_list {
    char **listp;    /* pointer to a list of errors */
    unsigned len;    /* length of the list */
    unsigned warn;   /* =_XFALSE - errors, =_XTRUE - warnings */
} _xerr_list;

static _xerr_list _xerr_lists[_XERR_LIST_MAX_LEN] = {
    { _xerr_mesg, _XMAXERR, _XFALSE},   /* basic errors list */
    { _xwarn_mesg,_XMAXWARN,_XTRUE}     /* basic warnings list */
};

/* number of elements in err_list */
static int _xerr_lists_end = 2;   

jmp_buf _xrestart;

int _xev_err(const char *file, int err_num, int line_num, const char *fn_name, int list_num){
    int num;

    if (err_num < 0) err_num = 0;
    if (list_num < 0 || list_num >= _xerr_lists_end || _xerr_lists[list_num].listp == (char **)NULL){
        (void) fprintf(stderr,"\nNot (properly) attached list of errors: list_num = %d\n",list_num);
        (void) fprintf(stderr," Call \"_xerr_list_attach\" in your program\n");
        if (! isatty(_fileno(stdout))){
            (void) fprintf(stderr,"\nNot (properly) attached list of errors: list_num = %d\n",list_num);
            (void) fprintf(stderr,"Call \"_xerr_list_attach\" in your program\n");
        }
        (void) fprintf(stdout, "\nExiting program\n");
        exit(0);
    }
   
    num = err_num;
    if ((unsigned int)num >= _xerr_lists[list_num].len) num = 0;
    if (_xcnt_errs && ++_xnum_errs >= _XMAX_ERRS){/* too many errors */
        (void) fprintf(stderr,"\n\"%s\", line %d: %s in function %s()\n",
                       file,line_num,_xerr_lists[list_num].listp[num],
                       isascii(*fn_name) ? fn_name : "???");
        if (!isatty(_fileno(stdout)))
            (void) fprintf(stdout,"\n\"%s\", line %d: %s in function %s()\n",
                           file,line_num,_xerr_lists[list_num].listp[num],
                           isascii(*fn_name) ? fn_name : "???");
        (void) fprintf(stdout, "Sorry, too many errors: %d\n",_xnum_errs);
        (void) fprintf(stdout, "Exiting program\n");
        exit(0);
    }
    if (_xerr_lists[list_num].warn)
        switch (_xerr_flag){
        case _XEF_SILENT: break;
        default:
            (void) fprintf(stderr,"\n\"%s\", line %d: %s in function %s()\n\n",
                           file,line_num,_xerr_lists[list_num].listp[num],
                           isascii(*fn_name) ? fn_name : "???");
            if (!isatty(_fileno(stdout)))
                (void) fprintf(stdout,"\n\"%s\", line %d: %s in function %s()\n\n",
                               file,line_num,_xerr_lists[list_num].listp[num],
                               isascii(*fn_name) ? fn_name : "???");
        break;
        }
    else
        switch (_xerr_flag){
        case _XEF_SILENT:
            longjmp(_xrestart,(err_num==0)? -1 : err_num);
            break;
        case _XEF_ABORT:
            (void) fprintf(stderr,"\n\"%s\", line %d: %s in function %s()\n",
                           file,line_num,_xerr_lists[list_num].listp[num],
                           isascii(*fn_name) ? fn_name : "???");
            if (!isatty(_fileno(stdout)))
                (void) fprintf(stdout,"\n\"%s\", line %d: %s in function %s()\n",
                               file,line_num,_xerr_lists[list_num].listp[num],
                               isascii(*fn_name) ? fn_name : "???");
            abort();
            break;
        case _XEF_JUMP:
            (void) fprintf(stderr,"\n\"%s\", line %d: %s in function %s()\n",
                           file,line_num,_xerr_lists[list_num].listp[num],
                           isascii(*fn_name) ? fn_name : "???");
            if (!isatty(_fileno(stdout)))
                (void) fprintf(stdout,"\n\"%s\", line %d: %s in function %s()\n",
                               file,line_num,_xerr_lists[list_num].listp[num],
                               isascii(*fn_name) ? fn_name : "???");
            longjmp(_xrestart,(err_num==0)? -1 : err_num);
            break;
        default:
            (void) fprintf(stderr,"\n\"%s\", line %d: %s in function %s()\n\n",
                           file,line_num,_xerr_lists[list_num].listp[num],
                           isascii(*fn_name) ? fn_name : "???");
            if (!isatty(_fileno(stdout)))
                (void) fprintf(stdout,"\n\"%s\", line %d: %s in function %s()\n\n",
                               file,line_num,_xerr_lists[list_num].listp[num],
                               isascii(*fn_name) ? fn_name : "???");
        
            break;
        }
   
    /* ensure exit if fall through */
    if (!_xerr_lists[list_num].warn) exit(0);

    return (0);
}

int _xset_err_flag(int flag){
    return flag ^= _xerr_flag ^= flag ^= _xerr_flag;
}

int _xcount_errs(int cnt){
    return cnt ^= _xcnt_errs ^= cnt ^= _xcnt_errs;
}

/*
 * attach a new list of errors pointed by err_ptr or change a previous one;
 * list_len is the number of elements in the list;
 * list_num is the list number;
 * warn == _FALSE - errors (stop the program),
 * warn == _TRUE - warnings (continue the program);
 * Note: lists numbered 0 and 1 are attached automatically,
 * you do not need to do it
 */
int _xerr_list_attach(int list_num, int list_len, char **err_ptr, int warn){
    if (list_num < 0 || list_len <= 0 || err_ptr == (char **)NULL) 
        return -1;
   
    if (list_num >= _XERR_LIST_MAX_LEN){
        (void) fprintf(stderr,"\n\"xmatrix.c\": increase the value of "
                       "_XERR_LIST_MAX_LEN in xconfig.h\n");

        if (!isatty(_fileno(stdout)))
            (void) fprintf(stderr,"\n\"xmatrix.c\": increase the value of "
                           "_XERR_LIST_MAX_LEN in xconfig.h\n");
        (void) fprintf(stdout, "Exiting program\n");
        exit(0);
    }

    if (_xerr_lists[list_num].listp != (char **)NULL && _xerr_lists[list_num].listp != err_ptr)
        _xfree(_xerr_lists[list_num].listp);
    _xerr_lists[list_num].listp = err_ptr;
    _xerr_lists[list_num].len = list_len;
    _xerr_lists[list_num].warn = warn;
    _xerr_lists_end = list_num + 1;
   
    return (list_num);
}

/* check if list_num is attached */
int _xerr_is_list_attached(int list_num){
    if (list_num < 0 || list_num >= _xerr_lists_end)
        return _XFALSE;
    return ((_xerr_lists[list_num].listp != (char **)NULL) ? _XTRUE : _XFALSE);
}

/* release the error list numbered list_num */
int _xerr_list_free(int list_num){
    if (list_num < 0 || list_num >= _xerr_lists_end) return -1;
    if (_xerr_lists[list_num].listp != (char **)NULL){
        _xerr_lists[list_num].listp = (char **)NULL;
        _xerr_lists[list_num].len = 0;
        _xerr_lists[list_num].warn = 0;
    }
    return (0);
}

/* names of types */
static char *_xmem_type_names[] = {
    "_xmat",      /* 0 - _XTYPE_MAT   */
    "_xband",     /* 1 - _XTYPE_BAND  */
    "_xperm",     /* 2 - _XTYPE_PERM  */
    "_xvec",      /* 3 - _XTYPE_VEC   */
    "_xivec",     /* 4 - _XTYPE_IVEC  */
    "_xiter",     /* 5 - _XTYPE_ITER  */
    "_xsprow",    /* 6 - _XTYPE_SPROW */
    "_xspmat",    /* 7 - _XTYPE_SPMAT */
    "_xzvec",     /* 8 - _XTYPE_ZVEC  */
    "_xzmat"      /* 9 - _XTYPE_ZMAT  */
};
#define _XMEM_NUM_STD_TYPES (sizeof(_xmem_type_names)/sizeof(_xmem_type_names[0]))

/* local array for keeping track of memory */
static _xmem_array _xmem_info_sum[_XMEM_NUM_STD_TYPES];

/*!
   free (deallocate) a matrix.
 */
int _xmat_free(_xmat *mat){
#ifdef _XSEGMENTED
    int i;
#endif

    if (!mat || (int)(mat->m) < 0 || (int)(mat->n) < 0) /* don't trust it */
        return (-1);
   
#ifndef _XSEGMENTED
    if (mat->base != (_xreal *)NULL){
        if (_xmem_info_is_on()){
            _xmem_bytes(_XTYPE_MAT,mat->max_m*mat->max_n*sizeof(_xreal),0);
        }
        _xfree(mat->base);
    }
#else
    for (i = 0; i < mat->max_m; i++)
        if (mat->me[i] != (_xreal *)NULL){
            if (_xmem_info_is_on()){
                _xmem_bytes(_XTYPE_MAT,mat->max_n*sizeof(_xreal),0);
            }
            _xfree(mat->me[i]);
        }
#endif
    if (mat->me != (_xreal **)NULL){
        if (_xmem_info_is_on()){
            _xmem_bytes(_XTYPE_MAT,mat->max_m*sizeof(_xreal *),0);
        }
        _xfree(mat->me);
    }
   
    if (_xmem_info_is_on()){
        _xmem_bytes(_XTYPE_MAT,sizeof(_xmat),0);
        _xmem_numvar(_XTYPE_MAT,-1);
    }
    _xfree(mat);
   
    return (0);
}

/*!
   deallocate (destroy) band matrix 
   -- returns (-1) on error and 0 otherwise
 */
int _xband_free(_xband *A){
    if (!A || A->lb < 0 || A->ub < 0) /* don't trust it */
        return (-1);

    if (A->mat) _xmat_free(A->mat);

    if (_xmem_info_is_on()){
        _xmem_bytes(_XTYPE_BAND,sizeof(_xband),0);
        _xmem_numvar(_XTYPE_BAND,-1);
    }

    _xfree(A);
    return (0);
}

/*!
   free (deallocate) a permutation.
 */
int _xperm_free(_xperm *px){
    if (px==(_xperm *)NULL || (int)(px->size) < 0)/* don't trust it */
        return (-1);
   
    if (px->pe == (unsigned int *)NULL){
        if (_xmem_info_is_on()){
            _xmem_bytes(_XTYPE_PERM,sizeof(_xperm),0);
            _xmem_numvar(_XTYPE_PERM,-1);
        }      
        _xfree(px);
    }
    else {
        if (_xmem_info_is_on()){
            _xmem_bytes(_XTYPE_PERM,sizeof(_xperm)+px->max_size*sizeof(unsigned int),0);
            _xmem_numvar(_XTYPE_PERM,-1);
        }
        _xfree(px->pe);
        _xfree(px);
    }
   
    return (0);
}

/*!
   free (deallocate) a vector.
 */
int _xvec_free(_xvec *vec){
    if (!vec || (int)(vec->dim) < 0)/* don't trust it */
        return (-1);
   
    if (vec->ve == (_xreal *)NULL){
        if (_xmem_info_is_on()){
            _xmem_bytes(_XTYPE_VEC,sizeof(_xvec),0);
            _xmem_numvar(_XTYPE_VEC,-1);
        }
        _xfree(vec);
    }
    else {
        if (_xmem_info_is_on()){
          _xmem_bytes(_XTYPE_VEC,sizeof(_xvec)+vec->max_dim*sizeof(_xreal),0);
          _xmem_numvar(_XTYPE_VEC,-1);
        }
        _xfree(vec->ve);
        _xfree(vec);
    }
   
    return (0);
}

/*!
   free (deallocate) an integer vector.
 */
int _xivec_free(_xivec *iv){
    if (!iv || iv->dim > _XMAXDIM) /* don't trust it */
        return (-1);
   
    if (iv->ive == (int *)NULL){
        if (_xmem_info_is_on()){
            _xmem_bytes(_XTYPE_IVEC,sizeof(_xivec),0);
            _xmem_numvar(_XTYPE_IVEC,-1);
        }
        _xfree(iv);
    }
    else {
        if (_xmem_info_is_on()){
            _xmem_bytes(_XTYPE_IVEC,sizeof(_xivec)+iv->max_dim*sizeof(int),0);
            _xmem_numvar(_XTYPE_IVEC,-1);
        } 
        _xfree(iv->ive);
        _xfree(iv);
    }
   
    return (0);
}

/*!
   release memory
 */
int _xiter_free(_xiter *ip){
    if (!ip) return -1;
   
    if (_xmem_info_is_on()){
        _xmem_bytes(_XTYPE_ITER,sizeof(_xiter),0);
        _xmem_numvar(_XTYPE_ITER,-1);
    }

    if (!ip->shared_x && ip->x) _xvec_free(ip->x);
    if (!ip->shared_b && ip->b) _xvec_free(ip->b);

    _xfree(ip);

    return (0);
}

/*!
   release a row of a matrix
 */
int _xsprow_free(_xsprow *r){
    if (!r)
        return -1;

    if (_xmem_info_is_on()){
        _xmem_bytes(_XTYPE_SPROW,sizeof(_xsprow),0);
        _xmem_numvar(_XTYPE_SPROW,-1);
    }
   
    if (r->elt){
        if (_xmem_info_is_on()){
            _xmem_bytes(_XTYPE_SPROW,r->maxlen*sizeof(_xrow_elt),0);
        }
        _xfree(r->elt);
    }
    _xfree(r);
    return 0;
}

/*!
   free up the memory for a sparse matrix
 */
int _xspmat_free(_xspmat *A){
    _xsprow *r;
    int i;
   
    if (!A)
        return (-1);
    if (A->start_row != (int *)NULL){
        if (_xmem_info_is_on()){
            _xmem_bytes(_XTYPE_SPMAT,A->max_n*sizeof(int),0);
        }
        _xfree(A->start_row);
    }
    if (A->start_idx != (int *)NULL){
        if (_xmem_info_is_on()){
            _xmem_bytes(_XTYPE_SPMAT,A->max_n*sizeof(int),0);
        }
        _xfree(A->start_idx);
    }
    if (!A->row){
        if (_xmem_info_is_on()){
            _xmem_bytes(_XTYPE_SPMAT,sizeof(_xspmat),0);
            _xmem_numvar(_XTYPE_SPMAT,-1);
        } 
        _xfree(A);
        return (0);
    }
    for (i = 0; i < A->m; i++){
        r = &(A->row[i]);
        if (r->elt != (_xrow_elt *)NULL){
            if (_xmem_info_is_on()){
                _xmem_bytes(_XTYPE_SPMAT,A->row[i].maxlen*sizeof(_xrow_elt),0);
            }
            _xfree(r->elt);
        }
    }
   
    if (_xmem_info_is_on()){
        if (A->row) 
            _xmem_bytes(_XTYPE_SPMAT,A->max_m*sizeof(_xsprow),0);
        _xmem_bytes(_XTYPE_SPMAT,sizeof(_xspmat),0);
        _xmem_numvar(_XTYPE_SPMAT,-1);
    }
   
    _xfree(A->row);
    _xfree(A);

    return (0);
}

/*!
 * return _xzvec & associated memory back to heap
 */
int _xzvec_free(_xzvec *vec){ 
    if (vec == (_xzvec *)NULL || (int)(vec->dim) < 0)
        return (-1);
        
    if (vec->ve == (_xcomplex *)NULL){
        if (_xmem_info_is_on()){
            _xmem_bytes(_XTYPE_ZVEC,sizeof(_xzvec),0);
            _xmem_numvar(_XTYPE_ZVEC,-1);
        }
        _xfree((char *)vec);
    }
    else {
        if (_xmem_info_is_on()){
            _xmem_bytes(_XTYPE_ZVEC,vec->max_dim*sizeof(_xcomplex)+sizeof(_xzvec),0);
            _xmem_numvar(_XTYPE_ZVEC,-1);
        }
        _xfree((char *)vec->ve);
        _xfree((char *)vec);
    }
    
    return (0); 
}

/*! return _xzmat & asoociated memory back to memory heap */
int _xzmat_free(_xzmat *mat){
#ifdef _XSEGMENTED
    int i;
#endif
    if (!mat || (int)(mat->m) < 0 ||
       (int)(mat->n) < 0 )
        /* don't trust it */
        return (-1);
#ifndef _XSEGMENTED
    if (mat->base != (_xcomplex *)NULL ){
        if (_xmem_info_is_on()){
            _xmem_bytes(_XTYPE_ZMAT,mat->max_m*mat->max_n*sizeof(_xcomplex),0);
        }    
        _xfree((char *)(mat->base));
    }
#else
    for (i=0; i<mat->max_m; i++)
        if (mat->me[i] != (_xcomplex *)NULL ){
            if (_xmem_info_is_on()){
                _xmem_bytes(_XTYPE_ZMAT,mat->max_n*sizeof(_xcomplex),0);
            }
            _xfree((char *)(mat->me[i]));
        }
#endif
    if (mat->me != (_xcomplex **)NULL ){
        if (_xmem_info_is_on()){
            _xmem_bytes(_XTYPE_ZMAT,mat->max_m*sizeof(_xcomplex *),0);
        }    
        _xfree((char *)(mat->me));
    }
   
    if (_xmem_info_is_on()){
        _xmem_bytes(_XTYPE_ZMAT,sizeof(_xzmat),0);
        _xmem_numvar(_XTYPE_ZMAT,-1);
    }
    _xfree((char *)mat);
   
    return (0); 
}

/* for freeing various types */
static int (*_xmem_free_funcs[_XMEM_NUM_STD_TYPES])() = {
    (int (*)())_xmat_free,
    (int (*)())_xband_free,
    (int (*)())_xperm_free,    
    (int (*)())_xvec_free,  
    (int (*)())_xivec_free,
    (int (*)())_xiter_free, 
    (int (*)())_xsprow_free, 
    (int (*)())_xspmat_free,
    (int (*)())_xzvec_free,  
    (int (*)())_xzmat_free
};

/* it is a global variable for passing pointers to local arrays defined here */
_xmem_connect _xmem_connects[_XMEM_CONNECT_MAX_LISTS] = {
    {   _xmem_type_names, 
        _xmem_free_funcs, 
        _XMEM_NUM_STD_TYPES, 
        _xmem_info_sum 
    } 
};

/* local memory variables */
static int _xmem_switched_on = _XMEM_SWITCH_ON_DEF;  /* on/off */

/*!
   check whether memory data being accumulated or not.
 */
int _xmem_info_is_on(void){
    return (_xmem_switched_on);
}

/*!
   turn memory info system on/off
 */
int _xmem_info_on(int sw){
    return (sw ^= _xmem_switched_on ^= sw ^= _xmem_switched_on);
}

/*!
   number of bytes used by a type.
 */
int _xmem_info_bytes(int type, int list){
    if (list < 0 || list >= _XMEM_CONNECT_MAX_LISTS )
        return (0);
    if (!_xmem_switched_on || type < 0 
        || (unsigned int)type >= _xmem_connects[list].ntypes
        || _xmem_connects[list].free_funcs[type] == NULL )
        return (0);
   
    return _xmem_connects[list].info_sum[type].bytes;
}

/*!
   number of structures of a type.
 */
int _xmem_info_numvar(int type, int list){
    if (list < 0 || list >= _XMEM_CONNECT_MAX_LISTS )
        return (0);
    if (!_xmem_switched_on || type < 0 
        || (unsigned int)type >= _xmem_connects[list].ntypes
        || _xmem_connects[list].free_funcs[type] == NULL )
        return (0);
   
    return _xmem_connects[list].info_sum[type].numvar;
}

/*!
   print memory info to a stream.
 */
void _xmem_info_file(FILE *fp, int list){
    unsigned int type;
    int t = 0, n = 0, nt = 0, d;
    _xmem_connect *mlist;
   
    if (!_xmem_switched_on || list < 0 || list >= _XMEM_CONNECT_MAX_LISTS)
        return;
   
    if (list == 0)
        (void) fprintf(fp," MEMORY INFORMATION (standard types):\n");
    else
        (void) fprintf(fp," MEMORY INFORMATION (list no. %d):\n",list);

    mlist = &_xmem_connects[list];

    for (type=0; type < mlist->ntypes; type++){
        if (mlist->type_names[type] == NULL ) continue;
        d = mlist->info_sum[type].bytes;
        t += d;
        n = mlist->info_sum[type].numvar;
        nt += n;
        (void) fprintf(fp," type %-7s %10d alloc. byte%c  %6d alloc. variable%c\n",
                       mlist->type_names[type], d, (d!=1 ? 's' : ' '),
                       n, (n!=1 ? 's' : ' '));
    }

    (void) fprintf(fp," %-12s %10d alloc. byte%c  %6d alloc. variable%c\n\n",
                   "total:",t, (t!=1 ? 's' : ' '), nt, (nt!=1 ? 's' : ' '));
}

/*!
   notify change in memory usage.
 */
void _xmem_bytes_list(int type, int old_size, int new_size, int list){
    _xmem_connect *mlist;
   
    if (list < 0 || list >= _XMEM_CONNECT_MAX_LISTS )
        return;
   
    mlist = &_xmem_connects[list];
    if (type < 0 || (unsigned int)type >= mlist->ntypes || mlist->free_funcs[type] == NULL)
        return;

    if (old_size < 0 || new_size < 0)
        _xerror(_XE_NEG,"_xmem_bytes_list");

    mlist->info_sum[type].bytes += new_size - old_size;
   
    /* check if the number of bytes is non-negative */
    if (old_size > 0){
        if (mlist->info_sum[type].bytes < 0){
            (void) fprintf(stderr,"\nWARNING !! memory info: allocated memory is less than 0\n");
            (void) fprintf(stderr,"\t TYPE %s \n\n", mlist->type_names[type]);

            if (!isatty(_fileno(stdout))){
                (void) fprintf(stdout,"\nWARNING !! memory info: allocated memory is less than 0\n");
                (void) fprintf(stdout,"\t TYPE %s \n\n", mlist->type_names[type]);
            }
        }
    }
}

/*!
   notify change in number of structures allocated.
 */
void _xmem_numvar_list(int type, int num, int list){
    _xmem_connect *mlist;
   
    if (list < 0 || list >= _XMEM_CONNECT_MAX_LISTS)
        return;
   
    mlist = &_xmem_connects[list];
    if (type < 0 || (unsigned int)type >= mlist->ntypes || mlist->free_funcs[type] == NULL)
        return;

    mlist->info_sum[type].numvar += num;
   
    /* check if the number of variables is non-negative */
    if (num < 0){
        if (mlist->info_sum[type].numvar < 0){
            (void) fprintf(stderr,"\nWARNING !! memory info: allocated # of variables is less than 0\n");
            (void) fprintf(stderr,"\t TYPE %s \n\n", mlist->type_names[type]);
            if (!isatty(_fileno(stdout))){
                (void) fprintf(stdout,"\nWARNING !! memory info: allocated # of variables is less than 0\n");
                (void) fprintf(stdout,"\t TYPE %s \n\n", mlist->type_names[type]);
            }
        }
    }
}

/* local type & variables for memory stat */
typedef struct __xmem_stat {
    void **var;   /* for &A, where A is a pointer */
    int type;     /* type of A */
    int mark;     /* what mark is chosen */
    char *fname;  /* source file name where last registered */
    int line;     /* line # of file where last registered */
} _xmem_stat;

static int _xmem_stat_mark_many = 0; /* how many marks are used */
static int _xmem_stat_mark_curr = 0; /* current mark */

static _xmem_stat _xmem_stat_var[_XMEM_HASHSIZE];
static unsigned int _xmem_hash_idx[_XMEM_HASHSIZE];
static unsigned int _xmem_hash_idx_end = 0;

static unsigned int _xmem_hash(void **ptr){
    return (unsigned int)((unsigned long)ptr % _XMEM_HASHSIZE);
}

static int _xmem_lookup(void **var){
    int k, j;

    k = _xmem_hash(var);
    if (_xmem_stat_var[k].var == var){
        return -1;
    }
    else if (_xmem_stat_var[k].var == NULL){
        return k;
    }
    else {  /* look for an empty place */
        j = k;
        while (_xmem_stat_var[j].var != var && j < _XMEM_HASHSIZE && _xmem_stat_var[j].var != NULL) 
            j++;

        if (_xmem_stat_var[j].var == NULL) return j;
        else if (_xmem_stat_var[j].var == var) return -1; 
        else { /* if (j == _XMEM_HASHSIZE) */
            j = 0;
            while (_xmem_stat_var[j].var != var && j < k && _xmem_stat_var[j].var != NULL) 
                j++;
            if (_xmem_stat_var[j].var == NULL) return j;
            else if (_xmem_stat_var[j].var == var) return -1; 
            else { /* if (j == k) */
                (void) fprintf(stderr,"\nWARNING !!! static memory: mem_stat_var is too small\n");
                (void) fprintf(stderr,"Increase MEM_HASHSIZE in file: %s (currently = %d)\n\n",
                               _XMEM_HASHSIZE_FILE, _XMEM_HASHSIZE);
                if (!isatty(_fileno(stdout))){
                    (void) fprintf(stdout,"\nWARNING !!! static memory: mem_stat_var is too small\n");
                    (void) fprintf(stdout,"Increase MEM_HASHSIZE in file: %s (currently = %d)\n\n",
                                   _XMEM_HASHSIZE_FILE, _XMEM_HASHSIZE);
                }
                _xerror(_XE_MEM,"_xmem_lookup");
            }
        }
    }

    return -1;
}

int _xmem_stat_reg_list(void **var, int type, int list, char *fname, int line){
    int n;

    if (list < 0 || list >= _XMEM_CONNECT_MAX_LISTS) return -1;
    if (_xmem_stat_mark_curr == 0) return 0;  /* not registered */
    if (var == NULL) return -1;               /* error */

    if (type < 0 || (unsigned int)type >= _xmem_connects[list].ntypes || _xmem_connects[list].free_funcs[type] == NULL){
        _xwarning(_XWARN_WRONG_TYPE,"_xmem_stat_reg_list");
        return -1;
    }
   
    if ((n = _xmem_lookup(var)) >= 0){
        _xmem_stat_var[n].var = var;
        _xmem_stat_var[n].mark = _xmem_stat_mark_curr;
        _xmem_stat_var[n].type = type;
        _xmem_stat_var[n].fname = fname;
        _xmem_stat_var[n].line = line;
        /* save n+1, not n */
        _xmem_hash_idx[_xmem_hash_idx_end++] = n+1;
    }

    return _xmem_stat_mark_curr;
}

/*!
   set mark for workspace.
 */
int _xmem_stat_mark(int mark){
    if (mark < 0){
        _xmem_stat_mark_curr = 0;
        return -1;   /* error */
    }
    else if (mark == 0){
        _xmem_stat_mark_curr = 0; 
        return 0;
    }

    _xmem_stat_mark_curr = mark;
    _xmem_stat_mark_many++;

    return mark;
}

int _xmem_stat_free_list(int mark, int list){
    unsigned int i,j;
    int (*free_fn)();

    if (list < 0 || list >= _XMEM_CONNECT_MAX_LISTS || _xmem_connects[list].free_funcs == NULL)
        return -1;

    if (mark < 0){
        _xmem_stat_mark_curr = 0;
        return -1;
    }
    else if (mark == 0){
        _xmem_stat_mark_curr = 0;
        return 0;
    }
   
    if (_xmem_stat_mark_many <= 0){
        _xwarning(_XWARN_NO_MARK,"_xmem_stat_free");
        return -1;
    }

    #ifdef _XDEBUG
    (void) fprintf(stdout, "mem_stat_free: Freeing variables registered for mark %d\n", mark);
    #endif/* _XDEBUG */
    /* deallocate the marked variables */
    for (i=0; (unsigned int)i < _xmem_hash_idx_end; i++){
        j = _xmem_hash_idx[i];
        if (j == 0) continue;
        else {
            j--;
            if (_xmem_stat_var[j].mark == mark){
                free_fn = _xmem_connects[list].free_funcs[_xmem_stat_var[j].type];
                #ifdef _XDEBUG
                (void) fprintf(stdout, "# Freeing variable(s) registered in file \"%s\", line %d\n",
                               _xmem_stat_var[j].fname, _xmem_stat_var[j].line);
                #endif/* _XDEBUG */
                if (free_fn != NULL)
                    (*free_fn)(*_xmem_stat_var[j].var);
                else
                    _xwarning(_XWARN_WRONG_TYPE,"_xmem_stat_free");
            
                *(_xmem_stat_var[j].var) = NULL;
                _xmem_stat_var[j].var = NULL;
                _xmem_stat_var[j].mark = 0;
                _xmem_stat_var[j].fname = NULL;
                _xmem_stat_var[j].line = 0;
                _xmem_hash_idx[i] = 0;
            }
        }
    }

    while (_xmem_hash_idx_end > 0 && _xmem_hash_idx[_xmem_hash_idx_end-1] == 0)
        _xmem_hash_idx_end--;

    _xmem_stat_mark_curr = 0;
    _xmem_stat_mark_many--;
    return (0);
}

/*!
   current workspace group
 */
int _xmem_stat_show_mark(void){
    return _xmem_stat_mark_curr;
}

int _xmem_stat_reg_vars(int list,int type,char *fname,int line, ...){
    va_list ap;
    int i=0;
    void **par;
    
    va_start(ap, line);
    while (par = va_arg(ap, void **)){
        _xmem_stat_reg_list(par,type,list,fname,line);
        i++;
    }
    va_end(ap);
    
    return (i);
}

/*!
   print information on registered workspace.
 */
void _xmem_stat_dump(FILE *fp, int list){
    unsigned int i,j,k=1;

    if (list < 0 || list >= _XMEM_CONNECT_MAX_LISTS || _xmem_connects[list].free_funcs == NULL)
        return;

    (void) fprintf(fp," Array mem_stat_var (list no. %d):\n",list);
    for (i=0; (unsigned int)i < _xmem_hash_idx_end; i++){
        j = _xmem_hash_idx[i];
        if (j == 0) continue;
        else {
            j--;
            (void) fprintf(fp," %d.  var = 0x%p, type = %s, mark = %d\n",
                           k,_xmem_stat_var[j].var,
                           (unsigned int)_xmem_stat_var[j].type < _xmem_connects[list].ntypes &&
                           _xmem_connects[list].free_funcs[_xmem_stat_var[j].type] != NULL ?
                           _xmem_connects[list].type_names[(int)_xmem_stat_var[j].type] : 
                           "???",
                           _xmem_stat_var[j].mark);
            k++;
        }
    }
   
    (void) fprintf(fp,"\n");
}

int _xmem_free_vars(int list){
    if (list < 0 || list >= _XMEM_CONNECT_MAX_LISTS)
        return -1;
   
    _xmem_connects[list].ntypes = 0;
    _xmem_connects[list].type_names = NULL;
    _xmem_connects[list].free_funcs = NULL;
    _xmem_connects[list].info_sum = NULL;
   
    return (0);
}

/*!
   check whether a list of types attached or not.
 */
int _xmem_is_list_attached(int list){
    if (list < 0 || list >= _XMEM_CONNECT_MAX_LISTS)
        return _XFALSE;

    return ((_xmem_connects[list].type_names != NULL &&
             _xmem_connects[list].free_funcs != NULL &&
             _xmem_connects[list].info_sum != NULL) ?
             _XTRUE : _XFALSE);
}

/*!
   add a new family of types
 */
int _xmem_attach_list(int list, 
                      int ntypes, 
                      char **type_names,
                      int (**free_funcs)(void *), 
                      _xmem_array* info_sum){

    if (list < 0 || list >= _XMEM_CONNECT_MAX_LISTS)
        return -1;

    if (type_names == NULL || free_funcs == NULL || info_sum == NULL || ntypes < 0)
        return -1;
   
    /* if a list exists do not overwrite */
    if (_xmem_connects[list].ntypes != 0)
        _xerror(_XE_OVERWRITE,"_xmem_attach_list");
   
    _xmem_connects[list].ntypes = ntypes;
    _xmem_connects[list].type_names = type_names;
    _xmem_connects[list].free_funcs = free_funcs;
    _xmem_connects[list].info_sum = info_sum;
   
    return (0);
}

void _xmem_dump_list(FILE *fp, int list){
    int i;
    _xmem_connect *mlist;

    if (list < 0 || list >= _XMEM_CONNECT_MAX_LISTS)
        return;

    mlist = &_xmem_connects[list];
    (void) fprintf(fp," %15s[%d]:\n","CONTENTS OF mem_connect",list);
    (void) fprintf(fp," %-7s   %-12s   %-9s   %s\n",
                   "name of",
                   "alloc.", "# alloc.",
                   "address");
    (void) fprintf(fp," %-7s   %-12s   %-9s   %s\n",
                   " type",
                   "bytes", "variables",
                   "of *_free()");

    for (i=0; (unsigned int)i < mlist->ntypes; i++) 
        (void) fprintf(fp,"  %-7s   %-12ld   %-9d   %p\n",
                       mlist->type_names[i], mlist->info_sum[i].bytes,
                       mlist->info_sum[i].numvar, mlist->free_funcs[i]);
    (void) fprintf(fp,"\n");
}

/*!
   inner product of arrays.
 */
double __xip__(const _xreal *dp1, const _xreal *dp2, int len){
#ifdef _XVUNROLL
    register int len4;
    register _xreal sum1,sum2,sum3;
#endif/*_XVUNROLL*/
    register int i;
    register _xreal sum;
    
    sum = (_xreal)0.0;
#ifdef _XVUNROLL
    sum1 = sum2 = sum3 = (_xreal)0.0;
    
    len4 = len / 4;
    len  = len % 4;
    
    for (i = 0; i < len4; i++){
        sum  += dp1[4*i]*dp2[4*i];
        sum1 += dp1[4*i+1]*dp2[4*i+1];
        sum2 += dp1[4*i+2]*dp2[4*i+2];
        sum3 += dp1[4*i+3]*dp2[4*i+3];
    }
    sum += sum1 + sum2 + sum3;
    dp1 += 4*len4;  dp2 += 4*len4;
#endif/*_XVUNROLL*/
    
    for (i = 0; i < len; i++)
        sum += dp1[i]*dp2[i];
    
    return (sum);
}

/*!
   form $x + s*$y for arrays.
 */
void __xmltadd__(_xreal *dp1, const _xreal *dp2, double s, int len){
    register int i;
#ifdef _XVUNROLL
    register int len4;
    
    len4 = len / 4;
    len  = len % 4;
    for (i = 0; i < len4; i++){
        dp1[4*i]   += s*dp2[4*i];
        dp1[4*i+1] += s*dp2[4*i+1];
        dp1[4*i+2] += s*dp2[4*i+2];
        dp1[4*i+3] += s*dp2[4*i+3];
    }
    dp1 += 4*len4;  dp2 += 4*len4;
#endif/*_XVUNROLL*/

    for (i = 0; i < len; i++)
        dp1[i] += (_xreal)(s*dp2[i]);
}

/*!
   add arrays.
 */
void __xadd__(const _xreal *dp1, const _xreal *dp2, _xreal *out, int len){
    register int i;
    for (i = 0; i < len; i++)
        out[i] = dp1[i] + dp2[i];
}

/*!
   substract an array from another.
 */
void __xsub__(const _xreal *dp1, const _xreal *dp2, _xreal *out, int len){
    register int i;
    for (i = 0; i < len; i++)
        out[i] = dp1[i] - dp2[i];
}

/*!
   scalar - vector multiplication for arrays.
 */
void __xsmlt__(const _xreal *dp, double s, _xreal *out, int len){
    register int i;
    for (i = 0; i < len; i++)
        out[i] = (_xreal)(s*dp[i]);
}

/*!
   zero an array.
 */
void __xzero__(_xreal *dp, int len){
#ifndef _XCHAR0_ISNOT_DBL0
    _xmem_zero((char *)dp,len*sizeof(_xreal));
#else
    register int i;
    for (i = 0; i < len; i++)
        dp[i] = (_xreal)0.0;
#endif/*_XCHAR0_ISNOT_DBL0*/
}

/*!
   allocate and initialize a vector.
 */
_xvec *_xvec_get(int size){
    _xvec *vector;
    
    if (size <0) _xerror(_XE_NEG, "_xvec_get");
    if ((vector=_xnew(_xvec)) == (_xvec *)NULL) _xerror(_XE_MEM,"_xvec_get");
    else if (_xmem_info_is_on()){
        _xmem_bytes(_XTYPE_VEC,0,sizeof(_xvec));
        _xmem_numvar(_XTYPE_VEC,1);
    }
    
    vector->dim = vector->max_dim = size;
    if ((vector->ve=_xnew_a(size,_xreal)) == (_xreal *)NULL){
        _xfree(vector);
        _xerror(_XE_MEM,"_xvec_get");
    }
    else if (_xmem_info_is_on()){
        _xmem_bytes(_XTYPE_VEC,0,size*sizeof(_xreal));
    }
    
    return (vector);
}

/*!
   zero a vector
 */
_xvec *_xvec_zero(_xvec *x){
    if (!x) _xerror(_XE_NULL, "_xvec_zero");
    __xzero__(x->ve,x->dim);
    return (x);
}

/*!
   set vector to all 1's
 */
_xvec *_xvec_ones(_xvec *x){
    unsigned int i;
    
    if (!x) _xerror(_XE_NULL,"_xvec_ones");
    for (i = 0; i < x->dim; i++)
        x->ve[i] = 1.0;
    
    return (x);
}

#ifdef LONG_MAX
#define _XMODULUS LONG_MAX
#else
#define _XMODULUS 1000000000L /* assuming long's at least 32 bits long */
#endif
#define _XMZ 0L

static long _xmrand_list[56];
static int _xstarted = _XFALSE;
static int _xinext = 0, _xinextp = 31;

double _xmrand(){
    long lval;
    static _xreal factor = 1.0/((_xreal)_XMODULUS);
    
    if (!_xstarted) _xsmrand(3127);
    
    _xinext = (_xinext >= 54) ? 0 : _xinext+1;
    _xinextp = (_xinextp >= 54) ? 0 : _xinextp+1;

    lval = _xmrand_list[_xinext]-_xmrand_list[_xinextp];
    if (lval < 0L) lval += _XMODULUS;
    _xmrand_list[_xinext] = lval;
    
    return (double)lval*factor;
}

void _xmrandlist(_xreal *a, int len){
    int i;
    long lval;
    static _xreal factor = 1.0/((_xreal)_XMODULUS);
    
    if (!_xstarted) _xsmrand(3127);
    
    for (i = 0; i < len; i++){
        _xinext = (_xinext >= 54) ? 0 : _xinext+1;
        _xinextp = (_xinextp >= 54) ? 0 : _xinextp+1;
        
        lval = _xmrand_list[_xinext]-_xmrand_list[_xinextp];
        if (lval < 0L) lval += _XMODULUS;
        _xmrand_list[_xinext] = lval;
        
        a[i] = (_xreal)lval*factor;
    }
}

void _xsmrand(int seed){
    int i;

    _xmrand_list[0] = (123413*seed) % _XMODULUS;
    for (i = 1; i < 55; i++)
        _xmrand_list[i] = (123413*_xmrand_list[i-1]) % _XMODULUS;

    _xstarted = _XTRUE;

    /* run _xmrand() through the list sufficient times to
       thoroughly randomise the array */
    for (i = 0; i < 55*55; i++) _xmrand();
}
/*!
   randomise entries of a vector, components
   independently & uniformly ditributed between 0 and 1
 */
_xvec *_xvec_rand(_xvec *x){
    if (!x) _xerror(_XE_NULL, "_xvec_rand");
    _xmrandlist(x->ve,x->dim);
    return (x);
}

/*!
    initialize x so that x->ve[i] == i
 */
_xvec *_xvec_count(_xvec *x){
    unsigned int i;
    
    if (!x) _xerror(_XE_NULL,"_xvec_count");
    
    for (i = 0; i < x->dim; i++)
        x->ve[i] = (_xreal)i;
    
    return (x);
}

/*!
   copy vector into new area :
   --out(i0:dim) <- in(i0:dim)
 */
_xvec *_xvec_copy(const _xvec *in, _xvec *out, unsigned int i0){
    if (!in) _xerror(_XE_NULL,"_xvec_copy");
    if (!_xvec_chk_idx(in,i0)) _xerror(_XE_BOUNDS,"_xvec_copy");
    if (in == out) return (out);
    if (!out || out->dim < in->dim)
      out = _xvec_resize(out, in->dim);
    _xmem_copy(&(in->ve[i0]),&(out->ve[i0]),(in->dim - i0)*sizeof(_xreal));
    
    return (out);
}

/*!
    copy selected pieces of a vector
  -- moves the length dim0 subvector with initial index i0
     to the corresponding subvector of out with initial index i1
  -- out is resized if necessary
 */
_xvec *_xvec_move(const _xvec *in, int i0, int dim0, _xvec *out, int i1){
    if (!in) _xerror(_XE_NULL, "_xvec_move");
    if (i0 < 0 || dim0 < 0 || i1 < 0 || (unsigned int)(i0 + dim0) > in->dim)
      _xerror(_XE_BOUNDS, "_xvec_move");
    if ((!out) || (unsigned int)(i1 + dim0) > out->dim)
      out = _xvec_resize(out, i1+dim0);
    _xmem_copy(&(in->ve[i0]),&(out->ve[i1]),dim0*sizeof(_xreal));
    return (out);
}

/*!
   resize a vector
 */
_xvec *_xvec_resize(_xvec *x, int new_dim){
    if (new_dim < 0) _xerror(_XE_NEG,"_xvec_resize");
    if (!x) return _xvec_get(new_dim);
    if (new_dim == x->dim) return x;/* nothing is changed */

    if (x->max_dim == 0) /* assume that it's from sub_vec */
        return _xvec_get(new_dim);
   
    if ((unsigned int)new_dim > x->max_dim){
        if (_xmem_info_is_on()){ 
            _xmem_bytes(_XTYPE_VEC,x->max_dim*sizeof(_xreal),
            new_dim*sizeof(_xreal));
        }

        x->ve = _xrenew(x->ve,new_dim,_xreal);
        if (!x->ve)
            _xerror(_XE_MEM,"_xvec_resize");
        x->max_dim = new_dim;
    }
   
    if ((unsigned int)new_dim > x->dim)
        __xzero__(&(x->ve[x->dim]),new_dim - x->dim);
    x->dim = new_dim;
   
    return (x);
}

/*!
    to allocate memory to many arguments, the function should be called:
  _xvec_get_vars(dim,&x,&y,&z,...,NULL);
  where
    int dim;
    _xvec *x,*y,*z,...;
    the last argument should be NULL!
    dim is the length of vectors x,y,z,...
    returned value is equal to the number of allocated variables
 */
int _xvec_get_vars(int dim, ...){
    va_list ap;
    int i = 0;
    _xvec **par;
    
    va_start(ap,dim);
    while (par = va_arg(ap, _xvec **)){ /* NULL ends the list */
        *par = _xvec_get(dim);
        i++;
    }
    va_end(ap);
    
    return (i);
}

/*!
    to resize memory for many arguments, the function should be called:
  _xvec_resize_vars(new_dim,&x,&y,&z,...,NULL);
  where
    int new_dim;
    _xvec *x,*y,*z,...;
    the last argument should be NULL!
    new_dim is the resized length of vectors x,y,z,...
    returned value is equal to the number of allocated variables
 */
int _xvec_resize_vars(int new_dim, ...){
    va_list ap;
    int i = 0;
    _xvec **par;
    
    va_start(ap,new_dim);
    while (par = va_arg(ap, _xvec **)){ /* NULL ends the list */
        *par = _xvec_resize(*par, new_dim);
        i++;
    }
    va_end(ap);
    
    return (i);
}

/*! 
   to deallocate memory for many arguments, the function should be called:
   _xvec_free_vars(&x,&y,&z,...,NULL);
   where 
     _xvec *x, *y, *z,...;
     The last argument should be NULL ! 
     There must be at least one not NULL argument.
     returned value is equal to the number of allocated variables.
*/
int _xvec_free_vars(_xvec **pv, ...){
    va_list ap;
    int i = 1;
    _xvec **par;
    
    _xvec_free(*pv);
    *pv = (_xvec *)NULL;
    va_start(ap,pv);
    while (par = va_arg(ap, _xvec **)){/* NULL ends list */
        _xvec_free(*par);
        *par = (_xvec *)NULL;
        i++;
    }
    va_end(ap);
    
    return (i);
}

/*!
   return sum of entries of a vector
 */
double _xvec_sum(const _xvec *x){
    unsigned int i;
    _xreal sum;
    
    if (!x) _xerror(_XE_NULL, "_xvec_sum");
    sum = (_xreal)0.0;
    for (i = 0; i < x->dim; i++)
        sum += x->ve[i];
    
    return (double)sum;
}

/*!
    compute minimum component of x, which is returned
  -- also sets min_idx to the index of this minimum
 */
double _xvec_min(const _xvec *x, int *min_idx){
    int i, i_min;
    _xreal min_val, tmp;

    if (!x) _xerror(_XE_NULL,"_xvec_min");
    if (x->dim <= 0) _xerror(_XE_SIZES,"_xvec_min");
    
    i_min = 0;
    min_val = x->ve[0];
    for (i = 1; (unsigned int)i < x->dim; i++){
        tmp = x->ve[i];
        if (tmp < min_val){
            min_val = tmp;
            i_min = i;
        }
    }

    if (min_idx != NULL)
        *min_idx = i_min;
    
    return (double)min_val;
}

/*!
    compute maximum component of x, which is returned
  -- also sets max_idx to the index of this maximum
 */
double _xvec_max(const _xvec *x, int *max_idx){
    int i, i_max;
    _xreal max_val, tmp;

    if (!x) _xerror(_XE_NULL,"_xvec_max");
    if (x->dim <= 0) _xerror(_XE_SIZES,"_xvec_max");

    i_max = 0;
    max_val = x->ve[0];
    for (i = 1; (unsigned int)i < x->dim; i++){
        tmp = x->ve[i];
        if (tmp > max_val){
            max_val = tmp;
            i_max = i;
        }
    }

    if (max_idx != NULL)
        *max_idx = i_max;

    return (double)max_val;
}

/*!
   inner product of two vectors from i0 downwards
   -- that is, returns a(i0:dim)^T.b(i0:dim)
 */
double _xvec_in_prod(const _xvec *a, const _xvec *b, unsigned int i0){
    unsigned int limit;
    if (!a || !b) _xerror(_XE_NULL, "_xvec_in_prod");
    limit = _xmin(a->dim,b->dim);
    if (i0 > limit) _xerror(_XE_BOUNDS, "_xvec_in_prod");
    return __xip__(&(a->ve[i0]),&(b->ve[i0]),(int)(limit-i0));
}

/*!
   compute (scaled) 1-norms of vectors
 */
double _xvec_norm1(const _xvec *x, const _xvec *scale){
    unsigned int i, dim;
    _xreal s, sum;
    
    if (!x) _xerror(_XE_NULL,"_xvec_norm1");
    dim = x->dim;
    sum = (_xreal)0.0;
    if (!scale)
        for (i = 0; i < dim; i++)
            sum += (_xreal)fabs(x->ve[i]);
    else if (scale->dim < dim)
        _xerror(_XE_SIZES,"_xvec_norm1");
    else
        for (i = 0; i < dim; i++){  
            s = scale->ve[i];
            sum += ( s== 0.0 ) ? (_xreal)fabs(x->ve[i]) : (_xreal)fabs(x->ve[i]/s);
        }
    
    return (double)(sum);
}

/*!
    compute (scaled) 2-norm (Euclidean norm) of vectors
 */
double _xvec_norm2(const _xvec *x, const _xvec *scale){
    unsigned int i, dim;
    _xreal s, sum;
    
    if (!x) _xerror(_XE_NULL,"_xvec_norm2");
    dim = x->dim;
    
    sum = (_xreal)0.0;
    if (!scale)
        for (i = 0; i < dim; i++)
            sum += (_xreal)_xsquare(x->ve[i]);
    else if (scale->dim < dim)
        _xerror(_XE_SIZES,"_xvec_norm2");
    else
        for (i = 0; i < dim; i++){
            s = scale->ve[i];
            sum += ( s== 0.0 ) ? (_xreal)_xsquare(x->ve[i]) :
                                 (_xreal)_xsquare(x->ve[i]/s);
        }
    
    return (double)sqrt(sum);
}

/*!
   compute (scaled) infinity-norm (supremum norm) of vectors
 */
double _xvec_norm_inf(const _xvec *x, const _xvec *scale){
    unsigned int i, dim;
    _xreal s, maxval, tmp;
    
    if (!x) _xerror(_XE_NULL,"_xvec_norm_inf");
    dim = x->dim;
    
    maxval = (_xreal)0.0;
    if (!scale)
        for (i = 0; i < dim; i++){  
            tmp = (_xreal)fabs(x->ve[i]);
            maxval = _xmax(maxval,tmp);
        }
    else if (scale->dim < dim)
        _xerror(_XE_SIZES,"_xvec_norm_inf");
    else
        for (i = 0; i < dim; i++){  
            s = scale->ve[i];
            tmp = ( s== 0.0 ) ? (_xreal)fabs(x->ve[i]) : (_xreal)fabs(x->ve[i]/s);
            maxval = _xmax(maxval,tmp);
        }
    
    return (double)(maxval);
}

/*!
   scalar-vector multiply -- out <- scalar*vector 
   -- may be in-situ
 */
_xvec *_xvec_sv_mlt(double scalar, const _xvec *vector, _xvec *out){
    if (vector == (_xvec *)NULL) _xerror(_XE_NULL, "_xvec_sv_mlt");
    if (out == (_xvec *)NULL || out->dim != vector->dim)
        out = _xvec_resize(out,vector->dim);
    if (scalar == 0.0) return _xvec_zero(out);
    if (scalar == 1.0) return _xvec_vcopy(vector,out);
    
    __xsmlt__(vector->ve,(double)scalar,out->ve,(int)(vector->dim));
    return (out);
}

/*!
    vector addition -- out <- v1+v2 -- may be in-situ
 */
_xvec *_xvec_add(const _xvec *vec1, const _xvec *vec2, _xvec *out){
    if (vec1 == (_xvec *)NULL || vec2 == (_xvec *)NULL)
        _xerror(_XE_NULL, "_xvec_add");
    if (vec1->dim != vec2->dim)
        _xerror(_XE_SIZES, "_xvec_add");
    if (out == (_xvec *)NULL || out->dim != vec1->dim)
        out = _xvec_resize(out,vec1->dim);
    __xadd__(vec1->ve,vec2->ve,out->ve,(int)(vec1->dim));
    return (out);
}

/*!
    scalar/vector multiplication and addition
  -- out = v1 + scale.v2
 */
_xvec *_xvec_mltadd(const _xvec *v1, const _xvec *v2, double scale, _xvec *out){
    if (v1 == (_xvec *)NULL || v2 == (_xvec *)NULL)
        _xerror(_XE_NULL, "_xvec_mltadd");
    if (v1->dim != v2->dim)
        _xerror(_XE_SIZES, "_xvec_mltadd");
    if (scale == 0.0) return _xvec_vcopy(v1,out);
    if (scale == 1.0) return _xvec_add(v1,v2,out);
    
    if (v2 != out){
        _xtracecatch(out = _xvec_vcopy(v1,out), "_xvec_mltadd");
      __xmltadd__(out->ve,v2->ve,scale,(int)(v1->dim));
    }
    else {
        _xtracecatch(out = _xvec_sv_mlt(scale,v2,out),"_xvec_mltadd");
        out = _xvec_add(v1,out,out);
    }
    
    return (out);
}

/*!
   vector subtraction -- may be in-situ
 */
_xvec *_xvec_sub(const _xvec *vec1, const _xvec *vec2, _xvec *out){
    if (vec1 == (_xvec *)NULL || vec2 == (_xvec *)NULL)
        _xerror(_XE_NULL, "_xvec_sub");
    if (vec1->dim != vec2->dim)
        _xerror(_XE_SIZES, "_xvec_sub");
    if (out == (_xvec *)NULL || out->dim != vec1->dim)
        out = _xvec_resize(out,vec1->dim);
    __xsub__(vec1->ve,vec2->ve,out->ve,(int)(vec1->dim));
    return (out);
}

/*!
    compute componentwise (Hadamard) product of x1 and x2
  -- result out is returned
 */
_xvec *_xvec_star(const _xvec *x1, const _xvec *x2, _xvec *out){
    int i;

    if (!x1 || !x2) _xerror(_XE_NULL,"_xvec_star");
    if (x1->dim != x2->dim) _xerror(_XE_SIZES,"_xvec_star");

    out = _xvec_resize(out,x1->dim);

    for (i = 0; (unsigned int)i < x1->dim; i++)
        out->ve[i] = x1->ve[i] * x2->ve[i];

    return (out);
}

/*!
    compute convolution product of two vectors
 */
_xvec *_xvec_conv(const _xvec *x1, const _xvec *x2, _xvec *out){
    int i;

    if (!x1 || !x2) _xerror(_XE_NULL,"_xvec_conv");
    if (x1 == out || x2 == out) _xerror(_XE_INSITU,"_xvec_conv");
    if (x1->dim == 0 || x2->dim == 0)
        return out = _xvec_resize(out,0);

    out = _xvec_resize(out,x1->dim + x2->dim - 1);
    _xvec_zero(out);
    for (i = 0; (unsigned int)i < x1->dim; i++)
        __xmltadd__(&(out->ve[i]),x2->ve,x1->ve[i],x2->dim);

    return (out);
}

/*!
    compute componentwise ratio of x2 and x1
  -- out[i] = x2[i] / x1[i]
  -- if x1[i] == 0 for some i, then raise E_SING error
  -- result out is returned
 */
_xvec *_xvec_slash(const _xvec *x1, const _xvec *x2, _xvec *out){
    int i;
    _xreal tmp;

    if (!x1 || !x2) _xerror(_XE_NULL,"_xvec_slash");
    if (x1->dim != x2->dim) _xerror(_XE_SIZES,"_xvec_slash");
    
    out = _xvec_resize(out,x1->dim);
    for (i = 0; (unsigned int)i < x1->dim; i++){
        tmp = x1->ve[i];
        if (tmp == 0.0) _xerror(_XE_SING,"_xvec_slash");
        out->ve[i] = x2->ve[i] / tmp;
    }

    return (out);
}

/*!
    compute a periodic convolution product
  -- the period is the dimension of x2
 */
_xvec *_xvec_pconv(const _xvec *x1, const _xvec *x2, _xvec *out){
    int i;

    if (! x1 || !x2) _xerror(_XE_NULL,"_xvec_pconv");
    if (x1 == out || x2 == out) _xerror(_XE_INSITU,"_xvec_pconv");

    out = _xvec_resize(out,x2->dim);
    if (x2->dim == 0) return (out);

    _xvec_zero(out);
    for (i = 0; (unsigned int)i < x1->dim; i++){
        __xmltadd__(&(out->ve[i]),x2->ve,x1->ve[i],x2->dim - i);
        if (i > 0)
            __xmltadd__(out->ve,&(x2->ve[x2->dim - i]),x1->ve[i],i);
    }

    return (out);
}

/*!
    return sum_i a[i].v[i], a[i] real, v[i] vectors
 */
_xvec *_xvec_lincomb(int n, const _xvec **v, const _xreal *a, _xvec *out){
    int i;
    
    if (!a || !v) _xerror(_XE_NULL,"_xvec_lincomb");
    if (n <= 0) return (_xvec *)NULL;
    
    for (i = 1; i < n; i++)
        if (out == v[i]) _xerror(_XE_INSITU,"_xvec_lincomb");
    
    out = _xvec_sv_mlt(a[0],v[0],out);
    for (i = 1; i < n; i++){
        if (!v[i]) _xerror(_XE_NULL,"_xvec_lincomb");
        if (v[i]->dim != out->dim) _xerror(_XE_SIZES,"_xvec_lincomb");
        out = _xvec_mltadd(out,v[i],a[i],out);
    }

    return (out);
}

/*!
    linear combinations taken from a list of arguments;
    calling:
      _xvec_linlist(out,v1,a1,v2,a2,...,vn,an,NULL);
    where v1 are vectors (_xvec *) and a1 are numbers (double)
 */
_xvec *_xvec_linlist(_xvec *out, _xvec *v1, double a1, ...){
    va_list ap;
    _xvec *par;
    double a_par;

    if (!v1) return (_xvec *)NULL;
   
    va_start(ap, a1);
    out = _xvec_sv_mlt(a1,v1,out);
   
    while (par = va_arg(ap,_xvec *)){ /* NULL ends the list */
        a_par = va_arg(ap,double);
        if (a_par == 0.0) continue;
        if (out == par) _xerror(_XE_INSITU,"_xvec_linlist");
        if (out->dim != par->dim) _xerror(_XE_SIZES,"_xvec_linlist");

        if (a_par == 1.0)
            out = _xvec_add(out,par,out);
        else if (a_par == -1.0)
            out = _xvec_sub(out,par,out);
        else
            out = _xvec_mltadd(out,par,a_par,out); 
    } 
   
    va_end(ap);
    return (out);
}

/*!
    sort vector x, and generates permutation that gives the order
  of the components; x = [1.3, 3.7, 0.5] -> [0.5, 1.3, 3.7] and
  the permutation is order = [2, 0, 1].
  -- if order is NULL on entry then it is ignored
  -- the sorted vector x is returned
 */
#define _XMAX_STACK 60
_xvec *_xvec_sort(_xvec *x, _xperm *order){
    _xreal *x_ve, tmp, v;
    int dim, i, j, l, r, tmp_i;
    int stack[_XMAX_STACK], sp;

    if (!x) _xerror(_XE_NULL,"_xvec_sort");
    if (order != (_xperm *)NULL && order->size != x->dim)
        order = _xperm_resize(order, x->dim);

    x_ve = x->ve;
    dim = x->dim;
    if (order != (_xperm *)NULL)
        _xperm_ident(order);

    if (dim <= 1) return (x);

    /* using quicksort algorithm in Sedgewick,
       "Algorithms in C", Ch. 9, pp. 118--122 (1990) */
    sp = 0;
    l = 0;  r = dim-1;  v = x_ve[0];
    for (;;){
        while (r > l){
            v = x_ve[r];
            i = l-1;
            j = r;
            for (;;){
                while (x_ve[++i] < v) ;
                --j;
                while (x_ve[j] > v && j != 0) --j;
                if (i >= j) break;
                
                tmp = x_ve[i];
                x_ve[i] = x_ve[j];
                x_ve[j] = tmp;
                if (order != (_xperm *)NULL){
                    tmp_i = order->pe[i];
                    order->pe[i] = order->pe[j];
                    order->pe[j] = tmp_i;
                }
            }
            tmp = x_ve[i];
            x_ve[i] = x_ve[r];
            x_ve[r] = tmp;
            if (order != (_xperm *)NULL){
                tmp_i = order->pe[i];
                order->pe[i] = order->pe[r];
                order->pe[r] = tmp_i;
            }
        
            if (i-l > r-i){   
                stack[sp++] = l;   
                stack[sp++] = i-1;   
                l = i+1;   
            }
            else {   
                stack[sp++] = i+1;   
                stack[sp++] = r;   
                r = i-1;   
            }
        }

        /* recursion elimination */
        if (sp == 0) break;
        r = stack[--sp];
        l = stack[--sp];
    }

    return (x);
}

static char _xline[_XMAXLINE];
static const char *_xformat = "%14.9g ";

/*!
   set the printf format string for the Meschach I/O operations
  -- returns the previous format string
 */
const char *_xsetformat(const char *f_string){
    const char *old_f_string;
    old_f_string = _xformat;
    if (f_string != (char *)NULL && *f_string != '\0' )
        _xformat = f_string;
    return old_f_string;
}

/*!
   skip white spaces and strings of the form #....\n
   Here .... is a comment string
 */
int _xskipjunk(FILE *fp){
    int c;
     
    for (;;){ /* forever do... */
    /* skip blanks */
        do {
            c = getc(fp);
        } while (isspace(c));
    
        /* skip comments (if any) */
        if (c == '#')
        /* yes it is a comment (line) */
            while ((c=getc(fp)) != '\n') ;
        else {
            ungetc(c,fp);
            break;
        }
    }
    return 0;
}

/*!
    print a dump of all pointers and data in a onto fp
  -- suitable for low-level debugging
 */
void _xvec_dump(FILE *fp,const _xvec *x){
    unsigned int i, tmp;
     
    if (!x){
        (void) fprintf(fp,"Vector: NULL\n");
        return;         
    }
    (void) fprintf(fp,"Vector: dim: %d @ 0x%lx\n",x->dim,(long)(x));
    if (! x->ve){
        (void) fprintf(fp,"NULL\n");
        return;
    }
    (void) fprintf(fp,"x->ve @ 0x%lx\n",(long)(x->ve));
    for (i=0, tmp=0; i<x->dim; i++, tmp++){
        (void) fprintf(fp,_xformat,x->ve[i]);
        if (tmp % 5 == 4) putc('\n',fp);
    }
    if (tmp % 5 != 0) putc('\n',fp);
}

/*!
    print a representation of x onto file/stream fp
 */
void _xvec_foutput(FILE *fp, const _xvec *x){
    unsigned int i, tmp;
     
    if (!x){
        (void) fprintf(fp,"Vector: NULL\n");
        return;
    }
    (void) fprintf(fp,"Vector: dim: %d\n",x->dim);
    if (x->ve == (_xreal *)NULL){
        fprintf(fp,"NULL\n");
        return;
    }
    for (i=0, tmp=0; i<x->dim; i++, tmp++){
        (void) fprintf(fp,_xformat,x->ve[i]);
        if (tmp % 5 == 4) putc('\n',fp);
    }
    if (tmp % 5 != 0) putc('\n',fp);
}

/*!
    input vector from file/stream fp
  -- input from a terminal is handled interactively
  -- batch/file input has the same format as produced by px_foutput
  except that whitespace and comments ("#..\n") are skipped
  -- returns x, which is created if x == NULL on entry
 */
_xvec *_xvec_finput(FILE *fp,_xvec *x){
    _xvec *_xifin_vec(), *_xbfin_vec();
    return (isatty(_fileno(fp)) ? _xifin_vec(fp,x) :
                                  _xbfin_vec(fp,x));
}

/*!
   interactive input of vector
 */
_xvec *_xifin_vec(FILE *fp,_xvec *vec){
    unsigned int i,dim,dynamic;  /* dynamic set if memory allocated here */
     
    /* get vector dimension */
    if (vec && vec->dim<_XMAXDIM){
        dim = vec->dim; 
        dynamic = _XFALSE;
    }
    else {
        dynamic = _XTRUE;
        do {
            (void) fprintf(stderr,"Vector: dim: ");
            if (fgets(_xline,_XMAXLINE,fp)==NULL)
                _xerror(_XE_INPUT,"_xifin_vec");
        } while (sscanf(_xline,"%u",&dim)<1 || dim>_XMAXDIM );
        vec = _xvec_get(dim);
    }
     
    /* input elements */
    for (i=0; i<dim; i++)
        do {
        redo:
            (void) fprintf(stderr,"entry %u: ",i);
            if (!dynamic)
                (void) fprintf(stderr,"old %14.9g new: ",vec->ve[i]);
            if (fgets(_xline,_XMAXLINE,fp)==NULL)
                _xerror(_XE_INPUT,"_xifin_vec");
            if ((*_xline == 'b' || *_xline == 'B') && i > 0){
                i--;
                dynamic = _XFALSE;
                goto redo;         
            }
            if ((*_xline == 'f' || *_xline == 'F') && i < dim-1){
                i++;
                dynamic = _XFALSE;
                goto redo;
            }
        #if _XREAL == _XDOUBLE
        } while (*_xline=='\0' || sscanf(_xline,"%lf",&vec->ve[i]) < 1);
        #elif _XREAL == _XFLOAT
        } while (*_xline=='\0' || sscanf(_xline,"%f",&vec->ve[i]) < 1);
        #endif
     
    return (vec);
}

_xvec *_xbfin_vec(FILE *fp,_xvec *vec){
    unsigned int i,dim;
    int io_code;
     
    /* get dimension */
    _xskipjunk(fp);
    if ((io_code=fscanf(fp," Vector: dim:%u",&dim)) < 1 || dim>_XMAXDIM)
        _xerror(io_code==EOF ? 7 : 6,"_xbfin_vec");
     
    /* allocate memory if necessary */
    if (!vec) vec = _xvec_resize(vec,dim);
     
    /* get entries */
    _xskipjunk(fp);
    for (i=0; i<dim; i++)
    #if _XREAL == _XDOUBLE
    if ((io_code=fscanf(fp,"%lf",&vec->ve[i])) < 1)
    #elif _XREAL == _XFLOAT
    if ((io_code=fscanf(fp,"%f",&vec->ve[i])) < 1)
    #endif
        _xerror(io_code==EOF ? 7 : 6,"_xbfin_vec");
     
    return (vec);
}

_xperm *_xperm_ident(_xperm *px){
    int i, px_size;
    unsigned int *px_pe;
    
    if (px == (_xperm *)NULL)
        _xerror(_XE_NULL,"_xperm_ident");
    
    px_size = px->size; px_pe = px->pe;
    for (i = 0; i < px_size; i++)
        px_pe[i] = i;
    
    return (px);
}   
    
_xperm *_xperm_get(int size){
    _xperm  *permute;
    int i;

    if (size < 0) _xerror(_XE_NEG,"_xperm_get");
    if ((permute=_xnew(_xperm)) == (_xperm *)NULL)
        _xerror(_XE_MEM,"_xperm_get");
    else if (_xmem_info_is_on()){
        _xmem_bytes(_XTYPE_PERM,0,sizeof(_xperm));
        _xmem_numvar(_XTYPE_PERM,1);
    }
   
    permute->size = permute->max_size = size;
    if ((permute->pe = _xnew_a(size,unsigned int)) == (unsigned int *)NULL )
        _xerror(_XE_MEM,"_xperm_get");
    else if (_xmem_info_is_on()){
        _xmem_bytes(_XTYPE_PERM,0,size*sizeof(unsigned int));
    }
   
    for (i=0; i<size; i++)
        permute->pe[i] = i;
   
    return (permute);
}

/*!
   copy permutation 'in' to 'out'
   -- out is resized to in->size
 */
_xperm *_xperm_copy(const _xperm *in, _xperm *out){
    if (in == (_xperm *)NULL) _xerror(_XE_NULL,"_xperm_copy");
    if (in == out) return (out);
    if (out == (_xperm *)NULL || out->size != in->size)
        out = _xperm_resize(out,in->size);
    
    _xmem_copy(in->pe,out->pe,in->size*sizeof(unsigned int));
    
    return (out);
}

_xperm *_xperm_resize(_xperm *px, int new_size){
    int i;
   
    if (new_size < 0) _xerror(_XE_NEG,"_xperm_resize");
    if (!px) return _xperm_get(new_size);
   
    /* nothing is changed */
    if (new_size == px->size)
        return (px);

    if ((unsigned int)new_size > px->max_size){
        if (_xmem_info_is_on()){
            _xmem_bytes(_XTYPE_PERM,px->max_size*sizeof(unsigned int),
                        new_size*sizeof(unsigned int));
        }
        px->pe = _xrenew(px->pe,new_size,unsigned int);
        if (!px->pe) _xerror(_XE_MEM,"_xperm_resize");
        px->max_size = new_size;
    }
    if (px->size <= (unsigned int)new_size)
    /* extend permutation */
        for (i = px->size; i < new_size; i++)
            px->pe[i] = i;
    else
        for (i = 0; i < new_size; i++)
            px->pe[i] = i;
   
    px->size = new_size;
   
    return (px);
}

int _xperm_get_vars(int dim, ...){
    va_list ap;
    int i=0;
    _xperm **par;
   
    va_start(ap, dim);
    while (par = va_arg(ap,_xperm **)){/* NULL ends the list */
        *par = _xperm_get(dim);
        i++;
    } 
    va_end(ap);

    return (i);
}

int _xperm_resize_vars(int new_dim, ...){
    va_list ap;
    int i=0;
    _xperm **par;
   
    va_start(ap, new_dim);
    while (par = va_arg(ap,_xperm **)){/* NULL ends the list*/
        *par = _xperm_resize(*par,new_dim);
        i++;
    } 
    va_end(ap);

    return (i);
}

int _xperm_free_vars(_xperm **vpx, ...){
    va_list ap;
    int i=1;
    _xperm **par;
   
    _xperm_free(*vpx);
    *vpx = (_xperm *)NULL;
    va_start(ap, vpx);
    while (par = va_arg(ap,_xperm **)){/* NULL ends the list */
        _xperm_free(*par); 
        *par = (_xperm *)NULL;
        i++;
    } 
    va_end(ap);

    return (i);
}

/*!
   apply function componentwise to a vector.
   maps function f over components of x: out[i] = f(x[i])
 */
_xvec *_xvec_map(double (*f)(double),const _xvec *x,_xvec *out){
    _xreal *x_ve, *out_ve;
    int i, dim;
    
    if (!x || !f) _xerror(_XE_NULL,"_xvec_map");
    if (!out || out->dim != x->dim)
        out = _xvec_resize(out,x->dim);
    
    dim = x->dim; x_ve = x->ve; out_ve = out->ve;
    for (i = 0; i < dim; i++)
        *out_ve++ = (_xreal)((*f)(*x_ve++));
    
    return (out);
}

/*!
    apply function componentwise to a vector.
    maps function f over components of x:
    sets out[i] = f(params, x[i]), i = 0, 1, .., dim-1
 */
_xvec *_xvec_vmap(double (*f)(void *,double),void *params,const _xvec *x,_xvec *out){
    _xreal *x_ve, *out_ve;
    int i, dim;
    
    if (!x || !f) _xerror(_XE_NULL,"_xvec_v_map");
    if (!out || out->dim != x->dim)
        out = _xvec_resize(out,x->dim);
    
    dim = x->dim; x_ve = x->ve; out_ve = out->ve;
    for (i = 0; i < dim; i++)
        *out_ve++ = (_xreal)((*f)(params,*x_ve++));
    
    return (out);
}

/*!
    invert permutation -- in situ
  -- taken from ACM Collected Algorithms #250
 */
_xperm *_xperm_inv(const _xperm *px, _xperm *out){
    int i, j, k, n, *p;
    
    out = _xperm_copy(px, out);
    n = out->size;
    p = (int *)(out->pe);
    for (n--; n>=0; n--){
        i = p[n];
        if (i < 0) p[n] = -1 - i;
        else if (i != n){
            k = n;
            while (_XTRUE){
                if (i < 0 || (unsigned int)i >= out->size)
                    _xerror(_XE_BOUNDS,"_xperm_inv");
                j = p[i]; p[i] = -1 - k;
                if (j == n){
                    p[n] = i;
                    break;
                }
                k = i; i = j;
            }
        }
    }
    return (out);
}

/*!
    permutation multiplication (composition)
 */
_xperm *_xperm_mlt(const _xperm *px1, const _xperm *px2, _xperm *out){
    unsigned int i,size;
    
    if (px1==(_xperm *)NULL || px2==(_xperm *)NULL)
        _xerror(_XE_NULL,"_xperm_mlt");
    if (px1->size != px2->size)
        _xerror(_XE_SIZES,"_xperm_mlt");
    if (px1 == out || px2 == out)
        _xerror(_XE_INSITU,"_xperm_mlt");
    if (out==(_xperm *)NULL || out->size < px1->size)
        out = _xperm_resize(out,px1->size);
    
    size = px1->size;
    for (i=0; i<size; i++)
        if (px2->pe[i] >= size)
            _xerror(_XE_BOUNDS,"_xperm_mlt");
        else
            out->pe[i] = px1->pe[px2->pe[i]];
    
    return (out);
}

/*!
   transpose elements of permutation
   -- Really multiplying a permutation by a transposition
 */
_xperm *_xperm_transp(_xperm *px, unsigned int i1, unsigned int i2){
    unsigned int temp;
    
    if (px==(_xperm *)NULL)
        _xerror(_XE_NULL,"_xperm_transp");
    
    if (i1 < px->size && i2 < px->size){
        temp = px->pe[i1];
        px->pe[i1] = px->pe[i2];
        px->pe[i2] = temp;
    }
    
    return (px);
}

/*!
   a cheap implementation of Quicksort on integers
   -- returns number of swaps
 */
static int _xqsort(int *a, int num){
    int i = 0, numswaps = 0, j, v;
    if (num <= 1) return (0);
    
    j = num; v = a[0];
    for (;;){
        while (a[++i] < v);
        while (a[--j] > v);
        if (i >= j) break;
        a[i] ^= a[j] ^= a[i] ^= a[j];
        numswaps++;
    }
    
    a[0] ^= a[j] ^= a[0] ^= a[j];
    if (j != 0) numswaps++;
    
    numswaps += _xqsort(&a[0],j);
    numswaps += _xqsort(&a[j+1],num-(j+1));
    
    return (numswaps);
}

/*!
   compute the ``sign'' of a permutation = +/-1 where
   px is the product of an even/odd # transpositions
 */
int _xperm_sign(const _xperm *px){
    int numtransp;
    _xperm *px2;
    
    if (px==(_xperm *)NULL)
        _xerror(_XE_NULL,"_xperm_sign");
    px2 = _xperm_copy(px,(_xperm *)NULL);
    numtransp = _xqsort((int *)px2->pe,px2->size);
    _xperm_free(px2);
    
    return ( numtransp % 2 ) ? -1 : 1;
}

/*!
   permute vector
 */
_xvec *_xperm_vec(_xperm *px, const _xvec *vector, _xvec *out){
    unsigned int old_i, i, size, start;
    _xreal tmp;
    
    if (!px || !vector) _xerror(_XE_NULL,"_xperm_vec");
    if (px->size > vector->dim) _xerror(_XE_SIZES,"_xperm_vec");
    if (!out || out->dim < vector->dim)
        out = _xvec_resize(out,vector->dim);
    
    size = px->size;
    if (size == 0) return _xvec_vcopy(vector,out);
    if (out != vector){
        for (i=0; i<size; i++)
            if (px->pe[i] >= size)
                _xerror(_XE_BOUNDS,"_xperm_vec");
            else
                out->ve[i] = vector->ve[px->pe[i]];
    }
    else {/* in situ algorithm */
        start = 0;
        while (start < size){
            old_i = start;
            i = px->pe[old_i];
            if (i >= size){
                start++;
                continue;
            }
            tmp = vector->ve[start];
            while (_XTRUE){
                vector->ve[old_i] = vector->ve[i];
                px->pe[old_i] = i+size;
                old_i = i;
                i = px->pe[old_i];
                if (i >= size) break;
                if (i == start){
                    vector->ve[old_i] = tmp;
                    px->pe[old_i] = i+size;
                    break;
                }
            }
            start++;
        }
        for (i = 0; i < size; i++)
            if (px->pe[i] < size)
                _xerror(_XE_BOUNDS,"_xperm_vec");
            else
                px->pe[i] = px->pe[i]-size;
    }
    
    return (out);
}

/*!
   apply the inverse of px to x, returning the result in out
 */
_xvec *_xperm_inv_vec(_xperm *px, const _xvec *x, _xvec *out){
    unsigned int i, size;
    
    if (!px || !x) _xerror(_XE_NULL,"_xperm_inv_vec");
    if (px->size > x->dim) _xerror(_XE_SIZES,"_xperm_inv_vec");
    /*if (x == out) _xerror(_XE_INSITU,"_xperm_inv_vec"); */
    if (!out || out->dim < x->dim)
        out = _xvec_resize(out,x->dim);
    
    size = px->size;
    if (size == 0) return _xvec_vcopy(x,out);
    if (out != x){
        for (i=0; i<size; i++)
            if (px->pe[i] >= size)
                _xerror(_XE_BOUNDS,"_xperm_inv_vec");
            else
                out->ve[px->pe[i]] = x->ve[i];
    }
    else {/* in situ algorithm --- cheat's way out */
        _xperm_inv(px,px);
        _xperm_vec(px,x,out);
        _xperm_inv(px,px);
    }

    return (out);
}

/*!
   permute columns of matrix A; out = A.px'
  -- May NOT be in situ
 */
_xmat *_xperm_cols(const _xperm *px, const _xmat *A, _xmat *out){
    int i, j, m, n, px_j;
    _xreal **A_me, **out_me;

    if (!A || !px) _xerror(_XE_NULL,"_xperm_cols");
    if (px->size != A->n) _xerror(_XE_SIZES,"_xperm_cols");
    if (A == out) _xerror(_XE_INSITU,"_xperm_cols");
    
    m = A->m; n = A->n;
    if (!out || out->m != m || out->n != n)
        out = _xmat_get(m,n);
    A_me = A->me; out_me = out->me;
    
    for (j = 0; j < n; j++){
        px_j = px->pe[j];
        if (px_j >= n)
            _xerror(_XE_BOUNDS,"_xperm_cols");
        for (i = 0; i < m; i++)
            out_me[i][px_j] = A_me[i][j];
    }
    
    return (out);
}

/*!
    permute columns of matrix A; out = px.A
  -- May NOT be in situ
 */
_xmat *_xperm_rows(const _xperm *px, const _xmat *A, _xmat *out){
    int i, j, m, n, px_i;
    _xreal **A_me, **out_me;
    
    if (!A || !px) _xerror(_XE_NULL,"_xperm_rows");
    if (px->size != A->m) _xerror(_XE_SIZES,"_xperm_rows");
    if (A == out) _xerror(_XE_INSITU,"_xperm_rows");
    
    m = A->m; n = A->n;
    if (!out || out->m != m || out->n != n)
        out = _xmat_get(m,n);
    A_me = A->me; out_me = out->me;
    
    for (i = 0; i < m; i++){
        px_i = px->pe[i];
        if (px_i >= m) _xerror(_XE_BOUNDS,"_xperm_rows");
        for (j = 0; j < n; j++)
            out_me[i][j] = A_me[px_i][j];
    }
    
    return (out);
}

/*!
    print a dump of all pointers and data in a onto fp
  -- suitable for low-level debugging
 */
void _xperm_dump(FILE *fp, const _xperm *px){
    unsigned int i;
     
    if (!px){
        (void) fprintf(fp,"Permutation: NULL\n");
        return;
    }
    (void) fprintf(fp,"Permutation: size: %u @ 0x%lx\n",px->size,(long)(px));
    if (!px->pe){
        fprintf(fp,"NULL\n");
        return;
    }
    (void) fprintf(fp,"px->pe @ 0x%lx\n",(long)(px->pe));
    for (i=0; i<px->size; i++)
        (void) fprintf(fp,"%u->%u ",i,px->pe[i]);
    
    (void) fprintf(fp,"\n");
}

/*!
   print a representation of px onto file/stream fp
 */
void _xperm_foutput(FILE *fp, const _xperm *px){
    unsigned int i;
     
    if (!px){
        (void) fprintf(fp,"Permutation: NULL\n");
        return;
    }
    (void) fprintf(fp,"Permutation: size: %u\n",px->size);
    if (px->pe == (unsigned int *)NULL){
        (void) fprintf(fp,"NULL\n");
        return;
    }
    for (i=0; i<px->size; i++)
        if (!(i % 8) && i != 0)
            (void) fprintf(fp,"\n  %u->%u ",i,px->pe[i]);
        else
            (void) fprintf(fp,"%u->%u ",i,px->pe[i]);
    (void) fprintf(fp,"\n");
}

/*!
   input permutation from file/stream fp
  -- input from a terminal is handled interactively
  -- batch/file input has the same format as produced by px_foutput
  except that whitespace and comments ("#..\n") are skipped
  -- returns px, which is created if px == NULL on entry
 */
_xperm *_xperm_finput(FILE *fp, _xperm *px){
    _xperm *_xipx_finput(), *_xbpx_finput();
    return (isatty(_fileno(fp)) ? _xipx_finput(fp,px) :
                                  _xbpx_finput(fp,px));
}

/*!
   interactive input of permutation
 */
_xperm *_xipx_finput(FILE *fp, _xperm *px){
    unsigned int i,j,size,dynamic; /* dynamic set if memory allocated here */
    unsigned int entry,ok;
     
    /* get permutation size */
    if (px && px->size<_XMAXDIM){
        size = px->size;
        dynamic = _XFALSE;        
    }
    else {
        dynamic = _XTRUE;
        do {
            (void) fprintf(stderr,"Permutation: size: ");
            if (fgets(_xline,_XMAXLINE,fp)==NULL )
                _xerror(_XE_INPUT,"_xipx_finput");
        } while (sscanf(_xline,"%u",&size)<1 || size>_XMAXDIM);
        px = _xperm_get(size);
    }
     
    /* get entries */
    i = 0;
    while (i<size){/* input entry */
        do {
        redo:
            (void) fprintf(stderr,"entry %u: ",i);
            if (!dynamic)
                (void) fprintf(stderr,"old: %u->%u new: ",i,px->pe[i]);
            if (fgets(_xline,_XMAXLINE,fp)==NULL) _xerror(_XE_INPUT,"_xipx_finput");
            if ((*_xline == 'b' || *_xline == 'B') && i > 0){
                i--;
                dynamic = _XFALSE;
                goto redo;
            }
        } while (*_xline=='\0' || sscanf(_xline,"%u",&entry) < 1);
        /* check entry */
        ok = (entry < size);
        for (j=0; j<i; j++) ok &= (entry != px->pe[j]);
        if (ok){
            px->pe[i] = entry;
            i++;
        }
    }
     
    return (px);
}

/*!
   batch-file input of permutation
 */
_xperm *_xbpx_finput(FILE *fp, _xperm *px){
    unsigned int i,j,size,entry,ok;
    int io_code;
     
    /* get size of permutation */
    _xskipjunk(fp);
    if ((io_code=fscanf(fp," Permutation: size:%u",&size)) < 1 || size>_XMAXDIM)
        _xerror(io_code==EOF ? 7 : 6,"_xbpx_finput");
     
    /* allocate memory if necessary */
    if (!px || px->size<size)
        px = _xperm_resize(px,size);
     
    /* get entries */
    _xskipjunk(fp);
    i = 0;
    while (i<size){/* input entry */
        if ((io_code=fscanf(fp,"%*u -> %u",&entry)) < 1)
            _xerror(io_code==EOF ? 7 : 6,"_xbpx_finput");
        /* check entry */
        ok = (entry < size);
        for (j=0; j<i; j++) ok &= (entry != px->pe[j]);
        if (ok){
            px->pe[i] = entry;
            i++;
        }   
        else
            _xerror(_XE_BOUNDS,"_xbpx_finput");
    }
     
    return (px);
}

/*!
    get an mxn matrix (in _xmat form) by dynamic memory allocation
  -- normally ALL matrices should be obtained this way
  -- if either m or n is negative this will raise an error
  -- note that 0 x n and m x 0 matrices can be created
 */
_xmat *_xmat_get(int m, int n){
    _xmat *matrix;
    int i;
   
    if (m < 0 || n < 0) _xerror(_XE_NEG,"_xmat_get");
    if ((matrix=_xnew(_xmat)) == (_xmat *)NULL)
        _xerror(_XE_MEM,"_xmat_get");
    else if (_xmem_info_is_on()){
        _xmem_bytes(_XTYPE_MAT,0,sizeof(_xmat));
        _xmem_numvar(_XTYPE_MAT,1);
    }
   
    matrix->m = matrix->max_m = m; 
    matrix->n = matrix->max_n = n;
    matrix->max_size = m*n;

#ifndef _XSEGMENTED
    if ((matrix->base = _xnew_a(m*n,_xreal)) == (_xreal *)NULL){
        _xfree(matrix);
        _xerror(_XE_MEM,"_xmat_get");
    }
    else if (_xmem_info_is_on()) {
        _xmem_bytes(_XTYPE_MAT,0,m*n*sizeof(_xreal));
    }
#else
    matrix->base = (_xreal *)NULL;
#endif
    if ((matrix->me = (_xreal **)_xcalloc(m,sizeof(_xreal *))) == (_xreal **)NULL){
        _xfree(matrix->base); 
        _xfree(matrix);
        _xerror(_XE_MEM,"_xmat_get");
    }
    else if (_xmem_info_is_on()){
        _xmem_bytes(_XTYPE_MAT,0,m*sizeof(_xreal *));
    }  
#ifndef _XSEGMENTED
    /* set up pointers */
    for (i=0; i<m; i++)
        matrix->me[i] = &(matrix->base[i*n]);
#else
    for (i = 0; i < m; i++)
        if ((matrix->me[i]=_xnew_a(n,_xreal)) == (_xreal *)NULL)
            _xerror(_XE_MEM,"_xmat_get");
        else if (_xmem_info_is_on()){
            _xmem_bytes(_XTYPE_MAT,0,n*sizeof(_xreal));
        }
#endif
    return (matrix);
}

/*!
   zero the matrix A
 */
_xmat *_xmat_zero(_xmat *A){
    int i, A_m, A_n;
    _xreal **A_me;
    
    if (!A) _xerror(_XE_NULL,"_xmat_zero");
    
    A_m = A->m; A_n = A->n; A_me = A->me;
    for (i = 0; i < A_m; i++)
        __xzero__(A_me[i],A_n);
    
    return (A);
}

/*!
   fill matrix with one's
 */
_xmat *_xmat_ones(_xmat *A){
    unsigned int i, j;

    if (!A) _xerror(_XE_NULL,"_xmat_ones");
    
    for (i = 0; i < A->m; i++)
        for (j = 0; j < A->n; j++)
            A->me[i][j] = (_xreal)1.0;
    
    return (A);
}

/*!
   initialize A to be a random vector, components
   independently & uniformly distributed between 0 and 1
 */
_xmat *_xmat_rand(_xmat *A){
    unsigned int i;

    if (!A) _xerror(_XE_NULL,"_xmat_rand");
    for (i = 0; i < A->m; i++)
        _xmrandlist(A->me[i],A->n);
    
    return (A);
}

/*!
    copy matrix into new area
    -- out(i0:m,j0:n) <- in(i0:m,j0:n)
 */
_xmat *_xmat_copy(const _xmat *in, _xmat *out, unsigned int i0, unsigned int j0){
    unsigned int i;

    if (!in) _xerror(_XE_NULL,"_xmat_copy");
    if (in==out) return (out);
    if (!out || out->m < in->m || out->n < in->n)
        out = _xmat_resize(out,in->m,in->n);
    
    for (i=i0; i < in->m; i++)
        _xmem_copy(&(in->me[i][j0]),&(out->me[i][j0]),
                  (in->n - j0)*sizeof(_xreal));
    
    return (out);
}

/*!
    copy selected pieces of a matrix
    -- moves the m0 x n0 submatrix with top-left cor-ordinates (i0,j0)
       to the corresponding submatrix of out with top-left co-ordinates
       (i1,j1)
    -- out is resized (& created) if necessary
 */
_xmat *_xmat_move(const _xmat *in, int i0, int j0, int m0, int n0, _xmat *out, int i1, int j1){
    int i;

    if (!in) _xerror(_XE_NULL,"_xmat_move");
    if (i0 < 0 || j0 < 0 || i1 < 0 || j1 < 0 || m0 < 0 || n0 < 0 || 
       (unsigned int)(i0+m0) > in->m || 
       (unsigned int)(j0+n0) > in->n)
        _xerror(_XE_BOUNDS,"_xmat_move");

    if (!out)
        out = _xmat_resize(out,i1+m0,j1+n0);
    else if ((unsigned int)(i1+m0) > out->m || (unsigned int)(j1+n0) > out->n)
        out = _xmat_resize(out,_xmax(out->m,(unsigned int)(i1+m0)),_xmax(out->n,(unsigned int)(j1+n0)));

    for (i = 0; i < m0; i++)
        _xmem_copy(&(in->me[i0+i][j0]),&(out->me[i1+i][j1]),n0*sizeof(_xreal));

    return (out);
}

/*!
    set A to being closest to identity matrix as possible
    -- i.e. A[i][j] == 1 if i == j and 0 otherwise
 */
_xmat *_xmat_ident(_xmat *A){
    int i, size;

    if (!A) _xerror(_XE_NULL,"_xmat_ident");
    
    _xmat_zero(A);
    size = _xmin(A->m,A->n);
    for (i = 0; i < size; i++)
        A->me[i][i] = (_xreal)1.0;
    
    return (A);
}

/*!
   return the matrix A of size new_m x new_n; A is zeroed
   -- if A == NULL on entry then the effect is equivalent to _xmat_get()
 */
_xmat *_xmat_resize(_xmat *A, int new_m, int new_n){
    int i;
    int new_max_m, new_max_n, new_size, old_m, old_n;
   
    if (new_m < 0 || new_n < 0) _xerror(_XE_NEG,"_xmat_resize");
    if (!A) return _xmat_get(new_m,new_n);
    if (new_m == A->m && new_n == A->n) return A; /* nothing was changed */

    old_m = A->m; old_n = A->n;
    if ((unsigned int)new_m > A->max_m){ /* re-allocate A->me */
        if (_xmem_info_is_on()){
            _xmem_bytes(_XTYPE_MAT,A->max_m*sizeof(_xreal *),new_m*sizeof(_xreal *));
        }
        A->me = _xrenew(A->me,new_m,_xreal *);
        if (!A->me) _xerror(_XE_MEM,"_xmat_resize");
    }
    new_max_m = _xmax((unsigned int)new_m,A->max_m);
    new_max_n = _xmax((unsigned int)new_n,A->max_n);
   
#ifndef _XSEGMENTED
    new_size = new_max_m*new_max_n;
    if ((unsigned int)new_size > A->max_size){/* re-allocate A->base */
        if (_xmem_info_is_on()){
            _xmem_bytes(_XTYPE_MAT,A->max_m*A->max_n*sizeof(_xreal),new_size*sizeof(_xreal));
        }

        A->base = _xrenew(A->base,new_size,_xreal);
        if (!A->base) _xerror(_XE_MEM,"_xmat_resize");
        A->max_size = new_size;
    }
   
    /* now set up A->me[i] */
    for (i = 0; i < new_m; i++)
        A->me[i] = &(A->base[i*new_n]);
   
    /* now shift data in matrix */
    if (old_n > new_n){
        for (i = 1; i < _xmin(old_m,new_m); i++)
            _xmem_copy((char *)&(A->base[i*old_n]),
                       (char *)&(A->base[i*new_n]),
                        sizeof(_xreal)*new_n);
    }
    else if (old_n < new_n){
        for (i = (int)(_xmin(old_m,new_m))-1; i > 0; i--){
        /* copy & then zero extra space */
            _xmem_copy((char *)&(A->base[i*old_n]),
                       (char *)&(A->base[i*new_n]),
                        sizeof(_xreal)*old_n);
            __xzero__(&(A->base[i*new_n+old_n]),(new_n-old_n));
        }
        __xzero__(&(A->base[old_n]),(new_n-old_n));
        A->max_n = new_n;
    }
    /* zero out the new rows.. */
    for (i = old_m; i < new_m; i++)
        __xzero__(&(A->base[i*new_n]),new_n);
#else
    if (A->max_n < new_n){
        _xreal *tmp;
        for (i = 0; i < A->max_m; i++){
            if (_xmem_info_is_on()){
                _xmem_bytes(_XTYPE_MAT,A->max_n*sizeof(_xreal),new_max_n*sizeof(_xreal));
            } 

            if ((tmp = _xrenew(A->me[i],new_max_n,_xreal)) == NULL)
                _xerror(_XE_MEM,"_xmat_resize");
            else {  
                A->me[i] = tmp;
            }
        }
        for (i = A->max_m; i < new_max_m; i++){
            if ((tmp = _xnew_a(new_max_n,_xreal)) == NULL)
                _xerror(_XE_MEM,"_xmat_resize");
            else {
                A->me[i] = tmp;
                if (_xmem_info_is_on()){
                    _xmem_bytes(_XTYPE_MAT,0,new_max_n*sizeof(_xreal));
                }     
            }
        }
    }
    else if (A->max_m < new_m){
        for (i = A->max_m; i < new_m; i++) 
            if ((A->me[i] = _xnew_a(new_max_n,_xreal)) == NULL)
                _xerror(_XE_MEM,"_xmat_resize");
            else if (_xmem_info_is_on()){
                _xmem_bytes(_XTYPE_MAT,0,new_max_n*sizeof(_xreal));
            }
    }
   
    if (old_n < new_n){
        for (i = 0; i < old_m; i++)
            __xzero__(&(A->me[i][old_n]),new_n-old_n);
    }
   
    /* zero out the new rows.. */
    for (i = old_m; i < new_m; i++)
        __xzero__(A->me[i],new_n);
#endif
   
    A->max_m = new_max_m;
    A->max_n = new_max_n;
    A->max_size = A->max_m*A->max_n;
    A->m = new_m;
    A->n = new_n;
   
    return (A);
}

/*!
    print a dump of all pointers and data in a onto fp
    -- suitable for low-level debugging
 */
void _xmat_dump(FILE *fp,const _xmat *a){
    unsigned int i, j, tmp;
     
    if (!a){
        (void) fprintf(fp,"Matrix: NULL\n");
        return;
    }
    (void) fprintf(fp,"Matrix: %d by %d @ 0x%lx\n",a->m,a->n,(long)a);
    (void) fprintf(fp,"\tmax_m = %d, max_n = %d, max_size = %d\n",
                   a->max_m, a->max_n, a->max_size);
    if (a->me == (_xreal **)NULL){
        (void) fprintf(fp,"NULL\n");
        return;
    }
    (void) fprintf(fp,"a->me @ 0x%lx\n",(long)(a->me));
    (void) fprintf(fp,"a->base @ 0x%lx\n",(long)(a->base));
    for (i=0; i<a->m; i++){/* for each row... */
        (void) fprintf(fp,"row %u: @ 0x%lx ",i,(long)(a->me[i]));
        for (j=0, tmp=2; j<a->n; j++, tmp++){ /* for each col in row... */
            (void) fprintf(fp,_xformat,a->me[i][j]);
            if (!(tmp % 5)) putc('\n',fp);
        }
        if (tmp % 5 != 1) putc('\n',fp);
    }
}

/*!
   print a representation of the matrix a onto file/stream fp
 */
void _xmat_foutput(FILE *fp, const _xmat *a){
    unsigned int i, j, tmp;
     
    if (a == (_xmat *)NULL){
        (void) fprintf(fp,"Matrix: NULL\n");
        return;
    }
    (void) fprintf(fp,"Matrix: %d by %d\n",a->m,a->n);
    if (a->me == (_xreal **)NULL){
        (void) fprintf(fp,"NULL\n");
        return;
    }
    for (i=0; i<a->m; i++){/* for each row... */
        (void) fprintf(fp,"row %u: ",i);
        for (j=0, tmp=2; j<a->n; j++, tmp++){ /* for each col in row... */
            (void) fprintf(fp,_xformat,a->me[i][j]);
            if (!(tmp % 5)) putc('\n',fp);
        }
        if (tmp % 5 != 1) putc('\n',fp);
    }
}

/*!
    input matrix
    -- input from a terminal is handled interactively
    -- batch/file input has the same format as produced by m_foutput
    except that whitespace and comments ("#..\n") are skipped
    -- returns a, which is created if a == NULL on entry
 */
_xmat *_xmat_finput(FILE *fp,_xmat *a){
    _xmat *_xim_finput(),*_xbm_finput();
    return (isatty(_fileno(fp))? _xim_finput(fp,a) :
                                 _xbm_finput(fp,a));
}

/*!
   interactive input of matrix
 */
_xmat *_xim_finput(FILE *fp,_xmat *mat){
    char c;
    unsigned int i, j, m, n, dynamic;
    /* dynamic set to _XTRUE if memory allocated here */
     
    /* get matrix size */
    if (mat && mat->m<_XMAXDIM && mat->n<_XMAXDIM){
        m = mat->m;
        n = mat->n;
        dynamic = _XFALSE;
    }
    else {
        dynamic = _XTRUE;
        do {
            (void) fprintf(stderr,"Matrix: rows cols:");
            if (fgets(_xline,_XMAXLINE,fp)==NULL)
                _xerror(_XE_INPUT,"_xim_finput");
        } while (sscanf(_xline,"%u%u",&m,&n)<2 || m>_XMAXDIM || n>_XMAXDIM);
        mat = _xmat_get(m,n);
    }
     
    /* input elements */
    for (i=0; i<m; i++){
    redo:
        (void) fprintf(stderr,"row %u:\n",i);
        for (j=0; j<n; j++)
            do {
            redo2:
                (void) fprintf(stderr,"entry (%u,%u): ",i,j);
                if (!dynamic)
                    (void) fprintf(stderr,"old %14.9g new: ",mat->me[i][j]);
                if (fgets(_xline,_XMAXLINE,fp)==NULL)
                    _xerror(_XE_INPUT,"_xim_finput");
                if ((*_xline == 'b' || *_xline == 'B') && j > 0){
                    j--;
                    dynamic = _XFALSE;
                    goto redo2;
                }
                if ((*_xline == 'f' || *_xline == 'F') && j < n-1){
                    j++;
                    dynamic = _XFALSE;
                    goto redo2;
                }
#if _XREAL == _XDOUBLE
            } while (*_xline=='\0' || sscanf(_xline,"%lf",&mat->me[i][j])<1);
#elif _XREAL == _XFLOAT
            } while (*_xline=='\0' || sscanf(_xline,"%f",&mat->me[i][j])<1);
#endif
        (void) fprintf(stderr,"Continue: ");
        (void) fscanf(fp,"%c",&c);
        if (c == 'n' || c == 'N'){
            dynamic = _XFALSE;
            goto redo;
        }
        if ((c == 'b' || c == 'B') /* && i > 0 */ ){
            if (i > 0) i--;
            dynamic = _XFALSE;
            goto redo;
        }
    }
     
    return (mat);
}

/*!
   batch-file input of matrix
 */
_xmat *_xbm_finput(FILE *fp,_xmat *mat){
    unsigned int i,j,m,n,dummy;
    int io_code;
     
    /* get dimension */
    _xskipjunk(fp);
    if ((io_code=fscanf(fp," Matrix: %u by %u",&m,&n)) < 2 || m>_XMAXDIM || n>_XMAXDIM)
        _xerror(io_code==EOF ? _XE_EOF : _XE_FORMAT,"_xbm_finput");
     
    /* allocate memory if necessary */
    if (!mat) mat = _xmat_resize(mat,m,n);
     
    /* get entries */
    for (i=0; i<m; i++){
        _xskipjunk(fp);
        if (fscanf(fp," row %u:",&dummy) < 1)
            _xerror(_XE_FORMAT,"_xbm_finput");
        for (j=0; j<n; j++)
#if _XREAL == _XDOUBLE
            if ((io_code=fscanf(fp,"%lf",&mat->me[i][j])) < 1)
#elif _XREAL == _XFLOAT
            if ((io_code=fscanf(fp,"%f",&mat->me[i][j])) < 1)
#endif
                 _xerror(io_code==EOF ? 7 : 6,"_xbm_finput");
    }
     
    return (mat);
}

int _xmat_get_vars(int m, int n, ...){
    va_list ap;
    int i=0;
    _xmat **par;
   
    va_start(ap, n);
    while (par = va_arg(ap,_xmat **)){ /* NULL ends the list*/
        *par = _xmat_get(m,n);
        i++;
    } 
    va_end(ap);

    return (i);
}

int _xmat_resize_vars(int m, int n, ...){
    va_list ap;
    int i=0;
    _xmat **par;
   
    va_start(ap, n);
    while (par = va_arg(ap,_xmat **)){/* NULL ends the list*/
        *par = _xmat_resize(*par,m,n);
        i++;
    } 
    va_end(ap);

    return (i);
}

int _xmat_free_vars(_xmat **va, ...){
    va_list ap;
    int i=1;
    _xmat **par;
   
    _xmat_free(*va);
    *va = (_xmat *)NULL;
    va_start(ap, va);
    while (par = va_arg(ap,_xmat **)){/* NULL ends the list*/
        _xmat_free(*par); 
        *par = (_xmat *)NULL;
        i++;
    } 
    va_end(ap);

    return (i);
}

/*!
   compute matrix 1-norm -- unscaled
 */
double _xmat_norm1(const _xmat *A){
    unsigned int i, j, m, n;
    _xreal maxval, sum;

    if (!A) _xerror(_XE_NULL,"_xmat_norm1");

    m = A->m; n = A->n;
    maxval = (_xreal)0.0;
    
    for (j = 0; j < n; j++){
        sum = (_xreal)0.0;
        for (i = 0; i < m; i++)
            sum += (_xreal)fabs(A->me[i][j]);
        maxval = _xmax(maxval,sum);
    }
    
    return (double)(maxval);
}

/*!
   compute matrix infinity-norm -- unscaled
 */
double _xmat_norm_inf(const _xmat *A){
    unsigned int i, j, m, n;
    _xreal maxval, sum;

    if (!A) _xerror(_XE_NULL,"_xmat_norm_inf");
    
    m = A->m; n = A->n;
    maxval = (_xreal)0.0;
    
    for (i = 0; i < m; i++){
        sum = (_xreal)0.0;
        for (j = 0; j < n; j++)
            sum += (_xreal)fabs(A->me[i][j]);
        maxval = _xmax(maxval,sum);
    }
    
    return (double)(maxval);
}

/*!
   compute matrix frobenius-norm -- unscaled
 */
double _xmat_norm_frob(const _xmat *A){
    unsigned int i, j, m, n;
    _xreal sum;
    
    if (!A) _xerror(_XE_NULL,"_xmat_norm_frob");
    
    m = A->m; n = A->n;
    sum = (_xreal)0.0;
    
    for (i = 0; i < m; i++)
        for (j = 0; j < n; j++)
            sum += (_xreal)_xsquare(A->me[i][j]);
    
    return sqrt(sum);
}

/*!
   matrix addition -- may be in-situ
 */
_xmat *_xmat_add(const _xmat *mat1, const _xmat *mat2, _xmat *out){
    unsigned int m,n,i;

    if (!mat1 || !mat2) _xerror(_XE_NULL,"_xmat_add");
    if (mat1->m != mat2->m || mat1->n != mat2->n) 
        _xerror(_XE_SIZES,"_xmat_add");
    if (!out || out->m != mat1->m || out->n != mat1->n)
        out = _xmat_resize(out,mat1->m,mat1->n);
    m = mat1->m; n = mat1->n;
    for (i=0; i<m; i++){
        __xadd__(mat1->me[i],mat2->me[i],out->me[i],(int)n);
    }
    
    return (out);
}

/*!
   matrix subtraction -- may be in-situ
 */
_xmat *_xmat_sub(const _xmat *mat1, const _xmat *mat2, _xmat *out){
    unsigned int m,n,i;

    if (!mat1 || !mat2) _xerror(_XE_NULL,"_xmat_sub");
    if (mat1->m != mat2->m || mat1->n != mat2->n)
        _xerror(_XE_SIZES,"_xmat_sub");
    if (!out || out->m != mat1->m || out->n != mat1->n)
        out = _xmat_resize(out,mat1->m,mat1->n);
    m = mat1->m; n = mat1->n;
    for (i=0; i<m; i++){
        __xsub__(mat1->me[i],mat2->me[i],out->me[i],(int)n);
    }
    
    return (out);
}

/*!
   matrix-matrix multiplication
 */
_xmat *_xmat_mlt(const _xmat *mat1, const _xmat *mat2, _xmat *out){
    unsigned int i,k, m, n, p;
    _xreal **A_v, **B_v;
    
    if (!mat1 || !mat2) _xerror(_XE_NULL,"_xmat_mlt");
    if (mat1->n != mat2->m) _xerror(_XE_SIZES,"_xmat_mlt");
    if (mat1 == out || mat2 == out)
        _xerror(_XE_INSITU,"_xmat_mlt");
    m = mat1->m; n = mat1->n; p = mat2->n;
    A_v = mat1->me; B_v = mat2->me;
    
    if (!out || out->m != mat1->m || out->n != mat2->n)
        out = _xmat_resize(out,mat1->m,mat2->n);
    
    _xmat_zero(out);
    for (i=0; i<m; i++)
        for (k=0; k<n; k++){
            if (A_v[i][k] != (_xreal)0.0)
                __xmltadd__(out->me[i],B_v[k],A_v[i][k],(int)p);
        }
    
    return (out);
}

/*!
    matrix-matrix transposed multiplication
  -- A.B^T is returned, and stored in out
 */
_xmat *_xmat_mtr_mlt(const _xmat *A, const _xmat *B, _xmat *out){
    unsigned int i, j, limit;

    if (!A || !B) _xerror(_XE_NULL,"_xmat_mtr_mlt");
    if (A == out || B == out)
        _xerror(_XE_INSITU,"_xmat_mtr_mlt");
    if (A->n != B->n)
        _xerror(_XE_SIZES,"_xmat_mtr_mlt");
    if (!out || out->m != A->m || out->n != B->m)
        out = _xmat_resize(out,A->m,B->m);
    
    limit = A->n;
    for (i = 0; i < A->m; i++)
        for (j = 0; j < B->m; j++){
            out->me[i][j] = (_xreal)__xip__(A->me[i],B->me[j],(int)limit);
        }
    
    return (out);
}

/*!
   matrix transposed-matrix multiplication
   -- A^T.B is returned, result stored in out
 */
_xmat *_xmat_trm_mlt(const _xmat *A, const _xmat *B, _xmat *out){
    unsigned int i, k, limit;

    if (!A || !B) _xerror(_XE_NULL,"_xmat_trm_mlt");
    if (A == out || B == out)
        _xerror(_XE_INSITU,"_xmat_trm_mlt");
    if (A->m != B->m)
        _xerror(_XE_SIZES,"_xmat_trm_mlt");
    if (!out || out->m != A->n || out->n != B->n)
        out = _xmat_resize(out,A->n,B->n);
    
    limit = B->n;
    _xmat_zero(out);
    for (k = 0; k < A->m; k++)
        for (i = 0; i < A->n; i++){
            if (A->me[k][i] != (_xreal)0.0)
                __xmltadd__(out->me[i],B->me[k],A->me[k][i],(int)limit);
        }
    
    return (out);
}

/*!
   transpose matrix
 */
_xmat *_xmat_transp(const _xmat *in, _xmat *out){
    unsigned int i, j;
    int in_situ;
    _xreal tmp;
    
    if (!in) _xerror(_XE_NULL,"_xmat_transp");
    if (in == out && in->n != in->m)
        _xerror(_XE_INSITU2,"_xmat_transp");
    in_situ = ( in == out );
    if (!out || out->m != in->n || out->n != in->m)
        out = _xmat_resize(out,in->n,in->m);
    
    if (!in_situ)
        for (i = 0; i < in->m; i++)
            for (j = 0; j < in->n; j++)
                out->me[j][i] = in->me[i][j];
    else
        for (i = 1; i < in->m; i++)
            for (j = 0; j < i; j++){
                tmp = in->me[i][j];
                in->me[i][j] = in->me[j][i];
                in->me[j][i] = tmp;
            }
    
    return (out);
}

/*!
   scalar-matrix multiply -- may be in-situ
 */
_xmat *_xmat_sm_mlt(double scalar, const _xmat *matrix, _xmat *out){
    unsigned int m,n,i;

    if (!matrix) _xerror(_XE_NULL,"_xmat_sm_mlt");
    if (!out || out->m != matrix->m || out->n != matrix->n)
        out = _xmat_resize(out,matrix->m,matrix->n);
    m = matrix->m; n = matrix->n;
    for (i=0; i<m; i++)
        __xsmlt__(matrix->me[i],(double)scalar,out->me[i],(int)n);
    
    return (out);
}

/*!
    matrix-vector multiplication 
  -- Note: b is treated as a column vector
 */
_xvec *_xmat_mv_mlt(const _xmat *A, const _xvec *b, _xvec *out){
    unsigned int i, m, n;
    _xreal **A_v, *b_v;
    
    if (!A || !b) _xerror(_XE_NULL,"_xmat_mv_mlt");
    if (A->n != b->dim)
        _xerror(_XE_SIZES,"_xmat_mv_mlt");
    if (b == out)
        _xerror(_XE_INSITU,"_xmat_mv_mlt");
    if (!out || out->dim != A->m)
        out = _xvec_resize(out,A->m);
    
    m = A->m; n = A->n;
    A_v = A->me; b_v = b->ve;
    for (i=0; i<m; i++){
        out->ve[i] = (_xreal)__xip__(A_v[i],b_v,(int)n);
    }
    
    return (out);
}

/*!
   vector-matrix multiplication 
   -- Note: b is treated as a row vector
 */
_xvec *_xmat_vm_mlt(const _xmat *A, const _xvec *b, _xvec *out){
    unsigned int j,m,n;

    if (!A || !b) _xerror(_XE_NULL,"_xmat_vm_mlt");
    if (A->m != b->dim)
        _xerror(_XE_SIZES,"_xmat_vm_mlt");
    if (b == out)
        _xerror(_XE_INSITU,"_xmat_vm_mlt");
    if (!out || out->dim != A->n)
        out = _xvec_resize(out,A->n);
    
    m = A->m; n = A->n;
    
    _xvec_zero(out);
    for (j = 0; j < m; j++)
        if (b->ve[j] != (_xreal)0.0 )
            __xmltadd__(out->ve,A->me[j],b->ve[j],(int)n);
    
    return (out);
}

/*!
    matrix-scalar multiply and add
  -- may be in situ
  -- returns out == A1 + s*A2
 */
_xmat *_xmat_ms_mltadd(const _xmat *A1, const _xmat *A2, double s, _xmat *out){
    unsigned int i, m, n;

    if (!A1 || !A2)
        _xerror(_XE_NULL,"_xmat_ms_mltadd");
    if (A1->m != A2->m || A1->n != A2->n)
        _xerror(_XE_SIZES,"_xmat_ms_mltadd");
    
    if (out != A1 && out != A2)
        out = _xmat_resize(out,A1->m,A1->n);
    
    if (s == 0.0)
        return _xmat_mcopy(A1,out);
    if (s == 1.0)
        return _xmat_add(A1,A2,out);
    
    _xtracecatch(out = _xmat_mcopy(A1,out),"_xmat_ms_mltadd");
    
    m = A1->m;  n = A1->n;
    for (i = 0; i < m; i++){
        __xmltadd__(out->me[i],A2->me[i],s,(int)n);
    }
    
    return (out);
}

/*!
    matrix-vector multiply and add
  -- may not be in situ
  -- returns out == v1 + alpha*A*v2
 */
_xvec *_xmat_mv_mltadd(const _xvec *v1, const _xvec *v2, const _xmat *A, double alpha, _xvec *out){
    unsigned int i, m, n;
    _xreal *v2_ve, *out_ve;
    
    if (!v1 || !v2 || !A)
        _xerror(_XE_NULL,"_xmat_mv_mltadd");
    if (out == v2)
        _xerror(_XE_INSITU,"_xmat_mv_mltadd");
    if (v1->dim != A->m || v2->dim != A->n)
        _xerror(_XE_SIZES,"_xmat_mv_mltadd");
    
    _xtracecatch(out = _xvec_vcopy(v1,out),"_xmat_mv_mltadd");
    
    v2_ve = v2->ve; out_ve = out->ve;
    m = A->m; n = A->n;
    
    if (alpha == 0.0)
        return (out);
    
    for (i = 0; i < m; i++){
        out_ve[i] += (_xreal)(alpha*__xip__(A->me[i],v2_ve,(int)n));
    }
    
    return (out);
}

/*!
    vector-matrix multiply and add
  -- may not be in situ
  -- returns out' == v1' + v2'*A
 */
_xvec *_xmat_vm_mltadd(const _xvec *v1, const _xvec *v2, const _xmat *A, double alpha, _xvec *out){
    unsigned int j, m, n;
    _xreal tmp, *out_ve;
    
    if (!v1 || !v2 || !A)
        _xerror(_XE_NULL,"_xmat_vm_mltadd");
    if (v2 == out)
        _xerror(_XE_INSITU,"_xmat_vm_mltadd");
    if (v1->dim != A->n || A->m != v2->dim)
        _xerror(_XE_SIZES,"_xmat_vm_mltadd");
    
    _xtracecatch(out = _xvec_vcopy(v1,out),"_xmat_vm_mltadd");
    
    out_ve = out->ve; m = A->m; n = A->n;
    for (j = 0; j < m; j++){
        tmp = (_xreal)(v2->ve[j]*alpha);
        if (tmp != (_xreal)0.0)
            __xmltadd__(out_ve,A->me[j],tmp,(int)n);
    }
    
    return (out);
}

/*!
   get a specified column of a matrix and retruns it as a vector
 */
_xvec *_xmat_get_col(const _xmat *mat, unsigned int col, _xvec *vec){
    unsigned int i;
   
    if (!mat) _xerror(_XE_NULL,"_xmat_get_col");
    if (col >= mat->n) _xerror(_XE_RANGE,"_xmat_get_col");
    if (!vec || vec->dim<mat->m)
        vec = _xvec_resize(vec,mat->m);
   
    for (i=0; i<mat->m; i++)
        vec->ve[i] = mat->me[i][col];
   
    return (vec);
}

/*!
   get a specified row of a matrix and retruns it as a vector
 */
_xvec *_xmat_get_row(const _xmat *mat, unsigned int row, _xvec *vec){
    unsigned int i;
   
    if (!mat) _xerror(_XE_NULL,"_xmat_get_row");
    if (row >= mat->m ) _xerror(_XE_RANGE,"_xmat_get_row");
    if (!vec || vec->dim<mat->n)
        vec = _xvec_resize(vec,mat->n);
   
    for (i=0; i<mat->n; i++)
        vec->ve[i] = mat->me[row][i];
   
    return (vec);
}

/*!
   return sub-vector which is formed by the elements i1 to i2
   -- as for sub_mat, storage is shared
 */
_xvec *_xvec_sub_vec(const _xvec *old, int i1, int i2, _xvec *new_one){
    if (!old) _xerror(_XE_NULL,"_xvec_sub_vec");
    if (i1 > i2 || old->dim < (unsigned int)(i2)) _xerror(_XE_RANGE,"_xvec_sub_vec");
   
    if (!new_one) new_one = _xnew(_xvec);
    if (!new_one) _xerror(_XE_MEM,"_xvec_sub_vec");
    else if (_xmem_info_is_on()){
        _xmem_bytes(_XTYPE_VEC,0,sizeof(_xvec));
    }
   
    new_one->dim = i2 - i1 + 1;
    new_one->ve = &(old->ve[i1]);
   
    return (new_one);
}

/*!
   return sub-matrix of old which is formed by the rectangle
   from (row1,col1) to (row2,col2)
   -- Note: storage is shared so that altering the "new_one"
   matrix will alter the "old" matrix
 */
_xmat *_xmat_sub_mat(const _xmat *old, 
                     unsigned int row1, 
                     unsigned int col1, 
                     unsigned int row2, 
                     unsigned int col2, _xmat *new_one){
    unsigned int i;
   
    if (!old) _xerror(_XE_NULL,"_xmat_sub_mat");
    if (row1 > row2 || col1 > col2 || row2 >= old->m || col2 >= old->n)
        _xerror(_XE_RANGE,"_xmat_sub_mat");
    if (!new_one || new_one->m < row2-row1+1){
        new_one = _xnew(_xmat);/*???*/
        new_one->me = _xnew_a(row2-row1+1,_xreal *);
        if (!new_one || new_one->me == (_xreal **)NULL)
            _xerror(_XE_MEM,"_xmat_sub_mat");
        else if (_xmem_info_is_on()) {
            _xmem_bytes(_XTYPE_MAT,0,sizeof(_xmat)+(row2-row1+1)*sizeof(_xreal *));
        }
    }
    new_one->m = row2-row1+1;
    new_one->n = col2-col1+1;
    new_one->base = (_xreal *)NULL;
   
    for (i=0; i < new_one->m; i++)
        new_one->me[i] = (old->me[i+row1]) + col1;
   
    return (new_one);
}

/*!
    set column of matrix to values given in vec (in situ)
  -- that is, mat(i0:lim,col) <- vec(i0:lim)
 */
_xmat *_xmat_set_col(_xmat *mat, unsigned int col, const _xvec *vec, unsigned int i0){
    unsigned int i,lim;
   
    if (!mat || !vec) _xerror(_XE_NULL,"_xmat_set_col");
    if (col >= mat->n) _xerror(_XE_RANGE,"_xmat_set_col");
    lim = _xmin(mat->m,vec->dim);
    for (i=i0; i<lim; i++)
        mat->me[i][col] = vec->ve[i];
   
    return (mat);
}

/*!
   set row of matrix to values given in vec (in situ)
 */
_xmat *_xmat_set_row(_xmat *mat, unsigned int row, const _xvec *vec, unsigned int j0){
    unsigned int j,lim;
   
    if (!mat || !vec) _xerror(_XE_NULL,"_xmat_set_row");
    if (row >= mat->m) _xerror(_XE_RANGE,"_xmat_set_row");
    lim = _xmin(mat->n,vec->dim);
    for (j=j0; j<lim; j++)
        mat->me[row][j] = vec->ve[j];
   
    return (mat);
}

/*!
   swap rows i and j of matrix A for cols lo through hi
 */
_xmat *_xmat_swap_rows(_xmat *A, int i, int j, int lo, int hi){
    int k;
    _xreal **A_me, tmp;
    
    if (!A)
        _xerror(_XE_NULL,"_xmat_swap_rows");
    if (i < 0 || j < 0 || (unsigned int)i >= A->m || (unsigned int)j >= A->m)
        _xerror(_XE_SIZES,"_xmat_swap_rows");
    lo = _xmax(0,lo);
    hi = _xmin((unsigned int)hi,A->n-1);
    A_me = A->me;
    
    for (k = lo; k <= hi; k++){
        tmp = A_me[k][i];
        A_me[k][i] = A_me[k][j];
        A_me[k][j] = tmp;
    }
    return (A);
}

/*!
   swap columns i and j of matrix A for cols lo through hi
 */
_xmat *_xmat_swap_cols(_xmat *A, int i, int j, int lo, int hi){
    int k;
    _xreal **A_me, tmp;
    
    if (!A)
        _xerror(_XE_NULL,"_xmat_swap_cols");
    if (i < 0 || j < 0 || (unsigned int)i >= A->n || (unsigned int)j >= A->n)
        _xerror(_XE_SIZES,"_xmat_swap_cols");
    lo = _xmax(0,lo);
    hi = _xmin((unsigned int)hi,A->m-1);
    A_me = A->me;
    
    for (k = lo; k <= hi; k++){
        tmp = A_me[i][k];
        A_me[i][k] = A_me[j][k];
        A_me[j][k] = tmp;
    }
    
    return (A);
}

/*!
   get integer vector
 */
_xivec *_xivec_get(int dim){
    _xivec *iv;
   
    if (dim < 0) _xerror(_XE_NEG,"_xivec_get");

    if ((iv=_xnew(_xivec)) == (_xivec *)NULL)
        _xerror(_XE_MEM,"_xivec_get");
    else if (_xmem_info_is_on()){
        _xmem_bytes(_XTYPE_IVEC,0,sizeof(_xivec));
        _xmem_numvar(_XTYPE_IVEC,1);
    }
   
    iv->dim = iv->max_dim = dim;
    if ((iv->ive = _xnew_a(dim,int)) == (int *)NULL )
        _xerror(_XE_MEM,"_xivec_get");
    else if (_xmem_info_is_on()){
        _xmem_bytes(_XTYPE_IVEC,0,dim*sizeof(int));
    }
   
    return (iv);
}

/*!
   zero the integer vector
 */
_xivec *_xivec_zero(_xivec *ix){
    unsigned int i;
   
    if (!ix) _xerror(_XE_NULL,"_xivec_zero");
   
    for (i = 0; i < ix->dim; i++)
        ix->ive[i] = 0; 
   
    return (ix);
}

/*!
   copy integer vector in to out
   -- out created/resized if necessary
 */
_xivec *_xivec_copy(const _xivec *in, _xivec *out){
    unsigned int i;
   
    if (!in) _xerror(_XE_NULL,"_xivec_copy");
    out = _xivec_resize(out,in->dim);
    for (i = 0; i < in->dim; i++)
        out->ive[i] = in->ive[i];
   
    return (out);
}

/*!
    move selected pieces of an IVEC
  -- moves the length dim0 subvector with initial index i0
     to the corresponding subvector of out with initial index i1
  -- out is resized if necessary
 */
_xivec *_xivec_move(const _xivec *in, int i0, int dim0, _xivec *out, int i1){
    if (!in) _xerror(_XE_NULL,"_xivec_move");
    if (i0 < 0 || dim0 < 0 || i1 < 0 || (unsigned int)(i0+dim0) > in->dim)
        _xerror(_XE_BOUNDS,"_xivec_move");

    if (!out || (unsigned int)(i1+dim0) > out->dim)
        out = _xivec_resize(out,i1+dim0);

    _xmem_copy(&(in->ive[i0]),&(out->ive[i1]),dim0*sizeof(int));

    return (out);
}
/*!
    return the _xivec with dimension new_dim
    -- iv is set to the zero vector
 */
_xivec *_xivec_resize(_xivec *iv, int new_dim){
    int i;
   
    if (new_dim < 0)
        _xerror(_XE_NEG,"_xivec_resize");

    if (!iv) return _xivec_get(new_dim);
   
    if (new_dim == iv->dim) return (iv);
    if ((unsigned int)(new_dim) > iv->max_dim){
        if (_xmem_info_is_on()){
            _xmem_bytes(_XTYPE_IVEC,iv->max_dim*sizeof(int),new_dim*sizeof(int));
        }
        iv->ive = _xrenew(iv->ive,new_dim,int);
        if (!iv->ive) _xerror(_XE_MEM,"_xivec_resize");
        iv->max_dim = new_dim;
    }
    if (iv->dim <= (unsigned int)new_dim)
        for (i = iv->dim; i < new_dim; i++)
            iv->ive[i] = 0;
    iv->dim = new_dim;
   
    return (iv);
}

int _xivec_get_vars(int dim, ...){
    va_list ap;
    int i=0;
    _xivec **par;
   
    va_start(ap, dim);
    while (par = va_arg(ap,_xivec **)){/* NULL ends the list*/
        *par = _xivec_get(dim);
        i++;
    } 
    va_end(ap);

    return (i);
}

int _xivec_resize_vars(int new_dim, ...){
    va_list ap;
    int i=0;
    _xivec **par;
   
    va_start(ap, new_dim);
    while (par = va_arg(ap,_xivec **)){ /* NULL ends the list*/
        *par = _xivec_resize(*par,new_dim);
        i++;
    } 
    va_end(ap);

    return (i);
}

int _xivec_free_vars(_xivec **ipv, ...){
    va_list ap;
    int i=1;
    _xivec **par;
   
    _xivec_free(*ipv);
    *ipv = (_xivec *)NULL;
    va_start(ap, ipv);
    while (par = va_arg(ap,_xivec **)){/* NULL ends the list*/
        _xivec_free(*par); 
        *par = (_xivec *)NULL;
        i++;
    } 
    va_end(ap);

    return (i);
}

/*!
   integer vector addition -- may be in-situ
 */
_xivec *_xivec_add(const _xivec *iv1, const _xivec *iv2, _xivec *out){
    unsigned int i;
    int *out_ive, *iv1_ive, *iv2_ive;
   
    if (!iv1 || !iv2) _xerror(_XE_NULL,"_xivec_add");
    if (iv1->dim != iv2->dim)
        _xerror(_XE_SIZES,"_xivec_add");
    if (!out || out->dim != iv1->dim)
        out = _xivec_resize(out,iv1->dim);
   
    out_ive = out->ive;
    iv1_ive = iv1->ive;
    iv2_ive = iv2->ive;
   
    for (i = 0; i < iv1->dim; i++)
        out_ive[i] = iv1_ive[i] + iv2_ive[i];
   
    return (out);
}

/*!
   integer vector addition -- may be in-situ
 */
_xivec *_xivec_sub(const _xivec *iv1, const _xivec *iv2, _xivec *out){
    unsigned int i;
    int *out_ive, *iv1_ive, *iv2_ive;
   
    if (!iv1 || !iv2) _xerror(_XE_NULL,"_xivec_sub");
    if (iv1->dim != iv2->dim)
        _xerror(_XE_SIZES,"_xivec_sub");
    if (!out || out->dim != iv1->dim)
        out = _xivec_resize(out,iv1->dim);
   
    out_ive = out->ive;
    iv1_ive = iv1->ive;
    iv2_ive = iv2->ive;
   
    for (i = 0; i < iv1->dim; i++)
        out_ive[i] = iv1_ive[i] - iv2_ive[i];
   
    return (out);
}

/*!
   sort vector x, and generates permutation that gives the order
   of the components; x = [1.3, 3.7, 0.5] -> [0.5, 1.3, 3.7] and
   the permutation is order = [2, 0, 1].
   -- if order is NULL on entry then it is ignored
   -- the sorted vector x is returned 
 */
_xivec *_xivec_sort(_xivec *x, _xperm *order){
    int *x_ive, dim, i, j, l, r, v;
    int stack[_XMAX_STACK], sp;
   
    if (!x) _xerror(_XE_NULL,"_xivec_sort");
    if (order && order->size != x->dim)
        order = _xperm_resize(order, x->dim);
   
    x_ive = x->ive;
    dim = x->dim;
    if (order) _xperm_ident(order);
   
    if (dim <= 1) return (x);
   
    /* using quicksort algorithm in Sedgewick,
      "Algorithms in C", Ch. 9, pp. 118--122 (1990) */
    sp = 0;
    l = 0; r = dim-1; v = x_ive[0];
    for (;;){
        while (r > l){
            v = x_ive[r];
            i = l-1;
            j = r;
            for (;;){
                while (x_ive[++i] < v);
                --j;
                while (x_ive[j] > v && j != 0) --j;
                if (i >= j) break;
                x_ive[i] ^= x_ive[j] ^= x_ive[i] ^= x_ive[j];
                if (order){
                    order->pe[i] ^= order->pe[j] ^= order->pe[i] ^= order->pe[j];
                }
            }
            x_ive[i] ^= x_ive[r] ^= x_ive[i] ^= x_ive[r];
            if (order){
                order->pe[i] ^= order->pe[r] ^= order->pe[i] = order->pe[r];
            }
      
            if (i-l > r-i){
                stack[sp++] = l; 
                stack[sp++] = i-1;
                l = i+1;
            }
            else {
                stack[sp++] = i+1;
                stack[sp++] = r;
                r = i-1;
            }
        }
      
        /* recursion elimination */
        if ( sp == 0 ) break;
        r = stack[--sp];
        l = stack[--sp];
    }
   
    return (x);
}

void _xivec_dump(FILE *fp, const _xivec *iv){
    unsigned int i;
   
    (void) fprintf(fp,"IntVector: ");
    if (!iv){
        (void) fprintf(fp,"**** NULL ****\n");
        return;
    }
    (void) fprintf(fp,"dim: %d, max_dim: %d\n",iv->dim,iv->max_dim);
    (void) fprintf(fp,"ive @ 0x%lx\n",(long)(iv->ive));
    for (i = 0; i < iv->max_dim; i++){
        if ((i+1) % 8)
            (void) fprintf(fp,"%8d ",iv->ive[i]);
        else
            (void) fprintf(fp,"%8d\n",iv->ive[i]);
    }
    if (i % 8) (void) fprintf(fp,"\n");
}

/*!
   print a representation of iv on stream fp
 */
void _xivec_foutput(FILE *fp, const _xivec *iv){
    unsigned int i;
   
    (void) fprintf(fp,"IntVector: ");
    if (!iv){
        (void) fprintf(fp,"**** NULL ****\n");
        return;
    }
    (void) fprintf(fp,"dim: %d\n",iv->dim);
    for (i = 0; i < iv->dim; i++){
        if ((i+1) % 8)
            (void) fprintf(fp,"%8d ",iv->ive[i]);
        else
            (void) fprintf(fp,"%8d\n",iv->ive[i]);
    }
    if (i % 8) (void) fprintf(fp,"\n");
}

/*!
   input integer vector from stream fp
  -- input from a terminal is handled interactively
  -- batch/file input has the same format as produced by
  iv_foutput except that whitespace and comments ("#...\n") 
  are skipped
 */
_xivec *_xivec_finput(FILE *fp, _xivec *iv){
    _xivec *_xiiv_finput(), *_xbiv_finput();
    return (isatty(_fileno(fp)) ? _xiiv_finput(fp,iv) :
                                  _xbiv_finput(fp,iv));
}

_xivec *_xiiv_finput(FILE *fp, _xivec *iv){
    unsigned int i,dim,dynamic; /* dynamic set if memory allocated here */
   
    /* get dimension */
    if (iv && iv->dim<_XMAXDIM){
        dim = iv->dim;
        dynamic = _XFALSE;
    }
    else {
        dynamic = _XTRUE;
        do {
            (void) fprintf(stderr,"IntVector: dim: ");
            if (fgets(_xline,_XMAXLINE,fp)==NULL)
                _xerror(_XE_INPUT,"_xiiv_finput");
        } while (sscanf(_xline,"%u",&dim)<1 || dim>_XMAXDIM);
        iv = _xivec_get(dim);
    }
   
    /* input elements */
    for (i=0; i<dim; i++)
        do {
        redo:
            (void) fprintf(stderr,"entry %u: ",i);
            if (!dynamic)
                (void) fprintf(stderr,"old: %-9d  new: ",iv->ive[i]);
            if (fgets(_xline,_XMAXLINE,fp)==NULL)
                _xerror(_XE_INPUT,"_xiiv_finput");
            if ((*_xline == 'b' || *_xline == 'B') && i > 0){
                i--;
                dynamic = _XFALSE;
                goto redo;
            }
            if ((*_xline == 'f' || *_xline == 'F') && i < dim-1){
                i++;
                dynamic = _XFALSE;
                goto redo;
            }
        } while (*_xline=='\0' || sscanf(_xline,"%d",&iv->ive[i]) < 1);
    
    return (iv);
}

_xivec *_xbiv_finput(FILE *fp, _xivec *iv){
    unsigned int i,dim;
    int io_code;
   
    /* get dimension */
    _xskipjunk(fp);
    if ((io_code=fscanf(fp," IntVector: dim:%u",&dim)) < 1 || dim>_XMAXDIM)
        _xerror(io_code==EOF ? 7 : 6,"_xbiv_finput");
   
    /* allocate memory if necessary */
    if (!iv || iv->dim<dim)
        iv = _xivec_resize(iv,dim);
   
    /* get entries */
    _xskipjunk(fp);
    for (i=0; i<dim; i++)
        if ((io_code=fscanf(fp,"%d",&iv->ive[i])) < 1)
            _xerror(io_code==EOF ? 7 : 6,"_xbiv_finput");
   
    return (iv);
}

/*!
    copy selected piece of matrix to a vector
  -- moves the m0 x n0 submatrix with top-left co-ordinate (i0,j0) to
     the subvector with initial index i1 (and length m0*n0)
  -- rows are copied contiguously
  -- out is resized if necessary
 */
_xvec *_xmat_mv_move(const _xmat *in, int i0, int j0, int m0, int n0, _xvec *out, int i1){
    int dim1, i;

    if (!in) _xerror(_XE_NULL,"_xvec_mv_move");
    if (i0 < 0 || j0 < 0 || m0 < 0 || n0 < 0 || i1 < 0 || 
       (unsigned int)(i0+m0) > in->m || 
       (unsigned int)(j0+n0) > in->n )
        _xerror(_XE_BOUNDS,"_xvec_mv_move");

    dim1 = m0*n0;
    if (!out || (unsigned int)(i1+dim1) > out->dim)
        out = _xvec_resize(out,i1+dim1);

    for (i = 0; i < m0; i++)
        _xmem_copy(&(in->me[i0+i][j0]),&(out->ve[i1+i*n0]),n0*sizeof(_xreal));

    return (out);
}

/*!
    copy selected piece of vector to a matrix
    -- moves the subvector with initial index i0 and length m1*n1 to
     the m1 x n1 submatrix with top-left co-ordinate (i1,j1)
    -- copying is done by rows
  -- out is resized if necessary
 */
_xmat *_xmat_vm_move(const _xvec *in, int i0, _xmat *out, int i1, int j1, int m1, int n1){
    int dim0, i;

    if (!in) _xerror(_XE_NULL,"_xmat_vm_move");
    if (i0 < 0 || i1 < 0 || j1 < 0 || m1 < 0 || n1 < 0 ||
       (unsigned int)(i0+m1*n1) > in->dim)
        _xerror(_XE_BOUNDS,"_xmat_vm_move");

    if (!out)
        out = _xmat_resize(out,i1+m1,j1+n1);
    else
        out = _xmat_resize(out,_xmax((unsigned int)(i1+m1),out->m),
                               _xmax((unsigned int)(j1+n1),out->n));

    dim0 = m1*n1;
    for (i = 0; i < m1; i++)
        _xmem_copy(&(in->ve[i0+i*n1]),&(out->me[i1+i][j1]),n1*sizeof(_xreal));

    return (out);
}

/*!
    allocate and initialize band matrix
 */
_xband *_xband_get(int lb, int ub, int n){
    _xband *A;

    if (lb < 0 || ub < 0 || n <= 0) _xerror(_XE_NEG,"_xband_get");
    if ((A = _xnew(_xband)) == (_xband *)NULL)
        _xerror(_XE_MEM,"_xband_get");
    else if (_xmem_info_is_on()){
        _xmem_bytes(_XTYPE_BAND,0,sizeof(_xband));
        _xmem_numvar(_XTYPE_BAND,1);
    }

    lb = A->lb = _xmin(n-1,lb);
    ub = A->ub = _xmin(n-1,ub);
    A->mat = _xmat_get(lb+ub+1,n);

    return (A);
}

/*!
   resize band matrix
 */
_xband *_xband_resize(_xband *A, int new_lb, int new_ub, int new_n){
    int lb,ub,i,j,l,shift,umin;
    _xreal **Av;

    if (new_lb < 0 || new_ub < 0 || new_n <= 0)
        _xerror(_XE_NEG,"_xband_resize");
    if (!A) return _xband_get(new_lb,new_ub,new_n);
    if ((unsigned int)(A->lb+A->ub+1) > A->mat->m)
        _xerror(_XE_INTERN,"_xband_resize");

    if (A->lb == new_lb && A->ub == new_ub && A->mat->n == new_n)
        return (A);

    lb = A->lb;
    ub = A->ub;
    Av = A->mat->me;
    umin = _xmin(ub,new_ub);

    /* ensure that unused triangles at edges are zero'd */
    for (i = 0; i < lb; i++)
        for (j = A->mat->n - lb + i; (unsigned int)j < A->mat->n; j++)
            Av[i][j] = (_xreal)0.0;  
        for (i = lb+1,l=1; l <= umin; i++,l++)
        for (j = 0; j < l; j++)
            Av[i][j] = (_xreal)0.0; 

    new_lb = A->lb = _xmin(new_lb,new_n-1);
    new_ub = A->ub = _xmin(new_ub,new_n-1);
    A->mat = _xmat_resize(A->mat,new_lb+new_ub+1,new_n);
    Av = A->mat->me;

    /* if new_lb != lb then move the rows to get the main diag 
      in the new_lb row */

    if (new_lb > lb){
        shift = new_lb-lb;

        for (i=lb+umin, l=i+shift; i >= 0; i--,l--)
            _xmem_copy(Av[i],Av[l],new_n*sizeof(_xreal));
        for (l=shift-1; l >= 0; l--)
            __xzero__(Av[l],new_n);
    }
    else if (new_lb < lb){ 
        shift = lb - new_lb;

        for (i=shift, l=0; i <= lb+umin; i++,l++)
            _xmem_copy(Av[i],Av[l],new_n*sizeof(_xreal));
        for (i=lb+umin+1; i <= new_lb+new_ub; i++)
            __xzero__(Av[i],new_n);
    }

    return (A);
}

/*!
    copy band matrix A to B, returning B
  -- if B is NULL, create
  -- B is set to the correct size
 */
_xband *_xband_copy(const _xband *A, _xband *B){
    int lb,ub,i,j,n;
   
    if (!A) _xerror(_XE_NULL,"_xband_copy");
    if (A == B) return (B);
   
    n = A->mat->n;
    if (!B)
        B = _xband_get(A->lb,A->ub,n);
    else if (B->lb != A->lb || B->ub != A->ub || B->mat->n != n )
        B = _xband_resize(B,A->lb,A->ub,n);
   
    if (A->mat == B->mat) return (B);

    ub = B->ub = A->ub;
    lb = B->lb = A->lb;
    for (i=0, j=n-lb; i <= lb; i++, j++)
        _xmem_copy(A->mat->me[i],B->mat->me[i],j*sizeof(_xreal));   

    for (i=lb+1, j=1; i <= lb+ub; i++, j++)
        _xmem_copy(A->mat->me[i]+j,B->mat->me[i]+j,(n - j)*sizeof(_xreal));     

    return (B);
}

/*!
   transpose band matrix
   in  -- band to be transpose
   out -- matrix after transposition
   can be done in situ
 */
_xband *_xband_transp(const _xband *in, _xband *out){
    int i, j, jj, l, k, lb, ub, lub, n, n1;
    int in_situ;
    _xreal **in_v, **out_v;
   
    if (!in || !in->mat)
        _xerror(_XE_NULL,"_xband_transp");

    lb = in->lb;
    ub = in->ub;
    lub = lb+ub;
    n = in->mat->n;
    n1 = n-1;

    in_situ = ( in == out );
    if (!in_situ)
        out = _xband_resize(out,ub,lb,n);
    else { /* only need to swap lb and ub fields */
        out->lb = ub;
        out->ub = lb;
    }

    in_v = in->mat->me;
    if (!in_situ){
        int sh_in,sh_out; 

        out_v = out->mat->me;
        for (i=0, l=lub, k=lb-i; i <= lub; i++,l--,k--){
            sh_in = _xmax(-k,0);
            sh_out = _xmax(k,0);
            _xmem_copy(&(in_v[i][sh_in]),&(out_v[l][sh_out]),
                      (n-sh_in-sh_out)*sizeof(_xreal));
        }
    }
    else if (ub == lb){
        _xreal tmp;
        for (i=0, l=lub, k=lb-i; i < lb; i++,l--,k--){
            for (j=n1-k, jj=n1; j >= 0; j--,jj--){
                tmp = in_v[l][jj];
                in_v[l][jj] = in_v[i][j];
                in_v[i][j] = tmp;
            }
        }
    }
    else if (ub > lb){/* hence i-ub <= 0 & l-lb >= 0 */
        int p,pp,lbi;
        for (i=0, l=lub; i < (lub+1)/2; i++,l--){
            lbi = lb-i;
            for (j=l-lb, jj=0, p=_xmax(-lbi,0), pp=_xmax(l-ub,0); j <= n1; j++,jj++,p++,pp++){
                in_v[l][pp] = in_v[i][p];
                in_v[i][jj] = in_v[l][j];
            }
            for (; p <= n1-_xmax(lbi,0); p++,pp++)
                in_v[l][pp] = in_v[i][p];
        }
      
        if (lub%2 == 0){ /* shift only */
            i = lub/2;
            for (j=_xmax(i-lb,0), jj=0; jj <= n1-ub+i; j++,jj++) 
                in_v[i][jj] = in_v[i][j];
        }
    }
    else { /* ub < lb, hence ub-l <= 0 & lb-i >= 0 */
        int p,pp,ubi;
        for (i=0, l=lub; i < (lub+1)/2; i++,l--){
            ubi = i-ub;
            for (j=n1-_xmax(lb-l,0), jj=n1-_xmax(-ubi,0), p=n1-lb+i, pp=n1;
                 p >= 0; j--, jj--, pp--, p--){
                in_v[i][jj] = in_v[l][j];
                in_v[l][pp] = in_v[i][p];
            }
            for (; jj >= _xmax(ubi,0); j--, jj--)
                in_v[i][jj] = in_v[l][j];
        }

        if (lub%2 == 0){ /* shift only */
            i = lub/2;
            for (j=n1-lb+i, jj=n1-_xmax(ub-i,0); j >= 0; j--, jj--) 
                in_v[i][jj] = in_v[i][j];
        }
    }

    return (out);
}

/*!
   zero band matrix A which is returned
 */
_xband *_xband_zero(_xband *A){
    if (!A) _xerror(_XE_NULL,"_xband_zero");
    _xmat_zero(A->mat);
    return (A);
}

/*!
    return out <- A+alpha*B
  -- out is created (as zero) if NULL
  -- if out is not the correct size, it is re-sized before the operation
  -- if A or B are null, and error is generated
 */
_xband *_xband_bds_mltadd(const _xband *A, const _xband *B, double alpha, _xband *out){
    int i;

    if (!A || !B) _xerror(_XE_NULL,"_xband_bds_mltadd");
    if (A->mat->n != B->mat->n) _xerror(_XE_SIZES,"_xband_bds_mltadd");
    if (A == out || B == out) _xerror(_XE_INSITU,"_xband_bds_mltadd");

    out = _xband_copy(A,out);
    out = _xband_resize(out,_xmax(A->lb,B->lb),_xmax(A->ub,B->ub),A->mat->n);
    for (i = 0; i <= B->lb + B->ub; i++)
        __xmltadd__(out->mat->me[i+out->lb-B->lb],B->mat->me[i],alpha,B->mat->n);
  
    return (out);
}

/*!
   return out <- s.A
 */
_xband *_xband_sbd_mltadd(_xreal s, const _xband *A, _xband *out){
    if (!A) _xerror(_XE_NULL,"_xband_sbd_mltadd");

    out = _xband_resize(out,A->lb,A->ub,A->mat->n);
    _xmat_sm_mlt(s,A->mat,out->mat);

    return (out);
}

/*!
   copy a square matrix to a band matrix with 
   lb subdiagonals and ub superdiagonals
 */
_xband *_xmat_mat2band(const _xmat *A, int lb, int ub, _xband *bA){
    int i, j, l, n1;
    _xreal **bmat;
   
    if (!A) _xerror(_XE_NULL,"_xband_mat2band");
    if (ub < 0 || lb < 0) _xerror(_XE_SIZES,"_xband_mat2band");
    if ( bA && bA->mat == A) _xerror(_XE_INSITU,"_xband_mat2band");

    n1 = A->n-1;
    lb = _xmin(n1,lb);
    ub = _xmin(n1,ub);
    bA = _xband_resize(bA,lb,ub,n1+1);
    bmat = bA->mat->me;

    for (j=0; j <= n1; j++)
        for (i=_xmin(n1,j+lb),l=lb+j-i; i >= _xmax(0,j-ub); i--,l++)
            bmat[l][j] = A->me[i][j];

    return (bA);
}

/*!
   copy band matrix bA to a square matrix A returning A
 */
_xmat *_xband_band2mat(const _xband *bA, _xmat *A){
    int i,j,l,n,n1;
    int lb, ub;
    _xreal **bmat;

    if (!bA) _xerror(_XE_NULL,"_xmat_band2mat");
    if (bA->mat == A) _xerror(_XE_INSITU,"_xmat_band2mat");

    ub = bA->ub;
    lb = bA->lb;
    n = bA->mat->n;
    n1 = n-1;
    bmat = bA->mat->me;

    A = _xmat_resize(A,n,n);
    _xmat_zero(A);

    for (j=0; j < n; j++)
        for (i=_xmin(n1,j+lb),l=lb+j-i; i >= _xmax(0,j-ub); i--,l++)
            A->me[i][j] = bmat[l][j];

    return (A);
}

/*!
   band matrix-vector multiply and add
   -- returns out <- x + s.bA.y
   -- if y is NULL then create y (as zero vector)
   -- error if either A or x is NULL
 */
_xvec *_xband_bdv_mltadd(const _xvec *x, const _xvec *y, const _xband *bA, double s, _xvec *out){
    unsigned int i, j;

    if (!bA || !x || !y) _xerror(_XE_NULL,"_xband_bdv_mltadd");
    if (bA->mat->n != x->dim || y->dim != x->dim)
        _xerror(_XE_SIZES,"_xband_bdv_mltadd");
    if (!out || out->dim != x->dim)
        out = _xvec_resize(out,x->dim);
    out = _xvec_vcopy(x,out);

    for (j = 0; j < x->dim; j++)
        for (i = _xmax(j-bA->ub,0); i <= j+bA->lb && i < x->dim; i++)
            out->ve[i] += (_xreal)(s*_xband_get_val(bA,i,j)*y->ve[j]);

    return (out);
}

/*!
   band matrix-vector multiply and add
   -- returns out^T <- x^T + s.y^T.bA
   -- if out is NULL then create out (as zero vector)
   -- error if either bA or x is NULL
 */
_xvec *_xband_vbd_mltadd(const _xvec *x, const _xvec *y, const _xband *bA, double s, _xvec *out){
    unsigned int i, j;

    if (!bA || !x || !y) _xerror(_XE_NULL,"_xband_vbd_mltadd");
    if (bA->mat->n != x->dim || y->dim != x->dim)
        _xerror(_XE_SIZES,"_xband_vbd_mltadd");
    if (!out || out->dim != x->dim)
        out = _xvec_resize(out,x->dim);
    out = _xvec_vcopy(x,out);

    for (j = 0; j < x->dim; j++)
        for (i = _xmax(j-bA->ub,0); i <= j+bA->lb && i < x->dim; i++)
            out->ve[j] += (_xreal)(s*_xband_get_val(bA,i,j)*y->ve[i]);

    return (out);
}

/* scratch area -- enough for a single line */
static char _xscratch[_XMAXLINE+1];

/* default value for fy_or_n */
static int _xy_n_dflt = _XTRUE;

/*!
   set the value of _xy_n_dflt to val
 */
int _xyn_dflt(int val){
    return (_xy_n_dflt = val);
}

/*!
    yes-or-no to question is string s
  -- question written to stderr, input from fp 
  -- if fp is NOT a tty then return _xy_n_dflt
 */
int _xfy_or_n(FILE *fp, const char *s){
    char *cp;

    if (!isatty(_fileno(fp))) return _xy_n_dflt;
    
    for (;;){
        (void) fprintf(stderr,"%s (y/n) ? ",s);
        if (fgets(_xscratch,_XMAXLINE,fp)==NULL)
            _xerror(_XE_INPUT,"_xfy_or_n");
        cp = _xscratch;
        while (isspace(*cp)) cp++;
        if (*cp == 'y' || *cp == 'Y')
            return _XTRUE;
        if (*cp == 'n' || *cp == 'N')
            return _XFALSE;
        (void) fprintf(stderr,"Please reply with 'y' or 'Y' for yes ");
        (void) fprintf(stderr,"and 'n' or 'N' for no.\n");
    }
}

/*!
    return integer read from file/stream fp
  -- prompt s on stderr if fp is a tty
  -- check that x lies between low and high: re-prompt if
    fp is a tty, error exit otherwise
  -- ignore check if low > high
 */
int _xfin_int(FILE *fp, const char *s, int low, int high){
    int retcode, x;

    if (!isatty(_fileno(fp))){
        _xskipjunk(fp);
        if ((retcode=fscanf(fp,"%d",&x)) == EOF)
            _xerror(_XE_INPUT,"_xfin_int");
        if (retcode <= 0)
            _xerror(_XE_FORMAT,"_xfin_int");
        if (low <= high && ( x < low || x > high))
            _xerror(_XE_BOUNDS,"_xfin_int");
        return (x);
    }
    
    for (;;){
        (void) fprintf(stderr,"%s: ",s);
        if (fgets(_xscratch,_XMAXLINE,stdin)==NULL)
            _xerror(_XE_INPUT,"_xfin_int");
        retcode = sscanf(_xscratch,"%d",&x);
        if ((retcode==1 && low > high) || (x >= low && x <= high))
            return (x);
        (void) fprintf(stderr,"Please type an integer in range [%d,%d].\n",
                       low,high);
    }
}

/*!
    return double read from file/stream fp
  -- prompt s on stderr if fp is a tty
  -- check that x lies between low and high: re-prompt if
    fp is a tty, error exit otherwise
  -- ignore check if low > high
 */
double _xfin_double(FILE *fp, const char *s, double low, double high){
    _xreal retcode, x;

    if (!isatty(_fileno(fp))){
        _xskipjunk(fp);
#if _XREAL == _XDOUBLE
        if ((retcode=fscanf(fp,"%lf",&x)) == EOF)
#elif _XREAL == _XFLOAT
        if ((retcode=(float)fscanf(fp,"%f",&x)) == EOF)
#endif
            _xerror(_XE_INPUT,"_xfin_double");
        if (retcode <= 0)
            _xerror(_XE_FORMAT,"_xfin_double");
        if (low <= high && (x < low || x > high))
            _xerror(_XE_BOUNDS,"_xfin_double");
        return (double)(x);
    }

    for (;;){
        (void) fprintf(stderr,"%s: ",s);
        if (fgets(_xscratch,_XMAXLINE,stdin)==NULL)
            _xerror(_XE_INPUT,"_xfin_double");
#if _XREAL == _XDOUBLE
        retcode = sscanf(_xscratch,"%lf",&x);
#elif _XREAL == _XFLOAT 
        retcode = (float)sscanf(_xscratch,"%f",&x);
#endif
        if ((retcode==1 && low > high) || (x >= low && x <= high))
            return (double)(x);
        (void) fprintf(stderr,"Please type an double in range [%g,%g].\n",
                       low,high);
    }
}

/*!
   back substitution with optional over-riding diagonal.
   -- can be in-situ but doesn't need to be
 */
_xvec *_xmat_usolve(const _xmat *matrix, const _xvec *b, _xvec *out, double diag){
    unsigned int dim;
    int i, i_lim;
    _xreal **mat_ent, *mat_row, *b_ent, *out_ent, *out_col, sum, tiny;
    
    if (!matrix || !b) _xerror(_XE_NULL,"_xmat_usolve");
    dim = _xmin(matrix->m,matrix->n);
    if (b->dim < dim) _xerror(_XE_SIZES,"_xmat_usolve");
    if (!out || out->dim < dim)
        out = _xvec_resize(out,matrix->n);
    
    mat_ent = matrix->me; b_ent = b->ve; out_ent = out->ve;
    tiny = (_xreal)(10.0/_XHUGE_VAL);
    
    for (i=dim-1; i>=0; i--)
        if (b_ent[i] != (_xreal)(0.0))
            break;
        else
            out_ent[i] = (_xreal)(0.0);
    
    i_lim = i;
    for ( ; i>=0; i--){
        sum = b_ent[i];
        mat_row = &(mat_ent[i][i+1]);
        out_col = &(out_ent[i+1]);
        sum -= (_xreal)(__xip__(mat_row,out_col,i_lim-i));
        
        if (diag==0.0){
            if (fabs(mat_ent[i][i]) <= tiny*fabs(sum))
                _xerror(_XE_SING,"_xmat_usolve");
            else
                out_ent[i] = sum/mat_ent[i][i];
        }
        else
            out_ent[i] = (_xreal)(sum/diag);
    }
    
    return (out);
}

/*!
   forward elimination with (optional) default diagonal value.
 */
_xvec *_xmat_lsolve(const _xmat *matrix, const _xvec *b, _xvec *out, double diag){
    unsigned int dim, i, i_lim;
    _xreal **mat_ent, *mat_row, *b_ent, *out_ent, *out_col, sum, tiny;
    
    if (!matrix || !b) _xerror(_XE_NULL,"_xmat_lsolve");
    dim = _xmin(matrix->m,matrix->n);
    if (b->dim < dim) _xerror(_XE_SIZES,"_xmat_lsolve");
    if (!out || out->dim < dim)
        out = _xvec_resize(out,matrix->n);
    
    mat_ent = matrix->me; b_ent = b->ve; out_ent = out->ve;
    for (i=0; i<dim; i++)
        if (b_ent[i] != (_xreal)(0.0))
            break;
        else
            out_ent[i] = (_xreal)(0.0);
        
    i_lim = i;
    tiny = (_xreal)(10.0/_XHUGE_VAL);
    for ( ; i<dim; i++){
        sum = b_ent[i];
        mat_row = &(mat_ent[i][i_lim]);
        out_col = &(out_ent[i_lim]);
        sum -= (_xreal)(__xip__(mat_row,out_col,(int)(i-i_lim)));
        
        if (diag==0.0){
            if (fabs(mat_ent[i][i]) <= tiny*fabs(sum) )
                _xerror(_XE_SING,"_xmat_lsolve");
            else
                out_ent[i] = sum/mat_ent[i][i];
        }
        else
            out_ent[i] = (_xreal)(sum/diag);
    }
    
    return (out);
}

/*!
   solves Dx=b where D is the diagonal of A -- may be in-situ
 */
_xvec *_xmat_dsolve(const _xmat *A, const _xvec *b, _xvec *x){
    unsigned int dim, i;
    _xreal tiny;
    
    if (!A || !b) _xerror(_XE_NULL,"_xmat_dsolve");
    dim = _xmin(A->m,A->n);
    if (b->dim < dim) _xerror(_XE_SIZES,"_xmat_dsolve");

    x = _xvec_resize(x,A->n);
    tiny = (_xreal)(10.0/_XHUGE_VAL);

    dim = b->dim;
    for (i=0; i<dim; i++)
        if (fabs(A->me[i][i]) <= tiny*fabs(b->ve[i]))
            _xerror(_XE_SING,"_xmat_dsolve");
        else
            x->ve[i] = b->ve[i]/A->me[i][i];
    
    return (x);
}

/*!
   forward elimination with (optional) default diagonal value
   using UPPER triangular part of matrix
 */
_xvec *_xmat_utsolve(const _xmat *U, const _xvec *b, _xvec *out, double diag){
    unsigned int dim, i, i_lim;
    _xreal **U_me, *b_ve, *out_ve, tmp, invdiag, tiny;
    
    if (!U || !b) _xerror(_XE_NULL,"_xmat_utsolve");
    dim = _xmin(U->m,U->n);
    if (b->dim < dim)
        _xerror(_XE_SIZES,"_xmat_utsolve");

    out = _xvec_resize(out,U->n);
    U_me = U->me; b_ve = b->ve; out_ve = out->ve;
    tiny = (_xreal)(10.0/_XHUGE_VAL);

    for (i=0; i<dim; i++)
        if (b_ve[i] != (_xreal)(0.0))
            break;
        else
            out_ve[i] = (_xreal)(0.0);
    
    i_lim = i;
    if (b != out){
        __xzero__(out_ve,out->dim);
        _xmem_copy(&(b_ve[i_lim]),&(out_ve[i_lim]),(dim-i_lim)*sizeof(_xreal));
    }

    if (diag == 0.0){
        for ( ; i<dim; i++){
            tmp = U_me[i][i];
            if (fabs(tmp) <= tiny*fabs(out_ve[i]))
                _xerror(_XE_SING,"_xmat_utsolve");
            out_ve[i] /= tmp;
            __xmltadd__(&(out_ve[i+1]),&(U_me[i][i+1]),-out_ve[i],dim-i-1);
        }
    }
    else {
        invdiag = (_xreal)(1.0/diag);
        for ( ; i<dim; i++ ){
            out_ve[i] *= invdiag;
            __xmltadd__(&(out_ve[i+1]),&(U_me[i][i+1]),-out_ve[i],dim-i-1);
        }
    }

    return (out);
}

/*!
   back substitution with optional over-riding diagonal
   using the LOWER triangular part of matrix
   -- can be in-situ but doesn't need to be
 */
_xvec *_xmat_ltsolve(const _xmat *L, const _xvec *b, _xvec *out, double diag){
    unsigned int dim;
    int i, i_lim;
    _xreal **L_me, *b_ve, *out_ve, tmp, invdiag, tiny;
    
    if (!L || !b) _xerror(_XE_NULL,"_xmat_ltsolve");
    dim = _xmin(L->m,L->n);
    if (b->dim < dim) _xerror(_XE_SIZES,"_xmat_ltsolve");

    out = _xvec_resize(out,L->n);
    L_me = L->me; b_ve = b->ve; out_ve = out->ve;
    tiny = (_xreal)(10.0/_XHUGE_VAL);
    
    for (i=dim-1; i>=0; i--)
        if (b_ve[i] != (_xreal)(0.0))
            break;

    i_lim = i;
    if (b != out){
        __xzero__(out_ve,out->dim);
        _xmem_copy(b_ve,out_ve,(i_lim+1)*sizeof(_xreal));
    }

    if (diag == 0.0){
        for ( ; i>=0; i--){
            tmp = L_me[i][i];
            if (fabs(tmp) <= tiny*fabs(out_ve[i]))
                _xerror(_XE_SING,"_xmat_ltsolve");
            out_ve[i] /= tmp;
            __xmltadd__(out_ve,L_me[i],-out_ve[i],i);
        }
    }
    else {
        invdiag = (_xreal)(1.0/diag);
        for ( ; i>=0; i--){
            out_ve[i] *= invdiag;
            __xmltadd__(out_ve,L_me[i],-out_ve[i],i);
        }
    }
    
    return (out);
}

/*!
   given an LU factorisation in A, solve A^T.x=b
 */
_xvec *_xmat_lutsolve(const _xmat *LU, _xperm *pivot, const _xvec *b, _xvec *x){
    if (!LU || !b || !pivot) _xerror(_XE_NULL,"_xmat_lutsolve");
    if (LU->m != LU->n || LU->n != b->dim) _xerror(_XE_SIZES,"_xmat_lutsolve");
    
    x = _xvec_vcopy(b,x);
    _xmat_utsolve(LU,x,x,0.0);      /* explicit diagonal */
    _xmat_ltsolve(LU,x,x,1.0);      /* implicit diagonal = 1 */
    _xperm_inv_vec(pivot,x,x);      /* x := P^T.tmp */
    
    return (x);
}

/*!
   calulates Householder vector to eliminate all entries after the
   i0 entry of the vector vec. It is returned as out. May be in-situ
 */
_xvec *_xvec_hhvec(const _xvec *vec, unsigned int i0, _xreal *beta, _xvec *out, _xreal *newval){
    _xreal norm;
    
    out = _xvec_copy(vec,out,i0);
    norm = (_xreal)sqrt(_xvec_in_prod(out,out,i0));
    if (norm <= (_xreal)(0.0)){
        *beta = (_xreal)(0.0);
        return (out);
    }
    *beta = (_xreal)(1.0/(norm * (norm+fabs(out->ve[i0]))));
    if (out->ve[i0] > (_xreal)(0.0))
        *newval = -norm;
    else
        *newval = norm;
    out->ve[i0] -= *newval;
    
    return (out);
}

/*!
   apply Householder transformation to vector 
   -- that is, out <- (I-beta.hh(i0:n).hh(i0:n)^T).in
   -- may be in-situ
 */
_xvec *_xvec_hhtrvec(const _xvec *hh, double beta, unsigned int i0, const _xvec *in, _xvec *out){
    _xreal scale;
    
    if (!hh || !in)
        _xerror(_XE_NULL,"_xvec_hhtrvec");
    if (in->dim != hh->dim)
        _xerror(_XE_SIZES,"_xvec_hhtrvec");
    if (i0 > in->dim)
        _xerror(_XE_BOUNDS,"_xvec_hhtrvec");
    
    scale = (_xreal)(beta*_xvec_in_prod(hh,in,i0));
    out = _xvec_vcopy(in,out);
    __xmltadd__(&(out->ve[i0]),&(hh->ve[i0]),-scale,(int)(in->dim-i0));
    
    return (out);
}

/*!
    transform a matrix by a Householder vector by rows
  starting at row i0 from column j0 -- in-situ
  -- that is, M(i0:m,j0:n) <- M(i0:m,j0:n)(I-beta.hh(j0:n).hh(j0:n)^T)
 */
_xmat *_xmat_hhtrrows(_xmat *M, unsigned int i0, unsigned int j0, const _xvec *hh, double beta){
    _xreal ip, scale;
    unsigned int i;
    
    if (!M || !hh)
        _xerror(_XE_NULL,"_xmat_hhtrrows");
    if (M->n != hh->dim)
        _xerror(_XE_RANGE,"_xmat_hhtrrows");
    if (i0 > M->m || j0 > M->n)
        _xerror(_XE_BOUNDS,"_xmat_hhtrrows");
    
    if (beta == 0.0) return (M);
    
    /* for each row ... */
    for (i = i0; i < M->m; i++){  
        /* compute inner product */
        ip = (_xreal)__xip__(&(M->me[i][j0]),&(hh->ve[j0]),(int)(M->n-j0));
        scale = (_xreal)(beta*ip);
        if (scale == (_xreal)(0.0))
            continue;
        
        /* do operation */
        __xmltadd__(&(M->me[i][j0]),&(hh->ve[j0]),-scale,
                   (int)(M->n-j0));
    }
    
    return (M);
}

/*!
    transform a matrix by a Householder vector by columns
  starting at row i0 from column j0 
  -- that is, M(i0:m,j0:n) <- (I-beta.hh(i0:m).hh(i0:m)^T)M(i0:m,j0:n)
  -- in-situ
  -- calls _hhtrcols() with the scratch vector w
  -- Meschach internal routines should call _hhtrcols() to
  avoid excessive memory allocation/de-allocation
 */
_xmat *_xmat_hhtrcols(_xmat *M, unsigned int i0, unsigned int j0, const _xvec *hh, double beta){
    _XSTATIC _xvec *w = (_xvec *)NULL;

    if (!M || !hh || !w)
        _xerror(_XE_NULL,"_xmat_hhtrcols");
    if (M->m != hh->dim)
        _xerror(_XE_SIZES,"_xmat_hhtrcols");
    if (i0 > M->m || j0 > M->n)
        _xerror(_XE_BOUNDS,"_xmat_hhtrcols");

    if (!w || w->dim < M->n)
        w = _xvec_resize(w,M->n);

    _XMEM_STAT_REG(w,_XTYPE_VEC);
    M = _xmat_hhvtrcols(M,i0,j0,hh,beta,w);

#ifdef _XTHREAD_SAFE
    _xvec_vfree(w);
#endif

    return (M);
}

/*!
    transform a matrix by a Householder vector by columns
  starting at row i0 from column j0 
  -- that is, M(i0:m,j0:n) <- (I-beta.hh(i0:m).hh(i0:m)^T)M(i0:m,j0:n)
  -- in-situ
  -- scratch vector w passed as argument
  -- raises error if w == NULL
 */
_xmat *_xmat_hhvtrcols(_xmat *M, unsigned int i0, unsigned int j0, const _xvec *hh, double beta, _xvec *w){
    unsigned int i;

    if (!M || !hh || !w)
        _xerror(_XE_NULL,"_xmat_hhvtrcols");
    if (M->m != hh->dim)
        _xerror(_XE_SIZES,"_xmat_hhvtrcols");
    if (i0 > M->m || j0 > M->n)
        _xerror(_XE_BOUNDS,"_xmat_hhvtrcols");
    
    if (beta == 0.0) return (M);
    
    if (w->dim < M->n)
        w = _xvec_resize(w,M->n);
    
    _xvec_zero(w);
    for (i = i0; i < M->m; i++)
        if (hh->ve[i] != (_xreal)(0.0))
            __xmltadd__(&(w->ve[j0]),&(M->me[i][j0]),hh->ve[i],
                       (int)(M->n-j0));
    for (i = i0; i < M->m; i++)
        if (hh->ve[i] != (_xreal)(0.0))
            __xmltadd__(&(M->me[i][j0]),&(w->ve[j0]),-beta*hh->ve[i],
                       (int)(M->n-j0));
    return (M);
}

/*!
   solves Qx = b, Q is an orthogonal matrix stored in compact
   form a la _xmat_qrfactor() -- may be in-situ
 */
_xvec *_xmat_qsolve(const _xmat *QR, const _xvec *diag, const _xvec *b, _xvec *x, _xvec *tmp){
    unsigned int dynamic;
    unsigned int k, limit;
    _xreal beta, r_ii, tmp_val;
    
    limit = _xmin(QR->m,QR->n);
    dynamic = _XFALSE;
    if (!QR || !diag || !b)
        _xerror(_XE_NULL,"_xmat_qsolve");
    if (diag->dim < limit || b->dim != QR->m)
        _xerror(_XE_SIZES,"_xmat_qsolve");
    x = _xvec_resize(x,QR->m);
    if (!tmp) dynamic = _XTRUE;
    tmp = _xvec_resize(tmp,QR->m);
    
    /* apply H/holder transforms in normal order */
    x = _xvec_vcopy(b,x);
    for (k = 0 ; k < limit ; k++){
        _xmat_get_col(QR,k,tmp);
        r_ii = (_xreal)fabs(tmp->ve[k]);
        tmp->ve[k] = diag->ve[k];
        tmp_val = (_xreal)(r_ii*fabs(diag->ve[k]));
        beta = (_xreal)(( tmp_val == (_xreal)0.0 ) ? 0.0 : 1.0/tmp_val);
        _xvec_hhtrvec(tmp,beta,k,x,x);
    }
    
    if (dynamic) _xvec_vfree(tmp);
    
    return (x);
}

/*!
   solve the system Q.R.x=b where Q & R are stored in compact form
   -- returns x, which is created if necessary
 */
_xvec *_xmat_qrsolve(const _xmat *QR, const _xvec *diag, const _xvec *b, _xvec *x){
    unsigned int limit;
    _XSTATIC _xvec *tmp = (_xvec *)NULL;
    
    if (!QR || !diag || !b)
        _xerror(_XE_NULL,"_xmat_qrsolve");
    limit = _xmin(QR->m,QR->n);
    if (diag->dim < limit || b->dim != QR->m)
        _xerror(_XE_SIZES,"_xmat_qrsolve");
    tmp = _xvec_resize(tmp,limit);
    _XMEM_STAT_REG(tmp,_XTYPE_VEC);

    x = _xvec_resize(x,QR->n);
    _xmat_qsolve(QR,diag,b,x,tmp);
    x = _xmat_usolve(QR,x,x,0.0);
    _xvec_resize(x,QR->n);

#ifdef _XTHREAD_SAFE
    _xvec_vfree(tmp);
#endif

    return (x);
}

/*!
    solve A^T.sc = c where the QR factors of A are stored in
  compact form
  -- returns sc
  -- original due to Mike Osborne modified Wed 09th Dec 1992
 */
_xvec *_xmat_qrtsolve(const _xmat *A, const _xvec *diag, const _xvec *c, _xvec *sc){
    int i, j, k, n, p;
    _xreal beta, r_ii, s, tmp_val;

    if (!A || !diag || !c)
        _xerror(_XE_NULL,"_xmat_qrtsolve");
    if (diag->dim < _xmin(A->m,A->n))
        _xerror(_XE_SIZES,"_xmat_qrtsolve");

    sc = _xvec_resize(sc,A->m);
    n = sc->dim;
    p = c->dim;
    k = (n==p ? p-2 : p-1);

    _xvec_zero(sc);
    sc->ve[0] = c->ve[0]/A->me[0][0];
    if (n == 1) return (sc);
    if (p > 1){
        for (i = 1; i < p; i++){
            s = (_xreal)(0.0);
            for (j = 0; j < i; j++)
                s += A->me[j][i]*sc->ve[j];
            if (A->me[i][i] == (_xreal)(0.0))
                _xerror(_XE_SING,"_xmat_qrtsolve");
            sc->ve[i]=(c->ve[i]-s)/A->me[i][i];
        }
    }
    for (i = k; i >= 0; i--){
        s = diag->ve[i]*sc->ve[i];
        for (j = i+1; j < n; j++)
            s += A->me[j][i]*sc->ve[j];
        r_ii = (_xreal)fabs(A->me[i][i]);
        tmp_val = (_xreal)(r_ii*fabs(diag->ve[i]));
        beta = (_xreal)(( tmp_val == (_xreal)(0.0)) ? 0.0 : 1.0/tmp_val);
        tmp_val = beta*s;
        sc->ve[i] -= tmp_val*diag->ve[i];
        for (j = i+1; j < n; j++)
            sc->ve[j] -= tmp_val*A->me[j][i];
    }

    return (sc);
}

/*!
   solve A.x = b where A is factored by _xmat_qrcpfactor()
   -- assumes that A is in the compact factored form
 */
_xvec *_xmat_qrcpsolve(const _xmat *QR, const _xvec *diag, _xperm *pivot, const _xvec *b, _xvec *x){
    _XSTATIC _xvec *tmp = (_xvec *)NULL;
    
    if (!QR || !diag || !pivot || !b)
        _xerror(_XE_NULL,"_xmat_qrcpsolve");
    if ((QR->m > diag->dim &&QR->n > diag->dim) || QR->n != pivot->size)
        _xerror(_XE_SIZES,"_xmat_qrcpsolve");
    
    tmp = _xmat_qrsolve(QR,diag,b,tmp);
    _XMEM_STAT_REG(tmp,_XTYPE_VEC);
    x = _xperm_inv_vec(pivot,tmp,x);

#ifdef _XTHREAD_SAFE
    _xvec_vfree(tmp);
#endif

    return (x);
}

/*!
   given an LU factorisation in A, solve Ax=b
 */
_xvec *_xmat_lusolve(const _xmat *LU, _xperm *pivot, const _xvec *b, _xvec *x){
    if (!LU || !b || !pivot)
        _xerror(_XE_NULL,"_xmat_lusolve");
    if (LU->m != LU->n || LU->n != b->dim)
        _xerror(_XE_SIZES,"_xmat_lusolve");

    x = _xvec_resize(x,b->dim);
    _xperm_vec(pivot,b,x);  /* x := P.b */
    _xmat_lsolve(LU,x,x,1.0); /* implicit diagonal = 1 */
    _xmat_usolve(LU,x,x,0.0); /* explicit diagonal */
    
    return (x);
}

/*!
   solve linear system A.x = b with A factored by _xmat_ldlfactor()
   -- returns x, which is created if it is NULL on entry
 */
_xvec *_xmat_ldlsolve(const _xmat *LDL, const _xvec *b, _xvec *x){
    if (!LDL || !b)
        _xerror(_XE_NULL,"_xmat_ldlsolve");
    if (LDL->m != LDL->n)
        _xerror(_XE_SQUARE,"_xmat_ldlsolve");
    if (LDL->m != b->dim)
        _xerror(_XE_SIZES,"_xmat_ldlsolve");
    x = _xvec_resize(x,b->dim);
    
    _xmat_lsolve(LDL,b,x,1.0);
    _xmat_dsolve(LDL,x,x);
    _xmat_ltsolve(LDL,x,x,1.0);
    
    return (x);
}

/*!
   given a CHolesky factorisation in A, solve A.x=b
 */
_xvec *_xmat_chsolve(const _xmat *A, const _xvec *b, _xvec *x){
    if (!A || !b)
        _xerror(_XE_NULL,"_xmat_chsolve");
    if (A->m != A->n || A->n != b->dim)
        _xerror(_XE_SIZES,"_xmat_chsolve");
    x = _xvec_resize(x,b->dim);
    _xmat_lsolve(A,b,x,0.0);
    _xmat_usolve(A,x,x,0.0);
    
    return (x);
}

/*!
   solve A.x = b where A has been factored a la _xmat_bkpfactor()
   -- returns x, which is created if NULL
 */
_xvec *_xmat_bkpsolve(const _xmat *A, _xperm *pivot, const _xperm *block, const _xvec *b, _xvec *x){
    _XSTATIC _xvec *tmp = (_xvec *)NULL; /* dummy storage needed */
    int i, j, n, onebyone;
    _xreal **A_me, a11, a12, a22, b1, b2, det, sum, *tmp_ve, tmp_diag;
    
    if (!A || !pivot || !block || !b)
        _xerror(_XE_NULL,"_xmat_bkpsolve");
    if (A->m != A->n)
        _xerror(_XE_SQUARE,"_xmat_bkpsolve");
    
    n = A->n;
    if (b->dim != n || pivot->size != n || block->size != n)
        _xerror(_XE_SIZES,"_xmat_bkpsolve");
    
    x = _xvec_resize(x,n);
    tmp = _xvec_resize(tmp,n);
    _XMEM_STAT_REG(tmp,_XTYPE_VEC);
    
    A_me = A->me; tmp_ve = tmp->ve;
    
    _xperm_vec(pivot,b,tmp);
    /* solve for lower triangular part */
    for (i = 0; i < n; i++){
        sum = _xvec_entry(tmp,i);
        if (block->pe[i] < (unsigned int)(i))
            for (j = 0; j < i-1; j++)
                sum -= _xmat_entry(A,i,j)*_xvec_entry(tmp,j);
        else
            for (j = 0; j < i; j++)
                sum -= _xmat_entry(A,i,j)*_xvec_entry(tmp,j);
        _xvec_set_val(tmp,i,sum);
    }
    /* solve for diagonal part */
    for (i = 0; i < n; i = onebyone ? i+1 : i+2){
        onebyone = ( block->pe[i] == i );
        if (onebyone){
            tmp_diag = _xmat_entry(A,i,i);
            if (tmp_diag == (_xreal)(0.0))
                _xerror(_XE_SING,"_xmat_bkpsolve");
            _xvec_set_val(tmp,i,_xvec_entry(tmp,i)/tmp_diag);
        }
        else {
            a11 = _xmat_entry(A,i,i);
            a22 = _xmat_entry(A,i+1,i+1);
            a12 = _xmat_entry(A,i+1,i);
            b1 = _xvec_entry(tmp,i);
            b2 = _xvec_entry(tmp,i+1);
            det = a11*a22-a12*a12;  /* < 0 : see _xmat_bkpfactor() */
            if (det == (_xreal)(0.0))
                _xerror(_XE_SING,"_xmat_bkpsolve");
            det = 1/det;
            _xvec_set_val(tmp,i,det*(a22*b1-a12*b2));
            _xvec_set_val(tmp,i+1,det*(a11*b2-a12*b1));
        }
    }
    /* solve for transpose of lower traingular part */
    for (i = n-1; i >= 0; i--){ /* use symmetry of factored form to get stride 1 */
        sum = _xvec_entry(tmp,i);
        if (block->pe[i] > (unsigned int)(i))
            for (j = i+2; j < n; j++)
                sum -= _xmat_entry(A,i,j)*_xvec_entry(tmp,j);
        else
            for (j = i+1; j < n; j++)
                sum -= _xmat_entry(A,i,j)*_xvec_entry(tmp,j);
        _xvec_set_val(tmp,i,sum);
    }
    /* and do final permutation */
    x = _xperm_inv_vec(pivot,tmp,x);

#ifdef _XTHREAD_SAFE
    _xvec_vfree(tmp);
#endif

    return (x);
}

/*!
   given an LU factorisation in bA, solve bA*x=b
   pivot is changed upon return
 */
_xvec *_xband_lusolve(const _xband *bA, _xperm *pivot, const _xvec *b, _xvec *x){
    int i,j,l,n,n1,pi,lb,ub,jmin, maxj;
    _xreal c, **bA_v;

    if (!bA || !b || !pivot)
        _xerror(_XE_NULL,"_xbdlusolve");
    if (bA->mat->n != b->dim || bA->mat->n != pivot->size)
        _xerror(_XE_SIZES,"_xbdlusolve");
 
    lb = bA->lb;
    ub = bA->ub;
    n = b->dim;
    n1 = n-1;
    bA_v = bA->mat->me;

    x = _xvec_resize(x,b->dim);
    _xperm_vec(pivot,b,x);

   /* solve Lx = b; implicit diagonal = 1 
      L is not permuted, therefore it must be permuted now
    */
   
    _xperm_inv(pivot,pivot);
    for (j=0; j < n; j++){
        jmin = j+1;
        c = x->ve[j];
        maxj = _xmax(0,j+lb-n1);
        for (i=jmin,l=lb-1; l >= maxj; i++,l--){
            if ((pi = pivot->pe[i]) < jmin) 
                pi = pivot->pe[i] = pivot->pe[pi];
            x->ve[pi] -= bA_v[l][j]*c;
        }
    }
    /* solve Ux = b; explicit diagonal */
    x->ve[n1] /= bA_v[lb][n1];
    for (i=n-2; i >= 0; i--){
        c = x->ve[i];
        for (j=_xmin(n1,i+ub), l=lb+j-i; j > i; j--,l--)
            c -= bA_v[l][j]*x->ve[j];
        x->ve[i] = c/bA_v[lb][i];
    }
   
    return (x);
}

/*!
   solve A*x = b, where A is factorized by 
   Choleski LDL^T factorization
 */
_xvec *_xband_ldlsolve(const _xband *A, const _xvec *b, _xvec *x){
    int i,j,l,n,n1,lb,ilb;
    _xreal c, **Av, *Avlb;

    if (!A || !b)
        _xerror(_XE_NULL,"_xbdldlsolve");
    if (A->mat->n != b->dim)
        _xerror(_XE_SIZES,"_xbdldlsolve");

    n = A->mat->n;
    n1 = n-1;
    x = _xvec_resize(x,n);
    lb = A->lb;
    Av = A->mat->me;  
    Avlb = Av[lb];
   
    /* solve L*y = b */
    x->ve[0] = b->ve[0];
    for (i=1; i < n; i++){
        ilb = i-lb;
        c = b->ve[i];
        for (j=_xmax(0,ilb), l=j-ilb; j < i; j++,l++)
            c -= Av[l][j]*x->ve[j];
        x->ve[i] = c;
    }
    /* solve D*z = y */
    for (i=0; i < n; i++) 
        x->ve[i] /= Avlb[i];

    /* solve L^T*x = z */
    for (i=n-2; i >= 0; i--){
        ilb = i+lb;
        c = x->ve[i];
        for (j=_xmin(n1,ilb), l=ilb-j; j > i; j--,l++)
            c -= Av[l][i]*x->ve[j];
        x->ve[i] = c;
    }

    return (x);
}

/*!
   compute Hessenberg factorisation in compact form.
   -- factorisation performed in situ
 */
_xmat *_xmat_hfactor(_xmat *A, _xvec *diag, _xvec *beta){
    _XSTATIC _xvec *hh = (_xvec *)NULL, *w = (_xvec *)NULL;
    int k, limit;
    
    if (!A || !diag || !beta)
        _xerror(_XE_NULL,"_xmat_hfactor");
    if (diag->dim < A->m - 1 || beta->dim < A->m - 1)
        _xerror(_XE_SIZES,"_xmat_hfactor");
    if (A->m != A->n)
        _xerror(_XE_SQUARE,"_xmat_hfactor");
    
    limit = A->m - 1;
    hh = _xvec_resize(hh,A->m);
    w  = _xvec_resize(w,A->n);
    _XMEM_STAT_REG(hh,_XTYPE_VEC);
    _XMEM_STAT_REG(w, _XTYPE_VEC);
    
    for (k = 0; k < limit; k++){
        /* compute the Householder vector hh */
        _xmat_get_col(A,(unsigned int)k,hh);
        _xvec_hhvec(hh,k+1,&beta->ve[k],hh,&A->me[k+1][k]);
        _xvec_set_val(diag,k,_xvec_entry(hh,k+1));
    
        /* apply Householder operation symmetrically to A */
        _xmat_hhvtrcols(A,k+1,k+1,hh,_xvec_entry(beta,k),w);
        _xmat_hhtrrows(A,0  ,k+1,hh,_xvec_entry(beta,k));
    }

#ifdef _XTHREAD_SAFE
    _xvec_vfree(hh); _xvec_vfree(w);
#endif

    return (A);
}

/*!
   Cholesky L.L' factorisation of A in-situ
 */
_xmat *_xmat_chfactor(_xmat *A){
    unsigned int i, j, k, n;
    _xreal **A_ent, *A_piv, *A_row, sum, tmp;
    
    if (!A)
        _xerror(_XE_NULL,"_xmat_chfactor");
    if (A->m != A->n)
        _xerror(_XE_SQUARE,"_xmat_chfactor");
    
    n = A->n; A_ent = A->me;
    for (k=0; k<n; k++){  
        /* do diagonal element */
        sum = A_ent[k][k];
        A_piv = A_ent[k];
        for (j=0; j<k; j++){
            tmp = *A_piv++;
            sum -= tmp*tmp;
        }
        if (sum <= (_xreal)(0.0))
            _xerror(_XE_POSDEF,"_xmat_chfactor");
        A_ent[k][k] = (_xreal)sqrt(sum);
        
        /* set values of column k */
        for (i=k+1; i<n; i++){
            sum = A_ent[i][k];
            A_piv = A_ent[k];
            A_row = A_ent[i];
            sum -= (_xreal)(__xip__(A_row,A_piv,(int)k));
            A_ent[j][i] = A_ent[i][j] = sum/A_ent[k][k];
        }
    }
    
    return (A);
}

/*!
   gaussian elimination with scaled partial pivoting
   -- Note: returns LU matrix which is A
 */
_xmat *_xmat_lufactor(_xmat *A, _xperm *pivot){
    unsigned int i, j, m, n;
    int i_max, k, k_max;
    _xreal **A_v, *A_piv, *A_row, max1, temp, tiny;
    
    _XSTATIC _xvec *scale = (_xvec *)NULL;
    
    if (!A || !pivot)
        _xerror(_XE_NULL,"_xmat_lufactor");
    if (pivot->size != A->m)
        _xerror(_XE_SIZES,"_xmat_lufactor");
    
    m = A->m; n = A->n;
    scale = _xvec_resize(scale,A->m);
    _XMEM_STAT_REG(scale,_XTYPE_VEC);
    
    A_v = A->me;
    tiny = (_xreal)(10.0/_XHUGE_VAL);
    
    /* initialise pivot with identity permutation */
    for (i=0; i<m; i++)
        pivot->pe[i] = i;
    
    /* set scale parameters */
    for (i=0; i<m; i++){
        max1 = (_xreal)(0.0);
        for (j=0; j<n; j++){
            temp = (_xreal)fabs(A_v[i][j]);
            max1 = _xmax(max1,temp);
        }
        scale->ve[i] = max1;
    }
    
    /* main loop */
    k_max = _xmin(m,n)-1;
    for (k=0; k<k_max; k++){
        /* find best pivot row */
        max1 = (_xreal)(0.0); i_max = -1;
        for (i=k; i<m; i++)
            if (fabs(scale->ve[i]) >= tiny*fabs(A_v[i][k])){
                temp = (_xreal)fabs(A_v[i][k])/scale->ve[i];
                if (temp > max1){ 
                    max1 = temp;
                    i_max = i;
                }
            }
        
        /* if no pivot then ignore column k... */
        if (i_max == -1){
            /* set pivot entry A[k][k] exactly to zero,rather than just "small" */
            A_v[k][k] = (_xreal)(0.0);
            continue;
        }
        
        /* do we pivot ? */
        if (i_max != k){/* yes we do... */
            _xperm_transp(pivot,i_max,k);
            for (j=0; j<n; j++){
                temp = A_v[i_max][j];
                A_v[i_max][j] = A_v[k][j];
                A_v[k][j] = temp;
            }
        }
        
        /* row operations */
        for (i=k+1; i<m; i++){/* for each row do... */
            /* Note: divide by zero should never happen */
            temp = A_v[i][k] = A_v[i][k]/A_v[k][k];
            A_piv = &(A_v[k][k+1]);
            A_row = &(A_v[i][k+1]);
            if (k+1 < (int)(n))
                __xmltadd__(A_row,A_piv,-temp,(int)(n-(k+1)));
        }
    }

#ifdef _XTHREAD_SAFE
    _xvec_vfree(scale);
#endif

    return (A);
}

/*!
   form the QR factorisation of A -- factorisation stored in
   compact form as described above ( not quite standard format )
 */
_xmat *_xmat_qrfactor(_xmat *A, _xvec *diag){
    unsigned int k,limit;
    _xreal beta;
    _XSTATIC _xvec *hh=(_xvec *)NULL, *w=(_xvec *)NULL;
    
    if (!A || !diag)
        _xerror(_XE_NULL,"_xmat_qrfactor");

    limit = _xmin(A->m,A->n);
    if (diag->dim < limit)
        _xerror(_XE_SIZES,"_xmat_qrfactor");
    
    hh = _xvec_resize(hh,A->m);
    w  = _xvec_resize(w, A->n);
    _XMEM_STAT_REG(hh,_XTYPE_VEC);
    _XMEM_STAT_REG(w, _XTYPE_VEC);
    
    for (k=0; k<limit; k++){
        /* get H/holder vector for the k-th column */
        _xmat_get_col(A,k,hh);
        _xvec_hhvec(hh,k,&beta,hh,&A->me[k][k]);
        diag->ve[k] = hh->ve[k];
        
        /* apply H/holder vector to remaining columns */
        _xmat_hhvtrcols(A,k,k+1,hh,beta,w);
    }

#ifdef _XTHREAD_SAFE
    _xvec_vfree(hh); _xvec_vfree(w);
#endif

    return (A);
}

/*!
   form the QR factorisation of A with column pivoting
   -- factorisation stored in compact form as described above
   ( not quite standard format )
 */
_xmat *_xmat_qrcpfactor(_xmat *A, _xvec *diag, _xperm *px){
    unsigned int i, i_max, j, k, limit;
    _XSTATIC _xvec *gamma=(_xvec *)NULL, *tmp1=(_xvec *)NULL, *tmp2=(_xvec *)NULL, *w=(_xvec *)NULL;
    _xreal beta, maxgamma, sum, tmp;
    
    if (!A || !diag || !px)
        _xerror(_XE_NULL,"_xmat_qrcpfactor");
    limit = _xmin(A->m,A->n);
    if (diag->dim < limit || px->size != A->n)
        _xerror(_XE_SIZES,"_xmat_qrcpfactor");
    
    tmp1 = _xvec_resize(tmp1,A->m);
    tmp2 = _xvec_resize(tmp2,A->m);
    gamma = _xvec_resize(gamma,A->n);
    w = _xvec_resize(w,A->n);
    _XMEM_STAT_REG(tmp1,_XTYPE_VEC);
    _XMEM_STAT_REG(tmp2,_XTYPE_VEC);
    _XMEM_STAT_REG(gamma,_XTYPE_VEC);
    _XMEM_STAT_REG(w,_XTYPE_VEC);
    
    /* initialise gamma and px */
    for (j=0; j<A->n; j++){
        px->pe[j] = j;
        sum = (_xreal)(0.0);
        for (i=0; i<A->m; i++)
            sum += (_xreal)_xsquare(A->me[i][j]);
        gamma->ve[j] = sum;
    }
    
    for (k=0; k<limit; k++){
        /* find "best" column to use */
        i_max = k; maxgamma = gamma->ve[k];
        for (i=k+1; i<A->n; i++)
            /* Loop invariant:maxgamma=gamma[i_max]>=gamma[l];l=k,...,i-1 */
            if (gamma->ve[i] > maxgamma){
                maxgamma = gamma->ve[i];
                i_max = i;
            }
  
        /* swap columns if necessary */
        if (i_max != k){
            /* swap gamma values */
            tmp = gamma->ve[k];
            gamma->ve[k] = gamma->ve[i_max];
            gamma->ve[i_max] = tmp;
        
            /* update column permutation */
            _xperm_transp(px,k,i_max);
        
            /* swap columns of A */
            for (i=0; i<A->m; i++){
                tmp = A->me[i][k];
                A->me[i][k] = A->me[i][i_max];
                A->me[i][i_max] = tmp;
            }
        }
  
        /* get H/holder vector for the k-th column */
        _xmat_get_col(A,k,tmp1);
        _xvec_hhvec(tmp1,k,&beta,tmp1,&A->me[k][k]);
        diag->ve[k] = tmp1->ve[k];
        
        /* apply H/holder vector to remaining columns */
        _xmat_hhvtrcols(A,k,k+1,tmp1,beta,w);
        
        /* update gamma values */
        for (j=k+1; j<A->n; j++)
            gamma->ve[j] -= (_xreal)_xsquare(A->me[k][j]);
    }

#ifdef _XTHREAD_SAFE
    _xvec_vfree(gamma); _xvec_vfree(tmp1); _xvec_vfree(tmp2); _xvec_vfree(w);
#endif

    return (A);
}

/*!
   Modified Cholesky L.L' factorisation of A in-situ
 */
_xmat *_xmat_mchfactor(_xmat *A, double tol){
    unsigned int i, j, k, n;
    _xreal **A_ent, *A_piv, *A_row, sum, tmp;
    
    if (!A)
        _xerror(_XE_NULL,"_xmat_mchfactor");
    if (A->m != A->n)
        _xerror(_XE_SQUARE,"_xmat_mchfactor");
    if (tol <= (_xreal)(0.0))
        _xerror(_XE_RANGE,"_xmat_mchfactor");
    
    n = A->n; A_ent = A->me;
    for (k=0; k<n; k++){  
        /* do diagonal element */
        sum = A_ent[k][k];
        A_piv = A_ent[k];
        for (j=0; j<k; j++){
            tmp = *A_piv++;
            sum -= tmp*tmp;
        }
        if ((double)(sum) <= tol)
            sum = (_xreal)tol;
        A_ent[k][k] = (_xreal)sqrt(sum);
        
        /* set values of column k */
        for (i=k+1; i<n; i++){
            sum = A_ent[i][k];
            A_piv = A_ent[k];
            A_row = A_ent[i];
            sum -= (_xreal)(__xip__(A_row,A_piv,(int)k));
            A_ent[j][i] = A_ent[i][j] = sum/A_ent[k][k];
        }
    }
    
    return (A);
}

#define _xalpha 0.6403882032022076 /* = (1+sqrt(17))/8 */
/*
  a row/column swap routine, assumed A != NULL & also SQUARE,
  assumed i & j in range
 */
static void _xinterchange(_xmat *A, int i, int j){
    _xreal **A_me, tmp;
    int k, n;
    
    A_me = A->me; n = A->n;
    if (i == j) return;
    if (i > j){
        i ^= j ^= i ^= j; 
    }
    for (k = 0; k < i; k++){
        tmp = _xmat_entry(A,k,i);
        _xmat_set_val(A,k,i,_xmat_entry(A,k,j));
        _xmat_set_val(A,k,j,tmp);
    }
    for (k = j+1; k < n; k++){
        tmp = _xmat_entry(A,j,k);
        _xmat_set_val(A,j,k,_xmat_entry(A,i,k));
        _xmat_set_val(A,i,k,tmp);
    }
    for (k = i+1; k < j; k++){
        tmp = _xmat_entry(A,k,j);
        _xmat_set_val(A,k,j,_xmat_entry(A,i,k));
        _xmat_set_val(A,i,k,tmp);
    }
    
    tmp = _xmat_entry(A,i,i);
    _xmat_set_val(A,i,i,_xmat_entry(A,j,j));
    _xmat_set_val(A,j,j,tmp);
}

/*!
    Bunch-Kaufman-Parlett factorisation of A in-situ
  -- A is factored into the form P'AP = MDM' where 
  P is a permutation matrix, M lower triangular and D is block
  diagonal with blocks of size 1 or 2
  -- P is stored in pivot; blocks[i]==i iff D[i][i] is a block
 */
_xmat *_xmat_bkpfactor(_xmat *A, _xperm *pivot, _xperm *blocks){
    int i, j, k, n, onebyone, r;
    _xreal **A_me, aii, aip1, aip1i, lambda, sigma, tmp;
    _xreal det, s, t;
    
    if (!A || !pivot || !blocks)
        _xerror(_XE_NULL,"_xmat_bkpfactor");
    if (A->m != A->n)
        _xerror(_XE_SQUARE,"_xmat_bkpfactor");
    if (A->m != pivot->size || pivot->size != blocks->size)
        _xerror(_XE_SIZES,"_xmat_bkpfactor");
    
    n = A->n;
    A_me = A->me;
    _xperm_ident(pivot);
    _xperm_ident(blocks);
    
    for (i = 0; i < n; i = onebyone ? i+1 : i+2){
        aii = (_xreal)fabs(_xmat_entry(A,i,i));
        lambda = (_xreal)(0.0);
        r = (i+1 < n) ? i+1 : i;
        for (k = i+1; k < n; k++){
            tmp = (_xreal)fabs(_xmat_entry(A,i,k));
            if (tmp >= lambda){
                lambda = tmp;
                r = k;
            }
        }
        
        /* determine if 1x1 or 2x2 block, and do pivoting if needed */
        if (aii >= _xalpha*lambda){
            onebyone = _XTRUE;
            goto dopivot;
        }
        /* compute sigma */
        sigma = (_xreal)(0.0);
        for (k = i; k < n; k++){
            if (k == r)
                continue;
            tmp = ( k > r ) ? (_xreal)fabs(_xmat_entry(A,r,k)) :
                              (_xreal)fabs(_xmat_entry(A,k,r));
            if (tmp > sigma)
                sigma = tmp;
        }
        if (aii*sigma >= _xalpha*_xsquare(lambda))
            onebyone = _XTRUE;
        else if (fabs(_xmat_entry(A,r,r)) >= _xalpha*sigma){
            _xinterchange(A,i,r);
            _xperm_transp(pivot,i,r);
            onebyone = _XTRUE;
        }
        else {
            _xinterchange(A,i+1,r);
            _xperm_transp(pivot,i+1,r);
            _xperm_transp(blocks,i,i+1);
            onebyone = _XFALSE;
        }
    
    dopivot:
        if (onebyone){ /* do one by one block */
            if (_xmat_entry(A,i,i) != 0.0){
                aii = _xmat_entry(A,i,i);
                for (j = i+1; j < n; j++){
                    tmp = _xmat_entry(A,i,j)/aii;
                    for (k = j; k < n; k++)
                        _xmat_sub_val(A,j,k,tmp*_xmat_entry(A,i,k));
                    _xmat_set_val(A,i,j,tmp);
                }
            }
        }
        else { /* do two by two block */
            det = (_xreal)(_xmat_entry(A,i,i)*_xmat_entry(A,i+1,i+1)-_xsquare(_xmat_entry(A,i,i+1)));
            /* Must have det < 0 */
            aip1i = _xmat_entry(A,i,i+1)/det;
            aii = _xmat_entry(A,i,i)/det;
            aip1 = _xmat_entry(A,i+1,i+1)/det;
            for (j = i+2; j < n; j++){
                s = - aip1i*_xmat_entry(A,i+1,j) + aip1*_xmat_entry(A,i,j);
                t = - aip1i*_xmat_entry(A,i,j) + aii*_xmat_entry(A,i+1,j);
                for (k = j; k < n; k++)
                    _xmat_sub_val(A,j,k,_xmat_entry(A,i,k)*s + _xmat_entry(A,i+1,k)*t);
                _xmat_set_val(A,i,j,s);
                _xmat_set_val(A,i+1,j,t);
            }
        }
    }
    /* set lower triangular half */
    for (i = 0; (unsigned int)(i) < A->m; i++)
        for (j = 0; j < i; j++)
            _xmat_set_val(A,i,j,_xmat_entry(A,j,i));
    
    return (A);
}   
    
/*!
   L.D.L' factorisation of A in-situ
 */
_xmat *_xmat_ldlfactor(_xmat *A){
    unsigned int i, k, n, p;
    _xreal **A_ent,d,sum;
    _XSTATIC _xvec *r = (_xvec *)NULL;
    
    if (!A)
        _xerror(_XE_NULL,"_xmat_ldlfactor");
    if (A->m != A->n)
        _xerror(_XE_SQUARE,"_xmat_ldlfactor");
    
    n = A->n; A_ent = A->me;
    r = _xvec_resize(r,n);
    _XMEM_STAT_REG(r,_XTYPE_VEC);
    
    for (k = 0; k < n; k++){
        sum = (_xreal)(0.0);
        for (p = 0; p < k; p++){
            r->ve[p] = A_ent[p][p]*A_ent[k][p];
            sum += r->ve[p]*A_ent[k][p];
        }
        d = A_ent[k][k] -= sum;
        
        if (d == 0.0)
            _xerror(_XE_SING,"_xmat_ldlfactor");
        for (i = k+1; i < n; i++){
            sum = (_xreal)(__xip__(A_ent[i],r->ve,(int)k));
            A_ent[i][k] = (A_ent[i][k] - sum)/d;
        }
    }

#ifdef _XTHREAD_SAFE
    _xvec_vfree(r);
#endif

    return (A);
}

/*!
   gaussian elimination with partial pivoting
   -- on entry, the matrix A in band storage with elements 
      in rows 0 to lb+ub; 
      The jth column of A is stored in the jth column of 
      band A (bA) as follows:
      bA->mat->me[lb+j-i][j] = A->me[i][j] for 
      max(0,j-lb) <= i <= min(A->n-1,j+ub);
   -- on exit: U is stored as an upper triangular matrix
      with lb+ub superdiagonals in rows lb to 2*lb+ub, 
      and the matrix L is stored in rows 0 to lb-1.
      Matrix U is permuted, whereas L is not permuted !!!
      Therefore we save some memory.
 */
_xband *_xband_lufactor(_xband *bA, _xperm *pivot){
    int i, j, k, l, n, n1, lb, ub, lub, k_end, k_lub;
    int i_max, shift;
    _xreal **bA_v, max1, temp;
   
    if (!bA || !pivot)
        _xerror(_XE_NULL,"_xband_lufactor");

    lb = bA->lb;
    ub = bA->ub;
    lub = lb+ub;
    n = bA->mat->n;
    n1 = n-1;
    lub = lb+ub;

    if (pivot->size != n)
        _xerror(_XE_SIZES,"_xband_lufactor");

   
    /* initialise pivot with identity permutation */
    for (i=0; i < n; i++)
        pivot->pe[i] = i;

    /* extend band matrix */
    /* extended part is filled with zeros */
    bA = _xband_resize(bA,lb,_xmin(n1,lub),n);
    bA_v = bA->mat->me;


    /* main loop */
    for (k=0; k < n1; k++){
        k_end = _xmax(0,lb+k-n1);
        k_lub = _xmin(k+lub,n1);

        /* find the best pivot row */
        max1 = (_xreal)(0.0); 
        i_max = -1;
        for (i=lb; i >= k_end; i--){
            temp = (_xreal)fabs(bA_v[i][k]);
            if (temp > max1){
                max1 = temp;
                i_max = i; 
            }
        }
      
        /* if no pivot then ignore column k... */
        if (i_max == -1)
            continue;
      
        /* do we pivot ? */
        if (i_max != lb){/* yes we do... */
            /* save transposition using non-shifted indices */
            shift = lb-i_max;
            _xperm_transp(pivot,k+shift,k);
            for (i=lb, j=k; j <= k_lub; i++,j++){
                temp = bA_v[i][j];
                bA_v[i][j] = bA_v[i-shift][j];
                bA_v[i-shift][j] = temp;
            }
        }
      
        /* row operations */
        for (i=lb-1; i >= k_end; i--){
            temp = bA_v[i][k] /= bA_v[lb][k];
            shift = lb-i;
            for (j=k+1,l=i+1; j <= k_lub; l++,j++)
                bA_v[l][j] -= temp*bA_v[l+shift][j];
        }
    }
   
    return (bA);
}

/*!
   L.D.L' factorisation of A in-situ;
   A is a band matrix
   it works using only lower bandwidth & main diagonal
   so it is possible to set A->ub = 0
 */
_xband *_xband_ldlfactor(_xband *A){
    int i,j,k,n,n1,lb,ki,jk,ji,lbkm,lbkp;
    _xreal **Av, c, cc;

    if (!A)
        _xerror(_XE_NULL,"_xband_ldlfactor");

    if (A->lb == 0) return (A);

    lb = A->lb;
    n = A->mat->n;
    n1 = n-1;
    Av = A->mat->me;
   
    for (k=0; k < n; k++){    
        lbkm = lb-k;
        lbkp = lb+k;

        /* matrix D */
        c = Av[lb][k];
        for (j=_xmax(0,-lbkm), jk=lbkm+j; j < k; j++, jk++){
            cc = Av[jk][j];
            c -= Av[lb][j]*cc*cc;
        }
        if (c == 0.0)
            _xerror(_XE_SING,"_xband_ldlfactor");
        Av[lb][k] = c;

        /* matrix L */
        for (i=_xmin(n1,lbkp), ki=lbkp-i; i > k; i--,ki++){
            c = Av[ki][k];
            for (j=_xmax(0,i-lb), ji=lb+j-i, jk=lbkm+j; j < k;j++, ji++, jk++)
                c -= Av[lb][j]*Av[ji][j]*Av[jk][j];
            Av[ki][k] = c/Av[lb][k];
        }
    }
   
    return (A);
}

/*!
   return inverse of A, provided A is not too rank deficient
   -- uses LU factorisation
 */
_xmat *_xmat_inverse(const _xmat *A, _xmat *out){
    int i;
    _XSTATIC _xvec *tmp = (_xvec *)NULL, *tmp2 = (_xvec *)NULL;
    _XSTATIC _xmat *A_cp = (_xmat *)NULL;
    _XSTATIC _xperm *pivot = (_xperm *)NULL;
    
    if (!A)
        _xerror(_XE_NULL,"_xmat_inverse");
    if (A->m != A->n)
        _xerror(_XE_SQUARE,"_xmat_inverse");
    if (!out || out->m < A->m || out->n < A->n)
        out = _xmat_resize(out,A->m,A->n);
    
    A_cp = _xmat_resize(A_cp,A->m,A->n);
    A_cp = _xmat_mcopy(A,A_cp);
    tmp = _xvec_resize(tmp,A->m);
    tmp2 = _xvec_resize(tmp2,A->m);
    pivot = _xperm_resize(pivot,A->m);
    _XMEM_STAT_REG(A_cp,_XTYPE_MAT);
    _XMEM_STAT_REG(tmp,_XTYPE_VEC);
    _XMEM_STAT_REG(tmp2,_XTYPE_VEC);
    _XMEM_STAT_REG(pivot,_XTYPE_PERM);
    
    _xtracecatch(_xmat_lufactor(A_cp,pivot),"_xmat_inverse");
    for (i = 0; (unsigned int)i < A->n; i++){
        _xvec_zero(tmp);
        tmp->ve[i] = (_xreal)(1.0);
        _xtracecatch(_xmat_lusolve(A_cp,pivot,tmp,tmp2),"_xmat_inverse");
        _xmat_mset_col(out,i,tmp2);
    }

#ifdef _XTHREAD_SAFE
    _xvec_vfree(tmp); _xvec_vfree(tmp2);
    _xmat_mfree(A_cp);_xperm_pfree(pivot);
#endif

    return (out);
}

/*!
   compute integer powers of a square matrix A, A^p
   -- uses tmp as temporary workspace
 */
_xmat *_xmat_vpow(const _xmat *A, int p, _xmat *tmp, _xmat *out){
    int it_cnt, k, max_bit;

#define _XZ(k) (((k) & 1) ? tmp : out)
   
    if (!A)
        _xerror(_XE_NULL,"_xmat_vpow");
    if (A->m != A->n)
        _xerror(_XE_SQUARE,"_xmat_vpow");
    if (p < 0)
        _xerror(_XE_NEG,"_xmat_vpow");

    out = _xmat_resize(out,A->m,A->n);
    tmp = _xmat_resize(tmp,A->m,A->n);
   
    if (p == 0)
        _xmat_ident(out);
    else if (p > 0){
        it_cnt = 1;
        for (max_bit = 0;; max_bit++)
            if ((p >> (max_bit+1)) == 0)
                break;
        tmp = _xmat_mcopy(A,tmp);
      
        for (k = 0; k < max_bit; k++){
            _xmat_mlt(_XZ(it_cnt),_XZ(it_cnt),_XZ(it_cnt+1));
            it_cnt++;
            if (p & (1 << (max_bit-1))){
                _xmat_mlt(A,_XZ(it_cnt),_XZ(it_cnt+1));
                it_cnt++;
            }
            p <<= 1;
        }
        if (it_cnt & 1)
            out = _xmat_mcopy(_XZ(it_cnt),out);
    }

    return out;

#undef _XZ   
}

/*!
   compute integer powers of a square matrix A, A^p
 */
_xmat *_xmat_pow(const _xmat *A, int p, _xmat *out){
    _XSTATIC _xmat *wkspace=(_xmat *)NULL, *tmp=(_xmat *)NULL;
   
    if (!A)
        _xerror(_XE_NULL,"_xmat_pow");
    if (A->m != A->n)
        _xerror(_XE_SQUARE,"_xmat_pow");
   
    wkspace = _xmat_resize(wkspace,A->m,A->n);
    _XMEM_STAT_REG(wkspace,_XTYPE_MAT);
    if (p < 0){
        tmp = _xmat_resize(tmp,A->m,A->n);
        _XMEM_STAT_REG(tmp,_XTYPE_MAT);
        _xtracecatch(_xmat_inverse(A,tmp),"_xmat_pow");
        out = _xmat_vpow(tmp, -p, wkspace, out);
    }
    else
        out = _xmat_vpow(A, p, wkspace, out);

#ifdef _XTHREAD_SAFE
    _xmat_mfree(wkspace); _xmat_mfree(tmp);
#endif

    return (out);
}

/*!
   compute matrix exponential of A and save it in out
   -- uses Pade approximation followed by repeated squaring
   -- eps is the tolerance used for the Pade approximation 
   -- A is not changed
   -- q_out - degree of the Pade approximation (q_out,q_out)
   -- j_out - the power of 2 for scaling the matrix A
              such that ||A/2^j_out|| <= 0.5
 */
_xmat *_xmat_vexp(const _xmat *A, double eps, _xmat *out, int *q_out, int *j_out){
    _XSTATIC _xmat *D = (_xmat *)NULL, *Apow = (_xmat *)NULL, *N = (_xmat *)NULL, *Y = (_xmat *)NULL;
    _XSTATIC _xvec *c1 = (_xvec *)NULL, *tmp = (_xvec *)NULL;
    _XSTATIC _xperm *pivot = (_xperm *)NULL;

    _xvec y0, y1;  /* additional structures */
    int j, k, l, q, r, s, j2max, t;
    double inf_norm, eqq, power2, c, sign;
   
    if (!A)
        _xerror(_XE_SIZES,"_xmat_vexp");
    if (A->m != A->n)
        _xerror(_XE_SIZES,"_xmat_vexp");
    if (A == out)
        _xerror(_XE_INSITU,"_xmat_vexp");
    if (eps < 0.0)
        _xerror(_XE_RANGE,"_xmat_vexp");
    else if (eps == 0.0)
        eps = _XMACHEPS;
      
    N = _xmat_resize(N,A->m,A->n);
    D = _xmat_resize(D,A->m,A->n);
    Apow = _xmat_resize(Apow,A->m,A->n);
    out = _xmat_resize(out,A->m,A->n);

    _XMEM_STAT_REG(N,_XTYPE_MAT);
    _XMEM_STAT_REG(D,_XTYPE_MAT);
    _XMEM_STAT_REG(Apow,_XTYPE_MAT);
   
    /* normalise A to have ||A||_inf <= 1 */
    inf_norm = _xmat_norm_inf(A);
    if (inf_norm <= 0.0){
        _xmat_ident(out);
        *q_out = -1;
        *j_out = 0;
        return (out);
    }
    else {
        j2max = (int)floor(1+log(inf_norm)/log(2.0));
        j2max = _xmax(0, j2max);
    }
   
    power2 = 1.0;
    for (k = 1; k <= j2max; k++)
        power2 *= 2;
    power2 = 1.0/power2;
    if (j2max > 0)
        _xmat_sm_mlt(power2,A,(_xmat *)A);
   
    /* compute order for polynomial approximation */
    eqq = 1.0/6.0;
    for (q = 1; eqq > eps; q++)
        eqq /= 16.0*(2.0*q+1.0)*(2.0*q+3.0);
   
    /* construct vector of coefficients */
    c1 = _xvec_resize(c1,q+1);
    _XMEM_STAT_REG(c1,_XTYPE_VEC);
    c1->ve[0] = 1.0;
    for (k = 1; k <= q; k++) 
        c1->ve[k] = (_xreal)(c1->ve[k-1]*(q-k+1)/((2*q-k+1)*(double)k));
   
    tmp = _xvec_resize(tmp,A->n);
    _XMEM_STAT_REG(tmp,_XTYPE_VEC);
   
    s = (int)floor(sqrt((double)q/2.0));
    if ( s <= 0 ) s = 1;
    _xmat_vpow(A,s,out,Apow);
    r = q/s;
   
    Y = _xmat_resize(Y,s,A->n);
    _XMEM_STAT_REG(Y,_XTYPE_MAT);
    /* y0 and y1 are pointers to rows of Y, N and D */
    y0.dim = y0.max_dim = A->n;   
    y1.dim = y1.max_dim = A->n;
   
    _xmat_zero(Y);
    _xmat_zero(N);
    _xmat_zero(D);
   
    for(j = 0; (unsigned int)j < A->n; j++){
        if (j > 0)
            Y->me[0][j-1] = 0.0;
        y0.ve = Y->me[0];
        y0.ve[j] = 1.0;
        for (k = 0; k < s-1; k++){
            y1.ve = Y->me[k+1];
            _xmat_mv_mlt(A,&y0,&y1);
            y0.ve = y1.ve;
        }

        y0.ve = N->me[j];
        y1.ve = D->me[j];
        t = s*r;
        for (l = 0; l <= q-t; l++){
            c = c1->ve[t+l];
            sign = ((t+l) & 1) ? -1.0 : 1.0;
            __xmltadd__(y0.ve,Y->me[l],c,Y->n);
            __xmltadd__(y1.ve,Y->me[l],c*sign,Y->n);
        }
      
        for (k=1; k <= r; k++){
            _xvec_vcopy(_xmat_mv_mlt(Apow,&y0,tmp),&y0);
            _xvec_vcopy(_xmat_mv_mlt(Apow,&y1,tmp),&y1);
            t = s*(r-k);
            for (l=0; l < s; l++){
                c = c1->ve[t+l];
                sign = ((t+l) & 1) ? -1.0 : 1.0;
                __xmltadd__(y0.ve,Y->me[l],c,Y->n);
                __xmltadd__(y1.ve,Y->me[l],c*sign,Y->n);
            }
        }
    }

    pivot = _xperm_resize(pivot,A->m);
    _XMEM_STAT_REG(pivot,_XTYPE_PERM);
   
    /* note that N and D are transposed,
       therefore we use LUTsolve;
       out is saved row-wise, and must be transposed 
       after this */

    _xmat_lufactor(D,pivot);
    for (k=0; (unsigned int)k < A->n; k++){
        y0.ve = N->me[k];
        y1.ve = out->me[k];
        _xmat_lutsolve(D,pivot,&y0,&y1);
    }
    _xmat_transp(out,out); 

    /* Use recursive squaring to turn the normalised exponential to the
       true exponential */

#define _XZ(k) ((k) & 1 ? Apow : out)

    for(k = 1; k <= j2max; k++)
        _xmat_mlt(_XZ(k-1),_XZ(k-1),_XZ(k));

    if (_XZ(k) == out)
        _xmat_mcopy(Apow,out);
   
    /* output parameters */
    *j_out = j2max;
    *q_out = q;

    /* restore the matrix A */
    _xmat_sm_mlt(1.0/power2,A,(_xmat *)A);

#ifdef _XTHREAD_SAFE
    _xmat_mfree(D); _xmat_mfree(Apow); _xmat_mfree(N); _xmat_mfree(Y);
    _xvec_vfree(c1); _xvec_vfree(tmp);
    _xperm_pfree(pivot);
#endif

    return (out);

#undef _XZ
}

/*!
   simple interface for _xmat_vexp
 */
_xmat *_xmat_exp(const _xmat *A, double eps, _xmat *out){
    int q_out, j_out;
    return _xmat_vexp(A,eps,out,&q_out,&j_out);
}

/*!
   compute sum_i a[i].A^i, where i=0,1,...dim(a);
   -- uses C. Van Loan's fast and memory efficient method
 */
_xmat *_xmat_poly(const _xmat *A, const _xvec *a, _xmat *out){
    _XSTATIC _xmat *Apow = (_xmat *)NULL, *Y = (_xmat *)NULL;
    _XSTATIC _xvec *tmp = (_xvec *)NULL;
    _xvec y0, y1;  /* additional vectors */
    int j, k, l, q, r, s, t;
   
    if (!A || !a)
        _xerror(_XE_NULL,"_xmat_poly");
    if (A->m != A->n)
        _xerror(_XE_SIZES,"_xmat_poly");
    if (A == out)
        _xerror(_XE_INSITU,"_xmat_poly");
   
    out = _xmat_resize(out,A->m,A->n);
    Apow = _xmat_resize(Apow,A->m,A->n);
    _XMEM_STAT_REG(Apow,_XTYPE_MAT);
    tmp = _xvec_resize(tmp,A->n);
    _XMEM_STAT_REG(tmp,_XTYPE_VEC);

    q = a->dim - 1;
    if (q == 0){
        _xmat_zero(out);
        for (j=0; (unsigned int)j < out->n; j++)
            out->me[j][j] = a->ve[0];
        return (out);
    }
    else if (q == 1){
        _xmat_sm_mlt(a->ve[1],A,out);
        for (j=0; (unsigned int)j < out->n; j++)
            out->me[j][j] += a->ve[0];
        return (out);
    }
   
    s = (int)floor(sqrt((double)q/2.0));
    if (s <= 0) s = 1;
    _xmat_vpow(A,s,out,Apow);
    r = q/s;
   
    Y = _xmat_resize(Y,s,A->n);
    _XMEM_STAT_REG(Y,_XTYPE_MAT);
    /* pointers to rows of Y */
    y0.dim = y0.max_dim = A->n;
    y1.dim = y1.max_dim = A->n;

    _xmat_zero(Y);
    _xmat_zero(out);
   
#define _XZ(k)  ((k) & 1 ? tmp : &y0)
#define _XZZ(k) ((k) & 1 ? tmp->ve : y0.ve)

    for( j = 0; (unsigned int)j < A->n; j++){
        if (j > 0 )
            Y->me[0][j-1] = 0.0;
        Y->me[0][j] = 1.0;
        y0.ve = Y->me[0];
        for (k = 0; k < s-1; k++){
            y1.ve = Y->me[k+1];
            _xmat_mv_mlt(A,&y0,&y1);
            y0.ve = y1.ve;
        }
        y0.ve = out->me[j];
        t = s*r;
        for (l = 0; l <= q-t; l++)
            __xmltadd__(y0.ve,Y->me[l],a->ve[t+l],Y->n);
      
        for (k=1; k <= r; k++){
            _xmat_mv_mlt(Apow,_XZ(k-1),_XZ(k)); 
            t = s*(r-k);
            for (l=0; l < s; l++)
                __xmltadd__(_XZZ(k),Y->me[l],a->ve[t+l],Y->n);
        }
        if (_XZ(k) == &y0) _xvec_vcopy(tmp,&y0);
    }

    _xmat_transp(out,out);

#ifdef _XTHREAD_SAFE
    _xmat_mfree(Apow); _xmat_mfree(Y); _xvec_vfree(tmp);  
#endif
   
    return (out);

#undef _XZ
#undef _XZZ
}

/*!
   d.i.t. fast Fourier transform 
        -- radix-2 FFT only
        -- vector extended to a power of 2
 */
void _xvec_fft(_xvec *x_re, _xvec *x_im){
    int i, ip, j, k, li, n, length;
    _xreal *xr, *xi;
    _xreal theta, pi = (_xreal)3.1415926535897932384;
    _xreal w_re, w_im, u_re, u_im, t_re, t_im;
    _xreal tmp, tmpr, tmpi;

    if (!x_re || !x_im)
        _xerror(_XE_NULL,"_xvec_fft");
    if (x_re->dim != x_im->dim)
        _xerror(_XE_SIZES,"_xvec_fft");

    n = 1;
    while (x_re->dim > (unsigned int)n) n *= 2;
    x_re = _xvec_resize(x_re,n);
    x_im = _xvec_resize(x_im,n);

    xr = x_re->ve;
    xi = x_im->ve;

    /* Decimation in time (DIT) algorithm */
    j = 0;
    for (i = 0; i < n-1; i++){
        if (i < j){
            tmp = xr[i];
            xr[i] = xr[j];
            xr[j] = tmp;
            tmp = xi[i];
            xi[i] = xi[j];
            xi[j] = tmp;
        }
        k = n / 2;
        while (k <= j){
            j -= k;
            k /= 2;
        }
        j += k;
    }

    /* Actual FFT */
    for (li = 1; li < n; li *= 2){
        length = 2*li;
        theta  = pi/li;
        u_re   = 1.0;
        u_im   = 0.0;
        if (li == 1){
            w_re = -1.0;
            w_im =  0.0;
        }
        else if (li == 2){
            w_re =  0.0;
            w_im =  1.0;
        }
        else {
            w_re = (_xreal)cos(theta);
            w_im = (_xreal)sin(theta);
        }
        for (j = 0; j < li; j++){
            for (i =  j; i < n; i += length){
                ip = i + li;
                /* step 1 */
                t_re = xr[ip]*u_re - xi[ip]*u_im;
                t_im = xr[ip]*u_im + xi[ip]*u_re;
                /* step 2 */
                xr[ip] = xr[i]  - t_re;
                xi[ip] = xi[i]  - t_im;
                /* step 3 */
                xr[i] += t_re;
                xi[i] += t_im;
            }
            tmpr = u_re*w_re - u_im*w_im;
            tmpi = u_im*w_re + u_re*w_im;
            u_re = tmpr;
            u_im = tmpi;
        }
    }
}

/*!
   inverse FFT using the same interface as _xvec_fft()
 */
void _xvec_ifft(_xvec *x_re, _xvec *x_im){
    /* we just use complex conjugates */
    _xvec_sv_mlt(-1.0,x_im,x_im);
    _xvec_fft(x_re,x_im);
    _xvec_sv_mlt(-1.0/((double)(x_re->dim)),x_im,x_im);
    _xvec_sv_mlt( 1.0/((double)(x_re->dim)),x_re,x_re);
}

/*!
   return an estimate of the condition number of LU given the
   LU factorisation in compact form
 */
double _xmat_lucondest(const _xmat *LU, _xperm *pivot){
    _XSTATIC _xvec *y = (_xvec *)NULL, *z = (_xvec *)NULL;
    _xreal cond_est, L_norm, U_norm, sum, tiny;
    int i, j, n;

    if (!LU || !pivot)
        _xerror(_XE_NULL,"_xmat_lucondest");
    if (LU->m != LU->n)
        _xerror(_XE_SQUARE,"_xmat_lucondest");
    if (LU->n != pivot->size)
        _xerror(_XE_SIZES,"_xmat_lucondest");

    tiny = (_xreal)(10.0/_XHUGE_VAL);

    n = LU->n;
    y = _xvec_resize(y,n);
    z = _xvec_resize(z,n);
    _XMEM_STAT_REG(y,_XTYPE_VEC);
    _XMEM_STAT_REG(z,_XTYPE_VEC);

    for (i = 0; i < n; i++){
        sum = 0.0;
        for (j = 0; j < i; j++)
            sum -= LU->me[j][i]*y->ve[j];
        sum -= (sum < 0.0) ? (_xreal)1.0 : -(_xreal)1.0;
        if (fabs(LU->me[i][i]) <= tiny*fabs(sum))
            return (_XHUGE_VAL);
        y->ve[i] = sum / LU->me[i][i];
    }

    _xcatchone(_XE_SING,
    _xmat_ltsolve(LU,y,y,1.0);
    _xmat_lusolve(LU,pivot,y,z);,
    return (_XHUGE_VAL));

    /* now estimate norm of A (even though it is not directly available) */
    /* actually computes ||L||_inf.||U||_inf */
    U_norm = 0.0;
    for (i = 0; i < n; i++){
        sum = 0.0;
        for (j = i; j < n; j++)
            sum += (_xreal)fabs(LU->me[i][j]);
        if (sum > U_norm)
            U_norm = sum;
    }
    L_norm = 0.0;
    for (i = 0; i < n; i++){
        sum = 1.0;
        for (j = 0; j < i; j++)
            sum += (_xreal)fabs(LU->me[i][j]);
        if (sum > L_norm)
            L_norm = sum;
    }

    _xtracecatch(cond_est = (_xreal)(U_norm*L_norm*_xvec_vnorm_inf(z)/_xvec_vnorm_inf(y)),
    "_xmat_lucondest");

#ifdef _XTHREAD_SAFE
    _xvec_vfree(y); _xvec_vfree(z);
#endif

    return (double)(cond_est);
}

/*
   compute out = upper_triang(U).x
  -- may be in situ
 */
static _xvec *_xumlt(const _xmat *U, const _xvec *x, _xvec *out){
    unsigned int i, limit;

    if (!U || !x)
        _xerror(_XE_NULL,"_xumlt");
    limit = _xmin(U->m,U->n);
    if (limit != x->dim)
        _xerror(_XE_SIZES,"_xumlt");
    if (!out || out->dim < limit)
        out = _xvec_resize(out,limit);

    for (i = 0; i < limit; i++)
        out->ve[i] = (_xreal)(__xip__(&(x->ve[i]),&(U->me[i][i]),limit - i));
    return (out);
}

/*
  return out = upper_triang(U)^T.x 
 */
static _xvec *_xutmlt(const _xmat *U, const _xvec *x, _xvec *out){
    _xreal sum;
    int i, j, limit;

    if (!U || !x)
        _xerror(_XE_NULL,"_xutmlt");
    limit = _xmin(U->m,U->n);
    if (!out || out->dim < (unsigned int)limit)
        out = _xvec_resize(out,limit);

    for (i = limit-1; i >= 0; i--){
        sum = 0.0;
        for (j = 0; j <= i; j++)
            sum += U->me[j][i]*x->ve[j];
        out->ve[i] = sum;
    }

    return (out);
}

/*!
   return an estimate of the 2-norm condition number of the
    matrix factorised by _xmat_qrfactor() or _xmat_qrcpfactor()
  -- note that as Q does not affect the 2-norm condition number,
    it is not necessary to pass the diag, beta (or pivot) vectors
  -- generates a lower bound on the true condition number
  -- if the matrix is exactly singular, _XHUGE_VAL is returned
  -- note that _xmat_qrcondest() is likely to be more reliable for
    matrices factored using _xmat_qrcpfactor()
 */
double _xmat_qrcondest(const _xmat *QR){
    _XSTATIC _xvec *y=(_xvec *)NULL;
    _xreal norm1, norm2, sum, tmp1, tmp2;
    int i, j, limit;

    if (!QR)
        _xerror(_XE_NULL,"_xmat_qrcondest");

    limit = _xmin(QR->m,QR->n);
    for (i = 0; i < limit; i++)
        if (QR->me[i][i] == 0.0)
            return (double)(_XHUGE_VAL);

    y = _xvec_resize(y,limit);
    _XMEM_STAT_REG(y,_XTYPE_VEC);
    /* use the trick for getting a unit vector y with ||R.y||_inf small
       from the LU condition estimator */
    for (i = 0; i < limit; i++){
        sum = 0.0;
        for (j = 0; j < i; j++)
            sum -= QR->me[j][i]*y->ve[j];
        sum -= (sum < 0.0) ? (_xreal)1.0 : -(_xreal)1.0;
        y->ve[i] = sum / QR->me[i][i];
    }
    _xutmlt(QR,y,y);

    /* now apply inverse power method to R^T.R */
    for (i = 0; i < 3; i++){
        tmp1 = (_xreal)_xvec_vnorm2(y);
        _xvec_sv_mlt(1/tmp1,y,y);
        _xmat_utsolve(QR,y,y,0.0);
        tmp2 = (_xreal)_xvec_vnorm2(y);
        _xvec_sv_mlt(1/_xvec_vnorm2(y),y,y);
        _xmat_usolve(QR,y,y,0.0);
    }
    /* now compute approximation for ||R^{-1}||_2 */
    norm1 = (_xreal)(sqrt(tmp1)*sqrt(tmp2));
    /* now use complementary approach to compute approximation to ||R||_2 */
    for (i = limit-1; i >= 0; i--){
        sum = 0.0;
        for (j = i+1; j < limit; j++)
            sum += QR->me[i][j]*y->ve[j];
        y->ve[i] = (sum >= 0.0) ? (_xreal)1.0 : -(_xreal)1.0;
        y->ve[i] = (QR->me[i][i] >= 0.0) ? y->ve[i] : - y->ve[i];
    }
    /* now apply power method to R^T.R */
    for (i = 0; i < 3; i++){
        tmp1 = (_xreal)_xvec_vnorm2(y);
        _xvec_sv_mlt(1/tmp1,y,y);
        _xumlt(QR,y,y);
        tmp2 = (_xreal)_xvec_vnorm2(y);
        _xvec_sv_mlt(1/tmp2,y,y);
        _xutmlt(QR,y,y);
    }
    norm2 = (_xreal)(sqrt(tmp1)*sqrt(tmp2));

#ifdef _XTHREAD_SAFE
    _xvec_vfree(y);
#endif

    return (double)(norm1*norm2);
}

/*!
    updates a CHolesky factorisation, replacing LDL' by
  MD~M' = LDL' + alpha.w.w' Note: w is overwritten
  Ref: Gill et al Math Comp 28, p516 Algorithm C1
 */
_xmat *_xmat_ldlupdate(_xmat *CHmat, _xvec *w, double alpha){
    unsigned int i,j;
    _xreal diag,new_diag,beta,p;
    
    if (!CHmat || !w)
        _xerror(_XE_NULL,"_xmat_ldlupdate");
    if (CHmat->m != CHmat->n || w->dim != CHmat->m)
        _xerror(_XE_SIZES,"_xmat_ldlupdate");
    
    for (j=0; j < w->dim; j++){
        p = w->ve[j];
        diag = CHmat->me[j][j];
        new_diag = CHmat->me[j][j] = (_xreal)(diag + alpha*p*p);
        if (new_diag <= 0.0)
            _xerror(_XE_POSDEF,"_xmat_ldlupdate");
        beta = (_xreal)(p*alpha/new_diag);
        alpha *= diag/new_diag;
        
        for (i=j+1; i < w->dim; i++){
            w->ve[i] -= p*CHmat->me[i][j];
            CHmat->me[i][j] += beta*w->ve[i];
            CHmat->me[j][i] = CHmat->me[i][j];
        }
    }
    
    return (CHmat);
}

/*!
   construct actual Hessenberg matrix
 */
_xmat *_xmat_makeh(const _xmat *H, _xmat *Hout){
    int i, j, limit;

    if (!H)
        _xerror(_XE_NULL,"_xmat_makeh");
    if (H->m != H->n)
        _xerror(_XE_SQUARE,"_xmat_makeh");
    Hout = _xmat_resize(Hout,H->m,H->m);
    Hout = _xmat_mcopy(H,Hout);
    
    limit = H->m;
    for (i = 1; i < limit; i++)
        for (j = 0; j < i-1; j++)
            _xmat_set_val(Hout,i,j,0.0);
    
    return (Hout);
}

/*!
   construct orthogonal matrix from Householder vectors stored in
   compact QR form
 */
_xmat *_xmat_makeq(const _xmat *QR, const _xvec *diag, _xmat *Qout){
    _XSTATIC _xvec *tmp1=(_xvec *)NULL,*tmp2=(_xvec *)NULL;
    unsigned int i, limit;
    _xreal beta, r_ii, tmp_val;
    int j;
    
    limit = _xmin(QR->m,QR->n);
    if (!QR || !diag)
        _xerror(_XE_NULL,"_xmat_makeq");
    if (diag->dim < limit)
        _xerror(_XE_SIZES,"_xmat_makeq");
    if (!Qout || Qout->m < QR->m || Qout->n < QR->m)
        Qout = _xmat_get(QR->m,QR->m);
    
    tmp1 = _xvec_resize(tmp1,QR->m);  /* contains basis vec & columns of Q */
    tmp2 = _xvec_resize(tmp2,QR->m);  /* contains H/holder vectors */
    _XMEM_STAT_REG(tmp1,_XTYPE_VEC);
    _XMEM_STAT_REG(tmp2,_XTYPE_VEC);
    
    for (i=0; i<QR->m ; i++){ 
        /* get i-th column of Q */
        /* set up tmp1 as i-th basis vector */
        for (j=0; (unsigned int)j<QR->m ; j++)
            tmp1->ve[j] = 0.0;
        tmp1->ve[i] = 1.0;
        
        /* apply H/h transforms in reverse order */
        for (j=limit-1; j>=0; j--){
            _xmat_get_col(QR,j,tmp2);
            r_ii = (_xreal)fabs(tmp2->ve[j]);
            tmp2->ve[j] = diag->ve[j];
            tmp_val = (_xreal)(r_ii*fabs(diag->ve[j]));
            beta = (_xreal)(( tmp_val == 0.0 ) ? 0.0 : 1.0/tmp_val);
            _xvec_hhtrvec(tmp2,beta,j,tmp1,tmp1);
        }
  
        /* insert into Q */
        _xmat_mset_col(Qout,i,tmp1);
    }

#ifdef _XTHREAD_SAFE
    _xvec_vfree(tmp1); _xvec_vfree(tmp2);
#endif

    return (Qout);
}

/*!
   construct upper triangular matrix from QR (compact form)
   -- may be in-situ (all it does is zero the lower 1/2)
 */
_xmat *_xmat_maker(const _xmat *QR, _xmat *Rout){
    unsigned int i,j;
    
    if (!QR)
        _xerror(_XE_NULL,"_xmat_maker");
    Rout = _xmat_mcopy(QR,Rout);
    
    for (i=1; i<QR->m; i++)
        for (j=0; j<QR->n && j<i; j++)
            Rout->me[i][j] = 0.0;
    
    return (Rout);
}

/*!
    construct the Hessenberg orthogonalising matrix Q;
  -- i.e. Hess M = Q.M.Q'
 */
_xmat *_xmat_makehq(_xmat *H, _xvec *diag, _xvec *beta, _xmat *Qout){
    int i, j, limit;
    _XSTATIC _xvec *tmp1 = (_xvec *)NULL, *tmp2 = (_xvec *)NULL;

    if (!H || !diag || !beta)
        _xerror(_XE_NULL,"_xmat_makehq");
    limit = H->m - 1;
    if ((int)(diag->dim) < limit || (int)(beta->dim) < limit)
        _xerror(_XE_SIZES,"_xmat_makehq");
    if (H->m != H->n)
        _xerror(_XE_SQUARE,"_xmat_makehq");
    Qout = _xmat_resize(Qout,H->m,H->m);
    
    tmp1 = _xvec_resize(tmp1,H->m);
    tmp2 = _xvec_resize(tmp2,H->m);
    _XMEM_STAT_REG(tmp1,_XTYPE_VEC);
    _XMEM_STAT_REG(tmp2,_XTYPE_VEC);
    
    for (i = 0; (unsigned int)i < H->m; i++){
        /* tmp1 = i'th basis vector */
        for (j = 0; (unsigned int)j < H->m; j++)
            _xvec_set_val(tmp1,j,0.0);
      
        _xvec_set_val(tmp1,i,1.0);
        /* apply H/h transforms in reverse order */
        for (j = limit-1; j >= 0; j--){
            _xmat_get_col(H,(unsigned int)j,tmp2);
            _xvec_set_val(tmp2,j+1,_xvec_entry(diag,j));
            _xvec_hhtrvec(tmp2,beta->ve[j],j+1,tmp1,tmp1);
        }
        
        /* insert into Qout */
        _xmat_mset_col(Qout,(unsigned int)i,tmp1);
    }

#ifdef _XTHREAD_SAFE
    _xvec_vfree(tmp1); _xvec_vfree(tmp2);
#endif

    return (Qout);
}

/*!
   return c,s parameters for Givens rotation to
   eliminate y in the vector [ x y ]'
 */
void _xvec_givens(double x, double y, _xreal *c, _xreal *s){
    _xreal norm;

    norm = (_xreal)sqrt(x*x+y*y);
    if (norm == (_xreal)(0.0)){
        *c = 1.0;
        *s = 0.0; 
    } /* identity */
    else {
        *c = (_xreal)(x/norm);
        *s = (_xreal)(y/norm);
    }
}

/*!
   apply Givens rotation to x's i & k components
 */
_xvec *_xvec_rot_vec(const _xvec *x, unsigned int i, unsigned int k, double c, double s, _xvec *out){
    _xreal temp;

    if (!x)
        _xerror(_XE_NULL,"_xvec_rot_vec");
    if (i >= x->dim || k >= x->dim)
        _xerror(_XE_RANGE,"_xvec_rot_vec");
    out = _xvec_vcopy(x,out);
    
    temp = (_xreal)(c*_xvec_entry(out,i) + s*_xvec_entry(out,k));
    _xvec_set_val(out,k,(_xreal)(-s*_xvec_entry(out,i)+c*_xvec_entry(out,k)));
    _xvec_set_val(out,i,temp);
    
    return (out);
}

/*!
   premultiply mat by givens rotation described by c,s
 */
_xmat *_xmat_rot_rows(const _xmat *mat, unsigned int i, unsigned int k, double c, double s, _xmat *out){
    unsigned int j;
    _xreal temp;
    
    if (!mat)
        _xerror(_XE_NULL,"_xmat_rot_rows");
    if (i >= mat->m || k >= mat->m)
        _xerror(_XE_RANGE,"_xmat_rot_rows");
    if (mat != out)
        out = _xmat_mcopy(mat,_xmat_resize(out,mat->m,mat->n));
    
    for (j=0; j<mat->n; j++){
        temp = (_xreal)(c*_xmat_entry(out,i,j) + s*_xmat_entry(out,k,j));
        _xmat_set_val(out,k,j, (_xreal)(-s*_xmat_entry(out,i,j) + c*_xmat_entry(out,k,j)));
        _xmat_set_val(out,i,j, temp);
    }
    
    return (out);
}

/*!
   postmultiply mat by givens rotation described by c,s
 */
_xmat *_xmat_rot_cols(const _xmat *mat, unsigned int i, unsigned int k, double c, double s, _xmat *out){
    unsigned int j;
    _xreal temp;
    
    if (!mat)
        _xerror(_XE_NULL,"_xmat_rot_cols");
    if (i >= mat->n || k >= mat->n)
        _xerror(_XE_RANGE,"_xmat_rot_cols");
    if (mat != out )
        out = _xmat_mcopy(mat,_xmat_resize(out,mat->m,mat->n));
    
    for (j=0; j<mat->m; j++){
        temp = (_xreal)(c*_xmat_entry(out,j,i) + s*_xmat_entry(out,j,k));
        _xmat_set_val(out,j,k, (_xreal)(-s*_xmat_entry(out,j,i) + c*_xmat_entry(out,j,k)));
        _xmat_set_val(out,j,i,temp);
    }
    
    return (out);
}

/*!
    update QR factorisation in expanded form (seperate matrices)
  Finds Q+, R+ s.t. Q+.R+ = Q.(R+u.v') and Q+ orthogonal, R+ upper triang
  Ref: Golub & van Loan Matrix Computations pp437-443
  -- does not update Q if it is NULL
 */
_xmat *_xmat_qrupdate(_xmat *Q, _xmat *R, _xvec *u, _xvec *v){
    int i,j,k;
    _xreal c,s,temp;
    
    if (!R || !u || !v)
        _xerror(_XE_NULL,"_xmat_qrupdate");
    if ((Q && ( Q->m != Q->n || R->m != Q->n)) || u->dim != R->m || v->dim != R->n)
        _xerror(_XE_SIZES,"_xmat_qrupdate");
    
    /* find largest k s.t. u[k] != 0 */
    for (k=R->m-1; k>=0; k--)
        if (u->ve[k] != 0.0)
            break;
    
    /* transform R+u.v' to Hessenberg form */
    for (i=k-1; i>=0; i--){
        /* get Givens rotation */
        _xvec_givens(u->ve[i],u->ve[i+1],&c,&s);
        _xmat_rot_rows(R,i,i+1,c,s,R);
        if (Q)
            _xmat_rot_cols(Q,i,i+1,c,s,Q);
        _xvec_rot_vec(u,i,i+1,c,s,u);
    }
    
    /* add into R */
    temp = u->ve[0];
    for (j=0; (unsigned int)j<R->n; j++)
        R->me[0][j] += temp*v->ve[j];
    
    /* transform Hessenberg to upper triangular */
    for (i=0; i<k; i++){
        _xvec_givens(R->me[i][i],R->me[i+1][i],&c,&s);
        _xmat_rot_rows(R,i,i+1,c,s,R);
        if (Q)
            _xmat_rot_cols(Q,i,i+1,c,s,Q);
    }
    
    return (R);
}

static void _xhhldr3(double x, double y, double z, _xreal *nu1, _xreal *beta, _xreal *newval){
    _xreal alpha;
    
    if (x >= 0.0)
        alpha = (_xreal)sqrt(x*x+y*y+z*z);
    else
        alpha = -(_xreal)sqrt(x*x+y*y+z*z);
    *nu1 = (_xreal)(x + alpha);
    *beta = (_xreal)(1.0/(alpha*(*nu1)));
    *newval = alpha;
}

static void _xhhldr3cols(_xmat *A, int k, int j0, double beta, double nu1, double nu2, double nu3){
    _xreal **A_me, ip, prod;
    int j, n;
    
    if (k < 0 || k+3 > (int)(A->m) || j0 < 0)
        _xerror(_XE_BOUNDS,"_xhhldr3cols");
    
    A_me = A->me; n = A->n;
    for (j = j0; j < n; j++){
        ip = (_xreal)(nu1*_xmat_entry(A,k,j)+nu2*_xmat_entry(A,k+1,j)+nu3*_xmat_entry(A,k+2,j));
        prod = (_xreal)(ip*beta);
    
        _xmat_add_val(A,k  ,j,-(_xreal)(prod*nu1));
        _xmat_add_val(A,k+1,j,-(_xreal)(prod*nu2));
        _xmat_add_val(A,k+2,j,-(_xreal)(prod*nu3));
    }
}

static void _xhhldr3rows(_xmat *A, int k, int i0, double beta, double nu1, double nu2, double nu3){
    _xreal **A_me, ip, prod;
      int i, m;
    
    if (k < 0 || k+3 > (int)(A->n))
        _xerror(_XE_BOUNDS,"_xhhldr3rows");
    
    A_me = A->me; m = A->m;
    i0 = _xmin(i0,m-1);
    
    for (i = 0; i <= i0; i++){
        ip = (_xreal)(nu1*_xmat_entry(A,i,k)+nu2*_xmat_entry(A,i,k+1)+nu3*_xmat_entry(A,i,k+2));
        prod = (_xreal)(ip*beta);
        _xmat_add_val(A,i,k  , -(_xreal)(prod*nu1));
        _xmat_add_val(A,i,k+1, -(_xreal)(prod*nu2));
        _xmat_add_val(A,i,k+2, -(_xreal)(prod*nu3));
    }
}

/*!
    compute the Schur decomposition of the matrix A in situ
  -- optionally, gives Q matrix such that Q^T.A.Q is upper triangular
  -- returns upper triangular Schur matrix
 */
_xmat *_xmat_schur(_xmat *A, _xmat *Q){
    int i, j, iter, k, k_min, k_max, k_tmp, n, split;
    _xreal beta2, c, discrim, dummy, nu1, s, tmp, x, y, z;
    _xreal **A_me;
    _xreal sqrt_macheps;
    _XSTATIC _xvec *diag=(_xvec *)NULL, *beta=(_xvec *)NULL;
    
    if (!A)
        _xerror(_XE_NULL,"_xmat_schur");
    if (A->m != A->n || (Q && Q->m != Q->n))
        _xerror(_XE_SQUARE,"_xmat_schur");
    if (Q && Q->m != A->m)
        _xerror(_XE_SIZES,"_xmat_schur");
    
    n = A->n;
    diag = _xvec_resize(diag,A->n);
    beta = _xvec_resize(beta,A->n);
    _XMEM_STAT_REG(diag,_XTYPE_VEC);
    _XMEM_STAT_REG(beta,_XTYPE_VEC);

    /* compute Hessenberg form */
    _xmat_hfactor(A,diag,beta);
    
    /* save Q if necessary */
    if (Q)
        Q = _xmat_makehq(A,diag,beta,Q);
    _xmat_makeh(A,A);
    sqrt_macheps = (_xreal)sqrt(_XMACHEPS);

    k_min = 0; A_me = A->me;
    while (k_min < n){
        _xreal a00, a01, a10, a11;
        double scale, t, numer, denom;
        
        /* find k_max to suit:
           submatrix k_min..k_max should be irreducible */
        k_max = n-1;
        for (k = k_min; k < k_max; k++)
            if (_xmat_entry(A,k+1,k) == 0.0){
                k_max = k;
                break;
            }
        
        if (k_max <= k_min){
            k_min = k_max + 1;
            continue;   /* outer loop */
        }

        /* check to see if we have a 2 x 2 block
           with complex eigenvalues */
        if (k_max == k_min + 1){
            a00 = _xmat_entry(A,k_min,k_min);
            a01 = _xmat_entry(A,k_min,k_max);
            a10 = _xmat_entry(A,k_max,k_min);
            a11 = _xmat_entry(A,k_max,k_max);
            tmp = a00 - a11;
        
            discrim = tmp*tmp + 4*a01*a10;
            if (discrim < 0.0){ 
            /* yes -- e-vals are complex
                 -- put 2 x 2 block in form [a b; c a];
                 then eigenvalues have real part a & imag part sqrt(|bc|) */
                numer = - tmp;
                denom = ( a01+a10 >= 0.0 ) ?
                        (a01+a10) + sqrt((a01+a10)*(a01+a10)+tmp*tmp) :
                        (a01+a10) - sqrt((a01+a10)*(a01+a10)+tmp*tmp);
                if (denom != 0.0){
                    t = numer/denom;
                    scale = c = (_xreal)(1.0/sqrt(1+t*t));
                    s = (_xreal)(c*t);
                }
                else {
                    c = 1.0;
                    s = 0.0;
                }
                _xmat_rot_cols(A,k_min,k_max,c,s,A);
                _xmat_rot_rows(A,k_min,k_max,c,s,A);
                if (Q)
                    _xmat_rot_cols(Q,k_min,k_max,c,s,Q);
                k_min = k_max + 1;
                continue;
            }
            else {
                /* discrim >= 0; i.e. block has two real eigenvalues */
                /* no -- e-vals are not complex;
                   split 2 x 2 block and continue */
                /* s/c = numer/denom */
                numer = ( tmp >= 0.0 ) ?
                        - tmp - sqrt(discrim) : - tmp + sqrt(discrim);
                denom = 2*a01;
                if (fabs(numer) < fabs(denom)){
                    t = numer/denom;
                    scale = c = (_xreal)(1.0/sqrt(1+t*t));
                    s = (_xreal)(c*t);
                }
                else if (numer != 0.0){
                    t = denom/numer;
                    scale = 1.0/sqrt(1+t*t);
                    c = (_xreal)(fabs(t)*scale);
                    s = ( t >= 0.0 ) ? (_xreal)scale : -(_xreal)scale;
                }
                else { /* numer == denom == 0 */
                    c = 0.0;
                    s = 1.0;
                }
                _xmat_rot_cols(A,k_min,k_max,c,s,A);
                _xmat_rot_rows(A,k_min,k_max,c,s,A);
                if (Q)
                    _xmat_rot_cols(Q,k_min,k_max,c,s,Q);
                k_min = k_max + 1;  /* go to next block */
                continue;
            }
        }

        /* now have r x r block with r >= 2:
           apply Francis QR step until block splits */
        split = _XFALSE; iter = 0;
        while (!split){
            iter++;
            
            /* set up Wilkinson/Francis complex shift */
            k_tmp = k_max - 1;
            
            a00 = _xmat_entry(A,k_tmp,k_tmp);
            a01 = _xmat_entry(A,k_tmp,k_max);
            a10 = _xmat_entry(A,k_max,k_tmp);
            a11 = _xmat_entry(A,k_max,k_max);

            /* treat degenerate cases differently
               -- if there are still no splits after five iterations
               and the bottom 2 x 2 looks degenerate, force it to
               split 
             */
#ifdef _XDEBUG
            (void) fprintf(stdout, "# _xmat_schur: bottom 2 x 2 = [%lg, %lg; %lg, %lg]\n",
                           a00, a01, a10, a11);
#endif
            if (iter >= 5 &&
                fabs(a00-a11) < sqrt_macheps*(fabs(a00)+fabs(a11)) &&
               (fabs(a01) < sqrt_macheps*(fabs(a00)+fabs(a11)) ||
                fabs(a10) < sqrt_macheps*(fabs(a00)+fabs(a11)))){
                if (fabs(a01) < sqrt_macheps*(fabs(a00)+fabs(a11)))
                    _xmat_set_val(A,k_tmp,k_max,0.0);
                if (fabs(a10) < sqrt_macheps*(fabs(a00)+fabs(a11))){
                    _xmat_set_val(A,k_max,k_tmp,0.0);
                    split = _XTRUE;
                    continue;
                }
            }  

            s = a00 + a11;
            t = a00*a11 - a01*a10;
            
            /* break loop if a 2 x 2 complex block */
            if (k_max == k_min + 1 && s*s < 4.0*t){
                split = _XTRUE;
                continue;
            }

            /* perturb shift if convergence is slow */
            if ((iter % 10) == 0){
                s += (_xreal)(iter*0.02); 
                t += (_xreal)(iter*0.02);
            }

            /* set up Householder transformations */
            k_tmp = k_min + 1;
            
            a00 = _xmat_entry(A,k_min,k_min);
            a01 = _xmat_entry(A,k_min,k_tmp);
            a10 = _xmat_entry(A,k_tmp,k_min);
            a11 = _xmat_entry(A,k_tmp,k_tmp);
            
            x = (_xreal)(a00*a00 + a01*a10 - s*a00 + t);
            y = a10*(a00+a11-s);
            if (k_min + 2 <= k_max)
                z = a10*A->me[k_min+2][k_tmp];
            else
                z = 0.0;
            
            for (k = k_min; k <= k_max-1; k++){
                if (k < k_max - 1){
                    _xhhldr3(x,y,z,&nu1,&beta2,&dummy);
                    _xtracecatch(_xhhldr3cols(A,k,_xmax(k-1,0), beta2,nu1,y,z),"_xmat_schur");
                    _xtracecatch(_xhhldr3rows(A,k,_xmin(n-1,k+3),beta2,nu1,y,z),"_xmat_schur");
                    if (Q)
                        _xhhldr3rows(Q,k,n-1,beta2,nu1,y,z);
                }
                else {
                    _xvec_givens(x,y,&c,&s);
                    _xmat_rot_cols(A,k,k+1,c,s,A);
                    _xmat_rot_rows(A,k,k+1,c,s,A);
                    if (Q)
                        _xmat_rot_cols(Q,k,k+1,c,s,Q);
                }
                x = _xmat_entry(A,k+1,k);
                if (k <= k_max - 2)
                    y = _xmat_entry(A,k+2,k);
                else
                    y = 0.0;
                if (k <= k_max - 3)
                    z = _xmat_entry(A,k+3,k);
                else
                    z = 0.0;
            }
            for (k = k_min; k <= k_max-2; k++){
                /* zero appropriate sub-diagonals */
                _xmat_set_val(A,k+2,k,0.0);
                if (k < k_max-2)
                    _xmat_set_val(A,k+3,k,0.0);
            }

            /* test to see if matrix should split */
            for (k = k_min; k < k_max; k++)
                if (fabs(A_me[k+1][k]) < _XMACHEPS*
                   (fabs(A_me[k][k])+fabs(A_me[k+1][k+1]))){
                    A_me[k+1][k] = 0.0;
                    split = _XTRUE;
                }
        }
    }
    
    /* polish up A by zeroing strictly lower triangular elements
       and small sub-diagonal elements */
    for (i = 0; (unsigned int)i < A->m; i++)
        for (j = 0; j < i-1; j++)
            A_me[i][j] = 0.0;
    for (i = 0; (unsigned int)i < A->m - 1; i++)
        if (fabs(A_me[i+1][i]) < _XMACHEPS*
           (fabs(A_me[i][i])+fabs(A_me[i+1][i+1])))
            A_me[i+1][i] = 0.0;

#ifdef _XTHREAD_SAFE
    _xvec_vfree(diag); _xvec_vfree(beta);
#endif

    return (A);
}

/*!
   compute real & imaginary parts of eigenvalues
  -- assumes T contains a block upper triangular matrix
    as produced by _xmat_schur()
  -- real parts stored in real_pt, imaginary parts in imag_pt
 */
void _xmat_schur_evals(_xmat *T, _xvec *real_pt, _xvec *imag_pt){
    int i, n;
    _xreal discrim, **T_me;
    _xreal diff, sum, tmp;
    
    if (!T || !real_pt || !imag_pt)
        _xerror(_XE_NULL,"_xmat_schur_evals");
    if (T->m != T->n)
        _xerror(_XE_SQUARE,"_xmat_schur_evals");
    
    n = T->n; T_me = T->me;
    real_pt = _xvec_resize(real_pt,(unsigned int)n);
    imag_pt = _xvec_resize(imag_pt,(unsigned int)n);
    
    i = 0;
    while (i < n){
        if (i < n-1 && T_me[i+1][i] != 0.0){
            /* should be a complex eigenvalue */
            sum  = (_xreal)(0.5*(T_me[i][i]+T_me[i+1][i+1]));
            diff = (_xreal)(0.5*(T_me[i][i]-T_me[i+1][i+1]));
            discrim = diff*diff + T_me[i][i+1]*T_me[i+1][i];
            if (discrim < 0.0){ 
            /* yes -- complex e-vals */
                real_pt->ve[i] = real_pt->ve[i+1] = sum;
                imag_pt->ve[i] = (_xreal)sqrt(-discrim);
                imag_pt->ve[i+1] = - imag_pt->ve[i];
            }
            else {
            /* no -- actually both real */
                tmp = (_xreal)sqrt(discrim);
                real_pt->ve[i]   = sum + tmp;
                real_pt->ve[i+1] = sum - tmp;
                imag_pt->ve[i]   = imag_pt->ve[i+1] = 0.0;
            }
            i += 2;
        }
        else {
          /* real eigenvalue */
            real_pt->ve[i] = T_me[i][i];
            imag_pt->ve[i] = 0.0;
            i++;
        }
    }
}

/*!
    return eigenvectors computed from the real Schur
    decomposition of a matrix
  -- T is the block upper triangular Schur matrix
  -- Q is the orthognal matrix where A = Q.T.Q^T
  -- if Q is null, the eigenvectors of T are returned
  -- X_re is the real part of the matrix of eigenvectors,
    and X_im is the imaginary part of the matrix.
  -- X_re is returned
 */
_xmat *_xmat_schur_vecs(_xmat *T, _xmat *Q, _xmat *X_re, _xmat *X_im){
    int i, j, limit;
    _xreal t11_re, t11_im, t12, t21, t22_re, t22_im;
    _xreal l_re, l_im, det_re, det_im, invdet_re, invdet_im;
    _xreal val1_re, val1_im, val2_re, val2_im;
    _xreal tmp_val1_re, tmp_val1_im, tmp_val2_re, tmp_val2_im, **T_me;
    _xreal sum, diff, discrim, magdet, norm, scale;
    
    _XSTATIC _xvec *tmp1_re=(_xvec *)NULL, *tmp1_im=(_xvec *)NULL;
    _XSTATIC _xvec *tmp2_re=(_xvec *)NULL, *tmp2_im=(_xvec *)NULL;
    
    if (!T || !X_re)
        _xerror(_XE_NULL,"_xmat_schur_vecs");
    if (T->m != T->n || X_re->m != X_re->n ||
       (Q && Q->m != Q->n ) ||
       (X_im && X_im->m != X_im->n))
        _xerror(_XE_SQUARE,"_xmat_schur_vecs");
    if (T->m != X_re->m ||
       (Q && T->m != Q->m ) ||
       (X_im && T->m != X_im->m))
        _xerror(_XE_SIZES,"_xmat_schur_vecs");
    
    tmp1_re = _xvec_resize(tmp1_re,T->m);
    tmp1_im = _xvec_resize(tmp1_im,T->m);
    tmp2_re = _xvec_resize(tmp2_re,T->m);
    tmp2_im = _xvec_resize(tmp2_im,T->m);
    _XMEM_STAT_REG(tmp1_re,_XTYPE_VEC);
    _XMEM_STAT_REG(tmp1_im,_XTYPE_VEC);
    _XMEM_STAT_REG(tmp2_re,_XTYPE_VEC);
    _XMEM_STAT_REG(tmp2_im,_XTYPE_VEC);
    
    T_me = T->me;
    i = 0;
    while (i < (int)(T->m)){
        if (i+1 < (int)(T->m) && T->me[i+1][i] != 0.0){
            /* complex eigenvalue */
            sum  = (_xreal)(0.5*(T_me[i][i]+T_me[i+1][i+1]));
            diff = (_xreal)(0.5*(T_me[i][i]-T_me[i+1][i+1]));
            discrim = diff*diff + T_me[i][i+1]*T_me[i+1][i];
            l_re = l_im = 0.0;
            if (discrim < 0.0){
                /* yes -- complex e-vals */
                l_re = sum;
                l_im = (_xreal)sqrt(-discrim);
            }
            else /* not correct Real Schur form */
                _xerror(_XE_RANGE,"_xmat_schur_vecs");
        }
        else {
            l_re = T_me[i][i];
            l_im = 0.0;
        }
    
        _xvec_zero(tmp1_im);
        _xvec_rand(tmp1_re);
        _xvec_sv_mlt(_XMACHEPS,tmp1_re,tmp1_re);
    
        /* solve (T-l.I)x = tmp1 */
        limit = ( l_im != 0.0 ) ? i+1 : i;
        for (j = limit+1; j < (int)(T->m); j++)
            tmp1_re->ve[j] = 0.0;
        j = limit;
        while (j >= 0){
            if (j > 0 && T->me[j][j-1] != 0.0){
                /* 2 x 2 diagonal block */
                val1_re = (_xreal)(tmp1_re->ve[j-1] -
                           __xip__(&(tmp1_re->ve[j+1]),&(T->me[j-1][j+1]),limit-j));
                val1_im = (_xreal)(tmp1_im->ve[j-1] -
                           __xip__(&(tmp1_im->ve[j+1]),&(T->me[j-1][j+1]),limit-j));
                val2_re = (_xreal)(tmp1_re->ve[j] -
                           __xip__(&(tmp1_re->ve[j+1]),&(T->me[j][j+1]),limit-j));
                val2_im = (_xreal)(tmp1_im->ve[j] -
                           __xip__(&(tmp1_im->ve[j+1]),&(T->me[j][j+1]),limit-j));
            
                t11_re = T_me[j-1][j-1] - l_re;
                t11_im = - l_im;
                t22_re = T_me[j][j] - l_re;
                t22_im = - l_im;
                t12 = T_me[j-1][j];
                t21 = T_me[j][j-1];
            
                scale = (_xreal)(fabs(T_me[j-1][j-1]) + fabs(T_me[j][j]) +
                                 fabs(t12) + fabs(t21) + fabs(l_re) + fabs(l_im));
            
                det_re = t11_re*t22_re - t11_im*t22_im - t12*t21;
                det_im = t11_re*t22_im + t11_im*t22_re;
                magdet = det_re*det_re+det_im*det_im;
                if (sqrt(magdet) < _XMACHEPS*scale){
                    det_re = (_xreal)_XMACHEPS*scale;
                    magdet = det_re*det_re+det_im*det_im;
                }
                invdet_re =   det_re/magdet;
                invdet_im = - det_im/magdet;
                tmp_val1_re = t22_re*val1_re-t22_im*val1_im-t12*val2_re;
                tmp_val1_im = t22_im*val1_re+t22_re*val1_im-t12*val2_im;
                tmp_val2_re = t11_re*val2_re-t11_im*val2_im-t21*val1_re;
                tmp_val2_im = t11_im*val2_re+t11_re*val2_im-t21*val1_im;
                tmp1_re->ve[j-1] = invdet_re*tmp_val1_re -
                                   invdet_im*tmp_val1_im;
                tmp1_im->ve[j-1] = invdet_im*tmp_val1_re +
                                   invdet_re*tmp_val1_im;
                tmp1_re->ve[j]   = invdet_re*tmp_val2_re -
                                   invdet_im*tmp_val2_im;
                tmp1_im->ve[j]   = invdet_im*tmp_val2_re +
                                   invdet_re*tmp_val2_im;
                j -= 2;
            }
            else {
                t11_re = T_me[j][j] - l_re;
                t11_im = - l_im;
                magdet = t11_re*t11_re + t11_im*t11_im;
                scale = (_xreal)(fabs(T_me[j][j]) + fabs(l_re));
                if (sqrt(magdet) < _XMACHEPS*scale){
                    t11_re = (_xreal)_XMACHEPS*scale;
                    magdet = t11_re*t11_re + t11_im*t11_im;
                }
                invdet_re =   t11_re/magdet;
                invdet_im = - t11_im/magdet;
                
                val1_re = (_xreal)(tmp1_re->ve[j] -
                           __xip__(&(tmp1_re->ve[j+1]),&(T->me[j][j+1]),limit-j));
                val1_im = (_xreal)(tmp1_im->ve[j] -
                           __xip__(&(tmp1_im->ve[j+1]),&(T->me[j][j+1]),limit-j));
                
                tmp1_re->ve[j] = invdet_re*val1_re - invdet_im*val1_im;
                tmp1_im->ve[j] = invdet_im*val1_re + invdet_re*val1_im;
                j -= 1;
            }
        }
    
        norm = (_xreal)(_xvec_vnorm_inf(tmp1_re) + _xvec_vnorm_inf(tmp1_im));
        _xvec_sv_mlt(1/norm,tmp1_re,tmp1_re);
        if (l_im != 0.0)
            _xvec_sv_mlt(1/norm,tmp1_im,tmp1_im);
        _xmat_mv_mlt(Q,tmp1_re,tmp2_re);
        if (l_im != 0.0)
            _xmat_mv_mlt(Q,tmp1_im,tmp2_im);
        if (l_im != 0.0)
            norm = (_xreal)sqrt(_xvec_vin_prod(tmp2_re,tmp2_re)+_xvec_vin_prod(tmp2_im,tmp2_im));
        else
            norm = (_xreal)_xvec_vnorm2(tmp2_re);
        _xvec_sv_mlt(1/norm,tmp2_re,tmp2_re);
        if (l_im != 0.0)
            _xvec_sv_mlt(1/norm,tmp2_im,tmp2_im);
    
        if (l_im != 0.0){
            if (!X_im)
                _xerror(_XE_NULL,"_xmat_schur_vecs");
            _xmat_mset_col(X_re,i,tmp2_re);
            _xmat_mset_col(X_im,i,tmp2_im);
            _xvec_sv_mlt(-1.0,tmp2_im,tmp2_im);
            _xmat_mset_col(X_re,i+1,tmp2_re);
            _xmat_mset_col(X_im,i+1,tmp2_im);
            i += 2;
        }
        else {
            _xmat_mset_col(X_re,i,tmp2_re);
            if (X_im)
                _xmat_mset_col(X_im,i,tmp1_im); /* zero vector */
            i += 1;
        }
    }

#ifdef _XTHREAD_SAFE
    _xvec_vfree(tmp1_re); _xvec_vfree(tmp1_im);
    _xvec_vfree(tmp2_re); _xvec_vfree(tmp2_im);
#endif

    return (X_re);
}

/*!
   find eigenvalues of symmetric tridiagonal matrices
  -- matrix represented by a pair of vectors a (diag entries)
    and b (sub- & super-diag entries)
  -- eigenvalues in a on return
 */
_xvec *_xmat_trieig(_xvec *a, _xvec *b, _xmat *Q){
    int i, i_min, i_max, n, split;
    _xreal *a_ve, *b_ve;
    _xreal b_sqr, bk, ak1, bk1, ak2, bk2, z;
    _xreal c, c2, cs, s, s2, d, mu;

#define _XSQRT2  1.4142135623730949
#define _xsgn(x) ( (x) >= 0 ? 1 : -1 )

    if (!a || !b)
        _xerror(_XE_NULL,"_xmat_trieig");
    if (a->dim != b->dim + 1 || ( Q && Q->m != a->dim))
        _xerror(_XE_SIZES,"_xmat_trieig");
    if (Q && Q->m != Q->n)
        _xerror(_XE_SQUARE,"_xmat_trieig");
    
    n = a->dim;
    a_ve = a->ve; b_ve = b->ve;
    
    i_min = 0;
    /* outer while loop */
    while (i_min < n){
        /* find i_max to suit;
           submatrix i_min..i_max should be irreducible */
        i_max = n-1;
        for (i = i_min; i < n-1; i++)
            if (b_ve[i] == 0.0){
                i_max = i;
                break;
            }
        if (i_max <= i_min){
            i_min = i_max + 1;
            continue; /* outer while loop */
        }
        
        /* repeatedly perform QR method until matrix splits */
        split = _XFALSE;
        while (!split){/* inner while loop */
            /* find Wilkinson shift */
            d = (a_ve[i_max-1] - a_ve[i_max])/2;
            b_sqr = b_ve[i_max-1]*b_ve[i_max-1];
            mu = (_xreal)(a_ve[i_max] - b_sqr/(d + _xsgn(d)*sqrt(d*d+b_sqr)));
        
            /* initial Givens' rotation */
            _xvec_givens(a_ve[i_min]-mu,b_ve[i_min],&c,&s);
            s = -s;
            if (fabs(c) < _XSQRT2){
                c2 = c*c;
                s2 = 1-c2;
            }
            else {
                s2 = s*s;
                c2 = 1-s2;
            }
            cs = c*s;
            ak1 = c2*a_ve[i_min]+s2*a_ve[i_min+1]-2*cs*b_ve[i_min];
            bk1 = cs*(a_ve[i_min]-a_ve[i_min+1])+(c2-s2)*b_ve[i_min];
            ak2 = s2*a_ve[i_min]+c2*a_ve[i_min+1]+2*cs*b_ve[i_min];
            bk2 = ( i_min < i_max-1 ) ? c*b_ve[i_min+1] : (_xreal)0.0;
            z = ( i_min < i_max-1 ) ? -s*b_ve[i_min+1] : (_xreal)0.0;
            a_ve[i_min] = ak1;
            a_ve[i_min+1] = ak2;
            b_ve[i_min] = bk1;
            if (i_min < i_max-1)
                b_ve[i_min+1] = bk2;
            if (Q)
                _xmat_rot_cols(Q,i_min,i_min+1,c,-s,Q);
        
            for (i = i_min+1; i < i_max; i++){
                /* get Givens' rotation for sub-block -- k == i-1 */
                _xvec_givens(b_ve[i-1],z,&c,&s);
                s = -s;
        
                /* perform Givens' rotation on sub-block */
                if (fabs(c) < _XSQRT2){
                    c2 = c*c;
                    s2 = 1-c2;
                }
                else {
                    s2 = s*s;
                    c2 = 1-s2;
                }
                cs = c*s;
                bk = c*b_ve[i-1] - s*z;
                ak1 = c2*a_ve[i]+s2*a_ve[i+1]-2*cs*b_ve[i];
                bk1 = cs*(a_ve[i]-a_ve[i+1])+(c2-s2)*b_ve[i];
                ak2 = s2*a_ve[i]+c2*a_ve[i+1]+2*cs*b_ve[i];
                bk2 = ( i+1 < i_max ) ? c*b_ve[i+1] : (_xreal)0.0;
                z = ( i+1 < i_max ) ? -s*b_ve[i+1] : (_xreal)0.0;
                a_ve[i] = ak1; a_ve[i+1] = ak2;
                b_ve[i] = bk1;
                if (i < i_max-1)
                    b_ve[i+1] = bk2;
                if (i > i_min)
                    b_ve[i-1] = bk;
                if (Q)
                    _xmat_rot_cols(Q,i,i+1,c,-s,Q);
            }
        
            /* test to see if matrix should be split */
            for (i = i_min; i < i_max; i++)
                if (fabs(b_ve[i]) < _XMACHEPS*
                   (fabs(a_ve[i])+fabs(a_ve[i+1]))){
                     b_ve[i] = 0.0;
                     split = _XTRUE;
                }
        }
    }

    return (a);
}

/*!
    compute eigenvalues of a dense symmetric matrix
  -- A **must** be symmetric on entry
  -- eigenvalues stored in out
  -- Q contains orthogonal matrix of eigenvectors
  -- returns vector of eigenvalues
 */
_xvec *_xmat_symmeig(const _xmat *A, _xmat *Q, _xvec *out){
    int i;
    _XSTATIC _xmat *tmp = (_xmat *)NULL;
    _XSTATIC _xvec *b = (_xvec *)NULL, *diag = (_xvec *)NULL, *beta = (_xvec *)NULL;
    
    if (!A)
        _xerror(_XE_NULL,"_xmat_symmeig");
    if (A->m != A->n)
        _xerror(_XE_SQUARE,"_xmat_symmeig");
    if (!out || out->dim != A->m)
        out = _xvec_resize(out,A->m);
    
    tmp  = _xmat_resize(tmp,A->m,A->n);
    tmp  = _xmat_mcopy(A,tmp);
    b    = _xvec_resize(b,A->m - 1);
    diag = _xvec_resize(diag,(unsigned int)A->m);
    beta = _xvec_resize(beta,(unsigned int)A->m);
    _XMEM_STAT_REG(tmp,_XTYPE_MAT);
    _XMEM_STAT_REG(b,_XTYPE_VEC);
    _XMEM_STAT_REG(diag,_XTYPE_VEC);
    _XMEM_STAT_REG(beta,_XTYPE_VEC);
    
    _xmat_hfactor(tmp,diag,beta);
    if (Q)
        _xmat_makehq(tmp,diag,beta,Q);
    
    for (i = 0; i < (int)(A->m - 1); i++){
        out->ve[i] = tmp->me[i][i];
        b->ve[i] = tmp->me[i][i+1];
    }
    out->ve[i] = tmp->me[i][i];
    _xmat_trieig(out,b,Q);

#ifdef _XTHREAD_SAFE
    _xmat_mfree(tmp); _xvec_vfree(b); _xvec_vfree(diag); _xvec_vfree(beta);
#endif

    return (out);
}

/*
    fix minor details about SVD
  -- make singular values non-negative
  -- sort singular values in decreasing order
  -- variables as for _xmat_bisvd()
  -- no argument checking
 */
static void _xfixsvd(_xvec *d, _xmat *U, _xmat *V){
#define _XSTACK 100
    int i, j, k, l, r, stack[_XSTACK], sp;
    _xreal tmp, v;

    /* make singular values non-negative */
    for (i = 0; (unsigned int)i < d->dim; i++)
        if (d->ve[i] < 0.0){
            d->ve[i] = - d->ve[i];
            if (U)
                for (j = 0; (unsigned int)j < U->m; j++)
                    U->me[i][j] = - U->me[i][j];
        }

    /* sort singular values */
    /* nonrecursive implementation of quicksort due to R.Sedgewick,
       "Algorithms in C", p. 122 (1990) */
    sp = -1;
    l = 0; r = d->dim - 1;
    for (;;){
        while (r > l){
            v = d->ve[r];
            i = l - 1; j = r;
            for (;;){/* inequalities are "backwards" for **decreasing** order */
                while (d->ve[++i] > v);
                while (d->ve[--j] < v);
                if (i >= j)
                    break;
                /* swap entries in d->ve */
                tmp = d->ve[i]; d->ve[i] = d->ve[j]; d->ve[j] = tmp;
                /* swap rows of U & V as well */
                if (U)
                    for (k = 0; (unsigned int)k < U->n; k++){
                        tmp = U->me[i][k];
                        U->me[i][k] = U->me[j][k];
                        U->me[j][k] = tmp;
                    }
                if (V)
                    for (k = 0; (unsigned int)k < V->n; k++){
                        tmp = V->me[i][k];
                        V->me[i][k] = V->me[j][k];
                        V->me[j][k] = tmp;
                    }
            }
            tmp = d->ve[i]; d->ve[i] = d->ve[r]; d->ve[r] = tmp;
            if (U)
                for (k = 0; (unsigned int)k < U->n; k++){
                    tmp = U->me[i][k];
                    U->me[i][k] = U->me[r][k];
                    U->me[r][k] = tmp;
                }
            if (V)
                for (k = 0; (unsigned int)k < V->n; k++){
                    tmp = V->me[i][k];
                    V->me[i][k] = V->me[r][k];
                    V->me[r][k] = tmp;
                }
            if (i - l > r - i){
                stack[++sp] = l;
                stack[++sp] = i-1;
                l = i+1;
            }
            else {
                stack[++sp] = i+1;
                stack[++sp] = r;
                r = i-1;
            }
        }
        if (sp < 0)
            break;
        r = stack[sp--];
        l = stack[sp--];
    }
#undef _XSTACK
}

/*
    perform preliminary factorisation for bisvd
  -- updates U and/or V, which ever is not NULL
 */
static _xmat *_xbifactor(_xmat *A, _xmat *U, _xmat *V){
    int k;
    _XSTATIC _xvec *tmp1=(_xvec *)NULL, *tmp2=(_xvec *)NULL, *w=(_xvec *)NULL;
    _xreal beta;
    
    if (!A)
        _xerror(_XE_NULL,"_xbifactor");
    if ((U && ( U->m != U->n ) ) || (V && ( V->m != V->n)))
        _xerror(_XE_SQUARE,"_xbifactor");
    if ((U && U->m != A->m ) || (V && V->m != A->n))
        _xerror(_XE_SIZES,"_xbifactor");
    
    tmp1 = _xvec_resize(tmp1,A->m);
    tmp2 = _xvec_resize(tmp2,A->n);
    w    = _xvec_resize(w,_xmax(A->m,A->n));
    _XMEM_STAT_REG(tmp1,_XTYPE_VEC);
    _XMEM_STAT_REG(tmp2,_XTYPE_VEC);
    _XMEM_STAT_REG(w,_XTYPE_VEC);
    
    if (A->m >= A->n)
        for (k = 0; (unsigned int)k < A->n; k++){
            _xmat_get_col(A,k,tmp1);
            _xvec_hhvec(tmp1,k,&beta,tmp1,&(A->me[k][k]));
            _xmat_hhvtrcols(A,k,k+1,tmp1,beta,w);
            if (U)
                _xmat_hhvtrcols(U,k,0,tmp1,beta,w);
            if (k+1 >= (int)(A->n))
                continue;
            _xmat_get_row(A,k,tmp2);
            _xvec_hhvec(tmp2,k+1,&beta,tmp2,&(A->me[k][k+1]));
            _xmat_hhtrrows(A,k+1,k+1,tmp2,beta);
            if (V)
                _xmat_hhvtrcols(V,k+1,0,tmp2,beta,w);
        }
    else
        for (k = 0; (unsigned int)k < A->m; k++){
            _xmat_get_row(A,k,tmp2);
            _xvec_hhvec(tmp2,k,&beta,tmp2,&(A->me[k][k]));
            _xmat_hhtrrows(A,k+1,k,tmp2,beta);
            if (V)
                _xmat_hhvtrcols(V,k,0,tmp2,beta,w);
            if (k+1 >= (int)(A->m))
                continue;
            _xmat_get_col(A,k,tmp1);
            _xvec_hhvec(tmp1,k+1,&beta,tmp1,&(A->me[k+1][k]));
            _xmat_hhvtrcols(A,k+1,k+1,tmp1,beta,w);
            if (U)
                _xmat_hhvtrcols(U,k+1,0,tmp1,beta,w);
        }

#ifdef _XTHREAD_SAFE
    _xvec_vfree(tmp1); _xvec_vfree(tmp2);
#endif

    return (A);
}

/*!
   svd of a bidiagonal m x n matrix represented by d (diagonal) and
      f (super-diagonals)
  -- returns with d set to the singular values, f zeroed
  -- if U, V non-NULL, the orthogonal operations are accumulated
    in U, V; if U, V == I on entry, then SVD == U^T.A.V
    where A is initial matrix
  -- returns d on exit
 */
_xvec *_xmat_bisvd(_xvec *d, _xvec *f, _xmat *U, _xmat *V){
    int i, j, n;
    int i_min, i_max, split;
    _xreal c, s, shift, size, z;
    _xreal d_tmp, diff, t11, t12, t22, *d_ve, *f_ve;
    
    if (!d || !f)
        _xerror(_XE_NULL,"_xmat_bisvd");
    if (d->dim != f->dim + 1)
        _xerror(_XE_SIZES,"_xmat_bisvd");
    
    n = d->dim;
    if ((U && (int)(U->n) < n) || (V && (int)(V->m) < n))
        _xerror(_XE_SIZES,"_xmat_bisvd");
    if ((U && U->m != U->n) || (V && V->m != V->n))
        _xerror(_XE_SQUARE,"_xmat_bisvd");
    
    if (n == 1){
        if (d->ve[0] < 0.0){
            d->ve[0] = - d->ve[0];
            if (U)
                _xmat_sm_mlt(-1.0,U,U);
        }
        return (d);
    }
    d_ve = d->ve; f_ve = f->ve;
    size = (_xreal)(_xvec_vnorm_inf(d) + _xvec_vnorm_inf(f));
    
    i_min = 0;
    /* outer while loop */
    while (i_min < n){
        /* find i_max to suit;
         submatrix i_min..i_max should be irreducible */
        i_max = n - 1;
        for (i = i_min; i < n - 1; i++)
            if (d_ve[i] == 0.0 || f_ve[i] == 0.0){
                i_max = i;
                if (f_ve[i] != 0.0){
                    /* have to ``chase'' f[i] element out of matrix */
                    z = f_ve[i]; f_ve[i] = 0.0;
                    for (j = i; j < n-1 && z != 0.0; j++){
                        _xvec_givens(d_ve[j+1],z, &c, &s);
                        s = -s;
                        d_ve[j+1] = c*d_ve[j+1] - s*z;
                        if (j+1 < n-1){
                            z = s*f_ve[j+1];
                            f_ve[j+1] = c*f_ve[j+1];
                        }
                        if (U)
                            _xmat_rot_rows(U,i,j+1,c,s,U);
                    }
                }
                break;
            }
        if (i_max <= i_min){
            i_min = i_max + 1;
            continue;
        }
    
        split = _XFALSE;
        while (!split){
            /* compute shift */
            t11 = d_ve[i_max-1]*d_ve[i_max-1] +(i_max > i_min+1 ? f_ve[i_max-2]*f_ve[i_max-2] : (_xreal)0.0);
            t12 = d_ve[i_max-1]*f_ve[i_max-1];
            t22 = d_ve[i_max]*d_ve[i_max] + f_ve[i_max-1]*f_ve[i_max-1];
            /* use e-val of [[t11,t12],[t12,t22]] matrix
             closest to t22 */
            diff = (t11-t22)/2;
            shift = (_xreal)(t22 - t12*t12/(diff + _xsgn(diff)*sqrt(diff*diff+t12*t12)));
            
            /* initial Givens' rotation */
            _xvec_givens(d_ve[i_min]*d_ve[i_min]-shift,d_ve[i_min]*f_ve[i_min], &c, &s);
            
            /* do initial Givens' rotations */
            d_tmp = c*d_ve[i_min] + s*f_ve[i_min];
            f_ve[i_min] = c*f_ve[i_min] - s*d_ve[i_min];
            d_ve[i_min] = d_tmp;
            z = s*d_ve[i_min+1];
            d_ve[i_min+1] = c*d_ve[i_min+1];
            if (V)
                _xmat_rot_rows(V,i_min,i_min+1,c,s,V);
            /* 2nd Givens' rotation */
            _xvec_givens(d_ve[i_min],z, &c, &s);
            d_ve[i_min] = c*d_ve[i_min] + s*z;
            d_tmp = c*d_ve[i_min+1] - s*f_ve[i_min];
            f_ve[i_min] = s*d_ve[i_min+1] + c*f_ve[i_min];
            d_ve[i_min+1] = d_tmp;
            if (i_min+1 < i_max){
                z = s*f_ve[i_min+1];
                f_ve[i_min+1] = c*f_ve[i_min+1];
            }
            if (U)
                _xmat_rot_rows(U,i_min,i_min+1,c,s,U);
    
            for (i = i_min+1; i < i_max; i++){
                /* get Givens' rotation for zeroing z */
                _xvec_givens(f_ve[i-1],z, &c, &s);
                f_ve[i-1] = c*f_ve[i-1] + s*z;
                d_tmp = c*d_ve[i] + s*f_ve[i];
                f_ve[i] = c*f_ve[i] - s*d_ve[i];
                d_ve[i] = d_tmp;
                z = s*d_ve[i+1];
                d_ve[i+1] = c*d_ve[i+1];
                if (V)
                  _xmat_rot_rows(V,i,i+1,c,s,V);
                /* get 2nd Givens' rotation */
                _xvec_givens(d_ve[i],z, &c, &s);
                d_ve[i] = c*d_ve[i] + s*z;
                d_tmp = c*d_ve[i+1] - s*f_ve[i];
                f_ve[i] = c*f_ve[i] + s*d_ve[i+1];
                d_ve[i+1] = d_tmp;
                if (i+1 < i_max){
                    z = s*f_ve[i+1];
                    f_ve[i+1] = c*f_ve[i+1];
                }
                if (U)
                    _xmat_rot_rows(U,i,i+1,c,s,U);
            }
            /* should matrix be split? */
            for (i = i_min; i < i_max; i++)
                if (fabs(f_ve[i]) < _XMACHEPS*(fabs(d_ve[i])+fabs(d_ve[i+1]))){
                    split = _XTRUE;
                    f_ve[i] = 0.0;
                }
                else if (fabs(d_ve[i]) < _XMACHEPS*size){
                    split = _XTRUE;
                    d_ve[i] = 0.0;
                }
        }
    }
    _xfixsvd(d,U,V);
    
    return (d);
}

/*!
    return vector of singular values in d
  -- also updates U and/or V, if one or the other is non-NULL
  -- destroys A
 */
_xvec *_xmat_svd(_xmat *A, _xmat *U, _xmat *V, _xvec *d){
    _XSTATIC _xvec *f=(_xvec *)NULL;
    int i, limit;
    _xmat *A_tmp;
    
    if (!A)
        _xerror(_XE_NULL,"_xmat_svd");
    if ((U && (U->m != U->n)) || (V && (V->m != V->n)))
        _xerror(_XE_SQUARE,"_xmat_svd");
    if ((U && U->m != A->m) || (V && V->m != A->n))
        _xerror(_XE_SIZES,"_xmat_svd");
    
    A_tmp = _xmat_mcopy(A,(_xmat *)NULL);
    if (U)
        _xmat_ident(U);
    if (V)
        _xmat_ident(V);
    
    limit = _xmin(A_tmp->m,A_tmp->n);
    d = _xvec_resize(d,limit);
    f = _xvec_resize(f,limit-1);
    _XMEM_STAT_REG(f,_XTYPE_VEC);
    
    _xbifactor(A_tmp,U,V);
    if (A_tmp->m >= A_tmp->n)
        for (i = 0; i < limit; i++){
            d->ve[i] = A_tmp->me[i][i];
            if (i+1 < limit)
                f->ve[i] = A_tmp->me[i][i+1];
        }
    else
        for (i = 0; i < limit; i++){
            d->ve[i] = A_tmp->me[i][i];
            if (i+1 < limit)
                f->ve[i] = A_tmp->me[i+1][i];
        }
    
    if (A_tmp->m >= A_tmp->n)
        _xmat_bisvd(d,f,U,V);
    else
        _xmat_bisvd(d,f,V,U);
    
    _xmat_mfree(A_tmp);
#ifdef _XTHREAD_SAFE
    _xvec_vfree(f);
#endif

    return (d);
}

/*!
   get, initialises and returns a SPROW structure
   -- max. length is maxlen
 */
_xsprow *_xsprow_get(int maxlen){
    _xsprow *r;
   
    if (maxlen < 0)
        _xerror(_XE_NEG,"_xsprow_get");

    r = _xnew(_xsprow);
    if (!r)
        _xerror(_XE_MEM,"_xsprow_get");
    else if (_xmem_info_is_on()) {
        _xmem_bytes(_XTYPE_SPROW,0,sizeof(_xsprow));
        _xmem_numvar(_XTYPE_SPROW,1);
    }
    r->elt = _xnew_a(maxlen,_xrow_elt);
    if (!r->elt)
        _xerror(_XE_MEM,"_xsprow_get");
    else if (_xmem_info_is_on()){
        _xmem_bytes(_XTYPE_SPROW,0,maxlen*sizeof(_xrow_elt));
    }
    r->len = 0;
    r->maxlen = maxlen;
    r->diag = -1;
   
    return (r);
}

/*!
   resize a _xsprow variable by means of realloc()
   -- n is a new size
   -- returns r
 */
_xsprow *_xsprow_resize(_xsprow *r, int n, int type){
    if (n < 0)
        _xerror(_XE_NEG,"_xsprow_resize");

    if (!r) 
        return _xsprow_get(n);
   
    if (n == r->len)
        return (r);

    if (!r->elt){
        r->elt = _xnew_a((unsigned)n,_xrow_elt);
        if (!r->elt)
            _xerror(_XE_MEM,"_xsprow_resize");
        else if (_xmem_info_is_on()){
            _xmem_bytes(type,0,n*sizeof(_xrow_elt));
        }
        r->maxlen = r->len = n;
        return (r);
    }

    if (n <= r->maxlen)
        r->len = n;
    else{
        if (_xmem_info_is_on()){
            _xmem_bytes(type,r->maxlen*sizeof(_xrow_elt),n*sizeof(_xrow_elt)); 
        }
        r->elt = _xrenew(r->elt,n,_xrow_elt);
        if (!r->elt)
            _xerror(_XE_MEM,"_xsprow_resize");
        r->maxlen = r->len = n;
    }
   
    return (r);
}

#define _XMINROWLEN 10
/*!
   expand row by means of realloc()
   -- type must be _XTYPE_SPMAT if r is a row of a _xsprow structure,
   otherwise it must be _XTYPE_SPROW
 */
_xsprow *_xsprow_xpd(_xsprow *r, int n, int type){
    int newlen;
   
    if (!r){
        r = _xnew(_xsprow);
        if (!r) 
            _xerror(_XE_MEM,"_xsprow_xpd");
        else if (_xmem_info_is_on()){
            if (type != _XTYPE_SPMAT && type != _XTYPE_SPROW)
                _xwarning(_XWARN_WRONG_TYPE,"_xsprow_xpd");
            _xmem_bytes(type,0,sizeof(_xsprow));
            if (type == _XTYPE_SPROW)
                _xmem_numvar(type,1);
        }
    }

    if (!r->elt){
        r->elt = _xnew_a((unsigned)n,_xrow_elt);
        if (!r->elt)
            _xerror(_XE_MEM,"_xsprow_xpd");
        else if (_xmem_info_is_on()){
            _xmem_bytes(type,0,n*sizeof(_xrow_elt));
        }
        r->len = 0;
        r->maxlen = n;
        return (r);
    }
    if (n <= r->len)
        newlen = _xmax(2*r->len + 1,_XMINROWLEN);
    else
        newlen = n;
    if (newlen <= r->maxlen){
        _xmem_zero((char *)(&(r->elt[r->len])),
                (newlen-r->len)*sizeof(_xrow_elt));
        r->len = newlen;
    }
    else {
        if (_xmem_info_is_on()){
            _xmem_bytes(type,r->maxlen*sizeof(_xrow_elt),
                        newlen*sizeof(_xrow_elt)); 
        }
        r->elt = _xrenew(r->elt,newlen,_xrow_elt);
        if (!r->elt)
            _xerror(_XE_MEM,"_xsprow_xpd");
        r->maxlen = newlen;
        r->len = newlen;
    }
   
    return (r);
}

/*!
   copy r1 and r2 into r_out
   -- cannot be done in-situ
   -- type must be _XTYPE_SPMAT or _XTYPE_SPROW depending on
      whether r_out is a row of a _xsprow structure
      or a _xsprow variable
   -- returns r_out
 */
_xsprow *_xsprow_copy(const _xsprow *r1, const _xsprow *r2, _xsprow *r_out, int type){
    int idx1, idx2, idx_out, len1, len2, len_out;
    _xrow_elt *elt1, *elt2, *elt_out;
   
    if (!r1 || !r2)
        _xerror(_XE_NULL,"_xsprow_copy");
    if (!r_out)
        r_out = _xsprow_get(_XMINROWLEN);
    if (r1 == r_out || r2 == r_out)
        _xerror(_XE_INSITU,"_xsprow_copy");
   
    /* Initialise */
    len1 = r1->len; len2 = r2->len; len_out = r_out->maxlen;
    idx1 = idx2 = idx_out = 0;
    elt1 = r1->elt; elt2 = r2->elt; elt_out = r_out->elt;
   
    while (idx1 < len1 || idx2 < len2){
        while (idx_out >= len_out){   /* r_out is too small */
            r_out->len = idx_out;
            r_out = _xsprow_xpd(r_out,0,type);
            len_out = r_out->maxlen;
            elt_out = &(r_out->elt[idx_out]);
        }
        if (idx2 >= len2 || (idx1 < len1 && elt1->col <= elt2->col)){
            elt_out->col = elt1->col;
            elt_out->val = elt1->val;
            if (elt1->col == elt2->col && idx2 < len2){
                elt2++;
                idx2++;
            }
            elt1++;
            idx1++;
        }
        else {
            elt_out->col = elt2->col;
            elt_out->val = 0.0;
            elt2++;
            idx2++;
        }
        elt_out++;
        idx_out++;
    }
    r_out->len = idx_out;
   
    return (r_out);
}

/*!
   merge r1 and r2 into r_out
   -- cannot be done in-situ
   -- type must be _XTYPE_SPMAT or _XTYPE_SPROW depending on
      whether r_out is a row of a _xsprow structure
      or a _xsprow variable
   -- returns r_out
 */
_xsprow *_xsprow_merge(const _xsprow *r1, const _xsprow *r2, _xsprow *r_out, int type){
    int idx1, idx2, idx_out, len1, len2, len_out;
    _xrow_elt *elt1, *elt2, *elt_out;
   
    if (!r1 || !r2)
        _xerror(_XE_NULL,"_xsprow_merge");
    if (!r_out)
        r_out = _xsprow_get(_XMINROWLEN);
    if (r1 == r_out || r2 == r_out)
        _xerror(_XE_INSITU,"_xsprow_merge");
   
    /* Initialise */
    len1 = r1->len; len2 = r2->len; len_out = r_out->maxlen;
    idx1 = idx2 = idx_out = 0;
    elt1 = r1->elt; elt2 = r2->elt; elt_out = r_out->elt;
   
    while (idx1 < len1 || idx2 < len2){
        if (idx_out >= len_out){   /* r_out is too small */
            r_out->len = idx_out;
            r_out = _xsprow_xpd(r_out,0,type);
            len_out = r_out->len;
            elt_out = &(r_out->elt[idx_out]);
        }
        if (idx2 >= len2 || (idx1 < len1 && elt1->col <= elt2->col)){
            elt_out->col = elt1->col;
            elt_out->val = elt1->val;
            if (elt1->col == elt2->col && idx2 < len2){
                elt2++;
                idx2++; 
            }
            elt1++;
            idx1++;
        }
        else {
            elt_out->col = elt2->col;
            elt_out->val = elt2->val;
            elt2++;
            idx2++;
        }
        elt_out++;
        idx_out++;
    }
    r_out->len = idx_out;
   
    return (r_out);
}

/*!
   get index into row for a given column in a given row
   -- return -1 on error
   -- return -(idx+2) where idx is index to insertion point
 */
int _xsprow_idx(const _xsprow *r, int col){
    register int lo, hi, mid;
    int tmp;
    register _xrow_elt *r_elt;
   
    r_elt = r->elt;
    if (r->len <= 0)
        return -2;
   
    /* try the hint */
    /* if ( hint >= 0 && hint < r->len && r_elt[hint].col == col )
       return hint; */
    /* otherwise use binary search... */
    /* code from K&R Ch. 6, p. 125 */
    lo = 0; hi = r->len - 1; mid = lo;
    while (lo <= hi){
        mid = (hi + lo)/2;
        if ((tmp=r_elt[mid].col-col) > 0)
            hi = mid-1;
        else if (tmp < 0)
            lo = mid+1;
        else /* tmp == 0 */
            return (mid);
    }

    return (r_elt[mid].col - col) > 0 ?
            -(mid+2): /* insert at mid   */
            -(mid+3); /* insert at mid+1 */
}

/*!
   set r_out <- r1 + alpha.r2
   -- cannot be in situ
   -- only for columns j0, j0+1, ...
   -- type must be _XTYPE_SPMAT or _XTYPE_SPROW depending on
      whether r_out is a row of a _xsprow structure
      or a _xsprow variable
   -- returns r_out
 */
_xsprow *_xsprow_mltadd(const _xsprow *r1, const _xsprow *r2, double alpha, int j0, _xsprow *r_out, int type){
    int idx1, idx2, idx_out, len1, len2, len_out;
    _xrow_elt *elt1, *elt2, *elt_out;
   
    if (!r1 || !r2)
        _xerror(_XE_NULL,"_xsprow_mltadd");
    if (r1 == r_out || r2 == r_out)
        _xerror(_XE_INSITU,"_xsprow_mltadd");
    if (j0 < 0)
        _xerror(_XE_BOUNDS,"_xsprow_mltadd");
    if (!r_out)
        r_out = _xsprow_get(_XMINROWLEN);
   
    /* Initialise */
    len1 = r1->len; len2 = r2->len; len_out = r_out->maxlen;
    idx1    = _xsprow_idx(r1,j0);
    idx2    = _xsprow_idx(r2,j0);
    idx_out = _xsprow_idx(r_out,j0);
    idx1    = (idx1 < 0) ? -(idx1+2) : idx1;
    idx2    = (idx2 < 0) ? -(idx2+2) : idx2;
    idx_out = (idx_out < 0) ? -(idx_out+2) : idx_out;
    elt1    = &(r1->elt[idx1]);
    elt2    = &(r2->elt[idx2]);
    elt_out = &(r_out->elt[idx_out]);
   
    while (idx1 < len1 || idx2 < len2){
        if (idx_out >= len_out){   /* r_out is too small */
            r_out->len = idx_out;
            r_out = _xsprow_xpd(r_out,0,type);
            len_out = r_out->maxlen;
            elt_out = &(r_out->elt[idx_out]);
        }
        if (idx2 >= len2 || (idx1 < len1 && elt1->col <= elt2->col)){
            elt_out->col = elt1->col;
            elt_out->val = elt1->val;
            if (idx2 < len2 && elt1->col == elt2->col){
                elt_out->val += (_xreal)(alpha*elt2->val);
                elt2++;
                idx2++;
            }
            elt1++;
            idx1++;
        }
        else {
            elt_out->col = elt2->col;
            elt_out->val = (_xreal)(alpha*elt2->val);
            elt2++;
            idx2++;
        }
        elt_out++;
        idx_out++;
    }
    r_out->len = idx_out;
   
    return (r_out);
}

/*!
   set r_out <- r1 + r2
   -- cannot be in situ
   -- only for columns j0, j0+1, ...
   -- type must be _XTYPE_SPMAT or _XTYPE_SPROW depending on
      whether r_out is a row of a _xsprow structure
      or a _xsprow variable
   -- returns r_out
 */
_xsprow *_xsprow_add(const _xsprow *r1, const _xsprow *r2, int j0, _xsprow *r_out, int type){
    int idx1, idx2, idx_out, len1, len2, len_out;
    _xrow_elt *elt1, *elt2, *elt_out;
   
    if (!r1 || !r2)
        _xerror(_XE_NULL,"_xsprow_add");
    if (r1 == r_out || r2 == r_out)
        _xerror(_XE_INSITU,"_xsprow_add");
    if (j0 < 0)
        _xerror(_XE_BOUNDS,"_xsprow_add");
    if (!r_out )
        r_out = _xsprow_get(_XMINROWLEN);
   
    /* Initialise */
    len1 = r1->len; len2 = r2->len; len_out = r_out->maxlen;
    idx1    = _xsprow_idx(r1,j0);
    idx2    = _xsprow_idx(r2,j0);
    idx_out = _xsprow_idx(r_out,j0);
    idx1    = (idx1 < 0) ? -(idx1+2) : idx1;
    idx2    = (idx2 < 0) ? -(idx2+2) : idx2;
    idx_out = (idx_out < 0) ? -(idx_out+2) : idx_out;
    elt1    = &(r1->elt[idx1]);
    elt2    = &(r2->elt[idx2]);
    elt_out = &(r_out->elt[idx_out]);
   
    while (idx1 < len1 || idx2 < len2){
        if (idx_out >= len_out){   /* r_out is too small */
            r_out->len = idx_out;
            r_out = _xsprow_xpd(r_out,0,type);
            len_out = r_out->maxlen;
            elt_out = &(r_out->elt[idx_out]);
        }
        if (idx2 >= len2 || (idx1 < len1 && elt1->col <= elt2->col)){
            elt_out->col = elt1->col;
            elt_out->val = elt1->val;
            if (idx2 < len2 && elt1->col == elt2->col){
                elt_out->val += elt2->val;
                elt2++;
                idx2++;
            }
            elt1++;
            idx1++;
        }
        else {
            elt_out->col = elt2->col;
            elt_out->val = elt2->val;
            elt2++;
            idx2++;
        }
        elt_out++;
        idx_out++;
    }
    r_out->len = idx_out;
   
    return (r_out);
}

/*!
   set r_out <- r1 - r2
   -- cannot be in situ
   -- only for columns j0, j0+1, ...
   -- type must be _XTYPE_SPMAT or _XTYPE_SPROW depending on
      whether r_out is a row of a _xsprow structure
      or a _xsprow variable
   -- returns r_out
 */
_xsprow *_xsprow_sub(const _xsprow *r1, const _xsprow *r2, int j0, _xsprow *r_out, int type){
    int idx1, idx2, idx_out, len1, len2, len_out;
    _xrow_elt *elt1, *elt2, *elt_out;
   
    if (!r1 || !r2)
        _xerror(_XE_NULL,"_xsprow_sub");
    if (r1 == r_out || r2 == r_out)
        _xerror(_XE_INSITU,"_xsprow_sub");
    if (j0 < 0)
        _xerror(_XE_BOUNDS,"_xsprow_sub");
    if (!r_out)
        r_out = _xsprow_get(_XMINROWLEN);
   
    /* Initialise */
    len1 = r1->len; len2 = r2->len; len_out = r_out->maxlen;
    idx1    = _xsprow_idx(r1,j0);
    idx2    = _xsprow_idx(r2,j0);
    idx_out = _xsprow_idx(r_out,j0);
    idx1    = (idx1 < 0) ? -(idx1+2) : idx1;
    idx2    = (idx2 < 0) ? -(idx2+2) : idx2;
    idx_out = (idx_out < 0) ? -(idx_out+2) : idx_out;
    elt1    = &(r1->elt[idx1]);
    elt2    = &(r2->elt[idx2]);
    elt_out = &(r_out->elt[idx_out]);
   
    while (idx1 < len1 || idx2 < len2){
        if (idx_out >= len_out){   /* r_out is too small */
            r_out->len = idx_out;
            r_out = _xsprow_xpd(r_out,0,type);
            len_out = r_out->maxlen;
            elt_out = &(r_out->elt[idx_out]);
        }
        if (idx2 >= len2 || (idx1 < len1 && elt1->col <= elt2->col)){
            elt_out->col = elt1->col;
            elt_out->val = elt1->val;
            if (idx2 < len2 && elt1->col == elt2->col){
                elt_out->val -= elt2->val;
                elt2++;
                idx2++;
            }
            elt1++;
            idx1++;
        }
        else {
            elt_out->col = elt2->col;
            elt_out->val = -elt2->val;
            elt2++;
            idx2++;
        }
        elt_out++;
        idx_out++;
    }
    r_out->len = idx_out;
   
    return (r_out);
}

/*!
   set r_out <- alpha*r1 
   -- can be in situ
   -- only for columns j0, j0+1, ...
   -- returns r_out
 */
_xsprow *_xsprow_smlt(const _xsprow *r1, double alpha, int j0, _xsprow *r_out, int type){
    int idx1, idx_out, len1;
    _xrow_elt *elt1, *elt_out;
   
    if (!r1)
        _xerror(_XE_NULL,"_xsprow_smlt");
    if (j0 < 0)
        _xerror(_XE_BOUNDS,"_xsprow_smlt");
    if (!r_out)
        r_out = _xsprow_get(_XMINROWLEN);
   
    /* Initialise */
    len1 = r1->len;
    idx1    = _xsprow_idx(r1,j0);
    idx_out = _xsprow_idx(r_out,j0);
    idx1    = (idx1 < 0) ? -(idx1+2) : idx1;
    idx_out = (idx_out < 0) ? -(idx_out+2) : idx_out;
    elt1    = &(r1->elt[idx1]);

    r_out = _xsprow_resize(r_out,idx_out+len1-idx1,type);  
    elt_out = &(r_out->elt[idx_out]);

    for (; idx1 < len1; elt1++,elt_out++,idx1++,idx_out++){
        elt_out->col = elt1->col;
        elt_out->val = (_xreal)(alpha*elt1->val);
    }

    r_out->len = idx_out;

    return (r_out);
}

/*!
   set the j-th column entry of the sparse row r
   -- Note: destroys the usual column & row access paths
 */
double _xsprow_set_val(_xsprow *r, int j, double val){
    int idx, idx2, new_len;
   
    if (!r)
        _xerror(_XE_NULL,"_xsprow_set_val");
   
    idx = _xsprow_idx(r,j);
    if (idx >= 0){
        r->elt[idx].val = (_xreal)val;
        return (val);
    }
   
    if (idx < -1){
        /* shift & insert new value */
        idx = -(idx+2); /* this is the intended insertion index */
        if (r->len >= r->maxlen){
            r->len = r->maxlen;
            new_len = _xmax(2*r->maxlen+1,5);
            if (_xmem_info_is_on()){
                _xmem_bytes(_XTYPE_SPROW,r->maxlen*sizeof(_xrow_elt),
                            new_len*sizeof(_xrow_elt)); 
            }
           
            r->elt = _xrenew(r->elt,new_len,_xrow_elt);
            if (!r->elt) /* can't allocate */
                _xerror(_XE_MEM,"_xsprow_set_val");
            r->maxlen = 2*r->maxlen+1;
        }
        for (idx2 = r->len-1; idx2 >= idx; idx2--)
            _xmem_copy((char *)(&(r->elt[idx2])),
                       (char *)(&(r->elt[idx2+1])),sizeof(_xrow_elt));
    
        r->len++;
        r->elt[idx].col = j;
        r->elt[idx].nxt_row = -1;
        r->elt[idx].nxt_idx = -1;
        return (r->elt[idx].val = (_xreal)val);
    }
    /* else -- idx == -1, error in index/matrix! */
    return (0.0);
}

/*!
   print relevant information about the sparse row r
 */
void _xsprow_dump(FILE *fp, const _xsprow *r){
    int j_idx;
    _xrow_elt *elts;
   
    (void) fprintf(fp,"SparseRow dump:\n");
    if (!r){
        (void) fprintf(fp,"*** NULL row ***\n");
        return;
    }
    (void) fprintf(fp,"row: len = %d, maxlen = %d, diag idx = %d\n",
                   r->len,r->maxlen,r->diag);
    (void) fprintf(fp,"element list @ 0x%lx\n",(long)(r->elt));
    if (!r->elt){
        (void) fprintf(fp,"*** NULL element list ***\n");
        return;
    }
    elts = r->elt;
    for (j_idx = 0; j_idx < r->len; j_idx++, elts++)
        (void) fprintf(fp,"Col: %d, Val: %g, nxt_row = %d, nxt_idx = %d\n",
                       elts->col,elts->val,elts->nxt_row,elts->nxt_idx);
    (void) fprintf(fp,"\n");
}

/*!
   print a representation of r on stream fp
 */
void _xsprow_foutput(FILE *fp, const _xsprow *r){
    int i, len;
    _xrow_elt *e;
   
    if (!r){
        (void) fprintf(fp,"SparseRow: **** NULL ****\n");
        return;
    }
    len = r->len;
    (void) fprintf(fp,"SparseRow: length: %d\n",len);
    for (i = 0, e = r->elt; i < len; i++, e++)
        (void) fprintf(fp,"Column %d: %g, next row: %d, next index %d\n",
                       e->col, e->val, e->nxt_row, e->nxt_idx);
}

/*!
   get sparse matrix
   -- len is number of elements available for each row without
   allocating further memory
 */
_xspmat *_xspmat_get(int m, int n, int maxlen){
    _xspmat *A;
    _xsprow *rows;
    int i;
   
    if (m < 0 || n < 0)
        _xerror(_XE_NEG,"_xspmat_get");

    maxlen = _xmax(maxlen,1);
    A = _xnew(_xspmat);
    if (!A) /* can't allocate */
        _xerror(_XE_MEM,"_xspmat_get");
    else if (_xmem_info_is_on()){
        _xmem_bytes(_XTYPE_SPMAT,0,sizeof(_xspmat));
        _xmem_numvar(_XTYPE_SPMAT,1);
    }
   
    A->row = rows = _xnew_a(m,_xsprow);
    if (!A->row ) /* can't allocate */
        _xerror(_XE_MEM,"_xspmat_get");
    else if (_xmem_info_is_on()){
        _xmem_bytes(_XTYPE_SPMAT,0,m*sizeof(_xsprow));
    }
   
    A->start_row = _xnew_a(n,int);
    A->start_idx = _xnew_a(n,int);
    if (!A->start_row || !A->start_idx) /* can't allocate */
        _xerror(_XE_MEM,"_xspmat_get");
    else if (_xmem_info_is_on()){
        _xmem_bytes(_XTYPE_SPMAT,0,2*n*sizeof(int));
    }
    for (i = 0; i < n; i++)
        A->start_row[i] = A->start_idx[i] = -1;
   
    A->m = A->max_m = m;
    A->n = A->max_n = n;
   
    for (i = 0; i < m; i++, rows++){
        rows->elt = _xnew_a(maxlen,_xrow_elt);
        if (!rows->elt)
            _xerror(_XE_MEM,"_xspmat_get");
        else if (_xmem_info_is_on()){
            _xmem_bytes(_XTYPE_SPMAT,0,maxlen*sizeof(_xrow_elt));
        }
        rows->len = 0;
        rows->maxlen = maxlen;
        rows->diag = -1;
    }
   
    return (A);
}

/*!
   zero all the (represented) elements of a sparse matrix
 */
_xspmat *_xspmat_zero(_xspmat *A){
    int i, idx, len;
    _xrow_elt *elt;
   
    if (!A)
        _xerror(_XE_NULL,"_xspmat_zero");
   
    for (i = 0; i < A->m; i++){
        elt = A->row[i].elt;
        len = A->row[i].len;
        for (idx = 0; idx < len; idx++)
            (*elt++).val = 0.0;
    }
   
    return (A);
}

/*!
   construct a copy of a given matrix
   -- note that the max_len fields (etc) are no larger in the copy
   than necessary
   -- result is returned
 */
_xspmat *_xspmat_copy(const _xspmat *A){
    _xspmat *out;
    _xsprow *row1, *row2;
    int i;
   
    if (!A)
        _xerror(_XE_NULL,"_xspmat_copy");
    if (!(out=_xnew(_xspmat)))
        _xerror(_XE_MEM,"_xspmat_copy");
    else if (_xmem_info_is_on()){
        _xmem_bytes(_XTYPE_SPMAT,0,sizeof(_xspmat));
        _xmem_numvar(_XTYPE_SPMAT,1);
    }
    out->m = out->max_m = A->m; out->n = out->max_n = A->n;

    /* set up rows */
    if (!(out->row=_xnew_a(A->m,_xsprow)))
        _xerror(_XE_MEM,"_xspmat_copy");
    else if (_xmem_info_is_on()){
        _xmem_bytes(_XTYPE_SPMAT,0,A->m*sizeof(_xsprow));
    }
    for (i = 0; i < A->m; i++){
        row1 = &(A->row[i]);
        row2 = &(out->row[i]);
        if (!(row2->elt=_xnew_a(_xmax(row1->len,3),_xrow_elt)))
            _xerror(_XE_MEM,"_xspmat_copy");
        else if (_xmem_info_is_on()){
            _xmem_bytes(_XTYPE_SPMAT,0,_xmax(row1->len,3)*sizeof(_xrow_elt));
        }
        row2->len = row1->len;
        row2->maxlen = _xmax(row1->len,3);
        row2->diag = row1->diag;
        _xmem_copy((char *)(row1->elt),(char *)(row2->elt),
                   row1->len*sizeof(_xrow_elt));
    }
   
    /* set up start arrays -- for column access */
    if (!(out->start_idx=_xnew_a(A->n,int)) ||
        !(out->start_row=_xnew_a(A->n,int)))
        _xerror(_XE_MEM,"_xspmat_copy");
    else if (_xmem_info_is_on()){
        _xmem_bytes(_XTYPE_SPMAT,0,2*A->n*sizeof(int));
    }
    _xmem_copy((char *)(A->start_idx),(char *)(out->start_idx),A->n*sizeof(int));
    _xmem_copy((char *)(A->start_row),(char *)(out->start_row),A->n*sizeof(int));
   
    return (out);
}

/*!
   set column access path; i.e. nxt_row, nxt_idx fields
   -- returns A
 */
_xspmat *_xspmat_col_access(_xspmat *A){
    int i, j, j_idx, len, m, n;
    _xsprow *row;
    _xrow_elt *r_elt;
    int *start_row, *start_idx;
   
    if (!A)
        _xerror(_XE_NULL,"_xspmat_col_access");
   
    m = A->m; n = A->n;
   
    /* initialise start_row and start_idx */
    start_row = A->start_row; start_idx = A->start_idx;
    for (j = 0; j < n; j++){
        *start_row++ = -1;
        *start_idx++ = -1;
    }
   
    start_row = A->start_row; start_idx = A->start_idx;
   
    /* now work UP the rows, setting nxt_row, nxt_idx fields */
    for (i = m-1; i >= 0; i--){
        row = &(A->row[i]);
        r_elt = row->elt;
        len   = row->len;
        for (j_idx = 0; j_idx < len; j_idx++, r_elt++){
            j = r_elt->col;
            r_elt->nxt_row = start_row[j];
            r_elt->nxt_idx = start_idx[j];
            start_row[j] = i;
            start_idx[j] = j_idx;
        }
    }
   
    A->flag_col = _XTRUE;
    return (A);
}

/*!
   set diagonal access path(s)
 */
_xspmat *_xspmat_diag_access(_xspmat *A){
    int i, m;
    _xsprow *row;
   
    if (!A)
        _xerror(_XE_NULL,"_xspmat_diag_access");
   
    m = A->m;
    row = A->row;
    for (i = 0; i < m; i++, row++)
        row->diag = _xsprow_idx(row,i);
   
    A->flag_diag = _XTRUE;
   
    return (A);
}

/*
   counts non-zeros in A
 */
static int _xnonzeros(const _xspmat *A){
    int cnt, i;

    if (!A)
        return 0;
    cnt = 0;
    for (i = 0; i < A->m; i++)
        cnt += A->row[i].len;

    return (cnt);
}

/*
  for _xspmat_bkpfactor()
  -- check that column access path is OK
 */
int _xspmat_chk_col_access(const _xspmat *A){
    int cnt_nz, j, row, idx;
    _xsprow *r;
    _xrow_elt *e;

    if (!A)
        _xerror(_XE_NULL,"_xspmat_chk_col_access");

    /* count nonzeros as we go down columns */
    cnt_nz = 0;
    for (j = 0; j < A->n; j++){
        row = A->start_row[j];
        idx = A->start_idx[j];
        while (row >= 0){
            if (row >= A->m || idx < 0)
                return (_XFALSE);
            r = &(A->row[row]);
            if (idx >= r->len)
                return (_XFALSE);
            e = &(r->elt[idx]);
            if (e->nxt_row >= 0 && e->nxt_row <= row)
                return (_XFALSE);
            row = e->nxt_row;
            idx = e->nxt_idx;
            cnt_nz++;
        }
    }

    return cnt_nz != _xnonzeros(A) ? _XFALSE : _XTRUE;
}

/*!
   copy sparse matrix (type 2) 
   -- keeps structure of the out matrix
 */
_xspmat *_xspmat_copy2(const _xspmat *A, _xspmat *out){
    int i;
    _xsprow *r1, *r2;
    _XSTATIC _xsprow *scratch = (_xsprow *)NULL;
   
    if (!A)
        _xerror(_XE_NULL,"_xspmat_copy2");
    if (!out)
        out = _xspmat_get(A->m,A->n,10);
    if (!scratch){
        scratch = _xsprow_xpd(scratch,_XMINROWLEN,_XTYPE_SPROW);
        _XMEM_STAT_REG(scratch,_XTYPE_SPROW);
    }

    if (out->m < A->m){
        if (_xmem_info_is_on()){
            _xmem_bytes(_XTYPE_SPMAT,A->max_m*sizeof(_xsprow),A->m*sizeof(_xsprow));
        }

        out->row = _xrenew(out->row,A->m,_xsprow);
        if (!out->row)
            _xerror(_XE_MEM,"_xspmat_copy2");
        
        for (i = out->m; i < A->m; i++){
            out->row[i].elt = _xnew_a(_XMINROWLEN,_xrow_elt);
            if (!out->row[i].elt)
                _xerror(_XE_MEM,"_xspmat_copy2");
            else if (_xmem_info_is_on()){
                _xmem_bytes(_XTYPE_SPMAT,0,_XMINROWLEN*sizeof(_xrow_elt));
            }
        
            out->row[i].maxlen = _XMINROWLEN;
            out->row[i].len = 0;
        }
        out->m = A->m;
    }
   
    out->flag_col = out->flag_diag = _XFALSE;
    for (i = 0; i < A->m; i++){
        r1 = &(A->row[i]); r2 = &(out->row[i]);
        _xsprow_copy(r1,r2,scratch,_XTYPE_SPROW);
        if (r2->maxlen < scratch->len)
            _xsprow_xpd(r2,scratch->len,_XTYPE_SPMAT);
        _xmem_copy((char *)(scratch->elt),(char *)(r2->elt),
                   scratch->len*sizeof(_xrow_elt));
        r2->len = scratch->len;
    }

    _xspmat_col_access(out);

#ifdef _XTHREAD_SAFE
    _xsprow_free(scratch);
#endif

    return (out);
}

/*!
   resize a sparse matrix
   -- don't destroying any contents if possible
   -- returns resized matrix
 */
_xspmat *_xspmat_resize(_xspmat *A, int m, int n){
    int i, len;
    _xsprow *r;
   
    if (m < 0 || n < 0)
        _xerror(_XE_NEG,"_xspmat_resize");

    if (!A)
        return _xspmat_get(m,n,10);

    if (m == A->m && n == A->n)
        return (A);

    if (m <= A->max_m){
        for (i = A->m; i < m; i++)
            A->row[i].len = 0;
        A->m = m;
    }
    else {
        if (_xmem_info_is_on()){
            _xmem_bytes(_XTYPE_SPMAT,A->max_m*sizeof(_xsprow),m*sizeof(_xsprow));
        }

        A->row = _xrenew(A->row,(unsigned)m,_xsprow);
        if (!A->row)
            _xerror(_XE_MEM,"_xspmat_resize");
        for (i = A->m; i < m; i++){
            if (!(A->row[i].elt = _xnew_a(_XMINROWLEN,_xrow_elt)))
                _xerror(_XE_MEM,"_xspmat_resize");
            else if (_xmem_info_is_on()){
                _xmem_bytes(_XTYPE_SPMAT,0,_XMINROWLEN*sizeof(_xrow_elt));
            }
            A->row[i].len = 0; A->row[i].maxlen = _XMINROWLEN;
        }
        A->m = A->max_m = m;
    }

    /* update number of rows */
    A->n = n;

    /* do we need to increase the size of start_idx[] and start_row[] ? */
    if (n > A->max_n){  /* only have to update the start_idx & start_row arrays */
        if (_xmem_info_is_on()){
            _xmem_bytes(_XTYPE_SPMAT,2*A->max_n*sizeof(int),2*n*sizeof(int));
        }

        A->start_row = _xrenew(A->start_row,(unsigned)n,int);
        A->start_idx = _xrenew(A->start_idx,(unsigned)n,int);
        if (!A->start_row || !A->start_idx)
            _xerror(_XE_MEM,"_xspmat_resize");
        A->max_n = n; /* ...and update max_n */

        return (A);
    }

    if (n <= A->n)
        /* make sure that all rows are truncated just before column n */
        for (i = 0; i < A->m; i++){
            r = &(A->row[i]);
            len = _xsprow_idx(r,n);
            if (len < 0)
                len = -(len+2);
            if (len < 0)
                _xerror(_XE_MEM,"_xspmat_resize");
            r->len = len;
        }
  
    return (A);
}

/*!
   remove zeros and near-zeros from a sparse matrix
 */
_xspmat *_xspmat_compact(_xspmat *A, double tol){
    int i, idx1, idx2;
    _xsprow *r;
    _xrow_elt *elt1, *elt2;
   
    if (!A)
        _xerror(_XE_NULL,"_xspmat_compact");
    if (tol < 0.0)
        _xerror(_XE_RANGE,"_xspmat_compact");
   
    A->flag_col = A->flag_diag = _XFALSE;
   
    for (i = 0; i < A->m; i++){
        r = &(A->row[i]);
        elt1 = elt2 = r->elt;
        idx1 = idx2 = 0;
        while (idx1 < r->len){
            if (fabs(elt1->val) <= tol){
                idx1++;
                elt1++;
                continue;
            }
            if (elt1 != elt2)
                _xmem_copy(elt1,elt2,sizeof(_xrow_elt));
            idx1++;
            elt1++;
            idx2++;
            elt2++;
        }
        r->len = idx2;
    }
   
    return (A);
}

/*!
   return the (i,j) entry of the sparse matrix A
 */
double _xspmat_get_val(const _xspmat *A, int i, int j){
    _xsprow *r;
    int idx;
   
    if (!A)
        _xerror(_XE_NULL,"_xspmat_get_val");
    if (i < 0 || i >= A->m || j < 0 || j >= A->n)
        _xerror(_XE_SIZES,"_xspmat_get_val");
   
    r = A->row+i;
    idx = _xsprow_idx(r,j);

    return (idx < 0 ? 0.0 : r->elt[idx].val);
}

/*!
   set the (i,j) entry of the sparse matrix A
 */
double _xspmat_set_val(_xspmat *A, int i, int j, double val){
    _xsprow *r;
    int idx, idx2, new_len;
   
    if (!A)
        _xerror(_XE_NULL,"_xspmat_set_val");
    if (i < 0 || i >= A->m || j < 0 || j >= A->n)
        _xerror(_XE_SIZES,"_xspmat_set_val");
   
    r = A->row+i;
    idx = _xsprow_idx(r,j);
    if (idx >= 0){
        r->elt[idx].val = (_xreal)val;
        return val;
    }
   
    if ( idx < -1 ){
        /* Note: this destroys the column & diag access paths */
        A->flag_col = A->flag_diag = _XFALSE;
        /* shift & insert new value */
        idx = -(idx+2); /* this is the intended insertion index */
        if (r->len >= r->maxlen){
            r->len = r->maxlen;
            new_len = _xmax(2*r->maxlen+1,5);
            if (_xmem_info_is_on()){
                _xmem_bytes(_XTYPE_SPMAT,A->row[i].maxlen*sizeof(_xrow_elt),
                            new_len*sizeof(_xrow_elt));
            }
            r->elt = _xrenew(r->elt,new_len,_xrow_elt);
            if (!r->elt) /* can't allocate */
                _xerror(_XE_MEM,"_xspmat_set_val");
            r->maxlen = 2*r->maxlen+1;
        }
        for (idx2 = r->len-1; idx2 >= idx; idx2--)
            _xmem_copy((char *)(&(r->elt[idx2])),
                       (char *)(&(r->elt[idx2+1])),sizeof(_xrow_elt));
        r->len++;
        r->elt[idx].col = j;
        return (r->elt[idx].val = (_xreal)val);
    }
    /* else -- idx == -1, error in index/matrix! */
    return (0.0);
}

/*!
   sparse matrix/dense vector multiply
   -- result is in out, which is returned unless out==NULL on entry
   --  if out==NULL on entry then the result vector is created
 */
_xvec *_xspmat_mv_mlt(const _xspmat *A, const _xvec *x, _xvec *out){
    int i, j_idx, m, n, max_idx;
    _xreal sum, *x_ve;
    _xsprow *r;
    _xrow_elt *elts;
   
    if (!A || !x)
        _xerror(_XE_NULL,"_xspmat_mv_mlt");
    if (x->dim != A->n)
        _xerror(_XE_SIZES,"_xspmat_mv_mlt");
    if (!out || (int)(out->dim) < A->m)
        out = _xvec_resize(out,A->m);
    if (out == x)
        _xerror(_XE_INSITU,"_xspmat_mv_mlt");
    m = A->m; n = A->n;
    x_ve = x->ve;
   
    for (i = 0; i < m; i++){
        sum = 0.0;
        r = &(A->row[i]);
        max_idx = r->len;
        elts    = r->elt;
        for (j_idx = 0; j_idx < max_idx; j_idx++, elts++)
            sum += elts->val*x_ve[elts->col];
        out->ve[i] = sum;
    }
   
    return (out);
}

/*!
   sparse matrix/dense vector multiply from left
   -- result is in out, which is returned unless out==NULL on entry
   -- if out==NULL on entry then result vector is created & returned
 */
_xvec *_xspmat_vm_mlt(const _xspmat *A, const _xvec *x, _xvec *out){
    int i, j_idx, m, n, max_idx;
    _xreal tmp, *x_ve, *out_ve;
    _xsprow *r;
    _xrow_elt *elts;
   
    if (!A || !x)
        _xerror(_XE_NULL,"_xspmat_vm_mlt");
    if (x->dim != A->m)
        _xerror(_XE_SIZES,"_xspmat_vm_mlt");
    if (!out || (int)(out->dim) < A->n)
        out = _xvec_resize(out,A->n);
    if (out == x)
        _xerror(_XE_INSITU,"_xspmat_vm_mlt");
   
    m = A->m; n = A->n;
    _xvec_zero(out);
    x_ve = x->ve; out_ve = out->ve;
   
    for (i = 0; i < m; i++){
        r = A->row+i;
        max_idx = r->len;
        elts    = r->elt;
        tmp = x_ve[i];
        for (j_idx = 0; j_idx < max_idx; j_idx++, elts++)
            out_ve[elts->col] += elts->val*tmp;
    }
   
    return (out);
}

/*!
   to allocate memory to many arguments. 
   the function should be called:
   _xspmat_get_vars(m,n,deg,&x,&y,&z,...,NULL);
   where 
     int m,n,deg;
     _xspmat *x, *y, *z,...;
     the last argument should be NULL ! 
     m x n is the dimension of matrices x,y,z,...
     returned value is equal to the number of allocated variables
 */
int _xspmat_get_vars(int m, int n, int deg, ...){
    va_list ap;
    int i=0;
    _xspmat **par;
   
    va_start(ap, deg);
    while (par = va_arg(ap,_xspmat **)){ /* NULL ends the list*/
        *par = _xspmat_get(m,n,deg);
        i++;
    } 
    va_end(ap);

    return (i);
}

/*! 
   to resize memory for many arguments. 
   the function should be called:
   _xspmat_resize_vars(m,n,&x,&y,&z,...,NULL);
   where 
     int m,n;
     _xspmat *x, *y, *z,...;
     the last argument should be NULL ! 
     m X n is the resized dimension of matrices x,y,z,...
     returned value is equal to the number of allocated variables.
     If one of x,y,z,.. arguments is NULL then memory is allocated to this 
     argument. 
*/
int _xspmat_resize_vars(int m, int n, ...){
    va_list ap;
    int i=0;
    _xspmat **par;
   
    va_start(ap, n);
    while (par = va_arg(ap,_xspmat **)){ /* NULL ends the list*/
        *par = _xspmat_resize(*par,m,n);
        i++;
    } 
    va_end(ap);

    return (i);
}

/*
   to deallocate memory for many arguments. 
   the function should be called:
   _xspmat_free_vars(&x,&y,&z,...,NULL);
   where 
     _xspmat *x, *y, *z,...;
     the last argument should be NULL ! 
     there must be at least one not NULL argument.
     returned value is equal to the number of allocated variables.
     returned value of x,y,z,.. is NULL.
 */
int _xspmat_free_vars(_xspmat **va, ...){
    va_list ap;
    int i=1;
    _xspmat **par;
   
    _xspmat_free(*va);
    *va = (_xspmat *)NULL;
    va_start(ap, va);
    while (par = va_arg(ap,_xspmat **)){ /* NULL ends the list*/
        _xspmat_free(*par); 
        *par = (_xspmat *)NULL;
        i++;
    } 
    va_end(ap);

    return (i);
}

/*!
   C = A+B, can be in situ
 */
_xspmat *_xspmat_add(const _xspmat *A, const _xspmat *B, _xspmat *C){
    int i, in_situ;
    _xsprow *rc;
    _XSTATIC _xsprow *tmp = (_xsprow *)NULL;

    if (!A || !B)
        _xerror(_XE_NULL,"_xspmat_add");
    if (A->m != B->m || A->n != B->n)
        _xerror(_XE_SIZES,"_xspmat_add");

    in_situ = (C == A || C == B) ? _XTRUE : _XFALSE;
    if (!C)
        C = _xspmat_get(A->m,A->n,5);
    else {
        if (C->m != A->m || C->n != A->n)
            _xerror(_XE_SIZES,"_xspmat_add");
        if (!in_situ) 
            _xspmat_zero(C);
    }

    if (!tmp && in_situ){
        tmp = _xsprow_get(_XMINROWLEN);
        _XMEM_STAT_REG(tmp,_XTYPE_SPROW);
    }

    if (in_situ)
        for (i=0; i < A->m; i++){
            rc = &(C->row[i]);
            _xsprow_add(&(A->row[i]),&(B->row[i]),0,tmp,_XTYPE_SPROW);
            _xsprow_resize(rc,tmp->len,_XTYPE_SPMAT);
            _xmem_copy(tmp->elt,rc->elt,tmp->len*sizeof(_xrow_elt));
            rc->len = tmp->len;
        }
    else
        for (i=0; i < A->m; i++){
            _xsprow_add(&(A->row[i]),&(B->row[i]),0,&(C->row[i]),_XTYPE_SPMAT);
        }

    C->flag_col = C->flag_diag = _XFALSE;

#ifdef _XTHREAD_SAFE
    _xsprow_free(tmp);
#endif

    return (C);
}

/*!
   C = A-B, cannot be in situ
 */
_xspmat *_xspmat_sub(const _xspmat *A, const _xspmat *B, _xspmat *C){
    int i, in_situ;
    _xsprow *rc;
    _XSTATIC _xsprow *tmp = (_xsprow *)NULL;
   
    if (!A || !B)
        _xerror(_XE_NULL,"_xspmat_sub");
    if (A->m != B->m || A->n != B->n)
        _xerror(_XE_SIZES,"_xspmat_sub");

    in_situ = (C == A || C == B) ? _XTRUE : _XFALSE;
    if (!C)
        C = _xspmat_get(A->m,A->n,5);
    else {
        if (C->m != A->m || C->n != A->n)
            _xerror(_XE_SIZES,"_xspmat_sub");
        if (!in_situ)
            _xspmat_zero(C);
    }

    if (!tmp && in_situ){
        tmp = _xsprow_get(_XMINROWLEN);
        _XMEM_STAT_REG(tmp,_XTYPE_SPROW);
    }

    if (in_situ)
        for (i=0; i < A->m; i++){
            rc = &(C->row[i]);
            _xsprow_sub(&(A->row[i]),&(B->row[i]),0,tmp,_XTYPE_SPROW);
            _xsprow_resize(rc,tmp->len,_XTYPE_SPMAT);
            _xmem_copy(tmp->elt,rc->elt,tmp->len*sizeof(_xrow_elt));
            rc->len = tmp->len;
        }
    else
        for (i=0; i < A->m; i++){
            _xsprow_sub(&(A->row[i]),&(B->row[i]),0,&(C->row[i]),_XTYPE_SPMAT);
        }

    C->flag_col = C->flag_diag = _XFALSE;

#ifdef _XTHREAD_SAFE
    _xsprow_free(tmp);
#endif

    return (C);
}

/*!
   B = alpha*A, can be in situ
 */
_xspmat *_xspmat_smlt(const _xspmat *A, double alpha, _xspmat *B){
    int i;

    if (!A)
        _xerror(_XE_NULL,"_xspmat_smlt");
    if (!B)
        B = _xspmat_get(A->m,A->n,5);
    else
        if (A->m != B->m || A->n != B->n)
            _xerror(_XE_SIZES,"_xspmat_smlt");

    for (i=0; i < A->m; i++){
        _xsprow_smlt(&(A->row[i]),alpha,0,&(B->row[i]),_XTYPE_SPMAT);
    }
   
    return (B);
}

/*!
   C = A+alpha*B, cannot be in situ
 */
_xspmat *_xspmat_mltadd(const _xspmat *A, const _xspmat *B, double alpha, _xspmat *C){
    int i, in_situ;
    _xsprow *rc;
    _XSTATIC _xsprow *tmp = (_xsprow *)NULL;

    if (!A || !B)
        _xerror(_XE_NULL,"_xspmat_mltadd");
    if (A->m != B->m || A->n != B->n)
        _xerror(_XE_SIZES,"_xspmat_mltadd");

    in_situ = (C == A || C == B) ? _XTRUE : _XFALSE;
    if (!C)
        C = _xspmat_get(A->m,A->n,5);
    else {
        if (C->m != A->m || C->n != A->n)
            _xerror(_XE_SIZES,"_xspmat_mltadd");
        if (!in_situ)
            _xspmat_zero(C);
    }

    if (!tmp && in_situ){
        tmp = _xsprow_get(_XMINROWLEN);
        _XMEM_STAT_REG(tmp,_XTYPE_SPROW);
    }

    if (in_situ)
        for (i=0; i < A->m; i++){
            rc = &(C->row[i]);
            _xsprow_mltadd(&(A->row[i]),&(B->row[i]),alpha,0,tmp,_XTYPE_SPROW);
            _xsprow_resize(rc,tmp->len,_XTYPE_SPMAT);
            _xmem_copy(tmp->elt,rc->elt,tmp->len*sizeof(_xrow_elt));
            rc->len = tmp->len;
        }
    else
        for (i=0; i < A->m; i++){
            _xsprow_mltadd(&(A->row[i]),&(B->row[i]),alpha,0,&(C->row[i]),_XTYPE_SPMAT);
        }
   
    C->flag_col = C->flag_diag = _XFALSE;

#ifdef _XTHREAD_SAFE
    _xsprow_free(tmp);
#endif
   
    return (C);
}

/*!
   convert a sparse matrix to a dense one
 */
_xmat *_xspmat_m2dense(const _xspmat *A, _xmat *out){
    int i, j_idx;
    _xsprow *row;
    _xrow_elt *elt;
   
    if (!A)
        _xerror(_XE_NULL,"_xspmat_m2dense");
    if (!out || (int)(out->m) < A->m || (int)(out->n) < A->n)
        out = _xmat_get(A->m,A->n);
   
    _xmat_zero(out);
    for (i = 0; i < A->m; i++){
        row = &(A->row[i]);
        elt = row->elt;
        for (j_idx = 0; j_idx < row->len; j_idx++, elt++)
            out->me[i][elt->col] = elt->val;
    }
   
    return (out);
}

/*!
    input sparse matrix from stream/file fp
  -- uses friendly input routine if fp is a tty
  -- uses format identical to output format otherwise
 */
_xspmat *_xspmat_finput(FILE *fp){
    int i, len, ret_val;
    int col, curr_col, m, n, tmp;
    _xreal val;
    _xspmat *A;
    _xsprow *rows;

#define _XMINSCRATCH 100

    static _xrow_elt *scratch = (_xrow_elt *)NULL;
    static int  scratch_len = 0;
    
    if (!scratch){
        scratch = _xnew_a(_XMINSCRATCH,_xrow_elt);
        if (!scratch)
            _xerror(_XE_MEM,"_xspmat_finput");
        scratch_len = _XMINSCRATCH;
    }
    
    for (i = 0; i < scratch_len; i++)
        scratch[i].nxt_row = scratch[i].nxt_idx = -1;
    
    if (isatty(_fileno(fp))){
        (void) fprintf(stderr,"SparseMatrix: ");
        do {
            (void) fprintf(stderr,"input rows cols: ");
            if (!fgets(_xline,_XMAXLINE,fp))
                _xerror(_XE_INPUT,"_xspmat_finput");
        } while(sscanf(_xline,"%u %u",&m,&n) != 2);
        A = _xspmat_get(m,n,5);
        rows = A->row;
        
        for (i = 0; i < m; i++){
            /* get a row... */
            (void) fprintf(stderr,"Row %d:\n",i);
            (void) fprintf(stderr,"Enter <col> <val> or 'e' to end row\n");
            curr_col = -1;
        
            len = 0;
            for (;;){  /* forever do... */
                /* if we need more scratch space, let's get it!
                   -- using amortized doubling */
                if (len >= scratch_len){
                    scratch = _xrenew(scratch,2*scratch_len,_xrow_elt);
                    if (!scratch)
                        _xerror(_XE_MEM,"_xspmat_finput");
                    scratch_len = 2*scratch_len;
                }
                do {  /* get an entry... */
                    (void) fprintf(stderr,"Entry %d: ",len);
                    if (!fgets(_xline,_XMAXLINE,fp))
                        _xerror(_XE_INPUT,"_xspmat_finput");
                    if (*_xline == 'e' || *_xline == 'E')
                        break;
#if _XREAL == _XDOUBLE
                } while (sscanf(_xline,"%u %lf",&col,&val) != 2 ||
#elif _XREAL == _XFLOAT
                } while (sscanf(_xline,"%u %f",&col,&val) != 2 ||
#endif
                col >= n || col <= curr_col );
        
                if (*_xline == 'e' || *_xline == 'E')
                    break;

                scratch[len].col = col;
                scratch[len].val = val;
                curr_col = col;
                
                len++;
            }

            /* Note: len = # elements in row */
            if (len > 5){
                if (_xmem_info_is_on()){
                    _xmem_bytes(_XTYPE_SPMAT,A->row[i].maxlen*sizeof(_xrow_elt),len*sizeof(_xrow_elt));  
                }
            
                rows[i].elt = (_xrow_elt *)_xrealloc((char *)rows[i].elt,len*sizeof(_xrow_elt));
                rows[i].maxlen = len;
            }
            _xmem_copy(scratch,rows[i].elt,len*sizeof(_xrow_elt));
            rows[i].len  = len;
            rows[i].diag = _xsprow_idx(&(rows[i]),i);
        }
    }
    else { /* not tty */
        ret_val = 0;
        _xskipjunk(fp);
        (void) fscanf(fp,"SparseMatrix:");
        _xskipjunk(fp);
        if ((ret_val=fscanf(fp,"%u by %u",&m,&n)) != 2)
            _xerror((ret_val == EOF) ? _XE_EOF : _XE_FORMAT,"_xspmat_finput");
        A = _xspmat_get(m,n,5);

        /* initialise start_row */
        for (i = 0; i < A->n; i++)
            A->start_row[i] = -1;
        
        rows = A->row;
        for (i = 0; i < m; i++){
            rows[i].diag = -1;
            _xskipjunk(fp);
            if ((ret_val=fscanf(fp,"row %d :",&tmp)) != 1 || tmp != i)
                _xerror((ret_val == EOF) ? _XE_EOF : _XE_FORMAT,"_xspmat_finput");
            curr_col = -1;
            len = 0;
            for (;;){ /* forever do... */
                /* if we need more scratch space, let's get it!
                   -- using amortized doubling */
                if (len >= scratch_len){
                    scratch = _xrenew(scratch,2*scratch_len,_xrow_elt);
                    if (!scratch)
                        _xerror(_XE_MEM,"_xspmat_finput");
                    scratch_len = 2*scratch_len;
                }
#if _XREAL == _XDOUBLE
                if ((ret_val=fscanf(fp,"%u : %lf",&col,&val)) != 2)
#elif _XREAL == _XFLOAT
                if ((ret_val=fscanf(fp,"%u : %f",&col,&val)) != 2)
#endif
                    break;
                if (col <= curr_col || col >= n)
                    _xerror(_XE_FORMAT,"_xspmat_finput");
                scratch[len].col = col;
                scratch[len].val = val;

                len++;
            }
            if (ret_val == EOF)
                _xerror(_XE_EOF,"_xspmat_finput");
            
            if (len > rows[i].maxlen){
                rows[i].elt = (_xrow_elt *)_xrealloc((char *)rows[i].elt,len*sizeof(_xrow_elt));
                rows[i].maxlen = len;
            }
            _xmem_copy(scratch,rows[i].elt,len*sizeof(_xrow_elt));
            rows[i].len = len;
            rows[i].diag = _xsprow_idx(&(rows[i]),i);
        }
    }

    return (A);
}

/*!
   print ALL relevant information about the sparse matrix A
 */
void _xspmat_dump(FILE *fp, const _xspmat *A){
    int i, j, j_idx;
    _xsprow *rows;
    _xrow_elt *elts;
    
    (void) fprintf(fp,"SparseMatrix dump:\n");
    if (!A){
        (void) fprintf(fp,"*** NULL ***\n");
        return;
    }
    (void) fprintf(fp,"Matrix at 0x%lx\n",(long)A);
    (void) fprintf(fp,"Dimensions: %d by %d\n",A->m,A->n);
    (void) fprintf(fp,"MaxDimensions: %d by %d\n",A->max_m,A->max_n);
    (void) fprintf(fp,"flag_col = %d, flag_diag = %d\n",A->flag_col,A->flag_diag);
    (void) fprintf(fp,"start_row @ 0x%lx:\n",(long)(A->start_row));
    for (j = 0; j < A->n; j++){
        (void) fprintf(fp,"%d ",A->start_row[j]);
        if (j % 10 == 9)
            (void) fprintf(fp,"\n");
    }
    (void) fprintf(fp,"\n");
    (void) fprintf(fp,"start_idx @ 0x%lx:\n",(long)(A->start_idx));
    for (j = 0; j < A->n; j++){
        (void) fprintf(fp,"%d ",A->start_idx[j]);
        if (j % 10 == 9)
            (void) fprintf(fp,"\n");
    }
    (void) fprintf(fp,"\n");
    (void) fprintf(fp,"Rows @ 0x%lx:\n",(long)(A->row));
    if (!A->row){
        (void) fprintf(fp,"*** NULL row ***\n");
        return;
    }
    rows = A->row;
    for (i = 0; i < A->m; i++){
        (void) fprintf(fp,"row %d: len = %d, maxlen = %d, diag idx = %d\n",
                       i,rows[i].len,rows[i].maxlen,rows[i].diag);
        (void) fprintf(fp,"element list @ 0x%lx\n",(long)(rows[i].elt));
        if (!rows[i].elt){
            (void) fprintf(fp,"*** NULL element list ***\n");
            continue;
        }
        elts = rows[i].elt;
        for (j_idx = 0; j_idx < rows[i].len; j_idx++, elts++)
            (void) fprintf(fp,"Col: %d, Val: %g, nxt_row = %d, nxt_idx = %d\n",
                           elts->col,elts->val,elts->nxt_row,elts->nxt_idx);
        (void) fprintf(fp,"\n");
    }
}

/*!
   output sparse matrix A to file/stream fp
 */
void _xspmat_foutput(FILE *fp, const _xspmat *A){
    int i, j_idx, m;
    _xsprow *rows;
    _xrow_elt *elts;
    
    (void) fprintf(fp,"SparseMatrix: ");
    if (!A){
        (void) fprintf(fp,"*** NULL ***\n");
        _xerror(_XE_NULL,"_xspmat_foutput");
        return;
    }
    (void) fprintf(fp,"%d by %d\n",A->m,A->n);
    m = A->m;
    if (!(rows=A->row)){
        (void) fprintf(fp,"*** NULL rows ***\n");
        _xerror(_XE_NULL,"_xspmat_foutput");
        return;
    }
    
    for (i = 0; i < m; i++){
        (void) fprintf(fp,"row %d: ",i);
        if (!(elts=rows[i].elt)){
            (void) fprintf(fp,"*** NULL element list ***\n");
            continue;
        }
        for (j_idx = 0; j_idx < rows[i].len; j_idx++){
            (void) fprintf(fp,"%d:%-20.15g ",elts[j_idx].col,elts[j_idx].val);
            if (j_idx % 3 == 2 && j_idx != rows[i].len-1)
              (void) fprintf(fp,"\n     ");
        }
        (void) fprintf(fp,"\n");
    }
    (void) fprintf(fp,"#\n");/* to stop looking beyond for next entry */
}

/*!
   print out sparse matrix **as a dense matrix**
 */
void _xspmat_foutput2(FILE *fp, const _xspmat *A){
    int cnt, i, j, j_idx;
    _xsprow *r;
    _xrow_elt *elt;
    
    if (!A){
        (void) fprintf(fp,"Matrix: *** NULL ***\n");
        return;
    }
    (void) fprintf(fp,"Matrix: %d by %d\n",A->m,A->n);
    for (i = 0; i < A->m; i++){
        (void) fprintf(fp,"row %d:",i);
        r = &(A->row[i]);
        elt = r->elt;
        cnt = j = j_idx = 0;
        while (j_idx < r->len || j < A->n){
            if (j_idx >= r->len)
                (void) fprintf(fp,"%14.9g ",0.0);
            else if (j < elt[j_idx].col)
                (void) fprintf(fp,"%14.9g ",0.0);
            else
                (void) fprintf(fp,"%14.9g ",elt[j_idx++].val);
            if (cnt++ % 4 == 3)
                (void) fprintf(fp,"\n");
            j++;
        }
        (void) fprintf(fp,"\n");
    }
}

/*  find the (partial) inner product of a pair of sparse rows
  -- uses a "merging" approach & assumes column ordered rows
  -- row indices for inner product are all < lim 
 */
static double _xsprow_ip(const _xsprow *row1, const _xsprow *row2, int lim){
    int idx1, idx2, len1, len2, tmp;
    register _xrow_elt *elts1, *elts2;
    register _xreal sum;
    
    elts1 = row1->elt; elts2 = row2->elt;
    len1 = row1->len;  len2 = row2->len;
    
    sum = 0.0;
    if ((len1 <= 0 || len2 <= 0) || (elts1->col >= lim || elts2->col >= lim))
        return (0.0);
    
    /* use sprow_idx() to speed up inner product where one row is
       much longer than the other */
    idx1 = idx2 = 0;
    if (len1 > 2*len2){
        idx1 = _xsprow_idx(row1,elts2->col);
        idx1 = (idx1 < 0) ? -(idx1+2) : idx1;
        if (idx1 < 0)
            _xerror(_XE_UNKNOWN,"_xsprow_ip");
        len1 -= idx1;
    }
    else if (len2 > 2*len1){
        idx2 = _xsprow_idx(row2,elts1->col);
        idx2 = (idx2 < 0) ? -(idx2+2) : idx2;
        if (idx2 < 0)
          _xerror(_XE_UNKNOWN,"_xsprow_ip");
        len2 -= idx2;
    }
    if (len1 <= 0 || len2 <= 0)
        return (0.0);
    
    elts1 = &(elts1[idx1]); elts2 = &(elts2[idx2]);
    for (;;){/* forever do... */
        if ((tmp=elts1->col-elts2->col) < 0){
            len1--; elts1++;
            if (!len1 || elts1->col >= lim)
              break;
        }
        else if (tmp > 0){
            len2--; elts2++;
            if (!len2 || elts2->col >= lim)
                break;
        }
        else {
            sum += elts1->val * elts2->val;
            len1--; elts1++;
            len2--; elts2++;
            if (!len1 || !len2 || elts1->col >= lim || elts2->col >= lim)
                break;
        }
    }
    
    return (sum);
}

/*
   return same as _xsprow_ip(row, row, lim)
 */
static double _xsprow_sqr(const _xsprow *row, int lim){
    register _xrow_elt *elts;
    int idx, len;
    register _xreal sum, tmp;
    
    sum = 0.0;
    elts = row->elt; len = row->len;
    for (idx = 0; idx < len; idx++, elts++){
        if (elts->col >= lim)
            break;
        tmp = elts->val;
        sum += tmp*tmp;
    }
    
    return (sum);
}

static int  _xscan_len = 0;
static int *_xscan_row = (int *)NULL, *_xscan_idx = (int *)NULL, *_xcol_list = (int *)NULL;

/*
    expand scan_row and scan_idx arrays
  -- return new length
 */
static int _xset_scan(int new_len){
    if (new_len <= _xscan_len)
        return _xscan_len;
    if (new_len <= _xscan_len+5)
        new_len += 5;

    /* update scan_len */
    _xscan_len = new_len;
    
    if (!_xscan_row || !_xscan_idx || !_xcol_list){
        _xscan_row = (int *)_xcalloc(new_len,sizeof(int));
        _xscan_idx = (int *)_xcalloc(new_len,sizeof(int));
        _xcol_list = (int *)_xcalloc(new_len,sizeof(int));
    }
    else {
        _xscan_row = (int *)_xrealloc((char *)_xscan_row,new_len*sizeof(int));
        _xscan_idx = (int *)_xrealloc((char *)_xscan_idx,new_len*sizeof(int));
        _xcol_list = (int *)_xrealloc((char *)_xcol_list,new_len*sizeof(int));
    }
    
    if (!_xscan_row || !_xscan_idx || !_xcol_list)
        _xerror(_XE_MEM,"_xset_scan");
    
    return (new_len);
}

/*!
    sparse Cholesky factorisation
  -- only the lower triangular part of A (incl. diagonal) is used
 */
_xspmat *_xspmat_chfactor(_xspmat *A){
    register int i;
    int idx, k, m, minim, n, num_scan, diag_idx, tmp1;
    _xreal pivot, tmp2;
    _xsprow *r_piv, *r_op;
    _xrow_elt *elt_piv, *elt_op, *old_elt;
    
    if (!A)
        _xerror(_XE_NULL,"_xspmat_chfactor");
    if (A->m != A->n)
        _xerror(_XE_SQUARE,"_xspmat_chfactor");
    
    /* set up access paths if not already done so */
    _xspmat_col_access(A);
    _xspmat_diag_access(A);
    
    m = A->m; n = A->n;
    for (k = 0; k < m; k++){
        r_piv = &(A->row[k]);
        if (r_piv->len > _xscan_len)
            _xset_scan(r_piv->len);
        elt_piv = r_piv->elt;
        diag_idx = _xsprow_idx2(r_piv,k,r_piv->diag);
        if (diag_idx < 0)
            _xerror(_XE_POSDEF,"_xspmat_chfactor");
        old_elt = &(elt_piv[diag_idx]);
        for (i = 0; i < r_piv->len; i++){
            if (elt_piv[i].col > k)
                break;
            _xcol_list[i] = elt_piv[i].col;
            _xscan_row[i] = elt_piv[i].nxt_row;
            _xscan_idx[i] = elt_piv[i].nxt_idx;
        }
        num_scan = i; /* number of actual entries in scan_row etc. */
        /* set diagonal entry of Cholesky factor */
        tmp2 = (_xreal)(elt_piv[diag_idx].val - _xsprow_sqr(r_piv,k));
        if (tmp2 <= 0.0)
            _xerror(_XE_POSDEF,"_xspmat_chfactor");
        elt_piv[diag_idx].val = pivot = (_xreal)sqrt(tmp2);
        
        /* now set the k-th column of the Cholesky factors */
        for (;;){ /* forever do... */
            /* find next row where something (non-trivial) happens
             i.e. find min(scan_row) */
            minim = n;
            for (i = 0; i < num_scan; i++){
                tmp1 = _xscan_row[i];
                minim = ( tmp1 >= 0 && tmp1 < minim ) ? tmp1 : minim;
            }
            if (minim >= n)
                break;  /* nothing more to do for this column */
            r_op = &(A->row[minim]);
            elt_op = r_op->elt;
        
            /* set next entry in column k of Cholesky factors */
            idx = _xsprow_idx2(r_op,k,_xscan_idx[num_scan-1]);
            if (idx < 0){ /* fill-in */
                _xspmat_set_val(A,minim,k,-_xsprow_ip(r_piv,r_op,k)/pivot);
                /* in case a realloc() has occurred... */
                elt_op = r_op->elt;
                /* now set up column access path again */
                idx = _xsprow_idx2(r_op,k,-(idx+2));
                tmp1 = old_elt->nxt_row;
                old_elt->nxt_row = minim;
                r_op->elt[idx].nxt_row = tmp1;
                tmp1 = old_elt->nxt_idx;
                old_elt->nxt_idx = idx;
                r_op->elt[idx].nxt_idx = tmp1;
            }
            else
                elt_op[idx].val = (_xreal)((elt_op[idx].val - _xsprow_ip(r_piv,r_op,k))/pivot);
        
            /* remember current element in column k for column chain */
            idx = _xsprow_idx2(r_op,k,idx);
            old_elt = &(r_op->elt[idx]);
        
            /* update scan_row */
            for (i = 0; i < num_scan; i++){
                if (_xscan_row[i] != minim)
                  continue;
                idx = _xsprow_idx2(r_op,_xcol_list[i],_xscan_idx[i]);
                if (idx < 0){
                    _xscan_row[i] = -1;
                    continue;
                }
                _xscan_row[i] = elt_op[idx].nxt_row;
                _xscan_idx[i] = elt_op[idx].nxt_idx;
            }
          
        }
    }
    
    return (A);
}

/*!
    solve L.L^T.out=b where L is a sparse matrix,
  -- out, b dense vectors
  -- returns out; operation may be in-situ
 */
_xvec *_xspmat_chsolve(_xspmat *L, const _xvec *b, _xvec *out){
    int i, j_idx, n, scan_idx, scan_row;
    _xsprow *row;
    _xrow_elt *elt;
    _xreal diag_val, sum, *out_ve;
    
    if (!L || !b)
        _xerror(_XE_NULL,"_xspmat_chsolve");
    if (L->m != L->n)
        _xerror(_XE_SQUARE,"_xspmat_chsolve");
    if (b->dim != L->m)
        _xerror(_XE_SIZES,"_xspmat_chsolve");
    
    if (!L->flag_col)
        _xspmat_col_access(L);
    if (!L->flag_diag)
        _xspmat_diag_access(L);
    
    out = _xvec_vcopy(b,out);
    out_ve = out->ve;
    
    /* forward substitution: solve L.x=b for x */
    n = L->n;
    for (i = 0; i < n; i++){
        sum = out_ve[i];
        row = &(L->row[i]);
        elt = row->elt;
        for (j_idx = 0; j_idx < row->len; j_idx++, elt++){
            if (elt->col >= i)
                break;
            sum -= elt->val*out_ve[elt->col];
        }
        if (row->diag >= 0)
            out_ve[i] = sum/(row->elt[row->diag].val);
        else
            _xerror(_XE_SING,"_xspmat_chsolve");
    }
    
    /* backward substitution: solve L^T.out = x for out */
    for (i = n-1; i >= 0; i--){
        sum = out_ve[i];
        row = &(L->row[i]);
        /* Note that row->diag >= 0 by above loop */
        elt = &(row->elt[row->diag]);
        diag_val = elt->val;
        
        /* scan down column */
        scan_idx = elt->nxt_idx;
        scan_row = elt->nxt_row;
        while (scan_row >= 0){
            row = &(L->row[scan_row]);
            elt = &(row->elt[scan_idx]);
            sum -= elt->val*out_ve[scan_row];
            scan_idx = elt->nxt_idx;
            scan_row = elt->nxt_row;
        }
        out_ve[i] = sum/diag_val;
    }
    
    return (out);
}

/*!
    sparse Incomplete Cholesky factorisation
  -- does a Cholesky factorisation assuming NO FILL-IN
  -- as for _xspmat_chfactor(), only the lower triangular part of A is used 
 */
_xspmat *_xspmat_ichfactor(_xspmat *A){
    int k, m, n, nxt_row, nxt_idx, diag_idx;
    _xreal pivot, tmp2;
    _xsprow *r_piv, *r_op;
    _xrow_elt *elt_piv, *elt_op;
    
    if (!A)
        _xerror(_XE_NULL,"_xspmat_ichfactor");
    if (A->m != A->n)
        _xerror(_XE_SQUARE,"_xspmat_ichfactor");
    
    /* set up access paths if not already done so */
    if (!A->flag_col)
        _xspmat_col_access(A);
    if (!A->flag_diag)
        _xspmat_diag_access(A);
    
    m = A->m; n = A->n;
    for (k = 0; k < m; k++){
        r_piv = &(A->row[k]);
        diag_idx = r_piv->diag;
        if (diag_idx < 0)
            _xerror(_XE_POSDEF,"_xspmat_ichfactor");
        
        elt_piv = r_piv->elt;
        
        /* set diagonal entry of Cholesky factor */
        tmp2 = (_xreal)(elt_piv[diag_idx].val - _xsprow_sqr(r_piv,k));
        if (tmp2 <= 0.0)
            _xerror(_XE_POSDEF,"_xspmat_ichfactor");
        elt_piv[diag_idx].val = pivot = (_xreal)sqrt(tmp2);
        
        /* find next row where something (non-trivial) happens */
        nxt_row = elt_piv[diag_idx].nxt_row;
        nxt_idx = elt_piv[diag_idx].nxt_idx;
        
        /* now set the k-th column of the Cholesky factors */
        while (nxt_row >= 0 && nxt_idx >= 0){
            /* nxt_row and nxt_idx give next next row (& index)
             of the entry to be modified */
            r_op = &(A->row[nxt_row]);
            elt_op = r_op->elt;
            elt_op[nxt_idx].val = (_xreal)((elt_op[nxt_idx].val - _xsprow_ip(r_piv,r_op,k))/pivot);
        
            nxt_row = elt_op[nxt_idx].nxt_row;
            nxt_idx = elt_op[nxt_idx].nxt_idx;
        }
    }
    
    return (A);
}

/*!
    symbolic sparse Cholesky factorisation
  -- does NOT do any floating point arithmetic; just sets up the structure
  -- only the lower triangular part of A (incl. diagonal) is used
 */
_xspmat *_xspmat_chsymb(_xspmat *A){
    register int i;
    int idx, k, m, minim, n, num_scan, diag_idx, tmp1;
    _xsprow *r_piv, *r_op;
    _xrow_elt *elt_piv, *elt_op, *old_elt;
    
    if (!A)
        _xerror(_XE_NULL,"_xspmat_chsymb");
    if (A->m != A->n)
        _xerror(_XE_SQUARE,"_xspmat_chsymb");
    
    /* set up access paths if not already done so */
    if (!A->flag_col)
        _xspmat_col_access(A);
    if (!A->flag_diag)
        _xspmat_diag_access(A);
    
    m = A->m; n = A->n;
    for (k = 0; k < m; k++){
        r_piv = &(A->row[k]);
        if (r_piv->len > _xscan_len)
            _xset_scan(r_piv->len);
        elt_piv = r_piv->elt;
        diag_idx = _xsprow_idx2(r_piv,k,r_piv->diag);
        if (diag_idx < 0)
            _xerror(_XE_POSDEF,"_xspmat_chsymb");
        old_elt = &(elt_piv[diag_idx]);
        for (i = 0; i < r_piv->len; i++){
            if (elt_piv[i].col > k)
              break;
            _xcol_list[i] = elt_piv[i].col;
            _xscan_row[i] = elt_piv[i].nxt_row;
            _xscan_idx[i] = elt_piv[i].nxt_idx;
        }
        
        num_scan = i; /* number of actual entries in scan_row etc. */
        
        /* now set the k-th column of the Cholesky factors */
        for (;;){ /* forever do... */
            /* find next row where something (non-trivial) happens
             i.e. find min(scan_row) */
            minim = n;
            for (i = 0; i < num_scan; i++){
                tmp1 = _xscan_row[i];
                minim = ( tmp1 >= 0 && tmp1 < minim ) ? tmp1 : minim;
            }
        
            if (minim >= n)
                break;  /* nothing more to do for this column */
            r_op = &(A->row[minim]);
            elt_op = r_op->elt;
        
            /* set next entry in column k of Cholesky factors */
            idx = _xsprow_idx2(r_op,k,_xscan_idx[num_scan-1]);
            if (idx < 0){ /* fill-in */
                _xspmat_set_val(A,minim,k,0.0);
                /* in case a realloc() has occurred... */
                elt_op = r_op->elt;
                /* now set up column access path again */
                idx = _xsprow_idx2(r_op,k,-(idx+2));
                tmp1 = old_elt->nxt_row;
                old_elt->nxt_row = minim;
                r_op->elt[idx].nxt_row = tmp1;
                tmp1 = old_elt->nxt_idx;
                old_elt->nxt_idx = idx;
                r_op->elt[idx].nxt_idx = tmp1;
            }
        
            /* remember current element in column k for column chain */
            idx = _xsprow_idx2(r_op,k,idx);
            old_elt = &(r_op->elt[idx]);
        
            /* update scan_row */
            for (i = 0; i < num_scan; i++){
                if (_xscan_row[i] != minim)
                    continue;
                idx = _xsprow_idx2(r_op,_xcol_list[i],_xscan_idx[i]);
                if (idx < 0){
                    _xscan_row[i] = -1;
                    continue;
                }
                _xscan_row[i] = elt_op[idx].nxt_row;
                _xscan_idx[i] = elt_op[idx].nxt_idx;
            } 
        }
    }
    
    return (A);
}

/*
  compute A.A^T where A is a given sparse matrix
 */
static _xspmat *_xcomp_aat(_xspmat *A){
    _xspmat *AAT;
    _xsprow *r, *r2;
    _xrow_elt *elts, *elts2;
    int i, idx, idx2, j, m, minim, n, num_scan, tmp1;
    _xreal ip;
    
    if (!A)
        _xerror(_XE_NULL,"_xcomp_aat");
    m = A->m; n = A->n;
    
    /* set up column access paths */
    if (!A->flag_col)
        _xspmat_col_access(A);
    
    AAT = _xspmat_get(m,m,10);
    for (i = 0; i < m; i++){
        /* initialisation */
        r = &(A->row[i]);
        elts = r->elt;
        
        /* set up scan lists for this row */
        if (r->len > _xscan_len)
            _xset_scan(r->len);
        for (j = 0; j < r->len; j++){
            _xcol_list[j] = elts[j].col;
            _xscan_row[j] = elts[j].nxt_row;
            _xscan_idx[j] = elts[j].nxt_idx;
        }
        num_scan = r->len;
        
        /* scan down the rows for next non-zero not
           associated with a diagonal entry */
        for (;;){
            minim = m;
            for (idx = 0; idx < num_scan; idx++){
                tmp1 = _xscan_row[idx];
                minim = ( tmp1 >= 0 && tmp1 < minim ) ? tmp1 : minim;
            }
            if (minim >= m)
                break;
            r2 = &(A->row[minim]);
            if (minim > i){
                ip = (_xreal)_xsprow_ip(r,r2,n);
                _xspmat_set_val(AAT,minim,i,ip);
                _xspmat_set_val(AAT,i,minim,ip);
            }
            /* update scan entries */
            elts2 = r2->elt;
            for (idx = 0; idx < num_scan; idx++){
                if (_xscan_row[idx] != minim || _xscan_idx[idx] < 0)
                    continue;
                idx2 = _xscan_idx[idx];
                _xscan_row[idx] = elts2[idx2].nxt_row;
                _xscan_idx[idx] = elts2[idx2].nxt_idx;
            }
        }
        
        /* set the diagonal entry */
        _xspmat_set_val(AAT,i,i,_xsprow_sqr(r,n));
    }
    
    return (AAT);
}

/*!
    sparse LU factorisation with pivoting
  -- uses partial pivoting and Markowitz criterion
      |a[p][k]| >= alpha * max_i |a[i][k]|
  -- creates fill-in as needed
  -- in situ factorisation
 */
_xspmat *_xspmat_lufactor(_xspmat *A, _xperm *px, double alpha){
    int i, best_i, k, idx, len, best_len, m, n;
    _xreal max_val, tmp;
    _xsprow *r, *r_piv, tmp_row;
    _XSTATIC _xsprow *merge = (_xsprow *)NULL;
    _XSTATIC _xvec *col_vals= (_xvec *)NULL;
    
    if (!A || !px)
        _xerror(_XE_NULL,"_xspmat_lufactor");
    if (alpha <= 0.0 || alpha > 1.0)
        _xerror(_XE_RANGE,"alpha in _xspmat_lufactor");
    if ((int)(px->size) <= A->m)
        px = _xperm_resize(px,A->m);
    _xperm_ident(px);
    col_vals = _xvec_resize(col_vals,A->m);
    _XMEM_STAT_REG(col_vals,_XTYPE_VEC);
    
    m = A->m; n = A->n;
    if (!A->flag_col)
        _xspmat_col_access(A);
    if (!A->flag_diag)
        _xspmat_diag_access(A);
    A->flag_col = A->flag_diag = _XFALSE;
    if (!merge){
        merge = _xsprow_get(20);
        _XMEM_STAT_REG(merge,_XTYPE_SPROW);
    }
    
    for (k = 0; k < n; k++){
        /* find pivot row/element for partial pivoting */
        /* get first row with a non-zero entry in the k-th column */
        max_val = 0.0;
        for (i = k; i < m; i++){
            r = &(A->row[i]);
            idx = _xsprow_idx(r,k);
            if (idx < 0)
                tmp = 0.0;
            else
                tmp = r->elt[idx].val;
            if (fabs(tmp) > max_val)
                max_val = (_xreal)fabs(tmp);
            col_vals->ve[i] = tmp;
        }
    
        if (max_val == 0.0)
            continue;
    
        best_len = n+1; /* only if no possibilities */
        best_i = -1;
        for (i = k; i < m; i++){
            tmp = (_xreal)fabs(col_vals->ve[i]);
            if (tmp == 0.0)
                continue;
            if (tmp >= alpha*max_val){
                r = &(A->row[i]);
                idx = _xsprow_idx(r,k);
                len = (r->len) - idx;
                if (len < best_len){
                    best_len = len;
                    best_i = i;
                }
            }
        }
    
        /* swap row #best_i with row #k */
        _xmem_copy(&(A->row[best_i]),&tmp_row,sizeof(_xsprow));
        _xmem_copy(&(A->row[k]),&(A->row[best_i]),sizeof(_xsprow));
        _xmem_copy(&tmp_row,&(A->row[k]),sizeof(_xsprow));
        /* swap col_vals entries */
        tmp = col_vals->ve[best_i];
        col_vals->ve[best_i] = col_vals->ve[k];
        col_vals->ve[k] = tmp;
        _xperm_transp(px,k,best_i);
    
        r_piv = &(A->row[k]);
        for (i = k+1; i < n; i++){
            /* compute and set multiplier */
            tmp = col_vals->ve[i]/col_vals->ve[k];
            if (tmp != 0.0)
                _xspmat_set_val(A,i,k,tmp);
            else
                continue;
            
            /* perform row operations */
            merge->len = 0;
            r = &(A->row[i]);
            _xsprow_mltadd(r,r_piv,-tmp,k+1,merge,_XTYPE_SPROW);
            idx = _xsprow_idx(r,k+1);
            if (idx < 0)
                idx = -(idx+2);
            /* see if r needs expanding */
            if (r->maxlen < idx + merge->len)
                _xsprow_xpd(r,idx+merge->len,_XTYPE_SPMAT);
            r->len = idx+merge->len;
            _xmem_copy((char *)(merge->elt),(char *)&(r->elt[idx]),
                       merge->len*sizeof(_xrow_elt));
        }
    }
#ifdef _XTHREAD_SAFE
    _xsprow_free(merge);_xvec_vfree(col_vals);
#endif

    return (A);
}

/*!
    solve A.x = b using factored matrix A from _xspmat_lufactor()
  -- returns x
  -- may not be in-situ
 */
_xvec *_xspmat_lusolve(const _xspmat *A, _xperm *pivot, const _xvec *b, _xvec *x){
    int i, idx, len, lim;
    _xreal sum, *x_ve;
    _xsprow *r;
    _xrow_elt *elt;
    
    if (!A || !b)
        _xerror(_XE_NULL,"_xspmat_lusolve");
    if ((pivot && A->m != pivot->size) || A->m != b->dim)
        _xerror(_XE_SIZES,"_xspmat_lusolve");
    if (!x || x->dim != A->n)
        x = _xvec_resize(x,A->n);
    
    if (pivot)
        x = _xperm_vec(pivot,b,x);
    else
        x = _xvec_vcopy(b,x);
    
    x_ve = x->ve;
    lim = _xmin(A->m,A->n);
    for (i = 0; i < lim; i++){
        sum = x_ve[i];
        r = &(A->row[i]);
        len = r->len;
        elt = r->elt;
        for ( idx = 0; idx < len && elt->col < i; idx++, elt++ )
        sum -= elt->val*x_ve[elt->col];
        x_ve[i] = sum;
    }
    
    for (i = lim-1; i >= 0; i--){
        sum = x_ve[i];
        r = &(A->row[i]);
        len = r->len;
        elt = &(r->elt[len-1]);
        for (idx = len-1; idx >= 0 && elt->col > i; idx--, elt--)
            sum -= elt->val*x_ve[elt->col];
        if (idx < 0 || elt->col != i || elt->val == 0.0)
            _xerror(_XE_SING,"_xspmat_lusolve");
        x_ve[i] = sum/elt->val;
    }
    
    return (x);
}

/*!
    solve A.x = b using factored matrix A from _xspmat_lufactor()
  -- returns x
  -- may not be in-situ
 */
_xvec *_xspmat_lutsolve(_xspmat *A, _xperm *pivot, const _xvec *b, _xvec *x){
    int i, idx, lim, rownum;
    _xreal sum, *tmp_ve;
    _xrow_elt *elt;
    _XSTATIC _xvec *tmp=(_xvec *)NULL;
    
    if (!A || !b)
        _xerror(_XE_NULL,"_xspmat_lutsolve");
    if ((pivot && A->m != pivot->size) || A->m != b->dim)
        _xerror(_XE_SIZES,"_xspmat_lutsolve");
    tmp = _xvec_vcopy(b,tmp);
    _XMEM_STAT_REG(tmp,_XTYPE_VEC);
    
    if (!A->flag_col)
        _xspmat_col_access(A);
    if (!A->flag_diag)
        _xspmat_diag_access(A);
    
    lim = _xmin(A->m,A->n);
    tmp_ve = tmp->ve;
    /* solve U^T.tmp = b */
    for (i = 0; i < lim; i++){
        sum = tmp_ve[i];
        rownum = A->start_row[i];
        idx    = A->start_idx[i];
        if (rownum < 0 || idx < 0)
            _xerror(_XE_SING,"_xspmat_lutsolve");
        while (rownum < i && rownum >= 0 && idx >= 0){
            elt = &(A->row[rownum].elt[idx]);
            sum -= elt->val*tmp_ve[rownum];
            rownum = elt->nxt_row;
            idx    = elt->nxt_idx;
        }
        if (rownum != i)
            _xerror(_XE_SING,"_xspmat_lutsolve");
        elt = &(A->row[rownum].elt[idx]);
        if (elt->val == 0.0)
            _xerror(_XE_SING,"_xspmat_lutsolve");
        tmp_ve[i] = sum/elt->val;
    }
    
    /* now solve L^T.tmp = (old) tmp */
    for (i = lim-1; i >= 0; i--){
        sum = tmp_ve[i];
        rownum = i;
        idx    = A->row[rownum].diag;
        if (idx < 0)
            _xerror(_XE_NULL,"_xspmat_lutsolve");
        elt = &(A->row[rownum].elt[idx]);
        rownum = elt->nxt_row;
        idx    = elt->nxt_idx;
        while (rownum < lim && rownum >= 0 && idx >= 0){
            elt = &(A->row[rownum].elt[idx]);
            sum -= elt->val*tmp_ve[rownum];
            rownum = elt->nxt_row;
            idx    = elt->nxt_idx;
        }
        tmp_ve[i] = sum;
    }
    
    if (pivot)
        x = _xperm_inv_vec(pivot,tmp,x);
    else
        x = _xvec_vcopy(tmp,x);
    
#ifdef _XTHREAD_SAFE
    _xvec_vfree(tmp);
#endif

    return (x);
}

/*!
    sparse modified incomplete LU factorisation with no pivoting
  -- all pivot entries are ensured to be >= alpha in magnitude
  -- setting alpha = 0 gives incomplete LU factorisation
  -- no fill-in is generated
  -- in situ factorisation
 */
_xspmat *_xspmat_ilufactor(_xspmat *A, double alpha){
    int i, k, idx, idx_piv, m, n, old_idx, old_idx_piv;
    _xsprow *r, *r_piv;
    _xreal piv_val, tmp;
    
    if (!A)
        _xerror(_XE_NULL,"_xspmat_ilufactor");
    if (alpha < 0.0)
        _xerror(_XE_RANGE,"alpha in _xspmat_ilufactor");
    
    m = A->m; n = A->n;
    _xspmat_diag_access(A);
    _xspmat_col_access(A);
    
    for (k = 0; k < n; k++){
        r_piv = &(A->row[k]);
        idx_piv = r_piv->diag;
        if (idx_piv < 0){
            _xsprow_set_val(r_piv,k,alpha);
            idx_piv = _xsprow_idx(r_piv,k);
        }
        if (idx_piv < 0)
            _xerror(_XE_BOUNDS,"_xspmat_ilufactor");
        old_idx_piv = idx_piv;
        piv_val = r_piv->elt[idx_piv].val;
        if (fabs(piv_val) < alpha)
            piv_val = (_xreal)(( piv_val < 0.0 ) ? -alpha : alpha);
        if (piv_val == 0.0) /* alpha == 0.0 too! */
            _xerror(_XE_SING,"_xspmat_ilufactor");
        
        /* go to next row with a non-zero in this column */
        i = r_piv->elt[idx_piv].nxt_row;
        old_idx = idx = r_piv->elt[idx_piv].nxt_idx;
        while (i >= k){
            /* perform row operations */
            r = &(A->row[i]);
            if (idx < 0){
                idx = r->elt[old_idx].nxt_idx;
                i = r->elt[old_idx].nxt_row;
                continue;
            }
            /* compute and set multiplier */
            r->elt[idx].val = tmp = r->elt[idx].val/piv_val;
            if (tmp == 0.0){
                idx = r->elt[old_idx].nxt_idx;
                i = r->elt[old_idx].nxt_row;
                continue;
            }
            idx_piv++;
            idx++;  /* now look beyond the multiplier entry */
            while (idx_piv < r_piv->len && idx < r->len){
                if (r_piv->elt[idx_piv].col < r->elt[idx].col)
                    idx_piv++;
                else if (r_piv->elt[idx_piv].col > r->elt[idx].col)
                    idx++;
                else { /* column numbers match */
                    r->elt[idx].val -= tmp*r_piv->elt[idx_piv].val;
                    idx++;
                    idx_piv++;
                }
            }
        
            /* bump to next row with a non-zero in column k */
            i = r->elt[old_idx].nxt_row;
            old_idx = idx = r->elt[old_idx].nxt_idx;
            /* and restore idx_piv to index of pivot entry */
            idx_piv = old_idx_piv;
        }
    }
    return (A);
}

/*
   return index (encoded if entry not allocated)
  of the element of row r with column j
  -- uses linear search
 */
static int _xunord_get_idx(_xsprow *r, int j){
    int idx;
    _xrow_elt *e;

    if (!r || !r->elt)
        _xerror(_XE_NULL,"_xunord_get_idx");
    for (idx = 0, e = r->elt; idx < r->len; idx++, e++)
        if (e->col == j)
            break;

    return (idx >= r->len) ? -(r->len+2) : idx;
}

/* 
   return value of the (i,j) entry of A
  -- same assumptions as _xunord_get_idx()
 */
static double _xunord_get_val(_xspmat *A, int i, int j){
    _xsprow *r;
    int idx;

    if (!A)
        _xerror(_XE_NULL,"_xunord_get_val");
    if (i < 0 || i >= A->m || j < 0 || j >= A->n)
        _xerror(_XE_BOUNDS,"_xunord_get_val");

    r = &(A->row[i]);
    idx = _xunord_get_idx(r,j);

    return (idx < 0) ? 0.0 : r->elt[idx].val;
}

/*
   bumps row and idx to next entry in column j
 */
static _xrow_elt *_xbkp_bump_col(_xspmat *A, int j, int *row, int *idx){
    _xsprow *r;
    _xrow_elt *e;

    if (*row < 0){
        *row = A->start_row[j];
        *idx = A->start_idx[j];
    }
    else {
        r = &(A->row[*row]);
        e = &(r->elt[*idx]);
        if (e->col != j)
            _xerror(_XE_INTERN,"_xbkp_bump_col");
        *row = e->nxt_row;
        *idx = e->nxt_idx;
    }

    return (*row < 0) ? (_xrow_elt *)NULL : &(A->row[*row].elt[*idx]);
}

/*
   swap the (i,j) with the (k,l) entry of sparse matrix
   -- either or both of the entries may be unallocated
 */
static _xspmat *_xbkp_swap_elt(_xspmat *A, int i1, int j1, int idx1, int i2, int j2, int idx2){
    int tmp_row, tmp_idx;
    _xsprow *r1, *r2;
    _xrow_elt *e1, *e2;
    _xreal tmp;

    if (!A)
        _xerror(_XE_NULL,"_xbkp_swap_elt");

    if (i1 < 0 || j1 < 0 || i2 < 0 || j2 < 0 ||
        i1 >= A->m || j1 >= A->n || i2 >= A->m || j2 >= A->n){
        _xerror(_XE_BOUNDS,"_xbkp_swap_elt");
    }

    if (i1 == i2 && j1 == j2)
        return (A);
    if (idx1 < 0 && idx2 < 0) /* neither allocated */
        return (A);

    r1 = &(A->row[i1]); r2 = &(A->row[i2]);
    if (idx1 < 0){ /* assume not allocated */
        idx1 = r1->len;
        if (idx1 >= r1->maxlen){
            _xtracecatch(_xsprow_xpd(r1,2*r1->maxlen+1,_XTYPE_SPMAT),"_xbkp_swap_elt"); 
        }
        r1->len = idx1+1;
        r1->elt[idx1].col = j1;
        r1->elt[idx1].val = 0.0;
        /* now patch up column access path */
        tmp_row = -1; tmp_idx = j1;
        _xspmat_chase_col(A,j1,&tmp_row,&tmp_idx,i1-1);
        
        if (tmp_row < 0){
            r1->elt[idx1].nxt_row = A->start_row[j1];
            r1->elt[idx1].nxt_idx = A->start_idx[j1];
            A->start_row[j1] = i1;
            A->start_idx[j1] = idx1;
        }
        else {
            _xrow_elt *tmp_e;
            tmp_e = &(A->row[tmp_row].elt[tmp_idx]);
            r1->elt[idx1].nxt_row = tmp_e->nxt_row;
            r1->elt[idx1].nxt_idx = tmp_e->nxt_idx;
            tmp_e->nxt_row = i1;
            tmp_e->nxt_idx = idx1;
        }
    }
    else if (r1->elt[idx1].col != j1)
        _xerror(_XE_INTERN,"_xbkp_swap_elt");
    if (idx2 < 0){
        idx2 = r2->len;
        if (idx2 >= r2->maxlen){
            _xtracecatch(_xsprow_xpd(r2,2*r2->maxlen+1,_XTYPE_SPMAT),"_xbkp_swap_elt");
        }

        r2->len = idx2+1;
        r2->elt[idx2].col = j2;
        r2->elt[idx2].val = 0.0;
        /* now patch up column access path */
        tmp_row = -1; tmp_idx = j2;
        _xspmat_chase_col(A,j2,&tmp_row,&tmp_idx,i2-1);
        if (tmp_row < 0){
            r2->elt[idx2].nxt_row = A->start_row[j2];
            r2->elt[idx2].nxt_idx = A->start_idx[j2];
            A->start_row[j2] = i2;
            A->start_idx[j2] = idx2;
        }
        else {
            _xrow_elt *tmp_e;
            tmp_e = &(A->row[tmp_row].elt[tmp_idx]);
            r2->elt[idx2].nxt_row = tmp_e->nxt_row;
            r2->elt[idx2].nxt_idx = tmp_e->nxt_idx;
            tmp_e->nxt_row = i2;
            tmp_e->nxt_idx = idx2;
        }
    }
    else if (r2->elt[idx2].col != j2)
        _xerror(_XE_INTERN,"_xbkp_swap_elt");

    e1 = &(r1->elt[idx1]); e2 = &(r2->elt[idx2]);

    tmp = e1->val;
    e1->val = e2->val;
    e2->val = tmp;

    return (A);
}

/*
   swap rows/cols i and j (symmetric pivot)
  -- uses just the upper triangular part
 */
static _xspmat *_xbkp_interchange(_xspmat *A, int i1, int i2){
    int tmp_row, tmp_idx;
    int row1, row2, idx1, idx2, tmp_row1, tmp_idx1, tmp_row2, tmp_idx2;
    _xsprow *r1, *r2;
    _xrow_elt *e1, *e2;
    _xivec *done_list = (_xivec *)NULL;

    if (!A)
        _xerror(_XE_NULL,"_xbkp_interchange");
    if (i1 < 0 || i1 >= A->n || i2 < 0 || i2 >= A->n)
        _xerror(_XE_BOUNDS,"_xbkp_interchange");
    if (A->m != A->n)
        _xerror(_XE_SQUARE,"_xbkp_interchange");

    if (i1 == i2)
        return (A);
    if (i1 > i2){
        tmp_idx = i1;
        i1 = i2;
        i2 = tmp_idx;
    }

    done_list = _xivec_resize(done_list,A->n);
    for (tmp_idx = 0; tmp_idx < A->n; tmp_idx++)
        done_list->ive[tmp_idx] = _XFALSE;
    row1 = -1; idx1 = i1;
    row2 = -1; idx2 = i2;
    e1 = _xbkp_bump_col(A,i1,&row1,&idx1);
    e2 = _xbkp_bump_col(A,i2,&row2,&idx2);

    while ((row1 >= 0 && row1 < i1) || (row2 >= 0 && row2 < i1)){
        /* Note: "row2 < i1" not "row2 < i2" as we must stop before the
           "knee bend" */
        if (row1 >= 0 && row1 < i1 && ( row1 < row2 || row2 < 0)){
            tmp_row1 = row1; tmp_idx1 = idx1;
            e1 = _xbkp_bump_col(A,i1,&tmp_row1,&tmp_idx1);
            if (!done_list->ive[row1]){
                if (row1 == row2)
                    _xbkp_swap_elt(A,row1,i1,idx1,row1,i2,idx2);
                else
                    _xbkp_swap_elt(A,row1,i1,idx1,row1,i2,-1);
                done_list->ive[row1] = _XTRUE;
            }
            row1 = tmp_row1; idx1 = tmp_idx1;
        }
        else if (row2 >= 0 && row2 < i1 && ( row2 < row1 || row1 < 0)){
            tmp_row2 = row2; tmp_idx2 = idx2;
            e2 = _xbkp_bump_col(A,i2,&tmp_row2,&tmp_idx2);
            if (!done_list->ive[row2]){
                if (row1 == row2)
                    _xbkp_swap_elt(A,row2,i1,idx1,row2,i2,idx2);
                else
                    _xbkp_swap_elt(A,row2,i1,-1,row2,i2,idx2);
                done_list->ive[row2] = _XTRUE;
            }
            row2 = tmp_row2; idx2 = tmp_idx2;
        }
        else if (row1 == row2){
            tmp_row1 = row1; tmp_idx1 = idx1;
            e1 = _xbkp_bump_col(A,i1,&tmp_row1,&tmp_idx1);
            tmp_row2 = row2; tmp_idx2 = idx2;
            e2 = _xbkp_bump_col(A,i2,&tmp_row2,&tmp_idx2);
            if (!done_list->ive[row1]){
                _xbkp_swap_elt(A,row1,i1,idx1,row2,i2,idx2);
                done_list->ive[row1] = _XTRUE;
            }
            row1 = tmp_row1; idx1 = tmp_idx1;
            row2 = tmp_row2; idx2 = tmp_idx2;
        }
    }

    /* ensure we are **past** the first knee */
    while (row2 >= 0 && row2 <= i1) e2 = _xbkp_bump_col(A,i2,&row2,&idx2);

    /* at/after 1st "knee bend" */
    r1 = &(A->row[i1]);
    idx1 = 0;
    e1 = &(r1->elt[idx1]);
    while (row2 >= 0 && row2 < i2){
        /* used for update of e2 at end of loop */
        tmp_row = row2; tmp_idx = idx2;
        if (!done_list->ive[row2]){
            r2 = &(A->row[row2]);
            _xbkp_bump_col(A,i2,&tmp_row,&tmp_idx);
            done_list->ive[row2] = _XTRUE;
            tmp_idx1 = _xunord_get_idx(r1,row2);
            _xtracecatch(_xbkp_swap_elt(A,row2,i2,idx2,i1,row2,tmp_idx1),"_xbkp_interchange");
        }

        /* update e1 and e2 */
        row2 = tmp_row; idx2 = tmp_idx;
        e2 = ( row2 >= 0 ) ? &(A->row[row2].elt[idx2]) : (_xrow_elt *)NULL;
    }

    idx1 = 0;
    e1 = r1->elt;
    while (idx1 < r1->len){
        if (e1->col >= i2 || e1->col <= i1){
            idx1++;
            e1++;
            continue;
        }
        if (!done_list->ive[e1->col]){
            tmp_idx2 = _xunord_get_idx(&(A->row[e1->col]),i2);
            _xtracecatch(_xbkp_swap_elt(A,i1,e1->col,idx1,e1->col,i2,tmp_idx2),"_xbkp_interchange");
            done_list->ive[e1->col] = _XTRUE;
        }
        idx1++;
        e1++;
    }

    /* at/after 2nd "knee bend" */
    idx1 = 0;
    e1 = &(r1->elt[idx1]);
    r2 = &(A->row[i2]);
    idx2 = 0;
    e2 = &(r2->elt[idx2]);
    while (idx1 < r1->len){
        if (e1->col <= i2){
            idx1++; 
            e1++;
            continue;
        }
        if (!done_list->ive[e1->col]){
            tmp_idx2 = _xunord_get_idx(r2,e1->col);
            _xtracecatch(_xbkp_swap_elt(A,i1,e1->col,idx1,i2,e1->col,tmp_idx2),"_xbkp_interchange");
            done_list->ive[e1->col] = _XTRUE;
        }
        idx1++;
        e1++;
    }

    idx2 = 0; e2 = r2->elt;
    while (idx2 < r2->len){
        if (e2->col <= i2){
            idx2++;
            e2++;
            continue;
        }
        if (!done_list->ive[e2->col]){
            tmp_idx1 = _xunord_get_idx(r1,e2->col);
            _xtracecatch(_xbkp_swap_elt(A,i2,e2->col,idx2,i1,e2->col,tmp_idx1),"_xbkp_interchange");
            done_list->ive[e2->col] = _XTRUE;
        }
        idx2++;
        e2++;
    }

    /* now interchange the digonal entries! */
    idx1 = _xunord_get_idx(&(A->row[i1]),i1);
    idx2 = _xunord_get_idx(&(A->row[i2]),i2);
    if (idx1 >= 0 || idx2 >= 0){
        _xtracecatch(_xbkp_swap_elt(A,i1,i1,idx1,i2,i2,idx2),"_xbkp_interchange");
    }

    return (A);
}

/*!
   chase column access path in column col, starting with
   row_num and idx, to find last row # in this column <= max_row
   -- row_num is returned; idx is also set by this routine
   -- assumes that the column access paths (possibly without the
   nxt_idx fields) are set up
 */
_xrow_elt *_xspmat_chase_col(const _xspmat *A, int col, int *row_num, int *idx, int max_row){
    int old_idx, old_row, tmp_idx, tmp_row;
    _xsprow *r;
    _xrow_elt *e;
    
    if (col < 0 || col >= A->n)
        _xerror(_XE_BOUNDS,"_xspmat_chase_col");
    tmp_row = *row_num;
    if (tmp_row < 0){
        if (A->start_row[col] > max_row){
            tmp_row = -1;
            tmp_idx = col;
            return (_xrow_elt *)NULL;
        }
        else {
            tmp_row = A->start_row[col];
            tmp_idx = A->start_idx[col];
        }
    }
    else
        tmp_idx = *idx;
    
    old_row = tmp_row;
    old_idx = tmp_idx;
    while (tmp_row >= 0 && tmp_row < max_row){
        r = &(A->row[tmp_row]);
        if (tmp_idx < 0 || tmp_idx >= r->len || r->elt[tmp_idx].col != col){
#ifdef _XDEBUG
            (void) fprintf(stdout, "_xspmat_chase_col:error: col = %d, row # = %d, idx = %d\n",
                           col, tmp_row, tmp_idx);
            (void) fprintf(stdout, "_xspmat_chase_col:error: old_row = %d, old_idx = %d\n",
                           old_row, old_idx);
            (void) fprintf(stdout, "_xspmat_chase_col:error: A =\n");
            _xspmat_dump(stdout,A);
#endif
            _xerror(_XE_INTERN,"_xspmat_chase_col");
        }
        e = &(r->elt[tmp_idx]);
        old_row = tmp_row;
        old_idx = tmp_idx;
        tmp_row = e->nxt_row;
        tmp_idx = e->nxt_idx;
    }
    if (old_row > max_row){
        old_row = -1;
        old_idx = col;
        e = (_xrow_elt *)NULL;
    }
    else if (tmp_row <= max_row && tmp_row >= 0){
        old_row = tmp_row;
        old_idx = tmp_idx;
    }

    *row_num = old_row;
    *idx = (old_row >= 0) ? old_idx : col;

    return (e);
}

/*!
   as for _xspmat_chase_col except that we want the first
  row whose row # >= min_row; -1 indicates no such row
 */
_xrow_elt *_xspmat_chase_past(const _xspmat *A, int col, int *row_num, int *idx, int min_row){
    _xsprow *r;
    _xrow_elt *e;
    int tmp_idx, tmp_row;

    tmp_row = *row_num;
    tmp_idx = *idx;
    _xspmat_chase_col(A,col,&tmp_row,&tmp_idx,min_row);
    if (tmp_row < 0){ /* use A->start_row[..] etc. */
        if (A->start_row[col] < 0)
            tmp_row = -1;
        else {
            tmp_row = A->start_row[col];
            tmp_idx = A->start_idx[col];
        }
    }
    else if (tmp_row < min_row){
        r = &(A->row[tmp_row]);
        if (tmp_idx < 0 || tmp_idx >= r->len || r->elt[tmp_idx].col != col)
            _xerror(_XE_INTERN,"_xspmat_chase_past");
        tmp_row = r->elt[tmp_idx].nxt_row;
        tmp_idx = r->elt[tmp_idx].nxt_idx;
    }

    *row_num = tmp_row;
    *idx = tmp_idx;
    if (tmp_row < 0)
        e = (_xrow_elt *)NULL;
    else {
        if (tmp_idx < 0 || tmp_idx >= A->row[tmp_row].len ||
            A->row[tmp_row].elt[tmp_idx].col != col )
            _xerror(_XE_INTERN,"_xspmat_chase_past");
        e = &(A->row[tmp_row].elt[tmp_idx]);
    }

    return (e);
}

/*!
   return minimum of an integer vector
   -- set index to the position in iv if index != NULL
 */
int _xivec_min(_xivec *iv, int *index){
    int i, i_min, min_val, tmp;
    
    if (!iv) 
        _xerror(_XE_NULL,"_xivec_min");
    if (iv->dim <= 0)
        _xerror(_XE_SIZES,"_xivec_min");
    i_min = 0;
    min_val = iv->ive[0];
    for (i = 1; (unsigned int)i < iv->dim; i++){
        tmp = iv->ive[i];
        if (tmp < min_val){
            min_val = tmp;
            i_min = i;
        }
    }
    
    if (index != (int *)NULL )
        *index = i_min;
    
    return (min_val);
}

/*!
   return maximum of an integer vector
   -- set index to the position in iv if index != NULL
 */
int _xivec_max(_xivec *iv, int *index){
    int i, i_max, max_val, tmp;
    
    if (!iv) 
        _xerror(_XE_NULL,"_xivec_max");
    if (iv->dim <= 0)
        _xerror(_XE_SIZES,"_xivec_max");
    i_max = 0;
    max_val = iv->ive[0];
    for (i = 1; (unsigned int)i < iv->dim; i++){
        tmp = iv->ive[i];
        if (tmp > max_val){
            max_val = tmp;
            i_max = i;
        }
    }
    
    if (index != (int *)NULL )
        *index = i_max;
    
    return (max_val);
}

/* return max { |A[j][k]| : k >= i, k != j, k != l } given j
   using symmetry and only the upper triangular part of A 
 */
static double _xmax_row_col(_xspmat *A, int i,int j, int l){
    int row_num, idx;
    _xsprow *r;
    _xrow_elt *e;
    _xreal max_val, tmp;

    if (!A)
        _xerror(_XE_NULL,"_xmax_row_col");
    if (i < 0 || i > A->n || j < 0 || j >= A->n)
        _xerror(_XE_BOUNDS,"_xmax_row_col");

    max_val = 0.0;
    idx = _xunord_get_idx(&(A->row[i]),j);
    if (idx < 0){
        row_num = -1;
        idx = j;
        e = _xspmat_chase_past(A,j,&row_num,&idx,i);
    }
    else {
        row_num = i;
        e = &(A->row[i].elt[idx]);
    }
    while (row_num >= 0 && row_num < j){
        if (row_num != l){
            tmp = (_xreal)fabs(e->val);
            if (tmp > max_val)
                max_val = tmp;
        }
        e = _xspmat_bump_col(A,j,&row_num,&idx);
    }
    r = &(A->row[j]);
    for (idx = 0, e = r->elt; idx < r->len; idx++, e++){
        if (e->col > j && e->col != l){
            tmp = (_xreal)fabs(e->val);
            if (tmp > max_val)
                max_val = tmp;
        }
    }

    return (max_val);
}

/*!
    move along to next nonzero entry in column col after row_num
  -- update row_num and idx 
 */
_xrow_elt *_xspmat_bump_col(const _xspmat *A, int col, int *row_num, int *idx){
    _xsprow *r;
    _xrow_elt *e;
    int tmp_row, tmp_idx;

    tmp_row = *row_num;
    tmp_idx = *idx;
    if (tmp_row < 0){
        tmp_row = A->start_row[col];
        tmp_idx = A->start_idx[col];
    }
    else {
        r = &(A->row[tmp_row]);
        if (tmp_idx < 0 || tmp_idx >= r->len || r->elt[tmp_idx].col != col)
            _xerror(_XE_INTERN,"_xspmat_bump_col");
        e = &(r->elt[tmp_idx]);
        tmp_row = e->nxt_row;
        tmp_idx = e->nxt_idx;
    }
    if (tmp_row < 0){
        e = (_xrow_elt *)NULL;
        tmp_idx = col;
    }
    else {
        if (tmp_idx < 0 || tmp_idx >= A->row[tmp_row].len ||
            A->row[tmp_row].elt[tmp_idx].col != col)
            _xerror(_XE_INTERN,"_xspmat_bump_col");
        e = &(A->row[tmp_row].elt[tmp_idx]);
    }

    *row_num = tmp_row;
    *idx = tmp_idx;

    return (e);
}

/*
   compare two columns -- for sorting rows using qsort()
 */
static int _xcol_cmp(const _xrow_elt *e1, const _xrow_elt *e2){
    return e1->col - e2->col;
}

/*!
   sparse Bunch-Kaufman-Parlett factorisation of A in-situ
   -- A is factored into the form P'AP = MDM' where 
   P is a permutation matrix, M lower triangular and D is block
   diagonal with blocks of size 1 or 2
   -- P is stored in pivot; blocks[i]==i iff D[i][i] is a block
 */
_xspmat *_xspmat_bkpfactor(_xspmat *A, _xperm *pivot, _xperm *blocks, double tol){
    int i, j, k, l, n, onebyone, r;
    int idx, idx1, idx_piv;
    int row_num;
    int best_deg, best_j, best_l, best_cost, mark_cost, deg, deg_j, deg_l, ignore_deg;
    int list_idx, list_idx2, old_list_idx;

    _xsprow *row, *r_piv, *r1_piv;
    _xrow_elt *e, *e1;
    _xreal aii, aip1, aip1i;
    _xreal det, max_j, max_l, s, t;

    _XSTATIC _xivec *scan_row =(_xivec *)NULL,*scan_idx=(_xivec *)NULL,*col_list=(_xivec *)NULL,*tmp_iv=(_xivec *)NULL;
    _XSTATIC _xivec *deg_list =(_xivec *)NULL;
    _XSTATIC _xivec *orig_idx =(_xivec *)NULL,*orig1_idx=(_xivec *)NULL;
    _XSTATIC _xperm *order = (_xperm *)NULL;

    if (!A || !pivot || !blocks)
        _xerror(_XE_NULL,"_xspmat_bkpfactor");
    if (A->m != A->n)
        _xerror(_XE_SQUARE,"_xspmat_bkpfactor");
    if (A->m != pivot->size || pivot->size != blocks->size)
        _xerror(_XE_SIZES,"_xspmat_bkpfactor");
    if (tol <= 0.0 || tol > 1.0)
        _xerror(_XE_RANGE,"_xspmat_bkpfactor");
    
    _xperm_ident(pivot); _xperm_ident(blocks);
    _xspmat_col_access(A); _xspmat_diag_access(A);

    n = A->n;
    ignore_deg = _XFALSE;
    deg_list = _xivec_resize(deg_list,n);
    if (order)
        _xperm_ident(order);
    order = _xperm_resize(order,n);
    _XMEM_STAT_REG(deg_list,_XTYPE_IVEC);
    _XMEM_STAT_REG(order,_XTYPE_PERM);

    scan_row = _xivec_resize(scan_row,5);
    scan_idx = _xivec_resize(scan_idx,5);
    col_list = _xivec_resize(col_list,5);
    orig_idx = _xivec_resize(orig_idx,5);
    orig_idx = _xivec_resize(orig1_idx,5);
    orig_idx = _xivec_resize(tmp_iv,5);
    _XMEM_STAT_REG(scan_row,_XTYPE_IVEC);
    _XMEM_STAT_REG(scan_idx,_XTYPE_IVEC);
    _XMEM_STAT_REG(col_list,_XTYPE_IVEC);
    _XMEM_STAT_REG(orig_idx,_XTYPE_IVEC);
    _XMEM_STAT_REG(orig1_idx,_XTYPE_IVEC);
    _XMEM_STAT_REG(tmp_iv,_XTYPE_IVEC);

    for (i = 0; i < n-1; i = onebyone ? i+1 : i+2){
        /* now we want to use a Markowitz-style selection rule for
           determining which rows to swap and whether to use
           1x1 or 2x2 pivoting 
        */
        
        /* get list of degrees of nodes */
        deg_list = _xivec_resize(deg_list,n-i);
        if (!ignore_deg)
            for (j = i; j < n; j++)
                deg_list->ive[j-i] = 0;
        else {
            for (j = i; j < n; j++)
                deg_list->ive[j-i] = 1;
            if (i < n)
                deg_list->ive[0] = 0;
        }
        order = _xperm_resize(order,n-i);
        _xperm_ident(order);

        if (!ignore_deg){
            for (j = i; j < n; j++){
                idx = 0;
                row = &(A->row[j]);
                e = &(row->elt[idx]);
                for (; idx < row->len; idx++, e++)
                    if (e->col >= i)
                        deg_list->ive[e->col - i]++;
            }
            /* now deg_list[k] == degree of node k+i */
            /* now sort them into increasing order */
            _xivec_sort(deg_list,order);
            /* now deg_list[idx] == degree of node i+order[idx] */
        }

        /* now we can chase through the nodes in order of increasing
           degree, picking out the ones that satisfy our stability
           criterion */
        list_idx = 0; r = -1;
        best_j = best_l = -1;
        for (deg = 0; deg <= n; deg++){
            _xreal ajj, all, ajl;
            if (list_idx >= (int)(deg_list->dim))
                break;  /* That's all folks! */
            old_list_idx = list_idx;
            while (list_idx < (int)(deg_list->dim) &&
                   deg_list->ive[list_idx] <= deg){
                j = i+order->pe[list_idx];
                if (j < i)
                    continue;
                /* can we use row/col j for a 1 x 1 pivot? */
                /* find max_j = max_{k>=i} {|A[k][j]|,|A[j][k]|} */
                ajj = (_xreal)fabs(_xunord_get_val(A,j,j));
                if (ajj == 0.0){
                    list_idx++;
                    continue; /* can't use this for 1 x 1 pivot */
                }
        
                max_j = (_xreal)_xmax_row_col(A,i,j,-1);
                if (ajj >= tol*max_j){
                    onebyone = _XTRUE;
                    best_j = j;
                    best_deg = deg_list->ive[list_idx];
                    break;
                }
                list_idx++;
            }
            if (best_j >= 0)
                break;
            best_cost = 2*n;  /* > any possible Markowitz cost (bound) */
            best_j = best_l = -1;
            list_idx = old_list_idx;
            while (list_idx < (int)(deg_list->dim) &&
                   deg_list->ive[list_idx] <= deg){
                j = i+order->pe[list_idx];
                ajj = (_xreal)fabs(_xunord_get_val(A,j,j));
                for (list_idx2 = 0; list_idx2 < list_idx; list_idx2++){
                    deg_j = deg;
                    deg_l = deg_list->ive[list_idx2];
                    l = i+order->pe[list_idx2];
                    if (l < i)
                        continue;
                    /* try using rows/cols (j,l) for a 2 x 2 pivot block */
                    all = (_xreal)fabs(_xunord_get_val(A,l,l));
                    ajl = ( j > l ) ? (_xreal)fabs(_xunord_get_val(A,l,j)) :
                                      (_xreal)fabs(_xunord_get_val(A,j,l));
                    det = (_xreal)fabs(ajj*all - ajl*ajl);
                    if (det == 0.0)
                        continue;
                    max_j = (_xreal)_xmax_row_col(A,i,j,l);
                    max_l = (_xreal)_xmax_row_col(A,i,l,j);
                    if (tol*(all*max_j+ajl*max_l) < det &&
                        tol*(ajl*max_j+ajj*max_l) < det ){
                      /* acceptably stable 2 x 2 pivot */
                      /* this is actually an overestimate of the
                         Markowitz cost for choosing (j,l) */
                        mark_cost = (ajj == 0.0) ?
                                    ((all == 0.0) ? deg_j+deg_l : deg_j+2*deg_l) :
                                    ((all == 0.0) ? 2*deg_j+deg_l : 2*(deg_j+deg_l));
                        if (mark_cost < best_cost){
                            onebyone = _XFALSE;
                            best_cost = mark_cost;
                            best_j = j;
                            best_l = l;
                            best_deg = deg_j;
                        }
                    }
                }
                list_idx++;
            }
            if (best_j >= 0)
                break;
        }

        if (best_deg > (int)floor(0.8*(n-i)))
            ignore_deg = _XTRUE;

        /* now do actual interchanges */
        if (best_j >= 0 && onebyone){
            _xbkp_interchange(A,i,best_j);
            _xperm_transp(pivot,i,best_j);
        }
        else if (best_j >= 0 && best_l >= 0 && ! onebyone){
            if (best_j == i || best_j == i+1){
                if (best_l == i || best_l == i+1){
                    /* no pivoting, but must update blocks permutation */
                    _xperm_transp(blocks,i,i+1);
                    goto dopivot;
                }
                _xbkp_interchange(A,(best_j == i) ? i+1 : i,best_l);
                _xperm_transp(pivot,(best_j == i) ? i+1 : i,best_l);
            }
            else if (best_l == i || best_l == i+1){
                _xbkp_interchange(A,(best_l == i) ? i+1 : i,best_j);
                _xperm_transp(pivot,(best_l == i) ? i+1 : i,best_j);
            }
            else { /* best_j & best_l outside i, i+1 */
                if (i != best_j){
                    _xbkp_interchange(A,i,best_j);
                    _xperm_transp(pivot,i,best_j);
                }     
                if (i+1 != best_l){
                    _xbkp_interchange(A,i+1,best_l);
                    _xperm_transp(pivot,i+1,best_l);
                }
            }
        }
        else /* can't pivot &/or nothing to pivot */
            continue;

        /* update blocks permutation */
        if (!onebyone)
            _xperm_transp(blocks,i,i+1);

    dopivot:
        if (onebyone){
            int idx_j, idx_k, s_idx, s_idx2;
            _xrow_elt *e_ij, *e_ik;
        
            r_piv = &(A->row[i]);
            idx_piv = _xunord_get_idx(r_piv,i);
            /* if idx_piv < 0 then aii == 0 and no pivoting can be done;
               -- this means that we should continue to the next iteration */
            if (idx_piv < 0)
                continue;
            aii = r_piv->elt[idx_piv].val;
            if (aii == 0.0)
                continue;
        
            /* for ( j = i+1; j < n; j++ )  { ... pivot step ... } */
            /* initialise scan_... etc for the 1 x 1 pivot */
            scan_row = _xivec_resize(scan_row,r_piv->len);
            scan_idx = _xivec_resize(scan_idx,r_piv->len);
            col_list = _xivec_resize(col_list,r_piv->len);
            orig_idx = _xivec_resize(orig_idx,r_piv->len);
            row_num = i; s_idx = idx = 0;
            e = &(r_piv->elt[idx]);
            for (idx = 0; idx < r_piv->len; idx++, e++){
                if (e->col < i)
                    continue;
                scan_row->ive[s_idx] = i;
                scan_idx->ive[s_idx] = idx;
                orig_idx->ive[s_idx] = idx;
                col_list->ive[s_idx] = e->col;
                s_idx++;
            }
            scan_row = _xivec_resize(scan_row,s_idx);
            scan_idx = _xivec_resize(scan_idx,s_idx);
            col_list = _xivec_resize(col_list,s_idx);
            orig_idx = _xivec_resize(orig_idx,s_idx);
        
            order = _xperm_resize(order,scan_row->dim);
            _xperm_ident(order);
            _xivec_sort(col_list,order);
        
            tmp_iv = _xivec_resize(tmp_iv,scan_row->dim);
            for (idx = 0; (unsigned int)idx < order->size; idx++)
                tmp_iv->ive[idx] = scan_idx->ive[order->pe[idx]];
            _xivec_copy(tmp_iv,scan_idx);
            for (idx = 0; (unsigned int)idx < order->size; idx++)
                tmp_iv->ive[idx] = scan_row->ive[order->pe[idx]];
            _xivec_copy(tmp_iv,scan_row);
            for (idx = 0; (unsigned int)idx < scan_row->dim; idx++)
                tmp_iv->ive[idx] = orig_idx->ive[order->pe[idx]];
            _xivec_copy(tmp_iv,orig_idx);
        
            /* now do actual pivot */
            /* for ( j = i+1; j < n-1; j++ ) .... */
        
            for (s_idx = 0; (unsigned int)s_idx < scan_row->dim; s_idx++){
                idx_j = orig_idx->ive[s_idx];
                if (idx_j < 0)
                    _xerror(_XE_INTERN,"_xspmat_bkpfactor");
                e_ij = &(r_piv->elt[idx_j]);
                j = e_ij->col;
                if (j < i+1)
                    continue;
                _xspmat_scan_to(A,scan_row,scan_idx,col_list,j);
                
                /* compute multiplier */
                t = e_ij->val / aii;
                
                /* for ( k = j; k < n; k++ ) { .... update A[j][k] .... } */
                /* this is the row in which pivoting is done */
                row = &(A->row[j]);
                for (s_idx2 = s_idx; (unsigned int)s_idx2 < scan_row->dim; s_idx2++){
                    idx_k = orig_idx->ive[s_idx2];
                    e_ik = &(r_piv->elt[idx_k]);
                    k = e_ik->col;
                    /* k >= j since col_list has been sorted */
                    if (scan_row->ive[s_idx2] == j){ /* no fill-in -- can be done directly */
                        idx = scan_idx->ive[s_idx2];
                        row->elt[idx].val -= t*e_ik->val;
                    }
                    else { /* fill-in -- insert entry & patch column */
                        int old_row, old_idx;
                        _xrow_elt *old_e, *new_e;
                        
                        old_row = scan_row->ive[s_idx2];
                        old_idx = scan_idx->ive[s_idx2];
                        
                        if (old_idx < 0)
                            _xerror(_XE_INTERN,"_xspmat_bkpfactor");
                        idx = row->len;
                        
                        if (row->len >= row->maxlen){
                            _xtracecatch(_xsprow_xpd(row,2*row->maxlen+1,_XTYPE_SPMAT),"_xspmat_bkpfactor");
                        }
        
                        row->len = idx+1;
                        new_e = &(row->elt[idx]);
                        new_e->val = -t*e_ik->val;
                        new_e->col = k;
                        
                        old_e = &(A->row[old_row].elt[old_idx]);
                        new_e->nxt_row = old_e->nxt_row;
                        new_e->nxt_idx = old_e->nxt_idx;
                        old_e->nxt_row = j;
                        old_e->nxt_idx = idx;
                    }
                }
                e_ij->val = t;
            }
        }
        else {  /* onebyone == _XFALSE */
                /* do 2 x 2 pivot */
            int idx_k, idx1_k, s_idx, s_idx2;
            int old_col;
            _xrow_elt *e_tmp;
        
            r_piv = &(A->row[i]);
            idx_piv = _xunord_get_idx(r_piv,i);
            aii = aip1i = 0.0;
            e_tmp = r_piv->elt;
            for (idx_piv = 0; idx_piv < r_piv->len; idx_piv++, e_tmp++)
                if (e_tmp->col == i)
                    aii = e_tmp->val;
                else if (e_tmp->col == i+1)
                    aip1i = e_tmp->val;
        
            r1_piv = &(A->row[i+1]);
            e_tmp = r1_piv->elt;
            aip1 = (_xreal)_xunord_get_val(A,i+1,i+1);
            det = aii*aip1 - aip1i*aip1i; /* Must have det < 0 */
            if (aii == 0.0 && aip1i == 0.0){
                onebyone = _XTRUE;
                continue; /* cannot pivot */
            }
        
            if (det == 0.0){
                if (aii != 0.0)
                    _xerror(_XE_RANGE,"_xspmat_bkpfactor");
                onebyone = _XTRUE;
                continue; /* cannot pivot */
            }
            aip1i = aip1i/det;
            aii = aii/det;
            aip1 = aip1/det;
        
            /* initialise scan_... etc for the 2 x 2 pivot */
            s_idx = r_piv->len + r1_piv->len;
            scan_row = _xivec_resize(scan_row,s_idx);
            scan_idx = _xivec_resize(scan_idx,s_idx);
            col_list = _xivec_resize(col_list,s_idx);
            orig_idx = _xivec_resize(orig_idx,s_idx);
            orig1_idx = _xivec_resize(orig1_idx,s_idx);
        
            e = r_piv->elt;
            for (idx = 0; idx < r_piv->len; idx++, e++){
                scan_row->ive[idx] = i;
                scan_idx->ive[idx] = idx;
                col_list->ive[idx] = e->col;
                orig_idx->ive[idx] = idx;
                orig1_idx->ive[idx] = -1;
            }
            e = r_piv->elt;
            e1 = r1_piv->elt;
            for (idx = 0; idx < r1_piv->len; idx++, e1++){
                scan_row->ive[idx+r_piv->len] = i+1;
                scan_idx->ive[idx+r_piv->len] = idx;
                col_list->ive[idx+r_piv->len] = e1->col;
                orig_idx->ive[idx+r_piv->len] = -1;
                orig1_idx->ive[idx+r_piv->len] = idx;
            }
        
            e1 = r1_piv->elt;
            order = _xperm_resize(order,scan_row->dim);
            _xperm_ident(order);
            _xivec_sort(col_list,order);
            tmp_iv = _xivec_resize(tmp_iv,scan_row->dim);
            for (idx = 0; (unsigned int)idx < order->size; idx++)
                tmp_iv->ive[idx] = scan_idx->ive[order->pe[idx]];
            _xivec_copy(tmp_iv,scan_idx);
            for (idx = 0; (unsigned int)idx < order->size; idx++)
                tmp_iv->ive[idx] = scan_row->ive[order->pe[idx]];
            _xivec_copy(tmp_iv,scan_row);
            for (idx = 0; (unsigned int)idx < scan_row->dim; idx++)
                tmp_iv->ive[idx] = orig_idx->ive[order->pe[idx]];
            _xivec_copy(tmp_iv,orig_idx);
            for (idx = 0; (unsigned int)idx < scan_row->dim; idx++)
                tmp_iv->ive[idx] = orig1_idx->ive[order->pe[idx]];
            _xivec_copy(tmp_iv,orig1_idx);
        
            s_idx = 0;
            old_col = -1;
            for (idx = 0; (unsigned int)idx < scan_row->dim; idx++){
                if (col_list->ive[idx] == old_col){
                    if (scan_row->ive[idx] == i){
                        scan_row->ive[s_idx-1] = scan_row->ive[idx];
                        scan_idx->ive[s_idx-1] = scan_idx->ive[idx];
                        col_list->ive[s_idx-1] = col_list->ive[idx];
                        orig_idx->ive[s_idx-1] = orig_idx->ive[idx];
                        orig1_idx->ive[s_idx-1] = orig1_idx->ive[idx-1];
                    }   
                    else if (idx > 0){
                        scan_row->ive[s_idx-1] = scan_row->ive[idx-1];
                        scan_idx->ive[s_idx-1] = scan_idx->ive[idx-1];
                        col_list->ive[s_idx-1] = col_list->ive[idx-1];
                        orig_idx->ive[s_idx-1] = orig_idx->ive[idx-1];
                        orig1_idx->ive[s_idx-1] = orig1_idx->ive[idx];
                    }   
                }
                else {
                    scan_row->ive[s_idx] = scan_row->ive[idx];
                    scan_idx->ive[s_idx] = scan_idx->ive[idx];
                    col_list->ive[s_idx] = col_list->ive[idx];
                    orig_idx->ive[s_idx] = orig_idx->ive[idx];
                    orig1_idx->ive[s_idx] = orig1_idx->ive[idx];
                    s_idx++;
                }
                old_col = col_list->ive[idx];
            }
            scan_row = _xivec_resize(scan_row,s_idx);
            scan_idx = _xivec_resize(scan_idx,s_idx);
            col_list = _xivec_resize(col_list,s_idx);
            orig_idx = _xivec_resize(orig_idx,s_idx);
            orig1_idx = _xivec_resize(orig1_idx,s_idx);
        
            /* for ( j = i+2; j < n; j++ )  { .... row operation .... } */
            for (s_idx = 0; (unsigned int)s_idx < scan_row->dim; s_idx++){
                int idx_piv, idx1_piv;
                _xreal aip1j, aij, aik, aip1k;
                _xrow_elt *e_ik, *e_ip1k;
                
                j = col_list->ive[s_idx];
                if (j < i+2)
                    continue;
                _xtracecatch(_xspmat_scan_to(A,scan_row,scan_idx,col_list,j),"_xspmat_bkpfactor");
                
                idx_piv = orig_idx->ive[s_idx];
                aij = ( idx_piv < 0 ) ? (_xreal)0.0 : r_piv->elt[idx_piv].val;
                idx1_piv = orig1_idx->ive[s_idx];
                aip1j = ( idx1_piv < 0 ) ? (_xreal)0.0 : r1_piv->elt[idx1_piv].val;
                s = - aip1i*aip1j + aip1*aij;
                t = - aip1i*aij + aii*aip1j;
                
                /* for ( k = j; k < n; k++ )  { .... update entry .... } */
                row = &(A->row[j]);
                /* set idx_k and idx1_k indices */
                s_idx2 = s_idx;
                k = col_list->ive[s_idx2];
                idx_k = orig_idx->ive[s_idx2];
                idx1_k = orig1_idx->ive[s_idx2];
                
                while ((unsigned int)s_idx2 < scan_row->dim){
                    k = col_list->ive[s_idx2];
                    idx_k = orig_idx->ive[s_idx2];
                    idx1_k = orig1_idx->ive[s_idx2];
                    e_ik = ( idx_k < 0 ) ? (_xrow_elt *)NULL : &(r_piv->elt[idx_k]);
                    e_ip1k = ( idx1_k < 0 ) ? (_xrow_elt *)NULL : &(r1_piv->elt[idx1_k]);
                    aik = ( idx_k >= 0 ) ? e_ik->val : (_xreal)0.0;
                    aip1k = ( idx1_k >= 0 ) ? e_ip1k->val : (_xreal)0.0;
                    if (scan_row->ive[s_idx2] == j){ /* no fill-in */
                        row = &(A->row[j]);
                        idx = scan_idx->ive[s_idx2];
                        if (idx < 0)
                            _xerror(_XE_INTERN,"_xspmat_bkpfactor");
                        row->elt[idx].val -= s*aik + t*aip1k;
                    }
                    else{ /* fill-in -- insert entry & patch column */
                        _xreal tmp;
                        int old_row, old_idx;
                        _xrow_elt *old_e, *new_e;
                        
                        tmp = - s*aik - t*aip1k;
                        if (tmp != 0.0){
                            row = &(A->row[j]);
                            old_row = scan_row->ive[s_idx2];
                            old_idx = scan_idx->ive[s_idx2];
                        
                            idx = row->len;
                            if (row->len >= row->maxlen){
                                _xtracecatch(_xsprow_xpd(row,2*row->maxlen+1,_XTYPE_SPMAT),"_xspmat_bkpfactor");
                            }
        
                            row->len = idx + 1;
                            new_e = &(row->elt[idx]);
                            new_e->val = tmp;
                            new_e->col = k;
                            
                            if (old_row < 0)
                              _xerror(_XE_INTERN,"_xspmat_bkpfactor");
                            old_e = &(A->row[old_row].elt[old_idx]);
                            new_e->nxt_row = old_e->nxt_row;
                            new_e->nxt_idx = old_e->nxt_idx;
                            old_e->nxt_row = j;
                            old_e->nxt_idx = idx;
                        }
                    }
        
                    /* update idx_k, idx1_k, s_idx2 etc */
                    s_idx2++;
                }
        
                /* store multipliers -- may involve fill-in (!) */
                idx = orig_idx->ive[s_idx];
                if (idx >= 0){
                    r_piv->elt[idx].val = s;
                }
                else if (s != 0.0){
                    int old_row, old_idx;
                    _xrow_elt *new_e, *old_e;
                    
                    old_row = -1; old_idx = j;
                    if (i > 0){
                        _xtracecatch(_xspmat_chase_col(A,j,&old_row,&old_idx,i-1),"_xspmat_bkpfactor");
                    }
                    idx = r_piv->len;
                    if (r_piv->len >= r_piv->maxlen){
                        _xtracecatch(_xsprow_xpd(r_piv,2*r_piv->maxlen+1,_XTYPE_SPMAT),"_xspmat_bkpfactor");
                    }
        
                    r_piv->len = idx + 1;
                    new_e = &(r_piv->elt[idx]);
                    new_e->val = s;
                    new_e->col = j;
                    if (old_row < 0){
                        new_e->nxt_row = A->start_row[j];
                        new_e->nxt_idx = A->start_idx[j];
                        A->start_row[j] = i;
                        A->start_idx[j] = idx;
                    }
                    else {
                        if (old_idx < 0)
                            _xerror(_XE_INTERN,"_xspmat_bkpfactor");
                        old_e = &(A->row[old_row].elt[old_idx]);
                        new_e->nxt_row = old_e->nxt_row;
                        new_e->nxt_idx = old_e->nxt_idx;
                        old_e->nxt_row = i;
                        old_e->nxt_idx = idx;
                    }
                }
                idx1 = orig1_idx->ive[s_idx];
                if (idx1 >= 0){
                    r1_piv->elt[idx1].val = t;
                }
                else if (t != 0.0){
                    int old_row, old_idx;
                    _xrow_elt *new_e, *old_e;
                    
                    old_row = -1; old_idx = j;
                    _xtracecatch(_xspmat_chase_col(A,j,&old_row,&old_idx,i),"_xspmat_bkpfactor");
                    idx1 = r1_piv->len;
                    if (r1_piv->len >= r1_piv->maxlen){
                        _xtracecatch(_xsprow_xpd(r1_piv,2*r1_piv->maxlen+1,_XTYPE_SPMAT),
                                     "_xspmat_bkpfactor");
                    }
        
                    r1_piv->len = idx1 + 1;
                    new_e = &(r1_piv->elt[idx1]);
                    new_e->val = t;
                    new_e->col = j;
                    if (idx1 < 0)
                      _xerror(_XE_INTERN,"_xspmat_bkpfactor");
                    new_e = &(r1_piv->elt[idx1]);
                    if (old_row < 0){
                      new_e->nxt_row = A->start_row[j];
                      new_e->nxt_idx = A->start_idx[j];
                      A->start_row[j] = i+1;
                      A->start_idx[j] = idx1;
                    }       
                    else {
                        old_idx = _xsprow_idx2(&(A->row[old_row]),j,old_idx);
                        if (old_idx < 0)
                            _xerror(_XE_INTERN,"_xspmat_bkpfactor");
                        old_e = &(A->row[old_row].elt[old_idx]);
                        new_e->nxt_row = old_e->nxt_row;
                        new_e->nxt_idx = old_e->nxt_idx;
                        old_e->nxt_row = i+1;
                        old_e->nxt_idx = idx1;
                    }
                }
            }
        }
    }

    /* now sort the rows arrays */
    for (i = 0; i < A->m; i++)
        qsort(A->row[i].elt,A->row[i].len,sizeof(_xrow_elt),(int (*)(const void *, const void *))_xcol_cmp);
    A->flag_col = A->flag_diag = _XFALSE;

#ifdef _XTHREAD_SAFE
    _xivec_ifree(scan_row); _xivec_ifree(scan_idx); _xivec_ifree(col_list);
    _xivec_ifree(tmp_iv); _xivec_ifree(deg_list); _xivec_ifree(orig_idx);
    _xivec_ifree(orig1_idx);_xperm_pfree(order);
#endif
    return (A);
}

/*!
    update scan (int) vectors to point to the last row in each
  column with row # <= max_row, if any
 */
void _xspmat_scan_to(_xspmat *A, _xivec *scan_row, _xivec *scan_idx, _xivec *col_list, int max_row){
    int col, idx, j_idx, row_num;
    _xsprow *r;
    _xrow_elt *e;

    if (!A || !scan_row || !scan_idx || !col_list)
        _xerror(_XE_NULL,"_xspmat_scan_to");
    if (scan_row->dim != scan_idx->dim || scan_idx->dim != col_list->dim)
        _xerror(_XE_SIZES,"_xspmat_scan_to");

    if (max_row < 0)
        return;

    if (!A->flag_col)
        _xspmat_col_access(A);

    for (j_idx = 0; (unsigned int)j_idx < scan_row->dim; j_idx++){
        row_num = scan_row->ive[j_idx];
        idx = scan_idx->ive[j_idx];
        col = col_list->ive[j_idx];

        if (col < 0 || col >= A->n)
           _xerror(_XE_BOUNDS,"_xspmat_scan_to");
        if (row_num < 0){
            idx = col;
            continue;
        }
        r = &(A->row[row_num]);
        if (idx < 0)
            _xerror(_XE_INTERN,"_xspmat_scan_to");
        e = &(r->elt[idx]);
        if (e->col != col)
            _xerror(_XE_INTERN,"_xspmat_scan_to");
        if (idx < 0){
#ifdef _XDEBUG
            (void) fprintf(stdout, "_xspmat_scan_to: row_num = %d, idx = %d, col = %d\n",row_num, idx, col);
#endif
            _xerror(_XE_INTERN,"_xspmat_scan_to");
        }
        while (e->nxt_row >= 0 && e->nxt_row <= max_row){
            row_num = e->nxt_row;
            idx = e->nxt_idx;
            e = &(A->row[row_num].elt[idx]);
        }
        scan_row->ive[j_idx] = row_num;
        scan_idx->ive[j_idx] = idx;
    }
}

/*!
   solve A.x = b where A has been factored a la _xspmat_bkpfactor()
   -- returns x, which is created if NULL
 */
_xvec *_xspmat_bkpsolve(_xspmat *A, _xperm *pivot, _xperm *block, const _xvec *b, _xvec *x){
    _XSTATIC _xvec *tmp=(_xvec *)NULL;  /* dummy storage needed */
    int i, n, onebyone;
    int row_num, idx;
    _xreal a11, a12, a22, b1, b2, det, sum, *tmp_ve, tmp_diag;
    _xsprow *r;
    _xrow_elt *e;
    
    if (!A || !pivot || !block || !b)
        _xerror(_XE_NULL,"_xspmat_bkpsolve");
    if (A->m != A->n)
        _xerror(_XE_SQUARE,"_xspmat_bkpsolve");
    n = A->n;
    if (b->dim != n || pivot->size != n || block->size != n)
        _xerror(_XE_SIZES,"_xspmat_bkpsolve");

    x = _xvec_resize(x,n);
    tmp = _xvec_resize(tmp,n);
    _XMEM_STAT_REG(tmp,_XTYPE_VEC);
    
    tmp_ve = tmp->ve;
    if (!A->flag_col)
        _xspmat_col_access(A);

    _xperm_vec(pivot,b,tmp);
    /* solve for lower triangular part */
    for (i = 0; i < n; i++){
        sum = tmp_ve[i];
        if (block->pe[i] < (unsigned int)i){
            row_num = -1; idx = i;
            e = _xspmat_bump_col(A,i,&row_num,&idx);
            while (row_num >= 0 && row_num < i-1){
                sum -= e->val*tmp_ve[row_num];
                e = _xspmat_bump_col(A,i,&row_num,&idx);
            }
        }
        else {
            row_num = -1; idx = i;
            e = _xspmat_bump_col(A,i,&row_num,&idx);
            while (row_num >= 0 && row_num < i){
                sum -= e->val*tmp_ve[row_num];
                e = _xspmat_bump_col(A,i,&row_num,&idx);
            }
        }
        tmp_ve[i] = sum;
    }
    /* solve for diagonal part */
    for (i = 0; i < n; i = onebyone ? i+1 : i+2){
        onebyone = ( block->pe[i] == i );
        if (onebyone){
            tmp_diag = (_xreal)_xspmat_get_val(A,i,i);
            if (tmp_diag == 0.0)
                _xerror(_XE_SING,"_xspmat_bkpsolve");
            tmp_ve[i] /= tmp_diag;
        }
        else {
            a11 = (_xreal)_xspmat_get_val(A,i,i);
            a22 = (_xreal)_xspmat_get_val(A,i+1,i+1);
            a12 = (_xreal)_xspmat_get_val(A,i,i+1);
            b1 = tmp_ve[i];
            b2 = tmp_ve[i+1];
            det = a11*a22-a12*a12;  /* < 0 : see BKPfactor() */
            if (det == 0.0)
                _xerror(_XE_SING,"_xspmat_bkpsolve");
            det = 1/det;
            tmp_ve[i]   = det*(a22*b1-a12*b2);
            tmp_ve[i+1] = det*(a11*b2-a12*b1);
        }
    }
    /* solve for transpose of lower triangular part */
    for (i = n-2; i >= 0; i--){
        sum = tmp_ve[i];
        if (block->pe[i] > (unsigned int)i){
            /* onebyone is false */
            if (i+2 >= n)
                continue;
            r = &(A->row[i]);
            idx = _xsprow_idx(r,i+2);
            idx = _xsprow_fixindex(idx);
            e = &(r->elt[idx]);
            for (; idx < r->len; idx++, e++)
                sum -= e->val*tmp_ve[e->col];
        }   
        else { /* onebyone */
            r = &(A->row[i]);
            idx = _xsprow_idx(r,i+1);
            idx = _xsprow_fixindex(idx);
            e = &(r->elt[idx]);
            for (; idx < r->len; idx++, e++)
                sum -= e->val*tmp_ve[e->col];
        }
        tmp_ve[i] = sum;
    }

    /* and do final permutation */
    x = _xperm_inv_vec(pivot,tmp,x);

#ifdef _XTHREAD_SAFE
    _xvec_vfree(tmp);
#endif

    return (x);
}

/*
  conjugate gradient routines
  uses sparse matrix input & sparse Cholesky factorisation in _xspmat_pccg().

  all the following routines use routines to define a matrix
    rather than use any explicit representation
    (with the exeception of the pccg() pre-conditioner)
  the matrix A is defined by

    _xvec *(*A)(void *params, _xvec *x, _xvec *y)

  where y = A.x on exit, and y is returned. The params argument is
  intended to make it easier to re-use & modify such routines.

  If we have a sparse matrix data structure
    _xspmat *A_mat;
  then these can be used by passing _xspmat_mv_mlt as the function, and
  A_mat as the param.
*/
static int _xmax_iter = 10000;
int _xcg_num_iters;

/*!
    set maximum number of iterations if numiter > 1
  -- just returns current max_iter otherwise
  -- returns old maximum
 */
int _xspmat_cg_set_maxiter(int numiter){
    int temp;

    if (numiter < 2)
        return _xmax_iter;
    temp = _xmax_iter;
    _xmax_iter = numiter;
    return (temp);
}

/*!
    solve A.x = b using pre-conditioner M
      (assumed factored a la _xspmat_chfctr())
  -- results are stored in x (if x != NULL), which is returned
 */
_xvec *_xspmat_pccg(_xvec *(*A)(void *, _xvec *, _xvec *), void *A_params, 
                    _xvec *(*M)(void *, _xvec *, _xvec *), void *M_params, _xvec *b, double eps, _xvec *x){
    _xvec *r = (_xvec *)NULL, *p = (_xvec *)NULL, *q = (_xvec *)NULL, *z = (_xvec *)NULL;
    int k;
    _xreal alpha, beta, ip, old_ip, norm_b;

    if (!A || !b)
        _xerror(_XE_NULL,"_xspmat_pccg");
    if (x == b)
        _xerror(_XE_INSITU,"_xspmat_pccg");
    x = _xvec_resize(x,b->dim);
    if (eps <= 0.0)
        eps = _XMACHEPS;
    
    r = _xvec_get(b->dim);
    p = _xvec_get(b->dim);
    q = _xvec_get(b->dim);
    z = _xvec_get(b->dim);
    
    norm_b = (_xreal)_xvec_vnorm2(b);
    _xvec_zero(x);
    r = _xvec_vcopy(b,r);
    old_ip = 0.0;
    for (k = 0;; k++){
        if (_xvec_vnorm2(r) < eps*norm_b)
            break;
        if (k > _xmax_iter)
            _xerror(_XE_ITER,"_xspmat_pccg");
        if (M)
            (*M)(M_params,r,z);
        else
            _xvec_vcopy(r,z); /* M == identity */
        ip = (_xreal)_xvec_vin_prod(z,r);
        if (k){ /* if ( k > 0 ) ... */
            beta = ip/old_ip;
            p = _xvec_mltadd(z,p,beta,p);
        }
        else { /* if ( k == 0 ) ... */
            beta = 0.0;
            p = _xvec_vcopy(z,p);
            old_ip = 0.0;
        }
        q = (*A)(A_params,p,q);
        alpha = (_xreal)(ip/_xvec_vin_prod(p,q));
        x = _xvec_mltadd(x,p,alpha,x);
        r = _xvec_mltadd(r,q,-alpha,r);
        old_ip = ip;
    }
    _xcg_num_iters = k;
    
    _xvec_vfree(p);
    _xvec_vfree(q);
    _xvec_vfree(r);
    _xvec_vfree(z);
    
    return (x);
}

/*!
    a simple interface to _xspmat_pccg() which uses sparse matrix
    data structures
  -- assumes that LLT contains the Cholesky factorisation of the
    actual pre-conditioner
 */
_xvec *_xspmat_sppccg(_xspmat *A, _xspmat *LLT, _xvec *b, double eps, _xvec *x){
    return _xspmat_pccg((_xvec *(*)(void *, _xvec *, _xvec *))_xspmat_mv_mlt, A, 
                        (_xvec *(*)(void *, _xvec *, _xvec *))_xspmat_chsolve, LLT, b, eps, x);
}

/*!
    use CGS to compute a solution x to A.x=b
  -- the matrix A is not passed explicitly, rather a routine
    A is passed where A(x,Ax,params) computes
    Ax = A.x
  -- the computed solution is passed
  tilde r0 parameter -- should be random???
  tol --error tolerance used
 */
_xvec *_xspmat_cgs(_xvec *(*A)(void *, _xvec *, _xvec *),void *A_params, _xvec *b, _xvec *r0, double tol, _xvec *x){
    _xvec *p, *q, *r, *u, *v, *tmp1, *tmp2;
    _xreal alpha, beta, norm_b, rho, old_rho, sigma;
    int iter;
    
    if (!A || !x || !b || !r0)
        _xerror(_XE_NULL,"_xspmat_cgs");
    if (x->dim != b->dim || r0->dim != x->dim)
        _xerror(_XE_SIZES,"_xspmat_cgs");
    if (tol <= 0.0)
        tol = _XMACHEPS;
    
    p = _xvec_get(x->dim);
    q = _xvec_get(x->dim);
    r = _xvec_get(x->dim);
    u = _xvec_get(x->dim);
    v = _xvec_get(x->dim);
    tmp1 = _xvec_get(x->dim);
    tmp2 = _xvec_get(x->dim);
    
    norm_b = (_xreal)_xvec_vnorm2(b);
    (*A)(A_params,x,tmp1);
    _xvec_sub(b,tmp1,r);
    _xvec_zero(p);
    _xvec_zero(q);
    old_rho = 1.0;
    
    iter = 0;
    while (_xvec_vnorm2(r) > tol*norm_b){
        if (++iter > _xmax_iter)
            break;
        rho = (_xreal)_xvec_vin_prod(r0,r);
        if (old_rho == 0.0)
            _xerror(_XE_SING,"_xspmat_cgs");
        beta = rho/old_rho;
        _xvec_mltadd(r,q,beta,u);
        _xvec_mltadd(q,p,beta,tmp1);
        _xvec_mltadd(u,tmp1,beta,p);
        
        (*A)(A_params,p,v);
        
        sigma = (_xreal)_xvec_vin_prod(r0,v);
        if (sigma == 0.0)
            _xerror(_XE_SING,"_xspmat_cgs");
        alpha = rho/sigma;
        _xvec_mltadd(u,v,-alpha,q);
        _xvec_add(u,q,tmp1);
        
        (*A)(A_params,tmp1,tmp2);
        
        _xvec_mltadd(r,tmp2,-alpha,r);
        _xvec_mltadd(x,tmp1,alpha,x);
        
        old_rho = rho;
    }
    _xcg_num_iters = iter;
    
    _xvec_vfree(p); _xvec_vfree(q); _xvec_vfree(r);
    _xvec_vfree(u); _xvec_vfree(v);
    _xvec_vfree(tmp1); _xvec_vfree(tmp2);
    
    return (x);
}

/*!
   simple interface for _xspmat data structures
 */
_xvec *_xspmat_spcgs(_xspmat *A, _xvec *b, _xvec *r0, double tol, _xvec *x){
    return _xspmat_cgs((_xvec *(*)(void *, _xvec *, _xvec *))_xspmat_mv_mlt,A,b,r0,tol,x);
}

/*!
  routine for performing LSQR -- the least squares QR algorithm
  of Paige and Saunders:
    "LSQR: an algorithm for sparse linear equations and
    sparse least squares", ACM Trans. Math. Soft., v. 8
    pp. 43--71 (1982)
    lsqr -- sparse CG-like least squares routine:
  -- find min_x ||A.x-b||_2 using A defined through A & AT
  -- return x (if x != NULL) 
  -- AT is A transposed 
 */
_xvec *_xspmat_lsqr(_xvec *(*A)(void *, _xvec *, _xvec *), _xvec *(*AT)(void *, _xvec *, _xvec *),void *A_params, _xvec *b, double tol, _xvec *x){
    _xvec *u, *v, *w, *tmp;
    _xreal alpha, beta, norm_b, phi, phi_bar,rho, rho_bar, rho_max, theta;
    _xreal s, c; /* for Givens' rotations */
    int iter, m, n;
    
    if (!b || !x)
        _xerror(_XE_NULL,"_xspmat_lsqr");
    if (tol <= 0.0)
        tol = _XMACHEPS;
    
    m = b->dim; n = x->dim;
    u = _xvec_get((unsigned int)m);
    v = _xvec_get((unsigned int)n);
    w = _xvec_get((unsigned int)n);
    tmp = _xvec_get((unsigned int)n);
    norm_b = (_xreal)_xvec_vnorm2(b);
    
    _xvec_zero(x);
    beta = (_xreal)_xvec_vnorm2(b);
    if (beta == 0.0)
        return (x);
    _xvec_sv_mlt(1.0/beta,b,u);
    _xtracecatch((*AT)(A_params,u,v),"_xspmat_lsqr");
    alpha = (_xreal)_xvec_vnorm2(v);
    if (alpha == 0.0)
        return (x);
    _xvec_sv_mlt(1.0/alpha,v,v);
    _xvec_vcopy(v,w);
    
    phi_bar = beta; rho_bar = alpha;
    rho_max = 1.0;
    iter = 0;
    do {
        if (++iter > _xmax_iter)
            _xerror(_XE_ITER,"_xspmat_lsqr");
        
        tmp = _xvec_resize(tmp,m);
        _xtracecatch((*A) (A_params,v,tmp),"_xspmat_lsqr");
        
        _xvec_mltadd(tmp,u,-alpha,u);
        beta = (_xreal)_xvec_vnorm2(u); _xvec_sv_mlt(1.0/beta,u,u);
        
        tmp = _xvec_resize(tmp,n);
        _xtracecatch((*AT)(A_params,u,tmp),"_xspmat_lsqr");
        _xvec_mltadd(tmp,v,-beta,v);
        alpha = (_xreal)_xvec_vnorm2(v); _xvec_sv_mlt(1.0/alpha,v,v);
        
        rho = (_xreal)sqrt(rho_bar*rho_bar+beta*beta);
        if (rho > rho_max)
            rho_max = rho;
        c = rho_bar/rho;
        s = beta/rho;
        theta   =  s*alpha;
        rho_bar = -c*alpha;
        phi     =  c*phi_bar;
        phi_bar =  s*phi_bar;
        
        /* update x & w */
        if (rho == 0.0)
            _xerror(_XE_SING,"_xspmat_lsqr");
        _xvec_mltadd(x,w,phi/rho,x);
        _xvec_mltadd(v,w,-theta/rho,w);
    } while (fabs(phi_bar*alpha*c) > tol*norm_b/rho_max);
    
    _xcg_num_iters = iter;
    
    _xvec_vfree(u);
    _xvec_vfree(v);
    _xvec_vfree(w);
    _xvec_vfree(tmp);
    
    return (x);
}

/*!
   simple interface for _xspmat data structures
 */
_xvec *_xspmat_splsqr(_xspmat *A, _xvec *b, double tol, _xvec *x){
    return _xspmat_lsqr((_xvec *(*)(void *, _xvec *, _xvec *))_xspmat_mv_mlt, 
                    (_xvec *(*)(void *, _xvec *, _xvec *))_xspmat_vm_mlt, A, b, tol, x);
}

/*!
    raw lanczos algorithm -- no re-orthogonalisation
  -- creates T matrix of size == m,
    but no larger than before beta_k == 0
  -- uses passed routine to do matrix-vector multiplies
 */
void _xspmat_lanczos(_xvec *(*A_fn)(void *, _xvec *, _xvec *), void *A_params, int m, _xvec *x0, _xvec *a, _xvec *b, _xreal *beta2, _xmat *Q){
    int j;
    _xvec *v, *w, *tmp;
    _xreal alpha, beta;
    
    if (!A_fn || !x0 || !a || !b)
        _xerror(_XE_NULL,"_xspmat_lanczos");
    if (m <= 0)
        _xerror(_XE_BOUNDS,"_xspmat_lanczos");
    if (Q && (Q->m < x0->dim || Q->n < (unsigned int)m))
        _xerror(_XE_SIZES,"_xspmat_lanczos");
    
    a = _xvec_resize(a,(unsigned int)m);
    b = _xvec_resize(b,(unsigned int)(m-1));
    v = _xvec_get(x0->dim);
    w = _xvec_get(x0->dim);
    tmp = _xvec_get(x0->dim);
    
    beta = 1.0;
    /* normalise x0 as w */
    _xvec_sv_mlt(1.0/_xvec_vnorm2(x0),x0,w);
    
    (*A_fn)(A_params,w,v);
    
    for (j = 0; j < m; j++){
        /* store w in Q if Q not NULL */
        if (Q)
            _xmat_mset_col(Q,j,w);
        
        alpha = (_xreal)_xvec_vin_prod(w,v);
        a->ve[j] = alpha;
        _xvec_mltadd(v,w,-alpha,v);
        beta = (_xreal)_xvec_vnorm2(v);
        if (beta == 0.0){
            _xvec_resize(a,(unsigned int)j+1);
            _xvec_resize(b,(unsigned int)j);
            *beta2 = 0.0;
            if (Q)
                Q = _xmat_resize(Q,Q->m,j+1);
            return;
        }
        if (j < m-1)
            b->ve[j] = beta;
        _xvec_vcopy(w,tmp);
        _xvec_sv_mlt(1/beta,v,w);
        _xvec_sv_mlt(-beta,tmp,v);
        (*A_fn)(A_params,w,tmp);
        _xvec_add(v,tmp,v);
    }
    *beta2 = beta;
    
    _xvec_vfree(v); _xvec_vfree(w); _xvec_vfree(tmp);
}

/*!
   version that uses sparse matrix data structure
 */
void _xspmat_splanczos(_xspmat *A, int m, _xvec *x0, _xvec *a, _xvec *b, _xreal *beta2, _xmat *Q){
    _xspmat_lanczos((_xvec *(*)(void *, _xvec *, _xvec *))_xspmat_mv_mlt,A,m,x0,a,b,beta2,Q);
}

/*
   return the product of a long list of numbers
  -- answer stored in mant (mantissa) and expt (exponent) 
 */
static double _xproduct(_xvec *a, double offset, int *expt){
    _xreal mant, tmp_fctr;
    int i, tmp_expt;
    
    if (!a)
        _xerror(_XE_NULL,"_xproduct");
    
    mant = 1.0;
    *expt = 0;
    if (offset == 0.0)
        for (i = 0; (unsigned int)i < a->dim; i++){
            mant *= (_xreal)frexp(a->ve[i],&tmp_expt);
            *expt += tmp_expt;
            if (!(i % 10)){
                mant = (_xreal)frexp(mant,&tmp_expt);
                *expt += tmp_expt;
            }
        }
    else
        for (i = 0; (unsigned int)i < a->dim; i++){
            tmp_fctr = (_xreal)(a->ve[i] - offset);
            tmp_fctr += (_xreal)((tmp_fctr > 0.0 ) ? -_XMACHEPS*offset : _XMACHEPS*offset);
            mant *= (_xreal)frexp(tmp_fctr,&tmp_expt);
            *expt += tmp_expt;
            if (!(i % 10)){
                mant = (_xreal)frexp(mant,&tmp_expt);
                *expt += tmp_expt;
            }
        }
    
    mant = (_xreal)frexp(mant,&tmp_expt);
    *expt += tmp_expt;
    
    return (mant);
}

/*
   return the product of a long list of numbers
  -- answer stored in mant (mantissa) and expt (exponent)
 */
static double _xproduct2(_xvec *a, int k, int *expt){
    _xreal mant, mu, tmp_fctr;
    int i, tmp_expt;
    
    if (!a)
        _xerror(_XE_NULL,"_xproduct2");
    if (k < 0 || (unsigned int)k >= a->dim)
        _xerror(_XE_BOUNDS,"_xproduct2");
    
    mant = 1.0;
    *expt = 0;
    mu = a->ve[k];
    for (i = 0; (unsigned int)i < a->dim; i++){
        if (i == k)
            continue;
        tmp_fctr = a->ve[i] - mu;
        tmp_fctr += (_xreal)(( tmp_fctr > 0.0 ) ? -_XMACHEPS*mu : _XMACHEPS*mu);
        mant *= (_xreal)frexp(tmp_fctr,&tmp_expt);
        *expt += tmp_expt;
        if (!(i % 10)){
            mant = (_xreal)frexp(mant,&tmp_expt);
            *expt += tmp_expt;
        }
    }
    mant = (_xreal)frexp(mant,&tmp_expt);
    *expt += tmp_expt;
    
    return (mant);
}

/*
  comparison function to pass to qsort()
 */
static int _xdbl_cmp(_xreal *x, _xreal *y){
    _xreal tmp;
    tmp = *x - *y;
    return (tmp > 0 ? 1 : tmp < 0 ? -1: 0);
}

/*!
    lanczos + error estimate for every e-val
  -- uses Cullum & Willoughby approach, Sparse Matrix Proc. 1978
  -- return multiple e-vals where multiple e-vals may not exist
  -- return evals vector
  -- x0 initial vector
  -- evals eigenvalue vector
  -- err_est error estimates of eigenvalues
 */
_xvec *_xspmat_lanczos2(_xvec *(*A_fn)(void *, _xvec *, _xvec *), void *A_params, int m, _xvec *x0, _xvec *evals, _xvec *err_est){
    _xvec *a;
    _XSTATIC _xvec *b=(_xvec *)NULL, *a2=(_xvec *)NULL, *b2=(_xvec *)NULL;
    _xreal beta, pb_mant, det_mant, det_mant1, det_mant2;
    int i, pb_expt, det_expt, det_expt1, det_expt2;
    
    if (!A_fn || !x0)
        _xerror(_XE_NULL,"_xspmat_lanczos2");
    if ( m <= 0 )
        _xerror(_XE_RANGE,"_xspmat_lanczos2");
    
    a = evals;
    a = _xvec_resize(a,(unsigned int)m);
    b = _xvec_resize(b,(unsigned int)(m-1));
    _XMEM_STAT_REG(b,_XTYPE_VEC);
    
    _xspmat_lanczos(A_fn,A_params,m,x0,a,b,&beta,(_xmat *)NULL);
    pb_mant = 0.0;
    if (err_est){
        pb_mant = (_xreal)_xproduct(b,(double)0.0,&pb_expt);
    }
    a2 = _xvec_resize(a2,a->dim - 1);
    b2 = _xvec_resize(b2,b->dim - 1);
    _XMEM_STAT_REG(a2,_XTYPE_VEC);
    _XMEM_STAT_REG(b2,_XTYPE_VEC);
    for (i = 0; (unsigned int)i < a2->dim - 1; i++){
        a2->ve[i] = a->ve[i+1];
        b2->ve[i] = b->ve[i+1];
    }
    a2->ve[a2->dim-1] = a->ve[a2->dim];
    _xmat_trieig(a,b,(_xmat *)NULL);
    
    /* sort evals as a courtesy */
    qsort((void *)(a->ve),(int)(a->dim),sizeof(_xreal),(int (*)(const void *, const void *))_xdbl_cmp);
    
    /* error estimates */
    if (err_est){
        err_est = _xvec_resize(err_est,(unsigned int)m);
        _xmat_trieig(a2,b2,(_xmat *)NULL);
        for (i = 0; (unsigned int)i < a->dim; i++){
            det_mant1 = (_xreal)_xproduct2(a,i,&det_expt1);
            det_mant2 = (_xreal)_xproduct(a2,(double)a->ve[i],&det_expt2);
            
            if (det_mant1 == 0.0){ /* multiple e-val of T */
                err_est->ve[i] = 0.0;
                continue;
            }
            else if (det_mant2 == 0.0){
                err_est->ve[i] = (_xreal)_XHUGE_VAL;
                continue;
            }
            if ((det_expt1 + det_expt2) % 2) /* if odd... */
                det_mant = (_xreal)sqrt(2.0*fabs(det_mant1*det_mant2));
            else /* if even... */
                det_mant = (_xreal)sqrt(fabs(det_mant1*det_mant2));
            det_expt = (det_expt1+det_expt2)/2;
            err_est->ve[i] = (_xreal)fabs(beta*ldexp(pb_mant/det_mant,pb_expt-det_expt));
        }
    }

#ifdef _XTHREAD_SAFE
    _xvec_vfree(b); _xvec_vfree(a2); _xvec_vfree(b2);
#endif

    return (a);
}

/*!
   version of _xspmat_lanczos2() that uses sparse matrix data structure 
 */
_xvec *_xspmat_splanczos2(_xspmat *A,int m, _xvec *x0, _xvec *evals, _xvec *err_est){
    return _xspmat_lanczos2((_xvec *(*)(void *, _xvec *, _xvec *))_xspmat_mv_mlt,A,m,x0,evals,err_est);
}

/*!
   standard information
 */
void _xiter_std_info(const _xiter *ip, double nres, _xvec *res, _xvec *Bres){
    if (nres >= 0.0)
        (void) fprintf(stdout, " %d. residual = %g\n",ip->steps,nres);
    else 
        (void) fprintf(stdout, " %d. residual = %g (WARNING !!! should be >= 0) \n",
                       ip->steps,nres);
}

/*!
   standard stopping criterion
 */
int _xiter_std_stop_crit(const _xiter *ip, double nres, _xvec *res, _xvec *Bres){
    /* standard stopping criterium */
    return (nres <= ip->init_res*ip->eps) ? _XTRUE : _XFALSE;
}

/*!
   create a new structure pointing to _xiter
 */
_xiter *_xiter_get(int lenb, int lenx){
    _xiter *ip;

    if ((ip = _xnew(_xiter)) == (_xiter *) NULL)
        _xerror(_XE_MEM,"_xiter_get");
    else if (_xmem_info_is_on()){
        _xmem_bytes(_XTYPE_ITER,0,sizeof(_xiter));
        _xmem_numvar(_XTYPE_ITER,1);
    }

    /* default values */
    ip->shared_x = _XFALSE;
    ip->shared_b = _XFALSE;
    ip->k = 0;
    ip->limit = _XITER_LIMIT_DEF;
    ip->eps = (_xreal)_XITER_EPS_DEF;
    ip->steps = 0;

    if (lenb > 0) ip->b = _xvec_get(lenb);
    else ip->b = (_xvec *)NULL;

    if (lenx > 0) ip->x = _xvec_get(lenx);
    else ip->x = (_xvec *)NULL;

    ip->ax = (_xfun_ax) NULL;
    ip->a_par = NULL; 
    ip->atx = (_xfun_ax) NULL;
    ip->at_par = NULL;
    ip->bx = (_xfun_ax) NULL;
    ip->b_par = NULL;
    ip->info = (_xfun_info)_xiter_std_info;
    ip->stop_crit = (_xfun_stop_crit)_xiter_std_stop_crit;
    ip->init_res = 0.0;
   
    return (ip);
}

/*!
   resize a structure pointing to _xiter
 */
_xiter *_xiter_resize(_xiter *ip, int new_lenb, int new_lenx){
    _xvec *old;

    if (!ip)
        _xerror(_XE_NULL,"_xiter_resize");

    old = ip->x;
    ip->x = _xvec_resize(ip->x,new_lenx);
    if (ip->shared_x && old != ip->x)
        _xwarning(_XWARN_SHARED_VEC,"_xiter_resize");
    old = ip->b;
    ip->b = _xvec_resize(ip->b,new_lenb);
    if (ip->shared_b && old != ip->b)
        _xwarning(_XWARN_SHARED_VEC,"_xiter_resize");

    return (ip);
}

/*!
   print out ip structure - for diagnostic purposes mainly
 */
void _xiter_dump(FILE *fp, _xiter *ip){
    if (!ip){
        (void) fprintf(fp," _xiter structure: NULL\n");
        return;
    }

    (void) fprintf(fp,"\n _xiter structure:\n");
    (void) fprintf(fp," ip->shared_x = %s, ip->shared_b = %s\n",
                  (ip->shared_x ? "TRUE" : "FALSE"),
                  (ip->shared_b ? "TRUE" : "FALSE") );
    (void) fprintf(fp," ip->k = %d, ip->limit = %d, ip->steps = %d, ip->eps = %g\n",
                 ip->k,ip->limit,ip->steps,ip->eps);
    (void) fprintf(fp," ip->x = 0x%p, ip->b = 0x%p\n",ip->x,ip->b);
    (void) fprintf(fp," ip->Ax = 0x%p, ip->A_par = 0x%p\n",ip->ax,ip->a_par);
    (void) fprintf(fp," ip->ATx = 0x%p, ip->AT_par = 0x%p\n",ip->atx,ip->at_par);
    (void) fprintf(fp," ip->Bx = 0x%p, ip->B_par = 0x%p\n",ip->bx,ip->b_par);
    (void) fprintf(fp," ip->info = 0x%p, ip->stop_crit = 0x%p, ip->init_res = %g\n",
     ip->info,ip->stop_crit,ip->init_res);
    (void) fprintf(fp,"\n");
}

/*!
   copy the structure ip1 to ip2 preserving vectors x and b of ip2
   (vectors x and b in ip2 are the same before and after _xiter_copy2)
   if ip2 == NULL then a new structure is created with x and b being NULL
   and other members are taken from ip1
 */
_xiter *_xiter_copy2(_xiter *ip1, _xiter *ip2){
    _xvec *x, *b;
    int shx, shb;

    if (!ip1) 
        _xerror(_XE_NULL,"_xiter_copy2");

    if (!ip2){
        if ((ip2 = _xnew(_xiter)) == (_xiter *)NULL)
            _xerror(_XE_MEM,"_xiter_copy2");
        else if (_xmem_info_is_on()){
            _xmem_bytes(_XTYPE_ITER,0,sizeof(_xiter));
            _xmem_numvar(_XTYPE_ITER,1);
        }
        ip2->x = ip2->b = NULL;
        ip2->shared_x = ip2->shared_x = _XFALSE;
    }

    x = ip2->x;
    b = ip2->b;
    shb = ip2->shared_b;
    shx = ip2->shared_x;
    _xmem_copy(ip1,ip2,sizeof(_xiter));
    ip2->x = x;
    ip2->b = b;
    ip2->shared_x = shx;
    ip2->shared_b = shb;

    return (ip2);
}

/*!
   copy the structure ip1 to ip2 copying also the vectors x and b
 */
_xiter *_xiter_copy(const _xiter *ip1, _xiter *ip2){
    _xvec *x, *b;

    if (!ip1) 
        _xerror(_XE_NULL,"_xiter_copy");

    if (!ip2){
        if ((ip2 = _xnew(_xiter)) == (_xiter *) NULL)
            _xerror(_XE_MEM,"_xiter_copy");
        else if (_xmem_info_is_on()){
            _xmem_bytes(_XTYPE_ITER,0,sizeof(_xiter));
            _xmem_numvar(_XTYPE_ITER,1);
        }
    }

    x = ip2->x;
    b = ip2->b;

    _xmem_copy(ip1,ip2,sizeof(_xiter));
    if (ip1->x)
        ip2->x = _xvec_vcopy(ip1->x,x);
    if (ip1->b)
        ip2->b = _xvec_vcopy(ip1->b,b);

    ip2->shared_x = ip2->shared_b = _XFALSE;

    return (ip2);
}

/*!
   generate symmetric positive definite n x n matrix, 
   nrow - number of nonzero entries in a row
 */
_xspmat *_xiter_gen_sym(int n, int nrow){
    _xspmat *A;
    _xvec *u;
    _xreal s1;
    int i, j, k, k_max;
   
    if (nrow <= 1) nrow = 2;
    /* nrow should be even */
    if ((nrow & 1)) nrow -= 1;
    A = _xspmat_get(n,n,nrow);
    u = _xvec_get(A->m);
    _xvec_zero(u);
    for (i = 0; i < A->m; i++){
        k_max = ((rand() >> 8) % (nrow/2));
        for (k = 0; k <= k_max; k++){
            j = (rand() >> 8) % A->n;
            s1 = (_xreal)_xmrand();
            _xspmat_set_val(A,i,j,s1);
            _xspmat_set_val(A,j,i,s1);
            u->ve[i] += (_xreal)fabs(s1);
            u->ve[j] += (_xreal)fabs(s1);
        }
    }
    /* ensure that A is positive definite */
    for (i = 0; i < A->m; i++)
        _xspmat_set_val(A,i,i,u->ve[i] + 1.0);
   
    _xvec_vfree(u);
    return (A);
}

/*!
   generate non-symmetric m x n sparse matrix, m >= n 
   nrow - number of entries in a row;
   diag - number which is put in diagonal entries and then permuted
   (if diag is zero then 1.0 is there)
 */
_xspmat *_xiter_gen_nonsym(int m, int n, int nrow, double diag){
    _xspmat *A;
    _xperm *px;
    int i, j, k, k_max;
    _xreal s1;
   
    if (nrow <= 1) nrow = 2;
    if (diag == 0.0) diag = 1.0;
    A = _xspmat_get(m,n,nrow);
    px = _xperm_get(n);
    for (i = 0; i < A->m; i++){
        k_max = (rand() >> 8) % (nrow-1);
        for (k = 0; k <= k_max; k++){
            j = (rand() >> 8) % A->n;
            s1 = (_xreal)_xmrand();
            _xspmat_set_val(A,i,j,-s1);
        }
    }
    /* to make it likely that A is nonsingular, use pivot... */
    for (i = 0; i < 2*A->n; i++){
        j = (rand() >> 8) % A->n;
        k = (rand() >> 8) % A->n;
        _xperm_transp(px,j,k);
    }
    for (i = 0; i < A->n; i++)
        _xspmat_set_val(A,i,px->pe[i],diag);  
   
    _xperm_pfree(px);
    return (A);
}

/*!
   generate non-symmetric positive definite n x n sparse matrix;
   nrow - number of entries in a row
 */
_xspmat *_xiter_gen_nonsym_posdef(int n, int nrow){
    _xspmat *A;
    _xperm *px;
    _xvec *u;
    int i, j, k, k_max;
    _xreal s1;
   
    if (nrow <= 1) nrow = 2;
    A = _xspmat_get(n,n,nrow);
    px = _xperm_get(n);
    u = _xvec_get(A->m);
    _xvec_zero(u);
    for (i = 0; i < A->m; i++){
        k_max = (rand() >> 8) % (nrow-1);
        for (k = 0; k <= k_max; k++){
            j = (rand() >> 8) % A->n;
            s1 = (_xreal)_xmrand();
            _xspmat_set_val(A,i,j,-s1);
            u->ve[i] += (_xreal)fabs(s1);
        }
    }
    /* ensure that A is positive definite */
    for (i = 0; i < A->m; i++)
        _xspmat_set_val(A,i,i,u->ve[i] + 1.0);
   
    _xperm_pfree(px);
    _xvec_vfree(u);
   
    return (A);
}

/*!
   Conjugate gradients method
 */
_xvec *_xiter_cg(_xiter *ip){
    _XSTATIC _xvec *r=(_xvec *)NULL, *p=(_xvec *)NULL, *q=(_xvec *)NULL, *z=(_xvec *)NULL;
    _xreal alpha, beta, inner, old_inner, nres;
    _xvec *rr;
   
    if (!ip)
        _xerror(_XE_NULL,"_xiter_cg");
    if (!ip->ax || !ip->b)
        _xerror(_XE_NULL,"_xiter_cg");
    if (ip->x == ip->b)
        _xerror(_XE_INSITU,"_xiter_cg");
    if (!ip->stop_crit)
        _xerror(_XE_NULL,"_xiter_cg");
   
    if (ip->eps <= 0.0)
        ip->eps = (_xreal)_XMACHEPS;
   
    r = _xvec_resize(r,ip->b->dim);
    p = _xvec_resize(p,ip->b->dim);
    q = _xvec_resize(q,ip->b->dim);
   
    _XMEM_STAT_REG(r,_XTYPE_VEC);
    _XMEM_STAT_REG(p,_XTYPE_VEC);
    _XMEM_STAT_REG(q,_XTYPE_VEC);
   
    if (ip->bx != (_xfun_ax)NULL){
        z = _xvec_resize(z,ip->b->dim);
        _XMEM_STAT_REG(z,_XTYPE_VEC);
        rr = z;
    }
    else 
    rr = r;
   
    if (ip->x){
        if (ip->x->dim != ip->b->dim)
            _xerror(_XE_SIZES,"_xiter_cg");
        ip->ax(ip->a_par,ip->x,p); /* p = A*x */
        _xvec_sub(ip->b,p,r);      /* r = b - A*x */
    }
    else { /* ip->x == 0 */
        ip->x = _xvec_get(ip->b->dim);
        ip->shared_x = _XFALSE;
        _xvec_vcopy(ip->b,r);
    }
   
    old_inner = 0.0;
    for (ip->steps = 0; ip->steps <= ip->limit; ip->steps++){
        if (ip->bx)
            (ip->bx)(ip->b_par,r,rr); /* rr = B*r */
      
        inner = (_xreal)_xvec_vin_prod(rr,r);
        nres = (_xreal)sqrt(fabs(inner));
        if (ip->info) ip->info(ip,nres,r,rr);
        if (ip->steps == 0) ip->init_res = nres;
        if (ip->stop_crit(ip,nres,r,rr)) break;
      
        if (ip->steps){
            beta = inner/old_inner;
            p = _xvec_mltadd(rr,p,beta,p);
        }
        else {
            beta = 0.0;
            p = _xvec_vcopy(rr,p);
            old_inner = 0.0;
        }
        (ip->ax)(ip->a_par,p,q);     /* q = A*p */
        alpha = (_xreal)_xvec_vin_prod(p,q);
        if (sqrt(fabs(alpha)) <= _XMACHEPS*ip->init_res) 
            _xerror(_XE_BREAKDOWN,"_xiter_cg");
        alpha = inner/alpha;
        _xvec_mltadd(ip->x,p,alpha,ip->x);
        _xvec_mltadd(r,q,-alpha,r);
        old_inner = inner;
    }

#ifdef _XTHREAD_SAFE
    _xvec_vfree(r); _xvec_vfree(p); _xvec_vfree(q); _xvec_vfree(z);
#endif

    return (ip->x);
}

/*!
   a simple interface to _xiter_cg() which uses sparse matrix
   data structures
   -- assumes that LLT contains the Cholesky factorisation of the
   actual preconditioner;
   use always as follows:
   x = _xiter_spcg(A,LLT,b,eps,x,limit,steps);
   or 
   x = _xiter_spcg(A,LLT,b,eps,VNULL,limit,steps);
   In the second case the solution vector is created.
 */
_xvec *_xiter_spcg(_xspmat *A, _xspmat *LLT, _xvec *b, double eps, _xvec *x, int limit, int *steps){
    _xiter *ip;
   
    ip = _xiter_get(0,0);
    ip->ax = (_xfun_ax)_xspmat_mv_mlt;
    ip->a_par = (void *)A;
    ip->bx = (_xfun_ax)_xspmat_chsolve;
    ip->b_par = (void *)LLT;
    ip->info = (_xfun_info)NULL;
    ip->b = b;
    ip->eps = (_xreal)eps;
    ip->limit = limit;
    ip->x = x;
    _xiter_cg(ip);
    x = ip->x;
    if (steps) *steps = ip->steps;
    ip->shared_x = ip->shared_b = _XTRUE;
    _xiter_free(ip);   /* release only _xiter structure */
    return (x);   
}

/*!
   raw lanczos algorithm -- no re-orthogonalisation
   -- creates T matrix of size == m,
   but no larger than before beta_k == 0
   -- uses passed routine to do matrix-vector multiplies
 */
void  _xiter_lanczos(_xiter *ip, _xvec *a, _xvec *b, _xreal *beta2, _xmat *Q){
    int j;
    _XSTATIC _xvec *v = (_xvec *)NULL, *w = (_xvec *)NULL, *tmp = (_xvec *)NULL;
    _xreal alpha, beta, c;
   
    if (!ip)
        _xerror(_XE_NULL,"_xiter_lanczos");
    if (!ip->ax || !ip->x || !a || !b)
        _xerror(_XE_NULL,"_xiter_lanczos");
    if (ip->k <= 0)
        _xerror(_XE_BOUNDS,"_xiter_lanczos");
    if (Q && (Q->n < ip->x->dim || Q->m < ip->k))
        _xerror(_XE_SIZES,"_xiter_lanczos");
   
    a = _xvec_resize(a,(unsigned int)ip->k);  
    b = _xvec_resize(b,(unsigned int)(ip->k-1));
    v = _xvec_resize(v,ip->x->dim);
    w = _xvec_resize(w,ip->x->dim);
    tmp = _xvec_resize(tmp,ip->x->dim);
    _XMEM_STAT_REG(v,_XTYPE_VEC);
    _XMEM_STAT_REG(w,_XTYPE_VEC);
    _XMEM_STAT_REG(tmp,_XTYPE_VEC);
   
    beta = 1.0;
    _xvec_zero(a);
    _xvec_zero(b);
    if (Q) _xmat_zero(Q);
   
    /* normalise x as w */
    c = (_xreal)_xvec_vnorm2(ip->x);
    if (c <= _XMACHEPS){ /* ip->x == 0 */
        *beta2 = 0.0;
        return;
    }
    else 
        _xvec_sv_mlt(1.0/c,ip->x,w);
   
    (ip->ax)(ip->a_par,w,v);
    for (j = 0; (unsigned int)j < ip->k; j++){
        /* store w in Q if Q not NULL */
        if (Q) _xmat_mset_row(Q,j,w);
      
        alpha = (_xreal)_xvec_vin_prod(w,v);
        a->ve[j] = alpha;
        _xvec_mltadd(v,w,-alpha,v);
        beta = (_xreal)_xvec_vnorm2(v);
        if (beta == 0.0){
            *beta2 = 0.0;
            return;
        }
      
        if ((unsigned int)j < ip->k-1)
            b->ve[j] = beta;
        _xvec_vcopy(w,tmp);
        _xvec_sv_mlt(1/beta,v,w);
        _xvec_sv_mlt(-beta,tmp,v);
        (ip->ax)(ip->a_par,w,tmp);
        _xvec_add(v,tmp,v);
    }
    *beta2 = beta;

#ifdef _XTHREAD_SAFE
    _xvec_vfree(v); _xvec_vfree(w); _xvec_vfree(tmp);
#endif
}

/*!
   version that uses sparse matrix data structure
 */
void  _xiter_splanczos(_xspmat *A, int m, _xvec *x0, _xvec *a, _xvec *b, _xreal *beta2, _xmat *Q){
    _xiter *ip;
   
    ip = _xiter_get(0,0);
    ip->shared_x = ip->shared_b = _XTRUE;
    ip->ax = (_xfun_ax)_xspmat_mv_mlt;
    ip->a_par = (void *) A;
    ip->x = x0;
    ip->k = m;
    _xiter_lanczos(ip,a,b,beta2,Q); 
    _xiter_free(ip);   /* release only _xiter structure */
}

/*!
   lanczos + error estimate for every e-val
   -- uses Cullum & Willoughby approach, Sparse Matrix Proc. 1978
   -- returns multiple e-vals where multiple e-vals may not exist
   -- returns evals vector
 */
_xvec *_xiter_lanczos2(_xiter *ip, _xvec *evals, _xvec *err_est){
    _XSTATIC _xvec *b=(_xvec *)NULL, *a2=(_xvec *)NULL, *b2=(_xvec *)NULL;
    _xvec *a;
    _xreal beta, pb_mant, det_mant, det_mant1, det_mant2;
    int i, pb_expt, det_expt, det_expt1, det_expt2;
   
    if (!ip)
        _xerror(_XE_NULL,"_xiter_lanczos2");
    if (!ip->ax || !ip->x)
        _xerror(_XE_NULL,"_xiter_lanczos2");
    if (ip->k <= 0)
        _xerror(_XE_RANGE,"_xiter_lanczos2");
   
    a = evals;
    a = _xvec_resize(a,(unsigned int)ip->k);
    b = _xvec_resize(b,(unsigned int)(ip->k-1));
    _XMEM_STAT_REG(b,_XTYPE_VEC);
   
    _xiter_lanczos(ip,a,b,&beta,(_xmat *)NULL);
   
    pb_mant = 0.0;
    if (err_est){
        pb_mant = (_xreal)_xproduct(b,(double)0.0,&pb_expt);
    }
   
    a2 = _xvec_resize(a2,a->dim - 1);
    b2 = _xvec_resize(b2,b->dim - 1);
    _XMEM_STAT_REG(a2,_XTYPE_VEC);
    _XMEM_STAT_REG(b2,_XTYPE_VEC);
    for (i = 0; (unsigned int)i < a2->dim - 1; i++){
        a2->ve[i] = a->ve[i+1];
        b2->ve[i] = b->ve[i+1];
    }
    a2->ve[a2->dim-1] = a->ve[a2->dim];
   
    _xmat_trieig(a,b,(_xmat *)NULL);
    /* sort evals as a courtesy */
    qsort((void *)(a->ve),(int)(a->dim),sizeof(_xreal),(int (*)(const void *, const void *))_xdbl_cmp);
    /* error estimates */
    if (err_est){
        err_est = _xvec_resize(err_est,(unsigned int)ip->k);
        _xmat_trieig(a2,b2,(_xmat *)NULL);
      
        for (i = 0; (unsigned int)i < a->dim; i++){
            det_mant1 = (_xreal)_xproduct2(a,i,&det_expt1);
            det_mant2 = (_xreal)_xproduct(a2,(double)a->ve[i],&det_expt2);
            if (det_mant1 == 0.0){ /* multiple e-val of T */
                err_est->ve[i] = 0.0;
                continue;
            }     
            else if (det_mant2 == 0.0){
                err_est->ve[i] = (_xreal)_XHUGE_VAL;
                continue;
            }     
            if ((det_expt1 + det_expt2) % 2) /* if odd... */
                det_mant = (_xreal)sqrt(2.0*fabs(det_mant1*det_mant2));
            else /* if even... */
                det_mant = (_xreal)sqrt(fabs(det_mant1*det_mant2));
            det_expt = (det_expt1+det_expt2)/2;
            err_est->ve[i] = (_xreal)fabs(beta*ldexp(pb_mant/det_mant,pb_expt-det_expt));
        }
    }

#ifdef _XTHREAD_SAFE
    _xvec_vfree(b); _xvec_vfree(a2); _xvec_vfree(b2);
#endif

    return (a);
}

/*!
   version of _xiter_lanczos2() that uses sparse matrix data structure 
 */
_xvec *_xiter_splanczos2(_xspmat *A, int m, _xvec *x0, _xvec *evals, _xvec *err_est){
    _xiter *ip;
    _xvec *a;
   
    ip = _xiter_get(0,0);
    ip->ax = (_xfun_ax)_xspmat_mv_mlt;
    ip->a_par = (void *) A;
    ip->x = x0;
    ip->k = m;
    a = _xiter_lanczos2(ip,evals,err_est);  
    ip->shared_x = ip->shared_b = _XTRUE;
    _xiter_free(ip);   /* release only _xiter structure */
    
    return (a);
}

/*!
   Conjugate gradient method, another variant - mainly for testing
 */
_xvec *_xiter_cg1(_xiter *ip){
    _XSTATIC _xvec *r = (_xvec *)NULL, *p = (_xvec *)NULL, *q = (_xvec *)NULL, *z = (_xvec *)NULL;
    _xreal alpha;
    double inner, nres;
    _xvec *rr;   /* rr == r or rr == z */
   
    if (!ip)
        _xerror(_XE_NULL,"_xiter_cg1");
    if (!ip->ax || !ip->b)
        _xerror(_XE_NULL,"_xiter_cg1");
    if (ip->x == ip->b)
        _xerror(_XE_INSITU,"_xiter_cg1");
    if (!ip->stop_crit)
        _xerror(_XE_NULL,"_xiter_cg1");
   
    if (ip->eps <= 0.0)
        ip->eps = (_xreal)_XMACHEPS;
   
    r = _xvec_resize(r,ip->b->dim);
    p = _xvec_resize(p,ip->b->dim);
    q = _xvec_resize(q,ip->b->dim);
   
    _XMEM_STAT_REG(r,_XTYPE_VEC);
    _XMEM_STAT_REG(p,_XTYPE_VEC);
    _XMEM_STAT_REG(q,_XTYPE_VEC);
   
    if (ip->bx != (_xfun_ax)NULL) {
        z = _xvec_resize(z,ip->b->dim);
        _XMEM_STAT_REG(z,_XTYPE_VEC);
        rr = z;
    }
    else 
        rr = r;
   
    if (ip->x){
        if (ip->x->dim != ip->b->dim)
            _xerror(_XE_SIZES,"_xiter_cg1");
        ip->ax(ip->a_par,ip->x,p);   /* p = A*x */
        _xvec_sub(ip->b,p,r);    /* r = b - A*x */
    }
    else {  /* ip->x == 0 */
        ip->x = _xvec_get(ip->b->dim);
        ip->shared_x = _XFALSE;
        _xvec_vcopy(ip->b,r);
    }
   
    if (ip->bx) (ip->bx)(ip->b_par,r,p);
    else _xvec_vcopy(r,p);
   
    inner = _xvec_vin_prod(p,r);
    nres = sqrt(fabs(inner));
    if (ip->info) ip->info(ip,nres,r,p);
    if (nres == 0.0) return ip->x;
   
    for (ip->steps = 0; ip->steps <= ip->limit; ip->steps++){
        ip->ax(ip->a_par,p,q);
        inner = _xvec_vin_prod(q,p);
        if (sqrt(fabs(inner)) <= _XMACHEPS*ip->init_res)
            _xerror(_XE_BREAKDOWN,"_xiter_cg1");

        alpha = (_xreal)(_xvec_vin_prod(p,r)/inner);
        _xvec_mltadd(ip->x,p,alpha,ip->x);
        _xvec_mltadd(r,q,-alpha,r);
      
        rr = r;
        if (ip->bx){
            ip->bx(ip->b_par,r,z);
            rr = z;
        }
      
        nres = _xvec_vin_prod(r,rr);
        if (nres < 0.0){
            _xwarning(_XWARN_RES_LESS_0,"_xiter_cg1");
            break;
        }
        nres = sqrt(fabs(nres));
        if (ip->info) ip->info(ip,nres,r,z);
        if (ip->steps == 0) ip->init_res = (_xreal)nres;
        if (ip->stop_crit(ip,nres,r,z)) break;
      
        alpha = (_xreal)(-_xvec_vin_prod(rr,q)/inner);
        _xvec_mltadd(rr,p,alpha,p);
    }

#ifdef _XTHREAD_SAFE
    _xvec_vfree(r); _xvec_vfree(p); _xvec_vfree(q); _xvec_vfree(z);
#endif

    return (ip->x);
}

/*!
   uses CGS to compute a solution x to A.x=b
 */
_xvec *_xiter_cgs(_xiter *ip, _xvec *r0){
    _XSTATIC _xvec *p = (_xvec *)NULL, *q = (_xvec *)NULL, *r = (_xvec *)NULL, *u = (_xvec *)NULL;
    _XSTATIC _xvec *v = (_xvec *)NULL, *z = (_xvec *)NULL;
    _xvec *tmp;
    _xreal alpha, beta, nres, rho, old_rho, sigma, inner;

    if (!ip)
        _xerror(_XE_NULL,"_xiter_cgs");
    if (!ip->ax || !ip->b || !r0)
        _xerror(_XE_NULL,"_xiter_cgs");
    if (ip->x == ip->b)
        _xerror(_XE_INSITU,"_xiter_cgs");
    if (!ip->stop_crit)
        _xerror(_XE_NULL,"_xiter_cgs");
    if (r0->dim != ip->b->dim)
        _xerror(_XE_SIZES,"_xiter_cgs");
   
    if (ip->eps <= 0.0) ip->eps = (_xreal)_XMACHEPS;
   
    p = _xvec_resize(p,ip->b->dim);
    q = _xvec_resize(q,ip->b->dim);
    r = _xvec_resize(r,ip->b->dim);
    u = _xvec_resize(u,ip->b->dim);
    v = _xvec_resize(v,ip->b->dim);

    _XMEM_STAT_REG(p,_XTYPE_VEC);
    _XMEM_STAT_REG(q,_XTYPE_VEC);
    _XMEM_STAT_REG(r,_XTYPE_VEC);
    _XMEM_STAT_REG(u,_XTYPE_VEC);
    _XMEM_STAT_REG(v,_XTYPE_VEC);

    if (ip->bx){
        z = _xvec_resize(z,ip->b->dim);
        _XMEM_STAT_REG(z,_XTYPE_VEC); 
    }

    if (ip->x){
        if (ip->x->dim != ip->b->dim)
            _xerror(_XE_SIZES,"_xiter_cgs");
        ip->ax(ip->a_par,ip->x,v);        /* v = A*x */
        if (ip->bx){
            _xvec_sub(ip->b,v,v);     /* v = b - A*x */
            (ip->bx)(ip->b_par,v,r);    /* r = B*(b-A*x) */
        }
        else _xvec_sub(ip->b,v,r);      /* r = b-A*x */
    }
    else {  /* ip->x == 0 */
        ip->x = _xvec_get(ip->b->dim);    /* x == 0 */
        ip->shared_x = _XFALSE;
        if (ip->bx) (ip->bx)(ip->b_par,ip->b,r);  /* r = B*b */
        else _xvec_vcopy(ip->b,r);                /* r = b */
    }

    _xvec_zero(p);  
    _xvec_zero(q);
    old_rho = 1.0;
   
    for (ip->steps = 0; ip->steps <= ip->limit; ip->steps++){
        inner = (_xreal)_xvec_vin_prod(r,r);
        nres = (_xreal)sqrt(fabs(inner));
        if (ip->steps == 0) ip->init_res = nres;

        if (ip->info) ip->info(ip,nres,r,(_xvec *)NULL);
        if (ip->stop_crit(ip,nres,r,(_xvec *)NULL)) break;

        rho = (_xreal)_xvec_vin_prod(r0,r);
        if (old_rho == 0.0)
          _xerror(_XE_BREAKDOWN,"_xiter_cgs");
        beta = rho/old_rho;
        _xvec_mltadd(r,q,beta,u);
        _xvec_mltadd(q,p,beta,v);
        _xvec_mltadd(u,v,beta,p);
      
        (ip->ax)(ip->a_par,p,q);
        if (ip->bx){
            (ip->bx)(ip->b_par,q,z);
            tmp = z;
        }
        else tmp = q;
      
        sigma = (_xreal)_xvec_vin_prod(r0,tmp);
        if (sigma == 0.0)
            _xerror(_XE_BREAKDOWN,"_xiter_cgs");
        alpha = rho/sigma;
        _xvec_mltadd(u,tmp,-alpha,q);
        _xvec_add(u,q,v);
      
        (ip->ax)(ip->a_par,v,u);
        if (ip->bx){
            (ip->bx)(ip->b_par,u,z);
            tmp = z;
        }
        else tmp = u;
      
        _xvec_mltadd(r,tmp,-alpha,r);
        _xvec_mltadd(ip->x,v,alpha,ip->x);
      
        old_rho = rho;
    }

#ifdef _XTHREAD_SAFE
    _xvec_vfree(p); _xvec_vfree(q); _xvec_vfree(r); _xvec_vfree(u);
    _xvec_vfree(v); _xvec_vfree(z);
#endif

    return (ip->x);
}

/*!
   simple interface for SPMAT data structures use always as follows:
      x = _xiter_spcgs(A,B,b,r0,tol,x,limit,steps);
   or 
      x = _xiter_spcgs(A,B,b,r0,tol,(_xvec *)NULL,limit,steps);
   In the second case the solution vector is created.  
   If B is not NULL then it is a preconditioner. 
 */
_xvec *_xiter_spcgs(_xspmat *A, _xspmat *B, _xvec *b, _xvec *r0, double tol, _xvec *x, int limit, int *steps){
    _xiter *ip;
   
    ip = _xiter_get(0,0);
    ip->ax = (_xfun_ax)_xspmat_mv_mlt;
    ip->a_par = (void *)A;
    if (B){
        ip->bx = (_xfun_ax)_xspmat_mv_mlt;
        ip->b_par = (void *)B;
    }
    else {
        ip->bx = (_xfun_ax)NULL;
        ip->b_par = NULL;
    }
    ip->info = (_xfun_info)NULL;
    ip->limit = limit;
    ip->b = b;
    ip->eps = (_xreal)tol;
    ip->x = x;
    _xiter_cgs(ip,r0);
    x = ip->x;
    if (steps) *steps = ip->steps;
    ip->shared_x = ip->shared_b = _XTRUE;   
    _xiter_free(ip);   /* release only _xiter structure */

    return (x);   
}

/*!
   Routine for performing LSQR -- the least squares QR algorithm
   of Paige and Saunders:
   "LSQR: an algorithm for sparse linear equations and
   sparse least squares", ACM Trans. Math. Soft., v. 8
   pp. 43--71 (1982)

   sparse CG-like least squares routine:
   -- finds min_x ||A.x-b||_2 using A defined through A & AT
   -- returns x (if x != NULL) 
 */
_xvec *_xiter_lsqr(_xiter *ip){
    _XSTATIC _xvec *u = (_xvec *)NULL, *v = (_xvec *)NULL, *w = (_xvec *)NULL, *tmp = (_xvec *)NULL;
    _xreal alpha, beta, phi, phi_bar;
    _xreal rho, rho_bar, rho_max, theta, nres;
    _xreal s, c;  /* for Givens' rotations */
    int m, n;
   
    if (!ip || !ip->b || !ip->ax || !ip->atx)
        _xerror(_XE_NULL,"_xiter_lsqr");
    if (ip->x == ip->b)
        _xerror(_XE_INSITU,"_xiter_lsqr");
    if (!ip->stop_crit || !ip->x)
        _xerror(_XE_NULL,"_xiter_lsqr");

    if (ip->eps <= 0.0 ) ip->eps = (_xreal)_XMACHEPS;
   
    m = ip->b->dim; 
    n = ip->x->dim;

    u = _xvec_resize(u,(unsigned int)m);
    v = _xvec_resize(v,(unsigned int)n);
    w = _xvec_resize(w,(unsigned int)n);
    tmp = _xvec_resize(tmp,(unsigned int)n);

    _XMEM_STAT_REG(u,_XTYPE_VEC);
    _XMEM_STAT_REG(v,_XTYPE_VEC);
    _XMEM_STAT_REG(w,_XTYPE_VEC);
    _XMEM_STAT_REG(tmp,_XTYPE_VEC);  

    if (ip->x) {
        ip->ax(ip->a_par,ip->x,u);        /* u = A*x */
        _xvec_sub(ip->b,u,u);       /* u = b-A*x */
    }
    else {  /* ip->x == 0 */
        ip->x = _xvec_get(ip->b->dim);
        ip->shared_x = _XFALSE;
        _xvec_vcopy(ip->b,u);               /* u = b */
    }
 
    beta = (_xreal)_xvec_vnorm2(u); 
    if (beta == 0.0) return ip->x;

    _xvec_sv_mlt(1.0/beta,u,u);
    (ip->atx)(ip->at_par,u,v);
    alpha = (_xreal)_xvec_vnorm2(v);
    if (alpha == 0.0) return ip->x;

    _xvec_sv_mlt(1.0/alpha,v,v);
    _xvec_vcopy(v,w);
    phi_bar = beta;
    rho_bar = alpha;
   
    rho_max = 1.0;
    for (ip->steps = 0; ip->steps <= ip->limit; ip->steps++){
        tmp = _xvec_resize(tmp,m);
        (ip->ax)(ip->a_par,v,tmp);
      
        _xvec_mltadd(tmp,u,-alpha,u);
        beta = (_xreal)_xvec_vnorm2(u); 
        _xvec_sv_mlt(1.0/beta,u,u);
      
        tmp = _xvec_resize(tmp,n);
        (ip->atx)(ip->at_par,u,tmp);
        _xvec_mltadd(tmp,v,-beta,v);
        alpha = (_xreal)_xvec_vnorm2(v);  
        _xvec_sv_mlt(1.0/alpha,v,v);
      
        rho = (_xreal)sqrt(rho_bar*rho_bar+beta*beta);
        if (rho > rho_max)
            rho_max = rho;
        c   = rho_bar/rho;
        s   = beta/rho;
        theta   =  s*alpha;
        rho_bar = -c*alpha;
        phi     =  c*phi_bar;
        phi_bar =  s*phi_bar;
      
        /* update ip->x & w */
        if (rho == 0.0)
            _xerror(_XE_BREAKDOWN,"_xiter_lsqr");
        _xvec_mltadd(ip->x,w,phi/rho,ip->x);
        _xvec_mltadd(v,w,-theta/rho,w);

        nres = (_xreal)fabs(phi_bar*alpha*c)*rho_max;

        if (ip->info) ip->info(ip,nres,w,(_xvec *)NULL);
        if (ip->steps == 0) ip->init_res = nres;
        if ( ip->stop_crit(ip,nres,w,(_xvec *)NULL)) break;
    } 

#ifdef _XTHREAD_SAFE
    _xvec_vfree(u); _xvec_vfree(v); _xvec_vfree(w); _xvec_vfree(tmp);
#endif

    return (ip->x);
}

/*!
   simple interface for _xspmat data structures
 */
_xvec *_xiter_splsqr(_xspmat *A, _xvec *b, double tol, _xvec *x, int limit, int *steps){
    _xiter *ip;
   
    ip = _xiter_get(0,0);
    ip->ax = (_xfun_ax)_xspmat_mv_mlt;
    ip->a_par = (void *)A;
    ip->atx = (_xfun_ax)_xspmat_vm_mlt;
    ip->at_par = (void *)A;
    ip->bx = (_xfun_ax)NULL;
    ip->b_par = NULL;

    ip->info = (_xfun_info)NULL;
    ip->limit = limit;
    ip->b = b;
    ip->eps = (_xreal)tol;
    ip->x = x;
    _xiter_lsqr(ip);
    x = ip->x;
    if (steps) *steps = ip->steps;
    ip->shared_x = ip->shared_b = _XTRUE;
    _xiter_free(ip);   /* release only _xiter structure */
    
    return (x);   
}

/*!
   an implementation of the Arnoldi method, iterative refinement is applied.
 */
_xmat *_xiter_arnoldi_iref(_xiter *ip, _xreal *h_rem, _xmat *Q, _xmat *H){
    _XSTATIC _xvec *u=(_xvec *)NULL, *r=(_xvec *)NULL, *s=(_xvec *)NULL, *tmp=(_xvec *)NULL;
    _xvec v;     /* auxiliary vector */
    int i,j;
    _xreal h_val, c;
   
    if (!ip)
        _xerror(_XE_NULL,"_xiter_arnoldi_iref");
    if (!ip->ax || !Q || !ip->x)
        _xerror(_XE_NULL,"_xiter_arnoldi_iref");
    if (ip->k <= 0)
        _xerror(_XE_BOUNDS,"_xiter_arnoldi_iref");
    if (Q->n != ip->x->dim || Q->m != ip->k)
        _xerror(_XE_SIZES,"_xiter_arnoldi_iref");
   
    _xmat_zero(Q);
    H = _xmat_resize(H,ip->k,ip->k);
    _xmat_zero(H);

    u = _xvec_resize(u,ip->x->dim);
    r = _xvec_resize(r,ip->k);
    s = _xvec_resize(s,ip->k);
    tmp = _xvec_resize(tmp,ip->x->dim);
    _XMEM_STAT_REG(u,_XTYPE_VEC);
    _XMEM_STAT_REG(r,_XTYPE_VEC);
    _XMEM_STAT_REG(s,_XTYPE_VEC);
    _XMEM_STAT_REG(tmp,_XTYPE_VEC);

    v.dim = v.max_dim = ip->x->dim;

    c = (_xreal)_xvec_vnorm2(ip->x);
    if (c <= 0.0)
        return (H);
    else {
        v.ve = Q->me[0];
        _xvec_sv_mlt(1.0/c,ip->x,&v);
    }

    _xvec_zero(r);
    _xvec_zero(s);
    for (i = 0; (unsigned int)i < ip->k; i++){
        v.ve = Q->me[i];
        u = (ip->ax)(ip->a_par,&v,u);
        for (j = 0; j <= i; j++){
            v.ve = Q->me[j];
            /* modified Gram-Schmidt */
            r->ve[j] = (_xreal)_xvec_vin_prod(&v,u);
            _xvec_mltadd(u,&v,-r->ve[j],u);
        }
        h_val = (_xreal)_xvec_vnorm2(u);
        /* if u == 0 then we have an exact subspace */
        if (h_val <= 0.0){
            *h_rem = h_val;
            return (H);
        }
        /* iterative refinement -- ensures near orthogonality */
        do {
            _xvec_zero(tmp);
            for (j = 0; j <= i; j++){
                v.ve = Q->me[j];
                s->ve[j] = (_xreal)_xvec_vin_prod(&v,u);
                _xvec_mltadd(tmp,&v,s->ve[j],tmp);
            }     
            _xvec_sub(u,tmp,u);
            _xvec_add(r,s,r);
        } while (_xvec_vnorm2(s) > 0.1*(h_val = (_xreal)_xvec_vnorm2(u)));
        /* now that u is nearly orthogonal to Q, update H */
        _xmat_mset_col(H,i,r);
        /* check once again if h_val is zero */
        if (h_val <= 0.0){
            *h_rem = h_val;
            return (H);
        }
        if (i == ip->k-1){
            *h_rem = h_val;
            continue;
        }
        _xmat_set_val(H,i+1,i,h_val);
        v.ve = Q->me[i+1];
        _xvec_sv_mlt(1.0/h_val,u,&v);
    }

#ifdef _XTHREAD_SAFE
    _xvec_vfree(u); _xvec_vfree(r); _xvec_vfree(s); _xvec_vfree(tmp);
#endif

    return (H);
}

/*!
   an implementation of the Arnoldi method, modified Gram-Schmidt algorithm
 */
_xmat *_xiter_arnoldi(_xiter *ip, _xreal *h_rem, _xmat *Q, _xmat *H){
    _XSTATIC _xvec *u=(_xvec *)NULL, *r=(_xvec *)NULL;
    _xvec v;        /* auxiliary vector */
    int i,j;
    _xreal h_val, c;
   
    if (!ip)
        _xerror(_XE_NULL,"_xiter_arnoldi");
    if (!ip->ax || !Q || !ip->x)
        _xerror(_XE_NULL,"_xiter_arnoldi");
    if (ip->k <= 0)
        _xerror(_XE_BOUNDS,"_xiter_arnoldi");
    if (Q->n != ip->x->dim || Q->m != ip->k)
        _xerror(_XE_SIZES,"_xiter_arnoldi");
   
    _xmat_zero(Q);
    H = _xmat_resize(H,ip->k,ip->k);
    _xmat_zero(H);

    u = _xvec_resize(u,ip->x->dim);
    r = _xvec_resize(r,ip->k);
    _XMEM_STAT_REG(u,_XTYPE_VEC);
    _XMEM_STAT_REG(r,_XTYPE_VEC);

    v.dim = v.max_dim = ip->x->dim;

    c = (_xreal)_xvec_vnorm2(ip->x);
    if (c <= 0.0)
        return (H);
    else {
        v.ve = Q->me[0];
        _xvec_sv_mlt(1.0/c,ip->x,&v);
    }

    _xvec_zero(r);
    for (i = 0; (unsigned int)i < ip->k; i++){
        v.ve = Q->me[i];
        u = (ip->ax)(ip->a_par,&v,u);
        for (j = 0; j <= i; j++){
            v.ve = Q->me[j];
            /* modified Gram-Schmidt */
            r->ve[j] = (_xreal)_xvec_vin_prod(&v,u);
            _xvec_mltadd(u,&v,-r->ve[j],u);
        }
        h_val = (_xreal)_xvec_vnorm2(u);
        /* if u == 0 then we have an exact subspace */
        if (h_val <= 0.0){
            *h_rem = h_val;
            return (H);
        }
        _xmat_mset_col(H,i,r);
        if (i == ip->k-1){
            *h_rem = h_val;
            continue;
        }
        _xmat_set_val(H,i+1,i,h_val);
        v.ve = Q->me[i+1];
        _xvec_sv_mlt(1.0/h_val,u,&v);
    }

#ifdef _XTHREAD_SAFE
    _xvec_vfree(u); _xvec_vfree(r);
#endif
   
    return (H);
}

/*!
    uses arnoldi() with an explicit representation of A
 */
_xmat *_xiter_sparnoldi(_xspmat *A, _xvec *x0, int m, _xreal *h_rem, _xmat *Q, _xmat *H){
    _xiter *ip;
   
    ip = _xiter_get(0,0);
    ip->ax = (_xfun_ax)_xspmat_mv_mlt;
    ip->a_par = (void *)A;
    ip->x = x0;
    ip->k = m;
    _xiter_arnoldi_iref(ip,h_rem,Q,H);
    ip->shared_x = ip->shared_b = _XTRUE;
    _xiter_free(ip);   /* release only _xiter structure */
   
    return (H); 
}

/* for testing gmres 
 */
static void _xtest_gmres(_xiter *ip, int i, _xmat *Q, _xmat *R, _xvec *givc, _xvec *givs, double h_val){
    _xvec vt, vt1;
    _XSTATIC _xmat *Q1=(_xmat *)NULL, *R1=(_xmat *)NULL;
    int j;
   
    /* test Q*A*Q^T = R  */
    Q = _xmat_resize(Q,i+1,ip->b->dim);
    Q1 = _xmat_resize(Q1,i+1,ip->b->dim);
    R1 = _xmat_resize(R1,i+1,i+1);
    _XMEM_STAT_REG(Q1,_XTYPE_MAT);
    _XMEM_STAT_REG(R1,_XTYPE_MAT);

    vt.dim = vt.max_dim = ip->b->dim;
    vt1.dim = vt1.max_dim = ip->b->dim;
    for (j=0; j <= i; j++){
        vt.ve = Q->me[j];
        vt1.ve = Q1->me[j];
        ip->ax(ip->a_par,&vt,&vt1);
    }

    _xmat_mtr_mlt(Q,Q1,R1);
    R1 = _xmat_resize(R1,i+2,i+1);
    for (j=0; j < i; j++)
        R1->me[i+1][j] = 0.0;
    R1->me[i+1][i] = (_xreal)h_val;
   
    for (j = 0; j <= i; j++){
        _xmat_rot_rows(R1,j,j+1,givc->ve[j],givs->ve[j],R1);
    }

    R1 = _xmat_resize(R1,i+1,i+1);
    _xmat_sub(R,R1,R1);
    (void) fprintf(stdout," %d. ||Q*A*Q^T - H|| = %g [cf. MACHEPS = %g]\n",
                 ip->steps,_xmat_norm_inf(R1),_XMACHEPS);
   
    /* check Q*Q^T = I */
    Q = _xmat_resize(Q,i+1,ip->b->dim);
    _xmat_mtr_mlt(Q,Q,R1);
    for (j=0; j <= i; j++)
        R1->me[j][j] -= 1.0;
    if (_xmat_norm_inf(R1) > _XMACHEPS*ip->b->dim)
        (void) fprintf(stdout, "! _xmat_norm_inf(Q*Q^T) = %g\n",_xmat_norm_inf(R1));  
#ifdef _XTHREAD_SAFE
    _xmat_mfree(Q1); _xmat_mfree(R1);
#endif
}

/*!
   generalised minimum residual algorithm of Saad & Schultz
   SIAM J. Sci. Stat. Comp. v.7, pp.856--869 (1986)
 */
_xvec *_xiter_gmres(_xiter *ip){
    _XSTATIC _xvec *u=(_xvec *)NULL, *r=(_xvec *)NULL, *rhs=(_xvec *)NULL;
    _XSTATIC _xvec *givs=(_xvec *)NULL, *givc=(_xvec *)NULL, *z=(_xvec *)NULL;
    _XSTATIC _xmat *Q = (_xmat *)NULL, *R = (_xmat *)NULL;

    _xvec *rr, v, v1;   /* additional pointers (not real vectors) */
    int i,j, done;
    _xreal nres;

#ifdef _XDEBUG 
    _xreal last_h;
#endif
   
    if (!ip)
        _xerror(_XE_NULL,"_xiter_gmres");
    if (!ip->ax || !ip->b)
        _xerror(_XE_NULL,"_xiter_gmres");
    if (!ip->stop_crit)
        _xerror(_XE_NULL,"_xiter_gmres");
    if (ip->k <= 0)
        _xerror(_XE_BOUNDS,"_xiter_gmres");
    if (ip->x && ip->x->dim != ip->b->dim)
        _xerror(_XE_SIZES,"_xiter_gmres");
    if (ip->eps <= 0.0) ip->eps = (_xreal)_XMACHEPS;

    r = _xvec_resize(r,ip->k+1);
    u = _xvec_resize(u,ip->b->dim);
    rhs = _xvec_resize(rhs,ip->k+1);
    givs = _xvec_resize(givs,ip->k);  /* Givens rotations */
    givc = _xvec_resize(givc,ip->k); 
   
    _XMEM_STAT_REG(r,_XTYPE_VEC);
    _XMEM_STAT_REG(u,_XTYPE_VEC);
    _XMEM_STAT_REG(rhs,_XTYPE_VEC);
    _XMEM_STAT_REG(givs,_XTYPE_VEC);
    _XMEM_STAT_REG(givc,_XTYPE_VEC);
   
    R = _xmat_resize(R,ip->k+1,ip->k);
    Q = _xmat_resize(Q,ip->k,ip->b->dim);
    _XMEM_STAT_REG(R,_XTYPE_MAT);
    _XMEM_STAT_REG(Q,_XTYPE_MAT);   

    if (!ip->x){  /* ip->x == 0 */
        ip->x = _xvec_get(ip->b->dim);
        ip->shared_x = _XFALSE;
    }   

    v.dim = v.max_dim = ip->b->dim;      /* v and v1 are pointers to rows */
    v1.dim = v1.max_dim = ip->b->dim;    /* of matrix Q */
   
    if (ip->bx != (_xfun_ax)NULL){    /* if precondition is defined */
        z = _xvec_resize(z,ip->b->dim);
        _XMEM_STAT_REG(z,_XTYPE_VEC);
    }
   
    done = _XFALSE;
    for (ip->steps = 0; ip->steps < ip->limit;){
        /* restart */
        ip->ax(ip->a_par,ip->x,u);        /* u = A*x */
        _xvec_sub(ip->b,u,u);       /* u = b - A*x */
        rr = u;                       /* rr is a pointer only */
      
        if (ip->bx){
            (ip->bx)(ip->b_par,u,z);        /* tmp = B*(b-A*x)  */
            rr = z;
        }
      
        nres = (_xreal)_xvec_vnorm2(rr);
        if (ip->steps == 0){
            if (ip->info) ip->info(ip,nres,(_xvec *)NULL,(_xvec *)NULL);
            ip->init_res = nres;
        }

        if (nres == 0.0){
            done = _XTRUE;
            break;
        }

        v.ve = Q->me[0];
        _xvec_sv_mlt(1.0/nres,rr,&v);
      
        _xvec_zero(r);
        _xvec_zero(rhs);
        rhs->ve[0] = nres;

        for (i = 0; (unsigned int)i < ip->k && ip->steps < ip->limit; i++){
            ip->steps++;
            v.ve = Q->me[i];  
            (ip->ax)(ip->a_par,&v,u);
            rr = u;
            if (ip->bx){
                (ip->bx)(ip->b_par,u,z);
                rr = z;
            }
   
            if ((unsigned int)i < ip->k - 1){
                v1.ve = Q->me[i+1];
                _xvec_vcopy(rr,&v1);
                for (j = 0; j <= i; j++){
                    v.ve = Q->me[j];
                    /* modified Gram-Schmidt algorithm */
                    r->ve[j] = (_xreal)_xvec_vin_prod(&v,&v1);
                    _xvec_mltadd(&v1,&v,-r->ve[j],&v1);
                }   
            
                r->ve[i+1] = nres = (_xreal)_xvec_vnorm2(&v1);
                if (nres <= _XMACHEPS*ip->init_res){
                    for (j = 0; j < i; j++) 
                        _xvec_rot_vec(r,j,j+1,givc->ve[j],givs->ve[j],r);
                    _xmat_mset_col(R,i,r);
                    done = _XTRUE;
                    break;
                }
                _xvec_sv_mlt(1.0/nres,&v1,&v1);
            }
            else {  /* i == ip->k - 1 */
                /* Q->me[ip->k] need not be computed */
                for (j = 0; j <= i; j++){
                    v.ve = Q->me[j];
                    r->ve[j] = (_xreal)_xvec_vin_prod(&v,rr);
                }       
                nres = (_xreal)(_xvec_vin_prod(rr,rr) - _xvec_vin_prod(r,r));
                if (sqrt(fabs(nres)) <= _XMACHEPS*ip->init_res){ 
                    for (j = 0; j < i; j++) 
                        _xvec_rot_vec(r,j,j+1,givc->ve[j],givs->ve[j],r);
                    _xmat_mset_col(R,i,r);
                    done = _XTRUE;
                    break;
                }
                if (nres < 0.0){ /* do restart */
                    i--; 
                    ip->steps--;
                    break;
                } 
                r->ve[i+1] = (_xreal)sqrt(nres);
            }

          /* QR update */
#ifdef _XDEBUG 
            last_h = r->ve[i+1]; /* for test only */
#endif
            for (j = 0; j < i; j++) 
                _xvec_rot_vec(r,j,j+1,givc->ve[j],givs->ve[j],r);
            _xvec_givens(r->ve[i],r->ve[i+1],&givc->ve[i],&givs->ve[i]);
            _xvec_rot_vec(r,i,i+1,givc->ve[i],givs->ve[i],r);
            _xvec_rot_vec(rhs,i,i+1,givc->ve[i],givs->ve[i],rhs);
   
            _xmat_mset_col(R,i,r);

            nres = (_xreal)fabs((double) rhs->ve[i+1]);
            if (ip->info) ip->info(ip,nres,(_xvec *)NULL,(_xvec *)NULL);
            if (ip->stop_crit(ip,nres,(_xvec *)NULL,(_xvec *)NULL)){
                done = _XTRUE;
                break;
            }
        }
      
        /* use ixi submatrix of R */
        if ((unsigned int)i >= ip->k) i = ip->k - 1;
        R = _xmat_resize(R,i+1,i+1);
        rhs = _xvec_resize(rhs,i+1);
      
#ifdef _XDEBUG
        _xtest_gmres(ip,i,Q,R,givc,givs,last_h);/* test only */
#endif
        _xmat_usolve(R,rhs,rhs,0.0);   /* solve a system: R*x = rhs */

        /* new approximation */
        for (j = 0; j <= i; j++){
            v.ve = Q->me[j]; 
            _xvec_mltadd(ip->x,&v,rhs->ve[j],ip->x);
        }  
        if (done) break;

        /* back to old dimensions */
        rhs = _xvec_resize(rhs,ip->k+1);
        R = _xmat_resize(R,ip->k+1,ip->k);
    }

#ifdef _XTHREAD_SAFE
    _xvec_vfree(u); _xvec_vfree(r); _xvec_vfree(rhs);
    _xvec_vfree(givs); _xvec_vfree(givc); _xvec_vfree(z);
    _xmat_mfree(Q); _xmat_mfree(R);
#endif

    return ip->x;
}

/*!
   a simple interface to _xiter_gmres
 */
_xvec *_xiter_spgmres(_xspmat *A, _xspmat *B, _xvec *b, double tol, _xvec *x, int k, int limit, int *steps){
    _xiter *ip;
   
    ip = _xiter_get(0,0);
    ip->ax = (_xfun_ax)_xspmat_mv_mlt;
    ip->a_par = (void *)A;
    if (B){
        ip->bx = (_xfun_ax)_xspmat_mv_mlt;
        ip->b_par = (void *) B;
    }
    else {
        ip->bx = (_xfun_ax)NULL;
        ip->b_par = NULL;
    }
    ip->k = k;
    ip->limit = limit;
    ip->info = (_xfun_info)NULL;
    ip->b = b;
    ip->eps = (_xreal)tol;
    ip->x = x;
    _xiter_gmres(ip);
    x = ip->x;
    if (steps) *steps = ip->steps;
    ip->shared_x = ip->shared_b = _XTRUE;
    _xiter_free(ip);   /* release only _xiter structure */

    return (x);   
}

/* 
  for testing mgcr 
 */
static void _xtest_mgcr(_xiter *ip, int i, _xmat *Q, _xmat *R){
    _xvec vt, vt1;
    _XSTATIC _xmat *R1=(_xmat *)NULL;
    _XSTATIC _xvec *r=(_xvec *)NULL, *r1=(_xvec *)NULL;
    _xvec *rr;
    int k,j;
    _xreal sm;
   
    /* check Q*Q^T = I */
    vt.dim = vt.max_dim = ip->b->dim;
    vt1.dim = vt1.max_dim = ip->b->dim;
   
    Q = _xmat_resize(Q,i+1,ip->b->dim);
    R1 = _xmat_resize(R1,i+1,i+1);
    r = _xvec_resize(r,ip->b->dim);
    r1 = _xvec_resize(r1,ip->b->dim);
    _XMEM_STAT_REG(R1,_XTYPE_MAT);
    _XMEM_STAT_REG(r,_XTYPE_VEC);
    _XMEM_STAT_REG(r1,_XTYPE_VEC);

    _xmat_zero(R1);
    for (k=1; k <= i; k++)
        for (j=1; j <= i; j++){
            vt.ve = Q->me[k];
            vt1.ve = Q->me[j];
            R1->me[k][j] = (_xreal)_xvec_vin_prod(&vt,&vt1);
        }
    for (j=1; j <= i; j++)
        R1->me[j][j] -= 1.0;
    if (_xmat_norm_inf(R1) > _XMACHEPS*ip->b->dim)
        (void) fprintf(stdout, "! (mgcr:) _xmat_norm_inf(Q*Q^T) = %g\n", _xmat_norm_inf(R1));  

    /* check (r_i,Ap_j) = 0 for j <= i */
    ip->ax(ip->a_par,ip->x,r);
    _xvec_sub(ip->b,r,r);
    rr = r;
    if (ip->bx){
        ip->bx(ip->b_par,r,r1);
        rr = r1;
    }
   
    (void) fprintf(stdout, "||r|| = %g\n",_xvec_vnorm2(rr));

    sm = 0.0;
    for (j = 1; j <= i; j++){
        vt.ve = Q->me[j];
        sm = (_xreal)_xmax(sm,_xvec_vin_prod(&vt,rr));
    }

    if (sm >= _XMACHEPS*ip->b->dim)
        (void) fprintf(stdout, "! (mgcr:) max_j (r,Ap_j) = %g\n",sm);

#ifdef _XTHREAD_SAFE
    _xmat_mfree(R1);
    _xvec_vfree(r); _xvec_vfree(r1);
#endif
}

/*!
   modified generalized conjugate residual algorithm;
   fast version of GCR;
 */
_xvec *_xiter_mgcr(_xiter *ip){
    _XSTATIC _xvec *As=(_xvec *)NULL, *beta=(_xvec *)NULL, *alpha=(_xvec *)NULL, *z=(_xvec *)NULL;
    _XSTATIC _xmat *N=(_xmat *)NULL, *H=(_xmat *)NULL;
   
    _xvec *rr, v, s;  /* additional pointer and structures */
    _xreal nres;      /* norm of a residual */
    _xreal dd;        /* coefficient d_i */
    int i,j;
    int done;         /* if TRUE then stop the iterative process */
    int dim;          /* dimension of the problem */
   
    /* ip cannot be NULL */
    if (!ip) _xerror(_XE_NULL,"_xiter_mgcr");
    /* Ax, b and stopping criterion must be given */
    if (!ip->ax || !ip->b || !ip->stop_crit) 
        _xerror(_XE_NULL,"_xiter_mgcr");
    /* at least one direction vector must exist */
    if (ip->k <= 0) _xerror(_XE_BOUNDS,"_xiter_mgcr");
    /* if the vector x is given then b and x must have the same dimension */
    if (ip->x && ip->x->dim != ip->b->dim)
        _xerror(_XE_SIZES,"_xiter_mgcr");
    if (ip->eps <= 0.0) ip->eps = (_xreal)_XMACHEPS;
   
    dim = ip->b->dim;
    As = _xvec_resize(As,dim);
    alpha = _xvec_resize(alpha,ip->k);
    beta = _xvec_resize(beta,ip->k);
   
    _XMEM_STAT_REG(As,_XTYPE_VEC);
    _XMEM_STAT_REG(alpha,_XTYPE_VEC);
    _XMEM_STAT_REG(beta,_XTYPE_VEC);
   
    H = _xmat_resize(H,ip->k,ip->k);
    N = _xmat_resize(N,ip->k,dim);
   
    _XMEM_STAT_REG(H,_XTYPE_MAT);
    _XMEM_STAT_REG(N,_XTYPE_MAT);
   
    /* if a preconditioner is defined */
    if (ip->bx){
        z = _xvec_resize(z,dim);
        _XMEM_STAT_REG(z,_XTYPE_VEC);
    }
   
    /* if x is NULL then it is assumed that x has 
       entries with value zero */
    if (!ip->x){
        ip->x = _xvec_get(ip->b->dim);
        ip->shared_x = _XFALSE;
    }
   
    /* v and s are additional pointers to rows of N */
    /* they must have the same dimension as rows of N */
    v.dim = v.max_dim = s.dim = s.max_dim = dim;
   
    done = _XFALSE;
    for (ip->steps = 0; ip->steps < ip->limit;){
        (*ip->ax)(ip->a_par,ip->x,As);       /* As = A*x */
        _xvec_sub(ip->b,As,As);              /* As = b - A*x */
        rr = As;                             /* rr is an additional pointer */
      
        /* if a preconditioner is defined */
        if (ip->bx){
            (*ip->bx)(ip->b_par,As,z);       /* z = B*(b-A*x)  */
            rr = z;                                  
        }
      
        /* norm of the residual */
        nres = (_xreal)_xvec_vnorm2(rr);
        dd = nres;                            /* dd = ||r_i||  */
      
        /* check if the norm of the residual is zero */
        if (ip->steps == 0){                
            /* information for a user */
            if (ip->info) (*ip->info)(ip,nres,As,rr); 
            ip->init_res = (_xreal)fabs(nres);
        }

        if (nres == 0.0){ 
            /* iterative process is finished */
            done = _XTRUE; 
            break;
        }
      
        /* save this residual in the first row of N */
        v.ve = N->me[0];
        _xvec_vcopy(rr,&v);
      
        for (i = 0; (unsigned int)i < ip->k && ip->steps < ip->limit; i++){
            ip->steps++;
            v.ve = N->me[i];     /* pointer to a row of N (=s_i) */
            /* note that we must use here &v, not v */
            (*ip->ax)(ip->a_par,&v,As); 
            rr = As;             /* As = A*s_i */
            if (ip->bx){
                (*ip->bx)(ip->b_par,As,z);  /* z = B*A*s_i  */
                rr = z;
            }
   
            if ((unsigned int)i < ip->k - 1){
                s.ve = N->me[i+1];         /* pointer to a row of N (=s_{i+1}) */
                _xvec_vcopy(rr,&s);        /* s_{i+1} = B*A*s_i */
                for (j = 0; j <= i-1; j++){
                    v.ve = N->me[j+1];         /* pointer to a row of N (=s_{j+1}) */
                    /* modified Gram-Schmidt algorithm */
                    beta->ve[j] = (_xreal)_xvec_vin_prod(&v,&s); /* beta_{j,i} */
                    _xvec_mltadd(&s,&v,- beta->ve[j],&s);    
                }
            
                /* beta_{i,i} = ||s_{i+1}||_2 */
                beta->ve[i] = nres = (_xreal)_xvec_vnorm2(&s);     
                if (nres <= _XMACHEPS*ip->init_res){
                    i--;
                    done = _XTRUE;
                    break;
                }
                _xvec_sv_mlt(1.0/nres,&s,&s);                 /* normalize s_{i+1} */
                v.ve = N->me[0];
                alpha->ve[i] = (_xreal)_xvec_vin_prod(&v,&s); /* alpha_i = (s_0 , s_{i+1}) */   
            }     
            else {
                for (j = 0; j <= i-1; j++){
                    v.ve = N->me[j+1];                          /* pointer to a row of N (=s_{j+1}) */
                    beta->ve[j] = (_xreal)_xvec_vin_prod(&v,rr);/* beta_{j,i} */
                }
                nres = (_xreal)_xvec_vin_prod(rr,rr);   /* rr = B*A*s_{k-1} */
                for (j = 0; j <= i-1; j++)
                    nres -= beta->ve[j]*beta->ve[j];
                
                if (sqrt(fabs(nres)) <= _XMACHEPS*ip->init_res){
                    /* s_k is zero */
                    i--;
                    done = _XTRUE;
                    break;
                }       
                if (nres < 0.0){ /* do restart */
                    i--; 
                    ip->steps--;
                    break; 
                }       
                beta->ve[i] = (_xreal)sqrt(nres); /* beta_{k-1,k-1} */
                v.ve = N->me[0];
                alpha->ve[i] = (_xreal)_xvec_vin_prod(&v,rr); 
                for (j = 0; j <= i-1; j++)
                    alpha->ve[i] -= beta->ve[j]*alpha->ve[j];
                alpha->ve[i] /= beta->ve[i]; /* alpha_{k-1} */
            }
            _xmat_mset_col(H,i,beta);
   
            nres = alpha->ve[i]/dd;
            if (fabs(nres-1.0) <= _XMACHEPS*ip->init_res) 
                dd = 0.0;
            else {
                nres = (_xreal)(1.0 - nres*nres);
                if (nres < 0.0){
                    nres = (_xreal)sqrt((double) -nres); 
                    if (ip->info) (*ip->info)(ip,-dd*nres,(_xvec *)NULL,(_xvec *)NULL);   
                    break;
                }   
                dd *= (_xreal)sqrt((double) nres);  
            }

            if (ip->info) (*ip->info)(ip,dd,(_xvec *)NULL,(_xvec *)NULL);     
            if (ip->stop_crit(ip,dd,(_xvec *)NULL,(_xvec *)NULL)){
                /* stopping criterion is satisfied */
                done = _XTRUE;
                break;
            }
        } /* end of for */
      
        if ((unsigned int)i >= ip->k) i = ip->k - 1;
      
        /* use (i+1) by (i+1) submatrix of H */
        H = _xmat_resize(H,i+1,i+1);
        alpha = _xvec_resize(alpha,i+1);
        _xmat_usolve(H,alpha,alpha,0.0);  /* c_i is saved in alpha */
      
        for (j = 0; j <= i; j++){
            v.ve = N->me[j];
            _xvec_mltadd(ip->x,&v,alpha->ve[j],ip->x);
        }
      
        if (done) break;  /* stop the iterative process */
        alpha = _xvec_resize(alpha,ip->k);
        H = _xmat_resize(H,ip->k,ip->k);
    }  /* end of while */

#ifdef _XTHREAD_SAFE
    _xvec_vfree(As); _xvec_vfree(beta); _xvec_vfree(alpha); _xvec_vfree(z);
    _xmat_mfree(N); _xmat_mfree(H);
#endif

    return (ip->x);
}

/*! 
   a simple interface to iter_mgcr no preconditioner 
 */
_xvec *_xiter_spmgcr(_xspmat *A, _xspmat *B, _xvec *b, double tol, _xvec *x, int k, int limit, int *steps){
    _xiter *ip;
   
    ip = _xiter_get(0,0);
    ip->ax = (_xfun_ax)_xspmat_mv_mlt;
    ip->a_par = (void *) A;
    if (B) {
        ip->bx = (_xfun_ax)_xspmat_mv_mlt;
        ip->b_par = (void *) B;
    }
    else {
        ip->bx = (_xfun_ax)NULL;
        ip->b_par = NULL;
    }

    ip->k = k;
    ip->limit = limit;
    ip->info = (_xfun_info)NULL;
    ip->b = b;
    ip->eps = (_xreal)tol;
    ip->x = x;
    _xiter_mgcr(ip);
    x = ip->x;
    if (steps) *steps = ip->steps;
    ip->shared_x = ip->shared_b = _XTRUE;
    _xiter_free(ip);   /* release only _xiter structure */

    return (x);   
}

/*!
   Conjugate gradients method for a normal equation a preconditioner
   B must be symmetric !!
 */
_xvec *_xiter_cgne(_xiter *ip){
    _XSTATIC _xvec *r = (_xvec *)NULL, *p = (_xvec *)NULL, *q = (_xvec *)NULL, *z = (_xvec *)NULL;
    _xreal alpha, beta, inner, old_inner, nres;
    _xvec *rr1;   /* pointer only */
   
    if (!ip)
        _xerror(_XE_NULL,"_xiter_cgne");
    if (!ip->ax || !ip->atx || !ip->b)
        _xerror(_XE_NULL,"_xiter_cgne");
    if (ip->x == ip->b)
        _xerror(_XE_INSITU,"_xiter_cgne");
    if (!ip->stop_crit)
        _xerror(_XE_NULL,"_xiter_cgne");
   
    if (ip->eps <= 0.0) ip->eps = (_xreal)_XMACHEPS;
   
    r = _xvec_resize(r,ip->b->dim);
    p = _xvec_resize(p,ip->b->dim);
    q = _xvec_resize(q,ip->b->dim);

    _XMEM_STAT_REG(r,_XTYPE_VEC);
    _XMEM_STAT_REG(p,_XTYPE_VEC);
    _XMEM_STAT_REG(q,_XTYPE_VEC);

    z = _xvec_resize(z,ip->b->dim);
    _XMEM_STAT_REG(z,_XTYPE_VEC);

    if (ip->x){
        if (ip->x->dim != ip->b->dim)
            _xerror(_XE_SIZES,"_xiter_cgne");
        ip->ax(ip->a_par,ip->x,p);  /* p = A*x */
        _xvec_sub(ip->b,p,z);   /* z = b - A*x */
    }
    else {  /* ip->x == 0 */
        ip->x = _xvec_get(ip->b->dim);
        ip->shared_x = _XFALSE;
        _xvec_vcopy(ip->b,z);
    }
    rr1 = z;
    if (ip->bx){
        (ip->bx)(ip->b_par,rr1,p);
        rr1 = p;
    }
    (ip->atx)(ip->at_par,rr1,r); /* r = A^T*B*(b-A*x)  */

    old_inner = 0.0;
    for (ip->steps = 0; ip->steps <= ip->limit; ip->steps++){
        rr1 = r;
        if (ip->bx){
            (ip->bx)(ip->b_par,r,z); /* rr = B*r */
            rr1 = z;
        }

        inner = (_xreal)_xvec_vin_prod(r,rr1);
        nres = (_xreal)sqrt(fabs(inner));
        if (ip->info) ip->info(ip,nres,r,rr1);
        if (ip->steps == 0) ip->init_res = nres;
        if (ip->stop_crit(ip,nres,r,rr1)) break;

        if (ip->steps){
            beta = inner/old_inner;
            p = _xvec_mltadd(rr1,p,beta,p);
        }
        else {
            beta = 0.0;
            p = _xvec_vcopy(rr1,p);
            old_inner = 0.0;
        }
        (ip->ax)(ip->a_par,p,q);     /* q = A*p */
        if (ip->bx){
            (ip->bx)(ip->b_par,q,z);
            (ip->atx)(ip->at_par,z,q);
            rr1 = q;      /* q = A^T*B*A*p */
        }
        else {
            (ip->atx)(ip->at_par,q,z);  /* z = A^T*A*p */
            rr1 = z;
        }

        alpha = (_xreal)(inner/_xvec_vin_prod(rr1,p));
        _xvec_mltadd(ip->x,p,alpha,ip->x);
        _xvec_mltadd(r,rr1,-alpha,r);
        old_inner = inner;
    }

#ifdef _XTHREAD_SAFE
    _xvec_vfree(r); _xvec_vfree(p); _xvec_vfree(q); _xvec_vfree(z);
#endif

    return (ip->x);
}

/*!
   a simple interface to iter_cgne() which uses sparse matrix data structures
   -- assumes that B contains an actual preconditioner (or NULL)
   use always as follows:
      x = _xiter_spcgne(A,B,b,eps,x,limit,steps);
   or 
      x = _xiter_spcgne(A,B,b,eps,(_xvec *)NULL,limit,steps);
   In the second case the solution vector is created.
 */
_xvec *_xiter_spcgne(_xspmat *A, _xspmat *B, _xvec *b, double eps, _xvec *x, int limit, int *steps){
    _xiter *ip;
   
    ip = _xiter_get(0,0);
    ip->ax = (_xfun_ax)_xspmat_mv_mlt;
    ip->a_par = (void *)A;
    ip->atx = (_xfun_ax)_xspmat_vm_mlt;
    ip->at_par = (void *)A;
    if (B){
        ip->bx = (_xfun_ax)_xspmat_mv_mlt;
        ip->b_par = (void *)B;
    }
    else {
        ip->bx = (_xfun_ax)NULL;
        ip->b_par = NULL;
    }
    ip->info = (_xfun_info)NULL;
    ip->b = b;
    ip->eps = (_xreal)eps;
    ip->limit = limit;
    ip->x = x;
    _xiter_cgne(ip);
    x = ip->x;
    if (steps) *steps = ip->steps;
    ip->shared_x = ip->shared_b = _XTRUE;
    _xiter_free(ip);   /* release only _xiter structure */
   
    return (x);   
}

/*! return |z| */
double _xzabs(_xcomplex z){
    double x,y,tmp;
    int x_expt, y_expt;

    /* Note: we must ensure that overflow does not occur! */
    x = ( z.re >= 0.0 ) ? z.re : -z.re;
    y = ( z.im >= 0.0 ) ? z.im : -z.im;
    if (x < y){
        tmp = x;x = y;y = tmp;
    }
    if (x == 0.0) /* then y == 0.0 as well */
        return 0.0;
    x = frexp(x,&x_expt);
    y = frexp(y,&y_expt);
    y = ldexp(y,y_expt-x_expt);
    tmp = sqrt(x*x+y*y);

    return ldexp(tmp,x_expt);
}

/*! create complex number real + i*imag */
_xcomplex _xzmake(double real, double imag){
    _xcomplex w;
    w.re = (_xreal)real; w.im = (_xreal)imag;
    return w;
}

/*! return z1+z2 */
_xcomplex _xzadd(_xcomplex z1, _xcomplex z2){
    _xcomplex z;
    z.re = z1.re + z2.re;
    z.im = z1.im + z2.im;
    return z;
}

/*! return z1-z2 */
_xcomplex _xzsub(_xcomplex z1, _xcomplex z2){
    _xcomplex z;
    z.re = z1.re - z2.re;
    z.im = z1.im - z2.im;
    return z;
}

/*! return z1*z2 */
_xcomplex _xzmlt(_xcomplex z1, _xcomplex z2){
    _xcomplex z;
    z.re = z1.re * z2.re - z1.im * z2.im;
    z.im = z1.re * z2.im + z1.im * z2.re;
    return z;
}

/*! return 1/z */
_xcomplex _xzinv(_xcomplex z){
    double x,y,tmp;
    int x_expt,y_expt;

    if (z.re == 0.0 && z.im == 0.0)
        _xerror(_XE_SING,"_xzinv");
    
    /* Note: we must ensure that overflow does not occur! */
    x = ( z.re >= 0.0 ) ? z.re : -z.re;
    y = ( z.im >= 0.0 ) ? z.im : -z.im;
    if (x < y){
        tmp = x;x = y;y = tmp;
    }
    x = frexp(x,&x_expt);
    y = frexp(y,&y_expt);
    y = ldexp(y,y_expt-x_expt);

    tmp = 1.0/(x*x + y*y);
    z.re = (_xreal)(z.re*tmp*ldexp(1.0,-2*x_expt));
    z.im = (_xreal)(-z.im*tmp*ldexp(1.0,-2*x_expt));

    return z;   
}

/*! return z1/z2 */
_xcomplex _xzdiv(_xcomplex z1, _xcomplex z2){
    return _xzmlt(z1,_xzinv(z2));
}

/*! return sqrt(z),use branch with re sqrt(z) >= 0 */
_xcomplex _xzsqrt(_xcomplex z){
    _xcomplex w; /* == sqrt(z) at end */
    double alpha;
    
    alpha = sqrt(0.5*(fabs(z.re) + _xzabs(z)));
    if (alpha != 0.0){
        if (z.re >= 0.0){
            w.re = (_xreal)alpha;
            w.im = (_xreal)(z.im / (2.0*alpha));
        }
        else {
            w.re = (_xreal)(fabs(z.im)/(2.0*alpha));
            w.im = ( z.im >= 0.0 ) ? (_xreal)alpha : (_xreal)(-alpha);
        }
    }
    else
        w.re = w.im = 0.0;

    return w;
}

/*! return exp(z) */
_xcomplex _xzexp(_xcomplex z){
    _xcomplex w;/* == exp(z) at end */
    double r;

    r = exp(z.re);
    w.re = (_xreal)(r*cos(z.im));
    w.im = (_xreal)(r*sin(z.im));

    return w;
}

/*! return log(z); uses principal branch with -pi <= Im log(z) <= pi  */
_xcomplex _xzlog(_xcomplex z){
    _xcomplex w; /* == log(z) at end */

    w.re = (_xreal)log(_xzabs(z));
    w.im = (_xreal)atan2(z.im,z.re);

    return w;
}

/*! return conjugate of z */
_xcomplex _xzconj(_xcomplex z){
    _xcomplex w; /* == conj(z) */

    w.re =   z.re;
    w.im = - z.im;
    return w;
}

/*! returns negative of z */
_xcomplex _xzneg(_xcomplex z){
    z.re = -z.re,z.im = -z.im;
    return z;
}

/*
 * zero an array of complex numbers
 */
void __xzzero__(_xcomplex *zp, int len){
    /* if a real precision zero is equivalent to a string of nulls */
    _xmem_zero((char *)zp,len*sizeof(_xcomplex));
    /* else, need to zero the array entry by entry */
    /*
    while (len--){
        zp->re = zp->im = (_xreal)0.0;
        zp++;
    }
    */
}

/*
 * complex conjugate
 */
void __xzconj__(_xcomplex *zp, int len){
    int i;
    for (i=0; i<len; i++)
        zp[i].im = -zp[i].im;
}

/*
 * scalar complex multiply array
 */
void __xzmlt__(const _xcomplex *zp, _xcomplex s, _xcomplex *out, int len){
    int i;
    _xlongreal t_re,t_im;
    
    for (i=0; i<len; i++){
        t_re = s.re*zp[i].re - s.im*zp[i].im;
        t_im = s.re*zp[i].im + s.im*zp[i].re;
        out[i].re = t_re;
        out[i].im = t_im;
    }
}

/*
 * add complex arrays 
 */
void __xzadd__(const _xcomplex *zp1, const _xcomplex *zp2, _xcomplex *out, int len){
    int i;
    for (i=0; i<len; i++){
        out[i].re = zp1[i].re + zp2[i].re;
        out[i].im = zp1[i].im + zp2[i].im;
    }
}

/*
 * subtract complex arrays 
 */
void __xzsub__(const _xcomplex *zp1, const _xcomplex *zp2, _xcomplex *out, int len){
    int i;
    for (i=0; i<len; i++){
        out[i].re = zp1[i].re - zp2[i].re;
        out[i].im = zp1[i].im - zp2[i].im;
    }
}

/*
 * scalar multiply and add i.e. complex saxpy
   -- computes zp1[i] += s.zp2[i]  if flag == 0
   -- computes zp1[i] += s.zp2[i]* if flag != 0 
 */
void __xzmltadd__(_xcomplex *zp1, const _xcomplex *zp2, _xcomplex s, int len, int flag){
    int i;
    _xlongreal t_re,t_im;
    
    if (!flag){
        for (i=0; i<len; i++){
            t_re = zp1[i].re + s.re*zp2[i].re - s.im*zp2[i].im;
            t_im = zp1[i].im + s.re*zp2[i].im + s.im*zp2[i].re;
            
            zp1[i].re = t_re;
            zp1[i].im = t_im;
        }
    }
    else {
        for (i=0; i<len; i++){
            t_re = zp1[i].re + s.re*zp2[i].re + s.im*zp2[i].im;
            t_im = zp1[i].im - s.re*zp2[i].im + s.im*zp2[i].re;
            
            zp1[i].re = t_re;
            zp1[i].im = t_im;
        }
    }
}

/*
 * inner product
   -- computes sum_i zp1[i].zp2[i] if flag == 0
               sum_i zp1[i]*.zp2[i] if flag != 0 
 */
_xcomplex __xzip__(const _xcomplex *zp1, const _xcomplex *zp2, int len, int flag){
    int i;
    _xcomplex sum = {0.0f,0.0f};
    
    if (flag){
        for (i=0; i<len; i++){
            sum.re += zp1[i].re*zp2[i].re + zp1[i].im*zp2[i].im;
            sum.im += zp1[i].re*zp2[i].im - zp1[i].im*zp2[i].re;
        }
    }
    else {
        for (i=0; i<len; i++){
            sum.re += zp1[i].re*zp2[i].re - zp1[i].im*zp2[i].im;
            sum.im += zp1[i].re*zp2[i].im + zp1[i].im*zp2[i].re;
        }
    }
    
    return sum;
}

/*!
 * get a _xzvec of dimension 'dim'
 * --Note: initialized to zero
 */
_xzvec *_xzvec_get(int dim){
    _xzvec *vector;

    if (dim < 0)
        _xerror(_XE_NEG,"_xzvec_get");

    if ((vector=_xnew(_xzvec)) == (_xzvec *)NULL)
        _xerror(_XE_MEM,"_xzvec_get");
    else if (_xmem_info_is_on()){
        _xmem_bytes(_XTYPE_ZVEC,0,sizeof(_xzvec));
        _xmem_numvar(_XTYPE_ZVEC,1);
    }
    vector->dim = vector->max_dim = dim;
    if ((vector->ve=_xnew_a(dim,_xcomplex)) == (_xcomplex *)NULL){
        _xfree(vector);
        _xerror(_XE_MEM,"_xzvec_get");
    }
    else if (_xmem_info_is_on()) {
        _xmem_bytes(_XTYPE_ZVEC,0,dim*sizeof(_xcomplex));
    }
    
    return (vector);
}

/*!
 * return the vector with dim 'new_dim'
 */
_xzvec *_xzvec_resize(_xzvec *x,int new_dim){
    if (new_dim < 0)
        _xerror(_XE_NEG,"_xzvec_resize");

    if (!x)
        return _xzvec_get(new_dim);
    
    if (new_dim == x->dim)
        return x;
    
    if (x->max_dim == 0)    /* assume that it's from sub vector */
        return _xzvec_get(new_dim);
    
    if (new_dim > (int)(x->max_dim)){
        if (_xmem_info_is_on()){ 
            _xmem_bytes(_XTYPE_ZVEC,x->max_dim*sizeof(_xcomplex),
                        new_dim*sizeof(_xcomplex));
        }
    
        x->ve = _xrenew(x->ve,new_dim,_xcomplex);
        if (!x->ve)
            _xerror(_XE_MEM,"_xzvec_resize");
        x->max_dim = new_dim;
    }
    
    if (new_dim > (int)(x->dim))
        __xzzero__(&(x->ve[x->dim]),new_dim - x->dim);
    x->dim = new_dim;
    
    return x;
}

/*!
 * zero all entries of a complex vector
 */
_xzvec *_xzvec_zero(_xzvec *x){
    if (!x)
        _xerror(_XE_NULL,"_xzvec_zero");
        
    __xzzero__(x->ve,x->dim);
    return x;
}

/*!
 * randomise a complex vector; uniform in [0,1)+[0,1)*i 
 */
_xzvec *_xzvec_rand(_xzvec *x){
    if (!x)
        _xerror(_XE_NULL,"_xzvec_rand");
        
    _xmrandlist((_xreal *)(x->ve),2*x->dim);
    return x;
}

/*!
 * copy vector into new area
 */
_xzvec *_xzvec_copy(const _xzvec *in, _xzvec *out, int i0){
    if (!in)
        _xerror(_XE_NULL,"_xzvec_copy");
    if (in == out)
        return (out);
    if (!out || out->dim < in->dim)
        out = _xzvec_resize(out, in->dim);
        
    _xmem_copy(&(in->ve[i0]),&(out->ve[i0]),(in->dim-i0)*sizeof(_xcomplex));
    return (out);
}

/*!
 * copy selected pieces of a vector
 * -- moves the length dim0 subvector with initial index i0
 *    to the corresponding subvector of out with initial index i1
 * -- out is resized if necessary
 */
_xzvec *_xzvec_move(const _xzvec *in, int i0, int dim0, _xzvec *out, int i1){
    if (!in)
        _xerror(_XE_NULL,"_xzvec_move");
    if (i0 < 0 || dim0 < 0 || i1 < 0 || i0+dim0 > (int)(in->dim))
        _xerror(_XE_BOUNDS,"_xzvec_move");  
    if (!out || i1+dim0 > (int)(out->dim))
        out = _xzvec_resize(out,i1+dim0);
        
    _xmem_copy(&(in->ve[i0]),&(out->ve[i1]),dim0*sizeof(_xcomplex));
    return (out);
}

/*!
 * to allocate memory to many arguments. 
 * the function should be called:
 * _xzvec_get_vars(dim,&x,&y,&z,...,NULL);
 * where 
 *   int dim;
 *   _xzvec *x, *y, *z,...;
 *   The last argument should be NULL ! 
 *   dim is the length of vectors x,y,z,...
 *   returned value is equal to the number of allocated variables
 *   other get_vars... functions are similar.
 *
 */
int _xzvec_get_vars(int dim, ...){
    int i=0;
    va_list ap;
    _xzvec **par;
    
    va_start(ap,dim);
    while (par = va_arg(ap,_xzvec **)){/* NULL ends the list */
        *par = _xzvec_get(dim);
        i++;
    }
    va_end(ap);
    
    return i;
}

/*!
 * to resize memory for many arguments. 
 * the function should be called:
 * _xzvec_resize_vars(new_dim,&x,&y,&z,...,NULL);
 * where 
 *   int new_dim;
 *   _xzvec *x, *y, *z,...;
 *   The last argument should be NULL ! 
 *   rdim is the resized length of vectors x,y,z,...
 *   returned value is equal to the number of allocated variables.
 *   If one of x,y,z,.. arguments is NULL then memory is allocated to this 
 *   argument. 
 *   other *_resize_vars() functions are similar.
 */
int _xzvec_resize_vars(int new_dim, ...){
    int i=0;
    va_list ap;
    _xzvec **par;
    
    va_start(ap,new_dim);
    while (par = va_arg(ap, _xzvec **)){/* NULL ends the list */
        *par = _xzvec_resize(*par,new_dim);
        i++;
    }
    va_end(ap);
    
    return i;
}

/*!
 * to deallocate memory for many arguments. 
 * the function should be called:
 * _xzvec_free_vars(&x,&y,&z,...,NULL);
 * where 
 *   _xzvec *x, *y, *z,...;
 *   The last argument should be NULL ! 
 *   There must be at least one not NULL argument.
 *   returned value is equal to the number of allocated variables.
 *   returned value of x,y,z,.. is VNULL.
 *   other *_free_vars() functions are similar.
 */
int _xzvec_free_vars(_xzvec**pv, ...){
    int i=1;
    va_list ap;
    _xzvec **par;
    
    _xzvec_free(*pv);
    *pv = (_xzvec *)NULL;
    
    va_start(ap,pv);
    while (par = va_arg(ap, _xzvec **)){/* NULL ends the list */
        _xzvec_free(*par);
        *par = (_xzvec *)NULL;
        i++;
    }
    va_end(ap);
    
    return i;
}

static const char *_xzformat = " (%14.9g, %14.9g) ";
const char *_xsetzformat(const char *fstring){
    const char *old_fstring;
    old_fstring = _xzformat;
    if (fstring != (char *)NULL && *fstring != '\0')
        _xzformat = fstring;
        
    return old_fstring;
}

_xcomplex _xzfinput(FILE *fp){
    int io_code;
    _xcomplex z;

    _xskipjunk(fp);
    if (isatty(_fileno(fp))){
        do {
            fprintf(stderr,"real and imag parts: ");
            if (fgets(_xline,_XMAXLINE,fp) == NULL)
                _xerror(_XE_EOF,"_xzfinput");
#if _XREAL == _XDOUBLE
            io_code = sscanf(_xline,"%lf%lf",&z.re,&z.im);
#elif _XREAL == _XFLOAT
            io_code = sscanf(_xline,"%f%f",&z.re,&z.im);
#endif
        } while (io_code != 2);
    }
    else
#if _XREAL == _XDOUBLE
        if ((io_code=fscanf(fp," (%lf,%lf)",&z.re,&z.im)) < 2)
#elif _XREAL == _XFLOAT
        if ((io_code=fscanf(fp," (%f,%f)",&z.re,&z.im)) < 2)
#endif
            _xerror((io_code == EOF) ? _XE_EOF : _XE_FORMAT,"_xzfinput");

    return z;
}

void _xzfoutput(FILE *fp, _xcomplex z){
    fprintf(fp,_xzformat,z.re,z.im);
    putc('\n',fp);
}

void _xzvec_dump(FILE *fp, _xzvec *x){
    unsigned int i,tmp;
    if (!x){
        fprintf(fp,"ComplexVector: NULL\n");
        return;
    }
    if (!x->ve){
        fprintf(fp,"NULL\n");
        return;
    }
    fprintf(fp,"x->ve @ 0x%lx\n",(long)(x->ve));
    for (i=0,tmp=0; i<x->dim; i++,tmp++){
        fprintf(fp,_xzformat,x->ve[i].re,x->ve[i].im);
        if (tmp%2 == 1) putc('\n',fp);
    }
    if (tmp%2 != 0) putc('\n',fp);
}

void _xzvec_foutput(FILE *fp, _xzvec *x){
    unsigned int i,tmp;
    if (!x){
        fprintf(fp,"ComplexVector: NULL\n");
        return;
    }
    if (!x->ve){
        fprintf(fp,"NULL\n");
        return;
    }
    for (i=0,tmp=0; i<x->dim; i++,tmp++){
        fprintf(fp,_xzformat,x->ve[i].re,x->ve[i].im);
        if (tmp%2 == 1) putc('\n',fp);
    }
    if (tmp%2 != 0) putc('\n',fp);
}

static _xzvec *_xzvec_ifinput(FILE *fp, _xzvec *x){
    unsigned int i,dim,dynamic;  /* dynamic set if memory allocated here */
     
    /* get vector dimension */
    if (x && x->dim < _XMAXDIM){
        dim = x->dim;
        dynamic = _XFALSE;
    }
    else {
        dynamic = _XTRUE;
        do {
            fprintf(stderr,"ComplexVector: dim: ");
            if (fgets(_xline,_XMAXLINE,fp)==NULL)
                _xerror(_XE_INPUT,"_xzvec_ifinput");
        } while (sscanf(_xline,"%u",&dim)<1 || dim > _XMAXDIM );
        x = _xzvec_get(dim);
    }
     
    /* input elements */
    for (i=0; i<dim; i++)
        do {
        redo:
            fprintf(stderr,"entry %u: ",i);
            if (!dynamic)
                fprintf(stderr,"old (%14.9g,%14.9g) new: ",
                        x->ve[i].re,x->ve[i].im);
            if (fgets(_xline,_XMAXLINE,fp)==NULL)
                _xerror(_XE_INPUT,"_xzvec_ifinput");
            if ((*_xline == 'b' || *_xline == 'B') && i > 0){
                i--;
                dynamic = _XFALSE;
                goto redo;
            }
            if ((*_xline == 'f' || *_xline == 'F') && i < dim-1){
                i++;
                dynamic = _XFALSE;
                goto redo;
            }
        } while (*_xline=='\0' ||
#if _XREAL == _XDOUBLE
                sscanf(_xline,"%lf%lf",
#elif _XREAL == _XFLOAT
                sscanf(_xline,"%f%f",
#endif  
                &x->ve[i].re,&x->ve[i].im) < 2);
     
    return (x);
}

static _xzvec *_xzvec_bfinput(FILE *fp, _xzvec *x){
    unsigned int i,dim;
    int io_code;
     
    /* get dimension */
    _xskipjunk(fp);
    if ((io_code=fscanf(fp," ComplexVector: dim:%u",&dim)) < 1 ||
        dim > _XMAXDIM)
        _xerror(io_code==EOF ? 7 : 6,"_xzvec_bfinput");

     
    /* allocate memory if necessary */
    if (!x || x->dim < dim)
        x = _xzvec_resize(x,dim);
     
    /* get entries */
    _xskipjunk(fp);
    for (i=0; i<dim; i++)
#if _XREAL == _XDOUBLE
        if ((io_code=fscanf(fp," (%lf,%lf)",
#elif _XREAL == _XFLOAT
        if ((io_code=fscanf(fp," (%f,%f)",
#endif
            &x->ve[i].re,&x->ve[i].im)) < 2)
            _xerror(io_code==EOF ? 7 : 6,"_xzvec_bfinput");
     
    return (x);
}

_xzvec *_xzvec_finput(FILE *fp, _xzvec *x){
    _xzvec *_xzvec_ifinput(FILE *, _xzvec *),*_xzvec_bfinput(FILE *, _xzvec *);
    
    if (isatty(_fileno(fp)))
        return _xzvec_ifinput(fp,x);
    else
        return _xzvec_bfinput(fp,x);
}

/*!
 * scalar-vector multiply -- may be in-situ
 */
_xzvec *_xzvec_mlt(_xcomplex scalar, const _xzvec *vector, _xzvec *out){
    if (!vector)
        _xerror(_XE_NULL,"_xzvec_mlt");
    if (!out || out->dim != vector->dim)
        out = _xzvec_resize(out,vector->dim);
    if (scalar.re == 0.0 && scalar.im == 0.0)
        return _xzvec_zero(out);
    if (scalar.re == 1.0 && scalar.im == 0.0)
        return _xzvec_vcopy(vector,out);
        
    __xzmlt__(vector->ve,scalar,out->ve,(int)(vector->dim));
    return (out);
}

/*!
 * vector addition -- may be in-situ
 */
_xzvec *_xzvec_add(const _xzvec *vec1, const _xzvec *vec2, _xzvec *out){
    if (!vec1 || !vec2)
        _xerror(_XE_NULL,"_xzvec_add");
    if (vec1->dim != vec2->dim)
        _xerror(_XE_SIZES,"_xzvec_add");
    if (!out || out->dim != vec1->dim)
        out = _xzvec_resize(out,vec1->dim);
    __xzadd__(vec1->ve,vec2->ve,out->ve,(int)(vec1->dim));
    return (out);
}

/*!
 * vector subtraction -- may be in-situ
 */
_xzvec *_xzvec_sub(const _xzvec *vec1, const _xzvec *vec2, _xzvec *out){
    if (!vec1 || !vec2)
        _xerror(_XE_NULL,"_xzvec_sub");
    if (vec1->dim != vec2->dim)
        _xerror(_XE_SIZES,"_xzvec_sub");
    if (!out || out->dim != vec1->dim)
        out = _xzvec_resize(out,vec1->dim);
    __xzsub__(vec1->ve,vec2->ve,out->ve,(int)(vec1->dim));
    return (out);
}

/*!
 * computes componentwise (Hadamard) product of x1 and x2
 * -- result out is returned
 */
_xzvec *_xzvec_star(const _xzvec *x1, const _xzvec *x2, _xzvec *out){
    int i;
    _xreal t_re,t_im;

    if (!x1 || !x2)
        _xerror(_XE_NULL,"_xzvec_star");
    if (x1->dim != x2->dim)
        _xerror(_XE_SIZES,"_xzvec_star");
    out = _xzvec_resize(out,x1->dim);

    for (i=0; i<(int)(x1->dim); i++){
        t_re = x1->ve[i].re*x2->ve[i].re - x1->ve[i].im*x2->ve[i].im;
        t_im = x1->ve[i].re*x2->ve[i].im + x1->ve[i].im*x2->ve[i].re;
        out->ve[i].re = t_re;
        out->ve[i].im = t_im;
    }

    return out;
}

/*!
 * computes componentwise ratio of x2 and x1
 * -- out[i] = x2[i] / x1[i]
 * -- if x1[i] == 0 for some i, then raise _XE_SING error
 * -- result out is returned
 */
_xzvec *_xzvec_slash(const _xzvec *x1, const _xzvec *x2, _xzvec *out){
    int i;
    _xreal r2,t_re,t_im;
    _xcomplex tmp;

    if (!x1 || !x2)
        _xerror(_XE_NULL,"_xzvec_slash");
    if (x1->dim != x2->dim)
        _xerror(_XE_SIZES,"_xzvec_slash");
    out = _xzvec_resize(out,x1->dim);

    for (i=0; i<(int)(x1->dim); i++){
        r2 = x1->ve[i].re*x1->ve[i].re + x1->ve[i].im*x1->ve[i].im;
        if (r2 == 0.0)
            _xerror(_XE_SING,"_xzvec_slash");
        tmp.re =   x1->ve[i].re / r2;
        tmp.im = - x1->ve[i].im / r2;
        t_re = tmp.re*x2->ve[i].re - tmp.im*x2->ve[i].im;
        t_im = tmp.re*x2->ve[i].im + tmp.im*x2->ve[i].re;
        out->ve[i].re = t_re;
        out->ve[i].im = t_im;
    }

    return out;
}

/*!
 * scalar/vector multiplication and addition
 * -- out = v1 + scale.v2
 */
_xzvec *_xzvec_mltadd(const _xzvec *v1, const _xzvec *v2, _xcomplex scale, _xzvec *out){
    if (!v1 || !v2)
        _xerror(_XE_NULL,"_xzvec_mltadd");
    if (v1->dim != v2->dim)
        _xerror(_XE_SIZES,"_xzvec_mltadd");
    if (scale.re == 0.0 && scale.im == 0.0)
        return _xzvec_vcopy(v1,out);
    if (scale.re == 1.0 && scale.im == 0.0)
        return _xzvec_add(v1,v2,out);
    if (v2 != out){
        _xtracecatch(out = _xzvec_vcopy(v1,out),"_xzvec_mltadd");
        __xzmltadd__(out->ve,v2->ve,scale,(int)(v1->dim),0);
    }
    else {
        _xtracecatch(out = _xzvec_mlt(scale,v2,out),"_xzvec_mltadd");
        out = _xzvec_add(v1,out,out);
    }

    return (out);
}

/*! compute (scaled) 1-norms of vectors */
double _xzvec_norm1(_xzvec *x, _xvec *scale){
    int i,dim;
    double s,sum;
    
    if (!x)
        _xerror(_XE_NULL,"_xzvec_norm1");
    dim = x->dim;
    
    sum = 0.0;
    if (!scale)
        for (i=0; i<dim; i++)
            sum += _xzabs(x->ve[i]);
    else if ((int)(scale->dim) < dim)
        _xerror(_XE_SIZES,"_xzvec_norm1");
    else
        for (i=0; i<dim; i++){
            s = scale->ve[i];
            sum += ( s== 0.0 ) ? _xzabs(x->ve[i]) : _xzabs(x->ve[i])/fabs(s);
    }
    
    return sum;
}

/*! compute (scaled) 2-norm (Euclidean norm) of vectors */
double _xzvec_norm2(_xzvec *x, _xvec *scale){
    int i,dim;
    double s,sum;
    
    if (!x)
        _xerror(_XE_NULL,"_xzvec_norm2");
    dim = x->dim;
    
    sum = 0.0;
    if (!scale)
        for (i=0; i<dim; i++)
            sum += _xsquare(x->ve[i].re) + _xsquare(x->ve[i].im);
    else if ((int)(scale->dim) < dim)
        _xerror(_XE_SIZES,"_xzvec_norm2");
    else
        for (i=0; i<dim; i++){
            s = scale->ve[i];
            sum += ( s== 0.0 ) ? _xsquare(x->ve[i].re) + _xsquare(x->ve[i].im) :
                                (_xsquare(x->ve[i].re) + _xsquare(x->ve[i].im))/_xsquare(s);
    }
    
    return sqrt(sum);
}

/*! compute (scaled) infinity-norm (supremum norm) of vectors */
double _xzvec_norm_inf(_xzvec *x, _xvec *scale){
    int i,dim;
    double s,maxval,tmp;
    
    if (!x)
        _xerror(_XE_NULL,"_xzvec_norm_inf");
    dim = x->dim;
    
    maxval = 0.0;
    if (!scale)
        for (i=0; i<dim; i++){
            tmp = _xzabs(x->ve[i]);
            maxval = _xmax(maxval,tmp);
        }
    else if ((int)(scale->dim) < dim)
        _xerror(_XE_SIZES,"_xzvec_norm_inf");
    else
        for (i=0; i<dim; i++){
            s = scale->ve[i];
            tmp = ( s == 0.0 ) ? _xzabs(x->ve[i]) : _xzabs(x->ve[i])/fabs(s);
            maxval = _xmax(maxval,tmp);
        }
    
    return maxval;
}

/*!
 * inner product of two vectors from i0 downwards
 * -- flag != 0 means compute sum_i a[i]*.b[i];
 * -- flag == 0 means compute sum_i a[i].b[i] 
 */
_xcomplex _xzvec_in_prod(const _xzvec *a, const _xzvec *b, unsigned int i0, unsigned int flag){
    unsigned int limit;

    if (!a || !b)
        _xerror(_XE_NULL,"_xzvec_in_prod");
    limit = _xmin(a->dim,b->dim);
    if (i0 > limit)
        _xerror(_XE_BOUNDS,"_xzvec_in_prod");

    return __xzip__(&(a->ve[i0]),&(b->ve[i0]),(int)(limit-i0),flag);
}

/*! return sum of entries of a vector */
_xcomplex _xzvec_sum(const _xzvec *x){
    unsigned int i;
    _xcomplex sum;

    if (!x)
        _xerror(_XE_NULL,"_xzvec_sum");

    sum.re = sum.im = 0.0f;
    for (i=0; i<x->dim; i++){
        sum.re += x->ve[i].re;
        sum.im += x->ve[i].im;
    }

    return sum;
}

/*! return sum_i a[i].v[i], a[i] real, v[i] vectors */
_xzvec *_xzvec_lincomb(int n, const _xzvec **v, const _xcomplex *a, _xzvec *out){
    int i;

    if (!a || !v)
        _xerror(_XE_NULL,"_xzvec_lincomb");
    if (n <= 0)
        return NULL;

    for (i=1; i<n; i++)
        if (out == v[i])
            _xerror(_XE_INSITU,"_xzvec_lincomb");

    out = _xzvec_mlt(a[0],v[0],out);
    for (i=1; i<n; i++){
        if (!v[i])
            _xerror(_XE_NULL,"_xzvec_lincomb");
        if (v[i]->dim != out->dim)
            _xerror(_XE_SIZES,"_xzvec_lincomb");
        out = _xzvec_mltadd(out,v[i],a[i],out);
    }

    return out;
}

/*!
 * linear combinations taken from a list of arguments;
 * calling:
 * _xzvec_linlist(out,v1,a1,v2,a2,...,vn,an,NULL);
 * where vi are vectors (_xzvec *) and ai are numbers (_xcomplex)
 */
_xzvec *_xzvec_linlist(_xzvec *out, _xzvec *v1, _xcomplex a1, ...){
    va_list ap;
    _xzvec *par;
    _xcomplex a_par;

    if (!v1)
        return NULL;
   
    va_start(ap, a1);
    out = _xzvec_mlt(a1,v1,out);
   
    while (par = va_arg(ap,_xzvec *)) {/* NULL ends the list*/
        a_par = va_arg(ap,_xcomplex);
        if (a_par.re == 0.0 && a_par.im == 0.0) continue;
        if (out == par)     
            _xerror(_XE_INSITU,"_xzvec_linlist");
        if (out->dim != par->dim)   
            _xerror(_XE_SIZES,"_xzvec_linlist");

        if (a_par.re == 1.0 && a_par.im == 0.0)
            out = _xzvec_add(out,par,out);
        else if (a_par.re == -1.0 && a_par.im == 0.0)
            out = _xzvec_sub(out,par,out);
        else
            out = _xzvec_mltadd(out,par,a_par,out); 
     } 
     va_end(ap);
     
     return out;
}

/*! permute vector */
_xzvec *_xperm_zvec(_xperm *px, _xzvec *vector, _xzvec *out){
    unsigned int old_i,i,size,start;
    _xcomplex tmp;
    
    if (!px || !vector)
        _xerror(_XE_NULL,"_xperm_zvec");
    if (px->size > vector->dim)
        _xerror(_XE_SIZES,"_xperm_zvec");
    if (!out || out->dim < vector->dim)
        out = _xzvec_resize(out,vector->dim);
    
    size = px->size;
    if (size == 0)
        return _xzvec_vcopy(vector,out);
    
    if (out != vector){
        for (i=0; i<size; i++)
            if (px->pe[i] >= size)
                _xerror(_XE_BOUNDS,"_xperm_zvec");
            else
                out->ve[i] = vector->ve[px->pe[i]];
    }
    else { /* in situ algorithm */
        start = 0;
        while (start < size){
            old_i = start;
            i = px->pe[old_i];
            if (i >= size){
                start++;
                continue;
            }
            tmp = vector->ve[start];
            while (_XTRUE){
                vector->ve[old_i] = vector->ve[i];
                px->pe[old_i] = i+size;
                old_i = i;
                i = px->pe[old_i];
                if (i >= size)
                    break;
                if (i == start){
                    vector->ve[old_i] = tmp;
                    px->pe[old_i] = i+size;
                    break;
                }
            }
            start++;
        }
        for (i=0; i<size; i++)
            if (px->pe[i] < size)
                _xerror(_XE_BOUNDS,"_xperm_zvec");
            else
                px->pe[i] = px->pe[i]-size;
    }
    
    return out;
}

/*!
 * apply the inverse of px to x, returning the result in out
 * -- may NOT be in situ
 */
_xzvec *_xperm_inv_zvec(_xperm *px, _xzvec *x, _xzvec *out){
    unsigned int i,size;
    
    if (!px || !x)
        _xerror(_XE_NULL,"_xperm_inv_zvec");
    if (px->size > x->dim)
        _xerror(_XE_SIZES,"_xperm_inv_zvec");
    if (!out || out->dim < x->dim)
        out = _xzvec_resize(out,x->dim);
    
    size = px->size;
    if (size == 0)
        return _xzvec_vcopy(x,out);
    if (out != x){
        for (i=0; i<size; i++)
        if (px->pe[i] >= size)
            _xerror(_XE_BOUNDS,"_xperm_inv_zvec");
        else
            out->ve[px->pe[i]] = x->ve[i];
    }
    else { /* in situ algorithm --- cheat's way out */
        _xperm_inv(px,px);
        _xperm_zvec(px,x,out);
        _xperm_inv(px,px);
    }
    
    return out;
}

/*! map function f over components of x: out[i] = f(x[i]) */
_xzvec *_xzvec_map(_xcomplex (*f)(_xcomplex),const _xzvec *x,_xzvec *out){
    _xcomplex *x_ve,*out_ve;
    int i,dim;

    if (!x || !f)
        _xerror(_XE_NULL,"_xzvec_map");
    if (!out || out->dim != x->dim)
        out = _xzvec_resize(out,x->dim);

    dim = x->dim; x_ve = x->ve; out_ve = out->ve;
    for (i=0; i<dim; i++)
        out_ve[i] = (*f)(x_ve[i]);

    return out;
}

/*! set out[i] = f(x[i],params) */
_xzvec *_xzvec_vmap(_xcomplex (*f)(void *,_xcomplex),void *params,const _xzvec *x,_xzvec *out){
    _xcomplex *x_ve,*out_ve;
    int i,dim;

    if (!x || !f)
        _xerror(_XE_NULL,"_xzvec_vmap");
    if (!out || out->dim != x->dim)
        out = _xzvec_resize(out,x->dim);

    dim = x->dim; x_ve = x->ve; out_ve = out->ve;
    for (i=0; i<dim; i++)
        out_ve[i] = (*f)(params,x_ve[i]);

    return out;
}

/* matlab routines */

/*! return an array 'out' such that each element of 'out' is the 
 *  absolute value of the corresponding element of 'in'
 */
_xvec *_xzvec_abs(const _xzvec *in, _xvec *out){
	unsigned int i;

	if (!in)
		_xerror(_XE_NULL,"_xzvec_abs");
	if ((out = _xvec_resize(out,in->dim)) == NULL)
		_xerror(_XE_MEM,"_xzvec_abs");
	for (i=0; i<in->dim; i++)
		out->ve[i] = _xzabs(in->ve[i]);

	return out;
}

/*! return the phase angles, in radians, for each element of complex array.
 *  the angles lie between ¡Àpi 
 */
_xvec *_xzvec_angle(const _xzvec *in, _xvec *out){
	_xcomplex c;
	unsigned int i;

	if (!in)
		_xerror(_XE_NULL,"_xzvec_abs");
	if ((out = _xvec_resize(out,in->dim)) == NULL)
		_xerror(_XE_MEM,"_xzvec_abs");
	for (i=0; i<in->dim; i++){
		c = in->ve[i];
		out->ve[i] = atan2(c.im,c.re);
	}

	return out;
}

/*! return the real part of the elements of the complex array */
_xvec *_xzvec_real(const _xzvec *in, _xvec *out){
	_xcomplex c;
	unsigned int i;

	if (!in)
		_xerror(_XE_NULL,"_xzvec_real");
	if ((out = _xvec_resize(out,in->dim)) == NULL)
		_xerror(_XE_MEM,"_xzvec_real");
	for (i=0; i<in->dim; i++){
		c = in->ve[i];
		out->ve[i] = c.re;
	}

	return out;
}

/*! return the imaginary part of the elements of array */
_xvec *_xzvec_imag(const _xzvec *in, _xvec *out){
	_xcomplex c;
	unsigned int i;

	if (!in)
		_xerror(_XE_NULL,"_xzvec_imag");
	if ((out = _xvec_resize(out,in->dim)) == NULL)
		_xerror(_XE_MEM,"_xzvec_imag");
	for (i=0; i<in->dim; i++){
		c = in->ve[i];
		out->ve[i] = c.im;
	}

	return out;
}

/*! get an mxn complex matrix (in _xzmat form) */
_xzmat *_xzmat_get(int m, int n){
    _xzmat *matrix;
    unsigned int i;
   
    if (m < 0 || n < 0)
        _xerror(_XE_NEG,"_xzmat_get");

    if ((matrix=_xnew(_xzmat)) == (_xzmat *)NULL)
        _xerror(_XE_MEM,"_xzmat_get");
    else if (_xmem_info_is_on()){
        _xmem_bytes(_XTYPE_ZMAT,0,sizeof(_xzmat));
        _xmem_numvar(_XTYPE_ZMAT,1);
    }
   
    matrix->m = matrix->max_m = m;
    matrix->n = matrix->max_n = n;
    matrix->max_size = m*n;
    
#ifndef _XSEGMENTED
    if ((matrix->base = _xnew_a(m*n,_xcomplex)) == (_xcomplex *)NULL){
        _xfree(matrix);
        _xerror(_XE_MEM,"_xzmat_get");
    }
    else if (_xmem_info_is_on()){
        _xmem_bytes(_XTYPE_ZMAT,0,m*n*sizeof(_xcomplex));
    }
#else
    matrix->base = (_xcomplex *)NULL;
#endif
    if ((matrix->me = (_xcomplex **)_xcalloc(m,sizeof(_xcomplex *))) == 
       (_xcomplex **)NULL){    
        _xfree(matrix->base); _xfree(matrix);
        _xerror(_XE_MEM,"_xzmat_get");
    }
    else if (_xmem_info_is_on()){
        _xmem_bytes(_XTYPE_ZMAT,0,m*sizeof(_xcomplex *));
    }
#ifndef _XSEGMENTED
    /* set up pointers */
    for (i=0; (int)(i)<m; i++)
        matrix->me[i] = &(matrix->base[i*n]);
#else
    for (i=0; i<m; i++)
       if ((matrix->me[i]=_xnew_a(n,_xcomplex)) == (_xcomplex *)NULL )
           _xerror(_XE_MEM,"_xzmat_get");
       else if (_xmem_info_is_on()){
           _xmem_bytes(_XTYPE_ZMAT,0,n*sizeof(_xcomplex));
       }
#endif
   
    return (matrix);
}

/*!
 * return the matrix A of size new_m x new_n; A is zeroed
 * -- if A == NULL on entry then the effect is equivalent to 
 * _xzmat_get()
 */
_xzmat *_xzmat_resize(_xzmat *A, int new_m, int new_n){
    unsigned int i,new_max_m,new_max_n,new_size,old_m,old_n;
   
    if (new_m < 0 || new_n < 0)
        _xerror(_XE_NEG,"_xzmat_resize");

    if (!A)
        return _xzmat_get(new_m,new_n);
   
    if (new_m == A->m && new_n == A->n)
        return A;

    old_m = A->m; old_n = A->n;
    if ((unsigned int)(new_m) > A->max_m){ /* re-allocate A->me */
        if (_xmem_info_is_on()){
            _xmem_bytes(_XTYPE_ZMAT,A->max_m*sizeof(_xcomplex *),
                        new_m*sizeof(_xcomplex *));
        }

        A->me = _xrenew(A->me,new_m,_xcomplex *);
        if (!A->me)
            _xerror(_XE_MEM,"_xzmat_resize");
    }
    new_max_m = _xmax((unsigned int)(new_m),A->max_m);
    new_max_n = _xmax((unsigned int)(new_n),A->max_n);
   
#ifndef _XSEGMENTED
    new_size = new_max_m*new_max_n;
    if (new_size > A->max_size){ /* re-allocate A->base */
        if (_xmem_info_is_on()){
            _xmem_bytes(_XTYPE_ZMAT,A->max_m*A->max_n*sizeof(_xcomplex),
                        new_size*sizeof(_xcomplex));      
        }

        A->base = _xrenew(A->base,new_size,_xcomplex);
        if (!A->base)
            _xerror(_XE_MEM,"_xzmat_resize");
        A->max_size = new_size;
    }
   
    /* now set up A->me[i] */
    for (i=0; (int)(i)<new_m; i++)
        A->me[i] = &(A->base[i*new_n]);
   
    /* now shift data in matrix */
    if (old_n > (unsigned int)(new_n)){
        for (i=1; i<_xmin(old_m,(unsigned int)(new_m)); i++)
            _xmem_copy((char *)&(A->base[i*old_n]),
                       (char *)&(A->base[i*new_n]),
                        sizeof(_xcomplex)*new_n);
    }
    else if (old_n < (unsigned int)(new_n)){
        for (i=_xmin(old_m,(unsigned int)(new_m))-1; i>0; i--){/* copy & then zero extra space */
            _xmem_copy((char *)&(A->base[i*old_n]),
                       (char *)&(A->base[i*new_n]),
                        sizeof(_xcomplex)*old_n);
            __xzzero__(&(A->base[i*new_n+old_n]),(new_n-old_n));
        }
        __xzzero__(&(A->base[old_n]),(new_n-old_n));
        A->max_n = new_n;
    }
    /* zero out the new rows.. */
    for (i=old_m; i<(unsigned int)(new_m); i++)
        __xzzero__(&(A->base[i*new_n]),new_n);
#else
    if (A->max_n < new_n){
        _xcomplex *tmp;
      
        for (i=0; i<A->max_m; i++){
            if (_xmem_info_is_on()){
                _xmem_bytes(_XTYPE_ZMAT,A->max_n*sizeof(_xcomplex),
                            new_max_n*sizeof(_xcomplex));
            }

            if ((tmp = _xrenew(A->me[i],new_max_n,_xcomplex)) == NULL)
                _xerror(_XE_MEM,"_xzmat_resize");
            else 
                A->me[i] = tmp;
        }
        for (i=A->max_m; i<new_max_m; i++){
            if ((tmp = _xnew_a(new_max_n,_xcomplex)) == NULL)
                _xerror(_XE_MEM,"_xzmat_resize");
            else {
                A->me[i] = tmp;
                if (_xmem_info_is_on()){
                    _xmem_bytes(_XTYPE_ZMAT,0,new_max_n*sizeof(_xcomplex));
                }
            }
        }
    }
    else if (A->max_m < new_m){
        for (i=A->max_m; i<new_m; i++)
            if ((A->me[i] = _xnew_a(new_max_n,_xcomplex)) == NULL)
                _xerror(_XE_MEM,"_xzmat_resize");
            else if (_xmem_info_is_on()){
                _xmem_bytes(_XTYPE_ZMAT,0,new_max_n*sizeof(_xcomplex));
            }  
    }
   
    if (old_n < new_n){
        for (i=0; i<old_m; i++)
            __xzzero__(&(A->me[i][old_n]),new_n-old_n);
    }
   
    /* zero out the new rows.. */
    for (i=old_m; i<new_m; i++)
        __xzzero__(A->me[i],new_n);
#endif
   
    A->max_m = new_max_m;
    A->max_n = new_max_n;
    A->max_size = A->max_m*A->max_n;
    A->m = new_m; A->n = new_n;
   
    return A;
}

/*! zero all entries of a complex matrix */
_xzmat *_xzmat_zero(_xzmat *A){
    unsigned int i;
   
    if (!A)
        _xerror(_XE_NULL,"_xzmat_zero");
    for (i = 0; i < A->m; i++)
        __xzzero__(A->me[i],A->n);
   
    return A;
}

/*! randomise a complex matrix; uniform in [0,1)+[0,1)*i */
_xzmat *_xzmat_rand(_xzmat *A){
    unsigned int i;

    if (!A)
        _xerror(_XE_NULL,"_xzmat_rand");

    for (i = 0; i < A->m; i++)
        _xmrandlist((_xreal *)(A->me[i]),2*A->n);

    return A;
}

int _xzmat_get_vars(int m, int n, ...){
    int i=0;
    va_list ap;
    _xzmat **par;
   
    va_start(ap, n);
    while (par = va_arg(ap,_xzmat **)){/* NULL ends the list*/
        *par = _xzmat_get(m,n);
        i++;
    } 
    va_end(ap);
    
    return i;
}

int _xzmat_resize_vars(int m, int n, ...){
    int i=0;
    va_list ap;
    _xzmat **par;
   
    va_start(ap, n);
    while (par = va_arg(ap,_xzmat **)) {/* NULL ends the list*/
        *par = _xzmat_resize(*par,m,n);
        i++;
    } 
    va_end(ap);
    
    return i;
}

int _xzmat_free_vars(_xzmat**va, ...){
    int i=1;
    va_list ap;
    _xzmat **par;
   
    _xzmat_free(*va);
    *va = (_xzmat *)NULL;
    va_start(ap, va);
    while (par = va_arg(ap,_xzmat **)){ /* NULL ends the list*/
        _xzmat_free(*par); 
        *par = (_xzmat *)NULL;
        i++;
    } 
    va_end(ap);
    
    return i;
}

/*! compute matrix 1-norm -- unscaled -- complex version */
double _xzmat_norm1(_xzmat *A){
    int i,j,m,n;
    double maxval,sum;
    
    if (!A)
        _xerror(_XE_NULL,"_xzmat_norm1");

    m = A->m; n = A->n;
    maxval = 0.0;
    
    for (j = 0; j < n; j++){
        sum = 0.0;
        for (i = 0; i < m; i ++)
            sum += _xzabs(A->me[i][j]);
        maxval = _xmax(maxval,sum);
    }
    
    return maxval;
}

/*! compute matrix infinity-norm -- unscaled -- complex version */
double _xzmat_norm_inf(_xzmat *A){
    int i,j,m,n;
    double maxval, sum;
    
    if (!A)
        _xerror(_XE_NULL,"_xzmat_norm_inf");
    
    m = A->m; n = A->n;
    maxval = 0.0;
    
    for (i = 0; i < m; i++){
        sum = 0.0;
        for (j = 0; j < n; j ++)
            sum += _xzabs(A->me[i][j]);
        maxval = _xmax(maxval,sum);
    }
    
    return maxval;
}

/*! compute matrix frobenius-norm -- unscaled */
double _xzmat_norm_frob(_xzmat *A){
    int i,j,m,n;
    double sum;
    
    if (!A)
        _xerror(_XE_NULL,"_xzmat_norm_frob");
    
    m = A->m; n = A->n;
    sum = 0.0;
    
    for (i = 0; i < m; i++)
        for (j = 0; j < n; j++)
            sum += _xsquare(A->me[i][j].re) + _xsquare(A->me[i][j].im);
    
    return sqrt(sum);
}

void _xzmat_dump(FILE *fp, _xzmat *a){
    unsigned int i, j, tmp;
     
    if (!a){
        fprintf(fp,"ComplexMatrix: NULL\n");
        return;
    }
    fprintf(fp,"ComplexMatrix: %d by %d @ 0x%lx\n",a->m,a->n,(long)a);
    fprintf(fp,"\tmax_m = %d, max_n = %d, max_size = %d\n",
            a->max_m, a->max_n, a->max_size);
    if (a->me == (_xcomplex **)NULL ){
        fprintf(fp,"NULL\n");
        return;
    }
    fprintf(fp,"a->me @ 0x%lx\n",(long)(a->me));
    fprintf(fp,"a->base @ 0x%lx\n",(long)(a->base));
    for (i=0; i<a->m; i++){ /* for each row... */
        fprintf(fp,"row %u: @ 0x%lx ",i,(long)(a->me[i]));
        for (j=0,tmp=1; j<a->n; j++, tmp++){ /* for each col in row... */
            fprintf(fp,_xzformat,a->me[i][j].re,a->me[i][j].im);
            if (!(tmp % 2)) putc('\n',fp);
        }
        if (tmp % 2 != 1) putc('\n',fp);
    }
}

void _xzmat_foutput(FILE *fp, _xzmat *a){
    unsigned int i,j,tmp;
     
    if (!a){
        fprintf(fp,"ComplexMatrix: NULL\n");
        return;
    }
    fprintf(fp,"ComplexMatrix: %d by %d\n",a->m,a->n);
    if (a->me == (_xcomplex **)NULL ){
        fprintf(fp,"NULL\n");
        return;
    }
    for (i=0; i<a->m; i++){   /* for each row... */
        fprintf(fp,"row %u: ",i);
        for (j=0, tmp=1; j<a->n; j++, tmp++){/* for each col in row... */
            fprintf(fp,_xzformat,a->me[i][j].re,a->me[i][j].im);
            if (!(tmp % 2)) putc('\n',fp);
        }
        if (tmp % 2 != 1) putc('\n',fp);
    }
}

_xzmat *_xzmat_ifinput(FILE *fp, _xzmat *mat){
    char c;
    unsigned int i,j,m,n,dynamic;
    /* dynamic set to _XTRUE if memory allocated here */
     
    /* get matrix size */
    if (mat && mat->m < _XMAXDIM && mat->n < _XMAXDIM){
        m = mat->m;
        n = mat->n;
        dynamic = _XFALSE;
    }
    else {
        dynamic = _XTRUE;
        do {
            fprintf(stderr,"ComplexMatrix: rows cols:");
            if (fgets(_xline,_XMAXLINE,fp)==NULL )
                _xerror(_XE_INPUT,"_xzmat_ifinput");
        } while (sscanf(_xline,"%u%u",&m,&n)<2 || m>_XMAXDIM || n>_XMAXDIM );
        mat = _xzmat_get(m,n);
    }
     
    /* input elements */
    for (i=0; i<m; i++){
    redo:
        fprintf(stderr,"row %u:\n",i);
        for (j=0; j<n; j++)
            do {
            redo2:
                fprintf(stderr,"entry (%u,%u): ",i,j);
                if (!dynamic)
                    fprintf(stderr,"old (%14.9g,%14.9g) new: ",
                            mat->me[i][j].re,mat->me[i][j].im);
                if (fgets(_xline,_XMAXLINE,fp)==NULL )
                    _xerror(_XE_INPUT,"_xzmat_ifinput");
                if ((*_xline == 'b' || *_xline == 'B') && j > 0){
                    j--;
                    dynamic = _XFALSE;
                    goto redo2;
                }
                if ((*_xline == 'f' || *_xline == 'F') && j < n-1){
                    j++;
                    dynamic = _XFALSE;
                    goto redo2;
                }
            } while (*_xline=='\0' ||
#if _XREAL == _XDOUBLE
              sscanf(_xline,"%lf%lf",
#elif _XREAL == _XFLOAT
              sscanf(_xline,"%f%f",
#endif  
              &mat->me[i][j].re,&mat->me[i][j].im)<1);
        fprintf(stderr,"Continue: ");
        fscanf(fp,"%c",&c);
        if (c == 'n' || c == 'N'){
            dynamic = _XFALSE;
            goto redo;
        }
        if ((c == 'b' || c == 'B') /* && i > 0 */){
            if (i > 0) i--;
            dynamic = _XFALSE;
            goto redo;
        }
    }
     
    return (mat);
}

_xzmat *_xzmat_bfinput(FILE *fp, _xzmat *mat){
    unsigned int i,j,m,n,dummy;
    int io_code;
     
    /* get dimension */
    _xskipjunk(fp);
    if ((io_code=fscanf(fp," ComplexMatrix: %u by %u",&m,&n)) < 2 ||
         m>_XMAXDIM || n>_XMAXDIM )
        _xerror(io_code==EOF ? _XE_EOF : _XE_FORMAT,"_xzmat_bfinput");
     
    /* allocate memory if necessary */
    if (!mat || mat->m<m || mat->n<n)
        mat = _xzmat_resize(mat,m,n);
     
    /* get entries */
    for (i=0; i<m; i++){
        _xskipjunk(fp);
        if (fscanf(fp," row %u:",&dummy) < 1)
            _xerror(_XE_FORMAT,"_xzmat_bfinput");
        for (j=0; j<n; j++){
#if _XREAL == _XDOUBLE
            if ((io_code=fscanf(fp," ( %lf , %lf )",
#elif _XREAL == _XFLOAT
            if ((io_code=fscanf(fp," ( %f , %f )",
#endif
                 &mat->me[i][j].re,&mat->me[i][j].im)) < 2 )
                _xerror(io_code==EOF ? _XE_EOF : _XE_FORMAT,"_xzmat_bfinput");
        }
    }
     
    return (mat);
}

_xzmat *_xzmat_finput(FILE *fp, _xzmat *a){
    _xzmat *_xzmat_ifinput(FILE *, _xzmat *),*_xzmat_bfinput(FILE *, _xzmat *);
     
    if (isatty(_fileno(fp)))
        return _xzmat_ifinput(fp,a);
    else
        return _xzmat_bfinput(fp,a);
}

/*! get a specified row of a matrix and retruns it as a vector */
_xzvec *_xzmat_get_row(_xzmat *mat, int row, _xzvec *vec){
    unsigned int lim;

    if (!mat)
        _xerror(_XE_NULL,"_xzmat_get_row");
    if (row < 0 || row >= (int)(mat->m))
        _xerror(_XE_RANGE,"_xzmat_get_row");
    if (!vec || vec->dim<mat->n )
        vec = _xzvec_resize(vec,mat->n);

    lim = _xmin(mat->n,vec->dim);
    _xmem_copy(mat->me[row],vec->ve,lim*sizeof(_xcomplex));

    return (vec);
}

/*! get a specified column of a matrix; returned as a vector */
_xzvec *_xzmat_get_col(_xzmat *mat, int col, _xzvec *vec){
    unsigned int i;

    if (!mat)
        _xerror(_XE_NULL,"_xzmat_get_col");
    if (col < 0 || col >= (int)(mat->n))
        _xerror(_XE_RANGE,"_xzmat_get_col");
    if (!vec || vec->dim<mat->m)
        vec = _xzvec_resize(vec,mat->m);

    for (i=0; i<mat->m; i++)
        vec->ve[i] = mat->me[i][col];

    return (vec);
}

/*! set row of matrix to values given in vec (in situ) */
_xzmat *_xzmat_set_row(_xzmat *mat, int row, _xzvec *vec){
    unsigned int lim;

    if (!mat || !vec)
        _xerror(_XE_NULL,"_xzmat_set_row");
    if ( row < 0 || row >= (int)(mat->m))
        _xerror(_XE_RANGE,"_xzmat_set_row");
    lim = _xmin(mat->n,vec->dim);
    _xmem_copy(vec->ve,mat->me[row],lim*sizeof(_xcomplex));

    return (mat);
}

/*! set column of matrix to values given in vec (in situ) */
_xzmat *_xzmat_set_col(_xzmat *mat, int col, _xzvec *vec){
    unsigned int i,lim;

    if (!mat || !vec)
        _xerror(_XE_NULL,"_xzmat_set_col");
    if (col < 0 || col >= (int)(mat->n))
        _xerror(_XE_RANGE,"_xzmat_set_col");
    lim = _xmin(mat->m,vec->dim);
    for (i=0; i<lim; i++)
        mat->me[i][col] = vec->ve[i];

    return (mat);
}

_xzmat *_xzmat_move(const _xzmat *in, int i0, int j0, int m0, int n0, _xzmat *out, int i1, int j1){
    int i;

    if (!in)
        _xerror(_XE_NULL,"_xzmat_move");
    if (i0 < 0 || j0 < 0 || i1 < 0 || j1 < 0 || m0 < 0 || n0 < 0 ||
        i0+m0 > (int)(in->m) || j0+n0 > (int)(in->n))
        _xerror(_XE_BOUNDS,"_xzmat_move");

    if (!out)
        out = _xzmat_resize(out,i1+m0,j1+n0);
    else if (i1+m0 > (int)(out->m) || j1+n0 > (int)(out->n))
        out = _xzmat_resize(out,_xmax((int)(out->m),i1+m0),_xmax((int)(out->n),j1+n0));

    for (i = 0; i < m0; i++)
        _xmem_copy(&(in->me[i0+i][j0]),&(out->me[i1+i][j1]),
                   n0*sizeof(_xcomplex));

    return out; 
}

_xzmat *_xzmat_copy(const _xzmat *in, _xzmat *out, int i0, int j0){
    unsigned int i;

    if (!in)
        _xerror(_XE_NULL,"_xzmat_copy");
    if (in==out)
        return (out);
    if (!out || out->m < in->m || out->n < in->n )
        out = _xzmat_resize(out,in->m,in->n);

    for (i=i0; i<in->m; i++)
        _xmem_copy(&(in->me[i][j0]),&(out->me[i][j0]),
                  (in->n - j0)*sizeof(_xcomplex));
                  
    return (out);
}

/*!
 *  copy selected piece of vector to a matrix
 *  -- moves the subvector with initial index i0 and length m1*n1 to
 *     the m1 x n1 submatrix with top-left co-ordinate (i1,j1)
 *  -- copying is done by rows
 *  -- out is resized if necessary
 */
_xzmat *_xzmat_vm_move(const _xzvec *in, int i0, _xzmat *out, int i1, int j1, int m1, int n1){
    int i,dim0;

    if (!in)
        _xerror(_XE_NULL,"_xzmat_vm_move");
    if (i0 < 0 || i1 < 0 || j1 < 0 || m1 < 0 || n1 < 0 ||
        i0+m1*n1 > (int)(in->dim) )
        _xerror(_XE_BOUNDS,"_xzmat_vm_move");

    if (!out)
        out = _xzmat_resize(out,i1+m1,j1+n1);
    else
        out = _xzmat_resize(out,_xmax(i1+m1,(int)(out->m)),_xmax(j1+n1,(int)(out->n)));

    dim0 = m1*n1;
    for (i=0; i<m1; i++)
        _xmem_copy(&(in->ve[i0+i*n1]),&(out->me[i1+i][j1]),n1*sizeof(_xcomplex));

    return out;
}

/*! matrix addition -- may be in-situ */         
_xzmat *_xzmat_add(_xzmat *mat1,_xzmat *mat2,_xzmat *out){
    unsigned int m,n,i;
    
    if (!mat1 || !mat2)
        _xerror(_XE_NULL,"_xzmat_add");
    if (mat1->m != mat2->m || mat1->n != mat2->n)
        _xerror(_XE_SIZES,"_xzmat_add");
    if (!out || out->m != mat1->m || out->n != mat1->n)
        out = _xzmat_resize(out,mat1->m,mat1->n);
    m = mat1->m; n = mat1->n;
    for (i=0; i<m; i++){
        __xzadd__(mat1->me[i],mat2->me[i],out->me[i],(int)n);
    }
    
    return (out);
}

/*! matrix subtraction -- may be in-situ */
_xzmat *_xzmat_sub(_xzmat *mat1,_xzmat *mat2,_xzmat *out){
    unsigned int i,m,n;
    
    if (!mat1 || !mat2)
        _xerror(_XE_NULL,"_xzmat_sub");
    if (mat1->m != mat2->m || mat1->n != mat2->n)
        _xerror(_XE_SIZES,"_xzmat_sub");
    if (!out || out->m != mat1->m || out->n != mat1->n)
        out = _xzmat_resize(out,mat1->m,mat1->n);
    m = mat1->m; n = mat1->n;
    for (i=0; i<m; i++){
        __xzsub__(mat1->me[i],mat2->me[i],out->me[i],(int)n);
    }
    
    return (out);
}

#define _xis_zero(z) ((z).re == 0.0 && (z).im == 0.0)

/*! matrix-matrix multiplication */
_xzmat *_xzmat_mlt(_xzmat *A,_xzmat *B,_xzmat *out){
    unsigned int i,k,m,n,p;
    _xcomplex **A_v,**B_v;
    
    if (!A || !B)
        _xerror(_XE_NULL,"_xzmat_mlt");
    if (A->n != B->m)
        _xerror(_XE_SIZES,"_xzmat_mlt");
    if (A == out || B == out)
        _xerror(_XE_INSITU,"_xzmat_mlt");
    m = A->m; n = A->n; p = B->n;
    A_v = A->me; B_v = B->me;
    
    if (!out || out->m != A->m || out->n != B->n)
        out = _xzmat_resize(out,A->m,B->n);
    
    _xzmat_zero(out);
    for (i=0; i<m; i++)
        for (k=0; k<n; k++){
            if (!_xis_zero(A_v[i][k]))
                __xzmltadd__(out->me[i],B_v[k],A_v[i][k],(int)p,0);
        }
    
    return out;
}

/*
 * Note: in the following routines, "adjoint" means complex conjugate
 * transpose: A* = conjugate(A^T)
 */
 
/*! 
 *  matrix-matrix adjoint multiplication
 *  -- A.B* is returned, and stored in out 
 */ 
_xzmat *_xzmat_ma_mlt(_xzmat *A,_xzmat *B,_xzmat *out){
    unsigned int i,j,limit;
    
    if (!A || !B)
        _xerror(_XE_NULL,"_xzmat_ma_mlt");
    if (A == out || B == out)
        _xerror(_XE_INSITU,"_xzmat_ma_mlt");
    if (A->n != B->n)
        _xerror(_XE_SIZES,"_xzmat_ma_mlt");
    if (!out || out->m != A->m || out->n != B->m)
        out = _xzmat_resize(out,A->m,B->m);
    
    limit = A->n;
    for (i=0; i<A->m; i++)
        for (j=0; j<B->m; j++)
            out->me[i][j] = __xzip__(B->me[j],A->me[i],(int)limit,1);
    
    return out;
}

/*!
 * matrix adjoint-matrix multiplication
 * -- A*.B is returned, result stored in out
 */
_xzmat *_xzmat_am_mlt(_xzmat *A,_xzmat *B,_xzmat *out){
    unsigned int i,k,limit;
    _xcomplex tmp;
    
    if (!A || !B)
        _xerror(_XE_NULL,"_xzmat_am_mlt");
    if (A == out || B == out)
        _xerror(_XE_INSITU,"_xzmat_am_mlt");
    if (A->m != B->m)
        _xerror(_XE_SIZES,"_xzmat_am_mlt");
    if (!out || out->m != A->n || out->n != B->n)
        out = _xzmat_resize(out,A->n,B->n);
    
    limit = B->n;
    _xzmat_zero(out);
    for (k=0; k<A->m; k++)
        for (i=0; i<A->n; i++){
            tmp.re =   A->me[k][i].re;
            tmp.im = - A->me[k][i].im;
            if (!_xis_zero(tmp) )
                __xzmltadd__(out->me[i],B->me[k],tmp,(int)limit,0);
        }
    
    return out;
}

/*! adjoint matrix */
_xzmat *_xzmat_adjoint(_xzmat *in,_xzmat *out){
    unsigned int i,j,in_situ;
    _xcomplex tmp;
    
    if (!in)
        _xerror(_XE_NULL,"_xzmat_adjoint");
    if (in == out && in->n != in->m )
        _xerror(_XE_INSITU2,"_xzmat_adjoint");
    in_situ = (in == out);
    if (!out || out->m != in->n || out->n != in->m)
        out = _xzmat_resize(out,in->n,in->m);
    
    if (!in_situ){
        for (i=0; i<in->m; i++)
            for (j=0; j<in->n; j++){
                out->me[j][i].re =   in->me[i][j].re;
                out->me[j][i].im = - in->me[i][j].im;
            }
    }
    else {
        for (i=0; i<in->m; i++){
            for (j=0; j<i; j++){
                tmp.re = in->me[i][j].re;
                tmp.im = in->me[i][j].im;
                in->me[i][j].re =   in->me[j][i].re;
                in->me[i][j].im = - in->me[j][i].im;
                in->me[j][i].re =   tmp.re;
                in->me[j][i].im = - tmp.im;
            }
            in->me[i][i].im = - in->me[i][i].im;
        }
    }
    
    return out;
}

/*! swap rows i and j of matrix A upto column lim */
_xzmat *_xzmat_swap_rows(_xzmat *A,int i,int j,int lo,int hi){
    int k;
    _xcomplex **A_me,tmp;
    
    if (!A)
        _xerror(_XE_NULL,"_xzmat_swap_rows");
    if (i < 0 || j < 0 || i >= (int)(A->m) || j >= (int)(A->m))
        _xerror(_XE_SIZES,"_xzmat_swap_rows");
    lo = _xmax(0,lo);
    hi = _xmin(hi,(int)(A->n-1));
    A_me = A->me;
    
    for (k=lo; k<=hi; k++){
        tmp = A_me[k][i];
        A_me[k][i] = A_me[k][j];
        A_me[k][j] = tmp;
    }
    
    return A;
}

/*! swap columns i and j of matrix A upto row lim */
_xzmat *_xzmat_swap_cols(_xzmat *A,int i,int j,int lo,int hi){
    int k;
    _xcomplex **A_me,tmp;
    
    if (!A)
        _xerror(_XE_NULL,"_xzmat_swap_cols");
    if (i < 0 || j < 0 || i >= (int)(A->n) || j >= (int)(A->n))
        _xerror(_XE_SIZES,"_xzmat_swap_cols");
    lo = _xmax(0,lo);
    hi = _xmin(hi,(int)(A->m-1));
    A_me = A->me;
    
    for (k=lo; k<=hi; k++){
        tmp = A_me[i][k];
        A_me[i][k] = A_me[j][k];
        A_me[j][k] = tmp;
    }
    
    return A;
}

/*!
 * matrix-scalar multiply and add
 * -- may be in situ
 * -- returns out == A1 + s*A2
 */
_xzmat *_xzmat_mz_mltadd(_xzmat *A1,_xzmat *A2, _xcomplex s, _xzmat *out){
    int i,m,n;
    
    if (!A1 || !A2)
        _xerror(_XE_NULL,"_xzmat_mz_mltadd");
    if (A1->m != A2->m || A1->n != A2->n)
        _xerror(_XE_SIZES,"_xzmat_mz_mltadd");

    if (out != A1 && out != A2)
        out = _xzmat_resize(out,A1->m,A1->n);
    
    if (s.re == 0.0 && s.im == 0.0)
        return _xzmat_mcopy(A1,out);
    if (s.re == 1.0 && s.im == 0.0)
        return _xzmat_add(A1,A2,out);
    
    out = _xzmat_mcopy(A1,out);
    
    m = A1->m; n = A1->n;
    for (i=0; i<m; i++)
        __xzmltadd__(out->me[i],A2->me[i],s,(int)n,0);
    
    return out;
}

/*! scalar-matrix multiply -- may be in-situ */
_xzmat *_xzmat_sm_mlt(_xcomplex scalar,_xzmat * matrix,_xzmat *out){
    unsigned int i,m,n;
    
    if (!matrix)
        _xerror(_XE_NULL,"_xzmat_sm_mlt");
    if (!out || out->m != matrix->m || out->n != matrix->n)
        out = _xzmat_resize(out,matrix->m,matrix->n);
    m = matrix->m; n = matrix->n;
    for (i=0; i<m; i++)
        __xzmlt__(matrix->me[i],scalar,out->me[i],(int)n);

    return (out);
}

/*!
 * copy selected piece of matrix to a vector
 * -- moves the m0 x n0 submatrix with top-left co-ordinate (i0,j0) to
 *    the subvector with initial index i1 (and length m0*n0)
 * -- rows are copied contiguously
 * -- out is resized if necessary 
 */
_xzvec *_xzmat_mv_move(const _xzmat *in, int i0, int j0, int m0, int n0, _xzvec *out, int i1){
    int i,dim1;

    if (!in)
        _xerror(_XE_NULL,"_xzmat_mv_move");
    if (i0 < 0 || j0 < 0 || m0 < 0 || n0 < 0 || i1 < 0 ||
        i0+m0 > (int)(in->m) || j0+n0 > (int)(in->n))
        _xerror(_XE_BOUNDS,"_xzmat_mv_move");

    dim1 = m0*n0;
    if (!out || i1+dim1 > (int)(out->dim))
        out = _xzvec_resize(out,i1+dim1);

    for (i=0; i<m0; i++)
        _xmem_copy(&(in->me[i0+i][j0]),&(out->ve[i1+i*n0]),n0*sizeof(_xcomplex));

    return out;
}

/*!
 * matrix-vector multiplication 
 * -- Note: b is treated as a column vector 
 */
_xzvec *_xzmat_mv_mlt(_xzmat *A,_xzvec *b,_xzvec *out){
    unsigned int i,m,n;
    _xcomplex **A_v,*b_v;
    
    if (!A || !b)
        _xerror(_XE_NULL,"_xzmat_mv_mlt");
    if (A->n != b->dim )
        _xerror(_XE_SIZES,"_xzmat_mv_mlt");
    if (b == out)
        _xerror(_XE_INSITU,"_xzmat_mv_mlt");
    if (!out || out->dim != A->m)
        out = _xzvec_resize(out,A->m);
    
    m = A->m; n = A->n;
    A_v = A->me; b_v = b->ve;
    for (i=0; i<m; i++)
        out->ve[i] = __xzip__(A_v[i],b_v,(int)n,0);
    
    return out;
}

/*! vector adjoint-matrix multiplication */
_xzvec *_xzmat_vm_mlt(_xzmat *A,_xzvec *b,_xzvec *out){
    unsigned int j,m,n;
    
    if (!A || !b)
        _xerror(_XE_NULL,"_xzmat_vm_mlt");
    if (A->m != b->dim)
        _xerror(_XE_SIZES,"_xzmat_vm_mlt");
    if (b == out)
        _xerror(_XE_INSITU,"_xzmat_vm_mlt");
    if (!out || out->dim != A->n)
        out = _xzvec_resize(out,A->n);
    
    m = A->m; n = A->n;
    _xzvec_zero(out);
    for (j=0; j<m; j++)
        if (b->ve[j].re != 0.0 || b->ve[j].im != 0.0)
            __xzmltadd__(out->ve,A->me[j],b->ve[j],(int)n,1);
    
    return out;
}

/*!
 * matrix-vector multiply and add
 * -- may not be in situ
 */
_xzvec *_xzmat_mv_mltadd(_xzvec *v1,_xzvec *v2,_xzmat *A,_xcomplex alpha,_xzvec *out){
    int i,m,n;
    _xcomplex tmp,*v2_ve,*out_ve;
    
    if (!v1 || !v2 || !A)
        _xerror(_XE_NULL,"_xzmat_mv_mltadd");
    if (out == v2)
        _xerror(_XE_INSITU,"_xzmat_mv_mltadd");
    if (v1->dim != A->m || v2->dim != A-> n )
        _xerror(_XE_SIZES,"_xzmat_mv_mltadd");
    
    _xtracecatch(out = _xzvec_vcopy(v1,out),"_xzmat_mv_mltadd");
    
    v2_ve = v2->ve; out_ve = out->ve;
    m = A->m; n = A->n;
    
    if (alpha.re == 0.0 && alpha.im == 0.0)
        return out;
    
    for (i=0; i<m; i++){
        tmp = __xzip__(A->me[i],v2_ve,(int)n,0);
        out_ve[i].re += alpha.re*tmp.re - alpha.im*tmp.im;
        out_ve[i].im += alpha.re*tmp.im + alpha.im*tmp.re;
    }
    
    return out;
}

/*!
 * vector-matrix multiply and add
 * -- may not be in situ
 * -- returns out == v1 + v2*.A
 */
_xzvec *_xzmat_vm_mltadd(_xzvec *v1,_xzvec *v2,_xzmat *A,_xcomplex alpha,_xzvec *out){
    int j,m,n;
    _xcomplex tmp,*out_ve;
    
    if (!v1 || !v2 || !A)
        _xerror(_XE_NULL,"_xzmat_vm_mltadd");
    if (v2 == out)
        _xerror(_XE_INSITU,"_xzmat_vm_mltadd");
    if (v1->dim != A->n || A->m != v2->dim)
        _xerror(_XE_SIZES,"_xzmat_vm_mltadd");
    
    _xtracecatch(out = _xzvec_vcopy(v1,out),"_xzmat_vm_mltadd");
    
    out_ve = out->ve; m = A->m; n = A->n;
    for (j=0; j<m; j++){
        tmp.re = v2->ve[j].re*alpha.re - v2->ve[j].im*alpha.im;
        tmp.im = v2->ve[j].re*alpha.im + v2->ve[j].im*alpha.re;
        if (tmp.re != 0.0 || tmp.im != 0.0)
            __xzmltadd__(out_ve,A->me[j],tmp,(int)n,1);
    }
    
    return out;
}

/*!
 * back substitution with optional over-riding diagonal
 * -- can be in-situ but doesn't need to be
 */
_xzvec *_xzmat_usolve(_xzmat *matrix, _xzvec *b, _xzvec *out, double diag){
    unsigned int dim;
    int i,i_lim;
    _xcomplex **mat_ent,*mat_row,*b_ent,*out_ent,*out_col,sum;
    
    if (!matrix || !b)
        _xerror(_XE_NULL,"_xzmat_usolve");
    dim = _xmin(matrix->m,matrix->n);
    if (b->dim < dim)
        _xerror(_XE_SIZES,"_xzmat_usolve");
    if (!out || out->dim < dim)
        out = _xzvec_resize(out,matrix->n);
    mat_ent = matrix->me; b_ent = b->ve; out_ent = out->ve;
    
    for (i=dim-1; i>=0; i--)
        if (!_xis_zero(b_ent[i]))
            break;
        else
            out_ent[i].re = out_ent[i].im = 0.0f;
            
    i_lim = i;
    for (i=i_lim; i>=0; i--){
        sum = b_ent[i];
        mat_row = &(mat_ent[i][i+1]);
        out_col = &(out_ent[i+1]);
        sum = _xzsub(sum,__xzip__(mat_row,out_col,i_lim-i,0));
        if (diag == 0.0){
            if (_xis_zero(mat_ent[i][i]))
                _xerror(_XE_SING,"_xzmat_usolve");
            else
                out_ent[i] = _xzdiv(sum,mat_ent[i][i]);
        }
        else {
            out_ent[i].re = (_xreal)(sum.re / diag);
            out_ent[i].im = (_xreal)(sum.im / diag);
        }
    }
    
    return (out);
}

/*! forward elimination with (optional) default diagonal value */
_xzvec *_xzmat_lsolve(_xzmat *matrix, _xzvec *b, _xzvec *out, double diag){
    unsigned int dim,i,i_lim;
    _xcomplex **mat_ent,*mat_row,*b_ent,*out_ent,*out_col,sum;
    
    if (!matrix || !b)
        _xerror(_XE_NULL,"_xzmat_lsolve");
    dim = _xmin(matrix->m,matrix->n);
    if (b->dim < dim)
        _xerror(_XE_SIZES,"_xzmat_lsolve");
    if (!out || out->dim < dim )
        out = _xzvec_resize(out,matrix->n);
    mat_ent = matrix->me; b_ent = b->ve; out_ent = out->ve;
    
    for (i=0; i<dim; i++)
        if (!_xis_zero(b_ent[i]))
            break;
        else
            out_ent[i].re = out_ent[i].im = 0.0f;
            
    i_lim = i;
    for (i=i_lim; i<dim; i++){
        sum = b_ent[i];
        mat_row = &(mat_ent[i][i_lim]);
        out_col = &(out_ent[i_lim]);
        sum = _xzsub(sum,__xzip__(mat_row,out_col,(int)(i-i_lim),0));
        if (diag == 0.0){
            if (_xis_zero(mat_ent[i][i]))
                _xerror(_XE_SING,"_xzmat_lsolve");
            else
                out_ent[i] = _xzdiv(sum,mat_ent[i][i]);
        }
        else {
            out_ent[i].re = (_xreal)(sum.re / diag);
            out_ent[i].im = (_xreal)(sum.im / diag);
        }
    }
    
    return (out);
}

/*! 
 * forward elimination with (optional) default diagonal value
 * using UPPER triangular part of matrix
 */
_xzvec *_xzmat_uasolve(_xzmat *U, _xzvec *b, _xzvec *out, double diag){
    unsigned int dim,i,i_lim;
    _xcomplex **U_me,*b_ve,*out_ve,tmp;
    double invdiag;
    
    if (!U || !b)
        _xerror(_XE_NULL,"_xzmat_uasolve");
    dim = _xmin(U->m,U->n);
    if (b->dim < dim)
        _xerror(_XE_SIZES,"_xzmat_uasolve");
    out = _xzvec_resize(out,U->n);
    U_me = U->me; b_ve = b->ve; out_ve = out->ve;
    
    for (i=0; i<dim; i++)
        if (!_xis_zero(b_ve[i]))
            break;
        else
            out_ve[i].re = out_ve[i].im = 0.0f;
            
    i_lim = i;
    if (b != out){
        __xzzero__(out_ve,out->dim);
        _xmem_copy(&(b_ve[i_lim]),&(out_ve[i_lim]),dim-i_lim*sizeof(_xcomplex));
    }

    if (diag == 0.0){
        for ( ; i<dim; i++){
            tmp = _xzconj(U_me[i][i]);
            if (_xis_zero(tmp))
                _xerror(_XE_SING,"_xzmat_uasolve");
            out_ve[i] = _xzdiv(out_ve[i],tmp);
            tmp.re = - out_ve[i].re;
            tmp.im = - out_ve[i].im;
            __xzmltadd__(&(out_ve[i+1]),&(U_me[i][i+1]),tmp,dim-i-1,1);
        }
    }
    else {
        invdiag = 1.0/diag;
        for ( ; i<dim; i++){
            out_ve[i].re *= invdiag;
            out_ve[i].im *= invdiag;
            tmp.re = - out_ve[i].re;
            tmp.im = - out_ve[i].im;
            __xzmltadd__(&(out_ve[i+1]),&(U_me[i][i+1]),tmp,dim-i-1,1);
        }
    }
    
    return (out);
}

/*! solve Dx=b where D is the diagonal of A -- may be in-situ */
_xzvec *_xzmat_dsolve(_xzmat *A, _xzvec *b, _xzvec *x){
    unsigned int dim,i;
    
    if (!A || !b)
        _xerror(_XE_NULL,"_xzmat_dsolve");
    dim = min(A->m,A->n);
    if (b->dim < dim)
        _xerror(_XE_SIZES,"_xzmat_dsolve");
    x = _xzvec_resize(x,A->n);
    
    dim = b->dim;
    for (i=0; i<dim; i++)
        if (_xis_zero(A->me[i][i]))
            _xerror(_XE_SING,"_xzmat_dsolve");
        else
            x->ve[i] = _xzdiv(b->ve[i],A->me[i][i]);
    
    return (x);
}

/*!
 * back substitution with optional over-riding diagonal
 * using the LOWER triangular part of matrix
 * -- can be in-situ but doesn't need to be
 */
_xzvec *_xzmat_lasolve(_xzmat *L, _xzvec *b, _xzvec *out, double diag){
    unsigned int dim;
    int i,i_lim;
    _xcomplex **L_me,*b_ve,*out_ve,tmp;
    double invdiag;
    
    if (!L || !b)
        _xerror(_XE_NULL,"_xzmat_lasolve");
    dim = _xmin(L->m,L->n);
    if (b->dim < dim)
        _xerror(_XE_SIZES,"_xzmat_lasolve");
    out = _xzvec_resize(out,L->n);
    L_me = L->me; b_ve = b->ve; out_ve = out->ve;
    
    for (i=dim-1; i>=0; i--)
        if (!_xis_zero(b_ve[i]))
            break;
            
    i_lim = i;
    if (b != out){
        __xzzero__(out_ve,out->dim);
        _xmem_copy(b_ve,out_ve,i_lim+1*sizeof(_xcomplex));
    }

    if (diag == 0.0){
        for ( ; i>=0; i--){
            tmp = _xzconj(L_me[i][i]);
            if (_xis_zero(tmp))
                _xerror(_XE_SING,"_xzmat_lasolve");
            out_ve[i] = _xzdiv(out_ve[i],tmp);
            tmp.re = - out_ve[i].re;
            tmp.im = - out_ve[i].im;
            __xzmltadd__(out_ve,L_me[i],tmp,i,1);
        }
    }
    else {
        invdiag = 1.0/diag;
        for ( ; i>=0; i--){
            out_ve[i].re *= invdiag;
            out_ve[i].im *= invdiag;
            tmp.re = - out_ve[i].re;
            tmp.im = - out_ve[i].im;
            __xzmltadd__(out_ve,L_me[i],tmp,i,1);
        }
    }
    
    return (out);
}

/*!
 *  solve Qx = b, Q is an orthogonal matrix stored in compact
 *  form a la QR factor
 *  -- may be in-situ
 */
_xzvec *_xzmat_qsolve(_xzmat *QR, _xzvec *diag, _xzvec *b, _xzvec *x, _xzvec *tmp){
    unsigned int dynamic;
    int k,limit;
    double beta,r_ii,tmp_val;
    
    limit = min(QR->m,QR->n);
    dynamic = _XFALSE;
    if (!QR || !diag || !b)
        _xerror(_XE_NULL,"_xzmat_qsolve");
    if (diag->dim < limit || b->dim != QR->m )
        _xerror(_XE_SIZES,"_xzmat_qsolve");
    x = _xzvec_resize(x,QR->m);
    if (!tmp)
        dynamic = _XTRUE;
    tmp = _xzvec_resize(tmp,QR->m);
    
    /* apply H/holder transforms in normal order */
    x = _xzvec_vcopy(b,x);
    for (k = 0 ; k<limit ; k++){
        _xzmat_get_col(QR,k,tmp);
        r_ii = _xzabs(tmp->ve[k]);
        tmp->ve[k] = diag->ve[k];
        tmp_val = (r_ii*_xzabs(diag->ve[k]));
        beta = (tmp_val == 0.0) ? 0.0 : 1.0/tmp_val;
        _xzvec_hhtrvec(tmp,beta,k,x,x);
    }
    
    if (dynamic)
        _xzvec_vfree(tmp);
    
    return (x);
}

/*!
 * calulates Householder vector to eliminate all entries after the
 * i0 entry of the vector vec. It is returned as out. May be in-situ
 */
_xzvec *_xzvec_hhvec(_xzvec *vec, int i0, _xreal *beta, _xzvec *out, _xcomplex *newval){
    _xcomplex tmp;
    double norm,abs_val;

    if (i0 < 0 || i0 >= vec->dim)
        _xerror(_XE_BOUNDS,"_xzvec_hhvec");
    out = _xzvec_copy(vec,out,i0);
    tmp = _xzvec_in_prod(out,out,i0,1);
    if (tmp.re <= 0.0f){
        *beta = 0.0f;
        *newval = out->ve[i0];
        return (out);
    }
    norm = sqrt(tmp.re);
    abs_val = _xzabs(out->ve[i0]);
    *beta = (_xreal)(1.0/(norm * (norm+abs_val)));
    if (abs_val == 0.0){
        newval->re = (_xreal)(norm);
        newval->im = 0.0f;
    }
    else { 
        abs_val    = -norm / abs_val;
        newval->re = (_xreal)(abs_val*out->ve[i0].re);
        newval->im = (_xreal)(abs_val*out->ve[i0].im);
    } 
      
    abs_val         = -norm / abs_val;
    out->ve[i0].re -= newval->re;
    out->ve[i0].im -= newval->im;

    return (out);
}

/*!
 * apply Householder transformation to vector -- may be in-situ
 */
_xzvec *_xzvec_hhtrvec(_xzvec *hh, double beta, int i0, _xzvec *in, _xzvec *out){
    _xcomplex scale,tmp;

    if (!hh || !in)
        _xerror(_XE_NULL,"_xzvec_hhtrvec");
    if (in->dim != hh->dim)
        _xerror(_XE_SIZES,"_xzvec_hhtrvec");
    if (i0 < 0 || i0 > in->dim)
        _xerror(_XE_BOUNDS,"_xzvec_hhtrvec");

    tmp = _xzvec_in_prod(hh,in,i0,1);
    scale.re = -beta*tmp.re;
    scale.im = -beta*tmp.im;
    out = _xzvec_vcopy(in,out);
    __xzmltadd__(&(out->ve[i0]),&(hh->ve[i0]),scale,
                  (int)(in->dim-i0),0);

    return (out);
}

/*! given an LU factorisation in A, solve Ax=b */
_xzvec *_xzmat_lusolve(_xzmat *A, _xperm *pivot, _xzvec *b, _xzvec *x){
    if (!A || !b || !pivot)
        _xerror(_XE_NULL,"_xzmat_lusolve");
    if (A->m != A->n || A->n != b->dim )
        _xerror(_XE_SIZES,"_xzmat_lusolve");

    x = _xperm_zvec(pivot,b,x); /* x := P.b */
    _xzmat_lsolve(A,x,x,1.0); /* implicit diagonal = 1 */
    _xzmat_usolve(A,x,x,0.0); /* explicit diagonal */

    return (x);
}

/*! given an LU factorisation in A, solve A^*.x=b */
_xzvec *_xzmat_luasolve(_xzmat *LU, _xperm *pivot, _xzvec *b, _xzvec *x){
    if (!LU || !b || !pivot)
        _xerror(_XE_NULL,"_xzmat_luasolve");
    if (LU->m != LU->n || LU->n != b->dim )
        _xerror(_XE_SIZES,"_xzmat_luasolve");

    x = _xzvec_vcopy(b,x);
    _xzmat_uasolve(LU,x,x,0.0);   /* explicit diagonal */
    _xzmat_lasolve(LU,x,x,1.0);   /* implicit diagonal = 1 */
    _xperm_inv_zvec(pivot,x,x);   /* x := P^*.x */

    return (x);
}

/*!
 * solve the system Q.R.x=b where Q & R are stored in compact form
 * -- return x, which is created if necessary
 */
_xzvec *_xzmat_qrsolve(_xzmat *QR, _xzvec *diag, _xzvec *b, _xzvec *x){
    int limit;
    _XSTATIC _xzvec *tmp = (_xzvec *)NULL;
    
    if (!QR || !diag || !b)
        _xerror(_XE_NULL,"_xzmat_qrsolve");
    limit = _xmin(QR->m,QR->n);
    if (diag->dim < limit || b->dim != QR->m)
        _xerror(_XE_SIZES,"_xzmat_qrsolve");
    tmp = _xzvec_resize(tmp,limit);
    _XMEM_STAT_REG(tmp,_XTYPE_ZVEC);

    x = _xzvec_resize(x,QR->n);
    _xzmat_qsolve(QR,diag,b,x,tmp);
    x = _xzmat_usolve(QR,x,x,0.0);
    x = _xzvec_resize(x,QR->n);

#ifdef _XTHREAD_SAFE
    _xzvec_vfree(tmp);
#endif

    return x;
}

/*!
 * solve the system (Q.R)*.x = b
 * -- Q & R are stored in compact form
 * -- returns x, which is created if necessary
 */
_xzvec *_xzmat_qrasolve(_xzmat *QR, _xzvec *diag, _xzvec *b, _xzvec *x){
    int j;
    unsigned int limit;
    double beta,r_ii,tmp_val;
    _XSTATIC _xzvec *tmp = (_xzvec *)NULL;
    
    if (!QR || !diag || !b)
        _xerror(_XE_NULL,"_xzmat_qrasolve");
    limit = _xmin(QR->m,QR->n);
    if (diag->dim < limit || b->dim != QR->n)
        _xerror(_XE_SIZES,"_xzmat_qrasolve");

    x = _xzvec_resize(x,QR->m);
    x = _xzmat_uasolve(QR,b,x,0.0);
    x = _xzvec_resize(x,QR->m);

    tmp = _xzvec_resize(tmp,x->dim);
    _XMEM_STAT_REG(tmp,_XTYPE_ZVEC);
    
    /* apply H/h transforms in reverse order */
    for (j=limit-1; j>=0; j--){
        _xzmat_get_col(QR,j,tmp);
        tmp = _xzvec_resize(tmp,QR->m);
        r_ii = _xzabs(tmp->ve[j]);
        tmp->ve[j] = diag->ve[j];
        tmp_val = (r_ii*_xzabs(diag->ve[j]));
        beta = ( tmp_val == 0.0 ) ? 0.0 : 1.0/tmp_val;
        _xzvec_hhtrvec(tmp,beta,j,x,x);
    }

#ifdef _XTHREAD_SAFE
    _xzvec_vfree(tmp);
#endif

    return x;
}

/*!
 * solve A.x = b where A is factored by QRCP factor
 * -- assume that A is in the compact factored form
 */
_xzvec *_xzmat_qrcpsolve(_xzmat *QR, _xzvec *diag, _xperm *pivot, _xzvec *b, _xzvec *x){
    if (!QR || !diag || !pivot || !b)
        _xerror(_XE_NULL,"_xzmat_qrcpsolve");
    if ((QR->m > diag->dim && QR->n > diag->dim) || QR->n != pivot->size )
        _xerror(_XE_SIZES,"_xzmat_qrcpsolve");
    
    x = _xzmat_qrsolve(QR,diag,b,x);
    x = _xperm_inv_zvec(pivot,x,x);

    return x;
}

/*! compute out = upper_triang(U).x -- may be in situ */
_xzvec *_xzmat_umlt(_xzmat *U, _xzvec *x, _xzvec *out){
    int i,limit;

    if (!U || !x)
        _xerror(_XE_NULL,"_xzmat_umlt");
    limit = _xmin(U->m,U->n);
    if (limit != x->dim )
        _xerror(_XE_SIZES,"_xzmat_umlt");
    if (!out || out->dim < limit)
        out = _xzvec_resize(out,limit);

    for (i=0; i<limit; i++)
        out->ve[i] = __xzip__(&(x->ve[i]),&(U->me[i][i]),limit - i,0);
        
    return out;
}

/*! return out = upper_triang(U)^T.x */
_xzvec *_xzmat_uamlt(_xzmat *U, _xzvec *x, _xzvec *out){
    _xcomplex tmp;
    int i,limit;

    if (!U || !x)
        _xerror(_XE_NULL,"_xzmat_uamlt");
    limit = _xmin(U->m,U->n);
    if (!out || out->dim < limit)
        out = _xzvec_resize(out,limit);

    for (i=limit-1; i>=0; i--){
        tmp = x->ve[i];
        out->ve[i].re = out->ve[i].im = 0.0f;
        __xzmltadd__(&(out->ve[i]),&(U->me[i][i]),tmp,limit-i-1,1);
    }

    return out;
}

/*! apply Givens rotation to x's i & k components */
_xzvec *_xzvec_rot_vec(_xzvec *x, int i, int k, double c, _xcomplex s, _xzvec *out){
    _xcomplex temp1,temp2;

    if (!x)
        _xerror(_XE_NULL,"_xzvec_rot_vec");
    if (i < 0 || i >= x->dim || k < 0 || k >= x->dim )
        _xerror(_XE_RANGE,"_xzvec_rot_vec");
    if (x != out)
        out = _xzvec_vcopy(x,out);

    temp1.re = c*out->ve[i].re
               - s.re*out->ve[k].re + s.im*out->ve[k].im;
    temp1.im = c*out->ve[i].im
               - s.re*out->ve[k].im - s.im*out->ve[k].re;
               
    temp2.re = c*out->ve[k].re
               + s.re*out->ve[i].re + s.im*out->ve[i].im;
    temp2.im = c*out->ve[k].im
               + s.re*out->ve[i].im - s.im*out->ve[i].re;

    out->ve[i] = temp1;
    out->ve[k] = temp2;

    return (out);
}

/*!
 * transform a matrix by a Householder vector by rows
 * starting at row i0 from column j0 
 * -- in-situ
 * -- that is, M(i0:m,j0:n) <- M(i0:m,j0:n)(I-beta.hh(j0:n).hh(j0:n)^T)
 */
_xzmat *_xzmat_hhtrrows(_xzmat *M, int i0, int j0, _xzvec *hh, double beta){
    _xcomplex ip,scale;
    int i;

    if (!M || !hh)
        _xerror(_XE_NULL,"_xzmat_hhtrrows");
    if (M->n != hh->dim)
        _xerror(_XE_RANGE,"_xzmat_hhtrrows");
    if (i0 < 0 || i0 > M->m || j0 < 0 || j0 > M->n)
        _xerror(_XE_BOUNDS,"_xzmat_hhtrrows");

    if (beta == 0.0)
        return (M);

    /* for each row ... */
    for (i=i0; i<M->m; i++){ /* compute inner product */
        ip = __xzip__(&(M->me[i][j0]),&(hh->ve[j0]),
                     (int)(M->n-j0),0);
        scale.re = -beta*ip.re;
        scale.im = -beta*ip.im;
        if (_xis_zero(scale))
            continue;

        /* do operation */
        __xzmltadd__(&(M->me[i][j0]),&(hh->ve[j0]),scale,
                    (int)(M->n-j0),1);
    }

    return (M);
}

/*!
 * transform a matrix by a Householder vector by columns
 * starting at row i0 from column j0 
 * -- that is, M(i0:m,j0:n) <- (I-beta.hh(i0:m).hh(i0:m)^T)M(i0:m,j0:n)
 * -- in-situ
 * -- calls _xzmat_hhvtrcols() with the scratch vector w
 * -- Meschach internal routines should call _xzmat_hhvtrcols() to
 * avoid excessive memory allocation/de-allocation
 */
_xzmat *_xzmat_hhtrcols(_xzmat *M, int i0, int j0, _xzvec *hh, double beta){
    _XSTATIC _xzvec *w = (_xzvec *)NULL;

    if (!M || !hh)
        _xerror(_XE_NULL,"_xzmat_hhtrcols");
    if (M->m != hh->dim )
        _xerror(_XE_SIZES,"_xzmat_hhtrcols");
    if (i0 < 0 || i0 > M->m || j0 < 0 || j0 > M->n )
        _xerror(_XE_BOUNDS,"_xzmat_hhtrcols");

    if (beta == 0.0 )
        return (M);

    if (!w || w->dim < M->n)
        w = _xzvec_resize(w,M->n);
    _XMEM_STAT_REG(w,_XTYPE_ZVEC);

    M = _xzmat_hhvtrcols(M,i0,j0,hh,beta,w);

#ifdef _XTHREAD_SAFE
    _xzvec_vfree(w);
#endif

    return M;
}

/*!
 * transform a matrix by a Householder vector by columns
 * starting at row i0 from column j0 
 * -- that is, M(i0:m,j0:n) <- (I-beta.hh(i0:m).hh(i0:m)^T)M(i0:m,j0:n)
 * -- in-situ
 * -- scratch vector w passed as argument
 * -- raises error if w == NULL
 */
_xzmat *_xzmat_hhvtrcols(_xzmat *M, int i0, int j0, _xzvec *hh, double beta, _xzvec *w){
    _xcomplex scale;
    int i;

    if (!M || !hh)
        _xerror(_XE_NULL,"_xzmat_hhvtrcols");
    if (M->m != hh->dim )
        _xerror(_XE_SIZES,"_xzmat_hhvtrcols");
    if (i0 < 0 || i0 > M->m || j0 < 0 || j0 > M->n )
        _xerror(_XE_BOUNDS,"_xzmat_hhvtrcols");

    if (beta == 0.0)
        return (M);

    if (w->dim < M->n )
        w = _xzvec_resize(w,M->n);
    _xzvec_zero(w);

    for (i=i0; i<M->m; i++)
        if (!_xis_zero(hh->ve[i]))
            __xzmltadd__(&(w->ve[j0]),&(M->me[i][j0]),hh->ve[i],
                        (int)(M->n-j0),1);
    for (i=i0; i<M->m; i++)
        if (!_xis_zero(hh->ve[i])){
            scale.re = -beta*hh->ve[i].re;
            scale.im = -beta*hh->ve[i].im;
            __xzmltadd__(&(M->me[i][j0]),&(w->ve[j0]),scale,
                        (int)(M->n-j0),1);
        }

    return (M);
}

/*!
 * compute Hessenberg factorisation in compact form.
 * -- factorisation performed in situ
 * -- for details of the compact form
 */
_xzmat *_xzmat_hfactor(_xzmat *A, _xzvec *diag){
    _XSTATIC _xzvec *tmp1 = (_xzvec *)NULL, *w = (_xzvec *)NULL;
    _xreal beta;
    int k,limit;

    if (!A || !diag)
        _xerror(_XE_NULL,"_xzmat_hfactor");
    if (diag->dim < A->m - 1 )
        _xerror(_XE_SIZES,"_xzmat_hfactor");
    if (A->m != A->n)
        _xerror(_XE_SQUARE,"_xzmat_hfactor");
    limit = A->m - 1;

    tmp1 = _xzvec_resize(tmp1,A->m);
    w    = _xzvec_resize(w,A->n);
    _XMEM_STAT_REG(tmp1,_XTYPE_ZVEC);
    _XMEM_STAT_REG(w,   _XTYPE_ZVEC);

    for (k=0; k<limit; k++){
        _xzmat_get_col(A,k,tmp1);
        _xzvec_hhvec(tmp1,k+1,&beta,tmp1,&A->me[k+1][k]);
        diag->ve[k] = tmp1->ve[k+1];
        
        _xzmat_hhvtrcols(A,k+1,k+1,tmp1,beta,w);
        _xzmat_hhtrrows(A,0,k+1,tmp1,beta);
    }

#ifdef _XTHREAD_SAFE
    _xzvec_vfree(tmp1); _xzvec_vfree(w);
#endif

    return (A);
}

/*!
 * unpack the compact representation of H and Q of a
 * Hessenberg factorisation
 * -- if either H or Q is NULL, then it is not unpacked
 * -- it can be in situ with HQ == H
 * -- return HQ
 */
_xzmat *_xzmat_hqunpack(_xzmat *HQ, _xzvec *diag, _xzmat *Q, _xzmat *H){
    int i,j,limit;
    double beta,r_ii,tmp_val;
    _XSTATIC _xzvec *tmp1 = (_xzvec *)NULL, *tmp2 = (_xzvec *)NULL;

    if (!HQ || !diag)
        _xerror(_XE_NULL,"_xzmat_hqunpack");
    if (HQ == Q || H == Q )
        _xerror(_XE_INSITU,"_xzmat_hqunpack");
    limit = HQ->m - 1;
    if (diag->dim < limit )
        _xerror(_XE_SIZES,"_xzmat_hqunpack");
    if (HQ->m != HQ->n )
        _xerror(_XE_SQUARE,"_xzmat_hqunpack");

    if (Q){
        Q = _xzmat_resize(Q,HQ->m,HQ->m);
        tmp1 = _xzvec_resize(tmp1,H->m);
        tmp2 = _xzvec_resize(tmp2,H->m);
        _XMEM_STAT_REG(tmp1,_XTYPE_ZVEC);
        _XMEM_STAT_REG(tmp2,_XTYPE_ZVEC);
        
        for (i=0; i<H->m; i++){
            for (j=0; j<H->m; j++)
                tmp1->ve[j].re = tmp1->ve[j].im = 0.0f;
            tmp1->ve[i].re = 1.0f;
        
            /* apply H/h transforms in reverse order */
            for (j=limit-1; j>=0; j--){
                _xzmat_get_col(HQ,j,tmp2);
                r_ii = _xzabs(tmp2->ve[j+1]);
                tmp2->ve[j+1] = diag->ve[j];
                tmp_val = (r_ii*_xzabs(diag->ve[j]));
                beta = ( tmp_val == 0.0 ) ? 0.0 : 1.0/tmp_val;
                _xzvec_hhtrvec(tmp2,beta,j+1,tmp1,tmp1);
            }
        
            /* insert into Q */
            _xzmat_set_col(Q,i,tmp1);
        }
    }

    if (H){
        H = _xzmat_mcopy(HQ,_xzmat_resize(H,HQ->m,HQ->n));
        
        limit = H->m;
        for (i=1; i<limit; i++)
            for (j=0; j<i-1; j++)
                H->me[i][j].re = H->me[i][j].im = 0.0f;
    }

#ifdef _XTHREAD_SAFE
    _xzvec_vfree(tmp1); _xzvec_vfree(tmp2);
#endif

    return HQ;
}

/*!
 * Note: The usual representation of a Householder transformation is taken
 * to be:
 * P = I - beta.u.u*
 * where beta = 2/(u*.u) and u is called the Householder vector
 * (u* is the conjugate transposed vector of u
 *
 * forms the QR factorisation of A
 * -- factorisation stored in compact form as described above
 * (not quite standard format)
 */
_xzmat *_xzmat_qrfactor(_xzmat *A, _xzvec *diag){
    unsigned int k,limit;
    _xreal beta;
    _XSTATIC _xzvec *tmp1=(_xzvec *)NULL, *w=(_xzvec *)NULL;
    
    if (!A || !diag)
        _xerror(_XE_NULL,"_xzmat_qrfactor");
    limit = _xmin(A->m,A->n);
    if (diag->dim < limit)
        _xerror(_XE_SIZES,"_xzmat_qrfactor");
    
    tmp1 = _xzvec_resize(tmp1,A->m);
    w    = _xzvec_resize(w,   A->n);
    _XMEM_STAT_REG(tmp1,_XTYPE_ZVEC);
    _XMEM_STAT_REG(w,   _XTYPE_ZVEC);
    
    for (k=0; k<limit; k++){
        /* get H/holder vector for the k-th column */
        _xzmat_get_col(A,k,tmp1);
        _xzvec_hhvec(tmp1,k,&beta,tmp1,&A->me[k][k]);
        diag->ve[k] = tmp1->ve[k];
    
        /* apply H/holder vector to remaining columns */
        _xtracecatch(_xzmat_hhvtrcols(A,k,k+1,tmp1,beta,w),"_xzmat_qrfactor");
    }

#ifdef _XTHREAD_SAFE
    _xzvec_vfree(tmp1); _xzvec_vfree(w);
#endif

    return (A);
}

/*!
 * Gaussian elimination with scaled partial pivoting
 * -- Note: returns LU matrix which is A
 */
_xzmat *_xzmat_lufactor(_xzmat *A, _xperm *pivot){
    unsigned int i,j,m,n;
    int i_max,k,k_max;
    double dtemp,max1;
    _xcomplex **A_v,*A_piv,*A_row,temp;
    _XSTATIC _xvec *scale = (_xvec *)NULL;

    if (!A || !pivot)
        _xerror(_XE_NULL,"_xzmat_lufactor");
    if (pivot->size != A->m)
        _xerror(_XE_SIZES,"_xzmat_lufactor");
    m = A->m; n = A->n;
    scale = _xvec_resize(scale,A->m);
    _XMEM_STAT_REG(scale,_XTYPE_VEC);
    A_v = A->me;

    /* initialise pivot with identity permutation */
    for (i=0; i<m; i++)
        pivot->pe[i] = i;

    /* set scale parameters */
    for (i=0; i<m; i++){
        max1 = 0.0;
        for (j=0; j<n; j++){
            dtemp = _xzabs(A_v[i][j]);
            max1  = _xmax(max1,dtemp);
        }
        scale->ve[i] = max1;
    }

    /* main loop */
    k_max = _xmin(m,n)-1;
    for (k=0; k<k_max; k++){
        /* find best pivot row */
        max1 = 0.0; i_max = -1;
        for (i=k; i<m; i++)
        if (scale->ve[i] > 0.0){
            dtemp = _xzabs(A_v[i][k])/scale->ve[i];
            if (dtemp > max1){
                max1 = dtemp;
                i_max = i;
            }
        }
        
        /* if no pivot then ignore column k... */
        if (i_max == -1)
            continue;

        /* do we pivot ? */
        if (i_max != k){   /* yes we do... */
            _xperm_transp(pivot,i_max,k);
            for (j=0; j<n; j++){
                temp = A_v[i_max][j];
                A_v[i_max][j] = A_v[k][j];
                A_v[k][j] = temp;
            }
        }
        
        /* row operations */
        for (i=k+1; i<m; i++ ){/* for each row do... */
            /* Note: divide by zero should never happen */
            temp = A_v[i][k] = _xzdiv(A_v[i][k],A_v[k][k]);
            A_piv = &(A_v[k][k+1]);
            A_row = &(A_v[i][k+1]);
            temp.re = - temp.re;
            temp.im = - temp.im;
            if (k+1 < n)
                __xzmltadd__(A_row,A_piv,temp,(int)(n-(k+1)),0);
        }
    }

#ifdef _XTHREAD_SAFE
    _xvec_vfree(scale);
#endif

    return A;
}

/*!
 * Note: The usual representation of a Householder transformation is taken
 * to be:
 * P = I - beta.u.u*
 * where beta = 2/(u*.u) and u is called the Householder vector
 * (u* is the conjugate transposed vector of u
 *
 * form the QR factorisation of A with column pivoting
 * -- factorisation stored in compact form as described above
 * ( not quite standard format )
 */
_xzmat *_xzmat_qrcpfactor(_xzmat *A, _xzvec *diag, _xperm *px){
    unsigned int i,i_max,j,k,limit;
    _XSTATIC _xzvec *tmp1=(_xzvec *)NULL, *tmp2=(_xzvec *)NULL, *w=(_xzvec *)NULL;
    _XSTATIC _xvec *gamma=(_xvec *)NULL;
    _xreal beta;
    double maxgamma, sum, tmp;
    _xcomplex ztmp;
    
    if (!A || !diag || !px)
        _xerror(_XE_NULL,"_xzmat_qrcpfactor");
    limit = _xmin(A->m,A->n);
    if (diag->dim < limit || px->size != A->n)
        _xerror(_XE_SIZES,"_xzmat_qrcpfactor");
    
    tmp1  = _xzvec_resize(tmp1,A->m);
    tmp2  = _xzvec_resize(tmp2,A->m);
    w     = _xzvec_resize(w,A->n);
    gamma = _xvec_resize(gamma,A->n);
    _XMEM_STAT_REG(tmp1,_XTYPE_ZVEC);
    _XMEM_STAT_REG(tmp2,_XTYPE_ZVEC);
    _XMEM_STAT_REG(w,   _XTYPE_ZVEC);
    _XMEM_STAT_REG(gamma,_XTYPE_VEC);
    
    /* initialise gamma and px */
    for (j=0; j<A->n; j++){
        px->pe[j] = j;
        sum = 0.0;
        for (i=0; i<A->m; i++)
            sum += _xsquare(A->me[i][j].re) + _xsquare(A->me[i][j].im);
        gamma->ve[j] = sum;
    }
    
    for (k=0; k<limit; k++){
        /* find "best" column to use */
        i_max = k; maxgamma = gamma->ve[k];
        for (i=k+1; i<A->n; i++)
            /* Loop invariant:maxgamma=gamma[i_max]
               >=gamma[l];l=k,...,i-1 
             */
            if (gamma->ve[i] > maxgamma){
                maxgamma = gamma->ve[i];
                i_max = i;
            }
    
        /* swap columns if necessary */
        if (i_max != k){
            /* swap gamma values */
            tmp = gamma->ve[k];
            gamma->ve[k] = gamma->ve[i_max];
            gamma->ve[i_max] = tmp;
        
            /* update column permutation */
            _xperm_transp(px,k,i_max);
        
            /* swap columns of A */
            for (i=0; i<A->m; i++){
                ztmp = A->me[i][k];
                A->me[i][k] = A->me[i][i_max];
                A->me[i][i_max] = ztmp;
            }
        }
    
        /* get H/holder vector for the k-th column */
        _xzmat_get_col(A,k,tmp1);
        _xzvec_hhvec(tmp1,k,&beta,tmp1,&A->me[k][k]);
        diag->ve[k] = tmp1->ve[k];
    
        /* apply H/holder vector to remaining columns */
        _xzmat_hhvtrcols(A,k,k+1,tmp1,beta,w);
    
        /* update gamma values */
        for (j=k+1; j<A->n; j++)
            gamma->ve[j] -= _xsquare(A->me[k][j].re)+_xsquare(A->me[k][j].im);
    }

#ifdef _XTHREAD_SAFE
    _xzvec_vfree(tmp1); _xzvec_vfree(tmp2); _xvec_vfree(gamma); _xzvec_vfree(w);
#endif
    return (A);
}

/*!
 * construct orthogonal matrix from Householder vectors stored in
 * compact QR form
 */
_xzmat *_xzmat_makeq(_xzmat *QR, _xzvec *diag, _xzmat *Qout){
    _XSTATIC _xzvec *tmp1=(_xzvec *)NULL,*tmp2=(_xzvec *)NULL;
    unsigned int i,limit;
    double beta,r_ii,tmp_val;
    int j;

    limit = _xmin(QR->m,QR->n);
    if (!QR || !diag)
        _xerror(_XE_NULL,"_xzmat_makeq");
    if (diag->dim < limit)
        _xerror(_XE_SIZES,"_xzmat_makeq");
    Qout = _xzmat_resize(Qout,QR->m,QR->m);

    tmp1 = _xzvec_resize(tmp1,QR->m);   /* contains basis vec & columns of Q */
    tmp2 = _xzvec_resize(tmp2,QR->m);   /* contains H/holder vectors */
    _XMEM_STAT_REG(tmp1,_XTYPE_ZVEC);
    _XMEM_STAT_REG(tmp2,_XTYPE_ZVEC);

    for (i=0; i<QR->m ; i++){/* get i-th column of Q */
        /* set up tmp1 as i-th basis vector */
        for (j=0; j<QR->m ; j++)
            tmp1->ve[j].re = tmp1->ve[j].im = 0.0f;
        tmp1->ve[i].re = 1.0f;
    
        /* apply H/h transforms in reverse order */
        for (j=limit-1; j>=0; j--){
            _xzmat_get_col(QR,j,tmp2);
            r_ii = _xzabs(tmp2->ve[j]);
            tmp2->ve[j] = diag->ve[j];
            tmp_val = (r_ii*_xzabs(diag->ve[j]));
            beta = ( tmp_val == 0.0 ) ? 0.0 : 1.0/tmp_val;
            _xzvec_hhtrvec(tmp2,beta,j,tmp1,tmp1);
        }
    
        /* insert into Q */
        _xzmat_set_col(Qout,i,tmp1);
    }

#ifdef _XTHREAD_SAFE
    _xzvec_vfree(tmp1); _xzvec_vfree(tmp2);
#endif

    return (Qout);
}

/*!
 * construct upper triangular matrix from QR (compact form)
 * -- may be in-situ (all it does is zero the lower 1/2)
 */
_xzmat *_xzmat_maker(_xzmat *QR, _xzmat *Rout){
    unsigned int i,j;
    
    if (!QR)
        _xerror(_XE_NULL,"_xzmat_maker");
    Rout = _xzmat_mcopy(QR,Rout);
    
    for (i=1; i<QR->m; i++)
        for (j=0; j<QR->n && j<i; j++)
            Rout->me[i][j].re = Rout->me[i][j].im = 0.0f;
    
    return (Rout);
}

/*! premultiply mat by givens rotation described by c,s */
_xzmat *_xzmat_rot_rows(_xzmat *mat, int i, int k, double c, _xcomplex s, _xzmat *out){
    unsigned int j;
    _xcomplex temp1, temp2;

    if (!mat)
        _xerror(_XE_NULL,"_xzmat_rot_rows");
    if (i < 0 || i >= mat->m || k < 0 || k >= mat->m )
        _xerror(_XE_RANGE,"_xzmat_rot_rows");

    if (mat != out)
        out = _xzmat_mcopy(mat,_xzmat_resize(out,mat->m,mat->n));

    for (j=0; j<mat->n; j++){
        temp1.re = c*out->me[i][j].re
                   - s.re*out->me[k][j].re + s.im*out->me[k][j].im;
        temp1.im = c*out->me[i][j].im
                   - s.re*out->me[k][j].im - s.im*out->me[k][j].re;
        
        temp2.re = c*out->me[k][j].re
                   + s.re*out->me[i][j].re + s.im*out->me[i][j].im;
        temp2.im = c*out->me[k][j].im
                   + s.re*out->me[i][j].im - s.im*out->me[i][j].re;
        
        out->me[i][j] = temp1;
        out->me[k][j] = temp2;
    }

    return (out);
}

/*! postmultiply mat by adjoint Givens rotation described by c,s */
_xzmat *_xzmat_rot_cols(_xzmat *mat, int i, int k, double c, _xcomplex s, _xzmat *out){
    unsigned int j;
    _xcomplex x, y;

    if (!mat)
        _xerror(_XE_NULL,"_xzmat_rot_cols");
    if (i < 0 || i >= mat->n || k < 0 || k >= mat->n )
        _xerror(_XE_RANGE,"_xzmat_rot_cols");

    if (mat != out)
        out = _xzmat_mcopy(mat,_xzmat_resize(out,mat->m,mat->n));

    for (j=0; j<mat->m; j++){
        x = out->me[j][i];  y = out->me[j][k];

        out->me[j][i].re = c*x.re - s.re*y.re - s.im*y.im;
        out->me[j][i].im = c*x.im - s.re*y.im + s.im*y.re;
        
        out->me[j][k].re = c*y.re + s.re*x.re - s.im*x.im;
        out->me[j][k].im = c*y.im + s.re*x.im + s.im*x.re;
    }

    return (out);
}

/*!
 * return inverse of A, provided A is not too rank deficient
 * -- uses LU factorisation
 */
_xzmat *_xzmat_inverse(_xzmat *A, _xzmat *out){
    int i;
    _XSTATIC _xzvec *tmp=(_xzvec *)NULL, *tmp2=(_xzvec *)NULL;
    _XSTATIC _xzmat *A_cp=(_xzmat *)NULL;
    _XSTATIC _xperm *pivot=(_xperm *)NULL;

    if (!A)
        _xerror(_XE_NULL,"_xzmat_inverse");
    if (A->m != A->n )
        _xerror(_XE_SQUARE,"_xzmat_inverse");
    if (!out || out->m < A->m || out->n < A->n )
        out = _xzmat_resize(out,A->m,A->n);

    A_cp  = _xzmat_resize(A_cp,A->m,A->n);
    A_cp  = _xzmat_mcopy(A,A_cp);
    tmp   = _xzvec_resize(tmp,A->m);
    tmp2  = _xzvec_resize(tmp2,A->m);
    pivot = _xperm_resize(pivot,A->m);
    _XMEM_STAT_REG(A_cp, _XTYPE_ZMAT);
    _XMEM_STAT_REG(tmp,  _XTYPE_ZVEC);
    _XMEM_STAT_REG(tmp2, _XTYPE_ZVEC);
    _XMEM_STAT_REG(pivot,_XTYPE_PERM);
    _xtracecatch(_xzmat_lufactor(A_cp,pivot),"_xzmat_inverse");
    for (i=0; i<A->n; i++){
        _xzvec_zero(tmp);
        tmp->ve[i].re = 1.0f;
        tmp->ve[i].im = 0.0f;
        _xtracecatch(_xzmat_lusolve(A_cp,pivot,tmp,tmp2),"_xzmat_inverse");
        _xzmat_set_col(out,i,tmp2);
    }

#ifdef _XTHREAD_SAFE
    _xzvec_vfree(tmp); _xzvec_vfree(tmp2);
    _xzmat_mfree(A_cp);_xperm_pfree(pivot);
#endif

    return out;
}

/*!
 * compute the Schur decomposition of the matrix A in situ
 * -- optionally, gives Q matrix such that Q^*.A.Q is upper triangular
 * -- returns upper triangular Schur matrix
 */
_xzmat *_xzmat_schur(_xzmat *A, _xzmat *Q){
    int i,j,iter,k,k_min,k_max,k_tmp,n,split;
    _xreal c;
    _xcomplex det,discrim,lambda,lambda0,lambda1,s,sum,ztmp;
    _xcomplex x,y;   /* for chasing algorithm */
    _xcomplex **A_me;
    _XSTATIC _xzvec *diag=(_xzvec *)NULL;
    
    if (!A)
        _xerror(_XE_NULL,"_xzmat_schur");
    if (A->m != A->n || ( Q && Q->m != Q->n ) )
        _xerror(_XE_SQUARE,"_xzmat_schur");
    if (Q && Q->m != A->m )
        _xerror(_XE_SIZES,"_xzmat_schur");
    n = A->n;
    diag = _xzvec_resize(diag,A->n);
    _XMEM_STAT_REG(diag,_XTYPE_ZVEC);
    
    /* compute Hessenberg form */
    _xzmat_hfactor(A,diag);
    
    /* save Q if necessary, and make A explicitly Hessenberg */
    _xzmat_hqunpack(A,diag,Q,A);

    k_min = 0; A_me = A->me;

    while (k_min < n){
        /* find k_max to suit:
           submatrix k_min..k_max should be irreducible 
         */
        k_max = n-1;
        for (k=k_min; k<k_max; k++)
            if (_xis_zero(A_me[k+1][k])){
                k_max = k;
                break;
            }

        if (k_max <= k_min){
            k_min = k_max + 1;
            continue;       /* outer loop */
        }

        /* now have r x r block with r >= 2:
           apply Francis QR step until block splits 
         */
        split = _XFALSE; iter = 0;
        while (!split){
            _xcomplex a00, a01, a10, a11;
            iter++;
        
            /* set up Wilkinson/Francis complex shift */
            /* use the smallest eigenvalue of the bottom 2 x 2 submatrix */
            k_tmp = k_max - 1;

            a00 = A_me[k_tmp][k_tmp];
            a01 = A_me[k_tmp][k_max];
            a10 = A_me[k_max][k_tmp];
            a11 = A_me[k_max][k_max];
            ztmp.re = 0.5*(a00.re - a11.re);
            ztmp.im = 0.5*(a00.im - a11.im);
            discrim = _xzsqrt(_xzadd(_xzmlt(ztmp,ztmp),_xzmlt(a01,a10)));
            sum.re  = 0.5*(a00.re + a11.re);
            sum.im  = 0.5*(a00.im + a11.im);
            lambda0 = _xzadd(sum,discrim);
            lambda1 = _xzsub(sum,discrim);
            det = _xzsub(_xzmlt(a00,a11),_xzmlt(a01,a10)); 
        
            if (_xis_zero(lambda0) && _xis_zero(lambda1)){                                                          
                lambda.re = lambda.im = 0.0f;
            } 
            else if (_xzabs(lambda0) > _xzabs(lambda1))
                lambda = _xzdiv(det,lambda0);
            else
                lambda = _xzdiv(det,lambda1);

            /* perturb shift if convergence is slow */
            if ((iter % 10) == 0){
                lambda.re += iter*0.02;
                lambda.im += iter*0.02;
            }

            /* set up Householder transformations */
            k_tmp = k_min + 1;

            x = _xzsub(A->me[k_min][k_min],lambda);
            y = A->me[k_min+1][k_min];

            /* use Givens' rotations to "chase" off-Hessenberg entry */
            for (k=k_min; k<=k_max-1; k++){
                _xzvec_givens(x,y,&c,&s);
                _xzmat_rot_cols(A,k,k+1,c,s,A);
                _xzmat_rot_rows(A,k,k+1,c,s,A);
                if (Q)
                    _xzmat_rot_cols(Q,k,k+1,c,s,Q);

                /* zero things that should be zero */
                if (k > k_min)
                    A->me[k+1][k-1].re = A->me[k+1][k-1].im = 0.0f;

                /* get next entry to chase along sub-diagonal */
                x = A->me[k+1][k];
                if (k <= k_max - 2)
                    y = A->me[k+2][k];
                else
                    y.re = y.im = 0.0f;
            }

            for (k = k_min; k <= k_max-2; k++){
                /* zero appropriate sub-diagonals */
                A->me[k+2][k].re = A->me[k+2][k].im = 0.0f;
            }

            /* test to see if matrix should split */
            for (k = k_min; k < k_max; k++)
                if (_xzabs(A_me[k+1][k]) < _XMACHEPS*
                   (_xzabs(A_me[k][k])+_xzabs(A_me[k+1][k+1]))){
                    A_me[k+1][k].re = A_me[k+1][k].im = 0.0f;
                    split = _XTRUE;
                }

        }
    }
    
    /* polish up A by zeroing strictly lower triangular elements
       and small sub-diagonal elements 
     */
    for (i = 0; i < A->m; i++)
        for (j = 0; j < i-1; j++)
            A_me[i][j].re = A_me[i][j].im = 0.0f;
    for (i = 0; i < A->m - 1; i++)
        if (_xzabs(A_me[i+1][i]) < _XMACHEPS*
           (_xzabs(A_me[i][i])+_xzabs(A_me[i+1][i+1])))
            A_me[i+1][i].re = A_me[i+1][i].im = 0.0f;

#ifdef _XTHREAD_SAFE
    _xzvec_vfree(diag);
#endif

    return A;
}

/*!
 * return an estimate of the 2-norm condition number of the
 * matrix factorised by QR factor or QRCP factor
 * -- note that as Q does not affect the 2-norm condition number,
 *    it is not necessary to pass the diag, beta (or pivot) vectors
 * -- generates a lower bound on the true condition number
 * -- if the matrix is exactly singular, _XHUGE_VAL is returned
 * -- note that QR condest is likely to be more reliable for
 *    matrices factored using QRCP factor
 */
double _xzmat_qrcondest(_xzmat *QR){
    _XSTATIC _xzvec *y=(_xzvec *)NULL;
    double norm,norm1,norm2,tmp1,tmp2;
    _xcomplex sum,tmp;
    int i,j,limit;

    if (!QR)
        _xerror(_XE_NULL,"_xzmat_qrcondest");

    limit = _xmin(QR->m,QR->n);
    for (i=0; i<limit; i++)
        if (_xis_zero(QR->me[i][i]))
            return _XHUGE_VAL;

    y = _xzvec_resize(y,limit);
    _XMEM_STAT_REG(y,_XTYPE_ZVEC);
    /* use the trick for getting a unit vector y with ||R.y||_inf small
       from the LU condition estimator
     */
    for (i=0; i<limit; i++){
        sum.re = sum.im = 0.0f;
        for (j=0; j<i; j++)
            sum = _xzsub(sum,_xzmlt(QR->me[j][i],y->ve[j]));
        norm1 = _xzabs(sum);
        if (norm1 == 0.0)
            sum.re = 1.0f;
        else {
            sum.re += sum.re / norm1;
            sum.im += sum.im / norm1;
        }
        y->ve[i] = _xzdiv(sum,QR->me[i][i]);
    }
    _xzmat_uamlt(QR,y,y);

    /* now apply inverse power method to R*.R */
    for (i=0; i<3; i++){
        tmp1 = _xzvec_vnorm2(y);
        _xzvec_mlt(_xzmake(1.0/tmp1,0.0),y,y);
        _xzmat_uasolve(QR,y,y,0.0);
        tmp2 = _xzvec_vnorm2(y);
        _xzvec_mlt(_xzmake(1.0/tmp2,0.0),y,y);
        _xzmat_usolve(QR,y,y,0.0);
    }
    /* now compute approximation for ||R^{-1}||_2 */
    norm1 = sqrt(tmp1)*sqrt(tmp2);

    /* now use complementary approach to compute approximation to ||R||_2 */
    for (i=limit-1; i>=0; i--){
        sum.re = sum.im = 0.0f;
        for (j=i+1; j<limit; j++)
            sum = _xzadd(sum,_xzmlt(QR->me[i][j],y->ve[j]));
        if (_xis_zero(QR->me[i][i]))
            return _XHUGE_VAL;
        tmp = _xzdiv(sum,QR->me[i][i]);
        if (_xis_zero(tmp)){
            y->ve[i].re = 1.0f;
            y->ve[i].im = 0.0f;
        }
        else {
            norm = _xzabs(tmp);
            y->ve[i].re = sum.re / norm;
            y->ve[i].im = sum.im / norm;
        }
    }

    /* now apply power method to R*.R */
    for (i=0; i<3; i++){
        tmp1 = _xzvec_vnorm2(y);
        _xzvec_mlt(_xzmake(1.0/tmp1,0.0),y,y);
        _xzmat_umlt(QR,y,y);
        tmp2 = _xzvec_vnorm2(y);
        _xzvec_mlt(_xzmake(1.0/tmp2,0.0),y,y);
        _xzmat_uamlt(QR,y,y);
    }
    norm2 = sqrt(tmp1)*sqrt(tmp2);

#ifdef _XTHREAD_SAFE
    _xzvec_vfree(y);
#endif

    return norm1*norm2;
}

/*! return an estimate of the condition number of LU given the
 *  LU factorisation in compact form
 */
double _xzmat_lucondest(_xzmat *LU, _xperm *pivot){
    _XSTATIC _xzvec *y = (_xzvec *)NULL, *z = (_xzvec *)NULL;
    double cond_est,L_norm,U_norm,norm,sn_inv;
    _xcomplex sum;
    int i,j,n;

    if (!LU || !pivot)
        _xerror(_XE_NULL,"_xzmat_lucondest");
    if (LU->m != LU->n)
        _xerror(_XE_SQUARE,"_xzmat_lucondest");
    if (LU->n != pivot->size )
        _xerror(_XE_SIZES,"_xzmat_lucondest");

    n = LU->n;
    y = _xzvec_resize(y,n);
    z = _xzvec_resize(z,n);
    _XMEM_STAT_REG(y,_XTYPE_ZVEC);
    _XMEM_STAT_REG(z,_XTYPE_ZVEC);

    cond_est = 0.0; /* should never be returned */
    for (i=0; i<n; i++){
        sum.re = 1.0f;
        sum.im = 0.0f;
        for (j=0; j<i; j++)
            sum = _xzsub(sum,_xzmlt(LU->me[j][i],y->ve[j]));
        sn_inv = 1.0 / _xzabs(sum);
        sum.re += sum.re * sn_inv;
        sum.im += sum.im * sn_inv;
        if (_xis_zero(LU->me[i][i]))
            return _XHUGE_VAL;
        y->ve[i] = _xzdiv(sum,LU->me[i][i]);
    }

    _xzmat_lasolve(LU,y,y,1.0);
    _xzmat_lusolve(LU,pivot,y,z);

    /* now estimate norm of A (even though it is not directly available) */
    /* actually computes ||L||_inf.||U||_inf */
    U_norm = 0.0;
    for (i=0; i<n; i++){
        norm = 0.0;
        for (j=i; j<n; j++)
            norm += _xzabs(LU->me[i][j]);
        if (norm > U_norm)
            U_norm = norm;
    }
    L_norm = 0.0;
    for (i=0; i<n; i++){
        norm = 1.0;
        for (j=0; j<i; j++)
            norm += _xzabs(LU->me[i][j]);
        if (norm > L_norm)
            L_norm = norm;
    }

    _xtracecatch(cond_est = U_norm*L_norm*_xzvec_vnorm_inf(z)/_xzvec_vnorm_inf(y),
                 "_xzmat_lucondest");
#ifdef _XTHREAD_SAFE
    _xzvec_vfree(y); _xzvec_vfree(z);
#endif

    return cond_est;
}

/*!
 * return c,s parameters for Givens rotation to
 * eliminate y in the **column** vector [ x y ]
 */
void _xzvec_givens(_xcomplex x, _xcomplex y, _xreal *c, _xcomplex *s){
    double inv_norm, norm;
    _xcomplex tmp;

    /* this is a safe way of computing sqrt(|x|^2+|y|^2) */
    tmp.re = (_xreal)_xzabs(x); tmp.im = (_xreal)_xzabs(y);
    norm = _xzabs(tmp);

    if (norm == 0.0){
        *c = (_xreal)1.0;
        s->re = s->im = (_xreal)0.0;    
    } /* identity */
    else {
        inv_norm = 1.0 / tmp.re;    /* inv_norm = 1/|x| */
        x.re *= inv_norm;
        x.im *= inv_norm;           /* normalise x */
        inv_norm = 1.0/norm;        /* inv_norm = 1/||[x,y]||2 */
        *c = tmp.re * inv_norm;
        /* now compute - conj(normalised x).y/||[x,y]||2 */
        s->re = - inv_norm*(x.re*y.re + x.im*y.im);
        s->im =   inv_norm*(x.re*y.im - x.im*y.re);
    }
}

