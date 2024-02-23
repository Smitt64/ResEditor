TEMPLATE = subdirs

SUBDIRS += \
    PropertyEditor \
    RsResEditor \
    rsbaseeditor \
    rsrescore \
    rsreslbrfn

rsrescore.depends = rsbaseeditor rsreslbrfn PropertyEditor
RsResEditor.depends = rsreslbrfn rsrescore
