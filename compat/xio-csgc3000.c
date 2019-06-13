/*
** Copyright (C) 2011 XXX all rights reserved.
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#include <math.h>
#include <assert.h>
#include "../xpower-io.h"
#include "../xpower-def.h"
#include "../xpower-hash.h"
#include "../xpower-array.h"
#include "../xpower-topology.h"

/*
 * first time for struct header only once
 */
#include "csgc3000/xcsgc3000.h"
/*
 * second time for function pointer variables
 */
#define _XTRICKY_VAR
#include "csgc3000/xcsgc3000.h"

/*
 * library handle and outside _XTRICKY
 */
#if defined(WIN32)
#include <windows.h>
static HINSTANCE handle;
#define _XCOMPAT_SONAME "xio-3k.dll"
#define _XTRICKY(rtype,name,ptype) name = (rtype(*)(ptype))GetProcAddress(handle,#name);if (!name) return _XFALSE;
#else
static void *handle;
#define _XCOMPAT_SONAME "../lib/libxio-3k.so"
#define _XTRICKY(rtype,name,ptype) name = (rtype(*)(ptype))dlsym(handle,#name);if (dlerror() || !name) return _XFALSE;
#endif

static int _xload_symbol(void){
#if defined(WIN32)
    handle = LoadLibrary(_XCOMPAT_SONAME);
#else
    handle = dlopen(_XCOMPAT_SONAME, RTLD_NOW);
    dlerror();
#endif
    if (!handle)
        return _XFALSE;
/*
 * third time for outside used
 */
#define _XTRICKY_OUT
#include "csgc3000/xcsgc3000.h"

    return _XTRUE;
}

static int __xcsgc3000_mode     = 0;
static int __xcsgc3000_database = 0;
static int __xcsgc3000_redundance [] = {
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

#define _XFACTOR 1.2
#define _XMAX_TAP_COUNT 18 /*max taps per vxformer/pxformer*/

int __xread_init(void){
    int cur,len = 0;
    if (!_xload_symbol())
        return _XFALSE;
    
    _xcsgc3000_database(&__xcsgc3000_database);
#define _XCHECK(X) if (X##_ok() == _XFALSE) return _XFALSE;
    
    _XCHECK(_xcompany)
    cur = _xcompany_size();/*company slice*/
    cur += __xcsgc3000_redundance[_XMODEL_COMPANY];
    len += cur * sizeof(_xmodel_company);
    
    _XCHECK(_xloadarea)
    cur = _xloadarea_size();/*loadarea slice*/
    cur += __xcsgc3000_redundance[_XMODEL_LOADAREA];
    len += cur * sizeof(_xmodel_loadarea);
    
    _XCHECK(_xsubstation)
    cur = _xsubstation_size();/*substation slice*/
    cur += __xcsgc3000_redundance[_XMODEL_SUBSTATION];
    len += cur * sizeof(_xmodel_substation);

    _XCHECK(_xvoltagelevel)
    cur = (int)(_xvoltagelevel_size() * _XFACTOR + 
                _xpowertransformer_size());/*section slice*/
    cur += __xcsgc3000_redundance[_XMODEL_SECTION];
    len += cur * sizeof(_xmodel_section);

    _XCHECK(_xconnectivitynode)
    cur = (int)(_xconnectivitynode_size() * _XFACTOR + 
                _xpowertransformer_size());/*node slice*/
    cur += __xcsgc3000_redundance[_XMODEL_SECTION];
    len += cur * (sizeof(_xmodel_node) + sizeof(_xtopology_bus));
    
    _XCHECK(_xbreaker)
    _XCHECK(_xdisconnector)
    _XCHECK(_xgrounddisconnector)
    cur = _xbreaker_size() + __xcsgc3000_redundance[_XMODEL_BREAKER];
    len += cur * (sizeof(_xmodel_breaker) + sizeof(_xmodel_coupler));
    len += (_xdisconnector_size() + _xgrounddisconnector_size()) *sizeof(_xmodel_breaker);
 
    _XCHECK(_xenergyconsumer)
    cur = _xenergyconsumer_size() + __xcsgc3000_redundance[_XMODEL_LOAD];
    len += cur * sizeof(_xmodel_load);

    _XCHECK(_xequivalentsource)
    _XCHECK(_xsynchronousmachine)
    cur = _xsynchronousmachine_size() + _xequivalentsource_size();
    cur += __xcsgc3000_redundance[_XMODEL_GENERATOR];
    len += cur * (sizeof(_xmodel_generator) + sizeof(_xmodel_swingbus));

    _XCHECK(_xbusbarsection)    
    cur = _xbusbarsection_size() + __xcsgc3000_redundance[_XMODEL_VOLTAGE];
    len += cur * sizeof(_xmodel_voltage);

    _XCHECK(_xcompensator)
    _XCHECK(_xaclinesegment)
    _XCHECK(_xtransformerwinding)
    cur = (int)(_xaclinesegment_size() * _XFACTOR + __xcsgc3000_redundance[_XMODEL_BRANCH]);
    len += cur * (sizeof(_xmodel_branch) + sizeof(_xmodel_tieline));
    len += (_xtransformerwinding_size() + _xcompensator_size())* sizeof(_xmodel_branch);
    len += _xcompensator_size() * sizeof(_xmodel_shunt);

    _XCHECK(_xtapchanger)
    cur = (int)(_xtransformerwinding_size() * _XFACTOR) + 
          __xcsgc3000_redundance[_XMODEL_VXFORMER];
    len += cur * sizeof(_xmodel_vxformer);
    len += cur * _XMAX_TAP_COUNT * sizeof(_xmodel_tap);

    _XCHECK(_xstaticvarcompensator)
    cur = _xstaticvarcompensator_size() + __xcsgc3000_redundance[_XMODEL_SVC];
    len += cur * sizeof(_xmodel_svc);
                
#undef _XCHECK
    return _xarray_allocate(len) >= len ? _XTRUE : _XFALSE; 
}

int __xread_company(void){
    int i,capacity;
    _xmodel_company* company;
    struct _xcompany_para *para = _xcompany_para_pointer();
   
    capacity = _xcompany_size() + __xcsgc3000_redundance[_XMODEL_COMPANY];
    if (_xarray_slice_bucket(_XMODEL_COMPANY,capacity) < capacity)
        return _XFALSE;
    
    _xused_of(company) = 0;
    _xmem_zero(_xpointer_of(company,0),_xcapacity_of(company)*sizeof(_xmodel_company));
    
    for (i=_xcompany_head(); _xcompany_valid_index(i); 
         i=_xcompany_next(),_xused_of(company)++){
        company = _xpointer_of(company,_xused_of(company));
        company->id = para[i].m_ID;
    }
    
    return _XTRUE;
}

int __xread_loadarea(void){
    int i,capacity;
    _xmodel_loadarea* loadarea;
    struct _xloadarea_para *para = _xloadarea_para_pointer();
    
    capacity = _xloadarea_size() + __xcsgc3000_redundance[_XMODEL_LOADAREA];
    if (_xarray_slice_bucket(_XMODEL_LOADAREA,capacity) < capacity)
        return _XFALSE;
    
    _xused_of(loadarea) = 0;
    _xmem_zero(_xpointer_of(loadarea,0),_xcapacity_of(loadarea)*sizeof(_xmodel_loadarea));
    
    do {
        if (capacity <= __xcsgc3000_redundance[_XMODEL_LOADAREA]){
            loadarea = _xpointer_of(loadarea,0);_xused_of(loadarea)++;
            loadarea->id = (_xpointer_of(company,0))->id;/*first company is always the internal*/
            break;
        }
        for (i=_xloadarea_head(); _xloadarea_valid_index(i); 
             i=_xloadarea_next(),_xused_of(loadarea)++){
            loadarea = _xpointer_of(loadarea,_xused_of(loadarea));
            loadarea->id = para[i].m_ID;
        }
    } while (0);
    
    return _XTRUE;
}

int __xread_substation(void){
    int i,capacity;
    _xmodel_substation* substation;
    struct _xsubstation_para *para = _xsubstation_para_pointer();
   
    capacity = _xsubstation_size() + __xcsgc3000_redundance[_XMODEL_SUBSTATION];
    if (_xarray_slice_bucket(_XMODEL_SUBSTATION,capacity) < capacity)
        return _XFALSE;
    
    _xused_of(substation) = 0;
    _xmem_zero(_xpointer_of(substation,0),_xcapacity_of(substation)*sizeof(_xmodel_substation));
    
    for (i=_xsubstation_head(); _xsubstation_valid_index(i); 
         i=_xsubstation_next(),_xused_of(substation)++){
        substation = _xpointer_of(substation,_xused_of(substation));
        substation->id       = para[i].m_ID;
        substation->type     = para[i].m_substationType;
        substation->company  = _xid2xidx(para[i].m_company_id,_XMODEL_COMPANY);
        substation->loadarea = _xid2xidx(para[i].m_loadarea_id,_XMODEL_LOADAREA);
        if (!substation->company || !substation->loadarea)
            return _XFALSE;   
    }
    
    return _XTRUE;
}

int __xread_section(void){
    int i,capacity;
    _xmodel_section* section;
    struct _xvoltagelevel_para *para = _xvoltagelevel_para_pointer();
   
    capacity = (int)(_xpowertransformer_size() + _xvoltagelevel_size() * _XFACTOR + 
                     __xcsgc3000_redundance[_XMODEL_SECTION]);
    if (_xarray_slice_bucket(_XMODEL_SECTION,capacity) < capacity)
        return _XFALSE;
    
    _xused_of(section) = 0;
    _xmem_zero(_xpointer_of(section,0),_xcapacity_of(section)*sizeof(_xmodel_section));
    
    for (i=_xvoltagelevel_head(); _xvoltagelevel_valid_index(i); 
         i=_xvoltagelevel_next(),_xused_of(section)++){
        section = _xpointer_of(section,_xused_of(section));
        section->id         = para[i].m_ID;
        section->substation = _xid2xidx(para[i].m_substation_id,_XMODEL_SUBSTATION);
        if (section->substation <= 0) return _XFALSE;
        section->company = _xpointer_of(substation,section->substation-1)->company;
        section->nominal = para[i].m_nominalVoltage;
        section->desired = section->nominal;
        section->high    = para[i].m_highVoltageLimit;
        section->low     = para[i].m_lowVoltageLimit;
        
        if (section->high < section->nominal) section->high = (_xreal)(1.1 * section->nominal);
        if (section->low >= section->nominal || section->low < (_xreal)(0.5 * section->nominal))
            section->low = (_xreal)(0.9 * section->nominal);
        
        section->ehigh = section->high;
        section->elow  = section->low;
        
        _xarray_insert_hash(section->id, _xused_of(section)+1, _XMODEL_SECTION);
    }
    
    return _XTRUE;
}

int __xread_node(void){
    int i,id,capacity;
    _xmodel_node* node;
    _xmodel_section *section;
    struct _xbay_para *bay = _xbay_para_pointer();
    struct _xconnectivitynode_para *para = _xconnectivitynode_para_pointer();
    
    capacity = (int)(_xpowertransformer_size() + _xconnectivitynode_size() * _XFACTOR + 
                     __xcsgc3000_redundance[_XMODEL_NODE]);
    if (_xarray_slice_bucket(_XMODEL_NODE,capacity) < capacity){
        return _XFALSE;
    }
    if (_xarray_slice_bucket(_XTOPOLOGY_TPBUS,capacity) < capacity){
        return _XFALSE;
    }
    _xused_of(node) = 0;_xused_of(tpbus) = 0;
    _xmem_zero(_xpointer_of(node,0),_xcapacity_of(node)*sizeof(_xmodel_node));
    _xmem_zero(_xpointer_of(tpbus,0),_xcapacity_of(tpbus)*sizeof(_xtopology_bus));
    
    for (i=_xconnectivitynode_head(); _xconnectivitynode_valid_index(i); 
         i=_xconnectivitynode_next(),_xused_of(node)++){
        node = _xpointer_of(node,_xused_of(node));
        node->id = para[i].m_ID;
        
        id = para[i].m_equipmentcontainer_tableid;
        if( id == VOLTAGELEVEL_ID)
            node->section = _xid2xidx(para[i].m_equipmentcontainer_id,_XMODEL_SECTION);
        else if(id == BAY_ID)
            node->section = _xid2xidx(bay[para[i].m_equipmentcontainer_idx].m_voltagelevel_id,_XMODEL_SECTION); 
            
        section = _xpointer_of(section,node->section-1);
        if (section) _xlist_push(section->nodes,&(node->velement)); 
          
        _xarray_insert_hash(node->id, _xused_of(node)+1, _XMODEL_NODE);     
    }
    
    return _XTRUE;
}

static inline int _xget_node(int idx){
    return _xterminal_valid_index(idx) ? _xterminal_para_pointer()[idx].m_connectivitynode_id : -1; 
}

static inline int _xget_container(int idx){
    register int i,j;
    register struct _xbay_para *bay = _xbay_para_pointer();
    register struct _xterminal_para *terminal = _xterminal_para_pointer();
    register struct _xconnectivitynode_para *connectivitynode = _xconnectivitynode_para_pointer();
    
    i = _xterminal_valid_index(idx) ? terminal[idx].m_connectivitynode_idx : -1;
    j = _xconnectivitynode_valid_index(i) ? connectivitynode[i].m_equipmentcontainer_tableid : -1;
    if (j == VOLTAGELEVEL_ID){
        return connectivitynode[i].m_equipmentcontainer_id;
    }
    else if (j == BAY_ID){
        return bay[connectivitynode[i].m_equipmentcontainer_idx].m_voltagelevel_id; 
    }
    else
        return -1;
}

static inline int _xget_breaker_status(int meas, int mode, int *actual){
    register int tid,idx,val,next,valid;
    register struct _xmeasurement_data *mdata = _xmeasurement_data_pointer();
    register struct _xstatusinput_data *sdata = _xstatusinput_data_pointer();

    if(!actual) return -1;  
    val=-1;*actual=0;next=meas;valid=_XTRUE;
    while (_xmeasurement_valid_index(next)){
        if (mdata[next].m_measurementtype_id == _XSwitchPosition &&
            mdata[next].m_useFlag){
            *actual = next+1;
            switch(mdata[next].m_srcCurrent){
            case 1:
                   tid = mdata[next].m_pointTableID1;
                   idx = mdata[next].m_pointIdx1;
                   break;
            case 2:
                   tid = mdata[next].m_pointTableID2;
                   idx = mdata[next].m_pointIdx2;
                   break;
            default:
                   tid = mdata[next].m_pointTableID0;
                   idx = mdata[next].m_pointIdx0;
                   break;
            }
            switch (tid){
            case STATUSINPUT_ID:
                if (!_xstatusinput_valid_index(idx)){
                    valid = _XFALSE;
                    break;
          
                }
                if (mode == _XREAL_SUBMODE)
                    val = sdata[idx].m_value;
                else if (mode == _XESTIM_SUBMODE)
                    val = (int)(_xmeasurement_estm_pointer()[next].m_Est_val);
                else if (mode == _XCALC_SUBMODE)
                    val = (int)(_xmeasurement_calc_pointer()[next].m_Cur_val);
                else if (mode == _XANALYSIS_SUBMODE)
                    val = (int)(_xmeasurement_anly_pointer()[next].m_Cur_val);
            default: 
                break;
            }
            if (valid)
                break;
        }
        next = mdata[next].m_objLinkNext;
        if (next==meas) break;
    }
    
    return val;
}

int __xread_breaker(void){
    int i,id,capacity;
    _xmodel_breaker *pbreaker;
    _xmodel_section *psection;
    
    struct _xbreaker_para *bpara = _xbreaker_para_pointer();
    struct _xdisconnector_para *dpara = _xdisconnector_para_pointer();
    struct _xgrounddisconnector_para *gpara = _xgrounddisconnector_para_pointer();
    
    i = _xbreaker_size() + __xcsgc3000_redundance[_XMODEL_BREAKER];
    capacity = i + _xdisconnector_size() + _xgrounddisconnector_size();
    if (_xarray_slice_bucket(_XMODEL_BREAKER,capacity) < capacity){
        return _XFALSE;
    }
    if (_xarray_slice_bucket(_XMODEL_COUPLER,i) < capacity){
        return _XFALSE;
    }
    _xused_of(breaker) = 0;_xused_of(coupler) = 0;
    _xmem_zero(_xpointer_of(breaker,0),_xcapacity_of(breaker)*sizeof(_xmodel_breaker));
    _xmem_zero(_xpointer_of(coupler,0),_xcapacity_of(coupler)*sizeof(_xmodel_coupler));
    
    for (i=_xbreaker_head(); _xbreaker_valid_index(i);
         i=_xbreaker_next(),_xused_of(breaker)++){
        pbreaker = _xpointer_of(breaker,_xused_of(breaker));
        pbreaker->id             = bpara[i].m_ID;
        pbreaker->status.type    = _XBT_BREAKER;
        pbreaker->status.normal  = !bpara[i].m_normalOpen;
        #define _XIS_BYPASS 4
        pbreaker->status.coupler = bpara[i].m_isBypass == _XIS_BYPASS;
        
        pbreaker->fnode   = _xid2xidx(_xget_node(bpara[i].m_terminal0_idx),_XMODEL_NODE);
        pbreaker->tnode   = _xid2xidx(_xget_node(bpara[i].m_terminal1_idx),_XMODEL_NODE);
        pbreaker->section = _xid2xidx(_xget_container(bpara[i].m_terminal0_idx),_XMODEL_SECTION);
        id = _xid2xidx(_xget_container(bpara[i].m_terminal1_idx),_XMODEL_SECTION);
        if (pbreaker->fnode <= 0 || pbreaker->tnode <= 0 || pbreaker->section <= 0 || pbreaker->section != id)
            return _XFALSE;
        psection = _xpointer_of(section, pbreaker->section-1);
        _xlist_push(psection->breakers,&(pbreaker->velement));
        
        if (_xget_breaker_status(_xbreaker_real_pointer()[i].m_measIdx, __xcsgc3000_mode, &(pbreaker->meas)) == 1)
            pbreaker->status.close = _XTRUE;    
    }

    for (i=_xdisconnector_head(); _xdisconnector_valid_index(i);
         i=_xdisconnector_next(),_xused_of(breaker)++){
        pbreaker = _xpointer_of(breaker,_xused_of(breaker));
        pbreaker->id             = dpara[i].m_ID;
        pbreaker->status.type    = _XBT_DISCONNECTOR;
        pbreaker->status.normal  = !dpara[i].m_normalOpen;
        
        pbreaker->fnode   = _xid2xidx(_xget_node(dpara[i].m_terminal0_idx),_XMODEL_NODE);
        pbreaker->tnode   = _xid2xidx(_xget_node(dpara[i].m_terminal1_idx),_XMODEL_NODE);
        pbreaker->section = _xid2xidx(_xget_container(dpara[i].m_terminal0_idx),_XMODEL_SECTION);
        id = _xid2xidx(_xget_container(dpara[i].m_terminal1_idx),_XMODEL_SECTION);
        if (pbreaker->fnode <= 0 || pbreaker->tnode <= 0 || pbreaker->section <= 0 || pbreaker->section != id)
            return _XFALSE;
        psection = _xpointer_of(section, pbreaker->section-1);
        _xlist_push(psection->breakers,&(pbreaker->velement));
        
        if (_xget_breaker_status(_xdisconnector_real_pointer()[i].m_measIdx, __xcsgc3000_mode, &(pbreaker->meas)) == 1)
            pbreaker->status.close = _XTRUE;
    }

    for (i=_xgrounddisconnector_head(); _xgrounddisconnector_valid_index(i);
         i=_xgrounddisconnector_next(),_xused_of(breaker)++){
        pbreaker = _xpointer_of(breaker,_xused_of(breaker));
        pbreaker->id             = gpara[i].m_ID;
        pbreaker->status.type    = _XBT_GROUNDDISCONNECTOR;
        pbreaker->status.normal  = !gpara[i].m_normalOpen;
        
        pbreaker->fnode   = _xid2xidx(_xget_node(gpara[i].m_terminal0_idx),_XMODEL_NODE);
        pbreaker->tnode   = _xid2xidx(_xget_node(gpara[i].m_terminal1_idx),_XMODEL_NODE);
        pbreaker->section = _xid2xidx(_xget_container(gpara[i].m_terminal0_idx),_XMODEL_SECTION);
        id = _xid2xidx(_xget_container(gpara[i].m_terminal1_idx),_XMODEL_SECTION);
        if (pbreaker->fnode <= 0 || pbreaker->tnode <= 0 || pbreaker->section <= 0 || pbreaker->section != id)
            return _XFALSE;
        psection = _xpointer_of(section, pbreaker->section-1);
        _xlist_push(psection->breakers,&(pbreaker->velement));
        
        if (_xget_breaker_status(_xgrounddisconnector_real_pointer()[i].m_measIdx, __xcsgc3000_mode, &(pbreaker->meas)) == 1)
            pbreaker->status.close = _XTRUE;
    }
    
    for (i=0; i<_xused_of(breaker); i++){
        pbreaker = _xpointer_of(breaker,i);
        pbreaker->status.telemeter = _XTRUE;
        if (pbreaker->status.coupler){
            _xmodel_coupler *coupler = _xpointer_of(coupler,_xused_of(coupler));
            _xused_of(coupler)++;
            coupler->breaker = i+1;
            coupler->status.from_to = _XTRUE;
            psection = _xpointer_of(section,pbreaker->section-1);
            _xlist_push(psection->couplers,&(coupler->velement));
        }
    }  
    return _XTRUE;
}

static inline void _xget_point_info(int meas, int *tid, int *pid, int *pidx){
    register struct _xmeasurement_data *mdata = _xmeasurement_data_pointer();
    switch(mdata[meas].m_srcCurrent){
    case 1:
        *tid  = mdata[meas].m_pointTableID1;
        *pid  = mdata[meas].m_ID;
        *pidx = mdata[meas].m_pointIdx1;
        break;
    case 2:
        *tid  = mdata[meas].m_pointTableID2;
        *pid  = mdata[meas].m_ID;
        *pidx = mdata[meas].m_pointIdx2;
        break;
    default:
        *tid  = mdata[meas].m_pointTableID0;
        *pid  = mdata[meas].m_ID;
        *pidx = mdata[meas].m_pointIdx0;
        break;
    }
}

/*
 * we have no analog structure to do this, only reserved for future
 */
static inline int _xpush_analog(int pid, int meas, int meastype, _xreal measwf, int volt, _xreal value, _xreal high){
    return -1; /*!unused*/
}

/* measurement index is the index + 1 of source measurement table, 
 * not the index used in internal array
 */
static inline int _xget_meas_idx(int obj, int volt, int meastype, int terminal, int meas, int mode,
                                 _xreal *high, _xreal *low, _xreal *nominal){
    int tid,pid,pidx,next,valid,measidx,calflag;
    _xreal value = (_xreal)0.0, measwf = (_xreal)100.0;

    struct _xmeasurement_data *mdata = _xmeasurement_data_pointer();
    struct _xanaloginput_data *adata = _xanaloginput_data_pointer();
    struct _xstatusinput_data *sdata = _xstatusinput_data_pointer();
    
    next=meas;measidx=0;valid=_XTRUE;
    if (high && low && nominal) *high = *low = *nominal = (_xreal)0.0;
    while (_xmeasurement_valid_index(next)){
        if (meastype == mdata[next].m_measurementtype_id && mdata[next].m_useFlag &&
           (terminal == mdata[next].m_terminal_id || terminal == -1)){
            _xget_point_info(next,&tid,&pid,&pidx);
            switch (tid){
            case ANALOGINPUT_ID:
                if (!_xanaloginput_valid_index(pidx)){
                    valid = _XFALSE;
                    break;
                }
                if (mode == _XREAL_SUBMODE)
                    value = adata[pidx].m_value;
                else if (mode == _XESTIM_SUBMODE)
                    value = _xmeasurement_estm_pointer()[next].m_Est_val;
                else if (mode == _XCALC_SUBMODE)
                    value = _xmeasurement_calc_pointer()[next].m_Cur_val;
                else if (mode == _XANALYSIS_SUBMODE)
                    value = _xmeasurement_anly_pointer()[next].m_Cur_val;
          
                if (high) *high = adata[pidx].m_operationalHigh;
                if (low) *low = adata[pidx].m_operationalLow;
                if (nominal) *nominal = adata[pidx].m_nominal;
                    
                break;
            case STATUSINPUT_ID:
                if (!_xstatusinput_valid_index(pidx)){
                    valid = _XFALSE;
                    break;
                }   
                if (mode == _XREAL_SUBMODE)
                    value = sdata[pidx].m_value;
                else if (mode == _XESTIM_SUBMODE)
                    value = _xmeasurement_estm_pointer()[next].m_Est_val;
                else if (mode == _XCALC_SUBMODE)
                    value = _xmeasurement_calc_pointer()[next].m_Cur_val;
                else if (mode == _XANALYSIS_SUBMODE)
                    value = _xmeasurement_anly_pointer()[next].m_Cur_val; 
                break;
            default:
                break;
            }
            
            if (mode == _XREAL_SUBMODE){
                measwf  = _xmeasurement_estm_pointer()[next].m_meas_wf; 
                calflag = _xmeasurement_estm_pointer()[next].m_calFlag;
            } 
            else if (mode == _XESTIM_SUBMODE){  
                measwf  = _xmeasurement_estm_pointer()[next].m_meas_wf; 
                calflag = _xmeasurement_estm_pointer()[next].m_calFlag;
            }
            else if (mode == _XCALC_SUBMODE){ 
                measwf  = _xmeasurement_calc_pointer()[next].m_meas_wf;
                calflag = _xmeasurement_calc_pointer()[next].m_calFlag;
            }
            else if (mode == _XANALYSIS_SUBMODE){
                measwf  = _xmeasurement_anly_pointer()[next].m_meas_wf;
                calflag = _xmeasurement_anly_pointer()[next].m_calFlag;
            }
            
            if (valid && calflag > 0){
                _xpush_analog(pid,next,meastype,measwf,volt,value,high ? *high : (_xreal)0.0);
                measidx = next + 1;
                break;
            }
        }
        next = mdata[next].m_objLinkNext;
        if (next == meas) break;
    }
    
    return measidx;
}

int __xread_load(void){
    int i,capacity;
    _xmodel_load* load;
    _xmodel_section *section;
    struct _xenergyconsumer_para *para = _xenergyconsumer_para_pointer();
    
    capacity = _xenergyconsumer_size() + __xcsgc3000_redundance[_XMODEL_LOAD];
    if (_xarray_slice_bucket(_XMODEL_LOAD,capacity) < capacity)
        return _XFALSE;
    
    _xused_of(load) = 0;
    _xmem_zero(_xpointer_of(load,0),_xcapacity_of(load)*sizeof(_xmodel_load));
    
    for (i=_xenergyconsumer_head(); _xenergyconsumer_valid_index(i);
         i=_xenergyconsumer_next(),_xused_of(load)++){
        load = _xpointer_of(load,_xused_of(load));
        load->id       = para[i].m_ID;
        load->node     = _xid2xidx(_xget_node(para[i].m_terminal0_idx),_XMODEL_NODE);
        load->section  = _xid2xidx(_xget_container(para[i].m_terminal0_idx),_XMODEL_SECTION);
        if (load->node <= 0 || load->section <= 0 || _xpointer_of(node,load->node-1)->section != load->section)
            return _XFALSE;
            
        if (para[i].m_loadArea_id < 0 && _xenergyconsumer_size() <= 1)
            load->loadarea = _xid2xidx(para[i].m_company_id,_XMODEL_LOADAREA);
        else
            load->loadarea = _xid2xidx(para[i].m_loadArea_id,_XMODEL_LOADAREA);
        
        section = _xpointer_of(section,load->section-1);
        _xlist_push(section->loads,&(load->velement)); 
        
        load->mw_meas   = _xget_meas_idx(load->id,load->section-1,
                                         _XThreePhaseActivePower,
                                         para[i].m_terminal0_id,
                                         _xenergyconsumer_real_pointer()[i].m_measIdx,
                                         __xcsgc3000_mode,
                                         &(load->high_mw),
                                         &(load->low_mw),
                                         &(load->def_mw)
        ); 
        load->mvar_meas = _xget_meas_idx(load->id,load->section-1,
                                         _XThreePhaseReactivePower,
                                         para[i].m_terminal0_id,
                                         _xenergyconsumer_real_pointer()[i].m_measIdx,
                                         __xcsgc3000_mode,
                                         &(load->high_mvar),
                                         &(load->low_mvar),
                                         &(load->def_mvar)
        ); 
        load->status.tel_mw   = load->mw_meas > 0   ? _XTRUE : _XFALSE;
        load->status.tel_mvar = load->mvar_meas > 0 ? _XTRUE : _XFALSE;
    }

    return _XTRUE;
}

int __xread_generator(void){
    int i,capacity;
    _xmodel_section *section;
    _xmodel_generator* generator;
    _xmodel_swingbus * swingbus;
    struct _xequivalentsource_para *epara = _xequivalentsource_para_pointer();
    struct _xsynchronousmachine_para *spara = _xsynchronousmachine_para_pointer();
    
    capacity = _xsynchronousmachine_size() + _xequivalentsource_size() +
               __xcsgc3000_redundance[_XMODEL_GENERATOR];
    if (_xarray_slice_bucket(_XMODEL_GENERATOR,capacity) < capacity)
        return _XFALSE;
    if (_xarray_slice_bucket(_XMODEL_SWINGBUS,capacity) < capacity)
        return _XFALSE;
    
    _xused_of(generator) = 0;_xused_of(swingbus) = 0;
    _xmem_zero(_xpointer_of(generator,0),_xcapacity_of(generator)*sizeof(_xmodel_generator));
    _xmem_zero(_xpointer_of(swingbus,0),_xcapacity_of(swingbus)*sizeof(_xmodel_swingbus));
    
    for (i=_xsynchronousmachine_head(); _xsynchronousmachine_valid_index(i);
         i=_xsynchronousmachine_next(),_xused_of(generator)++){
        generator = _xpointer_of(generator,_xused_of(generator));
        generator->id      = spara[i].m_ID;
        generator->rank    = spara[i].m_swingRank;
        
        generator->node    = _xid2xidx(_xget_node(spara[i].m_terminal0_idx),_XMODEL_NODE);
        generator->section = _xid2xidx(_xget_container(spara[i].m_terminal0_idx),_XMODEL_SECTION);
        if (generator->node <= 0 || generator->section <= 0)
            return _XFALSE;
        section = _xpointer_of(section,generator->section-1);
        _xlist_push(section->generators, &(generator->velement));
        
        generator->mw_meas   = _xget_meas_idx(generator->id,generator->section-1,
                                              _XThreePhaseActivePower,
                                              spara[i].m_terminal0_id,
                                              _xsynchronousmachine_real_pointer()[i].m_measIdx,
                                              __xcsgc3000_mode,
                                              &(generator->high_mw),
                                              &(generator->low_mw),
                                              &(generator->def_mw)
        ); 
        generator->mvar_meas = _xget_meas_idx(generator->id,generator->section-1,
                                              _XThreePhaseReactivePower,
                                              spara[i].m_terminal0_id,
                                              _xsynchronousmachine_real_pointer()[i].m_measIdx,
                                              __xcsgc3000_mode,
                                              &(generator->high_mvar),
                                              &(generator->low_mvar),
                                              &(generator->def_mvar)
        );

        generator->status.tel_mw      = generator->mw_meas > 0   ? _XTRUE : _XFALSE;
        generator->status.tel_mvar    = generator->mvar_meas > 0 ? _XTRUE : _XFALSE;
        generator->status.in_internal = section->company == 1 ? _XTRUE : _XFALSE;
        
        swingbus = _xpointer_of(swingbus,_xused_of(swingbus));_xused_of(swingbus)++;
        swingbus->id        = generator->id;
        swingbus->generator = _xused_of(generator) + 1;
    }
    
    for (i=_xequivalentsource_head(); _xequivalentsource_valid_index(i);
         i=_xequivalentsource_next(),_xused_of(generator)++){
        generator = _xpointer_of(generator,_xused_of(generator));
        generator->id      = epara[i].m_ID;
        
        generator->node    = _xid2xidx(_xget_node(epara[i].m_terminal0_idx),_XMODEL_NODE);
        generator->section = _xid2xidx(_xget_container(epara[i].m_terminal0_idx),_XMODEL_SECTION);
        if (generator->node <= 0 || generator->section <= 0)
            return _XFALSE;
        section = _xpointer_of(section,generator->section-1);
        _xlist_push(section->generators, &(generator->velement));
        
        generator->mw_meas   = _xget_meas_idx(generator->id,generator->section-1,
                                              _XThreePhaseActivePower,
                                              epara[i].m_terminal0_id,
                                              _xequivalentsource_real2_pointer()[i].m_measIdx,
                                              __xcsgc3000_mode,
                                              &(generator->high_mw),
                                              &(generator->low_mw),
                                              &(generator->def_mw)
        ); 
        generator->mvar_meas = _xget_meas_idx(generator->id,generator->section-1,
                                              _XThreePhaseReactivePower,
                                              epara[i].m_terminal0_id,
                                              _xequivalentsource_real2_pointer()[i].m_measIdx,
                                              __xcsgc3000_mode,
                                              &(generator->high_mvar),
                                              &(generator->low_mvar),
                                              &(generator->def_mvar)
        );

        generator->status.tel_mw      = generator->mw_meas > 0   ? _XTRUE : _XFALSE;
        generator->status.tel_mvar    = generator->mvar_meas > 0 ? _XTRUE : _XFALSE;
        generator->status.in_internal = section->company == 1 ? _XTRUE : _XFALSE;
        
        swingbus = _xpointer_of(swingbus,_xused_of(swingbus));_xused_of(swingbus)++;
        swingbus->id            = generator->id;
        swingbus->generator     = _xused_of(generator) + 1;
        swingbus->status.no_gen = _XTRUE;
    }
    
    return _XTRUE;
}

int __xread_voltage(void){
    int i,capacity;
    _xmodel_section *section;
    _xmodel_voltage* voltage;

    struct _xbusbarsection_para *para = _xbusbarsection_para_pointer();

    capacity = _xbusbarsection_size() + __xcsgc3000_redundance[_XMODEL_VOLTAGE];
    if (_xarray_slice_bucket(_XMODEL_VOLTAGE,capacity) < capacity)
        return _XFALSE;
    
    _xused_of(voltage) = 0;
    _xmem_zero(_xpointer_of(voltage,0),_xcapacity_of(voltage)*sizeof(_xmodel_voltage));
    
    for (i=_xbusbarsection_head(); _xbusbarsection_valid_index(i);
         i=_xbusbarsection_next(),_xused_of(voltage)++){
        voltage = _xpointer_of(voltage,_xused_of(voltage));
        voltage->id = para[i].m_ID;
        
        voltage->node    = _xid2xidx(_xget_node(para[i].m_terminal0_idx),_XMODEL_NODE);
        voltage->section = _xid2xidx(_xget_container(para[i].m_terminal0_idx),_XMODEL_SECTION);
        if (voltage->node <= 0 || voltage->section <= 0)
            return _XFALSE;
        section = _xpointer_of(section,voltage->section-1);
        _xlist_push(section->voltages, &(voltage->velement));
        
        voltage->freq_meas = _xget_meas_idx(voltage->id,voltage->section-1,
                                            _XFrequency,
                                            para[i].m_terminal0_id,
                                            _xbusbarsection_real_pointer()[i].m_measIdx,
                                            __xcsgc3000_mode,
                                            NULL,
                                            NULL,
                                            &(voltage->freq)
        );
        voltage->angle_meas = _xget_meas_idx(voltage->id,voltage->section-1,
                                             _XThreePhaseAngle,
                                             para[i].m_terminal0_id,
                                             _xbusbarsection_real_pointer()[i].m_measIdx,
                                             __xcsgc3000_mode,
                                             NULL,
                                             NULL,
                                             &(voltage->angle)
        );
        voltage->voltage_meas = _xget_meas_idx(voltage->id,voltage->section-1,
                                               _XThreePhaseVoltage,
                                               para[i].m_terminal0_id,
                                               _xbusbarsection_real_pointer()[i].m_measIdx,
                                               __xcsgc3000_mode,
                                               NULL,
                                               NULL,
                                               &(voltage->voltage)
        );
    }
    
    return _XTRUE;
}

/*
 * read transformer after reading aclinesegment
 */
static int __xread_transformer(void){
    int i,t1,t2,t3,tid;
    _xmodel_node *node;
    _xmodel_branch *branch;
    _xmodel_section *section,*fsection,*tsection;
    struct _xpowertransformer_para *para = _xpowertransformer_para_pointer();
    struct _xtransformerwinding_para *winding = _xtransformerwinding_para_pointer();
    
    for (i=_xpowertransformer_head(); _xpowertransformer_valid_index(i);
         i=_xpowertransformer_next()){
        t1 = para[i].m_transformerwindingprimary_idx;
        t2 = para[i].m_transformerwindingsecondary_idx;
        branch = _xpointer_of(branch,_xused_of(branch));
        branch->fsection = _xid2xidx(_xget_container(winding[t1].m_terminal0_idx),_XMODEL_SECTION);
        branch->tsection = _xid2xidx(_xget_container(winding[t2].m_terminal0_idx),_XMODEL_SECTION);
        if (branch->fsection <= 0 || branch->tsection <= 0)
            return _XFALSE;
        fsection = _xpointer_of(section,branch->fsection-1);
        tsection = _xpointer_of(section,branch->tsection-1);    
                
        branch->voltage = fsection->nominal;
        if (fsection->nominal < tsection->nominal){
            branch->voltage = tsection->nominal;
            t1 ^= t2 ^= t1 ^= t2;
        }
      
        if (para[i].m_transformerwindingtertiary_idx < 0){ /*2 trwindings*/
            branch->id = winding[t1].m_ID;
            branch->status.vxformer = _XTRUE;
            branch->fnode = _xid2xidx(_xget_node(winding[t1].m_terminal0_idx),_XMODEL_NODE);
            branch->tnode = _xid2xidx(_xget_node(winding[t2].m_terminal0_idx),_XMODEL_NODE);
            
            #define _XZB  (branch->voltage*branch->voltage)/_XBASE_MVA
            #define _XYB  _XBASE_MVA/(branch->voltage*branch->voltage)
            #define _XRX2 (branch->r * branch->r + branch->x * branch->x)
            #define _XSET_BRANCH(T1,T2)                                               \
                    branch->r = (_xreal)(winding[T1].m_r/_XZB);                       \
                    branch->x = (_xreal)(winding[T2].m_x/_XZB);                       \
                    if (fabs(branch->r) <= _XMACHEPS && fabs(branch->x) <= _XMACHEPS) \
                        branch->x = (_xreal)(_XMACHEPS);                              \
                    branch->g = (_xreal)(branch->r/_XRX2);                            \
                    branch->b = (_xreal)(branch->x/_XRX2);                            \
                    branch->bc_a = (_xreal)(winding[T1].m_b/_XZB);                    \
                    branch->bg_a = (_xreal)(winding[T1].m_g/_XZB);                   
       
        _XSET_BRANCH(t1,t2)
            _xused_of(branch)++;
        }
        else { /*3 trwindings*/
            int id;
            t3 = para[i].m_transformerwindingtertiary_idx;
            if ((id = _xget_container(winding[t3].m_terminal0_idx)) < 0)
                return _XFALSE;
                
            /* add one virtual section & node
             * no need to hash, avoid collision 
             */
            section = _xpointer_of(section, _xused_of(section));
            section->id = para[i].m_ID;
            section->nominal = (_xreal)1.0;
            
            tid = para[i].m_equipmentcontainer_tableid;
            if (tid == SUBSTATION_ID){
                section->substation = _xid2xidx(para[i].m_equipmentcontainer_id, _XMODEL_SUBSTATION);
            }
            else if (tid == VOLTAGELEVEL_ID){
                int idx = para[i].m_equipmentcontainer_idx;
                if (_xvoltagelevel_valid_index(idx))
                    section->substation = _xid2xidx(_xvoltagelevel_para_pointer()[idx].m_substation_id,
                                          _XMODEL_SUBSTATION);
            }
            else if (tid == BAY_ID){
                int idx = para[i].m_equipmentcontainer_idx;
                if (_xbay_valid_index(idx)){
                    idx = _xbay_para_pointer()[idx].m_voltagelevel_idx;
                    if (_xvoltagelevel_valid_index(idx))
                        section->substation = _xid2xidx(_xvoltagelevel_para_pointer()[idx].m_substation_id,
                                              _XMODEL_SUBSTATION);
                }
            }
            if (section->substation <= 0)
                return _XFALSE;
            section->company = _xpointer_of(substation,section->substation-1)->company;  
            
            node = _xpointer_of(node, _xused_of(node));
            node->id = para[i].m_ID;/*transformer id*/
            node->section = _xused_of(section) + 1;
            _xlist_push(section->nodes, &(node->velement));
            
            _xused_of(node)++;
            _xused_of(section)++;
            
            /*primary winding*/
            branch->id = winding[t1].m_ID;
            branch->status.vxformer = _XTRUE;
            branch->fnode = _xid2xidx(_xget_node(winding[t1].m_terminal0_idx),_XMODEL_NODE);
            branch->tnode = _xused_of(node);
            branch->tsection = _xused_of(section);
            branch->voltage  = fsection->nominal;
            
            _XSET_BRANCH(t1,t1) 
            _xused_of(branch)++;
            /*secondary winding*/
            branch = _xpointer_of(branch,_xused_of(branch));
            branch->id = winding[t2].m_ID;
            branch->status.vxformer = _XTRUE;
            branch->fnode = _xid2xidx(_xget_node(winding[t2].m_terminal0_idx),_XMODEL_NODE);
            branch->tnode = _xused_of(node);
            branch->fsection = _xid2xidx(tsection->id, _XMODEL_SECTION);
            branch->tsection = _xused_of(section);
            branch->voltage  = tsection->nominal;
            
            _XSET_BRANCH(t2,t2) 
            _xused_of(branch)++;
            /*tertiary winding*/
            branch = _xpointer_of(branch,_xused_of(branch));
            branch->id = winding[t3].m_ID;
            branch->status.vxformer = _XTRUE;
            branch->fnode = _xid2xidx(_xget_node(winding[t3].m_terminal0_idx),_XMODEL_NODE);
            branch->tnode = _xused_of(node);
            branch->fsection = _xid2xidx(id,_XMODEL_SECTION);
            branch->tsection = _xused_of(section);
            branch->voltage  = _xpointer_of(section,branch->fsection-1)->nominal;
            
            _XSET_BRANCH(t3,t3)
            _xused_of(branch)++;
        }
    }
    
    return _XTRUE;
    
#undef _XZB
#undef _XYB
#undef _XRX2
#undef _XSET_BRANCH
}

int __xread_branch(void){
    int i,capacity;
    _xmodel_branch *branch;
    _xmodel_section *fsection,*tsection;
    struct _xaclinesegment_para *para = _xaclinesegment_para_pointer();
    
    i = (int)(_xaclinesegment_size() * _XFACTOR + __xcsgc3000_redundance[_XMODEL_BRANCH]);
    capacity = _xtransformerwinding_size() + _xcompensator_size() + i;
    if (_xarray_slice_bucket(_XMODEL_BRANCH,capacity) < capacity)
        return _XFALSE;
    if (_xarray_slice_bucket(_XMODEL_TIELINE,i) < i)
        return _XFALSE;
    
    _xused_of(branch) = 0;_xused_of(tieline) = 0;
    _xmem_zero(_xpointer_of(branch,0),_xcapacity_of(branch)*sizeof(_xmodel_branch));
    _xmem_zero(_xpointer_of(tieline,0),_xcapacity_of(tieline)*sizeof(_xmodel_tieline));
    
    for (i=_xaclinesegment_head(); _xaclinesegment_valid_index(i);
         i=_xaclinesegment_next(),_xused_of(branch)++){
        branch = _xpointer_of(branch,_xused_of(branch));
        branch->id = para[i].m_ID;
        
        branch->fnode = _xid2xidx(_xget_node(para[i].m_terminal0_idx),_XMODEL_NODE);
        branch->tnode = _xid2xidx(_xget_node(para[i].m_terminal1_idx),_XMODEL_NODE);
        if (branch->fnode <= 0 || branch->tnode <= 0)
            return _XFALSE;
            
        branch->fsection = _xid2xidx(_xget_container(para[i].m_terminal0_idx),_XMODEL_SECTION);
        branch->tsection = _xid2xidx(_xget_container(para[i].m_terminal1_idx),_XMODEL_SECTION);
        if (branch->fsection <= 0 || branch->tsection <= 0){
            _xused_of(branch)--;
            continue;
        }
        fsection = _xpointer_of(section,branch->fsection-1);
        tsection = _xpointer_of(section,branch->tsection-1);
        if (fabs(fsection->nominal - tsection->nominal) > 5.0)
            return _XFALSE;
        branch->voltage = _xmax(fsection->nominal,tsection->nominal);
  
        #define _XZB  (branch->voltage*branch->voltage)/_XBASE_MVA
        #define _XYB  _XBASE_MVA/(branch->voltage*branch->voltage)
        #define _XRX2 (branch->r * branch->r + branch->x * branch->x)
        
        branch->r = (_xreal)(para[i].m_r/_XZB); /* p.u. resistance */
        branch->x = (_xreal)(para[i].m_x/_XZB); /* p.u. reactance  */
        if (fabs(branch->r) <= _XMACHEPS && fabs(branch->x) <= _XMACHEPS)
            branch->x = (_xreal)(_XMACHEPS);
        
        branch->g = branch->r/_XRX2;
        branch->b = branch->x/_XRX2;
        
        branch->bc_a = (_xreal)(0.5*para[i].m_bch/_XYB);
        branch->bc_b = (_xreal)(0.5*para[i].m_bch/_XYB);
        branch->bg_a = (_xreal)(0.5*para[i].m_gch/_XYB);
        branch->bg_b = (_xreal)(0.5*para[i].m_gch/_XYB);
        
        #undef _XRX2
        #undef _XYB
        #undef _XZB
        
        if (fsection->company != tsection->company){
            _xmodel_tieline *tieline = _xpointer_of(tieline, _xused_of(tieline));
            _xused_of(tieline)++;
            tieline->branch   = _xused_of(branch) + 1;
            tieline->acompany = fsection->company;
            tieline->bcompany = tsection->company;
            branch->status.tie = _XTRUE;
        }
    }
      
    return __xread_transformer();
}

static void __xread_vxtap(int vx, int idx, _xreal base){
    int i;
    _xmodel_tap *tap;
    _xmodel_vxformer *vxformer = _xpointer_of(vxformer, vx-1);
    struct _xtapchanger_para *para = _xtapchanger_para_pointer();
    
    if (_xtapchanger_valid_index(idx)){
        _xreal value = (_xreal)(para[idx].m_neutralKV/base);
        if (value < 0.5)
            value = (_xreal)1.0;
        
        for (i=para[idx].m_lowStep; i<=para[idx].m_highStep; i++){
            tap = _xpointer_of(tap,_xused_of(tap));
            tap->id = ((vxformer->id)<<5) + i;
            tap->xformer = vx;
            tap->status.vxformer = _XTRUE;
            if (i == para[idx].m_lowStep)    tap->status.min_pos = _XTRUE;
            if (i == para[idx].m_normalStep) tap->status.def_pos = _XTRUE;
            if (i == para[idx].m_highStep)   tap->status.max_pos = _XTRUE;
              
            tap->value   = (_xreal)((i-para[idx].m_neutralStep)*fabs(para[idx].m_stepVoltageIncrement)+value);
            tap->zfactor = tap->value * tap->value;
              
            _xused_of(tap)++;
            _xlist_push(vxformer->taps, &(tap->xelement));
        }   
    }
    else {
        tap = _xpointer_of(tap,_xused_of(tap));
        tap->id      = (vxformer->id)<<5;
        tap->xformer = vx;
        tap->value   = (_xreal)1.0;
        tap->zfactor = (_xreal)1.0;

        tap->status.vxformer = _XTRUE;
        tap->status.def_pos  = _XTRUE;
        tap->status.min_pos  = _XTRUE;
        tap->status.max_pos  = _XTRUE;
        _xused_of(tap)++;
        _xlist_push(vxformer->taps, &(tap->xelement));
    }
}

static void __xget_vxtap_meas(int vx, int idx, int wd){
    int meas;
    _xreal value;
    _xlist *lptr;
    _xmodel_tap *tap;
    _xmodel_branch *branch;
    _xmodel_vxformer *vxformer = _xpointer_of(vxformer,vx-1);
    struct _xtapchanger_para *para = _xtapchanger_para_pointer();
    struct _xtransformerwinding_para *winding = _xtransformerwinding_para_pointer();
    
    branch = _xpointer_of(branch,_xid2xidx(wd, _XMODEL_BRANCH)-1);
    assert(branch);
    
    branch->xformer = vx;
    vxformer->substation = _xpointer_of(section,branch->fsection-1)->substation;
    
    if (_xtapchanger_valid_index(idx) && _xtransformerwinding_valid_index(para[idx].m_transformerwinding_idx)){
        meas = _xget_meas_idx(para[idx].m_transformerwinding_id,
                              branch->fsection-1,
                              _XTapPosition,
                              -1,
                              _xtransformerwinding_real_pointer()[para[idx].m_transformerwinding_idx].m_measIdx,
                              __xcsgc3000_mode,
                              NULL,
                              NULL,
                              &value);
        if (meas <= 0){
            meas = _xget_meas_idx(para[idx].m_ID,
                              branch->fsection-1,
                              _XTapPosition,
                              -1,
                              _xtransformerwinding_real_pointer()[para[idx].m_transformerwinding_idx].m_measIdx,
                              __xcsgc3000_mode,
                              NULL,
                              NULL,
                              &value);
        }
        
        if (meas){
            _xlist *cur = NULL;
            int id = ((vxformer->id)<<5)+(int)value;
            vxformer->status.tap_on = _XTRUE;
            
            _xlist_for_each(lptr,vxformer->taps){
                tap = _xcontainer_of(lptr, _xmodel_tap, xelement);
                tap->meas = 0;
                tap->status.meas_pos = _XFALSE;
                if (tap->id == id){
                    cur = lptr;
                    tap->meas = meas;
                    tap->status.meas_pos = _XTRUE;
                }
            }
            /*present first*/
            if (cur){
                _xlist_for_each(lptr,vxformer->taps){
                    if (lptr->next == cur){
                        lptr->next = cur->next;
                        break;
                    }
                }
                _xlist_push(vxformer->taps,cur);
            }
        }
    }
}

int __xread_vxformer(void){
    int i,t1,t2,t3,s1,s2,idx,capacity;
    _xmodel_vxformer *vxformer;
    _xmodel_section *fsection, *tsection;
    struct _xtapchanger_para *tapchanger = _xtapchanger_para_pointer();
    struct _xtransformerwinding_para *winding = _xtransformerwinding_para_pointer();
    struct _xpowertransformer_para *para = _xpowertransformer_para_pointer();
    
    capacity = (int)(_xtransformerwinding_size() * _XFACTOR) + 
                     __xcsgc3000_redundance[_XMODEL_VXFORMER];
    if (_xarray_slice_bucket(_XMODEL_VXFORMER,capacity) < capacity)
        return _XFALSE;
    if (_xarray_slice_bucket(_XMODEL_TAP,capacity*_XMAX_TAP_COUNT) < capacity*_XMAX_TAP_COUNT)
        return _XFALSE;
    
    _xused_of(vxformer) = 0;_xused_of(tap) = 0;
    _xmem_zero(_xpointer_of(vxformer,0),_xcapacity_of(vxformer)*sizeof(_xmodel_vxformer));
    _xmem_zero(_xpointer_of(tap,0),_xcapacity_of(tap)*sizeof(_xmodel_tap));

    for (i=_xpowertransformer_head(); _xpowertransformer_valid_index(i);
         i=_xpowertransformer_next()){
        t1 = para[i].m_transformerwindingprimary_idx;
        t2 = para[i].m_transformerwindingsecondary_idx;
        
        s1 = _xid2xidx(_xget_container(winding[t1].m_terminal0_idx),_XMODEL_SECTION);
        s2 = _xid2xidx(_xget_container(winding[t2].m_terminal0_idx),_XMODEL_SECTION);
        if (s1 <= 0 || s2 <= 0)
            return _XFALSE;
                
        fsection = _xpointer_of(section, s1-1);
        tsection = _xpointer_of(section, s2-1);
        if (fsection->nominal < tsection->nominal){
            t1 ^= t2 ^= t1 ^= t2;
        }
        
        if (para[i].m_transformerwindingtertiary_idx < 0){/*2 winding*/  
            if (fsection->nominal < tsection->nominal){
                _xmodel_section *tmp;
                tmp = fsection;fsection = tsection; tsection = tmp;
            } 
            vxformer = _xpointer_of(vxformer,_xused_of(vxformer));
            vxformer->id = winding[t1].m_ID;
            
            idx = -1;
            if (_xtapchanger_valid_index(winding[t1].m_Tapchanger_idx)){
                idx = t1;
                vxformer->status.tap_b = _XFALSE;
            }
            else if (_xtapchanger_valid_index(winding[t2].m_Tapchanger_idx)){
                idx = t2;
                vxformer->status.tap_b = _XTRUE;
            }
            else
                vxformer->status.tap_bad = _XTRUE;
                
            __xread_vxtap(_xused_of(vxformer)+1,idx,fsection->nominal);
            __xget_vxtap_meas(_xused_of(vxformer)+1,idx,winding[t1].m_ID);
            _xused_of(vxformer)++;
        }
        else { /*3 trwindings*/
            int id;
            t3 = para[i].m_transformerwindingtertiary_idx;
            if ((id = _xget_container(winding[t3].m_terminal0_idx)) < 0)
                return _XFALSE;
            /*primary winding*/
            vxformer = _xpointer_of(vxformer,_xused_of(vxformer));
            vxformer->id = winding[t1].m_ID;
                
            __xread_vxtap(_xused_of(vxformer)+1,t1,fsection->nominal);
            __xget_vxtap_meas(_xused_of(vxformer)+1,t1,winding[t1].m_ID);
            _xused_of(vxformer)++;
            
            /*secondary winding*/
            vxformer = _xpointer_of(vxformer,_xused_of(vxformer));
            vxformer->id = winding[t2].m_ID;
                
            __xread_vxtap(_xused_of(vxformer)+1,t2,fsection->nominal);
            __xget_vxtap_meas(_xused_of(vxformer)+1,t2,winding[t2].m_ID);
            _xused_of(vxformer)++;
            
            /*tertiary winding*/
            fsection = _xpointer_of(section,_xid2xidx(id,_XMODEL_SECTION)-1);
            vxformer = _xpointer_of(vxformer,_xused_of(vxformer));
            vxformer->id = winding[t3].m_ID;
                
            __xread_vxtap(_xused_of(vxformer)+1,t3,fsection->nominal);
            __xget_vxtap_meas(_xused_of(vxformer)+1,t3,winding[t3].m_ID);
            _xused_of(vxformer)++;
        }
    }
    return _XTRUE;
}

int __xread_svc(void){
    int i,capacity;
    _xmodel_svc *svc;
    _xmodel_section *section;
    struct _xstaticvarcompensator_para *para = _xstaticvarcompensator_para_pointer();
    
    capacity = _xstaticvarcompensator_size() + __xcsgc3000_redundance[_XMODEL_SVC];
    if (_xarray_slice_bucket(_XMODEL_SVC,capacity) < capacity)
        return _XFALSE;
        
    _xused_of(svc) = 0;
    _xmem_zero(_xpointer_of(svc,0),_xcapacity_of(svc)*sizeof(_xmodel_svc));
    
    for (i=_xstaticvarcompensator_head(); _xstaticvarcompensator_valid_index(i);
         i=_xstaticvarcompensator_next(),_xused_of(svc)++){
        svc = _xpointer_of(svc,_xused_of(svc));
        svc->id      = para[i].m_ID;
        svc->node    = _xid2xidx(_xget_node(para[i].m_terminal0_idx),_XMODEL_NODE);
        svc->section = _xid2xidx(_xget_container(para[i].m_terminal0_idx),_XMODEL_SECTION);
        if (svc->node <= 0 || svc->section <= 0)
            return _XFALSE;
        
        section = _xpointer_of(section, svc->section-1);
        _xlist_push(section->svcs, &(svc->velement));
        
        svc->meas = _xget_meas_idx(svc->id,
                                   svc->section-1,
                                   _XThreePhaseReactivePower,
                                   -1,
                                   _xstaticvarcompensator_real_pointer()[i].m_measIdx,
                                   __xcsgc3000_mode,
                                   &(svc->high_mvar),
                                   &(svc->low_mvar),
                                   &(svc->def_mvar));
    }
    
    return _XTRUE;
}

int __xread_shunt(void){
    _xreal r,x;
    int i,idx,obj,tmp,capacity;
    _xmodel_shunt *shunt;
    _xmodel_branch *branch;
    _xmodel_section *section;
    struct _xcompensator_para *para = _xcompensator_para_pointer();
    
    capacity = _xcompensator_size() + __xcsgc3000_redundance[_XMODEL_SHUNT];
    if (_xarray_slice_bucket(_XMODEL_SHUNT,capacity) < capacity)
        return _XFALSE;
        
    _xused_of(shunt) = 0;
    _xmem_zero(_xpointer_of(shunt,0),_xcapacity_of(shunt)*sizeof(_xmodel_shunt));
    
    for (i=_xcompensator_head(); _xcompensator_valid_index(i);
         i=_xcompensator_next()){
        if (para[i].m_compensatorType == 1){/*shunt*/
            shunt = _xpointer_of(shunt, _xused_of(shunt));
            shunt->id      = para[i].m_ID;
            shunt->node    = _xid2xidx(_xget_node(para[i].m_terminal0_idx),_XMODEL_NODE);
            shunt->section = _xid2xidx(_xget_container(para[i].m_terminal0_idx),_XMODEL_SECTION);
            if (shunt->node <= 0 || shunt->section <= 0)
                return _XFALSE;
            section = _xpointer_of(section, shunt->section-1);
            _xlist_push(section->shunts,&(shunt->velement));
            
            #define _XYB _XBASE_MVA/(section->nominal*section->nominal)
            r = para[i].m_r,x = para[i].m_x;
            if (fabs(r) < _XMACHEPS && fabs(x) < _XMACHEPS)
                x = (_xreal)(_XMACHEPS);
            shunt->b = x/(r*r+x*x);
            shunt->b = (_xreal)(shunt->b/(_XYB));
            #undef _XYB
            
            obj = -1;
            tmp = idx = para[i].m_terminal0_idx;
            while (_xterminal_valid_index(idx)){
                obj = _xterminal_para_pointer()[idx].m_conductingequipment_tableid;
                if (obj == BREAKER_ID){
                    obj = _xterminal_para_pointer()[idx].m_conductingequipment_id;
                    break;
                }
                obj = -1;
                idx = _xterminal_para_pointer()[idx].m_nextTerminalIdx;
                if (idx == tmp)
                    break;
            }
            shunt->breaker = _xid2xidx(obj,_XMODEL_BREAKER);
            shunt->meas    = _xget_meas_idx(shunt->id,
                                            shunt->section-1,
                                            _XThreePhaseReactivePower,
                                            -1,
                                            _xcompensator_real_pointer()[i].m_measIdx,
                                            __xcsgc3000_mode,
                                            NULL,
                                            NULL,
                                            NULL);
            _xused_of(shunt)++;
        }
        else if (para[i].m_compensatorType == 2){/*series*/
            branch = _xpointer_of(branch, _xused_of(branch));
            branch->id       = para[i].m_ID;
            branch->fnode    = _xid2xidx(_xget_node(para[i].m_terminal0_idx),_XMODEL_NODE);
            branch->tnode    = _xid2xidx(_xget_node(para[i].m_terminal1_idx),_XMODEL_NODE);
            branch->fsection = _xid2xidx(_xget_container(para[i].m_terminal0_idx),_XMODEL_SECTION);
            branch->tsection = _xid2xidx(_xget_container(para[i].m_terminal1_idx),_XMODEL_SECTION);
            if (branch->fnode <= 0 || branch->tnode <= 0 || branch->fsection <= 0 || branch->tsection <= 0)
                return _XFALSE;
            branch->status.scap = _XTRUE;
            section = _xpointer_of(section, branch->fsection-1);
            
            #define _XZB (section->nominal*section->nominal)/_XBASE_MVA
            branch->r = (_xreal)(para[i].m_r/(_XZB));
            branch->x = (_xreal)(para[i].m_x/(_XZB));
            if (fabs(branch->r) < _XMACHEPS && fabs(branch->x) < _XMACHEPS)
                branch->x = (_xreal)(_XMACHEPS);
            branch->g = branch->r/(branch->r*branch->r+branch->x*branch->x);
            branch->b = branch->x/(branch->r*branch->r+branch->x*branch->x);
            #undef _XZB
              
            _xused_of(branch)++;
        }
    }
    
    return _XTRUE;
}

