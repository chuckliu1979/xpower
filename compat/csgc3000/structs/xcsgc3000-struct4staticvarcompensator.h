/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines the struct used for RTDBCStaticVarCompensator, and 
** must be same as that defined in rtdbblock 
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XCSGC3000_STRUCT4STATICVARCOMPENSATOR_H_
#define _XCSGC3000_STRUCT4STATICVARCOMPENSATOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef STATICVARCOMPENSATOR_ID
#define STATICVARCOMPENSATOR_ID 617
#endif

struct _xstaticvarcompensator_para {
    int m_ID;
    char m_name[32];
    char m_description[64];
    char m_phases[4];
    int m_terminal0_id;
    int m_terminal0_idx;
    float m_capacitiveRating;
    float m_inductiveRating;
    short m_SVCControlMode;
    float m_slope;
    int m_qlimitsetgroup_id;
    int m_qlimitsetgroup_idx;
    int m_basevoltage_id;
    int m_basevoltage_idx;
    int m_circuitId;
    int m_circuitIdx;
    int m_regulationschedule_id;
    int m_regulationschedule_idx;
    int m_equipmentcontainer_id;
    int m_equipmentcontainer_idx;
    int m_equipmentcontainer_tableid;
};

struct _xstaticvarcompensator_real {
    int m_measId;
    int m_measIdx;
};

struct _xstaticvarcompensator_real2 {
    int m_engzFlag;
    int m_islandFlag;
    int m_VViolFlag;
    int m_loopFlag;
    int m_feederFlag;
};

struct _xstaticvarcompensator_estm {
    float m_Q;
};

struct _xstaticvarcompensator_estm2 {
    int m_engzFlag;
    int m_islandFlag;
    int m_VViolFlag;
    int m_loopFlag;
    int m_feederFlag;
};

struct _xstaticvarcompensator_estm3 {
    float m_Qa;
    float m_Qb;
    float m_Qc;
};

struct _xstaticvarcompensator_calc {
    float m_Q;
};

struct _xstaticvarcompensator_calc2 {
    int m_engzFlag;
    int m_islandFlag;
    int m_VViolFlag;
    int m_loopFlag;
    int m_feederFlag;
};

struct _xstaticvarcompensator_calc3 {
    float m_Qa;
    float m_Qb;
    float m_Qc;
};

struct _xstaticvarcompensator_anly {
    float m_Q;
};

struct _xstaticvarcompensator_anly2 {
    int m_engzFlag;
    int m_islandFlag;
    int m_VViolFlag;
    int m_loopFlag;
    int m_feederFlag;
};

struct _xstaticvarcompensator_anly3 {
    float m_Qa;
    float m_Qb;
    float m_Qc;
};

#ifdef __cplusplus
}
#endif

#endif/*_XCSGC3000_STRUCT4STATICVARCOMPENSATOR_H_*/
 
