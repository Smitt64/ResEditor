#include "undopropertychange.h"
#include "customrectitem.h"
#include "basescene.h"
#include <QMetaObject>
#include <QMetaClassInfo>

UndoPropertyChange::UndoPropertyChange(BaseScene *scene, const QUuid &uuid, QUndoCommand *parent) :
    QUndoCommand(parent),
    m_pScene(scene),
    m_ItemId(uuid)
{

}

UndoPropertyChange::~UndoPropertyChange()
{

}

void UndoPropertyChange::setValues(const QVariant &old, const QVariant &new_)
{
    m_OldValue = old;
    m_NewValue = new_;
}

void UndoPropertyChange::setPropertyName(const QString &name)
{
    m_PropertyName = name;

    CustomRectItem *pItem = m_pScene->findItem(m_ItemId);
    if (pItem)
    {
        int typeID = pItem->metaObject()->indexOfClassInfo(CLASSINFO_UNDOREDO);
        if (typeID >= 0)
        {
            QString objtype = pItem->metaObject()->classInfo(typeID).value();
            setText(QObject::tr("%1: Изменение свойства [%2]").arg(objtype, m_PropertyName));
        }
        else
            setText(QObject::tr("Изменение свойства [%1]").arg(m_PropertyName));
    }
}

void UndoPropertyChange::redo()
{
    CustomRectItem *pItem = m_pScene->findItem(m_ItemId);
    if (pItem)
    {
        bool old_skip = pItem->setSkipUndoStack(true);
        pItem->setProperty(m_PropertyName.toLocal8Bit().data(), m_NewValue);
        pItem->setSkipUndoStack(old_skip);
    }
}

void UndoPropertyChange::undo()
{
    CustomRectItem *pItem = m_pScene->findItem(m_ItemId);
    if (pItem)
    {
        bool old_skip = pItem->setSkipUndoStack(true);
        pItem->setProperty(m_PropertyName.toLocal8Bit().data(), m_OldValue);
        pItem->setSkipUndoStack(old_skip);
    }
}
