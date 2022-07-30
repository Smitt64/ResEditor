#include "panelitem.h"
#include "respanel.h"
#include "textitem.h"
#include "styles/resstyle.h"
#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QDebug>

PanelItem::PanelItem(CustomRectItem *parent) :
    ContainerItem(parent),
    m_Panel(nullptr)
{
    setAvailableCorners(RIGHT | BOTTOM_RIGHT | BOTTOM);
    m_pDocument = new QTextDocument();
    m_pDocument->setDocumentMargin(0);

    setRubberBand(true);
}

PanelItem::~PanelItem()
{
    if (m_pDocument)
        delete m_pDocument;
}

void PanelItem::setPanel(ResPanel *panel)
{
    m_Panel = panel;

    setCoord(panel->pos());
    setSize(panel->size());

    for (TextStructList::iterator i = m_Panel->textBegin(); i != m_Panel->textEnd(); ++i)
    {
        TextStruct &textElement = *i;
        TextItem *item = new TextItem(this);
        item->setTextStruct(&textElement);
    }
}

void PanelItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    ContainerItem::paint(painter, option, widget);

    ResStyleOption opt;
    opt.init(this);
    opt.text = m_Panel->title();
    opt.borderStyle = ResStyle::Border_DoubleLine;

    style()->drawControl(ResStyle::Control_Panel, painter, &opt);
}
