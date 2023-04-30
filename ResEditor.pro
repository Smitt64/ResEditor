TEMPLATE = subdirs

SUBDIRS += \
    RsResEditor \
    rsbaseeditor \
    rsrescore \
    rsreslbrfn

rsrescore.depends = rsbaseeditor rsreslbrfn
RsResEditor.depends = rsreslbrfn rsrescore
