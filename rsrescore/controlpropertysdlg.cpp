#include "controlpropertysdlg.h"
#include "ui_controlpropertysdlg.h"

ControlPropertysDlg::ControlPropertysDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ControlPropertysDlg)
{
    ui->setupUi(this);
    ui->tabOrderWidget->adjustSize();
    adjustSize();
}

ControlPropertysDlg::~ControlPropertysDlg()
{
    delete ui;
}
