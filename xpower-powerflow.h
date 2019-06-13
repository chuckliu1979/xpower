/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines Dispatcher Training Simulator (DTS)  
** power flow solution routines.
**
** Created by chuck-liu@qq.com on 01/11/2011
**
** MATPOWER is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published
** by the Free Software Foundation, either version 3 of the License,
** or (at your option) any later version.
**
** MATPOWER is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with MATPOWER. If not, see <http://www.gnu.org/licenses/>.
**
** Additional permission under GNU GPL version 3 section 7
**
** If you modify MATPOWER, or any covered work, to interface with
** other modules (such as MATLAB code and MEX-files) available in a
** MATLAB(R) or comparable environment containing parts covered
** under other licensing terms, the licensors of MATPOWER grant
** you additional permission to convey the resulting work.
**
** CASEFORMAT    Defines the MATPOWER case file format.
**    A MATPOWER case file is an M-file or MAT-file that defines or returns
**    a struct named mpc, referred to as a "MATPOWER case struct". The fields
**    of this struct are baseMVA, bus, gen, branch, and (optional) gencost. With
**    the exception of baseMVA, a scalar, each data variable is a matrix, where
**    a row corresponds to a single bus, branch, gen, etc. The format of the
**    data is similar to the PTI format described in
**        http://www.ee.washington.edu/research/pstca/formats/pti.txt
**    except where noted. An item marked with (+) indicates that it is included
**    in this data but is not part of the PTI format. An item marked with (-) is
**    one that is in the PTI format but is not included here. Those marked with
**    (2) were added for version 2 of the case file format. The columns for
**    each data matrix are given below.
** 
**    MATPOWER Case Version Information:
**    There are two versions of the MATPOWER case file format. The current
**    version of MATPOWER uses version 2 of the MATPOWER case format
**    internally, and includes a 'version' field with a value of '2' to make
**    the version explicit. Earlier versions of MATPOWER used the version 1
**    case format, which defined the data matrices as individual variables,
**    as opposed to fields of a struct. Case files in version 1 format with
**    OPF data also included an (unused) 'areas' variable. While the version 1
**    format has now been deprecated, it is still be handled automatically by
**    LOADCASE and SAVECASE which are able to load and save case files in both
**    version 1 and version 2 formats.
** 
**    See also IDX_BUS, IDX_BRCH, IDX_GEN, IDX_AREA and IDX_COST regarding
**    constants which can be used as named column indices for the data matrices.
**    Also described in the first three are additional results columns that
**    are added to the bus, branch and gen matrices by the power flow and OPF
**    solvers.
** 
**    The case struct also also allows for additional fields to be included.
**    The OPF is designed to recognize fields named A, l, u, H, Cw, N,
**    fparm, z0, zl and zu as parameters used to directly extend the OPF
**    formulation (see OPF for details). Other user-defined fields may also
**    be included and will be automatically loaded by the LOADCASE function
**    and, given an appropriate 'savecase' callback function (see
**    ADD_USERFCN), saved by the SAVECASE function.
** 
**    Bus Data Format
**        1   bus number (positive integer)
**        2   bus type
**                PQ bus          = 1
**                PV bus          = 2
**                reference bus   = 3
**                isolated bus    = 4
**        3   Pd, real power demand (MW)
**        4   Qd, reactive power demand (MVAr)
**        5   Gs, shunt conductance (MW demanded at V = 1.0 p.u.)
**        6   Bs, shunt susceptance (MVAr injected at V = 1.0 p.u.)
**        7   area number, (positive integer)
**        8   Vm, voltage magnitude (p.u.)
**        9   Va, voltage angle (degrees)
**    (-)     (bus name)
**        10  baseKV, base voltage (kV)
**        11  zone, loss zone (positive integer)
**    (+) 12  maxVm, maximum voltage magnitude (p.u.)
**    (+) 13  minVm, minimum voltage magnitude (p.u.)
** 
**    Generator Data Format
**        1   bus number
**    (-)     (machine identifier, 0-9, A-Z)
**        2   Pg, real power output (MW)
**        3   Qg, reactive power output (MVAr)
**        4   Qmax, maximum reactive power output (MVAr)
**        5   Qmin, minimum reactive power output (MVAr)
**        6   Vg, voltage magnitude setpoint (p.u.)
**    (-)     (remote controlled bus index)
**        7   mBase, total MVA base of this machine, defaults to baseMVA
**    (-)     (machine impedance, p.u. on mBase)
**    (-)     (step up transformer impedance, p.u. on mBase)
**    (-)     (step up transformer off nominal turns ratio)
**        8   status,  >  0 - machine in service
**                     <= 0 - machine out of service
**    (-)     (% of total VAr's to come from this gen in order to hold V at
**                remote bus controlled by several generators)
**        9   Pmax, maximum real power output (MW)
**        10  Pmin, minimum real power output (MW)
**    (2) 11  Pc1, lower real power output of PQ capability curve (MW)
**    (2) 12  Pc2, upper real power output of PQ capability curve (MW)
**    (2) 13  Qc1min, minimum reactive power output at Pc1 (MVAr)
**    (2) 14  Qc1max, maximum reactive power output at Pc1 (MVAr)
**    (2) 15  Qc2min, minimum reactive power output at Pc2 (MVAr)
**    (2) 16  Qc2max, maximum reactive power output at Pc2 (MVAr)
**    (2) 17  ramp rate for load following/AGC (MW/min)
**    (2) 18  ramp rate for 10 minute reserves (MW)
**    (2) 19  ramp rate for 30 minute reserves (MW)
**    (2) 20  ramp rate for reactive power (2 sec timescale) (MVAr/min)
**    (2) 21  APF, area participation factor
** 
**    Branch Data Format
**        1   f, from bus number
**        2   t, to bus number
**    (-)     (circuit identifier)
**        3   r, resistance (p.u.)
**        4   x, reactance (p.u.)
**        5   b, total line charging susceptance (p.u.)
**        6   rateA, MVA rating A (long term rating)
**        7   rateB, MVA rating B (short term rating)
**        8   rateC, MVA rating C (emergency rating)
**        9   ratio, transformer off nominal turns ratio ( = 0 for lines )
**            (taps at 'from' bus, impedance at 'to' bus,
**             i.e. if r = x = 0, then ratio = Vf / Vt)
**        10  angle, transformer phase shift angle (degrees), positive => delay
**    (-)     (Gf, shunt conductance at from bus p.u.)
**    (-)     (Bf, shunt susceptance at from bus p.u.)
**    (-)     (Gt, shunt conductance at to bus p.u.)
**    (-)     (Bt, shunt susceptance at to bus p.u.)
**        11  initial branch status, 1 - in service, 0 - out of service
**    (2) 12  minimum angle difference, angle(Vf) - angle(Vt) (degrees)
**    (2) 13  maximum angle difference, angle(Vf) - angle(Vt) (degrees)
** 
**  (+) Generator Cost Data Format
**        NOTE: If gen has ng rows, then the first ng rows of gencost contain
**        the cost for active power produced by the corresponding generators.
**        If gencost has 2*ng rows then rows ng+1 to 2*ng contain the reactive
**        power costs in the same format.
**        1   model, 1 - piecewise linear, 2 - polynomial
**        2   startup, startup cost in US dollars
**        3   shutdown, shutdown cost in US dollars
**        4   N, number of cost coefficients to follow for polynomial
**            cost function, or number of data points for piecewise linear
**        5 and following, parameters defining total cost function f(p),
**            units of f and p are $/hr and MW (or MVAr), respectively.
**            (MODEL = 1) : p0, f0, p1, f1, ..., pn, fn
**                where p0 < p1 < ... < pn and the cost f(p) is defined by
**                the coordinates (p0,f0), (p1,f1), ..., (pn,fn) of the
**                end/break-points of the piecewise linear cost function
**            (MODEL = 2) : cn, ..., c1, c0
**                n+1 coefficients of an n-th order polynomial cost function,
**                starting with highest order, where cost is
**                f(p) = cn*p^n + ... + c1*p + c0
**  
**  (+) Area Data Format (deprecated)
**      (this data is not used by MATPOWER and is no longer necessary for
**       version 2 case files with OPF data).
**        1   i, area number
**        2   price_ref_bus, reference bus for that area
** 
**
*/

#ifndef _XPOWER_POWERFLOW_H_
#define _XPOWER_POWERFLOW_H_

#include <stdio.h>
#include "xpower-matrix.h"

#ifdef __cplusplus
extern "C" {
#endif

#define _XBUSTYPE_PQ   1
#define _XBUSTYPE_PV   2
#define _XBUSTYPE_REF  3
#define _XBUSTYPE_NONE 4

#define _XCOST_PW_LINEAR  1
#define _XCOST_POLYNOMIAL 2

#define _XPF_ALG_NEWTON 1 /* Newton-Raphson */
#define _XPF_ALG_FDPQXB 2 /* Fast-Decoupled (XB version) */
#define _XPF_ALG_FDPQBX 3 /* Fast-Decoupled (BX version) */
#define _XPF_ALG_GAUSS  4 /* Gauss-Seidel */

/* total 17
 * BUS_I,BUS_TYPE,PD,QD,GS,BS,BUS_AREA,VM,VA,BASE_KV,ZONE,VMAX,VMIN,
 * LAM_P,LAM_Q,MU_VMAX,MU_VMIN
 */
typedef struct __xpowerflow_bus {
    int bus_i;      /* bus number (positive integer) */
    int bus_type;   /* bus type (1 = PQ, 2 = PV, 3 = ref, 4 = isolated) */
    
    _xreal Pd;      /* real power demand (MW) */
    _xreal Qd;      /* reactive power demand (MVAr) */
    _xreal Gs;      /* shunt conductance (MW demanded at V = 1.0 p.u. ) */
    _xreal Bs;      /* shunt susceptance (MVAr injected at V = 1.0 p.u.) */
    
    int bus_area;   /* area number (positive integer) */
    
    _xreal Vm;      /* voltage magnitude (p.u.) */
    _xreal Va;      /* voltage angle (degree) */
    _xreal base_kV; /* base voltage (kV) */
    
    int zone;       /* loss zone (positive integer) */
    
    _xreal Vmax;    /* maximum voltage magnitude (p.u.) */
    _xreal Vmin;    /* minimum voltage magnitude (p.u.) */
    
    /* OPF */
    _xreal lam_P;   /* Lagrange multiplier on real power mismatch (u/MW) */
    _xreal lam_Q;   /* Lagrange multiplier on reactive power mismatch (u/MVAr) */
    _xreal mu_Vmax; /* Kuhn-Tucker multiplier on upper voltage limit (u/p.u.) */
    _xreal mu_Vmin; /* Kuhn-Tucker multiplier on lower voltage limit (u/p.u.) */
    
} _xpowerflow_bus;

/* total 25
 * GEN_BUS,PG,QG,QMAX,QMIN,VG,MBASE,GEN_STATUS,PMAX,PMIN,PC1,PC2,QC1MIN,QC1MAX,
 * QC2MIN,QC2MAX,RAMP_AGC,RAMP_10,RAMP_30,RAMP_Q,APF,
 * MU_PMAX,MU_PMIN,MU_QMAX,MU_QMIN
 */
typedef struct __xpowerflow_gen {
    int gen_bus;    /* bus number */
    
    _xreal Pg;      /* real power output (MW) */
    _xreal Qg;      /* reactive power output (MVAr) */
    _xreal Qmax;    /* maximum reactive power output (MVAr) */
    _xreal Qmin;    /* minimum reactive power output (MVAr) */
    _xreal Vg;      /* voltage magnitude setpoint (p.u.) */
    _xreal mBase;   /* total MVA base of this machine, defaults to baseMVA */
    
    int gen_status; /* >  0 - machine in service
                       <= 0 - machine out of service 
                     */
                     
    _xreal Pmax;    /* maximum real power output (MW) */
    _xreal Pmin;    /* minimum real power output (MW) */
    
    /* version 2 added */
    _xreal Pc1;     /* lower real power output of PQ capability curve (MW) */	
    _xreal Pc2;	    /* upper real power output of PQ capability curve (MW) */
    _xreal Qc1min;	/* minimum reactive power output at PC1 (MVAr) */
    _xreal Qc1max;	/* maximum reactive power output at PC1 (MVAr) */
    _xreal Qc2min;	/* minimum reactive power output at PC2 (MVAr) */
    _xreal Qc2max;	/* maximum reactive power output at PC2 (MVAr) */
    _xreal ramp_agc;/* ramp rate for load following/AGC (MW/min) */	
    _xreal ramp_10;	/* ramp rate for 10 minute reserves (MW) */
    _xreal ramp_30;	/* ramp rate for 30 minute reserves (MW) */
    _xreal ramp_q;	/* ramp rate for reactive power (2 sec timescale) (MVAr/min) */
    _xreal apf;     /* area participation factor */
    
    /* OPF */
    _xreal mu_Pmax; /* Kuhn-Tucker multiplier on upper Pg limit (u/MW) */
    _xreal mu_Pmin; /* Kuhn-Tucker multiplier on lower Pg limit (u/MW) */
    _xreal mu_Qmax; /* Kuhn-Tucker multiplier on upper Qg limit (u/MVAr) */
    _xreal mu_Qmin; /* Kuhn-Tucker multiplier on lower Qg limit (u/MVAr) */
    
} _xpowerflow_gen;

/* total 21
 * F_BUS,T_BUS,BR_R,BR_X,BR_B,RATE_A,RATE_B,RATE_C,TAP,SHIFT,BR_STATUS,ANGMIN,ANGMAX,
 * PF,QF,PT,QT,
 * MU_SF,MU_ST,MU_ANGMIN,MU_ANGMAX
 */
typedef struct __xpowerflow_branch {
    int f_bus;     /* from bus number */
    int t_bus;     /* to bus number */
    
    _xreal br_r;   /* resistance (p.u.) */
    _xreal br_x;   /* reactance (p.u.) */
    _xreal br_b;   /* total line charging susceptance (p.u.) */
    
    _xreal rate_a;    /* MVA rating A (long term rating) */
    _xreal rate_b;    /* MVA rating B (short term rating) */
    _xreal rate_c;    /* MVA rating C (emergency rating) */
    
    _xreal tap;    /* transformer off nominal turns ratio
                     (taps at 'from' bus, impedance at 'to' bus, 
                     i.e. tap = Vf / Vt)
                    */
    _xreal shift;  /* transformer phase shift angle (degrees), positive => delay */
    
    int br_status; /* initial branch status, 1 - in service, 0 - out of service */
    
    /* version 2 added */
    _xreal ang_min;/* minimum angle difference, angf - angt (degrees) */
    _xreal ang_max;/* maximum angle difference, angf - angt (degrees) */
    
    /* OPF */
    _xreal Pf;       /* real power injected at "from" bus end (MW) */
    _xreal Qf;       /* reactive power injected at "from" bus end (MVAr) */
    _xreal Pt;       /* real power injected at "to" bus end (MW) */
    _xreal Qt;       /* reactive power injected at "to" bus end (MVAr) */
    _xreal mu_Sf;    /* Kuhn-Tucker multiplier on MVA limit at "from" bus (u/MVA) */
    _xreal mu_St;    /* Kuhn-Tucker multiplier on MVA limit at "to" bus (u/MVA) */
    _xreal mu_angmin;/* Kuhn-Tucker multiplier lower angle difference limit (u/degree) */
    _xreal mu_angmax;/* Kuhn-Tucker multiplier upper angle difference limit (u/degree) */
    
} _xpowerflow_branch;

/* total 2
 * AREA_I,PRICE_REF_BUS
 * deprecated 
 */
typedef struct __xpowerflow_areas {
	int area_i;        /* area number */
	int price_ref_bus; /* reference bus for that area */
	
} _xpowerflow_areas;

/* total 5
 * MODEL,STARTUP,SHUTDOWN,NCOST,COST
 *
 * If gen has ng rows, then the first ng rows of gencost contain
 * the costs for active power produced by the corresponding generators.
 *
 * If gencost has 2ng rows, then rows ng + 1 through 2ng contain the reactive
 * power costs in the same format.
 */
typedef struct __xpowerflow_gencost {
	int model;      /* 1 - piecewise linear, 2 - polynomial */
	int startup;    /* startup cost in US dollars */
	int shutdown;   /* shutdown cost in US dollars */
	int ncost;      /* number of cost coefficients to follow for polynomial
	                   cost function, or number of data points for piecewise linear
	                   and following, cost data defining total cost function
	                   For polynomial cost:
	                           c2, c1, c0
	                   where the polynomial is c0 + c1*P + c2*P^2
	                   For piecewise linear cost:
	                           x0, y0, x1, y1, x2, y2, ...
	                   where x0 < x1 < x2 < ... and the points (x0,y0), (x1,y1),
	                   (x2,y2), ... are the end- and break-points of the cost function.
	                 */
	_xreal *cost;   /* array for c or (x,y)*/
	
} _xpowerflow_gencost;

/* internal - external numbering info */
typedef struct __xpowerflow_order {
	int state;   /*'i' or 'e'*/
	_xivec *i2e; /*bus i2e[internal-1] = external*/

} _xpowerflow_order;

/* struct for convenience */
typedef struct __xpowerflow_xpc {
	int version;
	_xreal baseMVA;
	_xpowerflow_order order;

	_xmat *A;
	_xmat *N;

    #define _XPC_FIELD(name) struct __xpowerflow_xpc_##name { \
	                             unsigned int elen;           \
								 unsigned int ilen;           \
								 _xpowerflow_##name *bucket;  \
                             }name;
	_XPC_FIELD(bus)
	_XPC_FIELD(gen)
	_XPC_FIELD(gencost)
	_XPC_FIELD(branch)
	_XPC_FIELD(areas)

	/* for power flow result */
	int et;
	int success;

} _xpowerflow_xpc;

/* struct for convenience */
typedef struct __xzspmat {
	_xspmat *re;
	_xspmat *im;

} _xzspmat;
#define _xzspmat_sfree(A) (_xspmat_sfree((A)->re),_xspmat_sfree((A)->im))
_XEXPORT extern void _xzspmat_foutput(_xzspmat *zspmat, FILE *fd);

typedef struct __xoption {
	int i;
	int max_it;
	int alg;
	int qlim;
	double tol;

} _xoption;

_XEXPORT extern void _xpowerflow_init_xpc(_xpowerflow_xpc *xpc);
_XEXPORT extern void _xpowerflow_free_xpc(_xpowerflow_xpc *xpc);
_XEXPORT extern void _xpowerflow_dump_xpc(_xpowerflow_xpc *xpc, FILE *fd);

_XEXPORT extern int _xpowerflow_cdf2matp(const char *cdf_file, _xpowerflow_xpc *xpc);
_XEXPORT extern int _xpowerflow_loadcase(const char *casefile, _xpowerflow_xpc *xpc);
_XEXPORT extern int _xpowerflow_ext2int(_xpowerflow_xpc *xpc);
_XEXPORT extern int _xpowerflow_int2ext(_xpowerflow_xpc *xpc);
_XEXPORT extern int _xpowerflow_v0(_xpowerflow_xpc *xpc, _xzvec **v0);
_XEXPORT extern int _xpowerflow_bustypes(_xpowerflow_xpc *xpc, int *ref, _xivec **pv, _xivec **pq);
_XEXPORT extern int _xpowerflow_make_b(_xpowerflow_xpc *xpc, int nargout, int alg, _xspmat **bp, _xspmat **bpp);
_XEXPORT extern int _xpowerflow_make_sbus(_xpowerflow_xpc *xpc, _xzvec **sbus);
_XEXPORT extern int _xpowerflow_make_ybus(_xpowerflow_xpc *xpc, int nargout, _xzspmat *ybus, _xzspmat *yf, _xzspmat *yt);
_XEXPORT extern int _xpowerflow_dsbus_dv(_xzvec *v, _xzspmat *ybus, _xzspmat *dvm, _xzspmat *dva);
_XEXPORT extern int _xpowerflow_newtonpf(_xzspmat *ybus, _xzvec *sbus, _xzvec *v0, _xivec *pv, _xivec *pq, _xoption *opt,_xzvec **vout);
_XEXPORT extern int _xpowerflow_gausspf(_xzspmat *ybus, _xzvec *sbus, _xzvec *v0, _xivec *pv, _xivec *pq, _xoption *opt,_xzvec **vout);
_XEXPORT extern int _xpowerflow_fdpf(_xzspmat *ybus, _xzvec *sbus, _xzvec *v0, _xspmat *bp, _xspmat *bpp, _xivec *pv, _xivec *pq, _xoption *opt,_xzvec **vout);
_XEXPORT extern int _xpowerflow_runpf(_xpowerflow_xpc *xpc, _xoption *opt);

_XEXPORT extern int _xpowerflow_printpf(_xpowerflow_xpc *xpc, FILE *fd);
_XEXPORT extern int _xpowerflow_make_jac(_xpowerflow_xpc *xpc, _xzspmat *ybus, _xzspmat *yf, _xzspmat *yt, _xspmat **jac);

_XEXPORT extern _xzvec *_xpowerflow_loss(_xpowerflow_xpc *xpc, _xzvec *loss);

#ifdef __cplusplus
}
#endif

#endif/*_XPOWER_POWERFLOW_H_*/

