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

QIcon IconForDataType(int DataType)
{
    QIcon icon;

    switch(DataType)
    {
    case ControlItem::INT16:
        icon = QIcon(":/img/TextBoxI16_24x.png");
        break;
    case ControlItem::INT32:
        icon = QIcon(":/img/TextBoxI32_24x.png");
        break;
    case ControlItem::INT64:
        icon = QIcon(":/img/TextBox_24x.png");
        break;
    case ControlItem::FLOAT:
    case ControlItem::FLOATG:
    case ControlItem::DOUBLE:
    case ControlItem::DOUBLEG:
    case ControlItem::MONEY:
    case ControlItem::MONEYR:
    case ControlItem::LMONEY:
    case ControlItem::LMONEYR:
    case ControlItem::DECIMAL:
    case ControlItem::NUMERIC:
         icon = QIcon(":/img/TextBoxMoney_24x.png");
        break;
    case ControlItem::STRING:
    case ControlItem::SNR:
    case ControlItem::NUMSTR:
        icon = QIcon(":/img/TextBox_24x.png");
        break;
    case ControlItem::DATE:
        icon = QIcon(":/img/DatePicker_24x.png");
        break;
    case ControlItem::TIME:
    case ControlItem::SHTM:
        icon = QIcon(":/img/TimePicker_24x.png");
        break;
    case ControlItem::CHAR:
    case ControlItem::UCHAR:
        icon = QIcon(":/img/TextBox_24x.png");
        break;
    case ControlItem::PICTURE:
        icon = QIcon(":/img/Image_24x.png");
        break;
    }

    return icon;
}

ControlItem::ControlItem(QGraphicsItem *parent) :
    CustomRectItem(parent),
    m_pFieldStruct(nullptr),
    m_FieldType(FET),
    m_DataType(INT16),
    m_DataLength(0),
    m_Point(0),
    m_Fdm(false),
    m_isText(false),
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
    m_isText = (bool)(!m_pFieldStruct->_field->vfl);
    //m_FieldType = static_cast<FieldType>(m_pFieldStruct->_field->Ftype);
    m_DataType = static_cast<DataType>(m_pFieldStruct->_field->FVt);
    m_DataLength = m_pFieldStruct->_field->FVp;
    m_ValueTemplate = m_pFieldStruct->formatStr;
    m_ToolTip = m_pFieldStruct->toolTip;
    m_ControlGroup = m_pFieldStruct->_field->group;
    m_HelpPage = m_pFieldStruct->_field->FHelp;

    qint16 st = m_pFieldStruct->_field->St >> 8;
    m_Style = static_cast<ResStyle::PanelStyle>((st) - 1);

    m_ControlName = value->name;
    m_ControlName2 = value->name2;
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
    {
        undoStack()->beginMacro(UndoPropertyChange::ChangePropertyMsg("dataType", metaObject()));
        pushUndoPropertyData("dataType", val);

        if (val == DATE)
        {
            QSize sz = getSize();
            sz.setWidth(10);

            pushUndoPropertyData("dataLength", 10);
            pushUndoPropertyData("size", sz);
        }
        else if (val == TIME)
        {
            QSize sz = getSize();
            sz.setWidth(8);

            pushUndoPropertyData("dataLength", 8);
            pushUndoPropertyData("size", sz);
        }
        else if (val == SHTM)
        {
            QSize sz = getSize();
            sz.setWidth(5);

            pushUndoPropertyData("dataLength", 5);
            pushUndoPropertyData("size", sz);
        }
        undoStack()->endMacro();
    }
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
    return m_isText;
}

void ControlItem::setIsText(const bool &val)
{
    checkPropSame("isText", val);

    if (isSkipUndoStack() || !undoStack())
    {
        m_isText = val;

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

const quint16 &ControlItem::signs() const
{
    return m_Point;
}

void ControlItem::setSigns(const quint16 &val)
{
    checkPropSame("signs", val);

    if (isSkipUndoStack() || !undoStack())
    {
        m_Point = val;
        emit signsChanged();
        update();
        scene()->update();
    }
    else
        pushUndoPropertyData("signs", val);
}

#define RESET_DLG_FLAG(flag) thisFlags.setFlag(flag, dlgFlags.testFlag(flag))
void ControlItem::FillItemControl(ControlPropertysDlg &dlg)
{
    setDataLength(dlg.dataLength());
    setSigns(dlg.point());
    setHelpPage(dlg.helpPage());
    setControlGroup(dlg.controlGroup());
    setControlName(dlg.controlName());
    setControlName2(dlg.nameText());
    setValueTemplate(dlg.valueTemplate());
    setToolTip(dlg.toolTip());
    setFdm(dlg.fdm());

    setFieldType((ControlItem::FieldType)dlg.fieldType());
    setDataType((ControlItem::DataType)dlg.dataType());
    setControlStyle((ResStyle::PanelStyle)dlg.style());

    ControlFlags thisFlags = (ControlFlags)controlFlags();
    ControlFlags dlgFlags = (ControlFlags)dlg.controlFlags();

    RESET_DLG_FLAG(RF_ASTEXT);
    RESET_DLG_FLAG(RF_GROUP);
    RESET_DLG_FLAG(RF_GROUPING);
    RESET_DLG_FLAG(RF_NOTABSTOP);
    RESET_DLG_FLAG(RF_DOWNBTN);

    setControlFlags(thisFlags);
}

QVariant ControlItem::userAction(const qint32 &action, const QVariant &param)
{
    BaseScene* customScene = qobject_cast<BaseScene*> (scene());
    if (action == ActionKeyEnter)
    {
        QGraphicsView *view = customScene->views().first();
        ControlPropertysDlg dlg(view);
        dlg.setControlItem(this);

        if (dlg.exec() == QDialog::Accepted)
        {
            undoStack()->beginMacro(tr("Изменение параметров элемента"));
            FillItemControl(dlg);
            undoStack()->endMacro();
        }
    }

    return QVariant();
}
