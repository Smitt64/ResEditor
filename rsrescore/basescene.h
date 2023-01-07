#ifndef BASESCENE_H
#define BASESCENE_H

#include "qgraphicsitem.h"
#include "rsrescore_global.h"
#include <QGraphicsScene>
#include <QUuid>
#include <QMap>

class QPainter;
class ResStyle;
class CustomRectItem;
class QAbstractItemModel;
class RSRESCORE_EXPORT BaseScene : public QGraphicsScene
{
    Q_OBJECT
    friend class CustomRectItem;
public:
    typedef QMap<CustomRectItem*, QPointF> CustomRectItemPoints;
    explicit BaseScene(QObject *parent = nullptr);
    QSize getGridSize() const;

    ResStyle *style();

    CustomRectItem *findItem(const QUuid &uuid) const;
    CustomRectItem *findTopLevelItem() const;

    void insertMousePressPoint(CustomRectItem *rectItem);
    const CustomRectItemPoints &mousePressPoints() const;
    void clearMousePressPoints();

    template<class T> QList<T> findItems()
    {
        QList<T> result;
        QList<QGraphicsItem*> tmplst = items();

        for (QGraphicsItem *item : tmplst)
        {
            T *converted = dynamic_cast<T*>(item);

            if (converted)
                result.append(converted);
        }

        return result;
    }

    template<class T> QList<T*> findItemsIntersect(const QRectF &sceneRect)
    {
        QList<T*> result;
        QList<QGraphicsItem*> tmplst = items();

        for (QGraphicsItem *item : tmplst)
        {
            T *converted = dynamic_cast<T*>(item);

            if (converted)
            {
                QRectF bound = item->mapRectToScene(item->boundingRect());

                if (sceneRect.intersects(bound))
                    result.append(converted);
            }
        }

        return result;
    }

signals:
    void propertyModelChanged(QAbstractItemModel *model);

protected:
    virtual void drawBackground (QPainter* painter, const QRectF &rect) Q_DECL_OVERRIDE;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;

private:
    ResStyle *m_pStyle;

    CustomRectItemPoints m_MousePressPoint;
};

#endif // BASESCENE_H
