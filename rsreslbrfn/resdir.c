/**********************************************************************
*        File Name:   resdir.c           April 28,1994
*       Programmer:   S. Kubrin
*      Description:
*  Public routines:
* Private routines:

*   VERSION INFO :   $Archive: /RS-Tools/Old_tools/panel/c/resdir.c $
                     $Revision: 14 $
                     $Date: 14.02.00 15:26 $
                     $Author: Lebedev $
                     $Modtime: 14.02.00 13:21 $


 January 13,1997   Serg Kubrin (K78)
     Add "Old Type Resource" string in comment to old resource
***********************************************************************/
#include <stdlib.h>
#include <string.h>
#include <unixio.h>

#include "res_lbr.h"

#include "rsconfig.h"


// -----------------------------------------------------------------------------
static int sortComment(RLibDirElem *left, RLibDirElem *right)
{
 int  cmp = strcmp(CMT_PTR(left), CMT_PTR(right));


 if(!cmp)
   return (int)left->type - (int)right->type;

 return cmp;
}

// -----------------------------------------------------------------------------
static int sortName(RLibDirElem *left, RLibDirElem *right)
{
 int  cmp = strcmp(left->name, right->name);


 if(!cmp)
   return (int)left->type - (int)right->type;

 return cmp;
}

// -----------------------------------------------------------------------------
static int sortType(RLibDirElem *left, RLibDirElem *right)
{
 int  cmp = (int)left->type - (int)right->type;


 if(!cmp)
   return strcmp(left->name, right->name);

 return cmp;
}

// -----------------------------------------------------------------------------
static int sortDate(RLibDirElem *left, RLibDirElem *right)
{
 int            cmp;
 unsigned long  rt = *((unsigned long *)&right->ftime);
 unsigned long  lt = *((unsigned long *)&left->ftime);


 cmp = rt > lt ? 1 : (rt == lt ? 0 : -1);

 if(!cmp)
   return strcmp(left->name, right->name);

 return cmp;
}

// -----------------------------------------------------------------------------
void SortDirectory(RLibDir *dir, DirSort sortby)
{
 int  (*fcmp)(const void *, const void *);


 switch(sortby)
      {
       case RS_NAME: fcmp = (int (*)(const void *, const void *))sortName;    break;
       case RS_TYPE: fcmp = (int (*)(const void *, const void *))sortType;    break;
       case RS_DATE: fcmp = (int (*)(const void *, const void *))sortDate;    break;
       case RS_COMM: fcmp = (int (*)(const void *, const void *))sortComment; break;
      }

 dir->sortby = sortby;

 qsort(dir->Elem, dir->NumElem, sizeof(RLibDirElem) + dir->cmtSize, fcmp);
}

// -----------------------------------------------------------------------------
int RemDirElem(RLibDir *dir, int n)
{
 memmove(ELEM_PTR(dir, n), ELEM_PTR(dir, n + 1), (dir->NumElem - n - 1)*(sizeof(RLibDirElem) + dir->cmtSize));

 dir->NumElem--;

 return 0;
}

// -----------------------------------------------------------------------------
int RemDirElemName(RLibDir *dir, const char *name, unsigned char type)
{
 int           stat = L_DIRMISS;
 RLibDirElem  *e;
 int           i;


 for(i = 0, e = dir->Elem; i < dir->NumElem; i++)
   {
    if(stricmp(e->name, name) == 0 && e->type == type)
     {
      RemDirElem(dir, i);

      stat = 0;

      break;
     }

    e = NEXT_DIR(e, dir);
   }

 return stat;
}

// -----------------------------------------------------------------------------
int AddDirElem(RLibDir *dir, const char *name, unsigned char type, long size, struct ftime *ftime,
               unsigned char ver, unsigned char flags, const char *cmt)
{
 void         *ptr;
 RLibDirElem  *e;
 int           i;


 for(i = 0, e = dir->Elem; i < dir->NumElem; i++)
   {
    if(stricmp(e->name, name) == 0 && e->type == type)
     {
      RemDirElem(dir, i);

      break;
     }

    e = NEXT_DIR(e, dir);
   }

 if(dir->NumElem == dir->alloced)
  {
   ptr = realloc(dir->Elem, (dir->alloced + 10)*(sizeof(RLibDirElem) + dir->cmtSize));

   if(ptr)
    {
     dir->Elem = (RLibDirElem *)ptr;
     dir->alloced += 10;
    }
   else
     return -1;
  }

 e = ELEM_PTR(dir, dir->NumElem);

 dir->NumElem++;

 strcpy(e->name, name);

 e->type  = type;
 e->size  = size;
 e->ftime = *ftime;
 e->flags = flags;
 e->ver   = ver;

 if(dir->cmtSize)
   if(cmt)
    {
     strncpy(CMT_PTR(e), cmt, dir->cmtSize);

     CMT_PTR(e)[dir->cmtSize - 1] = '\0';
    }
   else
     CMT_PTR(e)[0] = '\0';

 SortDirectory(dir, dir->sortby);

 return 0;
}

// -----------------------------------------------------------------------------
int BuildDirectory(RLibDir *dir, ResFile *rf, RLibDirElemFiltr filt, int num, int cmtSize)
{
 int           stat = 0;
 LibElem      *rc;
 RLibDirElem  *e;
 ResIter       iter;


 dir->NumElem = dir->alloced = 0;
 dir->Elem    = NULL;
 dir->sortby  = RS_NAME;

 if(num == -1)
   num = ResNumObjects(rf);

 if(num == 0)
   return 0;

 e = dir->Elem = (RLibDirElem *)malloc((sizeof(RLibDirElem) + cmtSize)*num);

 if(!dir->Elem)
   return L_ADDMEM;

 dir->alloced = num;
 dir->cmtSize = cmtSize;

 ResSetIter(&iter, rf);

 while((rc = (LibElem *)ResGetIter(&iter)) != NULL && !stat && num--)
      if((stat = ResReadPrefix(rf, P_RES(rc))) == 0)
       {
        ResHeader  *hd = ResGetCurHeader();


        strcpy(e->name, rc->name);

        e->type  = rc->type;
        e->size  = (long)MAKESIZE(hd); //hd->resSize;
        e->ftime = hd->ftime;
        e->flags = hd->headVer;
        e->ver   = hd->ver;

        if(!filt || filt(e) == 0)
         {
          if(dir->cmtSize)
           {
            if(hd->ver > 0)
             {
              unsigned char  len;


              if(read(rf->hd, &len, sizeof(unsigned char)) == sizeof(unsigned char) && len)
                read(rf->hd, CMT_PTR(e), dir->cmtSize < len ? dir->cmtSize : len);
              else
                CMT_PTR(e)[0] = '\0';
             }
            else
              strncpy(CMT_PTR(e), "*** Old Type Resource", dir->cmtSize < 22 ? dir->cmtSize : 22);

            CMT_PTR(e)[dir->cmtSize - 1] = '\0';
           }

          e = NEXT_DIR(e, dir);

          dir->NumElem++;
         }
       }

 if(!stat)
   SortDirectory(dir, RS_NAME);

 return stat;
}

// -----------------------------------------------------------------------------
void FreeDirectory(RLibDir *dir)
{
 if(dir->Elem)
   free(dir->Elem);

 dir->NumElem = dir->alloced = 0;
 dir->Elem    = NULL;
 dir->sortby  = RS_NAME;
}

/* <-- EOF --> */
