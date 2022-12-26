#ifndef EXTEXTSTYLE_H
#define EXTEXTSTYLE_H

#include <QObject>
#include <QJsonObject>
#include <QMetaType>
#include <type_traits>

// Константы выравнивания для TEXT. Находятся в младшем байте старшего слова поля St
/*#define  dTEXT_ALIGN_LEFT          0x00
#define  dTEXT_ALIGN_RIGHT         0x01
#define  dTEXT_ALIGN_CENTER        0x02*/

// Константы цвета для TEXT (палитра CGA/WEB)
/*#define  dTEXT_STYLE_BLACK            0       //
#define  dTEXT_STYLE_BLUE             1       //
#define  dTEXT_STYLE_GREEN            2       //
#define  dTEXT_STYLE_CYAN             3       //
#define  dTEXT_STYLE_RED              4       //
#define  dTEXT_STYLE_MAGENTA          5       //
#define  dTEXT_STYLE_BROWN            6       //
#define  dTEXT_STYLE_LTGRAY           7       //
#define  dTEXT_STYLE_DKGRAY           8       //
#define  dTEXT_STYLE_LTBLUE           9       //
#define  dTEXT_STYLE_LTGREEN         10       //
#define  dTEXT_STYLE_LTCYAN          11       //
#define  dTEXT_STYLE_LTRED           12       //
#define  dTEXT_STYLE_LTMAGENTA       13       //
#define  dTEXT_STYLE_YELLOW          14       //
#define  dTEXT_STYLE_WHITE           15       //*/

// Константы цвета для BACK
/*#define  dBACK_STYLE_WHITE            0       //
#define  dBACK_STYLE_BLUE             1       //
#define  dBACK_STYLE_GREEN            2       //
#define  dBACK_STYLE_CYAN             3       //
#define  dBACK_STYLE_RED              4       //
#define  dBACK_STYLE_MAGENTA          5       //
#define  dBACK_STYLE_BROWN            6       //
#define  dBACK_STYLE_LTGRAY           7       //
#define  dBACK_STYLE_DKGRAY           8       //
#define  dBACK_STYLE_LTBLUE           9       //
#define  dBACK_STYLE_LTGREEN         10       //
#define  dBACK_STYLE_LTCYAN          11       //
#define  dBACK_STYLE_LTRED           12       //
#define  dBACK_STYLE_LTMAGENTA       13       //
#define  dBACK_STYLE_YELLOW          14       //
#define  dBACK_STYLE_BLACK           15       //*/

class EwTextStyle : public QObject
{
    Q_OBJECT
    Q_FLAGS(TextFormatFlags)
    Q_PROPERTY(TextAlign align READ align WRITE setAlign NOTIFY alignChanged)
    Q_PROPERTY(TextFormatFlags format READ format WRITE setFormat NOTIFY formatChanged)
    Q_PROPERTY(TextColor textColor READ textColor WRITE setTextColor NOTIFY textColorChanged)
    Q_PROPERTY(BackColor backColor READ backColor WRITE setBackColor NOTIFY textBackChanged)
public:
    // Константы выравнивания для TEXT. Находятся в младшем байте старшего слова поля St
    enum TextAlign
    {
        dTEXT_ALIGN_LEFT = 0x00,
        dTEXT_ALIGN_RIGHT = 0x01,
        dTEXT_ALIGN_CENTER = 0x02,
    };

    // Константы форматирования для TEXT. Находятся в младшем байте старшего слова поля St
    enum TextFormat
    {
        dTEXT_FORMAT_NORMAL = 0x00,
        dTEXT_FORMAT_BOLD = 0x01,
        dTEXT_FORMAT_ITALIC = 0x02,
        dTEXT_FORMAT_UNDERLINE = 0x04,
        dTEXT_FORMAT_STRIKEOUT = 0x08,
    };

    // Константы цвета для TEXT (палитра CGA/WEB)
    enum TextColor
    {
        dTEXT_STYLE_BLACK = 0,
        dTEXT_STYLE_BLUE = 1,
        dTEXT_STYLE_GREEN = 2,
        dTEXT_STYLE_CYAN = 3,
        dTEXT_STYLE_RED = 4,
        dTEXT_STYLE_MAGENTA = 5,
        dTEXT_STYLE_BROWN = 6,
        dTEXT_STYLE_LTGRAY = 7,
        dTEXT_STYLE_DKGRAY = 8,
        dTEXT_STYLE_LTBLUE = 9,
        dTEXT_STYLE_LTGREEN = 10,
        dTEXT_STYLE_LTCYAN = 11,
        dTEXT_STYLE_LTRED = 12,
        dTEXT_STYLE_LTMAGENTA = 13,
        dTEXT_STYLE_YELLOW = 14,
        dTEXT_STYLE_WHITE = 15,
    };

    // Константы цвета для BACK
    enum BackColor
    {
        dBACK_STYLE_WHITE = 0,
        dBACK_STYLE_BLUE = 1,
        dBACK_STYLE_GREEN = 2,
        dBACK_STYLE_CYAN = 3,
        dBACK_STYLE_RED = 4,
        dBACK_STYLE_MAGENTA = 5,
        dBACK_STYLE_BROWN = 6,
        dBACK_STYLE_LTGRAY = 7,
        dBACK_STYLE_DKGRAY = 8,
        dBACK_STYLE_LTBLUE = 9,
        dBACK_STYLE_LTGREEN = 10,
        dBACK_STYLE_LTCYAN = 11,
        dBACK_STYLE_LTRED = 12,
        dBACK_STYLE_LTMAGENTA = 13,
        dBACK_STYLE_YELLOW = 14,
        dBACK_STYLE_BLACK = 15,
    };

    Q_ENUM(TextAlign)
    Q_ENUM(TextFormat)
    Q_ENUM(TextColor)
    Q_ENUM(BackColor)
    Q_DECLARE_FLAGS(TextFormatFlags, TextFormat)

    Q_INVOKABLE EwTextStyle(QObject *parent = nullptr);
    EwTextStyle(const EwTextStyle &other);
    virtual ~EwTextStyle();

    void fromOther(const EwTextStyle &other);

    const TextAlign &align() const;
    void setAlign(const TextAlign &value);

    const TextFormatFlags &format() const;
    void setFormat(const TextFormatFlags &value);

    const TextColor &textColor() const;
    void setTextColor(const TextColor &value);

    const BackColor &backColor() const;
    void setBackColor(const BackColor &value);

    static QJsonObject toJsonObject(const EwTextStyle &style);
    static EwTextStyle fromJsonObject(const QJsonObject &obj);

    EwTextStyle& operator=(const EwTextStyle& other);
    bool operator ==(const EwTextStyle& other) const;

signals:
    void alignChanged();
    void formatChanged();
    void textColorChanged();
    void textBackChanged();

private:
    TextAlign m_Align = dTEXT_ALIGN_LEFT;
    TextFormatFlags m_Format = dTEXT_FORMAT_NORMAL;
    TextColor m_Textcolor = dTEXT_STYLE_BLACK;
    BackColor m_Back = dBACK_STYLE_WHITE;
};

template<class T> bool operator ==(const T& a1, const EwTextStyle& a2)
{
    if (std::is_same_v<T, EwTextStyle>)
        return a2.operator ==(a1);

    return false;
}

template<class T>bool operator <(const T& a1, const EwTextStyle& a2)
{
    return false;
}

Q_DECLARE_METATYPE(QJsonObject)
Q_DECLARE_METATYPE(EwTextStyle)

Q_DECLARE_OPERATORS_FOR_FLAGS(EwTextStyle::TextFormatFlags)
Q_DECLARE_METATYPE(EwTextStyle::TextFormatFlags)

#endif // EXTEXTSTYLE_H
