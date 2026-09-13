#include "StdEditorScene.h"
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QSize>
#include <QRectF>
#include <QList>
#include <QGraphicsItem>
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>
#include "ControlItemsWrapper.h"
#include "PanelItem.h"
#include "scrolitem.h"

StdEditorScene::StdEditorScene(QObject *parent) :
    BaseScene(parent),
    m_fShowCursor(false),
    m_CursorPos(),
    m_controlItemsWrapper(nullptr)
{
    m_pCursorTimer = new QTimer(this);
    m_pCursorTimer->setInterval(500);
    m_pCursorTimer->setSingleShot(false);

    connect(m_pCursorTimer, &QTimer::timeout, [&]()
            {
                m_fShowCursor = !m_fShowCursor;
            });

    m_controlItemsWrapper = new ControlItemsWrapper(this);
    m_pCursorTimer->start();
}

StdEditorScene::~StdEditorScene()
{
}

ControlItemsWrapper *StdEditorScene::controlItemsWrapper()
{
    return m_controlItemsWrapper;
}

const QPointF &StdEditorScene::cursorPos() const
{
    return m_CursorPos;
}

void StdEditorScene::sceneItemPosChanged()
{
    PanelItem *panel = findFirst<PanelItem>();

    if (panel)
        panel->updateChildControlsOrder();
}

void StdEditorScene::setCursorPosition(const QPointF &scenePos)
{
    PanelItem* panel = findFirst<PanelItem>();
    if (!panel)
        return;

    QPointF localPos = panel->mapFromScene(scenePos);
    QSize gridSize = getGridSize();

    qreal xV = floor(localPos.x() / gridSize.width()) * gridSize.width();
    qreal yV = floor(localPos.y() / gridSize.height()) * gridSize.height();

    m_CursorPos = QPointF(xV, yV);
    update();
}

void StdEditorScene::handleSelectionChanged(const QList<QGraphicsItem*> &selectedItems)
{
    QVector<ControlItem*> controlItems;

    for (QGraphicsItem* item : selectedItems)
    {
        ControlItem* controlItem = dynamic_cast<ControlItem*>(item);
        if (controlItem)
            controlItems.append(controlItem);
    }

    if (!controlItems.isEmpty())
    {
        // Если выделено несколько ControlItem, создаем или обновляем обертку
        if (!m_controlItemsWrapper->undoStack())
            m_controlItemsWrapper->setUndoStack(controlItems.first()->undoStack());

        m_controlItemsWrapper->clearControlItems();
        m_controlItemsWrapper->addControlItems(controlItems);
        m_controlItemsWrapper->emitAll();
        emit propertyModelChanged(m_controlItemsWrapper->propertyModel());
    }
    else
    {
        BaseScene::handleSelectionChanged(selectedItems);
    }
}

void StdEditorScene::drawBackground(QPainter* painter, const QRectF &rect)
{
    BaseScene::drawBackground(painter, rect);

    QList<QGraphicsItem*> totalItems = items(Qt::AscendingOrder);
    for (QGraphicsItem *item : qAsConst(totalItems))
    {
        PanelItem *panel = dynamic_cast<PanelItem*>(item);

        if (panel)
        {
            QSize grSize = getGridSize();
            QRectF panelRect = panel->mapRectToScene(panel->boundingRect());
            panelRect.translate(QPointF(grSize.width() * 2, grSize.height()));
            painter->save();
            painter->setPen(Qt::NoPen);
            painter->setBrush(QBrush(Qt::Dense3Pattern));
            painter->drawRect(panelRect);
            painter->restore();
            break;
        }
    }
}

void StdEditorScene::drawForeground(QPainter *painter, const QRectF &rect)
{
    if (m_fShowCursor && !m_CursorPos.isNull())
    {
        painter->save();
        painter->setCompositionMode(QPainter::RasterOp_NotDestination);

        PanelItem* panel = findFirst<PanelItem>();
        QPointF sceneCursorPos = m_CursorPos;
        if (panel)
            sceneCursorPos = panel->mapToScene(m_CursorPos);

        painter->fillRect(QRectF(sceneCursorPos, QSizeF(getGridSize().width(), getGridSize().height())), Qt::black);
        painter->restore();
    }

    update(rect);
}

void StdEditorScene::keyPressEvent(QKeyEvent *keyEvent)
{
    PanelItem* panel = findFirst<PanelItem>();
    if (!panel)
    {
        BaseScene::keyPressEvent(keyEvent);
        return;
    }

    QSize grSize = getGridSize();
    QPointF localCursorPos = m_CursorPos;
    QPointF savepos = localCursorPos;

    if (keyEvent->key() == Qt::Key_Down)
        localCursorPos.setY(localCursorPos.y() + grSize.height());

    if (keyEvent->key() == Qt::Key_Up)
        localCursorPos.setY(localCursorPos.y() - grSize.height());

    if (keyEvent->key() == Qt::Key_Right)
        localCursorPos.setX(localCursorPos.x() + grSize.width());

    if (keyEvent->key() == Qt::Key_Left)
        localCursorPos.setX(localCursorPos.x() - grSize.width());

    QRectF panelRect = panel->boundingRect();
    QRectF availableRect = panelRect;

    if (panel->borderStyle() != ResStyle::Border_NoLine)
    {
        availableRect = panelRect.adjusted(grSize.width(), grSize.height(),
                                           -grSize.width(), -grSize.height());
    }

    QRectF cursorRect(localCursorPos, QSizeF(grSize.width(), grSize.height()));

    if (availableRect.contains(cursorRect))
        m_CursorPos = localCursorPos;
    else
        m_CursorPos = savepos;

    BaseScene::keyPressEvent(keyEvent);
}

void StdEditorScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    BaseScene::mousePressEvent(mouseEvent);

    /*if (mouseEvent->button() != Qt::LeftButton)
        return;*/

    PanelItem* panelItem = findFirst<PanelItem>();
    if (!panelItem)
        return;

    QSize gridSize = getGridSize();
    QPointF scenePos = mouseEvent->scenePos();
    QPointF localPos = panelItem->mapFromScene(scenePos);

    qreal xV = floor(localPos.x() / gridSize.width()) * gridSize.width();
    qreal yV = floor(localPos.y() / gridSize.height()) * gridSize.height();
    QPointF gridPos(xV, yV);

    QRectF cellRect(gridPos, gridSize);
    if (!cellRect.contains(localPos))
        return;

    QRectF panelBound = panelItem->boundingRect();
    QRectF availableBound = panelBound;

    if (panelItem->borderStyle() != ResStyle::Border_NoLine)
    {
        availableBound = panelBound.adjusted(gridSize.width(), gridSize.height(),
                                             -gridSize.width(), -gridSize.height());
    }

    if (!availableBound.contains(cellRect))
        return;

    bool positionBlocked = false;
    QList<QGraphicsItem*> itemsAtPos = items(scenePos);

    for (QGraphicsItem* item : qAsConst(itemsAtPos))
    {
        if (item == panelItem || dynamic_cast<ScrolAreaRectItem*>(item) || !item->isVisible())
            continue;

        QRectF itemRect = item->mapRectToScene(item->boundingRect());
        QRectF sceneCellRect = panelItem->mapToScene(cellRect).boundingRect();
        if (itemRect.intersects(sceneCellRect))
        {
            positionBlocked = true;
            break;
        }
    }

    if (!positionBlocked)
    {
        m_CursorPos = gridPos; // Сохраняем в локальных координатах panelItem
        update();
    }
}
