#ifndef NEWITEMSDLG_H
#define NEWITEMSDLG_H

#include <QDialog>

namespace Ui {
class NewItemsDlg;
}

enum
{
    RoleDescription = Qt::UserRole + 1,
    RoleAction,
    RoleGroup,
    RoleNeedName,
    RoleNeedLbr,
    RoleNeedPath,
    RoleNameLen,
    RoleIconName,
    RoleTitle
};

using GroupInfoMap = QMap<int, QVariant>;

class QListWidget;
class QTreeWidgetItem;
class QJsonObject;
class QListWidgetItem;
class LbrObjectInterface;
class NewItemsDlg : public QDialog
{
    Q_OBJECT

public:
    explicit NewItemsDlg(LbrObjectInterface *lbr, QWidget *parent = nullptr);
    ~NewItemsDlg();

    void buildStandartNewItems();
    void filterByAction(const QString& action);

    QListWidget *getGroup(const QString &name, bool addIfNotExists);
    void addFromMetaDataList(const QString &metadata);
    void addFromMetaData(const QJsonObject &metadata);

    QStringList getGroups() const;

    QString action() const;
    QString name() const;
    QString path() const;

    QList<GroupInfoMap> groupInfo(const QString &name);

    const QStringList &ribbonScrols() const;
    const QStringList &ribbonPannels() const;

    GroupInfoMap fillGroupInfoFromListItem(QListWidgetItem* item);
    GroupInfoMap getInfoForItem(const QString &guid);

private slots:
    void itemUpdated(QListWidgetItem *item);
    void pathButton();
    void updateAcceptButton();
    void itemDoubleClicked();

protected:
    virtual void showEvent(QShowEvent* event) Q_DECL_OVERRIDE;

private:
    void updateListSize(QListWidget *list);
    QListWidget *CreateSubList();
    void addItemToGroupList(QListWidget *list, const QJsonObject &metadata);

    QListWidgetItem *m_pSelectedItem;
    Ui::NewItemsDlg *ui;

    LbrObjectInterface *m_pLbrObj;

    QStringList m_RibbonScrols, m_RibbonPannels;
    QHash<QString, QTreeWidgetItem*> m_Groups;
    QHash<QString, GroupInfoMap> m_Templates;
};

#endif // NEWITEMSDLG_H
