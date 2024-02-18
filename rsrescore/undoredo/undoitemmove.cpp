#include "undoitemmove.h"
#include "customrectitem.h"
#include "basescene.h"
#include <QMetaClassInfo>

UndoItemMove::UndoItemMove(BaseScene *scene, const QUuid &uuid, QUndoCommand *parent) :
    QUndoCommand(parent),
    m_pScene(scene),
    m_ItemId(uuid)
{
}

UndoItemMove::~UndoItemMove()
{

}

void UndoItemMove::setPositions(const QPointF &old, const QPointF &new_)
{
    m_OldPos = old;
    m_NewPos = new_;
    m_Delta = new_ - old;

    CustomRectItem *pItem = m_pScene->findItem(m_ItemId);
    if (pItem)
    {
        int typeID = pItem->metaObject()->indexOfClassInfo(CLASSINFO_UNDOREDO);
        if (typeID >= 0)
        {
            QString objtype = pItem->metaObject()->classInfo(typeID).value();
            setText(QObject::tr("%1: Изменение положения").arg(objtype));
        }
        else
            setText(QObject::tr("Изменение положения"));
    }
}

void UndoItemMove::redo()
{
    CustomRectItem *pItem = m_pScene->findItem(m_ItemId);

    if (pItem)
    {
        pItem->setPos(m_NewPos);
        pItem->updateSizePos("UndoItemMove::redo");
    }

    m_pScene->update();
}

void UndoItemMove::undo()
{
    CustomRectItem *pItem = m_pScene->findItem(m_ItemId);

    if (pItem)
    {
        pItem->setPos(m_OldPos);
        pItem->updateSizePos("UndoItemMove::undo");
    }

    m_pScene->update();
}
