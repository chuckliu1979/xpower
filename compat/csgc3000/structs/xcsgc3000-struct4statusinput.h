/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines the struct used for RTDBCStatusInput, and 
** must be same as that defined in rtdbblock 
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XCSGC3000_STRUCT4STATUSINPUT_H_
#define _XCSGC3000_STRUCT4STATUSINPUT_H_

#include "xcsgc3000-struct4datetime.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef STATUSINPUT_ID
#define STATUSINPUT_ID 104
#endif

struct _xstatusinput_data {
    int m_ID;
    char m_name[32];
    int m_pointNum;
    int m_rtuID;
    short m_type;
    char m_value;
    char m_rawValue;
    int m_qualityCode;
    char m_controlInhibitFlag;
    char m_changeReason;
    short m_taggingFlags;
    short m_tripCount;
    short m_faultTripCount;
    struct _xdatetime m_timeStamp;
    char m_alarmProcMode;
    char m_alarmPriority;
    short m_alarmOptionFlags;
    short m_alarmDelayedTime;
    char m_TPInvolvedFlag;
    short m_tripCountLimit;
    short m_faultTripCountLimit;
    char m_reversePolarity;
    char m_statusType;
    short m_pointAddress;
    short m_groupAddress;
    short m_rtuAddress;
    short m_TwoBitTransitionTime;
    int m_TwoBitLinkedPointID;
    short m_directAlarmSuppressPtType;
    int m_directAlarmSuppressRTUID;
    int m_directAlarmSuppressPtNum;
    int m_directAlarmSuppressPtValue;
    short m_authorityArea;
    int m_voiceFileID;
    int m_voiceFileIdx;
    int m_popupDisplayFileID;
    int m_popupDisplayFileIdx;
    int m_statisticsID;
    int m_statisticsIdx;
    short m_savePeriodicity;
    short m_v2sGroupID;
    int m_measIdx;
    int m_alarmType;
    short m_linkedType;
    int m_substation_id;
    int m_substation_idx;
    char m_dataSource;
    char m_normalStatus;
    int m_RTUIdx;
    int m_nextIdxInRTU;
    int m_nextIdxInSubstation;
    int m_equipmentcontainer_id;
    short m_equipmentcontainer_tableid;
    int m_equipmentcontainer_idx;
};

struct _xstatusinput_anly {
    char m_value2;
    int m_qualityCode2;
};

#ifdef __cplusplus
}
#endif

#endif/*_XCSGC3000_STRUCT4STATUSINPUT_H_*/
 
