/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines the struct used for RTDBCBay, and 
** must be same as that defined in rtdbblock 
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XCSGC3000_STRUCT4BAY_H_
#define _XCSGC3000_STRUCT4BAY_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BAY_ID
#define BAY_ID 607
#endif

struct _xbay_para {
    int m_ID;
    char m_name[32];
    int m_substation_id;
    int m_substation_idx;
    int m_voltagelevel_id;
    int m_voltagelevel_idx;
    short m_bayEnergyMeasFlag;
    short m_bayPowerMeasFlag;
    short m_BreakerConfiguration;
    short m_BusBarConfiguration;
    int m_firstNodeId;
    int m_firstNodeIdx;
    int m_nextBayIdx;
    char m_bayType;
    short m_taggingFlags;
    int m_alarmCount;
    int m_eventCount;
    char m_alarmFlag;
    char m_eventFlag;
    char m_controlInhibitFlag;
    short m_authorityArea;
    int m_nextIdxInSubstation;
};

#ifdef __cplusplus
}
#endif

#endif/*_XCSGC3000_STRUCT4BAY_H_*/
 
