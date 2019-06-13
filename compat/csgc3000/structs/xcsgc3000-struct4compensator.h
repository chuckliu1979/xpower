/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines the struct used for RTDBCCompensator, and 
** must be same as that defined in rtdbblock 
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XCSGC3000_STRUCT4COMPENSATOR_H_
#define _XCSGC3000_STRUCT4COMPENSATOR_H_

#include "xcsgc3000-struct4datetime.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef COMPENSATOR_ID
#define COMPENSATOR_ID 618
#endif

struct _xcompensator_para {
    int m_ID;
    char m_name[32];
    char m_description[64];
    char m_phases[4];
    int m_terminal0_id;
    int m_terminal0_idx;
    int m_terminal1_id;
    int m_terminal1_idx;
    float m_AVRDelay;
    int m_maximumSections;
    float m_MVArPerSection;
    float m_nominalMVAr;
    int m_normalSections;
    float m_impedance;
    float m_r;
    float m_x;
    struct _xdatetime m_switchOnDate;
    int m_switchOnCount;
    float m_voltSensitivity;
    float m_yPerSection;
    short m_compensatorType;
    float m_minimumKV;
    float m_maximumKV;
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

struct _xcompensator_real {
    int m_measId;
    int m_measIdx;
};

struct _xcompensator_real2 {
    int m_engzFlag;
    int m_islandFlag;
    int m_VViolFlag;
    int m_loopFlag;
    int m_feederFlag;
};

struct _xcompensator_estm {
    float m_Q;
};

struct _xcompensator_estm2 {
    int m_engzFlag;
    int m_islandFlag;
    int m_VViolFlag;
    int m_loopFlag;
    int m_feederFlag;
};

struct _xcompensator_estm3 {
    float m_Qa;
    float m_Qb;
    float m_Qc;
};

struct _xcompensator_calc {
    float m_Q;
};

struct _xcompensator_calc2 {
    int m_engzFlag;
    int m_islandFlag;
    int m_VViolFlag;
    int m_loopFlag;
    int m_feederFlag;
};

struct _xcompensator_calc3 {
    float m_Qa;
    float m_Qb;
    float m_Qc;
};

struct _xcompensator_anly {
    float m_Q;
};

struct _xcompensator_anly2 {
    int m_engzFlag;
    int m_islandFlag;
    int m_VViolFlag;
    int m_loopFlag;
    int m_feederFlag;
};

struct _xcompensator_anly3 {
    float m_Qa;
    float m_Qb;
    float m_Qc;
};

#ifdef __cplusplus
}
#endif

#endif/*_XCSGC3000_STRUCT4COMPENSATOR_H_*/
 
