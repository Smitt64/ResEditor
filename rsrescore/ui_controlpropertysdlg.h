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
    QComboBox *comboBox;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_7;
    QFormLayout *formLayout_2;
    QLabel *label_2;
    QComboBox *comboBox_2;
    QLabel *label_3;
    QSpinBox *spinBox;
    QLabel *label_5;
    QSpinBox *spinBox_2;
    QVBoxLayout *verticalLayout;
    QCheckBox *checkBox;
    QFormLayout *formLayout;
    QLabel *label_4;
    QSpinBox *spinBox_3;
    QLabel *label_6;
    QSpinBox *spinBox_4;
    QHBoxLayout *horizontalLayout_4;
    QCheckBox *checkBox_2;
    QSpacerItem *horizontalSpacer_3;
    QHBoxLayout *horizontalLayout_2;
    QFormLayout *formLayout_3;
    QLabel *label_7;
    QLineEdit *lineEdit;
    QLabel *label_8;
    QComboBox *comboBox_3;
    QLineEdit *lineEdit_2;
    QLabel *label_9;
    QVBoxLayout *verticalLayout_2;
    QFormLayout *formLayout_5;
    QLabel *label_10;
    QSpinBox *spinBox_5;
    QHBoxLayout *horizontalLayout_3;
    QFormLayout *formLayout_4;
    QLabel *label_11;
    QSpinBox *spinBox_6;
    QSpacerItem *horizontalSpacer_2;
    QFormLayout *formLayout_6;
    QLabel *label_12;
    QLineEdit *lineEdit_3;
    QLabel *label_13;
    QLineEdit *lineEdit_4;
    QHBoxLayout *horizontalLayout_5;
    QVBoxLayout *verticalLayout_3;
    QCheckBox *checkBox_3;
    QCheckBox *checkBox_4;
    QCheckBox *checkBox_5;
    QSpacerItem *horizontalSpacer_4;
    ControlTabOrderEdit *tabOrderWidget;
    QHBoxLayout *horizontalLayout_6;
    QSpacerItem *horizontalSpacer_5;
    QVBoxLayout *verticalLayout_4;
    QCheckBox *checkBox_6;
    QCheckBox *checkBox_7;
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

        comboBox = new QComboBox(ControlPropertysDlg);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));
        comboBox->setMinimumSize(QSize(150, 0));

        horizontalLayout->addWidget(comboBox);

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

        comboBox_2 = new QComboBox(ControlPropertysDlg);
        comboBox_2->setObjectName(QString::fromUtf8("comboBox_2"));

        formLayout_2->setWidget(0, QFormLayout::FieldRole, comboBox_2);

        label_3 = new QLabel(ControlPropertysDlg);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_3);

        spinBox = new QSpinBox(ControlPropertysDlg);
        spinBox->setObjectName(QString::fromUtf8("spinBox"));

        formLayout_2->setWidget(1, QFormLayout::FieldRole, spinBox);

        label_5 = new QLabel(ControlPropertysDlg);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        formLayout_2->setWidget(2, QFormLayout::LabelRole, label_5);

        spinBox_2 = new QSpinBox(ControlPropertysDlg);
        spinBox_2->setObjectName(QString::fromUtf8("spinBox_2"));

        formLayout_2->setWidget(2, QFormLayout::FieldRole, spinBox_2);


        horizontalLayout_7->addLayout(formLayout_2);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        checkBox = new QCheckBox(ControlPropertysDlg);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));
        checkBox->setLayoutDirection(Qt::RightToLeft);

        verticalLayout->addWidget(checkBox);

        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label_4 = new QLabel(ControlPropertysDlg);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_4);

        spinBox_3 = new QSpinBox(ControlPropertysDlg);
        spinBox_3->setObjectName(QString::fromUtf8("spinBox_3"));

        formLayout->setWidget(0, QFormLayout::FieldRole, spinBox_3);

        label_6 = new QLabel(ControlPropertysDlg);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_6);

        spinBox_4 = new QSpinBox(ControlPropertysDlg);
        spinBox_4->setObjectName(QString::fromUtf8("spinBox_4"));

        formLayout->setWidget(1, QFormLayout::FieldRole, spinBox_4);


        verticalLayout->addLayout(formLayout);


        horizontalLayout_7->addLayout(verticalLayout);


        verticalLayout_5->addLayout(horizontalLayout_7);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        checkBox_2 = new QCheckBox(ControlPropertysDlg);
        checkBox_2->setObjectName(QString::fromUtf8("checkBox_2"));
        checkBox_2->setLayoutDirection(Qt::RightToLeft);

        horizontalLayout_4->addWidget(checkBox_2);

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

        lineEdit = new QLineEdit(ControlPropertysDlg);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        formLayout_3->setWidget(0, QFormLayout::FieldRole, lineEdit);

        label_8 = new QLabel(ControlPropertysDlg);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        formLayout_3->setWidget(1, QFormLayout::LabelRole, label_8);

        comboBox_3 = new QComboBox(ControlPropertysDlg);
        comboBox_3->setObjectName(QString::fromUtf8("comboBox_3"));

        formLayout_3->setWidget(1, QFormLayout::FieldRole, comboBox_3);

        lineEdit_2 = new QLineEdit(ControlPropertysDlg);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));

        formLayout_3->setWidget(2, QFormLayout::FieldRole, lineEdit_2);

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

        spinBox_5 = new QSpinBox(ControlPropertysDlg);
        spinBox_5->setObjectName(QString::fromUtf8("spinBox_5"));

        formLayout_5->setWidget(0, QFormLayout::FieldRole, spinBox_5);


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

        spinBox_6 = new QSpinBox(ControlPropertysDlg);
        spinBox_6->setObjectName(QString::fromUtf8("spinBox_6"));
        spinBox_6->setMinimumSize(QSize(65, 0));

        formLayout_4->setWidget(0, QFormLayout::FieldRole, spinBox_6);


        horizontalLayout_3->addLayout(formLayout_4);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);


        verticalLayout_5->addLayout(horizontalLayout_3);

        formLayout_6 = new QFormLayout();
        formLayout_6->setObjectName(QString::fromUtf8("formLayout_6"));
        label_12 = new QLabel(ControlPropertysDlg);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        formLayout_6->setWidget(0, QFormLayout::LabelRole, label_12);

        lineEdit_3 = new QLineEdit(ControlPropertysDlg);
        lineEdit_3->setObjectName(QString::fromUtf8("lineEdit_3"));

        formLayout_6->setWidget(0, QFormLayout::FieldRole, lineEdit_3);

        label_13 = new QLabel(ControlPropertysDlg);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        formLayout_6->setWidget(1, QFormLayout::LabelRole, label_13);

        lineEdit_4 = new QLineEdit(ControlPropertysDlg);
        lineEdit_4->setObjectName(QString::fromUtf8("lineEdit_4"));

        formLayout_6->setWidget(1, QFormLayout::FieldRole, lineEdit_4);


        verticalLayout_5->addLayout(formLayout_6);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        checkBox_3 = new QCheckBox(ControlPropertysDlg);
        checkBox_3->setObjectName(QString::fromUtf8("checkBox_3"));

        verticalLayout_3->addWidget(checkBox_3);

        checkBox_4 = new QCheckBox(ControlPropertysDlg);
        checkBox_4->setObjectName(QString::fromUtf8("checkBox_4"));

        verticalLayout_3->addWidget(checkBox_4);

        checkBox_5 = new QCheckBox(ControlPropertysDlg);
        checkBox_5->setObjectName(QString::fromUtf8("checkBox_5"));

        verticalLayout_3->addWidget(checkBox_5);


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
        checkBox_6 = new QCheckBox(ControlPropertysDlg);
        checkBox_6->setObjectName(QString::fromUtf8("checkBox_6"));

        verticalLayout_4->addWidget(checkBox_6);

        checkBox_7 = new QCheckBox(ControlPropertysDlg);
        checkBox_7->setObjectName(QString::fromUtf8("checkBox_7"));

        verticalLayout_4->addWidget(checkBox_7);


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
        checkBox->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\237\321\200\320\270\320\267\320\275\320\260\320\272 FDM", nullptr));
        label_4->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\242\320\276\321\207\320\272\320\260:", nullptr));
        label_6->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\222\321\213\321\201\320\276\321\202\320\260:", nullptr));
        checkBox_2->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\237\321\200\320\270\320\267\320\275\320\260\320\272 \321\202\320\265\320\272\321\201\321\202\320\260", nullptr));
        label_7->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\230\320\274\321\217/\320\242\320\265\320\272\321\201\321\202:", nullptr));
        label_8->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\246\320\262\320\265\321\202:", nullptr));
        label_9->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\250\320\260\320\261\320\273\320\276\320\275:", nullptr));
        label_10->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\223\321\200\321\203\320\277\320\277\320\260:", nullptr));
        label_11->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\241\321\202\321\200\320\260\320\275\320\270\321\206\320\260 \320\277\320\276\320\274\320\276\321\211\320\270:", nullptr));
        label_12->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\235\320\260\320\270\320\274\320\265\320\275\320\276\320\262\320\260\320\275\320\270\320\265 \320\277\320\276\320\273\321\217:", nullptr));
        label_13->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\237\320\276\320\264\321\201\320\272\320\260\320\267\320\272\320\260:", nullptr));
        checkBox_3->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\240\320\260\320\264\320\270\320\276\320\272\320\273\320\260\321\201\321\202\320\265\321\200", nullptr));
        checkBox_4->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\232\320\260\320\272 \321\202\320\265\320\272\321\201\321\202", nullptr));
        checkBox_5->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\223\321\200\321\203\320\277\320\277\320\270\321\200\320\276\320\262\320\272\320\260", nullptr));
        checkBox_6->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\230\321\201\320\272\320\273\321\216\321\207\320\270\321\202\321\214 \320\270\320\267 \320\276\320\261\321\205\320\276\320\264\320\260", nullptr));
        checkBox_7->setText(QCoreApplication::translate("ControlPropertysDlg", "\320\222\321\213\320\261\320\276\321\200 \320\270\320\267 \321\201\320\277\320\270\321\201\320\272\320\260", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ControlPropertysDlg: public Ui_ControlPropertysDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTROLPROPERTYSDLG_H
