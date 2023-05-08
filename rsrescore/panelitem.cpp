#include "panelitem.h"
#include "respanel.h"
#include "textitem.h"
#include "controlitem.h"
#include "styles/resstyle.h"
#include "basescene.h"
#include "undoredo/undoitemadd.h"
#include "panelpropertysdlg.h"
#include <QGraphicsView>
#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <QDebug>
#include <QGraphicsScene>
#include <QMetaObject>
#include <QMetaProperty>
#include <QEvent>
#include <QMimeData>
#include <QDynamicPropertyChangeEvent>
#include <QGraphicsSceneDragDropEvent>
#include <QKeyEvent>
#include <algorithm>

PanelItem::PanelItem(CustomRectItem *parent) :
    ContainerItem(parent),
    m_Panel(nullptr),
    m_PanelStyle(ResStyle::SCOM),
    m_PanelExclude(PanelExcludeFlags()),
    m_HelpPage(0),
    m_isCentered(false),
    m_isRightText(false),
    m_DragPixmap(nullptr),
    m_DragControl(nullptr)
{
    setAvailableCorners(RIGHT | BOTTOM_RIGHT | BOTTOM);
    setRubberBand(true);
    setAcceptDrops(true);
    setFlag(QGraphicsItem::ItemIsPanel);

    m_PanelExclude = ExcludeAutoNum | ExcludeShadow;
}

PanelItem::~PanelItem()
{
}

bool PanelItem::event(QEvent *e)
{
    if (e->type() == QEvent::DynamicPropertyChange)
    {
        QDynamicPropertyChangeEvent *pe = static_cast<QDynamicPropertyChangeEvent *>(e);

        if (pe->propertyName() == CONTRAST_PROPERTY)
        {
            update();
            scene()->update();
            return true;
        }
    }
    return ContainerItem::event(e);
}

QVariant PanelItem::userAction(const qint32 &action, const QVariant &param)
{
    if (action == ActionKeyEnter)
    {
        PanelPropertysDlg dlg(scene()->views().first());
        dlg.setPanelMode(true);
        dlg.setRectItem(this);
        dlg.exec();
    }
    return QVariant();
}

void PanelItem::setPanel(ResPanel *panel, const QString &comment)
{
    m_Panel = panel;
    m_Title = m_Panel->title();
    m_Status = m_Panel->status();
    m_Status2 = m_Panel->status2();
    m_HelpPage = m_Panel->helpPage();
    m_Comment = comment;

    setCoord(panel->pos());
    setSize(panel->size());

    for (TextStructList::iterator i = m_Panel->textBegin(); i != m_Panel->textEnd(); ++i)
    {
        TextStruct &textElement = *i;
        TextItem *item = new TextItem(this);
        item->setTextStruct(&textElement);
    }

    for (FieldStructList::iterator i = m_Panel->fieldBegin(); i != m_Panel->fieldEnd(); ++i)
    {
        FieldStruct &fieldElement = *i;
        ControlItem *item = new ControlItem(this);

        int self = std::distance(m_Panel->fieldBegin(), i);
        item->setFieldStruct(&fieldElement, self);
    }

    int borderCount = panel->borderCount();
    for (int i = 0; i < borderCount; i++)
    {
        BorderItem *item = new BorderItem(this);
        item->setBorderParams(panel->borderRect(i), panel->borderStyle(i));
    }
}

void PanelItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    ContainerItem::paint(painter, option, widget);

    ResStyleOption opt;
    opt.init(this);
    opt.text = m_Title;
    opt.borderStyle = borderStyle();

    if (!m_DragHighlightedRect.isValid())
    {
        style()->drawControl(ResStyle::Control_Panel, painter, &opt);

        painter->save();
        painter->drawRect(QRectF(opt.rect.bottomRight().x(), opt.rect.bottomRight().y(), 20, 20));
        painter->restore();
    }
    else
    {
        // Для режима перетаскивания элемента с панели инструментов
        painter->save();
        painter->drawPixmap(boundingRect().toRect(), *m_DragPixmap);
        painter->restore();
    }

    painter->save();
    QColor br = style()->color(ResStyle::Color_TextBg, &opt);
    QColor inverceColor = QColor::fromRgb(255 - br.red(), 255 - br.green(), 255 - br.blue());
    paintBevel(painter, inverceColor);
    painter->restore();

    if (m_DragHighlightedRect.isValid())
    {
        painter->save();
        painter->drawPixmap(m_DragHighlightedRect.toRect(), *m_DragControl);
        painter->restore();

        painter->save();
        QPen pens;
        pens.setCosmetic(true);
        pens.setColor(inverceColor);
        pens.setStyle(Qt::DotLine);
        painter->setPen(pens);

        //QRectF rc = m_DragHighlightedRect;
        //rc.setSize(QSizeF(m_DragHighlightedRect.width() - 2, m_DragHighlightedRect.height() - 2));
        painter->drawRect(m_DragHighlightedRect);
        painter->restore();
    }
}

const ResStyle::PanelStyle &PanelItem::panelStyle() const
{
    return m_PanelStyle;
}

void PanelItem::setPanelStyle(const ResStyle::PanelStyle &style)
{
    if (isSkipUndoStack() || !undoStack())
    {
        m_PanelStyle = style;
        emit panelStyleChanged();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("panelStyle", style);
}

QString PanelItem::title() const
{
    return m_Title;
}

void PanelItem::setTitle(const QString &text)
{
    if (isSkipUndoStack() || !undoStack())
    {
        m_Title = text;
        emit titleChanged();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("title", text);
}

QString PanelItem::status() const
{
    return m_Status;
}

void PanelItem::setStatus(const QString &text)
{
    if (isSkipUndoStack() || !undoStack())
    {
        m_Status = text;
        emit statusChanged();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("status", text);
}

QString PanelItem::status2() const
{
    return m_Status2;
}

void PanelItem::setStatus2(const QString &text)
{
    if (isSkipUndoStack() || !undoStack())
    {
        m_Status2 = text;
        emit status2Changed();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("status2", text);
}

const bool &PanelItem::isCentered() const
{
    return m_isCentered;
}

void PanelItem::setIsCentered(const bool &val)
{
    if (isSkipUndoStack() || !undoStack())
    {
        m_isCentered = val;
        emit isCenteredChanged();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("isCentered", val);
}

const bool &PanelItem::isRightText() const
{
    return m_isRightText;
}

void PanelItem::setIsRightText(const bool &val)
{
    if (isSkipUndoStack() || !undoStack())
    {
        m_isRightText = val;
        emit isRightTextChanged();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("isRightText", val);
}

PanelItem::PanelExcludeFlags PanelItem::panelExclude() const
{
    return m_PanelExclude;
}

void PanelItem::setPanelExclude(const PanelItem::PanelExcludeFlags &val)
{
    if (isSkipUndoStack() || !undoStack())
    {
        m_PanelExclude = val;
        emit panelExcludeChanged();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("panelExclude", QVariant::fromValue((int)val));
}

QString PanelItem::comment() const
{
    return m_Comment;
}

void PanelItem::setComment(const QString &text)
{
    if (isSkipUndoStack() || !undoStack())
    {
        m_Comment = text;
        emit commentChanged();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("comment", text);
}

const quint16 &PanelItem::helpPage() const
{
    return m_HelpPage;
}

void PanelItem::setHelpPage(const quint16 &val)
{
    if (isSkipUndoStack() || !undoStack())
    {
        m_HelpPage = val;
        emit helpPageChanged();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("helpPage", val);
}

void PanelItem::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasFormat(MIMETYPE_TOOLBOX))
    {
        if (!m_DragHighlightedRect.isValid())
        {
            QList<QGraphicsItem*> items;
            renderToPixmap(&m_DragPixmap);
            createFromJson(this, event->mimeData()->data(MIMETYPE_TOOLBOX), items);

            if (!items.empty())
            {
                QRectF rc = items.first()->boundingRect();
                for (const QGraphicsItem *item : qAsConst(items))
                    rc = rc.united(item->boundingRect());
                m_DragControl = new QPixmap(rc.toRect().size());
                m_DragControl->fill(Qt::transparent);
                m_DragHighlightedRect.setWidth(rc.width());
                m_DragHighlightedRect.setHeight(rc.height());

                for (QGraphicsItem *item : qAsConst(items))
                {
                    CustomRectItem *rectItem = dynamic_cast<CustomRectItem*>(item);

                    QPointF coord = ewCoordToReal(rectItem->getPoint());
                    rectItem->renderToPixmap(&m_DragControl, coord);
                }

                update();
                scene()->update();

                qDeleteAll(items);
            }
            else
                qDebug() << "PanelItem::dragEnterEvent: error deserialize";
        }

        setChildsVisible(false);
        event->acceptProposedAction();
    }
    else
        event->ignore();
}

void PanelItem::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasFormat(MIMETYPE_TOOLBOX))
    {
        QPointF newPos = event->pos();
        BaseScene* customScene = qobject_cast<BaseScene*> (scene());
        QSize gridSize = customScene->getGridSize();

        qreal xV = round(newPos.x() / gridSize.width()) * gridSize.width();
        qreal yV = round(newPos.y() / gridSize.height()) * gridSize.height();
        m_DragHighlightedRect.moveTo(QPointF(xV, yV));

        update();
    }
    else
    {
        dragLeaveEvent(nullptr);

        event->ignore();
    }
}

void PanelItem::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasFormat(MIMETYPE_TOOLBOX))
    {
        BaseScene *pScene = dynamic_cast<BaseScene*>(scene());
        UndoItemAdd *pUndo = new UndoItemAdd(pScene);
        pUndo->setData(event->mimeData()->data(MIMETYPE_TOOLBOX));
        pUndo->setOffset(realCoordToEw(event->pos()));

        undoStack()->push(pUndo);

        dragLeaveEvent(nullptr);
        event->accept();
    }
    else
    {
        dragLeaveEvent(nullptr);
        event->ignore();
    }
}

void PanelItem::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    m_DragHighlightedRect = QRectF();
    delete m_DragPixmap;
    delete m_DragControl;
    m_DragPixmap = nullptr;
    m_DragControl = nullptr;
    setChildsVisible(true);

    update();
    scene()->update();
}

void PanelItem::setChildsVisible(const bool &value)
{
    QList<QGraphicsItem*> childs = childItems();
    for (QGraphicsItem *child : qAsConst(childs))
        child->setVisible(value);
}

void PanelItem::updateChildControlsOrder()
{
    BaseScene *pScene = dynamic_cast<BaseScene*>(scene());
    QList<ControlItem*> pItems = pScene->findItems<ControlItem>();

    if (pItems.empty())
        return;

    std::sort(pItems.begin(), pItems.end(), [](ControlItem *a, ControlItem *b)
    {
        QRect rectA = a->geometry(),
                rectB = b->geometry();

        if (rectA.y() == rectB.y())
        {
            if (rectA.x() < rectB.x())
                return true;
        }

        return rectA.y() < rectB.y();
    });

    auto GetFldsSameY = [pItems](int y) -> QList<ControlItem*>
    {
        QList<ControlItem*> lst;

        for (ControlItem *item : pItems)
        {
            if (item->geometry().y() == y)
                lst.append(item);
        }
        return lst;
    };

    auto GetBottomField = [=](ControlItem *Current) -> int
    {
        if (pItems.last() == Current)
            return 0;

        int id = -1;
        int intexOf = pItems.indexOf(Current);
        QRect curGeometry = Current->geometry();

        for (int i = intexOf + 1; i < pItems.size(); i++)
        {
            QRect geometry = pItems.at(i)->geometry();

            if (geometry.y() > curGeometry.y())
            {
                // первое поле с большим y
                QList<ControlItem*> lst = GetFldsSameY(geometry.y());
                for (ControlItem *item : qAsConst(lst))
                {
                    if (item->geometry().intersects(geometry))
                    {
                        id = pItems.indexOf(item);
                        break;
                    }
                }

                if (id == -1)
                    id = pItems.indexOf(lst.first());

                break;
            }
        }

        if (id == -1)
            id = 0;

        return id;
    };

    auto GetUpField = [=](ControlItem *Current) -> int
    {
        if (pItems.first() == Current)
            return pItems.size() - 1;

        int id = -1;
        int intexOf = pItems.indexOf(Current);
        QRect curGeometry = Current->geometry();

        for (int i = intexOf - 1; i >= 0; i--)
        {
            QRect geometry = pItems.at(i)->geometry();

            if (geometry.y() < curGeometry.y())
            {
                // первое поле с большим y
                QList<ControlItem*> lst = GetFldsSameY(geometry.y());
                for (ControlItem *item : qAsConst(lst))
                {
                    if (item->geometry().intersects(geometry))
                    {
                        id = pItems.indexOf(item);
                        break;
                    }
                }

                if (id == -1)
                    id = pItems.indexOf(lst.first());

                break;
            }
        }

        if (id == -1)
            id = pItems.size() - 1;

        return id;
    };

    QHash<ControlItem*, ControTabOrder> NewOrders;
    for (ControlItem *item : qAsConst(pItems))
    {
        int selfId = pItems.indexOf(item);
        NewOrders[item] = item->tabOrder();

        ControTabOrder &tabOrder = NewOrders[item];
        tabOrder.setThisId(selfId);

        if (pItems.last() != item)
            tabOrder.setNext(selfId + 1);
        else
            tabOrder.setNext(0);

        if (pItems.first() != item)
            tabOrder.setPrevious(selfId - 1);
        else
            tabOrder.setPrevious(pItems.size() - 1);

        int bottom = GetBottomField(item);
        int up = GetUpField(item);
        tabOrder.setBottom(bottom);
        tabOrder.setUp(up);
    }

    QHashIterator<ControlItem*, ControTabOrder> NewOrdersIter(NewOrders);
    while (NewOrdersIter.hasNext())
    {
        NewOrdersIter.next();

        ControlItem *item = NewOrdersIter.key();
        const ControTabOrder &tabOrder = NewOrdersIter.value();

        if (item->tabOrder().thisid() != tabOrder.thisid())
            item->setTabOrder(tabOrder);
    }
}
