#ifndef EASYWININI_H
#define EASYWININI_H

#include <QObject>
#include <QGlobalStatic>
#include <QVector>

class EasyWinIni : public QObject
{
    Q_OBJECT
public:
    enum TextStyleColor
    {
        TEXT_STYLE_BLUE,
        TEXT_STYLE_GREEN,
        TEXT_STYLE_CYAN,
        TEXT_STYLE_RED,
        TEXT_STYLE_MAGENTA,
        TEXT_STYLE_BROWN,
        TEXT_STYLE_LTGRAY,
        TEXT_STYLE_DKGRAY,
        TEXT_STYLE_LTBLUE,
        TEXT_STYLE_LTGREEN,
        TEXT_STYLE_LTCYAN,
        TEXT_STYLE_LTRED,
        TEXT_STYLE_LTMAGENTA,
        TEXT_STYLE_YELLOW,
        TEXT_STYLE_WHITE,
    };

    enum TextBackColor
    {
        BACK_STYLE_BLUE,
        BACK_STYLE_GREEN,
        BACK_STYLE_CYAN,
        BACK_STYLE_RED,
        BACK_STYLE_MAGENTA,
        BACK_STYLE_BROWN,
        BACK_STYLE_LTGRAY,
        BACK_STYLE_DKGRAY,
        BACK_STYLE_LTBLUE,
        BACK_STYLE_LTGREEN,
        BACK_STYLE_LTCYAN,
        BACK_STYLE_LTRED,
        BACK_STYLE_LTMAGENTA,
        BACK_STYLE_YELLOW,
        BACK_STYLE_BLACK,
    };

    Q_ENUM(TextStyleColor)
    Q_ENUM(TextBackColor)

    EasyWinIni();
    virtual ~EasyWinIni();

    const QColor &textStyleColor(const TextStyleColor &style) const;
    QColor textStyleColor(const QString &shortname) const;

    const QColor &textBackColor(const TextBackColor &style) const;
    QColor textBackColor(const QString &shortname) const;

private:
    QVector<QColor> m_TextStyleColors;
    QVector<QColor> m_TextBackColors;
};

Q_GLOBAL_STATIC(EasyWinIni, easyWin)

#endif // EASYWININI_H
