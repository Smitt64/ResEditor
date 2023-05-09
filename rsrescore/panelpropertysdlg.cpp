#include "panelpropertysdlg.h"
#include "propertymodel/enumlistmodel.h"
#include "ui_panelpropertysdlg.h"
#include "customrectitem.h"
#include "panelitem.h"
#include <limits.h>

PanelPropertysDlg::PanelPropertysDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PanelPropertysDlg)
{
    ui->setupUi(this);

    m_pBorderStyle = new EnumListModel(this);
    m_pBorderStyle->loadFromJsonFile(":/json/BorderStyle.json");

    m_pPanelStyle = new EnumListModel(this);
    m_pPanelStyle->loadFromJsonFile(":/json/PanelStyleEnum.json");

    ui->borderStyle->setModel(m_pBorderStyle);
    ui->panelStyle->setModel(m_pPanelStyle);

    ui->helpPage->setMaximum(std::numeric_limits<quint16>::max());
}

PanelPropertysDlg::~PanelPropertysDlg()
{
    delete ui;
}

void PanelPropertysDlg::setPanelMode(const bool &enabled)
{
    ui->groupBoxScrol->setVisible(!enabled);
    ui->typeBox->clear();
    ui->typeBox->addItem("PANEL");
    adjustSize();
}

void PanelPropertysDlg::setWidgetData(QWidget *widget, const QString &propertyName)
{
    const char *widgetProperty = widget->metaObject()->userProperty().name();
    widget->setProperty(widgetProperty, m_pItem->property(propertyName.toLocal8Bit().data()));
}

void PanelPropertysDlg::setRectItem(CustomRectItem *item)
{
    m_pItem = item;

    m_pBorderStyle->setMetaEnum(item, "borderStyle");
    m_pPanelStyle->setMetaEnum(item, "panelStyle");

    setWidgetData(ui->titleEdit, "title");
    setWidgetData(ui->statusEdit, "status");
    setWidgetData(ui->statusEdit2, "status2");
    setWidgetData(ui->helpPage, "helpPage");
    setWidgetData(ui->commentEdit, "comment");

    QVariant borderStyle = m_pItem->property("borderStyle");
    QVariant panelStyle = m_pItem->property("panelStyle");
    ui->borderStyle->setCurrentIndex(m_pBorderStyle->indexFromValue(borderStyle.toInt()));
    ui->panelStyle->setCurrentIndex(m_pPanelStyle->indexFromValue(panelStyle.toInt()));

    qint32 ExcludeFlags = m_pItem->property("panelExclude").value<qint32>();

    ui->excludeAutoTab->setChecked(ExcludeFlags & PanelItem::ExcludeNavigation);
    ui->excludeAutoNum->setChecked(ExcludeFlags & PanelItem::ExcludeAutoNum);
    ui->excludeShadow->setChecked(ExcludeFlags & PanelItem::ExcludeShadow);

    ui->alignPanCenter->setChecked(m_pItem->property("isCentered").toBool());
    ui->alignTextRight->setChecked(m_pItem->property("isRightText").toBool());
}

ResStyle::BorderStyle PanelPropertysDlg::borderStyle() const
{
    return (ResStyle::BorderStyle)m_pBorderStyle->valueFromIndex(ui->borderStyle->currentIndex());
}

ResStyle::PanelStyle PanelPropertysDlg::panelStyle() const
{
    return (ResStyle::PanelStyle)m_pPanelStyle->valueFromIndex(ui->panelStyle->currentIndex());
}

PanelItem::PanelExcludeFlags PanelPropertysDlg::excludeFlags() const
{
    PanelItem::PanelExcludeFlags flags;
    flags.setFlag(PanelItem::ExcludeAutoNum, ui->excludeAutoNum->isChecked());
    flags.setFlag(PanelItem::ExcludeNavigation, ui->excludeAutoTab->isChecked());
    flags.setFlag(PanelItem::ExcludeShadow, ui->excludeShadow->isChecked());
    return flags;
}

QString PanelPropertysDlg::title() const
{
    return ui->titleEdit->text();
}

QString PanelPropertysDlg::status() const
{
    return ui->statusEdit->text();
}

QString PanelPropertysDlg::status2() const
{
    return ui->statusEdit2->text();
}

quint16 PanelPropertysDlg::helpPage() const
{
    return ui->helpPage->value();
}

bool PanelPropertysDlg::alignTextRight() const
{
    return ui->alignTextRight->isChecked();
}

bool PanelPropertysDlg::alignPanelCenter() const
{
    return ui->alignPanCenter->isChecked();
}
