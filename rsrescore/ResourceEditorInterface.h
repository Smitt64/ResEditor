#ifndef RESOURCEEDITORINTERFACE_H
#define RESOURCEEDITORINTERFACE_H

#include "rsrescore_global.h"
#include <QtPlugin>
#include <QString>

class BaseEditorWindow;
typedef struct
{
    BaseEditorWindow *wnd = nullptr;
    bool succeed = false;
    // Путь к файлу, созданному действием (например, новая библиотека
    // ресурсов): главное окно открывает его в приложении.
    // Пусто, если действие файл не создавало
    QString fileName;
}ResourceEditorResult;

class ResLib;
class LbrObjectInterface;
class BaseEditorWindow;
class SARibbonBar;
class SARibbonContextCategory;
class ResXmlReader;
class RSRESCORE_EXPORT ResourceEditorInterface
{
public:
    virtual ~ResourceEditorInterface() = default;
    virtual QList<qint16> resTypes() const = 0;

    virtual BaseEditorWindow *editor(const qint16 &Type, const QString &name, LbrObjectInterface *pLbrObj) = 0;

    virtual QString newItemsMetaList() = 0;
    virtual bool newItemsActionAvalible(const QString &guid) = 0;
    virtual ResourceEditorResult newItemsAction(const QString &guid, const QString &name, const QString &path, QWidget *parent = nullptr) = 0;

    virtual QList<SARibbonContextCategory*> contextCategoryes(SARibbonBar *ribbon) = 0;

    // XML-импортер ресурсов плагина (элементы своих типов внутри <reslib>)
    // для ResXmlLoaderChain. Экземпляром владеет плагин; nullptr —
    // импортера нет
    virtual ResXmlReader *xmlImporter() { return nullptr; }
};

#define ResourceEditorInterface_iid "org.rstyle.ResourceEditorInterface"
Q_DECLARE_INTERFACE(ResourceEditorInterface, ResourceEditorInterface_iid)

#endif // RESOURCEEDITORINTERFACE_H
