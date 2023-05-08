#include "controlitem.h"
#include <QPainter>
#include <QFont>
#include <QColor>
#include "basescene.h"
#include "containeritem.h"
#include "controlpropertysdlg.h"
#include "respanel.h"
#include "rsrescore.h"
#include "styles/resstyle.h"
#include "textitem.h"
#include "undoredo/undopropertychange.h"
#include "rscoreheader.h"
#include <QGraphicsScene>
#include <QUndoStack>
#include <QGraphicsView>

ControlItem::ControlItem(QGraphicsItem *parent) :
    CustomRectItem(parent),
    m_pFieldStruct(nullptr),
    m_FieldType(FET),
    m_DataType(INT16),
    m_DataLength(0),
    m_Fdm(false),
    m_Flags(ControlFlags()),
    m_Style(ResStyle::MainStyle),
    m_ControlGroup(0),
    m_HelpPage(0)
{
    setBrush(QBrush(Qt::darkBlue));
    updateCorners();
    setZValue(200.0);
}

ControlItem::~ControlItem()
{
    if (m_pFieldStruct)
        delete m_pFieldStruct;
}

void ControlItem::updateCorners()
{
    ResizeCornersFlags flags = ALL_NO_ROTATE;

    flags.setFlag(LEFT, false);
    flags.setFlag(TOP, false);
    flags.setFlag(TOP_LEFT, false);
    flags.setFlag(BOTTOM_LEFT, false);
    flags.setFlag(TOP_RIGHT, false);
    flags.setFlag(BOTTOM, false);
    flags.setFlag(BOTTOM_RIGHT, false);

    if (m_DataType == PICTURE)
    {
        flags.setFlag(BOTTOM, true);
        flags.setFlag(BOTTOM_RIGHT, true);
    }
    else
    {
        if (m_FieldType == FWR || m_FieldType == FVW)
        {
            flags.setFlag(BOTTOM, true);
            flags.setFlag(BOTTOM_RIGHT, true);
        }

        if ((m_DataType == CHAR || m_DataType == UCHAR) && !m_DataLength)
            flags.setFlag(RIGHT, false);
        else
            flags.setFlag(RIGHT, true);
    }

    setAvailableCorners(flags);
}

void ControlItem::setFieldStruct(struct FieldStruct *value, const int &id)
{
    m_pFieldStruct = new FieldStruct(*value);

    setCoord(QPoint(value->x(), value->y()));
    setSize(QSize(value->len(), value->height()));

    m_FieldType = static_cast<FieldType>(typeF(m_pFieldStruct->_field->Ftype));
    m_Fdm = !isActF(m_pFieldStruct->_field->Ftype);
    //m_FieldType = static_cast<FieldType>(m_pFieldStruct->_field->Ftype);
    m_DataType = static_cast<DataType>(m_pFieldStruct->_field->FVt);
    m_DataLength = m_pFieldStruct->_field->FVp;
    m_ValueTemplate = m_pFieldStruct->formatStr;
    m_ToolTip = m_pFieldStruct->toolTip;
    m_ControlGroup = m_pFieldStruct->_field->group;
    m_HelpPage = m_pFieldStruct->_field->FHelp;
    m_Style = static_cast<ResStyle::PanelStyle>(m_pFieldStruct->_field->St);
    m_ControlName = value->name;
    m_Flags = ControlFlags(m_pFieldStruct->_field->flags);

    m_TabOrder.setPrevious(m_pFieldStruct->_field->kl);
    m_TabOrder.setNext(m_pFieldStruct->_field->kr);
    m_TabOrder.setUp(m_pFieldStruct->_field->ku);
    m_TabOrder.setBottom(m_pFieldStruct->_field->kd);
    m_TabOrder.setThisId(id);

    updateCorners();
}

int ControlItem::lines() const
{
    QSize grid = style()->gridSize();
    return round(boundingRect().height() / grid.height());
}

bool ControlItem::isIntersects(const QRectF &thisBound, QGraphicsItem *item, const QRectF &itemBound) const
{
    TextItem *pIsText = dynamic_cast<TextItem*>(item);
    ContainerItem *pIsContainer = dynamic_cast<ContainerItem*>(item);

    if (pIsText && IsIn(m_FieldType, 2, ControlItem::FBT, ControlItem::FVT) && IsIn(m_DataType, 2, CHAR, UCHAR))
    {
        QUuid attached = pIsText->attachedControl();

        if (attached == uuid())
            return false;
    }
    else if (pIsContainer)
        return false;

    return true;
}

void ControlItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    ResStyleOption opt;
    opt.init(this);
    opt.ftype = style()->controlFromFvt(m_DataType);
    opt.text = style()->controlDefaultText(opt.ftype);

    if (m_FieldType == FBS)
        opt.ftype = ResStyle::Control_Button;

    if (m_Style != ResStyle::MainStyle)
        opt.panelStyle = m_Style;

    if (lines() != 1)
        opt.alignment = Qt::AlignLeft | Qt::AlignTop;

    if (style()->isNumeric(m_DataType))
    {
        opt.alignment &= ~Qt::AlignLeft;
        opt.alignment |= Qt::AlignRight;
    }

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
    checkPropSame("fieldType", val);

    if (isSkipUndoStack() || !undoStack())
    {
        m_FieldType = val;
        updateCorners();
        emit fieldTypeChanged();
        update();
        scene()->update();
    }
    else
    {
        QRect sz = geometry();
        sz.setHeight(1);
        QString msg = UndoPropertyChange::ChangePropertyMsg("fieldType", metaObject());

        undoStack()->beginMacro(msg);
        pushUndoPropertyData("fieldType", val);
        pushUndoPropertyData("geometry", sz);
        undoStack()->endMacro();
    }
}

const ControlItem::DataType &ControlItem::dataType() const
{
    return m_DataType;
}

void ControlItem::setDataType(const ControlItem::DataType &val)
{
    checkPropSame("dataType", val);

    if (isSkipUndoStack() || !undoStack())
    {
        m_DataType = val;
        updateCorners();
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
    checkPropSame("dataLength", val);

    if (isSkipUndoStack() || !undoStack())
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
    checkPropSame("fdm", val);

    if (isSkipUndoStack() || !undoStack())
    {
        m_Fdm = val;
        emit fdmChanged();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("fdm", val);
}

bool ControlItem::isText() const
{
    return m_Flags & RF_ASTEXT;
}

void ControlItem::setIsText(const bool &val)
{
    checkPropSame("isText", val);

    if (isSkipUndoStack() || !undoStack())
    {
        if (val)
            m_Flags |= RF_ASTEXT;
        else
            m_Flags &= ~RF_ASTEXT;

        emit isTextChanged();
        emit controlFlagsChanged();
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
    checkPropSame("controlName", val);

    if (isSkipUndoStack() || !undoStack())
    {
        m_ControlName = val;
        emit controlNameChanged();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("controlName", val);
}

const QString &ControlItem::controlName2() const
{
    return m_ControlName2;
}

void ControlItem::setControlName2(const QString &val)
{
    checkPropSame("controlName2", val);

    if (isSkipUndoStack() || !undoStack())
    {
        m_ControlName2 = val;
        emit controlName2Changed();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("controlName2", val);
}

const QString &ControlItem::toolTip() const
{
    return m_ToolTip;
}

void ControlItem::setToolTip(const QString &val)
{
    checkPropSame("toolTip", val);

    if (isSkipUndoStack() || !undoStack())
    {
        m_ToolTip = val;
        emit toolTipChanged();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("toolTip", val);
}

const ResStyle::PanelStyle &ControlItem::controlStyle() const
{
    return m_Style;
}

void ControlItem::setControlStyle(const ResStyle::PanelStyle &val)
{
    checkPropSame("controlStyle", val);

    if (isSkipUndoStack() || !undoStack())
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
    return m_ValueTemplate;
}

void ControlItem::setValueTemplate(const QString &val)
{
    checkPropSame("valueTemplate", val);

    if (isSkipUndoStack() || !undoStack())
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
    checkPropSame("controlGroup", val);

    if (isSkipUndoStack() || !undoStack())
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
    checkPropSame("helpPage", val);

    if (isSkipUndoStack() || !undoStack())
    {
        m_HelpPage = val;
        emit helpPageChanged();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("helpPage", val);
}

ControTabOrder ControlItem::tabOrder() const
{
    return m_TabOrder;
}

void ControlItem::setTabOrder(const ControTabOrder &val)
{
    checkPropSame("tabOrder", QVariant::fromValue(val));

    if (isSkipUndoStack() || !undoStack())
    {
        m_TabOrder = val;
        emit tabOrderChanged();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("tabOrder", QVariant::fromValue(val));
}

quint32 ControlItem::controlFlags() const
{
    return m_Flags;
}

void ControlItem::setControlFlags(quint32 val)
{
    checkPropSame("controlFlags", val);

    if (isSkipUndoStack() || !undoStack())
    {
        m_Flags = ControlFlags(val);
        emit controlFlagsChanged();
        emit isTextChanged();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("controlFlags", val);
}

QVariant ControlItem::userAction(const qint32 &action, const QVariant &param)
{
    BaseScene* customScene = qobject_cast<BaseScene*> (scene());
    if (action == ActionKeyEnter)
    {
        QGraphicsView *view = customScene->views().first();
        ControlPropertysDlg dlg(view);

        if (dlg.exec() == QDialog::Accepted)
            ;
    }

    return QVariant();
}
