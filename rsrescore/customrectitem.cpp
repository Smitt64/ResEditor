#include "customrectitem.h"
#include "basescene.h"
#include "undoredo/undoitemmove.h"
#include "undoredo/undoitemresize.h"
#include "undoredo/undopropertychange.h"
#include "undoredo/undoitemadd.h"
#include "styles/resstyle.h"
#include "propertymodel.h"
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
#include <QMetaClassInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QFile>
#include <QMetaType>
#include <QKeyEvent>

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
    m_ItemId = QUuid::createUuid();
    m_ResizeHandles.fill(QRect(0, 0, 0, 0), 8); //initially empty handles
    m_MousePressed = false;
    m_IsResizing = false;
    m_IsSelection = false;
    m_HasRubberBand = false;
    pRubberBand = nullptr;
    m_pUndoStack = nullptr;
    m_pPropertyModel = nullptr;
    m_SkipUndoStack = false;
    m_CanIntersects = true;
    m_SkipRenderIntersects = false;
    m_Size = QSize(1, 1);
    m_Coord = QPoint(0, 0);

    setFlags(QGraphicsItem::ItemIsSelectable |
             QGraphicsItem::ItemIsMovable |
             QGraphicsItem::ItemSendsGeometryChanges |
             QGraphicsItem::ItemClipsChildrenToShape |
             QGraphicsItem::ItemClipsToShape |
             QGraphicsItem::ItemIsFocusable);
    setInputMethodHints(Qt::ImhHiddenText);
    setAcceptDrops(false);

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

    bool oldskip = setSkipUndoStack(true);
    setSkipUndoStack(oldskip);
}

const QUuid &CustomRectItem::uuid() const
{
    return m_ItemId;
}

void CustomRectItem::setUuid(const QUuid &_uuid)
{
    m_ItemId = _uuid;
}

ResStyle *CustomRectItem::style() const
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
        QPointF newPos = value.toPointF();// - m_MousePressedPos;

        if(QApplication::mouseButtons() == Qt::LeftButton && qobject_cast<BaseScene*> (scene()))
        {
            //qDebug() << newPos;
            BaseScene* customScene = qobject_cast<BaseScene*> (scene());
            QSize gridSize = customScene->getGridSize();
            qreal xV = round(newPos.x() / gridSize.width()) * gridSize.width();
            qreal yV = round(newPos.y() / gridSize.height())* gridSize.height();

            QPointF nPos = QPointF(xV, yV);
            CustomRectItem *parentCustomItem = dynamic_cast<CustomRectItem*>(parentItem());
            if (parentCustomItem)
            {
                bool fCanMove = parentCustomItem->childCanMove(nPos, this);

                if (fCanMove)
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
    /*else if (change == QGraphicsItem::ItemSelectedChange)
    {
        setFocus();
    }*/

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
        m_MousePressedPos = mapFromScene(event->scenePos());
        m_MousePressed = true;

        m_IsResizing = mousePosOnHandles(event->scenePos()); //to check event on corners or not
        if (m_IsResizing)
            m_ActualRect = m_BoundingRect;
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
    BaseScene *customScene = qobject_cast<BaseScene*> (scene());
    QSize gridSize = customScene->getGridSize();

    if (m_IsResizing)
    {
        QPointF newPos = mapFromScene(event->scenePos());

        qreal xV = round(newPos.x() / gridSize.width()) * gridSize.width();
        qreal yV = round(newPos.y() / gridSize.height()) * gridSize.height();

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
        case ROTATE:
        case ALL_NO_ROTATE:
        case ALL:
            break;
        }

        prepareGeometryChange();
        updateSizePos("mouseMoveEvent is resizing");
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
            CustomRectItem *pTopItem = customScene->findTopLevelItem();
            QListIterator<QGraphicsItem*> iterator(items);
            while (iterator.hasNext())
            {
                QGraphicsItem *item = iterator.next();

                if (item != pTopItem)
                {
                    QRectF bound = item->mapRectToParent(item->boundingRect()).normalized();
                    bool intersects = bound.intersects(rc);
                    item->setSelected(intersects);

                    if (intersects)
                    {
                        CustomRectItem *rectItem = dynamic_cast<CustomRectItem*>(item);
                        customScene->insertMousePressPoint(rectItem);
                    }
                }
                setSelected(false);
            }
        }
        /*else if (m_MousePressed)
        {
            QPointF newPos = mapFromScene(event->scenePos()) - m_MousePressedPos;
            qreal xV = round(newPos.x() / gridSize.width()) * gridSize.width();
            qreal yV = round(newPos.y() / gridSize.height()) * gridSize.height();
            QPointF ptMouseMoveInItemsCoord = QPointF(xV, yV);

            CustomRectItem *parentCustomItem = dynamic_cast<CustomRectItem*>(parentItem());
            if (parentCustomItem)
            {
                QPointF realPos = pos() + ptMouseMoveInItemsCoord;
                bool fCanMove = parentCustomItem->childCanMove(realPos, this);

                if (fCanMove)
                {
                    setPos(realPos);

                    prepareGeometryChange();
                    updateSizePos("mouseMoveEvent not resizing");
                }

                update();
                scene()->update();
            }
            m_MousePressedPos = mapFromScene(event->scenePos());

        }*/

        QGraphicsItem::mouseMoveEvent(event);
    }
}

void CustomRectItem::onInsertUndoRedoMove(const QMap<CustomRectItem*, QPointF> &MousePressPoint)
{
    Q_UNUSED(MousePressPoint)
}

void CustomRectItem::insertUndoRedoMove()
{
    BaseScene* customScene = qobject_cast<BaseScene*> (scene());
    BaseScene::CustomRectItemPoints MousePressPoint = customScene->mousePressPoints();
    CustomRectItem *pTopItem = customScene->findTopLevelItem();

    int size = MousePressPoint.size();
    if (size <= 2)
    {
        if (size == 2)
            MousePressPoint.remove(pTopItem);

        auto mousePos = MousePressPoint.first();
        if (mousePos != pos() && undoStack())
        {
            UndoItemMove *undocmd = new UndoItemMove(customScene, uuid());
            undocmd->setPositions(mousePos, pos());

            undoStack()->beginMacro(undocmd->text());
            undoStack()->push(undocmd);
            onInsertUndoRedoMove(MousePressPoint);
            customScene->sceneItemPosChanged();
            undoStack()->endMacro();
        }
    }
    else if (!MousePressPoint.empty())
    {
        enum
        {
            id_uid = 0,
            id_item_value,
            id_item_pos,
            id_item
        };

        if (undoStack())
        {
            using UndoTuple = std::tuple<QUuid, QPointF, QPointF, CustomRectItem*>;
            QVector<UndoTuple> undoData;

            QMapIterator<CustomRectItem*, QPointF> items(MousePressPoint);
            while(items.hasNext())
            {
                auto item = items.next();

                CustomRectItem *rectitem = dynamic_cast<CustomRectItem*>(item.key());

                QPointF item_value = item.value();
                QPointF item_pos = rectitem->pos();
                UndoTuple tmp = std::make_tuple(rectitem->uuid(), item_value, item_pos, rectitem);

                if (std::get<id_item_value>(tmp) != std::get<id_item_pos>(tmp))
                    undoData.append(tmp);
            }

            if (!undoData.isEmpty())
            {
                undoStack()->beginMacro(tr("Перемещение группы элементов"));
                for (const auto &_tuple : undoData)
                {
                    UndoItemMove *undocmd = new UndoItemMove(customScene, std::get<id_uid>(_tuple));
                    undocmd->setPositions(std::get<id_item_value>(_tuple), std::get<id_item_pos>(_tuple));
                    undoStack()->push(undocmd);

                    std::get<id_item>(_tuple)->onInsertUndoRedoMove(MousePressPoint);
                }

                customScene->sceneItemPosChanged();
                //onInsertUndoRedoMove(MousePressPoint);
                undoStack()->endMacro();
            }

        }
    }
}

void CustomRectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_IsResizing)
    {
        BaseScene* customScene = qobject_cast<BaseScene*> (scene());
        QUndoCommand *undocmd = nullptr;
        //UndoItemResize *undocmd = new UndoItemResize(customScene, uuid());

        QPointF dPos = m_BoundingRect.topLeft() - m_ActualRect.topLeft();
        moveBy(dPos.rx(), dPos.ry());
        QSizeF vsize = m_BoundingRect.size();
        m_BoundingRect.setTopLeft(QPointF(0, 0));
        m_BoundingRect.setSize(vsize);

        createItemResizeUndoObj(customScene, m_ActualRect.size(), m_BoundingRect.size(), &undocmd);
        //undocmd->setSizes(m_ActualRect.size(), m_BoundingRect.size());

        if (undoStack())
            undoStack()->push(undocmd);
        else
        {
            undocmd->redo();
            delete undocmd;
        }
    }
    else
    {
        if ((rubberBand() && !rubberBand()->isVisible()) || !rubberBand())
            insertUndoRedoMove();
    }

    if (rubberBand())
        rubberBand()->hide();

    scene()->views()[0]->setCursor(Qt::ArrowCursor);

    afterReleaseMouse(event);
}

void CustomRectItem::afterReleaseMouse(QGraphicsSceneMouseEvent *event)
{
    scene()->views()[0]->setCursor(Qt::ArrowCursor);

    m_MousePressed = false;
    m_IsResizing = false;
    m_IsSelection = false;
    prepareGeometryChange();
    updateSizePos("CustomRectItem::afterReleaseMouse");
    update();
    scene()->update();

    if (event)
        QGraphicsItem::mouseReleaseEvent(event);
}

void CustomRectItem::setBoundingRect(const QRectF &bound)
{
    m_BoundingRect = bound;
}

const QRectF &CustomRectItem::actualRect() const
{
    return m_ActualRect;
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
    {
        return false;
    }

    return true;
}

const bool &CustomRectItem::isResizing() const
{
    return m_IsResizing;
}

const QBrush &CustomRectItem::getBrush() const
{
    return m_brush;
}

void CustomRectItem::setAvailableCorners(ResizeCornersFlags flags)
{
    m_AvailableCorners = flags;
    m_ResizeHandles.fill(QRect(0, 0, 0, 0), 8);
}

void CustomRectItem::setAvailableCorners(ResizeCorners flag, bool enable)
{
    m_AvailableCorners.setFlag(flag, enable);
}

const CustomRectItem::ResizeCornersFlags &CustomRectItem::availableCorners() const
{
    return m_AvailableCorners;
}

bool CustomRectItem::canResize(const QRectF &newRect, const ResizeCorners &corner) const
{
    BaseScene* customScene = qobject_cast<BaseScene*> (scene());
    QSize gridSize = customScene->getGridSize();

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

bool CustomRectItem::isIntersects(const QRectF &thisBound, QGraphicsItem *item, const QRectF &itemBound) const
{
    Q_UNUSED(thisBound)
    Q_UNUSED(item)
    Q_UNUSED(itemBound)

    if (m_CanIntersects)
        return true;

    return false;
}

bool CustomRectItem::setSkipRenderIntersects(bool value)
{
    bool old = m_SkipRenderIntersects;
    m_SkipRenderIntersects = value;

    return old;
}

void CustomRectItem::drawIntersects(QPainter *painter)
{
    if (m_SkipRenderIntersects)
        return;

    QGraphicsScene *pScene = scene();
    QList<QGraphicsItem*> itemList = pScene->items();

    QBrush saveBrush = painter->brush();
    for (QGraphicsItem *item : qAsConst(itemList))
    {
        bool skipCheck = false;
        CustomRectItem *rectitem = dynamic_cast<CustomRectItem*>(item);
        if (rectitem)
            skipCheck = !rectitem->isCanIntersects();

        if (item != this && item->parentItem() == parentItem() && !skipCheck)
        {
            QRectF mBoundingRect = mapRectToScene(boundingRect());
            QRectF mItemRect = item->mapRectToScene(item->boundingRect());

            if (mBoundingRect.intersects(mItemRect) && isIntersects(mBoundingRect, item, mItemRect))
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
    emit geometryChanged();
}

void CustomRectItem::updateSizePos(const QString &from)
{
    BaseScene* customScene = qobject_cast<BaseScene*> (scene());

    QSizeF tmpSize = m_BoundingRect.size();
    QSize gridSize = customScene->getGridSize();
    QPointF tmpPos = pos();

    qreal xV = round(tmpSize.width() / gridSize.width());
    qreal yV = round(tmpSize.height() / gridSize.height());

    qreal xP = round(tmpPos.x() / gridSize.width());
    qreal yP = round(tmpPos.y() / gridSize.height());

    m_Size = QSize(xV, yV);
    m_Coord = QPoint(xP, yP);

    if (!isResizing())
    {
        emit geometryChanged();
    }
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
    emit geometryChanged();
}

QPoint CustomRectItem::realCoordToEw(const QPointF &point)
{
    QSize gridSize = style()->gridSize();
    qreal xV = round(point.x() / gridSize.width());
    qreal yV = round(point.y() / gridSize.height());

    return QPoint(xV, yV);
}

QPointF CustomRectItem::ewCoordToReal(const QPoint &point)
{
    BaseScene* customScene = qobject_cast<BaseScene*> (scene());
    QSize gridSize = customScene->getGridSize();

    qreal xV = gridSize.width() * point.x();
    qreal yV = gridSize.height() * point.y();

    return QPoint(xV, yV);
}

QRect CustomRectItem::geometry() const
{
    QRect rect(m_Coord, m_Size);
    return rect;
}

void CustomRectItem::setGeometry(const QRect &_geometry)
{
    checkPropSame("geometry", _geometry);

    QSize grSize = style()->gridSize();
    BaseScene* customScene = qobject_cast<BaseScene*> (scene());
    QPointF newPos(_geometry.x() * grSize.width(), _geometry.y() * grSize.height());
    QSizeF newSize(_geometry.width() * grSize.width(), _geometry.height() * grSize.height());

    if (undoStack() && !isSkipUndoStack())
    {
        UndoItemMove *undocmdpos = new UndoItemMove(customScene, uuid());
        UndoItemResize *undocmdsize = new UndoItemResize(customScene, uuid());
        undocmdpos->setPositions(pos(), newPos);
        undocmdsize->setSizes(m_BoundingRect.size(), newSize);

        undoStack()->beginMacro(tr("%1: Изменение геометрии").arg(undoRedoType()));
        undoStack()->push(undocmdpos);
        undoStack()->push(undocmdsize);
        undoStack()->endMacro();
    }
    else
    {
        setPos(newPos);
        setSize(newSize);
        updateSizePos("setGeometry");
    }
}

QString CustomRectItem::undoRedoType() const
{
    QString name;
    int typeID = metaObject()->indexOfClassInfo(CLASSINFO_UNDOREDO);
    if (typeID >= 0)
        name = metaObject()->classInfo(typeID).value();

    return name;
}

void CustomRectItem::setSize(const QSizeF &size)
{
    m_BoundingRect.setWidth(size.width());
    m_BoundingRect.setHeight(size.height());

    updateSizePos("setSize");
    update();
    scene()->update();
}

QSize CustomRectItem::getSize() const
{
    return m_Size;
}

QPoint CustomRectItem::getPoint() const
{
    return m_Coord;
}

QSize CustomRectItem::gridSize() const
{
    BaseScene* customScene = qobject_cast<BaseScene*> (scene());
    return customScene->getGridSize();
}

PropertyModel *CustomRectItem::propertyModel()
{
    if (!m_pPropertyModel)
        m_pPropertyModel = new PropertyModel(this);

    return m_pPropertyModel;
}

bool CustomRectItem::setSkipUndoStack(const bool &value)
{
    bool old = m_SkipUndoStack;
    m_SkipUndoStack = value;
    return old;
}

const bool &CustomRectItem::isSkipUndoStack() const
{
    return m_SkipUndoStack;
}

void CustomRectItem::pushUndoPropertyData(const QString &propertyName, const QVariant &_newValue)
{
    BaseScene* customScene = qobject_cast<BaseScene*> (scene());
    UndoPropertyChange *undoprop = new UndoPropertyChange(customScene, uuid());
    undoprop->setPropertyName(propertyName);
    undoprop->setValues(property(propertyName.toLocal8Bit().data()), _newValue);
    undoStack()->push(undoprop);
}

QString CustomRectItem::getClassInfo(const QMetaObject *obj, const char *name) const
{
    int index = obj->indexOfClassInfo(name);

    if (index >= 0)
        return obj->classInfo(index).value();

    return QString();
}

bool CustomRectItem::checkPropSameValue(const QString &propertyName, const QVariant &value)
{
    QVariant propvalue = property(propertyName.toLocal8Bit().data());
    return propvalue == value;
}

void CustomRectItem::serializeProperty(QJsonObject &obj, const QMetaObject *meta, const QString &propertyName)
{
    int propertyId = meta->indexOfProperty(propertyName.toLocal8Bit().data());

    if (propertyId >= 0)
    {
        QMetaProperty prop = meta->property(propertyId);
        QVariant::Type type = prop.type();
        QVariant value = property(propertyName.toLocal8Bit().data());

        obj.insert("property", propertyName);
        if (type == QVariant::Point)
        {
            QJsonArray arr;
            QPoint point = value.toPoint();
            arr.append(point.x());
            arr.append(point.y());
            obj.insert("value", arr);
        }
        else if (type == QVariant::PointF)
        {
            QJsonArray arr;
            QPointF point = value.toPoint();
            arr.append(point.x());
            arr.append(point.y());
            obj.insert("value", arr);
        }
        else if (type == QVariant::Size)
        {
            QJsonArray arr;
            QSize point = value.toSize();
            arr.append(point.width());
            arr.append(point.height());
            obj.insert("value", arr);
        }
        else if (type == QVariant::SizeF)
        {
            QJsonArray arr;
            QSizeF point = value.toSizeF();
            arr.append(point.width());
            arr.append(point.height());
            obj.insert("value", arr);
        }
        else if (type == QVariant::Rect)
        {
            QJsonArray arr;
            QRect point = value.toRect();
            arr.append(point.x());
            arr.append(point.y());
            arr.append(point.width());
            arr.append(point.height());
            obj.insert("value", arr);
        }
        else if (type == QVariant::RectF)
        {
            QJsonArray arr;
            QRectF point = value.toRectF();
            arr.append(point.x());
            arr.append(point.y());
            arr.append(point.width());
            arr.append(point.height());
            obj.insert("value", arr);
        }
        else if (type == QVariant::Int || prop.isEnumType())
            obj.insert("value", value.toInt());
        else if (type == qMetaTypeId<quint16>())
            obj.insert("value", QJsonValue::fromVariant(value));
        else if (type == QVariant::Double)
            obj.insert("value", value.toDouble());
        else if (type == QVariant::String)
            obj.insert("value", value.toString());
        else if (type == QVariant::Bool)
            obj.insert("value", value.toBool());
        else
        {
            if (QMetaType::hasRegisteredConverterFunction(value.userType(), QMetaType::QJsonObject))
            {
                QJsonObject tmp = value.toJsonObject();
                obj.insert("value", tmp);
            }
        }
    }
}

void CustomRectItem::serialize(QJsonObject &data)
{
    QList<const QMetaObject*> metaobjects;
    const QMetaObject *obj = metaObject();

    do
    {
        metaobjects.append(obj);
        obj = obj->superClass();
    } while(obj);

    QJsonArray propertyArray;
    //std::reverse(metaobjects.begin(), metaobjects.end());

    bool fCustomRectItemPassed = false;
    QSet<QString> propertyList;

    QSet<QString> processedProp;
    for (auto metaobject : metaobjects)
    {
        int propCount = metaobject->propertyCount();
        for (int i = 0; i < propCount; i++)
        {
            const QMetaProperty &prop = metaobject->property(i);

            if (!fCustomRectItemPassed)
            {
                if (prop.isWritable())
                    propertyList.insert(prop.name());
            }
            else
                propertyList.remove(prop.name());
        }
        /*QString propertyString = getClassInfo(metaobject, CLASSINFO_SERIALIZE_PROP);
        QStringList propertyList;// = propertyString.split(";");

        if (!propertyString.contains(".json"))
            propertyList = propertyString.split(";");
        else
        {
            QFile f(propertyString);
            if (f.open(QIODevice::ReadOnly))
            {
                QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
                QJsonObject obj = doc.object();

                if (obj.contains("propertyList"))
                    propertyList = fromJsonArray<QString>(obj["propertyList"].toArray());
            }
        }*/

        /*for (const QString &propertyName : qAsConst(propertyList))
        {
            if (!propertyName.isEmpty() && !processedProp.contains(propertyName))
            {
                QJsonObject propObj;
                serializeProperty(propObj, metaobject, propertyName);
                propertyArray.append(propObj);
                processedProp.insert(propertyName);
            }
        }*/

        if (metaobject->className() == QString("CustomRectItem"))
            fCustomRectItemPassed = true;
    }

    for (const QString &propertyName : qAsConst(propertyList))
    {
        if (!propertyName.isEmpty() && !processedProp.contains(propertyName))
        {
            QJsonObject propObj;
            serializeProperty(propObj, metaObject(), propertyName);
            propertyArray.append(propObj);
            processedProp.insert(propertyName);
        }
    }

    data.insert("class", metaObject()->className());
    data.insert("uuid", uuid().toString());

    if (parentItem())
    {
        CustomRectItem *parentRectItem = dynamic_cast<CustomRectItem*>(parentItem());
        data.insert("parent", parentRectItem->uuid().toString());
    }

    data.insert("properties", propertyArray);
}

void CustomRectItem::serialize(QByteArray &data)
{
    QJsonObject rootObj;
    serialize(rootObj);

    QJsonDocument doc(rootObj);
    data = doc.toJson(QJsonDocument::Compact);
}

void CustomRectItem::deserialize(const QJsonObject &data)
{
    bool old = setSkipUndoStack(true);
    readProperties(data);
    setSkipUndoStack(old);
}

void CustomRectItem::deserialize(const QByteArray &data)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    deserialize(doc.object());
}

void CustomRectItem::readProperties(const QJsonObject &obj)
{
    QJsonArray properties = obj["properties"].toArray();

    for (const auto &prop : qAsConst(properties))
    {
        QJsonObject propObj = prop.toObject();
        deserializeProperty(propObj);
    }
}

void CustomRectItem::deserializeProperty(QJsonObject &obj)
{
    QString propertyName = obj["property"].toString();
    QVariant value;

    int propertyId = metaObject()->indexOfProperty(propertyName.toLocal8Bit().data());
    if (propertyId >= 0)
    {
        QMetaProperty prop = metaObject()->property(propertyId);
        QVariant::Type type = prop.type();

        if (type == QVariant::Point)
        {
            QVector<int> val = fromJsonArray<int>(obj["value"].toArray());
            QPoint pt(val[0], val[1]);
            value = QVariant::fromValue(pt);
        }
        else if (type == QVariant::PointF)
        {
            QVector<qreal> val = fromJsonArray<qreal>(obj["value"].toArray());
            QPointF pt(val[0], val[1]);
            value = QVariant::fromValue(pt);
        }
        else if (type == QVariant::Size)
        {
            QVector<int> val = fromJsonArray<int>(obj["value"].toArray());
            QSize pt(val[0], val[1]);
            value = QVariant::fromValue(pt);
        }
        else if (type == QVariant::SizeF)
        {
            QVector<qreal> val = fromJsonArray<qreal>(obj["value"].toArray());
            QSizeF pt(val[0], val[1]);
            value = QVariant::fromValue(pt);
        }
        else if (type == QVariant::Rect)
        {
            QVector<int> val = fromJsonArray<int>(obj["value"].toArray());
            QRect pt(val[0], val[1], val[2], val[3]);
            value = QVariant::fromValue(pt);
        }
        else if (type == QVariant::RectF)
        {
            QVector<qreal> val = fromJsonArray<qreal>(obj["value"].toArray());
            QRectF pt(val[0], val[1], val[2], val[3]);
            value = QVariant::fromValue(pt);
        }
        else if (type == QVariant::Int || prop.isEnumType())
            value = obj["value"].toInt();
        else if (type == qMetaTypeId<quint16>())
            value = obj["value"].toVariant();
        else if (type == QVariant::Double)
            value = obj["value"].toDouble();
        else if (type == QVariant::String)
            value = obj["value"].toString();
        else if (type == QVariant::Bool)
            value = obj["value"].toBool();
        else
        {
            int userType = prop.userType();
            if (QMetaType::hasRegisteredConverterFunction(QMetaType::QJsonObject, userType))
            {
                const QMetaObject *meta = QMetaType::metaObjectForType(userType);

                if (meta)
                {
                    QObject *newObj = meta->newInstance();

                    if (newObj)
                    {
                        QJsonObject valueObj = obj["value"].toObject();
                        bool convert = QMetaType::convert(&valueObj, QMetaType::QJsonObject, newObj, userType);

                        if (convert)
                            setProperty(propertyName.toLocal8Bit().data(), QVariant(userType, newObj));
                        delete newObj;
                    }
                }
                else
                {
                    void *toConvert = QMetaType::create(userType);

                    if (toConvert)
                    {
                        QJsonObject valueObj = obj["value"].toObject();
                        bool convert = QMetaType::convert(&valueObj, QMetaType::QJsonObject, toConvert, userType);

                        if (convert)
                        {
                            QVariant t = QVariant(userType, toConvert);
                            setProperty(propertyName.toLocal8Bit().data(), t);
                            QMetaType::destroy(userType, toConvert);
                        }
                    }
                }
            }
        }
    }

    if (value.isValid())
        setProperty(propertyName.toLocal8Bit().data(), value);
}

void CustomRectItem::renderToPixmap(QPixmap **pix, const QPointF &offset)
{
    if (!(*pix))
    {
        *pix = new QPixmap(m_BoundingRect.toRect().size());
        (*pix)->fill(Qt::transparent);
    }

    bool skipIntersects = setSkipRenderIntersects(true);

    QPainter painter(*pix);
    painter.save();

    if (!offset.isNull())
        painter.translate(offset);

    paint(&painter, nullptr, nullptr);
    painter.restore();

    std::function<void(QPainter*,QGraphicsItem*)> renderChild = [&renderChild](QPainter *painter, QGraphicsItem *item)
    {
        CustomRectItem *rectItem = dynamic_cast<CustomRectItem*>(item);

        if (rectItem->isVisible())
        {
            bool skipIntersectsChild = rectItem->setSkipRenderIntersects(true);

            painter->save();
            QPointF coord = rectItem->ewCoordToReal(rectItem->getPoint());
            painter->translate(coord);
            item->paint(painter, nullptr, nullptr);
            painter->restore();
            rectItem->setSkipRenderIntersects(skipIntersectsChild);

            QList<QGraphicsItem*> childs = item->childItems();
            for (QGraphicsItem *child : qAsConst(childs))
                renderChild(painter, child);
        }
    };

    QList<QGraphicsItem*> childs = childItems();
    for (QGraphicsItem *child : qAsConst(childs))
        renderChild(&painter, child);

    setSkipRenderIntersects(skipIntersects);
}

void CustomRectItem::createFromJson(CustomRectItem *parent, const QByteArray &data, QList<QGraphicsItem*> &items)
{
    if (!parent)
        return;

    BaseScene *pScene = dynamic_cast<BaseScene*>(scene());
    UndoItemAdd *pUndoRedo = new UndoItemAdd(pScene);

    pUndoRedo->setData(data);
    pUndoRedo->setItemsListPtr(&items);
    pUndoRedo->redo();

    delete pUndoRedo;
}

QVariant CustomRectItem::userAction(const qint32 &action, const QVariant &param)
{
    Q_UNUSED(action)
    Q_UNUSED(param)
    return QVariant();
}

void CustomRectItem::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return)
        userAction(ActionKeyEnter);
}

const bool &CustomRectItem::isCanIntersects() const
{
    return m_CanIntersects;
}

const bool &CustomRectItem::isMousePressed() const
{
    return m_MousePressed;
}

void CustomRectItem::setCanIntersects(const bool &flag)
{
    m_CanIntersects = flag;
}

void CustomRectItem::createItemResizeUndoObj(BaseScene* customScene,
                             const QSizeF &Actual,
                             const QSizeF &New,
                             QUndoCommand **cmd)
{
    UndoItemResize *undocmd = new UndoItemResize(customScene, uuid());
    undocmd->setSizes(Actual, New);

    *cmd = undocmd;
}
