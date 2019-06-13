#
#
#

TEMPLATE = lib
TARGET   = xio-3k
CONFIG  -= qt

HEADERS += structs/xcsgc3000-struct4aclinesegment.h        \
           structs/xcsgc3000-struct4analoginput.h          \
           structs/xcsgc3000-struct4bay.h                  \
           structs/xcsgc3000-struct4breaker.h              \
           structs/xcsgc3000-struct4busbarsection.h        \
           structs/xcsgc3000-struct4combustionturbine.h    \
           structs/xcsgc3000-struct4company.h              \
           structs/xcsgc3000-struct4compensator.h          \
           structs/xcsgc3000-struct4connectivitynode.h     \
           structs/xcsgc3000-struct4datetime.h             \
           structs/xcsgc3000-struct4disconnector.h         \
           structs/xcsgc3000-struct4energyconsumer.h       \
           structs/xcsgc3000-struct4equivalentsource.h     \
           structs/xcsgc3000-struct4grounddisconnector.h   \
           structs/xcsgc3000-struct4loadarea.h             \
           structs/xcsgc3000-struct4measurement.h          \
           structs/xcsgc3000-struct4powertransformer.h     \
           structs/xcsgc3000-struct4staticvarcompensator.h \
           structs/xcsgc3000-struct4statusinput.h          \
           structs/xcsgc3000-struct4substation.h           \
           structs/xcsgc3000-struct4synchronousmachine.h   \
           structs/xcsgc3000-struct4tapchanger.h           \
           structs/xcsgc3000-struct4terminal.h             \
           structs/xcsgc3000-struct4transformerwinding.h   \
           structs/xcsgc3000-struct4voltagelevel.h
           
HEADERS += xcsgc3000.h xcsgc3000-misc.h
SOURCES += xcsgc3000.cpp

xtest:HEADERS += xcsgc3000-test.h

win32 {
    DEFINES += _XEXPORT _WIN32 WIN32
}
