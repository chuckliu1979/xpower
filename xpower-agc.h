/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines Dispatcher Training Simulator (DTS)  
** AGC routines.
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XPOWER_AGC_H_
#define _XPOWER_AGC_H_

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * ACEi = Bi*biasFreq + biasPi
 *
 * TBC  - Tie line Bias Control
 * CFC  - Constant Frequency Control
 * CNIC - Constant Net Interchange Control
 *
 */
#define _XAGC_TBC  1
#define _XAGC_CFC  2
#define _XAGC_CNIC 3

#define _XALLOC_BY_CAPACITY        1
#define _XALLOC_BY_MOMENTOFINERTIA 2
#define _XALLOC_BY_FREQFACTOR      3

#ifdef __cplusplus
}
#endif

#endif/*_XPOWER_AGC_H_*/

