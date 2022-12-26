#include "rsrescore.h"
#include "ResourceEditorInterface.h"
#include "reslib.h"
#include "controlitem.h"
#include "textitem.h"
#include "panelitem.h"
#include "propertymodel/ewtextstylepropertytreeitem.h"
#include "styles/extextstyle.h"
#include <QPluginLoader>
#include <QFontDatabase>

Q_IMPORT_PLUGIN(BaseResourceEditor)

RsResCore *RsResCore::m_Inst = nullptr;
RsResCore::RsResCore()
{
    loadPlugins();
    QFontDatabase::addApplicationFont("TerminalVector.ttf");

    qRegisterMetaType<CustomRectItem*>();
    qRegisterMetaType<ControlItem*>();
    qRegisterMetaType<TextItem*>();
    qRegisterMetaType<ContainerItem*>();
    qRegisterMetaType<PanelItem*>();
    qRegisterMetaType<EwTextStyle>();
    qRegisterMetaType<EwTextStylePropertyTreeItem*>();
    qRegisterMetaType<QJsonObject>();

    QMetaType::registerComparators<EwTextStyle>();
    QMetaType::registerConverter<EwTextStyle, QJsonObject>(EwTextStyle::toJsonObject);
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
    case RES_PANEL:
        val = QIcon(":/img/Panel.png");
        break;
    case RES_SCROL:
        val = QIcon(":/img/Scrol.png");
        break;
    case RES_BS:
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
    case RES_MENU:
        val = "MENU";
        break;
    case RES_STAT:
        val = "STAT";
        break;
    case RES_DIALOG:
        val = "DIALOG";
        break;
    case RES_HIST:
        val = "HIST";
        break;
    case RES_REPORT:
        val = "REPORT";
        break;
    case RES_BFSTRUCT:
        val = "BFSTRUCT";
        break;
    case RES_DBLINK:
        val = "DBLINK";
        break;
    case RES_PANEL:
        val = "PANEL";
        break;
    case RES_SCROL:
        val = "SCROL";
        break;
    case RES_REP:
        val = "REP";
        break;
    case RES_BS:
        val = "BSCROL";
        break;
    case RES_LS:
        val = "LS";
        break;
    case RES_ACCEL:
        val = "ACCEL";
        break;
    case RES_STRTABLE:
        val = "STRTABLE";
        break;
    case RES_MENU2:
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
    {
        return values.first();
    }

    return nullptr;
}
