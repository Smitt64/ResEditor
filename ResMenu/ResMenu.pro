QT += widgets

TEMPLATE = lib
DEFINES += RESMENU_LIBRARY

TARGET = ResMenuPlugin

CONFIG += c++11 plugin static_runtime

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    commandidpropertytreeitem.cpp \
    commandkeydialog.cpp \
    editormenu.cpp \
    editormenubar.cpp \
    iconbrowser.cpp \
    iconpropertytreeitem.cpp \
    iconselectdialog.cpp \
    menuaction.cpp \
    menucodehighlighter.cpp \
    menueditorwindow.cpp \
    menuxmlloader.cpp \
    reskeymap.cpp \
    resmenu.cpp \
    resmenueditor.cpp \
    shortcuttextpropertytreeitem.cpp \
    undoredo/menuundocommands.cpp

HEADERS += \
    ResMenu_global.h \
    commandidpropertytreeitem.h \
    commandkeydialog.h \
    editormenu.h \
    editormenubar.h \
    iconbrowser.h \
    iconpropertytreeitem.h \
    iconselectdialog.h \
    menuaction.h \
    menucodehighlighter.h \
    menueditorwindow.h \
    menuxmlloader.h \
    reskeymap.h \
    resmenu.h \
    resmenueditor.h \
    shortcuttextpropertytreeitem.h \
    undoredo/menuundocommands.h

OTHER_FILES += menueditor.json \
    com.rs.lbr.resmenu.xml \
    doc/menu-resource-format.md \
    json/MenuToolBox.json

target.path = ../bin/reseditor
INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../rsrescore/release/ -lrsrescore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../rsrescore/debug/ -lrsrescore
else:unix: LIBS += -L$$OUT_PWD/../rsrescore/ -lrsrescore

INCLUDEPATH += $$PWD/../rsrescore
DEPENDPATH += $$PWD/../rsrescore

INCLUDEPATH += $$PWD/../PropertyEditor
DEPENDPATH += $$PWD/../PropertyEditor

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../PropertyEditor/release/ -lPropertyEditor
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../PropertyEditor/debug/ -lPropertyEditor
else:unix: LIBS += -L$$OUT_PWD/../PropertyEditor/ -lPropertyEditor

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../PropertyEditor/release/libPropertyEditor.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../PropertyEditor/debug/libPropertyEditor.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../PropertyEditor/release/PropertyEditor.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../PropertyEditor/debug/PropertyEditor.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../PropertyEditor/libPropertyEditor.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../rsreslbrfn/release/ -lrsreslbrfn
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../rsreslbrfn/debug/ -lrsreslbrfn
else:unix: LIBS += -L$$OUT_PWD/../rsreslbrfn/ -lrsreslbrfn

INCLUDEPATH += $$PWD/../rsreslbrfn
DEPENDPATH += $$PWD/../rsreslbrfn

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ToolsRuntimeProj/ToolsRuntime/release/ -lToolsRuntime
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ToolsRuntimeProj/ToolsRuntime/debug/ -lToolsRuntime
else:unix: LIBS += -L$$OUT_PWD/../ToolsRuntimeProj/ToolsRuntime/ -lToolsRuntime

INCLUDEPATH += $$PWD/../ToolsRuntimeProj/ToolsRuntime
DEPENDPATH += $$PWD/../ToolsRuntimeProj/ToolsRuntime

RESOURCES += \
    res.qrc

win32:RC_FILE += res/resmenu.rc

# Иконки плагина: хранятся в папке проекта (icons/vs_theme/...) и при
# сборке копируются в общую папку темы рядом с бинарниками
# (аналогично rsrescore.pro)
ICONS_SRC = $$PWD/icons
TEMPLATES_SRC = $$PWD/templates/menu

defineTest(copyToDestDir) {
    files = $$1
    dir = $$2
    # replace slashes in destination path for Windows
    win32:dir ~= s,/,\\,g

    for(file, files) {
        # replace slashes in source path for Windows
        win32:file ~= s,/,\\,g
        QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$shell_quote($$file) $$shell_quote($$dir) $$escape_expand(\\n\\t)
    }

    export(QMAKE_POST_LINK)
}

copyToDestDir($$ICONS_SRC, $$OUT_PWD/../bin/resources/icons)
copyToDestDir($$TEMPLATES_SRC, $$OUT_PWD/../bin/reseditor/templates/menu)
