#include "containeritem.h"
#include "basescene.h"
#include "styles/resstyle.h"
#include <QPainter>
#include <QtMath>
#include <QDebug>
#include <QMimeData>
#include <QGraphicsSceneDragDropEvent>

ContainerItem::ContainerItem(CustomRectItem *parent) :
    CustomRectItem(QRect(0, 0, 50, 50), parent),
    m_BorderStyle(ResStyle::Border_DoubleLine)
{
    setAcceptDrops(true);
    setAvailableCorners(CustomRectItem::RIGHT | CustomRectItem::BOTTOM | CustomRectItem::BOTTOM_RIGHT);
}

ContainerItem::ContainerItem(const QRect& rect, CustomRectItem* parent):
    CustomRectItem(rect, parent)
{
    setAcceptDrops(true);
}

ContainerItem::~ContainerItem()
{

}

const ResStyle::BorderStyle &ContainerItem::borderStyle() const
{
    return m_BorderStyle;
}

void ContainerItem::setBorderStyle(const ResStyle::BorderStyle &style)
{
    if (isSkipUndoStack())
    {
        m_BorderStyle = style;
        emit borderStyleChanged();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("borderStyle", style);
}

bool ContainerItem::childCanMove(const QPointF &newPos, CustomRectItem *item)
{
    BaseScene* customScene = qobject_cast<BaseScene*> (scene());
    QSize gridSize = customScene->getGridSize();
    QRectF thisBound = boundingRect();
    if (newPos.x() < gridSize.width() || newPos.y() < gridSize.height())
        return false;

    qreal w = item->boundingRect().width();
    qreal h = item->boundingRect().height();
    if (newPos.x() + w > thisBound.width() - gridSize.width() || newPos.y() + h > thisBound.height() - gridSize.height())
        return false;

    return true;
}

void ContainerItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    CustomRectItem::paint(painter, option, widget);

    QRectF BoundingRect = boundingRect();
    painter->fillRect(BoundingRect, getBrush());

    ResStyleOption opt;
    opt.init(this);
    opt.borderStyle = ResStyle::Border_DoubleLine;
    style()->drawControl(ResStyle::Control_Panel, painter, &opt);
}

void ContainerItem::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    /*QStringList formats = event->mimeData()->formats();
    qDebug() << formats;
    event->accept();*/
}

void ContainerItem::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    /*QStringList formats = event->mimeData()->formats();
    if (formats.contains("application/x-toolbox-common-date"))
    {
        event->accept();
    }
    qDebug() << formats;*/
}

void ContainerItem::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    //event->accept();
}
