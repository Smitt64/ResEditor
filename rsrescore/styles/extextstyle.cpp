#include "extextstyle.h"

EwTextStyle::EwTextStyle(QObject *parent) :
    QObject(parent)
{

}

EwTextStyle::EwTextStyle(const EwTextStyle &other) :
    QObject()
{
    fromOther(other);
}

EwTextStyle::~EwTextStyle()
{

}

void EwTextStyle::fromOther(const EwTextStyle &other)
{
    m_Align = other.m_Align;
    m_Format = other.m_Format;
    m_Textcolor = other.m_Textcolor;
    m_Back = other.m_Back;
}

EwTextStyle &EwTextStyle::operator=(const EwTextStyle &other)
{
    fromOther(other);
    return *this;
}

bool EwTextStyle::operator ==(const EwTextStyle& other) const
{
    return m_Align == other.m_Align &&
            m_Format == other.m_Format &&
            m_Textcolor == other.m_Textcolor &&
            m_Back == other.m_Back;
}

/*bool EwTextStyle::operator <(const EwTextStyle& other)
{
    return m_Align < other.m_Align &&
            m_Format < other.m_Format &&
            m_Textcolor < other.m_Textcolor &&
            m_Back < other.m_Back;
}*/

QJsonObject EwTextStyle::toJsonObject(const EwTextStyle &style)
{
    QJsonObject obj;
    obj.insert("align", style.m_Align);
    obj.insert("format", (int)style.m_Format);
    obj.insert("color", style.m_Textcolor);
    obj.insert("background", style.m_Back);
    return obj;
}

EwTextStyle EwTextStyle::fromJsonObject(const QJsonObject &obj)
{
    EwTextStyle style;
    style.setAlign(static_cast<TextAlign>(obj["align"].toInt()));
    style.setFormat(static_cast<TextFormat>(obj["format"].toInt()));
    style.setTextColor(static_cast<TextColor>(obj["color"].toInt()));
    style.setBackColor(static_cast<BackColor>(obj["background"].toInt()));

    return style;
}

const EwTextStyle::TextAlign &EwTextStyle::align() const
{
    return m_Align;
}

void EwTextStyle::setAlign(const TextAlign &value)
{
    m_Align = value;
    emit alignChanged();
}

const EwTextStyle::TextFormatFlags &EwTextStyle::format() const
{
    return m_Format;
}

void EwTextStyle::setFormat(const EwTextStyle::TextFormatFlags &value)
{
    m_Format = value;
    emit formatChanged();
}

const EwTextStyle::TextColor &EwTextStyle::textColor() const
{
    return m_Textcolor;
}

void EwTextStyle::setTextColor(const TextColor &value)
{
    m_Textcolor = value;
    emit textColorChanged();
}

const EwTextStyle::BackColor &EwTextStyle::backColor() const
{
    return m_Back;
}

void EwTextStyle::setBackColor(const EwTextStyle::BackColor &value)
{
    m_Back = value;
    emit textBackChanged();
}
