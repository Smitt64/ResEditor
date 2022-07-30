#include "textitem.h"
#include <QColor>
#include "respanel.h"
#include <QPainter>
#include <QFont>
#include <QDebug>
#include "styles/resstyle.h"
//#include <QGraphicsTextItem>

TextItem::TextItem(QGraphicsItem* parent) :
    CustomRectItem(parent),
    m_pTextStruct(nullptr)
{
    setBrush(QBrush(Qt::darkBlue));
    setAvailableCorners(0);
}

TextItem::~TextItem()
{

}

void TextItem::setTextStruct(TextStruct *value)
{
    m_pTextStruct = value;

    setCoord(QPoint(value->x(), value->y()));
    setSize(QSize(value->len(), 1));
}

void TextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    ResStyleOption opt;
    opt.init(this);
    opt.text = m_pTextStruct->value;

    style()->drawControl(ResStyle::Control_Label, painter, &opt);
    CustomRectItem::paint(painter, option, widget);
    drawIntersects(painter);

    painter->save();
    QColor br = style()->color(ResStyle::Color_TextBg);
    paintBevel(painter, QColor::fromRgb(255 - br.red(), 255 - br.green(), 255 - br.blue()));
    painter->restore();
}
