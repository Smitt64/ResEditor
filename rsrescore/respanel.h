#ifndef RESPANEL_H
#define RESPANEL_H

#include <QObject>
#include <QSize>
#include <QRect>
#include <QPoint>

#define TYPEF 0x0F
#define DUMMF 0x10
#define HIDEF 0x20 // Поле спрятано пользователем

// Константы для FIELD::FFlags
/*#define RF_REQUIRED     0x00000004
#define RF_NEGATIVE     0x00000008

#define RF_ELIPSBTN     0x00001000  // Поле с кнопкой "..." (вызов справочника)
#define RF_DOWNBTN      0x00002000  // Поле с кнопкой "" (выбор из списка)
#define RF_EMPTY        0x00004000
#define RF_TRIMZEROS    0x00008000

#define RF_3STATE       0x00000100  // Признак CheckBox'а с тремя состояниями (совместно с RF_CHECKBOX)

#define RF_GROUP        0x00010000
#define RF_CHECKBOX     0x00020000
#define RF_RADIO        0x00040000
#define RF_PASSWORD     0x00080000

#define RF_PROGRESS     0x00100000
#define RF_TREEITEM     0x00200000
#define RF_DYNLEN       0x00400000
#define RF_INDIC        0x00800000

#define RF_ASTEXT       0x01000000
#define RF_NOEDGE       0x02000000
#define RF_BLOCKHIDE    0x04000000  // Запретить скрывать колонку в автоскроллингах
#define RF_FLAT         0x08000000

#define RF_GROUPING     0x10000000
#define RF_NOTABSTOP    0x20000000
#define RF_NOCHECK      0x40000000
#define RF_ANSI_STRING  0x80000000*/

#define isActF(t) (!((t)&DUMMF))
#define setDF(t) ((t) | DUMMF)
#define typeF(t) ((t) & TYPEF)

typedef struct TextStruct
{
    struct TextR *_text;
    QString value;

    const qint8 &x() const;
    const qint8 &y() const;
    const qint16 &style() const;
    qint16 len() const;
}TextStruct;
typedef QList<TextStruct> TextStructList;

typedef struct FieldStruct
{
    FieldStruct();
    FieldStruct(const FieldStruct &other);
    struct FieldR *_field;

    QString name, name2, formatStr, toolTip;
    const qint8 &x() const;
    const qint8 &y() const;
    //const qint16 &style() const;
    const qint8 &len() const;
    const qint8 &height() const;

    const qint8 &kl() const;
    const qint8 &kr() const;
    const qint8 &ku() const;
    const qint8 &kd() const;

    void reset();
}FieldStruct;
typedef QList<FieldStruct> FieldStructList;

class ResBuffer;
class QDomElement;
class ResPanel : public QObject/*, public RsResInterface*/
{
    Q_OBJECT
    friend class RsResCore;
public:
    ResPanel(QObject *parent = nullptr);
    virtual ~ResPanel();

    //int load(const QString &name, ResLib *res);
    virtual int load(ResBuffer *data);
    virtual int loadXmlNode(const QDomElement &reslib);
    //virtual int loadProc(ResLib *res) Q_DECL_FINAL;

    QPoint pos() const;
    QSize size() const;

    QString title() const;
    QString status() const;
    QString status2() const;
    QString name() const;
    quint32 helpPage() const;

    TextStructList::iterator textBegin();
    TextStructList::iterator textEnd();

    FieldStructList::iterator fieldBegin();
    FieldStructList::iterator fieldEnd();

    void setName(const QString &val);

    void beginAddField(const QString &name, const QString &name2 = QString());
    void setFieldDataType(const quint8 &FieldType, const quint8 &DataType, const quint16 &DataLength);
    void setLenHeight(const quint8 &len, const quint8 &height);
    void setFormatTooltip(const QString &formatStr, const QString &toolTip = QString());
    void setFieldStyle(const quint16 &St);
    void setFieldGroup(const quint16 &ControlGroup);
    void setFieldHelp(const quint16 &HelpPage);
    void endAddField();

    int borderCount() const;
    QRect borderRect(const int &index) const;
    qint16 borderStyle(const int &index) const;

protected:
    virtual int readItems(struct PanelR *pp, ResBuffer *data, bool readName2);
    virtual bool readString(ResBuffer *data, char **s, qint16 vfl, qint16 lens);

    QString m_Status, m_StatusRD, m_Title, m_Name;

    struct PanelR *m_pPanel;
    QList<struct BordR> m_BordR;
    TextStructList m_Texts;
    FieldStructList m_Fields;

    FieldStruct m_NewField;
};

#endif // RESPANEL_H
