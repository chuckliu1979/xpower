/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines the struct used for RTDBCTransformerwinding, and 
** must be same as that defined in rtdbblock 
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XCSGC3000_STRUCT4TRANSFORMERWINDING_H_
#define _XCSGC3000_STRUCT4TRANSFORMERWINDING_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TRANSFORMERWINDING_ID
#define TRANSFORMERWINDING_ID 615
#endif

struct _xtransformerwinding_para {
    int m_ID;
    char m_Name[32];
    char m_description[64];
    char m_phases[4];
    int m_terminal0_id;
    int m_terminal0_idx;
    int m_powertransformer_id;
    int m_powertransformer_idx;
    int m_Tapchanger_id;
    int m_Tapchanger_idx;
    float m_ratedKV;
    int m_connectionType;
    float m_ratedMVA;
    float m_shortTermMVA;
    int m_windingType;
    char m_grounded;
    float m_rGround;
    float m_xGround;
    float m_g;
    float m_b;
    float m_r;
    float m_r0;
    float m_x;
    float m_x0;
    int m_basevoltage_id;
    int m_basevoltage_idx;
    int m_circuitId;
    int m_circuitIdx;
};

struct _xtransformerwinding_real {
    int m_measId;
    int m_measIdx;
};

struct _xtransformerwinding_real2 {
    int m_engzFlag;
    int m_islandFlag;
    int m_IViolFlag;
    int m_loopFlag;
    int m_feederFlag;
};

struct _xtransformerwinding_estm {
    float m_P;
    float m_Q;
    float m_I;
};

struct _xtransformerwinding_estm2 {
    int m_engzFlag;
    int m_islandFlag;
    int m_IViolFlag;
    int m_loopFlag;
    int m_feederFlag;
    int m_monitorFlag;
};

struct _xtransformerwinding_estm3 {
    float m_Pa;
    float m_Qa;
    float m_Ia;
    float m_Pb;
    float m_Qb;
    float m_Ib;
    float m_Pc;
    float m_Qc;
    float m_Ic;
};

struct _xtransformerwinding_calc {
    float m_P;
    float m_Q;
    float m_I;
};

struct _xtransformerwinding_calc2 {
    int m_engzFlag;
    int m_islandFlag;
    int m_IViolFlag;
    int m_loopFlag;
    int m_feederFlag;
    int m_monitorFlag;
};

struct _xtransformerwinding_calc3 {
    float m_Pa;
    float m_Qa;
    float m_Ia;
    float m_Pb;
    float m_Qb;
    float m_Ib;
    float m_Pc;
    float m_Qc;
    float m_Ic;
};

struct _xtransformerwinding_anly {
    float m_P;
    float m_Q;
    float m_I;
};

struct _xtransformerwinding_anly2 {
    int m_engzFlag;
    int m_islandFlag;
    int m_IViolFlag;
    int m_loopFlag;
    int m_feederFlag;
    int m_monitorFlag;
};

struct _xtransformerwinding_anly3 {
    float m_Pa;
    float m_Qa;
    float m_Ia;
    float m_Pb;
    float m_Qb;
    float m_Ib;
    float m_Pc;
    float m_Qc;
    float m_Ic;
};

#ifdef __cplusplus
}
#endif

#endif/*_XCSGC3000_STRUCT4TRANSFORMERWINDING_H_*/
 
