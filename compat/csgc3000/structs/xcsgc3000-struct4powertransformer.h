/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines the struct used for RTDBCPowertransformer, and 
** must be same as that defined in rtdbblock 
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XCSGC3000_STRUCT4POWERTRANSFORMER_H_
#define _XCSGC3000_STRUCT4POWERTRANSFORMER_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef POWERTRANSFORMER_ID
#define POWERTRANSFORMER_ID 609
#endif

struct _xpowertransformer_para {
	  int	m_ID;
	  char m_name[32];
	  char m_description[64];
	  int	m_equipmentcontainer_id;
	  int	m_equipmentcontainer_idx;
	  char m_phases[4];
	  short	m_transfcoolingType;
	  short	m_transformerType;
	  int	m_transformerwindingprimary_id;
	  int	m_transformerwindingprimary_idx;
	  int	m_transformerwindingtertiary_id;
	  int	m_transformerwindingtertiary_idx;
	  int	m_transformerwindingsecondary_id;
	  int	m_transformerwindingsecondary_idx;
	  int	m_nextTransfIdx;
	  short	m_equipmentcontainer_tableid;
};

#ifdef __cplusplus
}
#endif

#endif/*_XCSGC3000_STRUCT4POWERTRANSFORMER_H_*/
 
