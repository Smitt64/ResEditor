#ifndef RESMENUEDITORPLUGIN_H
#define RESMENUEDITORPLUGIN_H

#define BASERESOURCEEDITOR_H

#include "ResourceEditorInterface.h"
#include "restemplateregistry.h"
#include <QObject>
#include <QScopedPointer>

class SARibbonContextCategory;
class MenuXmlLoader;
class ResMenuEditorPlugin : public QObject, public ResourceEditorInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.rstyle.ResMenuEditorPlugin" FILE "menueditor.json")
    Q_INTERFACES(ResourceEditorInterface)
public:
    ResMenuEditorPlugin();
    virtual ~ResMenuEditorPlugin();

    virtual QList<qint16> resTypes() const Q_DECL_FINAL;
    virtual BaseEditorWindow *editor(const qint16 &Type, const QString &name, LbrObjectInterface *pLbrObj) Q_DECL_OVERRIDE;

    virtual QString newItemsMetaList() Q_DECL_FINAL;
    virtual bool newItemsActionAvalible(const QString &guid) Q_DECL_FINAL;
    virtual ResourceEditorResult newItemsAction(const QString &guid, const QString &name, const QString &path, QWidget *parent) Q_DECL_FINAL;

    QList<SARibbonContextCategory*> contextCategoryes(SARibbonBar *ribbon) Q_DECL_FINAL;

    // XML-импортер элементов <menu> для ResXmlLoaderChain
    // (создаётся лениво, владеет плагин)
    virtual ResXmlReader *xmlImporter() Q_DECL_OVERRIDE;

signals:

private:
    void SetupEditorTitle(BaseEditorWindow *wnd, const qint16 &Type, const QString &name, const QString &title);

    QScopedPointer<MenuXmlLoader> m_XmlImporter;

    // Пользовательские шаблоны меню (xml + json-дескриптор)
    ResTemplateRegistry m_templates;
};

#endif // RESMENUEDITORPLUGIN_H
