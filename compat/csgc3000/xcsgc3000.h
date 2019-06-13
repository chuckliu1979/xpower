/*
** Copyright (C) 2011 XXX all rights reserved.
**
** This file is tricky used, make sure there is no
** header #include guards since it will be include 
** multi-times.
**
** Created by chuck-liu@qq.com on 01/11/2011
**
*/

#include "../../xpower-decl.h"
#include "xcsgc3000-misc.h"
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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _XTRICKY_OUT
  #ifdef _XTRICKY_DEF
    #ifdef _XTRICKY
    #undef _XTRICKY
    #endif
    #define _XTRICKY(rtype,name,ptype) _XEXPORT extern rtype name(ptype);
  #elif defined(_XTRICKY_VAR)
    #ifdef _XTRICKY
    #undef _XTRICKY
    #endif
    #define _XTRICKY(rtype,name,ptype) static rtype (*name)(ptype); 
  #else
    #ifdef _XTRICKY
    #undef _XTRICKY
    #endif
    #define _XTRICKY(rtype,name,ptype)
  #endif
#endif

_XTRICKY(int, _xcsgc3000_database, int*)

_XTRICKY(int,                     _xcompany_ok,           void)
_XTRICKY(int,                     _xcompany_size,         void)
_XTRICKY(int,                     _xcompany_head,         void)
_XTRICKY(int,                     _xcompany_next,         void)
_XTRICKY(int,                     _xcompany_valid_index,   int)
_XTRICKY(void,                    _xcompany_free,         void)
_XTRICKY(struct _xcompany_para *, _xcompany_para_pointer, void)
_XTRICKY(struct _xcompany_real *, _xcompany_real_pointer, void)
_XTRICKY(struct _xcompany_estm *, _xcompany_estm_pointer, void)
_XTRICKY(struct _xcompany_calc *, _xcompany_calc_pointer, void)
_XTRICKY(struct _xcompany_anly *, _xcompany_anly_pointer, void)

_XTRICKY(int,                      _xloadarea_ok,           void)
_XTRICKY(int,                      _xloadarea_size,         void)
_XTRICKY(int,                      _xloadarea_head,         void)
_XTRICKY(int,                      _xloadarea_next,         void)
_XTRICKY(int,                      _xloadarea_valid_index,   int)
_XTRICKY(void,                     _xloadarea_free,         void)
_XTRICKY(struct _xloadarea_para *, _xloadarea_para_pointer, void)
_XTRICKY(struct _xloadarea_real *, _xloadarea_real_pointer, void)
_XTRICKY(struct _xloadarea_estm *, _xloadarea_estm_pointer, void)
_XTRICKY(struct _xloadarea_calc *, _xloadarea_calc_pointer, void)
_XTRICKY(struct _xloadarea_anly *, _xloadarea_anly_pointer, void)

_XTRICKY(int,                        _xsubstation_ok,           void)
_XTRICKY(int,                        _xsubstation_size,         void)
_XTRICKY(int,                        _xsubstation_head,         void)
_XTRICKY(int,                        _xsubstation_next,         void)
_XTRICKY(int,                        _xsubstation_valid_index,   int)
_XTRICKY(void,                       _xsubstation_free,         void)
_XTRICKY(struct _xsubstation_para *, _xsubstation_para_pointer, void)
_XTRICKY(struct _xsubstation_real *, _xsubstation_real_pointer, void)
_XTRICKY(struct _xsubstation_estm *, _xsubstation_estm_pointer, void)
_XTRICKY(struct _xsubstation_calc *, _xsubstation_calc_pointer, void)
_XTRICKY(struct _xsubstation_anly *, _xsubstation_anly_pointer, void)
_XTRICKY(struct _xsubstation_flag *, _xsubstation_flag_pointer, void)

_XTRICKY(int,                          _xvoltagelevel_ok,           void)
_XTRICKY(int,                          _xvoltagelevel_size,         void)
_XTRICKY(int,                          _xvoltagelevel_head,         void)
_XTRICKY(int,                          _xvoltagelevel_next,         void)
_XTRICKY(int,                          _xvoltagelevel_valid_index,   int)
_XTRICKY(void,                         _xvoltagelevel_free,         void)
_XTRICKY(struct _xvoltagelevel_para *, _xvoltagelevel_para_pointer, void)

_XTRICKY(int,                              _xpowertransformer_ok,           void)
_XTRICKY(int,                              _xpowertransformer_size,         void)
_XTRICKY(int,                              _xpowertransformer_head,         void)
_XTRICKY(int,                              _xpowertransformer_next,         void)
_XTRICKY(int,                              _xpowertransformer_valid_index,   int)
_XTRICKY(void,                             _xpowertransformer_free,         void)
_XTRICKY(struct _xpowertransformer_para *, _xpowertransformer_para_pointer, void)

_XTRICKY(int,                 _xbay_ok,           void)
_XTRICKY(int,                 _xbay_size,         void)
_XTRICKY(int,                 _xbay_head,         void)
_XTRICKY(int,                 _xbay_next,         void)
_XTRICKY(int,                 _xbay_valid_index,   int)
_XTRICKY(void,                _xbay_free,         void)
_XTRICKY(struct _xbay_para *, _xbay_para_pointer, void)

_XTRICKY(int,                              _xconnectivitynode_ok,           void)
_XTRICKY(int,                              _xconnectivitynode_size,         void)
_XTRICKY(int,                              _xconnectivitynode_head,         void)
_XTRICKY(int,                              _xconnectivitynode_next,         void)
_XTRICKY(int,                              _xconnectivitynode_valid_index,   int)
_XTRICKY(void,                             _xconnectivitynode_free,         void)
_XTRICKY(struct _xconnectivitynode_para *, _xconnectivitynode_para_pointer, void)
_XTRICKY(struct _xconnectivitynode_real *, _xconnectivitynode_real_pointer, void)
_XTRICKY(struct _xconnectivitynode_estm *, _xconnectivitynode_estm_pointer, void)
_XTRICKY(struct _xconnectivitynode_calc *, _xconnectivitynode_calc_pointer, void)
_XTRICKY(struct _xconnectivitynode_anly *, _xconnectivitynode_anly_pointer, void)

_XTRICKY(int,                      _xterminal_ok,           void)
_XTRICKY(int,                      _xterminal_size,         void)
_XTRICKY(int,                      _xterminal_head,         void)
_XTRICKY(int,                      _xterminal_next,         void)
_XTRICKY(int,                      _xterminal_valid_index,   int)
_XTRICKY(void,                     _xterminal_free,         void)
_XTRICKY(struct _xterminal_para *, _xterminal_para_pointer, void)

_XTRICKY(int,                         _xmeasurement_ok,           void)
_XTRICKY(int,                         _xmeasurement_size,         void)
_XTRICKY(int,                         _xmeasurement_head,         void)
_XTRICKY(int,                         _xmeasurement_next,         void)
_XTRICKY(int,                         _xmeasurement_valid_index,   int)
_XTRICKY(void,                        _xmeasurement_free,         void)
_XTRICKY(struct _xmeasurement_data *, _xmeasurement_data_pointer, void)
_XTRICKY(struct _xmeasurement_estm *, _xmeasurement_estm_pointer, void)
_XTRICKY(struct _xmeasurement_calc *, _xmeasurement_calc_pointer, void)
_XTRICKY(struct _xmeasurement_anly *, _xmeasurement_anly_pointer, void)

_XTRICKY(int,                         _xanaloginput_ok,           void)
_XTRICKY(int,                         _xanaloginput_size,         void)
_XTRICKY(int,                         _xanaloginput_head,         void)
_XTRICKY(int,                         _xanaloginput_next,         void)
_XTRICKY(int,                         _xanaloginput_valid_index,   int)
_XTRICKY(void,                        _xanaloginput_free,         void)
_XTRICKY(struct _xanaloginput_data *, _xanaloginput_data_pointer, void)
_XTRICKY(struct _xanaloginput_anly *, _xanaloginput_anly_pointer, void)

_XTRICKY(int,                         _xstatusinput_ok,           void)
_XTRICKY(int,                         _xstatusinput_size,         void)
_XTRICKY(int,                         _xstatusinput_head,         void)
_XTRICKY(int,                         _xstatusinput_next,         void)
_XTRICKY(int,                         _xstatusinput_valid_index,   int)
_XTRICKY(void,                        _xstatusinput_free,         void)
_XTRICKY(struct _xstatusinput_data *, _xstatusinput_data_pointer, void)
_XTRICKY(struct _xstatusinput_anly *, _xstatusinput_anly_pointer, void)

_XTRICKY(int,                      _xbreaker_ok,            void)
_XTRICKY(int,                      _xbreaker_size,          void)
_XTRICKY(int,                      _xbreaker_head,          void)
_XTRICKY(int,                      _xbreaker_next,          void)
_XTRICKY(int,                      _xbreaker_valid_index,    int)
_XTRICKY(void,                     _xbreaker_free,          void)
_XTRICKY(struct _xbreaker_para *,  _xbreaker_para_pointer,  void)
_XTRICKY(struct _xbreaker_real *,  _xbreaker_real_pointer,  void)
_XTRICKY(struct _xbreaker_real2 *, _xbreaker_real2_pointer, void)
_XTRICKY(struct _xbreaker_estm *,  _xbreaker_estm_pointer,  void)
_XTRICKY(struct _xbreaker_estm2 *, _xbreaker_estm2_pointer, void)
_XTRICKY(struct _xbreaker_estm3 *, _xbreaker_estm3_pointer, void)
_XTRICKY(struct _xbreaker_calc *,  _xbreaker_calc_pointer,  void)
_XTRICKY(struct _xbreaker_calc2 *, _xbreaker_calc2_pointer, void)
_XTRICKY(struct _xbreaker_calc3 *, _xbreaker_calc3_pointer, void)
_XTRICKY(struct _xbreaker_anly *,  _xbreaker_anly_pointer,  void)
_XTRICKY(struct _xbreaker_anly2 *, _xbreaker_anly2_pointer, void)
_XTRICKY(struct _xbreaker_anly3 *, _xbreaker_anly3_pointer, void)

_XTRICKY(int,                           _xdisconnector_ok,            void)
_XTRICKY(int,                           _xdisconnector_size,          void)
_XTRICKY(int,                           _xdisconnector_head,          void)
_XTRICKY(int,                           _xdisconnector_next,          void)
_XTRICKY(int,                           _xdisconnector_valid_index,    int)
_XTRICKY(void,                          _xdisconnector_free,          void)
_XTRICKY(struct _xdisconnector_para *,  _xdisconnector_para_pointer,  void)
_XTRICKY(struct _xdisconnector_real *,  _xdisconnector_real_pointer,  void)
_XTRICKY(struct _xdisconnector_real2 *, _xdisconnector_real2_pointer, void)
_XTRICKY(struct _xdisconnector_estm *,  _xdisconnector_estm_pointer,  void)
_XTRICKY(struct _xdisconnector_estm2 *, _xdisconnector_estm2_pointer, void)
_XTRICKY(struct _xdisconnector_estm3 *, _xdisconnector_estm3_pointer, void)
_XTRICKY(struct _xdisconnector_calc *,  _xdisconnector_calc_pointer,  void)
_XTRICKY(struct _xdisconnector_calc2 *, _xdisconnector_calc2_pointer, void)
_XTRICKY(struct _xdisconnector_calc3 *, _xdisconnector_calc3_pointer, void)
_XTRICKY(struct _xdisconnector_anly *,  _xdisconnector_anly_pointer,  void)
_XTRICKY(struct _xdisconnector_anly2 *, _xdisconnector_anly2_pointer, void)
_XTRICKY(struct _xdisconnector_anly3 *, _xdisconnector_anly3_pointer, void)

_XTRICKY(int,                                 _xgrounddisconnector_ok,            void)
_XTRICKY(int,                                 _xgrounddisconnector_size,          void)
_XTRICKY(int,                                 _xgrounddisconnector_head,          void)
_XTRICKY(int,                                 _xgrounddisconnector_next,          void)
_XTRICKY(int,                                 _xgrounddisconnector_valid_index,    int)
_XTRICKY(void,                                _xgrounddisconnector_free,          void)
_XTRICKY(struct _xgrounddisconnector_para *,  _xgrounddisconnector_para_pointer,  void)
_XTRICKY(struct _xgrounddisconnector_real *,  _xgrounddisconnector_real_pointer,  void)
_XTRICKY(struct _xgrounddisconnector_real2 *, _xgrounddisconnector_real2_pointer, void)
_XTRICKY(struct _xgrounddisconnector_estm *,  _xgrounddisconnector_estm_pointer,  void)
_XTRICKY(struct _xgrounddisconnector_estm2 *, _xgrounddisconnector_estm2_pointer, void)
_XTRICKY(struct _xgrounddisconnector_estm3 *, _xgrounddisconnector_estm3_pointer, void)
_XTRICKY(struct _xgrounddisconnector_calc *,  _xgrounddisconnector_calc_pointer,  void)
_XTRICKY(struct _xgrounddisconnector_calc2 *, _xgrounddisconnector_calc2_pointer, void)
_XTRICKY(struct _xgrounddisconnector_calc3 *, _xgrounddisconnector_calc3_pointer, void)
_XTRICKY(struct _xgrounddisconnector_anly *,  _xgrounddisconnector_anly_pointer,  void)
_XTRICKY(struct _xgrounddisconnector_anly2 *, _xgrounddisconnector_anly2_pointer, void)
_XTRICKY(struct _xgrounddisconnector_anly3 *, _xgrounddisconnector_anly3_pointer, void)

_XTRICKY(int,                             _xenergyconsumer_ok,            void)
_XTRICKY(int,                             _xenergyconsumer_size,          void)
_XTRICKY(int,                             _xenergyconsumer_head,          void)
_XTRICKY(int,                             _xenergyconsumer_next,          void)
_XTRICKY(int,                             _xenergyconsumer_valid_index,    int)
_XTRICKY(void,                            _xenergyconsumer_free,          void)
_XTRICKY(struct _xenergyconsumer_para *,  _xenergyconsumer_para_pointer,  void)
_XTRICKY(struct _xenergyconsumer_real *,  _xenergyconsumer_real_pointer,  void)
_XTRICKY(struct _xenergyconsumer_real2 *, _xenergyconsumer_real2_pointer, void)
_XTRICKY(struct _xenergyconsumer_estm *,  _xenergyconsumer_estm_pointer,  void)
_XTRICKY(struct _xenergyconsumer_estm2 *, _xenergyconsumer_estm2_pointer, void)
_XTRICKY(struct _xenergyconsumer_estm3 *, _xenergyconsumer_estm3_pointer, void)
_XTRICKY(struct _xenergyconsumer_calc *,  _xenergyconsumer_calc_pointer,  void)
_XTRICKY(struct _xenergyconsumer_calc2 *, _xenergyconsumer_calc2_pointer, void)
_XTRICKY(struct _xenergyconsumer_calc3 *, _xenergyconsumer_calc3_pointer, void)
_XTRICKY(struct _xenergyconsumer_anly *,  _xenergyconsumer_anly_pointer,  void)
_XTRICKY(struct _xenergyconsumer_anly2 *, _xenergyconsumer_anly2_pointer, void)
_XTRICKY(struct _xenergyconsumer_anly3 *, _xenergyconsumer_anly3_pointer, void)

_XTRICKY(int,                          _xcompensator_ok,            void)
_XTRICKY(int,                          _xcompensator_size,          void)
_XTRICKY(int,                          _xcompensator_head,          void)
_XTRICKY(int,                          _xcompensator_next,          void)
_XTRICKY(int,                          _xcompensator_valid_index,    int)
_XTRICKY(void,                         _xcompensator_free,          void)
_XTRICKY(struct _xcompensator_para *,  _xcompensator_para_pointer,  void)
_XTRICKY(struct _xcompensator_real *,  _xcompensator_real_pointer,  void)
_XTRICKY(struct _xcompensator_real2 *, _xcompensator_real2_pointer, void)
_XTRICKY(struct _xcompensator_estm *,  _xcompensator_estm_pointer,  void)
_XTRICKY(struct _xcompensator_estm2 *, _xcompensator_estm2_pointer, void)
_XTRICKY(struct _xcompensator_estm3 *, _xcompensator_estm3_pointer, void)
_XTRICKY(struct _xcompensator_calc *,  _xcompensator_calc_pointer,  void)
_XTRICKY(struct _xcompensator_calc2 *, _xcompensator_calc2_pointer, void)
_XTRICKY(struct _xcompensator_calc3 *, _xcompensator_calc3_pointer, void)
_XTRICKY(struct _xcompensator_anly *,  _xcompensator_anly_pointer,  void)
_XTRICKY(struct _xcompensator_anly2 *, _xcompensator_anly2_pointer, void)
_XTRICKY(struct _xcompensator_anly3 *, _xcompensator_anly3_pointer, void)

_XTRICKY(int,                                   _xstaticvarcompensator_ok,            void)
_XTRICKY(int,                                   _xstaticvarcompensator_size,          void)
_XTRICKY(int,                                   _xstaticvarcompensator_head,          void)
_XTRICKY(int,                                   _xstaticvarcompensator_next,          void)
_XTRICKY(int,                                   _xstaticvarcompensator_valid_index,    int)
_XTRICKY(void,                                  _xstaticvarcompensator_free,          void)
_XTRICKY(struct _xstaticvarcompensator_para *,  _xstaticvarcompensator_para_pointer,  void)
_XTRICKY(struct _xstaticvarcompensator_real *,  _xstaticvarcompensator_real_pointer,  void)
_XTRICKY(struct _xstaticvarcompensator_real2 *, _xstaticvarcompensator_real2_pointer, void)
_XTRICKY(struct _xstaticvarcompensator_estm *,  _xstaticvarcompensator_estm_pointer,  void)
_XTRICKY(struct _xstaticvarcompensator_estm2 *, _xstaticvarcompensator_estm2_pointer, void)
_XTRICKY(struct _xstaticvarcompensator_estm3 *, _xstaticvarcompensator_estm3_pointer, void)
_XTRICKY(struct _xstaticvarcompensator_calc *,  _xstaticvarcompensator_calc_pointer,  void)
_XTRICKY(struct _xstaticvarcompensator_calc2 *, _xstaticvarcompensator_calc2_pointer, void)
_XTRICKY(struct _xstaticvarcompensator_calc3 *, _xstaticvarcompensator_calc3_pointer, void)
_XTRICKY(struct _xstaticvarcompensator_anly *,  _xstaticvarcompensator_anly_pointer,  void)
_XTRICKY(struct _xstaticvarcompensator_anly2 *, _xstaticvarcompensator_anly2_pointer, void)
_XTRICKY(struct _xstaticvarcompensator_anly3 *, _xstaticvarcompensator_anly3_pointer, void)

_XTRICKY(int,                                 _xtransformerwinding_ok,            void)
_XTRICKY(int,                                 _xtransformerwinding_size,          void)
_XTRICKY(int,                                 _xtransformerwinding_head,          void)
_XTRICKY(int,                                 _xtransformerwinding_next,          void)
_XTRICKY(int,                                 _xtransformerwinding_valid_index,    int)
_XTRICKY(void,                                _xtransformerwinding_free,          void)
_XTRICKY(struct _xtransformerwinding_para *,  _xtransformerwinding_para_pointer,  void)
_XTRICKY(struct _xtransformerwinding_real *,  _xtransformerwinding_real_pointer,  void)
_XTRICKY(struct _xtransformerwinding_real2 *, _xtransformerwinding_real2_pointer, void)
_XTRICKY(struct _xtransformerwinding_estm *,  _xtransformerwinding_estm_pointer,  void)
_XTRICKY(struct _xtransformerwinding_estm2 *, _xtransformerwinding_estm2_pointer, void)
_XTRICKY(struct _xtransformerwinding_estm3 *, _xtransformerwinding_estm3_pointer, void)
_XTRICKY(struct _xtransformerwinding_calc *,  _xtransformerwinding_calc_pointer,  void)
_XTRICKY(struct _xtransformerwinding_calc2 *, _xtransformerwinding_calc2_pointer, void)
_XTRICKY(struct _xtransformerwinding_calc3 *, _xtransformerwinding_calc3_pointer, void)
_XTRICKY(struct _xtransformerwinding_anly *,  _xtransformerwinding_anly_pointer,  void)
_XTRICKY(struct _xtransformerwinding_anly2 *, _xtransformerwinding_anly2_pointer, void)
_XTRICKY(struct _xtransformerwinding_anly3 *, _xtransformerwinding_anly3_pointer, void)

_XTRICKY(int,                        _xtapchanger_ok,           void)
_XTRICKY(int,                        _xtapchanger_size,         void)
_XTRICKY(int,                        _xtapchanger_head,         void)
_XTRICKY(int,                        _xtapchanger_next,         void)
_XTRICKY(int,                        _xtapchanger_valid_index,   int)
_XTRICKY(void,                       _xtapchanger_free,         void)
_XTRICKY(struct _xtapchanger_para *, _xtapchanger_para_pointer, void)
_XTRICKY(struct _xtapchanger_real *, _xtapchanger_real_pointer, void)
_XTRICKY(struct _xtapchanger_estm *, _xtapchanger_estm_pointer, void)
_XTRICKY(struct _xtapchanger_calc *, _xtapchanger_calc_pointer, void)
_XTRICKY(struct _xtapchanger_anly *, _xtapchanger_anly_pointer, void)

_XTRICKY(int,                            _xaclinesegment_ok,            void)
_XTRICKY(int,                            _xaclinesegment_size,          void)
_XTRICKY(int,                            _xaclinesegment_head,          void)
_XTRICKY(int,                            _xaclinesegment_next,          void)
_XTRICKY(int,                            _xaclinesegment_valid_index,    int)
_XTRICKY(void,                           _xaclinesegment_free,          void)
_XTRICKY(struct _xaclinesegment_para *,  _xaclinesegment_para_pointer,  void)
_XTRICKY(struct _xaclinesegment_real *,  _xaclinesegment_real_pointer,  void)
_XTRICKY(struct _xaclinesegment_real2 *, _xaclinesegment_real2_pointer, void)
_XTRICKY(struct _xaclinesegment_estm *,  _xaclinesegment_estm_pointer,  void)
_XTRICKY(struct _xaclinesegment_estm2 *, _xaclinesegment_estm2_pointer, void)
_XTRICKY(struct _xaclinesegment_estm3 *, _xaclinesegment_estm3_pointer, void)
_XTRICKY(struct _xaclinesegment_calc *,  _xaclinesegment_calc_pointer,  void)
_XTRICKY(struct _xaclinesegment_calc2 *, _xaclinesegment_calc2_pointer, void)
_XTRICKY(struct _xaclinesegment_calc3 *, _xaclinesegment_calc3_pointer, void)
_XTRICKY(struct _xaclinesegment_anly *,  _xaclinesegment_anly_pointer,  void)
_XTRICKY(struct _xaclinesegment_anly2 *, _xaclinesegment_anly2_pointer, void)
_XTRICKY(struct _xaclinesegment_anly3 *, _xaclinesegment_anly3_pointer, void)

_XTRICKY(int,                            _xbusbarsection_ok,            void)
_XTRICKY(int,                            _xbusbarsection_size,          void)
_XTRICKY(int,                            _xbusbarsection_head,          void)
_XTRICKY(int,                            _xbusbarsection_next,          void)
_XTRICKY(int,                            _xbusbarsection_valid_index,    int)
_XTRICKY(void,                           _xbusbarsection_free,          void)
_XTRICKY(struct _xbusbarsection_para *,  _xbusbarsection_para_pointer,  void)
_XTRICKY(struct _xbusbarsection_real *,  _xbusbarsection_real_pointer,  void)
_XTRICKY(struct _xbusbarsection_real2 *, _xbusbarsection_real2_pointer, void)
_XTRICKY(struct _xbusbarsection_estm *,  _xbusbarsection_estm_pointer,  void)
_XTRICKY(struct _xbusbarsection_estm2 *, _xbusbarsection_estm2_pointer, void)
_XTRICKY(struct _xbusbarsection_estm3 *, _xbusbarsection_estm3_pointer, void)
_XTRICKY(struct _xbusbarsection_calc *,  _xbusbarsection_calc_pointer,  void)
_XTRICKY(struct _xbusbarsection_calc2 *, _xbusbarsection_calc2_pointer, void)
_XTRICKY(struct _xbusbarsection_calc3 *, _xbusbarsection_calc3_pointer, void)
_XTRICKY(struct _xbusbarsection_anly *,  _xbusbarsection_anly_pointer,  void)
_XTRICKY(struct _xbusbarsection_anly2 *, _xbusbarsection_anly2_pointer, void)
_XTRICKY(struct _xbusbarsection_anly3 *, _xbusbarsection_anly3_pointer, void)

_XTRICKY(int,                              _xequivalentsource_ok,           void)
_XTRICKY(int,                              _xequivalentsource_size,         void)
_XTRICKY(int,                              _xequivalentsource_head,         void)
_XTRICKY(int,                              _xequivalentsource_next,         void)
_XTRICKY(int,                              _xequivalentsource_valid_index,   int)
_XTRICKY(void,                             _xequivalentsource_free,         void)
_XTRICKY(struct _xequivalentsource_para *, _xequivalentsource_para_pointer, void)
_XTRICKY(struct _xequivalentsource_real *, _xequivalentsource_real_pointer, void)
_XTRICKY(struct _xequivalentsource_real2 *,_xequivalentsource_real2_pointer,void)
_XTRICKY(struct _xequivalentsource_estm *, _xequivalentsource_estm_pointer, void)
_XTRICKY(struct _xequivalentsource_calc *, _xequivalentsource_calc_pointer, void)
_XTRICKY(struct _xequivalentsource_anly *, _xequivalentsource_anly_pointer, void)

_XTRICKY(int,                                 _xsynchronousmachine_ok,            void)
_XTRICKY(int,                                 _xsynchronousmachine_size,          void)
_XTRICKY(int,                                 _xsynchronousmachine_head,          void)
_XTRICKY(int,                                 _xsynchronousmachine_next,          void)
_XTRICKY(int,                                 _xsynchronousmachine_valid_index,    int)
_XTRICKY(void,                                _xsynchronousmachine_free,          void)
_XTRICKY(struct _xsynchronousmachine_para *,  _xsynchronousmachine_para_pointer,  void)
_XTRICKY(struct _xsynchronousmachine_real *,  _xsynchronousmachine_real_pointer,  void)
_XTRICKY(struct _xsynchronousmachine_real2 *, _xsynchronousmachine_real2_pointer, void)
_XTRICKY(struct _xsynchronousmachine_estm *,  _xsynchronousmachine_estm_pointer,  void)
_XTRICKY(struct _xsynchronousmachine_estm2 *, _xsynchronousmachine_estm2_pointer, void)
_XTRICKY(struct _xsynchronousmachine_estm3 *, _xsynchronousmachine_estm3_pointer, void)
_XTRICKY(struct _xsynchronousmachine_calc *,  _xsynchronousmachine_calc_pointer,  void)
_XTRICKY(struct _xsynchronousmachine_calc2 *, _xsynchronousmachine_calc2_pointer, void)
_XTRICKY(struct _xsynchronousmachine_calc3 *, _xsynchronousmachine_calc3_pointer, void)
_XTRICKY(struct _xsynchronousmachine_anly *,  _xsynchronousmachine_anly_pointer,  void)
_XTRICKY(struct _xsynchronousmachine_anly2 *, _xsynchronousmachine_anly2_pointer, void)
_XTRICKY(struct _xsynchronousmachine_anly3 *, _xsynchronousmachine_anly3_pointer, void)

_XTRICKY(int,                               _xcombustionturbine_ok,           void)
_XTRICKY(int,                               _xcombustionturbine_size,         void)
_XTRICKY(int,                               _xcombustionturbine_head,         void)
_XTRICKY(int,                               _xcombustionturbine_next,         void)
_XTRICKY(int,                               _xcombustionturbine_valid_index,   int)
_XTRICKY(void,                              _xcombustionturbine_free,         void)
_XTRICKY(struct _xcombustionturbine_para *, _xcombustionturbine_para_pointer, void)

#ifdef _XTRICKY
#undef _XTRICKY
#endif
#ifdef _XTRICKY_DEF
#undef _XTRICKY_DEF
#endif
#ifdef _XTRICKY_VAR
#undef _XTRICKY_VAR
#endif
#ifdef _XTRICKY_OUT
#undef _XTRICKY_OUT
#endif

#ifdef __cplusplus
}
#endif

