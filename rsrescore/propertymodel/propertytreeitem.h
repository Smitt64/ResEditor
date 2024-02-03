#ifndef PROPERTYTREEITEM_H
#define PROPERTYTREEITEM_H

#include <QObject>
#include <QVariant>
#include <QJsonObject>
#include <QModelIndex>
#include <QSpinBox>

class CustomRectItem;
class QStyleOptionViewItem;
class PropertyTreeItem : public QObject
{
    Q_OBJECT
public:
    typedef QVector<PropertyTreeItem*> PropertyTreeItemList;
    typedef QMap<QString, QVariant> EditorPropMap;

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
    PropertyTreeItem(QObject *object, QObject *parent = nullptr);
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

    QObject *object() const;
    virtual void setObject(QObject *object);

    QModelIndex index() const;

    bool isEditable();
    const int &group() const;
    bool isEnabled() const;
    void setEnabled(const bool &v);

    PropertyTreeItemList::iterator begin();
    PropertyTreeItemList::iterator end();

    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual bool setEditorData(QWidget *editor, const QModelIndex &index) const;
    virtual bool setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    virtual void initFromJson(const QJsonObject &obj);
    void initFromJsonBaseTypes(const QJsonObject &obj, const QMetaType::Type &type);

signals:
    void dataChanged();

public slots:
    virtual void setPropertyName(const QString &name);
    virtual void setPropertyAlias(const QString &name);
    virtual void setGroup(const int &group);

protected:
    void ConnectNotify();
    QWidget *createDefaultEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index, const QMetaType::Type &type) const;

    QMetaMethod findMethod(const QString &name) const;
    QString m_PropertyName, m_PropertyAlias;
    QObject *m_pItem;

    TypeItem m_TypeItem;

private:
    PropertyTreeItemList m_childItems;
    int m_ItemGroup;
    QVariant m_isEditable;
    PropertyTreeItem *m_parentItem;

    EditorPropMap m_EditorProp;

    bool m_Enable;
};

#endif // PROPERTYTREEITEM_H
