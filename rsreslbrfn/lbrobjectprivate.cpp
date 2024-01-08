//#include "res_lbr.h"
#include "lbrobjectprivate.h"
#include "lbrreslistmodel.h"
#include <QDebug>
#include <QTextCodec>

#define dHRES_NAME            9  // Длина наименования ресурса в LBR
#define dHRES_COMMENT       136  // Длина комментария к ресурсу (с 0-символом)
#define dHRES_ITEM_NAME     101  // Длина наименования пункта меню (с

#define MAKESIZE(rh) ((size_t)(((rh)->resSizeLo) | ((size_t)((rh)->resSizeHi)) << 16))
#define RHEADSIZE(rh) ((rh)->headVer == 0 ? sizeof(ResHeaderOld) : sizeof (ResHeader))
#define ELEM_PTR(dir, num) ((RLibDirElem *)((char *)((dir)->Elem) + ((dir)->cmtSize + sizeof(RLibDirElem)) * (num)))
#define CMT_PTR(elem) ((char *)((elem) + 1))

LbrObjectPrivate::LbrObjectPrivate() :
    m_Flags(0)
{
    m_p866 = QTextCodec::codecForName("IBM 866");
}

LbrObjectPrivate::~LbrObjectPrivate()
{

}

void LbrObjectPrivate::InitResModel()
{
    Q_Q(LbrObject);
    m_pDirModel.reset(new LbrResListModel());
}

int LbrObjectPrivate::LibCmp(LibElem *original, LibElem *rc)
{
    if((rc->type == 0 || rc->type == original->type) && strcmp(original->name, rc->name) == 0)
        return 0;

    return 1;
}

bool LbrObjectPrivate::Read(void *data, const qint64 &size)
{
    qint64 len = m_pFile->read((char*)data, size);
    return len == size;
}

/*void LbrObjectPrivate::LibDirCvt(ResFile *rf, HRSLCVT hcvt, LibElem *rc)
{
    RslCvtLong(hcvt, &rc->rc.offset);
    RslCvtString(hcvt, rc->name);
}*/

void LbrObjectPrivate::DecodDirectory(Resource *rc, const unsigned &num)
{
    unsigned char parol = 0x55;
    unsigned i;
    char *pch = (char*)rc;

    for(i = 0 ; i < num; i++, pch++)
        *pch ^= parol;
}

bool LbrObjectPrivate::ReadHeader(const QString &password)
{
    if (!Read(&m_Header))
        return false;

    if (password != m_Header.password)
        return false;

    return true;
}

void LbrObjectPrivate::FreeDirectory()
{
/*#ifdef RES_MODIFY
    if(root->flags & DIR_MODIFIED)
        stat = ResPutDirectory(root);

    if(root->freeblock)
    {
        free(root->freeblock);

        root->freeblock = NULL;
    }
#endif*/
    m_Elements.clear();
    m_pDirectory.reset();
    m_pFreeBlock.reset();
}

bool LbrObjectPrivate::GetDirectory()
{
    bool stat = true;
    FreeDirectory();

    if (m_Header.NumElem)
    {
        if ((stat = m_pFile->seek(m_Header.DataOffset)))
        {
            unsigned size = m_Header.ElemSize * m_Header.NumElem;
            m_pDirectory.reset(reinterpret_cast<Resource*>(malloc(size)));

            if ((stat = Read(m_pDirectory.data(), size)))
            {
                DecodDirectory(m_pDirectory.data(), size);

                size = m_Header.NumFree * sizeof(FreeBlock);
                m_pFreeBlock.reset(reinterpret_cast<FreeBlock*>(malloc(size)));
                stat = Read(m_pFreeBlock.data(), size);
            }
        }
    }

    if (!stat)
        FreeDirectory();

    return stat;
}

RLibDirElem *LbrObjectPrivate::GetElement(const QString &name, const int &type)
{
    RLibDirElem *rc = nullptr;

    for (RLibDirElem *item : qAsConst(m_Elements))
    {
        if (item->type == type && !name.compare(item->name, Qt::CaseInsensitive))
        {
            rc = item;
            break;
        }
    }

    return rc;
}

int LbrObjectPrivate::CompareRes(LibElem *rc, const QString &name, const int &type)
{
    return !(rc->type == type && !name.compare(rc->name, Qt::CaseInsensitive));
}

LibElem *LbrObjectPrivate::GetResource(const QString &name, const int &type)
{
    LibElem *rc = nullptr;

    ResIter iter;
    InitResIter(&iter);
    while((rc = (LibElem*)ResGetIter(&iter)) && CompareRes(rc, name, type))
        ;

    return rc;
}

QByteArray LbrObjectPrivate::GetResourceData(const QString &name, const int &type, ResHeader *header, QString *comment)
{
    QByteArray resdata;

    LibElem *rc = GetResource(name, type);
    if (rc)
    {
        if (ResReadPrefix((Resource*)rc, header))
        {
            resdata = m_pFile->read(header->resSizeLo);

            if (header->ver > 0 && comment)
            {
                long delta = sizeof(quint16);
                m_pFile->seek(rc->rc.offset + sizeof(ResHeader) - delta);
                unsigned char len = 0;

                if (Read(&len) && len)
                {
                    len = dHRES_COMMENT < len ? dHRES_COMMENT : len;

                    char *commentstr = (char*)malloc(len + 1);
                    memset(commentstr, 0, len + 1);
                    Read(commentstr, len);
                    *comment = m_p866->toUnicode(commentstr);

                    free(commentstr);
                }
                /*else
                    comment[0] = '\0';*/
            }
        }
    }

    return resdata;
}

bool LbrObjectPrivate::Open(const QString &filename, const QString &password)
{
    bool stat = 0;
    m_pFile.reset(new QFile(filename));
    InitResModel();

    if ((stat = m_pFile->open(QIODevice::ReadWrite)))
    {
        stat = ReadHeader(password);

        if (stat)
            stat = GetDirectory();
    }

    if (stat)
        FillResoursesList();

    return stat;
}

void LbrObjectPrivate::InitResIter(ResIter *iter)
{
    iter->limit   = m_Header.NumElem;
    iter->ptr     = (char *)m_pDirectory.data();
    iter->size    = m_Header.ElemSize;
    iter->current = 0;
}

Resource *LbrObjectPrivate::ResGetIter(ResIter *iter)
{
    Resource  *rc = (Resource *)iter->ptr;

    if(iter->current >= iter->limit)
        return NULL;

    iter->current++;
    iter->ptr += iter->size;

    return rc;
}

bool LbrObjectPrivate::ResReadPrefix(Resource *rc, ResHeader *rhead)
{
    if (!m_pFile->seek(rc->offset))
        return false;

    if (!Read(rhead))
        return false;

    if(rhead->headVer == 0)
    {
        long delta = sizeof(quint16);
        rhead->resSizeHi = 0;

        qint64 pos = m_pFile->pos();
        if (!m_pFile->seek(pos - delta))
            return false;
    }

    return true;
}

void LbrObjectPrivate::FillResoursesList()
{
    RLibDir dir;
    memset(&dir, 0, sizeof(RLibDir));
    dir.NumElem = dir.alloced = 0;
    dir.sortby = RS_NAME;

    int num = m_Header.NumElem;
    if (!num)
        return;

    int size = (sizeof(RLibDirElem) + dHRES_COMMENT) * num;
    RLibDirElem *element = reinterpret_cast<RLibDirElem*>(malloc(size));

    dir.Elem = element;
    dir.alloced = num;
    dir.cmtSize = dHRES_COMMENT;

    ResIter iter;
    InitResIter(&iter);

    Resource *rc = nullptr;
    while ((rc = ResGetIter(&iter)) && num--)
    {
        LibElem *libelement = reinterpret_cast<LibElem*>(rc);

        ResHeader rhead;
        if (ResReadPrefix(rc, &rhead))
        {
            strcpy(element->name, libelement->name);

            element->type  = libelement->type;
            element->size  = (long)MAKESIZE(&rhead); //hd->resSize;
            element->ftime = rhead.ftime;
            element->flags = rhead.headVer;
            element->ver   = rhead.ver;

            char *comment = ((char *)((element) + 1));
            if (dir.cmtSize)
            {
                if(rhead.ver > 0)
                {
                    unsigned char len = 0;
                    if (Read(&len) && len)
                    {
                        len = dir.cmtSize < len ? dir.cmtSize : len;
                        Read(comment, len);
                    }
                    else
                        comment[0] = '\0';
                }
                else
                    strncpy(CMT_PTR(element), "*** Old Type Resource", dir.cmtSize < 22 ? dir.cmtSize : 22);

                comment[dir.cmtSize - 1] = '\0';
            }

            m_pDirModel->addDirElement(element);
            m_Elements.append(element);

            element = reinterpret_cast<RLibDirElem*>(((char*)(element + 1)) + dir.cmtSize);
            dir.NumElem++;
        }
    }
}

bool LbrObjectPrivate::Create(const QString &filename, const QString &password)
{
    QFile lbr(filename);

    if (!lbr.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;

    Header header;
    memset(&header, 0, sizeof(Header));

    header.DataOffset = sizeof(Header);
    header.ElemSize = sizeof(LibElem);
    strcpy(header.password, password.toLocal8Bit().data());

    return Write(&header, &lbr);
}

int LbrObjectPrivate::ResDelObject(Resource *res)
{
    int stat = 0, ind = 0;
    LibElem *resElem = (LibElem*)res;

    ResHeader rhead;
    LibElem *rc;

    ResIter iter;
    InitResIter(&iter);

    while((rc = (LibElem*)ResGetIter(&iter)) != NULL && CompareRes(rc, resElem->name, resElem->type) != 0)
        ind++;

    if (rc)
    {
        size_t sz = m_Header.ElemSize;

        if(ResReadPrefix((Resource*)rc, &rhead))
        {
            FreeBlock fb, *pfb, *p;
            int i, iscombined = 0;

            res->offset = fb.offset = rc->rc.offset;
            fb.size = (long)(MAKESIZE(&rhead) + RHEADSIZE(&rhead));

            memmove(rc, (char *)rc + sz, sz*(m_Header.NumElem - ind - 1));

            m_Header.NumElem--;
            m_Flags |= DIR_MODIFIED;

            for(i = 0, pfb = m_pFreeBlock.get(); i < (int)m_Header.NumFree; i++, pfb++)
            {
                if(fb.offset < pfb->offset)
                    break;
            }

            if(i > 0)  // Есть предыдущий блок
            {
                p = m_pFreeBlock.get() + i - 1;

                if(p->offset + p->size == fb.offset)
                {
                    p->size += fb.size;

                    fb.offset = p->offset;
                    fb.size   = p->size;

                    iscombined = 1;
                }
            }

            if(i < (int)m_Header.NumFree) // Есть следующий блок
            {
                p = m_pFreeBlock.get() + i;

                if(fb.offset + fb.size == p->offset)
                {
                    p->size   += fb.size;
                    p->offset = fb.offset;

                    if(iscombined)
                        DelFreeBlock(i - 1);

                    iscombined = 1;
                }
            }
            else
            {
                if(fb.offset + fb.size >= m_Header.DataOffset)
                {
                    m_Header.DataOffset = fb.offset;

                    if(iscombined)
                        DelFreeBlock(i - 1);

                    iscombined = 1;
                }
            }

            if(!iscombined)
                stat = AddFreeBlock(i, &fb);

            if (!stat)
                stat = ResFlush();
        }
        else
            stat = 1;
    }

    return stat;
}

int LbrObjectPrivate::ResFlush()
{
    int stat = 0;

    if (m_Flags & DIR_MODIFIED)
        stat = ResPutDirectory();

    return stat;
}

int LbrObjectPrivate::ResPutDirectory()
{
    int stat = 0;

    DecodDirectory(m_pDirectory.get(), m_Header.NumElem * m_Header.ElemSize);

    if (m_pFile->seek(m_Header.DataOffset))
    {
        qint64 sz = m_Header.ElemSize * m_Header.NumElem;
        if (m_pFile->write((const char*)m_pDirectory.get(), sz) == sz)
        {
            sz = m_Header.NumFree * sizeof(FreeBlock);
            if (m_pFile->write((const char*)m_pFreeBlock.get(), sz) == sz)
            {
                qint64 pos = m_pFile->pos();
                stat = !m_pFile->resize(pos);

                WriteFileHeader();
            }
        }
        else
            stat = 1;
    }
    else
        stat = 1;

    DecodDirectory(m_pDirectory.get(), m_Header.NumElem * m_Header.ElemSize);

    return stat;
}

void LbrObjectPrivate::WriteFileHeader()
{
    m_pFile->seek(0);
    Write(&m_Header);

    m_Flags &= ~DIR_MODIFIED;
}

void LbrObjectPrivate::DelFreeBlock(int ind)
{
    FreeBlock  *fb = m_pFreeBlock.get() + ind;
    memmove(fb, fb + 1, (m_Header.NumFree - ind - 1) * sizeof(FreeBlock));

    m_Header.NumFree--;
}

int LbrObjectPrivate::AddFreeBlock(int ind, FreeBlock *block)
{
    int stat = L_ADDMEM;
    FreeBlock *fb;
    void *ptr = realloc(m_pFreeBlock.get(), (m_Header.NumFree + 1) * sizeof(FreeBlock));

    if(ptr)
    {
        m_pFreeBlock.reset((FreeBlock*)ptr);

        fb = m_pFreeBlock.get() + ind;
        memmove(fb + 1, fb, (m_Header.NumFree - ind) * sizeof(FreeBlock));

        *fb = *block;
        m_Header.NumFree++;
        stat = 0;
    }

    return stat;
}

void LbrObjectPrivate::ResSetCurTime(ResHeader *rhead)
{
    struct ftime *ft = &rhead->ftime;
    time_t aclock;
    struct tm *t;

    time(&aclock);

    t = localtime(&aclock);

    ft->ft_year  = (short)t->tm_year + 1900 - 1980;
    ft->ft_month = t->tm_mon + 1;
    ft->ft_day   = t->tm_mday;

    ft->ft_hour  = t->tm_hour;
    ft->ft_min   = t->tm_min;
    ft->ft_tsec  = t->tm_sec/2;
}
