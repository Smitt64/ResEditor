#ifndef PROPERTYTREEITEM_H
#define PROPERTYTREEITEM_H

#include <QObject>
#include <QVariant>
#include <QJsonObject>
#include <QModelIndex>

class CustomRectItem;
class QStyleOptionViewItem;
class PropertyTreeItem : public QObject
{
    Q_OBJECT
public:
    enum TypeItem
    {
        TypeItem_Data,
        TypeItem_Group
    };

    enum PropertyTreeItemRole
    {
        RoleTypeItem = Qt::UserRole + 1,
        RoleGroup
    };
    Q_ENUM(PropertyTreeItemRole)
    Q_ENUM(TypeItem)
    PropertyTreeItem(CustomRectItem *rectItem, QObject *parent = nullptr);
    virtual ~PropertyTreeItem();

    int childCount() const;
    void appendChild(PropertyTreeItem *item);
    PropertyTreeItem *child(const int &row);
    virtual QVariant data(const int &role = Qt::DisplayRole) const;
    virtual void setData(const QVariant &value);
    int row() const;
    PropertyTreeItem *parentItem();
    PropertyTreeItem *parentItem() const;

    virtual Qt::ItemFlags flags() const;

    virtual QString propertyName() const;
    virtual QString propertyAlias() const;

    CustomRectItem *item();

    QModelIndex index() const;

    bool isEditable();
    const int &group() const;
    bool isEnabled() const;
    void setEnabled(const bool &v);

    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual bool setEditorData(QWidget *editor, const QModelIndex &index) const;
    virtual bool setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

signals:
    void dataChanged();

public slots:
    virtual void setPropertyName(const QString &name);
    virtual void setPropertyAlias(const QString &name);
    virtual void setGroup(const int &group);

protected:
    void connectNotify();
    QString m_PropertyName, m_PropertyAlias;
    CustomRectItem *m_pItem;

    TypeItem m_TypeItem;

private:
    QVector<PropertyTreeItem*> m_childItems;
    int m_ItemGroup;
    QVariant m_isEditable;
    PropertyTreeItem *m_parentItem;

    bool m_Enable;
};

#endif // PROPERTYTREEITEM_H
