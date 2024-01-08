#include "respanel.h"
#include "resbuffer.h"
#include "rscoreheader.h"
#include <QDebug>
#include <QDataStream>
#include <QDomElement>

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

FieldStruct::FieldStruct(const FieldStruct &other)
{
    _field = new FieldR();
    memcpy(_field, other._field, sizeof(FieldR));

    name = other.name;
    name2 = other.name2;
    formatStr = other.formatStr;
    toolTip = other.toolTip;
}

FieldStruct::FieldStruct()
{
    _field = nullptr;
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

void ResPanel::beginAddField(const QString &name, const QString &name2)
{
    m_NewField.reset();
    m_NewField.name  = name;
    m_NewField.name2 = name2;
}

void ResPanel::setFieldDataType(const quint8 &FieldType, const quint8 &DataType, const quint16 &DataLength)
{
    m_NewField._field->Ftype = FieldType;
    m_NewField._field->FVt = DataType;
    m_NewField._field->FVp = DataLength;
}

void ResPanel::setLenHeight(const quint8 &len, const quint8 &height)
{
    m_NewField._field->l = len;
    m_NewField._field->h = height;
}

void ResPanel::setFormatTooltip(const QString &formatStr, const QString &toolTip)
{
    m_NewField.formatStr = formatStr;
    m_NewField.toolTip = toolTip;
}

void ResPanel::setFieldStyle(const quint16 &St)
{
    m_NewField._field->St = St;
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
    m_pPanel->St = border | style;
}

void ResPanel::setPanelHelp(const quint16 &help)
{
    m_pPanel->PHelp = help;
}

void ResPanel::setPanelExcludeFlags(const quint32 &val)
{
    m_pPanel->flags = val;
}

void ResPanel::setPanelCentered(const bool &val)
{
    m_pPanel->flags |= RFP_CENTERED;
}

void ResPanel::setPanelRightText(const bool &val)
{
    m_pPanel->flags |= RFP_RIGHTTEXT;
}

void ResPanel::addBorder(const QRect &rect, const quint16 &St)
{
    BordR border;
    border.St = St;
    border.x = rect.x();
    border.y = rect.y();
    border.l = rect.width();
    border.h = rect.height();
    border.fl = border.y;

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
    {
        fieldsort.reset(new FieldSortData[m_Fields.size()]);
        // FieldSortData
    }

    data->setResVersion(2);
    stat = savePanel(data);

    if (!stat)
    {
        for (const BordR &br : qAsConst(m_BordR))
        {
            if (data->write((char*)&br, sizeof(BordR)) != sizeof(BordR))
                return 1;
        }
    }
    return stat;
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

        free(ResComment);
    }

    SizesTuple sizes;
    def_panelsize(sizes, data->version());

    m_pPanel->len = std::get<1>(sizes);
    m_pPanel->Nb = m_BordR.size();
    m_pPanel->Pnumt = m_pPanel->Nt = m_Texts.size();

    if (data->write((char*)m_pPanel, sizeof(PanelR)) != sizeof(PanelR))
        return 1;

    if (saveStatusLine(data))
        return 1;

    if (saveTitleLine(data))
        return 1;

    if (saveTextLabels(data))
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

    sizes = std::make_tuple(size, adsize);
}
