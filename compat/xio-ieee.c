/*
** Copyright (C) 2011 XXX all rights reserved.
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#include "../xpower-io.h"
#include "../xpower-array.h"
#include "../xpower-topology.h"

static int _xieee_cdf_reader(const char *cdf){
    FILE *fd = fopen(cdf,"r");
}

static int __xieee_mode     = 0;
static int __xieee_database = 0;
static int __xieee_redundance [] = {
    0,  /*_XMODEL_UNKNOWN    0  */
    1,  /*_XMODEL_COMPANY    1  */
    2,  /*_XMODEL_SUBSTATION 2  */
    2,  /*_XMODEL_LOADAREA   3  */
    12, /*_XMODEL_SECTION    4  */
    12, /*_XMODEL_NODE       5  */
    2,  /*_XMODEL_BREAKER    6  */
    0,  /*_XMODEL_COUPLER    7  */
    2,  /*_XMODEL_LOAD       8  */
    0,  /*_XMODEL_GENERATOR  9  */
    0,  /*_XMODEL_SWINGBUS   10 */
    0,  /*_XMODEL_VOLTAGE    11 */
    0,  /*_XMODEL_BRANCH     12 */
    0,  /*_XMODEL_TIELINE    13 */
    0,  /*_XMODEL_VXFORMER   14 */
    0,  /*_XMODEL_SVC        15 */
    0,  /*_XMODEL_SHUNT      16 */
    0,  /*_XMODEL_RESIDUAL   17 */
    0,  /*_XMODEL_CAPSEGMENT 18 */
    0,  /*_XMODEL_CAPACITOR  19 */
    0,  /*_XMODEL_PXFORMER   20 */
    0,  /*_XMODEL_INTERFACE  21 */
    0,  /*_XMODEL_ICOMPONENT 22 */
    0,  /*_XMODEL_TAP        23 */
    0,  /*_XMODEL_MOTOR      24 */
    0   /*_XMODEL_CASE       25 */
};

int __xread_init(void){
    return _XFALSE;
}

int __xread_company(void){
    _xmodel_company* company;
    int capacity = 1 + __xieee_redundance[_XMODEL_COMPANY];
   
    if (_xarray_slice_bucket(_XMODEL_COMPANY,capacity) < capacity)
        return _XFALSE;
    
    _xused_of(company) = 0;
    _xmem_zero(_xpointer_of(company,0),_xcapacity_of(company)*sizeof(_xmodel_company));
    
    _xmodel_company* company = _xpointer_of(company, 0);
    company->id = ++(_xused_of(company));
    
    return _XTRUE;
}

int __xread_substation(void){
    return _XFALSE;
}

int __xread_loadarea(void){
    return _XFALSE;
}

int __xread_section(void){
    return _XFALSE;
}

int __xread_node(void){
    return _XFALSE;
}

int __xread_breaker(void){
    return _XFALSE;
}

int __xread_load(void){
    return _XFALSE;
}

int __xread_generator(void){
    return _XFALSE;
}

int __xread_voltage(void){
    return _XFALSE;
}

int __xread_branch(void){
    return _XFALSE;
}

int __xread_vxformer(void){
    return _XFALSE;
}

int __xread_svc(void){
    return _XFALSE;
}

int __xread_shunt(void){
    return _XFALSE;
}

