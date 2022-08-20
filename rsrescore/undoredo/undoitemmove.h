#ifndef UNDOITEMMOVE_H
#define UNDOITEMMOVE_H

#include <QUndoCommand>
#include <QUuid>

class CustomRectItem;
class BaseScene;
class UndoItemMove : public QUndoCommand
{
public:
    UndoItemMove(BaseScene *scene, const QUuid &uuid, QUndoCommand *parent = nullptr);
    virtual ~UndoItemMove();

    void setPositions(const QPointF &old, const QPointF &new_);

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

private:
    BaseScene *m_pScene;

    QUuid m_ItemId;
    QPointF m_OldPos, m_NewPos, m_Delta;
};

#endif // UNDOITEMMOVE_H
