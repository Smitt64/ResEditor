#ifndef TOOLBOXDOCKWIDGET_H
#define TOOLBOXDOCKWIDGET_H

#include "rsrescore_global.h"
#include <QDockWidget>

class ToolBoxModel;
class QTreeView;
class RSRESCORE_EXPORT ToolBoxDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    ToolBoxDockWidget(QWidget *parent = nullptr);
    virtual ~ToolBoxDockWidget();

    void setModel(ToolBoxModel *model);

private:
    QTreeView *m_ToolBoxView;
};

#endif // TOOLBOXDOCKWIDGET_H
