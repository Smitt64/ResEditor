#include "lbrdllobjectprivate.h"
#include <QDir>
#include <QGuiApplication>
#include <QDebug>
#include "lbrreslistmodel.h"
#include "LbrObjectHeaders.h"
#include "resbuffer.h"
#include <QDataStream>

typedef struct tagModeleDirlement : RLibDirElem
{
    char comment[dHRES_COMMENT];
}ModeleDirlement;

LbrDllObjectPrivate::LbrDllObjectPrivate(LbrObjectInterface *obj):
    LbrObjectInterfacePrivate(obj),
    m_ResFile(nullptr),
    m_LibDir(nullptr)
{
    m_OemCodec = QTextCodec::codecForName("IBM 866");
}

LbrDllObjectPrivate::~LbrDllObjectPrivate()
{
    if (m_LibDir)
        _FreeDirectory(m_LibDir);
}

bool LbrDllObjectPrivate::loadLib()
{
    if (m_lib.isLoaded())
        return true;

    try
    {
        QDir dir = QDir::current();
        if (QFile::exists(dir.absoluteFilePath("rsrtlwm.dll")))
            m_lib.setFileName(dir.absoluteFilePath("rsrtlwm.dll"));
        else
        {
            dir = QDir(qApp->applicationDirPath());

            if (QFile::exists(dir.absoluteFilePath("rsrtlwm.dll")))
                m_lib.setFileName(dir.absoluteFilePath("rsrtlwm.dll"));
            else
                throw (QString("Can't find rsrtlwm.dll"));
        }

        if (!m_lib.load())
            throw (QString("Can't load rsrtlwm.dll: %1").arg(m_lib.errorString()));

        _OpenLib = resolve<tOpenLib>("_OpenLib");
        _ResClose = resolve<tResClose>("ResClose");
        _ResError = resolve<tResError>("ResError");

        _BuildDirectory = resolve<tBuildDirectory>("BuildDirectory");
        _FreeDirectory = resolve<tFreeDirectory>("FreeDirectory");

        _FindResObject = resolve<tFindResObject>("FindResObject");
        _GetResObject = resolve<tGetResObject>("GetResObject");
        _ResGetCurPtr = resolve<tResGetCurPtr>("ResGetCurPtr");
        _ResGetCurHeader = resolve<tResGetCurHeader>("ResGetCurHeader");
        _ResGetElem = resolve<tResGetElem>("ResGetElem");
        _ResReadPrefix = resolve<tResReadPrefix>("ResReadPrefix");
        _tResNumObjects = resolve<tResNumObjects>("ResNumObjects");
        _DeleteResObject = resolve<tDeleteResObject>("DeleteResObject");

        _LibOpenStream = resolve<tLibOpenStream>("LibOpenStream");
        _LibCloseStream = resolve<tLibCloseStream>("LibCloseStream");
        _LibReadStream = resolve<tLibReadStream>("LibReadStream");
        _LibWriteStream = resolve<tLibWriteStream>("LibWriteStream");

        /*_ResSetPosStream = resolve<tResSetPosStream>("ResSetPosStream");
        _ResGetSizeStream = resolve<tResGetSizeStream>("ResGetSizeStream");
        _ResGetPosStream = resolve<tResGetPosStream>("ResGetPosStream");*/
    }
    catch(QString &e)
    {
        qDebug() << e;
        return false;
    }

    return true;
}

bool LbrDllObjectPrivate::open(const QString &filename)
{
    bool hr = true;
    m_ResFile = (void*)malloc(sizeof(ResFile));

    int stat = _OpenLib(m_ResFile, filename.toLocal8Bit().data(), RO_MODIFY | RO_TRN);
    if (!stat)
    {
        m_pDirModel.reset(new LbrResListModel());

        hr = initObjectList();

        if (!hr)
        {
            _ResClose(m_ResFile);
            m_ResFile = nullptr;
            free(m_ResFile);
        }
    }
    else
    {
        qDebug() << _ResError(stat);
        hr = false;
    }
    return hr;
}

static int RLibDirElemFiltrFunc(RLibDirElem *rc)
{
    static QList<int> ResTypes =
        {
            LbrObjectInterface::RES_PANEL,
            LbrObjectInterface::RES_SCROL,
            LbrObjectInterface::RES_BS
        };

    if (ResTypes.contains(rc->type))
        return 0;

    return 1;
}

bool LbrDllObjectPrivate::initObjectList()
{
    if (m_LibDir)
        _FreeDirectory(m_LibDir);

    m_LibDir = malloc(sizeof(RLibDir));
    memset(m_LibDir, 0, sizeof(RLibDir));

    if (_BuildDirectory(m_LibDir, m_ResFile, (Qt::HANDLE)RLibDirElemFiltrFunc, -1, dHRES_COMMENT))
    {
        if (m_LibDir)
        {
            _FreeDirectory(m_LibDir);
            free(m_LibDir);
            m_LibDir = nullptr;
        }

        return false;
    }

    RLibDir *dir = (RLibDir*)m_LibDir;
    char *ptr = (char*)dir->Elem;
    for (int i = 0; i < dir->NumElem; i++)
    {
        RLibDirElem *elem = (RLibDirElem*)(ptr + (sizeof(RLibDirElem) + dHRES_COMMENT)*i);
        m_pDirModel->addDirElement(elem);
    }

    return true;
}

Qt::HANDLE LbrDllObjectPrivate::findLibDirElem(const QString &name, const int &type)
{
    RLibDirElem *found = nullptr;

    RLibDir *dir = (RLibDir*)m_LibDir;
    char *ptr = (char*)dir->Elem;
    for (int i = 0; i < dir->NumElem; i++)
    {
        RLibDirElem *elem = (RLibDirElem*)(ptr + (sizeof(RLibDirElem) + dHRES_COMMENT)*i);

        if (name == elem->name && elem->type == type)
        {
            found = elem;
            break;
        }
    }

    return found;
}

bool LbrDllObjectPrivate::isResExists(const QString &name, const int &type)
{
    if (!m_ResFile)
        return false;

    return !_FindResObject(m_ResFile, name.toLocal8Bit().data(), (char)type);
}

bool LbrDllObjectPrivate::deleteResource(const QString &name, const int &type)
{
    if (!m_ResFile)
        return false;

    int stat = _DeleteResObject(m_ResFile, name.toLocal8Bit().data(), (char)type);

    if (!stat)
        m_pDirModel->removeElement(name, type);

    return !stat;
}

void LbrDllObjectPrivate::getResource(const QString &name, const int &type, ResBuffer **buffer)
{
    Q_Q(LbrDllObject);

    LibStream strm1;
    if (!_LibOpenStream(&strm1, m_ResFile, RES_MODE_READ, name.toLocal8Bit().data(), type, 1, -1))
    {
        _ResReadPrefix(m_ResFile, &strm1.rc);
        ResHeader* rh = (ResHeader*)_ResGetCurHeader();

        int datalen = rh->resSizeLo;
        char* data = (char*)malloc(datalen);
        memset(data, 0, datalen);
        _LibReadStream(&strm1, data, datalen, NULL);
        QByteArray byte(data, datalen);
        free(data);

        q->createResBuffer(name, type, &byte, buffer);
        (*buffer)->setResHeader(rh);

        r_coord lens = 0;
        QDataStream commentStream(&byte, QIODevice::ReadOnly);
        commentStream >> lens;

        char *comment = (char*)malloc(lens + 1);
        memset(comment, 0, lens + 1);
        commentStream.readRawData(comment, lens);
        QString decoded = (*buffer)->decodeString(comment);
        (*buffer)->setComment(decoded);
        (*buffer)->open(QIODevice::ReadOnly);

        free(comment);

        _LibCloseStream(&strm1, false);

/*#ifdef _DEBUG
        QDir d = QDir::current();
        d.mkdir("loaddump");
        d.cd("loaddump");
        (*buffer)->debugSaveToFile(d.absoluteFilePath(QString("%1_%2.bin").arg(type).arg(name)));
#endif*/
    }
}

bool LbrDllObjectPrivate::beginSaveRes(const QString &name, const int &type, ResBuffer **buffer)
{
    Q_Q(LbrDllObject);

    LibStream *strm1 = (LibStream*)malloc(sizeof(LibStream));

    int stat = _LibOpenStream(strm1, m_ResFile, RES_MODE_WRITE,
                              name.toLocal8Bit(), type, 1, -1);

    if (stat)
    {
        qDebug() << "beginSaveRes: " << resError(stat);
        free(strm1);
        return false;
    }

    q->createResBuffer(name, type, buffer);
    (*buffer)->setResStream(strm1);
    (*buffer)->open(QIODevice::ReadWrite);

    return true;
}

bool LbrDllObjectPrivate::endSaveRes(ResBuffer **buffer)
{
    int stat = 0;
    bool abort = false;
    int size = (*buffer)->size();

    LibStream *strm1 = (LibStream*)(*buffer)->getResStream();
    if (!size || !strm1)
        abort = true;

    if (!abort)
    {
        int wrSize = 0;
        void *data = (void*)(*buffer)->data().data();
        strm1->derived.rhead.ver = (*buffer)->headerVersion();
        stat = _LibWriteStream(strm1, data, size, &wrSize);

        if (stat)
        {
            qDebug() << "endSaveRes error: " << resError(stat);
            abort = true;
        }
    }

    stat = _LibCloseStream(strm1, abort);

    if (stat)
    {
        qDebug() << "_LibCloseStream error: " << resError(stat);
    }
    else
    {
        ModeleDirlement elem;
        memset(&elem, 0, sizeof(ModeleDirlement));

        QString comment = (*buffer)->comment();
        strncpy_s(elem.comment, m_OemCodec->fromUnicode(comment), dHRES_COMMENT);
        strncpy_s(elem.name, (*buffer)->name().toLocal8Bit().data(), RESNAMELEN);
        elem.type = (*buffer)->type();
        elem.size = (*buffer)->size();
        elem.ftime = strm1->derived.rhead.ftime;
        elem.ver = strm1->derived.rhead.ver;
        m_pDirModel->addDirElement(&elem);
        //m_pDirModel->sort(0);
    }
    free(strm1);

    delete (*buffer);
    buffer = nullptr;

    return !stat;
}

QString LbrDllObjectPrivate::resError(int stat)
{
    char *err = _ResError(stat);
    return m_OemCodec->toUnicode(err);
}
