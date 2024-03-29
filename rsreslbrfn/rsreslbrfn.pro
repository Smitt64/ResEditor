CONFIG += qt private

TEMPLATE = lib
DEFINES += RSRESLBRFN_LIBRARY

CONFIG += c++11
include(../config.pri)
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
#LIBS += -lkernel32 -luser32 -lshell32 -lshlwapi -lshfolder -lUserenv -lpsapi
#DEFINES += RUSSIAN

SOURCES += \
    dllimport/lbrdllobject.cpp \
    dllimport/lbrdllobjectprivate.cpp \
    lbrobject.cpp \
    lbrobjectinterface.cpp \
    lbrobjectinterfaceprivate.cpp \
    lbrobjectprivate.cpp \
    lbrreslistmodel.cpp \
#    listobj.cpp
#    memstream.cpp \
#    panelstr.cpp \
#    res_lbr.c \
#    resdir.c \
#    rsexcept.cpp \
#    rsreslbrfn.cpp \
 \#   unifile.cpp
    resbuffer.cpp

HEADERS += \
#    dlmdef.h \
#    listobj.h \
    LbrObjectHeaders.h \
    dllimport/lbrdllobject.h \
    dllimport/lbrdllobjectprivate.h \
    lbrobject.h \
    lbrobjectinterface.h \
    lbrobjectinterfaceprivate.h \
    lbrobjectprivate.h \
    lbrreslistmodel.h \
    packpop.h \
    packpsh1.h \
    packpshn.h \
#    panel.h \
#    platform.h \
#    res_lbr.h \
#    rscomplr.h \
#    rsconfig.h \
#    rsexcept.h \
#    rslcvt.h \
    resbuffer.h \
    rsreslbrfn_global.h \
    rsreslbrfn.h
#    rstring.h \
#    rstypes.h \
#    systypes.h \
#   unifile.hpp \
#    unixio.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
