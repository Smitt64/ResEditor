#ifndef RECENTLBRLIST_H
#define RECENTLBRLIST_H

#include <QAction>
#include <QSettings>
#include "rsrescore_global.h"

class RSRESCORE_EXPORT RecentLbrList : public QObject
{
    Q_OBJECT
public:
    enum { MaxRecentFiles = 5 };
    RecentLbrList(QSettings *settings);

    void addFile(const QString &fileName);
    QList<QAction*> actions();

signals:
    void actionsChanged();

private:
    void _addFile(const QString &fileName);
    void updateRecentFileActions();
    QAction *recentFileActs[MaxRecentFiles];
    QList<QString> m_files;
    //QList<QAction*> actions;
    QSettings *_settings;
};

#endif // RECENTLBRLIST_H
