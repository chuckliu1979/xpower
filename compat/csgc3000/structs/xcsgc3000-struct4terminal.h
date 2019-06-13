/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines the struct used for RTDBCTerminal, and 
** must be same as that defined in rtdbblock 
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XCSGC3000_STRUCT4TERMINAL_H_
#define _XCSGC3000_STRUCT4TERMINAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TERMINAL_ID
#define TERMINAL_ID 606
#endif

struct _xterminal_para {
    int m_ID;
    char m_name[32];
    char m_description[64];
    int m_connectivitynode_id;
    int m_connectivitynode_idx;
    short m_conductingequipment_tableid;
    int m_conductingequipment_id;
    int m_conductingequipment_idx;
    int m_nextTerminalIdx;
};

#ifdef __cplusplus
}
#endif

#endif/*_XCSGC3000_STRUCT4TERMINAL_H_*/
 
