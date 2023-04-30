#ifndef CONTROLTABORDEREDIT_H
#define CONTROLTABORDEREDIT_H

#include <QWidget>

namespace Ui {
class ControlTabOrderEdit;
}

class ControTabOrder;
class ControlTabOrderEdit : public QWidget
{
    Q_OBJECT

public:
    ControlTabOrderEdit(QWidget *parent = nullptr);
    ~ControlTabOrderEdit();

    void setValue(const ControTabOrder * const value);
    void getValue(ControTabOrder *other);

private:
    Ui::ControlTabOrderEdit *ui;
};

#endif // CONTROLTABORDEREDIT_H
