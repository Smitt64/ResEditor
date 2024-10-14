QT += widgets

TEMPLATE = lib
CONFIG += staticlib static_runtime

CONFIG += c++11 plugin static

include(../config.pri)
#QMAKE_CXXFLAGS_DEBUG += /MTd

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/../rsrescore

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    baseresourceeditor.h\
    wizards/texttopanel/linescanner.h \
    wizards/texttopanel/textpage.h \
    wizards/texttopanel/texttopanelwizard.h

SOURCES += \
    baseresourceeditor.cpp \
    wizards/texttopanel/linescanner.cpp \
    wizards/texttopanel/textpage.cpp \
    wizards/texttopanel/texttopanelwizard.cpp

FORMS += \
    wizards/texttopanel/textpage.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../rsreslbrfn/release/ -lrsreslbrfn
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../rsreslbrfn/debug/ -lrsreslbrfn
else:unix:!macx: LIBS += -L$$OUT_PWD/../rsreslbrfn/ -lrsreslbrfn

INCLUDEPATH += $$PWD/../rsreslbrfn
DEPENDPATH += $$PWD/../rsreslbrfn
