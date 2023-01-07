#include "controltaborderedit.h"
#include "ui_controltaborderedit.h"

ControlTabOrderEdit::ControlTabOrderEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControlTabOrderEdit)
{
    ui->setupUi(this);
}

ControlTabOrderEdit::~ControlTabOrderEdit()
{
    delete ui;
}
