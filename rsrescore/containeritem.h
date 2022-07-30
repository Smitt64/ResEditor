#ifndef CONTAINERITEM_H
#define CONTAINERITEM_H

#include <customrectitem.h>

class ContainerItem : public CustomRectItem
{
public:
    ContainerItem(CustomRectItem* parent = nullptr);
    ContainerItem(const QRect& rect, CustomRectItem* parent = nullptr);
    virtual ~ContainerItem();

protected:
    virtual bool childCanMove(const QPointF &newPos, CustomRectItem *item) Q_DECL_OVERRIDE;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) Q_DECL_OVERRIDE;
    virtual void dropEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;
    virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;
    virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;
};

#endif // CONTAINERITEM_H
