#include "undoitemresize.h"
#include "basescene.h"
#include "customrectitem.h"
#include "scrolitem.h"
#include <QMetaClassInfo>

UndoItemResize::UndoItemResize(BaseScene *scene, const QUuid &uuid, QUndoCommand *parent) :
    QUndoCommand(parent),
    m_ItemId(uuid),
    m_pScene(scene)
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

BaseScene *UndoItemResize::scene()
{
    return m_pScene;
}

// --------------------------------------------------------------------------

UndoItemResizeScrol::UndoItemResizeScrol(BaseScene *scene, const QUuid &uuid, QUndoCommand *parent) :
    UndoItemResize(scene, uuid, parent)
{

}

UndoItemResizeScrol::~UndoItemResizeScrol()
{

}

void UndoItemResizeScrol::setSizes(const QSizeF &old, const QSizeF &new_)
{
    UndoItemResize::setSizes(old, new_);

    QSizeF delta = new_ - old;
    QSize grid = scene()->getGridSize();
    ScrolItem *pItem = scene()->findItemObj<ScrolItem>(m_ItemId);

    m_OldSize = QSize(pItem->rowLength(), pItem->rowNum());
    m_NewSize = QSize(pItem->rowLength() + delta.width() / grid.width(),
                      pItem->rowNum() + delta.height() / grid.height());
}

void UndoItemResizeScrol::redo()
{
    UndoItemResize::redo();
    ScrolItem *pItem = scene()->findItemObj<ScrolItem>(m_ItemId);

    bool old_skip = pItem->setSkipUndoStack(true);
    pItem->setRowLength(m_NewSize.width());
    pItem->setRowNum(m_NewSize.height());
    pItem->setSkipUndoStack(old_skip);
}

void UndoItemResizeScrol::undo()
{
    UndoItemResize::undo();

    ScrolItem *pItem = scene()->findItemObj<ScrolItem>(m_ItemId);

    bool old_skip = pItem->setSkipUndoStack(true);
    pItem->setRowLength(m_OldSize.width());
    pItem->setRowNum(m_OldSize.height());
    pItem->setSkipUndoStack(old_skip);
}
