#ifndef LBROBJECTPRIVATE_H
#define LBROBJECTPRIVATE_H

#include "lbrobject.h"
//#include "res_lbr.h"
#include "LbrObjectHeaders.h"
#include <QFile>

class LbrResListModel;
class LbrObjectPrivate
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

    LbrObjectPrivate();
    virtual ~LbrObjectPrivate();

    LbrObject * q_ptr;

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

    void InitResIter(ResIter *iter);
    Resource *ResGetIter(ResIter *iter);
    Header m_Header;

    QScopedPointer<Resource, QScopedPointerPodDeleter> m_pDirectory;
    QScopedPointer<FreeBlock, QScopedPointerPodDeleter> m_pFreeBlock;
    QScopedPointer<QFile> m_pFile;
    QScopedPointer<LbrResListModel> m_pDirModel;

    QVector<RLibDirElem*> m_Elements;

    QTextCodec *m_p866;
};

#endif // LBROBJECTPRIVATE_H
