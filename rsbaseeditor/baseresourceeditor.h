#ifndef BASERESOURCEEDITOR_H
#define BASERESOURCEEDITOR_H

#include "ResourceEditorInterface.h"
#include <QObject>

class BaseResourceEditor : public QObject, public ResourceEditorInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.rstyle.BaseResourceEditorInterface" FILE "baseeditor.json")
    Q_INTERFACES(ResourceEditorInterface)
public:
    BaseResourceEditor();
    virtual ~BaseResourceEditor();

    virtual QList<qint16> resTypes() const Q_DECL_FINAL;
    virtual BaseEditorWindow *editor(const qint16 &Type, const QString &name, ResLib *pResLib) Q_DECL_FINAL;
};

#endif // BASERESOURCEEDITOR_H
