/********************************************************************************
** Form generated from reading UI file 'controltaborderedit.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONTROLTABORDEREDIT_H
#define UI_CONTROLTABORDEREDIT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ControlTabOrderEdit
{
public:
    QHBoxLayout *horizontalLayout;
    QGridLayout *gridLayout;
    QWidget *widget_2;
    QSpinBox *spinBox;
    QWidget *widget;
    QSpinBox *spinBox_5;
    QSpinBox *spinBox_2;
    QSpinBox *spinBox_4;
    QWidget *widget_3;
    QSpinBox *spinBox_3;
    QWidget *widget_4;

    void setupUi(QWidget *ControlTabOrderEdit)
    {
        if (ControlTabOrderEdit->objectName().isEmpty())
            ControlTabOrderEdit->setObjectName(QString::fromUtf8("ControlTabOrderEdit"));
        ControlTabOrderEdit->resize(134, 80);
        horizontalLayout = new QHBoxLayout(ControlTabOrderEdit);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        widget_2 = new QWidget(ControlTabOrderEdit);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));

        gridLayout->addWidget(widget_2, 0, 0, 1, 1);

        spinBox = new QSpinBox(ControlTabOrderEdit);
        spinBox->setObjectName(QString::fromUtf8("spinBox"));
        spinBox->setMinimumSize(QSize(40, 0));

        gridLayout->addWidget(spinBox, 0, 1, 1, 1);

        widget = new QWidget(ControlTabOrderEdit);
        widget->setObjectName(QString::fromUtf8("widget"));

        gridLayout->addWidget(widget, 0, 2, 1, 1);

        spinBox_5 = new QSpinBox(ControlTabOrderEdit);
        spinBox_5->setObjectName(QString::fromUtf8("spinBox_5"));
        spinBox_5->setMinimumSize(QSize(40, 0));

        gridLayout->addWidget(spinBox_5, 1, 0, 1, 1);

        spinBox_2 = new QSpinBox(ControlTabOrderEdit);
        spinBox_2->setObjectName(QString::fromUtf8("spinBox_2"));

        gridLayout->addWidget(spinBox_2, 1, 1, 1, 1);

        spinBox_4 = new QSpinBox(ControlTabOrderEdit);
        spinBox_4->setObjectName(QString::fromUtf8("spinBox_4"));
        spinBox_4->setMinimumSize(QSize(40, 0));

        gridLayout->addWidget(spinBox_4, 1, 2, 1, 1);

        widget_3 = new QWidget(ControlTabOrderEdit);
        widget_3->setObjectName(QString::fromUtf8("widget_3"));

        gridLayout->addWidget(widget_3, 2, 0, 1, 1);

        spinBox_3 = new QSpinBox(ControlTabOrderEdit);
        spinBox_3->setObjectName(QString::fromUtf8("spinBox_3"));

        gridLayout->addWidget(spinBox_3, 2, 1, 1, 1);

        widget_4 = new QWidget(ControlTabOrderEdit);
        widget_4->setObjectName(QString::fromUtf8("widget_4"));

        gridLayout->addWidget(widget_4, 2, 2, 1, 1);


        horizontalLayout->addLayout(gridLayout);


        retranslateUi(ControlTabOrderEdit);

        QMetaObject::connectSlotsByName(ControlTabOrderEdit);
    } // setupUi

    void retranslateUi(QWidget *ControlTabOrderEdit)
    {
        ControlTabOrderEdit->setWindowTitle(QCoreApplication::translate("ControlTabOrderEdit", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ControlTabOrderEdit: public Ui_ControlTabOrderEdit {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTROLTABORDEREDIT_H
