#include "undoitemadd.h"
#include "customrectitem.h"
#include "basescene.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMetaType>

UndoItemAdd::UndoItemAdd(BaseScene *scene, QUndoCommand *parent) :
    QUndoCommand(parent),
    m_pScene(scene),
    m_ItemsCreated(nullptr),
    m_Offset(QPoint(0, 0))
{
    m_pPanel = m_pScene->findTopLevelItem();

    setText(QObject::tr("Добавление элементов на форму"));
}

UndoItemAdd::~UndoItemAdd()
{

}

void UndoItemAdd::setData(const QByteArray &data)
{
    m_Data = data;
    calcPasteOffest();
}

void UndoItemAdd::calcPasteOffest()
{
    QPoint pt;
    pt.setX(std::numeric_limits<decltype(pt.x())>::max());
    pt.setY(std::numeric_limits<decltype(pt.y())>::max());

    QJsonDocument doc = QJsonDocument::fromJson(m_Data);
    QJsonObject root = doc.object();

    QJsonArray jsitems = root["items"].toArray();
    for (const QJsonValue &element : qAsConst(jsitems))
    {
        QJsonObject obj = element.toObject();
        QJsonArray jsproperties = obj["properties"].toArray();

        for (const QJsonValue &propelement : qAsConst(jsproperties))
        {
            QJsonObject prop = propelement.toObject();

            if (prop["property"].toString() == "point")
            {
                QVector<int> val = fromJsonArray<int>(prop["value"].toArray());
                pt.setX(qMin(pt.x(), val[0]));
                pt.setY(qMin(pt.y(), val[1]));
            }
        }
    }

    if (pt.x() != std::numeric_limits<decltype(pt.x())>::max())
        m_PasteOffset.setX(pt.x());

    if (pt.y() != std::numeric_limits<decltype(pt.y())>::max())
        m_PasteOffset.setY(pt.y());
}

void UndoItemAdd::setOffset(const QPoint &offset)
{
    m_Offset = offset;
}

void UndoItemAdd::setItemsListPtr(QList<QGraphicsItem*> *ptr)
{
    m_ItemsCreated = ptr;
}

void UndoItemAdd::redo()
{
    QJsonDocument doc = QJsonDocument::fromJson(m_Data);
    QJsonObject root = doc.object();

    QJsonArray jsitems = root["items"].toArray();
    for (const QJsonValue &element : qAsConst(jsitems))
    {
        QJsonObject obj = element.toObject();
        QString classname = obj["class"].toString();

        int type = QMetaType::type(classname.toLocal8Bit() + "*");
        const QMetaObject *metaobj = QMetaType::metaObjectForType(type);

        QObject *newObject = metaobj->newInstance(Q_ARG(QGraphicsItem*, m_pPanel));
        CustomRectItem *rectItem = qobject_cast<CustomRectItem*>(newObject);

        if (rectItem)
        {
            bool oldSkip = rectItem->setSkipUndoStack(true);
            rectItem->deserialize(obj);

            QPoint originalPos = rectItem->getPoint();
            QPoint pos = originalPos + m_Offset - m_PasteOffset;
            QRect rc = rectItem->geometry();
            rc.moveTo(pos);
            rectItem->setCoord(pos);

            QString uuid = obj["uuid"].toString();
            if (m_CreatedItemUids.contains(uuid))
                rectItem->setUuid(m_CreatedItemUids[uuid]);
            else
                m_CreatedItemUids.insert(uuid, rectItem->uuid());

            rectItem->setSkipUndoStack(oldSkip);

            if (m_ItemsCreated)
                m_ItemsCreated->append(rectItem);

            rectItem->update();
        }
        else
            delete newObject;
    }

    m_pScene->update();
}

void UndoItemAdd::undo()
{
    for (const QUuid &uuid : qAsConst(m_CreatedItemUids))
    {
        CustomRectItem *pItem = m_pScene->findItem(uuid);

        if (!pItem)
            continue;

        m_pScene->removeItem(pItem);
        pItem->deleteLater();
    }
}
