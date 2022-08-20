#include "controlitem.h"
#include <QPainter>
#include <QFont>
#include <QColor>
#include "respanel.h"
#include "styles/resstyle.h"
#include "rscoreheader.h"
#include <QGraphicsScene>

ControlItem::ControlItem(QGraphicsItem *parent) :
    CustomRectItem(parent),
    m_pFieldStruct(nullptr),
    m_FieldType(FET),
    m_DataType(INT16),
    m_DataLength(0),
    m_Fdm(false),
    m_IsText(false),
    m_Style(ResStyle::MainStyle),
    m_ControlGroup(0),
    m_HelpPage(0)
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

    setCoord(QPoint(value->x(), value->y()));
    setSize(QSize(value->len(), value->height()));

    m_FieldType = static_cast<FieldType>(m_pFieldStruct->_field->Ftype);
    m_DataType = static_cast<DataType>(m_pFieldStruct->_field->FVt);
    m_DataLength = m_pFieldStruct->_field->FVp;
    m_ValueTemplate = m_pFieldStruct->formatStr;
    m_ControlGroup = m_pFieldStruct->_field->group;
    m_HelpPage = m_pFieldStruct->_field->FHelp;
}

int ControlItem::lines() const
{
    QSize grid = style()->gridSize();
    return round(boundingRect().height() / grid.height());
}

void ControlItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    ResStyleOption opt;
    opt.init(this);
    opt.ftype = style()->controlFromFvt(m_DataType);
    opt.text = style()->controlDefaultText(opt.ftype);

    if (m_Style != ResStyle::MainStyle)
        opt.panelStyle = m_Style;

    if (lines() != 1)
        opt.alignment = Qt::AlignLeft | Qt::AlignTop;

    style()->drawControl(opt.ftype, painter, &opt);
    CustomRectItem::paint(painter, option, widget);
    drawIntersects(painter);

    painter->save();
    QColor br = style()->color(ResStyle::Color_TextBg, &opt);
    paintBevel(painter, QColor::fromRgb(255 - br.red(), 255 - br.green(), 255 - br.blue()));
    painter->restore();
}

const ControlItem::FieldType &ControlItem::fieldType() const
{
    return m_FieldType;
}

void ControlItem::setFieldType(const ControlItem::FieldType &val)
{
    if (isSkipUndoStack())
    {
        m_FieldType = val;
        emit fieldTypeChanged();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("fieldType", val);
}

const ControlItem::DataType &ControlItem::dataType() const
{
    return m_DataType;
}

void ControlItem::setDataType(const ControlItem::DataType &val)
{
    if (isSkipUndoStack())
    {
        m_DataType = val;
        emit dataTypeChanged();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("dataType", val);
}

const quint16 &ControlItem::dataLength() const
{
    return m_DataLength;
}

void ControlItem::setDataLength(const quint16 &val)
{
    if (isSkipUndoStack())
    {
        m_DataLength = val;
        emit dataLengthChanged();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("dataLength", val);
}

quint16 ControlItem::length() const
{
    return getSize().width();
}

const bool &ControlItem::fdm() const
{
    return m_Fdm;
}

void ControlItem::setFdm(const bool &val)
{
    if (isSkipUndoStack())
    {
        m_Fdm = val;
        emit fdmChanged();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("fdm", val);
}

const bool &ControlItem::isText() const
{
    return m_IsText;
}

void ControlItem::setIsText(const bool &val)
{
    if (isSkipUndoStack())
    {
        m_IsText = val;
        emit isTextChanged();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("isText", val);
}

const QString &ControlItem::controlName() const
{
    return m_ControlName;
}

void ControlItem::setControlName(const QString &val)
{
    if (isSkipUndoStack())
    {
        m_ControlName = val;
        emit controlNameChanged();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("controlName", val);
}

const ResStyle::PanelStyle &ControlItem::controlStyle() const
{
    return m_Style;
}

void ControlItem::setControlStyle(const ResStyle::PanelStyle &val)
{
    if (isSkipUndoStack())
    {
        m_Style = val;
        emit controlStyleChanged();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("controlStyle", val);
}

const QString &ControlItem::valueTemplate() const
{
    return m_ControlName;
}

void ControlItem::setValueTemplate(const QString &val)
{
    if (isSkipUndoStack())
    {
        m_ValueTemplate = val;
        emit valueTemplateChanged();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("valueTemplate", val);
}

const quint16 &ControlItem::controlGroup() const
{
    return m_ControlGroup;
}

void ControlItem::setControlGroup(const quint16 &val)
{
    if (isSkipUndoStack())
    {
        m_ControlGroup = val;
        emit controlGroupChanged();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("controlGroup", val);
}

const quint16 &ControlItem::helpPage() const
{
    return m_HelpPage;
}

void ControlItem::setHelpPage(const quint16 &val)
{
    if (isSkipUndoStack())
    {
        m_HelpPage = val;
        emit helpPageChanged();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("helpPage", val);
}
