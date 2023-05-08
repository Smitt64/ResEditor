#ifndef CONTAINERITEM_H
#define CONTAINERITEM_H

#include <customrectitem.h>
#include "styles/resstyle.h"

class ContainerItem : public CustomRectItem
{
    Q_OBJECT
    Q_PROPERTY(ResStyle::BorderStyle borderStyle READ borderStyle WRITE setBorderStyle NOTIFY borderStyleChanged)

    Q_CLASSINFO(CLASSINFO_UNDOREDO, "CONTAINER")
    Q_CLASSINFO(CLASSINFO_PROPERTYLIST, ":/json/Container.json")
    Q_CLASSINFO(CLASSINFO_PROPERTYGROUP, "Container")
public:
    Q_INVOKABLE ContainerItem(QGraphicsItem* parent = nullptr);
    ContainerItem(const QRect& rect, CustomRectItem* parent = nullptr);
    virtual ~ContainerItem();

    const ResStyle::BorderStyle &borderStyle() const;
    void setBorderStyle(const ResStyle::BorderStyle &style);

signals:
    void borderStyleChanged();

protected:
    virtual bool childCanMove(const QPointF &newPos, CustomRectItem *item) Q_DECL_OVERRIDE;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) Q_DECL_OVERRIDE;
    virtual void dropEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;
    virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;
    virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) Q_DECL_OVERRIDE;

    virtual bool isIntersects(const QRectF &thisBound, QGraphicsItem *item, const QRectF &itemBound) const Q_DECL_OVERRIDE;

    ResStyle::BorderStyle m_BorderStyle;
};

class BorderItem : public ContainerItem
{
    Q_OBJECT
    Q_PROPERTY(ResStyle::BorderStyle borderStyle READ borderStyle WRITE setBorderStyle NOTIFY borderStyleChanged)

    Q_CLASSINFO(CLASSINFO_UNDOREDO, "BORDER")
    Q_CLASSINFO(CLASSINFO_PROPERTYLIST, ":/json/Container.json")
    Q_CLASSINFO(CLASSINFO_PROPERTYGROUP, "Border")
public:
    Q_INVOKABLE BorderItem(QGraphicsItem *parent = nullptr);
    BorderItem(const QRect& rect, CustomRectItem* parent = nullptr);
    virtual ~BorderItem();

    void setBorderParams(const QRect &rect, const qint16 &St);
};

Q_DECLARE_OPAQUE_POINTER(ContainerItem)
Q_DECLARE_OPAQUE_POINTER(BorderItem)

#endif // CONTAINERITEM_H
