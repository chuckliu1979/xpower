/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines Dispatcher Training Simulator (DTS) 
** topology sturctures and routines
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XPOWER_TOPOLOGY_H_
#define _XPOWER_TOPOLOGY_H_

#include "xpower-model.h"

#ifdef __cplusplus
extern "C" {
#endif

#define _XTOPOLOGY_TPBUS    (_XMODEL_END + 0)
#define _XTOPOLOGY_TPGEN    (_XMODEL_END + 1)
#define _XTOPOLOGY_TPBRANCH (_XMODEL_END + 2)
#define _XTOPOLOGY_TPISLAND (_XMODEL_END + 3)
#define _XTOPOLOGY_END      (_XMODEL_END + 4)

typedef struct __xtopology_bus {
    int bus_i;      /* number (positive integer) */
    int island;     /* island (positive integer) */
    
    _xreal Pd;      /* real power demand (MW) */
    _xreal Qd;      /* reactive power demand (MVAr) */
    _xreal Gs;      /* shunt conductance (MW demanded at V = 1.0 p.u. ) */
    _xreal Bs;      /* shunt susceptance (MVAr injected at V = 1.0 p.u.) */

    _xreal Vm;      /* voltage magnitude (p.u.) */
    _xreal Va;      /* voltage angle (degree) */    
    _xreal base_kV; /* base voltage (kV) */
    
} _xtopology_bus;

typedef struct __xtopology_gen {
    int gen_bus;     /* bus number (positive integer) */
    int gen_status; /* >  0 - machine in service
                       <= 0 - machine out of service 
                     */    
    _xreal Pg;      /* real power output (MW) */
    _xreal Qg;      /* reactive power output (MVAr) */
    _xreal Qmax;    /* maximum reactive power output (MVAr) */
    _xreal Qmin;    /* minimum reactive power output (MVAr) */
    _xreal Vg;      /* voltage magnitude setpoint (p.u.) */
    _xreal mBase;   /* total MVA base of this machine, defaults to baseMVA */
    
    _xreal factor;  /* actual accelerating power factor */
    _xreal freqFactor;
    _xreal freqTimeConstant;

} _xtopology_gen;

typedef struct __xtopology_branch {
	int model;
    int f_bus;     /* from bus number */
    int t_bus;     /* to bus number */
    
    int br_status; /* initial branch status, 1 - in service, 0 - out of service */
    
    _xreal br_r;   /* resistance (p.u.) */
    _xreal br_x;   /* reactance (p.u.) */
    _xreal br_b;   /* total line charging susceptance (p.u.) */
    
    _xreal tap;    /* transformer off nominal turns ratio
                     (taps at 'from' bus, impedance at 'to' bus, 
                     i.e. tap = Vf / Vt)
                    */
    _xreal shift;  /* transformer phase shift angle (degrees), positive => delay */
    
} _xtopology_branch;

typedef struct __xtopology_island {
    int isl;
    int nbus;
    int ngen;
    int nload;
    
    _xreal freq;
    _xreal preFreq;
    _xreal baseMVA;
    
    _xreal Pacc;           /* accelerating power (MW) */
    _xreal damping;
    _xreal momentum;
    _xreal ratedCapacity;  /* MVA */
  
} _xtopology_island;

_XEXPORT extern int _xtopology_cold;

_XEXPORT extern int _xtopology_update_section(void);
_XEXPORT extern int _xtopology_create_bus(int capacity);
_XEXPORT extern int _xtopology_create_branch(void);
_XEXPORT extern int _xtopology_create_island(void);
            
#ifdef __cplusplus
}
#endif

#endif/*_XPOWER_TOPOLOGY_H_*/
