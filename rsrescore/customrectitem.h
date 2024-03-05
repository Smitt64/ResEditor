#ifndef CUSTOMRECTITEM_H
#define CUSTOMRECTITEM_H

#include <QGraphicsObject>
#include "math.h"
#include <QBrush>
#include <QUuid>
#include <QJsonArray>

#define CLASSINFO_UNDOREDO "UNDOREDO"
#define CLASSINFO_PROPERTYLIST "PROPERTYLIST"
#define CLASSINFO_PROPERTYGROUP "PROPERTYGROUP"

#define checkPropSame(name,value) if (checkPropSameValue(name,value)) return

class QRubberBand;
class QUndoStack;
class ResStyle;
class PropertyModel;
class BaseScene;
class QUndoCommand;
class CustomRectItem : public QGraphicsObject
{
    Q_OBJECT
    Q_PROPERTY(QRect geometry READ geometry WRITE setGeometry NOTIFY geometryChanged)
    Q_PROPERTY(QSize size READ getSize WRITE setSize NOTIFY geometryChanged)
    Q_PROPERTY(QPoint point READ getPoint WRITE setCoord NOTIFY geometryChanged)

    Q_CLASSINFO(CLASSINFO_PROPERTYLIST, ":/json/CustomRectItem.json")
    Q_CLASSINFO(CLASSINFO_PROPERTYGROUP, "Common")

    friend class UndoItemMove;
    friend class UndoItemAdd;
    friend class UndoPropertyChange;
public:
    enum UserAction
    {
        ActionKeyEnter = 0,
        UserValue,
    };

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
        ROTATE = 1 << 9,

        ALL_NO_ROTATE = TOP_LEFT | TOP | TOP_RIGHT | RIGHT |
            BOTTOM_RIGHT | BOTTOM | BOTTOM_LEFT |
            LEFT,
        ALL = ALL_NO_ROTATE | ROTATE
    };
    Q_DECLARE_FLAGS(ResizeCornersFlags, ResizeCorners)

    Q_INVOKABLE CustomRectItem(QGraphicsItem* parent = nullptr);
    CustomRectItem(const QRect& rect, QGraphicsItem* parent = nullptr);
    QRectF boundingRect() const Q_DECL_OVERRIDE;

    void setBrush(const QBrush &brush);
    void setRubberBand(bool value);

    void setAvailableCorners(ResizeCornersFlags flags);
    void setAvailableCorners(ResizeCorners flag, bool enable);
    const ResizeCornersFlags &availableCorners() const;

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
    virtual void serialize(QJsonObject &data);
    virtual void deserialize(const QByteArray &data);
    virtual void deserialize(const QJsonObject &data);

    virtual void renderToPixmap(QPixmap **pix, const QPointF &offset = QPointF());

    QPoint realCoordToEw(const QPointF &point);
    QPointF ewCoordToReal(const QPoint &point);

    virtual QVariant userAction(const qint32 &action, const QVariant &param = QVariant());

    const bool &isCanIntersects() const;
    const bool &isMousePressed() const;
    void setCanIntersects(const bool &flag);

    bool setSkipUndoStack(const bool &value);
    const bool &isSkipUndoStack() const;

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
    virtual void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

    virtual bool childCanMove(const QPointF &newPos, CustomRectItem *item);
    virtual bool canResize(const QRectF &newRect, const ResizeCorners &corner) const;
    virtual bool checkPropSameValue(const QString &propertyName, const QVariant &value);

    virtual bool isIntersects(const QRectF &thisBound, QGraphicsItem *item, const QRectF &itemBound) const;
    virtual void createItemResizeUndoObj(BaseScene* customScene,
                                         const QSizeF &Actual,
                                         const QSizeF &New,
                                         QUndoCommand **cmd);

    const QBrush &getBrush() const;
    const bool &isResizing() const;
    const QRectF &actualRect() const;
    void setBoundingRect(const QRectF &bound);

    void drawIntersects(QPainter *painter);
    bool setSkipRenderIntersects(bool value);

    QSize gridSize() const;
    QString getClassInfo(const QMetaObject *obj, const char *name) const;
    void readProperties(const QJsonObject &obj);

    void createFromJson(CustomRectItem *parent, const QByteArray &data, QList<QGraphicsItem*> &items);

    void afterReleaseMouse(QGraphicsSceneMouseEvent *event = nullptr);
    void insertUndoRedoMove();
    virtual void onInsertUndoRedoMove(const QMap<CustomRectItem *, QPointF> &MousePressPoint);

private:
    void init();
    void serializeProperty(QJsonObject &obj, const QMetaObject *meta, const QString &propertyName);
    void deserializeProperty(QJsonObject &obj);
    QRubberBand *rubberBand();
    bool mousePosOnHandles(QPointF pos);
    void updateSizePos(const QString &from);

    QVector<QRectF> m_ResizeHandles;
    QRubberBand *pRubberBand;
    QPoint startDrag;

    //QPointF m_MousePressPoint;
    QMap<QGraphicsItem*, QPointF> m_MousePressPoint;

    bool m_HasRubberBand, m_CanIntersects, m_SkipRenderIntersects;;
    QBrush m_brush;
    QRectF m_BoundingRect, m_OldBoundingRect;
    QRectF m_ActualRect;
    bool m_IsResizing, m_IsSelection;
    bool m_MousePressed;
    QPointF m_MousePressedPos;
    ResizeCorners m_ResizeCorner;
    QFlags<ResizeCorners> m_AvailableCorners;

    QPoint m_Coord;
    QSize m_Size;

    QUuid m_ItemId;

    QUndoStack *m_pUndoStack;
    PropertyModel *m_pPropertyModel;
    bool m_SkipUndoStack;
};

template<class T>QVector<T> fromJsonArray(const QJsonArray &arr)
{
    QVector<T> result;
    result.reserve(arr.size());

    for (const auto &element : arr)
    {
        QVariant var = element.toVariant();
        result.push_back(var.value<T>());
    }

    return result;
}

Q_DECLARE_OPAQUE_POINTER(CustomRectItem)

Q_DECLARE_OPERATORS_FOR_FLAGS(CustomRectItem::ResizeCornersFlags)
#endif // CUSTOMRECTITEM_H
