/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines the struct used for RTDBCMeasurement, and 
** must be same as that defined in rtdbblock 
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XCSGC3000_STRUCT4MEASUREMENT_H_
#define _XCSGC3000_STRUCT4MEASUREMENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MEASUREMENT_ID
#define MEASUREMENT_ID 100
#endif

struct _xmeasurement_data {
    int m_ID;
    int m_measurementtype_id;
    int m_terminal_id;
    short m_sourceType;
    char m_sequenceNo;
    char m_useFlag;
    short m_powersystemresource_tableid;
    int m_powersystemresource_id;
    int m_objLinkNext;
    int m_pointID0;
    int m_controlID;
    short m_pointTableID0;
    short m_controlTableID;
    int m_limitsetgroup_id;
    int m_measTypeIdx;
    int m_powersystemresource_idx;
    int m_pointIdx0;
    int m_controlIdx;
    int m_measLinkNext;
    int m_limitGroupIdx;
    int m_measID;
    int m_pointID1;
    short m_pointTableID1;
    int m_pointIdx1;
    int m_pointID2;
    short m_pointTableID2;
    int m_pointIdx2;
    int m_srcUsed;
    int m_srcCurrent;
    int m_unit_id;
    int m_unit_idx;
};

struct _xmeasurement_estm {
    float m_Est_val;
    float m_Cur_val;
    float m_meas_wf;
    short m_dataQualityFlag;
    short m_calFlag;
    float m_limit;
    float m_checkStandard;
};

struct _xmeasurement_calc {
    float m_Est_val;
    float m_Cur_val;
    float m_meas_wf;
    short m_dataQualityFlag;
    short m_calFlag;
    float m_limit;
    float m_checkStandard;
};

struct _xmeasurement_anly {
    float m_Est_val;
    float m_Cur_val;
    float m_meas_wf;
    short m_dataQualityFlag;
    short m_calFlag;
    float m_limit;
    float m_checkStandard;
};

#ifdef __cplusplus
}
#endif

#endif/*_XCSGC3000_STRUCT4MEASUREMENT_H_*/
 
