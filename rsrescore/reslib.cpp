#include "reslib.h"
//#include "RsResHeader.h"
#include <QDir>
#include <QDebug>
#include <QApplication>
#include <QTextCodec>
#include "reslibdirmodel.h"
//#include "RsResHeader.h"
#include "RsResInterface.h"
#include <functional>
#include <io.h>
#include "res_lbr.h"
#include "rscoreheader.h"

#define RsResFile ((ResFile*)m_ResPtr)

static int  loc_FilterResType = -1;

ResLib::ResLib(QObject *parent)
    : QObject{parent},
      m_ResPtr(0)
{
    m_pModel = new ResLibDirModel(this);
}

ResLib::~ResLib()
{
    if (m_ResPtr)
    {
        ResClose(RsResFile);
        delete RsResFile;
    }
}

QString ResLib::resError(const int &stat)
{
    char *sResError = ResError(stat);
    QString error = decodeString(sResError);
    return error;
}

QString ResLib::decodeString(const char *str)
{
    QTextCodec *codec = QTextCodec::codecForName("IBM 866");
    return codec->toUnicode(str);
}

ResLibDirModel *ResLib::model()
{
    return m_pModel;
}

bool ResLib::open(const QString &resFile)
{
    bool hr = true;
    ResFile *res = new ResFile();
    memset(res, 0, sizeof(ResFile));
    int stat = _OpenLib(res, resFile.toLatin1().constData(), RO_READDIR);

    qDebug() << stat << resFile << resError(stat);

    if (!stat)
    {
        m_ResPtr = (qintptr)res;
        fillDir(res, m_pModel);
    }

    return hr;
}

static int __FilterRes(RLibDirElem *e)
{
    // Если тип не задан, отбираем все ресурсы, кроме отчетов
    if(loc_FilterResType <= 0)
    {
        if(e->type == RES_PANEL || e->type == RES_SCROL || e->type == RES_BS || e->type == RES_LS || e->type == RES_MENU2)
            return 0;
    }
    else if(e->type == loc_FilterResType)
        return 0;

    return -1;
}

void ResLib::resGetCurHeader(void **resh)
{
    *resh = (void*)ResGetCurHeader();
}

int ResLib::resHeaderVersion()
{
    return ResGetCurHeader()->ver;
}

int ResLib::readBytes(void *ptr, quint64 len)
{
    ResFile *res = RsResFile;
    int hd = res->hd;

    int readed = read(hd, ptr, len);
    if (readed != len)
    {
        int *err = _errno();
        qDebug() << "readBytes: " << *err;
    }
    return readed;
}

int ResLib::fileHandle() const
{
    ResFile *res = RsResFile;
    return res->hd;
}

int RsResLoader(ResFile *rf, void **pv)
{
    RsResInterface *pCallback = (RsResInterface*)(*pv);
    ResLib *pLib = pCallback->m_pLib;

    return pCallback->loadProc(pLib);
}

int ResLib::loadResource(const QString &name, const qint16 &type, RsResInterface *pCallback)
{
    pCallback->m_pLib = this;
    int stat = GetResObject(RsResFile, name.toLocal8Bit().data(), type, (void **)&pCallback, RsResLoader);

    return stat;
}

void ResLib::fillDir(void *resLbr, ResLibDirModel *model)
{
    RLibDir resDir;
    int stat = BuildDirectory(&resDir, (ResFile*)resLbr, __FilterRes, -1, dHRES_COMMENT);

    int  size = resDir.NumElem;
    for (int i = 0; i < size; i++)
    {
        RLibDirElem  *rec = ELEM_PTR(&resDir, i);
        char *cmt = CMT_PTR(rec);

        model->addElement(rec->type, decodeString(rec->name), decodeString(cmt));
    }
/*"MVKINSTE"*/
    /*HRSLRESFILE hResFile = (HRSLRESFILE)resLbr;
    void  *p = this;
    GetResObject(hResFile, "ACCPRMRS", RES_PANEL, (void**)&p, GetPanel);*/

    /*LibElem  rc;
    strncpy(rc.name, "MVKINSTE", RESNAMELEN - 1);
    rc.name[RESNAMELEN - 1] = '\0';
    strupr(rc.name);
     rc.type = RES_PANEL;

    ResStream stream;
    stat = ResOpenStream(&stream, RsResFile, (Resource*)&rc, RES_MODE_READ, 1, 0);
    qDebug() << "ResOpenStream" << stat;

    r_coord  lens = -1;
    int readed = 0;
    stat = ResReadStream(&stream, &lens, sizeof(lens), &readed);
    qDebug() << "ResReadStream" << stat;

    PanelR_1 pan;
    stat = ResReadStream(&stream, &pan, sizeof(PanelR_1), &readed);
    qDebug() << "ResReadStream" << stat;*/
}
