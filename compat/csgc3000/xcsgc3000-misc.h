/*
** Copyright (C) 2011 XXX all rights reserved.
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#ifndef _XCSGC3000_MISC_H_
#define _XCSGC3000_MISC_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MEASUREMENTTYPE_H_INCLUDED
#define MEASUREMENTTYPE_H_INCLUDED

#define _XCurrentA                 101 
#define _XThreePhaseCurrent        102
#define _XFrequency                103
#define _XPowerFactor              104
#define _XThreePhasePowerFactor    105
#define _XThreePhaseApparentPower  106
#define _XThreePhaseReactivePower  107
#define _XThreePhaseActivePower    108
#define _XApparentPower            109
#define _XReactivePowerA           110
#define _XActivePowerA             111
#define _XThreePhaseVoltage        112
#define _XPressure                 113
#define _XTemperature              114
#define _XThreePhaseAngle          115
#define _XApparentEnergy           116
#define _XReactiveEnergy           117
#define _XActiveEnergy             118
#define _XAutomatic                119
#define _XLocalOperation           120
#define _XAutomaticControl         121
#define _XSwitchPosition           122
#define _XTapPosition              123
#define _XOperationCount           124
/*#define _XParallel Transformers    125 */
#define _XCurrentB                 126
#define _XCurrentC                 127
#define _XActivePowerB             128
#define _XActivePowerC             129
#define _XReactivePowerB           130
#define _XReactivePowerC           131
#define _XVoltageB                 132
#define _XVoltageC                 133
#define _XPhaseAngleB              134
#define _XPhaseAngleC              135
#define _XPhaseAngleA              136
#define _XVoltageA                 137
#define _XVoltageAB                139
#define _XVoltageBC                140
#define _XVoltageCA                141
#define _XVoltageZero              142
/*#define _Xtemprature               143 */
#define _XCurrentThreePhaseAngle   144
#define _XCurrentPhaseAngleB       145
#define _XCurrentPhaseAngleC       146
#define _XCurrentPhaseAngleA       147
#define _XEqe                      148
#define _XDeltaE                   149

#else

#define _XCurrentA                 CurrentA                
#define _XThreePhaseCurrent        ThreePhaseCurrent       
#define _XFrequency                Frequency               
#define _XPowerFactor              PowerFactor             
#define _XThreePhasePowerFactor    ThreePhasePowerFactor   
#define _XThreePhaseApparentPower  ThreePhaseApparentPower 
#define _XThreePhaseReactivePower  ThreePhaseReactivePower 
#define _XThreePhaseActivePower    ThreePhaseActivePower   
#define _XApparentPower            ApparentPower           
#define _XReactivePowerA           ReactivePowerA          
#define _XActivePowerA             ActivePowerA            
#define _XThreePhaseVoltage        ThreePhaseVoltage       
#define _XPressure                 Pressure                
#define _XTemperature              Temperature             
#define _XThreePhaseAngle          ThreePhaseAngle         
#define _XApparentEnergy           ApparentEnergy          
#define _XReactiveEnergy           ReactiveEnergy          
#define _XActiveEnergy             ActiveEnergy            
#define _XAutomatic                Automatic               
#define _XLocalOperation           LocalOperation          
#define _XAutomaticControl         AutomaticControl        
#define _XSwitchPosition           SwitchPosition          
#define _XTapPosition              TapPosition             
#define _XOperationCount           OperationCount          
/*#define _XParallel Transformers    Parallel Transformers */   
#define _XCurrentB                 CurrentB                
#define _XCurrentC                 CurrentC                
#define _XActivePowerB             ActivePowerB            
#define _XActivePowerC             ActivePowerC            
#define _XReactivePowerB           ReactivePowerB          
#define _XReactivePowerC           ReactivePowerC          
#define _XVoltageB                 VoltageB                
#define _XVoltageC                 VoltageC                
#define _XPhaseAngleB              PhaseAngleB             
#define _XPhaseAngleC              PhaseAngleC             
#define _XPhaseAngleA              PhaseAngleA             
#define _XVoltageA                 VoltageA                
#define _XVoltageAB                VoltageAB               
#define _XVoltageBC                VoltageBC               
#define _XVoltageCA                VoltageCA               
#define _XVoltageZero              VoltageZero             
/*#define _Xtemprature               temprature */             
#define _XCurrentThreePhaseAngle   CurrentThreePhaseAngle  
#define _XCurrentPhaseAngleB       CurrentPhaseAngleB      
#define _XCurrentPhaseAngleC       CurrentPhaseAngleC      
#define _XCurrentPhaseAngleA       CurrentPhaseAngleA      
#define _XEqe                      Eqe                     
#define _XDeltaE                   DeltaE                  

#endif/*MEASUREMENTTYPE_H_INCLUDED*/

#ifndef DEFAULT_SUBMODE 
#define DEFAULT_SUBMODE

#define _XDEFAULT_SUBMODE   0
#define _XREAL_SUBMODE      0
#define _XESTIM_SUBMODE     1
#define _XCALC_SUBMODE      2
#define _XANALYSIS_SUBMODE  3
#define _XPRED_SUBMODE      4
#define _XSTUDY_SUBMODE     5

#else

#define _XDEFAULT_SUBMODE   DEFAULT_SUBMODE
#define _XREAL_SUBMODE      REAL_SUBMODE
#define _XESTIM_SUBMODE     ESTIM_SUBMODE
#define _XCALC_SUBMODE      CALC_SUBMODE
#define _XANALYSIS_SUBMODE  ANALYSIS_SUBMODE
#define _XPRED_SUBMODE      PRED_SUBMODE
#define _XSTUDY_SUBMODE     STUDY_SUBMODE

#endif

#ifdef __cplusplus
}
#endif

#endif/*_XCSGC3000_MISC_H_*/

