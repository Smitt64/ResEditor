#include "scrolitem.h"
#include "qgraphicsscene.h"
#include "respanel.h"

ScrolItem::ScrolItem(CustomRectItem* parent) :
    PanelItem(parent),
    m_RowNum(0),
    m_RowLength(0),
    m_RowHeight(0)
{

}

ScrolItem::~ScrolItem()
{

}

void ScrolItem::setPanel(ResPanel *panel, const QString &comment)
{
    QRect scrol = panel->scrol();
    PanelItem::setPanel(panel, comment);

    m_RowNum = scrol.height();
    m_RowLength = scrol.width();
    m_RowHeight = panel->rowHaight();
    m_ScrolPos = panel->scrolPos();
    //m_RowNum = panel->
}

const quint16 &ScrolItem::rowNum() const
{
    return m_RowNum;
}

void ScrolItem::setRowNum(const quint16 &val)
{
    checkPropSame("rowNum", val);

    if (isSkipUndoStack() || !undoStack())
    {
        m_RowNum = val;
        emit rowNumChanged();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("rowNum", val);
}

const quint16 &ScrolItem::rowLength() const
{
    return m_RowLength;
}

void ScrolItem::setRowLength(const quint16 &val)
{
    checkPropSame("rowLength", val);

    if (isSkipUndoStack() || !undoStack())
    {
        m_RowLength = val;
        emit rowLengthChanged();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("rowLength", val);
}

const quint16 &ScrolItem::rowHeight() const
{
    return m_RowHeight;
}

void ScrolItem::setRowHeight(const quint16 &val)
{
    checkPropSame("rowHeight", val);

    if (isSkipUndoStack() || !undoStack())
    {
        m_RowHeight = val;
        emit rowHeightChanged();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("rowHeight", val);
}

const QPoint &ScrolItem::scrolPos() const
{
    return m_ScrolPos;
}

void ScrolItem::setScrolPos(const QPoint &val)
{
    checkPropSame("scrolPos", val);

    if (isSkipUndoStack() || !undoStack())
    {
        m_ScrolPos = val;
        emit scrolPosChanged();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("scrolPos", val);
}
