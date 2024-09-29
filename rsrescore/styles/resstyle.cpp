#include "resstyle.h"
#include "customrectitem.h"
#include "rscoreheader.h"
#include "basescene.h"
#include "resapplication.h"
#include <QMetaClassInfo>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMetaObject>
#include <QFile>
#include <stdexcept>
#include <QSettings>

ResStyleOption::ResStyleOption() :
    contrast(false)
{

}

void ResStyleOption::castToGrpagicsItem(CustomRectItem *item, QGraphicsItem **out)
{
    if (!item)
        throw std::invalid_argument("item is null");

    if (!out)
        throw std::invalid_argument("out is null");

    *out = item;
}

void ResStyleOption::castToCustomRectItem(QGraphicsItem *item, CustomRectItem **out)
{
    if (!item)
        throw std::invalid_argument("item is null");

    if (!out)
        throw std::invalid_argument("out is null");

    *out = dynamic_cast<CustomRectItem*>(item);
}

const QMetaObject *ResStyleOption::CustomRectItemMetaobject(CustomRectItem *item)
{
    return item->metaObject();
}

QVariant ResStyleOption::CustomRectItemProperty(CustomRectItem *item, const char *propertyName)
{
    return item->property(propertyName);
}

QGraphicsItem *ResStyleOption::parentItem(QGraphicsItem *item)
{
    return item->parentItem();
}

void ResStyleOption::init(CustomRectItem *item)
{
    if (!item)
        return;

    BaseScene* customScene = qobject_cast<BaseScene*> (item->scene());

    rect = item->boundingRect();
    gridSize = customScene->getGridSize();
    alignment = Qt::AlignLeft | Qt::AlignVCenter;

    panelStyle = readProperty<ResStyle::PanelStyle>(item, "panelStyle", ResStyle::SCOM);
    contrast = readProperty<bool>(item, CONTRAST_PROPERTY, false);
    astext = readProperty<bool>(item, "isText", false);
}

// --------------------------------------------------------------------------

ResStyle::ResStyle() :
    QObject()
{
    m_BorderChars[Border_NoLine] = { QChar::Space, QChar::Space, QChar::Space, QChar::Space, QChar::Space, QChar::Space};
    m_BorderChars[Border_SingleLine] = { 9484, 9488, 9472, 9474, 9492, 9496 };
    m_BorderChars[Border_DoubleLine] = { 9556, 9559, 9552, 9553, 9562, 9565 };
    m_BorderChars[Border_Combine1]   = { 9554, 9557, 9552, 9474, 9560, 9563 };
    m_BorderChars[Border_Combine2]   = { 9555, 9558, 9472, 9553, 9561, 9564 };
    m_BorderChars[Border_Solid]      = { 9608, 9608, 9608, 9608, 9608, 9608 };

    int GridOption = getGridSizeOption();
    GrigSizes sizes = gridSizes();
    m_GridSize = sizes[GridOption];

    loadDefaultColors();
    resetFont(GridOption);
}

void ResStyle::resetFont(int GridOption)
{
    int fontsize = fontSizeForGrid(GridOption);

    m_Font = QFont("TerminalVector", fontsize);
    m_Font.setPixelSize(fontsize);
    m_Font.setFixedPitch(true);
    m_Font.setBold(true);
}

void ResStyle::loadDefaultColors()
{
    const QMetaObject *metaobject = metaObject();
    int infoId = metaobject->indexOfClassInfo(COLORSCHEME);

    if (infoId >= 0)
    {
        const QMetaClassInfo &info = metaobject->classInfo(infoId);
        const char *fname = info.value();

        QFile f(fname);
        if (f.open(QIODevice::ReadOnly))
        {
            QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
            QJsonObject rootObj = doc.object();
            QJsonArray colorStyles = rootObj["colorStyles"].toArray();

            for (const auto &style : qAsConst(colorStyles))
            {
                ColorScheme sheme;
                loadColorSheme(style.toObject(), sheme);

                PanelStyle pankey = static_cast<PanelStyle>(enumValueFromName("PanelStyle", sheme.name));
                m_ColorScheme.insert(pankey, sheme);
            }
        }
    }
}

int ResStyle::enumValueFromName(const QString &enumname, const QString &name) const
{
    int value = 0;
    const QMetaObject *metaobject = ResStyle::metaObject();
    int enumId = metaobject->indexOfEnumerator(enumname.toLocal8Bit().data());

    if (enumId >= 0)
    {
        const QMetaEnum &_enum = metaobject->enumerator(enumId);

        bool ok = false;
        value = _enum.keyToValue(name.toLocal8Bit().data(), &ok);

        if (!ok)
            throw std::bad_cast();
    }

    return value;
}

void ResStyle::loadColorSheme(const QJsonObject &obj, ColorScheme &sheme)
{
    auto readColor = [](const QJsonValue &colorValue) -> QColor
    {
        enum
        {
            color_Red,
            color_Green,
            color_Blue
        };
        QJsonArray colorArray = colorValue.toArray();
        QColor rgb(QColor::Rgb);
        rgb.setRed(colorArray[color_Red].toInt());
        rgb.setGreen(colorArray[color_Green].toInt());
        rgb.setBlue(colorArray[color_Blue].toInt());

        return rgb;
    };

    auto colorConstByName = [this](const QString &name) -> StyleColor
    {
        StyleColor style = Color_SceneBackground;
        style = static_cast<StyleColor>(enumValueFromName("StyleColor", name));

        return style;
    };

    sheme.name = obj["name"].toString();
    sheme.ContainerContrast = readColor(obj["contrast"]);

    QJsonArray colorsArray = obj["colors"].toArray();
    for (const auto &colorElement : qAsConst(colorsArray))
    {
        QJsonObject colorObj = colorElement.toObject();

        StyleColor colorType = colorConstByName(colorObj["name"].toString());
        QColor colorValue = readColor(colorObj["color"]);

        sheme.Colors.insert(colorType, colorValue);
    }
}

int ResStyle::getGridSizeOption() const
{
    int index = 0;
    QSettings *settings = ResApp->settings();

    settings->beginGroup("StdEditor");
    QString mode = settings->value("GridSizeMode", "Standard").toString();

    if (mode == "Large")
        index = 1;

    settings->endGroup();

    return index;
}

int ResStyle::fontSizeForGrid(int index)
{
    static QList<int> fontsizes = {
        14,
        20
    };

    return fontsizes[index];
}

GrigSizes ResStyle::gridSizes()
{
    static GrigSizes sizes = {
        QSize(8, 12),
        QSize(12, 16),
    };

    return sizes;
}

QStringList ResStyle::gridSizesName()
{
    static QStringList names = 
    {
        "Standard",
        "Large",
    };

    return names;
}

QSize ResStyle::gridSize() const
{
    return m_GridSize;
}

void ResStyle::setGridSize(const int &index)
{
    static GrigSizes sizes = gridSizes();
    m_GridSize = sizes[index];
    resetFont(index);
}

QColor ResStyle::color(const StyleColor &type, ResStyleOption *option) const
{
    if (!option && type == Color_SceneBackground)
        return QColor(192,192,192);

    const ColorScheme &sheme = m_ColorScheme[option->panelStyle];

    if (type == Color_ContainerBg)
    {
        if (option->contrast)
            return sheme.ContainerContrast;
    }

    return sheme.Colors[type];
}

ResStyle::BorderChars ResStyle::borderChars(const BorderStyle &style) const
{
    return m_BorderChars[style];
}

QFont ResStyle::font() const
{
    return m_Font;
}

ResStyle::ControlType ResStyle::controlFromFvt(const quint8 &fvt) const
{
    ControlType type = Control_Unknown;

    switch(fvt)
    {
    case FT_INT16:
    case FT_INT32:
    case FT_INT64:
        type = Control_Integer;
        break;
    case FT_STRING:
    case FT_SNR:
        type = Control_String;
        break;
    case FT_DATE:
        type = Control_Date;
        break;
    case FT_TIME:
        type = Control_Time;
        break;
    case FT_SHTM:
        type = Control_ShortTime;
        break;
    case FT_CHAR:
        type = Control_Char;
        break;
    case FT_UCHAR:
        type = Control_UChar;
        break;
    case FT_FLOAT:
    case FT_FLOATG:
        type = Control_Float;
        break;
    case FT_MONEY:
    case FT_NUMERIC:
        type = Control_Money;
        break;
    case FT_DOUBLE:
    case FT_DOUBLEG:
        type = Control_Double;
        break;
    }

    return type;
}

bool ResStyle::isNumeric(const quint8 &fvt) const
{
    bool hr = false;

    switch(fvt)
    {
    case FT_INT16:
    case FT_INT32:
    case FT_INT64:
    case FT_FLOAT:
    case FT_FLOATG:
    case FT_MONEY:
    case FT_NUMERIC:
    case FT_DOUBLE:
    case FT_DOUBLEG:
        hr = true;
        break;
    default:
        hr = false;
    }

    return hr;
}

QString ResStyle::controlDefaultText(const ControlType &type)
{
    QString text;

    switch(type)
    {
    case Control_Date:
        text = QString("00.00.0000");
        break;
    case Control_Time:
        text = QString("00:00:00");
        break;
    case Control_ShortTime:
        text = QString("00:00");
        break;
    case Control_String:
        text = QString("String");
        break;
    case Control_Char:
        text = QString("Char");
        break;
    case Control_UChar:
        text = QString("UChar");
        break;
    case Control_Integer:
        text = QString("0");
        break;
    case Control_Float:
    case Control_Money:
    case Control_Double:
        text = QString("0.00");
        break;
    case Control_Unknown:
    case Control_Label:
    case Control_Panel:
    case Control_Button:
        break;
    }
    return text;
}

QSize ResStyle::getSizeFromItem(const QRectF &rect)
{
    QSize _gridSize = gridSize();

    qreal xV = round(rect.width() / _gridSize.width());
    qreal yV = round(rect.height() / _gridSize.height());

    return QSize(xV, yV);
}

void ResStyle::drawSceneBackground(QPainter *painter, const QRectF &rect, ResStyleOption *option)
{
    QSize grs = gridSize();

    painter->save();
    painter->fillRect(rect, color(Color_SceneBackground, option));
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

void ResStyle::drawBorder(QPainter *painter, const BorderStyle &bs, const QRectF &rect, const QString &text, ResStyleOption *option)
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

    if (option->contrast)
    {
        painter->save();
        ResStyleOption opt = *option;
        opt.contrast = false;

        QColor brush = color(Color_ContainerBg, &opt);
        painter->setBrush(brush);
        painter->setPen(brush);
        painter->drawRects(QVector<QRect>{
                               QRect(0, 0, rect.width(), grs.height()), //top
                               QRect(0, rect.height() - grs.height(), rect.width(), grs.height()), // bottom
                               QRect(0, grs.height(), grs.width(), rect.height() - grs.height()), // left
                               QRect(rect.width() - grs.width(), grs.height(), grs.width(), rect.height() - grs.height()) // right
                           });
        painter->restore();
    }

    painter->save();
    painter->setFont(m_Font);
    painter->setPen(color(Color_BorderColor, option));
    painter->drawText(QRectF(0, 0, rect.width(), grs.height()), Qt::AlignVCenter, title);

    qreal text_height = grs.height();
    for (int y = 1; y < sz.height() - 1; y++)
        painter->drawText(QRectF(0, text_height * y, rect.width(), grs.height()), Qt::AlignVCenter, left_right);

    painter->drawText(QRectF(0, rect.height() - grs.height(), rect.width(), grs.height()), Qt::AlignVCenter, status);

    painter->restore();
}

void ResStyle::drawText(QPainter *painter, const QRectF &rect, const QString &text, const Qt::Alignment &alignment, const QColor &penColor)
{
    painter->save();
    painter->setPen(penColor);
    painter->setFont(m_Font);
    painter->drawText(rect, alignment, text);
    painter->restore();
}

void ResStyle::drawControl(const ControlType &type, QPainter *painter, ResStyleOption *option)
{
    if (type == Control_Panel)
    {
        painter->save();
        painter->fillRect(option->rect, color(Color_ContainerBg, option));

        QString text = option->borderStyle == BorderStyle::Border_NoLine ? "" : option->text;
        drawBorder(painter, (BorderStyle)option->borderStyle, option->rect, text, option);
        painter->restore();
    }
    else if (type == Control_Frame)
    {
        painter->save();
        painter->fillRect(option->rect, Qt::transparent);
        drawBorder(painter, (BorderStyle)option->borderStyle, option->rect, QString(), option);
        painter->restore();
    }
    else if (type == Control_Label)
    {
        painter->save();
        painter->fillRect(option->rect, color(Color_TextBg, option));
        drawText(painter, option->rect, option->text, option->alignment, color(Color_Text, option));
        painter->restore();
    }
    else if (type == Control_Button)
    {
        painter->save();
        QRectF rc = option->rect;
        rc.setWidth(rc.width() - option->gridSize.width());
        rc.setHeight(rc.height() - option->gridSize.height());
        painter->fillRect(rc, color(Color_Button, option));

        QColor shadowColor = color(Color_ButtonShadow, option);
        QRectF botomShadow(option->rect.topLeft(), option->rect.size());
        botomShadow.setTop(option->gridSize.height());
        botomShadow.setLeft(option->gridSize.width());
        botomShadow.setHeight(option->gridSize.height() / 2);

        QRectF rightShadow(option->rect.topLeft(), option->rect.size());
        rightShadow.setTop(option->gridSize.height() / 2);
        rightShadow.setLeft(rc.width());
        rightShadow.setHeight(option->gridSize.height() / 2);

        painter->fillRect(botomShadow, shadowColor);
        painter->fillRect(rightShadow, shadowColor);
        drawText(painter, rc, option->text, option->alignment | Qt::AlignVCenter, color(Color_ButtonText, option));
        painter->restore();
    }
    else if (type == Control_ScrolArea)
    {
        painter->save();
        QRectF rc = option->rect;
        QString mask(rc.width() / option->gridSize.width(), QChar('~'));

        painter->fillRect(option->rect, Qt::black);
        qreal offset = option->gridSize.height() * option->rowHeight;
        int count = (rc.height() - offset) / option->gridSize.height();

        for (int i = 0; i < count; i++)
        {
            QRectF row = rc;
            row.setX(0);
            row.setY(offset + option->gridSize.height() * i);
            row.setHeight(option->gridSize.height());
            drawText(painter, row, mask, option->alignment, color(Color_Text, option));
        }
        painter->restore();
    }
    else
    {
        painter->save();
        QBrush brush(color(Color_ControlBg, option));

        if (option->astext)
            brush.setStyle(Qt::Dense4Pattern);

        painter->fillRect(option->rect, brush);
        drawText(painter, option->rect, option->text, option->alignment, color(Color_ControlText, option));
        painter->restore();
    }
}
