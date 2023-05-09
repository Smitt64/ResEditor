#ifndef PANELPROPERTYSDLG_H
#define PANELPROPERTYSDLG_H

#include <QDialog>
#include "styles/resstyle.h"
#include "panelitem.h"

namespace Ui {
class PanelPropertysDlg;
}

class EnumListModel;
class CustomRectItem;
class PanelPropertysDlg : public QDialog
{
    Q_OBJECT

public:
    explicit PanelPropertysDlg(QWidget *parent = nullptr);
    ~PanelPropertysDlg();

    void setPanelMode(const bool &enabled);
    void setRectItem(CustomRectItem *item);

    ResStyle::BorderStyle borderStyle() const;
    ResStyle::PanelStyle panelStyle() const;
    PanelItem::PanelExcludeFlags excludeFlags() const;
    QString title() const;
    QString status() const;
    QString status2() const;
    quint16 helpPage() const;
    bool alignTextRight() const;
    bool alignPanelCenter() const;

private:
    void setWidgetData(QWidget *widget, const QString &propertyName);
    Ui::PanelPropertysDlg *ui;

    CustomRectItem *m_pItem;
    EnumListModel *m_pBorderStyle;
    EnumListModel *m_pPanelStyle;
};

#endif // PANELPROPERTYSDLG_H
