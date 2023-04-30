#include "ctrltabordertreeitem.h"
#include "controlitem.h"
#include "controtaborder.h"
#include "qmetaobject.h"
#include "widgets/controltaborderedit.h"

CtrlTabOrderTreeItem::CtrlTabOrderTreeItem(QObject *rectItem, QObject *parent)
    : PropertyTreeItem{rectItem, parent},
      m_pTabOrder(nullptr)
{

}

CtrlTabOrderTreeItem::~CtrlTabOrderTreeItem()
{
    if (m_pTabOrder)
        delete m_pTabOrder;
}

/*Qt::ItemFlags CtrlTabOrderTreeItem::flags() const
{
    Qt::ItemFlags flag = PropertyTreeItem::flags();
    flag &= ~Qt::ItemIsEditable;
    return flag;
}*/

void CtrlTabOrderTreeItem::setPropertyName(const QString &name)
{
    PropertyTreeItem::setPropertyName(name);

    const QMetaObject *metaobject = m_pItem->metaObject();
    int propIndex = metaobject->indexOfProperty(m_PropertyName.toLocal8Bit().data());

    if (propIndex >= 0)
    {
        QMetaProperty prop = metaobject->property(propIndex);

        if (prop.userType() == QMetaType::type("ControTabOrder"))
        {
            QVariant variantvalue = m_pItem->property(m_PropertyName.toLocal8Bit());
            ControTabOrder value = variantvalue.value<ControTabOrder>();

            m_pTabOrder = new ControTabOrder();
            m_pTabOrder->fromOther(value);

            connect(m_pTabOrder, &ControTabOrder::bottomChanged, this, &CtrlTabOrderTreeItem::propertyChanged);
            connect(m_pTabOrder, &ControTabOrder::thisidChanged, this, &CtrlTabOrderTreeItem::propertyChanged);
            connect(m_pTabOrder, &ControTabOrder::nextChanged, this, &CtrlTabOrderTreeItem::propertyChanged);
            connect(m_pTabOrder, &ControTabOrder::previousChanged, this, &CtrlTabOrderTreeItem::propertyChanged);
            connect(m_pTabOrder, &ControTabOrder::upChanged, this, &CtrlTabOrderTreeItem::propertyChanged);

            QMetaMethod updateSlot = findMethod("itemTabOrderChanged");
            connect(m_pItem, prop.notifySignal(), this, updateSlot);
        }
    }
}

void CtrlTabOrderTreeItem::itemTabOrderChanged()
{
    ControlItem *item = qobject_cast<ControlItem*>(sender());

    if (!item)
        return;

    m_pTabOrder->fromOther(item->tabOrder());
    emit dataChanged();
}

QVariant CtrlTabOrderTreeItem::data(const int &role) const
{
    if (role == Qt::EditRole || role == Qt::DisplayRole)
    {
        return QString("№ %1, [🡅: %2, 🡄: %3, 🡆: %4, 🡇: %5]")
                .arg(m_pTabOrder->thisid())
                .arg(m_pTabOrder->up())
                .arg(m_pTabOrder->previous())
                .arg(m_pTabOrder->next())
                .arg(m_pTabOrder->bottom());
    }

    return PropertyTreeItem::data(role);
}

QWidget *CtrlTabOrderTreeItem::createEditor(QWidget *wparent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    ControlTabOrderEdit *pEdit = new ControlTabOrderEdit(wparent);
    return pEdit;
}

bool CtrlTabOrderTreeItem::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    ControlTabOrderEdit *tabOrder = qobject_cast<ControlTabOrderEdit*>(editor);

    if (!tabOrder)
        return false;

    tabOrder->setValue(m_pTabOrder);

    return true;
}

bool CtrlTabOrderTreeItem::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    ControlTabOrderEdit *tabOrder = qobject_cast<ControlTabOrderEdit*>(editor);

    if (!tabOrder)
        return false;

    tabOrder->getValue(m_pTabOrder);
    return true;
}

QSize CtrlTabOrderTreeItem::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(134, 80);
}

void CtrlTabOrderTreeItem::propertyChanged()
{
    m_pItem->setProperty(m_PropertyName.toLocal8Bit(), QVariant::fromValue(*m_pTabOrder));
}
