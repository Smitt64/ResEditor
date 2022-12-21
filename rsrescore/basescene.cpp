#include "basescene.h"
#include "styles/resstyle.h"
#include "customrectitem.h"
#include "propertymodel/propertymodel.h"
#include <QPainter>

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

void BaseScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    qDebug() << "BaseScene::mousePressEvent";
    QGraphicsScene::mousePressEvent(mouseEvent);
}
