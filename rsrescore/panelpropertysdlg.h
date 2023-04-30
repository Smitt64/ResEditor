#ifndef PANELPROPERTYSDLG_H
#define PANELPROPERTYSDLG_H

#include <QDialog>

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

private:
    void setWidgetData(QWidget *widget, const QString &propertyName);
    Ui::PanelPropertysDlg *ui;

    CustomRectItem *m_pItem;
    EnumListModel *m_pBorderStyle;
    EnumListModel *m_pPanelStyle;
};

#endif // PANELPROPERTYSDLG_H
