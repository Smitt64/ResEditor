#include "controltaborderedit.h"
#include "ui_controltaborderedit.h"
#include "controtaborder.h"

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

void ControlTabOrderEdit::setValue(const ControTabOrder * const value)
{
    ui->up->setValue(value->up());
    ui->bottom->setValue(value->bottom());
    ui->left->setValue(value->previous());
    ui->right->setValue(value->next());
    ui->self->setValue(value->thisid());
}

void ControlTabOrderEdit::getValue(ControTabOrder *other)
{
    other->setUp(ui->up->value());
    other->setBottom(ui->bottom->value());
    other->setPrevious(ui->left->value());
    other->setNext(ui->right->value());
    other->setThisId(ui->self->value());
}
