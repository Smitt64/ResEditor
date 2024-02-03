#ifndef POINTPROPERTYTREEITEM_H
#define POINTPROPERTYTREEITEM_H

#include "propertytreeitem.h"

class PointPropertyTreeItem : public PropertyTreeItem
{
    Q_OBJECT
public:
    enum NodeSubType
    {
        SubTypeRootPoint = -1,
        SubTypeX,
        SubTypeY
    };

    explicit PointPropertyTreeItem(QObject *rectItem, const NodeSubType &type = SubTypeRootPoint, QObject *parent = nullptr);
    virtual ~PointPropertyTreeItem();

    virtual QVariant data(const int &role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual QString propertyAlias() const Q_DECL_OVERRIDE;
    virtual void initFromJson(const QJsonObject &obj) Q_DECL_OVERRIDE;

    virtual Qt::ItemFlags flags() const Q_DECL_OVERRIDE;
    virtual void setData(const QVariant &value) Q_DECL_OVERRIDE;
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

public slots:
    virtual void setPropertyName(const QString &name) Q_DECL_OVERRIDE;

private:
    void addChild(const NodeSubType &st);
    NodeSubType m_SubType;
};

#endif // POINTPROPERTYTREEITEM_H
