#ifndef LBROBJECTHEADERS_H
#define LBROBJECTHEADERS_H

#include <QtCore>
#include <packpsh1.h>

#define RESNAMELEN 9
#define RESFILELEN 255

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

#include <packpop.h>

#endif // LBROBJECTHEADERS_H
