#include "easywinini.h"
#include <QColor>
#include <QMetaEnum>

EasyWinIni::EasyWinIni() :
    QObject()
{
    m_TextStyleColors =
    {
        QColor(0, 0, 170),     // TEXT_STYLE_BLUE
        QColor(0, 170, 0),     // TEXT_STYLE_GREEN
        QColor(0, 170, 170),   // TEXT_STYLE_CYAN
        QColor(170, 0, 0),     // TEXT_STYLE_RED
        QColor(170, 0, 170),   // TEXT_STYLE_MAGENTA
        QColor(170, 85, 0),    // TEXT_STYLE_BROWN
        QColor(170, 170, 170), // TEXT_STYLE_LTGRAY
        QColor(85, 85, 85),    // TEXT_STYLE_DKGRAY
        QColor(85, 85, 255),   // TEXT_STYLE_LTBLUE
        QColor(85, 255, 85),   // TEXT_STYLE_LTGREEN
        QColor(85, 255, 255),  // TEXT_STYLE_LTCYAN
        QColor(255, 85, 85),   // TEXT_STYLE_LTRED
        QColor(255, 85, 255),  // TEXT_STYLE_LTMAGENTA
        QColor(255, 255,  85), // TEXT_STYLE_YELLOW
        QColor(255, 255, 255)  // TEXT_STYLE_WHITE
    };

    m_TextBackColors =
    {
        QColor(  0, 191, 255), // BACK_STYLE_BLUE
        QColor(128, 128,   0), // BACK_STYLE_GREEN
        QColor(  0, 255, 255), // BACK_STYLE_CYAN
        QColor(250, 128, 114), // BACK_STYLE_RED
        QColor(238, 130, 238), // BACK_STYLE_MAGENTA
        QColor(160,  82,  45), // BACK_STYLE_BROWN
        QColor(211, 211, 211), // BACK_STYLE_LTGRAY
        QColor(128, 128, 128), // BACK_STYLE_DKGRAY
        QColor(176, 196, 222), // BACK_STYLE_LTBLUE
        QColor(173, 255,  47), // BACK_STYLE_LTGREEN
        QColor(224, 255, 255), // BACK_STYLE_LTCYAN
        QColor(255, 192, 203), // BACK_STYLE_LTRED
        QColor(230, 230, 250), // BACK_STYLE_LTMAGENTA
        QColor(255, 255, 224), // BACK_STYLE_YELLOW
        QColor(  0,   0,   0)  // BACK_STYLE_BLACK
    };
}

EasyWinIni::~EasyWinIni()
{

}

const QColor &EasyWinIni::textStyleColor(const TextStyleColor &style) const
{
    return m_TextStyleColors[style];
}

QColor EasyWinIni::textStyleColor(const QString &shortname) const
{
    QColor color;
    int index = metaObject()->indexOfEnumerator("TextStyleColor");
    QMetaEnum styleEnum = metaObject()->enumerator(index);

    int size = styleEnum.keyCount();
    for (int i = 0; i < size; i++)
    {
        QString key = styleEnum.key(i);
        int lastIndex = key.lastIndexOf("_");
        key = key.mid(lastIndex + 1);

        if (key == shortname)
            color = m_TextStyleColors[styleEnum.value(i)];
    }

    return color;
}

const QColor &EasyWinIni::textBackColor(const TextBackColor &style) const
{
    return m_TextStyleColors[style];
}

QColor EasyWinIni::textBackColor(const QString &shortname) const
{
    QColor color;
    int index = metaObject()->indexOfEnumerator("TextBackColor");
    QMetaEnum styleEnum = metaObject()->enumerator(index);

    int size = styleEnum.keyCount();
    for (int i = 0; i < size; i++)
    {
        QString key = styleEnum.key(i);
        int lastIndex = key.lastIndexOf("_");
        key = key.mid(lastIndex + 1);

        if (key == shortname)
            color = m_TextBackColors[styleEnum.value(i)];
    }

    return color;
}
