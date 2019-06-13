/*
** Copyright (C) 2011 XXX all rights reserved.
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#include <assert.h>
#include <stdlib.h>

#if 0
#include "ems/measurementtype.h"
#include "rtdbblock/appblock/appblockall.h"
#include "rtdbblock/scadablock/scadablockall.h"
#else
#include "xcsgc3000-test.h"
#endif

/* boolean values */
#ifndef _XTRUE
#define _XTRUE  1
#endif

#ifndef _XFALSE
#define _XFALSE 0
#endif

/*
 * make sure we have the same struct size as the block defined.
 * This only guarantee the struct size, not members
 */
template <bool> struct _xcompile_assert_struct{};
#define _xcompile_assert(expr,msg)                       \
typedef _xcompile_assert_struct<static_cast<bool>(expr)> \
        msg[static_cast<bool>(expr) ? 1 : -1]
/*
 * function proto
 */
#define _XTRICKY_DEF
#include "xcsgc3000.h"

static int __xcsgc3000_database = 0;
int _xcsgc3000_database(int *val){
    int tmp = __xcsgc3000_database;
    if (val) __xcsgc3000_database = *val;
    return tmp;
}

/*
 * company
 */
static RTDBCCompany *_xcompany_pointer = NULL;
static inline RTDBCCompany *_xcompany(void){
    _xcompile_assert(COMPANY_ID == 600,id_not_match);
    return _xcompany_pointer ? _xcompany_pointer :
           _xcompany_pointer = new RTDBCCompany(__xcsgc3000_database);
}

int _xcompany_ok(void){
    return _xcompany()->CanUse() && 
           _xcompany()->m_ppara  &&
           _xcompany()->m_preal  &&
           _xcompany()->m_pestm  &&
           _xcompany()->m_pcal   &&
           _xcompany()->m_panaly;
}

int _xcompany_size(void){
    return _xcompany()->GetCurRecNum();
}

int _xcompany_head(void){
    return _xcompany()->GetFirstRecIndex();
}

int _xcompany_next(void){
    return _xcompany()->GetNextRecIndex();
}

int _xcompany_valid_index(int index){
    return _xcompany()->ValidIndex(index) ? _XTRUE : _XFALSE;
}

void _xcompany_free(void){
    delete _xcompany_pointer;
    _xcompany_pointer = NULL;
}

struct _xcompany_para *_xcompany_para_pointer(void){
    _xcompile_assert(sizeof(struct _xcompany_para) == 
                     sizeof(COMPANY_PARA),struct_not_match);
    return (struct _xcompany_para *)(_xcompany()->m_ppara);
}

struct _xcompany_real *_xcompany_real_pointer(void){
    _xcompile_assert(sizeof(struct _xcompany_real) == 
                     sizeof(COMPANY_REAL),struct_not_match);
    return (struct _xcompany_real *)(_xcompany()->m_preal);
}

struct _xcompany_estm *_xcompany_estm_pointer(void){
    _xcompile_assert(sizeof(struct _xcompany_estm) == 
                     sizeof(COMPANY_ESTM),struct_not_match);
    return (struct _xcompany_estm *)(_xcompany()->m_pestm);
}

struct _xcompany_calc *_xcompany_calc_pointer(void){
    _xcompile_assert(sizeof(struct _xcompany_calc) == 
                     sizeof(COMPANY_CAL),struct_not_match);
    return (struct _xcompany_calc *)(_xcompany()->m_pcal);
}

struct _xcompany_anly *_xcompany_anly_pointer(void){
    _xcompile_assert(sizeof(struct _xcompany_anly) == 
                     sizeof(COMPANY_ANALY),struct_not_match);
    return (struct _xcompany_anly *)(_xcompany()->m_panaly);
}

/*
 * loadarea
 */
static RTDBCLoadArea *_xloadarea_pointer = NULL;
static inline RTDBCLoadArea *_xloadarea(void){
    _xcompile_assert(LOADAREA_ID == 601,id_not_match);
    return _xloadarea_pointer ? _xloadarea_pointer :
           _xloadarea_pointer = new RTDBCLoadArea(__xcsgc3000_database);
}

int _xloadarea_ok(void){
    return _xloadarea()->CanUse() && 
           _xloadarea()->m_ppara  &&
           _xloadarea()->m_preal  &&
           _xloadarea()->m_pestm  &&
           _xloadarea()->m_pcal   &&
           _xloadarea()->m_panaly;
}

int _xloadarea_size(void){
    return _xloadarea()->GetCurRecNum();
}

int _xloadarea_head(void){
    return _xloadarea()->GetFirstRecIndex();
}

int _xloadarea_next(void){
    return _xloadarea()->GetNextRecIndex();
}

int _xloadarea_valid_index(int index){
    return _xloadarea()->ValidIndex(index) ? _XTRUE : _XFALSE;
}

void _xloadarea_free(void){
    delete _xloadarea_pointer;
    _xloadarea_pointer = NULL;
}

struct _xloadarea_para *_xloadarea_para_pointer(void){
    _xcompile_assert(sizeof(struct _xloadarea_para) == 
                     sizeof(LOADAREA_PARA),struct_not_match);
    return (struct _xloadarea_para *)(_xloadarea()->m_ppara);
}

struct _xloadarea_real *_xloadarea_real_pointer(void){
    _xcompile_assert(sizeof(struct _xloadarea_real) == 
                     sizeof(LOADAREA_REAL),struct_not_match);
    return (struct _xloadarea_real *)(_xloadarea()->m_preal);
}

struct _xloadarea_estm *_xloadarea_estm_pointer(void){
    _xcompile_assert(sizeof(struct _xloadarea_estm) == 
                     sizeof(LOADAREA_ESTM),struct_not_match);
    return (struct _xloadarea_estm *)(_xloadarea()->m_pestm);
}

struct _xloadarea_calc *_xloadarea_calc_pointer(void){
    _xcompile_assert(sizeof(struct _xloadarea_calc) == 
                     sizeof(LOADAREA_CAL),struct_not_match);
    return (struct _xloadarea_calc *)(_xloadarea()->m_pcal);
}

struct _xloadarea_anly *_xloadarea_anly_pointer(void){
    _xcompile_assert(sizeof(struct _xloadarea_anly) == 
                     sizeof(LOADAREA_ANALY),struct_not_match);
    return (struct _xloadarea_anly *)(_xloadarea()->m_panaly);
}

/*
 * substation
 */
static RTDBCSubstation *_xsubstation_pointer = NULL;
static inline RTDBCSubstation *_xsubstation(void){
    _xcompile_assert(SUBSTATION_ID == 602,id_not_match);
    return _xsubstation_pointer ? _xsubstation_pointer :
           _xsubstation_pointer = new RTDBCSubstation(__xcsgc3000_database);
}

int _xsubstation_ok(void){
    return _xsubstation()->CanUse() && 
           _xsubstation()->m_ppara  &&
           _xsubstation()->m_preal  &&
           _xsubstation()->m_pestm  &&
           _xsubstation()->m_pcal   &&
           _xsubstation()->m_panaly &&
           _xsubstation()->m_pflag;
}

int _xsubstation_size(void){
    return _xsubstation()->GetCurRecNum();
}

int _xsubstation_head(void){
    return _xsubstation()->GetFirstRecIndex();
}

int _xsubstation_next(void){
    return _xsubstation()->GetNextRecIndex();
}

int _xsubstation_valid_index(int index){
    return _xsubstation()->ValidIndex(index) ? _XTRUE : _XFALSE;
}

void _xsubstation_free(void){
    delete _xsubstation_pointer;
    _xsubstation_pointer = NULL;
}

struct _xsubstation_para *_xsubstation_para_pointer(void){
    _xcompile_assert(sizeof(struct _xsubstation_para) == 
                     sizeof(SUBSTATION_PARA),struct_not_match);
    return (struct _xsubstation_para *)(_xsubstation()->m_ppara);
}

struct _xsubstation_real *_xsubstation_real_pointer(void){
    _xcompile_assert(sizeof(struct _xsubstation_real) == 
                     sizeof(SUBSTATION_REAL),struct_not_match);
    return (struct _xsubstation_real *)(_xsubstation()->m_preal);
}

struct _xsubstation_estm *_xsubstation_estm_pointer(void){
    _xcompile_assert(sizeof(struct _xsubstation_estm) == 
                     sizeof(SUBSTATION_ESTM),struct_not_match);
    return (struct _xsubstation_estm *)(_xsubstation()->m_pestm);
}

struct _xsubstation_calc *_xsubstation_calc_pointer(void){
    _xcompile_assert(sizeof(struct _xsubstation_calc) == 
                     sizeof(SUBSTATION_CAL),struct_not_match);
    return (struct _xsubstation_calc *)(_xsubstation()->m_pcal);
}

struct _xsubstation_anly *_xsubstation_anly_pointer(void){
    _xcompile_assert(sizeof(struct _xsubstation_anly) == 
                     sizeof(SUBSTATION_ANALY),struct_not_match);
    return (struct _xsubstation_anly *)(_xsubstation()->m_panaly);
}

struct _xsubstation_flag *_xsubstation_flag_pointer(void){
    _xcompile_assert(sizeof(struct _xsubstation_flag) == 
                     sizeof(SUBSTATION_FLAG),struct_not_match);
    return (struct _xsubstation_flag *)(_xsubstation()->m_pflag);
}

/*
 * voltagelevel
 */
static RTDBCVoltagelevel *_xvoltagelevel_pointer = NULL;
static inline RTDBCVoltagelevel *_xvoltagelevel(void){
    _xcompile_assert(VOLTAGELEVEL_ID == 604,id_not_match);
    return _xvoltagelevel_pointer ? _xvoltagelevel_pointer :
           _xvoltagelevel_pointer = new RTDBCVoltagelevel(__xcsgc3000_database);
}

int _xvoltagelevel_ok(void){
    return _xvoltagelevel()->CanUse() && 
           _xvoltagelevel()->m_ppara;
}

int _xvoltagelevel_size(void){
    return _xvoltagelevel()->GetCurRecNum();
}

int _xvoltagelevel_head(void){
    return _xvoltagelevel()->GetFirstRecIndex();
}

int _xvoltagelevel_next(void){
    return _xvoltagelevel()->GetNextRecIndex();
}

int _xvoltagelevel_valid_index(int index){
    return _xvoltagelevel()->ValidIndex(index) ? _XTRUE : _XFALSE;
}

void _xvoltagelevel_free(void){
    delete _xvoltagelevel_pointer;
    _xvoltagelevel_pointer = NULL;
}

struct _xvoltagelevel_para *_xvoltagelevel_para_pointer(void){
    _xcompile_assert(sizeof(struct _xvoltagelevel_para) == 
                     sizeof(VOLTAGELEVEL_PARA),struct_not_match);
    return (struct _xvoltagelevel_para *)(_xvoltagelevel()->m_ppara);
}

/*
 * powertransformer
 */
static RTDBCPowertransformer *_xpowertransformer_pointer = NULL;
static inline RTDBCPowertransformer *_xpowertransformer(void){
    _xcompile_assert(POWERTRANSFORMER_ID == 609,id_not_match);
    return _xpowertransformer_pointer ? _xpowertransformer_pointer :
           _xpowertransformer_pointer = new RTDBCPowertransformer(__xcsgc3000_database);
}

int _xpowertransformer_ok(void){
    return _xpowertransformer()->CanUse() && 
           _xpowertransformer()->m_ppara;
}

int _xpowertransformer_size(void){
    return _xpowertransformer()->GetCurRecNum();
}

int _xpowertransformer_head(void){
    return _xpowertransformer()->GetFirstRecIndex();
}

int _xpowertransformer_next(void){
    return _xpowertransformer()->GetNextRecIndex();
}

int _xpowertransformer_valid_index(int index){
    return _xpowertransformer()->ValidIndex(index) ? _XTRUE : _XFALSE;
}

void _xpowertransformer_free(void){
    delete _xpowertransformer_pointer;
    _xpowertransformer_pointer = NULL;
}

struct _xpowertransformer_para *_xpowertransformer_para_pointer(void){
    _xcompile_assert(sizeof(struct _xpowertransformer_para) == 
                     sizeof(POWERTRANSFORMER_PARA),struct_not_match);
    return (struct _xpowertransformer_para *)(_xpowertransformer()->m_ppara);
}

/*
 * bay
 */
static RTDBCBay *_xbay_pointer = NULL;
static inline RTDBCBay *_xbay(void){
    _xcompile_assert(BAY_ID == 607,id_not_match);
    return _xbay_pointer ? _xbay_pointer :
           _xbay_pointer = new RTDBCBay(__xcsgc3000_database);
}

int _xbay_ok(void){
    return _xbay()->CanUse() && 
           _xbay()->m_ppara;
}

int _xbay_size(void){
    return _xbay()->GetCurRecNum();
}

int _xbay_head(void){
    return _xbay()->GetFirstRecIndex();
}

int _xbay_next(void){
    return _xbay()->GetNextRecIndex();
}

int _xbay_valid_index(int index){
    return _xbay()->ValidIndex(index) ? _XTRUE : _XFALSE;
}

void _xbay_free(void){
    delete _xbay_pointer;
    _xbay_pointer = NULL;
}

struct _xbay_para *_xbay_para_pointer(void){
    _xcompile_assert(sizeof(struct _xbay_para) == 
                     sizeof(BAY_PARA),struct_not_match);
    return (struct _xbay_para *)(_xbay()->m_ppara);
}

/*
 * connectivitynode
 */
static RTDBCConnectivitynode *_xconnectivitynode_pointer = NULL;
static inline RTDBCConnectivitynode *_xconnectivitynode(void){
    _xcompile_assert(CONNECTIVITYNODE_ID == 605,id_not_match);
    return _xconnectivitynode_pointer ? _xconnectivitynode_pointer :
           _xconnectivitynode_pointer = new RTDBCConnectivitynode(__xcsgc3000_database);
}

int _xconnectivitynode_ok(void){
    return _xconnectivitynode()->CanUse() && 
           _xconnectivitynode()->m_ppara  &&
           _xconnectivitynode()->m_preal  &&
           _xconnectivitynode()->m_pestm  &&
           _xconnectivitynode()->m_pcal   &&
           _xconnectivitynode()->m_panaly;
}

int _xconnectivitynode_size(void){
    return _xconnectivitynode()->GetCurRecNum();
}

int _xconnectivitynode_head(void){
    return _xconnectivitynode()->GetFirstRecIndex();
}

int _xconnectivitynode_next(void){
    return _xconnectivitynode()->GetNextRecIndex();
}

int _xconnectivitynode_valid_index(int index){
    return _xconnectivitynode()->ValidIndex(index) ? _XTRUE : _XFALSE;
}

void _xconnectivitynode_free(void){
    delete _xconnectivitynode_pointer;
    _xconnectivitynode_pointer = NULL;
}

struct _xconnectivitynode_para *_xconnectivitynode_para_pointer(void){
    _xcompile_assert(sizeof(struct _xconnectivitynode_para) == 
                     sizeof(CONNECTIVITYNODE_PARA),struct_not_match);
    return (struct _xconnectivitynode_para *)(_xconnectivitynode()->m_ppara);
}

struct _xconnectivitynode_real *_xconnectivitynode_real_pointer(void){
    _xcompile_assert(sizeof(struct _xconnectivitynode_real) == 
                     sizeof(CONNECTIVITYNODE_REAL),struct_not_match);
    return (struct _xconnectivitynode_real *)(_xconnectivitynode()->m_preal);
}

struct _xconnectivitynode_estm *_xconnectivitynode_estm_pointer(void){
    _xcompile_assert(sizeof(struct _xconnectivitynode_estm) == 
                     sizeof(CONNECTIVITYNODE_ESTM),struct_not_match);
    return (struct _xconnectivitynode_estm *)(_xconnectivitynode()->m_pestm);
}

struct _xconnectivitynode_calc *_xconnectivitynode_calc_pointer(void){
    _xcompile_assert(sizeof(struct _xconnectivitynode_calc) == 
                     sizeof(CONNECTIVITYNODE_CAL),struct_not_match);
    return (struct _xconnectivitynode_calc *)(_xconnectivitynode()->m_pcal);
}

struct _xconnectivitynode_anly *_xconnectivitynode_anly_pointer(void){
    _xcompile_assert(sizeof(struct _xconnectivitynode_anly) == 
                     sizeof(CONNECTIVITYNODE_ANALY),struct_not_match);
    return (struct _xconnectivitynode_anly *)(_xconnectivitynode()->m_panaly);
}

/*
 * terminal
 */
static RTDBCTerminal *_xterminal_pointer = NULL;
static inline RTDBCTerminal *_xterminal(void){
    _xcompile_assert(TERMINAL_ID == 606,id_not_match);
    return _xterminal_pointer ? _xterminal_pointer :
           _xterminal_pointer = new RTDBCTerminal(__xcsgc3000_database);
}

int _xterminal_ok(void){
    return _xterminal()->CanUse() && 
           _xterminal()->m_ppara;
}

int _xterminal_size(void){
    return _xterminal()->GetCurRecNum();
}

int _xterminal_head(void){
    return _xterminal()->GetFirstRecIndex();
}

int _xterminal_next(void){
    return _xterminal()->GetNextRecIndex();
}

int _xterminal_valid_index(int index){
    return _xterminal()->ValidIndex(index) ? _XTRUE : _XFALSE;
}

void _xterminal_free(void){
    delete _xterminal_pointer;
    _xterminal_pointer = NULL;
}

struct _xterminal_para *_xterminal_para_pointer(void){
    _xcompile_assert(sizeof(struct _xterminal_para) == 
                     sizeof(TERMINAL_PARA),struct_not_match);
    return (struct _xterminal_para *)(_xterminal()->m_ppara);
}

/*
 * measurement
 */
static RTDBCMeasurement *_xmeasurement_pointer = NULL;
static inline RTDBCMeasurement *_xmeasurement(void){
    _xcompile_assert(MEASUREMENT_ID == 100,id_not_match);
    return _xmeasurement_pointer ? _xmeasurement_pointer :
           _xmeasurement_pointer = new RTDBCMeasurement(__xcsgc3000_database);
}

int _xmeasurement_ok(void){
    return _xmeasurement()->CanUse() && 
           _xmeasurement()->m_pdata  &&
           _xmeasurement()->m_pestm  &&
           _xmeasurement()->m_pcal   &&
           _xmeasurement()->m_panaly;
}

int _xmeasurement_size(void){
    return _xmeasurement()->GetCurRecNum();
}

int _xmeasurement_head(void){
    return _xmeasurement()->GetFirstRecIndex();
}

int _xmeasurement_next(void){
    return _xmeasurement()->GetNextRecIndex();
}

int _xmeasurement_valid_index(int index){
    return _xmeasurement()->ValidIndex(index) ? _XTRUE : _XFALSE;
}

void _xmeasurement_free(void){
    delete _xmeasurement_pointer;
    _xmeasurement_pointer = NULL;
}

struct _xmeasurement_data *_xmeasurement_data_pointer(void){
    _xcompile_assert(sizeof(struct _xmeasurement_data) == 
                     sizeof(MEASUREMENT_DATA),struct_not_match);
    return (struct _xmeasurement_data *)(_xmeasurement()->m_pdata);
}

struct _xmeasurement_estm *_xmeasurement_estm_pointer(void){
    _xcompile_assert(sizeof(struct _xmeasurement_estm) == 
                     sizeof(MEASUREMENT_ESTM),struct_not_match);
    return (struct _xmeasurement_estm *)(_xmeasurement()->m_pestm);
}

struct _xmeasurement_calc *_xmeasurement_calc_pointer(void){
    _xcompile_assert(sizeof(struct _xmeasurement_calc) == 
                     sizeof(MEASUREMENT_CAL),struct_not_match);
    return (struct _xmeasurement_calc *)(_xmeasurement()->m_pcal);
}

struct _xmeasurement_anly *_xmeasurement_anly_pointer(void){
    _xcompile_assert(sizeof(struct _xmeasurement_anly) == 
                     sizeof(MEASUREMENT_ANALY),struct_not_match);
    return (struct _xmeasurement_anly *)(_xmeasurement()->m_panaly);
}

/*
 * analoginput
 */
static RTDBCAnalogInput *_xanaloginput_pointer = NULL;
static inline RTDBCAnalogInput *_xanaloginput(void){
    _xcompile_assert(ANALOGINPUT_ID == 101,id_not_match);
    return _xanaloginput_pointer ? _xanaloginput_pointer :
           _xanaloginput_pointer = new RTDBCAnalogInput(__xcsgc3000_database);
}

int _xanaloginput_ok(void){
    return _xanaloginput()->CanUse() && 
           _xanaloginput()->m_pdata  &&
           _xanaloginput()->m_panalysis;
}

int _xanaloginput_size(void){
    return _xanaloginput()->GetCurRecNum();
}

int _xanaloginput_head(void){
    return _xanaloginput()->GetFirstRecIndex();
}

int _xanaloginput_next(void){
    return _xanaloginput()->GetNextRecIndex();
}

int _xanaloginput_valid_index(int index){
    return _xanaloginput()->ValidIndex(index) ? _XTRUE : _XFALSE;
}

void _xanaloginput_free(void){
    delete _xanaloginput_pointer;
    _xanaloginput_pointer = NULL;
}

struct _xanaloginput_data *_xanaloginput_data_pointer(void){
    _xcompile_assert(sizeof(struct _xanaloginput_data) == 
                     sizeof(ANALOGINPUT_DATA),struct_not_match);
    return (struct _xanaloginput_data *)(_xanaloginput()->m_pdata);
}

struct _xanaloginput_anly *_xanaloginput_anly_pointer(void){
    _xcompile_assert(sizeof(struct _xanaloginput_anly) == 
                     sizeof(ANALOGINPUT_ANALYSIS),struct_not_match);
    return (struct _xanaloginput_anly *)(_xanaloginput()->m_panalysis);
}

/*
 * statusinput
 */
static RTDBCStatusInput *_xstatusinput_pointer = NULL;
static inline RTDBCStatusInput *_xstatusinput(void){
    _xcompile_assert(STATUSINPUT_ID == 104,id_not_match);
    return _xstatusinput_pointer ? _xstatusinput_pointer :
           _xstatusinput_pointer = new RTDBCStatusInput(__xcsgc3000_database);
}

int _xstatusinput_ok(void){
    return _xstatusinput()->CanUse() && 
           _xstatusinput()->m_pdata  &&
           _xstatusinput()->m_panalysis;
}

int _xstatusinput_size(void){
    return _xstatusinput()->GetCurRecNum();
}

int _xstatusinput_head(void){
    return _xstatusinput()->GetFirstRecIndex();
}

int _xstatusinput_next(void){
    return _xstatusinput()->GetNextRecIndex();
}

int _xstatusinput_valid_index(int index){
    return _xstatusinput()->ValidIndex(index) ? _XTRUE : _XFALSE;
}

void _xstatusinput_free(void){
    delete _xstatusinput_pointer;
    _xstatusinput_pointer = NULL;
}

struct _xstatusinput_data *_xstatusinput_data_pointer(void){
    _xcompile_assert(sizeof(struct _xstatusinput_data) == 
                     sizeof(STATUSINPUT_DATA),struct_not_match);
    return (struct _xstatusinput_data *)(_xstatusinput()->m_pdata);
}

struct _xstatusinput_anly *_xstatusinput_anly_pointer(void){
    _xcompile_assert(sizeof(struct _xstatusinput_anly) == 
                     sizeof(STATUSINPUT_ANALYSIS),struct_not_match);
    return (struct _xstatusinput_anly *)(_xstatusinput()->m_panalysis);
}

/*
 * breaker
 */
static RTDBCBreaker *_xbreaker_pointer = NULL;
static inline RTDBCBreaker *_xbreaker(void){
    _xcompile_assert(BREAKER_ID == 611,id_not_match);
    return _xbreaker_pointer ? _xbreaker_pointer :
           _xbreaker_pointer = new RTDBCBreaker(__xcsgc3000_database);
}

int _xbreaker_ok(void){
    return _xbreaker()->CanUse()      && 
           _xbreaker()->m_ppara       &&
           _xbreaker()->m_prealState  &&
           _xbreaker()->m_prealFlag   &&
           _xbreaker()->m_pestmState  &&
           _xbreaker()->m_pestmFlag   &&
           _xbreaker()->m_pestmState3 &&
           _xbreaker()->m_pcalState   &&
           _xbreaker()->m_pcalFlag    &&
           _xbreaker()->m_pcalState3  &&
           _xbreaker()->m_panalyState &&
           _xbreaker()->m_panalyFlag  &&
           _xbreaker()->m_panalyState3;
}

int _xbreaker_size(void){
    return _xbreaker()->GetCurRecNum();
}

int _xbreaker_head(void){
    return _xbreaker()->GetFirstRecIndex();
}

int _xbreaker_next(void){
    return _xbreaker()->GetNextRecIndex();
}

int _xbreaker_valid_index(int index){
    return _xbreaker()->ValidIndex(index) ? _XTRUE : _XFALSE;
}

void _xbreaker_free(void){
    delete _xbreaker_pointer;
    _xbreaker_pointer = NULL;
}

struct _xbreaker_para *_xbreaker_para_pointer(void){
    _xcompile_assert(sizeof(struct _xbreaker_para) == 
                     sizeof(BREAKER_PARA),struct_not_match);
    return (struct _xbreaker_para *)(_xbreaker()->m_ppara);
}

struct _xbreaker_real *_xbreaker_real_pointer(void){
    _xcompile_assert(sizeof(struct _xbreaker_real) == 
                     sizeof(BREAKER_REALSTATE),struct_not_match);
    return (struct _xbreaker_real *)(_xbreaker()->m_prealState);
}

struct _xbreaker_real2 *_xbreaker_real2_pointer(void){
    _xcompile_assert(sizeof(struct _xbreaker_real2) == 
                     sizeof(BREAKER_REALFLAG),struct_not_match);
    return (struct _xbreaker_real2 *)(_xbreaker()->m_prealFlag);
}

struct _xbreaker_estm *_xbreaker_estm_pointer(void){
    _xcompile_assert(sizeof(struct _xbreaker_estm) == 
                     sizeof(BREAKER_ESTMSTATE),struct_not_match);
    return (struct _xbreaker_estm *)(_xbreaker()->m_pestmState);
}

struct _xbreaker_estm2 *_xbreaker_estm2_pointer(void){
    _xcompile_assert(sizeof(struct _xbreaker_estm2) == 
                     sizeof(BREAKER_ESTMFLAG),struct_not_match);
    return (struct _xbreaker_estm2 *)(_xbreaker()->m_pestmFlag);
}

struct _xbreaker_estm3 *_xbreaker_estm3_pointer(void){
    _xcompile_assert(sizeof(struct _xbreaker_estm3) == 
                     sizeof(BREAKER_ESTMSTATE3),struct_not_match);
    return (struct _xbreaker_estm3 *)(_xbreaker()->m_pestmState3);
}

struct _xbreaker_calc *_xbreaker_calc_pointer(void){
    _xcompile_assert(sizeof(struct _xbreaker_calc) == 
                     sizeof(BREAKER_CALSTATE),struct_not_match);
    return (struct _xbreaker_calc *)(_xbreaker()->m_pcalState);
}

struct _xbreaker_calc2 *_xbreaker_calc2_pointer(void){
    _xcompile_assert(sizeof(struct _xbreaker_calc2) == 
                     sizeof(BREAKER_CALFLAG),struct_not_match);
    return (struct _xbreaker_calc2 *)(_xbreaker()->m_pcalFlag);
}

struct _xbreaker_calc3 *_xbreaker_calc3_pointer(void){
    _xcompile_assert(sizeof(struct _xbreaker_calc3) == 
                     sizeof(BREAKER_CALSTATE3),struct_not_match);
    return (struct _xbreaker_calc3 *)(_xbreaker()->m_pcalState3);
}

struct _xbreaker_anly *_xbreaker_anly_pointer(void){
    _xcompile_assert(sizeof(struct _xbreaker_anly) == 
                     sizeof(BREAKER_ANALYSTATE),struct_not_match);
    return (struct _xbreaker_anly *)(_xbreaker()->m_panalyState);
}

struct _xbreaker_anly2 *_xbreaker_anly2_pointer(void){
    _xcompile_assert(sizeof(struct _xbreaker_anly2) == 
                     sizeof(BREAKER_ANALYFLAG),struct_not_match);
    return (struct _xbreaker_anly2 *)(_xbreaker()->m_panalyFlag);
}

struct _xbreaker_anly3 *_xbreaker_anly3_pointer(void){
    _xcompile_assert(sizeof(struct _xbreaker_anly3) == 
                     sizeof(BREAKER_ANALYSTATE3),struct_not_match);
    return (struct _xbreaker_anly3 *)(_xbreaker()->m_panalyState3);
}

/*
 * disconnector
 */
static RTDBCDisconnector *_xdisconnector_pointer = NULL;
static inline RTDBCDisconnector *_xdisconnector(void){
    _xcompile_assert(DISCONNECTOR_ID == 623,id_not_match);
    return _xdisconnector_pointer ? _xdisconnector_pointer :
           _xdisconnector_pointer = new RTDBCDisconnector(__xcsgc3000_database);
}

int _xdisconnector_ok(void){
    return _xdisconnector()->CanUse()      && 
           _xdisconnector()->m_ppara       &&
           _xdisconnector()->m_prealState  &&
           _xdisconnector()->m_prealFlag   &&
           _xdisconnector()->m_pestmState  &&
           _xdisconnector()->m_pestmFlag   &&
           _xdisconnector()->m_pestmState3 &&
           _xdisconnector()->m_pcalState   &&
           _xdisconnector()->m_pcalFlag    &&
           _xdisconnector()->m_pcalState3  &&
           _xdisconnector()->m_panalyState &&
           _xdisconnector()->m_panalyFlag  &&
           _xdisconnector()->m_panalyState3;
}

int _xdisconnector_size(void){
    return _xdisconnector()->GetCurRecNum();
}

int _xdisconnector_head(void){
    return _xdisconnector()->GetFirstRecIndex();
}

int _xdisconnector_next(void){
    return _xdisconnector()->GetNextRecIndex();
}

int _xdisconnector_valid_index(int index){
    return _xdisconnector()->ValidIndex(index) ? _XTRUE : _XFALSE;
}

void _xdisconnector_free(void){
    delete _xdisconnector_pointer;
    _xdisconnector_pointer = NULL;
}

struct _xdisconnector_para *_xdisconnector_para_pointer(void){
    _xcompile_assert(sizeof(struct _xdisconnector_para) == 
                     sizeof(DISCONNECTOR_PARA),struct_not_match);
    return (struct _xdisconnector_para *)(_xdisconnector()->m_ppara);
}

struct _xdisconnector_real *_xdisconnector_real_pointer(void){
    _xcompile_assert(sizeof(struct _xdisconnector_real) == 
                     sizeof(DISCONNECTOR_REALSTATE),struct_not_match);
    return (struct _xdisconnector_real *)(_xdisconnector()->m_prealState);
}

struct _xdisconnector_real2 *_xdisconnector_real2_pointer(void){
    _xcompile_assert(sizeof(struct _xdisconnector_real2) == 
                     sizeof(DISCONNECTOR_REALFLAG),struct_not_match);
    return (struct _xdisconnector_real2 *)(_xdisconnector()->m_prealFlag);
}

struct _xdisconnector_estm *_xdisconnector_estm_pointer(void){
    _xcompile_assert(sizeof(struct _xdisconnector_estm) == 
                     sizeof(DISCONNECTOR_ESTMSTATE),struct_not_match);
    return (struct _xdisconnector_estm *)(_xdisconnector()->m_pestmState);
}

struct _xdisconnector_estm2 *_xdisconnector_estm2_pointer(void){
    _xcompile_assert(sizeof(struct _xdisconnector_estm2) == 
                     sizeof(DISCONNECTOR_ESTMFLAG),struct_not_match);
    return (struct _xdisconnector_estm2 *)(_xdisconnector()->m_pestmFlag);
}

struct _xdisconnector_estm3 *_xdisconnector_estm3_pointer(void){
    _xcompile_assert(sizeof(struct _xdisconnector_estm3) == 
                     sizeof(DISCONNECTOR_ESTMSTATE3),struct_not_match);
    return (struct _xdisconnector_estm3 *)(_xdisconnector()->m_pestmState3);
}

struct _xdisconnector_calc *_xdisconnector_calc_pointer(void){
    _xcompile_assert(sizeof(struct _xdisconnector_calc) == 
                     sizeof(DISCONNECTOR_CALSTATE),struct_not_match);
    return (struct _xdisconnector_calc *)(_xdisconnector()->m_pcalState);
}

struct _xdisconnector_calc2 *_xdisconnector_calc2_pointer(void){
    _xcompile_assert(sizeof(struct _xdisconnector_calc2) == 
                     sizeof(DISCONNECTOR_CALFLAG),struct_not_match);
    return (struct _xdisconnector_calc2 *)(_xdisconnector()->m_pcalFlag);
}

struct _xdisconnector_calc3 *_xdisconnector_calc3_pointer(void){
    _xcompile_assert(sizeof(struct _xdisconnector_calc3) == 
                     sizeof(DISCONNECTOR_CALSTATE3),struct_not_match);
    return (struct _xdisconnector_calc3 *)(_xdisconnector()->m_pcalState3);
}

struct _xdisconnector_anly *_xdisconnector_anly_pointer(void){
    _xcompile_assert(sizeof(struct _xdisconnector_anly) == 
                     sizeof(DISCONNECTOR_ANALYSTATE),struct_not_match);
    return (struct _xdisconnector_anly *)(_xdisconnector()->m_panalyState);
}

struct _xdisconnector_anly2 *_xdisconnector_anly2_pointer(void){
    _xcompile_assert(sizeof(struct _xdisconnector_anly2) == 
                     sizeof(DISCONNECTOR_ANALYFLAG),struct_not_match);
    return (struct _xdisconnector_anly2 *)(_xdisconnector()->m_panalyFlag);
}

struct _xdisconnector_anly3 *_xdisconnector_anly3_pointer(void){
    _xcompile_assert(sizeof(struct _xdisconnector_anly3) == 
                     sizeof(DISCONNECTOR_ANALYSTATE3),struct_not_match);
    return (struct _xdisconnector_anly3 *)(_xdisconnector()->m_panalyState3);
}

/*
 * grounddisconnector
 */
static RTDBCGroundDisconnector *_xgrounddisconnector_pointer = NULL;
static inline RTDBCGroundDisconnector *_xgrounddisconnector(void){
    _xcompile_assert(GROUNDDISCONNECTOR_ID == 624,id_not_match);
    return _xgrounddisconnector_pointer ? _xgrounddisconnector_pointer :
           _xgrounddisconnector_pointer = new RTDBCGroundDisconnector(__xcsgc3000_database);
}

int _xgrounddisconnector_ok(void){
    return _xgrounddisconnector()->CanUse()      && 
           _xgrounddisconnector()->m_ppara       &&
           _xgrounddisconnector()->m_prealState  &&
           _xgrounddisconnector()->m_prealFlag   &&
           _xgrounddisconnector()->m_pestmState  &&
           _xgrounddisconnector()->m_pestmFlag   &&
           _xgrounddisconnector()->m_pestmState3 &&
           _xgrounddisconnector()->m_pcalState   &&
           _xgrounddisconnector()->m_pcalFlag    &&
           _xgrounddisconnector()->m_pcalState3  &&
           _xgrounddisconnector()->m_panalyState &&
           _xgrounddisconnector()->m_panalyFlag  &&
           _xgrounddisconnector()->m_panalyState3;
}

int _xgrounddisconnector_size(void){
    return _xgrounddisconnector()->GetCurRecNum();
}

int _xgrounddisconnector_head(void){
    return _xgrounddisconnector()->GetFirstRecIndex();
}

int _xgrounddisconnector_next(void){
    return _xgrounddisconnector()->GetNextRecIndex();
}

int _xgrounddisconnector_valid_index(int index){
    return _xgrounddisconnector()->ValidIndex(index) ? _XTRUE : _XFALSE;
}

void _xgrounddisconnector_free(void){
    delete _xgrounddisconnector_pointer;
    _xgrounddisconnector_pointer = NULL;
}

struct _xgrounddisconnector_para *_xgrounddisconnector_para_pointer(void){
    _xcompile_assert(sizeof(struct _xgrounddisconnector_para) == 
                     sizeof(GROUNDDISCONNECTOR_PARA),struct_not_match);
    return (struct _xgrounddisconnector_para *)(_xgrounddisconnector()->m_ppara);
}

struct _xgrounddisconnector_real *_xgrounddisconnector_real_pointer(void){
    _xcompile_assert(sizeof(struct _xgrounddisconnector_real) == 
                     sizeof(GROUNDDISCONNECTOR_REALSTATE),struct_not_match);
    return (struct _xgrounddisconnector_real *)(_xgrounddisconnector()->m_prealState);
}

struct _xgrounddisconnector_real2 *_xgrounddisconnector_real2_pointer(void){
    _xcompile_assert(sizeof(struct _xgrounddisconnector_real2) == 
                     sizeof(GROUNDDISCONNECTOR_REALFLAG),struct_not_match);
    return (struct _xgrounddisconnector_real2 *)(_xgrounddisconnector()->m_prealFlag);
}

struct _xgrounddisconnector_estm *_xgrounddisconnector_estm_pointer(void){
    _xcompile_assert(sizeof(struct _xgrounddisconnector_estm) == 
                     sizeof(GROUNDDISCONNECTOR_ESTMSTATE),struct_not_match);
    return (struct _xgrounddisconnector_estm *)(_xgrounddisconnector()->m_pestmState);
}

struct _xgrounddisconnector_estm2 *_xgrounddisconnector_estm2_pointer(void){
    _xcompile_assert(sizeof(struct _xgrounddisconnector_estm2) == 
                     sizeof(GROUNDDISCONNECTOR_ESTMFLAG),struct_not_match);
    return (struct _xgrounddisconnector_estm2 *)(_xgrounddisconnector()->m_pestmFlag);
}

struct _xgrounddisconnector_estm3 *_xgrounddisconnector_estm3_pointer(void){
    _xcompile_assert(sizeof(struct _xgrounddisconnector_estm3) == 
                     sizeof(GROUNDDISCONNECTOR_ESTMSTATE3),struct_not_match);
    return (struct _xgrounddisconnector_estm3 *)(_xgrounddisconnector()->m_pestmState3);
}

struct _xgrounddisconnector_calc *_xgrounddisconnector_calc_pointer(void){
    _xcompile_assert(sizeof(struct _xgrounddisconnector_calc) == 
                     sizeof(GROUNDDISCONNECTOR_CALSTATE),struct_not_match);
    return (struct _xgrounddisconnector_calc *)(_xgrounddisconnector()->m_pcalState);
}

struct _xgrounddisconnector_calc2 *_xgrounddisconnector_calc2_pointer(void){
    _xcompile_assert(sizeof(struct _xgrounddisconnector_calc2) == 
                     sizeof(GROUNDDISCONNECTOR_CALFLAG),struct_not_match);
    return (struct _xgrounddisconnector_calc2 *)(_xgrounddisconnector()->m_pcalFlag);
}

struct _xgrounddisconnector_calc3 *_xgrounddisconnector_calc3_pointer(void){
    _xcompile_assert(sizeof(struct _xgrounddisconnector_calc3) == 
                     sizeof(GROUNDDISCONNECTOR_CALSTATE3),struct_not_match);
    return (struct _xgrounddisconnector_calc3 *)(_xgrounddisconnector()->m_pcalState3);
}

struct _xgrounddisconnector_anly *_xgrounddisconnector_anly_pointer(void){
    _xcompile_assert(sizeof(struct _xgrounddisconnector_anly) == 
                     sizeof(GROUNDDISCONNECTOR_ANALYSTATE),struct_not_match);
    return (struct _xgrounddisconnector_anly *)(_xgrounddisconnector()->m_panalyState);
}

struct _xgrounddisconnector_anly2 *_xgrounddisconnector_anly2_pointer(void){
    _xcompile_assert(sizeof(struct _xgrounddisconnector_anly2) == 
                     sizeof(GROUNDDISCONNECTOR_ANALYFLAG),struct_not_match);
    return (struct _xgrounddisconnector_anly2 *)(_xgrounddisconnector()->m_panalyFlag);
}

struct _xgrounddisconnector_anly3 *_xgrounddisconnector_anly3_pointer(void){
    _xcompile_assert(sizeof(struct _xgrounddisconnector_anly3) == 
                     sizeof(GROUNDDISCONNECTOR_ANALYSTATE3),struct_not_match);
    return (struct _xgrounddisconnector_anly3 *)(_xgrounddisconnector()->m_panalyState3);
}

/*
 * energyconsumer
 */
static RTDBCEnergyconsumer *_xenergyconsumer_pointer = NULL;
static inline RTDBCEnergyconsumer *_xenergyconsumer(void){
    _xcompile_assert(ENERGYCONSUMER_ID == 613,id_not_match);
    return _xenergyconsumer_pointer ? _xenergyconsumer_pointer :
           _xenergyconsumer_pointer = new RTDBCEnergyconsumer(__xcsgc3000_database);
}

int _xenergyconsumer_ok(void){
    return _xenergyconsumer()->CanUse()      && 
           _xenergyconsumer()->m_ppara       &&
           _xenergyconsumer()->m_prealState  &&
           _xenergyconsumer()->m_prealFlag   &&
           _xenergyconsumer()->m_pestmState  &&
           _xenergyconsumer()->m_pestmFlag   &&
           _xenergyconsumer()->m_pestmState3 &&
           _xenergyconsumer()->m_pcalState   &&
           _xenergyconsumer()->m_pcalFlag    &&
           _xenergyconsumer()->m_pcalState3  &&
           _xenergyconsumer()->m_panalyState &&
           _xenergyconsumer()->m_panalyFlag  &&
           _xenergyconsumer()->m_panalyState3;
}

int _xenergyconsumer_size(void){
    return _xenergyconsumer()->GetCurRecNum();
}

int _xenergyconsumer_head(void){
    return _xenergyconsumer()->GetFirstRecIndex();
}

int _xenergyconsumer_next(void){
    return _xenergyconsumer()->GetNextRecIndex();
}

int _xenergyconsumer_valid_index(int index){
    return _xenergyconsumer()->ValidIndex(index) ? _XTRUE : _XFALSE;
}

void _xenergyconsumer_free(void){
    delete _xenergyconsumer_pointer;
    _xenergyconsumer_pointer = NULL;
}

struct _xenergyconsumer_para *_xenergyconsumer_para_pointer(void){
    _xcompile_assert(sizeof(struct _xenergyconsumer_para) == 
                     sizeof(ENERGYCONSUMER_PARA),struct_not_match);
    return (struct _xenergyconsumer_para *)(_xenergyconsumer()->m_ppara);
}

struct _xenergyconsumer_real *_xenergyconsumer_real_pointer(void){
    _xcompile_assert(sizeof(struct _xenergyconsumer_real) == 
                     sizeof(ENERGYCONSUMER_REALSTATE),struct_not_match);
    return (struct _xenergyconsumer_real *)(_xenergyconsumer()->m_prealState);
}

struct _xenergyconsumer_real2 *_xenergyconsumer_real2_pointer(void){
    _xcompile_assert(sizeof(struct _xenergyconsumer_real2) == 
                     sizeof(ENERGYCONSUMER_REALFLAG),struct_not_match);
    return (struct _xenergyconsumer_real2 *)(_xenergyconsumer()->m_prealFlag);
}

struct _xenergyconsumer_estm *_xenergyconsumer_estm_pointer(void){
    _xcompile_assert(sizeof(struct _xenergyconsumer_estm) == 
                     sizeof(ENERGYCONSUMER_ESTMSTATE),struct_not_match);
    return (struct _xenergyconsumer_estm *)(_xenergyconsumer()->m_pestmState);
}

struct _xenergyconsumer_estm2 *_xenergyconsumer_estm2_pointer(void){
    _xcompile_assert(sizeof(struct _xenergyconsumer_estm2) == 
                     sizeof(ENERGYCONSUMER_ESTMFALG),struct_not_match);
    return (struct _xenergyconsumer_estm2 *)(_xenergyconsumer()->m_pestmFlag);
}

struct _xenergyconsumer_estm3 *_xenergyconsumer_estm3_pointer(void){
    _xcompile_assert(sizeof(struct _xenergyconsumer_estm3) == 
                     sizeof(ENERGYCONSUMER_ESTMSTATE3),struct_not_match);
    return (struct _xenergyconsumer_estm3 *)(_xenergyconsumer()->m_pestmState3);
}

struct _xenergyconsumer_calc *_xenergyconsumer_calc_pointer(void){
    _xcompile_assert(sizeof(struct _xenergyconsumer_calc) == 
                     sizeof(ENERGYCONSUMER_CALSTATE),struct_not_match);
    return (struct _xenergyconsumer_calc *)(_xenergyconsumer()->m_pcalState);
}

struct _xenergyconsumer_calc2 *_xenergyconsumer_calc2_pointer(void){
    _xcompile_assert(sizeof(struct _xenergyconsumer_calc2) == 
                     sizeof(ENERGYCONSUMER_CALFLAG),struct_not_match);
    return (struct _xenergyconsumer_calc2 *)(_xenergyconsumer()->m_pcalFlag);
}

struct _xenergyconsumer_calc3 *_xenergyconsumer_calc3_pointer(void){
    _xcompile_assert(sizeof(struct _xenergyconsumer_calc3) == 
                     sizeof(ENERGYCONSUMER_CALSTATE3),struct_not_match);
    return (struct _xenergyconsumer_calc3 *)(_xenergyconsumer()->m_pcalState3);
}

struct _xenergyconsumer_anly *_xenergyconsumer_anly_pointer(void){
    _xcompile_assert(sizeof(struct _xenergyconsumer_anly) == 
                     sizeof(ENERGYCONSUMER_ANALYSTATE),struct_not_match);
    return (struct _xenergyconsumer_anly *)(_xenergyconsumer()->m_panalyState);
}

struct _xenergyconsumer_anly2 *_xenergyconsumer_anly2_pointer(void){
    _xcompile_assert(sizeof(struct _xenergyconsumer_anly2) == 
                     sizeof(ENERGYCONSUMER_ANALYFLAG),struct_not_match);
    return (struct _xenergyconsumer_anly2 *)(_xenergyconsumer()->m_panalyFlag);
}

struct _xenergyconsumer_anly3 *_xenergyconsumer_anly3_pointer(void){
    _xcompile_assert(sizeof(struct _xenergyconsumer_anly3) == 
                     sizeof(ENERGYCONSUMER_ANALYSTATE3),struct_not_match);
    return (struct _xenergyconsumer_anly3 *)(_xenergyconsumer()->m_panalyState3);
}

/*
 * compensator
 */
static RTDBCCompensator *_xcompensator_pointer = NULL;
static inline RTDBCCompensator *_xcompensator(void){
    _xcompile_assert(COMPENSATOR_ID == 618,id_not_match);
    return _xcompensator_pointer ? _xcompensator_pointer :
           _xcompensator_pointer = new RTDBCCompensator(__xcsgc3000_database);
}

int _xcompensator_ok(void){
    return _xcompensator()->CanUse()      && 
           _xcompensator()->m_ppara       &&
           _xcompensator()->m_prealState  &&
           _xcompensator()->m_prealFlag   &&
           _xcompensator()->m_pestmState  &&
           _xcompensator()->m_pestmFlag   &&
           _xcompensator()->m_pestmState3 &&
           _xcompensator()->m_pcalState   &&
           _xcompensator()->m_pcalFlag    &&
           _xcompensator()->m_pcalState3  &&
           _xcompensator()->m_panalyState &&
           _xcompensator()->m_panalyFlag  &&
           _xcompensator()->m_panalyState3;
}

int _xcompensator_size(void){
    return _xcompensator()->GetCurRecNum();
}

int _xcompensator_head(void){
    return _xcompensator()->GetFirstRecIndex();
}

int _xcompensator_next(void){
    return _xcompensator()->GetNextRecIndex();
}

int _xcompensator_valid_index(int index){
    return _xcompensator()->ValidIndex(index) ? _XTRUE : _XFALSE;
}

void _xcompensator_free(void){
    delete _xcompensator_pointer;
    _xcompensator_pointer = NULL;
}

struct _xcompensator_para *_xcompensator_para_pointer(void){
    _xcompile_assert(sizeof(struct _xcompensator_para) == 
                     sizeof(COMPENSATOR_PARA),struct_not_match);
    return (struct _xcompensator_para *)(_xcompensator()->m_ppara);
}

struct _xcompensator_real *_xcompensator_real_pointer(void){
    _xcompile_assert(sizeof(struct _xcompensator_real) == 
                     sizeof(COMPENSATOR_REALSTATE),struct_not_match);
    return (struct _xcompensator_real *)(_xcompensator()->m_prealState);
}

struct _xcompensator_real2 *_xcompensator_real2_pointer(void){
    _xcompile_assert(sizeof(struct _xcompensator_real2) == 
                     sizeof(COMPENSATOR_REALFLAG),struct_not_match);
    return (struct _xcompensator_real2 *)(_xcompensator()->m_prealFlag);
}

struct _xcompensator_estm *_xcompensator_estm_pointer(void){
    _xcompile_assert(sizeof(struct _xcompensator_estm) == 
                     sizeof(COMPENSATOR_ESTMSTATE),struct_not_match);
    return (struct _xcompensator_estm *)(_xcompensator()->m_pestmState);
}

struct _xcompensator_estm2 *_xcompensator_estm2_pointer(void){
    _xcompile_assert(sizeof(struct _xcompensator_estm2) == 
                     sizeof(COMPENSATOR_ESTMFLAG),struct_not_match);
    return (struct _xcompensator_estm2 *)(_xcompensator()->m_pestmFlag);
}

struct _xcompensator_estm3 *_xcompensator_estm3_pointer(void){
    _xcompile_assert(sizeof(struct _xcompensator_estm3) == 
                     sizeof(COMPENSATOR_ESTMSTATE3),struct_not_match);
    return (struct _xcompensator_estm3 *)(_xcompensator()->m_pestmState3);
}

struct _xcompensator_calc *_xcompensator_calc_pointer(void){
    _xcompile_assert(sizeof(struct _xcompensator_calc) == 
                     sizeof(COMPENSATOR_CALSTATE),struct_not_match);
    return (struct _xcompensator_calc *)(_xcompensator()->m_pcalState);
}

struct _xcompensator_calc2 *_xcompensator_calc2_pointer(void){
    _xcompile_assert(sizeof(struct _xcompensator_calc2) == 
                     sizeof(COMPENSATOR_CALFLAG),struct_not_match);
    return (struct _xcompensator_calc2 *)(_xcompensator()->m_pcalFlag);
}

struct _xcompensator_calc3 *_xcompensator_calc3_pointer(void){
    _xcompile_assert(sizeof(struct _xcompensator_calc3) == 
                     sizeof(COMPENSATOR_CALSTATE3),struct_not_match);
    return (struct _xcompensator_calc3 *)(_xcompensator()->m_pcalState3);
}

struct _xcompensator_anly *_xcompensator_anly_pointer(void){
    _xcompile_assert(sizeof(struct _xcompensator_anly) == 
                     sizeof(COMPENSATOR_ANALYSTATE),struct_not_match);
    return (struct _xcompensator_anly *)(_xcompensator()->m_panalyState);
}

struct _xcompensator_anly2 *_xcompensator_anly2_pointer(void){
    _xcompile_assert(sizeof(struct _xcompensator_anly2) == 
                     sizeof(COMPENSATOR_ANALYFLAG),struct_not_match);
    return (struct _xcompensator_anly2 *)(_xcompensator()->m_panalyFlag);
}

struct _xcompensator_anly3 *_xcompensator_anly3_pointer(void){
    _xcompile_assert(sizeof(struct _xcompensator_anly3) == 
                     sizeof(COMPENSATOR_ANALYSTATE3),struct_not_match);
    return (struct _xcompensator_anly3 *)(_xcompensator()->m_panalyState3);
}

/*
 * staticvarcompensator
 */
static RTDBCStaticVarCompensator *_xstaticvarcompensator_pointer = NULL;
static inline RTDBCStaticVarCompensator *_xstaticvarcompensator(void){
    _xcompile_assert(STATICVARCOMPENSATOR_ID == 617,id_not_match);
    return _xstaticvarcompensator_pointer ? _xstaticvarcompensator_pointer :
           _xstaticvarcompensator_pointer = new RTDBCStaticVarCompensator(__xcsgc3000_database);
}

int _xstaticvarcompensator_ok(void){
    return _xstaticvarcompensator()->CanUse()      && 
           _xstaticvarcompensator()->m_ppara       &&
           _xstaticvarcompensator()->m_prealState  &&
           _xstaticvarcompensator()->m_prealFlag   &&
           _xstaticvarcompensator()->m_pestmState  &&
           _xstaticvarcompensator()->m_pestmFlag   &&
           _xstaticvarcompensator()->m_pestmState3 &&
           _xstaticvarcompensator()->m_pcalState   &&
           _xstaticvarcompensator()->m_pcalFlag    &&
           _xstaticvarcompensator()->m_pcalState3  &&
           _xstaticvarcompensator()->m_panalyState &&
           _xstaticvarcompensator()->m_panalyFlag  &&
           _xstaticvarcompensator()->m_panalyState3;
}

int _xstaticvarcompensator_size(void){
    return _xstaticvarcompensator()->GetCurRecNum();
}

int _xstaticvarcompensator_head(void){
    return _xstaticvarcompensator()->GetFirstRecIndex();
}

int _xstaticvarcompensator_next(void){
    return _xstaticvarcompensator()->GetNextRecIndex();
}

int _xstaticvarcompensator_valid_index(int index){
    return _xstaticvarcompensator()->ValidIndex(index) ? _XTRUE : _XFALSE;
}

void _xstaticvarcompensator_free(void){
    delete _xstaticvarcompensator_pointer;
    _xstaticvarcompensator_pointer = NULL;
}

struct _xstaticvarcompensator_para *_xstaticvarcompensator_para_pointer(void){
    _xcompile_assert(sizeof(struct _xstaticvarcompensator_para) == 
                     sizeof(STATICVARCOMPENSATOR_PARA),struct_not_match);
    return (struct _xstaticvarcompensator_para *)(_xstaticvarcompensator()->m_ppara);
}

struct _xstaticvarcompensator_real *_xstaticvarcompensator_real_pointer(void){
    _xcompile_assert(sizeof(struct _xstaticvarcompensator_real) == 
                     sizeof(STATICVARCOMPENSATOR_REALSTATE),struct_not_match);
    return (struct _xstaticvarcompensator_real *)(_xstaticvarcompensator()->m_prealState);
}

struct _xstaticvarcompensator_real2 *_xstaticvarcompensator_real2_pointer(void){
    _xcompile_assert(sizeof(struct _xstaticvarcompensator_real2) == 
                     sizeof(STATICVARCOMPENSATOR_REALFLAG),struct_not_match);
    return (struct _xstaticvarcompensator_real2 *)(_xstaticvarcompensator()->m_prealFlag);
}

struct _xstaticvarcompensator_estm *_xstaticvarcompensator_estm_pointer(void){
    _xcompile_assert(sizeof(struct _xstaticvarcompensator_estm) == 
                     sizeof(STATICVARCOMPENSATOR_ESTMSTATE),struct_not_match);
    return (struct _xstaticvarcompensator_estm *)(_xstaticvarcompensator()->m_pestmState);
}

struct _xstaticvarcompensator_estm2 *_xstaticvarcompensator_estm2_pointer(void){
    _xcompile_assert(sizeof(struct _xstaticvarcompensator_estm2) == 
                     sizeof(STATICVARCOMPENSATOR_ESTMFLAG),struct_not_match);
    return (struct _xstaticvarcompensator_estm2 *)(_xstaticvarcompensator()->m_pestmFlag);
}

struct _xstaticvarcompensator_estm3 *_xstaticvarcompensator_estm3_pointer(void){
    _xcompile_assert(sizeof(struct _xstaticvarcompensator_estm3) == 
                     sizeof(STATICVARCOMPENSATOR_ESTMSTATE3),struct_not_match);
    return (struct _xstaticvarcompensator_estm3 *)(_xstaticvarcompensator()->m_pestmState3);
}

struct _xstaticvarcompensator_calc *_xstaticvarcompensator_calc_pointer(void){
    _xcompile_assert(sizeof(struct _xstaticvarcompensator_calc) == 
                     sizeof(STATICVARCOMPENSATOR_CALSTATE),struct_not_match);
    return (struct _xstaticvarcompensator_calc *)(_xstaticvarcompensator()->m_pcalState);
}

struct _xstaticvarcompensator_calc2 *_xstaticvarcompensator_calc2_pointer(void){
    _xcompile_assert(sizeof(struct _xstaticvarcompensator_calc2) == 
                     sizeof(STATICVARCOMPENSATOR_CALFLAG),struct_not_match);
    return (struct _xstaticvarcompensator_calc2 *)(_xstaticvarcompensator()->m_pcalFlag);
}

struct _xstaticvarcompensator_calc3 *_xstaticvarcompensator_calc3_pointer(void){
    _xcompile_assert(sizeof(struct _xstaticvarcompensator_calc3) == 
                     sizeof(STATICVARCOMPENSATOR_CALSTATE3),struct_not_match);
    return (struct _xstaticvarcompensator_calc3 *)(_xstaticvarcompensator()->m_pcalState3);
}

struct _xstaticvarcompensator_anly *_xstaticvarcompensator_anly_pointer(void){
    _xcompile_assert(sizeof(struct _xstaticvarcompensator_anly) == 
                     sizeof(STATICVARCOMPENSATOR_ANALYSTATE),struct_not_match);
    return (struct _xstaticvarcompensator_anly *)(_xstaticvarcompensator()->m_panalyState);
}

struct _xstaticvarcompensator_anly2 *_xstaticvarcompensator_anly2_pointer(void){
    _xcompile_assert(sizeof(struct _xstaticvarcompensator_anly2) == 
                     sizeof(STATICVARCOMPENSATOR_ANALYFLAG),struct_not_match);
    return (struct _xstaticvarcompensator_anly2 *)(_xstaticvarcompensator()->m_panalyFlag);
}

struct _xstaticvarcompensator_anly3 *_xstaticvarcompensator_anly3_pointer(void){
    _xcompile_assert(sizeof(struct _xstaticvarcompensator_anly3) == 
                     sizeof(STATICVARCOMPENSATOR_ANALYSTATE3),struct_not_match);
    return (struct _xstaticvarcompensator_anly3 *)(_xstaticvarcompensator()->m_panalyState3);
}

/*
 * transformerwinding
 */
static RTDBCTransformerwinding *_xtransformerwinding_pointer = NULL;
static inline RTDBCTransformerwinding *_xtransformerwinding(void){
    _xcompile_assert(TRANSFORMERWINDING_ID == 615,id_not_match);
    return _xtransformerwinding_pointer ? _xtransformerwinding_pointer :
           _xtransformerwinding_pointer = new RTDBCTransformerwinding(__xcsgc3000_database);
}

int _xtransformerwinding_ok(void){
    return _xtransformerwinding()->CanUse()      && 
           _xtransformerwinding()->m_ppara       &&
           _xtransformerwinding()->m_prealState  &&
           _xtransformerwinding()->m_prealFlag   &&
           _xtransformerwinding()->m_pestmState  &&
           _xtransformerwinding()->m_pestmFlag   &&
           _xtransformerwinding()->m_pestmState3 &&
           _xtransformerwinding()->m_pcalState   &&
           _xtransformerwinding()->m_pcalFlag    &&
           _xtransformerwinding()->m_pcalState3  &&
           _xtransformerwinding()->m_panalyState &&
           _xtransformerwinding()->m_panalyFlag  &&
           _xtransformerwinding()->m_panalyState3;
}

int _xtransformerwinding_size(void){
    return _xtransformerwinding()->GetCurRecNum();
}

int _xtransformerwinding_head(void){
    return _xtransformerwinding()->GetFirstRecIndex();
}

int _xtransformerwinding_next(void){
    return _xtransformerwinding()->GetNextRecIndex();
}

int _xtransformerwinding_valid_index(int index){
    return _xtransformerwinding()->ValidIndex(index) ? _XTRUE : _XFALSE;
}

void _xtransformerwinding_free(void){
    delete _xtransformerwinding_pointer;
    _xtransformerwinding_pointer = NULL;
}

struct _xtransformerwinding_para *_xtransformerwinding_para_pointer(void){
    _xcompile_assert(sizeof(struct _xtransformerwinding_para) == 
                     sizeof(TRANSFORMERWINDING_PARA),struct_not_match);
    return (struct _xtransformerwinding_para *)(_xtransformerwinding()->m_ppara);
}

struct _xtransformerwinding_real *_xtransformerwinding_real_pointer(void){
    _xcompile_assert(sizeof(struct _xtransformerwinding_real) == 
                     sizeof(TRANSFORMERWINDING_REALSTATE),struct_not_match);
    return (struct _xtransformerwinding_real *)(_xtransformerwinding()->m_prealState);
}

struct _xtransformerwinding_real2 *_xtransformerwinding_real2_pointer(void){
    _xcompile_assert(sizeof(struct _xtransformerwinding_real2) == 
                     sizeof(TRANSFORMERWINDING_REALFLAG),struct_not_match);
    return (struct _xtransformerwinding_real2 *)(_xtransformerwinding()->m_prealFlag);
}

struct _xtransformerwinding_estm *_xtransformerwinding_estm_pointer(void){
    _xcompile_assert(sizeof(struct _xtransformerwinding_estm) == 
                     sizeof(TRANSFORMERWINDING_ESTMSTATE),struct_not_match);
    return (struct _xtransformerwinding_estm *)(_xtransformerwinding()->m_pestmState);
}

struct _xtransformerwinding_estm2 *_xtransformerwinding_estm2_pointer(void){
    _xcompile_assert(sizeof(struct _xtransformerwinding_estm2) == 
                     sizeof(TRANSFORMERWINDING_ESTMFLAG),struct_not_match);
    return (struct _xtransformerwinding_estm2 *)(_xtransformerwinding()->m_pestmFlag);
}

struct _xtransformerwinding_estm3 *_xtransformerwinding_estm3_pointer(void){
    _xcompile_assert(sizeof(struct _xtransformerwinding_estm3) == 
                     sizeof(TRANSFORMERWINDING_ESTMSTATE3),struct_not_match);
    return (struct _xtransformerwinding_estm3 *)(_xtransformerwinding()->m_pestmState3);
}

struct _xtransformerwinding_calc *_xtransformerwinding_calc_pointer(void){
    _xcompile_assert(sizeof(struct _xtransformerwinding_calc) == 
                     sizeof(TRANSFORMERWINDING_CALSTATE),struct_not_match);
    return (struct _xtransformerwinding_calc *)(_xtransformerwinding()->m_pcalState);
}

struct _xtransformerwinding_calc2 *_xtransformerwinding_calc2_pointer(void){
    _xcompile_assert(sizeof(struct _xtransformerwinding_calc2) == 
                     sizeof(TRANSFORMERWINDING_CALFLAG),struct_not_match);
    return (struct _xtransformerwinding_calc2 *)(_xtransformerwinding()->m_pcalFlag);
}

struct _xtransformerwinding_calc3 *_xtransformerwinding_calc3_pointer(void){
    _xcompile_assert(sizeof(struct _xtransformerwinding_calc3) == 
                     sizeof(TRANSFORMERWINDING_CALSTATE3),struct_not_match);
    return (struct _xtransformerwinding_calc3 *)(_xtransformerwinding()->m_pcalState3);
}

struct _xtransformerwinding_anly *_xtransformerwinding_anly_pointer(void){
    _xcompile_assert(sizeof(struct _xtransformerwinding_anly) == 
                     sizeof(TRANSFORMERWINDING_ANALYSTATE),struct_not_match);
    return (struct _xtransformerwinding_anly *)(_xtransformerwinding()->m_panalyState);
}

struct _xtransformerwinding_anly2 *_xtransformerwinding_anly2_pointer(void){
    _xcompile_assert(sizeof(struct _xtransformerwinding_anly2) == 
                     sizeof(TRANSFORMERWINDING_ANALYFLAG),struct_not_match);
    return (struct _xtransformerwinding_anly2 *)(_xtransformerwinding()->m_panalyFlag);
}

struct _xtransformerwinding_anly3 *_xtransformerwinding_anly3_pointer(void){
    _xcompile_assert(sizeof(struct _xtransformerwinding_anly3) == 
                     sizeof(TRANSFORMERWINDING_ANALYSTATE3),struct_not_match);
    return (struct _xtransformerwinding_anly3 *)(_xtransformerwinding()->m_panalyState3);
}

/*
 * tapchanger
 */
static RTDBCTapchanger *_xtapchanger_pointer = NULL;
static inline RTDBCTapchanger *_xtapchanger(void){
    _xcompile_assert(TAPCHANGER_ID == 616,id_not_match);
    return _xtapchanger_pointer ? _xtapchanger_pointer :
           _xtapchanger_pointer = new RTDBCTapchanger(__xcsgc3000_database);
}

int _xtapchanger_ok(void){
    return _xtapchanger()->CanUse()      && 
           _xtapchanger()->m_ppara       &&
           _xtapchanger()->m_prealState  &&
           _xtapchanger()->m_pestmState  &&
           _xtapchanger()->m_pcalState   &&
           _xtapchanger()->m_panalyState;
}

int _xtapchanger_size(void){
    return _xtapchanger()->GetCurRecNum();
}

int _xtapchanger_head(void){
    return _xtapchanger()->GetFirstRecIndex();
}

int _xtapchanger_next(void){
    return _xtapchanger()->GetNextRecIndex();
}

int _xtapchanger_valid_index(int index){
    return _xtapchanger()->ValidIndex(index) ? _XTRUE : _XFALSE;
}

void _xtapchanger_free(void){
    delete _xtapchanger_pointer;
    _xtapchanger_pointer = NULL;
}

struct _xtapchanger_para *_xtapchanger_para_pointer(void){
    _xcompile_assert(sizeof(struct _xtapchanger_para) == 
                     sizeof(TAPCHANGER_PARA),struct_not_match);
    return (struct _xtapchanger_para *)(_xtapchanger()->m_ppara);
}

struct _xtapchanger_real *_xtapchanger_real_pointer(void){
    _xcompile_assert(sizeof(struct _xtapchanger_real) == 
                     sizeof(TAPCHANGER_REALSTATE),struct_not_match);
    return (struct _xtapchanger_real *)(_xtapchanger()->m_prealState);
}

struct _xtapchanger_estm *_xtapchanger_estm_pointer(void){
    _xcompile_assert(sizeof(struct _xtapchanger_estm) == 
                     sizeof(TAPCHANGER_ESTMSTATE),struct_not_match);
    return (struct _xtapchanger_estm *)(_xtapchanger()->m_pestmState);
}

struct _xtapchanger_calc *_xtapchanger_calc_pointer(void){
    _xcompile_assert(sizeof(struct _xtapchanger_calc) == 
                     sizeof(TAPCHANGER_CALSTATE),struct_not_match);
    return (struct _xtapchanger_calc *)(_xtapchanger()->m_pcalState);
}

struct _xtapchanger_anly *_xtapchanger_anly_pointer(void){
    _xcompile_assert(sizeof(struct _xtapchanger_anly) == 
                     sizeof(TAPCHANGER_ANALYSTATE),struct_not_match);
    return (struct _xtapchanger_anly *)(_xtapchanger()->m_panalyState);
}

/*
 * aclinesegment
 */
static RTDBCAclinesegment *_xaclinesegment_pointer = NULL;
static inline RTDBCAclinesegment *_xaclinesegment(void){
    _xcompile_assert(ACLINESEGMENT_ID == 614,id_not_match);
    return _xaclinesegment_pointer ? _xaclinesegment_pointer :
           _xaclinesegment_pointer = new RTDBCAclinesegment(__xcsgc3000_database);
}

int _xaclinesegment_ok(void){
    return _xaclinesegment()->CanUse()      && 
           _xaclinesegment()->m_ppara       &&
           _xaclinesegment()->m_prealState  &&
           _xaclinesegment()->m_prealFlag   &&
           _xaclinesegment()->m_pestmState  &&
           _xaclinesegment()->m_pestmFlag   &&
           _xaclinesegment()->m_pestmState3 &&
           _xaclinesegment()->m_pcalState   &&
           _xaclinesegment()->m_pcalFlag    &&
           _xaclinesegment()->m_pcalState3  &&
           _xaclinesegment()->m_panalyState &&
           _xaclinesegment()->m_panalyFlag  &&
           _xaclinesegment()->m_panalyState3;
}

int _xaclinesegment_size(void){
    return _xaclinesegment()->GetCurRecNum();
}

int _xaclinesegment_head(void){
    return _xaclinesegment()->GetFirstRecIndex();
}

int _xaclinesegment_next(void){
    return _xaclinesegment()->GetNextRecIndex();
}

int _xaclinesegment_valid_index(int index){
    return _xaclinesegment()->ValidIndex(index) ? _XTRUE : _XFALSE;
}

void _xaclinesegment_free(void){
    delete _xaclinesegment_pointer;
    _xaclinesegment_pointer = NULL;
}

struct _xaclinesegment_para *_xaclinesegment_para_pointer(void){
    _xcompile_assert(sizeof(struct _xaclinesegment_para) == 
                     sizeof(ACLINESEGMENT_PARA),struct_not_match);
    return (struct _xaclinesegment_para *)(_xaclinesegment()->m_ppara);
}

struct _xaclinesegment_real *_xaclinesegment_real_pointer(void){
    _xcompile_assert(sizeof(struct _xaclinesegment_real) == 
                     sizeof(ACLINESEGMENT_REALSTATE),struct_not_match);
    return (struct _xaclinesegment_real *)(_xaclinesegment()->m_prealState);
}

struct _xaclinesegment_real2 *_xaclinesegment_real2_pointer(void){
    _xcompile_assert(sizeof(struct _xaclinesegment_real2) == 
                     sizeof(ACLINESEGMENT_REALFLAG),struct_not_match);
    return (struct _xaclinesegment_real2 *)(_xaclinesegment()->m_prealFlag);
}

struct _xaclinesegment_estm *_xaclinesegment_estm_pointer(void){
    _xcompile_assert(sizeof(struct _xaclinesegment_estm) == 
                     sizeof(ACLINESEGMENT_ESTMSTATE),struct_not_match);
    return (struct _xaclinesegment_estm *)(_xaclinesegment()->m_pestmState);
}

struct _xaclinesegment_estm2 *_xaclinesegment_estm2_pointer(void){
    _xcompile_assert(sizeof(struct _xaclinesegment_estm2) == 
                     sizeof(ACLINESEGMENT_ESTMFLAG),struct_not_match);
    return (struct _xaclinesegment_estm2 *)(_xaclinesegment()->m_pestmFlag);
}

struct _xaclinesegment_estm3 *_xaclinesegment_estm3_pointer(void){
    _xcompile_assert(sizeof(struct _xaclinesegment_estm3) == 
                     sizeof(ACLINESEGMENT_ESTMSTATE3),struct_not_match);
    return (struct _xaclinesegment_estm3 *)(_xaclinesegment()->m_pestmState3);
}

struct _xaclinesegment_calc *_xaclinesegment_calc_pointer(void){
    _xcompile_assert(sizeof(struct _xaclinesegment_calc) == 
                     sizeof(ACLINESEGMENT_CALSTATE),struct_not_match);
    return (struct _xaclinesegment_calc *)(_xaclinesegment()->m_pcalState);
}

struct _xaclinesegment_calc2 *_xaclinesegment_calc2_pointer(void){
    _xcompile_assert(sizeof(struct _xaclinesegment_calc2) == 
                     sizeof(ACLINESEGMENT_CALFLAG),struct_not_match);
    return (struct _xaclinesegment_calc2 *)(_xaclinesegment()->m_pcalFlag);
}

struct _xaclinesegment_calc3 *_xaclinesegment_calc3_pointer(void){
    _xcompile_assert(sizeof(struct _xaclinesegment_calc3) == 
                     sizeof(ACLINESEGMENT_CALSTATE3),struct_not_match);
    return (struct _xaclinesegment_calc3 *)(_xaclinesegment()->m_pcalState3);
}

struct _xaclinesegment_anly *_xaclinesegment_anly_pointer(void){
    _xcompile_assert(sizeof(struct _xaclinesegment_anly) == 
                     sizeof(ACLINESEGMENT_ANALYSTATE),struct_not_match);
    return (struct _xaclinesegment_anly *)(_xaclinesegment()->m_panalyState);
}

struct _xaclinesegment_anly2 *_xaclinesegment_anly2_pointer(void){
    _xcompile_assert(sizeof(struct _xaclinesegment_anly2) == 
                     sizeof(ACLINESEGMENT_ANALYFLAG),struct_not_match);
    return (struct _xaclinesegment_anly2 *)(_xaclinesegment()->m_panalyFlag);
}

struct _xaclinesegment_anly3 *_xaclinesegment_anly3_pointer(void){
    _xcompile_assert(sizeof(struct _xaclinesegment_anly3) == 
                     sizeof(ACLINESEGMENT_ANALYSTATE3),struct_not_match);
    return (struct _xaclinesegment_anly3 *)(_xaclinesegment()->m_panalyState3);
}

/*
 * busbarsection
 */
static RTDBCBusbarsection *_xbusbarsection_pointer = NULL;
static inline RTDBCBusbarsection *_xbusbarsection(void){
    _xcompile_assert(BUSBARSECTION_ID == 610,id_not_match);
    return _xbusbarsection_pointer ? _xbusbarsection_pointer :
           _xbusbarsection_pointer = new RTDBCBusbarsection(__xcsgc3000_database);
}

int _xbusbarsection_ok(void){
    return _xbusbarsection()->CanUse()      && 
           _xbusbarsection()->m_ppara       &&
           _xbusbarsection()->m_prealState  &&
           _xbusbarsection()->m_prealFlag   &&
           _xbusbarsection()->m_pestmState  &&
           _xbusbarsection()->m_pestmFlag   &&
           _xbusbarsection()->m_pestmState3 &&
           _xbusbarsection()->m_pcalState   &&
           _xbusbarsection()->m_pcalFlag    &&
           _xbusbarsection()->m_pcalState3  &&
           _xbusbarsection()->m_panalyState &&
           _xbusbarsection()->m_panalyFlag  &&
           _xbusbarsection()->m_panalyState3;
}

int _xbusbarsection_size(void){
    return _xbusbarsection()->GetCurRecNum();
}

int _xbusbarsection_head(void){
    return _xbusbarsection()->GetFirstRecIndex();
}

int _xbusbarsection_next(void){
    return _xbusbarsection()->GetNextRecIndex();
}

int _xbusbarsection_valid_index(int index){
    return _xbusbarsection()->ValidIndex(index) ? _XTRUE : _XFALSE;
}

void _xbusbarsection_free(void){
    delete _xbusbarsection_pointer;
    _xbusbarsection_pointer = NULL;
}

struct _xbusbarsection_para *_xbusbarsection_para_pointer(void){
    _xcompile_assert(sizeof(struct _xbusbarsection_para) == 
                     sizeof(BUSBARSECTION_PARA),struct_not_match);
    return (struct _xbusbarsection_para *)(_xbusbarsection()->m_ppara);
}

struct _xbusbarsection_real *_xbusbarsection_real_pointer(void){
    _xcompile_assert(sizeof(struct _xbusbarsection_real) == 
                     sizeof(BUSBARSECTION_REALSTATE),struct_not_match);
    return (struct _xbusbarsection_real *)(_xbusbarsection()->m_prealState);
}

struct _xbusbarsection_real2 *_xbusbarsection_real2_pointer(void){
    _xcompile_assert(sizeof(struct _xbusbarsection_real2) == 
                     sizeof(BUSBARSECTION_REALFLAG),struct_not_match);
    return (struct _xbusbarsection_real2 *)(_xbusbarsection()->m_prealFlag);
}

struct _xbusbarsection_estm *_xbusbarsection_estm_pointer(void){
    _xcompile_assert(sizeof(struct _xbusbarsection_estm) == 
                     sizeof(BUSBARSECTION_ESTMSTATE),struct_not_match);
    return (struct _xbusbarsection_estm *)(_xbusbarsection()->m_pestmState);
}

struct _xbusbarsection_estm2 *_xbusbarsection_estm2_pointer(void){
    _xcompile_assert(sizeof(struct _xbusbarsection_estm2) == 
                     sizeof(BUSBARSECTION_ESTMFLAG),struct_not_match);
    return (struct _xbusbarsection_estm2 *)(_xbusbarsection()->m_pestmFlag);
}

struct _xbusbarsection_estm3 *_xbusbarsection_estm3_pointer(void){
    _xcompile_assert(sizeof(struct _xbusbarsection_estm3) == 
                     sizeof(BUSBARSECTION_ESTMSTATE3),struct_not_match);
    return (struct _xbusbarsection_estm3 *)(_xbusbarsection()->m_pestmState3);
}

struct _xbusbarsection_calc *_xbusbarsection_calc_pointer(void){
    _xcompile_assert(sizeof(struct _xbusbarsection_calc) == 
                     sizeof(BUSBARSECTION_CALSTATE),struct_not_match);
    return (struct _xbusbarsection_calc *)(_xbusbarsection()->m_pcalState);
}

struct _xbusbarsection_calc2 *_xbusbarsection_calc2_pointer(void){
    _xcompile_assert(sizeof(struct _xbusbarsection_calc2) == 
                     sizeof(BUSBARSECTION_CALFLAG),struct_not_match);
    return (struct _xbusbarsection_calc2 *)(_xbusbarsection()->m_pcalFlag);
}

struct _xbusbarsection_calc3 *_xbusbarsection_calc3_pointer(void){
    _xcompile_assert(sizeof(struct _xbusbarsection_calc3) == 
                     sizeof(BUSBARSECTION_CALSTATE3),struct_not_match);
    return (struct _xbusbarsection_calc3 *)(_xbusbarsection()->m_pcalState3);
}

struct _xbusbarsection_anly *_xbusbarsection_anly_pointer(void){
    _xcompile_assert(sizeof(struct _xbusbarsection_anly) == 
                     sizeof(BUSBARSECTION_ANALYSTATE),struct_not_match);
    return (struct _xbusbarsection_anly *)(_xbusbarsection()->m_panalyState);
}

struct _xbusbarsection_anly2 *_xbusbarsection_anly2_pointer(void){
    _xcompile_assert(sizeof(struct _xbusbarsection_anly2) == 
                     sizeof(BUSBARSECTION_ANALYFLAG),struct_not_match);
    return (struct _xbusbarsection_anly2 *)(_xbusbarsection()->m_panalyFlag);
}

struct _xbusbarsection_anly3 *_xbusbarsection_anly3_pointer(void){
    _xcompile_assert(sizeof(struct _xbusbarsection_anly3) == 
                     sizeof(BUSBARSECTION_ANALYSTATE3),struct_not_match);
    return (struct _xbusbarsection_anly3 *)(_xbusbarsection()->m_panalyState3);
}

/*
 * equivalentsource
 */
static RTDBCEquivalentsource *_xequivalentsource_pointer = NULL;
static inline RTDBCEquivalentsource *_xequivalentsource(void){
    _xcompile_assert(EQUIVALENTSOURCE_ID == 680,id_not_match);
    return _xequivalentsource_pointer ? _xequivalentsource_pointer :
           _xequivalentsource_pointer = new RTDBCEquivalentsource(__xcsgc3000_database);
}

int _xequivalentsource_ok(void){
    return _xequivalentsource()->CanUse()     && 
           _xequivalentsource()->m_ppara      &&
           _xequivalentsource()->m_preal      &&
           _xequivalentsource()->m_prealState &&
           _xequivalentsource()->m_pestm      &&
           _xequivalentsource()->m_pcal       &&
           _xequivalentsource()->m_panaly;
}

int _xequivalentsource_size(void){
    return _xequivalentsource()->GetCurRecNum();
}

int _xequivalentsource_head(void){
    return _xequivalentsource()->GetFirstRecIndex();
}

int _xequivalentsource_next(void){
    return _xequivalentsource()->GetNextRecIndex();
}

int _xequivalentsource_valid_index(int index){
    return _xequivalentsource()->ValidIndex(index) ? _XTRUE : _XFALSE;
}

void _xequivalentsource_free(void){
    delete _xequivalentsource_pointer;
    _xequivalentsource_pointer = NULL;
}

struct _xequivalentsource_para *_xequivalentsource_para_pointer(void){
    _xcompile_assert(sizeof(struct _xequivalentsource_para) == 
                     sizeof(EQUIVALENTSOURCE_PARA),struct_not_match);
    return (struct _xequivalentsource_para *)(_xequivalentsource()->m_ppara);
}

struct _xequivalentsource_real *_xequivalentsource_real_pointer(void){
    _xcompile_assert(sizeof(struct _xequivalentsource_real) == 
                     sizeof(EQUIVALENTSOURCE_REAL),struct_not_match);
    return (struct _xequivalentsource_real *)(_xequivalentsource()->m_preal);
}

struct _xequivalentsource_real2 *_xequivalentsource_real2_pointer(void){
    _xcompile_assert(sizeof(struct _xequivalentsource_real2) == 
                     sizeof(EQUIVALENTSOURCE_REALSTATE),struct_not_match);
    return (struct _xequivalentsource_real2 *)(_xequivalentsource()->m_prealState);
}

struct _xequivalentsource_estm *_xequivalentsource_estm_pointer(void){
    _xcompile_assert(sizeof(struct _xequivalentsource_estm) == 
                     sizeof(EQUIVALENTSOURCE_ESTM),struct_not_match);
    return (struct _xequivalentsource_estm *)(_xequivalentsource()->m_pestm);
}

struct _xequivalentsource_calc *_xequivalentsource_calc_pointer(void){
    _xcompile_assert(sizeof(struct _xequivalentsource_calc) == 
                     sizeof(EQUIVALENTSOURCE_CAL),struct_not_match);
    return (struct _xequivalentsource_calc *)(_xequivalentsource()->m_pcal);
}

struct _xequivalentsource_anly *_xequivalentsource_anly_pointer(void){
    _xcompile_assert(sizeof(struct _xequivalentsource_anly) == 
                     sizeof(EQUIVALENTSOURCE_ANALY),struct_not_match);
    return (struct _xequivalentsource_anly *)(_xequivalentsource()->m_panaly);
}

/*
 * synchronousmachine
 */
static RTDBCSynchronousMachine *_xsynchronousmachine_pointer = NULL;
static inline RTDBCSynchronousMachine *_xsynchronousmachine(void){
    _xcompile_assert(SYNCHRONOUSMACHINE_ID == 612,id_not_match);
    return _xsynchronousmachine_pointer ? _xsynchronousmachine_pointer :
           _xsynchronousmachine_pointer = new RTDBCSynchronousMachine(__xcsgc3000_database);
}

int _xsynchronousmachine_ok(void){
    return _xsynchronousmachine()->CanUse()      && 
           _xsynchronousmachine()->m_ppara       &&
           _xsynchronousmachine()->m_prealState  &&
           _xsynchronousmachine()->m_prealFlag   &&
           _xsynchronousmachine()->m_pestmState  &&
           _xsynchronousmachine()->m_pestmFlag   &&
           _xsynchronousmachine()->m_pestmState3 &&
           _xsynchronousmachine()->m_pcalState   &&
           _xsynchronousmachine()->m_pcalFlag    &&
           _xsynchronousmachine()->m_pcalState3  &&
           _xsynchronousmachine()->m_panalyState &&
           _xsynchronousmachine()->m_panalyFlag  &&
           _xsynchronousmachine()->m_panalyState3;
}

int _xsynchronousmachine_size(void){
    return _xsynchronousmachine()->GetCurRecNum();
}

int _xsynchronousmachine_head(void){
    return _xsynchronousmachine()->GetFirstRecIndex();
}

int _xsynchronousmachine_next(void){
    return _xsynchronousmachine()->GetNextRecIndex();
}

int _xsynchronousmachine_valid_index(int index){
    return _xsynchronousmachine()->ValidIndex(index) ? _XTRUE : _XFALSE;
}

void _xsynchronousmachine_free(void){
    delete _xsynchronousmachine_pointer;
    _xsynchronousmachine_pointer = NULL;
}

struct _xsynchronousmachine_para *_xsynchronousmachine_para_pointer(void){
    _xcompile_assert(sizeof(struct _xsynchronousmachine_para) == 
                     sizeof(SYNCHRONOUSMACHINE_PARA),struct_not_match);
    return (struct _xsynchronousmachine_para *)(_xsynchronousmachine()->m_ppara);
}

struct _xsynchronousmachine_real *_xsynchronousmachine_real_pointer(void){
    _xcompile_assert(sizeof(struct _xsynchronousmachine_real) == 
                     sizeof(SYNCHRONOUSMACHINE_REALSTATE),struct_not_match);
    return (struct _xsynchronousmachine_real *)(_xsynchronousmachine()->m_prealState);
}

struct _xsynchronousmachine_real2 *_xsynchronousmachine_real2_pointer(void){
    _xcompile_assert(sizeof(struct _xsynchronousmachine_real2) == 
                     sizeof(SYNCHRONOUSMACHINE_REALFLAG),struct_not_match);
    return (struct _xsynchronousmachine_real2 *)(_xsynchronousmachine()->m_prealFlag);
}

struct _xsynchronousmachine_estm *_xsynchronousmachine_estm_pointer(void){
    _xcompile_assert(sizeof(struct _xsynchronousmachine_estm) == 
                     sizeof(SYNCHRONOUSMACHINE_ESTMSTATE),struct_not_match);
    return (struct _xsynchronousmachine_estm *)(_xsynchronousmachine()->m_pestmState);
}

struct _xsynchronousmachine_estm2 *_xsynchronousmachine_estm2_pointer(void){
    _xcompile_assert(sizeof(struct _xsynchronousmachine_estm2) == 
                     sizeof(SYNCHRONOUSMACHINE_ESTMFLAG),struct_not_match);
    return (struct _xsynchronousmachine_estm2 *)(_xsynchronousmachine()->m_pestmFlag);
}

struct _xsynchronousmachine_estm3 *_xsynchronousmachine_estm3_pointer(void){
    _xcompile_assert(sizeof(struct _xsynchronousmachine_estm3) == 
                     sizeof(SYNCHRONOUSMACHINE_ESTMSTATE3),struct_not_match);
    return (struct _xsynchronousmachine_estm3 *)(_xsynchronousmachine()->m_pestmState3);
}

struct _xsynchronousmachine_calc *_xsynchronousmachine_calc_pointer(void){
    _xcompile_assert(sizeof(struct _xsynchronousmachine_calc) == 
                     sizeof(SYNCHRONOUSMACHINE_CALSTATE),struct_not_match);
    return (struct _xsynchronousmachine_calc *)(_xsynchronousmachine()->m_pcalState);
}

struct _xsynchronousmachine_calc2 *_xsynchronousmachine_calc2_pointer(void){
    _xcompile_assert(sizeof(struct _xsynchronousmachine_calc2) == 
                     sizeof(SYNCHRONOUSMACHINE_CALFLAG),struct_not_match);
    return (struct _xsynchronousmachine_calc2 *)(_xsynchronousmachine()->m_pcalFlag);
}

struct _xsynchronousmachine_calc3 *_xsynchronousmachine_calc3_pointer(void){
    _xcompile_assert(sizeof(struct _xsynchronousmachine_calc3) == 
                     sizeof(SYNCHRONOUSMACHINE_CALSTATE3),struct_not_match);
    return (struct _xsynchronousmachine_calc3 *)(_xsynchronousmachine()->m_pcalState3);
}

struct _xsynchronousmachine_anly *_xsynchronousmachine_anly_pointer(void){
    _xcompile_assert(sizeof(struct _xsynchronousmachine_anly) == 
                     sizeof(SYNCHRONOUSMACHINE_ANALYSTATE),struct_not_match);
    return (struct _xsynchronousmachine_anly *)(_xsynchronousmachine()->m_panalyState);
}

struct _xsynchronousmachine_anly2 *_xsynchronousmachine_anly2_pointer(void){
    _xcompile_assert(sizeof(struct _xsynchronousmachine_anly2) == 
                     sizeof(SYNCHRONOUSMACHINE_ANALYFLAG),struct_not_match);
    return (struct _xsynchronousmachine_anly2 *)(_xsynchronousmachine()->m_panalyFlag);
}

struct _xsynchronousmachine_anly3 *_xsynchronousmachine_anly3_pointer(void){
    _xcompile_assert(sizeof(struct _xsynchronousmachine_anly3) == 
                     sizeof(SYNCHRONOUSMACHINE_ANALYSTATE3),struct_not_match);
    return (struct _xsynchronousmachine_anly3 *)(_xsynchronousmachine()->m_panalyState3);
}

/*
 * combustionturbine
 */
static RTDBCCombustionTurbine *_xcombustionturbine_pointer = NULL;
static inline RTDBCCombustionTurbine *_xcombustionturbine(void){
    _xcompile_assert(COMBUSTIONTURBINE_ID == 670,id_not_match);
    return _xcombustionturbine_pointer ? _xcombustionturbine_pointer :
           _xcombustionturbine_pointer = new RTDBCCombustionTurbine(__xcsgc3000_database);
}

int _xcombustionturbine_ok(void){
    return _xcombustionturbine()->CanUse() && 
           _xcombustionturbine()->m_ppara;
}

int _xcombustionturbine_size(void){
    return _xcombustionturbine()->GetCurRecNum();
}

int _xcombustionturbine_head(void){
    return _xcombustionturbine()->GetFirstRecIndex();
}

int _xcombustionturbine_next(void){
    return _xcombustionturbine()->GetNextRecIndex();
}

int _xcombustionturbine_valid_index(int index){
    return _xcombustionturbine()->ValidIndex(index) ? _XTRUE : _XFALSE;
}

void _xcombustionturbine_free(void){
    delete _xcombustionturbine_pointer;
    _xcombustionturbine_pointer = NULL;
}

struct _xcombustionturbine_para *_xcombustionturbine_para_pointer(void){
    _xcompile_assert(sizeof(struct _xcombustionturbine_para) == 
                     sizeof(COMBUSTIONTURBINE_PARA),struct_not_match);
    return (struct _xcombustionturbine_para *)(_xcombustionturbine()->m_ppara);
}

