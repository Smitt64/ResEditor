#ifndef BANKDISTRIBSELECT_H
#define BANKDISTRIBSELECT_H

#include <QDialog>
#include "rsrescore_global.h"

namespace Ui {
class BankDistribSelect;
}

class QTreeWidgetItem;
class RSRESCORE_EXPORT BankDistribSelect : public QDialog
{
    Q_OBJECT

public:
    explicit BankDistribSelect(QWidget *parent = nullptr);
    ~BankDistribSelect();

    QString path() const;

private slots:
    void on_pushButton_clicked();

    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

private:
    Ui::BankDistribSelect *ui;
};

#endif // BANKDISTRIBSELECT_H
