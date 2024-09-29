#include "recentlbrlist.h"
#include "qsettings.h"
#include <QFileInfo>
#include <QDebug>

RecentLbrList::RecentLbrList(QSettings *settings) :
    QObject(),
    _settings(settings)
{
    for (int i = 0; i < MaxRecentFiles; ++i) 
    {
         recentFileActs[i] = new QAction(this);
         recentFileActs[i]->setVisible(false);
    }

    int size = _settings->beginReadArray("RecentFiles");
    for (int i = 0; i < size; ++i)
    {
        _settings->setArrayIndex(i);
        _addFile(_settings->value("FileName").toString());
    }
    _settings->endArray();

    updateRecentFileActions();
}

void RecentLbrList::_addFile(const QString &fileName)
{
    m_files.removeAll(fileName);
    m_files.prepend(fileName);

    if (m_files.size() > MaxRecentFiles)
        m_files.removeLast();
}

void RecentLbrList::addFile(const QString &fileName)
{
    _addFile(fileName);
    updateRecentFileActions();
}

void RecentLbrList::updateRecentFileActions()
{
    for (int i = 0; i < m_files.size(); i++)
    {
        QFileInfo file = QFileInfo(m_files.at(i));

        QAction *action = recentFileActs[i];
        action->setText(file.fileName());
        action->setData(m_files.at(i));
        action->setToolTip(m_files.at(i));
        action->setStatusTip(m_files.at(i));
        action->setVisible(true);
    }

    _settings->beginWriteArray("RecentFiles");
    for (int i = 0; i < m_files.size(); i++)
    {    
        _settings->setArrayIndex(i);
        _settings->setValue("FileName", m_files.at(i));
    }
    _settings->endArray();
    _settings->sync();

    emit actionsChanged();
}

QList<QAction*> RecentLbrList::actions()
{
    QList<QAction*> actions;

    for (int i = 0; i < MaxRecentFiles; ++i) 
        actions.append(recentFileActs[i]);

    return actions;
}
