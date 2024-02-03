#include "pointpropertytreeitem.h"
#include <QMetaObject>
#include <QMetaProperty>

PointPropertyTreeItem::PointPropertyTreeItem(QObject *rectItem, const NodeSubType &type, QObject *parent)
    : PropertyTreeItem{rectItem, parent},
    m_SubType(type)
{
}

PointPropertyTreeItem::~PointPropertyTreeItem()
{

}

void PointPropertyTreeItem::addChild(const NodeSubType &st)
{
    PointPropertyTreeItem *newItem = new PointPropertyTreeItem(m_pItem, st);
    newItem->setPropertyName(m_PropertyName);
    newItem->setGroup(group());
    appendChild(newItem);
}

QString PointPropertyTreeItem::propertyAlias() const
{
    if (m_SubType == SubTypeX)
        return tr("X");
    else if (m_SubType == SubTypeY)
        return tr("Y");

    return PropertyTreeItem::propertyAlias();
}

void PointPropertyTreeItem::initFromJson(const QJsonObject &obj)
{
    if (m_SubType == SubTypeRootPoint)
    {
        if (obj.contains("x"))
            child(SubTypeX)->initFromJsonBaseTypes(obj["x"].toObject(), QMetaType::Int);

        if (obj.contains("y"))
            child(SubTypeY)->initFromJsonBaseTypes(obj["y"].toObject(), QMetaType::Int);
    }
}

Qt::ItemFlags PointPropertyTreeItem::flags() const
{
    if (m_SubType == SubTypeRootPoint)
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;

    return PropertyTreeItem::flags();
}

QWidget *PointPropertyTreeItem::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (m_SubType == SubTypeRootPoint)
        return nullptr;

    return createDefaultEditor(parent, option, index, QMetaType::Int);
}

void PointPropertyTreeItem::setData(const QVariant &value)
{
    const QMetaObject *metaobject = m_pItem->metaObject();
    int propIndex = metaobject->indexOfProperty(m_PropertyName.toLocal8Bit().data());

    if (propIndex >= 0)
    {
        QMetaProperty prop = metaobject->property(propIndex);
        QVariant propvalue = m_pItem->property(m_PropertyName.toLocal8Bit().data());

        QPoint pos = propvalue.toPoint();
        if (m_SubType == SubTypeX)
            pos.setX(value.toInt());
        else if (m_SubType == SubTypeY)
            pos.setY(value.toInt());

        m_pItem->setProperty(m_PropertyName.toLocal8Bit().data(), pos);
        emit dataChanged();
        emit parentItem()->dataChanged();
    }
}

void PointPropertyTreeItem::setPropertyName(const QString &name)
{
    PropertyTreeItem::setPropertyName(name);

    if (m_SubType == SubTypeRootPoint)
    {
        addChild(SubTypeX);
        addChild(SubTypeY);
    }
}

QVariant PointPropertyTreeItem::data(const int &role) const
{
    QObject *item = m_pItem;
    PointPropertyTreeItem *pThis = const_cast<PointPropertyTreeItem*>(this);
    if (m_SubType != SubTypeRootPoint)
    {
        item = pThis->parentItem()->object();
        pThis = dynamic_cast<PointPropertyTreeItem*>(pThis->parentItem());
    }

    QVariant variant = item->property(pThis->m_PropertyName.toLocal8Bit().data());
    QPoint point = variant.toPoint();

    if (role == Qt::DisplayRole)
    {
        if (m_SubType == SubTypeRootPoint)
        {
            return QString("(%1:%2)")
                .arg(point.x())
                .arg(point.y());
        }
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (m_SubType == SubTypeX)
            return point.x();
        else if (m_SubType == SubTypeY)
            return point.y();
    }

    if (role == Qt::BackgroundRole)
        return PropertyTreeItem::data(role);

    return PropertyTreeItem::data(role);
}
