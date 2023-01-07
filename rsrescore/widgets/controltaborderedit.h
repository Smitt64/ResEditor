#ifndef CONTROLTABORDEREDIT_H
#define CONTROLTABORDEREDIT_H

#include <QWidget>

namespace Ui {
class ControlTabOrderEdit;
}

class ControlTabOrderEdit : public QWidget
{
    Q_OBJECT

public:
    explicit ControlTabOrderEdit(QWidget *parent = nullptr);
    ~ControlTabOrderEdit();

private:
    Ui::ControlTabOrderEdit *ui;
};

#endif // CONTROLTABORDEREDIT_H
