#ifndef UNDOITEMDELETE_H
#define UNDOITEMDELETE_H

#include <QUndoCommand>
#include <QUuid>
#include <QPointer>

class StdEditorScene;
class UndoItemDelete : public QUndoCommand
{
public:
    UndoItemDelete(StdEditorScene *scene, const QUuid &uuid, QUndoCommand *parent = nullptr);
    virtual ~UndoItemDelete();

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

private:
    QByteArray m_Data;

    StdEditorScene *m_pScene;
    QUuid m_ItemId, m_ParentItemId;

    int m_TypeId;
    QMetaObject *m_MetaObject;
};

#endif // UNDOITEMDELETE_H
