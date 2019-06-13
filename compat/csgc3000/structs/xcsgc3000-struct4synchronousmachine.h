/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines the struct used for RTDBCSynchronousMachine, and 
** must be same as that defined in rtdbblock 
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XCSGC3000_STRUCT4SYNCHRONOUSMACHINE_H_
#define _XCSGC3000_STRUCT4SYNCHRONOUSMACHINE_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SYNCHRONOUSMACHINE_ID
#define SYNCHRONOUSMACHINE_ID 612
#endif

struct _xsynchronousmachine_para {
    int m_ID;
    char m_name[32];
    char m_description[64];
    int m_company_id;
    char m_phases[4];
    int m_terminal0_id;
    int m_terminal0_idx;
    float m_baseMVar;
    float m_damping;
    float m_inertia;
    float m_r;
    float m_r0;
    float m_x;
    float m_x0;
    float m_xDirectSubtrans;
    float m_xDirectSync;
    float m_xDirectTrans;
    float m_xQuadSubtrans;
    float m_xQuadSync;
    float m_xQuadTrans;
    short m_operatingMode;
    short m_type;
    float m_condenserMW;
    int m_MVarCapabilityCurves_id;
    int m_MVarCapabilityCurves_idx;
    float m_penalty;
    float m_mw_Alloc;
    int m_plimitsetgroup_id;
    int m_plimitsetgroup_idx;
    float m_minimumMVAr;
    float m_maximumMVAr;
    float m_minimumkV;
    float m_maximumkV;
    int m_basevoltage_id;
    int m_basevoltage_idx;
    int m_circuitId;
    int m_circuitIdx;
    int m_subcontrolarea_id;
    int m_subcontrolarea_idx;
    int m_nextGenIdx;
    int m_swingRank;
    float m_defaultP;
    float m_defaultQ;
    short m_voltRegulate;
    float m_ratedMVA;
    int m_primeMover_id;
    int m_primeMover_idx;
    int m_regulationschedule_id;
    int m_regulationschedule_idx;
    int m_qlimitsetgroup_id;
    int m_qlimitsetgroup_idx;
};

struct _xsynchronousmachine_real {
    int m_measId;
    int m_measIdx;
};

struct _xsynchronousmachine_real2 {
    int m_islandFlag;
    int m_loopFlag;
    int m_feederFlag;
};

struct _xsynchronousmachine_estm {
    float m_P;
    float m_Q;
};

struct _xsynchronousmachine_estm2 {
    int m_islandFlag;
    int m_loopFlag;
    int m_feederFlag;
};

struct _xsynchronousmachine_estm3 {
    float m_Pa;
    float m_Qa;
    float m_Pb;
    float m_Qb;
    float m_Pc;
    float m_Qc;
};

struct _xsynchronousmachine_calc {
    float m_P;
    float m_Q;
};

struct _xsynchronousmachine_calc2 {
    int m_islandFlag;
    int m_loopFlag;
    int m_feederFlag;
};

struct _xsynchronousmachine_calc3 {
    float m_Pa;
    float m_Qa;
    float m_Pb;
    float m_Qb;
    float m_Pc;
    float m_Qc;
};

struct _xsynchronousmachine_anly {
    float m_P;
    float m_Q;
};

struct _xsynchronousmachine_anly2 {
    int m_islandFlag;
    int m_loopFlag;
    int m_feederFlag;
};

struct _xsynchronousmachine_anly3 {
    float m_Pa;
    float m_Qa;
    float m_Pb;
    float m_Qb;
    float m_Pc;
    float m_Qc;
};

#ifdef __cplusplus
}
#endif

#endif/*_XCSGC3000_STRUCT4SYNCHRONOUSMACHINE_H_*/
 
