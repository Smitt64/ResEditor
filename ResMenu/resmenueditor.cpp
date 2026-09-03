#include "ResMenuEditor.h"
#include <lbrobject.h>
#include "SARibbon.h"
#include "commandidpropertytreeitem.h"
#include "iconpropertytreeitem.h"
#include "menueditorwindow.h"
#include "menuxmlloader.h"
#include "resbuffer.h"
#include "rsrescore.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>

namespace {

// Пункт интерфейса создания из MenuNewItems.json по его guid.
// Пустой объект — guid неизвестен
QJsonObject menuNewItemByGuid(const QString &guid)
{
    QFile f(QStringLiteral(":/json/MenuNewItems.json"));
    if (!f.open(QIODevice::ReadOnly))
        return QJsonObject();

    const QJsonArray items = QJsonDocument::fromJson(f.readAll())
                                 .object()
                                 .value(QStringLiteral("items")).toArray();

    for (const QJsonValue &v : items)
    {
        const QJsonObject obj = v.toObject();
        if (!obj.value(QStringLiteral("action")).toString().compare(
                guid, Qt::CaseInsensitive))
            return obj;
    }
    return QJsonObject();
}

} // anonymous namespace

ResMenuEditorPlugin::ResMenuEditorPlugin() :
    QObject(nullptr),
    ResourceEditorInterface()
{
    // Для "viewclass" : "..." в MenuItem.json —
    // PropertyModel находит класс по имени через QMetaType
    qRegisterMetaType<IconPropertyTreeItem*>();
    qRegisterMetaType<CommandIdPropertyTreeItem*>();

    // Пользовательские шаблоны меню: пользовательский каталог
    // в приоритете над рабочим и программным (добавлен первым)
    m_templates.addSearchDir(ResTemplateRegistry::userTemplatesDir(QStringLiteral("menu")));
    m_templates.addSearchDir(ResTemplateRegistry::workingTemplatesDir(QStringLiteral("menu")));
    m_templates.addSearchDir(ResTemplateRegistry::programTemplatesDir(QStringLiteral("menu")));
}

ResMenuEditorPlugin::~ResMenuEditorPlugin()
{

}

QList<qint16> ResMenuEditorPlugin::resTypes() const
{
    return QList<qint16>()
        << LbrObject::RES_MENU
        << LbrObject::RES_MENU2;
}

ResXmlReader *ResMenuEditorPlugin::xmlImporter()
{
    if (!m_XmlImporter)
        m_XmlImporter.reset(new MenuXmlLoader());

    return m_XmlImporter.data();
}

QList<SARibbonContextCategory*> ResMenuEditorPlugin::contextCategoryes(SARibbonBar *ribbon)
{
    SARibbonContextCategory *rescat = ribbon->addContextCategory(tr("Ресурс"), QColor(0xBFFFBF), 0);
    return { rescat };
}

QString ResMenuEditorPlugin::newItemsMetaList()
{
    QFile f(":/json/MenuNewItems.json");
    f.open(QIODevice::ReadOnly);

    QTextStream stream(&f);
    stream.setCodec("UTF-8");

    // Диалог создания пересобирается каждый раз — перечитываем шаблоны,
    // подложенные файлы видны без перезапуска приложения
    m_templates.rescan();

    return m_templates.appendToMetaList(stream.readAll());
}

bool ResMenuEditorPlugin::newItemsActionAvalible(const QString &guid)
{
    // обрабатываем все пункты, перечисленные в MenuNewItems.json,
    // и пользовательские шаблоны меню
    return !menuNewItemByGuid(guid).isEmpty() || m_templates.contains(guid);
}

ResourceEditorResult ResMenuEditorPlugin::newItemsAction(const QString &guid, const QString &name, const QString &path, QWidget *parent)
{
    ResourceEditorResult result;

    QString tpl;
    const QJsonObject item = menuNewItemByGuid(guid);

    if (!item.isEmpty())
    {
        // Шаблон меню: опциональное поле "template" пункта — путь к XML
        // (ресурс qrc ":/..." или файл на диске) в формате экспорта
        tpl = item.value(QStringLiteral("template")).toString();
    }
    else if (m_templates.contains(guid))
    {
        // Пользовательский шаблон меню из файла
        tpl = m_templates.templatePath(guid);
    }
    else
        return result;

    // Создать новое меню для библиотеки ресурсов
    MenuEditorWindow *pNewEditor = new MenuEditorWindow();
    pNewEditor->setWindowIcon(RsResCore::inst()->iconFromResType(LbrObject::RES_MENU2));
    pNewEditor->setupEditor();

    if (!tpl.isEmpty())
    {
        QString error;
        if (!pNewEditor->loadFromXmlFile(tpl, &error))
        {
            QMessageBox::warning(parent, tr("Создание меню"),
                                 tr("Не удалось загрузить шаблон меню:\n%1\n%2")
                                     .arg(tpl, error));
            delete pNewEditor;
            return result;
        }
    }

    // имя ресурса из диалога создания (needname в MenuNewItems.json)
    // важнее имени, записанного в шаблоне
    if (!name.isEmpty())
        pNewEditor->setMenuName(name);

    SetupEditorTitle(pNewEditor, LbrObject::RES_MENU2, name, QString());

    result.succeed = true;
    result.wnd = pNewEditor;
    return result;
}

BaseEditorWindow *ResMenuEditorPlugin::editor(const qint16 &Type, const QString &name, LbrObjectInterface *pLbrObj)
{
    BaseEditorWindow *wnd = nullptr;

    static const QList<quint16> Types =
    {
        LbrObject::RES_MENU,
        LbrObject::RES_MENU2
    };

    if (Types.contains(Type))
    {
        //MenuEditorWindow *wn
        wnd = new MenuEditorWindow();
        wnd->setWindowIcon(RsResCore::inst()->iconFromResType(Type));
        wnd->setupEditor();

        ResBuffer *buffer = nullptr;
        pLbrObj->getResource(name, Type, &buffer);

        if (buffer)
        {
            qobject_cast<MenuEditorWindow*>(wnd)->setMenu(buffer);
            delete buffer;

            SetupEditorTitle(wnd, Type, name, /*testPan->title()*/QString());
        }
        else
        {
            delete wnd;
            wnd = nullptr;
        }
        /*wnd = new StdPanelEditor(Type);
        wnd->setWindowIcon(RsResCore::inst()->iconFromResType(Type));
        wnd->setupEditor();

        ResBuffer *buffer = nullptr;
        pLbrObj->getResource(name, Type, &buffer);

        if (buffer)
        {
            QScopedPointer<ResPanel> testPan(new ResPanel());
            testPan->load(buffer);
            qobject_cast<StdPanelEditor*>(wnd)->setPanel(testPan.data(), buffer->comment());
            delete buffer;

            SetupEditorTitle(wnd, Type, name, testPan->title());
        }
        else
        {
            delete wnd;
            wnd = nullptr;
        }*/
    }

    return wnd;
}

void ResMenuEditorPlugin::SetupEditorTitle(BaseEditorWindow *wnd, const qint16 &Type, const QString &name, const QString &title)
{
    QString str = QString("%1 [%2]: %3")
                      .arg(RsResCore::inst()->typeNameFromResType(Type), name, title);

    qDebug() << str;
    wnd->setWindowTitle(str);
}