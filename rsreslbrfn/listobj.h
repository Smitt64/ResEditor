#if !defined(__LISTOBJ_H) /*&& !defined(__DLMINTF_H)*/
#define __LISTOBJ_H

#include <stddef.h>

//#include "tooldef.h"
#include "rsreslbrfn_global.h"
#include <packpshn.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
   LOBJ - базовый элемент для списковых структур. Он должен быть включен в создаваемые Вами 
   элементы данных, которые должны объединяться в списки. Если оъект должен входить в два 
   списка одновременно, то он должен иметь два подобъекта LOBJ.

   CNTLIST - это корень любого списка объектов содержащих LOBJ. Для каждого списка должен 
   существовать свой объект типа CNTLIST. Функции инициализации CNTLIST последним
   параметром передается смещение соответствующего подобъекта LOBJ в определенном элементе.

   delta == offsetof (MYLOBJ,lobj)

   Функция деинициализации удаляет все элементы из списка. При этом если не установлен 
   LST_NOTOWN флаг обладания объектами, то для каждого элемента вызывается функция очистки,
   указанная пользователем в качестве параметра (если она не NULL).
   ЗАМЕЧАНИЕ: если элемент одновременно входит в несколько списков, только один список может
              обладать объектами

   Для использования умных списков необходимо вместо LOBJ использовать SOBJ и при 
   инициализации списка указать флаг LST_SMART.
*/

typedef struct tagLOBJ  LOBJ;

struct tagLOBJ
     {
      LOBJ *next;
     };


// Smart list link object
typedef struct tagSOBJ  SOBJ;
struct tagSOBJ
     {
      SOBJ               *next;
      struct tagCNTLIST  *owner;
     };


typedef struct tagCNTLIST
      {
       LOBJ           *last;
       int             delta;  // Смещение объекта LOBJ в элементе списка
       unsigned char   flags;

       #ifdef RSL_MD_FLAT
           void       *sect;
       #endif
      } CNTLIST;
                                  
#ifndef BUILD_DLM
    #define  SobjFirstItem(sobj) _LobjGetObj(sobj->owner, _LobjFirst(sobj->owner))
    #define  SobjLastItem(sobj)  _LobjGetObj(sobj->owner, sobj->owner->last)

    #define  SobjNextItem(sobj)  _LobjGetObj(sobj->owner, _LobjNext(sobj->owner, (LOBJ *)sobj))
    #define  SobjPrevItem(sobj)  _LobjGetObj(sobj->owner, _LobjPrev(sobj->owner, (LOBJ *)sobj))
#endif

//------ Значения flags
#define  LST_DEFAULT 0   // Добавление в конец владение элементами
#define  LST_NOTOWN  1   // Список не владеет своими элементами
#define  LST_ADDBEG  2   // Новые элементы добавляются в голову списка

#define  LST_SMART   4   // Список хранит объекты SOBJ

#define  ADD_FLAGS(lst ,flg)  (lst)->flags |= (flg) 

typedef int  (*LCMPFUNC)(void *, void *);
typedef int  (*LITRFUNC)(void *, void *);
typedef void (*LCLRFUNC)(void *);

typedef int  ( *LSRTFUNC)(void *left, void *right, void *data);

#ifdef BUILD_DLM       
    void DLMAPI LobjInitList(CNTLIST *lst, int delta);
    void DLMAPI LobjDoneList(CNTLIST *lst, LCLRFUNC func);
#endif
               
#ifdef RSL_MD_FLAT
    RSRESLBRFN_EXPORT void InitSafeList (CNTLIST *lst, unsigned char flag, int delta, void *sect);
    RSRESLBRFN_EXPORT void DoneSafeList (CNTLIST *lst, LCLRFUNC func);
    RSRESLBRFN_EXPORT void InitCntListEx(CNTLIST *lst, unsigned char flag, int delta, void *sect);
#endif

RSRESLBRFN_EXPORT void InitCntList(CNTLIST *lst, unsigned char flag, int delta);
RSRESLBRFN_EXPORT void DoneCntList(CNTLIST *lst, LCLRFUNC func);

RSRESLBRFN_EXPORT void   LobjRemove   (CNTLIST *lst, void *obj, LCLRFUNC func);
RSRESLBRFN_EXPORT int    LobjForEach  (CNTLIST *lst, LITRFUNC func, void *args);
RSRESLBRFN_EXPORT int    LobjForEachL (CNTLIST *lst, LITRFUNC func, void *args);
RSRESLBRFN_EXPORT void * LobjFirstThat(CNTLIST *lst, LCMPFUNC func, void *args);
RSRESLBRFN_EXPORT void * LobjLastThat (CNTLIST *lst, LCMPFUNC func, void *args);
RSRESLBRFN_EXPORT void * LobjAt       (CNTLIST *lst, int index);
RSRESLBRFN_EXPORT int    LobjNitems   (CNTLIST *lst);
RSRESLBRFN_EXPORT void   LobjInsert   (CNTLIST *lst, void *obj);
RSRESLBRFN_EXPORT void * LobjPrevItem (CNTLIST *lst, void *obj);
RSRESLBRFN_EXPORT void * LobjNextItem (CNTLIST *lst, void *obj);
RSRESLBRFN_EXPORT void * LobjFirstItem(CNTLIST *lst);
RSRESLBRFN_EXPORT void * LobjLastItem (CNTLIST *lst);

// June 23, 1997
RSRESLBRFN_EXPORT void   SobjInit  (SOBJ *obj);
RSRESLBRFN_EXPORT void   SobjUnlink(SOBJ *obj);

RSRESLBRFN_EXPORT void  LobjDetach      (CNTLIST *lst, void *obj);
RSRESLBRFN_EXPORT void *LobjForEachEx   (CNTLIST *lst, LITRFUNC func, void *args, int *retval);
RSRESLBRFN_EXPORT void *LobjForEachLEx  (CNTLIST *lst, LITRFUNC func, void *args, int *retval);
RSRESLBRFN_EXPORT int   LobjIndexOf     (CNTLIST *lst, void *obj);
RSRESLBRFN_EXPORT void  LobjInsertAt    (CNTLIST *lst, void *obj, int at);
RSRESLBRFN_EXPORT void *LobjSetLast     (CNTLIST *lst, void *obj);
RSRESLBRFN_EXPORT void  LobjInsertBefore(CNTLIST *lst, void *ptr, void *Target);

// April 25,1996
RSRESLBRFN_EXPORT void  LobjInsertAfter(CNTLIST *lst, void *ptr, void *Targ);
RSRESLBRFN_EXPORT void  LobjInsertSort (CNTLIST *lst, void *obj, LSRTFUNC func, void *sortData);
RSRESLBRFN_EXPORT void  SortCntList    (CNTLIST *lst, LSRTFUNC func, void *sortData);


// October 14, 1997
RSRESLBRFN_EXPORT void *LobjExtractFirst(CNTLIST *lst);
RSRESLBRFN_EXPORT void *LobjExtractAfter(CNTLIST *lst, void *obj);


RSRESLBRFN_EXPORT LOBJ *_LobjFirst (CNTLIST *lst);
RSRESLBRFN_EXPORT LOBJ *_LobjNext  (CNTLIST *lst, LOBJ *ob);
RSRESLBRFN_EXPORT LOBJ *_LobjPrev  (CNTLIST *lst, LOBJ *ob);
RSRESLBRFN_EXPORT void *_LobjGetObj(CNTLIST *lst, LOBJ *ob);

#ifdef __cplusplus
}
#endif

#include <packpop.h>

#endif  //listobj_h

/* <-- EOF --> */
