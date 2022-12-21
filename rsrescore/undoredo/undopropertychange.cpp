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

QString UndoPropertyChange::ChangePropertyMsg(const QString &property, const QMetaObject *obj)
{
    if (obj)
    {
        int typeID = obj->indexOfClassInfo(CLASSINFO_UNDOREDO);

        if (typeID >= 0)
        {
            QString objtype = obj->classInfo(typeID).value();
            return QObject::tr("%1: Изменение свойства [%2]").arg(objtype, property);
        }
    }

    return QObject::tr("Изменение свойства [%1]").arg(property);
}

void UndoPropertyChange::setPropertyName(const QString &name)
{
    m_PropertyName = name;

    CustomRectItem *pItem = m_pScene->findItem(m_ItemId);
    QString msg = ChangePropertyMsg(m_PropertyName, pItem->metaObject());
    setText(msg);
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
