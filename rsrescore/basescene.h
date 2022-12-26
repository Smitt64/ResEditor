#ifndef BASESCENE_H
#define BASESCENE_H

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
