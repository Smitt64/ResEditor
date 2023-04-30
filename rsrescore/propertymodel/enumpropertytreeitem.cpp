#include "enumpropertytreeitem.h"
#include "customrectitem.h"
#include "enumlistmodel.h"
#include <QMetaObject>
#include <QMetaProperty>
#include <QMetaEnum>
#include <QComboBox>
#include <QMapIterator>

EnumPropertyTreeItem::EnumPropertyTreeItem(CustomRectItem *rectItem, QObject *parent)
    : PropertyTreeItem{rectItem, parent}
{
    m_pModel = new EnumListModel(this);
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
            return m_pModel->valueToAlias(value.toInt());
        }
    }

    return PropertyTreeItem::data(role);
}

void EnumPropertyTreeItem::loadEnumAlias(const QJsonArray &array)
{
    m_pModel->loadFromJsonArray(array);
}

void EnumPropertyTreeItem::loadEnumAlias(const QString &filename)
{
    m_pModel->loadFromJsonFile(filename);
}

void EnumPropertyTreeItem::setPropertyName(const QString &name)
{
    PropertyTreeItem::setPropertyName(name);

    const QMetaObject *metaobject = m_pItem->metaObject();
    int propIndex = metaobject->indexOfProperty(m_PropertyName.toLocal8Bit().data());

    if (propIndex < 0)
        return;

    QMetaProperty prop = metaobject->property(propIndex);
    QMetaEnum metaenum = prop.enumerator();

    m_pModel->setMetaEnum(metaenum);
}

QWidget *EnumPropertyTreeItem::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QComboBox *combo = new QComboBox(parent);
    combo->setModel(m_pModel);

    return combo;
}

bool EnumPropertyTreeItem::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *combo = qobject_cast<QComboBox*>(editor);

    if (!combo)
        return false;

    QVariant varData = index.data(Qt::EditRole);
    int value = varData.toInt();
    combo->setCurrentIndex(m_pModel->indexFromValue(value));

    return true;
}

bool EnumPropertyTreeItem::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *combo = qobject_cast<QComboBox*>(editor);

    if (!combo)
        return false;

    int keyValue = m_pModel->valueFromIndex(combo->currentIndex());
    model->setData(index, keyValue);

    return true;
}
