#ifndef RSRESCORE_H
#define RSRESCORE_H

#include "rsrescore_global.h"
#include <QIcon>
#include <QMultiHash>

class ResourceEditorInterface;
class RSRESCORE_EXPORT RsResCore
{
public:
    static RsResCore *inst();

    static QIcon iconFromResType(const qint16 &Type);
    static QString typeNameFromResType(const qint16 &Type);

    ResourceEditorInterface *pluginForType(const qint16 &Type);
private:
    RsResCore();
    void loadPlugins();

    static RsResCore *m_Inst;

    QList<ResourceEditorInterface*> m_Plugins;
    QMultiHash<qint16,ResourceEditorInterface*> m_PluginTypes;
};

#endif // RSRESCORE_H
