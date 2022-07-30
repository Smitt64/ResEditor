#ifndef __RES_H
#define __RES_H

#ifndef _RESH
#define _RESH

#include "rsreslbrfn_global.h"
//#include "pexpdef.h"

#ifndef RSL_FTIME_DEFINED
#define RSL_FTIME_DEFINED

#include <packpsh1.h>

struct ftime
{
    unsigned   ft_tsec  : 5;   /* Two second interval */
    unsigned   ft_min   : 6;   /* Minutes */
    unsigned   ft_hour  : 5;   /* Hours */
    unsigned   ft_day   : 5;   /* Days */
    unsigned   ft_month : 4;   /* Months */
    unsigned   ft_year  : 7;   /* Year */
};

#include <packpop.h>

#endif

#include <systypes.h>

#include "rslcvt.h"
#include "rstypes.h"
#include "listobj.h"

#ifdef __cplusplus
extern "C" {
#endif

//    Коды ошибок
#define  L_OPENLINK      1  // Не возможно открыть прилинкованый ресурс
#define  L_OPENLIB       2  // Не возможно открыть библиотеку ресурсов
#define  L_LIBTYPE       3  // Неверен формат файла
#define  L_DIRNOPEN      4  // Не возможно прочитать каталог, файл не открыт
#define  L_DIRNOMEM      5  // Не возможно прочитать каталог, нет памяти
#define  L_DIRREAD       6  // Ошибка чтения каталога
#define  L_DIRWR         7  // Ошибка записи каталога
#define  L_DIREMPTY      8  // Каталог пуст
#define  L_DIRMISS       9  // Элемент каталога отсутствует
#define  L_DUPOB        10  // Дублирование элемента каталога
#define  L_POSLIB       11  // Не возможно спозиционироваться в библиотеке
#define  L_ADDMEM       12  // Не возможно добавить элемент , нет памяти
#define  L_RESGET       13  // Не возможно прочитать ресурс
#define  L_RESPUT       14  // Не возможно записать ресурс
#define  L_WRSIZE       15  // Размер записанного ресурса не равен запрошенному
#define  L_VERSION      16  // Неподдерживаемая версия ресурса
#define  L_TRNACTIV     17  // Transaction already activ
#define  L_TRN          18  // Error in transaction
#define  L_OPENFORWR    19  // Error open for write access denide
#define  L_STRMMODE     20  // Bad stream mode
#define  L_OLDFORMAT    21  // Устаревший формат файла

#define  L_LASTER       21  // Номер последней ошибки

//  Bit flags for ResOpen
#define  RO_USELINKED    1  // Позволяет использовать прилинкованный ресурс
#define  RO_READDIR      2  // При открытии ресурса автоматически читать оглавление
#define  RO_MODIFY       4  // Позволяет модифицировать ресурсы подразумевает RO_READDIR
#define  RO_CREATE       8  // Если ресурс-файл не существует, создать его подразумевает RO_MODIFY
#define  RO_NEW         16  // Используется совместно с RO_CRETE. Если ресурс-файл существует, то он удаляется
#define  RO_FLUSH       32  // Производить запись оглавления и очистку буферов после каждого изменения
#define  RO_TRN         64  // Use Transaction


//  Флаги состояния
#define  DIR_READ        1  // Каталог прочитан
#define  DIR_MODIFIED    2  // Коталог модифицирован
#define  FREE_READ       4  // Список свободных блоков прочитан
#define  NEED_FLUSH      8  // Необходимо выполнять flush при модификациях
#define  USE_TRN        16  // Update operations is transactional
#define  READ_MODE      32  // Read only mode

/*
   Resource file состоит из заголовка и следующих за ним объектов
   В заголовке содержится смещение объекта, который используется как
   каталог по этому же смещению записывается и новый объект
*/

#include <packpsh1.h>

//  Эта структура записана в начале файла
typedef struct tagHeader
      {
       char       password[7]; /*  Парольное слово  */
       db_int32   DataOffset;  /*  Смещение в библиотеке для записи нового элемента */
       db_uint16  NumElem;     /*  Количество объектов в библиотеке */
       db_uint16  ElemSize;    /*  Размер элемента каталога         */
       db_uint16  NumFree;     /*  Количество свободных блоков      */
       uchar      isTrn;       /*  File in transaction              */
       char       reserv[7];
      } Header;

//  Данная структура записана в начале каждого ресурса
//  Функции читающие и пишущие ресурсы могут получать доступ к
//  текущему ResHeader через функции
//  ResGetCurHeader, ResSetCurHeader. К текущему элементу каталога
//  через функцию ResGetCurPtr
typedef struct tagResHeaderOld
      {
       db_uint16      resSize;  /* Размер объекта   */
       unsigned char  reserv;   /* Object reserved  */
       unsigned char  ver;      /* Object version   */
       struct ftime   ftime;    /* Дата и время создания из io.h */
      } ResHeaderOld;

typedef struct tagResHeader
      {
       db_uint16      resSizeLo;  /* Размер объекта   */
       unsigned char  headVer;    /* Object header version  */
       unsigned char  ver;        /* Object version   */
       struct ftime   ftime;      /* Дата и время создания из io.h */
       db_uint16      resSizeHi;  /* Размер объекта Hi bits in header version 1 */
      } ResHeader;

#define  MAKESIZE(rh)        ((size_t)( ((rh)->resSizeLo) | ((size_t)((rh)->resSizeHi) ) << 16))
#define  RHEADSIZE(rh)       ((rh)->headVer == 0 ? sizeof(ResHeaderOld) : sizeof (ResHeader))
#define  RHEADSIZEFOR(size)  ((size) < 0xffff ? sizeof(ResHeaderOld) : sizeof (ResHeader))

//  Это стандарная часть элемент оглавления.
//  Все каталоги пользователя должны быть унаследованы от этой структуры
//  Смотри LibElem
typedef  struct tagResource
      {
       db_int32 offset;            /* Смещение объекта в библиотеке */
      } Resource;

#define P_RES(ptr)   ((Resource*)(ptr))

typedef struct
      {
       long  offset;   /* Смещение свободного блока в файле */
       long  size;
      } FreeBlock;

//  Для преобразования к функции сравнения каталогов
typedef int  (*RES_CMP)(Resource *rc1, Resource *rc2);
typedef void (*RES_CVT)(struct tagResFile *, HRSLCVT hcvt, Resource *);

#include <packpop.h>

#include <packpshn.h>

typedef struct tagResFile
      {
       Header      head;        /* Заголовок файла                          */
       int         hd;          /* Дескриптор файла. -1 если файл не открыт */
       Resource   *Directory;   /* Элементы оглавления                */
       FreeBlock  *freeblock;   /* Свободные блоки                    */
       RES_CMP     ResCmp;
                                /* Функция сравнения элементов каталога     */
       unsigned    flags;       /* Каталог прочитан,модифицирован ...       */
       long        base;        /* Смещение начала файла ресурсов           */

       char       *fileName;

       HRSLCVT     hcvtRd;
       HRSLCVT     hcvtWr;
       RES_CVT     cvtDirProc;
      } ResFile;


typedef ResFile *HRSLRESFILE;

typedef int (*RES_PUT)(HRSLRESFILE, void *);
typedef int (*RES_GET)(HRSLRESFILE, void **);

// Описание всех функций смотри в файле RES.DOC

RSRESLBRFN_EXPORT char      *ResError        (int stat);
RSRESLBRFN_EXPORT int        ResGetObject    (HRSLRESFILE rf, Resource *rc, void **data, RES_GET Get);
RSRESLBRFN_EXPORT int        ResPutObject    (HRSLRESFILE rf, Resource *rc, void *data, RES_PUT Put, long size);
RSRESLBRFN_EXPORT int        ResWritePrefix  (HRSLRESFILE rf, Resource *rc);
RSRESLBRFN_EXPORT int        ResReadPrefix   (HRSLRESFILE rf, Resource *rc);
RSRESLBRFN_EXPORT ResHeader *ResGetCurHeader (void);
RSRESLBRFN_EXPORT Resource  *ResGetCurPtr    (void);
RSRESLBRFN_EXPORT Resource  *ResSetCurPtr    (Resource *rc);
RSRESLBRFN_EXPORT ResFile   *ResFileGetCurPtr(void);
RSRESLBRFN_EXPORT ResFile   *ResFileSetCurPtr(ResFile *rf);
RSRESLBRFN_EXPORT void       ResSetCurHeader (void);
// RSRESLBRFN_EXPORT void         ResClear            (HRSLRESFILE rf);
RSRESLBRFN_EXPORT int        SetResModify    (int Modify);
RSRESLBRFN_EXPORT int        ResOpen         (ResFile *rt, const char *lib, const char *password, RES_CMP ResCmp,
                                      RES_CVT resCvt, int flags, ...);
RSRESLBRFN_EXPORT int        ResClose        (HRSLRESFILE rt);
RSRESLBRFN_EXPORT int        ResGetElem      (HRSLRESFILE rt, Resource *res);
RSRESLBRFN_EXPORT int        ResAddElem      (HRSLRESFILE rf, Resource *rc);
RSRESLBRFN_EXPORT int        ResPutDirectory (HRSLRESFILE rt);
RSRESLBRFN_EXPORT int        ResGetDirectory (HRSLRESFILE rt);
RSRESLBRFN_EXPORT int        ResFreeDirectory(HRSLRESFILE root);
RSRESLBRFN_EXPORT int        ResNumObjects   (HRSLRESFILE rf);
RSRESLBRFN_EXPORT int        ResDelObject    (HRSLRESFILE rf, Resource *res);

int ResReadPrefixEx (ResFile *rf, Resource *rc, ResHeader *rhead);


//  Механизм итерации по каталогу
typedef struct tagResIter
      {
       unsigned   limit;
       unsigned   size;
       char      *ptr;
       unsigned   current;
      } ResIter;

RSRESLBRFN_EXPORT void      ResSetIter(ResIter *iter, HRSLRESFILE rf);
RSRESLBRFN_EXPORT Resource *ResGetIter(ResIter *iter);

typedef struct tagMemStream
      {
       CNTLIST  pages;
       size_t curPos;
      } TMemStream;

size_t MemStremWrite    (TMemStream *st, char *buff, size_t sz);
size_t MemStremRead     (TMemStream *st, char *buff, size_t sz);
size_t MemStremGetSize  (TMemStream *st);
int    MemStremSetPos   (TMemStream *st, size_t pos);
int    MemStremGetPos   (TMemStream *st, size_t *pos);
void   MemStremInit     (TMemStream *st);
void   MemStremDone     (TMemStream *st);
int    MemStremSaveToRes(TMemStream *st, int hd);


typedef struct tagResStream
      {
       ResFile    *rf;
       int         mode;
       int         stat;
       long        size;
       Resource   *rc;
       TMemStream  memStrm;
       size_t      strmCurSize;
       bool        useMewStrm;
    // April 19,2001
       ResHeader   rhead;
       long        resLibPos;  // Position in LBR for read/write operations
      } ResStream;

#define  RES_MODE_CLOSED   0
#define  RES_MODE_READ     1
#define  RES_MODE_WRITE    2

RSRESLBRFN_EXPORT int ResOpenStream (ResStream *strm, ResFile *rf, Resource *rc, int mode, int ver, long size);
RSRESLBRFN_EXPORT int ResCloseStream(ResStream *strm);
RSRESLBRFN_EXPORT int ResWriteStream(ResStream *strm, void *buff, int size, int *);
RSRESLBRFN_EXPORT int ResReadStream (ResStream *strm, void *buff, int size, int *);

int ResSetPosStream (ResStream *strm, size_t pos);
int ResGetSizeStream(ResStream *strm, size_t *size);
int ResGetPosStream (ResStream *strm, size_t *pos);


//  ***************************************************************
//           ResLib  part

RSRESLBRFN_EXPORT char *MakeEXT(char *buff, char *name, char *ext);


// -----------------------------------------------------------------------------
#define  RESNAMELEN    9       // Длина наименования ресурса (в прикладном коде без 0-символа,
                               // а в библиотеке ресурсов - с 0-символом).
#define  RESFILELEN  255       // Длина наименования библиотеки ресурсов (может включать также
                               // относительный или абсолютный путь). По аналогии с RESNAMELEN
                               // место под 0-символ нужно выделять отдельно.


// -----------------------------------------------------------------------------
typedef struct tagLibElem
      {
       Resource        rc;
       char            name[RESNAMELEN];
       unsigned  char  type;
      } LibElem;

typedef struct tagLibStream
      {
       ResStream  derived;
       LibElem    rc;
       int        oldflags;
       bool       inTrnMode;
      } LibStream;

// -----------------------------------------------------------------------------
// Types of resources
#define  RES_MENU         1    // Меню
#define  RES_STAT         2    // Строка статюса
#define  RES_DIALOG       3    // Окно диалог
#define  RES_HIST         4    // Строки подсказок
#define  RES_REPORT       5    // Отчет для печати
#define  RES_BFSTRUCT     6    // Структура Btrieve файла
#define  RES_DBLINK       7    // Связь между экранной формой и файлом данных

#define  RES_PANEL        8
#define  RES_SCROL        9
#define  RES_REP         10
#define  RES_BS          11
#define  RES_LS          12
#define  RES_ACCEL       13
#define  RES_STRTABLE    14

#define  RES_MENU2       15

#define  RES_LAST       RES_MENU2

#define  RES_RSFORM     100
#define  RES_RSMENU     101
#define  RES_RSREPORT   102
#define  RES_IMGLST     104
#define  RES_RSAXEXP    110
#define  RES_RSCODEGEN  111

// Types of objects in RES_MENU resource
#define  RES_MITEM        1
#define  RES_MPOPUP       2
#define  RES_DELIM        3

// Types of objects in RES_DIALOG resource
#define  RES_CLUST        1
#define  RES_HVIEW        2
#define  RES_LABEL        3
#define  RES_TEXT         4
#define  RES_BUTN         5
#define  RES_SBUTN        6
#define  RES_LIST         7
#define  RES_BLIST        8
#define  RES_EDIT         9
#define  RES_CHAIN       10



RSRESLBRFN_EXPORT int  _OpenLib    (HRSLRESFILE rf, const char *fname, int flags);
RSRESLBRFN_EXPORT int  PutResObject(HRSLRESFILE rf, const char *name, unsigned char type, void *data, RES_PUT Put, long size);
RSRESLBRFN_EXPORT int  GetResObject(HRSLRESFILE rf, const char *name, unsigned char type, void **data, RES_GET Get);

RSRESLBRFN_EXPORT int  FindResObject  (HRSLRESFILE rf, const char *name, unsigned char type);
RSRESLBRFN_EXPORT int  DeleteResObject(HRSLRESFILE rf, const char *name, unsigned char type);
RSRESLBRFN_EXPORT int  RenameResObject(HRSLRESFILE rf, const char *name, unsigned char type, const char *newName);

RSRESLBRFN_EXPORT int  LibOpenStream (LibStream *strm, ResFile *rf, int mode, const char *name, unsigned char type,
                              int ver, long size);
RSRESLBRFN_EXPORT int  LibCloseStream(LibStream *strm, bool abort);
RSRESLBRFN_EXPORT int  LibWriteStream(LibStream *strm, void *buff, int size,int*);
RSRESLBRFN_EXPORT int  LibReadStream (LibStream *strm, void *buff, int size,int*);

#define  LibSetPosStream(pStrm, pos)    ResSetPosStream(&(pStrm)->derived, pos)
#define  LibGetPosStream(pStrm, pos)    ResGetPosStream(&(pStrm)->derived, pos)
#define  LibGetSizeStream(pStrm, size)  ResGetSizeStream(&(pStrm)->derived, size)

//----------------------------------------------------------------------
//      Построение каталога
typedef enum
      {
       RS_NAME, RS_TYPE, RS_DATE, RS_COMM
      } DirSort;

typedef struct
      {
       char           name[RESNAMELEN];
       unsigned char  type;
       long           size;
       struct ftime   ftime;
       unsigned char  flags;  // Пользовательские флаги
       unsigned char  ver;
      } RLibDirElem;

typedef struct
      {
       RLibDirElem  *Elem;
       int           NumElem;   // Количество заполненных элементов в списке
       int           alloced;   // Количество элементов под которое распределена память
       DirSort       sortby;    // Порядок сортировки
       int           cmtSize;
      } RLibDir;


#define  NEXT_DIR(elem, dir)   ((RLibDirElem *)((char *)((elem) + 1) + (dir)->cmtSize))
#define  CMT_PTR(elem)         ((char *)((elem) + 1))
#define  ELEM_PTR(dir, num)    ((RLibDirElem *)((char *)((dir)->Elem) + ((dir)->cmtSize + sizeof(RLibDirElem)) * (num)))


typedef int (*RLibDirElemFiltr)(RLibDirElem *);


RSRESLBRFN_EXPORT void SortDirectory  (RLibDir *dir, DirSort sortby);
RSRESLBRFN_EXPORT int  BuildDirectory (RLibDir *dir, HRSLRESFILE rf, RLibDirElemFiltr filt, int num, int cmtSize);
RSRESLBRFN_EXPORT void FreeDirectory  (RLibDir *dir);
RSRESLBRFN_EXPORT int  RemDirElem     (RLibDir *dir, int n);
RSRESLBRFN_EXPORT int  RemDirElemName (RLibDir *dir, const char *name, unsigned char type);
RSRESLBRFN_EXPORT int  AddDirElem     (RLibDir *dir, const char *name, unsigned char type, long size, struct ftime *ftime,
                               unsigned char ver, unsigned char flags, const char *cmt);
RSRESLBRFN_EXPORT void SetResLibBaseAddr(long addr);

#include <packpop.h>

#ifdef __cplusplus
}
#endif

#endif  // _RESH
#endif  // __RES_H

/* <-- EOF --> */
