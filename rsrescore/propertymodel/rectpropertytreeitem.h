#ifndef RECTPROPERTYTREEITEM_H
#define RECTPROPERTYTREEITEM_H

#include "propertytreeitem.h"
#include <QObject>

class RectPropertyTreeItem : public PropertyTreeItem
{
    Q_OBJECT
public:
    enum NodeSubType
    {
        SubTypeRootRect,
        SubTypeX,
        SubTypeY,
        SubTypeWidth,
        SubTypeHeight
    };

    explicit RectPropertyTreeItem(QObject *rectItem, const NodeSubType &type = SubTypeRootRect, QObject *parent = nullptr);
    virtual ~RectPropertyTreeItem();

    virtual QVariant data(const int &role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual QString propertyAlias() const Q_DECL_OVERRIDE;

private:
    void addChild(const NodeSubType &st);
    NodeSubType m_SubType;
};

#endif // RECTPROPERTYTREEITEM_H
