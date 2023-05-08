#include "rsrescore.h"
#include "ResourceEditorInterface.h"
#include "controtaborder.h"
#include "propertymodel/ctrltabordertreeitem.h"
#include "propertymodel/labeltextpropertyitem.h"
#include "qapplication.h"
#include "lbrobject.h"
#include "controlitem.h"
#include "textitem.h"
#include "panelitem.h"
#include "respanel.h"
#include "propertymodel/ewtextstylepropertytreeitem.h"
#include "styles/extextstyle.h"
#include <QPluginLoader>
#include <QFontDatabase>
#include <QToolButton>
#include <QDomDocument>

Q_IMPORT_PLUGIN(BaseResourceEditor)

RsResCore *RsResCore::m_Inst = nullptr;

RsResCore::RsResCore()
{
}

void RsResCore::init()
{
    loadPlugins();
    QFontDatabase::addApplicationFont("TerminalVector.ttf");

    qRegisterMetaType<CustomRectItem*>();
    qRegisterMetaType<ControlItem*>();
    qRegisterMetaType<TextItem*>();
    qRegisterMetaType<ContainerItem*>();
    qRegisterMetaType<PanelItem*>();
    qRegisterMetaType<BorderItem*>();
    qRegisterMetaType<ContainerItem*>();
    qRegisterMetaType<EwTextStyle>();
    qRegisterMetaType<ControTabOrder>();
    qRegisterMetaType<EwTextStylePropertyTreeItem*>();
    qRegisterMetaType<CtrlTabOrderTreeItem*>();
    qRegisterMetaType<LabelTextPropertyItem*>();
    qRegisterMetaType<QJsonObject>();

    QMetaType::registerComparators<EwTextStyle>();
    QMetaType::registerConverter<EwTextStyle, QJsonObject>(EwTextStyle::toJsonObject);
    QMetaType::registerConverter<QJsonObject, EwTextStyle>(EwTextStyle::fromJsonObject);

    QMetaType::registerComparators<ControTabOrder>();
    QMetaType::registerConverter<ControTabOrder, QJsonObject>(ControTabOrder::toJsonObject);
    QMetaType::registerConverter<QJsonObject, ControTabOrder>(ControTabOrder::fromJsonObject);
}

RsResCore *RsResCore::inst()
{
    if (!m_Inst)
        m_Inst = new RsResCore();
    return m_Inst;
}

QIcon RsResCore::iconFromResType(const qint16 &Type)
{
    QIcon val;
    switch(Type)
    {
    case LbrObject::RES_PANEL:
        val = QIcon(":/img/Panel.png");
        break;
    case LbrObject::RES_SCROL:
        val = QIcon(":/img/Scrol.png");
        break;
    case LbrObject::RES_BS:
        val = QIcon(":/img/BScrol.png");
        break;
    default:
        val = QIcon(":/img/Unknown.png");
    }

    return val;
}

QString RsResCore::typeNameFromResType(const qint16 &Type)
{
    QString val;
    switch(Type)
    {
    case LbrObject::RES_MENU:
        val = "MENU";
        break;
    case LbrObject::RES_STAT:
        val = "STAT";
        break;
    case LbrObject::RES_DIALOG:
        val = "DIALOG";
        break;
    case LbrObject::RES_HIST:
        val = "HIST";
        break;
    case LbrObject::RES_REPORT:
        val = "REPORT";
        break;
    case LbrObject::RES_BFSTRUCT:
        val = "BFSTRUCT";
        break;
    case LbrObject::RES_DBLINK:
        val = "DBLINK";
        break;
    case LbrObject::RES_PANEL:
        val = "PANEL";
        break;
    case LbrObject::RES_SCROL:
        val = "SCROL";
        break;
    case LbrObject::RES_REP:
        val = "REP";
        break;
    case LbrObject::RES_BS:
        val = "BSCROL";
        break;
    case LbrObject::RES_LS:
        val = "LS";
        break;
    case LbrObject::RES_ACCEL:
        val = "ACCEL";
        break;
    case LbrObject::RES_STRTABLE:
        val = "STRTABLE";
        break;
    case LbrObject::RES_MENU2:
        val = "MENU2";
        break;
    }
    return val;
}

void RsResCore::loadPlugins()
{
    const auto staticInstances = QPluginLoader::staticInstances();

    for (QObject *plug : staticInstances)
    {
        ResourceEditorInterface *plugin = qobject_cast<ResourceEditorInterface*>(plug);

        if (plugin)
        {
            m_Plugins.append(plugin);

            QList<qint16> types = plugin->resTypes();
            for (qint16 type : qAsConst(types))
                m_PluginTypes.insert(type, plugin);
        }
    }
}

ResourceEditorInterface *RsResCore::pluginForType(const qint16 &Type)
{
    QList<ResourceEditorInterface*> values = m_PluginTypes.values(Type);

    if (!values.empty())
        return values.first();

    return nullptr;
}

ResourceEditorInterface *RsResCore::pluginForNewAction(const QString &guid)
{
    ResourceEditorInterface *plugin = nullptr;
    for (ResourceEditorInterface *item : qAsConst(m_Plugins))
    {
        if (item->newItemsActionAvalible(guid))
        {
            plugin = item;
            break;
        }
    }

    return plugin;
}

QStringList RsResCore::newItemsMetaList() const
{
    QStringList metalist;

    for (ResourceEditorInterface *item : m_Plugins)
    {
        QString str = item->newItemsMetaList();

        if (!str.isEmpty())
            metalist.push_back(str);
    }

    return metalist;
}

void RsResCore::loadFromXml(QIODevice *device, ResPanel **panel)
{
    QDomDocument doc;
    doc.setContent(device);

    QDomElement root = doc.documentElement();

    if (root.tagName() == "reslib")
    {
        QDomNode reslibnode = root.firstChild();
        QDomElement reslib = reslibnode.toElement();

        if (reslib.tagName() == "panel")
        {
            *panel = new ResPanel();
            ResPanel *ptr = *panel;
            ptr->loadXmlNode(reslib);
        }
    }
}

static QString strippedActionText(QString s)
{
    s.remove(QString::fromLatin1("..."));
    for (int i = 0; i < s.size(); ++i)
    {
        if (s.at(i) == QLatin1Char('&'))
            s.remove(i, 1);
    }

    return s.trimmed();
}

template<class T>void AddShortcutToToolTip(T *action)
{
    if (!action->shortcut().isEmpty())
    {
        QString tooltip = action->property("tooltipBackup").toString();
        if (tooltip.isEmpty())
        {
            tooltip = action->toolTip();
            if (tooltip != strippedActionText(action->text()))
                action->setProperty("tooltipBackup", action->toolTip());  // action uses a custom tooltip. Backup so that we can restore it later.
        }

        QColor shortcutTextColor = QApplication::palette().color(QPalette::ToolTipText);
        QString shortCutTextColorName;
        if (shortcutTextColor.value() == 0)
            shortCutTextColorName = "gray";  // special handling for black because lighter() does not work there [QTBUG-9343].
        else
        {
            int factor = (shortcutTextColor.value() < 128) ? 150 : 50;
            shortCutTextColorName = shortcutTextColor.lighter(factor).name();
        }
        action->setToolTip(QString("<p style='white-space:pre'>%1&nbsp;&nbsp;<code style='color:%2; font-size:small'>%3</code></p>")
                           .arg(tooltip, shortCutTextColorName, action->shortcut().toString(QKeySequence::NativeText)));
    }
}

void AddShortcutToToolTip(QAction *action)
{
    AddShortcutToToolTip<QAction>(action);
}

void AddShortcutToToolTip(QToolButton *action)
{
    AddShortcutToToolTip<QToolButton>(action);
}
