/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines Dispatcher Training Simulator (DTS) 
** object information and only used for internal.
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XPAS_ARRAY_H_
#define _XPAS_ARRAY_H_

#include "xpower-model.h"
#include "xpower-topology.h"

#ifdef __cplusplus
extern "C" {
#endif
   
typedef struct __xarray {
#define _XARRAY_MEMBER(m,t) int m##_used; int m##_capacity; t* m##_ptr;
    _XARRAY_MEMBER(company,   _xmodel_company)
    _XARRAY_MEMBER(substation,_xmodel_substation)
    _XARRAY_MEMBER(loadarea,  _xmodel_loadarea)
    _XARRAY_MEMBER(section,   _xmodel_section)
    _XARRAY_MEMBER(node,      _xmodel_node)
    _XARRAY_MEMBER(breaker,   _xmodel_breaker)
    _XARRAY_MEMBER(coupler,   _xmodel_coupler)
    _XARRAY_MEMBER(load,      _xmodel_load)
    _XARRAY_MEMBER(generator, _xmodel_generator)
    _XARRAY_MEMBER(swingbus,  _xmodel_swingbus)
    _XARRAY_MEMBER(voltage,   _xmodel_voltage)
    _XARRAY_MEMBER(branch,    _xmodel_branch)
    _XARRAY_MEMBER(tieline,   _xmodel_tieline)
    _XARRAY_MEMBER(vxformer,  _xmodel_vxformer)
    _XARRAY_MEMBER(svc,       _xmodel_svc)
    _XARRAY_MEMBER(shunt,     _xmodel_shunt)
    _XARRAY_MEMBER(residual,  _xmodel_residual)
    _XARRAY_MEMBER(capsegment,_xmodel_capsegment)
    _XARRAY_MEMBER(capacitor, _xmodel_capacitor)
    _XARRAY_MEMBER(pxformer,  _xmodel_pxformer)
    _XARRAY_MEMBER(interface, _xmodel_interface)
    _XARRAY_MEMBER(icomponent,_xmodel_icomponent)
    _XARRAY_MEMBER(tap,       _xmodel_tap)
    _XARRAY_MEMBER(motor,     _xmodel_motor)
    _XARRAY_MEMBER(case,      _xmodel_case)
    _XARRAY_MEMBER(tpbus,     _xtopology_bus)
    _XARRAY_MEMBER(tpgen,     _xtopology_gen)
    _XARRAY_MEMBER(tpbranch,  _xtopology_branch)
    _XARRAY_MEMBER(tpisland,  _xtopology_island)
    int used; int length; char bucket[_XFLEX_ARRAY];
} _xarray;

#define _xused_of(m)      (_xarray_ptr->m##_used)
#define _xpointer_of(m,i) ((i) >= 0 ? _xarray_ptr->m##_ptr + (i) : NULL)
#define _xcapacity_of(m)  (_xarray_ptr->m##_capacity)

extern _xarray *_xarray_ptr;
extern void _xdump_array(FILE *fp);
extern void _xarray_deallocate(void);
extern int _xarray_allocate(int);
extern int _xarray_slice_bucket(int,int);
extern int _xarray_insert_hash(int,int,int);

#ifdef __cplusplus
}
#endif

#endif/*_XPAS_ARRAY_H_*/
