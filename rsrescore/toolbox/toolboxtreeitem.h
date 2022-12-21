#ifndef TOOLBOXTREEITEM_H
#define TOOLBOXTREEITEM_H

#include <QObject>
#include <QVariant>
#include <QJsonObject>
#include <QModelIndex>
#include <QIcon>
#include "toolbox/toolboxmodel.h"

class QStyleOptionViewItem;
class ToolBoxTreeItem : public QObject
{
    Q_OBJECT
public:
    enum TypeItem
    {
        TypeItem_Data,
        TypeItem_Group
    };

    enum ToolBoxTreeItemRole
    {
        RoleTypeItem = ToolBoxModel::TypeItemRole,
        RoleGroup = ToolBoxModel::GroupRole
    };
    Q_ENUM(ToolBoxTreeItemRole)
    Q_ENUM(TypeItem)
    ToolBoxTreeItem(const TypeItem &type, QObject *parent = nullptr);
    virtual ~ToolBoxTreeItem();

    int childCount() const;
    void appendChild(ToolBoxTreeItem *item);
    ToolBoxTreeItem *child(const int &row);
    virtual QVariant data(const int &role = Qt::DisplayRole) const;

    int row() const;
    ToolBoxTreeItem *parentItem();
    ToolBoxTreeItem *parentItem() const;

    virtual Qt::ItemFlags flags() const;

    QModelIndex index() const;
    void setAlias(const QString &alias);
    void setIcon(const QIcon &icon);
    void setMimeData(const QString &mimetype, const QByteArray &data);

signals:
    void dataChanged();

protected:
    QString m_Alias;

    TypeItem m_TypeItem;

private:
    QIcon m_Icon;
    QString m_MimeType;
    QByteArray m_MimeData;
    QVector<ToolBoxTreeItem*> m_childItems;
    ToolBoxTreeItem *m_parentItem;

    bool m_Enable;
};

#endif // TOOLBOXTREEITEM_H
