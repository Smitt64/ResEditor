#include "undoitemmove.h"
#include "customrectitem.h"
#include <QMetaClassInfo>

UndoItemMove::UndoItemMove(CustomRectItem *item, QUndoCommand *parent) :
    QUndoCommand(parent),
    m_pItem(item)
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

    int typeID = m_pItem->metaObject()->indexOfClassInfo(CLASSINFO_UNDOREDO);
    if (typeID >= 0)
    {
        QString objtype = m_pItem->metaObject()->classInfo(typeID).value();
        setText(QObject::tr("%1: Изменение положения").arg(objtype));
    }
    else
        setText(QObject::tr(" Изменение положения"));
}

void UndoItemMove::redo()
{
    m_pItem->setPos(m_NewPos);
    m_pItem->updateSizePos();
}

void UndoItemMove::undo()
{
    m_pItem->setPos(m_OldPos);
    m_pItem->updateSizePos();
}
