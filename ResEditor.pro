TEMPLATE = subdirs

SUBDIRS += \
    PropertyEditor \
    ResMenu \
    RsResEditor \
    rsbaseeditor \
    rsrescore \
    rsreslbrfn \
    ToolsRuntimeProj

rsrescore.depends = rsbaseeditor rsreslbrfn PropertyEditor ToolsRuntimeProj
ResMenu.depends = rsrescore rsbaseeditor rsreslbrfn PropertyEditor ToolsRuntimeProj
RsResEditor.depends = rsreslbrfn rsrescore ToolsRuntimeProj
