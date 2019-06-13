/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines the struct used for SDateTime, and 
** must be same as that defined in osi 
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XCSGC3000_STRUCT4DATETIME_H_
#define _XCSGC3000_STRUCT4DATETIME_H_

#ifdef __cplusplus
extern "C" {
#endif

struct _xdatetime {
    unsigned int m_seconds;
    unsigned int m_microseconds;
};

#ifdef __cplusplus
}
#endif

#endif/*_XCSGC3000_STRUCT4DATETIME_H_*/
 
