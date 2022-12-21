#include "propertytreeview.h"
#include "propertygrouptreeitem.h"
#include "propertytreedelegate.h"
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QDebug>
#include <QHeaderView>
#include <QStyleOptionHeader>
#include <QFile>

PropertyTreeView::PropertyTreeView(QWidget *parent) :
    QTreeView(parent)
{
    setIndentation(10);
    setUniformRowHeights(true);

    m_pDelegate = new PropertyTreeDelegate(this);
    setItemDelegate(m_pDelegate);
}

PropertyTreeView::~PropertyTreeView()
{
    if (m_pDelegate)
        delete m_pDelegate;
}

void PropertyTreeView::drawRow(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    int type = index.data(PropertyTreeItem::RoleTypeItem).value<PropertyTreeItem::TypeItem>();

    if (type == PropertyTreeItem::TypeItem_Group)
    {
        if (selectionModel()->isSelected(index))
            opt.state |= QStyle::State_Selected;

        int firstSection = header()->logicalIndex(0);
        int lastSection = header()->logicalIndex(header()->count() - 1);
        int left = header()->sectionViewportPosition(firstSection);
        int right = header()->sectionViewportPosition(lastSection) + header()->sectionSize(lastSection);

        opt.rect.setX(left);
        opt.rect.setWidth(right - left);
        opt.backgroundBrush = Qt::darkGray;

        painter->fillRect(opt.rect, opt.backgroundBrush);
        itemDelegate(index)->paint(painter, opt, index);
    }
    else
        QTreeView::drawRow(painter, option, index);
}

void PropertyTreeView::drawBranches(QPainter *painter, const QRect &rect, const QModelIndex &index) const
{
    QBrush brush = index.data(Qt::BackgroundRole).value<QColor>();

    painter->fillRect(rect, brush);

    painter->save();
    QPen pen(Qt::lightGray);
    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);
    painter->drawLine(QLine(rect.left(), rect.bottom(), rect.right(), rect.bottom()));
    painter->restore();
    QTreeView::drawBranches(painter, rect, index);
}
