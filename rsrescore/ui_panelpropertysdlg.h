/********************************************************************************
** Form generated from reading UI file 'panelpropertysdlg.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PANELPROPERTYSDLG_H
#define UI_PANELPROPERTYSDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_PanelPropertysDlg
{
public:
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout;
    QFormLayout *formLayout_2;
    QLabel *label;
    QComboBox *typeBox;
    QLabel *label_2;
    QComboBox *borderStyle;
    QFormLayout *formLayout;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_3;
    QComboBox *panelStyle;
    QFrame *frame;
    QFormLayout *formLayout_3;
    QLabel *label_4;
    QLineEdit *titleEdit;
    QLabel *label_5;
    QLineEdit *statusEdit;
    QLabel *label_6;
    QLineEdit *statusEdit2;
    QGroupBox *groupBoxScrol;
    QHBoxLayout *horizontalLayout_3;
    QFormLayout *formLayout_4;
    QLabel *label_7;
    QSpinBox *spinBox;
    QLabel *label_8;
    QHBoxLayout *horizontalLayout_2;
    QSpinBox *spinBox_2;
    QSpinBox *spinBox_3;
    QLabel *label_9;
    QSpinBox *spinBox_4;
    QLabel *label_10;
    QSpinBox *spinBox_5;
    QSpacerItem *horizontalSpacer_3;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout;
    QCheckBox *checkBox;
    QCheckBox *checkBox_2;
    QCheckBox *checkBox_3;
    QHBoxLayout *horizontalLayout_7;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_2;
    QCheckBox *excludeAutoTab;
    QCheckBox *excludeAutoNum;
    QCheckBox *excludeShadow;
    QSpacerItem *verticalSpacer_2;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_5;
    QSpacerItem *horizontalSpacer_4;
    QCheckBox *alignTextRight;
    QHBoxLayout *horizontalLayout_6;
    QSpacerItem *horizontalSpacer_5;
    QCheckBox *alignPanCenter;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_11;
    QSpinBox *helpPage;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_12;
    QLineEdit *commentEdit;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *PanelPropertysDlg)
    {
        if (PanelPropertysDlg->objectName().isEmpty())
            PanelPropertysDlg->setObjectName(QString::fromUtf8("PanelPropertysDlg"));
        PanelPropertysDlg->resize(600, 508);
        PanelPropertysDlg->setMinimumSize(QSize(600, 0));
        PanelPropertysDlg->setMaximumSize(QSize(600, 16777215));
        verticalLayout_4 = new QVBoxLayout(PanelPropertysDlg);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        formLayout_2 = new QFormLayout();
        formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
        label = new QLabel(PanelPropertysDlg);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label);

        typeBox = new QComboBox(PanelPropertysDlg);
        typeBox->setObjectName(QString::fromUtf8("typeBox"));
        typeBox->setEnabled(false);
        typeBox->setMinimumSize(QSize(200, 0));
        typeBox->setMaximumSize(QSize(200, 16777215));

        formLayout_2->setWidget(0, QFormLayout::FieldRole, typeBox);

        label_2 = new QLabel(PanelPropertysDlg);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_2);

        borderStyle = new QComboBox(PanelPropertysDlg);
        borderStyle->setObjectName(QString::fromUtf8("borderStyle"));
        borderStyle->setMinimumSize(QSize(200, 0));
        borderStyle->setMaximumSize(QSize(200, 16777215));

        formLayout_2->setWidget(1, QFormLayout::FieldRole, borderStyle);


        horizontalLayout->addLayout(formLayout_2);

        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        horizontalSpacer = new QSpacerItem(88, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        formLayout->setItem(0, QFormLayout::LabelRole, horizontalSpacer);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        formLayout->setItem(0, QFormLayout::FieldRole, horizontalSpacer_2);

        label_3 = new QLabel(PanelPropertysDlg);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_3);

        panelStyle = new QComboBox(PanelPropertysDlg);
        panelStyle->setObjectName(QString::fromUtf8("panelStyle"));

        formLayout->setWidget(1, QFormLayout::FieldRole, panelStyle);


        horizontalLayout->addLayout(formLayout);


        verticalLayout_4->addLayout(horizontalLayout);

        frame = new QFrame(PanelPropertysDlg);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setFrameShape(QFrame::HLine);
        frame->setFrameShadow(QFrame::Plain);

        verticalLayout_4->addWidget(frame);

        formLayout_3 = new QFormLayout();
        formLayout_3->setObjectName(QString::fromUtf8("formLayout_3"));
        label_4 = new QLabel(PanelPropertysDlg);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout_3->setWidget(0, QFormLayout::LabelRole, label_4);

        titleEdit = new QLineEdit(PanelPropertysDlg);
        titleEdit->setObjectName(QString::fromUtf8("titleEdit"));
        titleEdit->setMaxLength(255);

        formLayout_3->setWidget(0, QFormLayout::FieldRole, titleEdit);

        label_5 = new QLabel(PanelPropertysDlg);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        formLayout_3->setWidget(1, QFormLayout::LabelRole, label_5);

        statusEdit = new QLineEdit(PanelPropertysDlg);
        statusEdit->setObjectName(QString::fromUtf8("statusEdit"));
        statusEdit->setMaxLength(255);

        formLayout_3->setWidget(1, QFormLayout::FieldRole, statusEdit);

        label_6 = new QLabel(PanelPropertysDlg);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        formLayout_3->setWidget(2, QFormLayout::LabelRole, label_6);

        statusEdit2 = new QLineEdit(PanelPropertysDlg);
        statusEdit2->setObjectName(QString::fromUtf8("statusEdit2"));
        statusEdit2->setMaxLength(255);

        formLayout_3->setWidget(2, QFormLayout::FieldRole, statusEdit2);


        verticalLayout_4->addLayout(formLayout_3);

        groupBoxScrol = new QGroupBox(PanelPropertysDlg);
        groupBoxScrol->setObjectName(QString::fromUtf8("groupBoxScrol"));
        horizontalLayout_3 = new QHBoxLayout(groupBoxScrol);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        formLayout_4 = new QFormLayout();
        formLayout_4->setObjectName(QString::fromUtf8("formLayout_4"));
        label_7 = new QLabel(groupBoxScrol);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        formLayout_4->setWidget(0, QFormLayout::LabelRole, label_7);

        spinBox = new QSpinBox(groupBoxScrol);
        spinBox->setObjectName(QString::fromUtf8("spinBox"));

        formLayout_4->setWidget(0, QFormLayout::FieldRole, spinBox);

        label_8 = new QLabel(groupBoxScrol);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        formLayout_4->setWidget(1, QFormLayout::LabelRole, label_8);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        spinBox_2 = new QSpinBox(groupBoxScrol);
        spinBox_2->setObjectName(QString::fromUtf8("spinBox_2"));

        horizontalLayout_2->addWidget(spinBox_2);

        spinBox_3 = new QSpinBox(groupBoxScrol);
        spinBox_3->setObjectName(QString::fromUtf8("spinBox_3"));

        horizontalLayout_2->addWidget(spinBox_3);


        formLayout_4->setLayout(1, QFormLayout::FieldRole, horizontalLayout_2);

        label_9 = new QLabel(groupBoxScrol);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        formLayout_4->setWidget(2, QFormLayout::LabelRole, label_9);

        spinBox_4 = new QSpinBox(groupBoxScrol);
        spinBox_4->setObjectName(QString::fromUtf8("spinBox_4"));

        formLayout_4->setWidget(2, QFormLayout::FieldRole, spinBox_4);

        label_10 = new QLabel(groupBoxScrol);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        formLayout_4->setWidget(3, QFormLayout::LabelRole, label_10);

        spinBox_5 = new QSpinBox(groupBoxScrol);
        spinBox_5->setObjectName(QString::fromUtf8("spinBox_5"));

        formLayout_4->setWidget(3, QFormLayout::FieldRole, spinBox_5);


        horizontalLayout_3->addLayout(formLayout_4);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);

        groupBox_2 = new QGroupBox(groupBoxScrol);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        verticalLayout = new QVBoxLayout(groupBox_2);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        checkBox = new QCheckBox(groupBox_2);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));

        verticalLayout->addWidget(checkBox);

        checkBox_2 = new QCheckBox(groupBox_2);
        checkBox_2->setObjectName(QString::fromUtf8("checkBox_2"));

        verticalLayout->addWidget(checkBox_2);

        checkBox_3 = new QCheckBox(groupBox_2);
        checkBox_3->setObjectName(QString::fromUtf8("checkBox_3"));

        verticalLayout->addWidget(checkBox_3);


        horizontalLayout_3->addWidget(groupBox_2);

        horizontalLayout_3->setStretch(0, 1);
        horizontalLayout_3->setStretch(2, 1);

        verticalLayout_4->addWidget(groupBoxScrol);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        groupBox_3 = new QGroupBox(PanelPropertysDlg);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        verticalLayout_2 = new QVBoxLayout(groupBox_3);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        excludeAutoTab = new QCheckBox(groupBox_3);
        excludeAutoTab->setObjectName(QString::fromUtf8("excludeAutoTab"));

        verticalLayout_2->addWidget(excludeAutoTab);

        excludeAutoNum = new QCheckBox(groupBox_3);
        excludeAutoNum->setObjectName(QString::fromUtf8("excludeAutoNum"));

        verticalLayout_2->addWidget(excludeAutoNum);

        excludeShadow = new QCheckBox(groupBox_3);
        excludeShadow->setObjectName(QString::fromUtf8("excludeShadow"));

        verticalLayout_2->addWidget(excludeShadow);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_2);


        horizontalLayout_7->addWidget(groupBox_3);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_4);

        alignTextRight = new QCheckBox(PanelPropertysDlg);
        alignTextRight->setObjectName(QString::fromUtf8("alignTextRight"));
        alignTextRight->setMinimumSize(QSize(182, 0));

        horizontalLayout_5->addWidget(alignTextRight);


        verticalLayout_3->addLayout(horizontalLayout_5);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_5);

        alignPanCenter = new QCheckBox(PanelPropertysDlg);
        alignPanCenter->setObjectName(QString::fromUtf8("alignPanCenter"));
        alignPanCenter->setMinimumSize(QSize(182, 0));

        horizontalLayout_6->addWidget(alignPanCenter);


        verticalLayout_3->addLayout(horizontalLayout_6);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_11 = new QLabel(PanelPropertysDlg);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_4->addWidget(label_11);

        helpPage = new QSpinBox(PanelPropertysDlg);
        helpPage->setObjectName(QString::fromUtf8("helpPage"));
        helpPage->setMaximumSize(QSize(100, 16777215));

        horizontalLayout_4->addWidget(helpPage);


        verticalLayout_3->addLayout(horizontalLayout_4);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer);


        horizontalLayout_7->addLayout(verticalLayout_3);


        verticalLayout_4->addLayout(horizontalLayout_7);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        label_12 = new QLabel(PanelPropertysDlg);
        label_12->setObjectName(QString::fromUtf8("label_12"));

        horizontalLayout_8->addWidget(label_12);

        commentEdit = new QLineEdit(PanelPropertysDlg);
        commentEdit->setObjectName(QString::fromUtf8("commentEdit"));
        commentEdit->setMaxLength(255);

        horizontalLayout_8->addWidget(commentEdit);


        verticalLayout_4->addLayout(horizontalLayout_8);

        buttonBox = new QDialogButtonBox(PanelPropertysDlg);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout_4->addWidget(buttonBox);


        retranslateUi(PanelPropertysDlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), PanelPropertysDlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), PanelPropertysDlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(PanelPropertysDlg);
    } // setupUi

    void retranslateUi(QDialog *PanelPropertysDlg)
    {
        PanelPropertysDlg->setWindowTitle(QCoreApplication::translate("PanelPropertysDlg", "\320\237\320\260\321\200\320\260\320\274\320\265\321\202\321\200\321\213 \320\277\320\260\320\275\320\265\320\273\320\270", nullptr));
        label->setText(QCoreApplication::translate("PanelPropertysDlg", "\320\242\320\270\320\277:", nullptr));
        label_2->setText(QCoreApplication::translate("PanelPropertysDlg", "\320\240\320\260\320\274\320\272\320\260:", nullptr));
        label_3->setText(QCoreApplication::translate("PanelPropertysDlg", "\320\241\321\202\320\270\320\273\321\214 \320\277\320\260\320\275\320\265\320\273\320\270:", nullptr));
        label_4->setText(QCoreApplication::translate("PanelPropertysDlg", "\320\227\320\260\320\263\320\276\320\273\320\276\320\262\320\276\320\272:", nullptr));
        label_5->setText(QCoreApplication::translate("PanelPropertysDlg", "\320\241\321\202\320\260\321\202\321\203\321\201:", nullptr));
        label_6->setText(QCoreApplication::translate("PanelPropertysDlg", "\320\241\321\202\320\260\321\202\321\203\321\201 2:", nullptr));
        groupBoxScrol->setTitle(QCoreApplication::translate("PanelPropertysDlg", "\320\241\320\272\321\200\320\276\320\273\320\270\320\275\320\263", nullptr));
        label_7->setText(QCoreApplication::translate("PanelPropertysDlg", "\320\247\320\270\321\201\320\273\320\276 \321\201\321\202\321\200\320\276\320\272:", nullptr));
        label_8->setText(QCoreApplication::translate("PanelPropertysDlg", "\320\221\320\260\320\267\320\276\320\262\321\213\320\265 \320\272\320\276\320\276\321\200\320\264\320\270\320\275\320\260\321\202\321\213:", nullptr));
        label_9->setText(QCoreApplication::translate("PanelPropertysDlg", "\320\224\320\273\320\270\320\275\320\260 \321\201\321\202\321\200\320\276\320\272\320\270:", nullptr));
        label_10->setText(QCoreApplication::translate("PanelPropertysDlg", "\320\222\321\213\321\201\320\276\321\202\320\260 \321\201\321\202\321\200\320\276\320\272\320\270:", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("PanelPropertysDlg", "\320\220\320\262\321\202\320\276\320\274\320\260\321\202\320\270\321\207\320\265\321\201\320\272\320\270", nullptr));
        checkBox->setText(QCoreApplication::translate("PanelPropertysDlg", "\320\240\320\260\320\267\320\274\320\265\321\200 \320\277\320\260\320\275\320\265\320\273\320\270", nullptr));
        checkBox_2->setText(QCoreApplication::translate("PanelPropertysDlg", "\320\240\320\260\320\267\320\274\320\265\321\211\320\260\321\202\321\214 \320\277\320\276\320\273\321\217", nullptr));
        checkBox_3->setText(QCoreApplication::translate("PanelPropertysDlg", "\320\244\320\276\321\200\320\274\320\270\321\200\320\276\320\262\320\260\321\202\321\214 \320\267\320\260\320\263\320\276\320\273\320\276\320\262\320\276\320\272", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("PanelPropertysDlg", "\320\230\321\201\320\272\320\273\321\216\321\207\320\270\321\202\321\214", nullptr));
        excludeAutoTab->setText(QCoreApplication::translate("PanelPropertysDlg", "\320\220\320\262\321\202\320\276\320\274\320\260\321\202\320\270\321\207\320\265\321\201\320\272\320\270\320\271 \320\276\320\261\321\205\320\276\320\264", nullptr));
        excludeAutoNum->setText(QCoreApplication::translate("PanelPropertysDlg", "\320\220\320\262\321\202\320\276\320\274\320\260\321\202\320\270\321\207\320\265\321\201\320\272\321\203\321\216 \320\275\321\203\320\274\320\265\321\200\320\260\321\206\320\270\321\216 \320\277\320\276\320\273\320\265\320\271", nullptr));
        excludeShadow->setText(QCoreApplication::translate("PanelPropertysDlg", "\320\236\321\202\320\276\320\261\321\200\320\260\320\266\320\265\320\275\320\270\320\265 \321\202\320\265\320\275\320\270", nullptr));
        alignTextRight->setText(QCoreApplication::translate("PanelPropertysDlg", "\320\222\321\213\321\200\320\260\320\262\320\275\320\270\320\262\320\260\320\275\320\270\320\265 \321\202\320\265\320\272\321\201\321\202\320\260 \321\201\320\277\321\200\320\260\320\262\320\260", nullptr));
        alignPanCenter->setText(QCoreApplication::translate("PanelPropertysDlg", "\320\222\321\213\320\262\320\276\320\264\320\270\321\202\321\214 \320\277\320\260\320\275\320\265\320\273\321\214 \320\277\320\276 \321\206\320\265\320\275\321\202\321\200\321\203", nullptr));
        label_11->setText(QCoreApplication::translate("PanelPropertysDlg", "\320\241\321\202\321\200\320\260\320\275\320\270\321\206\320\260 \320\277\320\276\320\274\320\276\321\211\320\270:", nullptr));
        label_12->setText(QCoreApplication::translate("PanelPropertysDlg", "\320\232\320\276\320\274\320\274\320\265\320\275\321\202\320\260\321\200\320\270\320\271:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PanelPropertysDlg: public Ui_PanelPropertysDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PANELPROPERTYSDLG_H
