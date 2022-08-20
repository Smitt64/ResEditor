#include "propertydockwidget.h"
#include "propertytreeview.h"
#include "propertymodel.h"
#include <QMainWindow>
#include <QDebug>
#include <QHeaderView>
// https://stackoverflow.com/questions/27149733/qtreeview-merge-some-cells
PropertyDockWidget::PropertyDockWidget(QWidget *paretn) :
    QDockWidget(paretn)
{
    m_pTreeView = new PropertyTreeView(this);
    m_pContainer = new QMainWindow();
    m_pContainer->setCentralWidget(m_pTreeView);
    m_pContainer->setMinimumWidth(350);
    setWidget(m_pContainer);

    setWindowTitle(tr("Свойства"));

    connect(m_pTreeView, &QTreeView::expanded, [=](const QModelIndex &index)
    {
        m_Expanded.insert(index.data().toString());
    });

    connect(m_pTreeView, &QTreeView::collapsed, [=](const QModelIndex &index)
    {
        m_Expanded.remove(index.data().toString());
    });
}

PropertyDockWidget::~PropertyDockWidget()
{

}

void PropertyDockWidget::setPropertyModel(QAbstractItemModel *model)
{
    m_pTreeView->setModel(model);

    if (!model)
        return;

    for (const auto &str : qAsConst(m_Expanded))
    {
        QModelIndexList lst = model->match(model->index(0, 0), Qt::DisplayRole, str, Qt::MatchFixedString);

        if (!lst.empty())
            m_pTreeView->expand(lst.first());
    };

    m_pTreeView->header()->resizeSection(0, 150);
}
