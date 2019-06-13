/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines the struct used for RTDBCLoadArea, and 
** must be same as that defined in rtdbblock 
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XCSGC3000_STRUCT4LOADAREA_H_
#define _XCSGC3000_STRUCT4LOADAREA_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LOADAREA_ID
#define LOADAREA_ID 601
#endif

struct _xloadarea_para {
    int m_ID;
    char m_name[32];
    char m_description[64];
    int m_company_id;
    int m_company_idx;
    int m_arealosscurve_id;
    int m_arealosscurve_idx;
    int m_arealoadcurve_id;
    int m_arealoadcurve_idx;
    int m_plimitsetgroup_id;
    int m_plimitsetgroup_idx;
    int m_qlimitsetgroup_id;
    int m_qlimitsetgroup_idx;
};

struct _xloadarea_real {
    int m_measId;
    int m_measIdx;
};

struct _xloadarea_estm {
    float m_loadP;
    float m_loadQ;
};

struct _xloadarea_calc {
    float m_loadP;
    float m_loadQ;
};

struct _xloadarea_anly {
    float m_loadP;
    float m_loadQ;
};

#ifdef __cplusplus
}
#endif

#endif/*_XCSGC3000_STRUCT4LOADAREA_H_*/
 
