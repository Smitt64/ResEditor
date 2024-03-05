#include "controlpropertysdlg.h"
#include "controlitem.h"
#include "enumlistmodel.h"
#include "ui_controlpropertysdlg.h"
#include <QMetaProperty>
#include <limits>

ControlPropertysDlg::ControlPropertysDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ControlPropertysDlg)
{
    ui->setupUi(this);
    ui->tabOrderWidget->adjustSize();
    adjustSize();

    m_pFieldType = new EnumListModel(this);
    m_pFieldType->loadFromJsonFile(":/json/FieldTypeEnum.json");

    m_DataType = new EnumListModel(this);
    m_DataType->loadFromJsonFile(":/json/DataTypeEnum.json");

    m_pStyle = new EnumListModel(this);
    m_pStyle->loadFromJsonFile(":/json/PanelStyleEnum.json");

    ui->fieldTypeBox->setModel(m_pFieldType);
    ui->dataTypeBox->setModel(m_DataType);
    ui->controlStyle->setModel(m_pStyle);

    ui->helpPage->setMaximum(std::numeric_limits<qint16>::max());
    ui->helpPage->setMinimum(0);
}

ControlPropertysDlg::~ControlPropertysDlg()
{
    delete ui;
}

template<class T> T getWidgetData(QWidget *widget)
{
    const QMetaObject *meta = widget->metaObject();
    const char *propertyName = meta->userProperty().name();

    return widget->property(propertyName).value<T>();
}

quint16 ControlPropertysDlg::dataLength() const
{
    return getWidgetData<quint16>(ui->dataLength);
}

quint16 ControlPropertysDlg::length() const
{
    return getWidgetData<quint16>(ui->length);
}

quint16 ControlPropertysDlg::lines() const
{
    return getWidgetData<quint16>(ui->lines);
}

quint16 ControlPropertysDlg::point() const
{
    return getWidgetData<quint16>(ui->point);
}

quint16 ControlPropertysDlg::controlGroup() const
{
    return getWidgetData<quint16>(ui->controlGroup);
}

bool ControlPropertysDlg::fdm() const
{
    return getWidgetData<bool>(ui->fdmBox);
}

bool ControlPropertysDlg::isText() const
{
    return getWidgetData<bool>(ui->isText);
}

QString ControlPropertysDlg::nameText() const
{
    return getWidgetData<QString>(ui->nameTextEdit);
}

QString ControlPropertysDlg::valueTemplate() const
{
    return getWidgetData<QString>(ui->valueTemplate);
}

quint16 ControlPropertysDlg::helpPage() const
{
    return getWidgetData<quint16>(ui->helpPage);
}

QString ControlPropertysDlg::controlName() const
{
    return getWidgetData<QString>(ui->controlName);
}

QString ControlPropertysDlg::toolTip() const
{
    return getWidgetData<QString>(ui->toolTip);
}

void ControlPropertysDlg::setControlItem(ControlItem *item)
{
    m_pItem = item;

    m_pFieldType->setMetaEnum(item, "fieldType");
    m_DataType->setMetaEnum(item, "dataType");
    m_pStyle->setMetaEnum(item, "controlStyle");

    setIntWidgetLimit(ui->dataLength);
    setIntWidgetLimit(ui->length);
    setIntWidgetLimit(ui->lines);
    setIntWidgetLimit(ui->point);
    setIntWidgetLimit(ui->helpPage);
    setIntWidgetLimit(ui->controlGroup);

    setWidgetData(ui->dataLength, "dataLength");
    setWidgetData(ui->length, "length");
    setWidgetData(ui->lines, "lines");
    setWidgetData(ui->point, "signs");
    setWidgetData(ui->fdmBox, "fdm");
    setWidgetData(ui->isText, "isText");
    setWidgetData(ui->nameTextEdit, "controlName2");
    setWidgetData(ui->controlGroup, "controlGroup");
    setWidgetData(ui->valueTemplate, "valueTemplate");
    setWidgetData(ui->helpPage, "helpPage");
    setWidgetData(ui->controlName, "controlName");
    setWidgetData(ui->toolTip, "toolTip");

    QVariant fieldType = m_pItem->property("fieldType");
    QVariant dataType = m_pItem->property("dataType");
    QVariant controlStyle = m_pItem->property("controlStyle");

    ui->fieldTypeBox->setCurrentIndex(m_pFieldType->indexFromValue(fieldType.toInt()));
    ui->dataTypeBox->setCurrentIndex(m_DataType->indexFromValue(dataType.toInt()));
    ui->controlStyle->setCurrentIndex(m_pStyle->indexFromValue(controlStyle.toInt()));

    ControlItem::ControlFlags controlFlags = (ControlItem::ControlFlags)m_pItem->controlFlags();
    ui->flagText->setChecked(controlFlags.testFlag(ControlItem::RF_ASTEXT));
    ui->flagGroup->setChecked(controlFlags.testFlag(ControlItem::RF_GROUP) ||
                              controlFlags.testFlag(ControlItem::RF_GROUPING));

    ui->flagExcludeTab->setChecked(controlFlags.testFlag(ControlItem::RF_NOTABSTOP));
    ui->flagSelectList->setChecked(controlFlags.testFlag(ControlItem::RF_DOWNBTN));

    ControTabOrder tabOrder = m_pItem->tabOrder();
    ui->tabOrderWidget->setValue(&tabOrder);
}

qint32 ControlPropertysDlg::fieldType() const
{
    return m_pFieldType->valueFromIndex(ui->fieldTypeBox->currentIndex());
}

qint32 ControlPropertysDlg::dataType() const
{
    return m_DataType->valueFromIndex(ui->dataTypeBox->currentIndex());
}

qint32 ControlPropertysDlg::style() const
{
    return m_pStyle->valueFromIndex(ui->controlStyle->currentIndex());
}

qint32 ControlPropertysDlg::controlFlags() const
{
    ControlItem::ControlFlags fl;
    fl.setFlag(ControlItem::RF_ASTEXT, ui->flagText->isChecked());
    fl.setFlag(ControlItem::RF_GROUP, ui->flagGroup->isChecked());
    fl.setFlag(ControlItem::RF_GROUPING, ui->flagGroup->isChecked());

    fl.setFlag(ControlItem::RF_NOTABSTOP, ui->flagExcludeTab->isChecked());
    fl.setFlag(ControlItem::RF_DOWNBTN, ui->flagSelectList->isChecked());

    return fl;
}

void ControlPropertysDlg::setIntWidgetLimit(QWidget *widget)
{
    widget->setProperty("minimum", 0);
    widget->setProperty("maximum", std::numeric_limits<quint16>::max());
}

void ControlPropertysDlg::setWidgetData(QWidget *widget, const QString &propertyName)
{
    const char *widgetProperty = widget->metaObject()->userProperty().name();
    widget->setProperty(widgetProperty, m_pItem->property(propertyName.toLocal8Bit().data()));
}
