#ifndef BASESCENE_H
#define BASESCENE_H

#include "rsrescore_global.h"
#include <QGraphicsScene>

class QPainter;
class ResStyle;
class RSRESCORE_EXPORT BaseScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit BaseScene(QObject *parent = nullptr);
    QSize getGridSize() const;

    ResStyle *style();

protected:
    void drawBackground (QPainter* painter, const QRectF &rect);

private:
    ResStyle *m_pStyle;
};

#endif // BASESCENE_H
