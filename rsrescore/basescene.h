#ifndef BASESCENE_H
#define BASESCENE_H

#include "rsrescore_global.h"
#include "propertymodel/propertymodel.h"
#include <QGraphicsScene>
#include <QUuid>

class QPainter;
class ResStyle;
class CustomRectItem;
class RSRESCORE_EXPORT BaseScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit BaseScene(QObject *parent = nullptr);
    QSize getGridSize() const;

    ResStyle *style();

    CustomRectItem *findItem(const QUuid &uuid) const;

signals:
    void propertyModelChanged(QAbstractItemModel *model);

protected:
    virtual void drawBackground (QPainter* painter, const QRectF &rect) Q_DECL_OVERRIDE;

private:
    ResStyle *m_pStyle;
};

#endif // BASESCENE_H
