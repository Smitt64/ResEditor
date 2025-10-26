#include "scrolitem.h"
#include "panelpropertysdlg.h"
#include "qgraphicsscene.h"
#include "basescene.h"
#include "respanel.h"
#include "undoredo/undoitemresize.h"
#include "undoredo/undopropertychange.h"
#include <QStyleOptionGraphicsItem>
#include <QUndoStack>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>
#include <QPropertyAnimation>

ScrolAreaRectItem::ScrolAreaRectItem(CustomRectItem* parent) :
    CustomRectItem(parent),
    m_IsResizingRowHeight(false),
    m_RowHeightChanged(false),
    m_HoverOverIndicator(false)
{
    m_Scrol = qobject_cast<ScrolItem*>(parent);
    setCoord(m_Scrol->scrolPos());
    setSize(QSize(m_Scrol->rowLength(), m_Scrol->rowNum()));

    setZValue(1000);
    setOpacity(0.7);
    setUndoStack(nullptr);
    setCanIntersects(false);
    setAcceptHoverEvents(true);

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
    if (!isVisible() || boundingRect().isEmpty())
        return;

    ResStyleOption opt;
    opt.init(this);
    opt.rowHeight = m_Scrol->rowHeight();

    style()->drawControl(ResStyle::Control_ScrolArea, painter, &opt);

    CustomRectItem::paint(painter, option, widget);

    painter->save();
    QColor br = style()->color(ResStyle::Color_TextBg, &opt);
    paintBevel(painter, QColor::fromRgb(255 - br.red(), 255 - br.green(), 255 - br.blue()));

    // Рисуем индикатор текущей высоты строки
    QRectF rect = boundingRect();
    qreal currentHeightPixels = m_Scrol->rowHeight() * style()->gridSize().height();
    qreal indicatorY = rect.top() + currentHeightPixels;

    // Изменяем цвет линии при наведении
    QColor indicatorColor = m_HoverOverIndicator ? QColor(0, 200, 255) : QColor(0, 120, 215);

    // Линия индикатора (увеличиваем толщину для лучшего хвата)
    painter->setPen(QPen(indicatorColor, 4)); // Увеличиваем толщину с 3 до 4
    painter->drawLine(rect.left(), indicatorY, rect.right(), indicatorY);

    // Треугольник-маркер
    painter->setPen(Qt::NoPen);
    painter->setBrush(indicatorColor);
    QPolygonF triangle;
    triangle << QPointF(rect.right() - 10, indicatorY - 5)
             << QPointF(rect.right() - 10, indicatorY + 5)
             << QPointF(rect.right(), indicatorY);
    painter->drawPolygon(triangle);

    // Отображаем текущую высоту строки
    painter->setPen(Qt::white);
    painter->setFont(QFont("Arial", 8));
    QString heightText = QString("%1 стр.").arg(m_Scrol->rowHeight());
    painter->drawText(QRectF(rect.left(), indicatorY - 20, rect.width(), 20),
                      Qt::AlignCenter, heightText);

    painter->restore();
}

void ScrolAreaRectItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        QRectF rect = boundingRect();
        qreal currentHeight = m_Scrol->rowHeight() * style()->gridSize().height();

        // Увеличиваем зону обнаружения с 6 до 10 пикселей
        QRectF heightIndicator(rect.left(), rect.top() + currentHeight - 5, rect.width(), 10);

        if (heightIndicator.contains(event->pos()))
        {
            m_IsResizingRowHeight = true;
            m_StartRowHeight = m_Scrol->rowHeight();
            m_StartMousePos = event->scenePos();
            m_StartTopLeft = sceneBoundingRect().topLeft();
            event->accept();
            return;
        }
    }

    CustomRectItem::mousePressEvent(event);
}

void ScrolAreaRectItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_IsResizingRowHeight)
    {
        BaseScene* customScene = qobject_cast<BaseScene*>(scene());
        QSize gridSize = customScene->getGridSize();

        // Вычисляем изменение позиции мыши
        qreal deltaY = event->scenePos().y() - m_StartMousePos.y();
        qreal deltaHeight = round(deltaY / gridSize.height());
        qreal newRowHeight = m_StartRowHeight + deltaHeight;

        // Ограничения высоты
        if (newRowHeight < 1) newRowHeight = 1;
        if (newRowHeight > 20) newRowHeight = 20;

        // Устанавливаем новую высоту только если изменилась
        if (newRowHeight != m_Scrol->rowHeight())
        {
            // Временно отключаем undo stack для промежуточных изменений
            bool oldSkip = m_Scrol->setSkipUndoStack(true);
            m_Scrol->setRowHeight(newRowHeight);
            m_Scrol->setSkipUndoStack(oldSkip);

            m_RowHeightChanged = true; // Отмечаем что было изменение
            update();
            scene()->update();
        }

        event->accept();
        return;
    }

    CustomRectItem::mouseMoveEvent(event);
}

void ScrolAreaRectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_IsResizingRowHeight)
    {
        // Добавляем в undo stack только если было реальное изменение
        if (m_RowHeightChanged && m_Scrol->undoStack() && m_StartRowHeight != m_Scrol->rowHeight())
        {
            // Создаем команду undo/redo с правильными значениями
            UndoPropertyChange* undoCmd = new UndoPropertyChange(
                qobject_cast<BaseScene*>(scene()),
                m_Scrol->uuid()
                );
            undoCmd->setPropertyName("rowHeight");
            undoCmd->setValues(m_StartRowHeight, m_Scrol->rowHeight());
            m_Scrol->undoStack()->push(undoCmd);
            emit rowHeightChanged();
        }

        m_IsResizingRowHeight = false;
        m_RowHeightChanged = false;
        event->accept();
        return;
    }

    CustomRectItem::mouseReleaseEvent(event);
}

void ScrolAreaRectItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    CustomRectItem::hoverMoveEvent(event);

    QRectF rect = boundingRect();
    qreal currentHeightPixels = m_Scrol->rowHeight() * style()->gridSize().height();

    // Увеличиваем зону обнаружения с 6 до 10 пикселей
    QRectF heightIndicator(rect.left(), rect.top() + currentHeightPixels - 5, rect.width(), 10);

    bool wasHovering = m_HoverOverIndicator;
    m_HoverOverIndicator = heightIndicator.contains(event->pos());

    if (m_HoverOverIndicator)
    {
        setCursor(Qt::SizeVerCursor);
        if (!wasHovering) // Обновляем только при изменении состояния
            update();
    }
    else
    {
        setCursor(Qt::ArrowCursor);
        if (wasHovering) // Обновляем только при изменении состояния
            update();
    }
}

void ScrolAreaRectItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    if (m_HoverOverIndicator)
    {
        m_HoverOverIndicator = false;
        update(); // Обновляем для смены цвета линии
    }

    setCursor(Qt::ArrowCursor);
    CustomRectItem::hoverLeaveEvent(event);
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
    m_IsScrolAreaVisible = m_ScrolArea->isVisible();
    PanelItem::dragEnterEvent(event);
}

void ScrolItem::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    PanelItem::dragLeaveEvent(event);
    m_ScrolArea->setVisible(m_IsScrolAreaVisible);
}

void ScrolItem::showScrolArea(bool visible)
{
    if (!m_ScrolArea) return;

    // Если уже в нужном состоянии, выходим
    if (visible && m_ScrolArea->isVisible() && m_ScrolArea->opacity() == 0.7) return;
    if (!visible && !m_ScrolArea->isVisible()) return;

    if (visible)
    {
        // Сбрасываем прозрачность для анимации появления
        m_ScrolArea->setOpacity(0.0);
        m_ScrolArea->setVisible(true);

        QPropertyAnimation *animation = new QPropertyAnimation(m_ScrolArea, "opacity");
        animation->setDuration(200);
        animation->setStartValue(0.0);
        animation->setEndValue(0.7);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }
    else
    {
        QPropertyAnimation *animation = new QPropertyAnimation(m_ScrolArea, "opacity");
        animation->setDuration(200);
        animation->setStartValue(m_ScrolArea->opacity());
        animation->setEndValue(0.0);

        connect(animation, &QPropertyAnimation::finished, this, [this]()
        {
            if (m_ScrolArea)
            {
                m_ScrolArea->setVisible(false);
                // Восстанавливаем стандартную прозрачность для следующего показа
                m_ScrolArea->setOpacity(0.7);
            }
        });

        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }
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
