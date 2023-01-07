#ifndef RSRESCORE_H
#define RSRESCORE_H

#include "qaction.h"
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

RSRESCORE_EXPORT void AddShortcutToToolTip(QAction *action);
RSRESCORE_EXPORT void AddShortcutToToolTip(QToolButton *action);

template<class T> bool IsIn(const T &what, int count, ...)
{
    int i = 0;
    bool result = false;
    va_list args;
    va_start(args, count);
    while (i < count && !result)
    {
        i++;
        const T &value = va_arg(args, T);

        if (what == value)
            result = true;
    }
    va_end(args);

    return result;
}

#endif // RSRESCORE_H
