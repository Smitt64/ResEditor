#include "propertymodel.h"
#include "propertymodel/propertytreeitem.h"
#include "flagpropertytreeitem.h"
#include "propertygrouptreeitem.h"
#include "customrectitem.h"
#include "rectpropertytreeitem.h"
#include "propertygrouptreeitem.h"
#include "enumpropertytreeitem.h"
#include <algorithm>
#include <QGraphicsItem>
#include <QMetaObject>
#include <QMetaClassInfo>
#include <QMetaProperty>
#include <QMetaType>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "ewtextstylepropertytreeitem.h"

PropertyModel::PropertyModel(CustomRectItem *item, QObject *parent)
    : QAbstractItemModel{parent},
      m_pItem(item)
{
    rootItem = new PropertyTreeItem(item, this);
    setupModelData(item, rootItem);
}

PropertyModel::~PropertyModel()
{
    delete rootItem;
}

bool PropertyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole || index.column() != Column_Value)
        return QAbstractItemModel::setData(index, value, role);

    PropertyTreeItem *item = static_cast<PropertyTreeItem*>(index.internalPointer());
    if (item)
        item->setData(value);

    return true;
}

QVariant PropertyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    PropertyTreeItem *item = static_cast<PropertyTreeItem*>(index.internalPointer());
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (index.column() == Column_Property)
            return item->propertyAlias();

        return item->data(role);
    }
    else if (role == Qt::SizeHintRole)
    {
        return QSize(10, 20);

    }
    else if (role == Qt::BackgroundColorRole ||
             role == Qt::BackgroundRole ||
             role == PropertyTreeItem::RoleTypeItem ||
             role == PropertyTreeItem::RoleGroup ||
             role == Qt::ForegroundRole)
    {
        EwTextStylePropertyTreeItem *tmp = qobject_cast<EwTextStylePropertyTreeItem*>(item);

        if (tmp)
        {
            QVariant tmpcolor = tmp->data(Qt::BackgroundRole);

            if (!tmpcolor.isValid())
                tmpcolor = tmp->data(Qt::BackgroundRole);
        }

        return item->data(role);
    }

    return QVariant();
}

Qt::ItemFlags PropertyModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    if (index.column() == PropertyModel::Column_Value)
    {
        PropertyTreeItem *item = static_cast<PropertyTreeItem*>(index.internalPointer());
        return item->flags();
    }

    return QAbstractItemModel::flags(index);
}

QVariant PropertyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    static const QVector<QString> headers = { tr("Свойство"), tr("Значение") };

    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return headers[section];

    return QVariant();
}

QModelIndex PropertyModel::index(const PropertyTreeItem *item) const
{
    PropertyTreeItem *parentItem = item->parentItem();
    return createIndex(item->row(), PropertyModel::Column_Value, parentItem);
}

QModelIndex PropertyModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    PropertyTreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<PropertyTreeItem*>(parent.internalPointer());

    PropertyTreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);

    return QModelIndex();
}

QModelIndex PropertyModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    PropertyTreeItem *childItem = static_cast<PropertyTreeItem*>(index.internalPointer());
    PropertyTreeItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    if (!parentItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int PropertyModel::rowCount(const QModelIndex &parent) const
{
    PropertyTreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<PropertyTreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int PropertyModel::columnCount(const QModelIndex &parent) const
{
    return 2;
}

QString PropertyModel::getPropertyListJson(const QMetaObject *obj)
{
    return getClassInfo(obj, CLASSINFO_PROPERTYLIST);
}

QString PropertyModel::getClassInfo(const QMetaObject *obj, const char *name)
{
    int index = obj->indexOfClassInfo(name);

    if (index >= 0)
        return obj->classInfo(index).value();

    return QString();
}

void PropertyModel::setupModelData(CustomRectItem *item, PropertyTreeItem *parent)
{
    QList<const QMetaObject*> metaobjects;
    const QMetaObject *obj = item->metaObject();

    do
    {
        metaobjects.append(obj);
        obj = obj->superClass();
    } while(obj);

    std::reverse(metaobjects.begin(), metaobjects.end());

    int Group = 0;
    QSet<QString> processed;
    for (auto metaobject : metaobjects)
    {
        QString json_name = getPropertyListJson(metaobject);

        QFile f(json_name);
        if (!json_name.isEmpty() && !processed.contains(json_name) && f.open(QIODevice::ReadOnly))
        {
            PropertyGroupTreeItem *groupItem = new PropertyGroupTreeItem(getClassInfo(metaobject, CLASSINFO_PROPERTYGROUP));
            rootItem->appendChild(groupItem);

            QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
            QJsonObject root = doc.object();
            QJsonArray propertylist = root["propertylist"].toArray();

            for (const auto &prop : qAsConst(propertylist))
                addProperty(prop.toObject(), metaobject, Group);

            processed.insert(json_name);
            Group++;
        }
    }
}

void PropertyModel::appendItem(PropertyTreeItem *item)
{
    connect(item, &PropertyTreeItem::dataChanged, [=]()
    {
        QModelIndex index = item->index();
        emit this->dataChanged(index, QModelIndex(), { Qt::DisplayRole });
    });
    rootItem->appendChild(item);
}

void PropertyModel::addProperty(const QJsonObject &obj, const QMetaObject *meta, const int &group)
{
    QString name = obj["name"].toString();
    QString alias = obj["alias"].toString();
    QString viewClass = obj["viewclass"].toString();
    int propertyId = meta->indexOfProperty(name.toLocal8Bit().data());

    if (propertyId >= 0)
    {
        QMetaProperty prop = meta->property(propertyId);

        PropertyTreeItem *itemBase = nullptr;
        if (!viewClass.isEmpty())
        {
            viewClass += "*";
            int id = QMetaType::type(viewClass.toLocal8Bit());

            if (id != QMetaType::UnknownType)
            {
                const QMetaObject *viewMetaObject = QMetaType::metaObjectForType(id);
                QObject *pInstance = viewMetaObject->newInstance(Q_ARG(QObject*, m_pItem), Q_ARG(QObject*, this));
                PropertyTreeItem *pTreeItem = qobject_cast<PropertyTreeItem*>(pInstance);

                if (pTreeItem)
                {
                    pTreeItem->initFromJson(obj);
                    pTreeItem->setPropertyAlias(alias);
                    pTreeItem->setGroup(group);
                    pTreeItem->setPropertyName(name);
                    itemBase = pTreeItem;
                }
            }
        }
        else if (prop.type() == QVariant::Rect)
        {
            RectPropertyTreeItem *item = new RectPropertyTreeItem(m_pItem, RectPropertyTreeItem::SubTypeRootRect, this);
            item->setPropertyName(name);
            item->setPropertyAlias(alias);
            item->setGroup(group);

            itemBase = item;
        }
        else
        {
            if (prop.isFlagType())
            {
                FlagPropertyTreeItem *item = new FlagPropertyTreeItem(m_pItem, FlagPropertyTreeItem::SubTypeRoot, this);
                item->setGroup(group);
                item->setPropertyName(name);
                item->setPropertyAlias(alias);

                if (obj.contains("enum"))
                    item->loadEnumAlias(obj["enum"].toArray());

                itemBase = item;
            }
            else if (prop.isEnumType())
            {
                EnumPropertyTreeItem *item = new EnumPropertyTreeItem(m_pItem, this);
                item->setPropertyName(name);
                item->setPropertyAlias(alias);
                item->setGroup(group);

                if (obj.contains("enum"))
                {
                    if (obj["enum"].isArray())
                        item->loadEnumAlias(obj["enum"].toArray());
                    else
                        item->loadEnumAlias(obj["enum"].toString());
                }

                itemBase = item;
            }
            else
            {
                PropertyTreeItem *item = new PropertyTreeItem(m_pItem, this);
                item->setPropertyName(name);
                item->setPropertyAlias(alias);
                item->setGroup(group);
                itemBase = item;
            }
        }

        if (obj.contains("enable"))
            itemBase->setEnabled(obj["enable"].toBool());

        if (itemBase)
            appendItem(itemBase);
    }
}
