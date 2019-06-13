/*
** Copyright (C) 2011 XXX all rights reserved.
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XPOWER_IO_H_
#define _XPOWER_IO_H_

#include "xpower-config.h"

#ifdef __cplusplus
extern "C" {
#endif

/*frontend functions*/
_XEXPORT extern int _xread_init(void);
_XEXPORT extern int _xread_company(void);
_XEXPORT extern int _xread_substation(void);
_XEXPORT extern int _xread_loadarea(void);
_XEXPORT extern int _xread_section(void);
_XEXPORT extern int _xread_node(void);
_XEXPORT extern int _xread_breaker(void);
_XEXPORT extern int _xread_load(void);
_XEXPORT extern int _xread_generator(void);
_XEXPORT extern int _xread_voltage(void);
_XEXPORT extern int _xread_branch(void);
_XEXPORT extern int _xread_vxformer(void);
_XEXPORT extern int _xread_svc(void);
_XEXPORT extern int _xread_shunt(void);

/*backend functions*/
extern int __xread_init(void);
extern int __xread_company(void);
extern int __xread_substation(void);
extern int __xread_loadarea(void);
extern int __xread_section(void);
extern int __xread_node(void);
extern int __xread_breaker(void);
extern int __xread_load(void);
extern int __xread_generator(void);
extern int __xread_voltage(void);
extern int __xread_branch(void);
extern int __xread_vxformer(void);
extern int __xread_svc(void);
extern int __xread_shunt(void);

#ifdef __cplusplus
}
#endif

#endif/*_XPOWER_IO_H_*/

