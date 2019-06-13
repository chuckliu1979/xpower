#
#
#

TEMPLATE = lib
TARGET   = xpss
CONFIG  -= qt

include(../src.pri)

HEADERS += xpower-agc.h       \
           xpower-array.h     \
           xpower-config.h    \
           xpower-decl.h      \
           xpower-def.h       \
           xpower-io.h        \
           xpower-matrix.h    \
           xpower-model.h     \
           xpower-powerflow.h \
           xpower-thread.h    \
           xpower-topology.h  \
           xpower-util.h
           
SOURCES += xpower-agc.c       \
           xpower-array.c     \
           xpower-hash.c      \
           xpower-io.c        \
           xpower-malloc.c    \
           xpower-matrix.c    \
           xpower-model.c     \
           xpower-powerflow.c \
           xpower-thread.c    \
           xpower-topology.c  \
           xpower-util.c
           
HEADERS += compat/ieee/ieee-testcase.h
SOURCES += compat/ieee/ieee-testcase.c \
           compat/xio-csgc3000.c

unix :SOURCES += compat/xthread-posix.c
win32:SOURCES += compat/xthread-win32.c

win32 {
    DEFINES += _XEXPORT _WIN32 WIN32
}
