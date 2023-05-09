#ifndef CONTROLPROPERTYSDLG_H
#define CONTROLPROPERTYSDLG_H

#include <QDialog>

namespace Ui {
class ControlPropertysDlg;
}

class ControlItem;
class EnumListModel;
class ControlPropertysDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ControlPropertysDlg(QWidget *parent = nullptr);
    ~ControlPropertysDlg();

    void setControlItem(ControlItem *item);

private:
    void setWidgetData(QWidget *widget, const QString &propertyName);
    Ui::ControlPropertysDlg *ui;

    ControlItem *m_pItem;
    EnumListModel *m_pFieldType, *m_DataType, *m_pStyle;
};

#endif // CONTROLPROPERTYSDLG_H
