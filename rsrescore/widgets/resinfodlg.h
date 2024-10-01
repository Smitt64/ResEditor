#ifndef RESINFODLG_H
#define RESINFODLG_H

#include <QDialog>

namespace Ui {
class ResInfoDlg;
}

class ResInfoDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ResInfoDlg(QWidget *parent = nullptr);
    ~ResInfoDlg();

    void setTitle(const QString &text);
    void setType(const QString &text);
    void setFields(const int &count);
    void setLabels(const int &count);

private:
    void setValue(const QString &key, const QString &text);
    Ui::ResInfoDlg *ui;
};

#endif // RESINFODLG_H
