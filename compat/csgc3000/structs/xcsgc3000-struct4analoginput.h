/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines the struct used for RTDBCAnalogInput, and 
** must be same as that defined in rtdbblock 
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XCSGC3000_STRUCT4ANALOGINPUT_H_
#define _XCSGC3000_STRUCT4ANALOGINPUT_H_

#include "xcsgc3000-struct4datetime.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ANALOGINPUT_ID
#define ANALOGINPUT_ID 101
#endif

struct _xanaloginput_data {
    int m_ID;
    char m_name[32];
    short m_type;
    int m_pointNum;
    int m_rtuID;
    float m_value;
    int m_qualityCode;
    struct _xdatetime m_timeStamp;
    float m_rateOfChange;
    float m_rawValue;
    short m_violationFlags;
    short m_taggingFlags;
    char m_controlInhibitFlag;
    char m_alarmProcMode;
    char m_alarmPriority;
    char m_TPInvolvedFlag;
    short m_alarmOptionFlags;
    short m_authorityArea;
    int m_limitSetID;
    int m_limitSetIdx;
    float m_reportDeadband;
    float m_sensorMaximum;
    float m_sensorMinimum;
    float m_zeroClampping;
    float m_scaleFactor;
    float m_scaleOffset;
    float m_smoothingFactor;
    short m_pointAddress;
    short m_groupAddress;
    short m_rtuAddress;
    short m_alarmDelayedTime;
    int m_voiceFileID;
    int m_voiceFileIdx;
    int m_popupDisplayFileID;
    int m_popupDisplayFileIdx;
    short m_directAlarmSuppressPtType;
    int m_directAlarmSuppressRTUID;
    int m_directAlarmSuppressPtNum;
    int m_directAlarmSuppressPtValue;
    int m_statisticsID;
    int m_statisticsIdx;
    short m_savePeriodicity;
    int m_measIdx;
    int m_alarmType;
    int m_linkedPointID;
    short m_linkedType;
    float m_maximum;
    struct _xdatetime m_maxTime;
    float m_minimum;
    struct _xdatetime m_minTime;
    float m_operationalLow;
    float m_operationalHigh;
    float m_warningLow;
    float m_warningHigh;
    float m_emergencyLow;
    float m_emergencyHigh;
    float m_rateOfChangeLimit;
    char m_isPercentageLimits;
    float m_nominal;
    float m_deadband;
    int m_substation_id;
    int m_substation_idx;
    char m_dataSource;
    int m_RTUIdx;
    int m_nextIdxInRTU;
    int m_nextIdxInSubstation;
    int m_equipmentcontainer_id;
    short m_equipmentcontainer_tableid;
    int m_equipmentcontainer_idx;
};

struct _xanaloginput_anly {
    float m_value2;
    int m_qualityCode2;
};

#ifdef __cplusplus
}
#endif

#endif/*_XCSGC3000_STRUCT4ANALOGINPUT_H_*/
 
