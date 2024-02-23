#include "scrolitem.h"
#include "panelpropertysdlg.h"
#include "qgraphicsscene.h"
#include "respanel.h"
#include "undoredo/undoitemresize.h"
#include <QStyleOptionGraphicsItem>
#include <QUndoStack>
#include <QGraphicsView>

ScrolAreaRectItem::ScrolAreaRectItem(CustomRectItem* parent) :
    CustomRectItem(parent)
{
    m_Scrol = qobject_cast<ScrolItem*>(parent);
    setCoord(m_Scrol->scrolPos());
    setSize(QSize(m_Scrol->rowLength(), m_Scrol->rowNum()));

    setZValue(1000);
    setOpacity(0.7);
    setUndoStack(nullptr);
    setCanIntersects(false);

    connect(this, &ScrolAreaRectItem::geometryChanged, [=]() -> void
    {
        QRect rc = geometry();
        if (!this->isMousePressed())
        {
            bool fChanged = false;

            if (scrolPos() != rc.topLeft())
                fChanged = true;

            if (!fChanged && rowLength() != rc.width())
                fChanged = true;

            if (!fChanged && rowNum() != rc.height())
                fChanged = true;

            if (fChanged)
            {
                m_Scrol->undoStack()->beginMacro(tr("Изменение области скролинга"));
                m_Scrol->setScrolPos(rc.topLeft());
                m_Scrol->setRowLength(rc.width());
                m_Scrol->setRowNum(rc.height());
                m_Scrol->undoStack()->endMacro();
            }
        }
    });

    auto sizeChanged = [this]() -> void
    {
        bool fChanged = false;

        QRect rc = geometry();
        if (scrolPos() != rc.topLeft())
            fChanged = true;

        if (!fChanged && rowLength() != rc.width())
            fChanged = true;

        if (!fChanged && rowNum() != rc.height())
            fChanged = true;

        blockSignals(true);
        QSize size(m_Scrol->rowLength(), m_Scrol->rowNum());
        setSize(size);
        blockSignals(false);

        emit rowLengthChanged();
        emit rowNumChanged();
    };

    connect(m_Scrol, &ScrolItem::rowLengthChanged, sizeChanged);
    connect(m_Scrol, &ScrolItem::rowNumChanged, sizeChanged);

    connect(m_Scrol, &ScrolItem::scrolPosChanged, [this]() -> void
    {
        blockSignals(true);
        setCoord(m_Scrol->scrolPos());
        blockSignals(false);
        emit scrolPosChanged();
    });
}

ScrolAreaRectItem::~ScrolAreaRectItem()
{

}

const quint16 &ScrolAreaRectItem::rowNum() const
{
    return m_Scrol->rowNum();
}

void ScrolAreaRectItem::setRowNum(const quint16 &val)
{
    m_Scrol->setRowNum(val);

    blockSignals(true);
    QSize size(m_Scrol->rowLength(), m_Scrol->rowNum());
    setSize(size);
    blockSignals(false);
}

const quint16 &ScrolAreaRectItem::rowLength() const
{
    return m_Scrol->rowLength();
}

void ScrolAreaRectItem::setRowLength(const quint16 &val)
{
    m_Scrol->setRowLength(val);

    blockSignals(true);
    QSize size(m_Scrol->rowLength(), m_Scrol->rowNum());
    setSize(size);
    blockSignals(false);
}

const quint16 &ScrolAreaRectItem::rowHeight() const
{
    return m_Scrol->rowHeight();
}

void ScrolAreaRectItem::setRowHeight(const quint16 &val)
{
    m_Scrol->setRowHeight(val);
}

const QPoint &ScrolAreaRectItem::scrolPos() const
{
    return m_Scrol->scrolPos();
}

void ScrolAreaRectItem::setScrolPos(const QPoint &val)
{
    m_Scrol->setScrolPos(val);
}

bool ScrolAreaRectItem::isIntersects(const QRectF &thisBound, QGraphicsItem *item, const QRectF &itemBound) const
{
    return false;
}

void ScrolAreaRectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    ResStyleOption opt;
    opt.init(this);
    opt.rowHeight = m_Scrol->rowHeight();

    style()->drawControl(ResStyle::Control_ScrolArea, painter, &opt);

    CustomRectItem::paint(painter, option, widget);

    painter->save();
    QColor br = style()->color(ResStyle::Color_TextBg, &opt);
    paintBevel(painter, QColor::fromRgb(255 - br.red(), 255 - br.green(), 255 - br.blue()));
    painter->restore();
}

// --------------------------------------------------------------------

ScrolItem::ScrolItem(CustomRectItem* parent) :
    PanelItem(parent),
    m_RowNum(0),
    m_RowLength(0),
    m_RowHeight(0),
    m_ScrolFlags(ScrolFlags())
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

    m_ScrolArea = new ScrolAreaRectItem(this);
    m_ScrolArea->setVisible(false);

    m_ScrolFlags = ScrolFlags(panel->scrolFlags());
    m_Type = (ScrolType)panel->type();
}

const ScrolItem::ScrolType &ScrolItem::scrolType() const
{
    return m_Type;
}

void ScrolItem::setScrolType(const ScrolItem::ScrolType &val)
{
    checkPropSame("scrolType", val);

    if (isSkipUndoStack() || !undoStack())
    {
        m_Type = val;
        emit scrolTypeChanged();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("scrolType", val);
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

const ScrolItem::ScrolFlags &ScrolItem::scrolFlags() const
{
    return m_ScrolFlags;
}

void ScrolItem::setScrolFlags(const ScrolItem::ScrolFlags &val)
{
    checkPropSame("scrolFlags", QVariant::fromValue((int)val));

    if (isSkipUndoStack() || !undoStack())
    {
        m_ScrolFlags = val;
        emit scrolFlagsChanged();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("scrolFlags", QVariant::fromValue((int)val));
}

void ScrolItem::createItemResizeUndoObj(BaseScene* customScene,
                             const QSizeF &Actual,
                             const QSizeF &New,
                             QUndoCommand **cmd)
{
    UndoItemResizeScrol *undocmd = new UndoItemResizeScrol(customScene, uuid());
    undocmd->setSizes(Actual, New);

    *cmd = undocmd;
}

void ScrolItem::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{

}

void ScrolItem::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    bool isScrolAreaVisible = m_ScrolArea->isVisible();
    PanelItem::dragLeaveEvent(event);

    m_ScrolArea->setVisible(isScrolAreaVisible);
}

void ScrolItem::showScrolArea(bool visible)
{
    m_ScrolArea->setVisible(visible);
}

void ScrolItem::FillItemPanel(PanelPropertysDlg &dlg)
{
    setScrolFlags((ScrolItem::ScrolFlags)dlg.srolFlags());
    setScrolPos(dlg.scrolPos());
    setRowNum(dlg.rowNum());
    setRowHeight(dlg.rowHeight());
    setRowLength(dlg.rowLength());
    setScrolType((ScrolType)dlg.scrolType());

    PanelItem::FillItemPanel(dlg);
}

QVariant ScrolItem::userAction(const qint32 &action, const QVariant &param)
{
    if (action == ActionKeyEnter)
    {
        PanelPropertysDlg dlg(scene()->views().first());
        dlg.setWindowTitle(tr("Параметры скролинга"));
        dlg.setFixedHeight(450);
        dlg.setRectItem(this);

        if (dlg.exec() == QDialog::Accepted)
        {
            undoStack()->beginMacro(tr("Изменение параметров скролинга"));
            FillItemPanel(dlg);
            undoStack()->endMacro();
        }
    }
    return QVariant();
}
