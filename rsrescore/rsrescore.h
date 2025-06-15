#ifndef RSRESCORE_H
#define RSRESCORE_H

#include "qaction.h"
#include "rsrescore_global.h"
#include <QIcon>
#include <QMultiHash>

#define DIR_SECTION "dir"
#define USES_SECTION "uses"
#define LBR_RECENTFOLDERS_CONTEXT "LbrFolders"

class ResPanel;
class LbrObjectInterface;
class ResourceEditorInterface;
class SARibbonContextCategory;
class SARibbonBar;
class RSRESCORE_EXPORT RsResCore
{
public:
    RsResCore();
    static RsResCore *inst();

    static QString iconNameFromResType(const qint16 &Type);
    static QIcon iconFromResType(const qint16 &Type);
    static QString typeNameFromResType(const qint16 &Type);
    static QList<qint16> types();
    static QList<qint16> stdTypes();

    ResourceEditorInterface *pluginForType(const qint16 &Type);
    ResourceEditorInterface *pluginForNewAction(const QString &guid);
    QStringList newItemsMetaList() const;
    void init();

    void loadFromXml(QIODevice *device, ResPanel **panel);
    const char *resTypePrefix(int tp);

    QString saveResToXml(const qint16 &Type,
                      const QString &name,
                      LbrObjectInterface *lbr,
                      const QString &dirtemplate,
                      const QString &encode = QString("UTF-8"));

    QList<SARibbonContextCategory*> contextCategoryes(SARibbonBar *ribbon);

private:
    void loadPlugins();

    static RsResCore *m_Inst;

    QList<ResourceEditorInterface*> m_Plugins;
    QMultiHash<qint16,ResourceEditorInterface*> m_PluginTypes;
};

//Q_GLOBAL_STATIC(RsResCore, staticResCore)

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
