QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
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
    savefilesdlg.cpp \
    selectresourcedlg.cpp \
    subwindowsmodel.cpp \
    windowslistdlg.cpp

HEADERS += \
    mainwindow.h \
    newitemsdlg.h \
    savefilesdlg.h \
    selectresourcedlg.h \
    subwindowsmodel.h \
    windowslistdlg.h

FORMS += \
    mainwindow.ui \
    newitemsdlg.ui \
    savefilesdlg.ui \
    selectresourcedlg.ui \
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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ToolsRuntimeProj/ToolsRuntime/release/ -lToolsRuntime
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ToolsRuntimeProj/ToolsRuntime/debug/ -lToolsRuntime
else:unix: LIBS += -L$$OUT_PWD/../ToolsRuntimeProj/ToolsRuntime/ -lToolsRuntime

INCLUDEPATH += $$PWD/../ToolsRuntimeProj/ToolsRuntime $$PWD/../ToolsRuntimeProj/ToolsRuntime/optionsdlg $$PWD/../ToolsRuntimeProj/ToolsRuntime/widgets
DEPENDPATH += $$PWD/../ToolsRuntimeProj/ToolsRuntime


#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../SARibbon-master/bin_qt5.15.2_Release_x86/bin/ -lSARibbonBard
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../SARibbon-master/bin_qt5.15.2_Debug_x86/bin/ -lSARibbonBard

#INCLUDEPATH += $$PWD/../../SARibbon-master/src/SARibbonBar
#DEPENDPATH += $$PWD/../../SARibbon-master/src/SARibbonBar
