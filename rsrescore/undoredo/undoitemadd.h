#ifndef UNDOITEMADD_H
#define UNDOITEMADD_H

#include "qgraphicsitem.h"
#include <QUuid>
#include <QUndoCommand>

class BaseScene;
class CustomRectItem;
class QGraphicsItem;
class UndoItemAdd : public QUndoCommand
{
public:
    UndoItemAdd(BaseScene *scene, QUndoCommand *parent = nullptr);
    virtual ~UndoItemAdd();

    void setData(const QByteArray &data);
    void setOffset(const QPoint &offset);
    void setItemsListPtr(QList<QGraphicsItem*> *ptr);

    virtual void redo() Q_DECL_OVERRIDE;
    virtual void undo() Q_DECL_OVERRIDE;

private:
    void calcPasteOffest();
    CustomRectItem *m_pPanel;
    QByteArray m_Data;
    BaseScene *m_pScene;

    QList<QGraphicsItem*> *m_ItemsCreated;
    QMap<QString,QUuid> m_CreatedItemUids;
    QPoint m_Offset, m_PasteOffset;
};

#endif // UNDOITEMADD_H
