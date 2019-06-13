/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines Dispatcher Training Simulator (DTS) topology sturctures
** and routines
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#include "xpower-array.h"
#include "xpower-topology.h"

/*
 * set breaker status to _XTTUE (changed) if breaker status
 * changed
 */
static inline void _xtopology_update_changed_breaker(void){
    int i;
    _xlist *address;
    _xmodel_section *section;
    _xmodel_breaker *breaker;

    for (i=0; i<_xused_of(section); i++){
        section = _xpointer_of(section,i);
        address = section->breakers;
        
        while (address){
            breaker = _xcontainer_of(address,_xmodel_breaker, velement);
            if (breaker->status.close != breaker->old.close){
                section->status.changed = _XTRUE;
                breaker->old = breaker->status;
            }
            address = address->next;
        }
    }
}

/*
 * equalize the topology bus number within a section
 */
static inline void _xtopology_equalize_section(_xmodel_section *section){
    int more;
    _xlist *address;
    
    do {
        more = _XFALSE; 
        address = section ? section->breakers : (_xlist *)NULL;
        	
        while (address){
            _xmodel_breaker *breaker = _xcontainer_of(address,_xmodel_breaker,velement);
            if (breaker->status.close){
                _xmodel_node *fnode = _xpointer_of(node,breaker->fnode - 1);
                _xmodel_node *tnode = _xpointer_of(node,breaker->tnode - 1);
                if (!fnode || !tnode)
                    _xerror(_XE_NULL,"_xtopology_equalize_section");
                if (tnode->assign != fnode->assign){
                    tnode->assign = _xmin(tnode->assign,fnode->assign);
                    fnode->assign = tnode->assign;
                    more = _XTRUE;
                }
            }
            address = address->next;
        }
        
    } while (more);
}

/*
 * assign the breaker's from-to nodes topology bus number 
 */
static inline void _xtopology_assign_breaker_node(_xmodel_breaker *breaker, int *bus){
    _xmodel_node *fnode = _xpointer_of(node,breaker->fnode - 1);
    _xmodel_node *tnode = _xpointer_of(node,breaker->tnode - 1);
    
    if (!fnode || !tnode)
        _xerror(_XE_NULL,"_xtopology_assign_node"); 
    else if (fnode->assign)
        tnode->assign = fnode->assign;
    else if (tnode->assign)
        fnode->assign = tnode->assign;
    else {
        fnode->assign = ++(*bus);
        tnode->assign = *bus;
    }
}

int _xtopology_cold = _XTRUE;

/*!
 * initialize section's topology bus information, return bus count
 */
int _xtopology_update_section(void){
    int i,j,bus;
    _xlist *address;
    _xmodel_node *node;
    _xmodel_breaker *breaker;
    _xmodel_section *section;
    
    _xtopology_update_changed_breaker();
    for (i=0; i<_xused_of(section); i++){
        section = _xpointer_of(section,i);
        if (section->nodes && (section->status.changed || _xtopology_cold)){
            bus = 0;
            section->status.changed = _XFALSE;
            _xlist_for_each(address,section->nodes){
                node = _xcontainer_of(address,_xmodel_node,velement);
                node->assign = 0;
            }
            _xlist_for_each(address,section->breakers){
                breaker = _xcontainer_of(address,_xmodel_breaker,velement);
                if (breaker->status.close)
                    _xtopology_assign_breaker_node(breaker,&bus);
            }
            _xtopology_equalize_section(section);
            
            bus = 0;
            _xlist_for_each(address,section->nodes){
                node = _xcontainer_of(address,_xmodel_node,velement);
                bus = _xmax(bus,node->assign);
            }
            _xlist_for_each(address,section->nodes){
                node = _xcontainer_of(address,_xmodel_node,velement);
                if (node->assign == 0)
                    node->assign = ++bus;
            }
            section->buses = bus;
        }
    }/* for loop */
    
    bus = 0; /* max buses */
    for (i=0; i<_xused_of(section); i++){
        section = _xpointer_of(section,i);
        _xlist_for_each(address,section->nodes){
            node = _xcontainer_of(address,_xmodel_node,velement);
            if (node->assign > 0){
                _xlist *atmp;_xmodel_node *ntmp;
                
                bus++;
                j = node->assign;
                _xlist_for_each(atmp,address){
                    ntmp = _xcontainer_of(atmp,_xmodel_node,velement);
                    if (ntmp->assign == j)
                        ntmp->assign = -bus;
                }
            }
        }
        _xlist_for_each(address,section->nodes){
            node = _xcontainer_of(address,_xmodel_node,velement);
            node->assign = abs(node->assign);
        }
    }
    
    for (i=0; i<_xused_of(section); i++){/* non-busbar node */
        section = _xpointer_of(section,i);
        _xlist_for_each(address,section->nodes){
            node = _xcontainer_of(address,_xmodel_node,velement);
            if (node->busbar <= 0){
                _xlist *atmp;_xmodel_node *ntmp;
                _xlist_for_each(atmp,section->nodes){
                    ntmp = _xcontainer_of(atmp,_xmodel_node,velement);
                    if (ntmp->assign == node->assign && ntmp->busbar){
                        node->busbar = -abs(ntmp->busbar);
                        break;
                    }
                }
            }
        }
    }
    
    return bus;
}

/*!
 * create topology bus used for power flow calculation
 */
int _xtopology_create_bus(int capacity){
    int i,*flag = NULL;
    _xlist *address;
    _xmodel_section *section;
    _xmodel_node *fnode,*tnode;
    
    if (capacity <= 0 || capacity > _xcapacity_of(tpbus))
        return _XFALSE;

    for (i=0; i<_xused_of(section); i++){
        section = _xpointer_of(section,i);
        address = section->shunts;
        while (address){/* process shunt */
            _xmodel_shunt *shunt = _xcontainer_of(address,_xmodel_shunt,velement);
            shunt->status.outage = _XFALSE;
            if (shunt->breaker){
                _xmodel_breaker *breaker = _xpointer_of(breaker,shunt->breaker - 1);
                fnode = _xpointer_of(node,breaker->fnode - 1);
                tnode = _xpointer_of(node,breaker->tnode - 1);
                if (fnode->assign != tnode->assign){
                    shunt->status.outage = _XTRUE;
                    if (breaker->fnode == shunt->node)
                        fnode->assign = tnode->assign;
                    else
                        tnode->assign = fnode->assign;
                }
            }
            address = address->next;
        }/* shunt while loop */
    }
    
    for (i=0; i<_xused_of(capacitor); i++){/* process capacitor */
        _xmodel_capacitor *capacitor = _xpointer_of(capacitor,i);
        capacitor->status.outage = _XTRUE;
        if (1){/* for local variables */
            _xmodel_capsegment *capsegment = NULL;
            _xmodel_node *snode = _xpointer_of(node,capacitor->snode - 1);
            _xmodel_node *lnode = _xpointer_of(node,capacitor->lnode - 1);
            
            if (lnode->assign != snode->assign){
                int lconn,sconn;
                lconn = sconn = _XFALSE;
                address = capacitor->capsegments;
                while (address){
                    capsegment = _xcontainer_of(address,_xmodel_capsegment,celement);
                    fnode = _xpointer_of(node,capsegment->fnode - 1);
                    tnode = _xpointer_of(node,capsegment->tnode - 1);
                    if (fnode->assign == lnode->assign ||
                        tnode->assign == lnode->assign)
                        lconn = _XTRUE;
                    if (fnode->assign == snode->assign ||
                        tnode->assign == snode->assign)
                        sconn = _XTRUE;
                    if (lconn && sconn){
                        capacitor->status.outage = _XFALSE;
                        break;
                    }
                    address = address->next;
                }/*while loop*/
            }
            
            if (!capsegment && !capacitor->status.outage)
                lnode->assign = snode->assign;
                
            address = capacitor->capsegments;
            while (address){
                capsegment = _xcontainer_of(address,_xmodel_capsegment,celement);
                fnode = _xpointer_of(node,capsegment->fnode - 1);
                tnode = _xpointer_of(node,capsegment->tnode - 1);
                capsegment->status.outage = _XTRUE;
                if (!capacitor->status.outage){
                    if (fnode->assign != tnode->assign)
                        capsegment->status.outage = _XFALSE;
                    fnode->assign = snode->assign;
                    tnode->assign = snode->assign;
                }
                address = address->next;
            }/*while loop*/
        }
    }
    
    /* create bus */
    _xused_of(tpbus) = 0;
    _xmem_zero(_xpointer_of(tpbus,0),_xcapacity_of(tpbus)*sizeof(_xtopology_bus));
    flag = _xnew_a(capacity + 1, int);
    for (i=0; i<_xused_of(section); i++){/*node*/
        section = _xpointer_of(section,i);
        if (section->nodes){
            address = section->nodes;
            while (address){
                fnode = _xcontainer_of(address,_xmodel_node,velement);
                if (!flag[fnode->assign]){/* populate topology bus*/
                    _xtopology_bus *pbus = _xpointer_of(tpbus,_xused_of(tpbus));
                    flag[fnode->assign]  = ++(_xused_of(tpbus));/*index + 1*/
                    
                    pbus->base_kV = section->nominal;
                    pbus->bus_i   = fnode->assign;

                    if (_xtopology_cold || fnode->busbar <= 0){
                        pbus->Va = (_xreal)0.0;
                        pbus->Vm = (_xreal)1.05;
                    }
                    else {
                        _xmodel_voltage *pvolt = _xpointer_of(voltage,abs(fnode->busbar)-1);
                        pbus->Va = pvolt->angle;
                        pbus->Vm = pvolt->voltage;
                    }
                }
                fnode->tphook = flag[fnode->assign];
                address = address->next;
            }/* while loop*/
        }
    }/* for loop*/
    _xfree(flag);flag = NULL;
    
    return _XTRUE;
}

int _xtopology_create_branch(void){
    int i;
    _xmodel_branch *branch;
    _xmodel_node *fnode,*tnode;
	_xtopology_bus *fbus,*tbus;
	_xtopology_branch *tpbranch;

	for (i=0; i<_xused_of(branch); i++){
		branch = _xpointer_of(branch,i);
		fnode = _xpointer_of(node,branch->fnode-1);
		tnode = _xpointer_of(node,branch->tnode-1);
		if (!fnode || !tnode)
			return _XFALSE;

		fbus = _xpointer_of(tpbus,fnode->tphook-1);
		tbus = _xpointer_of(tpbus,tnode->tphook-1);
		if (!fbus || !tbus)
			return _XFALSE;

		tpbranch = _xpointer_of(tpbranch,_xused_of(tpbranch));
		tpbranch->model = i+1;
		tpbranch->f_bus = fbus->bus_i;
		tpbranch->t_bus = tbus->bus_i;
		tpbranch->br_r  = branch->r;
		tpbranch->br_x  = branch->x;
		tpbranch->br_b  = branch->bc_a + branch->bc_b;
		tpbranch->br_status = branch->status.in_service;

		_xused_of(tpbranch)++;
	}

	return _XTRUE;
}

int _xtopology_create_island(void){
	return _XFALSE;
}
