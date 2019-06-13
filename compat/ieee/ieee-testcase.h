/*
** Copyright (C) 2011 XXX all rights reserved.
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/
 
#ifndef _XIEEE_TESTCASE_H_
#define _XIEEE_TESTCASE_H_

#include <stdio.h>
#include "../../xpower-decl.h"
#include "../../xpower-matrix.h"

#ifndef _XEXPORT
#define _XEXPORT
#endif

/*!
 * The complete description in the paper "Common Data Format for the Exchange
 * of Solved Load Flow Data", Working Group on a Common Format for the Exchange
 * of Solved Load Flow Data,_IEEE Transactions on Power Apparatus and Systems_,
 * Vol. PAS-92, No. 6, November/December 1973, pp. 1916-1925.
 *
 * Resources of Power Systems Test Case Archive can be found at website:
 * http://www.ee.washington.edu/research/pstca/
 *
 * But the partial description of the Common Data Format at that website seems
 * incorrect somewhere, such as bus name, maximum line length, etc.
 *
 * My implementation assume that :
 * 1. section Title, Bus, Branch are mandatory sections, others are optional.
 * 2. sequnce number in header is the item counts, and [129-132] do not parse
 * 3. others conform to the paper, not partial description
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/*! Title Data - Mandatory */
typedef struct __xieee_title { /* title data */
    char date[8];       /* columns   2-  9  date, in format DD/MM/YY with
                         * leading zeros. if no date provided, use 0b/0b/0b
                         * where b is blank
                         */
    char sender[20];    /* columns  11- 30  sender's name */
    float base_mva;     /* columns  32- 37  MVA base, mandatory */
    int year;           /* columns  39- 42  year */
    int season;         /* column   44      season (S - Summer, W - Winter) */
    char caseid[32];    /* columns  46- 73  case identification */ 
    
} _xieee_title;

/*! Bus Data - Mandatory
 *  HEADER("BUS DATA FOLLOWS")
 *  DELIMITER(-999)
 */
typedef struct __xieee_bus {
    int number;         /* columns   1-  4  bus number,from 1 to 9999 */
    char name[12];      /* columns   6- 17  bus name, left justified with a 
                         * maximun of 12 chars 
                         */
    int area;           /* columns  19- 20  area number from 1 to 99, don't
                         * use zero 
                         */
    int zone;           /* columns  21- 23  loss zone number from 1 to 999,
                         * optional 
                         */
    int type;           /* column   26      type 
                         * 0 - unregulated (load, PQ)
                         * 1 - hold MVAR generation within voltage limits (PQ)
                         * 2 - hold voltage within VAR limits (gen, PV)
                         * 3 - hold voltage and angle (swing, V-Theta must 
                         *     always have one)
                         */
    float voltage;      /* columns  28- 33  final voltage, p.u. */
    float angle;        /* columns  34- 40  final angle, degrees */
    float load_mw;      /* columns  41- 49  load MW */
    float load_mvar;    /* columns  50- 58  load MVAR */
    float gen_mw;       /* columns  59- 67  gen MW */
    float gen_mvar;     /* columns  68- 75  gen MVAR */
    float base_kv;      /* columns  77- 83  base KV,optional */
    float desired_volt; /* columns  85- 90  desired volts,p.u.(this is desired
                         * remote voltage is this bus is controlling another 
                         * bus)
                         */
    float max_mvar;     /* columns  91- 98  maximum MVAR or voltage limit */
    float min_mvar;     /* columns  99-106  minimum MVAR or voltage limit */
    float g;            /* columns 107-114  shunt conductance G, p.u. */
    float b;            /* columns 115-122  shunt susceptance B, p.u. */
    int cntrl_bus;      /* columns 124-127  remote controlled bus number */
    /*int seq;*/        /* columns 129-132  sequence number from 1 to 9999 */
    
} _xieee_bus;

/*! Branch Data - Mandatory
 *  HEADER("BRANCH DATA FOLLOWS")
 *  DELIMITER(-999)
 */
typedef struct __xieee_branch {
    int tapbus;         /* columns   1-  4  tap bus number, for transformers
                         * or phase shifters, the side of the model the 
                         * non-unity tap is on
                         */
    int zbus;           /* columns   6-  9  Z bus number, for transformers 
                         * and phase shifters, the side of the model the 
                         * device impedance is on
                         */
    int area;           /* columns  11- 12  line area from 1 to 99, optional*/
    int zone;           /* columns  13- 15  loss zone from 1 to 999,optional*/
    int circuit;        /* columns  17      circuit from 1 to 9, use 1 for 
                         * single line
                         */
    int type;           /* columns  19      type 
                         * 0 - transmission line
                         * 1 - fixed tap
                         * 2 - variable tap for voltage control (TCUL, LTC)
                         * 3 - variable tap (turn ratio) for MVAR control
                         * 4 - variable phase angle for MW control 
                         *     (phase shifter)
                         */
    float r;            /* columns  20- 29  branch resistance R, p.u. */
    float x;            /* columns  30- 39  branch reactance X, p.u. no zero
                         * impedance lines 
                         */
    float b;            /* columns  41- 49  line charging B, p.u. (total line
                         * charging, +B) 
                         */
    int mva_rating1;    /* columns  51- 55  line MVA rating No 1, optional */
    int mva_rating2;    /* columns  57- 61  line MVA rating No 2, optional */
    int mva_rating3;    /* columns  63- 67  line MVA rating No 3, optional */
    int cntrl_bus;      /* columns  69- 72  control bus number */
    int side;           /* column   74      side
                         * 0 - controlled bus is one of the terminals
                         * 1 - controlled bus is near the tap side
                         * 2 - controlled bus is near the impedance side(Z bus)
                         */
    float ratio;        /* columns  77- 82  transformer final turns ratio */
    float angle;        /* columns  84- 90  transformer (phase shifter)
                         * final angle
                         */
    float min_tap;      /* columns  91- 97  minimum tap or phase shift */
    float max_tap;      /* columns  98-104  maximum tap or phase shift */
    float step;         /* columns 106-111  step size */
    float min_volt;     /* columns 113-119  minimum voltage, MVAR or MW limit*/
    float max_volt;     /* columns 120-126  maximum voltage, MVAR or MW limit*/
    /*int seq;*/        /* columns 129-132  sequence number from 1 to 9999 */
         
} _xieee_branch;

/*! Loss Zone Data
 *  HEADER("LOSS ZONES FOLLOWS")
 *  DELIMITER(-99)
 */
typedef struct __xieee_zone {
    int number;         /* columns   1-  3  loss zone number */
    char name[12];      /* columns   5- 16  loss zone name */
    /*int seq;*/        /* columns 129-132  sequence number from 1 to 9999 */
        
} _xieee_zone;

/*! Interchange Data
 *  HEADER("INTERCHANGE DATA FOLLOWS")
 *  DELIMITER(-9)
 */
typedef struct __xieee_ichg {
    int area;           /* columns   1-  2  area number, no zeros! */
    int swing_number;   /* columns   4-  7  interchange swing bus number */
    char swing_name[12];/* columns   9- 20  alternate swing bus name,optional*/
    float mw_export;    /* columns  21- 28  area interchange export,MW(+=out)*/
    float mw_tolerance; /* columns  29- 35  area interchange tolerance, MW */
    char area_code[8];  /* columns  38- 43  area code (abbreviated name) */
    char area_name[32]; /* columns  46- 75  area name, optional */
    /*int seq;*/        /* columns 129-132  sequence number from 1 to 9999 */
        
} _xieee_ichg;

/*! Line Data
 *  HEADER("TIE LINES FOLLOW")
 *  DELIMITER(-999)
 */
typedef struct __xieee_line {
    int metered_bus;    /* columns    1-  4  metered bus number */
    int metered_area;   /* columns    7-  8  metered area number */
    int bus;            /* columns   11- 14  non-metered bus number */
    int area;           /* columns   17- 18  non-metered area number */
    int circuit;        /* columns   21      circuit number */
    /*int seq;*/        /* columns  129-132  sequence number from 1 to 9999 */
    
} _xieee_line;

_XEXPORT extern _xieee_title _xieee_cdf_title;

#define _XIEEE_VAR(name) _XEXPORT extern size_t _xieee_cdf_##name##_size; \
                         _XEXPORT extern _xieee_##name *_xieee_cdf_##name;                       
_XIEEE_VAR(bus)
_XIEEE_VAR(zone)
_XIEEE_VAR(ichg)
_XIEEE_VAR(line)
_XIEEE_VAR(branch)

/* I/O function */ 
_XEXPORT extern int _xieee_cdf_input(FILE *fd);
_XEXPORT extern int _xieee_cdf_output(FILE *fd);
_XEXPORT extern int _xieee_cdf_dump(FILE *fd);

/* free */
_XEXPORT extern void _xieee_cdf_free(void);

/* YBUS & BBUS */
_XEXPORT extern int _xieee_cdf_ybus(_xperm **pivot, _xspmat **g, _xspmat **b, int imag);
_XEXPORT extern int _xieee_cdf_bbus(_xperm **pivot, _xspmat **bp, _xspmat **bpp, int xb);

#ifdef __cplusplus
}
#endif

#endif/*_XIEEE_TESTCASE_H_*/
 
