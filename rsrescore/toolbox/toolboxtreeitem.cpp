#include "toolboxtreeitem.h"
#include "toolbox/toolboxmodel.h"
#include <QMetaObject>
#include <QMetaProperty>
#include <QColor>

ToolBoxTreeItem::ToolBoxTreeItem(const TypeItem &type, QObject *parent)
    : QObject{parent},
      m_TypeItem(type),
      m_parentItem(nullptr)
{
}

ToolBoxTreeItem::~ToolBoxTreeItem()
{
    qDeleteAll(m_childItems);
}

int ToolBoxTreeItem::childCount() const
{
    return m_childItems.size();
}

ToolBoxTreeItem *ToolBoxTreeItem::child(const int &row)
{
    if (row < 0 || row >= m_childItems.size())
        return nullptr;

    return m_childItems[row];
}

Qt::ItemFlags ToolBoxTreeItem::flags() const
{
    Qt::ItemFlags flag = Qt::ItemIsSelectable;
    /*ToolBoxTreeItem *pThis = const_cast<ToolBoxTreeItem*>(this);

    if (pThis->isEnabled())
    {
        flag |= Qt::ItemIsEnabled;

        if (pThis->isEditable())
            flag |= Qt::ItemIsEditable;
    }*/

    return flag;
}

QVariant ToolBoxTreeItem::data(const int &role) const
{

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        return m_Alias;
        /*if (m_TypeItem == TypeItem_Data && m_pItem)
            return m_pItem->property(m_PropertyName.toLocal8Bit().data());
        else
            return m_PropertyAlias;*/
    }
    else if (role == ToolBoxModel::MimeTypeRole)
        return m_MimeType;
    else if (role == ToolBoxModel::MimeDataRole)
        return m_MimeData;
    else if (role == Qt::DecorationRole)
        return m_Icon;
    else if (role == RoleTypeItem)
        return m_TypeItem;
    else if (role == Qt::SizeHintRole)
    {
        if (m_TypeItem == TypeItem_Data)
            return QSize(10, 24);
        else
            return QSize(10, 20);
    }

    return QVariant();
}

int ToolBoxTreeItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<ToolBoxTreeItem*>(this));

    return 0;
}

ToolBoxTreeItem *ToolBoxTreeItem::parentItem()
{
    return m_parentItem;
}

ToolBoxTreeItem *ToolBoxTreeItem::parentItem() const
{
    return m_parentItem;
}

void ToolBoxTreeItem::appendChild(ToolBoxTreeItem *item)
{
    item->m_parentItem = this;
    m_childItems.append(item);
}

QModelIndex ToolBoxTreeItem::index() const
{
    ToolBoxModel *model = qobject_cast<ToolBoxModel*>(parent());

    if (model)
        return model->index(this);

    return QModelIndex();
}

void ToolBoxTreeItem::setAlias(const QString &alias)
{
    m_Alias = alias;
}

void ToolBoxTreeItem::setIcon(const QIcon &icon)
{
    m_Icon = icon;
}

void ToolBoxTreeItem::setMimeData(const QString &mimetype, const QByteArray &data)
{
    m_MimeType = mimetype;
    m_MimeData = data;
}
