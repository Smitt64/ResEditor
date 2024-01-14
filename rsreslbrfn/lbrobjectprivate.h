#ifndef LBROBJECTPRIVATE_H
#define LBROBJECTPRIVATE_H
//#include "res_lbr.h"
#include "lbrobject.h"
#include "lbrobjectinterfaceprivate.h"
#include "LbrObjectHeaders.h"
#include <QFile>

class LbrResListModel;
class LbrObjectPrivate : public LbrObjectInterfacePrivate
{
    Q_DECLARE_PUBLIC(LbrObject);

public:
    enum DirSort
    {
        RS_NAME,
        RS_TYPE,
        RS_DATE,
        RS_COMM
    };

    LbrObjectPrivate(LbrObjectInterface *obj);
    virtual ~LbrObjectPrivate();

    static int LibCmp(LibElem *original, LibElem *rc);
    //static void LibDirCvt(ResFile *rf, HRSLCVT hcvt, LibElem *rc);

    bool Open(const QString &filename, const QString &password = QString("RESLIB"));
    bool Create(const QString &filename, const QString &password = QString("RESLIB"));

    template<class T>bool Read(T *data)
    {
        qint64 len = m_pFile->read((char*)data, sizeof(T));
        return len == sizeof(T);
    }

    template<class T>bool Write(T *data, QFile *f = nullptr)
    {
        QFile *file = f;
        if (!file)
            file = m_pFile.data();

        qint64 len = file->write((char*)data, sizeof(T));
        return len == sizeof(T);
    }

    bool Read(void *data, const qint64 &size);
    void InitResModel();

    RLibDirElem *GetElement(const QString &name, const int &type);
    LibElem *GetResource(const QString &name, const int &type);
    int CompareRes(LibElem *rc, const QString &name, const int &type);
    QByteArray GetResourceData(const QString &name, const int &type, ResHeader *header, QString *comment = nullptr);

    int ResDelObject(Resource *res);
    int ResFlush();

private:
    //  Механизм итерации по каталогу
    typedef struct tagResIter
    {
        unsigned limit;
        unsigned size;
        char     *ptr;
        unsigned current;
    } ResIter;

    typedef struct
    {
        RLibDirElem  *Elem;
        int           NumElem;   // Количество заполненных элементов в списке
        int           alloced;   // Количество элементов под которое распределена память
        DirSort       sortby;    // Порядок сортировки
        int           cmtSize;
    } RLibDir;

    bool ReadHeader(const QString &password);
    bool GetDirectory();
    void FreeDirectory();
    void DecodDirectory(Resource *rc, const unsigned &num);

    void FillResoursesList();
    bool ResReadPrefix(Resource *rc, ResHeader *rhead);

    int ResPutDirectory();
    void ResSetCurTime(ResHeader *rhead);
    void WriteFileHeader();

    void InitResIter(ResIter *iter);
    Resource *ResGetIter(ResIter *iter);
    void DelFreeBlock(int ind);
    int AddFreeBlock(int ind, FreeBlock *block);
    Header m_Header;
    quint32 m_Flags;

    QScopedPointer<Resource, QScopedPointerPodDeleter> m_pDirectory;
    QScopedPointer<FreeBlock, QScopedPointerPodDeleter> m_pFreeBlock;
    QScopedPointer<QFile> m_pFile;
    QScopedPointer<LbrResListModel> m_pDirModel;

    QVector<RLibDirElem*> m_Elements;
};

#endif // LBROBJECTPRIVATE_H
