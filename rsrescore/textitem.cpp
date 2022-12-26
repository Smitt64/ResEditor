#include "textitem.h"
#include <QColor>
#include "qgraphicsscene.h"
#include "respanel.h"
#include "styles/resstyle.h"
#include "undoredo/undopropertychange.h"
#include <QPainter>
#include <QFont>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QUndoStack>

#define CHECK_STR "[ ]"
#define RADIO_STR "( )"

TextItem::TextItem(QGraphicsItem* parent) :
    CustomRectItem(parent),
    m_Type(TypeLabel),
    m_CheckRadioPos(-1)
{
    m_TextStyle = new EwTextStyle(this);

    setBrush(QBrush(Qt::darkBlue));
    setAvailableCorners(0);
}

TextItem::~TextItem()
{

}

void TextItem::setTextStruct(TextStruct *value)
{
    m_Value = value->value;
    detectType();

    setCoord(QPoint(value->x(), value->y()));
    setSize(QSize(value->len(), 1));
}

void TextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    ResStyleOption opt;
    opt.init(this);
    opt.text = m_Value;

    style()->drawControl(ResStyle::Control_Label, painter, &opt);
    CustomRectItem::paint(painter, option, widget);
    drawIntersects(painter);

    painter->save();
    QColor br = style()->color(ResStyle::Color_TextBg, &opt);
    paintBevel(painter, QColor::fromRgb(255 - br.red(), 255 - br.green(), 255 - br.blue()));
    painter->restore();
}

const TextItem::TextItemType &TextItem::textType() const
{
    return m_Type;
}

void TextItem::detectType()
{
    m_CheckRadioPos = m_Value.indexOf("[ ]");

    if (m_CheckRadioPos != -1)
        m_Type = TypeCheck;

    if (m_Type == TypeLabel)
    {
        m_CheckRadioPos = m_Value.indexOf("( )");

        if (m_CheckRadioPos != -1)
            m_Type = TypeRadio;
    }

    if (m_Type == TypeCheck || m_Type == TypeRadio)
        setAvailableCorners(RIGHT);
}

bool TextItem::canResize(const QRectF &newRect, const ResizeCorners &corner) const
{
    bool fResize = CustomRectItem::canResize(newRect, corner);

    if (fResize)
    {
        if (m_Type == TypeCheck || m_Type == TypeRadio)
        {
            QSize gridSize = style()->gridSize();
            QString tmp = m_Value;

            if (m_CheckRadioPos == 0)
                tmp = tmp.remove(m_CheckRadioPos, 3);
            else
                tmp = tmp.mid(0, m_CheckRadioPos);

            tmp = tmp.trimmed();

            int newWidth = round(newRect.width() / gridSize.width());
            QString newText = checkRadioStr(m_Type) + QString(1, QChar::Space) + tmp;

            if (newWidth < newText.length())
                fResize = false;
        }
    }

    return fResize;
}

QString TextItem::checkRadioStr(const TextItemType &type) const
{
    QString value;

    switch(type)
    {
    case TypeCheck:
        value = CHECK_STR;
        break;
    case TypeRadio:
        value = RADIO_STR;
        break;
    }

    return value;
}

void TextItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    CustomRectItem::mouseMoveEvent(event);

    if (isResizing())
    {
        if (m_Type == TypeCheck || m_Type == TypeRadio)
        {
            QRectF rect = boundingRect();

            QSize gridSize = style()->gridSize();
            QString tmp = m_Value;

            if (m_CheckRadioPos == 0)
                tmp = tmp.remove(m_CheckRadioPos, 3);
            else
                tmp = tmp.mid(0, m_CheckRadioPos);

            tmp = tmp.trimmed();

            int newWidth = round(rect.width() / gridSize.width());
            int spaceLen = abs(newWidth - tmp.length()) - 3;

            if (m_CheckRadioPos == 0)
            {
                QString newText = checkRadioStr(m_Type) + QString(spaceLen, QChar::Space) + tmp;
                m_Value = newText;
            }
            else
            {
                QString newText = tmp + QString(spaceLen, QChar::Space) + checkRadioStr(m_Type);
                m_Value = newText;
            }
        }
    }
}

QString TextItem::text() const
{
    return m_Value;
}

void TextItem::setText(const QString &txt)
{
    checkPropSame("text", txt);

    QRect sz = geometry();
    sz.setWidth(txt.length());

    if (isSkipUndoStack())
    {
        m_Value = txt;
        setGeometry(sz);

        emit textChanged();
        emit geometryChanged();

        update();
        scene()->update();
    }
    else
    {
        QRect sz = geometry();
        sz.setWidth(txt.length());
        QString msg = UndoPropertyChange::ChangePropertyMsg("text", metaObject());

        undoStack()->beginMacro(msg);
        pushUndoPropertyData("text", txt);
        pushUndoPropertyData("geometry", sz);
        undoStack()->endMacro();
    }
}

EwTextStyle TextItem::textStyle()
{
    return m_TextStyle;
}

void TextItem::setTextStyle(const EwTextStyle &style)
{
    checkPropSame("textStyle", QVariant::fromValue(style));

    if (isSkipUndoStack())
    {
        m_TextStyle = style;
        emit textStyleChanged();

        update();
        scene()->update();
    }
    else
    {
        pushUndoPropertyData("textStyle", QVariant::fromValue(style));
    }
}
