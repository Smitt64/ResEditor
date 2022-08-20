#include "propertygrouptreeitem.h"
#include <QColor>

PropertyGroupTreeItem::PropertyGroupTreeItem(const QString &group, QObject *parent)
    : PropertyTreeItem{nullptr, parent}
{
    setPropertyAlias(group);
    m_TypeItem = PropertyTreeItem::TypeItem_Group;
}

PropertyGroupTreeItem::~PropertyGroupTreeItem()
{

}

QVariant PropertyGroupTreeItem::data(const int &role) const
{
    if (role == Qt::BackgroundColorRole || role == Qt::BackgroundRole)
        return QColor(Qt::darkGray);
    else if (role == Qt::ForegroundRole)
        return QColor(Qt::white);

    return PropertyTreeItem::data(role);
}
