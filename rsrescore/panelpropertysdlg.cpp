#include "panelpropertysdlg.h"
#include "ui_panelpropertysdlg.h"

PanelPropertysDlg::PanelPropertysDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PanelPropertysDlg)
{
    ui->setupUi(this);
}

PanelPropertysDlg::~PanelPropertysDlg()
{
    delete ui;
}

void PanelPropertysDlg::setPanelMode(const bool &enabled)
{
    ui->groupBoxScrol->setVisible(!enabled);
    adjustSize();
}
