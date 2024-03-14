#include "respanel.h"
#include "resbuffer.h"
#include "rscoreheader.h"
#include <stdlib.h>
#include <QDebug>
#include <lbrobject.h>
#include <QDataStream>
#include <QDomElement>
#include <QDomDocument>
#include <QFile>

#define  RFP_REJREQ        0x00000004
#define  RFP_UPDREQ        0x00000008
#define  RFP_DELREQ        0x00000010

#define  RFP_CHNGF         0x00000020   // ширина поля скроллинга была изменена
                                        // динамически в EasyWin

#define  RFP_NOCLOSE       0x00000040   // для EasyWin

#define  RFP_RIGHTTEXT     0x00000080   // все тексты в панели имеют правое выравнивание
#define  RFP_TRANSPARENT   0x00000100   // Не используется (см. RFP_CENTERED)
#define  RFP_ANIMATED      0x00000200   // Не используется
#define  RFP_FLOATING      0x00000400
#define  RFP_PINX          0x00000800
#define  RFP_PINY          0x00001000
#define  RFP_NOAUTODIR     0x00002000
#define  RFP_NOAUTONUM     0x00004000
#define  RFP_NOSHADOW      0x00008000

#define  RFP_CENTERED      0x00000100   // Вместо RFP_TRANSPARENT

#define RF_ASTEXT       0x01000000
#define RF_NOTABSTOP    0x20000000

#define RFP_AUTOFILL      0x00010000   // расширять панель и обл. скролинга до экрана
#define RFP_AUTOFIELDS    0x00020000   // автоматически устанавливать кординать полей в скролинге
#define RFP_AUTOHEAD      0x00040000   // генерировать заголовки колонок в скролинге
#define RFP_REVERSORDER   0x00080000   // обратный порядок записей
#define RFP_DENYSORT      0x01000000   // запретить сортировку скролинга


#define  dGET_BORDER(p)     ((p)->St >> 8 ? 1 : 0)

typedef struct
{
    r_coord y;
    r_coord x;
    int num;
}FieldSortData;

TextStruct::TextStruct(const TextStruct &other):
    _text(nullptr)
{
    value = other.value;

    if (other._text)
    {
        _text = new TextR();
        memcpy(_text, other._text, sizeof(TextR));
    }
}

const qint8 &TextStruct::x() const
{
    return _text->x;
}

const qint8 &TextStruct::y() const
{
    return _text->y;
}

const qint16 &TextStruct::style() const
{
    return _text->St;
}

qint16 TextStruct::len() const
{
    return value.length();
}

// ----------------------------------------------------------------

bool FieldStruct::compare(const FieldStruct &s1, const FieldStruct &s2)
{
    int kv = s1.y() - s2.y();

    if (!kv)
        return s1.x() - s2.x();

    return kv < 0;
}

FieldStruct::FieldStruct(const FieldStruct &other)
{
    _field = new FieldR();
    memcpy(_field, other._field, sizeof(FieldR));

    name = other.name;
    name2 = other.name2;
    formatStr = other.formatStr;
    toolTip = other.toolTip;
}

/*FieldStruct::FieldStruct(const FieldStruct &&other)
{
    _field = std::move(other._field);

    name = other.name;
    name2 = other.name2;
    formatStr = other.formatStr;
    toolTip = other.toolTip;
}*/

FieldStruct::FieldStruct()
{
    _field = nullptr;
}

FieldStruct::~FieldStruct()
{
    if (_field)
    {
        delete _field;
        _field = nullptr;
    }
}

FieldStruct &FieldStruct::operator =(const FieldStruct &other)
{
    if (_field)
    {
        delete _field;
        _field = nullptr;
    }

    _field = new FieldR();
    memcpy(_field, other._field, sizeof(FieldR));

    name = other.name;
    name2 = other.name2;
    formatStr = other.formatStr;
    toolTip = other.toolTip;

    return *this;
}

const qint8 &FieldStruct::x() const
{
    return _field->x;
}

const qint8 &FieldStruct::y() const
{
    return _field->y;
}

const qint8 &FieldStruct::len() const
{
    return _field->l;
}

const qint8 &FieldStruct::height() const
{
    return _field->h;
}

const qint8 &FieldStruct::kl() const
{
    return _field->kl;
}

const qint8 &FieldStruct::kr() const
{
    return _field->kr;
}

const qint8 &FieldStruct::ku() const
{
    return _field->ku;
}

const qint8 &FieldStruct::kd() const
{
    return _field->kd;
}

bool FieldStruct::isActiveFld() const
{
    return !(_field->Ftype & DUMMF) &&
           !(_field->Ftype & HIDEF) &&
           !(_field->flags & RF_ASTEXT);
}

bool FieldStruct::isTabStopFld() const
{
    return !(_field->flags & RF_NOTABSTOP);
}

void FieldStruct::reset()
{
    if (_field)
        delete _field;

    _field = new FieldR();
    memset(_field, 0, sizeof(FieldR));

    name = QString();
    name2 = QString();
    formatStr = QString();
    toolTip = QString();
}

// ----------------------------------------------------------------

ResPanel::ResPanel(QObject *parent) :
    QObject(parent),
    m_pPanel(nullptr)
{
    m_pPanel = new PanelR();
    memset(m_pPanel, 0, sizeof(PanelR));
}

ResPanel::~ResPanel()
{
    if (m_pPanel)
        delete m_pPanel;

    for (TextStruct &textitem : m_Texts)
    {
        if (textitem._text)
            delete textitem._text;

        textitem._text = nullptr;
    }

    for (FieldStruct fielditem : m_Fields)
    {
        if (fielditem._field)
            delete fielditem._field;

        fielditem._field = nullptr;
    }

    m_Texts.clear();
    m_Fields.clear();
}

/*static int rds(int hd, char **s, r_coord vfl, r_coord lens, HRSLCVT hcvtRd)
{
    int  err = 0;


    if(vfl)
        err = (lseek(hd, (long)lens, SEEK_CUR) == -1L);
    else if(lens)
    {
        err = read(hd, *s, lens);

        if(!(err = (err != lens)))
            RslCvtString(hcvtRd, *s);
    }

    return err;
}*/

/*int ResPanel::loadProc(ResLib *res)
{
    int err = 0;
    int ver = res->resHeaderVersion();
    bool readName2 = false;

    if(ver >= 1)
    {
        r_coord  lens;
        err = res->readBytes(&lens, sizeof(lens));

        if(!(err = (err != sizeof(lens))))
        {
            if(lens)
                err = rds(res->fileHandle(), NULL, 1, lens, nullptr);
        }
    }

    int toRead = 0;
    if(ver == 0)
        toRead = sizeof(PanelR_0);
    else if(ver == 1)
        toRead = sizeof(PanelR_1);
    else
        toRead = sizeof(PanelR);

    int readed = res->readBytes(m_pPanel, toRead);
    if (readed == toRead)
    {
        if(m_pPanel->flags & P_NAME2ALLOCED)
        {
            readName2 = true;
            m_pPanel->flags &= ~P_NAME2ALLOCED;
        }

        readItems(m_pPanel, res, readName2);
    }

    return err;
}*/

/*int ResPanel::readItems(struct PanelR *pp, ResLib *res, bool readName2)
{
    int err = 0;
    int ver = res->resHeaderVersion();

    r_coord  lens;
    err = res->readBytes(&lens, sizeof(lens));
    if(!(err = (err != sizeof(lens))))
    {
        if(lens)
        {
            char *s = (char*)malloc(sizeof(char)*(lens + 1));
            memset(s, 0, lens + 1);

            err = rds(res->fileHandle(), (char**)&s, 0, lens, nullptr);
            m_Status = res->decodeString(s);
            free(s);
        }
    }

    if(ver >= 1)
    {
        err = res->readBytes(&lens, sizeof(lens));
        if(!(err = (err != sizeof(lens))))
        {
            if(lens)
            {
                char *s = (char*)malloc(sizeof(char)*(lens + 1));
                memset(s, 0, lens + 1);

                err = rds(res->fileHandle(), &s, 0, lens, nullptr);
                m_StatusRD = res->decodeString(s);
                free(s);
            }
        }
    }

    err = res->readBytes(&lens, sizeof(lens));
    if(!(err = (err != sizeof(lens))))
    {
        if(lens)
        {
            char *s = (char*)malloc(sizeof(char)*(lens + 1));
            memset(s, 0, lens + 1);

            err = rds(res->fileHandle(), &s, 0, lens, nullptr);
            m_Title = res->decodeString(s);
            free(s);
        }
    }

    for(int i = 0; i < pp->Nb && !err; i++)
    {
        BordR  bb;
        err = res->readBytes(&bb, sizeof(BordR));

        if(!(err = (err != sizeof(BordR))))
        {
            RslCvtUShort(hcvtRd, &bb.St);
            m_BordR.append(bb);
            //InsertB(p, bb.St, bb.x, bb.y, bb.l, bb.h, bb.fl);
        }
    }

    for(int i = 0; i < pp->Nt && !err; i++)
    {
        TextR  tt;
        err = res->readBytes(&tt, sizeof(TextR));

        if(!(err = (err != sizeof(TextR))) && tt.lens)
        {
            RslCvtUShort(hcvtRd, &tt.St);

            char *s = (char*)malloc(sizeof(char)*(tt.lens + 1));
            memset(s, 0, tt.lens + 1);
            rds(res->fileHandle(), &s, tt.vfl, tt.lens, nullptr);

            TextStruct text;
            text._text = new TextR();
            memcpy(text._text, &tt, sizeof(TextR));
            text.value = res->decodeString(s);
            m_Texts.append(text);
            free(s);
        }
    }

    for(int i = 0; i < pp->Pnumf && !err; i++)
    {
        FieldStruct element;
        element._field = new FieldR();
        memset(element._field, 0, sizeof(FieldR));

        int toRead;
        if(ver == 0)
            toRead = sizeof(FieldR_0);
        else if(ver == 1)
            toRead = sizeof(FieldR_1);
        else
            toRead = sizeof(FieldR);

        element._field->flags = 0;
        element._field->group = 0;
        element._field->nameLen = element._field->formLen = 0;

        err = res->readBytes(element._field, toRead);

        if(!(err = (err != toRead)))
        {
            if(element._field->FVt == FT_LDOUBLE10)
                element._field->FVt = FT_DOUBLE;

            if(element._field->FVt == FT_NUMERIC)
               element._field->FVp &= 0xFF;

            char *s = (char*)malloc(sizeof(char)*(element._field->lens + 1));
            memset(s, 0, element._field->lens + 1);
            if(readName2 || !element._field->vfl)
            {
                err = rds(res->fileHandle(), &s, 0, element._field->lens, nullptr);
                element.name2 = res->decodeString(s);
            }
            else
                err = rds(res->fileHandle(), &s, element._field->vfl, element._field->lens, nullptr);
            free(s);

            if(ver >= 1)
            {
                if(element._field->nameLen)
                {
                    //f->name = s;
                    s = (char*)malloc(sizeof(char)*(element._field->nameLen + 1));
                    memset(s, 0, element._field->nameLen + 1);

                    err = rds(res->fileHandle(), &s, 0, element._field->nameLen, nullptr);
                    element.name = res->decodeString(s);
                    free(s);
                }

                if(element._field->formLen)
                {
                    //f->formatStr = s;
                    s = (char*)malloc(sizeof(char)*(element._field->formLen + 1));
                    memset(s, 0, element._field->formLen + 1);

                    err = rds(res->fileHandle(), &s, 0, element._field->formLen, nullptr);
                    element.formatStr = res->decodeString(s);
                    free(s);
                }

                if(ver >= 2)
                {
                    if(element._field->tooltipLen)
                    {
                        s = (char*)malloc(sizeof(char)*(element._field->tooltipLen + 1));
                        memset(s, 0, element._field->tooltipLen + 1);

                        err = rds(res->fileHandle(), &s, 0, element._field->tooltipLen, nullptr);
                        element.toolTip = res->decodeString(s);
                        free(s);
                    }
                }
            }
        }

        m_Fields.append(element);
    }

    return err;
}*/

/*int ResPanel::load(const QString &name, ResLib *res)
{
    m_Name = name;
    return res->loadResource(name, RES_PANEL, this);
}*/

bool ResPanel::readString(ResBuffer *data, char **s, qint16 vfl, qint16 lens)
{
    qint64 pos = data->pos() + lens;

    if (vfl)
    {
        if (!data->seek(pos))
            return false;
    }
    else if (lens)
    {
        char *tmp = nullptr;
        if (*s)
            tmp = *s;
        else
            tmp = (char*)malloc(lens + 1);

        if (data->read(tmp, lens) != lens)
            return false;
    }

    return true;
}

int ResPanel::load(ResBuffer *data)
{
    int err = 0;
    int ver = data->headerVersion();
    bool readName2 = false;
    m_Name = data->name();
    m_Type = data->type();
    m_ResTime = data->getResDateTime();

    if (ver >= 1)
    {
        r_coord lens = 0;
        err = data->read((char*)&lens, sizeof(lens));

        if(!(err = (err != sizeof(lens))))
        {
            if(lens)
                readString(data, NULL, 1, lens);
        }
    }

    int toRead = 0;
    if(ver == 0)
        toRead = sizeof(PanelR_0);
    else if(ver == 1)
        toRead = sizeof(PanelR_1);
    else
        toRead = sizeof(PanelR);

    int readed = data->read((char*)m_pPanel, toRead);
    if (readed == toRead)
    {
        if(m_pPanel->flags & P_NAME2ALLOCED)
        {
            readName2 = true;
            m_pPanel->flags &= ~P_NAME2ALLOCED;
        }
    }

    readItems(m_pPanel, data, readName2);

    return 0;
}

int ResPanel::readItems(struct PanelR *pp, ResBuffer *data, bool readName2)
{
    int err = 0;
    int ver = data->headerVersion();

    r_coord  lens;
    err = data->read((char*)&lens, sizeof(lens));
    if(!(err = (err != sizeof(lens))))
    {
        if(lens)
        {
            char *s = (char*)malloc(sizeof(char)*(lens + 1));
            memset(s, 0, lens + 1);

            err = !readString(data, (char**)&s, 0, lens);
            m_Status = data->decodeString(s);
            free(s);
        }
    }

    if(ver >= 1)
    {
        err = data->read((char*)&lens, sizeof(lens));
        if(!(err = (err != sizeof(lens))))
        {
            if(lens)
            {
                char *s = (char*)malloc(sizeof(char)*(lens + 1));
                memset(s, 0, lens + 1);

                err = !readString(data, &s, 0, lens);
                m_StatusRD = data->decodeString(s);
                free(s);
            }
        }
    }

    err = data->read((char*)&lens, sizeof(lens));
    if(!(err = (err != sizeof(lens))))
    {
        if(lens)
        {
            char *s = (char*)malloc(sizeof(char)*(lens + 1));
            memset(s, 0, lens + 1);

            err = !readString(data, &s, 0, lens);
            m_Title = data->decodeString(s);
            qDebug() << m_Title;
            free(s);
        }
    }

    for(int i = 0; i < pp->Nb && !err; i++)
    {
        BordR  bb;
        err = data->read((char*)&bb, sizeof(BordR));

        if(!(err = (err != sizeof(BordR))))
        {
            //RslCvtUShort(hcvtRd, &bb.St);
            m_BordR.append(bb);
            //InsertB(p, bb.St, bb.x, bb.y, bb.l, bb.h, bb.fl);
        }
    }

    for(int i = 0; i < pp->Nt && !err; i++)
    {
        TextR tt;
        err = data->read((char*)&tt, sizeof(TextR));

        if(!(err = (err != sizeof(TextR))) && tt.lens)
        {
            char *s = (char*)malloc(sizeof(char)*(tt.lens + 1));
            memset(s, 0, tt.lens + 1);
            readString(data, &s, tt.vfl, tt.lens);

            TextStruct text;
            text._text = new TextR();
            memcpy(text._text, &tt, sizeof(TextR));
            text.value = data->decodeString(s);
            m_Texts.append(text);
            free(s);
        }
    }

    for(int i = 0; i < pp->Pnumf && !err; i++)
    {
        FieldStruct element;
        element._field = new FieldR();
        memset(element._field, 0, sizeof(FieldR));

        int toRead;
        if(ver == 0)
            toRead = sizeof(FieldR_0);
        else if(ver == 1)
            toRead = sizeof(FieldR_1);
        else
            toRead = sizeof(FieldR);
        element._field->flags = 0;
        element._field->group = 0;
        element._field->nameLen = element._field->formLen = 0;

        err = data->read((char*)element._field, toRead);

        if(!(err = (err != toRead)))
        {
            if(element._field->FVt == FT_LDOUBLE10)
                element._field->FVt = FT_DOUBLE;

            if(element._field->FVt == FT_NUMERIC)
               element._field->FVp &= 0xFF;

            if (data->type() != LbrObject::RES_PANEL)
            {
                element._field->x += pp->x;
                element._field->y += pp->y;
            }

            char *s = (char*)malloc(sizeof(char)*(element._field->lens + 1));
            memset(s, 0, element._field->lens + 1);
            if(readName2 || !element._field->vfl)
            {
                err = !readString(data, &s, 0, element._field->lens);
                element.name2 = data->decodeString(s);
            }
            else
                err = !readString(data, &s, element._field->vfl, element._field->lens);
            free(s);

            if(ver >= 1)
            {
                if(element._field->nameLen)
                {
                    s = (char*)malloc(sizeof(char)*(element._field->nameLen + 1));
                    memset(s, 0, element._field->nameLen + 1);

                    err = !readString(data, &s, 0, element._field->nameLen);
                    element.name = data->decodeString(s);
                    free(s);
                }

                if(element._field->formLen)
                {
                    s = (char*)malloc(sizeof(char)*(element._field->formLen + 1));
                    memset(s, 0, element._field->formLen + 1);

                    err = !readString(data, &s, 0, element._field->formLen);
                    element.formatStr = data->decodeString(s);
                    free(s);
                }

                if(ver >= 2)
                {
                    if(element._field->tooltipLen)
                    {
                        s = (char*)malloc(sizeof(char)*(element._field->tooltipLen + 1));
                        memset(s, 0, element._field->tooltipLen + 1);

                        err = !readString(data, &s, 0, element._field->tooltipLen);
                        element.toolTip = data->decodeString(s);
                        free(s);
                    }
                }
            }
        }

        m_Fields.append(element);
    }

    return err;
}

QPoint ResPanel::pos() const
{
    return QPoint(m_pPanel->x1, m_pPanel->y1);
}

QSize ResPanel::size() const
{
    return QSize(m_pPanel->x2 - m_pPanel->x1 + 1, m_pPanel->y2 - m_pPanel->y1 + 1);
}

QRect ResPanel::scrol() const
{
    return QRect(m_pPanel->x, m_pPanel->y,
                 m_pPanel->l, m_pPanel->Mn);
}

void ResPanel::setScrol(const QRect &scrl)
{
    m_pPanel->x = scrl.x();
    m_pPanel->y = scrl.y();

    m_pPanel->l = scrl.width();
    m_pPanel->Mn = scrl.height();
}

quint16 ResPanel::rowHaight() const
{
    return m_pPanel->h;
}

void ResPanel::setRowHeight(const quint16 &val)
{
    m_pPanel->h = val;
}

QPoint ResPanel::scrolPos() const
{
    return QPoint(m_pPanel->x, m_pPanel->y);
}

QString ResPanel::title() const
{
    return m_Title;
}

QString ResPanel::status() const
{
    return m_Status;
}

QString ResPanel::status2() const
{
    return m_StatusRD;
}

QString ResPanel::name() const
{
    return m_Name;
}

qint16 ResPanel::type() const
{
    return m_Type;
}

quint16 ResPanel::helpPage() const
{
    return m_pPanel->PHelp;
}

ResStyle::BorderStyle ResPanel::panelBorder() const
{
    quint16 border = m_pPanel->St >> 8;
    return (ResStyle::BorderStyle)border;
}

ResStyle::PanelStyle ResPanel::panelStyle() const
{
    quint16 style = m_pPanel->St & 0x00ff;
    return (ResStyle::PanelStyle)style;
}

TextStructList::iterator ResPanel::textBegin()
{
    return m_Texts.begin();
}

TextStructList::iterator ResPanel::textEnd()
{
    return m_Texts.end();
}

FieldStructList::iterator ResPanel::fieldBegin()
{
    return m_Fields.begin();
}

FieldStructList::iterator ResPanel::fieldEnd()
{
    return m_Fields.end();
}

int ResPanel::loadXmlNode(const QDomElement &reslib)
{
    m_Name = reslib.attribute("name", "0");
    m_pPanel->St = reslib.attribute("St", "0").toInt();
    m_pPanel->x1 = reslib.attribute("x1", "0").toInt();
    m_pPanel->x2 = reslib.attribute("x2", "0").toInt();
    m_pPanel->y1 = reslib.attribute("y1", "0").toInt();
    m_pPanel->y2 = reslib.attribute("y2", "0").toInt();

    m_pPanel->PHelp = reslib.attribute("PHelp", "0").toInt();
    m_pPanel->Pff = reslib.attribute("Pff", "0").toInt();
    m_pPanel->flags = reslib.attribute("flags", "0").toInt();

    if (reslib.tagName() != "panel")
    {
        m_pPanel->x = reslib.attribute("x", "0").toInt();
        m_pPanel->y = reslib.attribute("y", "0").toInt();

        m_pPanel->l = reslib.attribute("l", "0").toInt();
        m_pPanel->h = reslib.attribute("h", "0").toInt();
        m_pPanel->Mn = reslib.attribute("Mn", "0").toInt();
    }

    QDomNode n = reslib.firstChild();
    while(!n.isNull())
    {
        QDomElement e = n.toElement();

        if (e.tagName() == "stline")
            m_Status = e.text();
        else if (e.tagName() == "headLine")
            m_Title = e.text();
        else if (e.tagName() == "text")
        {
            TextStruct text;
            text._text = new TextR();
            text._text->St = e.attribute("St", "0").toInt();
            text._text->x = e.attribute("x", "0").toInt();
            text._text->y = e.attribute("y", "0").toInt();
            text.value = e.text();
            m_Texts.append(text);
        }
        else if (e.tagName() == "field")
        {
            FieldStruct element;
            element._field = new FieldR();
            element._field->Ftype = e.attribute("Ftype", "0").toInt();
            element._field->St = e.attribute("St", "0").toInt();
            element._field->FVt = e.attribute("FVt", "0").toInt();
            element._field->FVp = e.attribute("FVp", "0").toInt();
            element._field->x = e.attribute("x", "0").toInt();
            element._field->y = e.attribute("y", "0").toInt();
            element._field->l = e.attribute("l", "0").toInt();
            element._field->h = e.attribute("h", "0").toInt();
            element._field->kl = e.attribute("kl", "0").toInt();
            element._field->kr = e.attribute("kr", "0").toInt();
            element._field->ku = e.attribute("ku", "0").toInt();
            element._field->kd = e.attribute("kd", "0").toInt();
            element._field->FHelp = e.attribute("FHelp", "0").toInt();
            element._field->vfl = e.attribute("vfl", "0").toInt();
            element._field->flags = e.attribute("flags", "0").toInt();
            element._field->group = e.attribute("group", "0").toInt();

            m_Fields.append(element);
        }
        n = n.nextSibling();
    }
    return 0;
}

void ResPanel::setName(const QString &val)
{
    m_Name = val;
}

void ResPanel::setComment(const QString &val)
{
    m_Comment = val;
}

void ResPanel::setType(const quint16 &type)
{
    m_Type = type;
}

void ResPanel::beginAddField(const QString &name, const QString &name2)
{
    m_NewField.reset();
    m_NewField.name  = name;
    m_NewField.name2 = name2;
}

void ResPanel::setFieldDataType(const quint8 &FieldType,
                                const quint8 &DataType,
                                const quint16 &DataLength,
                                const bool &fdm)
{
    m_NewField._field->Ftype = FieldType;
    m_NewField._field->FVt = DataType;
    m_NewField._field->FVp = DataLength;

    if (fdm)
        m_NewField._field->Ftype |= DUMMF;
}

void ResPanel::setFieldFlags(const quint32 &value)
{
    m_NewField._field->flags = value;
}

void ResPanel::setLenHeight(const quint8 &len, const quint8 &height)
{
    m_NewField._field->l = len;
    m_NewField._field->h = height;
}

void ResPanel::setFieldPos(const quint8 &x, const quint8 &y)
{
    m_NewField._field->x = x;
    m_NewField._field->y = y;
}

void ResPanel::setFormatTooltip(const QString &formatStr, const QString &toolTip)
{
    m_NewField.formatStr = formatStr;
    m_NewField.toolTip = toolTip;
}

void ResPanel::setFieldStyle(const quint16 &St)
{
    qint16 st = static_cast<qint16>(St) + 1;
    m_NewField._field->St = st << 8;
}

void ResPanel::setFieldGroup(const quint16 &ControlGroup)
{
    m_NewField._field->group = ControlGroup;
}

void ResPanel::setFieldHelp(const quint16 &HelpPage)
{
    m_NewField._field->FHelp = HelpPage;
}

void ResPanel::endAddField()
{
    m_Fields.append(m_NewField);
    m_pPanel->Pnumf = m_Fields.size();
}

int ResPanel::borderCount() const
{
    return m_BordR.size();
}

QRect ResPanel::borderRect(const int &index) const
{
    const BordR &border = m_BordR[index];
    return QRect(border.x, border.y, border.l, border.h);
}

qint16 ResPanel::borderStyle(const int &index) const
{
    const BordR &border = m_BordR[index];
    return border.fl;
}

bool ResPanel::isCentered() const
{
    return m_pPanel->flags & RFP_CENTERED;
}

bool ResPanel::isRightText() const
{
    return m_pPanel->flags & RFP_RIGHTTEXT;
}

bool ResPanel::isExcludeNavigation() const
{
    return m_pPanel->flags & RFP_NOAUTODIR;
}

bool ResPanel::isExcludeAutoNum() const
{
    return m_pPanel->flags & RFP_NOAUTONUM;
}

bool ResPanel::isExcludeShadowNum() const
{
    return m_pPanel->flags & RFP_NOSHADOW;
}

#define CHECK_AND_SET_FLAGS(fl) if (m_pPanel->flags & fl) flags |= fl
qint32 ResPanel::scrolFlags() const
{
    qint32 flags = 0;

    CHECK_AND_SET_FLAGS(RFP_AUTOFILL);
    CHECK_AND_SET_FLAGS(RFP_AUTOFIELDS);
    CHECK_AND_SET_FLAGS(RFP_AUTOHEAD);
    CHECK_AND_SET_FLAGS(RFP_REVERSORDER);
    CHECK_AND_SET_FLAGS(RFP_DENYSORT);

    return flags;
}

void ResPanel::setPanelRect(const QRect &geometry)
{
    m_pPanel->x1 = geometry.x();
    m_pPanel->y1 = geometry.y();
    m_pPanel->x2 = geometry.x() + geometry.width() - 1;
    m_pPanel->y2 = geometry.y() + geometry.height() - 1;
}

void ResPanel::setPanelStrings(const QString &Title, const QString &Status, const QString &StatusRD)
{
    m_Status = Status;
    m_StatusRD = StatusRD;
    m_Title = Title;
}

void ResPanel::setPanelStyle(const ResStyle::BorderStyle &border, const ResStyle::PanelStyle &style)
{
    m_pPanel->St = (border << 8) | (style & 0x00ff);
}

void ResPanel::setPanelHelp(const quint16 &help)
{
    m_pPanel->PHelp = help;
}

void ResPanel::setPanelExcludeFlags(const quint32 &val)
{
    m_pPanel->flags |= val;
}

void ResPanel::setScrolFlags(const quint32 &val)
{
    m_pPanel->flags |= val;
}

void ResPanel::setPanelCentered(const bool &val)
{
    if (val)
        m_pPanel->flags |= RFP_CENTERED;
    else
        m_pPanel->flags &= ~RFP_CENTERED;
}

void ResPanel::setPanelRightText(const bool &val)
{
    if (val)
        m_pPanel->flags |= RFP_RIGHTTEXT;
    else
        m_pPanel->flags &= ~RFP_RIGHTTEXT;
}

void ResPanel::addBorder(const QRect &rect, const quint16 &St)
{
    BordR border;
    border.St = 0;
    border.x = rect.x();
    border.y = rect.y();
    border.l = rect.width();
    border.h = rect.height();
    border.fl = St;

    m_BordR.append(border);
}

void ResPanel::addText(const QString &value, const quint16 &x, const quint16 &y, const quint16 &St)
{
    TextStruct text;

    text._text = new TextR();
    text._text->x = x;
    text._text->y = y;
    text._text->St = St;
    text.value = value;

    m_Texts.append(text);
}

int ResPanel::save(ResBuffer *data)
{
    int stat = 0;
    QScopedArrayPointer<FieldSortData> fieldsort{};
    if (!isExcludeAutoNum())
        fieldsort.reset(new FieldSortData[m_Fields.size()]);

    data->setResVersion(1);
    stat = savePanel(data);

    return stat;
}

void ResPanel::prepFields()
{
    if (!m_Fields.empty())
        qSort(m_Fields.begin(), m_Fields.end(), FieldStruct::compare);

    if(m_pPanel->flags & RFP_NOAUTODIR)
    {
        for(int i = 0; i < m_Fields.size(); i++)
        {
            if(m_Fields[i].isActiveFld() && m_Fields[i].isTabStopFld())
            {
                m_pPanel->Pff = i;
                break;
            }
        }
    }
    else
    {
        if (m_Fields.empty())
            return;

        // Построение маршрута обхода полей
        int corn = -1, left, right, up, down, j = 0;
        FieldStruct* cf = &m_Fields[0],
            *nf, *fcorn, *fleft, *fright, *fup, *fdown;

        auto compcorn = [](const FieldStruct *cf, const FieldStruct *fcorn) -> int
        {
            if(cf->y() > fcorn->y())
                return 0;

            if(cf->y() < fcorn->y())
                return 1;

            if(cf->x() < fcorn->x())
                return 1;

            return 0;
        };

        auto compleft = [](const FieldStruct *cf, const FieldStruct *nf, const FieldStruct *f) -> int
        {
            int  cfx = cf->x(),
                cfy = cf->y(),
                nfx = nf->x() - cfx,
                nfy = nf->y() - cfy,
                fx  = f->x()  - cfx,
                fy  = f->y()  - cfy;


            if(fy == 0)
            {
                if(fx < 0)
                    if(nfy == 0 && nfx < 0 && nfx > fx)
                        return 1;
                    else
                        return 0;
                else if(nfy == 0 && nfx > 0 && nfx < fx)
                    return 0;
                else
                    return 1;
            }
            else if(fy < 0)
            {
                if(nfy == fy)
                    if(nfx > fx)
                        return 1;
                    else
                        return 0;
                else if(nfy > fy)
                    if(nfy < 0)
                        return 1;
                    else if(nfy == 0 && nfx < 0)
                        return 1;
                    else
                        return 0;
                else
                    return 0;
            }
            else
            {
                if(nfy == fy)
                    if(nfx > fx)
                        return 1;
                    else
                        return 0;
                else if(nfy < fy)
                    if(nfy < 0)
                        return 1;
                    else if(nfy == 0 && nfx < 0)
                        return 1;
                    else
                        return 0;
                else
                    return 1;
            }
        };

        auto cmx = [](int fx, int nfx) -> int
        {
            if(fx <= 0)
                if(nfx <= 0 && nfx > fx)
                    return 1;
                else
                    return 0;
            else if(nfx > 0 && nfx > fx)
                return 0;
            else
                return 1;
        };

        auto compup = [=](const FieldStruct *cf, const FieldStruct *nf, const FieldStruct *f) -> int
        {
            int  cfx = cf->x(),
                cfy = cf->y(),
                nfx = nf->x() - cfx,
                nfy = nf->y() - cfy,
                fx  = f->x()  - cfx,
                fy  = f->y()  - cfy;


            if(fy == 0)
            {
                if(nfy != 0)
                    return 1;
                else
                    return cmx(fx, nfx);
            }
            else if(fy < 0)
            {
                if(nfy >= 0)
                    return 0;
                else if(nfy < fy)
                    return 0;
                else if(nfy > fy)
                    return 1;
                else
                    return cmx(fx, nfx);
            }
            else
            {
                if(nfy == 0)
                    return 0;
                else if(nfy < 0)
                    return 1;
                else if(nfy > fy)
                    return 1;
                else if(nfy < fy)
                    return 0;
                else
                    return cmx(fx, nfx);
            }
        };

        auto compdown = [=](const FieldStruct *cf, const FieldStruct *nf, const FieldStruct *f) -> int
        {
            int  cfx = cf->x(),
                cfy = cf->y(),
                nfx = nf->x() - cfx,
                nfy = nf->y() - cfy,
                fx  = f->x()  - cfx,
                fy  = f->y()  - cfy;


            if(fy == 0)
            {
                if(nfy != 0)
                    return 1;
                else
                    return cmx(fx, nfx);
            }
            else if(fy < 0)
            {
                if(nfy > 0)
                    return 1;
                else if(nfy == 0)
                    return 0;
                else if(nfy > fy)
                    return 0;
                else if(nfy < fy)
                    return 1;
                else
                    return cmx(fx, nfx);
            }
            else
            {
                if(nfy == 0)
                    return 0;
                else if(nfy < 0)
                    return 0;
                else if(nfy > fy)
                    return 0;
                else if(nfy < fy)
                    return 1;
                else
                    return cmx(fx, nfx);
            }
        };

        for (int i = 0; i < m_Fields.size(); i++, cf++)
        {
            if (cf->isActiveFld())
            {
                if (cf->isTabStopFld())
                {
                    if (corn < 0)
                    {
                        corn = i;
                        fcorn = cf;
                    }
                    else if (compcorn(cf, fcorn))
                        corn = i;
                }

                for (j = 0; j < m_Fields.size(); j++)
                {
                    if (!m_Fields[j].isActiveFld() || i == j)
                        continue;

                    left = right = up = down = j;
                    fleft = fright = fup = fdown = nf = &m_Fields[j];

                    break;
                }

                if (nf)
                {
                    while (++j < m_Fields.size())
                    {
                        ++nf;

                        if (nf->isActiveFld() && i != j)
                        {
                            if (compleft(cf, nf, fleft))
                            {
                                left = j;
                                fleft = nf;
                            }

                            if (!compleft(cf, nf, fright))
                            {
                                right = j;
                                fright = nf;
                            }

                            if (compup(cf, nf, fup))
                            {
                                up = j;
                                fup = nf;
                            }

                            if (compdown(cf, nf, fdown))
                            {
                                down = j;
                                fdown = nf;
                            }
                        }
                    }

                    cf->_field->kl = left;
                    cf->_field->kr = right;
                    cf->_field->ku = up;
                    cf->_field->kd = down;
                }
                else
                    cf->_field->kl = cf->_field->kr = cf->_field->ku = cf->_field->kd = i;
            }
        }

        m_pPanel->Pff = corn;
    }
}

int ResPanel::savePanel(ResBuffer *data)
{
    r_coord lens = 0;

    lens = (r_coord)m_Comment.size();

    if(lens)
       lens++;

    if (data->write((char*)&lens, sizeof(r_coord)) != sizeof(r_coord))
        return 1;

    if(lens)
    {
        char *ResComment = (char*)malloc(lens);
        memset(ResComment, 0, lens);

        data->encodeString(m_Comment, ResComment, lens);
        if (data->write(ResComment, lens) != lens)
            return 1;

        data->setComment(m_Comment);
        free(ResComment);
    }

    prepFields();

    m_pPanel->Nb = m_BordR.size();
    m_pPanel->Pnumt = m_pPanel->Nt = m_Texts.size();
    m_pPanel->Pnumf = m_Fields.size();
    m_pPanel->flags |= P_NAME2ALLOCED;

    SizesTuple sizes;
    def_panelsize(sizes, data->version());
    m_pPanel->len = std::get<1>(sizes);

    if (!m_pPanel->Pff)
        m_pPanel->Pff = -1;

    int PanelSize = 0;
    if (data->headerVersion() >= 2)
        PanelSize = sizeof(PanelR);
    else
        PanelSize = sizeof(PanelR_1);

    if (data->write((char*)m_pPanel, PanelSize) != PanelSize)
        return 1;

    if (saveStatusLine(data))
        return 1;

    if (saveTitleLine(data))
        return 1;

    if (saveBorders(data))
        return 1;

    if (saveTextLabels(data))
        return 1;

    if (saveFields(data))
        return 1;

    return 0;
}

int ResPanel::saveStatusLine(ResBuffer *data)
{
    r_coord lens = (r_coord)m_Status.size();
    if(lens)
        lens++;

    if (data->write((char*)&lens, sizeof(r_coord)) != sizeof(r_coord))
        return 1;

    if(lens)
    {
        char *Status = (char*)malloc(lens);
        memset(Status, 0, lens);

        data->encodeString(m_Status, Status, lens);
        if (data->write(Status, lens) != lens)
            return 1;

        free(Status);
    }

    lens = (r_coord)m_StatusRD.size();
    if(lens)
        lens++;

    if (data->write((char*)&lens, sizeof(r_coord)) != sizeof(r_coord))
        return 1;

    if(lens)
    {
        char *StatusRD = (char*)malloc(lens);
        memset(StatusRD, 0, lens);

        data->encodeString(m_StatusRD, StatusRD, lens);
        if (data->write(StatusRD, lens) != lens)
            return 1;

        free(StatusRD);
    }

    return 0;
}

int ResPanel::saveTitleLine(ResBuffer *data)
{
    r_coord lens = (r_coord)m_Title.size();
    if(lens)
        lens++;

    if (data->write((char*)&lens, sizeof(r_coord)) != sizeof(r_coord))
        return 1;

    if(lens)
    {
        char *Title = (char*)malloc(lens);
        memset(Title, 0, lens);

        data->encodeString(m_Title, Title, lens);
        if (data->write(Title, lens) != lens)
            return 1;

        free(Title);
    }

    return 0;
}

int ResPanel::saveBorders(ResBuffer *data)
{
    for (const BordR &box : m_BordR)
    {
        if (data->write((char*)&box, sizeof(BordR)) != sizeof(BordR))
            return 1;
    }

    return 0;
}

int ResPanel::saveFields(ResBuffer *data)
{
    int ver = data->headerVersion();
    for (FieldStruct fld : m_Fields)
    {
        fld._field->vfl = fld.name2.isEmpty() ? 0 : 1;
        fld._field->lens = fld.name2.length();

        fld._field->x -= m_pPanel->x;
        fld._field->y -= m_pPanel->y;

        if (!fld.name2.isEmpty())
            fld._field->lens ++;

        fld._field->nameLen = fld.name.length();
        if (!fld.name.isEmpty())
            fld._field->nameLen ++;

        fld._field->formLen = fld.formatStr.length();
        if (!fld.formatStr.isEmpty())
            fld._field->formLen ++;

        fld._field->tooltipLen = fld.toolTip.length();
        if (!fld.toolTip.isEmpty())
            fld._field->tooltipLen ++;

        int fldsize = (ver >= 2) ? sizeof(FieldR) : sizeof(FieldR_1);
        if (data->write((char*)fld._field, fldsize) != fldsize)
            return 1;

        if(fld._field->lens)
        {
            if (!data->writeString(fld.name2))
                return 1;
        }

        if(fld._field->nameLen)
        {
            if (!data->writeString(fld.name))
                return 1;
        }

        if(fld._field->formLen)
        {
            if (!data->writeString(fld.formatStr))
                return 1;
        }

        if(fld._field->tooltipLen)
        {
            if (!data->writeString(fld.toolTip))
                return 1;
        }

        //ResBuffer::debugSaveToFile("1_savefld.txt", )
    }

    return 0;
}

int ResPanel::saveTextLabels(ResBuffer *data)
{
    for (TextStruct &text : m_Texts)
    {
        TextR tr;

        tr.St = text._text->St;
        tr.x  = (r_coord)text._text->x;
        tr.y  = (r_coord)text._text->y;
        tr.lens = (r_coord)text.value.size();

        if(tr.lens)
        {
            tr.vfl = 0;
            tr.lens++;
        }
        else
            tr.vfl = 1;

        if (data->write((char*)&tr, sizeof(TextR)) != sizeof(TextR))
            return 1;

        if(tr.lens)
        {
            char *str = (char*)malloc(tr.lens);
            memset(str, 0, tr.lens);

            data->encodeString(text.value, str, tr.lens);
            if(data->write((char*)str, tr.lens) != tr.lens)
                return 1;

            free(str);
        }
    }

    return 0;
}

void ResPanel::def_panelsize(ResPanel::SizesTuple &sizes, int ver)
{
    int   sizePnl = (ver >= 2) ? sizeof(PanelR) : sizeof(PanelR_1);
    int   sizeFld = (ver >= 2) ? sizeof(FieldR) : sizeof(FieldR_1);
    long  size = sizePnl + sizeof(r_coord) * 4 + sizeFld * m_Fields.size();

    int adsize = 0;

    if(!m_Comment.isEmpty())
       size += m_Comment.size() + 1;

    if(!m_Status.isEmpty())
    {
        size += m_Status.size() + 1;
        adsize += m_Status.size() + 1;
    }

    if(!m_StatusRD.isEmpty())
    {
        size += m_StatusRD.size() + 1;
        adsize += m_StatusRD.size() + 1;
    }

    if(!m_Title.isEmpty())
    {
        size += m_Title.size() + 1;
        adsize += m_Title.size() + 1;
    }

    if(!m_Title.isEmpty())
        size += m_BordR.size() * sizeof(BordR);

    if(!m_Texts.isEmpty())
    {
        size += m_Texts.size() * sizeof(TextR);

        for (TextStruct &item : m_Texts)
            adsize += item.value.size() + 1;
    }

    if(!m_Fields.isEmpty())
    {
        size += m_Fields.size() * sizeof(FieldR);

        for (FieldStruct &item : m_Fields)
        {
            if (!item.formatStr.isEmpty())
                adsize += item.formatStr.size() + 1;

            if (!item.toolTip.isEmpty())
                adsize += item.toolTip.size() + 1;

            if (!item.name.isEmpty())
                adsize += item.name.size() + 1;

            if (!item.name2.isEmpty())
                adsize += item.name2.size() + 1;
        }
    }

    sizes = std::make_tuple(size, adsize);
}

// Проверка на выход элемента за пределы области
bool ResPanel::__CheckElement(int x, int y, int h, int l, int x1, int y1, int x2, int y2, int border)
{
    bool  ret = true;

    x += x1;
    y += y1;

    if(x < (x1 + border))
        ret = false;

    if(y < (y1 + border))
        ret = false;

    if(x + l - 1 > (x2 - border))
        ret = false;

    if(y + h - 1 > (y2 - border))
        ret = false;

    return ret;
}

// Проверка пересечения двух прямоугольников
bool ResPanel::__CheckCrossRect(int r1_x, int r1_y, int r1_h, int r1_l, int r2_x, int r2_y, int r2_h, int r2_l)
{
    int   r1_left = r1_x, r1_top = r1_y, r1_right = r1_x + r1_l - 1, r1_bottom = r1_y + r1_h - 1,
        r2_left = r2_x, r2_top = r2_y, r2_right = r2_x + r2_l - 1, r2_bottom = r2_y + r2_h - 1;
    bool  ret     = ((r1_left > r2_right) || (r2_left > r1_right) || (r1_top > r2_bottom) || (r2_top > r1_bottom));

    return ret;
}

// Проверка полного вхождения прямоугольника r1 в r2
bool ResPanel::__CheckEntryRect(int r1_x, int r1_y, int r1_h, int r1_l, int r2_x, int r2_y, int r2_h, int r2_l)
{
    int r1_left = r1_x, r1_top = r1_y, r1_right = r1_x + r1_l - 1, r1_bottom = r1_y + r1_h - 1,
        r2_left = r2_x, r2_top = r2_y, r2_right = r2_x + r2_l - 1, r2_bottom = r2_y + r2_h - 1;
    bool  ret = ((r1_left >= r2_left) && (r1_top >= r2_top) && (r1_right <= r2_right) && (r1_bottom <= r2_bottom));

    return ret;
}

// Проверка взаимного пересечения двух полей
bool ResPanel::__CheckCrossField(FieldR *f1, FieldR *f2)
{
    return __CheckCrossRect(f1->x, f1->y, f1->h, f1->l, f2->x, f2->y, f2->h, f2->l);
}

// Проверка текущего поля на пересечение с остальными
bool ResPanel::__CheckCrossFields(int curr)
{
    bool ret = true;
    for(int i = 0; i < m_Fields.size(); i++)
    {
        if(i != curr)
        {
            FieldR *fld = m_Fields[i]._field;
            ret = __CheckCrossField(fld, m_Fields[curr]._field);

            if(!ret)
                break;
        }
    }

    return ret;
}

int ResPanel::checkResource()
{
    int  stat = 0;

    switch(type())
    {
    case LbrObject::RES_PANEL:
        stat = checkPanel();
        break;

    case LbrObject::RES_SCROL:
    case LbrObject::RES_LS:
    case LbrObject::RES_BS:
        stat = checkScrol();
        break;
    }

    return stat;
}

int ResPanel::checkPanel()
{
    int stat = 0;
    int border = dGET_BORDER(m_pPanel);

    int i;
    int x, y, h, l;

    bool ret = true;
    for (i = 0; i < m_Fields.size(); i++)
    {
        x = m_Fields[i]._field->x;
        y = m_Fields[i]._field->y;
        h = m_Fields[i]._field->h;
        l = m_Fields[i]._field->l;

        // Проверка на выход элемента за пределы области
        ret = __CheckElement(x, y, h, l, m_pPanel->x1, m_pPanel->y1, m_pPanel->x2, m_pPanel->y2, border);

        if (ret)
        {
            ret = __CheckCrossFields(i);

            if(!ret)
            {
                stat = 2;
                break;
            }
        }
    }

    // Проверяем текстовые метки и рамки
    for(i = 0; i < m_Texts.size(); i++)
    {
        x = m_Texts[i]._text->x;
        y = m_Texts[i]._text->y;
        h = 1;
        l = m_Texts[i].value.size() - 1;

        ret = __CheckElement(x, y, h, l + 1, m_pPanel->x1, m_pPanel->y1, m_pPanel->x2, m_pPanel->y2, border);
        if(!ret)
        {
            stat = 4;
            break;
        }
    }

    for(i = 0; i < m_BordR.size(); i++)
    {
        x = m_BordR[i].x;
        y = m_BordR[i].y;
        h = m_BordR[i].h;
        l = m_BordR[i].l;

        ret = __CheckElement(x, y, h, l, m_pPanel->x1, m_pPanel->y1, m_pPanel->x2, m_pPanel->y2, border);
        if(!ret)
        {
            stat = 5;
            break;
        }
    }

    return stat;
}

int ResPanel::checkScrol()
{
    int stat = checkPanel();

    if (!stat)
        stat = checkScrolRect(m_pPanel->x, m_pPanel->y, m_pPanel->h * m_pPanel->Mn, m_pPanel->l);

    return stat;
}

int ResPanel::checkScrolRect(int sx, int sy, int sh, int sl)
{
    int  stat = 0;

    // Проверка задания области скроллинга
    if((sh <= 0) || (sl <= 0))
        stat = 7;

    // Проверка корректности определения области скроллинга
    if(!stat)
    {
        int  border = dGET_BORDER(m_pPanel);

        // Область скроллинга не должна выходить за границы панели
        if(!__CheckElement(sx, sy, sh, sl, m_pPanel->x1, m_pPanel->y1, m_pPanel->x2, m_pPanel->y2, border))
            stat = 8;

        // Если скроллинг с автоформированием заголовка, то область скроллинга
        // должна начинаться с 4-й строки
        if(!stat && (m_pPanel->flags & RFP_AUTOHEAD))
        {
            if(sy < (border + 3))
                stat = 6;
        }
    }

    if(!stat)
    {
        if(m_Fields.size() > 0)//m_pPanel->Pnumf > 0)
        {
            // Эту проверку выполняем только в том случае, если не установлен флаг
            // "Автоматическое размещение полей"
            if(!(m_pPanel->flags & RFP_AUTOFIELDS))
            {
                int  x, y, h, l;


                for(int  i = 0; i < m_Fields.size(); i++)
                {
                    x = m_Fields[i]._field->x;
                    y = m_Fields[i]._field->y;
                    h = m_Fields[i]._field->h;
                    l = m_Fields[i]._field->l;

                    if(!__CheckEntryRect(x, y, h, l, sx, sy, sh, sl))
                    {
                        stat = 3;
                        break;
                    }
                }
            }
        }
        else
            stat = 9;
    }

    return stat;
}

QString ResPanel::saveXml(const QString &encode)
{
    QDomDocument doc;

    QString resType;
    switch(type())
    {
    case LbrObject::RES_PANEL:
        resType = "panel";
        break;
    case LbrObject::RES_SCROL:
        resType = "scrol";
        break;
    case LbrObject::RES_LS:
        resType = "lscrol";
        break;
    case LbrObject::RES_BS:
        resType = "bscrol";
        break;
    }

    QString result;
    QTextStream stream(&result);
    stream.setCodec(encode.toLocal8Bit().data());
    stream << QString("<?xml version=\"1.0\" encoding=\"%1\"?>").arg(encode) << Qt::endl;
    stream << "<reslib xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"" << Qt::endl;
    stream << "    xsi:schemaLocation=\"http://www.softlab.ru reslib.xsd\"" << Qt::endl;
    stream << "    xmlns=\"http://www.softlab.ru\">" << Qt::endl;


    stream << QString(" <%1 name=\"%2\" dt=\"%3\" St=\"%4\" x1=\"%5\" y1=\"%6\" x2=\"%7\" y2=\"%8\" "
                      "PHelp=\"%9\" Pff=\"%10\" flags=\"%11\"")
                  .arg(resType)
                  .arg(name())
                  .arg(m_ResTime.toString("yyyy-MM-ddTHH:mm:ss"))
                  .arg(m_pPanel->St)
                  .arg(m_pPanel->x1)
                  .arg(m_pPanel->y1)
                  .arg(m_pPanel->x2)
                  .arg(m_pPanel->y2)
                  .arg(m_pPanel->PHelp)
                  .arg(m_pPanel->Pff)
                  .arg(m_pPanel->flags);

    if(type() != LbrObject::RES_PANEL)
        stream << QString(" Mn=\"%1\" x=\"%2\" y=\"%3\" l=\"%44\" h=\"%5\">")
                      .arg(m_pPanel->Mn)
                      .arg(m_pPanel->x)
                      .arg(m_pPanel->y)
                      .arg(m_pPanel->l)
                      .arg(m_pPanel->h) << Qt::endl;
    else
        stream << ">" << Qt::endl;

    if (!m_Comment.isEmpty())
        stream << QString("  <comment>%1</comment>").arg(m_Comment) << Qt::endl;

    if (!m_Status.isEmpty())
        stream << QString("  <stline>%1</stline>").arg(m_Status) << Qt::endl;

    if (!m_StatusRD.isEmpty())
        stream << QString("  <stlineRd>%1</stlineRd>").arg(m_StatusRD) << Qt::endl;

    if (!m_Title.isEmpty())
        stream << QString("  <headLine>%1</headLine>").arg(m_Title) << Qt::endl;

    for(int i = 0; i < m_pPanel->Nb; ++i)
    {
        stream << QString("  <bord  St=\"%d\" x=\"%d\" y=\"%d\" l=\"%d\" h=\"%d\" fl=\"%d\"/>")
                      .arg(m_BordR[i].St)
               .arg(m_BordR[i].x)
               .arg(m_BordR[i].y)
               .arg(m_BordR[i].l)
               .arg(m_BordR[i].h)
               .arg(m_BordR[i].fl)
               << Qt::endl;
    }

    for(int i = 0; i < m_pPanel->Nt; ++i)
    {
        stream << QString("  <text  St=\"%1\" x=\"%2\" y=\"%3\">%4</text>")
                      .arg(m_Texts[i]._text->St)
                      .arg(m_Texts[i]._text->x)
                      .arg(m_Texts[i]._text->y)
                      .arg(m_Texts[i].value)
               << Qt::endl;
    }

    for(int i = 0; i < m_pPanel->Pnumf; ++i)
    {
        stream << QString("  <field  Ftype=\"%1\" St=\"%2\" FVt=\"%3\" FVp=\"%4\" x=\"%5\" y=\"%6\" l=\"%7\" h=\"%8\" ")
                      .arg(m_Fields[i]._field->Ftype)
                      .arg(m_Fields[i]._field->St)
                      .arg(m_Fields[i]._field->FVt)
                      .arg(m_Fields[i]._field->FVp)
                      .arg(m_Fields[i]._field->x)
                      .arg(m_Fields[i]._field->y)
                      .arg(m_Fields[i]._field->l)
                      .arg(m_Fields[i]._field->h);

        stream << QString("kl=\"%1\" kr=\"%2\" ku=\"%3\" kd=\"%4\" FHelp=\"%5\" vfl=\"%6\" flags=\"%7\" group=\"%8\">")
                      .arg(m_Fields[i]._field->kl)
                      .arg(m_Fields[i]._field->kr)
                      .arg(m_Fields[i]._field->ku)
                      .arg(m_Fields[i]._field->kd)
                      .arg(m_Fields[i]._field->FHelp)
                      .arg(m_Fields[i]._field->vfl)
                      .arg(m_Fields[i]._field->flags)
                      .arg(m_Fields[i]._field->group) << Qt::endl;

        if (!m_Fields[i].name2.isEmpty())
            stream << QString("   <label>%1</label>").arg(m_Fields[i].name2) << Qt::endl;

        if (!m_Fields[i].name.isEmpty())
            stream << QString("   <name>%1</name>").arg(m_Fields[i].name) << Qt::endl;

        if (!m_Fields[i].formatStr.isEmpty())
            stream << QString("   <fmtname>%1</fmtname>").arg(m_Fields[i].formatStr) << Qt::endl;

        if (!m_Fields[i].toolTip.isEmpty())
            stream << QString("   <tooltip>%1</tooltip>").arg(m_Fields[i].toolTip) << Qt::endl;

        stream << QString("  </field>") << Qt::endl;
    }

    stream << QString(" </%1>").arg(resType) << Qt::endl;
    stream << QString("</reslib>");

    return result;
}

QString ResPanel::GetCheckError(int stat)
{
    QStringList mes = {
        tr("Проверка выполнена успешно"),
        tr("Выход поля за границы панели"),
        tr("Перекрытие полей"),
        tr("Поле вне области скроллинга"),
        tr("Выход метки за границы панели"),
        tr("Выход рамки за границы панели"),
        tr("Некорректное определение области скроллинга"),
        tr("Область скроллинга не определена"),
        tr("Выход области скроллинга за границы панели"),
        tr("Отсутствуют поля")
    };

    return mes[stat];
}
