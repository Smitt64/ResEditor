/********************************************************************************
** Form generated from reading UI file 'windowslistdlg.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WINDOWSLISTDLG_H
#define UI_WINDOWSLISTDLG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTreeView>

QT_BEGIN_NAMESPACE

class Ui_WindowsListDlg
{
public:
    QHBoxLayout *horizontalLayout;
    QTreeView *treeView;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *WindowsListDlg)
    {
        if (WindowsListDlg->objectName().isEmpty())
            WindowsListDlg->setObjectName(QString::fromUtf8("WindowsListDlg"));
        WindowsListDlg->resize(343, 300);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/img/WindowsHS.png"), QSize(), QIcon::Normal, QIcon::Off);
        WindowsListDlg->setWindowIcon(icon);
        horizontalLayout = new QHBoxLayout(WindowsListDlg);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        treeView = new QTreeView(WindowsListDlg);
        treeView->setObjectName(QString::fromUtf8("treeView"));
        treeView->setIconSize(QSize(20, 20));

        horizontalLayout->addWidget(treeView);

        buttonBox = new QDialogButtonBox(WindowsListDlg);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Vertical);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel);

        horizontalLayout->addWidget(buttonBox);


        retranslateUi(WindowsListDlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), WindowsListDlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), WindowsListDlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(WindowsListDlg);
    } // setupUi

    void retranslateUi(QDialog *WindowsListDlg)
    {
        WindowsListDlg->setWindowTitle(QCoreApplication::translate("WindowsListDlg", "\320\241\320\277\320\270\321\201\320\276\320\272 \320\276\320\272\320\276\320\275", nullptr));
    } // retranslateUi

};

namespace Ui {
    class WindowsListDlg: public Ui_WindowsListDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WINDOWSLISTDLG_H
