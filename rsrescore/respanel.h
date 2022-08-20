#ifndef RESPANEL_H
#define RESPANEL_H

#include "rsrescore_global.h"
#include "RsResInterface.h"
#include <QObject>
#include <QSize>
#include <QPoint>

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
    FieldStruct() {}
    FieldStruct(const FieldStruct &other);
    struct FieldR *_field;

    QString name, name2, formatStr, toolTip;
    const qint8 &x() const;
    const qint8 &y() const;
    //const qint16 &style() const;
    const qint8 &len() const;
    const qint8 &height() const;
}FieldStruct;
typedef QList<FieldStruct> FieldStructList;

class ResLib;
class ResPanel : public QObject, public RsResInterface
{
    Q_OBJECT
public:
    ResPanel(QObject *parent = nullptr);
    virtual ~ResPanel();

    int load(const QString &name, ResLib *res);
    virtual int loadProc(ResLib *res) Q_DECL_FINAL;

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

private:
    int readItems(struct PanelR *pp, ResLib *res, bool readName2);
    QString m_Status, m_StatusRD, m_Title, m_Name;

    struct PanelR *m_pPanel;
    QList<struct BordR> m_BordR;
    TextStructList m_Texts;
    FieldStructList m_Fields;
};

#endif // RESPANEL_H
