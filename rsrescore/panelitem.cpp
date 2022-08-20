#include "panelitem.h"
#include "respanel.h"
#include "textitem.h"
#include "controlitem.h"
#include "styles/resstyle.h"
#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <QDebug>
#include <QGraphicsScene>
#include <QMetaObject>
#include <QMetaProperty>
#include <QEvent>
#include <QDynamicPropertyChangeEvent>

PanelItem::PanelItem(CustomRectItem *parent) :
    ContainerItem(parent),
    m_Panel(nullptr),
    m_PanelStyle(ResStyle::SCOM),
    m_PanelExclude(PanelExcludeFlags()),
    m_HelpPage(0),
    m_isCentered(false),
    m_isRightText(false)
{
    setAvailableCorners(RIGHT | BOTTOM_RIGHT | BOTTOM);
    setRubberBand(true);
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

void PanelItem::setPanel(ResPanel *panel)
{
    m_Panel = panel;
    m_Title = m_Panel->title();
    m_Status = m_Panel->status();
    m_Status2 = m_Panel->status2();
    m_HelpPage = m_Panel->helpPage();

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
        item->setFieldStruct(&fieldElement);
    }
}

void PanelItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    ContainerItem::paint(painter, option, widget);

    ResStyleOption opt;
    opt.init(this);
    opt.text = m_Panel->title();
    opt.borderStyle = borderStyle();

    style()->drawControl(ResStyle::Control_Panel, painter, &opt);

    painter->save();
    painter->drawRect(QRectF(opt.rect.bottomRight().x(), opt.rect.bottomRight().y(), 20, 20));
    painter->restore();

    painter->save();
    QColor br = style()->color(ResStyle::Color_TextBg, &opt);
    paintBevel(painter, QColor::fromRgb(255 - br.red(), 255 - br.green(), 255 - br.blue()));
    painter->restore();
}

const ResStyle::PanelStyle &PanelItem::panelStyle() const
{
    return m_PanelStyle;
}

void PanelItem::setPanelStyle(const ResStyle::PanelStyle &style)
{
    if (isSkipUndoStack())
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
    if (isSkipUndoStack())
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
    if (isSkipUndoStack())
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
    if (isSkipUndoStack())
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
    if (isSkipUndoStack())
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
    if (isSkipUndoStack())
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
    if (isSkipUndoStack())
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
    if (isSkipUndoStack())
    {
        m_Comment = text;
        emit commentChanged();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("comment", text);
}

const quint32 &PanelItem::helpPage() const
{
    return m_HelpPage;
}

void PanelItem::setHelpPage(const quint32 &val)
{
    if (isSkipUndoStack())
    {
        m_HelpPage = val;
        emit helpPageChanged();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("helpPage", val);
}
