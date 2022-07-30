#include "controlitem.h"
#include <QPainter>
#include <QFont>
#include <QColor>
#include "respanel.h"

ControlItem::ControlItem(QGraphicsItem *parent) :
    CustomRectItem(parent),
    m_pFieldStruct(nullptr)
{
    setBrush(QBrush(Qt::darkBlue));
}

ControlItem::~ControlItem()
{
    if (m_pFieldStruct)
        delete m_pFieldStruct;
}

void ControlItem::setFieldStruct(struct FieldStruct *value)
{
    m_pFieldStruct = new FieldStruct(*value);
}

void ControlItem::paintControl(const quint8 &FType, QPainter *painter, const QRectF &rect)
{
    painter->save();
    QFont font("TerminalVector", 10);
    font.setBold(true);
    font.setFixedPitch(true);

    painter->setPen(Qt::white);
    painter->setFont(font);

    //painter->fillRect(boundingRect(), getBrush());

    painter->restore();
}

void ControlItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();
    painter->fillRect(boundingRect(), getBrush());
    //painter->drawText(boundingRect(), Qt::AlignVCenter, m_pTextStruct->value);
    painter->restore();
    CustomRectItem::paint(painter, option, widget);
    drawIntersects(painter);

    painter->save();
    QColor br = getBrush().color();
    paintBevel(painter, QColor::fromRgb(255 - br.red(), 255 - br.green(), 255 - br.blue()));
    painter->restore();
}
