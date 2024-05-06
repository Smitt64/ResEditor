#include "textpage.h"
#include "ui_textpage.h"

TextPage::TextPage(QWidget *parent)
    : QWizardPage(parent)
    , ui(new Ui::TextPage)
{
    ui->setupUi(this);
}

TextPage::~TextPage()
{
    delete ui;
}
