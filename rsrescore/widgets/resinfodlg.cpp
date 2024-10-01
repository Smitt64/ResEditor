#include "resinfodlg.h"
#include "ui_resinfodlg.h"

ResInfoDlg::ResInfoDlg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ResInfoDlg)
{
    ui->setupUi(this);
}

ResInfoDlg::~ResInfoDlg()
{
    delete ui;
}

void ResInfoDlg::setValue(const QString &key, const QString &text)
{
    QString str = ui->textBrowser->toHtml();
    str = str.replace(key, text);
    ui->textBrowser->setHtml(str);
}

void ResInfoDlg::setTitle(const QString &text)
{
    setValue("${title}", text);
}

void ResInfoDlg::setType(const QString &text)
{
    setValue("${type}", text);
}

void ResInfoDlg::setFields(const int &count)
{
    setValue("${fields}", QString::number(count));
}

void ResInfoDlg::setLabels(const int &count)
{
    setValue("${labels}", QString::number(count));
}
