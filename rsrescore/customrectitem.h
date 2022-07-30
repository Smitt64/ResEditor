#ifndef CUSTOMRECTITEM_H
#define CUSTOMRECTITEM_H

#include <QGraphicsObject>
#include "math.h"
#include <QBrush>

#define CLASSINFO_UNDOREDO "UNDOREDO"

class QRubberBand;
class QUndoStack;
class ResStyle;
class CustomRectItem : public QGraphicsObject
{
    friend class UndoItemMove;
public:
    enum ResizeCorners
    {
      TOP_LEFT = 1 << 1,
      TOP = 1 << 2,
      TOP_RIGHT = 1 << 3,
      RIGHT = 1 << 4,
      BOTTOM_RIGHT = 1 << 5,
      BOTTOM = 1 << 6,
      BOTTOM_LEFT = 1 << 7,
      LEFT = 1 << 8,
      ROTATE = 1 << 9
    };
    Q_DECLARE_FLAGS(ResizeCornersFlags, ResizeCorners)

    CustomRectItem(QGraphicsItem* parent = nullptr);
    CustomRectItem(const QRect& rect, QGraphicsItem* parent = nullptr);
    QRectF boundingRect() const Q_DECL_OVERRIDE;

    void setBrush(const QBrush &brush);
    void setRubberBand(bool value);

    void setAvailableCorners(ResizeCornersFlags flags);

    void setCoord(const QPoint &coord);
    void setSize(const QSize &size);

    QSize getSize() const;

    QUndoStack *undoStack();
    void setUndoStack(QUndoStack *stack);

    ResStyle *style();

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) Q_DECL_OVERRIDE;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) Q_DECL_OVERRIDE;
    void paintBevel(QPainter *painter, QColor color = Qt::black);
    void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;

    virtual bool childCanMove(const QPointF &newPos, CustomRectItem *item);
    virtual bool canResize(const QRectF &newRect, const ResizeCorners &corner) const;

    const QBrush &getBrush() const;

    void drawIntersects(QPainter *painter);

    QSize gridSize() const;

private:
    void init();
    QRubberBand *rubberBand();
    bool mousePosOnHandles(QPointF pos);
    void updateSizePos();
    void insertUndoRedoMove();
    QVector<QRectF> m_ResizeHandles;
    QRubberBand *pRubberBand;
    QPoint startDrag;

    //QPointF m_MousePressPoint;
    QMap<QGraphicsItem*, QPointF> m_MousePressPoint;

    bool m_HasRubberBand;
    QBrush m_brush;
    QRectF m_BoundingRect, m_OldBoundingRect;
    QRectF m_ActualRect;
    bool m_IsResizing, m_IsSelection;
    bool m_MousePressed;
    ResizeCorners m_ResizeCorner;
    QFlags<ResizeCorners> m_AvailableCorners;

    QPoint m_Coord;
    QSize m_Size;

    QUndoStack *m_pUndoStack;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(CustomRectItem::ResizeCornersFlags)
#endif // CUSTOMRECTITEM_H
