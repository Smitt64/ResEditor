#include "reslistcategorycontentwidget.h"
#include "lbrobjectinterface.h"
#include "reslistdockwidget.h"
#include <QVBoxLayout>
#include <QMainWindow>

ResListCategoryContentWidget::ResListCategoryContentWidget(LbrObjectInterface *pLbrObj, QWidget *parent) :
    CategoryContentWidget(parent)
{
    setLayoutType(LayoutFullWidget);
    m_pContainer = ResListDockWidget::MakeContainer();
    ResListDockWidget::SetModel(m_pContainer, pLbrObj->list());

    setMainWidget(m_pContainer);

    QTreeView *m_List = m_pContainer->findChild<QTreeView*>("pList");
    m_List->setSortingEnabled(true);
    m_List->sortByColumn(0, Qt::AscendingOrder);

    connect(m_List, &QTreeView::clicked, this, &ResListCategoryContentWidget::onClicked);
    connect(m_List, &QTreeView::doubleClicked, this, [=](const QModelIndex &index)
    {
        onClicked(index);
        emit openResource(m_name, m_type);
    });
}

ResListCategoryContentWidget::~ResListCategoryContentWidget()
{

}

void ResListCategoryContentWidget::onClicked(const QModelIndex &index)
{
    QVariant var = m_pContainer->property("m_pFiler");
    ResFilterModel *m_pFiler = nullptr;

    if (var.isValid())
        m_pFiler = var.value<ResFilterModel*>();

    QModelIndex source = m_pFiler->mapToSource(index);
    ResFilterModel *model = qobject_cast<ResFilterModel*>(m_pFiler);

    model->getResNameAndType(source, m_name, m_type);
    //ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}