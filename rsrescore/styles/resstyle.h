#ifndef RESSTYLE_H
#define RESSTYLE_H

#include <QPainter>
#include <QObject>

class CustomRectItem;
class ResStyleOption
{
public:
    ResStyleOption();
    void init(CustomRectItem *item);

    QRectF rect;
    QString text;
    int borderStyle;
};

class ResStyle : public QObject
{
    Q_OBJECT
public:
    enum StyleColor
    {
        Color_SceneBackground = 0,
        Color_ContainerBg,
        Color_BorderColor,
        Color_TextBg,
        Color_Text,
        Color_ControlBg,
        Color_Control,

        Color_Count
    };

    enum BorderStyle
    {
        Border_NoLine,
        Border_SingleLine,
        Border_DoubleLine,

        Border_Count
    };

    enum ControlType
    {
        Control_Label,
        Control_Panel,
        Control_Date,
        Control_Time,
        Control_String
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

    ResStyle();

    virtual QSize gridSize() const;
    virtual QColor color(const StyleColor &type) const;
    virtual BorderChars borderChars(const BorderStyle &style) const;
    virtual QFont font() const;

    virtual void drawSceneBackground(QPainter *painter, const QRectF &rect);
    virtual void drawBorder(QPainter *painter, const BorderStyle &bs, const QRectF &rect, const QString &text);
    virtual void drawText(QPainter *painter, const QRectF &rect, const QString &text);

    virtual void drawControl(const ControlType &type, QPainter *painter, ResStyleOption *option);

protected:
    QSize getSizeFromItem(const QRectF &rect);
    QColor m_Colors[Color_Count];
    BorderChars m_BorderChars[Border_Count];
    QFont m_Font;
};

#endif // RESSTYLE_H
