#ifndef CUSTOMRECTITEM_H
#define CUSTOMRECTITEM_H

#include <QGraphicsObject>
#include "math.h"
#include <QBrush>
#include <QUuid>

#define CLASSINFO_UNDOREDO "UNDOREDO"
#define CLASSINFO_PROPERTYLIST "PROPERTYLIST"
#define CLASSINFO_PROPERTYGROUP "PROPERTYGROUP"
#define CLASSINFO_SERIALIZE_PROP "PROPERTYSERIALIZE"

class QRubberBand;
class QUndoStack;
class ResStyle;
class PropertyModel;
class CustomRectItem : public QGraphicsObject
{
    Q_OBJECT
    Q_PROPERTY(QRect geometry READ geometry WRITE setGeometry NOTIFY geometryChanged)
    Q_PROPERTY(QSize size READ getSize WRITE setSize NOTIFY geometryChanged)
    Q_PROPERTY(QPoint point READ getPoint WRITE setCoord NOTIFY geometryChanged)

    Q_CLASSINFO(CLASSINFO_PROPERTYLIST, ":/json/CustomRectItem.json")
    Q_CLASSINFO(CLASSINFO_PROPERTYGROUP, "Common")
    Q_CLASSINFO(CLASSINFO_SERIALIZE_PROP, "size;point")

    friend class UndoItemMove;
    friend class UndoPropertyChange;
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

    Q_INVOKABLE CustomRectItem(QGraphicsItem* parent = nullptr);
    CustomRectItem(const QRect& rect, QGraphicsItem* parent = nullptr);
    QRectF boundingRect() const Q_DECL_OVERRIDE;

    void setBrush(const QBrush &brush);
    void setRubberBand(bool value);

    void setAvailableCorners(ResizeCornersFlags flags);

    QRect geometry() const;
    void setGeometry(const QRect &_geometry);

    void setCoord(const QPoint &coord);
    void setSize(const QSize &size);
    void setSize(const QSizeF &size);

    QSize getSize() const;
    QPoint getPoint() const;

    QUndoStack *undoStack();
    void setUndoStack(QUndoStack *stack);

    ResStyle *style() const;

    const QUuid &uuid() const;
    void setUuid(const QUuid &_uuid);
    QString undoRedoType() const;

    PropertyModel *propertyModel();

    virtual void serialize(QByteArray &data);
    virtual void deserialize(const QByteArray &data);

signals:
    void geometryChanged();

protected:
    void pushUndoPropertyData(const QString &propertyName, const QVariant &_newValue);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) Q_DECL_OVERRIDE;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) Q_DECL_OVERRIDE;
    void paintBevel(QPainter *painter, QColor color = Qt::black);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;

    virtual bool childCanMove(const QPointF &newPos, CustomRectItem *item);
    virtual bool canResize(const QRectF &newRect, const ResizeCorners &corner) const;

    const QBrush &getBrush() const;
    const bool &isResizing() const;

    void drawIntersects(QPainter *painter);

    bool setSkipUndoStack(const bool &value);
    const bool &isSkipUndoStack() const;

    QSize gridSize() const;
    QString getClassInfo(const QMetaObject *obj, const char *name) const;
    void readProperties(const QJsonObject &obj);

private:
    void init();
    void serializeProperty(QJsonObject &obj, const QMetaObject *meta, const QString &propertyName);
    void deserializeProperty(QJsonObject &obj);
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

    QUuid m_ItemId;

    QUndoStack *m_pUndoStack;
    PropertyModel *m_pPropertyModel;
    bool m_SkipUndoStack;
};

Q_DECLARE_OPAQUE_POINTER(CustomRectItem)

Q_DECLARE_OPERATORS_FOR_FLAGS(CustomRectItem::ResizeCornersFlags)
#endif // CUSTOMRECTITEM_H
