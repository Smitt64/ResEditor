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
        TextPage->resize(800, 600);
        TextPage->setMinimumSize(QSize(800, 600));
        verticalLayout = new QVBoxLayout(TextPage);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        textEdit = new QTextEdit(TextPage);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        textEdit->setLineWrapMode(QTextEdit::LineWrapMode::NoWrap);

        verticalLayout->addWidget(textEdit);


        retranslateUi(TextPage);

        QMetaObject::connectSlotsByName(TextPage);
    } // setupUi

    void retranslateUi(QWizardPage *TextPage)
    {
        TextPage->setWindowTitle(QCoreApplication::translate("TextPage", "WizardPage", nullptr));
        TextPage->setTitle(QCoreApplication::translate("TextPage", "\320\234\320\260\320\272\320\265\321\202 \321\201\321\202\321\200\320\260\320\275\320\270\321\206\321\213", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TextPage: public Ui_TextPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEXTPAGE_H
