#include "controlpropertysdlg.h"
#include "controlitem.h"
#include "enumlistmodel.h"
#include "ui_controlpropertysdlg.h"
#include <limits.h>

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

void ControlPropertysDlg::setControlItem(ControlItem *item)
{
    m_pItem = item;

    m_pFieldType->setMetaEnum(item, "fieldType");
    m_DataType->setMetaEnum(item, "dataType");
    m_pStyle->setMetaEnum(item, "controlStyle");

    setWidgetData(ui->dataLength, "dataLength");
    setWidgetData(ui->length, "length");
    setWidgetData(ui->lines, "lines");
    setWidgetData(ui->point, "point");
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

    ui->fieldTypeBox->setCurrentIndex(m_pFieldType->indexFromValue(fieldType.toInt()));
    ui->dataTypeBox->setCurrentIndex(m_DataType->indexFromValue(dataType.toInt()));

    ControlItem::ControlFlags controlFlags = (ControlItem::ControlFlags)m_pItem->controlFlags();
    ui->flagText->setChecked(controlFlags.testFlag(ControlItem::RF_ASTEXT));
    ui->flagGroup->setChecked(controlFlags.testFlag(ControlItem::RF_GROUP) ||
                              controlFlags.testFlag(ControlItem::RF_GROUPING));
    ui->flagExcludeTab->setChecked(controlFlags.testFlag(ControlItem::RF_NOTABSTOP));
    ui->flagSelectList->setChecked(controlFlags.testFlag(ControlItem::RF_DOWNBTN));

    ControTabOrder tabOrder = m_pItem->tabOrder();
    ui->tabOrderWidget->setValue(&tabOrder);
}

void ControlPropertysDlg::setWidgetData(QWidget *widget, const QString &propertyName)
{
    const char *widgetProperty = widget->metaObject()->userProperty().name();
    widget->setProperty(widgetProperty, m_pItem->property(propertyName.toLocal8Bit().data()));
}
