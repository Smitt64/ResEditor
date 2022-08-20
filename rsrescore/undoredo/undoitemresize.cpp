#include "undoitemresize.h"
#include "basescene.h"
#include "customrectitem.h"
#include <QMetaClassInfo>

UndoItemResize::UndoItemResize(BaseScene *scene, const QUuid &uuid, QUndoCommand *parent) :
    QUndoCommand(parent),
    m_pScene(scene),
    m_ItemId(uuid)
{

}

UndoItemResize::~UndoItemResize()
{

}


void UndoItemResize::setSizes(const QSizeF &old, const QSizeF &new_)
{
    m_OldSize = old;
    m_NewSize = new_;

    CustomRectItem *pItem = m_pScene->findItem(m_ItemId);
    if (pItem)
    {
        int typeID = pItem->metaObject()->indexOfClassInfo(CLASSINFO_UNDOREDO);
        if (typeID >= 0)
        {
            QString objtype = pItem->metaObject()->classInfo(typeID).value();
            setText(QObject::tr("%1: Изменение размера").arg(objtype));
        }
        else
            setText(QObject::tr("Изменение размера"));
    }
}

void UndoItemResize::redo()
{
    CustomRectItem *pItem = m_pScene->findItem(m_ItemId);

    if (pItem)
        pItem->setSize(m_NewSize);
}

void UndoItemResize::undo()
{
    CustomRectItem *pItem = m_pScene->findItem(m_ItemId);

    if (pItem)
        pItem->setSize(m_OldSize);
}
