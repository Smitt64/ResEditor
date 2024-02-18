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
    virtual BaseEditorWindow *editor(const qint16 &Type, const QString &name, LbrObjectInterface *pLbrObj) Q_DECL_OVERRIDE;

    virtual QString newItemsMetaList() Q_DECL_FINAL;
    virtual bool newItemsActionAvalible(const QString &guid) Q_DECL_FINAL;
    virtual BaseEditorWindow *newItemsAction(const QString &guid, const QString &name, const QString &path) Q_DECL_FINAL;

private:
    void SetupEditorTitle(BaseEditorWindow *wnd, const qint16 &Type,
                          const QString &name, const QString &title);

    BaseEditorWindow *LoadResFromXmlTemplate(const QString &filename, const QString &name, const quint16 &type);
};

#endif // BASERESOURCEEDITOR_H
