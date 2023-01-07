/********************************************************************************
** Form generated from reading UI file 'texteditwithbutton.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEXTEDITWITHBUTTON_H
#define UI_TEXTEDITWITHBUTTON_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TextEditWithButton
{
public:
    QHBoxLayout *horizontalLayout;
    QLineEdit *lineEdit;
    QToolButton *toolButton;

    void setupUi(QWidget *TextEditWithButton)
    {
        if (TextEditWithButton->objectName().isEmpty())
            TextEditWithButton->setObjectName(QString::fromUtf8("TextEditWithButton"));
        TextEditWithButton->resize(290, 23);
        horizontalLayout = new QHBoxLayout(TextEditWithButton);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        lineEdit = new QLineEdit(TextEditWithButton);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        horizontalLayout->addWidget(lineEdit);

        toolButton = new QToolButton(TextEditWithButton);
        toolButton->setObjectName(QString::fromUtf8("toolButton"));
        toolButton->setFocusPolicy(Qt::NoFocus);

        horizontalLayout->addWidget(toolButton);


        retranslateUi(TextEditWithButton);

        QMetaObject::connectSlotsByName(TextEditWithButton);
    } // setupUi

    void retranslateUi(QWidget *TextEditWithButton)
    {
        TextEditWithButton->setWindowTitle(QCoreApplication::translate("TextEditWithButton", "Form", nullptr));
        toolButton->setText(QCoreApplication::translate("TextEditWithButton", "...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TextEditWithButton: public Ui_TextEditWithButton {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEXTEDITWITHBUTTON_H
