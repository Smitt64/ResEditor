#ifndef LBRDLLOBJECTPRIVATE_H
#define LBRDLLOBJECTPRIVATE_H

#include "lbrdllobject.h"
#include "lbrobjectinterfaceprivate.h"
#include <QLibrary>

typedef int (*tOpenLib)(Qt::HANDLE,const char*,int);
typedef int (*tResClose)(Qt::HANDLE);
typedef char*(*tResError)(int);

typedef int(*tFindResObject)(Qt::HANDLE, const char*, unsigned char);
typedef Qt::HANDLE(*tResGetCurPtr)(void);
typedef Qt::HANDLE(*tResGetCurHeader)(void);
typedef int(*tResGetElem)(Qt::HANDLE, Qt::HANDLE);
typedef int(*tResReadPrefix)(Qt::HANDLE, Qt::HANDLE);
typedef int(*tResNumObjects)(Qt::HANDLE);
typedef int(*tGetResObject)(Qt::HANDLE, const char*, unsigned char, void**, Qt::HANDLE);
typedef int(*tDeleteResObject)(Qt::HANDLE, const char*, unsigned char);
//int  GetResObject(HRSLRESFILE rf, const char *name, unsigned char type, void **data, RES_GET Get);

//int BuildDirectoryt(RLibDir *dir, HRSLRESFILE rf, RLibDirElemFiltr filt, int num, int cmtSize);
typedef int (*tBuildDirectory)(Qt::HANDLE, Qt::HANDLE, Qt::HANDLE, int, int);
typedef void (*tFreeDirectory)(Qt::HANDLE);

typedef int (*tLibOpenStream)(Qt::HANDLE, Qt::HANDLE, int, const char*, unsigned char, int, long);
typedef int (*tLibCloseStream)(Qt::HANDLE, bool);
typedef int (*tLibReadStream)(Qt::HANDLE, void*, int, int*);
typedef int (*tLibWriteStream)(Qt::HANDLE, void*, int, int *);

typedef int (*tResSetPosStream)(Qt::HANDLE, size_t);
typedef int (*tResGetSizeStream)(Qt::HANDLE, size_t*);
typedef int (*tResGetPosStream)(Qt::HANDLE, size_t*);
/*int  LibOpenStream (LibStream *strm, ResFile *rf, int mode, const char *name, unsigned char type,
                  int ver, long size)*/
class LbrResListModel;
class LbrDllObjectPrivate : public LbrObjectInterfacePrivate
{
    Q_DECLARE_PUBLIC(LbrDllObject);
public:
    LbrDllObjectPrivate(LbrObjectInterface *obj);
    virtual ~LbrDllObjectPrivate();

    bool isResExists(const QString &name, const int &type);

    template<class T> T resolve(const QString &funcname)
    {
        if (!m_lib.isLoaded())
            throw (QString("Can't resolve [%1]: rsrtlwm.dll not loaded").arg(funcname));

        T func = (T)m_lib.resolve(funcname.toLocal8Bit().data());
        if (!func)
            throw (QString("Can't resolve [%1]: %2").arg(funcname, m_lib.errorString()));

        return func;
    }

    bool loadLib();
    bool open(const QString &filename);
    bool initObjectList();
    void getResource(const QString &name, const int &type, ResBuffer **buffer);
    bool deleteResource(const QString &name, const int &type);

    bool beginSaveRes(const QString &name, const int &type, ResBuffer **buffer);
    bool endSaveRes(ResBuffer **buffer);
    QString resError(int stat);
    Qt::HANDLE findLibDirElem(const QString &name, const int &type);

    QLibrary m_lib;

    tOpenLib _OpenLib;
    tResClose _ResClose;
    tResError _ResError;

    tBuildDirectory _BuildDirectory;
    tFreeDirectory _FreeDirectory;

    tFindResObject _FindResObject;
    tGetResObject _GetResObject;
    tResGetCurPtr _ResGetCurPtr;
    tResGetCurHeader _ResGetCurHeader;
    tResGetElem _ResGetElem;
    tResReadPrefix _ResReadPrefix;
    tResNumObjects _tResNumObjects;
    tDeleteResObject _DeleteResObject;

    tLibOpenStream _LibOpenStream;
    tLibCloseStream _LibCloseStream;
    tLibReadStream _LibReadStream;
    tLibWriteStream _LibWriteStream;

    /*tResSetPosStream _ResSetPosStream;
    tResGetSizeStream _ResGetSizeStream;
    tResGetPosStream _ResGetPosStream;*/

    void *m_ResFile, *m_LibDir;
    QTextCodec *m_OemCodec;
    QScopedPointer<LbrResListModel> m_pDirModel;
};

#endif // LBRDLLOBJECTPRIVATE_H
