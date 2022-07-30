#include "containeritem.h"
#include "basescene.h"
#include <QPainter>
#include <QtMath>
#include <QDebug>
#include <QMimeData>
#include <QGraphicsSceneDragDropEvent>

ContainerItem::ContainerItem(CustomRectItem *parent) :
    CustomRectItem(QRect(0, 0, 50, 50), parent)
{
    setAcceptDrops(true);
}

ContainerItem::ContainerItem(const QRect& rect, CustomRectItem* parent):
    CustomRectItem(rect, parent)
{
    setAcceptDrops(true);
}

ContainerItem::~ContainerItem()
{

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
    QRectF BoundingRect = boundingRect();
    painter->fillRect(BoundingRect, getBrush());

    /*QFont font("Terminal", 16);
    QFont font2("Terminal", 12);
    QFontMetrics metrics(font);
    painter->setPen(Qt::white);
    painter->setFont(font);

    QChar chtopleft = QChar(9556); // top left
    QChar chtopright = QChar(9559);
    QChar horline = QChar(9552);
    QChar verline = QChar(9553);
    QChar chbotleft = QChar(9562);
    QChar chbotright = QChar(9565);

    QPen framepen;
    framepen.setColor(Qt::yellow);

    painter->setPen(framepen);
    int quad = metrics.horizontalAdvance(chtopleft) / 4;

    int horizontalAdvance = BoundingRect.width() / metrics.horizontalAdvance(horline);
    for (int i = 1; i < horizontalAdvance; i++)
    {
        painter->drawText(BoundingRect.left() + i * metrics.horizontalAdvance(horline) - quad, metrics.height() / 2 - 1 + BoundingRect.top(), horline);
        painter->drawText(BoundingRect.left() + i * metrics.horizontalAdvance(horline) - quad, BoundingRect.bottom(), horline);
    }

    int verticalAdvance = qRound((qreal)BoundingRect.height() / (qreal)metrics.height());
    for (int i = 1; i < verticalAdvance; i++)
    {
        painter->drawText(BoundingRect.left(), i * (metrics.height() + BoundingRect.top()) + metrics.height() / 4, verline);
        painter->drawText(BoundingRect.right() - metrics.horizontalAdvance(verline), i * (metrics.height() + BoundingRect.top()) + metrics.height() / 4, verline);
    }

    //Верхние границы
    painter->drawText(BoundingRect.left(), metrics.height() / 2 - 1 + BoundingRect.top(), chtopleft);
    painter->drawText(BoundingRect.right() - metrics.horizontalAdvance(chtopright), metrics.height() / 2 - 1 + BoundingRect.top(), chtopright);

    // нижние границы
    painter->drawText(BoundingRect.left(), BoundingRect.bottom(), chbotleft);
    painter->drawText(BoundingRect.right() - metrics.horizontalAdvance(chtopright), BoundingRect.bottom(), chbotright);

    font2.setPixelSize(12);
    font2.setBold(true);
    painter->setFont(font2);*/

    /*QFontMetrics metrics2(font2);
    QRect titleRect = metrics2.boundingRect(title);
    QPoint pos(BoundingRect.left() + metrics2.horizontalAdvance(chtopleft) + 8, BoundingRect.top());//-metrics2.height()
    titleRect.moveTo(pos);
    painter->fillRect(titleRect, getBrush());
    painter->drawText(pos.x(), titleRect.height() - titleRect.height()/4, title);*/

    CustomRectItem::paint(painter, option, widget);
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
