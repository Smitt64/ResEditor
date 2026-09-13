#ifndef STDEDITORSCENE_H
#define STDEDITORSCENE_H

#include "BaseScene.h"
#include <QObject>
#include <QTimer>
#include <QPointF>
#include <QSize>
#include <QGraphicsItem>
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>

class ControlItemsWrapper;
class StdEditorScene : public BaseScene
{
    Q_OBJECT

public:
    explicit StdEditorScene(QObject *parent = nullptr);
    virtual ~StdEditorScene();

    ControlItemsWrapper *controlItemsWrapper();
    const QPointF &cursorPos() const;

    virtual void sceneItemPosChanged() Q_DECL_OVERRIDE;

    void setCursorPosition(const QPointF &scenePos);

protected:
    virtual void handleSelectionChanged(const QList<QGraphicsItem*> &selectedItems) Q_DECL_OVERRIDE;
    virtual void drawBackground(QPainter* painter, const QRectF &rect) Q_DECL_OVERRIDE;
    virtual void drawForeground(QPainter *painter, const QRectF &rect) Q_DECL_OVERRIDE;
    virtual void keyPressEvent(QKeyEvent *keyEvent) Q_DECL_OVERRIDE;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE;

private:
    bool m_fShowCursor;
    QTimer *m_pCursorTimer;
    QPointF m_CursorPos;
    ControlItemsWrapper *m_controlItemsWrapper;
};

#endif // STDEDITORSCENE_H
