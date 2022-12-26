#include "rectpropertytreeitem.h"
#include <QRect>

RectPropertyTreeItem::RectPropertyTreeItem(QObject *rectItem, const NodeSubType &type, QObject *parent)
    : PropertyTreeItem{rectItem, parent},
      m_SubType(type)
{
    if (m_SubType == SubTypeRootRect)
    {
        addChild(SubTypeX);
        addChild(SubTypeY);
        addChild(SubTypeWidth);
        addChild(SubTypeHeight);
    }
}

RectPropertyTreeItem::~RectPropertyTreeItem()
{

}

void RectPropertyTreeItem::addChild(const NodeSubType &st)
{
    RectPropertyTreeItem *item = new RectPropertyTreeItem(m_pItem, st);
    item->setPropertyName(m_PropertyName);
    item->setGroup(group());
    //item->m_PropertyName = m_PropertyName;
    appendChild(item);
}

QString RectPropertyTreeItem::propertyAlias() const
{
    if (m_SubType == SubTypeX)
        return tr("X");
    else if (m_SubType == SubTypeY)
        return tr("Y");
    else if (m_SubType == SubTypeWidth)
        return tr("Ширина");
    else if (m_SubType == SubTypeHeight)
        return tr("Высота");

    return PropertyTreeItem::propertyAlias();
}

QVariant RectPropertyTreeItem::data(const int &role) const
{
    QObject *item = m_pItem;
    RectPropertyTreeItem *pThis = const_cast<RectPropertyTreeItem*>(this);
    if (m_SubType != SubTypeRootRect)
    {
        item = pThis->parentItem()->object();
        pThis = dynamic_cast<RectPropertyTreeItem*>(pThis->parentItem());
    }

    QVariant variant = item->property(pThis->m_PropertyName.toLocal8Bit().data());
    QRect rect = variant.toRect();

    if (role == Qt::DisplayRole)
    {
        if (m_SubType == SubTypeRootRect)
        {
            return QString("[(%1 : %2), %3 x %4]")
                    .arg(rect.x())
                    .arg(rect.y())
                    .arg(rect.width())
                    .arg(rect.height());
        }
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (m_SubType == SubTypeX)
            return rect.x();
        else if (m_SubType == SubTypeY)
            return rect.y();
        else if (m_SubType == SubTypeWidth)
            return rect.width();
        else if (m_SubType == SubTypeHeight)
            return rect.height();
    }

    if (role == Qt::BackgroundRole)
        return PropertyTreeItem::data(role);

    return PropertyTreeItem::data(role);
}
