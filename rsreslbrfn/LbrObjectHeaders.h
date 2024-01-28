#ifndef LBROBJECTHEADERS_H
#define LBROBJECTHEADERS_H

#include <QtCore>
#include <packpsh1.h>

#define RESNAMELEN 9
#define RESFILELEN 255
#define dHRES_COMMENT       136  // Длина комментария к ресурсу (с 0-символом)

typedef signed char    r_coord;
typedef short          coord;

struct ftime
{
    unsigned   ft_tsec  : 5;   /* Two second interval */
    unsigned   ft_min   : 6;   /* Minutes */
    unsigned   ft_hour  : 5;   /* Hours */
    unsigned   ft_day   : 5;   /* Days */
    unsigned   ft_month : 4;   /* Months */
    unsigned   ft_year  : 7;   /* Year */
};

//  Эта структура записана в начале файла
typedef struct tagHeader
{
    char     password[7]; //  Парольное слово
    qint32   DataOffset;  //  Смещение в библиотеке для записи нового элемента
    quint16  NumElem;     //  Количество объектов в библиотеке
    quint16  ElemSize;    //  Размер элемента каталога
    quint16  NumFree;     //  Количество свободных блоков
    quint8   isTrn;       //  File in transaction
    char     reserv[7];
} Header;

// Данная структура записана в начале каждого ресурса
typedef struct tagResHeader
{
    quint16 resSizeLo;  // Размер объекта
    quint8  headVer;    //* Object header version
    quint8  ver;        // Object version
    ftime   ftime;      // Дата и время создания из io.h
    quint16 resSizeHi;  // Размер объекта Hi bits in header version 1
} ResHeader;

typedef struct tagResHeaderOld
{
    quint16 resSize;  /* Размер объекта   */
    unsigned char reserv;   /* Object reserved  */
    unsigned char ver;      /* Object version   */
    struct ftime ftime;    /* Дата и время создания из io.h */
} ResHeaderOld;

typedef struct tagResource
{
    qint32 offset; // Смещение объекта в библиотеке
} Resource;

typedef struct
{
    long offset; // Смещение свободного блока в файле
    long size;
} FreeBlock;

typedef struct tagLibElem
{
    Resource rc;
    char     name[RESNAMELEN];
    quint8   type;
} LibElem;

// Построение каталога
typedef struct
{
    char    name[RESNAMELEN];
    quint8  type;
    long    size;
    ftime   ftime;
    quint8  flags;  // Пользовательские флаги
    quint8  ver;
} RLibDirElem;

typedef enum
{
    RS_NAME, RS_TYPE, RS_DATE, RS_COMM
} DirSort;

typedef struct
{
    RLibDirElem  *Elem;
    int           NumElem;   // Количество заполненных элементов в списке
    int           alloced;   // Количество элементов под которое распределена память
    DirSort       sortby;    // Порядок сортировки
    int           cmtSize;
} RLibDir;


typedef struct tagResFile
{
    Header      head;        /* Заголовок файла                          */
    int         hd;          /* Дескриптор файла. -1 если файл не открыт */
    Resource   *Directory;   /* Элементы оглавления                */
    FreeBlock  *freeblock;   /* Свободные блоки                    */
    void     *ResCmp;
    /* Функция сравнения элементов каталога     */
    unsigned    flags;       /* Каталог прочитан,модифицирован ...       */
    long        base;        /* Смещение начала файла ресурсов           */

    char       *fileName;

    void     *hcvtRd;
    void     *hcvtWr;
    void     *cvtDirProc;
} ResFile;

typedef struct tagCNTLIST
{
    void           *last;
    int             delta;  // Смещение объекта LOBJ в элементе списка
    unsigned char   flags;

//#ifdef RSL_MD_FLAT
    void       *sect;
//#endif
} CNTLIST;

typedef struct tagMemStream
{
    CNTLIST  pages;
    size_t curPos;
} TMemStream;

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

typedef struct tagLibStream
{
    ResStream  derived;
    LibElem    rc;
    int        oldflags;
    bool       inTrnMode;
} LibStream;

#include <packpop.h>

#define P_RES(ptr)   ((Resource*)(ptr))
#define  RES_MODE_CLOSED   0
#define  RES_MODE_READ     1
#define  RES_MODE_WRITE    2

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

#endif // LBROBJECTHEADERS_H
