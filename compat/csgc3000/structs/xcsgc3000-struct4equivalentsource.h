/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines the struct used for RTDBCEquivalentsource, and 
** must be same as that defined in rtdbblock 
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XCSGC3000_STRUCT4EQUIVALENTSOURCE_H_
#define _XCSGC3000_STRUCT4EQUIVALENTSOURCE_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EQUIVALENTSOURCE_ID
#define EQUIVALENTSOURCE_ID 680
#endif

struct _xequivalentsource_para {
    int m_ID;
    char m_name[32];
    char m_description[64];
    int m_terminal0_id;
    int m_terminal0_idx;
    char  m_phases[4];
    float m_r;
    float m_x;
    float m_Rn;
    float m_Xn;
    float m_R0;
    float m_X0;
    float m_voltageMagnitude;
    float m_voltageAngle;
    int m_basevoltage_id;
    int m_basevoltage_idx;
};

struct _xequivalentsource_real {
    float m_InjP;
    float m_InjQ;
    float m_InjPa;
    float m_InjQa;
    float m_InjPb;
    float m_InjQb;
    float m_InjPc;
    float m_InjQc;
    int m_islandFlag;
    int m_engzFlag;
};

struct _xequivalentsource_real2 {
    int m_measId;
    int m_measIdx;
};

struct _xequivalentsource_estm {
    float m_InjP;
    float m_InjQ;
    float m_InjPa;
    float m_InjQa;
    float m_InjPb;
    float m_InjQb;
    float m_InjPc;
    float m_InjQc;
    int m_islandFlag;
    int m_engzFlag;
};

struct _xequivalentsource_calc {
    float m_InjP;
    float m_InjQ;
    float m_InjPa;
    float m_InjQa;
    float m_InjPb;
    float m_InjQb;
    float m_InjPc;
    float m_InjQc;
    int m_islandFlag;
    int m_engzFlag;
};

struct _xequivalentsource_anly {
    float m_InjP;
    float m_InjQ;
    float m_InjPa;
    float m_InjQa;
    float m_InjPb;
    float m_InjQb;
    float m_InjPc;
    float m_InjQc;
    int m_islandFlag;
    int m_engzFlag;
};

#ifdef __cplusplus
}
#endif

#endif/*_XCSGC3000_STRUCT4EQUIVALENTSOURCE_H_*/
 
