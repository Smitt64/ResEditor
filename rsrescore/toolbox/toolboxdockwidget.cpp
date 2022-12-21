#include "toolboxdockwidget.h"
#include "toolbox/toolboxmodel.h"
#include "toolbox/toolboxtreeview.h"
#include <QTreeView>

ToolBoxDockWidget::ToolBoxDockWidget(QWidget *parent)  :
    QDockWidget(parent)
{
    m_ToolBoxView = new ToolBoxTreeView(this);
    m_ToolBoxView->setRootIsDecorated(false);
    setWindowTitle(tr("Панель инструментов"));
    setWidget(m_ToolBoxView);
}

ToolBoxDockWidget::~ToolBoxDockWidget()
{

}

void ToolBoxDockWidget::setModel(ToolBoxModel *model)
{
    m_ToolBoxView->setModel(model);
    m_ToolBoxView->expandAll();
}
