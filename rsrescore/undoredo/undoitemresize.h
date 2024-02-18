#ifndef UNDOITEMRESIZE_H
#define UNDOITEMRESIZE_H

#include <QUndoCommand>
#include <QUuid>
#include <QSizeF>

class CustomRectItem;
class BaseScene;
class UndoItemResize : public QUndoCommand
{
public:
    UndoItemResize(BaseScene *scene, const QUuid &uuid, QUndoCommand *parent = nullptr);
    virtual ~UndoItemResize();

    virtual void setSizes(const QSizeF &old, const QSizeF &new_);

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

    BaseScene *scene();

protected:
    QUuid m_ItemId;
    QSizeF m_OldSize, m_NewSize;

private:
    BaseScene *m_pScene;
};

class UndoItemResizeScrol : public UndoItemResize
{
public:
    UndoItemResizeScrol(BaseScene *scene, const QUuid &uuid, QUndoCommand *parent = nullptr);
    virtual ~UndoItemResizeScrol();

    virtual void setSizes(const QSizeF &old, const QSizeF &new_) Q_DECL_OVERRIDE;

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

private:
    QSize m_OldSize, m_NewSize;
};

#endif // UNDOITEMRESIZE_H
