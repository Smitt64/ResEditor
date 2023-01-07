#ifndef CONTROLPROPERTYSDLG_H
#define CONTROLPROPERTYSDLG_H

#include <QDialog>

namespace Ui {
class ControlPropertysDlg;
}

class ControlPropertysDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ControlPropertysDlg(QWidget *parent = nullptr);
    ~ControlPropertysDlg();

private:
    Ui::ControlPropertysDlg *ui;
};

#endif // CONTROLPROPERTYSDLG_H
