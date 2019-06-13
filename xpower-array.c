/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines Dispatcher Training Simulator (DTS) object 
** information and only used for internal.
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#include <assert.h>
#include <stdlib.h>
#include "xpower-hash.h"
#include "xpower-array.h"

#define _xdump_tpbus NULL
#define _xdump_tpgen NULL
#define _xdump_tpbranch NULL
#define _xdump_tpisland NULL

/*
 * array member information, used for dynamic
 */
typedef struct __xarray_info {
    int id;
    int size;
    int offsetof_used;
    int offsetof_capacity;
    int offsetof_ptr;
    const char *name;
    struct _xhash table;
    void (*dump)(FILE *, void *);
} _xarray_info;

static _xarray_info _xarray_infos[] = {
#define _XARRAY_INFO(N,M,T) {N,                             \
                             sizeof(T),                     \
                             offsetof(_xarray,M##_used),    \
                             offsetof(_xarray,M##_capacity),\
                             offsetof(_xarray,M##_ptr),     \
                             #T,                            \
                             {0,0,NULL},                    \
                             _xdump_##M}
    {0,0,0,0,0,NULL,{0,0,NULL},NULL},/*_XMODEL_UNKNOWN*/
    _XARRAY_INFO(_XMODEL_COMPANY,    company,   _xmodel_company),   
    _XARRAY_INFO(_XMODEL_SUBSTATION, substation,_xmodel_substation),
    _XARRAY_INFO(_XMODEL_LOADAREA,   loadarea,  _xmodel_loadarea),  
    _XARRAY_INFO(_XMODEL_SECTION,    section,   _xmodel_section),   
    _XARRAY_INFO(_XMODEL_NODE,       node,      _xmodel_node),      
    _XARRAY_INFO(_XMODEL_BREAKER,    breaker,   _xmodel_breaker),   
    _XARRAY_INFO(_XMODEL_COUPLER,    coupler,   _xmodel_coupler),   
    _XARRAY_INFO(_XMODEL_LOAD,       load,      _xmodel_load),      
    _XARRAY_INFO(_XMODEL_GENERATOR,  generator, _xmodel_generator), 
    _XARRAY_INFO(_XMODEL_SWINGBUS,   swingbus,  _xmodel_swingbus),  
    _XARRAY_INFO(_XMODEL_VOLTAGE,    voltage,   _xmodel_voltage),   
    _XARRAY_INFO(_XMODEL_BRANCH,     branch,    _xmodel_branch),    
    _XARRAY_INFO(_XMODEL_TIELINE,    tieline,   _xmodel_tieline),   
    _XARRAY_INFO(_XMODEL_VXFORMER,   vxformer,  _xmodel_vxformer),  
    _XARRAY_INFO(_XMODEL_SVC,        svc,       _xmodel_svc),       
    _XARRAY_INFO(_XMODEL_SHUNT,      shunt,     _xmodel_shunt),     
    _XARRAY_INFO(_XMODEL_RESIDUAL,   residual,  _xmodel_residual),  
    _XARRAY_INFO(_XMODEL_CAPSEGMENT, capsegment,_xmodel_capsegment),
    _XARRAY_INFO(_XMODEL_CAPACITOR,  capacitor, _xmodel_capacitor), 
    _XARRAY_INFO(_XMODEL_PXFORMER,   pxformer,  _xmodel_pxformer),  
    _XARRAY_INFO(_XMODEL_INTERFACE,  interface, _xmodel_interface), 
    _XARRAY_INFO(_XMODEL_ICOMPONENT, icomponent,_xmodel_icomponent),
    _XARRAY_INFO(_XMODEL_TAP,        tap,       _xmodel_tap),       
    _XARRAY_INFO(_XMODEL_MOTOR,      motor,     _xmodel_motor),     
    _XARRAY_INFO(_XMODEL_CASE,       case,      _xmodel_case), 
    _XARRAY_INFO(_XTOPOLOGY_TPBUS,   tpbus,     _xtopology_bus),
    _XARRAY_INFO(_XTOPOLOGY_TPGEN,   tpgen,     _xtopology_gen),
    _XARRAY_INFO(_XTOPOLOGY_TPBRANCH,tpbranch,  _xtopology_branch),
    _XARRAY_INFO(_XTOPOLOGY_TPISLAND,tpisland,  _xtopology_island)   
};
#define _XARRAY_INFO_LENGTH sizeof(_xarray_infos)/sizeof(_xarray_infos[0])

/*!
 * return the xidx of id for type model.
 */
int _xid2xidx(int id, int type){
    register int i,j,k;
    register char *p;
    struct _xhash *table = &(_xarray_infos[type].table);
    
    if (table->bucket){/*ok,using hash search*/
        i = _xlookup_hash(id, table);
        if (i) return i;
    }
    
    /* 
     * oops! got hash collision or no hash table creation,
     * use forloop to search, not fast!
     */
    i = _xarray_infos[type].size;
    j = *(int *)((char *)(_xarray_ptr)+_xarray_infos[type].offsetof_used);
    p = *(char **)((char *)(_xarray_ptr)+_xarray_infos[type].offsetof_ptr);
    for (k=0; k<j; k++,p+=i){ if (*(_xid *)p == id) return k+1;}
    return 0;
}

int _xarray_insert_hash(int hash, int index, int type){
    struct _xhash *table = &(_xarray_infos[type].table);
    if (!table->bucket)
        return 0;
        
    return _xinsert_hash(hash,index,table);
}

/*!
 * the global array pointer, it is used internal only!!
 */
_xarray *_xarray_ptr = NULL;

/*!
 * dump the array information, can be used for low level debug
 */
void _xdump_array(FILE *fp){
    register int i,j,k;
    register char *p;
    
    if (!_xarray_ptr){
        (void) fprintf(fp,"\n#####ARRAY[0x00000000] : [0x00000000](0/0)\nEND#####ARRAY\n");
        return;
    }
    (void) fprintf(fp,"\n#####ARRAY[0x%08lx] : [0x%08lx](%d/%d)\n",
                  (long)(_xarray_ptr),
                  (long)(_xarray_ptr->bucket),
                  (_xarray_ptr->used),
                  (_xarray_ptr->length));
    for (i=1; i<_XARRAY_INFO_LENGTH; i++){
        _xarray_info *pinfo = &(_xarray_infos[i]);
        j = *(int *)((char *)_xarray_ptr+pinfo->offsetof_used);
        k = *(int *)((char *)_xarray_ptr+pinfo->offsetof_capacity);
        p = *(char **)((char *)(_xarray_ptr)+pinfo->offsetof_ptr);
        (void) fprintf(fp,"\t##%d##%s : [0x%08lx](%d/%d)\n",pinfo->id, pinfo->name,(long)p,j,k);
        if (j && p && pinfo->dump){
            register int m;
            for (m=0; m<j; m++){
                pinfo->dump(fp, p);
                p += pinfo->size;
            }
        }
        (void) fprintf(fp,"\n");
    }
    (void) fprintf(fp,"END#####ARRAY\n");
}

/*!
 * allocate the global array with given length, the elements have not been
 * initialized 
 */
int _xarray_allocate(int length){
    int err = 0;
    
    _xmutex_lock(_xthread_runtime_mutex);
    do {
        if (_xarray_ptr){
            length = _xarray_ptr->length;
            break;
        }
        if (length <= 0) {
            length = -1;
            break;
        }
        if ((_xarray_ptr = (_xarray *)_xnew_a(sizeof(_xarray)+length*sizeof(char),char)) == NULL){
            err = _XE_MEM;
            break;
        }
        length += _XFLEX_ARRAY_SIZE;
        _xarray_ptr->length = length;
    } while (0);
    _xmutex_unlock(_xthread_runtime_mutex);
    
    if (err) _xerror(err,"_xarray_allocate"); 
    return (length);
}

/*!
 * deallocate the array
 */
void _xarray_deallocate(void){
    _xmutex_lock(_xthread_runtime_mutex);
    if (_xarray_ptr){
        int i;
        for (i=0; i<_XARRAY_INFO_LENGTH; i++){
            _xfree(_xarray_infos[i].table.bucket);
            _xinit_hash(&(_xarray_infos[i].table));
        }
        _xfree(_xarray_ptr);
        _xarray_ptr = NULL;
    }
    _xmutex_unlock(_xthread_runtime_mutex);
}

/*!
 * slice a capacity bucket for type, return actual capacity or
 * current capacity
 */
int _xarray_slice_bucket(int type, int capacity){
    _xmutex_lock(_xthread_runtime_mutex);
    do {
        register int *i; register char **p;
        if (!_xarray_ptr || capacity <= 0 || type <=0 || type >= _XARRAY_INFO_LENGTH){
            capacity = -1;
            break;
        }
        i = (int *)((char *)_xarray_ptr+_xarray_infos[type].offsetof_capacity);
        p = (char **)((char *)(_xarray_ptr)+_xarray_infos[type].offsetof_ptr);
        if (*i){
            capacity = *i;
            break;
        }
        if (_xarray_ptr->used + capacity > _xarray_ptr->length){
            capacity = -1;
            break;
        }
        *i = capacity;
        *p = _xarray_ptr->bucket + _xarray_ptr->used;
        _xarray_ptr->used += capacity;
        
        /* for section & node, we use hash table
         */
        if (type == _XMODEL_SECTION || type == _XMODEL_NODE){
            if (_xarray_infos[type].table.bucket == NULL){
                _xarray_infos[type].table.bucket = (struct _xhash_entry *)_xcalloc(sizeof(struct _xhash_entry), capacity);
                _xarray_infos[type].table.size   = capacity;
            }
        }
    } while (0);
    _xmutex_unlock(_xthread_runtime_mutex);
    
    return (capacity);
}
