/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines the struct used for RTDBCVoltagelevel, and 
** must be same as that defined in rtdbblock 
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XCSGC3000_STRUCT4VOLTAGELEVEL_H_
#define _XCSGC3000_STRUCT4VOLTAGELEVEL_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef VOLTAGELEVEL_ID
#define VOLTAGELEVEL_ID 604
#endif

struct _xvoltagelevel_para {
	  int	m_ID;
	  char m_name[32];
	  int	m_substation_id;
	  int	m_substation_idx;
	  int	m_basevoltage_id;
	  int	m_basevoltage_idx;
	  float	m_nominalVoltage;
	  float	m_highVoltageLimit;
	  float	m_lowVoltageLimit;
	  int	m_nextVoltIdx;
	  int	m_firstNodeId;
	  int	m_firstNodeIdx;
	  int	m_firstBayId;
	  int	m_firstBayIdx;
	  short	m_authorityArea;
};

#ifdef __cplusplus
}
#endif

#endif/*_XCSGC3000_STRUCT4VOLTAGELEVEL_H_*/
 
