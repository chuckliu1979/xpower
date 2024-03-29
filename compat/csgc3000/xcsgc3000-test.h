/*
** Copyright (C) 2011 XXX all rights reserved.
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XCSGC3000_TEST_H_
#define _XCSGC3000_TEST_H_

#include "structs/xcsgc3000-struct4datetime.h"
#include "structs/xcsgc3000-struct4company.h"
#include "structs/xcsgc3000-struct4loadarea.h"
#include "structs/xcsgc3000-struct4substation.h"
#include "structs/xcsgc3000-struct4voltagelevel.h"
#include "structs/xcsgc3000-struct4powertransformer.h"
#include "structs/xcsgc3000-struct4bay.h"
#include "structs/xcsgc3000-struct4connectivitynode.h"
#include "structs/xcsgc3000-struct4terminal.h"
#include "structs/xcsgc3000-struct4measurement.h"
#include "structs/xcsgc3000-struct4analoginput.h"
#include "structs/xcsgc3000-struct4statusinput.h"
#include "structs/xcsgc3000-struct4breaker.h"
#include "structs/xcsgc3000-struct4disconnector.h"
#include "structs/xcsgc3000-struct4grounddisconnector.h"
#include "structs/xcsgc3000-struct4energyconsumer.h"
#include "structs/xcsgc3000-struct4compensator.h"
#include "structs/xcsgc3000-struct4staticvarcompensator.h"
#include "structs/xcsgc3000-struct4transformerwinding.h"
#include "structs/xcsgc3000-struct4tapchanger.h"
#include "structs/xcsgc3000-struct4aclinesegment.h"
#include "structs/xcsgc3000-struct4busbarsection.h"
#include "structs/xcsgc3000-struct4equivalentsource.h"
#include "structs/xcsgc3000-struct4synchronousmachine.h"
#include "structs/xcsgc3000-struct4combustionturbine.h"

#define _XTYPEDEF(type1,type2) typedef struct type1 type2;
#define _XCLASS_BEG(name)                                \
              class name {                               \
              public:                                    \
                  name(short dbNo){                      \
                      SetStructPointer();                \
                  }                                      \
                  virtual ~name(){}                      \
              public:                                    \
                  int CanUse(){return 1;}                \
                  int ValidIndex(int x){return 0;}       \
                  int GetCurRecNum(){return 0;}          \
                  int GetFirstRecIndex(){return -1;}     \
                  int GetNextRecIndex(){return 0;}       \
              protected:                                 \
                  int SetStructPointer(){return 0;}      \
              public:
                
#define _XCLASS_END };
#define _XCLASS_MEMBER(type,member) type *m_p##member;

_XTYPEDEF(_xdatetime, SDateTime)

_XTYPEDEF(_xcompany_para, COMPANY_PARA)
_XTYPEDEF(_xcompany_real, COMPANY_REAL)
_XTYPEDEF(_xcompany_estm, COMPANY_ESTM)
_XTYPEDEF(_xcompany_calc, COMPANY_CAL)
_XTYPEDEF(_xcompany_anly, COMPANY_ANALY)

_XCLASS_BEG(RTDBCCompany)
_XCLASS_MEMBER(COMPANY_PARA,para)
_XCLASS_MEMBER(COMPANY_REAL,real)
_XCLASS_MEMBER(COMPANY_ESTM,estm)
_XCLASS_MEMBER(COMPANY_CAL, cal)
_XCLASS_MEMBER(COMPANY_ANALY,analy)
_XCLASS_END

_XTYPEDEF(_xloadarea_para, LOADAREA_PARA)
_XTYPEDEF(_xloadarea_real, LOADAREA_REAL)
_XTYPEDEF(_xloadarea_estm, LOADAREA_ESTM)
_XTYPEDEF(_xloadarea_calc, LOADAREA_CAL)
_XTYPEDEF(_xloadarea_anly, LOADAREA_ANALY)

_XCLASS_BEG(RTDBCLoadArea)
_XCLASS_MEMBER(LOADAREA_PARA,para)
_XCLASS_MEMBER(LOADAREA_REAL,real)
_XCLASS_MEMBER(LOADAREA_ESTM,estm)
_XCLASS_MEMBER(LOADAREA_CAL, cal)
_XCLASS_MEMBER(LOADAREA_ANALY,analy)
_XCLASS_END

_XTYPEDEF(_xsubstation_para, SUBSTATION_PARA)
_XTYPEDEF(_xsubstation_real, SUBSTATION_REAL)
_XTYPEDEF(_xsubstation_estm, SUBSTATION_ESTM)
_XTYPEDEF(_xsubstation_calc, SUBSTATION_CAL)
_XTYPEDEF(_xsubstation_anly, SUBSTATION_ANALY)
_XTYPEDEF(_xsubstation_flag, SUBSTATION_FLAG)

_XCLASS_BEG(RTDBCSubstation)
_XCLASS_MEMBER(SUBSTATION_PARA,para)
_XCLASS_MEMBER(SUBSTATION_REAL,real)
_XCLASS_MEMBER(SUBSTATION_ESTM,estm)
_XCLASS_MEMBER(SUBSTATION_CAL, cal)
_XCLASS_MEMBER(SUBSTATION_ANALY,analy)
_XCLASS_MEMBER(SUBSTATION_FLAG,flag)
_XCLASS_END

_XTYPEDEF(_xvoltagelevel_para, VOLTAGELEVEL_PARA)

_XCLASS_BEG(RTDBCVoltagelevel)
_XCLASS_MEMBER(VOLTAGELEVEL_PARA,para)
_XCLASS_END

_XTYPEDEF(_xpowertransformer_para, POWERTRANSFORMER_PARA)

_XCLASS_BEG(RTDBCPowertransformer)
_XCLASS_MEMBER(POWERTRANSFORMER_PARA,para)
_XCLASS_END

_XTYPEDEF(_xbay_para, BAY_PARA)

_XCLASS_BEG(RTDBCBay)
_XCLASS_MEMBER(BAY_PARA,para)
_XCLASS_END

_XTYPEDEF(_xconnectivitynode_para, CONNECTIVITYNODE_PARA)
_XTYPEDEF(_xconnectivitynode_real, CONNECTIVITYNODE_REAL)
_XTYPEDEF(_xconnectivitynode_estm, CONNECTIVITYNODE_ESTM)
_XTYPEDEF(_xconnectivitynode_calc, CONNECTIVITYNODE_CAL)
_XTYPEDEF(_xconnectivitynode_anly, CONNECTIVITYNODE_ANALY)

_XCLASS_BEG(RTDBCConnectivitynode)
_XCLASS_MEMBER(CONNECTIVITYNODE_PARA,para)
_XCLASS_MEMBER(CONNECTIVITYNODE_REAL,real)
_XCLASS_MEMBER(CONNECTIVITYNODE_ESTM,estm)
_XCLASS_MEMBER(CONNECTIVITYNODE_CAL, cal)
_XCLASS_MEMBER(CONNECTIVITYNODE_ANALY,analy)
_XCLASS_END

_XTYPEDEF(_xterminal_para, TERMINAL_PARA)

_XCLASS_BEG(RTDBCTerminal)
_XCLASS_MEMBER(TERMINAL_PARA,para)
_XCLASS_END

_XTYPEDEF(_xmeasurement_data, MEASUREMENT_DATA)
_XTYPEDEF(_xmeasurement_estm, MEASUREMENT_ESTM)
_XTYPEDEF(_xmeasurement_calc, MEASUREMENT_CAL)
_XTYPEDEF(_xmeasurement_anly, MEASUREMENT_ANALY)

_XCLASS_BEG(RTDBCMeasurement)
_XCLASS_MEMBER(MEASUREMENT_DATA,data)
_XCLASS_MEMBER(MEASUREMENT_ESTM,estm)
_XCLASS_MEMBER(MEASUREMENT_CAL, cal)
_XCLASS_MEMBER(MEASUREMENT_ANALY,analy)
_XCLASS_END

_XTYPEDEF(_xanaloginput_data, ANALOGINPUT_DATA)
_XTYPEDEF(_xanaloginput_anly, ANALOGINPUT_ANALYSIS)

_XCLASS_BEG(RTDBCAnalogInput)
_XCLASS_MEMBER(ANALOGINPUT_DATA,data)
_XCLASS_MEMBER(ANALOGINPUT_ANALYSIS,analysis)
_XCLASS_END

_XTYPEDEF(_xstatusinput_data, STATUSINPUT_DATA)
_XTYPEDEF(_xstatusinput_anly, STATUSINPUT_ANALYSIS)

_XCLASS_BEG(RTDBCStatusInput)
_XCLASS_MEMBER(STATUSINPUT_DATA,data)
_XCLASS_MEMBER(STATUSINPUT_ANALYSIS,analysis)
_XCLASS_END

_XTYPEDEF(_xbreaker_para ,BREAKER_PARA)
_XTYPEDEF(_xbreaker_real ,BREAKER_REALSTATE)
_XTYPEDEF(_xbreaker_real2,BREAKER_REALFLAG)
_XTYPEDEF(_xbreaker_estm ,BREAKER_ESTMSTATE)
_XTYPEDEF(_xbreaker_estm2,BREAKER_ESTMFLAG)
_XTYPEDEF(_xbreaker_estm3,BREAKER_ESTMSTATE3)
_XTYPEDEF(_xbreaker_calc ,BREAKER_CALSTATE)
_XTYPEDEF(_xbreaker_calc2,BREAKER_CALFLAG)
_XTYPEDEF(_xbreaker_calc3,BREAKER_CALSTATE3)
_XTYPEDEF(_xbreaker_anly ,BREAKER_ANALYSTATE)
_XTYPEDEF(_xbreaker_anly2,BREAKER_ANALYFLAG)
_XTYPEDEF(_xbreaker_anly3,BREAKER_ANALYSTATE3)

_XCLASS_BEG(RTDBCBreaker)
_XCLASS_MEMBER(BREAKER_PARA, para)       
_XCLASS_MEMBER(BREAKER_REALSTATE,realState)  
_XCLASS_MEMBER(BREAKER_REALFLAG,realFlag)   
_XCLASS_MEMBER(BREAKER_ESTMSTATE,estmState)  
_XCLASS_MEMBER(BREAKER_ESTMFLAG,estmFlag)   
_XCLASS_MEMBER(BREAKER_ESTMSTATE3,estmState3) 
_XCLASS_MEMBER(BREAKER_CALSTATE,calState)   
_XCLASS_MEMBER(BREAKER_CALFLAG,calFlag)    
_XCLASS_MEMBER(BREAKER_CALSTATE3,calState3)  
_XCLASS_MEMBER(BREAKER_ANALYSTATE,analyState) 
_XCLASS_MEMBER(BREAKER_ANALYFLAG,analyFlag)  
_XCLASS_MEMBER(BREAKER_ANALYSTATE3,analyState3)
_XCLASS_END

_XTYPEDEF(_xdisconnector_para ,DISCONNECTOR_PARA)
_XTYPEDEF(_xdisconnector_real ,DISCONNECTOR_REALSTATE)
_XTYPEDEF(_xdisconnector_real2,DISCONNECTOR_REALFLAG)
_XTYPEDEF(_xdisconnector_estm ,DISCONNECTOR_ESTMSTATE)
_XTYPEDEF(_xdisconnector_estm2,DISCONNECTOR_ESTMFLAG)
_XTYPEDEF(_xdisconnector_estm3,DISCONNECTOR_ESTMSTATE3)
_XTYPEDEF(_xdisconnector_calc ,DISCONNECTOR_CALSTATE)
_XTYPEDEF(_xdisconnector_calc2,DISCONNECTOR_CALFLAG)
_XTYPEDEF(_xdisconnector_calc3,DISCONNECTOR_CALSTATE3)
_XTYPEDEF(_xdisconnector_anly ,DISCONNECTOR_ANALYSTATE)
_XTYPEDEF(_xdisconnector_anly2,DISCONNECTOR_ANALYFLAG)
_XTYPEDEF(_xdisconnector_anly3,DISCONNECTOR_ANALYSTATE3)

_XCLASS_BEG(RTDBCDisconnector)
_XCLASS_MEMBER(DISCONNECTOR_PARA,para)       
_XCLASS_MEMBER(DISCONNECTOR_REALSTATE,realState)  
_XCLASS_MEMBER(DISCONNECTOR_REALFLAG,realFlag)   
_XCLASS_MEMBER(DISCONNECTOR_ESTMSTATE,estmState)  
_XCLASS_MEMBER(DISCONNECTOR_ESTMFLAG,estmFlag)   
_XCLASS_MEMBER(DISCONNECTOR_ESTMSTATE3,estmState3) 
_XCLASS_MEMBER(DISCONNECTOR_CALSTATE,calState)   
_XCLASS_MEMBER(DISCONNECTOR_CALFLAG,calFlag)    
_XCLASS_MEMBER(DISCONNECTOR_CALSTATE3,calState3)  
_XCLASS_MEMBER(DISCONNECTOR_ANALYSTATE,analyState) 
_XCLASS_MEMBER(DISCONNECTOR_ANALYFLAG,analyFlag)  
_XCLASS_MEMBER(DISCONNECTOR_ANALYSTATE3,analyState3)
_XCLASS_END

_XTYPEDEF(_xgrounddisconnector_para ,GROUNDDISCONNECTOR_PARA)
_XTYPEDEF(_xgrounddisconnector_real ,GROUNDDISCONNECTOR_REALSTATE)
_XTYPEDEF(_xgrounddisconnector_real2,GROUNDDISCONNECTOR_REALFLAG)
_XTYPEDEF(_xgrounddisconnector_estm ,GROUNDDISCONNECTOR_ESTMSTATE)
_XTYPEDEF(_xgrounddisconnector_estm2,GROUNDDISCONNECTOR_ESTMFLAG)
_XTYPEDEF(_xgrounddisconnector_estm3,GROUNDDISCONNECTOR_ESTMSTATE3)
_XTYPEDEF(_xgrounddisconnector_calc ,GROUNDDISCONNECTOR_CALSTATE)
_XTYPEDEF(_xgrounddisconnector_calc2,GROUNDDISCONNECTOR_CALFLAG)
_XTYPEDEF(_xgrounddisconnector_calc3,GROUNDDISCONNECTOR_CALSTATE3)
_XTYPEDEF(_xgrounddisconnector_anly ,GROUNDDISCONNECTOR_ANALYSTATE)
_XTYPEDEF(_xgrounddisconnector_anly2,GROUNDDISCONNECTOR_ANALYFLAG)
_XTYPEDEF(_xgrounddisconnector_anly3,GROUNDDISCONNECTOR_ANALYSTATE3)

_XCLASS_BEG(RTDBCGroundDisconnector)
_XCLASS_MEMBER(GROUNDDISCONNECTOR_PARA,para)
_XCLASS_MEMBER(GROUNDDISCONNECTOR_REALSTATE,realState)
_XCLASS_MEMBER(GROUNDDISCONNECTOR_REALFLAG,realFlag)
_XCLASS_MEMBER(GROUNDDISCONNECTOR_ESTMSTATE,estmState)
_XCLASS_MEMBER(GROUNDDISCONNECTOR_ESTMFLAG,estmFlag)
_XCLASS_MEMBER(GROUNDDISCONNECTOR_ESTMSTATE3,estmState3)
_XCLASS_MEMBER(GROUNDDISCONNECTOR_CALSTATE,calState)
_XCLASS_MEMBER(GROUNDDISCONNECTOR_CALFLAG,calFlag)
_XCLASS_MEMBER(GROUNDDISCONNECTOR_CALSTATE3,calState3)
_XCLASS_MEMBER(GROUNDDISCONNECTOR_ANALYSTATE,analyState)
_XCLASS_MEMBER(GROUNDDISCONNECTOR_ANALYFLAG,analyFlag)
_XCLASS_MEMBER(GROUNDDISCONNECTOR_ANALYSTATE3,analyState3)
_XCLASS_END

_XTYPEDEF(_xenergyconsumer_para ,ENERGYCONSUMER_PARA)
_XTYPEDEF(_xenergyconsumer_real ,ENERGYCONSUMER_REALSTATE)
_XTYPEDEF(_xenergyconsumer_real2,ENERGYCONSUMER_REALFLAG)
_XTYPEDEF(_xenergyconsumer_estm ,ENERGYCONSUMER_ESTMSTATE)
_XTYPEDEF(_xenergyconsumer_estm2,ENERGYCONSUMER_ESTMFALG)
_XTYPEDEF(_xenergyconsumer_estm3,ENERGYCONSUMER_ESTMSTATE3)
_XTYPEDEF(_xenergyconsumer_calc ,ENERGYCONSUMER_CALSTATE)
_XTYPEDEF(_xenergyconsumer_calc2,ENERGYCONSUMER_CALFLAG)
_XTYPEDEF(_xenergyconsumer_calc3,ENERGYCONSUMER_CALSTATE3)
_XTYPEDEF(_xenergyconsumer_anly ,ENERGYCONSUMER_ANALYSTATE)
_XTYPEDEF(_xenergyconsumer_anly2,ENERGYCONSUMER_ANALYFLAG)
_XTYPEDEF(_xenergyconsumer_anly3,ENERGYCONSUMER_ANALYSTATE3)

_XCLASS_BEG(RTDBCEnergyconsumer)
_XCLASS_MEMBER(ENERGYCONSUMER_PARA,para)
_XCLASS_MEMBER(ENERGYCONSUMER_REALSTATE,realState)
_XCLASS_MEMBER(ENERGYCONSUMER_REALFLAG,realFlag)
_XCLASS_MEMBER(ENERGYCONSUMER_ESTMSTATE,estmState)
_XCLASS_MEMBER(ENERGYCONSUMER_ESTMFALG,estmFlag)
_XCLASS_MEMBER(ENERGYCONSUMER_ESTMSTATE3,estmState3)
_XCLASS_MEMBER(ENERGYCONSUMER_CALSTATE,calState)
_XCLASS_MEMBER(ENERGYCONSUMER_CALFLAG,calFlag)
_XCLASS_MEMBER(ENERGYCONSUMER_CALSTATE3,calState3)
_XCLASS_MEMBER(ENERGYCONSUMER_ANALYSTATE,analyState)
_XCLASS_MEMBER(ENERGYCONSUMER_ANALYFLAG,analyFlag)
_XCLASS_MEMBER(ENERGYCONSUMER_ANALYSTATE3,analyState3)
_XCLASS_END

_XTYPEDEF(_xcompensator_para ,COMPENSATOR_PARA)
_XTYPEDEF(_xcompensator_real ,COMPENSATOR_REALSTATE)
_XTYPEDEF(_xcompensator_real2,COMPENSATOR_REALFLAG)
_XTYPEDEF(_xcompensator_estm ,COMPENSATOR_ESTMSTATE)
_XTYPEDEF(_xcompensator_estm2,COMPENSATOR_ESTMFLAG)
_XTYPEDEF(_xcompensator_estm3,COMPENSATOR_ESTMSTATE3)
_XTYPEDEF(_xcompensator_calc ,COMPENSATOR_CALSTATE)
_XTYPEDEF(_xcompensator_calc2,COMPENSATOR_CALFLAG)
_XTYPEDEF(_xcompensator_calc3,COMPENSATOR_CALSTATE3)
_XTYPEDEF(_xcompensator_anly ,COMPENSATOR_ANALYSTATE)
_XTYPEDEF(_xcompensator_anly2,COMPENSATOR_ANALYFLAG)
_XTYPEDEF(_xcompensator_anly3,COMPENSATOR_ANALYSTATE3)

_XCLASS_BEG(RTDBCCompensator)
_XCLASS_MEMBER(COMPENSATOR_PARA,para)
_XCLASS_MEMBER(COMPENSATOR_REALSTATE,realState)
_XCLASS_MEMBER(COMPENSATOR_REALFLAG,realFlag)
_XCLASS_MEMBER(COMPENSATOR_ESTMSTATE,estmState)
_XCLASS_MEMBER(COMPENSATOR_ESTMFLAG,estmFlag)
_XCLASS_MEMBER(COMPENSATOR_ESTMSTATE3,estmState3)
_XCLASS_MEMBER(COMPENSATOR_CALSTATE,calState)
_XCLASS_MEMBER(COMPENSATOR_CALFLAG,calFlag)
_XCLASS_MEMBER(COMPENSATOR_CALSTATE3,calState3)
_XCLASS_MEMBER(COMPENSATOR_ANALYSTATE,analyState)
_XCLASS_MEMBER(COMPENSATOR_ANALYFLAG,analyFlag)
_XCLASS_MEMBER(COMPENSATOR_ANALYSTATE3,analyState3)
_XCLASS_END

_XTYPEDEF(_xstaticvarcompensator_para ,STATICVARCOMPENSATOR_PARA)
_XTYPEDEF(_xstaticvarcompensator_real ,STATICVARCOMPENSATOR_REALSTATE)
_XTYPEDEF(_xstaticvarcompensator_real2,STATICVARCOMPENSATOR_REALFLAG)
_XTYPEDEF(_xstaticvarcompensator_estm ,STATICVARCOMPENSATOR_ESTMSTATE)
_XTYPEDEF(_xstaticvarcompensator_estm2,STATICVARCOMPENSATOR_ESTMFLAG)
_XTYPEDEF(_xstaticvarcompensator_estm3,STATICVARCOMPENSATOR_ESTMSTATE3)
_XTYPEDEF(_xstaticvarcompensator_calc ,STATICVARCOMPENSATOR_CALSTATE)
_XTYPEDEF(_xstaticvarcompensator_calc2,STATICVARCOMPENSATOR_CALFLAG)
_XTYPEDEF(_xstaticvarcompensator_calc3,STATICVARCOMPENSATOR_CALSTATE3)
_XTYPEDEF(_xstaticvarcompensator_anly ,STATICVARCOMPENSATOR_ANALYSTATE)
_XTYPEDEF(_xstaticvarcompensator_anly2,STATICVARCOMPENSATOR_ANALYFLAG)
_XTYPEDEF(_xstaticvarcompensator_anly3,STATICVARCOMPENSATOR_ANALYSTATE3)

_XCLASS_BEG(RTDBCStaticVarCompensator)
_XCLASS_MEMBER(STATICVARCOMPENSATOR_PARA,para)
_XCLASS_MEMBER(STATICVARCOMPENSATOR_REALSTATE,realState)
_XCLASS_MEMBER(STATICVARCOMPENSATOR_REALFLAG,realFlag)
_XCLASS_MEMBER(STATICVARCOMPENSATOR_ESTMSTATE,estmState)
_XCLASS_MEMBER(STATICVARCOMPENSATOR_ESTMFLAG,estmFlag)
_XCLASS_MEMBER(STATICVARCOMPENSATOR_ESTMSTATE3,estmState3)
_XCLASS_MEMBER(STATICVARCOMPENSATOR_CALSTATE,calState)
_XCLASS_MEMBER(STATICVARCOMPENSATOR_CALFLAG,calFlag)
_XCLASS_MEMBER(STATICVARCOMPENSATOR_CALSTATE3,calState3)
_XCLASS_MEMBER(STATICVARCOMPENSATOR_ANALYSTATE,analyState)
_XCLASS_MEMBER(STATICVARCOMPENSATOR_ANALYFLAG,analyFlag)
_XCLASS_MEMBER(STATICVARCOMPENSATOR_ANALYSTATE3,analyState3)
_XCLASS_END

_XTYPEDEF(_xtransformerwinding_para ,TRANSFORMERWINDING_PARA)
_XTYPEDEF(_xtransformerwinding_real ,TRANSFORMERWINDING_REALSTATE)
_XTYPEDEF(_xtransformerwinding_real2,TRANSFORMERWINDING_REALFLAG)
_XTYPEDEF(_xtransformerwinding_estm ,TRANSFORMERWINDING_ESTMSTATE)
_XTYPEDEF(_xtransformerwinding_estm2,TRANSFORMERWINDING_ESTMFLAG)
_XTYPEDEF(_xtransformerwinding_estm3,TRANSFORMERWINDING_ESTMSTATE3)
_XTYPEDEF(_xtransformerwinding_calc ,TRANSFORMERWINDING_CALSTATE)
_XTYPEDEF(_xtransformerwinding_calc2,TRANSFORMERWINDING_CALFLAG)
_XTYPEDEF(_xtransformerwinding_calc3,TRANSFORMERWINDING_CALSTATE3)
_XTYPEDEF(_xtransformerwinding_anly ,TRANSFORMERWINDING_ANALYSTATE)
_XTYPEDEF(_xtransformerwinding_anly2,TRANSFORMERWINDING_ANALYFLAG)
_XTYPEDEF(_xtransformerwinding_anly3,TRANSFORMERWINDING_ANALYSTATE3)

_XCLASS_BEG(RTDBCTransformerwinding)
_XCLASS_MEMBER(TRANSFORMERWINDING_PARA,para)
_XCLASS_MEMBER(TRANSFORMERWINDING_REALSTATE,realState)
_XCLASS_MEMBER(TRANSFORMERWINDING_REALFLAG,realFlag)
_XCLASS_MEMBER(TRANSFORMERWINDING_ESTMSTATE,estmState)
_XCLASS_MEMBER(TRANSFORMERWINDING_ESTMFLAG,estmFlag)
_XCLASS_MEMBER(TRANSFORMERWINDING_ESTMSTATE3,estmState3)
_XCLASS_MEMBER(TRANSFORMERWINDING_CALSTATE,calState)
_XCLASS_MEMBER(TRANSFORMERWINDING_CALFLAG,calFlag)
_XCLASS_MEMBER(TRANSFORMERWINDING_CALSTATE3,calState3)
_XCLASS_MEMBER(TRANSFORMERWINDING_ANALYSTATE,analyState)
_XCLASS_MEMBER(TRANSFORMERWINDING_ANALYFLAG,analyFlag)
_XCLASS_MEMBER(TRANSFORMERWINDING_ANALYSTATE3,analyState3)
_XCLASS_END

_XTYPEDEF(_xtapchanger_para,TAPCHANGER_PARA)
_XTYPEDEF(_xtapchanger_real,TAPCHANGER_REALSTATE)
_XTYPEDEF(_xtapchanger_estm,TAPCHANGER_ESTMSTATE)
_XTYPEDEF(_xtapchanger_calc,TAPCHANGER_CALSTATE)
_XTYPEDEF(_xtapchanger_anly,TAPCHANGER_ANALYSTATE)

_XCLASS_BEG(RTDBCTapchanger)
_XCLASS_MEMBER(TAPCHANGER_PARA,para)
_XCLASS_MEMBER(TAPCHANGER_REALSTATE,realState)
_XCLASS_MEMBER(TAPCHANGER_ESTMSTATE,estmState)
_XCLASS_MEMBER(TAPCHANGER_CALSTATE,calState)
_XCLASS_MEMBER(TAPCHANGER_ANALYSTATE,analyState)
_XCLASS_END

_XTYPEDEF(_xaclinesegment_para ,ACLINESEGMENT_PARA)
_XTYPEDEF(_xaclinesegment_real ,ACLINESEGMENT_REALSTATE)
_XTYPEDEF(_xaclinesegment_real2,ACLINESEGMENT_REALFLAG)
_XTYPEDEF(_xaclinesegment_estm ,ACLINESEGMENT_ESTMSTATE)
_XTYPEDEF(_xaclinesegment_estm2,ACLINESEGMENT_ESTMFLAG)
_XTYPEDEF(_xaclinesegment_estm3,ACLINESEGMENT_ESTMSTATE3)
_XTYPEDEF(_xaclinesegment_calc ,ACLINESEGMENT_CALSTATE)
_XTYPEDEF(_xaclinesegment_calc2,ACLINESEGMENT_CALFLAG)
_XTYPEDEF(_xaclinesegment_calc3,ACLINESEGMENT_CALSTATE3)
_XTYPEDEF(_xaclinesegment_anly ,ACLINESEGMENT_ANALYSTATE)
_XTYPEDEF(_xaclinesegment_anly2,ACLINESEGMENT_ANALYFLAG)
_XTYPEDEF(_xaclinesegment_anly3,ACLINESEGMENT_ANALYSTATE3)

_XCLASS_BEG(RTDBCAclinesegment)
_XCLASS_MEMBER(ACLINESEGMENT_PARA,para)
_XCLASS_MEMBER(ACLINESEGMENT_REALSTATE,realState)
_XCLASS_MEMBER(ACLINESEGMENT_REALFLAG,realFlag)
_XCLASS_MEMBER(ACLINESEGMENT_ESTMSTATE,estmState)
_XCLASS_MEMBER(ACLINESEGMENT_ESTMFLAG,estmFlag)
_XCLASS_MEMBER(ACLINESEGMENT_ESTMSTATE3,estmState3)
_XCLASS_MEMBER(ACLINESEGMENT_CALSTATE,calState)
_XCLASS_MEMBER(ACLINESEGMENT_CALFLAG,calFlag)
_XCLASS_MEMBER(ACLINESEGMENT_CALSTATE3,calState3)
_XCLASS_MEMBER(ACLINESEGMENT_ANALYSTATE,analyState)
_XCLASS_MEMBER(ACLINESEGMENT_ANALYFLAG,analyFlag)
_XCLASS_MEMBER(ACLINESEGMENT_ANALYSTATE3,analyState3)
_XCLASS_END

_XTYPEDEF(_xbusbarsection_para ,BUSBARSECTION_PARA)
_XTYPEDEF(_xbusbarsection_real ,BUSBARSECTION_REALSTATE)
_XTYPEDEF(_xbusbarsection_real2,BUSBARSECTION_REALFLAG)
_XTYPEDEF(_xbusbarsection_estm ,BUSBARSECTION_ESTMSTATE)
_XTYPEDEF(_xbusbarsection_estm2,BUSBARSECTION_ESTMFLAG)
_XTYPEDEF(_xbusbarsection_estm3,BUSBARSECTION_ESTMSTATE3)
_XTYPEDEF(_xbusbarsection_calc ,BUSBARSECTION_CALSTATE)
_XTYPEDEF(_xbusbarsection_calc2,BUSBARSECTION_CALFLAG)
_XTYPEDEF(_xbusbarsection_calc3,BUSBARSECTION_CALSTATE3)
_XTYPEDEF(_xbusbarsection_anly ,BUSBARSECTION_ANALYSTATE)
_XTYPEDEF(_xbusbarsection_anly2,BUSBARSECTION_ANALYFLAG)
_XTYPEDEF(_xbusbarsection_anly3,BUSBARSECTION_ANALYSTATE3)

_XCLASS_BEG(RTDBCBusbarsection)
_XCLASS_MEMBER(BUSBARSECTION_PARA,para)
_XCLASS_MEMBER(BUSBARSECTION_REALSTATE,realState)
_XCLASS_MEMBER(BUSBARSECTION_REALFLAG,realFlag)
_XCLASS_MEMBER(BUSBARSECTION_ESTMSTATE,estmState)
_XCLASS_MEMBER(BUSBARSECTION_ESTMFLAG,estmFlag)
_XCLASS_MEMBER(BUSBARSECTION_ESTMSTATE3,estmState3)
_XCLASS_MEMBER(BUSBARSECTION_CALSTATE,calState)
_XCLASS_MEMBER(BUSBARSECTION_CALFLAG,calFlag)
_XCLASS_MEMBER(BUSBARSECTION_CALSTATE3,calState3)
_XCLASS_MEMBER(BUSBARSECTION_ANALYSTATE,analyState)
_XCLASS_MEMBER(BUSBARSECTION_ANALYFLAG,analyFlag)
_XCLASS_MEMBER(BUSBARSECTION_ANALYSTATE3,analyState3)
_XCLASS_END

_XTYPEDEF(_xequivalentsource_para ,EQUIVALENTSOURCE_PARA)
_XTYPEDEF(_xequivalentsource_real ,EQUIVALENTSOURCE_REAL)
_XTYPEDEF(_xequivalentsource_real2,EQUIVALENTSOURCE_REALSTATE)
_XTYPEDEF(_xequivalentsource_estm ,EQUIVALENTSOURCE_ESTM)
_XTYPEDEF(_xequivalentsource_calc ,EQUIVALENTSOURCE_CAL)
_XTYPEDEF(_xequivalentsource_anly ,EQUIVALENTSOURCE_ANALY)

_XCLASS_BEG(RTDBCEquivalentsource)
_XCLASS_MEMBER(EQUIVALENTSOURCE_PARA,para)
_XCLASS_MEMBER(EQUIVALENTSOURCE_REAL,real)
_XCLASS_MEMBER(EQUIVALENTSOURCE_REALSTATE,realState)
_XCLASS_MEMBER(EQUIVALENTSOURCE_ESTM,estm)
_XCLASS_MEMBER(EQUIVALENTSOURCE_CAL,cal)
_XCLASS_MEMBER(EQUIVALENTSOURCE_ANALY,analy)
_XCLASS_END

_XTYPEDEF(_xsynchronousmachine_para ,SYNCHRONOUSMACHINE_PARA)
_XTYPEDEF(_xsynchronousmachine_real ,SYNCHRONOUSMACHINE_REALSTATE)
_XTYPEDEF(_xsynchronousmachine_real2,SYNCHRONOUSMACHINE_REALFLAG)
_XTYPEDEF(_xsynchronousmachine_estm ,SYNCHRONOUSMACHINE_ESTMSTATE)
_XTYPEDEF(_xsynchronousmachine_estm2,SYNCHRONOUSMACHINE_ESTMFLAG)
_XTYPEDEF(_xsynchronousmachine_estm3,SYNCHRONOUSMACHINE_ESTMSTATE3)
_XTYPEDEF(_xsynchronousmachine_calc ,SYNCHRONOUSMACHINE_CALSTATE)
_XTYPEDEF(_xsynchronousmachine_calc2,SYNCHRONOUSMACHINE_CALFLAG)
_XTYPEDEF(_xsynchronousmachine_calc3,SYNCHRONOUSMACHINE_CALSTATE3)
_XTYPEDEF(_xsynchronousmachine_anly ,SYNCHRONOUSMACHINE_ANALYSTATE)
_XTYPEDEF(_xsynchronousmachine_anly2,SYNCHRONOUSMACHINE_ANALYFLAG)
_XTYPEDEF(_xsynchronousmachine_anly3,SYNCHRONOUSMACHINE_ANALYSTATE3)

_XCLASS_BEG(RTDBCSynchronousMachine)
_XCLASS_MEMBER(SYNCHRONOUSMACHINE_PARA ,para)
_XCLASS_MEMBER(SYNCHRONOUSMACHINE_REALSTATE,realState)
_XCLASS_MEMBER(SYNCHRONOUSMACHINE_REALFLAG,realFlag)
_XCLASS_MEMBER(SYNCHRONOUSMACHINE_ESTMSTATE ,estmState)
_XCLASS_MEMBER(SYNCHRONOUSMACHINE_ESTMFLAG,estmFlag)
_XCLASS_MEMBER(SYNCHRONOUSMACHINE_ESTMSTATE3,estmState3)
_XCLASS_MEMBER(SYNCHRONOUSMACHINE_CALSTATE ,calState)
_XCLASS_MEMBER(SYNCHRONOUSMACHINE_CALFLAG,calFlag)
_XCLASS_MEMBER(SYNCHRONOUSMACHINE_CALSTATE3,calState3)
_XCLASS_MEMBER(SYNCHRONOUSMACHINE_ANALYSTATE ,analyState)
_XCLASS_MEMBER(SYNCHRONOUSMACHINE_ANALYFLAG,analyFlag)
_XCLASS_MEMBER(SYNCHRONOUSMACHINE_ANALYSTATE3,analyState3)
_XCLASS_END

_XTYPEDEF(_xcombustionturbine_para,COMBUSTIONTURBINE_PARA)

_XCLASS_BEG(RTDBCCombustionTurbine)
_XCLASS_MEMBER(COMBUSTIONTURBINE_PARA,para)
_XCLASS_END

#endif/*_XCSGC3000_TEST_H_*/

