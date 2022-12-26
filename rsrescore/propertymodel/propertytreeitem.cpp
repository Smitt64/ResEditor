#include "propertytreeitem.h"
#include "propertymodel.h"
#include <QMetaObject>
#include <QMetaProperty>
#include <QColor>

PropertyTreeItem::PropertyTreeItem(QObject *object, QObject *parent)
    : QObject{parent},
      m_pItem(object),
      m_TypeItem(TypeItem_Data),
      m_ItemGroup(0),
      m_parentItem(nullptr),
      m_Enable(true)
{
}

PropertyTreeItem::~PropertyTreeItem()
{
    qDeleteAll(m_childItems);
}

const int &PropertyTreeItem::group() const
{
    return m_ItemGroup;
}

void PropertyTreeItem::ConnectNotify()
{
    if (!m_pItem)
        return;

    const QMetaObject *metaobject = m_pItem->metaObject();
    int propid = metaobject->indexOfProperty(m_PropertyName.toLocal8Bit().data());

    if (propid >= 0)
    {
        QMetaProperty prop = metaobject->property(propid);

        if (prop.hasNotifySignal())
        {
            QString notifySignal = QString("2") + QString::fromLocal8Bit(prop.notifySignal().methodSignature());
            connect(m_pItem, notifySignal.toLocal8Bit().data(), this, SIGNAL(dataChanged()));
        }
    }
}

int PropertyTreeItem::childCount() const
{
    return m_childItems.size();
}

PropertyTreeItem *PropertyTreeItem::child(const int &row)
{
    if (row < 0 || row >= m_childItems.size())
        return nullptr;

    return m_childItems[row];
}

Qt::ItemFlags PropertyTreeItem::flags() const
{
    Qt::ItemFlags flag = Qt::ItemIsSelectable;
    PropertyTreeItem *pThis = const_cast<PropertyTreeItem*>(this);

    if (pThis->isEnabled())
    {
        flag |= Qt::ItemIsEnabled;

        if (pThis->isEditable())
            flag |= Qt::ItemIsEditable;
    }

    return flag;
}

void PropertyTreeItem::setData(const QVariant &value)
{
    if (!m_pItem)
        return;

    m_pItem->setProperty(m_PropertyName.toLocal8Bit().data(), value);
}

QVariant PropertyTreeItem::data(const int &role) const
{
    static QVector<QColor> colors =
    {
        QColor(255, 230, 191),
        QColor(255, 255, 191),
        QColor(191, 255, 191),
        QColor(199, 255, 255),
        QColor(234, 191, 255),
        QColor(255, 191, 239)
    };

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (m_TypeItem == TypeItem_Data && m_pItem)
            return m_pItem->property(m_PropertyName.toLocal8Bit().data());
        else
            return m_PropertyAlias;
    }
    else if (role == Qt::BackgroundColorRole || role == Qt::BackgroundRole)
        return colors[m_ItemGroup];
    else if (role == RoleTypeItem)
    {
        return m_TypeItem;
    }
    else if (role == RoleGroup)
        return m_ItemGroup;

    return QVariant();
}

int PropertyTreeItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<PropertyTreeItem*>(this));

    return 0;
}

void PropertyTreeItem::setGroup(const int &group)
{
    m_ItemGroup = group;
}

PropertyTreeItem *PropertyTreeItem::parentItem()
{
    return m_parentItem;
}

PropertyTreeItem *PropertyTreeItem::parentItem() const
{
    return m_parentItem;
}

QString PropertyTreeItem::propertyName() const
{
    return m_PropertyName;
}

QObject *PropertyTreeItem::object()
{
    return m_pItem;
}

void PropertyTreeItem::setObject(QObject *object)
{
    m_pItem = object;
}

QString PropertyTreeItem::propertyAlias() const
{
    return m_PropertyAlias;
}

void PropertyTreeItem::appendChild(PropertyTreeItem *item)
{
    item->m_parentItem = this;
    m_childItems.append(item);
}

void PropertyTreeItem::setPropertyName(const QString &name)
{
    m_PropertyName = name;
    ConnectNotify();
}

void PropertyTreeItem::setPropertyAlias(const QString &name)
{
    m_PropertyAlias = name;
}

QModelIndex PropertyTreeItem::index() const
{
    PropertyModel *model = qobject_cast<PropertyModel*>(parent());

    if (model)
        return model->index(this);

    return QModelIndex();
}

bool PropertyTreeItem::isEditable()
{
    if (!m_pItem)
        return false;

    if (m_isEditable.isNull())
    {
        const QMetaObject *metaobject = m_pItem->metaObject();
        int propid = metaobject->indexOfProperty(m_PropertyName.toLocal8Bit().data());

        if (propid >= 0)
        {
            QMetaProperty prop = metaobject->property(propid);
            m_isEditable = prop.isWritable();
        }
    }

    return m_isEditable.toBool();
}

bool PropertyTreeItem::isEnabled() const
{
    return m_Enable;
}

void PropertyTreeItem::setEnabled(const bool &v)
{
    m_Enable = v;
}

QWidget *PropertyTreeItem::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return nullptr;
}

bool PropertyTreeItem::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    return false;
}

bool PropertyTreeItem::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    return false;
}

void PropertyTreeItem::initFromJson(const QJsonObject &obj)
{

}

PropertyTreeItem::PropertyTreeItemList::iterator PropertyTreeItem::begin()
{
    return m_childItems.begin();
}

PropertyTreeItem::PropertyTreeItemList::iterator PropertyTreeItem::end()
{
    return m_childItems.end();
}
