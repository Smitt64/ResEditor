#include "panelstructmodel.h"
#include <QVariant>
#include <QList>
#include <memory>
#include <QGraphicsItem>
#include <QMetaObject>
#include <QMetaClassInfo>
#include "customrectitem.h"
#include "textitem.h"
#include "panelitem.h"
#include "basescene.h"
#include "controlitem.h"
#include <QIcon>

class TreeItem
{
    enum
    {
        fld_Element = 0,
        fld_Class,
    };
public:
    TreeItem(CustomRectItem *item, TreeItem *parentItem = nullptr)
    {
        m_pItem = item;
        m_parentItem = parentItem;
    }

    void appendChild(std::unique_ptr<TreeItem> &&child)
    {
        m_childItems.push_back(std::move(child));
    }

    TreeItem *child(int row)
    {
        return row >= 0 && row < childCount() ? m_childItems.at(row).get() : nullptr;
    }

    int childCount() const
    {
        return int(m_childItems.size());
    }

    int columnCount() const
    {
        return 2;
    }
    QVariant data(int column, int role) const
    {
        if (role == Qt::DisplayRole)
        {
            const QMetaObject *obj = m_pItem->metaObject();

            if (column == fld_Class)
            {
                int index = obj->indexOfClassInfo(CLASSINFO_UNDOREDO);
                return obj->classInfo(index).value();
            }
            else
            {
                QPoint p = m_pItem->getPoint();
                QString text;
                QString pos = QString("(%1,%2) ")
                        .arg(p.x())
                        .arg(p.y());

                PanelItem *panelItem = qobject_cast<PanelItem*>(m_pItem);
                TextItem *textItem = qobject_cast<TextItem*>(m_pItem);
                ControlItem *controlItem = qobject_cast<ControlItem*>(m_pItem);

                if (textItem)
                {
                    text = pos + QString(": %1").arg(textItem->text());
                }
                else if(panelItem)
                {
                    text = pos + QString(": %1").arg(panelItem->title());
                }
                else if(controlItem)
                {
                    int DataTypeIndex = obj->indexOfEnumerator("DataType");
                    int FieldTypeIndex = obj->indexOfEnumerator("FieldType");

                    QMetaEnum DataTypeEnum = obj->enumerator(DataTypeIndex);
                    QMetaEnum FieldTypeEnum = obj->enumerator(FieldTypeIndex);

                    text = pos + QString(": %2, %1")
                            .arg(DataTypeEnum.valueToKey(controlItem->property("dataType").toInt()))
                            .arg(FieldTypeEnum.valueToKey(controlItem->property("fieldType").toInt()));
                }

                return text;
            }
        }
        else if (role == Qt::DecorationRole && column == fld_Element)
        {
            PanelItem *panelItem = qobject_cast<PanelItem*>(m_pItem);
            TextItem *textItem = qobject_cast<TextItem*>(m_pItem);
            ControlItem *controlItem = qobject_cast<ControlItem*>(m_pItem);

            if (textItem)
                return QIcon(":/img/Label_24x.png");
            else if(panelItem)
                return QIcon(":/img/Panel.png");
            else if (controlItem)
                return IconForDataType(controlItem->dataType());
        }
        return QVariant();
    }

    int row() const
    {
        if (m_parentItem == nullptr)
            return 0;

        const auto it = std::find_if(m_parentItem->m_childItems.cbegin(), m_parentItem->m_childItems.cend(),
                                     [this](const std::unique_ptr<TreeItem> &treeItem) {
            return treeItem.get() == this;
        });

        if (it != m_parentItem->m_childItems.cend())
            return std::distance(m_parentItem->m_childItems.cbegin(), it);

        Q_ASSERT(false); // should not happen
        return -1;
    }

    TreeItem *parentItem()
    {
        return m_parentItem;
    }

    std::vector<std::unique_ptr<TreeItem>> m_childItems;
    CustomRectItem *m_pItem;
    TreeItem *m_parentItem;
};

// -----------------------------------------------------------------

PanelStructModel::PanelStructModel(CustomRectItem *item, QObject *parent) :
    QAbstractItemModel(parent)
{
    rootItem.reset(new TreeItem(nullptr));

    rootItem->appendChild(std::make_unique<TreeItem>(item, rootItem.get()));

    connect(item, SIGNAL(structChanged()), this, SLOT(structChanged()));
}

PanelStructModel::~PanelStructModel() = default;

void PanelStructModel::structChanged()
{
    TreeItem *panel = rootItem->child(0);

    QModelIndex root = createIndex(0, 0, panel);

    beginResetModel();
    if (panel->childCount())
        panel->m_childItems.clear();

    BaseScene *pScene = qobject_cast<BaseScene*>(panel->m_pItem->scene());
    QList<QGraphicsItem*> elements = pScene->items();

    QList<CustomRectItem*> rects;
    for (auto element : qAsConst(elements))
    {
        CustomRectItem *rectItem = dynamic_cast<CustomRectItem*>(element);

        if (rectItem && rectItem != panel->m_pItem)
            rects.append(rectItem);
    }

    for (auto element : qAsConst(rects))
    {
        panel->appendChild(std::make_unique<TreeItem>(element, panel));
    }
    endResetModel();
}

int PanelStructModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();

    return rootItem->columnCount();
}

QVariant PanelStructModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || (role != Qt::DisplayRole && role != Qt::DecorationRole && role != CustomRectItemRole))
        return {};

    const auto *item = static_cast<const TreeItem*>(index.internalPointer());

    if (role == CustomRectItemRole)
        return QVariant::fromValue<CustomRectItem*>(item->m_pItem);

    return item->data(index.column(), role);
}

Qt::ItemFlags PanelStructModel::flags(const QModelIndex &index) const
{
    return index.isValid()
        ? QAbstractItemModel::flags(index) : Qt::ItemFlags(Qt::NoItemFlags);
}

QVariant PanelStructModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    static QStringList headers = {tr("Элемент"), tr("Тип")};

    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return headers[section];

    return QVariant();
}

QModelIndex PanelStructModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return {};

    TreeItem *parentItem = parent.isValid()
        ? static_cast<TreeItem*>(parent.internalPointer())
        : rootItem.get();

    if (auto *childItem = parentItem->child(row))
        return createIndex(row, column, childItem);

    return {};
}

QModelIndex PanelStructModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return {};

    auto *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parentItem();

    return parentItem != rootItem.get()
        ? createIndex(parentItem->row(), 0, parentItem) : QModelIndex{};
}

int PanelStructModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    const TreeItem *parentItem = parent.isValid()
        ? static_cast<const TreeItem*>(parent.internalPointer())
        : rootItem.get();

    return parentItem->childCount();
}
