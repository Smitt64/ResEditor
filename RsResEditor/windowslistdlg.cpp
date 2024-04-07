#include "windowslistdlg.h"
#include "ui_windowslistdlg.h"
#include "subwindowsmodel.h"
#include <QPushButton>
#include <QMdiSubWindow>

WindowsListDlg::WindowsListDlg(SubWindowsModel *Model, QMdiArea *mdi, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WindowsListDlg)
{
    ui->setupUi(this);

    pSelect = ui->buttonBox->addButton(tr("Выбрать"), QDialogButtonBox::ActionRole);
    pClose = ui->buttonBox->addButton(tr("Закрыть"), QDialogButtonBox::ActionRole);

    pModel = Model;
    pMdi = mdi;

    ui->treeView->setRootIsDecorated(false);
    ui->treeView->setUniformRowHeights(true);
    ui->treeView->setExpandsOnDoubleClick(false);
    ui->treeView->setHeaderHidden(true);
    ui->treeView->setModel(pModel);
    ui->treeView->expandAll();

    connect(pSelect, SIGNAL(clicked(bool)), SLOT(ActivateWindow()));
    connect(pClose, SIGNAL(clicked(bool)), SLOT(CloseWindow()));
}

WindowsListDlg::~WindowsListDlg()
{
    delete ui;
}

void WindowsListDlg::CloseWindow()
{
    QModelIndex index = ui->treeView->currentIndex();

    if (index.isValid())
    {
        QMdiSubWindow *wnd = pModel->window(index);
        wnd->close();
    }
}

void WindowsListDlg::ActivateWindow()
{
    QModelIndex index = ui->treeView->currentIndex();
    QMdiSubWindow *wnd = pModel->window(index);

    pMdi->setActiveSubWindow(wnd);
}
