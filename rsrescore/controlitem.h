#ifndef CONTROLITEM_H
#define CONTROLITEM_H

#include "customrectitem.h"
#include "styles/resstyle.h"
#include <QObject>

class ControlItem : public CustomRectItem
{
    Q_OBJECT
    Q_PROPERTY(ControlItem::FieldType fieldType READ fieldType WRITE setFieldType NOTIFY fieldTypeChanged)
    Q_PROPERTY(DataType dataType READ dataType WRITE setDataType NOTIFY dataTypeChanged)
    Q_PROPERTY(quint16 dataLength READ dataLength WRITE setDataLength NOTIFY dataLengthChanged)
    Q_PROPERTY(quint16 length READ length NOTIFY lengthChanged)
    Q_PROPERTY(quint16 lines READ lines NOTIFY linesChanged)
    Q_PROPERTY(bool fdm READ fdm WRITE setFdm NOTIFY fdmChanged)
    Q_PROPERTY(bool isText READ isText WRITE setIsText NOTIFY isTextChanged)
    Q_PROPERTY(QString controlName READ controlName WRITE setControlName NOTIFY controlNameChanged)
    Q_PROPERTY(QString valueTemplate READ valueTemplate WRITE setValueTemplate NOTIFY valueTemplateChanged)
    Q_PROPERTY(QString toolTip READ toolTip WRITE setToolTip NOTIFY toolTipChanged)
    Q_PROPERTY(ResStyle::PanelStyle controlStyle READ controlStyle WRITE setControlStyle NOTIFY controlStyleChanged)
    Q_PROPERTY(quint16 controlGroup READ controlGroup WRITE setControlGroup NOTIFY controlGroupChanged)
    Q_PROPERTY(quint16 helpPage READ helpPage WRITE setHelpPage NOTIFY helpPageChanged)

    Q_CLASSINFO(CLASSINFO_UNDOREDO, "CONTROL")
    Q_CLASSINFO(CLASSINFO_PROPERTYLIST, ":/json/Control.json")
    Q_CLASSINFO(CLASSINFO_PROPERTYGROUP, "Control")

    friend class TextItem;
public:
    enum FieldType
    {
        FET = 0x01, // Редактируемое поле
        FBT = 0x02, // Кнопка
        FBS = 0x03, // Кнопка с тенью
        FWR = 0x04, // Редактируемое поле многострочное
        FVT = 0x05, // Нередактируемое поле
        FSP = 0x06, // Subpanel
        FCL = 0x07, // Cluster (Check box or Radio button)
        FVW = 0x08, // Многострочное нередактируюемое
    };

    enum DataType
    {
        INT16 = 0,
        INT32 = 1,
        INT64 = 26,
        FLOAT = 2,
        FLOATG = 3,
        DOUBLE = 4,
        DOUBLEG = 5,
        LDOUBLE = 4,
        MONEY = 6,
        MONEYR = 16,
        LMONEY = 14,
        LMONEYR = 15,
        DECIMAL = 19,
        NUMERIC = 25,
        STRING = 7,
        SNR = 8,
        NUMSTR = 18,
        DATE = 9,
        TIME = 10,
        SHTM = 11,
        CHAR = 12,
        UCHAR = 13,
        PICTURE = 27,
    };
    Q_ENUM(FieldType)
    Q_ENUM(DataType)
    Q_INVOKABLE ControlItem(QGraphicsItem* parent = nullptr);
    virtual ~ControlItem();

    void setFieldStruct(struct FieldStruct *value);

    int lines() const;

    const ControlItem::FieldType &fieldType() const;
    void setFieldType(const ControlItem::FieldType &val);

    const DataType &dataType() const;
    void setDataType(const ControlItem::DataType &val);

    const quint16 &dataLength() const;
    void setDataLength(const quint16 &val);

    quint16 length() const;

    const bool &fdm() const;
    void setFdm(const bool &val);

    const bool &isText() const;
    void setIsText(const bool &val);

    const QString &controlName() const;
    void setControlName(const QString &val);

    const QString &toolTip() const;
    void setToolTip(const QString &val);

    const ResStyle::PanelStyle &controlStyle() const;
    void setControlStyle(const ResStyle::PanelStyle &val);

    const QString &valueTemplate() const;
    void setValueTemplate(const QString &val);

    const quint16 &controlGroup() const;
    void setControlGroup(const quint16 &val);

    const quint16 &helpPage() const;
    void setHelpPage(const quint16 &val);

    virtual QVariant userAction(const qint32 &action, const QVariant &param = QVariant()) Q_DECL_OVERRIDE;

signals:
    void fieldTypeChanged();
    void dataTypeChanged();
    void dataLengthChanged();
    void lengthChanged();
    void fdmChanged();
    void linesChanged();
    void isTextChanged();
    void controlNameChanged();
    void controlStyleChanged();
    void valueTemplateChanged();
    void controlGroupChanged();
    void helpPageChanged();
    void toolTipChanged();

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR) Q_DECL_OVERRIDE;
    virtual bool isIntersects(const QRectF &thisBound, QGraphicsItem *item, const QRectF &itemBound) const Q_DECL_OVERRIDE;

private:
    struct FieldStruct *m_pFieldStruct;
    void updateCorners();

    FieldType m_FieldType;
    DataType m_DataType;
    quint16 m_DataLength;
    bool m_Fdm, m_IsText;

    QString m_ControlName, m_ValueTemplate, m_ToolTip;
    ResStyle::PanelStyle m_Style;

    quint16 m_ControlGroup;
    quint16 m_HelpPage;
};

Q_DECLARE_METATYPE(ControlItem*)

#endif // CONTROLITEM_H
