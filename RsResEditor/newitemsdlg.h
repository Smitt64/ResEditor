#ifndef NEWITEMSDLG_H
#define NEWITEMSDLG_H

#include <QDialog>
#include <QIcon>
#include <QStyledItemDelegate>
#include <QValidator>

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
    RoleTitle,
    RoleValidator,
    RoleUserTemplate,
    RoleBadge
};

using GroupInfoMap = QMap<int, QVariant>;

// Рисует бейдж (RoleBadge) в правом верхнем углу элемента списка,
// помеченного RoleUserTemplate (пользовательские шаблоны)
class BadgeItemDelegate : public QStyledItemDelegate
{
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const Q_DECL_OVERRIDE;

private:
    mutable QHash<QString, QIcon> m_BadgeCache;
};

class StdPanNameValidator : public QValidator
{
    Q_OBJECT
public:
    Q_INVOKABLE StdPanNameValidator(QObject* parent = nullptr);
    virtual ~StdPanNameValidator() = default;

    QValidator::State validate(QString &input, int &pos) const Q_DECL_OVERRIDE;
};

Q_DECLARE_OPAQUE_POINTER(StdPanNameValidator)

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

    // Generic-доступ к секциям "ribbon" из метаданных (panels/scrols/menus/...)
    const QStringList &ribbonSection(const QString &key) const;
    QStringList ribbonSectionKeys() const;

    GroupInfoMap fillGroupInfoFromListItem(QListWidgetItem* item);
    GroupInfoMap getInfoForItem(const QString &guid);

    static QValidator *createValidator(const QString &className, QObject* parent = nullptr);

    // Иконка с бейджем в правом нижнем углу (пометка пользовательских
    // шаблонов). badgeName — имя иконки темы ("UserPurple" и т.п.)
    static QIcon badgedIcon(const QIcon &base, const QString &badgeName);

private slots:
    void itemUpdated(QListWidgetItem *item);
    void pathButton();
    void updateAcceptButton();
    void itemDoubleClicked();
    void applyFilter(const QString &text);

protected:
    virtual void showEvent(QShowEvent* event) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    virtual bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;

private:
    void updateListSize(QListWidget *list);
    void updateAllListSizes();
    QListWidget *CreateSubList();
    void addItemToGroupList(QListWidget *list, const QJsonObject &metadata);

    QListWidgetItem *m_pSelectedItem;
    Ui::NewItemsDlg *ui;

    LbrObjectInterface *m_pLbrObj;

    QMap<QString, QStringList> m_RibbonSections;
    QHash<QString, QTreeWidgetItem*> m_Groups;
    QHash<QString, GroupInfoMap> m_Templates;

    QValidator *m_pValidator;
};

#endif // NEWITEMSDLG_H
