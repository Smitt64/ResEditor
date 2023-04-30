#include "propertytreedelegate.h"
#include "propertytreeitem.h"
#include <QTreeView>
#include <QPainter>
#include <QHeaderView>

PropertyTreeDelegate::PropertyTreeDelegate(QObject *parent) :
    QStyledItemDelegate(parent),
    m_IsEditorOpened(false)
{

}

PropertyTreeDelegate::~PropertyTreeDelegate()
{

}

void PropertyTreeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    PropertyTreeDelegate *pThis = const_cast<PropertyTreeDelegate*>(this);

    if (pThis->m_IsEditorOpened)
        opt.text = QString();

    QStyledItemDelegate::paint(painter, opt, index);

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

void PropertyTreeDelegate::dataChanged(const QModelIndex &index, const QVector<int> &roles)
{
    QAbstractItemModel *model = const_cast<QAbstractItemModel*>(index.model());
    emit model->dataChanged(index, index, roles);
}

QWidget *PropertyTreeDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget *editor = nullptr;
    PropertyTreeItem *item = static_cast<PropertyTreeItem*>(index.internalPointer());
    PropertyTreeDelegate *pThis = const_cast<PropertyTreeDelegate*>(this);

    if (item)
    {
        pThis->m_IsEditorOpened = true;
        pThis->dataChanged(index, QVector<int>{Qt::SizeHintRole});
        editor = item->createEditor(parent, option, index);
    }

    if (!editor)
        editor = QStyledItemDelegate::createEditor(parent, option, index);

    return editor;
}

void PropertyTreeDelegate::destroyEditor(QWidget *editor, const QModelIndex &index) const
{
    PropertyTreeDelegate *pThis = const_cast<PropertyTreeDelegate*>(this);

    pThis->m_IsEditorOpened = false;
    QStyledItemDelegate::destroyEditor(editor, index);
    pThis->dataChanged(index, QVector<int>{Qt::SizeHintRole});
}

void PropertyTreeDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    PropertyTreeItem *item = static_cast<PropertyTreeItem*>(index.internalPointer());

    if (!item || !editor)
        QStyledItemDelegate::setEditorData(editor, index);

    bool hr = item->setEditorData(editor, index);

    if (item && item->sizeHint(QStyleOptionViewItem(), index).isValid())
        editor->adjustSize();

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

QSize PropertyTreeDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    PropertyTreeDelegate *pThis = const_cast<PropertyTreeDelegate*>(this);
    PropertyTreeItem *item = static_cast<PropertyTreeItem*>(index.internalPointer());

    QSize sz = QStyledItemDelegate::sizeHint(option, index);
    if (item && pThis->m_IsEditorOpened && index.column() == 1)
    {
        QTreeView *view = qobject_cast<QTreeView*>(parent());
        int sectionSize = view->header()->sectionSize(index.column());

        if (!view)
            return sz;

        QSize cell(sectionSize, sz.height());
        QSize sz = item->sizeHint(option, index);

        if (sz.isValid())
            return QSize(cell.width(), sz.height());
        /*QVariant value = index.data(Qt::SizeHintRole);
        QSize sz = item->sizeHint(option, index);
        QSize real = !sz.isValid() ? value.toSize() : sz;
        qDebug() << "sizeHint" << sz << real << option.rect;
        return real;*/
        return cell;
    }

    return sz;
}
