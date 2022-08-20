#ifndef PROPERTYDOCKWIDGET_H
#define PROPERTYDOCKWIDGET_H

#include "rsrescore_global.h"
#include <QDockWidget>
#include <QObject>
#include <QSet>

class QMainWindow;
class PropertyTreeView;
class QAbstractItemModel;
class RSRESCORE_EXPORT PropertyDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    PropertyDockWidget(QWidget *paretn = nullptr);
    virtual ~PropertyDockWidget();

public slots:
    void setPropertyModel(QAbstractItemModel *model);

private:
    QMainWindow *m_pContainer;
    PropertyTreeView *m_pTreeView;

    QSet<QString> m_Expanded;
};

#endif // PROPERTYDOCKWIDGET_H
