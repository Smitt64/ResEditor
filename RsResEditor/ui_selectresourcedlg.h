/********************************************************************************
** Form generated from reading UI file 'selectresourcedlg.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SELECTRESOURCEDLG_H
#define UI_SELECTRESOURCEDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SelectResourceDlg
{
public:
    QVBoxLayout *verticalLayout;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SelectResourceDlg)
    {
        if (SelectResourceDlg->objectName().isEmpty())
            SelectResourceDlg->setObjectName(QString::fromUtf8("SelectResourceDlg"));
        SelectResourceDlg->resize(581, 398);
        verticalLayout = new QVBoxLayout(SelectResourceDlg);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        buttonBox = new QDialogButtonBox(SelectResourceDlg);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(SelectResourceDlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), SelectResourceDlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SelectResourceDlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(SelectResourceDlg);
    } // setupUi

    void retranslateUi(QDialog *SelectResourceDlg)
    {
        SelectResourceDlg->setWindowTitle(QCoreApplication::translate("SelectResourceDlg", "\320\236\321\202\320\272\321\200\321\213\321\202\321\214 \321\200\320\265\321\201\321\203\321\200\321\201", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SelectResourceDlg: public Ui_SelectResourceDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SELECTRESOURCEDLG_H
