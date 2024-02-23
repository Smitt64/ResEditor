QT += widgets uitools xml

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
    controlpropertysdlg.cpp \
    controtaborder.cpp \
    customrectitem.cpp \
    easywinini.cpp \
    panelitem.cpp \
    panelpropertysdlg.cpp \
    propertymodel/ctrltabordertreeitem.cpp \
    propertymodel/ewtextstylepropertytreeitem.cpp \
    propertymodel/labeltextpropertyitem.cpp \
    propertymodel/propertydockwidget.cpp \
    propertymodel/textstyledlg.cpp \
    reslibdirmodel.cpp \
    reslistdockwidget.cpp \
    respanel.cpp \
    rsrescore.cpp \
    scrolitem.cpp \
    statusbarelement.cpp \
    stdpaneleditor.cpp \
    styles/extextstyle.cpp \
    styles/resstyle.cpp \
    textitem.cpp \
    toolbox/toolboxdockwidget.cpp \
    toolbox/toolboxmodel.cpp \
    toolbox/toolboxtreeitem.cpp \
    toolbox/toolboxtreeview.cpp \
    toolbox/toolboxwidgetinterface.cpp \
    undoredo/undoitemadd.cpp \
    undoredo/undoitemdelete.cpp \
    undoredo/undoitemmove.cpp \
    undoredo/undoitemresize.cpp \
    undoredo/undopropertychange.cpp \
    widgets/characterwidget.cpp \
    widgets/controltaborderedit.cpp \
    widgets/labeltexteditdlg.cpp

HEADERS += \
    ResourceEditorInterface.h \
    RsResInterface.h \
    baseeditorview.h \
    baseeditorwindow.h \
    #baseresourceeditor.h \
    basescene.h \
    containeritem.h \
    controlitem.h \
    controlpropertysdlg.h \
    controtaborder.h \
    customrectitem.h \
    easywinini.h \
    panelitem.h \
    panelpropertysdlg.h \
    propertymodel/ctrltabordertreeitem.h \
    propertymodel/ewtextstylepropertytreeitem.h \
    propertymodel/labeltextpropertyitem.h \
    propertymodel/propertydockwidget.h \
    propertymodel/textstyledlg.h \
    reslibdirmodel.h \
    reslistdockwidget.h \
    respanel.h \
    rscoreheader.h \
    rsrescore_global.h \
    rsrescore.h \
    scrolitem.h \
    statusbarelement.h \
    stdpaneleditor.h \
    styles/extextstyle.h \
    styles/resstyle.h \
    textitem.h \
    toolbox/toolboxdockwidget.h \
    toolbox/toolboxmodel.h \
    toolbox/toolboxtreeitem.h \
    toolbox/toolboxtreeview.h \
    toolbox/toolboxwidgetinterface.h \
    undoredo/undoitemadd.h \
    undoredo/undoitemdelete.h \
    undoredo/undoitemmove.h \
    undoredo/undoitemresize.h \
    undoredo/undopropertychange.h \
    widgets/characterwidget.h \
    widgets/controltaborderedit.h \
    widgets/labeltexteditdlg.h

FORMS += \
    controlpropertysdlg.ui \
    panelpropertysdlg.ui \
    propertymodel/textstyledlg.ui \
    widgets/controltaborderedit.ui \
    widgets/labeltexteditdlg.ui

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
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
else:unix:!macx: LIBS += -L$$OUT_PWD/../rsreslbrfn/ -lrsreslbrfn

INCLUDEPATH += $$PWD/../rsreslbrfn
DEPENDPATH += $$PWD/../rsreslbrfn

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../PropertyEditor/release/ -lPropertyEditor
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../PropertyEditor/debug/ -lPropertyEditor
else:unix: LIBS += -L$$OUT_PWD/../PropertyEditor/ -lPropertyEditor

INCLUDEPATH += $$PWD/../PropertyEditor
DEPENDPATH += $$PWD/../PropertyEditor

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../PropertyEditor/release/libPropertyEditor.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../PropertyEditor/debug/libPropertyEditor.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../PropertyEditor/release/PropertyEditor.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../PropertyEditor/debug/PropertyEditor.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../PropertyEditor/libPropertyEditor.a
