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

    quint16 dataLength() const;
    quint16 length() const;
    quint16 lines() const;
    quint16 point() const;
    quint16 controlGroup() const;
    quint16 helpPage() const;
    bool fdm() const;
    bool isText() const;
    QString nameText() const;
    QString valueTemplate() const;
    QString controlName() const;
    QString toolTip() const;

    qint32 fieldType() const;
    qint32 dataType() const;
    qint32 style() const;

    qint32 controlFlags() const;

private:
    void setWidgetData(QWidget *widget, const QString &propertyName);
    void setIntWidgetLimit(QWidget *widget);

    Ui::ControlPropertysDlg *ui;

    ControlItem *m_pItem;
    EnumListModel *m_pFieldType, *m_DataType, *m_pStyle;
};

#endif // CONTROLPROPERTYSDLG_H
