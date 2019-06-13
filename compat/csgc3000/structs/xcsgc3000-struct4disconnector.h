/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines the struct used for RTDBCDisconnector, and 
** must be same as that defined in rtdbblock 
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XCSGC3000_STRUCT4DISCONNECTOR_H_
#define _XCSGC3000_STRUCT4DISCONNECTOR_H_

#include "xcsgc3000-struct4datetime.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DISCONNECTOR_ID
#define DISCONNECTOR_ID 623
#endif

struct _xdisconnector_para {
    int m_ID;
    char m_name[32];
    char m_description[64];
    char m_phases[4];
    int m_terminal0_id;
    int m_terminal0_idx;
    int m_terminal1_id;
    int m_terminal1_idx;
    char m_normalOpen;
    struct _xdatetime m_switchOnDate;
    float m_ampInterrupt;
    short m_PhysicalType;
    float m_ampRating;
    float m_inTransitTime;
    int m_basevoltage_id;
    int m_basevoltage_idx;
    int m_circuitId;
    int m_circuitIdx;
    int m_equipmentcontainer_id;
    int m_equipmentcontainer_idx;
    short m_APPType;
    int m_equipmentcontainer_tableid;
};

struct _xdisconnector_real {
    int m_measId;
    int m_measIdx;
};

struct _xdisconnector_real2 {
    int m_engzFlag;
    int m_islandFlag;
    int m_loopFlag;
    int m_feederFlag;
};

struct _xdisconnector_estm {
    char m_status;
    int m_switchOnCount;
    float m_P;
    float m_Q;
    float m_I;
};

struct _xdisconnector_estm2 {
    int m_engzFlag;
    int m_islandFlag;
    int m_loopFlag;
    int m_feederFlag;
};

struct _xdisconnector_estm3 {
    float m_Ia;
    float m_Ib;
    float m_Ic;
};

struct _xdisconnector_calc {
    char m_status;
    int m_switchOnCount;
    float m_P;
    float m_Q;
    float m_I;
};

struct _xdisconnector_calc2 {
    int m_engzFlag;
    int m_islandFlag;
    int m_loopFlag;
    int m_feederFlag;
};

struct _xdisconnector_calc3 {
    float m_Ia;
    float m_Ib;
    float m_Ic;
};

struct _xdisconnector_anly {
    char m_status;
    int m_switchOnCount;
    float m_P;
    float m_Q;
    float m_I;
};

struct _xdisconnector_anly2 {
    int m_engzFlag;
    int m_islandFlag;
    int m_loopFlag;
    int m_feederFlag;
};

struct _xdisconnector_anly3 {
    float m_Ia;
    float m_Ib;
    float m_Ic;
};

#ifdef __cplusplus
}
#endif

#endif/*_XCSGC3000_STRUCT4DISCONNECTOR_H_*/
 
