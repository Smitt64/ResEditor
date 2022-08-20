#include "propertytreedelegate.h"
#include "propertytreeitem.h"
#include <QPainter>

PropertyTreeDelegate::PropertyTreeDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{

}

PropertyTreeDelegate::~PropertyTreeDelegate()
{

}

void PropertyTreeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    int type = index.data(PropertyTreeItem::RoleTypeItem).value<PropertyTreeItem::TypeItem>();
    if (type != PropertyTreeItem::TypeItem_Group)
    {
        QRect rect = option.rect;
        painter->save();
        QPen pen(Qt::lightGray);
        pen.setStyle(Qt::SolidLine);
        painter->setPen(pen);
        painter->drawLine(QLine(rect.right(), rect.top(), rect.right(), rect.bottom()));
        painter->drawLine(QLine(rect.left(), rect.bottom(), rect.right(), rect.bottom()));
        painter->restore();
    }
}

QWidget *PropertyTreeDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget *editor = nullptr;
    PropertyTreeItem *item = static_cast<PropertyTreeItem*>(index.internalPointer());

    if (item)
        editor = item->createEditor(parent, option, index);

    if (!editor)
        editor = QStyledItemDelegate::createEditor(parent, option, index);

    return editor;
}

void PropertyTreeDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    PropertyTreeItem *item = static_cast<PropertyTreeItem*>(index.internalPointer());

    if (!item || !editor)
        QStyledItemDelegate::setEditorData(editor, index);

    bool hr = item->setEditorData(editor, index);
    if (!hr)
        QStyledItemDelegate::setEditorData(editor, index);
}

void PropertyTreeDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    PropertyTreeItem *item = static_cast<PropertyTreeItem*>(index.internalPointer());

    if (!item || !editor)
        QStyledItemDelegate::setModelData(editor, model, index);

    bool hr = item->setModelData(editor, model, index);
    if (!hr)
        QStyledItemDelegate::setModelData(editor, model, index);
}
