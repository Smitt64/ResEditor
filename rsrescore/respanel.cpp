#include "respanel.h"
#include "reslib.h"
#include "res_lbr.h"
#include "rscoreheader.h"
#include <QDebug>

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
}

static int rds(int hd, char **s, r_coord vfl, r_coord lens, HRSLCVT hcvtRd)
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
}

int ResPanel::loadProc(ResLib *res)
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
}

int ResPanel::readItems(struct PanelR *pp, ResLib *res, bool readName2)
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
                        s = (char*)malloc(sizeof(char)*(element._field->formLen + 1));
                        memset(s, 0, element._field->formLen + 1);

                        err = rds(res->fileHandle(), &s, 0, element._field->tooltipLen, nullptr);
                        element.toolTip = res->decodeString(s);
                        free(s);
                    }
                }
            }
        }

        //qDebug() << "element._field->FVt: " << element._field->FVt;
        m_Fields.append(element);
    }

    return err;
}

int ResPanel::load(const QString &name, ResLib *res)
{
    m_Name = name;
    return res->loadResource(name, RES_PANEL, this);
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

quint32 ResPanel::helpPage() const
{
    return m_pPanel->PHelp;
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
