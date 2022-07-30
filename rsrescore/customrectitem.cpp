#include "customrectitem.h"
#include "basescene.h"
#include "undoredo/undoitemmove.h"
#include "styles/resstyle.h"
#include <QApplication>
#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QFontDatabase>
#include <QRubberBand>
#include <QRect>
#include <QListIterator>
#include <QStyle>
#include <QStyleOption>
#include <QPainterPath>

#define SEL_RECT_ZIZE 6
#define PIE 3.1415926535897932384626433832795

CustomRectItem::CustomRectItem(QGraphicsItem* parent):
    QGraphicsObject(parent)
{
    init();
}

CustomRectItem::CustomRectItem(const QRect& rect, QGraphicsItem* parent):
    QGraphicsObject(parent)
{
    m_BoundingRect = rect;
    init();
}

void CustomRectItem::init()
{
    m_ResizeHandles.fill(QRect(0, 0, 0, 0), 8); //initially empty handles
    m_MousePressed = false;
    m_IsResizing = false;
    m_IsSelection = false;
    m_HasRubberBand = false;
    pRubberBand = nullptr;
    m_pUndoStack = nullptr;

    setFlags(QGraphicsItem::ItemIsSelectable |
             QGraphicsItem::ItemIsMovable |
             QGraphicsItem::ItemSendsGeometryChanges |
             QGraphicsItem::ItemClipsChildrenToShape |
             QGraphicsItem::ItemClipsToShape);
    setInputMethodHints(Qt::ImhHiddenText);

    m_AvailableCorners.setFlag(TOP_LEFT);
    m_AvailableCorners.setFlag(TOP);
    m_AvailableCorners.setFlag(TOP_RIGHT);
    m_AvailableCorners.setFlag(RIGHT);
    m_AvailableCorners.setFlag(BOTTOM_RIGHT);
    m_AvailableCorners.setFlag(BOTTOM);
    m_AvailableCorners.setFlag(BOTTOM_LEFT);
    m_AvailableCorners.setFlag(LEFT);

    CustomRectItem *pParent = dynamic_cast<CustomRectItem*>(parentItem());
    if (pParent)
        setUndoStack(pParent->undoStack());
}

ResStyle *CustomRectItem::style()
{
    BaseScene* pScene = qobject_cast<BaseScene*> (scene());

    if (!pScene)
        return nullptr;

    return pScene->style();
}

QRectF CustomRectItem::boundingRect() const
{
  return m_BoundingRect;
}

void CustomRectItem::setBrush(const QBrush &brush)
{
    m_brush = brush;
}

void CustomRectItem::setUndoStack(QUndoStack *stack)
{
    m_pUndoStack = stack;
}

QUndoStack *CustomRectItem::undoStack()
{
    return m_pUndoStack;
}

QVariant CustomRectItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionChange && scene())
    {
        QPointF newPos = value.toPointF();
        if(QApplication::mouseButtons() == Qt::LeftButton && qobject_cast<BaseScene*> (scene()))
        {
            BaseScene* customScene = qobject_cast<BaseScene*> (scene());
            QSize gridSize = customScene->getGridSize();
            qreal xV = round(newPos.x() / gridSize.width()) * gridSize.width();
            qreal yV = round(newPos.y() / gridSize.height())* gridSize.height();

            QPointF nPos = QPointF(xV, yV);
            CustomRectItem *parentCustomItem = dynamic_cast<CustomRectItem*>(parentItem());
            if (parentCustomItem)
            {
                if (parentCustomItem->childCanMove(nPos, this))
                    return nPos;
                else
                    return pos();
            }
            return nPos;
        }
        else
            return newPos;
    }
    else if (change == QGraphicsItem::ItemSceneChange)
    {
        BaseScene* customScene = qobject_cast<BaseScene*> (scene());

        if (customScene)
        {
            QSize gridSize = customScene->getGridSize();

            qreal wV = round(m_BoundingRect.width() / gridSize.width()) * gridSize.width();
            qreal hV = round(m_BoundingRect.width() / gridSize.height())* gridSize.height();

            m_BoundingRect.setWidth(wV);
            m_BoundingRect.setHeight(hV);
        }
    }

    return QGraphicsItem::itemChange(change, value);
}

void CustomRectItem::paintBevel(QPainter *painter, QColor color)
{
    if (isSelected())
    {
        QPen pens;
        pens.setCosmetic(true); //to maintain same width of pen across zoom levels
        //draw rect
        pens.setColor(color);
        pens.setStyle(Qt::DashLine);
        painter->setPen(pens);

        QRectF rc = m_BoundingRect;
        rc.setSize(QSizeF(m_BoundingRect.width() - 1, m_BoundingRect.height() - 1));
        painter->drawRect(rc);

        //draw resize handles
        pens.setStyle(Qt::SolidLine);
        painter->setBrush(Qt::white);
        painter->setPen(pens);
        painter->drawRects(m_ResizeHandles);
    }
}

void CustomRectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    if (isSelected())
    {
        qreal scale = scene()->views().at(0)->transform().m11(); //get current sace factor
        qreal rectSize = SEL_RECT_ZIZE / scale; //this is to maintain same size for resize handle rects
        QRectF handles(0, 0, rectSize, rectSize);
        QRectF m_CornerRect = boundingRect().adjusted(rectSize / 2, rectSize / 2, -rectSize / 2, -rectSize / 2);

        if (m_AvailableCorners.testFlag(TOP_LEFT))
        {
            handles.moveCenter(m_CornerRect.topLeft());  //TopLeft
            m_ResizeHandles.replace(0, handles);
        }

        if (m_AvailableCorners.testFlag(TOP_RIGHT))
        {
            handles.moveCenter(m_CornerRect.topRight()); //TopRight
            m_ResizeHandles.replace(2, handles);
        }

        if (m_AvailableCorners.testFlag(BOTTOM_RIGHT))
        {
            handles.moveCenter(m_CornerRect.bottomRight());  //BottomRight
            m_ResizeHandles.replace(4, handles);
        }

        if (m_AvailableCorners.testFlag(BOTTOM_LEFT))
        {
            handles.moveCenter(m_CornerRect.bottomLeft()); //BottomLeft
            m_ResizeHandles.replace(6, handles);
        }

        QPointF center;
        if (m_AvailableCorners.testFlag(TOP))
        {
            center = QPointF(m_CornerRect.center().x(), m_CornerRect.top()); //Top
            handles.moveCenter(center);
            m_ResizeHandles.replace(1, handles);
        }

        if (m_AvailableCorners.testFlag(RIGHT))
        {
            center = QPointF(m_CornerRect.right(), m_CornerRect.center().y()); //Right
            handles.moveCenter(center);
            m_ResizeHandles.replace(3, handles);
        }

        if (m_AvailableCorners.testFlag(BOTTOM))
        {
            center = QPointF(m_CornerRect.center().x(), m_CornerRect.bottom());  //Bottom
            handles.moveCenter(center);
            m_ResizeHandles.replace(5, handles);
        }

        if (m_AvailableCorners.testFlag(LEFT))
        {
            center = QPointF(m_CornerRect.left(), m_CornerRect.center().y());  //Left
            handles.moveCenter(center);
            m_ResizeHandles.replace(7, handles);
        }
    }
}

void CustomRectItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);

    if (event->button() == Qt::LeftButton)
    {
        QList<QGraphicsItem*> items = scene()->selectedItems();
        m_MousePressed = true;
        m_MousePressPoint.clear();
        m_IsResizing = mousePosOnHandles(event->scenePos()); //to check event on corners or not
        if (m_IsResizing)
            m_ActualRect = m_BoundingRect;

        if (!items.empty())
        {
            for (QGraphicsItem *selected : qAsConst(items))
            {
                CustomRectItem *rectItem = dynamic_cast<CustomRectItem*>(selected);
                m_MousePressPoint.insert(rectItem, selected->pos());
            }
        }
        else
            m_MousePressPoint.insert(this, pos());
    }
    else if (event->button() == Qt::RightButton)
    {
        if (rubberBand())
        {
            QPointF mapped = scene()->views()[0]->mapFromScene(event->scenePos());
            startDrag = QPoint(mapped.x(), mapped.y());
            rubberBand()->setGeometry(QRect(startDrag, QSize()));
            rubberBand()->show();
            m_IsSelection = true;

            scene()->views()[0]->setCursor(Qt::CrossCursor);
        }
    }
}

void CustomRectItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    BaseScene* customScene = qobject_cast<BaseScene*> (scene());
    QSize gridSize = customScene->getGridSize();

    if (m_IsResizing)
    {
        QPointF newPos = mapFromScene(event->scenePos());
        //qDebug() << "newPos: " << newPos << this->scene()->sceneRect();
        qreal xV = round(newPos.x() / gridSize.width()) * gridSize.width();
        qreal yV = round(newPos.y() / gridSize.height()) * gridSize.height();

        //QPointF ptMouseMoveInItemsCoord = mapFromScene(event->scenePos());
        QRectF tmpBoundingRect = m_BoundingRect;
        QPointF ptMouseMoveInItemsCoord = QPointF(xV, yV);
        switch (m_ResizeCorner)
        {
        case TOP_LEFT:
            if (this->scene()->sceneRect().contains(event->scenePos()) && m_AvailableCorners.testFlag(TOP_LEFT))
            {
                tmpBoundingRect.setTopLeft(ptMouseMoveInItemsCoord);
                tmpBoundingRect = tmpBoundingRect.normalized();

                if (canResize(tmpBoundingRect, m_ResizeCorner))
                    m_BoundingRect = tmpBoundingRect;

                scene()->views()[0]->setCursor(Qt::SizeFDiagCursor);
                qDebug() << "TOP_LEFT::transformOriginPoint()" << m_BoundingRect.center() << pos();
            }
            break;
        case TOP:
            if (this->scene()->sceneRect().contains(event->scenePos()) && m_AvailableCorners.testFlag(TOP))
            {
                tmpBoundingRect.setTop(ptMouseMoveInItemsCoord.y());
                tmpBoundingRect = tmpBoundingRect.normalized();

                if (canResize(tmpBoundingRect, m_ResizeCorner))
                    m_BoundingRect = tmpBoundingRect;

                scene()->views()[0]->setCursor(Qt::SizeVerCursor);
                qDebug() << "TOP::transformOriginPoint()" << m_BoundingRect.center();
            }
            break;
        case TOP_RIGHT:
            if (this->scene()->sceneRect().contains(event->scenePos()) && m_AvailableCorners.testFlag(TOP_RIGHT))
            {
                tmpBoundingRect.setTopRight(ptMouseMoveInItemsCoord);
                tmpBoundingRect = tmpBoundingRect.normalized();

                if (canResize(tmpBoundingRect, m_ResizeCorner))
                    m_BoundingRect = tmpBoundingRect;

                scene()->views()[0]->setCursor(Qt::SizeBDiagCursor);
                qDebug() << "TOP_RIGHT::transformOriginPoint()" << m_BoundingRect.center();
            }
            break;
        case RIGHT:
            if (this->scene()->sceneRect().contains(event->scenePos()) && m_AvailableCorners.testFlag(RIGHT))
            {
                tmpBoundingRect.setRight(ptMouseMoveInItemsCoord.x());
                tmpBoundingRect = tmpBoundingRect.normalized();

                if (canResize(tmpBoundingRect, m_ResizeCorner))
                    m_BoundingRect = tmpBoundingRect;

                scene()->views()[0]->setCursor(Qt::SizeHorCursor);
                qDebug() << "RIGHT::transformOriginPoint()" << m_BoundingRect.center();
            }
            break;
        case BOTTOM_RIGHT:
            if (this->scene()->sceneRect().contains(event->scenePos()) && m_AvailableCorners.testFlag(BOTTOM_RIGHT))
            {
                tmpBoundingRect.setBottomRight(ptMouseMoveInItemsCoord);
                tmpBoundingRect = tmpBoundingRect.normalized();

                if (canResize(tmpBoundingRect, m_ResizeCorner))
                    m_BoundingRect = tmpBoundingRect;

                scene()->views()[0]->setCursor(Qt::SizeFDiagCursor);
                qDebug() << "BOTTOM_RIGHT::transformOriginPoint()" << m_BoundingRect.center();
            }
            break;
        case BOTTOM:
            if (this->scene()->sceneRect().contains(event->scenePos()) && m_AvailableCorners.testFlag(BOTTOM))
            {
                //m_BoundingRect.setBottom(event->pos().y());
                tmpBoundingRect.setBottom(ptMouseMoveInItemsCoord.y());
                tmpBoundingRect = tmpBoundingRect.normalized();

                if (canResize(tmpBoundingRect, m_ResizeCorner))
                    m_BoundingRect = tmpBoundingRect;

                scene()->views()[0]->setCursor(Qt::SizeVerCursor);
                qDebug() << "BOTTOM::transformOriginPoint()" << m_BoundingRect.center();
            }
            break;
        case BOTTOM_LEFT:
            if (this->scene()->sceneRect().contains(event->scenePos()) && m_AvailableCorners.testFlag(BOTTOM_LEFT))
            {
                tmpBoundingRect.setBottomLeft(ptMouseMoveInItemsCoord);
                tmpBoundingRect = tmpBoundingRect.normalized();

                if (canResize(tmpBoundingRect, m_ResizeCorner))
                    m_BoundingRect = tmpBoundingRect;

                scene()->views()[0]->setCursor(Qt::SizeBDiagCursor);
                qDebug() << "BOTTOM_LEFT::transformOriginPoint()" << m_BoundingRect.center();
            }
            break;
        case LEFT:
            if (this->scene()->sceneRect().contains(event->scenePos()) && m_AvailableCorners.testFlag(LEFT))
            {
                tmpBoundingRect.setLeft(ptMouseMoveInItemsCoord.x());
                tmpBoundingRect = tmpBoundingRect.normalized();

                if (canResize(tmpBoundingRect, m_ResizeCorner))
                    m_BoundingRect = tmpBoundingRect;

                scene()->views()[0]->setCursor(Qt::SizeHorCursor);
                qDebug() << "LEFT::transformOriginPoint()" << m_BoundingRect.center();
            }
            break;
        }

        prepareGeometryChange();
        updateSizePos();
        update();
        scene()->update();
    }
    else
    {
        if (m_HasRubberBand && rubberBand() && m_IsSelection)
        {
            QPointF mapped = scene()->views()[0]->mapFromScene(event->scenePos());
            QPoint p(mapped.x(), mapped.y());
            rubberBand()->setGeometry(QRect(startDrag, p).normalized());

            QRectF rc = mapRectFromScene(rubberBand()->geometry()).normalized();
            QList<QGraphicsItem*> items = childItems();
            QListIterator<QGraphicsItem*> iterator(items);
            while (iterator.hasNext())
            {
                QGraphicsItem *item = iterator.next();

                if (item != this)
                {
                    QRectF bound = item->mapRectToParent(item->boundingRect()).normalized();
                    item->setSelected(bound.intersects(rc));
                }
                setSelected(false);
            }
        }
        prepareGeometryChange();
        updateSizePos();
        update();
        scene()->update();
        QGraphicsItem::mouseMoveEvent(event);
    }
}

void CustomRectItem::insertUndoRedoMove()
{
    if (m_MousePressPoint.size() == 1)
    {
        auto mousePos = m_MousePressPoint.first();
        if (mousePos != pos() && undoStack())
        {
            CustomRectItem *rectitem = dynamic_cast<CustomRectItem*>(m_MousePressPoint.firstKey());
            UndoItemMove *undocmd = new UndoItemMove(rectitem);
            undocmd->setPositions(mousePos, pos());

            undoStack()->push(undocmd);
        }
    }
    else if (!m_MousePressPoint.empty())
    {
        if (undoStack())
        {
            undoStack()->beginMacro(tr("Перемещение группы элементов"));

            QMapIterator<QGraphicsItem*, QPointF> items(m_MousePressPoint);
            while(items.hasNext())
            {
                auto item = items.next();

                CustomRectItem *rectitem = dynamic_cast<CustomRectItem*>(item.key());
                UndoItemMove *undocmd = new UndoItemMove(rectitem);
                undocmd->setPositions(item.value(), rectitem->pos());

                undoStack()->push(undocmd);
            }
            undoStack()->endMacro();
        }
    }
}

void CustomRectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_IsResizing)
    {
        QPointF dPos = m_BoundingRect.topLeft() - m_ActualRect.topLeft();
        moveBy(dPos.rx(), dPos.ry());
        QSizeF vsize = m_BoundingRect.size();
        m_BoundingRect.setTopLeft(QPointF(0, 0));
        m_BoundingRect.setSize(vsize);
    }
    else
    {
        if ((rubberBand() && !rubberBand()->isVisible()) || !rubberBand())
            insertUndoRedoMove();
    }

    if (rubberBand())
        rubberBand()->hide();

    scene()->views()[0]->setCursor(Qt::ArrowCursor);

    m_MousePressed = false;
    m_IsResizing = false;
    m_IsSelection = false;
    prepareGeometryChange();
    updateSizePos();
    update();
    scene()->update();

    QGraphicsItem::mouseReleaseEvent(event);
}

bool CustomRectItem::mousePosOnHandles(QPointF pos)
{
    bool resizable = false;
    int rem4Index = 8;// +(qRound(this->rotation()) / 45);
    if (mapToScene(m_ResizeHandles[(0 + rem4Index) % 8]).containsPoint(pos, Qt::WindingFill))
    {
        m_ResizeCorner = TOP_LEFT;
        resizable = true;
    }
    else if (mapToScene(m_ResizeHandles[(1 + rem4Index) % 8]).containsPoint(pos, Qt::WindingFill))
    {
        m_ResizeCorner = TOP;
        resizable = true;
    }
    else if (mapToScene(m_ResizeHandles[(2 + rem4Index) % 8]).containsPoint(pos, Qt::WindingFill))
    {
        m_ResizeCorner = TOP_RIGHT;
        resizable = true;
    }
    else if (mapToScene(m_ResizeHandles[(3 + rem4Index) % 8]).containsPoint(pos, Qt::WindingFill))
    {
        m_ResizeCorner = RIGHT;
        resizable = true;
    }
    else if (mapToScene(m_ResizeHandles[(4 + rem4Index) % 8]).containsPoint(pos, Qt::WindingFill))
    {
        m_ResizeCorner = BOTTOM_RIGHT;
        resizable = true;
    }
    else if (mapToScene(m_ResizeHandles[(5 + rem4Index) % 8]).containsPoint(pos, Qt::WindingFill))
    {
        m_ResizeCorner = BOTTOM;
        resizable = true;
    }
    else if (mapToScene(m_ResizeHandles[(6 + rem4Index) % 8]).containsPoint(pos, Qt::WindingFill))
    {
        m_ResizeCorner = BOTTOM_LEFT;
        resizable = true;
    }
    else if (mapToScene(m_ResizeHandles[(7 + rem4Index) % 8]).containsPoint(pos, Qt::WindingFill))
    {
        m_ResizeCorner = LEFT;
        resizable = true;
    }
    return resizable;
}

void CustomRectItem::setRubberBand(bool value)
{
    m_HasRubberBand = value;
}

QRubberBand *CustomRectItem::rubberBand()
{
    if (!m_HasRubberBand)
        return nullptr;

    if (!pRubberBand && !scene()->views().empty())
        pRubberBand = new QRubberBand(QRubberBand::Rectangle, scene()->views()[0]);

    return pRubberBand;
}

bool CustomRectItem::childCanMove(const QPointF &newPos, CustomRectItem *item)
{
    Q_UNUSED(item)
    if (newPos.x() < 0 || newPos.y() < 0)
        return false;

    return true;
}

const QBrush &CustomRectItem::getBrush() const
{
    return m_brush;
}

void CustomRectItem::setAvailableCorners(ResizeCornersFlags flags)
{
    m_AvailableCorners = flags;
}

bool CustomRectItem::canResize(const QRectF &newRect, const ResizeCorners &corner) const
{
    BaseScene* customScene = qobject_cast<BaseScene*> (scene());
    QSize gridSize = customScene->getGridSize();

    //qDebug() << newRect;
    if (newRect.width() < gridSize.width() || newRect.height() < gridSize.height())
        return false;

    if (corner != TOP_LEFT && corner != TOP && corner != TOP_RIGHT)
    {
        if (/*newRect.x() < 0 || */newRect.y() < 0)
            return false;
    }

    if (corner != LEFT && corner != TOP_LEFT && corner != BOTTOM_LEFT)
    {
        if (newRect.x() < 0)
            return false;
    }

    return true;
}

void CustomRectItem::drawIntersects(QPainter *painter)
{
    QGraphicsScene *pScene = scene();
    QList<QGraphicsItem*> itemList = pScene->items();

    QBrush saveBrush = painter->brush();
    for (QGraphicsItem *item : qAsConst(itemList))
    {
        if (item != this && item->parentItem() == parentItem())
        {
            QRectF mBoundingRect = mapRectToScene(boundingRect());
            QRectF mItemRect = item->mapRectToScene(item->boundingRect());

            if (mBoundingRect.intersects(mItemRect))
            {
                painter->save();
                painter->setCompositionMode(QPainter::CompositionMode_Plus/*CompositionMode_Overlay*/);
                QPolygonF intersected = mapFromScene(mBoundingRect.intersected(mItemRect).normalized());
                painter->setBrush(Qt::red);
                painter->drawPolygon(intersected);
                painter->drawRect(mBoundingRect.intersected(mItemRect).normalized());
                painter->restore();
            }
        }
    }
    painter->setBrush(saveBrush);
}

void CustomRectItem::setCoord(const QPoint &coord)
{
    BaseScene* customScene = qobject_cast<BaseScene*> (scene());
    QSize gridSize = customScene->getGridSize();

    qreal xV = gridSize.width() * coord.x();
    qreal yV = gridSize.height() * coord.y();

    prepareGeometryChange();
    m_Coord = coord;
    moveBy(xV - x(), yV - y());

    update();
    scene()->update();
}

void CustomRectItem::updateSizePos()
{
    BaseScene* customScene = qobject_cast<BaseScene*> (scene());

    QSizeF tmpSize = m_BoundingRect.size();
    QSize gridSize = customScene->getGridSize();

    qreal xV = round(tmpSize.width() / gridSize.width());
    qreal yV = round(tmpSize.height() / gridSize.height());

    m_Size = QSize(xV, yV);
}

void CustomRectItem::setSize(const QSize &size)
{
    BaseScene* customScene = qobject_cast<BaseScene*> (scene());
    QSize gridSize = customScene->getGridSize();

    qreal xV = gridSize.width() * size.width();
    qreal yV = gridSize.height() * size.height();

    prepareGeometryChange();
    m_Size = size;
    m_BoundingRect.setWidth(xV);
    m_BoundingRect.setHeight(yV);

    update();
    scene()->update();
}

QSize CustomRectItem::getSize() const
{
    return m_Size;
}

QSize CustomRectItem::gridSize() const
{
    BaseScene* customScene = qobject_cast<BaseScene*> (scene());
    return customScene->getGridSize();
}
