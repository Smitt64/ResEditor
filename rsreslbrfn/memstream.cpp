#include <sys/stat.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "platform.h"
#include "unixio.h"

#include <time.h>

#include "res_lbr.h"
#include "rstring.h"

//#include "unifile.hpp"

#include "panel.h"

#include "rsconfig.h"


//-@H------ BEGIN __ __________________________________________ --------@@-

#define  MEMSTREAMSIZE  512

// -----------------------------------------------------------------------------
typedef struct tagMemPage
      {
       LOBJ     elem;
       size_t   maxSize;
       size_t   curSize;
       char     buff[MEMSTREAMSIZE];
      } TMemPage;

// -----------------------------------------------------------------------------
static int FndPosFunc(TMemPage *pg, size_t *pagePos)
{
 if(*pagePos < pg->maxSize)
   return 1;  // Yes! The position is in this page

 *pagePos -= pg->maxSize;

 return 0;
}

// -----------------------------------------------------------------------------
size_t MemStremWrite(TMemStream *st, char *buff, size_t sz)
{
 size_t     pagePos = st->curPos;
 TMemPage  *pg      = (TMemPage *)LobjFirstThat(&st->pages, (LCMPFUNC)FndPosFunc, &pagePos);

 size_t  wrSize = 0;

 size_t  available;
 size_t  toWrite;


 for(;;)
   {
    if(!pg)
     {
      pg = (TMemPage *)malloc(sizeof(TMemPage));

      if(!pg)
        return -1;

      LobjInsert(&st->pages, pg);

      pg->maxSize = MEMSTREAMSIZE;
      pg->curSize = 0;

      pagePos = 0;
     }

    available = pg->maxSize - pagePos;
    toWrite   = rs_min(available, sz);

    memmove(pg->buff + pagePos, buff + wrSize, toWrite);

    st->curPos += toWrite;
    pagePos    += toWrite;
    wrSize     += toWrite;
    sz         -= toWrite;

    if(pagePos > pg->curSize)
      pg->curSize = pagePos;

    if(sz)
     {
      pg = (TMemPage *)LobjNextItem(&st->pages, pg);

      pagePos = 0;
     }
    else
      break;
   }

 return wrSize;
}

// -----------------------------------------------------------------------------
size_t MemStremRead(TMemStream *st, char *buff, size_t sz)
{
 size_t     pagePos = st->curPos;
 TMemPage  *pg      = (TMemPage *)LobjFirstThat(&st->pages, (LCMPFUNC)FndPosFunc, &pagePos);

 size_t  available;
 size_t  rdSize = 0;
 size_t  toRead;


 for(;;)
   {
    if(!pg)
      break;

    available = pg->curSize - pagePos;
    toRead    = rs_min(available, sz);

    memmove(buff + rdSize, pg->buff + pagePos, toRead);

    st->curPos += toRead;
    rdSize     += toRead;
    sz         -= toRead;
    pagePos    += toRead;

    if(sz)
     {
      pg = (TMemPage *)LobjNextItem(&st->pages, pg);

      pagePos = 0;
     }
    else
      break;
   }

 return rdSize;
}

// -----------------------------------------------------------------------------
struct WrtData
     {
      int     hd;
      size_t  sz;
      int     stat;
     };

// -----------------------------------------------------------------------------
static int WriteProc(TMemPage *pg, struct WrtData *data)
{
 size_t  sz = write(data->hd, pg->buff, (int)pg->curSize);


 if(sz != -1)
  {
   data->sz  += sz;
   data->stat = 0;

   return 0;
  }

 data->stat = L_RESPUT;

 return 1;
}

// -----------------------------------------------------------------------------
int MemStremSaveToRes(TMemStream *st, int hd)
{
 struct WrtData  data;


 data.hd   = hd;
 data.sz   = 0;
 data.stat = 0;

 LobjForEach(&st->pages, (LITRFUNC)WriteProc, &data);

 return data.stat;
}

// -----------------------------------------------------------------------------
static int CalcSize(TMemPage *pg, size_t *strmSize)
{
 *strmSize += pg->curSize;

 return 0;
}

// -----------------------------------------------------------------------------
size_t MemStremGetSize(TMemStream *st)
{
 size_t size = 0;

 LobjForEach(&st->pages, (LITRFUNC)CalcSize, &size);

 return size;
}

// -----------------------------------------------------------------------------
int MemStremSetPos(TMemStream *st, size_t pos)
{
 size_t  sz = MemStremGetSize(st);


 if(pos >= 0 && pos <= sz)
  {
   st->curPos = pos;

   return 0;
  }

 return L_POSLIB;
}

// -----------------------------------------------------------------------------
int MemStremGetPos(TMemStream *st, size_t *pos)
{
 *pos = st->curPos;

 return 0;
}

// -----------------------------------------------------------------------------
static void ClearPage(TMemPage *pg)
{
 free(pg);
}

// -----------------------------------------------------------------------------
void MemStremInit(TMemStream *st)
{
#ifdef BUILD_DLM
 LobjInitList(&st->pages, offsetof(TMemPage, elem));
#else
 InitCntList(&st->pages, LST_DEFAULT, offsetof(TMemPage, elem));
#endif

 st->curPos = 0;
}

// -----------------------------------------------------------------------------
void MemStremDone(TMemStream *st)
{
#ifdef BUILD_DLM
 LobjDoneList(&st->pages, (LCLRFUNC)offsetof(TMemPage, elem));
#else
 DoneCntList(&st->pages, (LCLRFUNC)ClearPage);
#endif
}

//-@@------ END __ ____________________________________________ --------@E-

/* <-- EOF --> */
