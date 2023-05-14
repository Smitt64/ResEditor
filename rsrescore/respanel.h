#ifndef RESPANEL_H
#define RESPANEL_H

#include <QObject>
#include <QSize>
#include <QRect>
#include <QPoint>
#include "styles/resstyle.h"

#define TYPEF 0x0F
#define DUMMF 0x10
#define HIDEF 0x20 // Поле спрятано пользователем

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

    ResStyle::BorderStyle panelBorder() const;
    ResStyle::PanelStyle panelStyle() const;

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
    bool isCentered() const;
    bool isRightText() const;
    bool isExcludeNavigation() const;
    bool isExcludeAutoNum() const;
    bool isExcludeShadowNum() const;

    void setPanelRect(const QRect &geometry);

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
