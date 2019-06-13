/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines the struct used for RTDBCCombustionTurbine, and 
** must be same as that defined in rtdbblock 
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XCSGC3000_STRUCT4COMBUSTIONTURBINE_H_
#define _XCSGC3000_STRUCT4COMBUSTIONTURBINE_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef COMBUSTIONTURBINE_ID
#define COMBUSTIONTURBINE_ID 670
#endif

struct _xcombustionturbine_para {
    int m_ID;
    char m_name[32];
    char m_description[64];
    float m_capabilityVersusFrequency;
    float m_timeConstant;
};

#ifdef __cplusplus
}
#endif

#endif/*_XCSGC3000_STRUCT4COMBUSTIONTURBINE_H_*/
 
