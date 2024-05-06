#include "textpage.h"
#include "ui_textpage.h"
#include <QTextBlock>

TextPage::TextPage(QWidget *parent)
    : QWizardPage(parent)
    , ui(new Ui::TextPage)
{
    ui->setupUi(this);

    QTextDocument *doc = ui->textEdit->document();
    QFont font = doc->defaultFont();
    font.setFamily("Courier New");
    doc->setDefaultFont(font);
}

TextPage::~TextPage()
{
    delete ui;
}

bool TextPage::validatePage()
{
    QTextDocument *doc = ui->textEdit->document();
    QTextBlock block = doc->firstBlock();

    while (block.isValid())
    {
        QTextCursor cursor = QTextCursor(block);

        //for (int i = )
        block = block.next();
    }

    return false;
}
