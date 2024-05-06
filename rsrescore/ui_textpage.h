/********************************************************************************
** Form generated from reading UI file 'textpage.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEXTPAGE_H
#define UI_TEXTPAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWizardPage>

QT_BEGIN_NAMESPACE

class Ui_TextPage
{
public:
    QVBoxLayout *verticalLayout;
    QTextEdit *textEdit;

    void setupUi(QWizardPage *TextPage)
    {
        if (TextPage->objectName().isEmpty())
            TextPage->setObjectName(QString::fromUtf8("TextPage"));
        TextPage->resize(642, 451);
        verticalLayout = new QVBoxLayout(TextPage);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        textEdit = new QTextEdit(TextPage);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        textEdit->setLineWrapMode(QTextEdit::NoWrap);

        verticalLayout->addWidget(textEdit);


        retranslateUi(TextPage);

        QMetaObject::connectSlotsByName(TextPage);
    } // setupUi

    void retranslateUi(QWizardPage *TextPage)
    {
        TextPage->setWindowTitle(QCoreApplication::translate("TextPage", "WizardPage", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TextPage: public Ui_TextPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEXTPAGE_H
