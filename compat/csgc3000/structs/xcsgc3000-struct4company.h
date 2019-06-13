/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines the struct used for RTDBCCompany, and 
** must be same as that defined in rtdbblock 
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XCSGC3000_STRUCT4COMPANY_H_
#define _XCSGC3000_STRUCT4COMPANY_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef COMPANY_ID
#define COMPANY_ID 600
#endif

struct _xcompany_para {
    int m_ID;
    char m_name[32];
    char m_description[64];
    short m_companyType;
};

struct _xcompany_real {
    float m_loadP;
    float m_loadQ;
    float m_powerFactor;
    float m_genP;
    float m_genQ;
    float m_shuntQ;
    float m_lossP;
    float m_lossQ;
    float m_ichSchedP;
    float m_netExchangP;
    float m_netExchangQ;
    float m_penalty;
};

struct _xcompany_estm {
    float m_loadP;
    float m_loadQ;
    float m_powerFactor;
    float m_genP;
    float m_genQ;
    float m_shuntQ;
    float m_lossP;
    float m_lossQ;
    float m_ichSchedP;
    float m_netExchangP;
    float m_netExchangQ;
    float m_penalty;
};

struct _xcompany_calc {
    float m_loadP;
    float m_loadQ;
    float m_powerFactor;
    float m_genP;
    float m_genQ;
    float m_shuntQ;
    float m_lossP;
    float m_lossQ;
    float m_ichschedP;
    float m_netExchangP;
    float m_netExchangQ;
    float m_penalty;
};

struct _xcompany_anly {
    float m_loadP;
    float m_loadQ;
    float m_powerFactor;
    float m_genP;
    float m_genQ;
    float m_shuntQ;
    float m_lossP;
    float m_lossQ;
    float m_ichSchedP;
    float m_netExchangP;
    float m_netExchangQ;
    float m_penalty;
};

#ifdef __cplusplus
}
#endif

#endif/*_XCSGC3000_STRUCT4COMPANY_H_*/
 
