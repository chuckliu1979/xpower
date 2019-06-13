/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines the struct used for RTDBCTapchanger, and 
** must be same as that defined in rtdbblock 
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XCSGC3000_STRUCT4TAPCHANGER_H_
#define _XCSGC3000_STRUCT4TAPCHANGER_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TAPCHANGER_ID
#define TAPCHANGER_ID 616
#endif

struct _xtapchanger_para {
    int m_ID;
    char m_name[32];
    char m_description[64];
    short m_highStep;
    short m_lowStep;
    float m_neutralKV;
    short m_neutralStep;
    short m_normalStep;
    float m_stepPhaseshiftIncrement;
    float m_stepVoltageIncrement;
    float m_initialDelay;
    float m_subsequentDelay;
    int m_transformerwinding_id;
    int m_transformerwinding_idx;
};

struct _xtapchanger_real {
    int m_measId;
    int m_measIdx;
};

struct _xtapchanger_estm {
    float m_curStep;
};

struct _xtapchanger_calc {
    float m_curStep;
};

struct _xtapchanger_anly {
    float m_curStep;
};

#ifdef __cplusplus
}
#endif

#endif/*_XCSGC3000_STRUCT4TAPCHANGER_H_*/
 
