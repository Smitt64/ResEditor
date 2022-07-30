#include "resstyle.h"
#include "customrectitem.h"

ResStyleOption::ResStyleOption()
{

}

void ResStyleOption::init(CustomRectItem *item)
{
    rect = item->boundingRect();
}

// --------------------------------------------------------------------------

ResStyle::ResStyle() :
    QObject()
{
    m_Colors[Color_SceneBackground] = QColor(192,192,192);
    m_Colors[Color_ContainerBg] = QColor(128, 128, 0);
    m_Colors[Color_BorderColor] = m_Colors[Color_ContainerBg].lighter(200);
    m_Colors[Color_TextBg] = QColor(Qt::darkBlue);
    m_Colors[Color_Text] = QColor(Qt::white);
    m_Colors[Color_ControlBg] = QColor(Qt::darkBlue);
    m_Colors[Color_Control] = QColor(Qt::white);

    m_BorderChars[Border_NoLine] = { QChar::Space, QChar::Space, QChar::Space, QChar::Space, QChar::Space, QChar::Space};
    m_BorderChars[Border_SingleLine] = { 9484, 9488, 9472, 9474, 9492, 9496 };
    m_BorderChars[Border_DoubleLine] = { 9556, 9559, 9552, 9553, 9562, 9565 };

    m_Font = QFont("TerminalVector", 10);
    m_Font.setFixedPitch(true);
}

QSize ResStyle::gridSize() const
{
    return QSize(8, 12);
}

QColor ResStyle::color(const StyleColor &type) const
{
    return m_Colors[type];
}

ResStyle::BorderChars ResStyle::borderChars(const BorderStyle &style) const
{
    return m_BorderChars[style];
}

QFont ResStyle::font() const
{
    return m_Font;
}

QSize ResStyle::getSizeFromItem(const QRectF &rect)
{
    QSize _gridSize = gridSize();

    qreal xV = round(rect.width() / _gridSize.width());
    qreal yV = round(rect.height() / _gridSize.height());

    return QSize(xV, yV);
}

void ResStyle::drawSceneBackground(QPainter *painter, const QRectF &rect)
{
    QSize grs = gridSize();

    painter->save();
    painter->fillRect(rect, m_Colors[Color_SceneBackground]);
    QPen pen;
    painter->setPen(pen);

    qint32 left = qint32(rect.left()) - (int(rect.left()) % grs.width());
    qint32 top = qint32(rect.top()) - (int(rect.top()) % grs.width());
    QVector<QPointF> points;

    for (qint32 x = left; x < rect.right(); x += grs.width())
    {
        for (qint32 y = top; y < rect.bottom(); y += grs.height())
            points.append(QPointF(x,y));
    }

    painter->drawPoints(points.data(), points.size());
    painter->restore();
}

void ResStyle::drawBorder(QPainter *painter, const BorderStyle &bs, const QRectF &rect, const QString &text)
{
    const BorderChars ch = borderChars(bs);
    QSize sz = getSizeFromItem(rect);
    QSize grs = gridSize();

    int title_width = sz.width() - 2;
    QString subTitle = text.mid(0, title_width);

    int fillLenght = title_width - subTitle.length();
    QString title = ch.chtopleft + subTitle + QString(fillLenght, ch.horline) + ch.chtopright;
    QString left_right = ch.verline + QString(title_width, QChar::Space) + ch.verline;
    QString status = ch.chbotleft + QString(title_width, ch.horline) + ch.chbotright;

    painter->save();
    painter->setFont(m_Font);
    painter->setPen(m_Colors[Color_BorderColor]);
    painter->drawText(QRectF(0, 0, rect.width(), grs.height()), Qt::AlignVCenter, title);

    qreal text_height = grs.height();
    for (int y = 1; y < sz.height() - 1; y++)
        painter->drawText(QRectF(0, text_height * y, rect.width(), grs.height()), Qt::AlignVCenter, left_right);

    painter->drawText(QRectF(0, rect.height() - grs.height(), rect.width(), grs.height()), Qt::AlignVCenter, status);

    painter->restore();
}

void ResStyle::drawText(QPainter *painter, const QRectF &rect, const QString &text)
{
    painter->save();
    painter->setPen(Qt::white);
    painter->setFont(m_Font);
    painter->drawText(rect, Qt::AlignVCenter, text);
    painter->restore();
}

void ResStyle::drawControl(const ControlType &type, QPainter *painter, ResStyleOption *option)
{
    if (type == Control_Panel)
    {
        painter->save();
        painter->fillRect(option->rect, m_Colors[Color_ContainerBg]);
        drawBorder(painter, (BorderStyle)option->borderStyle, option->rect, option->text);
        painter->restore();
    }
    else if (type == Control_Label)
    {
        painter->save();
        painter->fillRect(option->rect, m_Colors[Color_TextBg]);
        drawText(painter, option->rect, option->text);
        painter->restore();
    }
}
