QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
#static_runtime

include(../config.pri)
#QMAKE_CXXFLAGS_DEBUG += /MTd

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TARGET = WorkRes
RC_FILE = res/winfmt.rc

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    newitemsdlg.cpp \
    subwindowsmodel.cpp \
    windowslistdlg.cpp

HEADERS += \
    mainwindow.h \
    newitemsdlg.h \
    subwindowsmodel.h \
    windowslistdlg.h

FORMS += \
    mainwindow.ui \
    newitemsdlg.ui \
    windowslistdlg.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../rsrescore/release/ -lrsrescore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../rsrescore/debug/ -lrsrescore
else:unix: LIBS += -L$$OUT_PWD/../rsrescore/ -lrsrescore

INCLUDEPATH += $$PWD/../rsrescore
DEPENDPATH += $$PWD/../rsrescore

INCLUDEPATH += $$PWD/../rsreslbrfn
DEPENDPATH += $$PWD/../rsreslbrfn

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../rsreslbrfn/release/ -lrsreslbrfn
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../rsreslbrfn/debug/ -lrsreslbrfn
else:unix:!macx: LIBS += -L$$OUT_PWD/../rsreslbrfn/ -lrsreslbrfn

INCLUDEPATH += $$PWD/../rsreslbrfn
DEPENDPATH += $$PWD/../rsreslbrfn

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../RsWorkTools/ToolsRuntime/release/ -lToolsRuntime
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../RsWorkTools/ToolsRuntime/debug/ -lToolsRuntime
else:unix: LIBS += -L$$OUT_PWD/../RsWorkTools/ToolsRuntime/ -lToolsRuntime

INCLUDEPATH += $$PWD/../RsWorkTools/ToolsRuntime
DEPENDPATH += $$PWD/../RsWorkTools/ToolsRuntime
