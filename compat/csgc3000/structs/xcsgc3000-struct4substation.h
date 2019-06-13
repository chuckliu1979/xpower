/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines the struct used for RTDBCCompany, and 
** must be same as that defined in rtdbblock 
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XCSGC3000_STRUCT4SUBSTATION_H_
#define _XCSGC3000_STRUCT4SUBSTATION_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SUBSTATION_ID
#define SUBSTATION_ID 602
#endif

struct _xsubstation_para {
	  int	m_ID;
	  char m_name[32];
	  char m_description[64];
	  int	m_company_id;
	  int	m_company_idx;
	  int	m_loadarea_id;
	  int	m_loadarea_idx;
	  int	m_firstCircuitId;
	  int	m_firstCircuitIdx;
	  int	m_firstVoltId;
	  int	m_firstVoltIdx;
	  int	m_substationType;
	  int	m_subcontrolarea_id;
	  int	m_subcontrolarea_idx;
	  int	m_nextStationIdx;
	  int	m_firsttransfId;
	  int	m_firsttransfIdx;
	  int	m_firstBayId;
	  int	m_firstBayIdx;
	  int	m_firstAnaloginputId;
	  int	m_firstAnaloginputIdx;
	  int	m_firstStatusinputId;
	  int	m_firstStatusinputIdx;
	  int	m_firstAccumulatorinputId;
	  int	m_firstAccumulatorinputIdx;
};

struct _xsubstation_real {
	  float	m_loadP;
	  float	m_loadQ;
	  float	m_powerFactor;
	  float	m_genP;
	  float	m_genQ;
	  float	m_shuntQ;
};

struct _xsubstation_estm {
	  float	m_loadP;
	  float	m_loadQ;
	  float	m_powerFactor;
	  float	m_genP;
	  float	m_genQ;
	  float	m_shuntQ;
};

struct _xsubstation_calc {
	  float	m_loadP;
	  float	m_loadQ;
	  float	m_powerFactor;
	  float	m_genP;
	  float	m_genQ;
	  float	m_shuntQ;
};

struct _xsubstation_anly {
	  float	m_loadP;
	  float	m_loadQ;
	  float	m_powerFactor;
	  float	m_genP;
	  float	m_genQ;
	  float	m_shuntQ;
};

struct _xsubstation_flag {
	  int	m_alarmCount;
	  int	m_eventCount;
	  char m_alarmflag;
	  char m_eventflag;
	  char m_commStatus;
	  int	m_ied1stId;
	  int	m_ied1stIdx;
	  char m_controlInhibitFlag;
	  short	m_taggingFlags;
	  short	m_authorityArea;
};

#ifdef __cplusplus
}
#endif

#endif/*_XCSGC3000_STRUCT4SUBSTATION_H_*/

