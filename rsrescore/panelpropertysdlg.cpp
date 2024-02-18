#include "panelpropertysdlg.h"
#include "propertymodel/enumlistmodel.h"
#include "ui_panelpropertysdlg.h"
#include "customrectitem.h"
#include "panelitem.h"
#include "scrolitem.h"
#include <QResizeEvent>

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

    ui->typeBox->addItem("SCROL");
    ui->typeBox->addItem("BSCROL");
    ui->typeBox->addItem("LSCROL");

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

    if (enabled)
        ui->typeBox->addItem("PANEL");
    else
    {
        ui->typeBox->addItem("SCROL");
        ui->typeBox->addItem("BSCROL");
        ui->typeBox->addItem("LSCROL");
    }
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

    qint32 ScrolFlags = m_pItem->property("scrolFlags").value<qint32>();
    ui->checkAutoFld->setChecked(ScrolFlags & ScrolItem::ScrolAutoFields);
    ui->checkAutoHeader->setChecked(ScrolFlags & ScrolItem::ScrolAutoHeader);
    ui->checkAutoSize->setChecked(ScrolFlags & ScrolItem::ScrolAutoFill);

    QPoint pos = m_pItem->property("scrolPos").toPoint();
    ui->scrolRowH->setValue(m_pItem->property("rowHeight").value<quint16>());
    ui->scrolWidth->setValue(m_pItem->property("rowLength").value<quint16>());
    ui->scrolRows->setValue(m_pItem->property("rowNum").value<quint16>());
    ui->scrolX->setValue(pos.x());
    ui->scrolY->setValue(pos.y());

    qint16 ScrolType = m_pItem->property("scrolType").value<qint16>();

    if (ScrolType == ScrolItem::TypeSCROL)
        ui->typeBox->setCurrentIndex(0);
    else if (ScrolType == ScrolItem::TypeBSCROL)
        ui->typeBox->setCurrentIndex(1);
    else if (ScrolType == ScrolItem::TypeLSCROL)
        ui->typeBox->setCurrentIndex(2);
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

void PanelPropertysDlg::resizeEvent(QResizeEvent *e)
{
    //qDebug() << e->size();
    QDialog::resizeEvent(e);
}

qint32 PanelPropertysDlg::srolFlags() const
{
    qint32 ScrolFlags = 0;

    if (ui->checkAutoFld->isChecked())
        ScrolFlags |= ScrolItem::ScrolAutoFields;

    if (ui->checkAutoSize->isChecked())
        ScrolFlags |= ScrolItem::ScrolAutoFill;

    if (ui->checkAutoHeader->isChecked())
        ScrolFlags |= ScrolItem::ScrolAutoHeader;

    return ScrolFlags;
}

QPoint PanelPropertysDlg::scrolPos() const
{
    return QPoint(ui->scrolX->value(), ui->scrolY->value());
}

quint16 PanelPropertysDlg::rowNum() const
{
    return ui->scrolRows->value();
}

quint16 PanelPropertysDlg::rowLength() const
{
    return ui->scrolWidth->value();
}

quint16 PanelPropertysDlg::rowHeight() const
{
    return ui->scrolRowH->value();
}

quint16 PanelPropertysDlg::scrolType() const
{
    quint16 typew = 0;

    switch(ui->typeBox->currentIndex())
    {
    case 0:
        typew = ScrolItem::TypeSCROL;
        break;
    case 1:
        typew = ScrolItem::TypeBSCROL;
        break;
    case 2:
        typew = ScrolItem::TypeLSCROL;
        break;
    }

    return typew;
}
