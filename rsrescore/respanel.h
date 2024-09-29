#ifndef RESPANEL_H
#define RESPANEL_H

#include <QObject>
#include <QSize>
#include <QRect>
#include <QPoint>
#include "rscoreheader.h"
#include "styles/resstyle.h"

#define TYPEF 0x0F
#define DUMMF 0x10
#define HIDEF 0x20 // Поле спрятано пользователем

#define isActF(t) (!((t)&DUMMF))
#define setDF(t) ((t) | DUMMF)
#define typeF(t) ((t) & TYPEF)

class TextStruct
{
public:
    TextStruct() :
        _text(nullptr)
    {

    }

    TextStruct(const TextStruct &other);

    ~TextStruct()
    {
        if (_text)
            delete _text;
    }

    struct TextR *_text;
    QString value;

    const qint8 &x() const;
    const qint8 &y() const;
    const qint16 &style() const;
    qint16 len() const;

    TextStruct &operator =(const TextStruct &other);
    static int compare(const void *elem1, const void *elem2);
};
typedef QVector<TextStruct> TextStructList;

typedef struct FieldStruct
{
    FieldStruct();
    FieldStruct(const FieldStruct &other);
    //FieldStruct(const FieldStruct &&other);
    virtual ~FieldStruct();

    FieldStruct &operator =(const FieldStruct &other);

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

    bool isActiveFld() const;
    bool isTabStopFld() const;

    void reset();

    //static bool compare(const FieldStruct &s1, const FieldStruct &s2);
    static int compare(const void *s1, const void *s2);
}FieldStruct;
typedef QVector<FieldStruct> FieldStructList;

class ResBuffer;
class QDomElement;
class ErrorsModel;
class ResPanel : public QObject/*, public RsResInterface*/
{
    Q_OBJECT
    friend class RsResCore;
public:
    ResPanel(QObject *parent = nullptr);
    virtual ~ResPanel();

    //int load(const QString &name, ResLib *res);
    virtual int load(ResBuffer *data);
    virtual int save(ResBuffer *data);
    virtual QString saveXml(const QString &encode = QString("UTF-8"));
    virtual int loadXmlNode(const QDomElement &reslib);
    //virtual int loadProc(ResLib *res) Q_DECL_FINAL;

    QPoint pos() const;
    QSize size() const;

    QRect scrol() const;
    void setScrol(const QRect &scrl);

    quint16 rowHaight() const;
    void setRowHeight(const quint16 &val);
    QPoint scrolPos() const;

    QString title() const;
    QString status() const;
    QString status2() const;
    quint16 helpPage() const;

    QString name() const;
    qint16 type() const;

    ResStyle::BorderStyle panelBorder() const;
    ResStyle::PanelStyle panelStyle() const;

    TextStructList::iterator textBegin();
    TextStructList::iterator textEnd();

    FieldStructList::iterator fieldBegin();
    FieldStructList::iterator fieldEnd();

    void setName(const QString &val);
    void setComment(const QString &val);
    void setType(const quint16 &type);

    void beginAddField(const QString &name, const QString &name2 = QString());
    void setFieldDataType(const quint8 &FieldType,
                          const quint8 &DataType,
                          const quint16 &DataLength,
                          const bool &fdm,
                          const bool &istext);
    void setFieldFlags(const quint32 &value);
    void setLenHeight(const quint8 &len, const quint8 &height);
    void setFieldPos(const quint8 &x, const quint8 &y);
    void setFormatTooltip(const QString &formatStr, const QString &toolTip = QString());
    void setFieldStyle(const quint16 &St);
    void setFieldGroup(const quint16 &ControlGroup);
    void setFieldHelp(const quint16 &HelpPage);
    void endAddField();

    int borderCount() const;
    QRect borderRect(const int &index) const;
    qint16 borderStyle(const int &index) const;
    void addBorder(const QRect &rect, const quint16 &St);
    void addText(const QString &value, const quint16 &x, const quint16 &y, const quint16 &St);

    bool isCentered() const;
    bool isRightText() const;
    bool isExcludeNavigation() const;
    bool isExcludeAutoNum() const;
    bool isExcludeShadowNum() const;

    qint32 scrolFlags() const;

    void setPanelRect(const QRect &geometry);
    void setPanelHelp(const quint16 &help);
    void setPanelExcludeFlags(const quint32 &val);
    void setScrolFlags(const quint32 &val);
    void setPanelCentered(const bool &val);
    void setPanelRightText(const bool &val);
    void setPanelStyle(const ResStyle::BorderStyle &border, const ResStyle::PanelStyle &style);
    void setPanelStrings(const QString &Title, const QString &Status, const QString &StatusRD);

    int checkResource(ErrorsModel *errors = nullptr);
    static QString GetCheckError(int stat);

protected:
    typedef std::tuple<long,long> SizesTuple;
    void def_panelsize(SizesTuple &sizes, int ver);
    int savePanel(ResBuffer *data);
    int saveStatusLine(ResBuffer *data);
    int saveTitleLine(ResBuffer *data);
    int saveTextLabels(ResBuffer *data);
    int saveBorders(ResBuffer *data);
    int saveFields(ResBuffer *data);
    void prepFields();
    virtual int readItems(struct PanelR *pp, ResBuffer *data, bool readName2);
    virtual bool readString(ResBuffer *data, char **s, qint16 vfl, qint16 lens);

    int checkPanel(ErrorsModel *errors = nullptr);
    int checkScrol(ErrorsModel *errors = nullptr);

private:
    bool __CheckElement(int x, int y, int h, int l, int x1, int y1, int x2, int y2, int border);
    bool __CheckCrossRect(int r1_x, int r1_y, int r1_h, int r1_l, int r2_x, int r2_y, int r2_h, int r2_l);
    bool __CheckEntryRect(int r1_x, int r1_y, int r1_h, int r1_l, int r2_x, int r2_y, int r2_h, int r2_l);
    bool __CheckCrossField(FieldR *f1, FieldR *f2);
    bool __CheckCrossFields(int curr);
    int checkScrolRect(int sx, int sy, int sh, int sl, ErrorsModel *errors);

    QString m_Status, m_StatusRD, m_Title, m_Name, m_Comment;
    qint16 m_Type;

    QDateTime m_ResTime;

    struct PanelR *m_pPanel;
    QVector<struct BordR> m_BordR;
    TextStructList m_Texts;
    FieldStructList m_Fields;

    FieldStruct m_NewField;
};

#endif // RESPANEL_H
