/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines the struct used for RTDBCEnergyconsumer, and 
** must be same as that defined in rtdbblock 
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XCSGC3000_STRUCT4ENERGYCONSUMER_H_
#define _XCSGC3000_STRUCT4ENERGYCONSUMER_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ENERGYCONSUMER_ID
#define ENERGYCONSUMER_ID 613
#endif

struct _xenergyconsumer_para {
    int m_ID;
    char m_name[32];
    char m_description[64];
    int m_company_id;
    int m_company_idx;
    char  m_phases[4];
    int m_terminal0_id;
    int m_terminal0_idx;
    char m_conformingLoadFlag;
    int m_customerCount;
    float m_pFexp;
    float m_pFixed;
    float m_pFixedPct;
    float m_pNom;
    float m_pNomPct;
    float m_powerFactor;
    float m_pVexp;
    float m_qFexp;
    float m_qFixed;
    float m_qFixedPct;
    float m_qNom;
    float m_qNomPct;
    float m_qVexp;
    int m_loadDemandModels;
    int m_loadArea_id;
    int m_loadArea_idx;
    int m_plimitsetgroup_id;
    int m_plimitsetgroup_idx;
    int m_qlimitsetgroup_id;
    int m_qlimitsetgroup_idx;
    int m_basevoltage_id;
    int m_basevoltage_idx;
    int m_circuitId;
    int m_circuitIdx;
};

struct _xenergyconsumer_real {
    int m_measId;
    int m_measIdx;
};

struct _xenergyconsumer_real2 {
    int m_islandFlag;
    int m_loopFlag;
    int m_feederFlag;
    int m_engzFlag;
};

struct _xenergyconsumer_estm {
    float m_P;
    float m_Q;
};

struct _xenergyconsumer_estm2 {
    int m_islandFlag;
    int m_loopFlag;
    int m_feederFlag;
    int m_engzFlag;
};

struct _xenergyconsumer_estm3 {
    float m_Pa;
    float m_Qa;
    float m_Pb;
    float m_Qb;
    float m_Pc;
    float m_Qc;
};

struct _xenergyconsumer_calc {
    float m_P;
    float m_Q;
};

struct _xenergyconsumer_calc2 {
    int m_islandFlag;
    int m_loopFlag;
    int m_feederFlag;
    int m_engzFlag;
};

struct _xenergyconsumer_calc3 {
    float m_Pa;
    float m_Qa;
    float m_Pb;
    float m_Qb;
    float m_Pc;
    float m_Qc;
};

struct _xenergyconsumer_anly {
    float m_P;
    float m_Q;
};

struct _xenergyconsumer_anly2 {
    int m_islandFlag;
    int m_loopFlag;
    int m_feederFlag;
    int m_engzFlag;
};

struct _xenergyconsumer_anly3 {
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

#endif/*_XCSGC3000_STRUCT4ENERGYCONSUMER_H_*/
 
