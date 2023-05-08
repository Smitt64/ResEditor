#include "flagpropertytreeitem.h"
#include "qcheckbox.h"
#include "qfont.h"
#include <QMetaObject>
#include <QMetaProperty>
#include <QMetaEnum>

FlagPropertyTreeItem::FlagPropertyTreeItem(QObject *rectItem, const NodeSubType &type, QObject *parent)
    : PropertyTreeItem{rectItem, parent},
      m_SubType(type)
{
}

FlagPropertyTreeItem::~FlagPropertyTreeItem()
{

}

void FlagPropertyTreeItem::addChild(const QString &flagName)
{
    FlagPropertyTreeItem *item = new FlagPropertyTreeItem(m_pItem, SubTypeFlag);
    item->m_flagName = flagName;
    item->setPropertyName(m_PropertyName);
    item->setGroup(group());
    appendChild(item);
}

void FlagPropertyTreeItem::setPropertyName(const QString &name)
{
    PropertyTreeItem::setPropertyName(name);
    if (m_SubType == SubTypeRoot)
    {
        const QMetaObject *metaobject = m_pItem->metaObject();
        int propid = metaobject->indexOfProperty(name.toLocal8Bit().data());

        if (propid >= 0)
        {
            QMetaProperty prop = metaobject->property(propid);
            QMetaEnum _enum = prop.enumerator();

            for (int i = 0; i < _enum.keyCount(); i++)
                addChild(_enum.key(i));
        }
    }
}

QString FlagPropertyTreeItem::propertyAlias() const
{
    if (m_SubType != SubTypeRoot)
    {
        FlagPropertyTreeItem *parentFlag = dynamic_cast<FlagPropertyTreeItem*>(parentItem());

        QString Alias = parentFlag->m_KeyAlias[m_flagName];

        if (Alias.isEmpty())
            Alias = m_flagName;

        return Alias;
    }

    return PropertyTreeItem::propertyAlias();
}

void FlagPropertyTreeItem::setObject(QObject *object)
{
    PropertyTreeItem::setObject(object);

    if (m_SubType == SubTypeRoot)
    {
        for (PropertyTreeItem *child : *this)
            child->setObject(object);
    }
}

void FlagPropertyTreeItem::setData(const QVariant &value)
{
    if (!m_pItem)
        return;

    if (m_SubType != SubTypeRoot)
    {
        const QMetaObject *metaobject = m_pItem->metaObject();
        int propIndex = metaobject->indexOfProperty(m_PropertyName.toLocal8Bit().data());

        if (propIndex >= 0)
        {
            QMetaProperty prop = metaobject->property(propIndex);
            QMetaEnum metaenum = prop.enumerator();
            QVariant propvalue = m_pItem->property(m_PropertyName.toLocal8Bit().data());

            quint32 propflags = propvalue.value<quint32>();
            quint32 flag = metaenum.keyToValue(m_flagName.toLocal8Bit().data());

            if (value.toBool())
            {
                if (flag)
                    propflags |= flag;
                else
                    propflags = flag;
            }
            else
                propflags &= ~flag;

            m_pItem->setProperty(m_PropertyName.toLocal8Bit().data(), propflags);
            emit dataChanged();
            emit parentItem()->dataChanged();
        }
    }
}

QVariant FlagPropertyTreeItem::data(const int &role) const
{
    const QMetaObject *metaobject = m_pItem->metaObject();
    int propIndex = metaobject->indexOfProperty(m_PropertyName.toLocal8Bit().data());

    if (propIndex < 0)
        return QVariant();

    QMetaProperty prop = metaobject->property(propIndex);
    QMetaEnum metaenum = prop.enumerator();
    QVariant value = m_pItem->property(m_PropertyName.toLocal8Bit().data());

    if (m_SubType != SubTypeRoot)
    {
        if (role == Qt::DisplayRole || role == Qt::EditRole)
        {
            QByteArray valueToKeys = metaenum.valueToKeys(value.toInt());

            if (valueToKeys.contains(m_flagName.toLocal8Bit()))
                return true;
            else
                return false;
        }
        else if (role == Qt::FontRole)
        {
            if (data(Qt::EditRole).toBool())
            {
                QFont f;
                f.setBold(true);
                return f;
            }
        }
    }
    else
    {
        if (role == Qt::DisplayRole)
        {
            QString name;
            FlagPropertyTreeItem *pThis = const_cast<FlagPropertyTreeItem*>(this);

            int count = childCount();
            for (int i = 0; i < count; i++)
            {
                PropertyTreeItem *item = pThis->child(i);
                if (item->data(Qt::EditRole).toBool())
                {
                    if (!name.isEmpty())
                        name += "|";

                    name += item->propertyAlias();
                }
            }

            return name;
        }
    }

    return PropertyTreeItem::data(role);
}

void FlagPropertyTreeItem::loadEnumAlias(const QJsonArray &array)
{
    for (const auto &element : array)
    {
        QJsonObject obj = element.toObject();
        QString key = element["key"].toString();
        QString alias = element["alias"].toString();

        m_KeyAlias.insert(key, alias);
        m_AliasKey.insert(alias, key);
    }
}

QWidget *FlagPropertyTreeItem::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QCheckBox *editor = new QCheckBox(parent);
    return editor;
}
