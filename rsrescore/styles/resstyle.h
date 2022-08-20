#ifndef RESSTYLE_H
#define RESSTYLE_H

#include <QPainter>
#include <QObject>
#include <QMetaObject>
#include <QVariant>
#include <QJsonObject>
#include <QHash>

#define COLORSCHEME "COLORSCHEME"
#define CONTRAST_PROPERTY "contrast"

class CustomRectItem;
class QGraphicsItem;
class ResStyleOption;
class ResStyle : public QObject
{
    Q_OBJECT
    Q_CLASSINFO(COLORSCHEME, ":/json/ResStyleColoSheme.json")
public:
    enum StyleColor
    {
        Color_SceneBackground = 0,
        Color_ContainerBg,
        Color_BorderColor,
        Color_TextBg,
        Color_Text,
        Color_ControlBg,
        Color_ControlText,

        Color_Count
    };

    enum BorderStyle
    {
        Border_NoLine,
        Border_SingleLine,
        Border_DoubleLine,
        Border_Combine1,
        Border_Combine2,
        Border_Solid,

        Border_Count
    };

    enum ControlType
    {
        Control_Unknown = -1,
        Control_Label,
        Control_Panel,
        Control_Date,
        Control_Time,
        Control_String,
        Control_Char,
        Control_UChar,
        Control_Integer,
        Control_Float,
        Control_Money,
        Control_Double,
    };

    enum PanelStyle
    {
        MainStyle = 0,
        SCOM, // Основной стиль
        SMES, // Стиль сообщений
        RMES, // Стиль аварийных сообщений
        SHLP, // Стиль помощи
        SMEN, // Стиль меню
        SBCM, // Стиль с яркой рамкой
        SCRL, // Стиль справочных скролингов
    };

    typedef struct BorderChars
    {
        QChar chtopleft;
        QChar chtopright;
        QChar horline;
        QChar verline;
        QChar chbotleft;
        QChar chbotright;
    }BorderChars;

    Q_ENUM(BorderStyle)
    Q_ENUM(PanelStyle)
    Q_ENUM(StyleColor)

    ResStyle();

    virtual QSize gridSize() const;
    virtual QColor color(const StyleColor &type, ResStyleOption *option = nullptr) const;
    virtual BorderChars borderChars(const BorderStyle &style) const;
    virtual QFont font() const;

    virtual void drawSceneBackground(QPainter *painter, const QRectF &rect, ResStyleOption *option = nullptr);
    virtual void drawBorder(QPainter *painter, const BorderStyle &bs, const QRectF &rect, const QString &text, ResStyleOption *option);
    virtual void drawText(QPainter *painter, const QRectF &rect, const QString &text, const Qt::Alignment &alignment = Qt::AlignLeft | Qt::AlignVCenter, const QColor &penColor = Qt::white);

    virtual void drawControl(const ControlType &type, QPainter *painter, ResStyleOption *option);

    ControlType controlFromFvt(const quint8 &fvt) const;
    virtual QString controlDefaultText(const ControlType &type);
protected:
    typedef struct ColorScheme
    {
        QHash<StyleColor, QColor> Colors;
        QColor ContainerContrast;
        QString name;
    }ColorScheme;

    QHash<PanelStyle, ColorScheme> m_ColorScheme;

    QSize getSizeFromItem(const QRectF &rect);
    BorderChars m_BorderChars[Border_Count];
    QFont m_Font;

private:
    void loadDefaultColors();
    void loadColorSheme(const QJsonObject &obj, ColorScheme &sheme);
    int enumValueFromName(const QString &enumname, const QString &name) const;
};

class ResStyleOption
{
public:
    ResStyleOption();
    void init(CustomRectItem *item);

    QRectF rect;
    QString text;
    ResStyle::BorderStyle borderStyle;
    ResStyle::ControlType ftype;
    ResStyle::PanelStyle panelStyle;
    bool contrast;

    Qt::Alignment alignment;

protected:
    inline void castToGrpagicsItem(CustomRectItem *item, QGraphicsItem **out);
    inline void castToCustomRectItem(QGraphicsItem *item, CustomRectItem **out);
    inline const QMetaObject *CustomRectItemMetaobject(CustomRectItem *item);
    inline QVariant CustomRectItemProperty(CustomRectItem *item, const char *propertyName);
    inline QGraphicsItem *parentItem(QGraphicsItem *item);

    template<class T>T readProperty(CustomRectItem *item, const char *propertyName, const T &_default = T())
    {
        T retVal = _default;
        QGraphicsItem *curentItem = nullptr;
        castToGrpagicsItem(item, &curentItem);

        while (curentItem)
        {
            CustomRectItem *curentRectItem = nullptr;
            castToCustomRectItem(curentItem, &curentRectItem);

            if (curentRectItem)
            {
                QVariant val = CustomRectItemProperty(curentRectItem, propertyName);

                if (val.isValid())
                {
                    retVal = val.value<T>();
                    break;
                }
            }

            curentItem = parentItem(curentItem);
        }

        return retVal;
    }
};

#endif // RESSTYLE_H
