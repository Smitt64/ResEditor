#ifndef NEWITEMSDLG_H
#define NEWITEMSDLG_H

#include <QDialog>

namespace Ui {
class NewItemsDlg;
}

class QListWidget;
class QTreeWidgetItem;
class QJsonObject;
class QListWidgetItem;
class NewItemsDlg : public QDialog
{
    Q_OBJECT

public:
    explicit NewItemsDlg(QWidget *parent = nullptr);
    ~NewItemsDlg();

    void buildStandartNewItems();

    QListWidget *getGroup(const QString &name, bool addIfNotExists);
    void addFromMetaDataList(const QString &metadata);
    void addFromMetaData(const QJsonObject &metadata);

    QString action() const;
    QString name() const;
    QString path() const;

private slots:
    void itemUpdated(QListWidgetItem *item);
    void pathButton();
    void updateAcceptButton();
    void itemDoubleClicked();

private:
    void updateListSize(QListWidget *list);
    QListWidget *CreateSubList();
    void addItemToGroupList(QListWidget *list, const QJsonObject &metadata);

    QListWidgetItem *m_pSelectedItem;
    Ui::NewItemsDlg *ui;

    QHash<QString, QTreeWidgetItem*> m_Groups;
};

#endif // NEWITEMSDLG_H
