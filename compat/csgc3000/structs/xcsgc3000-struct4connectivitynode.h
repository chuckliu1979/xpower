/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines the struct used for RTDBCConnectivitynode, and 
** must be same as that defined in rtdbblock 
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XCSGC3000_STRUCT4CONNECTIVITYNODE_H_
#define _XCSGC3000_STRUCT4CONNECTIVITYNODE_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONNECTIVITYNODE_ID
#define CONNECTIVITYNODE_ID 605
#endif

struct _xconnectivitynode_para {
    int m_ID;
    char m_name[32];
    int m_equipmentcontainer_id;
    int m_equipmentcontainer_idx;
    short m_equipmentcontainer_tableid;
    int m_nextNodeIdx;
    int m_nodeType;
    int m_busType;
    int m_firstTerminalId;
    int m_firstTerminalIdx;
    int m_observabilityFlag;
};

struct _xconnectivitynode_real {
    int m_TpNodeId;
    int m_islandId;
    int m_islandIdx;
};

struct _xconnectivitynode_estm {
    int m_TpNodeId;
    int m_islandId;
    int m_islandIdx;
};

struct _xconnectivitynode_calc {
    int m_TpNodeId;
    int m_islandId;
    int m_islandIdx;
};

struct _xconnectivitynode_anly {
    int m_TpNodeId;
    int m_islandId;
    int m_islandIdx;
};

#ifdef __cplusplus
}
#endif

#endif/*_XCSGC3000_STRUCT4CONNECTIVITYNODE_H_*/
 
