#include "containeritem.h"
#include "basescene.h"
#include "styles/resstyle.h"
#include <QPainter>
#include <QtMath>
#include <QDebug>
#include <QMimeData>
#include <QGraphicsSceneDragDropEvent>

ContainerItem::ContainerItem(QGraphicsItem *parent) :
    CustomRectItem(QRect(0, 0, 50, 50), parent),
    m_BorderStyle(ResStyle::Border_DoubleLine)
{
    setAcceptDrops(true);
    setZValue(0.0);
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

bool ContainerItem::isIntersects(const QRectF &thisBound, QGraphicsItem *item, const QRectF &itemBound) const
{
    return false;
}

void ContainerItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    CustomRectItem::paint(painter, option, widget);

    QRectF BoundingRect = boundingRect();
    painter->fillRect(BoundingRect, Qt::transparent);
    //painter->fillRect(BoundingRect, getBrush());

    ResStyleOption opt;
    opt.init(this);
    opt.borderStyle = m_BorderStyle;
    style()->drawControl(ResStyle::Control_Frame, painter, &opt);

    painter->save();
    QColor br = style()->color(ResStyle::Color_TextBg, &opt);
    QColor inverceColor = QColor::fromRgb(255 - br.red(), 255 - br.green(), 255 - br.blue());
    paintBevel(painter, inverceColor);
    painter->restore();
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

// =====================================================================

BorderItem::BorderItem(QGraphicsItem *parent) :
    ContainerItem(parent)
{

}

BorderItem::BorderItem(const QRect& rect, CustomRectItem* parent) :
    ContainerItem(rect, parent)
{

}

BorderItem::~BorderItem()
{

}

void BorderItem::setBorderParams(const QRect &rect, const qint16 &St)
{
    setCoord(rect.topLeft());
    setSize(rect.size());
    m_BorderStyle = (ResStyle::BorderStyle)St;
}
