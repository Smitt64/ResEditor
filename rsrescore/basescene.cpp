#include "basescene.h"
#include "styles/resstyle.h"
#include <QPainter>

BaseScene::BaseScene(QObject *parent)
    : QGraphicsScene{parent}
{
    m_pStyle = new ResStyle();
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
