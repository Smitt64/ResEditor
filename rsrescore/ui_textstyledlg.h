/********************************************************************************
** Form generated from reading UI file 'textstyledlg.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEXTSTYLEDLG_H
#define UI_TEXTSTYLEDLG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_TextStyleDlg
{
public:
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout_6;
    QVBoxLayout *verticalLayout_4;
    QLabel *label;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_2;
    QToolButton *toolAlignLeft;
    QToolButton *toolAlignCenter;
    QToolButton *toolAlignRight;
    QSpacerItem *horizontalSpacer;
    QFrame *frame;
    QVBoxLayout *verticalLayout_5;
    QLabel *label_2;
    QFormLayout *formLayout;
    QCheckBox *checkBox;
    QCheckBox *checkBox_3;
    QCheckBox *checkBox_2;
    QCheckBox *checkBox_4;
    QFrame *frame_2;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_3;
    QComboBox *comboTextColor;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_4;
    QComboBox *comboTextBack;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *TextStyleDlg)
    {
        if (TextStyleDlg->objectName().isEmpty())
            TextStyleDlg->setObjectName(QString::fromUtf8("TextStyleDlg"));
        TextStyleDlg->resize(347, 293);
        horizontalLayout_3 = new QHBoxLayout(TextStyleDlg);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setSpacing(10);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        label = new QLabel(TextStyleDlg);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout_4->addWidget(label);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        toolAlignLeft = new QToolButton(TextStyleDlg);
        toolAlignLeft->setObjectName(QString::fromUtf8("toolAlignLeft"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/img/TextLeftJustify_16x.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolAlignLeft->setIcon(icon);
        toolAlignLeft->setCheckable(true);
        toolAlignLeft->setChecked(true);

        horizontalLayout->addWidget(toolAlignLeft);

        toolAlignCenter = new QToolButton(TextStyleDlg);
        toolAlignCenter->setObjectName(QString::fromUtf8("toolAlignCenter"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/img/TextCenter_16x.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolAlignCenter->setIcon(icon1);
        toolAlignCenter->setCheckable(true);

        horizontalLayout->addWidget(toolAlignCenter);

        toolAlignRight = new QToolButton(TextStyleDlg);
        toolAlignRight->setObjectName(QString::fromUtf8("toolAlignRight"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/img/TextRightJustify_16x.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolAlignRight->setIcon(icon2);
        toolAlignRight->setCheckable(true);

        horizontalLayout->addWidget(toolAlignRight);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout_4->addLayout(horizontalLayout);


        verticalLayout_6->addLayout(verticalLayout_4);

        frame = new QFrame(TextStyleDlg);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setFrameShape(QFrame::HLine);
        frame->setFrameShadow(QFrame::Raised);

        verticalLayout_6->addWidget(frame);

        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        label_2 = new QLabel(TextStyleDlg);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout_5->addWidget(label_2);

        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        formLayout->setHorizontalSpacing(50);
        checkBox = new QCheckBox(TextStyleDlg);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));

        formLayout->setWidget(0, QFormLayout::LabelRole, checkBox);

        checkBox_3 = new QCheckBox(TextStyleDlg);
        checkBox_3->setObjectName(QString::fromUtf8("checkBox_3"));

        formLayout->setWidget(0, QFormLayout::FieldRole, checkBox_3);

        checkBox_2 = new QCheckBox(TextStyleDlg);
        checkBox_2->setObjectName(QString::fromUtf8("checkBox_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, checkBox_2);

        checkBox_4 = new QCheckBox(TextStyleDlg);
        checkBox_4->setObjectName(QString::fromUtf8("checkBox_4"));

        formLayout->setWidget(1, QFormLayout::FieldRole, checkBox_4);


        verticalLayout_5->addLayout(formLayout);


        verticalLayout_6->addLayout(verticalLayout_5);

        frame_2 = new QFrame(TextStyleDlg);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setFrameShape(QFrame::HLine);
        frame_2->setFrameShadow(QFrame::Raised);

        verticalLayout_6->addWidget(frame_2);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(30);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        label_3 = new QLabel(TextStyleDlg);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        verticalLayout_3->addWidget(label_3);

        comboTextColor = new QComboBox(TextStyleDlg);
        comboTextColor->setObjectName(QString::fromUtf8("comboTextColor"));
        comboTextColor->setMinimumSize(QSize(120, 0));

        verticalLayout_3->addWidget(comboTextColor);


        horizontalLayout_2->addLayout(verticalLayout_3);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        label_4 = new QLabel(TextStyleDlg);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        verticalLayout_2->addWidget(label_4);

        comboTextBack = new QComboBox(TextStyleDlg);
        comboTextBack->setObjectName(QString::fromUtf8("comboTextBack"));
        comboTextBack->setMinimumSize(QSize(120, 0));

        verticalLayout_2->addWidget(comboTextBack);


        horizontalLayout_2->addLayout(verticalLayout_2);


        verticalLayout_6->addLayout(horizontalLayout_2);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_6->addItem(verticalSpacer);

        buttonBox = new QDialogButtonBox(TextStyleDlg);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(false);

        verticalLayout_6->addWidget(buttonBox);


        horizontalLayout_3->addLayout(verticalLayout_6);


        retranslateUi(TextStyleDlg);
        QObject::connect(buttonBox, SIGNAL(accepted()), TextStyleDlg, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), TextStyleDlg, SLOT(reject()));

        QMetaObject::connectSlotsByName(TextStyleDlg);
    } // setupUi

    void retranslateUi(QDialog *TextStyleDlg)
    {
        TextStyleDlg->setWindowTitle(QCoreApplication::translate("TextStyleDlg", "Dialog", nullptr));
        label->setText(QCoreApplication::translate("TextStyleDlg", "\320\222\321\213\321\200\320\260\320\262\320\275\320\270\320\262\320\260\320\275\320\270\320\265:", nullptr));
        toolAlignLeft->setText(QString());
        toolAlignCenter->setText(QString());
        toolAlignRight->setText(QString());
        label_2->setText(QCoreApplication::translate("TextStyleDlg", "\320\235\320\260\321\207\320\265\321\200\321\202\320\260\320\275\320\270\320\265:", nullptr));
        checkBox->setText(QCoreApplication::translate("TextStyleDlg", "\320\237\320\276\320\273\321\203\320\266\320\270\321\200\320\275\321\213\320\271", nullptr));
        checkBox_3->setText(QCoreApplication::translate("TextStyleDlg", "\320\237\320\276\320\264\321\207\320\265\321\200\320\272\320\275\321\203\321\202\321\213\320\271", nullptr));
        checkBox_2->setText(QCoreApplication::translate("TextStyleDlg", "\320\232\321\203\321\200\321\201\320\270\320\262", nullptr));
        checkBox_4->setText(QCoreApplication::translate("TextStyleDlg", "\320\227\320\260\321\207\320\265\321\200\320\272\320\275\321\203\321\202\321\213\320\271", nullptr));
        label_3->setText(QCoreApplication::translate("TextStyleDlg", "\320\246\320\262\320\265\321\202 \321\202\320\265\320\272\321\201\321\202\320\260:", nullptr));
        label_4->setText(QCoreApplication::translate("TextStyleDlg", "\320\246\320\262\320\265\321\202 \321\204\320\276\320\275\320\260:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TextStyleDlg: public Ui_TextStyleDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEXTSTYLEDLG_H
