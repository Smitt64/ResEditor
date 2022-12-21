#include "undoitemdelete.h"
#include "customrectitem.h"
#include "basescene.h"
#include <QDebug>
#include <QMetaType>
#include <QMetaClassInfo>

UndoItemDelete::UndoItemDelete(BaseScene *scene, const QUuid &uuid, QUndoCommand *parent) :
    QUndoCommand(parent),
    m_pScene(scene),
    m_ItemId(uuid)
{
    CustomRectItem *pItem = m_pScene->findItem(m_ItemId);
    CustomRectItem *pParentItem = dynamic_cast<CustomRectItem*>(pItem->parentItem());

    if (pParentItem)
        m_ParentItemId = pParentItem->uuid();

    QString pointerName = QString("%1*").arg(pItem->metaObject()->className());
    m_TypeId = QMetaType::type(pointerName.toLocal8Bit());
    pItem->serialize(m_Data);

    int typeID = pItem->metaObject()->indexOfClassInfo(CLASSINFO_UNDOREDO);
    if (typeID >= 0)
    {
        QString objtype = pItem->metaObject()->classInfo(typeID).value();
        setText(QObject::tr("%1: Удаление элемента").arg(objtype));
    }
    else
        setText(QObject::tr("Удаление элемента"));
}

UndoItemDelete::~UndoItemDelete()
{

}

void UndoItemDelete::redo()
{
    CustomRectItem *pItem = m_pScene->findItem(m_ItemId);

    if (m_TypeId == QMetaType::UnknownType)
        return;

    if (!pItem)
        qDebug() << "UndoItemDelete::redo: can't find item " << m_ItemId;
    else
    {
        m_pScene->removeItem(pItem);
        pItem->deleteLater();
    }
}

void UndoItemDelete::undo()
{
    if (m_TypeId == QMetaType::UnknownType)
        return;

    CustomRectItem *pParentItem = m_pScene->findItem(m_ParentItemId);
    const QMetaObject *metaobject = QMetaType::metaObjectForType(m_TypeId);
    QObject *obj = metaobject->newInstance(Q_ARG(QGraphicsItem*, pParentItem));

    CustomRectItem *pItem = qobject_cast<CustomRectItem*>(obj);
    pItem->deserialize(m_Data);
    pItem->setUuid(m_ItemId);
}
