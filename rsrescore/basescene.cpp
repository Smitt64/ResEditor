#include "basescene.h"
#include "styles/resstyle.h"
#include "customrectitem.h"
#include "propertymodel.h"
#include <QPainter>
#include <QMapIterator>
#include <QGraphicsSceneMouseEvent>

BaseScene::BaseScene(QObject *parent)
    : QGraphicsScene{parent}
{
    m_pStyle = new ResStyle();

    connect(this, &QGraphicsScene::selectionChanged, [=]()
    {
        QList<QGraphicsItem*> sel = selectedItems();

        if (sel.size() == 1)
        {
            CustomRectItem *rectItem = dynamic_cast<CustomRectItem*>(sel.first());

            if (rectItem)
            {
                PropertyModel *model = rectItem->propertyModel();
                emit propertyModelChanged(model);
            }
        }
        else
            emit propertyModelChanged(nullptr);
    });
}

void BaseScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    m_pStyle->drawSceneBackground(painter, rect);
}

QSize BaseScene::getGridSize() const
{
    return m_pStyle->gridSize();
}

ResStyle *BaseScene::style()
{
    return m_pStyle;
}

CustomRectItem *BaseScene::findItem(const QUuid &uuid) const
{
    CustomRectItem *foundItem = nullptr;
    QList<QGraphicsItem*> elements = items();

    for (auto element : qAsConst(elements))
    {
        CustomRectItem *rectItem = dynamic_cast<CustomRectItem*>(element);

        if (rectItem && rectItem->uuid() == uuid)
        {
            foundItem = rectItem;
            break;
        }
    }

    return foundItem;
}

CustomRectItem *BaseScene::findTopLevelItem() const
{
    CustomRectItem *foundItem = nullptr;
    QList<QGraphicsItem*> elements = items();

    for (auto element : qAsConst(elements))
    {
        CustomRectItem *rectItem = dynamic_cast<CustomRectItem*>(element);

        if (rectItem && !rectItem->parentItem())
        {
            foundItem = rectItem;
            break;
        }
    }

    return foundItem;
}

void BaseScene::insertMousePressPoint(CustomRectItem *rectItem)
{
    if (m_MousePressPoint.contains(rectItem))
        return;

    m_MousePressPoint.insert(rectItem, rectItem->pos());
}

void BaseScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mousePressEvent(event);

    if (event->button() == Qt::LeftButton)
    {
        QList<QGraphicsItem*> itemsAboutPos = items(event->scenePos());
        CustomRectItem *pTopItem = findTopLevelItem();

        if ((event->modifiers() & Qt::ControlModifier) != Qt::ControlModifier)
        {
            bool NeedClear = true;
            for (QGraphicsItem *itemAtPos : qAsConst(itemsAboutPos))
            {
                CustomRectItem *rectitem = dynamic_cast<CustomRectItem*>(itemAtPos);

                if (pTopItem != rectitem && m_MousePressPoint.contains(rectitem))
                {
                    NeedClear = false;
                    break;
                }
            }

            if (NeedClear)
                clearMousePressPoints();
        }

        if (!itemsAboutPos.empty())
        {
            for (QGraphicsItem *selected : qAsConst(itemsAboutPos))
            {
                CustomRectItem *rectItem = dynamic_cast<CustomRectItem*>(selected);
                m_MousePressPoint.insert(rectItem, selected->pos());
            }
        }
    }
}

const BaseScene::CustomRectItemPoints &BaseScene::mousePressPoints() const
{
    return m_MousePressPoint;
}

void BaseScene::clearMousePressPoints()
{
    m_MousePressPoint.clear();
}

void BaseScene::sceneItemPosChanged()
{

}
