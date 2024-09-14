#include "propertydockwidget.h"
#include "propertytreeview.h"
#include "propertymodel.h"
#include "customrectitem.h"
#include <QGraphicsScene>
#include <QMainWindow>
#include <QDebug>
#include <QHeaderView>
#include <QSplitter>
// https://stackoverflow.com/questions/27149733/qtreeview-merge-some-cells
PropertyDockWidget::PropertyDockWidget(QWidget *paretn) :
    QDockWidget(paretn)
{
    m_pTreeView = new PropertyTreeView(this);
    m_pTreeView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_pContainer = new QMainWindow();
    m_pContainer->setMinimumWidth(350);

    m_pStructView = new QTreeView(this);
    m_pStructView->setRootIsDecorated(false);
    m_pStructView->setIndentation(10);
    m_pStructView->setItemsExpandable(false);
    m_pStructView->header()->setStretchLastSection(false);

    m_pStructView->setMaximumHeight(200);
    m_pStructView->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    m_pSplitter = new QSplitter(Qt::Vertical, this);
    m_pSplitter->setObjectName("PropertyDockSplitter");
    m_pSplitter->addWidget(m_pTreeView);
    m_pSplitter->addWidget(m_pStructView);

    m_pContainer->setCentralWidget(m_pSplitter);
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

    connect(m_pStructView, &QTreeView::clicked, [=](const QModelIndex &index)
    {
        CustomRectItem *item = index.data(Qt::UserRole + 1).value<CustomRectItem*>();

        if (item)
        {
            item->scene()->clearSelection();
            item->setSelected(true);
        }
    });
}

PropertyDockWidget::~PropertyDockWidget()
{

}

void PropertyDockWidget::setStructModel(QAbstractItemModel *model)
{
    m_pStructView->setModel(model);

    if (!model)
        return;

    m_pStructView->expandAll();

    m_pStructView->header()->setSectionResizeMode(0, QHeaderView::Stretch);

    connect(model, &QAbstractItemModel::modelReset, [=]()
    {
        m_pStructView->expandAll();
    });
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
