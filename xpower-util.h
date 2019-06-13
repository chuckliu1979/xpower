/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines Dispatcher Training Simulator (DTS)  
** util routines.
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XPOWER_UTIL_H_
#define _XPOWER_UTIL_H_

#include "xpower-matrix.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _XMATH
  _XEXPORT extern float _xutil_sqrt(float val);
  _XEXPORT extern float _xutil_fabs(float val);
  _XEXPORT extern float _xutil_rsqrt(float val);
#else
  #define _xutil_sqrt(x)  sqrt(x)
  #define _xutil_fabs(x)  fabs(x)
  #define _xutil_rsqrt(x) 1.0/sqrt(x)
#endif

_XEXPORT extern void _xutil_sleep(int milliseconds);
_XEXPORT extern void _xutil_clear(FILE *fd);
_XEXPORT extern char *_xutil_basename(char *path);

_XEXPORT extern int _xutil_call_command(char **nargv);
_XEXPORT extern double _xutil_rk4(_xvec *(*f)(double,_xvec *,_xvec *),double t,double h,_xvec *x);

#ifdef __cplusplus
}
#endif

#endif/*_XPOWER_UTIL_H_*/

