#include "toolboxmodel.h"
#include "toolbox/toolboxtreeitem.h"
#include <QSize>
#include <QMimeData>

ToolBoxModel::ToolBoxModel(QObject *parent)
    : QAbstractItemModel{parent}
{
    rootItem = new ToolBoxTreeItem(ToolBoxTreeItem::TypeItem_Group, this);
    //setupModelData(item, rootItem);
}

ToolBoxModel::~ToolBoxModel()
{
    delete rootItem;
}

bool ToolBoxModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == ToolBoxModel::DragIconRole && index.isValid())
    {
        ToolBoxTreeItem *item = static_cast<ToolBoxTreeItem*>(index.internalPointer());
        if (item)
        {
            item->setDragIcon(value.value<QIcon>());
            return true;
        }
    }

    return QAbstractItemModel::setData(index, value, role);
}

QVariant ToolBoxModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    ToolBoxTreeItem *item = static_cast<ToolBoxTreeItem*>(index.internalPointer());
    if (role == Qt::DisplayRole || role == Qt::DecorationRole ||
            role == ToolBoxModel::MimeTypeRole ||
            role == ToolBoxModel::MimeDataRole ||
            role == ToolBoxModel::DragIconRole ||
            role == Qt::SizeHintRole)
    {
        return item->data(role);
    }
    else if (role == Qt::BackgroundColorRole ||
             role == Qt::BackgroundRole ||
             role == ToolBoxTreeItem::RoleTypeItem ||
             role == ToolBoxTreeItem::RoleGroup ||
             role == Qt::ForegroundRole)
        return item->data(role);

    return QVariant();
}

Qt::ItemFlags ToolBoxModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    if (index.column() == ToolBoxModel::Column_Value)
    {
        ToolBoxTreeItem *item = static_cast<ToolBoxTreeItem*>(index.internalPointer());
        return item->flags();
    }

    return QAbstractItemModel::flags(index);
}

QVariant ToolBoxModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    static const QVector<QString> headers = { tr("Элемент") };

    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return headers[section];

    return QVariant();
}

QModelIndex ToolBoxModel::index(const ToolBoxTreeItem *item) const
{
    ToolBoxTreeItem *parentItem = item->parentItem();
    return createIndex(item->row(), ToolBoxModel::Column_Value, parentItem);
}

QModelIndex ToolBoxModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    ToolBoxTreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<ToolBoxTreeItem*>(parent.internalPointer());

    ToolBoxTreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);

    return QModelIndex();
}

QModelIndex ToolBoxModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    ToolBoxTreeItem *childItem = static_cast<ToolBoxTreeItem*>(index.internalPointer());
    ToolBoxTreeItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    if (!parentItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int ToolBoxModel::rowCount(const QModelIndex &parent) const
{
    ToolBoxTreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<ToolBoxTreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int ToolBoxModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}


void ToolBoxModel::appendItem(ToolBoxTreeItem *item)
{
    rootItem->appendChild(item);
}

void ToolBoxModel::addCategory(const QString &name)
{
    // Пользовательские toolbox-файлы могут ссылаться на уже
    // существующую группу — дубликат не создаём
    if (m_Groups.contains(name))
        return;

    ToolBoxTreeItem *item = new ToolBoxTreeItem(ToolBoxTreeItem::TypeItem_Group);
    item->setAlias(name);
    m_Groups[name] = item;
    appendItem(item);
}

void ToolBoxModel::addItem(const QStringList &category, const QString &name, QMimeData *data, const QIcon &icon)
{
    for (const QString &categ_ : category)
    {
        // Пользовательский toolbox может сослаться на несуществующую
        // группу — пропускаем, а не падаем на nullptr
        ToolBoxTreeItem *group = m_Groups.value(categ_);
        if (!group)
        {
            qWarning("ToolBoxModel: группа \"%s\" не найдена, элемент \"%s\" пропущен",
                     qPrintable(categ_), qPrintable(name));
            continue;
        }

        QString mimetype = data->formats().first();
        ToolBoxTreeItem *item = new ToolBoxTreeItem(ToolBoxTreeItem::TypeItem_Data);
        item->setAlias(name);
        item->setIcon(icon);
        item->setMimeData(mimetype, data->data(mimetype));

        group->appendChild(item);
    }
}

ToolBoxTreeItem *ToolBoxModel::category(const QString &name)
{
    if (!m_Groups.contains(name))
        return nullptr;

    return m_Groups[name];
}
