QT += widgets

TEMPLATE = lib
DEFINES += RSRESCORE_LIBRARY

CONFIG += c++11 plugin static_runtime
QTPLUGIN += BaseResourceEditor

include(../config.pri)
#QMAKE_CXXFLAGS_DEBUG += /MTd

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    baseeditorview.cpp \
    baseeditorwindow.cpp \
    #baseresourceeditor.cpp \
    basescene.cpp \
    containeritem.cpp \
    controlitem.cpp \
    customrectitem.cpp \
    panelitem.cpp \
    reslib.cpp \
    reslibdirmodel.cpp \
    reslistdockwidget.cpp \
    respanel.cpp \
    rsrescore.cpp \
    statusbarelement.cpp \
    stdpaneleditor.cpp \
    styles/resstyle.cpp \
    textitem.cpp \
    undoredo/undoitemmove.cpp

HEADERS += \
    ResourceEditorInterface.h \
    RsResInterface.h \
    baseeditorview.h \
    baseeditorwindow.h \
    #baseresourceeditor.h \
    basescene.h \
    containeritem.h \
    controlitem.h \
    customrectitem.h \
    panelitem.h \
    reslib.h \
    reslibdirmodel.h \
    reslistdockwidget.h \
    respanel.h \
    rscoreheader.h \
    rsrescore_global.h \
    rsrescore.h \
    statusbarelement.h \
    stdpaneleditor.h \
    styles/resstyle.h \
    textitem.h \
    undoredo/undoitemmove.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += lib

RESOURCES += \
    resource.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../rsbaseeditor/release/ -lrsbaseeditor
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../rsbaseeditor/debug/ -lrsbaseeditor
else:unix: LIBS += -L$$OUT_PWD/../rsbaseeditor/ -lrsbaseeditor

INCLUDEPATH += $$PWD/../rsbaseeditor
DEPENDPATH += $$PWD/../rsbaseeditor

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../rsbaseeditor/release/librsbaseeditor.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../rsbaseeditor/debug/librsbaseeditor.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../rsbaseeditor/release/rsbaseeditor.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../rsbaseeditor/debug/rsbaseeditor.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../rsbaseeditor/librsbaseeditor.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../rsreslbrfn/release/ -lrsreslbrfn
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../rsreslbrfn/debug/ -lrsreslbrfn
else:unix: LIBS += -L$$OUT_PWD/../rsreslbrfn/ -lrsreslbrfn

INCLUDEPATH += $$PWD/../rsreslbrfn
DEPENDPATH += $$PWD/../rsreslbrfn
