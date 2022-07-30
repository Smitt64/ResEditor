#include "baseeditorview.h"
#include "basescene.h"
#include <QResizeEvent>
#include <QtDebug>

BaseEditorView::BaseEditorView(QWidget *parent) :
    QGraphicsView(parent)
{
    setAlignment(Qt::AlignTop|Qt::AlignLeft);
}

BaseEditorView::~BaseEditorView()
{

}

QSize BaseEditorView::gridSize() const
{
    return pScene->getGridSize();
}

void BaseEditorView::setupScene()
{
    pScene = new BaseScene(this);
    setScene(pScene);
    pScene->setSceneRect(QRectF(0, 0, width(), height()));
}

void BaseEditorView::resizeEvent(QResizeEvent *event)
{
    const QSize &sz = event->size();
    pScene->setSceneRect(QRectF(0, 0, sz.width(), sz.height()));
}
