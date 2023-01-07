/********************************************************************************
** Form generated from reading UI file 'labeltexteditdlg.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LABELTEXTEDITDLG_H
#define UI_LABELTEXTEDITDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LabelTextEditDlg
{
public:
    QVBoxLayout *verticalLayout;
    QLineEdit *lineEdit;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *LabelTextEditDlg)
    {
        if (LabelTextEditDlg->objectName().isEmpty())
            LabelTextEditDlg->setObjectName(QString::fromUtf8("LabelTextEditDlg"));
        LabelTextEditDlg->resize(528, 338);
        verticalLayout = new QVBoxLayout(LabelTextEditDlg);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        lineEdit = new QLineEdit(LabelTextEditDlg);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        verticalLayout->addWidget(lineEdit);

        scrollArea = new QScrollArea(LabelTextEditDlg);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 508, 260));
        scrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout->addWidget(scrollArea);

        buttonBox = new QDialogButtonBox(LabelTextEditDlg);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(LabelTextEditDlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), LabelTextEditDlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), LabelTextEditDlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(LabelTextEditDlg);
    } // setupUi

    void retranslateUi(QDialog *LabelTextEditDlg)
    {
        LabelTextEditDlg->setWindowTitle(QCoreApplication::translate("LabelTextEditDlg", "\320\240\320\265\320\264\320\260\320\272\321\202\320\270\321\200\320\276\320\262\320\260\320\275\320\270\320\265 \321\202\320\265\320\272\321\201\321\202\320\260", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LabelTextEditDlg: public Ui_LabelTextEditDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LABELTEXTEDITDLG_H
