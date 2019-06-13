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

#ifndef _XPOWER_MATRIX_H_
#define _XPOWER_MATRIX_H_

#include "xpower-config.h"

#ifdef __cplusplus
extern "C" {
#endif

_XEXPORT extern const int _x_version;
_XEXPORT extern const char *_xversion();

#define _XMAXDIM  10000001

/* vector definition */
typedef struct __xvec {
    unsigned int dim;
    unsigned int max_dim;
    _xreal *ve;
} _xvec;

/* matrix definition */
typedef struct __xmat {
    unsigned int m;
    unsigned int n;
    unsigned int max_m;
    unsigned int max_n;
    unsigned int max_size;
    _xreal **me;
    _xreal *base; /* base is base of alloc'd mem */
} _xmat;

/* band matrix definition */
typedef struct __xband {
    _xmat *mat; /* matrix */
    int lb;
    int ub;     /* lower and upper bandwidth */
} _xband;

/* permutation definition */
typedef struct __xperm {
    unsigned int size;
    unsigned int max_size;
    unsigned int *pe;
} _xperm;

/* integer vector definition */
typedef struct __xivec {
    unsigned int dim;
    unsigned int max_dim;
    int *ive;
} _xivec;

/* basic sparse types */
typedef struct __xrow_elt {
    int col;
    int nxt_row;
    int nxt_idx;
    _xreal val;
} _xrow_elt;

typedef struct __xsprow {
    int len;
    int maxlen;
    int diag;
    _xrow_elt *elt; /* elt[maxlen] */
} _xsprow;

typedef struct __xspmat {
    int m;
    int n;
    int max_m;
    int max_n;
    _xsprow *row;  /* row[max_m] */
    int *start_row;  /* start_row[max_n] */
    int *start_idx;  /* start_idx[max_n] */
    char flag_col;
    char flag_diag;
} _xspmat;

/* Note that the first allocated entry in column j is start_row[j];
  This starts the chain down the columns using the nxt_row and nxt_idx
  fields of each entry in each row. 
 */
typedef struct __xpair { int pos; _xreal val; } _xpair;
typedef struct __xspvec {
    int dim;
    int max_dim;
    _xpair *elt;    /* elt[max_dim] */
} _xspvec;

/* basic structure for iterative methods */
#define _XITER_LIMIT_DEF 1000
#define _XITER_EPS_DEF   1e-6

typedef _xvec* (*_xfun_ax)(void*,_xvec*,_xvec*);
typedef struct __xiter {
    int shared_x;   /* if TRUE then x is shared and it will not be free'd */ 
    int shared_b;   /* if TRUE then b is shared and it will not be free'd */
    unsigned int k; /* no. of direction (search) vectors; =0 - none */
    int limit;      /* upper bound on the no. of iter. steps */
    int steps;      /* no. of iter. steps done */
    _xreal eps;     /* accuracy required */
    _xvec *x;       /* input: initial guess, output: approximate solution */
    _xvec *b;       /* right hand side of the equation A*x = b */
    _xfun_ax ax;    /* function computing y = A*x */
    void *a_par;    /* parameters for Ax */
    _xfun_ax atx;   /* function  computing y = A^T*x,T = transpose */
    void *at_par;   /* parameters for ATx */
    _xfun_ax bx;    /* function computing y = B*x; B - preconditioner */
    void *b_par;    /* parameters for Bx */
    _xfun_ax btx;   /* function computing y = B^T*x; B - preconditioner */
    void *bt_par;   /* parameters for BTx */
    void (*info)(struct __xiter*,double,_xvec*,_xvec*);/* function giving some information for a user;
                                                        nres - a norm of a residual res */
    int (*stop_crit)(struct __xiter*,double,_xvec*,_xvec*);/* stopping criterion:
                                                            nres - a norm of res;
                                                            res - residual;
                                                            if returned value == _XTRUE then stop;
                                                            if returned value == _XFALSE then continue; */


    _xreal init_res; /* the norm of the initial residual */
} _xiter;
typedef void (*_xfun_info)(_xiter*, double, _xvec *,_xvec *);
typedef int (*_xfun_stop_crit)(_xiter*, double, _xvec *,_xvec *);

/* complex */
#define _XZ_CONJ   1
#define _XZ_NOCONJ 0

/* complex definition */
typedef struct __xcomplex {
    _xreal re;
    _xreal im;
} _xcomplex;

/* complex vector definition */
typedef struct __xzvec {
    unsigned int dim;
    unsigned int max_dim;
    _xcomplex *ve;
} _xzvec;

/* complex matrix definition */
typedef struct __xzmat {
    unsigned int m;
    unsigned int n;
    unsigned int max_m;
    unsigned int max_n;
    unsigned int max_size;
    _xcomplex *base;          /* base is base of alloc'd mem */
    _xcomplex **me;
} _xzmat;

/* basic routines */
_XEXPORT extern double __xip__(const _xreal *, const _xreal *, int);
_XEXPORT extern void __xmltadd__(_xreal *, const _xreal *, double, int);
_XEXPORT extern void __xadd__(const _xreal *, const _xreal *, _xreal *, int);
_XEXPORT extern void __xsub__(const _xreal *, const _xreal *, _xreal *, int);
_XEXPORT extern void __xsmlt__(const _xreal *, double, _xreal *, int);
_XEXPORT extern void __xzero__(_xreal *, int);
_XEXPORT extern void __xzzero__(_xcomplex *, int);
_XEXPORT extern void __xzconj__(_xcomplex *, int);
_XEXPORT extern void __xzmlt__(const _xcomplex *, _xcomplex, _xcomplex *, int);
_XEXPORT extern void __xzadd__(const _xcomplex *, const _xcomplex *, _xcomplex *, int);
_XEXPORT extern void __xzsub__(const _xcomplex *, const _xcomplex *, _xcomplex *, int);
_XEXPORT extern void __xzmltadd__(_xcomplex *, const _xcomplex *, _xcomplex, int, int);
_XEXPORT extern _xcomplex __xzip__(const _xcomplex *, const _xcomplex *, int, int);

/* vector routines */
_XEXPORT extern _xvec *_xvec_get(int);
_XEXPORT extern _xvec *_xvec_zero(_xvec *);
_XEXPORT extern _xvec *_xvec_ones(_xvec *);
_XEXPORT extern _xvec *_xvec_rand(_xvec *);
_XEXPORT extern _xvec *_xvec_count(_xvec *);
_XEXPORT extern _xvec *_xvec_copy(const _xvec *, _xvec *, unsigned int);
_XEXPORT extern _xvec *_xvec_move(const _xvec *, int, int, _xvec *, int);
_XEXPORT extern _xvec *_xvec_resize(_xvec *, int);
_XEXPORT extern _xvec *_xvec_sub_vec(const _xvec *, int, int, _xvec *);
_XEXPORT extern int _xvec_free(_xvec *);
_XEXPORT extern int _xvec_get_vars(int, ...);
_XEXPORT extern int _xvec_resize_vars(int, ...);
_XEXPORT extern int _xvec_free_vars(_xvec **, ...);

/* FFT */
_XEXPORT extern void _xvec_fft(_xvec *, _xvec *);
_XEXPORT extern void _xvec_ifft(_xvec *, _xvec *);
_XEXPORT extern void _xvec_givens(double, double, _xreal *, _xreal *);

/* vector op */
_XEXPORT extern double _xvec_sum(const _xvec *);
_XEXPORT extern double _xvec_min(const _xvec *, int *);
_XEXPORT extern double _xvec_max(const _xvec *, int *);
_XEXPORT extern double _xvec_in_prod(const _xvec *, const _xvec *, unsigned int);
_XEXPORT extern double _xvec_norm1(const _xvec *, const _xvec *);
_XEXPORT extern double _xvec_norm2(const _xvec *, const _xvec *);
_XEXPORT extern double _xvec_norm_inf(const _xvec *, const _xvec *);
_XEXPORT extern _xvec *_xvec_sv_mlt(double, const _xvec *, _xvec *);
_XEXPORT extern _xvec *_xvec_add(const _xvec *, const _xvec *, _xvec *);
_XEXPORT extern _xvec *_xvec_mltadd(const _xvec *, const _xvec *, double, _xvec *);
_XEXPORT extern _xvec *_xvec_sub(const _xvec *, const _xvec *, _xvec *);
_XEXPORT extern _xvec *_xvec_star(const _xvec *, const _xvec *, _xvec *);
_XEXPORT extern _xvec *_xvec_conv(const _xvec *, const _xvec *, _xvec *);
_XEXPORT extern _xvec *_xvec_slash(const _xvec *, const _xvec *, _xvec *);
_XEXPORT extern _xvec *_xvec_pconv(const _xvec *, const _xvec *, _xvec *);
_XEXPORT extern _xvec *_xvec_lincomb(int, const _xvec **, const _xreal *, _xvec *);
_XEXPORT extern _xvec *_xvec_linlist(_xvec *, _xvec *, double, ...);
_XEXPORT extern _xvec *_xvec_sort(_xvec *, _xperm *);
_XEXPORT extern _xvec *_xvec_map(double (*)(double),const _xvec *,_xvec *);
_XEXPORT extern _xvec *_xvec_vmap(double (*)(void *,double),void *,const _xvec *,_xvec *);
_XEXPORT extern _xvec *_xvec_rot_vec(const _xvec *, unsigned int, unsigned int, double, double, _xvec *);
_XEXPORT extern _xvec *_xvec_hhvec(const _xvec *, unsigned int, _xreal *, _xvec *, _xreal *);
_XEXPORT extern _xvec *_xvec_hhtrvec(const _xvec *, double, unsigned int, const _xvec *, _xvec *);

/* vector I/O routines */
_XEXPORT extern void _xvec_dump(FILE *,const _xvec *);
_XEXPORT extern void _xvec_foutput(FILE *, const _xvec *);
_XEXPORT extern _xvec *_xvec_finput(FILE *,_xvec *);

/* vector macros */
#define _xvec_output(vec)      _xvec_foutput(stdout,vec)
#define _xvec_input(vec)       _xvec_finput(stdin,vec)
#define _xvec_vnorm1(x)        _xvec_norm1(x,(_xvec *)NULL)
#define _xvec_vnorm2(x)        _xvec_norm2(x,(_xvec *)NULL)
#define _xvec_vnorm_inf(x)     _xvec_norm_inf(x,(_xvec *)NULL)
#define _xvec_vcopy(in,out)    _xvec_copy(in,out,0)
#define _xvec_vin_prod(a,b)    _xvec_in_prod(a,b,0)
#define _xvec_vfree(vec)      (_xvec_free(vec), (vec)=(_xvec *)NULL)
#define _xvec_entry(x,i)       _xvec_get_val(x,i)
#define _xvec_get_val(x,i)     ((x)->ve[(i)])
#define _xvec_set_val(x,i,val) ((x)->ve[(i)] = (val))
#define _xvec_add_val(x,i,val) ((x)->ve[(i)] += (val))
#define _xvec_sub_val(x,i,val) ((x)->ve[(i)] -= (val))
#define _xvec_chk_idx(x,i)     ((i)>=0 && (i)<(x)->dim)

/* perm routines */
_XEXPORT extern _xperm *_xperm_get(int);
_XEXPORT extern _xperm *_xperm_copy(const _xperm *, _xperm *);
_XEXPORT extern _xperm *_xperm_resize(_xperm *, int);
_XEXPORT extern _xperm *_xperm_ident(_xperm *);
_XEXPORT extern int _xperm_free(_xperm *);
_XEXPORT extern int _xperm_get_vars(int, ...);
_XEXPORT extern int _xperm_resize_vars(int, ...);
_XEXPORT extern int _xperm_free_vars(_xperm **, ...);

/* perm op */
_XEXPORT extern int _xperm_sign(const _xperm *);
_XEXPORT extern _xperm *_xperm_inv(const _xperm *, _xperm *);
_XEXPORT extern _xperm *_xperm_mlt(const _xperm *, const _xperm *, _xperm *);
_XEXPORT extern _xperm *_xperm_transp(_xperm *, unsigned int, unsigned int);

_XEXPORT extern _xvec *_xperm_vec(_xperm *, const _xvec *, _xvec *);
_XEXPORT extern _xvec *_xperm_inv_vec(_xperm *, const _xvec *, _xvec *);

_XEXPORT extern _xmat *_xperm_cols(const _xperm *, const _xmat *, _xmat *);
_XEXPORT extern _xmat *_xperm_rows(const _xperm *, const _xmat *, _xmat *);

_XEXPORT extern _xzvec *_xperm_zvec(_xperm *, _xzvec *, _xzvec *);
_XEXPORT extern _xzvec *_xperm_inv_zvec(_xperm *, _xzvec *, _xzvec *);

/* perm I/O routines */
_XEXPORT extern void _xperm_dump(FILE *, const _xperm *);
_XEXPORT extern void _xperm_foutput(FILE *, const _xperm *);
_XEXPORT extern _xperm *_xperm_finput(FILE *,_xperm *);

/* perm macros */
#define _xperm_output(px) _xperm_foutput(stdout,px)
#define _xperm_input(px)  _xperm_finput(stdin,px)
#define _xperm_pfree(px) (_xperm_free(px), (px)=(_xperm *)NULL)

/* matrix routines */
_XEXPORT extern _xmat *_xmat_get(int, int);
_XEXPORT extern _xmat *_xmat_zero(_xmat *);
_XEXPORT extern _xmat *_xmat_ones(_xmat *);
_XEXPORT extern _xmat *_xmat_rand(_xmat *);
_XEXPORT extern _xmat *_xmat_copy(const _xmat *, _xmat *, unsigned int, unsigned int);
_XEXPORT extern _xmat *_xmat_move(const _xmat *, int, int, int, int, _xmat *, int, int);
_XEXPORT extern _xmat *_xmat_ident(_xmat *);
_XEXPORT extern _xmat *_xmat_resize(_xmat *, int, int);
_XEXPORT extern _xmat *_xmat_vm_move(const _xvec *, int, _xmat *, int, int, int, int);
_XEXPORT extern int _xmat_free(_xmat *);
_XEXPORT extern int _xmat_get_vars(int, int, ...);
_XEXPORT extern int _xmat_resize_vars(int, int, ...);
_XEXPORT extern int _xmat_free_vars(_xmat **, ...);

/* matrix op */
_XEXPORT extern double _xmat_norm1(const _xmat *);
_XEXPORT extern double _xmat_norm_inf(const _xmat *);
_XEXPORT extern double _xmat_norm_frob(const _xmat *);
_XEXPORT extern _xmat *_xmat_add(const _xmat *, const _xmat *, _xmat *);
_XEXPORT extern _xmat *_xmat_sub(const _xmat *, const _xmat *, _xmat *);
_XEXPORT extern _xmat *_xmat_mlt(const _xmat *, const _xmat *, _xmat *);
_XEXPORT extern _xmat *_xmat_mtr_mlt(const _xmat *, const _xmat *, _xmat *);
_XEXPORT extern _xmat *_xmat_trm_mlt(const _xmat *, const _xmat *, _xmat *);
_XEXPORT extern _xmat *_xmat_transp(const _xmat *, _xmat *);
_XEXPORT extern _xmat *_xmat_sm_mlt(double, const _xmat *, _xmat *);
_XEXPORT extern _xmat *_xmat_swap_rows(_xmat *, int, int, int, int);
_XEXPORT extern _xmat *_xmat_swap_cols(_xmat *, int, int, int, int);
_XEXPORT extern _xmat *_xmat_sub_mat(const _xmat *, unsigned int, unsigned int, unsigned int, unsigned int, _xmat *);
_XEXPORT extern _xmat *_xmat_set_col(_xmat *, unsigned int, const _xvec *, unsigned int);
_XEXPORT extern _xmat *_xmat_set_row(_xmat *, unsigned int, const _xvec *, unsigned int);
_XEXPORT extern _xmat *_xmat_ms_mltadd(const _xmat *, const _xmat *, double, _xmat *);

_XEXPORT extern _xmat *_xmat_pow(const _xmat *, int, _xmat *);
_XEXPORT extern _xmat *_xmat_exp(const _xmat *, double, _xmat *);
_XEXPORT extern _xmat *_xmat_vpow(const _xmat *, int, _xmat *, _xmat *);
_XEXPORT extern _xmat *_xmat_vexp(const _xmat *, double, _xmat *, int *, int *);
_XEXPORT extern _xmat *_xmat_poly(const _xmat *, const _xvec *, _xmat *);
_XEXPORT extern _xmat *_xmat_inverse(const _xmat *, _xmat *);

_XEXPORT extern _xvec *_xmat_mv_mltadd(const _xvec *, const _xvec *, const _xmat *, double, _xvec *);
_XEXPORT extern _xvec *_xmat_vm_mltadd(const _xvec *, const _xvec *, const _xmat *, double, _xvec *);
_XEXPORT extern _xvec *_xmat_get_col(const _xmat *, unsigned int, _xvec *);
_XEXPORT extern _xvec *_xmat_get_row(const _xmat *, unsigned int, _xvec *);
_XEXPORT extern _xvec *_xmat_mv_mlt(const _xmat *, const _xvec *, _xvec *);
_XEXPORT extern _xvec *_xmat_vm_mlt(const _xmat *, const _xvec *, _xvec *);
_XEXPORT extern _xvec *_xmat_mv_move(const _xmat *, int, int, int, int, _xvec *, int);

_XEXPORT extern _xband *_xmat_mat2band(const _xmat *, int, int, _xband *);

/* matrix I/O routines */
_XEXPORT extern void _xmat_dump(FILE *,const _xmat *);
_XEXPORT extern void _xmat_foutput(FILE *, const _xmat *);
_XEXPORT extern _xmat *_xmat_finput(FILE *,_xmat *);

/* matrix macros */
#define _xmat_output(mat)            _xmat_foutput(stdout,mat)
#define _xmat_input(mat)             _xmat_finput(stdin,mat)
#define _xmat_mcopy(in,out)          _xmat_copy(in,out,0,0)
#define _xmat_mset_row(mat,row,vec)  _xmat_set_row(mat,row,vec,0)
#define _xmat_mset_col(mat,col,vec)  _xmat_set_col(mat,col,vec,0)
#define _xmat_mfree(mat)            (_xmat_free(mat), (mat)=(_xmat *)NULL)
#define _xmat_entry(A,i,j)           _xmat_get_val(A,i,j)
#define _xmat_get_val(A,i,j)         ((A)->me[(i)][(j)])
#define _xmat_set_val(A,i,j,val)     ((A)->me[(i)][(j)] = (val))
#define _xmat_add_val(A,i,j,val)     ((A)->me[(i)][(j)] += (val))
#define _xmat_sub_val(A,i,j,val)     ((A)->me[(i)][(j)] -= (val))
#define _xmat_chk_idx(A,i,j)         ((i)>=0 && (i)<(A)->m && (j)>=0 && (j)<=(A)->n)

/* ivector routines */
_XEXPORT extern _xivec *_xivec_get(int);
_XEXPORT extern _xivec *_xivec_zero(_xivec *);
_XEXPORT extern _xivec *_xivec_copy(const _xivec *, _xivec *);
_XEXPORT extern _xivec *_xivec_move(const _xivec *, int, int, _xivec *, int);
_XEXPORT extern _xivec *_xivec_resize(_xivec *, int);
_XEXPORT extern int _xivec_free(_xivec *);
_XEXPORT extern int _xivec_get_vars(int, ...);
_XEXPORT extern int _xivec_resize_vars(int, ...);
_XEXPORT extern int _xivec_free_vars(_xivec **, ...);

/* ivector op */
_XEXPORT extern _xivec *_xivec_add(const _xivec *, const _xivec *, _xivec *);
_XEXPORT extern _xivec *_xivec_sub(const _xivec *, const _xivec *, _xivec *);
_XEXPORT extern _xivec *_xivec_sort(_xivec *, _xperm *);
_XEXPORT extern int _xivec_min(_xivec *, int *);
_XEXPORT extern int _xivec_max(_xivec *, int *);

/* ivecotr I/O routines */
_XEXPORT extern void _xivec_dump(FILE *, const _xivec *);
_XEXPORT extern void _xivec_foutput(FILE *, const _xivec *);
_XEXPORT extern _xivec *_xivec_finput(FILE *, _xivec *);

/* ivector macros */
#define _xivec_ifree(iv) (_xivec_free(iv), (iv)=(_xivec *)NULL)
#define _xivec_output(iv) _xivec_foutput(stdout,iv)
#define _xivec_input(iv)  _xivec_finput(stdin,iv)

/* band routines */
_XEXPORT extern _xband *_xband_get(int, int, int);
_XEXPORT extern _xband *_xband_zero(_xband *);
_XEXPORT extern _xband *_xband_copy(const _xband *, _xband *);
_XEXPORT extern _xband *_xband_resize(_xband *, int, int, int);
_XEXPORT extern int _xband_free(_xband *);

/* band op */
_XEXPORT extern _xband *_xband_transp(const _xband *, _xband *);
_XEXPORT extern _xband *_xband_bds_mltadd(const _xband *, const _xband *, double, _xband *);
_XEXPORT extern _xband *_xband_sbd_mltadd(_xreal, const _xband *, _xband *);

_XEXPORT extern _xvec *_xband_bdv_mltadd(const _xvec *, const _xvec *, const _xband *, double, _xvec *);
_XEXPORT extern _xvec *_xband_vbd_mltadd(const _xvec *, const _xvec *, const _xband *, double, _xvec *);

_XEXPORT extern _xmat *_xband_band2mat(const _xband *, _xmat *);

/* band macros */
#define _xband_entry(A,i,j)       _xband_get_val(A,i,j)
#define _xband_set_val(A,i,j,val) ((A)->mat->me[(A)->lb+(j)-(i)][(j)] = (val))
#define _xband_add_val(A,i,j,val) ((A)->mat->me[(A)->lb+(j)-(i)][(j)] += (val))
#define _xband_get_val(A,i,j)     ((A)->mat->me[(A)->lb+(j)-(i)][(j)])
#define _xband_chk_idx(A,i,j)     ((i)>=max(0,(j)-(A)->ub) && (j)>=max(0,(i)-(A)->lb) && (i)<(A)->mat->n && (j)<(A)->mat->n)

/* factorisation routines */
_XEXPORT extern _xvec *_xmat_qsolve(const _xmat *, const _xvec *, const _xvec *, _xvec *, _xvec *);
_XEXPORT extern _xvec *_xmat_dsolve(const _xmat *, const _xvec *, _xvec *);
_XEXPORT extern _xvec *_xmat_usolve(const _xmat *, const _xvec *, _xvec *, double);
_XEXPORT extern _xvec *_xmat_lsolve(const _xmat *, const _xvec *, _xvec *, double);
_XEXPORT extern _xvec *_xmat_utsolve(const _xmat *, const _xvec *, _xvec *, double);
_XEXPORT extern _xvec *_xmat_ltsolve(const _xmat *, const _xvec *, _xvec *, double);
_XEXPORT extern _xvec *_xmat_qrsolve(const _xmat *, const _xvec *, const _xvec *, _xvec *);
_XEXPORT extern _xvec *_xmat_chsolve(const _xmat *, const _xvec *, _xvec *);
_XEXPORT extern _xvec *_xmat_lusolve(const _xmat *, _xperm *, const _xvec *, _xvec *);
_XEXPORT extern _xvec *_xmat_lutsolve(const _xmat *, _xperm *, const _xvec *, _xvec *);
_XEXPORT extern _xvec *_xmat_bkpsolve(const _xmat *, _xperm *, const _xperm *, const _xvec *, _xvec *);
_XEXPORT extern _xvec *_xmat_ldlsolve(const _xmat *, const _xvec *, _xvec *);
_XEXPORT extern _xvec *_xmat_qrtsolve(const _xmat *, const _xvec *, const _xvec *, _xvec *);
_XEXPORT extern _xvec *_xmat_qrcpsolve(const _xmat *, const _xvec *, _xperm *, const _xvec *, _xvec *);

_XEXPORT extern _xvec *_xband_lusolve(const _xband *, _xperm *, const _xvec *, _xvec *);
_XEXPORT extern _xvec *_xband_ldlsolve(const _xband *, const _xvec *, _xvec *);

_XEXPORT extern _xmat *_xmat_hfactor(_xmat *, _xvec *, _xvec *);
_XEXPORT extern _xmat *_xmat_chfactor(_xmat *);
_XEXPORT extern _xmat *_xmat_lufactor(_xmat *, _xperm *);
_XEXPORT extern _xmat *_xmat_qrfactor(_xmat *, _xvec *);
_XEXPORT extern _xmat *_xmat_mchfactor(_xmat *, double);
_XEXPORT extern _xmat *_xmat_ldlfactor(_xmat *);
_XEXPORT extern _xmat *_xmat_bkpfactor(_xmat *, _xperm *, _xperm *);
_XEXPORT extern _xmat *_xmat_qrcpfactor(_xmat *, _xvec *, _xperm *);

_XEXPORT extern _xband *_xband_lufactor(_xband *, _xperm *);
_XEXPORT extern _xband *_xband_ldlfactor(_xband *);

_XEXPORT extern double _xmat_qrcondest(const _xmat *);
_XEXPORT extern double _xmat_lucondest(const _xmat *, _xperm *);

_XEXPORT extern _xmat *_xmat_ldlupdate(_xmat *, _xvec *, double);
_XEXPORT extern _xmat *_xmat_qrupdate(_xmat *, _xmat *, _xvec *, _xvec *);

_XEXPORT extern _xmat *_xmat_makeh(const _xmat *, _xmat *);
_XEXPORT extern _xmat *_xmat_makeq(const _xmat *, const _xvec *, _xmat *);
_XEXPORT extern _xmat *_xmat_maker(const _xmat *, _xmat *);
_XEXPORT extern _xmat *_xmat_makehq(_xmat *, _xvec *, _xvec *, _xmat *);

_XEXPORT extern _xmat *_xmat_rot_rows(const _xmat *, unsigned int, unsigned int, double, double, _xmat *);
_XEXPORT extern _xmat *_xmat_rot_cols(const _xmat *, unsigned int, unsigned int, double, double, _xmat *);

_XEXPORT extern _xmat *_xmat_hhtrrows(_xmat *, unsigned int, unsigned int, const _xvec *, double);
_XEXPORT extern _xmat *_xmat_hhtrcols(_xmat *, unsigned int, unsigned int, const _xvec *, double);
_XEXPORT extern _xmat *_xmat_hhvtrcols(_xmat *, unsigned int, unsigned int, const _xvec *, double, _xvec *);

/* eigenvalue routines */
_XEXPORT extern void _xmat_schur_evals(_xmat *, _xvec *, _xvec *);
_XEXPORT extern _xmat *_xmat_schur(_xmat *, _xmat *);
_XEXPORT extern _xmat *_xmat_schur_vecs(_xmat *, _xmat *, _xmat *, _xmat *);

_XEXPORT extern _xvec *_xmat_trieig(_xvec *, _xvec *, _xmat *);
_XEXPORT extern _xvec *_xmat_symmeig(const _xmat *, _xmat *, _xvec *);

/* singular value decomposition */
_XEXPORT extern _xvec *_xmat_svd(_xmat *, _xmat *, _xmat *, _xvec *);
_XEXPORT extern _xvec *_xmat_bisvd(_xvec *, _xvec *, _xmat *, _xmat *);

/* sparse routines */
_XEXPORT extern _xspmat *_xspmat_get(int, int, int);
_XEXPORT extern _xspmat *_xspmat_zero(_xspmat *);
_XEXPORT extern _xspmat *_xspmat_copy(const _xspmat *);
_XEXPORT extern _xspmat *_xspmat_copy2(const _xspmat *, _xspmat *);
_XEXPORT extern _xspmat *_xspmat_resize(_xspmat *, int, int);
_XEXPORT extern _xspmat *_xspmat_compact(_xspmat *, double);

_XEXPORT extern double _xspmat_get_val(const _xspmat *, int, int);
_XEXPORT extern double _xspmat_set_val(_xspmat *, int, int, double);

_XEXPORT extern _xvec *_xspmat_mv_mlt(const _xspmat *, const _xvec *, _xvec *);
_XEXPORT extern _xvec *_xspmat_vm_mlt(const _xspmat *, const _xvec *, _xvec *);

_XEXPORT extern int _xspmat_free(_xspmat *);
_XEXPORT extern int _xspmat_get_vars(int, int, int, ...);
_XEXPORT extern int _xspmat_resize_vars(int, int, ...);
_XEXPORT extern int _xspmat_free_vars(_xspmat **, ...);

_XEXPORT extern _xspmat *_xspmat_col_access(_xspmat *);
_XEXPORT extern _xspmat *_xspmat_diag_access(_xspmat *);
_XEXPORT extern int _xspmat_chk_col_access(const _xspmat *);

/* sparse op */
_XEXPORT extern _xspmat *_xspmat_add(const _xspmat *, const _xspmat *, _xspmat *);
_XEXPORT extern _xspmat *_xspmat_sub(const _xspmat *, const _xspmat *, _xspmat *);
_XEXPORT extern _xspmat *_xspmat_smlt(const _xspmat *, double, _xspmat *);
_XEXPORT extern _xspmat *_xspmat_mltadd(const _xspmat *, const _xspmat *, double, _xspmat *);

_XEXPORT extern _xmat *_xspmat_m2dense(const _xspmat *, _xmat *);

/* sparse I/O */
_XEXPORT extern _xspmat *_xspmat_finput(FILE *);
_XEXPORT extern void _xspmat_dump(FILE *, const _xspmat *);
_XEXPORT extern void _xspmat_foutput(FILE *, const _xspmat *);
_XEXPORT extern void _xspmat_foutput2(FILE *, const _xspmat *);

/* sparse row op */
_XEXPORT extern _xsprow *_xsprow_get(int);
_XEXPORT extern _xsprow *_xsprow_xpd(_xsprow *, int, int);
_XEXPORT extern _xsprow *_xsprow_resize(_xsprow *, int, int);
_XEXPORT extern _xsprow *_xsprow_copy(const _xsprow *, const _xsprow *, _xsprow *, int);
_XEXPORT extern _xsprow *_xsprow_merge(const _xsprow *, const _xsprow *, _xsprow *, int);
_XEXPORT extern _xsprow *_xsprow_mltadd(const _xsprow *, const _xsprow *, double, int, _xsprow *, int);
_XEXPORT extern _xsprow *_xsprow_add(const _xsprow *, const _xsprow *, int, _xsprow *, int);
_XEXPORT extern _xsprow *_xsprow_sub(const _xsprow *, const _xsprow *, int, _xsprow *, int);
_XEXPORT extern _xsprow *_xsprow_smlt(const _xsprow *, double, int, _xsprow *, int);

_XEXPORT extern double _xsprow_set_val(_xsprow *, int, double);
_XEXPORT extern int _xsprow_free(_xsprow *);
_XEXPORT extern int _xsprow_idx(const _xsprow *, int);

_XEXPORT extern void _xsprow_dump(FILE *, const _xsprow *);
_XEXPORT extern void _xsprow_foutput(FILE *, const _xsprow *);

/* sparse matrix & row macros */
#define _xspmat_input()                  _xspmat_finput(stdin)
#define _xspmat_output(A)                _xspmat_foutput(stdout,(A))
#define _xspmat_output2(A)               _xspmat_foutput2(stdout,(A))
#define _xspmat_sfree(A)                (_xspmat_free((A)),(A)=(_xspmat *)NULL)

#define _xsprow_output(r)                _xsprow_foutput(stdout,(r))
#define _xsprow_vmltadd(r1,r2,alpha,out) _xsprow_mltadd(r1,r2,alpha,0,out)
#define _xsprow_idx2(r,c,hint)           (((hint) >= 0 && (hint) < (r)->len && \
                                         (r)->elt[hint].col == (c)) ? (hint) : _xsprow_idx((r),(c)))
#define _xsprow_fixindex(idx)            ((idx) == -1 ? (_xerror(_XE_BOUNDS,"_xsprow_fixindex"),0) : \
                                          (idx) < 0 ? -((idx)+2) : (idx))

/* sparse factorisation routines */
_XEXPORT extern _xspmat *_xspmat_chfactor(_xspmat *);
_XEXPORT extern _xspmat *_xspmat_ichfactor(_xspmat *);
_XEXPORT extern _xspmat *_xspmat_chsymb(_xspmat *);
_XEXPORT extern _xspmat *_xspmat_lufactor(_xspmat *, _xperm *, double);
_XEXPORT extern _xspmat *_xspmat_ilufactor(_xspmat *, double);
_XEXPORT extern _xspmat *_xspmat_bkpfactor(_xspmat *, _xperm *, _xperm *, double);

_XEXPORT extern _xvec *_xspmat_chsolve(_xspmat *, const _xvec *, _xvec *);
_XEXPORT extern _xvec *_xspmat_lusolve(const _xspmat *, _xperm *, const _xvec *, _xvec *);
_XEXPORT extern _xvec *_xspmat_lutsolve(_xspmat *, _xperm *, const _xvec *, _xvec *);
_XEXPORT extern _xvec *_xspmat_bkpsolve(_xspmat *, _xperm *, _xperm *, const _xvec *, _xvec *);

_XEXPORT extern _xrow_elt *_xspmat_bump_col(const _xspmat *, int, int *, int *);
_XEXPORT extern _xrow_elt *_xspmat_chase_col(const _xspmat *, int, int *, int *, int);
_XEXPORT extern _xrow_elt *_xspmat_chase_past(const _xspmat *, int, int *, int *, int);

_XEXPORT extern void _xspmat_scan_to(_xspmat *, _xivec *, _xivec *, _xivec *, int);
_XEXPORT extern int _xspmat_cg_set_maxiter(int);

_XEXPORT extern _xvec *_xspmat_spcgs(_xspmat *, _xvec *, _xvec *, double, _xvec *);
_XEXPORT extern _xvec *_xspmat_cgs(_xvec *(*)(void *, _xvec *, _xvec *),void *, _xvec *, _xvec *, double, _xvec *);
_XEXPORT extern _xvec *_xspmat_sppccg(_xspmat *, _xspmat *, _xvec *, double, _xvec *);
_XEXPORT extern _xvec *_xspmat_pccg(_xvec *(*)(void *, _xvec *, _xvec *), void *, 
                                    _xvec *(*)(void *, _xvec *, _xvec *), void *, _xvec *, double, _xvec *);
_XEXPORT extern _xvec *_xspmat_splsqr(_xspmat *, _xvec *, double, _xvec *);
_XEXPORT extern _xvec *_xspmat_lsqr(_xvec *(*)(void *, _xvec *, _xvec *), 
                                    _xvec *(*)(void *, _xvec *, _xvec *),void *, _xvec *, double, _xvec *);

_XEXPORT extern void _xspmat_splanczos(_xspmat *, int, _xvec *, _xvec *, _xvec *, _xreal *, _xmat *);
_XEXPORT extern void _xspmat_lanczos(_xvec *(*)(void *, _xvec *, _xvec *), void *, int, 
                                     _xvec *, _xvec *, _xvec *, _xreal *, _xmat *);

_XEXPORT extern _xvec *_xspmat_splanczos2(_xspmat *,int, _xvec *, _xvec *, _xvec *);
_XEXPORT extern _xvec *_xspmat_lanczos2(_xvec *(*)(void *, _xvec *, _xvec *), void *, int, 
                                        _xvec *, _xvec *, _xvec *); 

/* iterative macros */
#define _xiter_ax(ip,fun,fun_par)  (ip->ax=(_xfun_ax)(fun),ip->a_par=(void *)(fun_par),0)
#define _xiter_atx(ip,fun,fun_par) (ip->atx=(_xfun_ax)(fun),ip->at_par=(void *)(fun_par),0)
#define _xiter_bx(ip,fun,fun_par)  (ip->bx=(_xfun_ax)(fun),ip->b_par=(void *)(fun_par),0)
#define _xiter_btx(ip,fun,fun_par) (ip->btx=(_xfun_ax)(fun),ip->bt_par=(void *)(fun_par),0)
#define _xiter_ifree(ip)           (_xiter_free(ip), (ip)=(_xiter *)NULL)

/* iterative routines */
_XEXPORT extern _xiter *_xiter_get(int, int);
_XEXPORT extern _xiter *_xiter_copy(const _xiter *, _xiter *);
_XEXPORT extern _xiter *_xiter_copy2(_xiter *, _xiter *);
_XEXPORT extern _xiter *_xiter_resize(_xiter *, int, int);

_XEXPORT extern void _xiter_dump(FILE *, _xiter *);
_XEXPORT extern void _xiter_std_info(const _xiter *, double, _xvec *, _xvec *);

_XEXPORT extern int _xiter_free(_xiter *);
_XEXPORT extern int _xiter_std_stop_crit(const _xiter *, double, _xvec *, _xvec *);

_XEXPORT extern _xspmat *_xiter_gen_sym(int, int);
_XEXPORT extern _xspmat *_xiter_gen_nonsym(int, int, int, double);
_XEXPORT extern _xspmat *_xiter_gen_nonsym_posdef(int, int);

/* iterative procedures */
_XEXPORT extern _xvec *_xiter_cg(_xiter *);
_XEXPORT extern _xvec *_xiter_cg1(_xiter *);
_XEXPORT extern _xvec *_xiter_spcg(_xspmat *, _xspmat *, _xvec *, double, _xvec *, int, int *);
_XEXPORT extern _xvec *_xiter_cgs(_xiter *, _xvec *);
_XEXPORT extern _xvec *_xiter_spcgs(_xspmat *, _xspmat *, _xvec *, _xvec *, double, _xvec *, int, int *);
_XEXPORT extern _xvec *_xiter_lsqr(_xiter *);
_XEXPORT extern _xvec *_xiter_splsqr(_xspmat *, _xvec *, double, _xvec *, int, int *);
_XEXPORT extern _xvec *_xiter_gmres(_xiter *);
_XEXPORT extern _xvec *_xiter_spgmres(_xspmat *, _xspmat *, _xvec *, double, _xvec *, int, int, int *);
_XEXPORT extern _xvec *_xiter_mgcr(_xiter *);
_XEXPORT extern _xvec *_xiter_spmgcr(_xspmat *, _xspmat *, _xvec *, double, _xvec *, int, int, int *);
_XEXPORT extern _xvec *_xiter_lanczos2(_xiter *, _xvec *, _xvec *);
_XEXPORT extern _xvec *_xiter_splanczos2(_xspmat *, int, _xvec *, _xvec *, _xvec *);
_XEXPORT extern _xvec *_xiter_cgne(_xiter *);
_XEXPORT extern _xvec *_xiter_spcgne(_xspmat *, _xspmat *, _xvec *, double, _xvec *, int, int *);
_XEXPORT extern _xmat *_xiter_arnoldi_iref(_xiter *, _xreal *, _xmat *, _xmat *);
_XEXPORT extern _xmat *_xiter_arnoldi(_xiter *, _xreal *, _xmat *, _xmat *);
_XEXPORT extern _xmat *_xiter_sparnoldi(_xspmat *, _xvec *, int, _xreal *, _xmat *, _xmat *);
_XEXPORT extern void  _xiter_lanczos(_xiter *, _xvec *, _xvec *, _xreal *, _xmat *);
_XEXPORT extern void  _xiter_splanczos(_xspmat *, int, _xvec *, _xvec *, _xvec *, _xreal *, _xmat *);

/* _xcomplex routines */
_XEXPORT extern double _xzabs(_xcomplex);
_XEXPORT extern _xcomplex _xzmake(double, double);
_XEXPORT extern _xcomplex _xzadd(_xcomplex, _xcomplex);
_XEXPORT extern _xcomplex _xzsub(_xcomplex, _xcomplex);
_XEXPORT extern _xcomplex _xzmlt(_xcomplex, _xcomplex);
_XEXPORT extern _xcomplex _xzinv(_xcomplex);
_XEXPORT extern _xcomplex _xzdiv(_xcomplex, _xcomplex);
_XEXPORT extern _xcomplex _xzsqrt(_xcomplex);
_XEXPORT extern _xcomplex _xzexp(_xcomplex);
_XEXPORT extern _xcomplex _xzlog(_xcomplex);
_XEXPORT extern _xcomplex _xzconj(_xcomplex);
_XEXPORT extern _xcomplex _xzneg(_xcomplex);

_XEXPORT extern _xcomplex _xzfinput(FILE *);
_XEXPORT extern void _xzfoutput(FILE *, _xcomplex);

#define _xzinput()   _xzfinput(stdin)
#define _xzoutput(z) _xzfoutput(stdout,z)

/* _xzvec routines */
_XEXPORT extern _xzvec *_xzvec_get(int);
_XEXPORT extern _xzvec *_xzvec_resize(_xzvec *,int);
_XEXPORT extern _xzvec *_xzvec_zero(_xzvec *);
_XEXPORT extern _xzvec *_xzvec_rand(_xzvec *);
_XEXPORT extern _xzvec *_xzvec_copy(const _xzvec *, _xzvec *, int);
_XEXPORT extern _xzvec *_xzvec_move(const _xzvec *, int, int, _xzvec *, int);
_XEXPORT extern _xzvec *_xzvec_mlt(_xcomplex, const _xzvec *, _xzvec *);
_XEXPORT extern _xzvec *_xzvec_add(const _xzvec *, const _xzvec *, _xzvec *);
_XEXPORT extern _xzvec *_xzvec_sub(const _xzvec *, const _xzvec *, _xzvec *);
_XEXPORT extern _xzvec *_xzvec_star(const _xzvec *, const _xzvec *, _xzvec *);
_XEXPORT extern _xzvec *_xzvec_slash(const _xzvec *, const _xzvec *, _xzvec *);
_XEXPORT extern _xzvec *_xzvec_mltadd(const _xzvec *, const _xzvec *, _xcomplex, _xzvec *);
_XEXPORT extern _xzvec *_xzvec_lincomb(int, const _xzvec **, const _xcomplex *, _xzvec *);
_XEXPORT extern _xzvec *_xzvec_linlist(_xzvec *, _xzvec *, _xcomplex, ...);
_XEXPORT extern _xzvec *_xzvec_map(_xcomplex (*)(_xcomplex),const _xzvec *,_xzvec *);
_XEXPORT extern _xzvec *_xzvec_vmap(_xcomplex (*)(void *,_xcomplex),void *,const _xzvec *,_xzvec *);

_XEXPORT extern int _xzvec_get_vars(int, ...);
_XEXPORT extern int _xzvec_resize_vars(int, ...);
_XEXPORT extern int _xzvec_free_vars(_xzvec**, ...);
_XEXPORT extern int _xzvec_free(_xzvec *);

_XEXPORT extern double _xzvec_norm1(_xzvec *, _xvec *);
_XEXPORT extern double _xzvec_norm2(_xzvec *, _xvec *);
_XEXPORT extern double _xzvec_norm_inf(_xzvec *, _xvec *);

_XEXPORT extern _xcomplex _xzvec_sum(const _xzvec *);
_XEXPORT extern _xcomplex _xzvec_in_prod(const _xzvec *, const _xzvec *, unsigned int, unsigned int);

_XEXPORT extern void _xzvec_dump(FILE *, _xzvec *);
_XEXPORT extern void _xzvec_foutput(FILE *, _xzvec *);
_XEXPORT extern _xzvec *_xzvec_finput(FILE *, _xzvec *);

/* add for matlab */
_XEXPORT extern _xvec *_xzvec_abs(const _xzvec *, _xvec *);
_XEXPORT extern _xvec *_xzvec_angle(const _xzvec *, _xvec *);
_XEXPORT extern _xvec *_xzvec_real(const _xzvec *, _xvec *);
_XEXPORT extern _xvec *_xzvec_imag(const _xzvec *, _xvec *);

#define _xzvec_vcopy(x,y)       _xzvec_copy(x,y,0)
#define _xzvec_vnorm1(x)        _xzvec_norm1(x,(_xvec *)NULL)
#define _xzvec_vnorm2(x)        _xzvec_norm2(x,(_xvec *)NULL)
#define _xzvec_vnorm_inf(x)     _xzvec_norm_inf(x,(_xvec *)NULL)
#define _xzvec_vin_prod(a,b)    _xzvec_in_prod(a,b,0,1)
#define _xzvec_vfree(x)        (_xzvec_free(x),(x)=(_xzvec *)NULL)
#define _xzvec_input(x)         _xzvec_finput(stdin,x)
#define _xzvec_output(x)        _xzvec_foutput(stdout,x)
#define _xzvec_entry(x,i)       _xzvec_get_val(x,i)
#define _xzvec_set_val(x,i,val) ((x)->ve[(i)] = (val))
#define _xzvec_add_val(x,i,val) ((x)->ve[(i)] = _xzadd((x)->ve[(i)],(val)))
#define _xzvec_sub_val(x,i,val) ((x)->ve[(i)] = _xzsub((x)->ve[(i)],(val)))
#define _xzvec_get_val(x,i)     ((x)->ve[(i)])

/* _xzmat routines */
_XEXPORT extern _xzmat *_xzmat_get(int, int);
_XEXPORT extern _xzmat *_xzmat_resize(_xzmat *, int, int);
_XEXPORT extern _xzmat *_xzmat_zero(_xzmat *);
_XEXPORT extern _xzmat *_xzmat_rand(_xzmat *);
_XEXPORT extern _xzmat *_xzmat_copy(const _xzmat *, _xzmat *, int, int);
_XEXPORT extern _xzmat *_xzmat_move(const _xzmat *,int, int, int, int, _xzmat *, int, int);

_XEXPORT extern _xzmat *_xzmat_vm_move(const _xzvec *, int, _xzmat *, int, int, int, int);           
_XEXPORT extern _xzmat *_xzmat_add(_xzmat *,_xzmat *,_xzmat *);
_XEXPORT extern _xzmat *_xzmat_sub(_xzmat *,_xzmat *,_xzmat *);
_XEXPORT extern _xzmat *_xzmat_mlt(_xzmat *,_xzmat *,_xzmat *);
_XEXPORT extern _xzmat *_xzmat_ma_mlt(_xzmat *,_xzmat *,_xzmat *);
_XEXPORT extern _xzmat *_xzmat_am_mlt(_xzmat *,_xzmat *,_xzmat *);
_XEXPORT extern _xzmat *_xzmat_adjoint(_xzmat *,_xzmat *);
_XEXPORT extern _xzmat *_xzmat_swap_rows(_xzmat *,int,int,int,int);
_XEXPORT extern _xzmat *_xzmat_swap_cols(_xzmat *,int,int,int,int);
_XEXPORT extern _xzmat *_xzmat_mz_mltadd(_xzmat *,_xzmat *,_xcomplex,_xzmat *);
_XEXPORT extern _xzmat *_xzmat_sm_mlt(_xcomplex,_xzmat *,_xzmat *);

_XEXPORT extern _xzvec *_xzmat_mv_move(const _xzmat *, int, int, int, int, _xzvec *, int);
_XEXPORT extern _xzvec *_xzmat_mv_mlt(_xzmat *,_xzvec *,_xzvec *);
_XEXPORT extern _xzvec *_xzmat_vm_mlt(_xzmat *,_xzvec *,_xzvec *);
_XEXPORT extern _xzvec *_xzmat_mv_mltadd(_xzvec *,_xzvec *,_xzmat *,_xcomplex,_xzvec *);
_XEXPORT extern _xzvec *_xzmat_vm_mltadd(_xzvec *,_xzvec *,_xzmat *,_xcomplex,_xzvec *);

_XEXPORT extern double _xzmat_norm1(_xzmat *);
_XEXPORT extern double _xzmat_norm_inf(_xzmat *);
_XEXPORT extern double _xzmat_norm_frob(_xzmat *);

_XEXPORT extern int _xzmat_free(_xzmat *);
_XEXPORT extern int _xzmat_get_vars(int,int, ...);
_XEXPORT extern int _xzmat_resize_vars(int,int, ...);
_XEXPORT extern int _xzmat_free_vars(_xzmat**, ...);

_XEXPORT extern _xzvec *_xzmat_get_row(_xzmat *, int, _xzvec *);
_XEXPORT extern _xzvec *_xzmat_get_col(_xzmat *, int, _xzvec *);
_XEXPORT extern _xzmat *_xzmat_set_row(_xzmat *, int, _xzvec *);
_XEXPORT extern _xzmat *_xzmat_set_col(_xzmat *, int, _xzvec *);

_XEXPORT extern void _xzmat_dump(FILE *, _xzmat *);
_XEXPORT extern void _xzmat_foutput(FILE *, _xzmat *);
_XEXPORT extern _xzmat *_xzmat_finput(FILE *, _xzmat *);

#define _xzmat_mcopy(A,B)           _xzmat_copy(A,B,0,0)
#define _xzmat_input(A)             _xzmat_finput(stdin,A)
#define _xzmat_output(A)            _xzmat_foutput(stdout,A)
#define _xzmat_mfree(A)            (_xzmat_free(A),(A) = (_xzmat *)NULL)
#define _xzmat_entry(A,i,j)         _xzmat_get_val(A,i,j)
#define _xzmat_set_val(A,i,j,val)  ((A)->me[(i)][(j)] = (val))
#define _xzmat_add_val(A,i,j,val)  ((A)->me[(i)][(j)] = _xzadd((A)->me[(i)][(j)],(val)))
#define _xzmat_sub_val(A,i,j,val)  ((A)->me[(i)][(j)] = _xzsub((A)->me[(i)][(j)],(val)))
#define _xzmat_get_val(A,i,j)      ((A)->me[(i)][(j)])

/* z factorisation routines */
_XEXPORT extern _xzvec *_xzmat_usolve(_xzmat *, _xzvec *, _xzvec *, double);
_XEXPORT extern _xzvec *_xzmat_lsolve(_xzmat *, _xzvec *, _xzvec *, double);
_XEXPORT extern _xzvec *_xzmat_uasolve(_xzmat *, _xzvec *, _xzvec *, double);
_XEXPORT extern _xzvec *_xzmat_dsolve(_xzmat *, _xzvec *, _xzvec *);
_XEXPORT extern _xzvec *_xzmat_lasolve(_xzmat *, _xzvec *, _xzvec *, double);
_XEXPORT extern _xzvec *_xzmat_qsolve(_xzmat *, _xzvec *, _xzvec *, _xzvec *, _xzvec *);
_XEXPORT extern _xzvec *_xzvec_hhvec(_xzvec *,int, _xreal *, _xzvec *, _xcomplex *);
_XEXPORT extern _xzvec *_xzvec_hhtrvec(_xzvec *, double, int, _xzvec *, _xzvec *);
_XEXPORT extern _xzvec *_xzmat_lusolve(_xzmat *, _xperm *, _xzvec *, _xzvec *);
_XEXPORT extern _xzvec *_xzmat_luasolve(_xzmat *, _xperm *, _xzvec *, _xzvec *);
_XEXPORT extern _xzvec *_xzmat_qrsolve(_xzmat *, _xzvec *, _xzvec *, _xzvec *);
_XEXPORT extern _xzvec *_xzmat_qrasolve(_xzmat *, _xzvec *, _xzvec *, _xzvec *);
_XEXPORT extern _xzvec *_xzmat_qrcpsolve(_xzmat *, _xzvec *, _xperm *, _xzvec *, _xzvec *);
_XEXPORT extern _xzvec *_xzmat_umlt(_xzmat *, _xzvec *, _xzvec *);
_XEXPORT extern _xzvec *_xzmat_uamlt(_xzmat *, _xzvec *, _xzvec *);
_XEXPORT extern _xzvec *_xzvec_rot_vec(_xzvec *, int, int, double, _xcomplex, _xzvec *);

_XEXPORT extern _xzmat *_xzmat_hhtrrows(_xzmat *, int, int, _xzvec *, double);
_XEXPORT extern _xzmat *_xzmat_hhtrcols(_xzmat *, int, int, _xzvec *, double);
_XEXPORT extern _xzmat *_xzmat_hhvtrcols(_xzmat *, int, int, _xzvec *, double, _xzvec *);
_XEXPORT extern _xzmat *_xzmat_hfactor(_xzmat *, _xzvec *);
_XEXPORT extern _xzmat *_xzmat_hqunpack(_xzmat *, _xzvec *, _xzmat *, _xzmat *);
_XEXPORT extern _xzmat *_xzmat_qrfactor(_xzmat *, _xzvec *);
_XEXPORT extern _xzmat *_xzmat_lufactor(_xzmat *, _xperm *);
_XEXPORT extern _xzmat *_xzmat_qrcpfactor(_xzmat *, _xzvec *, _xperm *);
_XEXPORT extern _xzmat *_xzmat_makeq(_xzmat *, _xzvec *, _xzmat *);
_XEXPORT extern _xzmat *_xzmat_maker(_xzmat *, _xzmat *);
_XEXPORT extern _xzmat *_xzmat_rot_rows(_xzmat *, int, int, double, _xcomplex, _xzmat *);
_XEXPORT extern _xzmat *_xzmat_rot_cols(_xzmat *, int, int, double, _xcomplex, _xzmat *);
_XEXPORT extern _xzmat *_xzmat_inverse(_xzmat *, _xzmat *);
_XEXPORT extern _xzmat *_xzmat_schur(_xzmat *, _xzmat *);

_XEXPORT extern double _xzmat_qrcondest(_xzmat *);
_XEXPORT extern double _xzmat_lucondest(_xzmat *, _xperm *);

_XEXPORT extern void _xzvec_givens(_xcomplex, _xcomplex, _xreal *, _xcomplex *);

#ifdef __cplusplus
}
#endif

#endif/*_XPOWER_MATRIX_H_*/

