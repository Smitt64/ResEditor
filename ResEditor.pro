TEMPLATE = subdirs

SUBDIRS += \
    PropertyEditor \
    RsResEditor \
    rsbaseeditor \
    rsrescore \
    rsreslbrfn \
    ToolsRuntimeProj

rsrescore.depends = rsbaseeditor rsreslbrfn PropertyEditor ToolsRuntimeProj
RsResEditor.depends = rsreslbrfn rsrescore ToolsRuntimeProj
