#include "enumpropertytreeitem.h"
#include "customrectitem.h"
#include <QMetaObject>
#include <QMetaProperty>
#include <QMetaEnum>
#include <QComboBox>
#include <QMapIterator>

EnumPropertyTreeItem::EnumPropertyTreeItem(CustomRectItem *rectItem, QObject *parent)
    : PropertyTreeItem{rectItem, parent}
{

}

EnumPropertyTreeItem::~EnumPropertyTreeItem()
{

}

QVariant EnumPropertyTreeItem::data(const int &role) const
{
    if (role == Qt::DisplayRole)
    {
        const QMetaObject *metaobject = m_pItem->metaObject();
        int propIndex = metaobject->indexOfProperty(m_PropertyName.toLocal8Bit().data());

        if (propIndex < 0)
            return QVariant();
        else
        {
            QVariant value = m_pItem->property(m_PropertyName.toLocal8Bit().data());

            QMetaProperty prop = metaobject->property(propIndex);
            QMetaEnum metaenum = prop.enumerator();

            const char *key = metaenum.valueToKey(value.toInt());

            if (!key)
                return value;

            if (m_KeyAlias.empty())
                return key;

            return m_KeyAlias[key];
        }
    }

    return PropertyTreeItem::data(role);
}

void EnumPropertyTreeItem::loadEnumAlias(const QJsonArray &array)
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

QWidget *EnumPropertyTreeItem::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const QMetaObject *metaobject = m_pItem->metaObject();
    int propIndex = metaobject->indexOfProperty(m_PropertyName.toLocal8Bit().data());

    if (propIndex < 0)
        return nullptr;

    QComboBox *combo = new QComboBox(parent);
    QMetaProperty prop = metaobject->property(propIndex);
    QMetaEnum metaenum = prop.enumerator();

    if (m_KeyAlias.empty())
    {
        for (int i = 0; i < metaenum.keyCount(); i++)
            combo->addItem(metaenum.key(i), metaenum.key(i));
    }
    else
    {
        for (int i = 0; i < metaenum.keyCount(); i++)
        {
            const char *key = metaenum.key(i);

            if (m_KeyAlias.contains(key))
                combo->addItem(m_KeyAlias[key], key);
        }
    }

    return combo;
}

bool EnumPropertyTreeItem::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *combo = qobject_cast<QComboBox*>(editor);

    if (!combo)
        return false;

    const QMetaObject *metaobject = m_pItem->metaObject();
    int propIndex = metaobject->indexOfProperty(m_PropertyName.toLocal8Bit().data());

    if (propIndex < 0)
        return false;

    QMetaProperty prop = metaobject->property(propIndex);
    QMetaEnum metaenum = prop.enumerator();

    QVariant varData = index.data(Qt::EditRole);
    int value = varData.toInt();
    const char *key = metaenum.valueToKey(value);

    for (int i = 0; i < metaenum.keyCount(); i++)
    {
        if (!strcmp(metaenum.key(i), key))
            combo->setCurrentIndex(i);
    }

    return true;
}

bool EnumPropertyTreeItem::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *combo = qobject_cast<QComboBox*>(editor);

    if (!combo)
        return false;

    const QMetaObject *metaobject = m_pItem->metaObject();
    int propIndex = metaobject->indexOfProperty(m_PropertyName.toLocal8Bit().data());

    if (propIndex < 0)
        return false;

    QMetaProperty prop = metaobject->property(propIndex);
    QMetaEnum metaenum = prop.enumerator();

    QVariant userData = combo->currentData();
    QString key = userData.toString();
    int keyValue = metaenum.keyToValue(key.toLocal8Bit().data());
    model->setData(index, keyValue);

    return true;
}
