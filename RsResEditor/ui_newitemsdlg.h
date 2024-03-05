/********************************************************************************
** Form generated from reading UI file 'newitemsdlg.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NEWITEMSDLG_H
#define UI_NEWITEMSDLG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_NewItemsDlg
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QTreeWidget *treeWidget;
    QLabel *descriptionLabel;
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *nameEdit;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_2;
    QLineEdit *pathEdit;
    QPushButton *pathButton;
    QSpacerItem *horizontalSpacer_3;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *NewItemsDlg)
    {
        if (NewItemsDlg->objectName().isEmpty())
            NewItemsDlg->setObjectName(QString::fromUtf8("NewItemsDlg"));
        NewItemsDlg->resize(558, 409);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/img/NewDocumentHS.png"), QSize(), QIcon::Normal, QIcon::Off);
        NewItemsDlg->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(NewItemsDlg);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        treeWidget = new QTreeWidget(NewItemsDlg);
        treeWidget->setObjectName(QString::fromUtf8("treeWidget"));
        treeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        treeWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        treeWidget->setIndentation(10);
        treeWidget->setRootIsDecorated(true);
        treeWidget->setItemsExpandable(true);
        treeWidget->setAnimated(true);
        treeWidget->setHeaderHidden(true);

        horizontalLayout->addWidget(treeWidget);

        descriptionLabel = new QLabel(NewItemsDlg);
        descriptionLabel->setObjectName(QString::fromUtf8("descriptionLabel"));
        descriptionLabel->setMinimumSize(QSize(200, 0));
        descriptionLabel->setMaximumSize(QSize(200, 16777215));
        descriptionLabel->setTextFormat(Qt::RichText);
        descriptionLabel->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);

        horizontalLayout->addWidget(descriptionLabel);


        verticalLayout->addLayout(horizontalLayout);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setVerticalSpacing(2);
        label = new QLabel(NewItemsDlg);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        nameEdit = new QLineEdit(NewItemsDlg);
        nameEdit->setObjectName(QString::fromUtf8("nameEdit"));
        nameEdit->setEnabled(false);

        gridLayout->addWidget(nameEdit, 0, 1, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 0, 2, 2, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 0, 3, 1, 1);

        label_2 = new QLabel(NewItemsDlg);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 2, 1);

        pathEdit = new QLineEdit(NewItemsDlg);
        pathEdit->setObjectName(QString::fromUtf8("pathEdit"));
        pathEdit->setEnabled(false);
        pathEdit->setReadOnly(true);

        gridLayout->addWidget(pathEdit, 2, 1, 1, 1);

        pathButton = new QPushButton(NewItemsDlg);
        pathButton->setObjectName(QString::fromUtf8("pathButton"));
        pathButton->setEnabled(false);

        gridLayout->addWidget(pathButton, 2, 2, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_3, 2, 3, 1, 1);

        gridLayout->setColumnStretch(1, 2);
        gridLayout->setColumnStretch(3, 1);

        verticalLayout->addLayout(gridLayout);

        buttonBox = new QDialogButtonBox(NewItemsDlg);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(NewItemsDlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), NewItemsDlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), NewItemsDlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(NewItemsDlg);
    } // setupUi

    void retranslateUi(QDialog *NewItemsDlg)
    {
        NewItemsDlg->setWindowTitle(QCoreApplication::translate("NewItemsDlg", "\320\235\320\276\320\262\321\213\320\271 \321\215\320\273\320\265\320\274\320\265\320\275\321\202", nullptr));
        QTreeWidgetItem *___qtreewidgetitem = treeWidget->headerItem();
        ___qtreewidgetitem->setText(0, QCoreApplication::translate("NewItemsDlg", "\320\235\320\276\320\262\321\213\320\271 \321\201\321\202\320\276\320\273\320\261\320\265\321\206", nullptr));
        descriptionLabel->setText(QString());
        label->setText(QCoreApplication::translate("NewItemsDlg", "\320\230\320\274\321\217:", nullptr));
        label_2->setText(QCoreApplication::translate("NewItemsDlg", "\320\240\320\260\321\201\320\277\320\276\320\273\320\276\320\266\320\265\320\275\320\270\320\265:", nullptr));
        pathButton->setText(QCoreApplication::translate("NewItemsDlg", "\320\236\320\261\320\267\320\276\321\200...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class NewItemsDlg: public Ui_NewItemsDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NEWITEMSDLG_H
