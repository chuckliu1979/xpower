/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file defines Dispatcher Training Simulator (DTS)  
** models for system information.
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XPOWER_MODEL_H_
#define _XPOWER_MODEL_H_

#include "xpower-config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int _xid;
typedef int _xidx;

typedef struct __xlist {
    struct __xlist *prev; /*unused so far*/
    struct __xlist *next;
} _xlist;
#define _xlist_push(head,entry)   do{_xlist *tmp=(head); (head)=(entry); (entry)->next=tmp;}while(0)
#define _xlist_for_each(pos,head) for(pos = head; pos; pos = pos->next)

/* model macro */
#define _XMODEL_UNKNOWN    0  /* unknown */
#define _XMODEL_COMPANY    1  /* company */
#define _XMODEL_SUBSTATION 2  /* substation */
#define _XMODEL_LOADAREA   3  /* loadarea */
#define _XMODEL_SECTION    4  /* section */
#define _XMODEL_NODE       5  /* node */
#define _XMODEL_BREAKER    6  /* breaker */
#define _XMODEL_COUPLER    7  /* coupler */
#define _XMODEL_LOAD       8  /* load */
#define _XMODEL_GENERATOR  9  /* generator */
#define _XMODEL_SWINGBUS   10 /* swingbus */
#define _XMODEL_VOLTAGE    11 /* voltage */
#define _XMODEL_BRANCH     12 /* branch */
#define _XMODEL_TIELINE    13 /* tieline */
#define _XMODEL_VXFORMER   14 /* vxformer */ 
#define _XMODEL_SVC        15 /* svc */
#define _XMODEL_SHUNT      16 /* shunt */
#define _XMODEL_RESIDUAL   17 /* residual */
#define _XMODEL_CAPSEGMENT 18 /* capsegment */
#define _XMODEL_CAPACITOR  19 /* capacitor */
#define _XMODEL_PXFORMER   20 /* pxformer */
#define _XMODEL_INTERFACE  21 /* interface */
#define _XMODEL_ICOMPONENT 22 /* icomponent */
#define _XMODEL_TAP        23 /* tap */
#define _XMODEL_MOTOR      24 /* motor */
#define _XMODEL_CASE       25 /* case */
#define _XMODEL_END        26 /*should be the last */ 
#define _xmodel_check(c)   ((c)>_XMODEL_UNKNOWN && (c)<_XMODEL_END)

typedef struct __xmodel_company {/*_XMODEL_COMPANY*/
    _xid id;
  
    _xreal load_mw;        /* total mw load */                    
    _xreal load_mvar;      /* total mvar load */                  
    _xreal power_factor;   /* load power factor in percentage */                 
    _xreal gen_mw;         /* total mw generation */               
    _xreal gen_mvar;       /* total mvar generation */             
    _xreal charge_mvar;    /* line charging mvar */               
    _xreal shunt_mvar;     /* shunt device mvar */               
    _xreal ich_sched;      /* net mw schedule interchange */            
    _xreal ich_mw;         /* net mw interchange (positive - flow out) */
    _xreal ich_mvar;       /* net mvar interch.  (positive - flow out) */ 
    _xreal intern_ich;     /* mw interchange with internal company */
    _xreal loss_mw;        /* total mw losses */                  
    _xreal loss_mvar;      /* total mvar losses */                
    _xreal penalty;        /* company interchange loss penalty factor */

} _xmodel_company;

typedef struct __xmodel_substation {/*_XMODEL_SUBSTATION*/
    _xid id; 

    _xidx company;  /* company */
    _xidx loadarea; /* loadarea */
    int type;       /* substation type */
    
    _xreal load_mw;      /* total mw load */                     
    _xreal load_mvar;    /* total mvar load */                  
    _xreal power_factor; /* load power factor in percentage */                 
    _xreal gen_mw;       /* total mw generation */              
    _xreal gen_mvar;     /* total mvar generation */             
    _xreal shunt_mvar;   /* shunt device mvar */

} _xmodel_substation;

typedef struct __xmodel_loadarea {/*_XMODEL_LOADAREA*/
    _xid id;

    _xreal est_mw;  /* estimate load */
    _xreal des_mw;  /* desired */
    _xreal dts_mw;  /* dts */
    _xreal ble_mw;  /* ble load */
    _xreal meas_mw; /* measured */

    _xidx meas;
    struct __xmodel_loadarea_status {
        unsigned int updated    : 1; /* load should be updated */
        unsigned int in_service : 1; /* in service */
        unsigned int calculated : 1; /* calculated */
        unsigned int spares     : 29;
    } status;

} _xmodel_loadarea;

typedef struct __xmodel_section {/*_XMODEL_SECTION*/
    _xid id;
    
    _xidx company;     /* company */
    _xidx substation;  /* substation */
    _xlist *breakers;  /* breakers */
    _xlist *shunts;    /* shunts */
    _xlist *generators;/* generators */
    _xlist *nodes;     /* nodes */
    _xlist *loads;     /* loads */
    _xlist *scaps;     /* series capacitor */
    _xlist *voltages;  /* voltages */
    _xlist *svcs;      /* svcs */
    _xlist *couplers;  /* couplers */

    _xreal nominal;    /* nominal kv */
    _xreal desired;    /* desired kv */
    _xreal high;       /* high kv limit */
    _xreal low;        /* low kv limit */
    _xreal ehigh;      /* high kv emergency limit */
    _xreal elow;       /* low kv emergency limit */  

    int buses;         /* buses */
    struct __xmodel_section_status {
        unsigned int changed         : 1; /* breaker changed */
        unsigned int ca_monitored    : 1; /* ca monitored section */
        unsigned int zero_injection  : 1; /* zero injection */
        unsigned int zone            : 1; /* defined zone */
        unsigned int spares          : 28;
    } status; 

} _xmodel_section;

typedef struct __xmodel_node {/*_XMODEL_NODE*/
    _xid id;

    _xidx section;   /* voltage level(section)*/
    _xlist velement; /* section element */
    _xidx busbar;    /* busbar(_xmdl_voltage) index + 1 */            
    int assign;    /* assign #.*/
    _xidx tphook;    /* tpbus */

} _xmodel_node;

typedef struct __xmodel_breaker {/*_XMODEL_BREAKER*/
    _xid id;

    _xidx section;     /* voltage level(section)*/
    _xidx fnode;       /* from node */
    _xidx tnode;       /* to node */
    _xlist velement;   /* section element */

    _xidx meas;

    /* breaker types */
    #define _XBT_BREAKER            0
    #define _XBT_DISCONNECTOR       1
    #define _XBT_GROUNDDISCONNECTOR 2
    #define _XBT_COUPLER            3
    struct __xmodel_breaker_status {
        unsigned int close     : 1; /* current is "close" */
        unsigned int normal    : 1; /* normal is "close" */
        unsigned int telemeter : 1; /* telemetered */
        unsigned int coupler   : 1; /* bus coupler */
        unsigned int changed   : 1;
        unsigned int type      : 2; /* breaker, disconnector, ground disconnector */
        unsigned int spares    : 25;
    } status,old,display;

} _xmodel_breaker;

typedef struct __xmodel_coupler {/*_XMODEL_COUPLER*/
    _xid id;

    _xidx breaker;   /* breaker */
    _xlist velement; /* section element */

    _xidx meas;
    struct __xmodel_coupler_status {
        unsigned int close   : 1;
        unsigned int from_to : 1;
        unsigned int spares  : 30;
    } status;

    _xreal flow_mw;
    _xreal flow_mvar;

} _xmodel_coupler;

typedef struct __xmodel_load {/*_XMODEL_LOAD*/
    _xid id;

    _xidx loadarea;  /* loadarea */
    _xidx section;   /* voltage level(section)*/
    _xidx node;      /* node */
    _xlist velement; /* section element */

    struct __xmodel_load_status {
    #define _xmodel_load_clear_status(status) *((int *)&status) &= 0x00000000
        unsigned int changed    : 1;
        unsigned int tel_mw     : 1;
        unsigned int tel_mvar   : 1;
        unsigned int updated_mw : 1;
        unsigned int updated_mv : 1;
        unsigned int updated    : 1;
        unsigned int in_service : 1;
        unsigned int            : 1;
        unsigned int est_mw     : 1;
        unsigned int est_mv     : 1;
        unsigned int se_err_mw  : 1;
        unsigned int se_err_mv  : 1;
        unsigned int spares     : 20;
    } status,old;

    _xidx tphook;
    _xidx mw_meas;
    _xidx mvar_meas;

    _xreal meas_mw;
    _xreal meas_mvar;
    _xreal def_mw;
    _xreal def_mvar;
    _xreal est_mw;
    _xreal est_mvar;
    _xreal ble_mw;
    _xreal ble_mvar;
    _xreal high_mw;
    _xreal high_mvar;
    _xreal low_mw;
    _xreal low_mvar;

    _xreal g;
    _xreal b;

} _xmodel_load;

typedef struct __xmodel_generator {/*_XMODEL_GENERATOR*/
    _xid id;  

    _xidx section;   /* voltage level(section)*/
    _xidx node;      /* node */
    _xlist velement; /* section element */

    _xreal des_kv;
    _xreal est_kv;
    _xreal freq;
    _xreal penalty;

    _xreal def_mw;
    _xreal def_mvar;
    _xreal est_mw;
    _xreal est_mvar;
    _xreal eb_mw;
    _xreal eb_mvar;
    _xreal high_mw;
    _xreal high_mvar;
    _xreal low_mw;
    _xreal low_mvar;

    int rank;
    struct __xmodel_generator_status {
    #define _xmodel_generator_clear_status(status) *((int *)&status) &= 0x00ffffff
        unsigned int solved        : 1;
        unsigned int in_service    : 1;
        unsigned int changed       : 1;
        unsigned int saved_service : 1;
        unsigned int tel_mw        : 1;
        unsigned int tel_mvar      : 1;
        unsigned int               : 2;
        unsigned int d_curve       : 1;
        unsigned int v_regulated   : 1;
        unsigned int in_internal   : 1;
        unsigned int spares        : 21;
    } status,old;
    _xidx tphook;
    _xidx mw_meas;
    _xidx mvar_meas;

} _xmodel_generator;

typedef struct __xmodel_swingbus {/*_XMODEL_SWINGBUS*/
    _xid id;

    _xidx generator; /* generator */

    _xreal freq;
    _xreal angle;
    _xreal voltage;

    _xreal gen_mw;
    _xreal gen_mvar;
    _xreal load_mw;
    _xreal load_mvar;

    struct __xmodel_swingbus_status {
        unsigned int no_gen         : 1;
        unsigned int valid_net      : 1;
        unsigned int valid_solution : 1;
        unsigned int ext_area       : 1;
        unsigned int swing          : 1;
        unsigned int solved         : 1;
        unsigned int spares         : 26;
    } status;
  
} _xmodel_swingbus;

typedef struct __xmodel_voltage {/*_XMODEL_VOLTAGE*/
    _xid id;

    _xidx node;     /* node */
    _xidx section;  /* voltage level(section)*/
    _xlist velement;/* section element */

    _xreal freq;
    _xreal angle;
    _xreal voltage;
    _xreal pre_voltage;
    
    _xidx freq_meas;
    _xidx angle_meas;
    _xidx voltage_meas;

} _xmodel_voltage;

typedef struct __xmodel_branch {/*_XMODEL_BRANCH*/
    _xid id;

    _xidx fsection;  /* from section */
    _xidx tsection;  /* to section */
    _xidx fnode;     /* from node */
    _xidx tnode;     /* to node */
    _xidx atpbranch; /* a side tpbranch */
    _xidx btpbranch; /* b side tpbranch */
    _xidx xformer;   /* vxformer or pxformer */
    int island;
    int dms_island;
    
    _xreal r;         /* p.u. resistance */
    _xreal x;         /* p.u. reactance */
    _xreal g;         /* p.u. conductance */
    _xreal b;         /* p.u. susceptance */
    _xreal bc_a;      /* p.u. a side line charging supceptance */
    _xreal bc_b;      /* p.u. b side line charging supceptance */
    _xreal bg_a;      /* p.u. a side line charging admittance */
    _xreal bg_b;      /* p.u. b side line charging admittance */
    _xreal esta_mw;   /* a side estimate mw flow */          
    _xreal estb_mw;   /* b side estimate mw flow */          
    _xreal esta_mvar; /* a side side estimate mvar flow */         
    _xreal estb_mvar; /* b side estimate mvar flow */ 
    _xreal voltage;   /* base line voltage,not p.u.*/

    int ydn; /*transformer connection type */
    struct __xmodel_branch_status {
    #define _xmodel_branch_clear_status(status) *((int *)&status) &= 0xff000000
        unsigned int vxformer     : 1;
        unsigned int pxformer     : 1;
        unsigned int tie          : 1;
        unsigned int scap         : 1;
        unsigned int ca_monitored : 1;
        unsigned int              : 3;
        unsigned int single       : 1;
        unsigned int a_conn       : 1;
        unsigned int b_conn       : 1;
        unsigned int changed      : 1;
        unsigned int in_service   : 1;
        unsigned int est_flow     : 1;
        unsigned int zone         : 1;
        unsigned int island       : 1;
        unsigned int outage       : 1;
        unsigned int model        : 1;
        unsigned int se_solved    : 1;
        unsigned int lf_solved    : 1;
        unsigned int viol_anorm   : 1;
        unsigned int viol_alte    : 1;
        unsigned int viol_aste    : 1;
        unsigned int viol_bnorm   : 1;
        unsigned int viol_blte    : 1;
        unsigned int viol_bste    : 1;
        unsigned int ca_outage    : 1;
        unsigned int spares       : 5;
    } status,old;

} _xmodel_branch;

typedef struct __xmodel_tieline {/*_XMODEL_TIELINE*/
    _xid id;

    _xidx branch;   /* branch */
    _xidx acompany; /* company a */
    _xidx bcompany; /* company b */

    _xreal aest_mw;
    _xreal best_mw;
    _xreal aest_mvar;
    _xreal best_mvar;

    struct __xmodel_tieline_status {
        unsigned int in_service : 1;
        unsigned int limit      : 2; /*0=ampere 1=mva 2=mw*/
        unsigned int spares     : 29;
    } status;

} _xmodel_tieline;

typedef struct __xmodel_vxformer {/*_XMODEL_VXFORMER*/
    _xid id;

    _xidx substation;/* substation */
    _xlist *taps;    /* taps, present first */

    _xreal des_kv;
    _xreal est_kv;

    struct __xmodel_vxformer_status {
        unsigned int tap_b       : 1;
        unsigned int tap_on      : 1; 
        unsigned int tap_est     : 1;
        unsigned int tap_bad     : 1;
        unsigned int tap_bus     : 1;
        unsigned int in_service  : 1;
        unsigned int continuous  : 1;
        unsigned int v_regulated : 1;
        unsigned int spares      : 24;
    } status;

} _xmodel_vxformer;

typedef struct __xmodel_svc {/*_XMODEL_SVC*/
    _xid id;

    _xidx section;   /* voltage level(section)*/
    _xidx node;      /* node */
    _xlist velement; /* section element */

    _xreal def_mvar;
    _xreal est_mvar;
    _xreal dts_mvar;
    _xreal high_mvar;
    _xreal low_mvar;

    _xidx meas;
    _xidx tphook;
    struct __xmodel_svc_status {
    #define _xmodel_svc_clear_status(status) *((int *)&status) &= 0x00ffffff
        unsigned int in_service  : 1;
        unsigned int changed     : 1;
        unsigned int tel         : 1;
        unsigned int solved      : 1;
        unsigned int             : 4;
        unsigned int v_regulated : 1;
        unsigned int spares      : 23;
    } status,old; 

} _xmodel_svc;

typedef struct __xmodel_shunt {/*_XMODEL_SHUNT*/
    _xid id;

    _xidx section;   /* voltage level(section)*/
    _xidx node;      /* node */
    _xidx branch;    /* branch */
    _xidx breaker;   /* breaker*/
    _xlist velement; /* section element */

    _xidx meas;
    _xidx tphook;
    struct __xmodel_shunt_status {
        unsigned int changed     : 1;
        unsigned int in_service  : 1;
        unsigned int v_regulated : 1;
        unsigned int charge      : 1;
        unsigned int outage      : 1;
        unsigned int spares      : 27;
    } status,old;

    _xreal b;
    _xreal des_kv;
    _xreal est_kv;

} _xmodel_shunt; 

typedef struct __xmodel_residual {/*_XMODEL_RESIDUAL*/
    _xid id;

    _xreal load_mw;
    _xreal gen_mw;
    _xreal loss_mw;
    _xreal boundary_mw;
    _xreal residual_mw;

    int inj;

} _xmodel_residual; 
    
typedef struct __xmodel_capsegment {/*_XMODEL_CAPSEGMENT*/
    _xid id;

    _xidx fnode;      /* from node */
    _xidx tnode;      /* to node */
    _xlist celement;  /* capacitor element */

    _xreal x;
    struct __xmodel_capsegment_status {
        unsigned int changed :1;
        unsigned int outage  :1;
        unsigned int spares  :30;
    } status,old;

} _xmodel_capsegment;
 
typedef struct __xmodel_capacitor {/*_XMODEL_CAPACITOR*/
    _xid id;

    _xidx branch;            /* branch */
    _xidx snode;             /* section side node */
    _xidx lnode;             /* line side node */
    _xlist *capsegments;     /* capsegments */
    _xlist velement;         /* section element */

    struct __xmodel_capacitor_status {
        unsigned int changed : 1;
        unsigned int outage  : 1;
        unsigned int spares  : 30;
    } status,old;

} _xmodel_capacitor; 
 
typedef struct __xmodel_pxformer {/*_XMODEL_PXFORMER*/
    _xid id;

    _xidx substation;/* substation */
    _xlist *taps;    /* taps, present first */

    struct __xmodel_pxformer_status {
        unsigned int tap_b       : 1;
        unsigned int tap_on      : 1; 
        unsigned int tap_est     : 1;
        unsigned int tap_bad     : 1;
        unsigned int in_service  : 1;
        unsigned int continuous  : 1;
        unsigned int v_regulated : 1;
        unsigned int spares      : 25;
    } status;

} _xmodel_pxformer;
  
typedef struct __xmodel_interface {/*_XMODEL_INTERFACE*/
    _xid id;

    _xidx company; /* company */
    _xlist *icomponents;
    _xreal mw;
    _xreal mvar;
    _xreal mva;
    _xreal max;

} _xmodel_interface;
 
typedef struct __xmodel_icomponent {/*_XMODEL_ICOMPONENT*/
    _xid id;

    _xidx branch;    /* branch */
    _xidx interface; /* interface */
    _xlist ielement; /* interface element */

    _xreal est_mw;
    _xreal est_mvar;
    _xreal est_mva;

    struct __xmodel_icomponent_status {
        unsigned int out_pos   : 1;
        unsigned int from_meas : 1;
        unsigned int spares    : 30;
    } status;

} _xmodel_icomponent; 
  
typedef struct __xmodel_tap {/*_XMODEL_TAP*/
    _xid id;

    _xidx xformer;
    _xlist xelement;

    _xreal pos;
    _xreal value; /* voltage ratio or phrase angle */
    _xreal zfactor;

    _xidx meas;
    struct __xmodel_tap_status {
        unsigned int vxformer     : 1;
        unsigned int pxformer     : 1;
        unsigned int def_pos      : 1;
        unsigned int min_pos      : 1;
        unsigned int max_pos      : 1;
        unsigned int meas_pos     : 1;
        unsigned int spares       : 26;
    } status;

} _xmodel_tap;
    
typedef struct __xmodel_motor {/*_XMODEL_MOTOR*/
    _xid id;

    _xidx section; /* voltage level(section)*/
    _xidx node;    /* node */

    _xreal des_kv;
    _xreal est_kv;
    _xreal g;
    _xreal b;

    struct __xmodel_motor_status {
        unsigned int updated       : 1;
        unsigned int in_service    : 1;
        unsigned int changed       : 1;
        unsigned int updated_mw    : 1;
        unsigned int updated_mv    : 1;
        unsigned int tel_mw        : 1;
        unsigned int tel_mvr       : 1;
        unsigned int               : 1;
        unsigned int mw_est        : 1;
        unsigned int mv_est        : 1;
        unsigned int mw_se_err     : 1;
        unsigned int mv_se_err     : 1;
        unsigned int spares        : 20;
    } status;

} _xmodel_motor;
    
typedef struct __xmodel_case {/*_XMODEL_CASE*/
    _xid id;
    union {int encode; char decode[sizeof(int)];} pnp;
    int number;
} _xmodel_case; 

/* dump function */
_XEXPORT extern void _xdump_company(FILE *, void *);
_XEXPORT extern void _xdump_substation(FILE *, void *);
_XEXPORT extern void _xdump_loadarea(FILE *, void *);
_XEXPORT extern void _xdump_section(FILE *, void *);
_XEXPORT extern void _xdump_node(FILE *, void *);
_XEXPORT extern void _xdump_breaker(FILE *, void *);
_XEXPORT extern void _xdump_coupler(FILE *, void *);
_XEXPORT extern void _xdump_load(FILE *, void *);
_XEXPORT extern void _xdump_generator(FILE *, void *);
_XEXPORT extern void _xdump_swingbus(FILE *, void *);
_XEXPORT extern void _xdump_voltage(FILE *, void *);
_XEXPORT extern void _xdump_branch(FILE *, void *);
_XEXPORT extern void _xdump_tieline(FILE *, void *);
_XEXPORT extern void _xdump_vxformer(FILE *, void *);
_XEXPORT extern void _xdump_svc(FILE *, void *);
_XEXPORT extern void _xdump_shunt(FILE *, void *);
_XEXPORT extern void _xdump_residual(FILE *, void *);
_XEXPORT extern void _xdump_capsegment(FILE *, void *);
_XEXPORT extern void _xdump_capacitor(FILE *, void *);
_XEXPORT extern void _xdump_pxformer(FILE *, void *);
_XEXPORT extern void _xdump_interface(FILE *, void *);
_XEXPORT extern void _xdump_icomponent(FILE *, void *);
_XEXPORT extern void _xdump_tap(FILE *, void *);
_XEXPORT extern void _xdump_motor(FILE *, void *);
_XEXPORT extern void _xdump_case(FILE *, void *);

_XEXPORT extern int _xid2xidx(int, int);

#ifdef __cplusplus
}
#endif

#endif/*_XPOWER_MODEL_H_*/

