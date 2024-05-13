#ifndef SELECTRESOURCEDLG_H
#define SELECTRESOURCEDLG_H

#include <QDialog>

namespace Ui {
class SelectResourceDlg;
}

class QMainWindow;
class QAbstractItemModel;
class SelectResourceDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SelectResourceDlg(QWidget *parent = nullptr);
    ~SelectResourceDlg();

    void setModel(QAbstractItemModel *model);
    void getRes(int &type, QString &name);

private slots:
    void onDoubleClicked(const QModelIndex &index);
    void onClicked(const QModelIndex &index);

private:
    Ui::SelectResourceDlg *ui;

    int m_type;
    QString m_name;
    QMainWindow *m_pContainer;
};

#endif // SELECTRESOURCEDLG_H
