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

bool ControlPropertysDlg::hasUniformValue(QObject* obj, const char* propertyName) const
{
    if (!obj)
        return true;

    if (!propertyName)
        return true;

    const QMetaObject* metaObject = obj->metaObject();

    int methodIndex = metaObject->indexOfMethod("hasUniformValue(const char*)");
    if (methodIndex == -1)
        methodIndex = metaObject->indexOfMethod("hasUniformValue(QString)");

    if (methodIndex == -1)
        return true;

    QMetaMethod method = metaObject->method(methodIndex);

    bool result = true;
    QGenericReturnArgument returnArg = Q_RETURN_ARG(bool, result);
    QGenericArgument inputArg = Q_ARG(const char*, propertyName);

    bool invokeSuccess = method.invoke(obj,
                                       Qt::DirectConnection,
                                       returnArg,
                                       inputArg);

    if (!invokeSuccess)
        return true;

    return result;
}

void ControlPropertysDlg::setControlItem(CustomRectItem *item)
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

    if (hasUniformValue(m_pItem, "fieldType"))
        ui->fieldTypeBox->setCurrentIndex(m_pFieldType->indexFromValue(fieldType));
    else
        ui->fieldTypeBox->setCurrentIndex(-1);

    if (hasUniformValue(m_pItem, "dataType"))
        ui->dataTypeBox->setCurrentIndex(m_DataType->indexFromValue(dataType));
    else
        ui->dataTypeBox->setCurrentIndex(-1);

    if (hasUniformValue(m_pItem, "controlStyle"))
        ui->controlStyle->setCurrentIndex(m_pStyle->indexFromValue(controlStyle));
    else
        ui->controlStyle->setCurrentIndex(-1);

    ControlItem::ControlFlags controlFlags = (ControlItem::ControlFlags)m_pItem->property("controlFlags").toInt();//m_pItem->controlFlags();
    ui->flagText->setChecked(controlFlags.testFlag(ControlItem::RF_ASTEXT));
    ui->flagGroup->setChecked(controlFlags.testFlag(ControlItem::RF_GROUP) ||
                              controlFlags.testFlag(ControlItem::RF_GROUPING));

    ui->flagExcludeTab->setChecked(controlFlags.testFlag(ControlItem::RF_NOTABSTOP));
    ui->flagSelectList->setChecked(controlFlags.testFlag(ControlItem::RF_DOWNBTN));

    if (hasUniformValue(m_pItem, "tabOrder"))
    {
        QVariant tabOrderVar = m_pItem->property("tabOrder");
        ControTabOrder tabOrder = tabOrderVar.value<ControTabOrder>();
        ui->tabOrderWidget->setValue(&tabOrder);
        ui->tabOrderWidget->setEnabled(true);
    }
    else
        ui->tabOrderWidget->setEnabled(false);

    saveInitialUIState();
}

qint32 ControlPropertysDlg::fieldType() const
{
    if (ui->fieldTypeBox->currentIndex() < 0)
        return -1;
    return m_pFieldType->valueFromIndex(ui->fieldTypeBox->currentIndex());
}

qint32 ControlPropertysDlg::dataType() const
{
    if (ui->dataTypeBox->currentIndex() < 0)
        return -1;

    return m_DataType->valueFromIndex(ui->dataTypeBox->currentIndex());
}

qint32 ControlPropertysDlg::style() const
{
    if (ui->controlStyle->currentIndex() < 0)
        return -1;

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

    QCheckBox *check = qobject_cast<QCheckBox*>(widget);
    QVariant var = m_pItem->property(propertyName.toLocal8Bit().data());

    if (var.type() == QVariant::Bool && check)
    {
        if (hasUniformValue(m_pItem, propertyName.toLocal8Bit().data()))
            widget->setProperty(widgetProperty, var);
        else
            check->setCheckState(Qt::PartiallyChecked);
    }
    else
        widget->setProperty(widgetProperty, var);
}

void ControlPropertysDlg::saveInitialUIState()
{
    m_initialUIState.clear();

    // Сохраняем начальные значения из UI
    m_initialUIState["dataLength"] = dataLength();
    m_initialUIState["length"] = length();
    m_initialUIState["lines"] = lines();
    m_initialUIState["signs"] = point();
    m_initialUIState["controlGroup"] = controlGroup();
    m_initialUIState["helpPage"] = helpPage();
    m_initialUIState["fdm"] = fdm();
    m_initialUIState["isText"] = isText();
    m_initialUIState["controlName2"] = nameText();
    m_initialUIState["valueTemplate"] = valueTemplate();
    m_initialUIState["controlName"] = controlName();
    m_initialUIState["toolTip"] = toolTip();
    m_initialUIState["fieldType"] = fieldType();
    m_initialUIState["dataType"] = dataType();
    m_initialUIState["controlStyle"] = style();
    m_initialUIState["controlFlags"] = controlFlags();
}

QMap<QString, QVariant> ControlPropertysDlg::getChangedProperties() const
{
    QMap<QString, QVariant> changedProps;

    // Сравниваем текущие значения UI с начальными
    auto checkChange = [&](const QString& propName) {
        QVariant currentValue = getUIPropertyValue(propName);
        QVariant initialValue = m_initialUIState.value(propName);

        if (currentValue != initialValue) {
            changedProps[propName] = currentValue;
        }
    };

    // Проверяем все свойства
    checkChange("dataLength");
    checkChange("signs");
    checkChange("helpPage");
    checkChange("controlGroup");
    checkChange("controlName");
    checkChange("controlName2");
    checkChange("valueTemplate");
    checkChange("toolTip");
    checkChange("fdm");
    checkChange("fieldType");
    checkChange("dataType");
    checkChange("controlStyle");
    checkChange("controlFlags");

    return changedProps;
}

QVariant ControlPropertysDlg::getUIPropertyValue(const QString &propertyName) const
{
    // Маппинг имен свойств на геттеры
    if (propertyName == "dataLength") return dataLength();
    if (propertyName == "length") return length();
    if (propertyName == "lines") return lines();
    if (propertyName == "signs") return point();
    if (propertyName == "controlGroup") return controlGroup();
    if (propertyName == "helpPage") return helpPage();
    if (propertyName == "fdm") return fdm();
    if (propertyName == "isText") return isText();
    if (propertyName == "controlName2") return nameText();
    if (propertyName == "valueTemplate") return valueTemplate();
    if (propertyName == "controlName") return controlName();
    if (propertyName == "toolTip") return toolTip();
    if (propertyName == "fieldType") return fieldType();
    if (propertyName == "dataType") return dataType();
    if (propertyName == "controlStyle") return style();
    if (propertyName == "controlFlags") return controlFlags();

    return QVariant();
}
