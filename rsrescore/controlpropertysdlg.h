#ifndef CONTROLPROPERTYSDLG_H
#define CONTROLPROPERTYSDLG_H

#include <QDialog>
#include <QMap>

namespace Ui {
class ControlPropertysDlg;
}

class CustomRectItem;
class EnumListModel;
class ControlPropertysDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ControlPropertysDlg(QWidget *parent = nullptr);
    ~ControlPropertysDlg();

    void setControlItem(CustomRectItem *item);

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

    QMap<QString, QVariant> getChangedProperties() const;

private:
    bool hasUniformValue(QObject *obj, const char *propertyName) const;
    void setWidgetData(QWidget *widget, const QString &propertyName);
    void setIntWidgetLimit(QWidget *widget);

    void saveInitialUIState(); // Сохраняем начальное состояние UI
    QVariant getUIPropertyValue(const QString &propertyName) const; // Получаем текущее значение из UI

    Ui::ControlPropertysDlg *ui;

    CustomRectItem *m_pItem;
    EnumListModel *m_pFieldType, *m_DataType, *m_pStyle;

    QMap<QString, QVariant> m_initialUIState;
};

#endif // CONTROLPROPERTYSDLG_H
