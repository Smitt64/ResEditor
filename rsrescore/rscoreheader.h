#ifndef RSCOREHEADER_H
#define RSCOREHEADER_H

#include <io.h>

#define  dHRES_NAME            9  // Длина наименования ресурса в LBR
#define  dHRES_COMMENT       136  // Длина комментария к ресурсу (с 0-символом)
#define  dHRES_ITEM_NAME     101  // Длина наименования пункта меню (с
#define  P_NAME2ALLOCED    0x00100000   // Рзмер поля name включён в общий размер панели. Флаг устанавливается в redit
                                    // при сохранении ресурса
#define  RFP_REJREQ        0x00000004
#define  RFP_UPDREQ        0x00000008
#define  RFP_DELREQ        0x00000010

#define  RFP_CHNGF         0x00000020   // ширина поля скроллинга была изменена
                                        // динамически в EasyWin

#define  RFP_NOCLOSE       0x00000040   // для EasyWin

#define  RFP_RIGHTTEXT     0x00000080   // все тексты в панели имеют правое выравнивание
#define  RFP_TRANSPARENT   0x00000100   // Не используется (см. RFP_CENTERED)
#define  RFP_ANIMATED      0x00000200   // Не используется
#define  RFP_FLOATING      0x00000400
#define  RFP_PINX          0x00000800
#define  RFP_PINY          0x00001000
#define  RFP_NOAUTODIR     0x00002000
#define  RFP_NOAUTONUM     0x00004000
#define  RFP_NOSHADOW      0x00008000

#define  RFP_CENTERED      0x00000100   // Вместо RFP_TRANSPARENT

// This flags is used by BSCROL
#define  RFP_AUTOFILL      0x00010000   // расширять панель и обл. скролинга до экрана
#define  RFP_AUTOFIELDS    0x00020000   // автоматически устанавливать кординать полей в скролинге
#define  RFP_AUTOHEAD      0x00040000   // генерировать заголовки колонок в скролинге
#define  RFP_REVERSORDER   0x00080000   // обратный порядок записей
#define  RFP_DENYSORT      0x01000000   // запретить сортировку скролинга

enum TFVT
{
    FT_INT16,         // 0
    FT_INT32,         // 1
    FT_FLOAT,         // 2
    FT_FLOATG,        // 3
    FT_DOUBLE,        // 4
    FT_DOUBLEG,       // 5

    FT_MONEY,     // 6
    FT_STRING,        // 7
    FT_SNR,           // 8
    FT_DATE,          // 9
    FT_TIME,          // 10
    FT_SHTM,          // 11
    FT_CHAR,          // 12
    FT_UCHAR,         // 13

    FT_LMONEY,    // 14
    FT_LMONEYR,   // 15
    FT_MONEYR,    // 16

    FT_LDOUBLE10,     // 17

    FT_NUMSTR,        // 18

    FT_DECIMAL,   // 19

    FT_MONEY_OLD,     // 20
    FT_LMONEY_OLD,    // 21
    FT_MONEYR_OLD,    // 22
    FT_LMONEYR_OLD,   // 23

    FT_BINARY,        // 24
    FT_NUMERIC,       // 25
    //FT_BTRDEC,        // 26   // Btrieve decimal type (15 bytes) Used only as Btriev Data field
    FT_INT64,         // 27
    FT_PICTURE,       // 28

    FT_NATIVE = 0x80,

    FT_LDOUBLE = FT_LDOUBLE10,

    FT_INT16_NATIVE     = FT_INT16     | FT_NATIVE,
    FT_INT32_NATIVE     = FT_INT32     | FT_NATIVE,
    FT_FLOAT_NATIVE     = FT_FLOAT     | FT_NATIVE,
    FT_FLOATG_NATIVE    = FT_FLOATG    | FT_NATIVE,
    FT_DOUBLE_NATIVE    = FT_DOUBLE    | FT_NATIVE,
    FT_DOUBLEG_NATIVE   = FT_DOUBLEG   | FT_NATIVE,
    FT_LDOUBLE_NATIVE   = FT_LDOUBLE   | FT_NATIVE,
    FT_LDOUBLE10_NATIVE = FT_LDOUBLE10 | FT_NATIVE,
    FT_MONEY_NATIVE     = FT_MONEY     | FT_NATIVE,
    FT_MONEYR_NATIVE    = FT_MONEYR    | FT_NATIVE,
    FT_LMONEY_NATIVE    = FT_LMONEY    | FT_NATIVE,
    FT_LMONEYR_NATIVE   = FT_LMONEYR   | FT_NATIVE,
    FT_DECIMAL_NATIVE   = FT_DECIMAL   | FT_NATIVE,
    FT_NUMERIC_NATIVE   = FT_NUMERIC   | FT_NATIVE,
    FT_DATE_NATIVE      = FT_DATE      | FT_NATIVE,
    FT_TIME_NATIVE      = FT_TIME      | FT_NATIVE,
    FT_SHTM_NATIVE      = FT_SHTM      | FT_NATIVE,
    FT_INT64_NATIVE     = FT_INT64     | FT_NATIVE
};

typedef signed char    r_coord;
typedef short          coord;
typedef signed long ResInt32;
typedef unsigned long ResUInt32;
typedef signed short ResInt16;
typedef unsigned short ResUInt16;
typedef unsigned char ResUChar;

#include <packpsh1.h>
typedef struct PanelR_0
{
    ResUInt16    St;
    ResUChar        Pnumf, Pnumt, Nb, Nt;
    r_coord      x1, y1, x2, y2;
    ResInt16     PHelp;
    ResUChar        Pff;

    ResInt16     Mn;
    r_coord      x, y, l, h;

    ResInt16     len;
} PanelR_0;

typedef struct PanelR_1
{
    ResUInt16    St;
    ResUChar        Pnumf, Pnumt, Nb, Nt;
    r_coord      x1, y1, x2, y2;
    ResInt16     PHelp;
    ResUChar        Pff;

    ResInt16     Mn;
    r_coord      x, y, l, h;

    ResInt16     len;

    ResUInt32    flags;  // RFP_... flags
} PanelR_1;

typedef struct PanelR
{
    ResUInt16    St;
    ResUChar        Pnumf, Pnumt, Nb, Nt;
    r_coord      x1, y1, x2, y2;
    ResInt16     PHelp;
    ResUChar        Pff; // first field

    ResInt16     Mn;
    r_coord      x, y, l, h;

    ResInt16     len;

    ResUInt32    flags;    // RFP_... flags
    ResUInt32    flagsEx1, // ╨хчхЁт
    flagsEx2; // ╨хчхЁт
} PanelR;

typedef struct TextR
{
    ResInt16      St;
    r_coord        x, y;

    r_coord        vfl, lens;
} TextR;

typedef struct BordR
{
    ResInt16  St;
    r_coord    x, y, l, h, fl;
} BordR;

typedef struct FieldR_0
{
    ResUChar          Ftype;
    ResUInt16      St;
    ResUChar          FVt;
    ResInt16       FVp;
    r_coord        x, y, l, h;
    r_coord        kl, kr, ku, kd;
    ResInt16       FHelp;

    r_coord        vfl, lens;
} FieldR_0;

// -----------------------------------------------
typedef struct FieldR_1
{
    ResUChar          Ftype;
    ResUInt16      St;
    ResUChar          FVt;
    ResUInt16       FVp;
    r_coord        x, y, l, h;
    r_coord        kl, kr, ku, kd;
    ResUInt16       FHelp;

    r_coord        vfl, lens;

    r_coord        nameLen, formLen;
    ResUInt32      flags;
    ResUInt16       group;
} FieldR_1;

// -----------------------------------------------
typedef struct FieldR
{
    ResUChar          Ftype;
    ResUInt16      St;
    ResUChar          FVt;
    ResInt16       FVp;
    r_coord        x, y, l, h;
    r_coord        kl, kr, ku, kd;
    ResInt16       FHelp;

    r_coord        vfl, lens;

    r_coord        nameLen, formLen;
    ResUInt32      flags;
    ResInt16       group;
    r_coord        tooltipLen;         // ─ышэр ёЄЁюъш тёяы√тр■∙хщ яюфёърчъш
} FieldR;

#include <packpop.h>

#endif // RSCOREHEADER_H
