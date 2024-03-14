TEMPLATE = subdirs

SUBDIRS += \
    PropertyEditor \
    RsResEditor \
    rsbaseeditor \
    rsrescore \
    rsreslbrfn \
    RsWorkTools/ToolsRuntime

rsrescore.depends = rsbaseeditor rsreslbrfn PropertyEditor RsWorkTools/ToolsRuntime
RsResEditor.depends = rsreslbrfn rsrescore RsWorkTools/ToolsRuntime
