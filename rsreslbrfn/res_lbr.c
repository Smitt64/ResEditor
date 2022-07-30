#include <sys/stat.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <windows.h>
#include "platform.h"

#include "unixio.h"

#include <time.h>

#include "res_lbr.h"
#include "rstring.h"

#include "unifile.hpp"
#include "panel.h"
#include "rsconfig.h"

#pragma warning(disable: 4390)  // empty controlled statement found; is this the intent?

char RSL_FAR _PANEL_STR_RES_NAME[] = "RЕSLIВ";

const char RSL_FAR *_PANEL_STR_RES_MSG[] = {
   "Can not open linked resourse",
   "Can not open resource library",
   "Bad resourse file format",
   "Can not read directory - file is closed",
   "Can not read directory - no more memory",
   "Can not reading file",
   "Can not write file",
   "Directory is empty",
   "Missing directory entry",
   "Duplicate object in library",
   "Can not position in library",
   "Can not add object in library - out of memory",
   "Can not read resource",
   "Can not write resource",
   "Bad written resource size",
   "Bad resource version",
   "Transaction already active",
   "Error in transaction",
   "Sharing violation",
   "Bad open mode"
} ;
/*
  RES_MODIFY   - если определена позволяет обновлять файл ресурсов
*/
#define  RES_MODIFY  1

// Static DATA!! -------------------------------------------------
// Used only by old functions ResGetObject and ResPutObject
static ResHeader    ResHead;
static Resource     *Res;
static ResFile      *CurRf;

// -----------------------------------------------------------------------------
int   ResAbortTrn  (ResFile *rt);
char *ResMakeBkName(ResFile *rf, char *buff);

// -----------------------------------------------------------------------------
#ifdef RSL_MD_FLAT
#define  RLOCAL
#define  RFAR
#else
#define  RLOCAL  _near _pascal
#define  RFAR    _far
#endif

// -----------------------------------------------------------------------------
#ifdef RSL_PL_MS

#include "packpsh1.h"
static struct
{
    char Parol[23];
    long LibBase;
} LibLink = { "", 0L };

// Bind im main.c now
// LibLink = { "Linked resource system", 0L };

#include "packpop.h"

// -----------------------------------------------------------------------------
void SetResLibBaseAddr(long addr)
{
    LibLink.LibBase = addr;
}
#endif

// -----------------------------------------------------------------------------
int commit(int hd)
{
    int  dupHd = dup(hd);

    return close(dupHd);
}

// -----------------------------------------------------------------------------
void ResSetIter(ResIter *iter, ResFile *rf)
{
    iter->limit   = rf->head.NumElem;
    iter->ptr     = (char *)rf->Directory;
    iter->size    = rf->head.ElemSize;
    iter->current = 0;
}

const char *progPathEx(bool isAnsi)
{
    static char  nameBufAnsi[_MAX_PATH];
    static char  nameBufOem[_MAX_PATH];

    // Using static doesn't matter even if using in multithread environment because
    // (possible) simultameous initial calls will write the same value!
    if(!*nameBufAnsi)
    {
        GetModuleFileNameA(NULL, nameBufAnsi, sizeof(nameBufAnsi));

#ifdef USE_UFILE
        RslToOEM(nameBufOem, nameBufAnsi);
#endif
    }

    return isAnsi ? nameBufAnsi : nameBufOem;
}

const char *progPath(void)
{
#ifdef USE_UFILE
    if(IsRsLocaleLoaded && RslDefProgEncode != RSL_CP_RSANSI && RslDefProgEncode != RSL_CP_LCANSI)
        return progPathEx(false);
#endif

    return progPathEx(true);
}

// -----------------------------------------------------------------------------
Resource *ResGetIter(ResIter *iter)
{
    Resource  *rc = (Resource *)iter->ptr;


    if(iter->current >= iter->limit)
        return NULL;

    iter->current++;
    iter->ptr += iter->size;

    return rc;
}

// -----------------------------------------------------------------------------
int ResFreeDirectory(ResFile *root)
{
    int  stat = 0;

#ifdef RES_MODIFY
    if(root->flags & DIR_MODIFIED)
        stat = ResPutDirectory(root);

    if(root->freeblock)
    {
        free(root->freeblock);

        root->freeblock = NULL;
    }
#endif

    if(root->Directory)
    {
        free(root->Directory);

        root->Directory = NULL;
    }

    root->flags &= ~(DIR_READ | FREE_READ);

    return stat;
}

// -----------------------------------------------------------------------------
static void RLOCAL DecodDirectory(Resource *rc, unsigned num)
{
    unsigned char  parol = 0x55;
    unsigned       i;
    char          *pch = (char *)rc;


    for(i = 0 ; i < num; i++, pch++)
        *pch ^= parol;
}

// -----------------------------------------------------------------------------
static int RLOCAL ResGetFileHeader(ResFile *rf, const char *password)
{
    if(lseek(rf->hd, rf->base, SEEK_SET) != -1L)
        if(read(rf->hd, &rf->head, sizeof(Header)) != -1)
        {
            RslCvtString(rf->hcvtRd, rf->head.password);
            RslCvtLong  (rf->hcvtRd, &rf->head.DataOffset);
            RslCvtUShort(rf->hcvtRd, &rf->head.NumElem);
            RslCvtUShort(rf->hcvtRd, &rf->head.ElemSize);
            RslCvtUShort(rf->hcvtRd, &rf->head.NumFree);

            if(strcmp(rf->head.password, password) == 0)
                return 0;
            else
                return L_LIBTYPE;
        }

    return L_DIRREAD;
}

// -----------------------------------------------------------------------------
static void CvtDirectory(ResFile *rt, int write)
{
    HRSLCVT  hcvt = write ? rt->hcvtWr : rt->hcvtRd;


    if(rt->cvtDirProc && hcvt)
    {
        ResIter    iter;
        Resource  *rc;


        ResSetIter(&iter, rt);

        while((rc = ResGetIter(&iter)) != NULL)
            rt->cvtDirProc(rt, hcvt, rc);
    }
}

// -----------------------------------------------------------------------------
int ResGetDirectory(ResFile *rt)
{
    int  stat = 0;


    if(rt->hd < 0)
        return L_DIRNOPEN;

    ResFreeDirectory(rt);

    if(rt->head.NumElem)
        if(lseek(rt->hd, rt->head.DataOffset + rt->base, SEEK_SET) != -1L)
        {
            unsigned  size = rt->head.ElemSize*rt->head.NumElem;


            if((rt->Directory = (Resource *)malloc(size)) != NULL)
                if(read(rt->hd, rt->Directory, size) != -1)
                {
                    DecodDirectory(rt->Directory, rt->head.NumElem*rt->head.ElemSize);

                    CvtDirectory(rt, 0);

                    rt->flags |= DIR_READ;

#ifdef RES_MODIFY
                    size = rt->head.NumFree*sizeof(FreeBlock);

                    if(rt->head.NumFree)
                        if((rt->freeblock = (FreeBlock *)malloc(size)) != NULL && read(rt->hd, rt->freeblock, size) != -1)
                            rt->flags |= FREE_READ;
                        else
                            stat = L_DIRREAD;
#endif
                }
                else
                    stat = L_DIRREAD;
            else
                stat = L_DIRNOMEM;
        }
        else
            stat = L_DIRREAD;

    if(stat)
        ResFreeDirectory(rt);

    return stat;
}

// -----------------------------------------------------------------------------
#ifdef RES_MODIFY
// -----------------------------------------------------------------------------
static int RLOCAL ResPutFileHeader(ResFile *rf)
{
    Header  temp = rf->head;


    RslCvtString(rf->hcvtWr, temp.password);
    RslCvtLong  (rf->hcvtWr, &temp.DataOffset);
    RslCvtUShort(rf->hcvtWr, &temp.NumElem);
    RslCvtUShort(rf->hcvtWr, &temp.ElemSize);
    RslCvtUShort(rf->hcvtWr, &temp.NumFree);

    if(lseek(rf->hd, rf->base, SEEK_SET) != -1L)
        if(write(rf->hd, &temp, sizeof(Header)) != -1)
            return 0;

    return L_DIRWR;
}

// -----------------------------------------------------------------------------
int ResPutDirectory(ResFile *rt)
{
    int  stat;


    CvtDirectory(rt, 1);

    DecodDirectory(rt->Directory, rt->head.NumElem*rt->head.ElemSize);

    if(lseek(rt->hd, rt->head.DataOffset + rt->base, SEEK_SET) != -1L)
        if(write(rt->hd, rt->Directory, rt->head.ElemSize*rt->head.NumElem) != -1)
            if(write(rt->hd, rt->freeblock, rt->head.NumFree*sizeof(FreeBlock)) != -1)
            {
                ftruncate(rt->hd, tell(rt->hd));

                if((stat = ResPutFileHeader(rt)) == 0)
                    rt->flags &= ~DIR_MODIFIED;
            }
            else
                stat = L_DIRWR;
        else
            stat = L_DIRWR;
    else
        stat = L_DIRWR;

    DecodDirectory(rt->Directory, rt->head.NumElem*rt->head.ElemSize);

    CvtDirectory(rt, 0);

    return stat;
}

// -----------------------------------------------------------------------------
int ResFlush(ResFile *rf)
{
    int  stat = 0;


    if(rf->flags & DIR_MODIFIED)
    {
        stat = ResPutDirectory(rf);

        if(!stat)
            commit(rf->hd);
    }

    return stat;
}

// -----------------------------------------------------------------------------
static void RLOCAL CopyRC_To_Array(ResFile *rf, Resource *array, unsigned ind, Resource *rc)
{
    unsigned  size = rf->head.ElemSize;

    memmove((char *)array + size*ind, rc, size);
}

/*
    static void RLOCAL CopyRC_From_Array ( ResFile *rf,Resource *array,int ind,Resource *rc)
    {
       int size = rf->head.ElemSize;
       movmem ((char*)array+ size*ind,rc,size);
    }
    */

// -----------------------------------------------------------------------------
int ResAddElem(ResFile *rf, Resource *rc)
{
    int    stat = 0;
    void  *ptr;


    //  Каталог должен быть прочитан !
    ptr = realloc(rf->Directory, rf->head.ElemSize*(rf->head.NumElem + 1));

    if(ptr)
    {
        rf->Directory = (Resource *)ptr;

        CopyRC_To_Array(rf, rf->Directory, rf->head.NumElem, rc);

        rf->head.NumElem++;
        rf->flags |= DIR_MODIFIED;
    }
    else
        stat = L_ADDMEM;

    return stat;
}

// -----------------------------------------------------------------------------
static int ResReplaceElem(ResFile *rt, Resource *res, Resource *newRc)
{
    int       need_close = 0, stat;
    ResIter   iter;
    Resource  *rc;


    if((rt->flags & DIR_READ) && !rt->Directory)
        return L_DIREMPTY;

    if(!(rt->flags & DIR_READ))
    {
        if((stat = ResGetDirectory(rt)) != 0)
            return stat;

        need_close = 1;
    }

    stat = L_DIRMISS;

    ResSetIter(&iter, rt);

    while((rc = ResGetIter(&iter)) != NULL && (*rt->ResCmp)(rc, res) != 0)
        ;

    if(rc)
    {
        Resource  save;


        memmove(&save, rc, sizeof(Resource));
        memmove(rc, newRc, rt->head.ElemSize);
        memmove(rc, &save, sizeof(Resource));

        rt->flags |= DIR_MODIFIED;

        if(rt->flags & NEED_FLUSH)
            stat = ResFlush(rt);
        else
            stat = 0;
    }

    if(need_close)
        ResFreeDirectory(rt);

    return stat;
}
#endif

// -----------------------------------------------------------------------------
int ResGetElem(ResFile *rt, Resource *res)
{
    int        need_close = 0, stat;
    ResIter    iter;
    Resource  *rc;


    if((rt->flags & DIR_READ) && !rt->Directory)
        return L_DIREMPTY;

    if(!(rt->flags & DIR_READ))
    {
        if((stat = ResGetDirectory(rt)) != 0)
            return stat;

        need_close = 1;
    }

    stat = L_DIRMISS;

    ResSetIter(&iter, rt);

    while((rc = ResGetIter(&iter)) != NULL && (*rt->ResCmp)(rc, res) != 0)
        ;

    if(rc)
    {
        memmove(res, rc, rt->head.ElemSize);

        res->offset += rt->base;

        stat = 0;
    }

    if(need_close)
        ResFreeDirectory(rt);

    return stat;
}

// -----------------------------------------------------------------------------
int ResClose(ResFile *rt)
{
    int  stat;


    if(rt->hd < 0)
        return 0;

    stat = ResFreeDirectory(rt);

    close(rt->hd);

    rt->hd = -1;

    if(rt->fileName)
        free(rt->fileName);

    if(rt->hcvtRd) RslCloseCvt(rt->hcvtRd);
    if(rt->hcvtWr) RslCloseCvt(rt->hcvtWr);

    return stat;
}

// -----------------------------------------------------------------------------
static void RLOCAL ResClear(ResFile *rf)
{
    memset(rf, 0, sizeof(ResFile));

    rf->hd = -1;
}

// -----------------------------------------------------------------------------
int ResOpen(ResFile *rt, const char *lib, const char *password, RES_CMP ResCmp, RES_CVT resCvt, int flags, ...)
//  lib - full path to library
//  Дополнительный параметр - ElemSize
{
#ifdef RES_MODIFY
    char  buff[_MAX_PATH];
#endif

    int   stat = 0;
    int   was_create = 0;
    int   mode  = O_RDONLY | O_BINARY;
    int   share = SH_DENYNO;


    ResClear(rt);

    rt->ResCmp     = ResCmp;
    rt->cvtDirProc = resCvt;

    // Now simply convert from 866 to current
    rt->hcvtRd = RslOpenCvt(RSL_DEF_CODEPAGE, 0);
    rt->hcvtWr = RslOpenCvt(0, RSL_DEF_CODEPAGE);

#ifdef RES_MODIFY
    if(flags & RO_CREATE)
        flags |= RO_MODIFY;

    if(flags & RO_MODIFY)
    {
        flags |= RO_READDIR;
        mode  = O_RDWR | O_BINARY;
        share = SH_DENYWR;
    }
#endif

#ifdef RSL_PL_MS
    if((flags & RO_USELINKED) && LibLink.LibBase)
        if((rt->hd = sopen_rs(progPath(), O_RDONLY | O_BINARY, SH_DENYNO, 0)) ==  -1)
            stat = L_OPENLINK;
        else
            rt->base = LibLink.LibBase;
    else
#endif
    {
        if(lib == NULL)
            return 0;

        if(!access_rs(lib, 0) && !(flags & RO_NEW))  // Exist
        {
            if((rt->hd = sopen_rs(lib, mode, share, 0)) ==  -1)
            {
                if(errno == EACCES)
                    stat = L_OPENFORWR;
                else
                    stat = L_OPENLIB;
            }
        }
#ifdef RES_MODIFY
        else if(flags & RO_CREATE)
        {
            va_list  ap;


            if((rt->hd = sopen_rs(lib, mode | O_CREAT | O_TRUNC, share, S_IREAD | S_IWRITE)) == -1)
            {
                if(errno == EACCES)
                    stat = L_OPENFORWR;
                else
                    stat = L_OPENLIB;
            }
            else
            {
                strcpy(rt->head.password, password);

                rt->head.DataOffset = sizeof(Header);
                rt->head.NumElem    = 0;
                rt->Directory       = NULL;
                rt->flags           |= DIR_READ;

                va_start(ap, flags);
                rt->head.ElemSize = (short int)(va_arg(ap, int));
                va_end(ap);

                if((stat = ResPutFileHeader(rt)) == 0)
                    was_create = 1;
            }
        }
#endif
        else
            stat = L_OPENLIB;
    }

    if(!stat && !was_create && (stat = ResGetFileHeader(rt, password)) == 0 && (flags & RO_READDIR))
    {
        stat = ResGetDirectory(rt);

        rt->flags |= DIR_READ;
    }


#ifdef RES_MODIFY
    if(!stat && (rt->fileName = (char *)malloc(strlen(lib) + 1)) != NULL)
        strcpy(rt->fileName, lib);

    if(!stat && rt->head.isTrn && access_rs(ResMakeBkName(rt, buff), 0) == 0)
        stat = ResAbortTrn(rt);
#endif

    if(flags & RO_TRN)
    {
        rt->flags |= USE_TRN;

        flags |= RO_FLUSH;
    }

    if(flags & RO_FLUSH)
        rt->flags |= NEED_FLUSH;

    if(!(flags & RO_MODIFY))
        rt->flags |= READ_MODE;

    if(stat)
        ResClose(rt);

    return stat;
}

/*************************************************************************/
/*************************************************************************/

// -----------------------------------------------------------------------------
ResHeader *ResGetCurHeader(void)
{
    return &ResHead;
}

// -----------------------------------------------------------------------------
Resource *ResGetCurPtr(void)
{
    return Res;
}

// -----------------------------------------------------------------------------
Resource *ResSetCurPtr(Resource *rc)
{
    Resource  *res = Res;


    if(rc)
        Res = rc;

    return res;
}

// -----------------------------------------------------------------------------
ResFile *ResFileGetCurPtr(void)
{
    return CurRf;
}

// -----------------------------------------------------------------------------
ResFile *ResFileSetCurPtr(ResFile *rf)
{
    ResFile  *cur = CurRf;


    if(rf)
        CurRf = rf;

    return cur;
}

// -----------------------------------------------------------------------------
#ifdef RES_MODIFY
// -----------------------------------------------------------------------------
void ResSetCurTime(ResHeader *rhead)
{
    struct ftime  *ft = &rhead->ftime;
    time_t         aclock;
    struct tm     *t;


    time(&aclock);

    t = localtime(&aclock);

    ft->ft_year  = (short)t->tm_year + 1900 - 1980;
    ft->ft_month = t->tm_mon + 1;
    ft->ft_day   = t->tm_mday;

    ft->ft_hour  = t->tm_hour;
    ft->ft_min   = t->tm_min;
    ft->ft_tsec  = t->tm_sec/2;
}

// -----------------------------------------------------------------------------
void ResSetCurHeader(void)
{
    ResSetCurTime(&ResHead);
}
#endif

// -----------------------------------------------------------------------------
int ResReadPrefixEx(ResFile *rf, Resource *rc, ResHeader *rhead)
{
    if(lseek(rf->hd, rc->offset, SEEK_SET) == -1L)
        return L_POSLIB;

    if(read(rf->hd, rhead, sizeof(ResHeader)) == -1)
        return L_RESGET;

    if(rhead->headVer == 0)
    {
        long  delta = sizeof(db_uint16);


        rhead->resSizeHi = 0;

        if(lseek(rf->hd, -delta, SEEK_CUR) == -1L)
            return L_POSLIB;
    }

    RslCvtUShort(rf->hcvtRd, &rhead->resSizeLo);
    RslCvtUShort(rf->hcvtRd, &rhead->resSizeHi);

    return 0;
}

// -----------------------------------------------------------------------------
int ResReadPrefix(ResFile *rf, Resource *rc)
{
    return ResReadPrefixEx(rf, rc, &ResHead);
}

// -----------------------------------------------------------------------------
#ifdef RES_MODIFY
// -----------------------------------------------------------------------------
int ResWritePrefixEx(ResFile *rf, Resource *rc, ResHeader *rhead)
{
    ResHeader  temp = *rhead;


    RslCvtUShort(rf->hcvtWr, &temp.resSizeLo);
    RslCvtUShort(rf->hcvtWr, &temp.resSizeHi);

    if(lseek(rf->hd, rc->offset, SEEK_SET) == -1L)
        return L_POSLIB;

    if(write(rf->hd, &temp, RHEADSIZE(&temp)) == -1)
        return L_RESPUT;

    return 0;
}

// -----------------------------------------------------------------------------
int ResWritePrefix(ResFile *rf, Resource *rc)
{
    return ResWritePrefixEx(rf, rc, &ResHead);
}

// -----------------------------------------------------------------------------
static void RLOCAL DelFreeBlock(ResFile *rf, int ind)
{
    FreeBlock  *fb = rf->freeblock + ind;


    memmove(fb, fb + 1, (rf->head.NumFree - ind - 1)*sizeof(FreeBlock));

    rf->head.NumFree--;
}

// -----------------------------------------------------------------------------
static int RLOCAL AddFreeBlock(ResFile *rf, int ind, FreeBlock *block)
{
    int         stat = L_ADDMEM;
    FreeBlock  *fb;
    void       *ptr  = realloc(rf->freeblock, (rf->head.NumFree + 1)*sizeof(FreeBlock));


    if(ptr)
    {
        rf->freeblock = (FreeBlock *)ptr;

        fb = rf->freeblock + ind;

        memmove(fb + 1, fb, (rf->head.NumFree - ind)*sizeof(FreeBlock));

        *fb = *block;

        rf->head.NumFree++;

        stat = 0;
    }

    return stat;
}

// -----------------------------------------------------------------------------
static long RLOCAL ResFindOffset(ResFile *rf, long size)
{
    FreeBlock  *fb;
    int         i;


    for(i = 0, fb = rf->freeblock; i < (int)rf->head.NumFree; i++, fb++)
        if(fb->size >= size)
        {
            if(fb->size == size)
            {
                long  offset = fb->offset;

                DelFreeBlock(rf, i);

                return offset;
            }

            fb->size -= size;

            return fb->offset + fb->size;
        }

    return rf->head.DataOffset;
}

// -----------------------------------------------------------------------------
int ResPutObject(ResFile *rf, Resource *rc, void *data, RES_PUT Put, long size)
{
    long    offs;
    int     stat;
    size_t  resSize;


    if((stat = ResGetElem(rf, rc)) == 0)
        return L_DUPOB;
    else if(!(stat == L_DIRMISS || stat == L_DIREMPTY))
        return stat;

    ResSetCurHeader();

    ResHead.ver = 0;
    ResHead.headVer = (size == -1 || size < 0xffff) ? 0 : 1;

    // June 28,1994
    rc->offset = size == -1L ? rf->head.DataOffset : ResFindOffset(rf, size + RHEADSIZE(&ResHead));

    if(lseek(rf->hd, rc->offset + RHEADSIZE(&ResHead), SEEK_SET) == -1L)
        return L_POSLIB;

    Res   = rc;
    CurRf = rf;

    if((stat = (*Put)(rf, data)) != 0)
        return stat;

    if((offs = tell(rf->hd)) == -1L)
        return L_POSLIB;

    resSize = offs - rc->offset - RHEADSIZE(&ResHead);

    ResHead.resSizeLo = (db_uint16)(resSize & 0xFFFF);
    ResHead.resSizeHi = (db_uint16)((resSize >> 16) & 0xFFFF);

    if((size != -1 && resSize > (size_t)size) || (ResHead.ver == 0 && resSize > 0xFFFF))
        return L_WRSIZE;

    if((stat = ResWritePrefix(rf, rc)) != 0)
        return stat;

    if((stat = ResAddElem(rf, rc)) != 0 )
        return L_ADDMEM;

    if(offs > rf->head.DataOffset)
        rf->head.DataOffset = offs;

    if(rf->flags & NEED_FLUSH)
        return ResFlush(rf);

    return 0;
}

// -----------------------------------------------------------------------------
int ResDelObject(ResFile *rf, Resource *res)
{
    ResHeader  rhead;
    ResIter    iter;
    Resource  *rc;
    int        ind = 0, stat = L_DIRMISS;


    ResSetIter(&iter, rf);

    while((rc = ResGetIter(&iter)) != NULL && (*rf->ResCmp)(rc, res) != 0)
        ind++;

    if(rc)
    {
        size_t  sz = rf->head.ElemSize;


        if((stat = ResReadPrefixEx(rf, rc, &rhead)) == 0)
        {
            FreeBlock  fb, *pfb, *p;
            int        i, iscombined = 0;


            res->offset = fb.offset = rc->offset;

            fb.size = (long)(MAKESIZE(&rhead) + RHEADSIZE(&rhead));

            memmove(rc, (char *)rc + sz, sz*(rf->head.NumElem - ind - 1));

            rf->head.NumElem--;
            rf->flags |= DIR_MODIFIED;

            for(i = 0, pfb = rf->freeblock; i < (int)rf->head.NumFree; i++, pfb++)
                if(fb.offset < pfb->offset)
                    break;

            if(i > 0)  // Есть предыдущий блок
            {
                p = rf->freeblock + i - 1;

                if(p->offset + p->size == fb.offset)
                {
                    p->size += fb.size;

                    fb.offset = p->offset;
                    fb.size   = p->size;

                    iscombined = 1;
                }
            }

            if(i < (int)rf->head.NumFree) // Есть следующий блок
            {
                p = rf->freeblock + i;

                if(fb.offset + fb.size == p->offset)
                {
                    p->size   += fb.size;
                    p->offset = fb.offset;

                    if(iscombined)
                        DelFreeBlock(rf, i - 1);

                    iscombined = 1;
                }
            }
            else
            {
                if(fb.offset + fb.size >= rf->head.DataOffset)
                {
                    rf->head.DataOffset = fb.offset;

                    if(iscombined)
                        DelFreeBlock(rf, i - 1);

                    iscombined = 1;
                }
            }

            if(!iscombined)
                stat = AddFreeBlock(rf, i, &fb);

            if(!stat && rf->flags & NEED_FLUSH)
                stat = ResFlush(rf);
        }
    }

    return stat;
}
#endif

// -----------------------------------------------------------------------------
int ResOpenStream(ResStream *strm, ResFile *rf, Resource *rc, int mode, int ver, long size)
{
    strm->rf          = rf;
    strm->stat        = 0;
    strm->size        = size;
    strm->rc          = rc;
    strm->useMewStrm  = false;
    strm->strmCurSize = 0;
    strm->mode        = RES_MODE_CLOSED;
    strm->resLibPos   = -1;

#ifdef RES_MODIFY
    if(mode == RES_MODE_WRITE)
    {
        if((strm->stat = ResGetElem(rf, rc)) == 0)
            return L_DUPOB;
        else if(!(strm->stat == L_DIRMISS || strm->stat == L_DIREMPTY))
            return strm->stat;

        strm->stat = 0;

        if(size == -1)
        {
            MemStremInit(&strm->memStrm);

            strm->useMewStrm = true;
        }
        else
        {
            strm->rhead.headVer = size < 0xffff ? 0 : 1;

            // June 28,1994
            rc->offset = ResFindOffset(rf, size + RHEADSIZE(&strm->rhead));

            strm->strmCurSize = size;
            strm->resLibPos   = rc->offset + RHEADSIZE(&strm->rhead);
        }

        ResSetCurTime(&strm->rhead);

        strm->rhead.ver = (unsigned char)ver;
    }
    else
#endif
    {
        if((strm->stat = ResGetElem(rf, rc)) != 0)
            return strm->stat;

        if((strm->stat = ResReadPrefixEx(rf, rc, &strm->rhead)) != 0)
            return strm->stat;

        // When in read mode, we know the size of object.
        strm->strmCurSize = strm->size = (long)MAKESIZE(&strm->rhead);    // strm->rhead.resSize;
        strm->resLibPos   = (long)(rc->offset + RHEADSIZE(&strm->rhead)); // sizeof(ResHeader);
    }

    strm->mode = mode;

    return strm->stat;
}

// -----------------------------------------------------------------------------
int ResGetPosStream(ResStream *strm, size_t *pos)
{
    if(strm->mode == RES_MODE_CLOSED)
        return L_STRMMODE;

    if(strm->useMewStrm)
        return MemStremGetPos(&strm->memStrm, pos);

    *pos = strm->resLibPos - strm->rc->offset - RHEADSIZE(&strm->rhead); //sizeof (ResHeader);

    return 0;
}

// -----------------------------------------------------------------------------
int ResSetPosStream(ResStream *strm, size_t pos)
{
    size_t  offs;


    if(strm->mode == RES_MODE_CLOSED)
        return L_STRMMODE;

    if(strm->useMewStrm)
        return MemStremSetPos(&strm->memStrm, pos);

    offs = strm->resLibPos;
    offs -= strm->rc->offset + RHEADSIZE(&strm->rhead); //sizeof (ResHeader);

    if(offs > strm->strmCurSize)
        strm->strmCurSize = offs;

    if(pos < 0 || (strm->size != -1 && pos > (size_t)strm->size))
        return L_POSLIB;

    strm->resLibPos = (long)(strm->rc->offset + pos + RHEADSIZE(&strm->rhead)); //sizeof(ResHeader);

    return 0;
}

// -----------------------------------------------------------------------------
int ResGetSizeStream(ResStream *strm, size_t *size)
{
    size_t  offs;


    if(strm->useMewStrm)
    {
        *size = MemStremGetSize(&strm->memStrm);

        return 0;
    }

    offs = strm->resLibPos;
    offs -= strm->rc->offset + RHEADSIZE(&strm->rhead); //sizeof (ResHeader);

    if(offs > strm->strmCurSize)
        strm->strmCurSize = offs;

    *size = strm->strmCurSize;

    return 0;
}

// -----------------------------------------------------------------------------
int ResCloseStream(ResStream *strm)
{
#ifdef RES_MODIFY
    size_t  resSize;


    if(strm->mode == RES_MODE_WRITE)
    {
        if(!strm->stat)
        {
            if(strm->useMewStrm)
            {
                resSize = strm->size = (long)MemStremGetSize(&strm->memStrm);

                strm->rhead.headVer = resSize < 0xffff ? 0 : 1;
                strm->rc->offset    = ResFindOffset(strm->rf, strm->size + RHEADSIZE(&strm->rhead));
                strm->resLibPos     = strm->rc->offset + RHEADSIZE(&strm->rhead);

                if(lseek(strm->rf->hd, strm->resLibPos, SEEK_SET) != -1L)
                    strm->stat = MemStremSaveToRes(&strm->memStrm, strm->rf->hd);
                else
                    strm->stat = L_POSLIB;

                MemStremDone(&strm->memStrm);

                strm->useMewStrm = false;

                if(strm->stat)
                    return strm->stat;

                if((strm->resLibPos = tell(strm->rf->hd)) == -1L)
                    return L_POSLIB;
            }
            else
                ResGetSizeStream(strm, &resSize);

            strm->rhead.resSizeLo  = (db_uint16)(resSize & 0xFFFF);
            strm->rhead.resSizeHi  = (db_uint16)((resSize >> 16) & 0xFFFF);

            if((strm->size != -1 && resSize > (size_t)strm->size) || (strm->rhead.ver == 0 && resSize > 0xFFFF))
                return L_WRSIZE;

            if((strm->stat = ResWritePrefixEx(strm->rf, strm->rc, &strm->rhead)) != 0)
                return strm->stat;

            if((strm->stat = ResAddElem(strm->rf, strm->rc)) != 0)
                return L_ADDMEM;

            if(strm->rc->offset + resSize + RHEADSIZE(&strm->rhead) > (size_t)strm->rf->head.DataOffset)
                strm->rf->head.DataOffset = (int32)(strm->rc->offset + resSize + RHEADSIZE(&strm->rhead));

            if(strm->rf->flags & NEED_FLUSH)
                return ResFlush(strm->rf);
        }
        else if(strm->useMewStrm)
            MemStremDone(&strm->memStrm);
    }
#endif

    strm->mode = RES_MODE_CLOSED;

    return strm->stat;
}

// -----------------------------------------------------------------------------
#ifdef RES_MODIFY
// -----------------------------------------------------------------------------
int ResWriteStream(ResStream *strm, void *buff, int size, int *wrSize)
{
    if(strm->mode == RES_MODE_WRITE)
    {
        if(!strm->stat)
        {
            size_t  sz;


            if(strm->useMewStrm)
                sz = MemStremWrite(&strm->memStrm, (char *)buff, size);
            else
            {
                if(lseek(strm->rf->hd, strm->resLibPos, SEEK_SET) == -1L)
                    strm->stat = L_POSLIB;

                if((sz = write(strm->rf->hd, buff, size)) != -1)
                    strm->resLibPos += (long)sz;
            }

            if(sz == -1)
                strm->stat = L_RESPUT;

            if(wrSize)
                *wrSize = (int)sz;
        }
    }
    else
        strm->stat = L_STRMMODE;

    return strm->stat;
}
#endif

// -----------------------------------------------------------------------------
int ResReadStream(ResStream *strm, void *buff, int size, int *rdSize)
{
    if(strm->mode == RES_MODE_READ)
    {
        if(!strm->stat)
        {
            size_t  sz;

            if(strm->useMewStrm)
                sz = MemStremRead(&strm->memStrm, (char *)buff, size);
            else
            {
                if(lseek(strm->rf->hd, strm->resLibPos, SEEK_SET) == -1L)
                    strm->stat = L_POSLIB;

                if((sz = read(strm->rf->hd, buff, size)) != -1)
                    strm->resLibPos += (long)sz;
            }

            if(sz == -1)
                strm->stat = L_RESGET;

            if(rdSize)
                *rdSize = (int)sz;
        }
    }
    else
        strm->stat = L_STRMMODE;

    return strm->stat;
}

// -----------------------------------------------------------------------------
int ResGetObject(ResFile *rf, Resource *rc, void **data, RES_GET Get)
{
    int  stat;


    if((stat = ResGetElem(rf, rc)) != 0 )
        return stat;

    if((stat = ResReadPrefix(rf, rc)) != 0)
        return stat;

    Res   = rc;
    CurRf = rf;

    return (*Get)(rf, data);
}

// -----------------------------------------------------------------------------
int ResNumObjects(ResFile *rf)
{
    return rf->head.NumElem;
}

// -----------------------------------------------------------------------------
char *ResError(int stat)
{
    if(stat > 0 && stat <= L_LASTER)
        return (char *)_PANEL_STR_RES_MSG[stat - 1];

    return "";
}

// -----------------------------------------------------------------------------
#ifdef RES_MODIFY
//-@H------ BEGIN __Transaction _______________________________ --------@@-

// -----------------------------------------------------------------------------
char *ResMakeBkName(ResFile *rf, char *buff)
{
    char  *p;


    strcpy(buff, rf->fileName);

    if((p = strrchr(buff, '.')) != NULL)
        *p = '\0';

    strcat(buff,".ltr");

    return buff;
}

// -----------------------------------------------------------------------------
int ResWriteTrnHead(ResFile *rf, int hd)
{
    if(write(hd, &rf->head, sizeof(Header)) != -1)
        return 0;

    return L_TRN;
}

// -----------------------------------------------------------------------------
int ResWriteTrnDic(ResFile *rf, int hd)
{
    if(write(hd, rf->Directory, rf->head.ElemSize*rf->head.NumElem) != -1)
        if(write(hd, rf->freeblock, rf->head.NumFree*sizeof(FreeBlock)) != -1)
            return 0;

    return L_TRN;
}

// -----------------------------------------------------------------------------
int ResReadTrnHead(ResFile *rf, int hd)
{
    if(read(hd, &rf->head, sizeof(Header)) != -1)
        return 0;

    return L_TRN;
}

// -----------------------------------------------------------------------------
int ResReadTrnDic(ResFile *rf, int hd)
{
    int  stat = 0;


    ResFreeDirectory(rf);

    if(rf->head.NumElem)
    {
        int  size = rf->head.ElemSize * rf->head.NumElem;


        if((rf->Directory = (Resource *)malloc(size)) != NULL)
            if(read (hd,rf->Directory,size) != -1)
            {
                rf->flags |= DIR_READ;

                size = rf->head.NumFree*sizeof(FreeBlock);

                if(rf->head.NumFree)
                    if((rf->freeblock = (FreeBlock *)malloc(size)) != NULL && read(hd, rf->freeblock, size) != -1)
                        rf->flags |= FREE_READ;
                    else
                        stat = L_TRN;
            }
            else
                stat = L_TRN;
        else
            stat = L_DIRNOMEM;
    }

    if(stat)
        ResFreeDirectory(rf);

    return stat;
}

// -----------------------------------------------------------------------------
int ResBeginTrn(ResFile *rf)
{
    char  backFile[_MAX_PATH];
    int   hd, backOk = 0;


    if(rf->head.isTrn)
        return L_TRNACTIV;

    if(!rf->fileName)
        return L_TRN;

    ResMakeBkName(rf, backFile);

    if((hd = sopen_rs(backFile, O_WRONLY | O_BINARY | O_CREAT | O_TRUNC, SH_DENYRW, S_IREAD | S_IWRITE)) != -1)
    {
        if(ResWriteTrnHead(rf, hd) == 0 && ResWriteTrnDic(rf, hd) == 0)
            backOk = 1;

        close(hd);

        if(backOk)
        {
            rf->head.isTrn = 1;

            if(ResPutFileHeader(rf) == 0)
            {
                commit(rf->hd);

                return 0;
            }
            else
                rf->head.isTrn = 0;
        }
    }

    unlink_rs(backFile);

    return L_TRN;
}

// -----------------------------------------------------------------------------
int ResEndTrn(ResFile *rf)
{
    char  backFile[_MAX_PATH];


    if(!rf->head.isTrn)
        return 0;

    if(!rf->fileName)
        return L_TRN;

    ResMakeBkName(rf, backFile);

    rf->head.isTrn = 0;

    if(ResPutFileHeader(rf) == 0)
    {
        commit(rf->hd);
        unlink_rs(backFile);

        return 0;
    }

    return L_TRN;
}

// -----------------------------------------------------------------------------
int ResAbortTrn(ResFile *rf)
{
    int   hd, backOk = 0;
    char  backFile [_MAX_PATH];


    if(!rf->head.isTrn)
        return 0;

    if(!rf->fileName)
        return L_TRN;

    ResMakeBkName(rf, backFile);

    if((hd = sopen_rs(backFile, O_RDONLY | O_BINARY, SH_DENYRW, 0)) != -1)
    {
        if(ResReadTrnHead(rf, hd) == 0 && ResReadTrnDic(rf, hd) == 0)
            backOk = 1;

        close(hd);

        if(backOk)
        {
            rf->head.isTrn = 1;

            if(ResPutDirectory(rf) == 0)
            {
                commit(rf->hd);

                return ResEndTrn(rf);
            }
        }
    }

    return L_TRN;
}
//-@@------ END __ Transaction ________________________________ --------@E-
#endif

/***************************************************************************/
/***************************************************************************/
//             Library part
static int LibCmp(LibElem *original, LibElem *rc)
// return 0 if original == rc else 1
{
    if((rc->type == 0 || rc->type == original->type) && strcmp(original->name, rc->name) == 0)
        return 0;

    return 1;
}

// -----------------------------------------------------------------------------
static void LibDirCvt(ResFile *rf, HRSLCVT hcvt, LibElem *rc)
{
    RslCvtLong(hcvt, &rc->rc.offset);
    RslCvtString(hcvt, rc->name);
}

// -----------------------------------------------------------------------------
static char *ResFileName(char *fname)
{
    return fname;
}

// -----------------------------------------------------------------------------
int _OpenLib(ResFile *rf, const char *fname, int flags)
{
    char  buf[_MAX_PATH];
    int   stat;


    fname = fname ? ResFileName(strcpy(buf, fname)) : fname;

    stat = ResOpen(rf, fname, "RESLIB", (RES_CMP)LibCmp, (RES_CVT)LibDirCvt, flags, sizeof(LibElem));

    if(stat == L_LIBTYPE)
        stat = ResOpen(rf, fname, _PANEL_STR_RES_NAME, (RES_CMP)LibCmp, (RES_CVT)LibDirCvt, flags, sizeof(LibElem));

    return stat;
}

// -----------------------------------------------------------------------------
int LibOpenStream(LibStream *strm, ResFile *rf, int mode, const char *name, unsigned char type, int ver, long size)
{
    strncpy(strm->rc.name, name, RESNAMELEN - 1);

    strm->rc.name[RESNAMELEN - 1] = '\0';

    strupr(strm->rc.name);

    strm->rc.type      = type;
    strm->derived.stat = 0;
    strm->inTrnMode    = false;

#ifdef RES_MODIFY
    if(mode == RES_MODE_WRITE)
    {
        if(rf->flags & READ_MODE)
            return L_RESPUT;

        strm->oldflags = rf->flags & (NEED_FLUSH | USE_TRN);

        if(size != -1 && (rf->flags & USE_TRN))
        {
            strm->derived.stat = ResBeginTrn(rf);

            if(!strm->derived.stat)
                strm->inTrnMode = true;
        }
    }
#endif

    if(!strm->derived.stat)
        strm->derived.stat = ResOpenStream(&strm->derived, rf, P_RES(&strm->rc), mode, ver, size);

#ifdef RES_MODIFY
    if(strm->derived.stat && strm->inTrnMode)
    {
        ResAbortTrn(rf);

        rf->flags |= strm->oldflags;

        strm->inTrnMode = false;
    }
#endif

    return strm->derived.stat;
}

// -----------------------------------------------------------------------------
int LibCloseStream(LibStream *strm, bool abort)
{
    int       stat;
    int       mode = strm->derived.mode;
    ResFile  *rf   = strm->derived.rf;


    if(abort)
        strm->derived.stat = -1;

#ifdef RES_MODIFY
    if(mode == RES_MODE_WRITE && !strm->derived.stat && (rf->flags & USE_TRN) && !strm->inTrnMode)
    {
        strm->derived.stat = ResBeginTrn(rf);

        if(!strm->derived.stat)
            strm->inTrnMode = true;
    }
#endif

    stat = ResCloseStream(&strm->derived);

#ifdef RES_MODIFY
    if(mode == RES_MODE_WRITE && strm->inTrnMode)
    {
        if(!stat && !abort)
            ResEndTrn(rf);
        else
            ResAbortTrn(rf);

        rf->flags |= strm->oldflags;
    }

    strm->inTrnMode = false;
#endif

    return stat;
}

// -----------------------------------------------------------------------------
#ifdef RES_MODIFY
// -----------------------------------------------------------------------------
int LibWriteStream(LibStream *strm, void *buff, int size, int *wrSize)
{
    return ResWriteStream(&strm->derived, buff, size, wrSize);
}
#endif

// -----------------------------------------------------------------------------
int LibReadStream(LibStream *strm, void *buff, int size, int *rdSize)
{
    return ResReadStream(&strm->derived, buff, size, rdSize);
}

// -----------------------------------------------------------------------------
int GetResObject(ResFile *rf, const char *name, unsigned char type, void **data, RES_GET Get)
{
    LibElem  rc;


    strncpy(rc.name, name, RESNAMELEN - 1);

    rc.name[RESNAMELEN - 1] = '\0';

    strupr(rc.name);

    rc.type = type;

    return ResGetObject(rf, P_RES(&rc), data, Get);
}

// -----------------------------------------------------------------------------
int FindResObject(ResFile *rf, const char *name, unsigned char type)
{
    LibElem  rc;


    strncpy(rc.name, name, RESNAMELEN - 1);

    rc.name[RESNAMELEN - 1] = '\0';

    strupr(rc.name);

    rc.type = type;

    return ResGetElem(rf, P_RES(&rc));
}

int PutResObject(ResFile *rf, const char *name, unsigned char type, void *data, RES_PUT Put, long size)
{
    int      stat = 0;
    int      oldflags;
    LibElem  rc;


    strncpy(rc.name, name, RESNAMELEN - 1);

    rc.name[RESNAMELEN - 1] = '\0';

    strupr(rc.name);

    rc.type = type;

    if(rf->flags & READ_MODE)
        return L_RESPUT;

    oldflags = rf->flags & (NEED_FLUSH | USE_TRN);

    if(rf->flags & USE_TRN)
        stat = ResBeginTrn(rf);

    if(!stat)
        stat = ResPutObject(rf, P_RES(&rc), data, Put, size);

    if(rf->flags & USE_TRN)
    {
        if(!stat)
            ResEndTrn(rf);
        else
            ResAbortTrn(rf);

        rf->flags |= oldflags;
    }

    return stat;
}

// -----------------------------------------------------------------------------
int DeleteResObject(ResFile *rf, const char *name, unsigned char type)
{
    int      stat = 0;
    int      oldflags;
    LibElem  rc;


    strncpy(rc.name, name, RESNAMELEN - 1);

    rc.name[RESNAMELEN - 1] = '\0';

    strupr(rc.name);

    rc.type = type;

    if(rf->flags & READ_MODE)
        return L_RESPUT;

    oldflags = rf->flags & (NEED_FLUSH | USE_TRN);

    if(rf->flags & USE_TRN)
        stat = ResBeginTrn(rf);

    if(!stat)
        stat = ResDelObject(rf, P_RES(&rc));

    if(rf->flags & USE_TRN)
    {
        if(!stat)
            ResEndTrn(rf);
        else
            ResAbortTrn(rf);

        rf->flags |= oldflags;
    }

    return stat;
}

// -----------------------------------------------------------------------------
int RenameResObject(ResFile *rf, const char *name, unsigned char type, const char *newName)
{
    int      stat = 0;
    LibElem  rc;


    strncpy(rc.name, name, RESNAMELEN - 1);

    rc.name[RESNAMELEN - 1] = '\0';

    strupr(rc.name);

    rc.type = type;

    if(FindResObject(rf, newName, type) == 0)
        return L_DUPOB;

    stat = ResGetElem(rf, P_RES(&rc));

    if(!stat)
    {
        LibElem  newRc;


        memmove(&newRc, &rc, sizeof(LibElem));

        strncpy(newRc.name, newName, RESNAMELEN - 1);

        newRc.name[RESNAMELEN - 1] = '\0';

        strupr(newRc.name);

        stat = ResReplaceElem(rf, P_RES(&rc), P_RES(&newRc));
    }

    return stat;
}

// -----------------------------------------------------------------------------
#ifdef TESTRES
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
int main (void)
{
    char    *data = "This is a test buffer";
    int      stat;
    ResFile  rf;


    stat = _OpenLib(&rf, "test.lbr", RO_CREATE | RO_TRN);

    if(!stat)
    {
        size_t     pos;
        LibStream  strm1;
        LibStream  strm2;


        if(FindResObject(&rf, "Res5", 1) == 0)
            DeleteResObject(&rf, "Res5", 1);

        if(FindResObject(&rf, "Res2", 1) == 0)
            DeleteResObject(&rf, "Res2", 1);

        //#if 0
        stat = LibOpenStream(&strm1, &rf, RES_MODE_WRITE, "Res5", 1, 0, -1);

        if(!stat)
        {
            stat = LibOpenStream(&strm2, &rf, RES_MODE_WRITE, "Res2", 1, 0, -1);

            if(!stat)
            {
                stat = LibWriteStream(&strm1, "Hello ", 6, NULL);

                if(!stat)
                    stat = LibWriteStream(&strm2, "This ", 5, NULL);

                if(!stat)
                    stat = LibWriteStream(&strm1, ", World!", 9, NULL);

                if(!stat)
                    stat = LibWriteStream(&strm2, "is a test.", 11, NULL);

                /*
       if (!LibGetPosStream (&strm,&pos))
          if (!LibSetPosStream (&strm,pos-6))
             {
             stat = LibWriteStream (&strm,data,strlen (data) + 1 ,NULL);
             if (!stat)
                printf ("OK\n");
             }
*/
                if(!stat)
                    printf("Test passed!\n");

                stat = LibCloseStream(&strm2, false);
            }

            stat = LibCloseStream (&strm1,false);
        }
        //#endif

        ResClose(&rf);
    }

    if(stat)
        printf("*** %s\n", ResError(stat));

    stat = _OpenLib(&rf, "test.lbr", RO_READDIR);

    if(!stat)
    {
        size_t     pos;
        LibStream  strm1;
        LibStream  strm2;
        char       buff1[40];
        char       buff2[40];


        stat = LibOpenStream(&strm1, &rf, RES_MODE_READ, "Res5", 1, 0, -1);

        if(!stat)
        {
            stat = LibOpenStream(&strm2, &rf, RES_MODE_READ, "Res2", 1, 0, -1);

            if(!stat)
            {
                stat = LibReadStream(&strm1, buff1, 4, NULL);

                if(!stat)
                    stat = LibReadStream(&strm2, buff2, 7, NULL);

                if(!stat)
                    stat = LibReadStream(&strm1, buff1 + 4, 11, NULL);

                if(!stat)
                    stat = LibReadStream(&strm2, buff2 + 7, 9, NULL);

                if(!stat)
                {
                    printf("Res5 = %s\n", buff1);
                    printf("Res2 = %s\n", buff2);
                }

                /*
       if (!LibGetPosStream (&strm,&pos))
          if (!LibSetPosStream (&strm,pos-6))
             {
             stat = LibWriteStream (&strm,data,strlen (data) + 1 ,NULL);
             if (!stat)
                printf ("OK\n");
             }
*/
                if(!stat)
                    printf("Test passed!");

                stat = LibCloseStream(&strm2, false);
            }

            stat = LibCloseStream(&strm1, false);
        }
        //#endif

        ResClose(&rf);
    }

    if(stat)
        printf("*** %s\n", ResError(stat));

    return stat;
}
#endif

/* <-- EOF --> */
