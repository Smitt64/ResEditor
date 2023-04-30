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
    QSpinBox *up;
    QWidget *widget;
    QSpinBox *left;
    QSpinBox *self;
    QSpinBox *right;
    QWidget *widget_3;
    QSpinBox *bottom;
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

        up = new QSpinBox(ControlTabOrderEdit);
        up->setObjectName(QString::fromUtf8("up"));
        up->setMinimumSize(QSize(40, 0));

        gridLayout->addWidget(up, 0, 1, 1, 1);

        widget = new QWidget(ControlTabOrderEdit);
        widget->setObjectName(QString::fromUtf8("widget"));

        gridLayout->addWidget(widget, 0, 2, 1, 1);

        left = new QSpinBox(ControlTabOrderEdit);
        left->setObjectName(QString::fromUtf8("left"));
        left->setMinimumSize(QSize(40, 0));

        gridLayout->addWidget(left, 1, 0, 1, 1);

        self = new QSpinBox(ControlTabOrderEdit);
        self->setObjectName(QString::fromUtf8("self"));

        gridLayout->addWidget(self, 1, 1, 1, 1);

        right = new QSpinBox(ControlTabOrderEdit);
        right->setObjectName(QString::fromUtf8("right"));
        right->setMinimumSize(QSize(40, 0));

        gridLayout->addWidget(right, 1, 2, 1, 1);

        widget_3 = new QWidget(ControlTabOrderEdit);
        widget_3->setObjectName(QString::fromUtf8("widget_3"));

        gridLayout->addWidget(widget_3, 2, 0, 1, 1);

        bottom = new QSpinBox(ControlTabOrderEdit);
        bottom->setObjectName(QString::fromUtf8("bottom"));

        gridLayout->addWidget(bottom, 2, 1, 1, 1);

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
