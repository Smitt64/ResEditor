#ifndef UNDOITEMMOVE_H
#define UNDOITEMMOVE_H

#include <QUndoCommand>

class CustomRectItem;
class UndoItemMove : public QUndoCommand
{
public:
    UndoItemMove(CustomRectItem *item, QUndoCommand *parent = nullptr);
    virtual ~UndoItemMove();

    void setPositions(const QPointF &old, const QPointF &new_);

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

private:
    CustomRectItem *m_pItem;
    QPointF m_OldPos, m_NewPos, m_Delta;

};

#endif // UNDOITEMMOVE_H
