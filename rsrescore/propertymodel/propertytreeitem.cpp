#include "propertytreeitem.h"
#include "propertymodel.h"
#include "qcheckbox.h"
#include "qsize.h"
#include <QMetaObject>
#include <QMetaProperty>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QColor>
#include <QLineEdit>

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

QMetaMethod PropertyTreeItem::findMethod(const QString &name) const
{
    const QMetaObject *metaobject = metaObject();
    int size = metaobject->methodCount();

    for (int i = 0; i < size; i++)
    {
        QMetaMethod method = metaobject->method(i);
        if (name == method.name())
            return method;
    }
    return QMetaMethod();
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

QObject *PropertyTreeItem::object() const
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

QWidget *CreateStringEditor(QWidget *parent)
{
    QLineEdit *editor = new QLineEdit(parent);
    editor->setMaxLength(255);
    editor->setClearButtonEnabled(true);
    return editor;
}

QWidget *CreateBoolEditor(QWidget *parent)
{
    QCheckBox *editor = new QCheckBox(parent);
    return editor;
}

template<class T>
QWidget *CreateNumberEditor(QWidget *parent, const PropertyTreeItem::EditorPropMap &prop)
{
    QWidget *editor = nullptr;
    T Min = std::numeric_limits<T>::min();
    T Max = std::numeric_limits<T>::max();

    if (!std::is_floating_point<T>::value)
    {
        QSpinBox *pSpinBox = new QSpinBox(parent);

        if (prop.contains("min"))
            pSpinBox->setMinimum(prop["min"].toInt());
        else
            pSpinBox->setMinimum(Min);

        if (prop.contains("max"))
            pSpinBox->setMaximum(prop["max"].toInt());
        else
            pSpinBox->setMaximum(Max);

        editor = pSpinBox;
    }
    else
    {
        QDoubleSpinBox *pSpinBox = new QDoubleSpinBox(parent);
        pSpinBox->setMinimum(Min);
        pSpinBox->setMaximum(Max);
        editor = pSpinBox;
    }

    return editor;
}

QWidget *PropertyTreeItem::createDefaultEditor(QWidget *parent,
                                               const QStyleOptionViewItem &option,
                                               const QModelIndex &index,
                                               const QMetaType::Type &type) const
{
    QWidget *pEditor = nullptr;

    switch(type)
    {
    case QMetaType::Bool:
        pEditor = CreateBoolEditor(parent);
        break;
    case QMetaType::Int:
        pEditor = CreateNumberEditor<int>(parent, m_EditorProp);
        break;
    case QMetaType::UInt:
        pEditor = CreateNumberEditor<unsigned int>(parent, m_EditorProp);
        break;
    case QMetaType::Double:
        pEditor = CreateNumberEditor<double>(parent, m_EditorProp);
        break;
    case QMetaType::Long:
        pEditor = CreateNumberEditor<long>(parent, m_EditorProp);
        break;
    case QMetaType::LongLong:
        pEditor = CreateNumberEditor<long long>(parent, m_EditorProp);
        break;
    case QMetaType::Short:
        pEditor = CreateNumberEditor<short>(parent, m_EditorProp);
        break;
    case QMetaType::ULong:
        pEditor = CreateNumberEditor<unsigned long>(parent, m_EditorProp);
        break;
    case QMetaType::ULongLong:
        pEditor = CreateNumberEditor<unsigned long long>(parent, m_EditorProp);
        break;
    case QMetaType::UShort:
        pEditor = CreateNumberEditor<unsigned short>(parent, m_EditorProp);
        break;
    case QMetaType::Float:
        pEditor = CreateNumberEditor<float>(parent, m_EditorProp);
        break;
    case QMetaType::QString:
        pEditor = CreateStringEditor(parent);
        break;
    }

    return pEditor;
}

QWidget *PropertyTreeItem::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget *pEditor = nullptr;
    const QMetaObject *metaobject = m_pItem->metaObject();
    int propid = metaobject->indexOfProperty(m_PropertyName.toLocal8Bit().data());

    if (propid >= 0)
    {
        QMetaProperty prop = metaobject->property(propid);

        int type = prop.type();
        pEditor = createDefaultEditor(parent, option, index, (QMetaType::Type)type);
    }

    return pEditor;
}

bool PropertyTreeItem::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    return false;
}

bool PropertyTreeItem::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    return false;
}

QSize PropertyTreeItem::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize();
}

template<class T>void ReadEditorNumberProperty(PropertyTreeItem::EditorPropMap &prop, const QJsonObject &obj)
{
    T Min = std::numeric_limits<T>::min();
    T Max = std::numeric_limits<T>::max();

    if (obj.contains("min"))
    {
        T minValue = obj["min"].toVariant().value<T>();

        if (minValue < Min)
            minValue = Min;

        prop["min"] = QVariant::fromValue(minValue);
    }

    if (obj.contains("max"))
    {
        T maxValue = obj["max"].toVariant().value<T>();

        if (maxValue > Max)
            maxValue = Max;

        prop["max"] = QVariant::fromValue(maxValue);
    }
}

void PropertyTreeItem::initFromJsonBaseTypes(const QJsonObject &obj, const QMetaType::Type &type)
{
    switch(type)
    {
    case QMetaType::Int:
        ReadEditorNumberProperty<int>(m_EditorProp, obj);
        break;
    case QMetaType::UInt:
        ReadEditorNumberProperty<unsigned int>(m_EditorProp, obj);
        break;
    case QMetaType::Double:
        ReadEditorNumberProperty<double>(m_EditorProp, obj);
        break;
    case QMetaType::Long:
        ReadEditorNumberProperty<long>(m_EditorProp, obj);
        break;
    case QMetaType::LongLong:
        ReadEditorNumberProperty<long long>(m_EditorProp, obj);
        break;
    case QMetaType::Short:
        ReadEditorNumberProperty<short>(m_EditorProp, obj);
        break;
    case QMetaType::ULong:
        ReadEditorNumberProperty<unsigned long>(m_EditorProp, obj);
        break;
    case QMetaType::ULongLong:
        ReadEditorNumberProperty<unsigned long long>(m_EditorProp, obj);
        break;
    case QMetaType::UShort:
        ReadEditorNumberProperty<unsigned short>(m_EditorProp, obj);
        break;
    case QMetaType::Float:
        ReadEditorNumberProperty<float>(m_EditorProp, obj);
        break;
    }
}

void PropertyTreeItem::initFromJson(const QJsonObject &obj)
{
    const QMetaObject *metaobject = m_pItem->metaObject();
    int propid = metaobject->indexOfProperty(m_PropertyName.toLocal8Bit().data());

    if (propid >= 0)
    {
        QMetaProperty prop = metaobject->property(propid);

        int type = prop.type();
        initFromJsonBaseTypes(obj, (QMetaType::Type)type);
    }
}

PropertyTreeItem::PropertyTreeItemList::iterator PropertyTreeItem::begin()
{
    return m_childItems.begin();
}

PropertyTreeItem::PropertyTreeItemList::iterator PropertyTreeItem::end()
{
    return m_childItems.end();
}
