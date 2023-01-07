#ifndef PANELPROPERTYSDLG_H
#define PANELPROPERTYSDLG_H

#include <QDialog>

namespace Ui {
class PanelPropertysDlg;
}

class PanelPropertysDlg : public QDialog
{
    Q_OBJECT

public:
    explicit PanelPropertysDlg(QWidget *parent = nullptr);
    ~PanelPropertysDlg();

    void setPanelMode(const bool &enabled);

private:
    Ui::PanelPropertysDlg *ui;
};

#endif // PANELPROPERTYSDLG_H
