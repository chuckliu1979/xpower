/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines the struct used for RTDBCAclinesegment, and 
** must be same as that defined in rtdbblock 
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XCSGC3000_STRUCT4ACLINESEGMENT_H_
#define _XCSGC3000_STRUCT4ACLINESEGMENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ACLINESEGMENT_ID
#define ACLINESEGMENT_ID 614
#endif

struct _xaclinesegment_para {
	  int	m_ID;
	  char m_name[32];
	  char m_description[64];
	  char m_phases[4];
	  int	m_terminal0_id;
	  int	m_terminal0_idx;
	  int	m_terminal1_id;
	  int	m_terminal1_idx;
	  float	m_length;
	  char	m_type[10];
	  float	m_bch;
	  float	m_b0ch;
	  float	m_gch;
	  float	m_g0ch;
	  float	m_r;
	  float	m_r0;
	  float	m_x;
	  float	m_x0;
	  int	m_basevoltage_id;
	  int	m_basevoltage_idx;
	  int	m_circuitId;
	  int	m_circuitIdx;
	  int	m_fromBay_id;
	  int	m_fromBay_idx;
	  int	m_toBay_id;
	  int	m_toBay_idx;
	  int	m_linkNextIdx;
	  int	m_equipmentcontainer_id;
	  int	m_equipmentcontainer_idx;
	  int	m_equipmentcontainer_tableid;
};

struct _xaclinesegment_real {
	  int	m_measId;
	  int	m_measIdx;
};

struct _xaclinesegment_real2 {
	  int	m_engzFlag;
	  int	m_islandFlag;
	  int	m_IViolFlag;
	  int	m_loopFlag;
	  int	m_feederFlag;
};

struct _xaclinesegment_estm {
	  float	m_frP;
	  float	m_frQ;
	  float	m_frI;
	  float	m_toP;
	  float	m_toQ;
	  float	m_toI;
};

struct _xaclinesegment_estm2 {
	  int	m_engzFlag;
	  int	m_islandFlag;
	  int	m_IViolFlag;
	  int	m_loopFlag;
	  int	m_feederFlag;
	  int	m_monitorFlag;
};

struct _xaclinesegment_estm3 {
	  float	m_frIa;
	  float	m_toIa;
	  float	m_frIb;
	  float	m_toIb;
	  float	m_frIc;
	  float	m_toIc;
	  float	m_frIn;
	  float	m_toIn;
	  float	m_frPa;
	  float	m_toPa;
	  float	m_frPb;
	  float	m_toPb;
	  float	m_frPc;
	  float	m_toPc;
	  float	m_frQa;
	  float	m_toQa;
	  float	m_frQb;
	  float	m_toQb;
	  float	m_frQc;
	  float	m_toQc;
};

struct _xaclinesegment_calc {
	  float	m_frP;
	  float	m_frQ;
	  float	m_frI;
	  float	m_toP;
	  float	m_toQ;
	  float	m_toI;
};

struct _xaclinesegment_calc2 {
	  int	m_engzFlag;
	  int	m_islandFlag;
	  int	m_IViolFlag;
	  int	m_loopFlag;
	  int	m_feederFlag;
	  int	m_monitorFlag;
};

struct _xaclinesegment_calc3 {
	  float	m_frIa;
	  float	m_toIa;
	  float	m_frIb;
	  float	m_toIb;
	  float	m_frIc;
	  float	m_toIc;
	  float	m_frIn;
	  float	m_toIn;
	  float	m_frPa;
	  float	m_toPa;
	  float	m_frPb;
	  float	m_toPb;
	  float	m_frPc;
	  float	m_toPc;
	  float	m_frQa;
	  float	m_toQa;
	  float	m_frQb;
	  float	m_toQb;
	  float	m_frQc;
	  float	m_toQc;
};

struct _xaclinesegment_anly {
	  float	m_frP;
	  float	m_frQ;
	  float	m_frI;
	  float	m_toP;
	  float	m_toQ;
	  float	m_toI;
};

struct _xaclinesegment_anly2 {
	  int	m_engzFlag;
	  int	m_islandFlag;
	  int	m_IViolFlag;
	  int	m_loopFlag;
	  int	m_feederFlag;
	  int	m_monitorFlag;
};

struct _xaclinesegment_anly3 {
	  float	m_frIa;
	  float	m_toIa;
	  float	m_frIb;
	  float	m_toIb;
	  float	m_frIc;
	  float	m_toIc;
	  float	m_frIn;
	  float	m_toIn;
	  float	m_frPa;
	  float	m_toPa;
	  float	m_frPb;
	  float	m_toPb;
	  float	m_frPc;
	  float	m_toPc;
	  float	m_frQa;
	  float	m_toQa;
	  float	m_frQb;
	  float	m_toQb;
	  float	m_frQc;
	  float	m_toQc;
};

#ifdef __cplusplus
}
#endif

#endif/*_XCSGC3000_STRUCT4ACLINESEGMENT_H_*/
 
