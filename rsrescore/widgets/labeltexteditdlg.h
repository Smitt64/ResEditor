#ifndef LABELTEXTEDITDLG_H
#define LABELTEXTEDITDLG_H

#include <QDialog>

namespace Ui {
class LabelTextEditDlg;
}

class CharacterWidget;
class QLineEdit;
class LabelTextEditDlg : public QDialog
{
    Q_OBJECT

public:
    explicit LabelTextEditDlg(QWidget *parent = nullptr);
    ~LabelTextEditDlg();

    QLineEdit *editor();

private:
    Ui::LabelTextEditDlg *ui;
    CharacterWidget *m_pCharMap;
};

#endif // LABELTEXTEDITDLG_H
