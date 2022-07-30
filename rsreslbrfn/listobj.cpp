#include "listobj.h"
#include "platform.h"


// -----------------------------------------------------------------------------
#ifdef RSL_MD_FLAT
    #define LSTLOCAL
#else
    #define  LSTLOCAL  _near _pascal
#endif

// -----------------------------------------------------------------------------
#if defined ( RSL_MD_MT ) && defined ( RSL_PL_WIN32 )
    // -----------------------------------------------------------------------------
    #ifndef _INC_WINDOWS
        #include <windows.h>
    #endif

    #define  RSL_INIT_CRS(lst)   if(lst->sect) InitializeCriticalSection((LPCRITICAL_SECTION)lst->sect);
    #define  RSL_DEL_CRS(lst)    if(lst->sect) DeleteCriticalSection((LPCRITICAL_SECTION)lst->sect);
    #define  RSL_ENTER_CRS(lst)  if(lst->sect) EnterCriticalSection((LPCRITICAL_SECTION)lst->sect);
    #define  RSL_LEAVE_CRS(lst)  if(lst->sect) LeaveCriticalSection((LPCRITICAL_SECTION)lst->sect);
#elif defined ( RSL_MD_MT ) && defined ( RSL_MD_SUN )
    // -----------------------------------------------------------------------------
    #ifndef _PTHREAD_H
        #include <pthread.h>
    #endif // _PTHREAD_H

    #define  RSL_INIT_CRS(lst)   if(lst->sect) pthread_mutex_init((pthread_mutex_t *)lst->sect, NULL);
    #define  RSL_DEL_CRS(lst)    if(lst->sect) pthread_mutex_destroy((pthread_mutex_t *)lst->sect);
    #define  RSL_ENTER_CRS(lst)  if(lst->sect) pthread_mutex_lock((pthread_mutex_t *)lst->sect);
    #define  RSL_LEAVE_CRS(lst)  if(lst->sect) pthread_mutex_unlock((pthread_mutex_t *)lst->sect);
#else // defined ( RSL_MD_MT ) && defined ( RSL_MD_SUN )
    // -----------------------------------------------------------------------------
    #define  RSL_INIT_CRS(lst)
    #define  RSL_DEL_CRS(lst)
    #define  RSL_ENTER_CRS(lst)
    #define  RSL_LEAVE_CRS(lst)
#endif

// -----------------------------------------------------------------------------
// Этот макрос преобразует указатель на объект пользователя, унаследованный от
// LOBJ к указателю на соответствующий LOBJ
#define  P_LOBJ(lst, ptr)  ((LOBJ *)((char *)(ptr) + (lst)->delta))

// -----------------------------------------------------------------------------
// Этот макрос преобразует указатель на LOBJ к соответствующему
// объекту пользователя
#define  P_LSON(lst, ptr)  ((void *)((char *)(ptr) - (lst)->delta))


// -----------------------------------------------------------------------------
static LOBJ *LSTLOCAL LobjFirst(CNTLIST *lst)
{
 if(lst->last == NULL)
   return NULL;
 else
   return lst->last->next;
}

// -----------------------------------------------------------------------------
// 14 October, 1997
// This is the fastest version of old LobjPrev
static LOBJ  *LSTLOCAL LobjPrev(CNTLIST *lst, LOBJ *obj)
{
// last can not be NULL (obj must be in list) !!!!
 LOBJ  *res = lst->last;


 while(res->next != obj)
      res = res->next;

 return res;
}

// -----------------------------------------------------------------------------
static LOBJ *LSTLOCAL LobjPrevNull(CNTLIST *lst, LOBJ *obj)
{
 if(obj == LobjFirst(lst))
   return NULL;
 else
   return LobjPrev(lst, obj);
}

//----------- Begin Public Functions -----

// -----------------------------------------------------------------------------
void *LobjLastItem(CNTLIST *lst)
{
 void  *obj;


 RSL_ENTER_CRS(lst)

 obj = lst->last ? P_LSON(lst, lst->last) : NULL;

 RSL_LEAVE_CRS(lst)

 return obj;
}

// -----------------------------------------------------------------------------
void *LobjFirstItem(CNTLIST *lst)
{
 LOBJ  *ob;
 void  *obj;


 RSL_ENTER_CRS(lst)

 ob  = LobjFirst(lst);
 obj = ob ? P_LSON(lst, ob) : NULL;

 RSL_LEAVE_CRS(lst)

 return obj;
}

// -----------------------------------------------------------------------------
void *LobjNextItem(CNTLIST *lst, void *obj)
{
 LOBJ  *ob = P_LOBJ(lst, obj);


 RSL_ENTER_CRS(lst)

 obj = (ob == lst->last ? NULL : P_LSON(lst, ob->next));

 RSL_LEAVE_CRS(lst)

 return obj;
}

// -----------------------------------------------------------------------------
void *LobjPrevItem(CNTLIST *lst, void *obj)
{
 LOBJ  *ob = P_LOBJ(lst, obj);


 RSL_ENTER_CRS(lst)

 ob  = LobjPrevNull(lst, ob);
 obj =  ob ? P_LSON(lst, ob) : NULL;

 RSL_LEAVE_CRS(lst)

 return obj;
}

// -----------------------------------------------------------------------------
void LobjInsertAfter(CNTLIST *lst, void *ptr, void *Targ)
// если Target == NULL, вставка происходит или в голови или
// в хвост в зависимости от значения флага lst->flags
{
 LOBJ  *Target, *p;


 if(!lst || !ptr)
   return;

 p = P_LOBJ(lst, ptr);

// -- @H June 23,1997 ------------------------------------------
 if(lst->flags & LST_SMART)
  {
   SOBJ  *obj = (SOBJ *)p;


   SobjUnlink(obj);

   obj->owner = lst;
  }
// -- June 23,1997 ---------------------------------------------

 RSL_ENTER_CRS(lst)

 if(Targ != NULL)
  {
   Target = P_LOBJ(lst, Targ);

   p->next = Target->next;

   Target->next= p;

   if(Target == lst->last)
     lst->last = p;
  }
 else
  {
   if(lst->last == NULL)
    {
     p->next = p;
     lst->last = p;
    }
   else if(!(lst->flags & LST_ADDBEG)) // Добавляем в хвост
    {
     p->next = lst->last->next;

     lst->last->next = p;
     lst->last       = p;
    }
   else  // Добавляем в голову
    {
     p->next = lst->last->next;
     lst->last->next = p;
    }
  }

 RSL_LEAVE_CRS(lst)
}

// -----------------------------------------------------------------------------
void LobjInsertBefore(CNTLIST *lst, void *ptr, void *Targ)
{
 unsigned char  oldFlags;


 RSL_ENTER_CRS(lst)

 oldFlags = lst->flags;

 if(Targ)
  {
   LOBJ  *ob = P_LOBJ(lst, Targ);


   ob = LobjPrevNull(lst, ob);

   Targ = ob ? P_LSON(lst, ob) : NULL;

   lst->flags |= LST_ADDBEG; // For corect operatin before first (eq NULL)
  }

 LobjInsertAfter(lst, ptr, Targ);

 lst->flags = oldFlags;

 RSL_LEAVE_CRS(lst)
}

// -----------------------------------------------------------------------------
void LobjInsert(CNTLIST *lst, void *obj)
{
 LobjInsertAfter(lst, obj, NULL);
}

// -----------------------------------------------------------------------------
//  Индексы начинаются с 0
void *LobjAt(CNTLIST *lst, int index)
{
 LOBJ  *temp;


 RSL_ENTER_CRS(lst)

 temp = lst->last;

 if(temp)
  {
   index++;

   while(index-- > 0)
        temp = temp->next;
  }

 RSL_LEAVE_CRS(lst)

 return temp ? P_LSON(lst, temp) : NULL;
}

// -----------------------------------------------------------------------------
void LobjInsertAt(CNTLIST *lst, void *obj, int at)
{
 LobjInsertBefore(lst, obj, LobjAt(lst, at));
}

// -----------------------------------------------------------------------------
void *LobjSetLast(CNTLIST *lst, void *obj)
//
{
 LOBJ  *oldlast = lst->last;


 lst->last = obj ? P_LOBJ(lst, obj) : NULL;

 return oldlast ? P_LSON(lst, oldlast) : NULL;
}

// -----------------------------------------------------------------------------
int LobjIndexOf(CNTLIST *lst, void *ptr)
{
 int    index = 0;
 LOBJ  *p = P_LOBJ(lst, ptr);
 LOBJ  *temp;


 RSL_ENTER_CRS(lst)

 temp = lst->last;

 if(temp)
  {
   do
    {
     index++;

     temp = temp->next;
    } while(temp != p && temp != lst->last);
  }

 RSL_LEAVE_CRS(lst)

 if(!temp)
   return -1;

 if(temp != p)
   return 0;
 else
   return index - 1;
}

// -----------------------------------------------------------------------------
int LobjNitems (CNTLIST *lst)
{
   return lst->last ? LobjIndexOf (lst,P_LSON (lst,lst->last)) + 1 : 0;
}

// -----------------------------------------------------------------------------
void *LobjForEachEx(CNTLIST *lst, LITRFUNC func, void *args, int *retval)
{
 LOBJ  *temp;
 LOBJ  *findObj = NULL;


 *retval = 0;

 RSL_ENTER_CRS(lst)

 temp = lst->last;

 if(temp)
  {
   do
    {
     temp = temp->next;

     if((*retval = func(P_LSON(lst, temp), args)) != 0)
      {
       findObj = temp;

       break;
      }
    } while(temp != lst->last);
  }

 RSL_LEAVE_CRS(lst)

 return findObj ? P_LSON(lst, findObj) : NULL;
}

// -----------------------------------------------------------------------------
int LobjForEach(CNTLIST *lst, LITRFUNC func, void *args)
{
 int  retval;


 LobjForEachEx(lst, (LITRFUNC)func, args, &retval);

 return retval;
}

// -----------------------------------------------------------------------------
void *LobjFirstThat(CNTLIST *lst, LCMPFUNC func, void *args)
{
 int retval;


 return LobjForEachEx(lst, (LITRFUNC)func, args, &retval);
}

// -----------------------------------------------------------------------------
void *LobjForEachLEx(CNTLIST *lst, LITRFUNC func, void *args, int *retval)
{
 LOBJ  *temp;
 LOBJ  *findObj = NULL;


 *retval = 0;

 RSL_ENTER_CRS(lst)

 temp = lst->last;

 if(temp)
  {
   do
    {
     if((*retval = func(P_LSON(lst, temp), args)) != 0)
      {
       findObj = temp;

       break;
      }

     temp = LobjPrev(lst, temp);
    } while(temp != lst->last);
  }

 RSL_LEAVE_CRS(lst)

 return findObj ? P_LSON(lst, findObj) : NULL;
}

// -----------------------------------------------------------------------------
int LobjForEachL(CNTLIST *lst, LITRFUNC func, void *args)
{
 int  retval;


 LobjForEachLEx(lst, (LITRFUNC)func, args, &retval);

 return retval;
}

// -----------------------------------------------------------------------------
void *LobjLastThat(CNTLIST *lst, LCMPFUNC func, void *args)
{
 int  retval;


 return LobjForEachLEx(lst, (LITRFUNC)func, args, &retval);
}

// -----------------------------------------------------------------------------
static void DetachObj(CNTLIST *lst, LOBJ *obj)
{
 LOBJ  *prev;


 RSL_ENTER_CRS(lst)

 prev       = LobjPrev(lst, obj);
 prev->next = obj->next;
 obj->next  = NULL;

// June 23,1997
 if(lst->flags & LST_SMART)
   ((SOBJ *)obj)->owner = NULL;

 if(lst->last == obj)
   lst->last = prev == obj ? NULL : prev;

 RSL_LEAVE_CRS(lst)
}

// -----------------------------------------------------------------------------
void LobjDetach(CNTLIST *lst, void *ptr)
{
 DetachObj(lst, P_LOBJ(lst, ptr));
}

// -----------------------------------------------------------------------------
void LobjRemove(CNTLIST *lst, void *obj, LCLRFUNC func)
{
 if(!obj)
   return ;

 LobjDetach(lst, obj);

 if(!(lst->flags & LST_NOTOWN) && func)
   func(obj);
}

// -----------------------------------------------------------------------------
void DoneCntList(CNTLIST *lst, LCLRFUNC func)
{
 if(lst->last)
  {
   LOBJ  *objToDel;
   LOBJ  *temp = lst->last->next;


   if(lst->flags & LST_NOTOWN)
    {
     if(lst->flags & LST_SMART)
      {
       do
        {
         objToDel = temp;
         temp = objToDel->next;
         ((SOBJ *)objToDel)->owner = NULL;
        } while(objToDel != lst->last);
      }
    }
   else
    {
     bool  nextLoop;


     do
      {
       objToDel = temp;
       temp = objToDel->next;

       nextLoop = objToDel != lst->last;

       if(lst->flags & LST_SMART)
         ((SOBJ *)objToDel)->owner = NULL;

       if(func)
         func(P_LSON(lst, objToDel));
      } while(nextLoop);
    }

   lst->last = NULL;
  }
}

// -----------------------------------------------------------------------------
void InitCntList(CNTLIST *lst, unsigned char flag, int delta)
{
 lst->last   = NULL;
 lst->flags  = flag;
 lst->delta  = delta;
#ifdef RSL_MD_FLAT
 lst->sect   = NULL;
#endif
}

// -----------------------------------------------------------------------------
#ifdef RSL_MD_FLAT
    // -----------------------------------------------------------------------------
    void InitCntListEx(CNTLIST *lst, unsigned char flag, int delta, void *sect)
    {
     InitCntList(lst, flag, delta);

     lst->sect = sect;
    }

    // -----------------------------------------------------------------------------
    void DoneSafeList(CNTLIST *lst, LCLRFUNC func)
    {
     DoneCntList(lst, func);

     RSL_DEL_CRS(lst)

    #ifdef RSL_MD_FLAT
     lst->sect = NULL;
    #endif
    }

    // -----------------------------------------------------------------------------
    void InitSafeList(CNTLIST *lst, unsigned char flag, int delta, void *sect)
    {
     InitCntListEx(lst, flag, delta, sect);

     RSL_INIT_CRS(lst)
    }
#endif


// April 25,1996

// -----------------------------------------------------------------------------
struct ListFndData
     {
      LSRTFUNC  func;
      void     *sortData;
      void     *prevObj;
      void     *objToAdd;
     };

// -----------------------------------------------------------------------------
static int fndFunc(void *testObj, struct ListFndData *data)
{
 if(data->func(testObj, data->objToAdd, data->sortData) > 0)
   return 1;

 data->prevObj = testObj;

 return 0;
}

// -----------------------------------------------------------------------------
void LobjInsertSort(CNTLIST *lst, void *obj, LSRTFUNC func, void *sortData)
{
 struct ListFndData  data;
 unsigned char       oldFlags;


 RSL_ENTER_CRS(lst)

 oldFlags = lst->flags;

 lst->flags |= LST_ADDBEG;

 data.func     = func;
 data.sortData = sortData;
 data.prevObj  = NULL;
 data.objToAdd = obj;

 LobjFirstThat(lst, (LCMPFUNC)fndFunc, &data);
 // it's return an object that grater than obj. So insert after prevObj
 // If prevObj == NULL, insert at begining of list

 LobjInsertAfter(lst, obj, data.prevObj);

 lst->flags = oldFlags;

 RSL_LEAVE_CRS(lst)
}

// -----------------------------------------------------------------------------
void SortCntList(CNTLIST *orig, LSRTFUNC func, void *sortData)
{
 CNTLIST  lst;


 RSL_ENTER_CRS(orig)

 lst.delta = orig->delta;
 lst.flags = LST_ADDBEG | LST_NOTOWN;
 lst.last  = orig->last;

 orig->last = NULL;

 if(lst.last)
  {
   LOBJ  *objToDel;
   LOBJ  *temp = lst.last->next;


   do
    {
     objToDel = temp;
     temp = objToDel->next;

// June 23,1997
     if(orig->flags & LST_SMART)
       ((SOBJ *)objToDel)->owner = NULL;

     LobjInsertSort(orig, P_LSON(&lst, objToDel), func, sortData);
    } while(objToDel != lst.last);

   lst.last = NULL;
  }

 RSL_LEAVE_CRS(orig)
}

// June 23,1997

// -----------------------------------------------------------------------------
void SobjInit(SOBJ *obj)
{
 obj->owner = NULL;
}

// -----------------------------------------------------------------------------
void SobjUnlink(SOBJ *obj)
{
 if(obj->owner)
   DetachObj(obj->owner, (LOBJ *)obj);
}

// -----------------------------------------------------------------------------
void *LobjExtractFirst(CNTLIST *lst)
{
 LOBJ  *obj, *prev;


 RSL_ENTER_CRS(lst)

 if(lst->last == NULL)
  {
   RSL_LEAVE_CRS(lst)

   return NULL;
  }

 obj  = lst->last->next;
 prev = lst->last;

 prev->next = obj->next;
 obj->next  = NULL;

// June 23,1997
 if(lst->flags & LST_SMART)
   ((SOBJ *)obj)->owner = NULL;

 if(lst->last == obj)
   lst->last = prev == obj ? NULL : prev;

 RSL_LEAVE_CRS(lst)

 return P_LSON(lst, obj);
}

// -----------------------------------------------------------------------------
void *LobjExtractAfter(CNTLIST *lst, void *o)
{
 LOBJ *obj, *prev;


 RSL_ENTER_CRS(lst)

 prev = P_LOBJ(lst, o);
 obj = prev->next;

 prev->next = obj->next;
 obj->next  = NULL;

// June 23,1997
 if(lst->flags & LST_SMART)
   ((SOBJ *)obj)->owner = NULL;

 if(lst->last == obj)
   lst->last = prev == obj ? NULL : prev;

 RSL_LEAVE_CRS(lst)

 return P_LSON(lst, obj);
}

// -----------------------------------------------------------------------------
LOBJ *_LobjFirst(CNTLIST *lst)
{
 LOBJ  *ob;


 RSL_ENTER_CRS(lst)

 ob = LobjFirst(lst);

 RSL_LEAVE_CRS(lst)

 return ob;
}

// -----------------------------------------------------------------------------
LOBJ *_LobjNext(CNTLIST *lst, LOBJ *ob)
{
 RSL_ENTER_CRS(lst)

 ob = (ob == lst->last ? NULL : ob->next);

 RSL_LEAVE_CRS(lst)

 return ob;
}

// -----------------------------------------------------------------------------
LOBJ *_LobjPrev(CNTLIST *lst, LOBJ *ob)
{
 RSL_ENTER_CRS(lst)

 ob = LobjPrevNull(lst, ob);

 RSL_LEAVE_CRS(lst)

 return ob;
}

// -----------------------------------------------------------------------------
void *_LobjGetObj(CNTLIST *lst, LOBJ *ob)
{
 return ob ? P_LSON(lst, ob) : NULL;
}


// -----------------------------------------------------------------------------
#ifdef  TEST
    // ------ Код тестирования ------------------------
    #include <alloc.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdio.h>


    // -----------------------------------------------------------------------------
    CNTLIST  lst, lst2;  // заводим два списка

    // -----------------------------------------------------------------------------
    typedef struct
          {
           SOBJ   d1;
           SOBJ   d2;
           char  *name;
           int    num;
          } MYOBJ;


    // -----------------------------------------------------------------------------
    void *AllocObj(size_t size)
    {
     MYOBJ  *ptr = calloc(1, size);


     if(!ptr)
      {
       printf("Out of memory!\n");

       exit(1);
      }

     return ptr;
    }

    // -----------------------------------------------------------------------------
    void FreeObj(void *ptr)
    {
     free(ptr);
    }

    // -----------------------------------------------------------------------------
    MYOBJ *AddMyobj(char *name, int n)
    {
     MYOBJ  *ptr = AllocObj(sizeof(MYOBJ));


     ptr->name = AllocObj(strlen(name) + 1);
     ptr->num  = n;

     strcpy(ptr->name, name);

     SobjInit(&ptr->d1);
     SobjInit(&ptr->d2);

     LobjInsert(&lst, ptr);
     LobjInsert(&lst2, ptr);

     return ptr;
    }

    // -----------------------------------------------------------------------------
    void FreeMyobj(MYOBJ *obj)
    {
     FreeObj(obj->name);

     SobjUnlink(&obj->d1);
     SobjUnlink(&obj->d2);

     FreeObj(obj);
    }

    // -----------------------------------------------------------------------------
    static int cmpfun(MYOBJ *ob, char *name)
    {
     if(strcmp(ob->name, name) == 0)
       return 1;

     return 0;
    }

    // -----------------------------------------------------------------------------
    #pragma warn -par
    static int itrfunc(MYOBJ *ob, void *d)
    {
     printf("%d\n", ob->num);

     return 0;
    }

    // -----------------------------------------------------------------------------
    #pragma warn -par
    int srtFunc(MYOBJ *ob1, MYOBJ *ob2, char *data)
    {
    //   printf ("%s\n",data);
      return ob1->num - ob2->num;
    //   return strcmp (ob1->name,ob2->name);
    }

    // -----------------------------------------------------------------------------
    #pragma warn +par
    void main(void)
    {
     int             n, i;
     MYOBJ          *ob;
     unsigned long   memstart, memfinish;


     memstart = coreleft();

     InitCntList(&lst, LST_SMART, offsetof(MYOBJ, d1));
     InitCntList(&lst2, LST_SMART | LST_NOTOWN | LST_ADDBEG, offsetof(MYOBJ, d2));

     AddMyobj("First",  5);
     AddMyobj("Second", 2);
     AddMyobj("Third",  3);
     AddMyobj("Forth",  4);
     AddMyobj("Fifth",  5);
     AddMyobj("Sixth",  6);

     LobjForEach(&lst, (LITRFUNC)itrfunc, NULL);

     printf("\n");
    //   LobjForEach   (&lst2,(LITRFUNC)itrfunc,NULL);
    //   printf ("\n");

     SortCntList(&lst, (LSRTFUNC)srtFunc, "SortData");

     LobjForEach(&lst, (LITRFUNC)itrfunc, NULL);

     ob = LobjFirstThat(&lst, (LCMPFUNC)cmpfun, "Third");

     n = LobjIndexOf(&lst, ob);
     n = LobjIndexOf(&lst2, ob);

    //   LobjRemove (&lst2,ob,(LCLRFUNC)FreeMyobj);
    //   LobjRemove (&lst,ob,(LCLRFUNC)FreeMyobj);

    // Теперь можно просто удалить объект, а он сам исключит себя из тех
    // списков в которые он входит!!!
     FreeMyobj(ob);

     n  = LobjNitems(&lst);
     n  = LobjNitems(&lst2);

     printf("\nNumber of items %d\n\n", n);

     for(i = 0; i < n; i++)
       {
        ob = LobjAt(&lst2, i);

        printf("-%s Ind1 = %d Ind2 = %d\n", ob->name, LobjIndexOf(&lst, ob), LobjIndexOf(&lst2, ob));
       }

     printf("\n");

     DoneCntList(&lst2, (LCLRFUNC)FreeMyobj);
     DoneCntList(&lst, (LCLRFUNC)FreeMyobj);

     memfinish = coreleft();

     printf("At start %lu At finish %lu Delta %lu\n", memstart, memfinish, memstart - memfinish);
    }
#endif

/* <-- EOF --> */
