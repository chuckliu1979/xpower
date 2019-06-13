/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines the struct used for RTDBCBusbarsection, and 
** must be same as that defined in rtdbblock 
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XCSGC3000_STRUCT4BUSBARSECTION_H_
#define _XCSGC3000_STRUCT4BUSBARSECTION_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BUSBARSECTION_ID
#define BUSBARSECTION_ID 610
#endif

struct _xbusbarsection_para {
    int m_ID;
    char m_name[32];
    char m_description[64];
    char m_phases[4];
    int m_terminal0_id;
    int m_terminal0_idx;
    int m_basevoltage_id;
    int m_basevoltage_idx;
    int m_circuitId;
    int m_circuitIdx;
    int m_equipmentcontainer_id;
    int m_equipmentcontainer_idx;
    int m_equipmentcontainer_tableid;
};

struct _xbusbarsection_real {
    int m_measId;
    int m_measIdx;
};

struct _xbusbarsection_real2 {
    int m_engzFlag;
    int m_islandFlag;
    int m_VViolFlag;
    int m_loopFlag;
    int m_feederFlag;
    int m_bustype;
};

struct _xbusbarsection_estm {
    float m_volt;
    float m_angle;
    float m_frequency;
};

struct _xbusbarsection_estm2 {
    int m_engzFlag;
    int m_islandFlag;
    int m_VViolFlag;
    int m_loopFlag;
    int m_feederFlag;
    int m_monitorFlag;
    int m_bustype;
};

struct _xbusbarsection_estm3 {
    float m_Va;
    float m_Anga;
    float m_Vb;
    float m_Angb;
    float m_Vc;
    float m_Angc;
};

struct _xbusbarsection_calc {
    float m_volt;
    float m_angle;
    float m_frequency;
};

struct _xbusbarsection_calc2 {
    int m_engzFlag;
    int m_islandFlag;
    int m_VViolFlag;
    int m_loopFlag;
    int m_feederFlag;
    int m_monitorFlag;
    int m_bustype;
};

struct _xbusbarsection_calc3 {
    float m_Va;
    float m_Anga;
    float m_Vb;
    float m_Angb;
    float m_Vc;
    float m_Angc;
};

struct _xbusbarsection_anly {
    float m_volt;
    float m_angle;
    float m_frequency;
};

struct _xbusbarsection_anly2 {
    int m_engzFlag;
    int m_islandFlag;
    int m_VViolFlag;
    int m_loopFlag;
    int m_feederFlag;
    int m_monitorFlag;
    int m_bustype;
};

struct _xbusbarsection_anly3 {
    float m_Va;
    float m_Anga;
    float m_Vb;
    float m_Angb;
    float m_Vc;
    float m_Angc;
};

#ifdef __cplusplus
}
#endif

#endif/*_XCSGC3000_STRUCT4BUSBARSECTION_H_*/
 
