#ifndef BASERESOURCEEDITOR_H
#define BASERESOURCEEDITOR_H

#include "ResourceEditorInterface.h"
#include "restemplateregistry.h"
#include <QObject>

class ErrorsModel;
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
    virtual ResourceEditorResult newItemsAction(const QString &guid, const QString &name, const QString &path, QWidget *parent) Q_DECL_FINAL;

    QList<SARibbonContextCategory*> contextCategoryes(SARibbonBar *ribbon) Q_DECL_FINAL;
private:
    void ShowErrors(ErrorsModel *model, QWidget *parent);
    void SetupEditorTitle(BaseEditorWindow *wnd, const qint16 &Type,
                          const QString &name, const QString &title);

    BaseEditorWindow *LoadResFromXmlTemplate(const QString &filename, const QString &name, const quint16 &type, ErrorsModel *model);
    BaseEditorWindow *LoadResFromXmlTemplate(QIODevice *device, const QString &name, const std::initializer_list<quint16> &type, ErrorsModel *model);

    // Пользовательские шаблоны панелей (xml + json-дескриптор)
    ResTemplateRegistry m_templates;
};

#endif // BASERESOURCEEDITOR_H
