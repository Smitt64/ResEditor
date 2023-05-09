/********************************************************************************
** Form generated from reading UI file 'controlpropertysdlg.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONTROLPROPERTYSDLG_H
#define UI_CONTROLPROPERTYSDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <widgets\controltaborderedit.h>

QT_BEGIN_NAMESPACE

class Ui_ControlPropertysDlg
{
public:
    QVBoxLayout *verticalLayout_5;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QComboBox *fieldTypeBox;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_7;
    QFormLayout *formLayout_2;
    QLabel *label_2;
    QComboBox *dataTypeBox;
    QLabel *label_3;
    QSpinBox *dataLength;
    QLabel *label_5;
    QSpinBox *length;
    QVBoxLayout *verticalLayout;
    QCheckBox *fdmBox;
    QFormLayout *formLayout;
    QLabel *label_4;
    QSpinBox *point;
    QLabel *label_6;
    QSpinBox *lines;
    QHBoxLayout *horizontalLayout_4;
    QCheckBox *isText;
    QSpacerItem *horizontalSpacer_3;
    QHBoxLayout *horizontalLayout_2;
    QFormLayout *formLayout_3;
    QLabel *label_7;
    QLineEdit *nameTextEdit;
    QLabel *label_8;
    QComboBox *controlStyle;
    QLineEdit *valueTemplate;
    QLabel *label_9;
    QVBoxLayout *verticalLayout_2;
    QFormLayout *formLayout_5;
    QLabel *label_10;
    QSpinBox *controlGroup;
    QHBoxLayout *horizontalLayout_3;
    QFormLayout *formLayout_4;
    QLabel *label_11;
    QSpinBox *helpPage;
    QSpacerItem *horizontalSpacer_2;
    QFormLayout *formLayout_6;
    QLabel *label_12;
    QLineEdit *controlName;
    QLabel *label_13;
    QLineEdit *toolTip;
    QHBoxLayout *horizontalLayout_5;
    QVBoxLayout *verticalLayout_3;
    QCheckBox *flagRadio;
    QCheckBox *flagText;
    QCheckBox *flagGroup;
    QSpacerItem *horizontalSpacer_4;
    ControlTabOrderEdit *tabOrderWidget;
    QHBoxLayout *horizontalLayout_6;
    QSpacerItem *horizontalSpacer_5;
    QVBoxLayout *verticalLayout_4;
    QCheckBox *flagExcludeTab;
    QCheckBox *flagSelectList;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *ControlPropertysDlg)
    {
        if (ControlPropertysDlg->objectName().isEmpty())
            ControlPropertysDlg->setObjectName(QString::fromUtf8("ControlPropertysDlg"));
        ControlPropertysDlg->resize(400, 504);
        ControlPropertysDlg->setMinimumSize(QSize(390, 0));
        ControlPropertysDlg->setAutoFillBackground(true);
        verticalLayout_5 = new QVBoxLayout(ControlPropertysDlg);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(ControlPropertysDlg);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        fieldTypeBox = new QComboBox(ControlPropertysDlg);
        fieldTypeBox->setObjectName(QString::fromUtf8("fieldTypeBox"));
        fieldTypeBox->setMinimumSize(QSize(150, 0));

        horizontalLayout->addWidget(fieldTypeBox);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        horizontalLayout->setStretch(2, 1);

        verticalLayout_5->addLayout(horizontalLayout);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        formLayout_2 = new QFormLayout();
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        label_2 = new QLabel(ControlPropertysDlg);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label_2);

        dataTypeBox = new QComboBox(ControlPropertysDlg);
        dataTypeBox->setObjectName(QString::fromUtf8("dataTypeBox"));

        formLayout_2->setWidget(0, QFormLayout::FieldRole, dataTypeBox);

        label_3 = new QLabel(ControlPropertysDlg);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_3);

        dataLength = new QSpinBox(ControlPropertysDlg);
        dataLength->setObjectName(QString::fromUtf8("dataLength"));

        formLayout_2->setWidget(1, QFormLayout::FieldRole, dataLength);

        label_5 = new QLabel(ControlPropertysDlg);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        formLayout_2->setWidget(2, QFormLayout::LabelRole, label_5);

        length = new QSpinBox(ControlPropertysDlg);
        length->setObjectName(QString::fromUtf8("length"));

        formLayout_2->setWidget(2, QFormLayout::FieldRole, length);


        horizontalLayout_7->addLayout(formLayout_2);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        fdmBox = new QCheckBox(ControlPropertysDlg);
        fdmBox->setObjectName(QString::fromUtf8("fdmBox"));
        fdmBox->setLayoutDirection(Qt::RightToLeft);

        verticalLayout->addWidget(fdmBox);

        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label_4 = new QLabel(ControlPropertysDlg);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_4);

        point = new QSpinBox(ControlPropertysDlg);
        point->setObjectName(QString::fromUtf8("point"));

        formLayout->setWidget(0, QFormLayout::FieldRole, point);

        label_6 = new QLabel(ControlPropertysDlg);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_6);

        lines = new QSpinBox(ControlPropertysDlg);
        lines->setObjectName(QString::fromUtf8("lines"));

        formLayout->setWidget(1, QFormLayout::FieldRole, lines);


        verticalLayout->addLayout(formLayout);


        horizontalLayout_7->addLayout(verticalLayout);


        verticalLayout_5->addLayout(horizontalLayout_7);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        isText = new QCheckBox(ControlPropertysDlg);
        isText->setObjectName(QString::fromUtf8("isText"));

        horizontalLayout_4->addWidget(isText);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_3);


        verticalLayout_5->addLayout(horizontalLayout_4);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        formLayout_3 = new QFormLayout();
        formLayout_3->setObjectName(QString::fromUtf8("formLayout_3"));
        label_7 = new QLabel(ControlPropertysDlg);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        formLayout_3->setWidget(0, QFormLayout::LabelRole, label_7);

        nameTextEdit = new QLineEdit(ControlPropertysDlg);
        nameTextEdit->setObjectName(QString::fromUtf8("nameTextEdit"));

        formLayout_3->setWidget(0, QFormLayout::FieldRole, nameTextEdit);

        label_8 = new QLabel(ControlPropertysDlg);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        formLayout_3->setWidget(1, QFormLayout::LabelRole, label_8);

        controlStyle = new QComboBox(ControlPropertysDlg);
        controlStyle->setObjectName(QString::fromUtf8("controlStyle"));

        formLayout_3->setWidget(1, QFormLayout::FieldRole, controlStyle);

        valueTemplate = new QLineEdit(ControlPropertysDlg);
        valueTemplate->setObjectName(QString::fromUtf8("valueTemplate"));

        formLayout_3->setWidget(2, QFormLayout::FieldRole, valueTemplate);

        label_9 = new QLabel(ControlPropertysDlg);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        formLayout_3->setWidget(2, QFormLayout::LabelRole, label_9);


        horizontalLayout_2->addLayout(formLayout_3);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        formLayout_5 = new QFormLayout();
        formLayout_5->setObjectName(QString::fromUtf8("formLayout_5"));
        label_10 = new QLabel(ControlPropertysDlg);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        formLayout_5->setWidget(0, QFormLayout::LabelRole, label_10);

        controlGroup = new QSpinBox(ControlPropertysDlg);
        controlGroup->setObjectName(QString::fromUtf8("controlGroup"));

        formLayout_5->setWidget(0, QFormLayout::FieldRole, controlGroup);


        verticalLayout_2->addLayout(formLayout_5);


        horizontalLayout_2->addLayout(verticalLayout_2);


        verticalLayout_5->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        formLayout_4 = new QFormLayout();
        formLayout_4->setObjectName(QString::fromUtf8("formLayout_4"));
        label_11 = new QLabel(ControlPropertysDlg);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        formLayout_4->setWidget(0, QFormLayout::LabelRole, label_11);

        helpPage = new QSpinBox(ControlPropertysDlg);
        helpPage->setObjectName(QString::fromUtf8("helpPage"));
        helpPage->setMinimumSize(QSize(65, 0));

        formLayout_4->setWidget(0, QFormLayout::FieldRole, helpPage);


        horizontalLayout_3->addLayout(formLayout_4);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);


        verticalLayout_5->addLayout(horizontalLayout_3);

        formLayout_6 = new QFormLayout();
        formLayout_6->setObjectName(QString::fromUtf8("formLayout_6"));
        label_12 = new QLabel(ControlPropertysDlg);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        formLayout_6->setWidget(0, QFormLayout::LabelRole, label_12);

        controlName = new QLineEdit(ControlPropertysDlg);
        controlName->setObjectName(QString::fromUtf8("controlName"));

        formLayout_6->setWidget(0, QFormLayout::FieldRole, controlName);

        label_13 = new QLabel(ControlPropertysDlg);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        formLayout_6->setWidget(1, QFormLayout::LabelRole, label_13);

        toolTip = new QLineEdit(ControlPropertysDlg);
        toolTip->setObjectName(QString::fromUtf8("toolTip"));

        formLayout_6->setWidget(1, QFormLayout::FieldRole, toolTip);


        verticalLayout_5->addLayout(formLayout_6);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        flagRadio = new QCheckBox(ControlPropertysDlg);
        flagRadio->setObjectName(QString::fromUtf8("flagRadio"));

        verticalLayout_3->addWidget(flagRadio);

        flagText = new QCheckBox(ControlPropertysDlg);
        flagText->setObjectName(QString::fromUtf8("flagText"));

        verticalLayout_3->addWidget(flagText);

        flagGroup = new QCheckBox(ControlPropertysDlg);
        flagGroup->setObjectName(QString::fromUtf8("flagGroup"));

        verticalLayout_3->addWidget(flagGroup);


        horizontalLayout_5->addLayout(verticalLayout_3);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_4);

        tabOrderWidget = new ControlTabOrderEdit(ControlPropertysDlg);
        tabOrderWidget->setObjectName(QString::fromUtf8("tabOrderWidget"));

        horizontalLayout_5->addWidget(tabOrderWidget);


        verticalLayout_5->addLayout(horizontalLayout_5);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_5);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        flagExcludeTab = new QCheckBox(ControlPropertysDlg);
        flagExcludeTab->setObjectName(QString::fromUtf8("flagExcludeTab"));

        verticalLayout_4->addWidget(flagExcludeTab);

        flagSelectList = new QCheckBox(ControlPropertysDlg);
        flagSelectList->setObjectName(QString::fromUtf8("flagSelectList"));

        verticalLayout_4->addWidget(flagSelectList);


        horizontalLayout_6->addLayout(verticalLayout_4);


        verticalLayout_5->addLayout(horizontalLayout_6);

        buttonBox = new QDialogButtonBox(ControlPropertysDlg);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout_5->addWidget(buttonBox);


        retranslateUi(ControlPropertysDlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), ControlPropertysDlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), ControlPropertysDlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(ControlPropertysDlg);
    } // setupUi

    void retranslateUi(QDialog *ControlPropertysDlg)
    {
        ControlPropertysDlg->setWindowTitle(QCoreApplication::translate("ControlPropertysDlg", "\320\237\320\260\321\200\320\260\320\274\320\265\321\202\321\200\321\213 \320\277\320\276\320\273\321\217", nullptr));
        label->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\242\320\270\320\277 \320\277\320\276\320\273\321\217:", nullptr));
        label_2->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\242\320\270\320\277 \320\267\320\275\320\260\321\207\320\265\320\275\320\270\321\217:", nullptr));
        label_3->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\227\320\275\320\260\320\272\320\276\320\262:", nullptr));
        label_5->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\224\320\273\320\270\320\275\320\260:", nullptr));
        fdmBox->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\237\321\200\320\270\320\267\320\275\320\260\320\272 FDM", nullptr));
        label_4->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\242\320\276\321\207\320\272\320\260:", nullptr));
        label_6->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\222\321\213\321\201\320\276\321\202\320\260:", nullptr));
        isText->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\237\321\200\320\270\320\267\320\275\320\260\320\272 \321\202\320\265\320\272\321\201\321\202\320\260", nullptr));
        label_7->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\230\320\274\321\217/\320\242\320\265\320\272\321\201\321\202:", nullptr));
        label_8->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\246\320\262\320\265\321\202:", nullptr));
        label_9->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\250\320\260\320\261\320\273\320\276\320\275:", nullptr));
        label_10->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\223\321\200\321\203\320\277\320\277\320\260:", nullptr));
        label_11->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\241\321\202\321\200\320\260\320\275\320\270\321\206\320\260 \320\277\320\276\320\274\320\276\321\211\320\270:", nullptr));
        label_12->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\235\320\260\320\270\320\274\320\265\320\275\320\276\320\262\320\260\320\275\320\270\320\265 \320\277\320\276\320\273\321\217:", nullptr));
        label_13->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\237\320\276\320\264\321\201\320\272\320\260\320\267\320\272\320\260:", nullptr));
        flagRadio->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\240\320\260\320\264\320\270\320\276\320\272\320\273\320\260\321\201\321\202\320\265\321\200", nullptr));
        flagText->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\232\320\260\320\272 \321\202\320\265\320\272\321\201\321\202", nullptr));
        flagGroup->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\223\321\200\321\203\320\277\320\277\320\270\321\200\320\276\320\262\320\272\320\260", nullptr));
        flagExcludeTab->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\230\321\201\320\272\320\273\321\216\321\207\320\270\321\202\321\214 \320\270\320\267 \320\276\320\261\321\205\320\276\320\264\320\260", nullptr));
        flagSelectList->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\222\321\213\320\261\320\276\321\200 \320\270\320\267 \321\201\320\277\320\270\321\201\320\272\320\260", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ControlPropertysDlg: public Ui_ControlPropertysDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTROLPROPERTYSDLG_H
