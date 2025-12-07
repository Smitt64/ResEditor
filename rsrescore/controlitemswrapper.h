#ifndef CONTROLITEMSWRAPPER_H
#define CONTROLITEMSWRAPPER_H

#include <QObject>
#include <QVector>
#include <QUndoStack>
#include "controlitem.h"
#include "propertymodel.h"
#include "controtaborder.h"

class ControlItemsWrapper : public CustomRectItem
{
    Q_OBJECT
    // Все свойства ControlItem
    Q_PROPERTY(ControlItem::FieldType fieldType READ fieldType WRITE setFieldType NOTIFY fieldTypeChanged)
    Q_PROPERTY(ControlItem::DataType dataType READ dataType WRITE setDataType NOTIFY dataTypeChanged)
    Q_PROPERTY(quint16 dataLength READ dataLength WRITE setDataLength NOTIFY dataLengthChanged)
    Q_PROPERTY(quint16 length READ length NOTIFY lengthChanged)
    Q_PROPERTY(quint16 lines READ lines NOTIFY linesChanged)
    Q_PROPERTY(quint16 signs READ signs WRITE setSigns NOTIFY signsChanged)
    Q_PROPERTY(bool fdm READ fdm WRITE setFdm NOTIFY fdmChanged)
    Q_PROPERTY(bool isText READ isText WRITE setIsText NOTIFY isTextChanged)
    Q_PROPERTY(QString controlName READ controlName WRITE setControlName NOTIFY controlNameChanged)
    Q_PROPERTY(QString controlName2 READ controlName2 WRITE setControlName2 NOTIFY controlName2Changed)
    Q_PROPERTY(QString valueTemplate READ valueTemplate WRITE setValueTemplate NOTIFY valueTemplateChanged)
    Q_PROPERTY(QString toolTip READ toolTip WRITE setToolTip NOTIFY toolTipChanged)
    Q_PROPERTY(ResStyle::PanelStyle controlStyle READ controlStyle WRITE setControlStyle NOTIFY controlStyleChanged)
    Q_PROPERTY(quint16 controlGroup READ controlGroup WRITE setControlGroup NOTIFY controlGroupChanged)
    Q_PROPERTY(quint16 helpPage READ helpPage WRITE setHelpPage NOTIFY helpPageChanged)
    Q_PROPERTY(ControlItem::ControlFlags controlFlags READ controlFlags WRITE setControlFlags NOTIFY controlFlagsChanged)
    Q_PROPERTY(ControTabOrder tabOrder READ tabOrder WRITE setTabOrder NOTIFY tabOrderChanged)
    Q_PROPERTY(bool noTabStop READ noTabStop WRITE setNoTabStop NOTIFY tabNoTabStopChanged)
    Q_PROPERTY(bool listSelect READ listSelect WRITE setListSelect NOTIFY listSelectChanged)

    Q_CLASSINFO(CLASSINFO_UNDOREDO, "CONTROL")
    Q_CLASSINFO(CLASSINFO_PROPERTYLIST, ":/json/ControlMulty.json")
    Q_CLASSINFO(CLASSINFO_PROPERTYGROUP, "Control")
    Q_CLASSINFO(CLASSINFO_UNIFORMVALUEFUNC, "hasUniformValue")

public:
    explicit ControlItemsWrapper(QObject *parent = nullptr);
    virtual ~ControlItemsWrapper();

    // Добавление объектов ControlItem
    void addControlItem(ControlItem *item);
    void addControlItems(const QVector<ControlItem*> &items);
    void clearControlItems();

    QVariant userAction(const qint32 &action, const QVariant &param = QVariant());

    // Получение списка объектов
    const QVector<ControlItem*> &controlItems() const;

    // Q_INVOKABLE функция для проверки одинаковых значений
    Q_INVOKABLE bool hasUniformValue(const char *propertyName) const;

    // Свойства с учетом алиасов
    ControlItem::FieldType fieldType() const;
    ControlItem::DataType dataType() const;
    quint16 dataLength() const;
    quint16 length() const;
    quint16 lines() const;
    quint16 signs() const;
    bool fdm() const;
    bool isText() const;
    QString controlName() const;
    QString controlName2() const;
    QString valueTemplate() const;
    QString toolTip() const;
    ResStyle::PanelStyle controlStyle() const;
    quint16 controlGroup() const;
    quint16 helpPage() const;
    ControlItem::ControlFlags controlFlags() const;
    ControTabOrder tabOrder() const;
    bool noTabStop() const;
    bool listSelect() const;

    void emitAll();

public slots:
    // Слоты для установки значений
    void setFieldType(ControlItem::FieldType val);
    void setDataType(ControlItem::DataType val);
    void setDataLength(quint16 val);
    void setSigns(quint16 val);
    void setFdm(bool val);
    void setIsText(bool val);
    void setControlName(const QString &val);
    void setControlName2(const QString &val);
    void setValueTemplate(const QString &val);
    void setToolTip(const QString &val);
    void setControlStyle(ResStyle::PanelStyle val);
    void setControlGroup(quint16 val);
    void setHelpPage(quint16 val);
    void setControlFlags(ControlItem::ControlFlags val);
    void setTabOrder(ControTabOrder val);
    void setNoTabStop(const bool &val);
    void setListSelect(const bool &val);

signals:
    // Сигналы для всех свойств
    void fieldTypeChanged();
    void dataTypeChanged();
    void dataLengthChanged();
    void lengthChanged();
    void linesChanged();
    void signsChanged();
    void fdmChanged();
    void isTextChanged();
    void controlNameChanged();
    void controlName2Changed();
    void valueTemplateChanged();
    void toolTipChanged();
    void controlStyleChanged();
    void controlGroupChanged();
    void helpPageChanged();
    void controlFlagsChanged();
    void tabOrderChanged();
    void tabNoTabStopChanged();
    void listSelectChanged();

private:
    void connectControlItemSignals(ControlItem *item);
    void disconnectControlItemSignals(ControlItem *item);

    QVector<ControlItem*> m_controlItems;
    PropertyModel *m_propertyModel;
    bool m_inMacro;
};

#endif // CONTROLITEMSWRAPPER_H
