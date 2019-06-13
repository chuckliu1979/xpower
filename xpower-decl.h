/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines Dispatcher Training Simulator (DTS)  
** models for system information.
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XPOWER_DECL_H_
#define _XPOWER_DECL_H_
 
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
  #if defined(_XEXPORT)
    #undef _XEXPORT
    #define _XEXPORT _declspec(dllexport)
  #else
    #define _XEXPORT _declspec(dllimport)
  #endif/*_XEXPORT*/
#else
  #define _XEXPORT
#endif

#ifdef __MINGW32__
#undef _XEXPORT
#define _XEXPORT _CRTIMP
#endif

#endif/*_XPOWER_DECL_H_*/

