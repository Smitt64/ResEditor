#ifndef PROPERTYGROUPTREEITEM_H
#define PROPERTYGROUPTREEITEM_H

#include "propertytreeitem.h"
#include <QObject>

class PropertyGroupTreeItem : public PropertyTreeItem
{
    Q_OBJECT
public:
    explicit PropertyGroupTreeItem(const QString &group, QObject *parent = nullptr);
    virtual ~PropertyGroupTreeItem();

    virtual QVariant data(const int &role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
};

#endif // PROPERTYGROUPTREEITEM_H
