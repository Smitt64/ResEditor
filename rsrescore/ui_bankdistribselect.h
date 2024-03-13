/********************************************************************************
** Form generated from reading UI file 'bankdistribselect.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BANKDISTRIBSELECT_H
#define UI_BANKDISTRIBSELECT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_BankDistribSelect
{
public:
    QVBoxLayout *verticalLayout;
    QTreeWidget *treeWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *BankDistribSelect)
    {
        if (BankDistribSelect->objectName().isEmpty())
            BankDistribSelect->setObjectName(QString::fromUtf8("BankDistribSelect"));
        BankDistribSelect->resize(496, 329);
        verticalLayout = new QVBoxLayout(BankDistribSelect);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        treeWidget = new QTreeWidget(BankDistribSelect);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem(treeWidget);
        __qtreewidgetitem->setFlags(Qt::ItemIsEnabled);
        QTreeWidgetItem *__qtreewidgetitem1 = new QTreeWidgetItem(treeWidget);
        __qtreewidgetitem1->setFlags(Qt::ItemIsEnabled);
        treeWidget->setObjectName(QString::fromUtf8("treeWidget"));
        treeWidget->setRootIsDecorated(false);
        treeWidget->header()->setVisible(false);

        verticalLayout->addWidget(treeWidget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        pushButton = new QPushButton(BankDistribSelect);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        horizontalLayout->addWidget(pushButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        buttonBox = new QDialogButtonBox(BankDistribSelect);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        horizontalLayout->addWidget(buttonBox);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(BankDistribSelect);
        QObject::connect(buttonBox, SIGNAL(accepted()), BankDistribSelect, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), BankDistribSelect, SLOT(reject()));

        QMetaObject::connectSlotsByName(BankDistribSelect);
    } // setupUi

    void retranslateUi(QDialog *BankDistribSelect)
    {
        BankDistribSelect->setWindowTitle(QCoreApplication::translate("BankDistribSelect", "Dialog", nullptr));
        QTreeWidgetItem *___qtreewidgetitem = treeWidget->headerItem();
        ___qtreewidgetitem->setText(0, QCoreApplication::translate("BankDistribSelect", "1", nullptr));

        const bool __sortingEnabled = treeWidget->isSortingEnabled();
        treeWidget->setSortingEnabled(false);
        QTreeWidgetItem *___qtreewidgetitem1 = treeWidget->topLevelItem(0);
        ___qtreewidgetitem1->setText(0, QCoreApplication::translate("BankDistribSelect", "\320\237\320\276\320\273\321\214\320\267\320\276\320\262\320\260\321\202\320\265\320\273\321\214\321\201\320\272\320\270\320\265 \320\277\321\203\321\202\320\270", nullptr));
        QTreeWidgetItem *___qtreewidgetitem2 = treeWidget->topLevelItem(1);
        ___qtreewidgetitem2->setText(0, QCoreApplication::translate("BankDistribSelect", "\320\220\320\262\321\202\320\276\320\276\320\277\321\200\320\265\320\264\320\265\320\273\320\265\320\275\320\275\321\213\320\265", nullptr));
        treeWidget->setSortingEnabled(__sortingEnabled);

        pushButton->setText(QCoreApplication::translate("BankDistribSelect", "\320\224\320\276\320\261\320\260\320\262\320\270\321\202\321\214", nullptr));
    } // retranslateUi

};

namespace Ui {
    class BankDistribSelect: public Ui_BankDistribSelect {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BANKDISTRIBSELECT_H
