#include "selectresourcedlg.h"
#include "reslistdockwidget.h"
#include "ui_selectresourcedlg.h"
#include <QMainWindow>
#include <QTreeView>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QIcon>

SelectResourceDlg::SelectResourceDlg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SelectResourceDlg)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/img/openbook.png"));
    m_pContainer = ResListDockWidget::MakeContainer();

    ui->verticalLayout->insertWidget(0, m_pContainer);

    QTreeView *m_List = m_pContainer->findChild<QTreeView*>("pList");
    m_List->setSortingEnabled(true);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    connect(m_List, &QTreeView::doubleClicked, this, &SelectResourceDlg::onDoubleClicked);
    connect(m_List, &QTreeView::clicked, this, &SelectResourceDlg::onClicked);
}

SelectResourceDlg::~SelectResourceDlg()
{
    delete ui;
}

void SelectResourceDlg::setModel(QAbstractItemModel *model)
{
    ResListDockWidget::SetModel(m_pContainer, model);
}

void SelectResourceDlg::onClicked(const QModelIndex &index)
{
    QVariant var = m_pContainer->property("m_pFiler");
    ResFilterModel *m_pFiler = nullptr;

    if (var.isValid())
        m_pFiler = var.value<ResFilterModel*>();

    QModelIndex source = m_pFiler->mapToSource(index);
    ResFilterModel *model = qobject_cast<ResFilterModel*>(m_pFiler);

    model->getResNameAndType(source, m_name, m_type);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

void SelectResourceDlg::onDoubleClicked(const QModelIndex &index)
{
    onClicked(index);
    emit accept();
}

void SelectResourceDlg::getRes(int &type, QString &name)
{
    type = m_type;
    name = m_name;
}
