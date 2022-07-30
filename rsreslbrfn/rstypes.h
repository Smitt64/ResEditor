/*-----------------------------------------------------------------------*-

 File Name   : rstypes.h

 Sybsystem   :
 Description :

 Source      :

 Library(s)  :


 VERSION INFO: $Archive: /RS-Tools/Old_tools/tools/h/rstypes.h $
               $Revision: 31 $
               $Date: 6.09.00 15:25 $
               $Author: Kubrin $
               $Modtime: 16.08.00 18:05 $


 Copyright (c) 1991 - 1994 by R-Style Software Lab.
 All Rights Reserved.

-*- History -------------------------------------------------------------*-
 November 11,1996  Sergei Kubrin (K78) - Create file
-*-----------------------------------------------------------------------*/

#ifndef  __RSTYPES_H
#define  __RSTYPES_H

#include <limits.h>

// Поддержка сборки под MSVS 2017
#if defined(_MSC_VER) && (_MSC_VER >= 1900)
    #include <stdint.h>
#endif

#include <systypes.h>
#include <platform.h>
/*#include <rsdbhd.hpp>
#include <rsdbtype.hpp>
#include "int64.h"*/


// [!!!:x64] Закомментировано для проверки сборки под Win64
/*
#if defined(RSL_MD_SUN) || defined(RSL_PL_WIN64)
    #define  NEED_DB_TYPES
#endif
*/


/*******************************************************************************
 db_float, db_basefloat
 db_double, db_basedouble

 max, min and zero value
*******************************************************************************/

#if defined(TYPES_FOR_CODECHECK)

    typedef struct db_float
          {
           char  a[4];
          } db_float;

    typedef struct db_basefloat
          {
           char  a[4];
          } db_basefloat;

    typedef struct db_double
          {
           char  a[8];
          } db_double;

    typedef struct db_basedouble
          {
           char  a[8];
          } db_basedouble;

#elif defined(NEED_DB_TYPES)

    #include "rsfloat.h"

#else
    typedef float        db_float;  
    typedef float        db_basefloat;  

    typedef double       db_double;
    typedef double       db_basedouble;
#endif

#ifndef FLOAT_MAX
    #define  FLOAT_MAX    ((float)(3.40282347E+38))
#endif

#ifndef FLOAT_MIN
    #define  FLOAT_MIN    ((float)(-3.40282347E+38))  // ((float)(1.17549435E-38))
#endif

#ifndef FLOAT_ZERO
    #define  FLOAT_ZERO   ((float)0.0)
#endif

#if defined(SQLBUILD)
    #define  SQL_DOUBLE_MAX  ((double)(9.0E+125))
    #define  SQL_DOUBLE_MIN  ((double)(-9.0E+125))  // ((double)(1.0E-130))
#else
    #define  BTR_DOUBLE_MAX  ((double)(1.7976931348623158E+308))
    #define  BTR_DOUBLE_MIN  ((double)(-1.7976931348623158E+308))  // ((double)(2.2250738585072014E-308))
#endif

#ifndef DOUBLE_MAX
    #ifndef SQLBUILD
        #define  DOUBLE_MAX   BTR_DOUBLE_MAX
    #else
        #define  DOUBLE_MAX   SQL_DOUBLE_MAX
    #endif
#endif

#ifndef DOUBLE_MIN
    #ifndef SQLBUILD
        #define  DOUBLE_MIN   BTR_DOUBLE_MIN
    #else
        #define  DOUBLE_MIN   SQL_DOUBLE_MIN
    #endif
#endif

#ifndef DOUBLE_ZERO
    #define  DOUBLE_ZERO  ((double)0.0)
#endif

#define  RSL_DOUBLE_DIGITS  16

#define  DB_FLOAT_MAX    ((db_float)FLOAT_MAX)
#define  DB_FLOAT_MIN    ((db_float)FLOAT_MIN)
#define  DB_FLOAT_ZERO   ((db_float)FLOAT_ZERO)

#define  DB_DOUBLE_MAX   ((db_double)DOUBLE_MAX)
#define  DB_DOUBLE_MIN   ((db_double)DOUBLE_MIN)
#define  DB_DOUBLE_ZERO  ((db_double)DOUBLE_ZERO)


/*******************************************************************************
 db_double10
 db_basedouble10

 max, min and zero value
*******************************************************************************/

#if defined(TYPES_FOR_CODECHECK)

    typedef struct db_double10
          {
           char  a[10];
          } db_double10;

    typedef struct db_basedouble10
          {
           char  a[10];
          } db_basedouble10;

    #ifndef DOUBLE10_MAX
        #define  DOUBLE10_MAX    ((long double)0.0)
    #endif

    #ifndef DOUBLE10_MIN
        #define  DOUBLE10_MIN    ((long double)0.0)
    #endif

    #ifndef DOUBLE10_ZERO
        #define  DOUBLE10_ZERO   ((long double)0.0)
    #endif

    #define  RSL_DOUBLE10_DIGITS  18

#elif defined(RSL_MD_SUN)

    #include "rsldoubl.h"

    #ifndef DOUBLE10_MAX
        #define  DOUBLE10_MAX    ((double)(1.7976931348623158E+308))
    #endif

    #ifndef DOUBLE10_MIN
        #define  DOUBLE10_MIN    ((double)(-1.7976931348623158E+308))  // ((double)(2.2250738585072014E-308))
    #endif

    #ifndef DOUBLE10_ZERO
        #define  DOUBLE10_ZERO   ((double)0.0)
    #endif

    #define  RSL_DOUBLE10_DIGITS  16

#elif defined(RSL_PL_MS)

    /*#include "rsldoubl.h"

    #ifndef DOUBLE10_MAX
        #define  DOUBLE10_MAX    ((long double)(1.7976931348623158E+308))
    #endif

    #ifndef DOUBLE10_MIN
        #define  DOUBLE10_MIN    ((long double)(-1.7976931348623158E+308))  // ((long double)(2.2250738585072014E-308))
    #endif

    #ifndef DOUBLE10_ZERO
        #define  DOUBLE10_ZERO   ((long double)0.0)
    #endif

    #define  RSL_DOUBLE10_DIGITS  16*/

#endif //defined(RSL_PL_MS)

#if defined(DB_DBL10_TYPES)

    #define  DB_DOUBLE10_MAX    ((db_double10)DOUBLE10_MAX)
    #define  DB_DOUBLE10_MIN    ((db_double10)DOUBLE10_MIN)
    #define  DB_DOUBLE10_ZERO   ((db_double10)DOUBLE10_ZERO)

#endif


/*******************************************************************************
 db_ldouble
 db_lbasedouble

 max, min and zero value
*******************************************************************************/

#if defined(SQLBUILD)

    typedef db_double       db_ldouble;
    typedef db_basedouble   db_lbasedouble;

    #ifndef LDOUBLE_MAX
        #define  LDOUBLE_MAX     DOUBLE_MAX
    #endif

    #ifndef LDOUBLE_MIN
        #define  LDOUBLE_MIN     DOUBLE_MIN
    #endif

    #ifndef LDOUBLE_ZERO
        #define  LDOUBLE_ZERO    DOUBLE_ZERO
    #endif

    #define  RSL_LDOUBLE_DIGITS  RSL_DOUBLE_DIGITS
#else //#if defined(SQLBUILD)

    #if defined(DB_DBL10_TYPES)
        typedef db_double10       db_ldouble;
        typedef db_basedouble10   db_lbasedouble;
    #else
        #define  NO_DB_LDBL_TYPES
    #endif

    #ifndef LDOUBLE_MAX
        #define  LDOUBLE_MAX     DOUBLE10_MAX
    #endif

    #ifndef LDOUBLE_MIN
        #define  LDOUBLE_MIN     DOUBLE10_MIN
    #endif

    #ifndef LDOUBLE_ZERO
        #define  LDOUBLE_ZERO    DOUBLE10_ZERO
    #endif

    #define  RSL_LDOUBLE_DIGITS  RSL_DOUBLE10_DIGITS
#endif //#if defined(SQLBUILD)

// -----------------------------------------------------------------------------
#define  DB_LDOUBLE_MAX   ((db_ldouble)LDOUBLE_MAX)
#define  DB_LDOUBLE_MIN   ((db_ldouble)LDOUBLE_MIN)
#define  DB_LDOUBLE_ZERO  ((db_ldouble)LDOUBLE_ZERO)


/*******************************************************************************
 db_int8, db_baseint8
 db_uint8, db_ubaseint8

 max and min value
*******************************************************************************/

typedef int8    db_int8;
typedef uint8   db_uint8;

typedef int8    db_baseint8;
typedef uint8   db_ubaseint8;

#ifndef INT8_MIN
    #define  INT8_MIN   SCHAR_MIN
#endif

#ifndef INT8_MAX
    #define  INT8_MAX   SCHAR_MAX
#endif

#ifndef UINT8_MAX
    #define  UINT8_MAX  UCHAR_MAX
#endif

#define  DB_INT8_MAX    ((db_int8)INT8_MAX)
#define  DB_INT8_MIN    ((db_int8)INT8_MIN)
#define  DB_INT8_ZERO   ((db_int8)0)

#define  DB_UINT8_MAX   ((db_uint8)UINT8_MAX)
#define  DB_UINT8_ZERO  ((db_uint8)0)


/*******************************************************************************
 db_int16, db_baseint16
 db_int32, db_baseint32
 db_int64, db_baseint64
 db_uint16, db_ubaseint16
 db_uint32, db_ubaseint32
 db_uint64, db_ubaseint64

 max and min value
*******************************************************************************/

#if defined(TYPES_FOR_CODECHECK)
    typedef struct db_int16
          {
           char  a[2];
          } db_int16;

    typedef struct db_int32
          {
           char  a[4];
          } db_int32;

    typedef struct db_int64
          {
           char  a[8];
          } db_int64;
                       
    typedef struct db_uint16
          {
           char  a[2];
          } db_uint16;

    typedef struct db_uint32
          {
           char  a[4];
          } db_uint32;

    typedef struct db_uint64
          {
           char  a[8];
          } db_uint64;

    typedef struct db_baseint16
          {
           char  a[2];
          } db_baseint16;

    typedef struct db_baseint32
          {
           char  a[4];
          } db_baseint32;

    typedef struct db_baseint64
          {
           char  a[8];
          } db_baseint64;

    typedef struct db_ubaseint16
          {
           char  a[2];
          } db_ubaseint16;

    typedef struct db_ubaseint32
          {
           char  a[4];
          } db_ubaseint32;

    typedef struct db_ubaseint64
          {
           char  a[8];
          } db_ubaseint64;

#elif defined(NEED_DB_TYPES)

    #include "rsdigit.h"

#else
    //#include "int64.h"

    typedef int16     db_int16;
    typedef int32     db_int32;
    typedef int64_t   db_int64;
                       
    typedef uint16    db_uint16;
    typedef uint32    db_uint32;
    typedef uint64_t  db_uint64;

    typedef int16     db_baseint16;
    typedef int32     db_baseint32;
    typedef int64_t   db_baseint64;

    typedef uint16    db_ubaseint16;
    typedef uint32    db_ubaseint32;
    typedef uint64_t  db_ubaseint64;
#endif

// -----------------------------------------------------------------------------
#ifndef INT16_MIN
    #define  INT16_MIN   SHRT_MIN
#endif

#ifndef INT16_MAX
    #define  INT16_MAX   SHRT_MAX
#endif

#ifndef INT32_MIN
    #define  INT32_MIN   LONG_MIN
#endif

#ifndef INT32_MAX
    #define  INT32_MAX   LONG_MAX
#endif

#ifndef INT64_MIN
    #define  INT64_MIN   LLONG_MIN
#endif

#ifndef INT64_MAX
    #define  INT64_MAX   LLONG_MAX
#endif

#ifndef UINT16_MAX
    #define  UINT16_MAX  USHRT_MAX
#endif

#ifndef UINT32_MAX
    #define  UINT32_MAX  ULONG_MAX
#endif

#ifndef UINT64_MAX
    #define  UINT64_MAX  ULLONG_MAX
#endif

#define  DB_INT16_MAX    ((db_int16)INT16_MAX)
#define  DB_INT16_MIN    ((db_int16)INT16_MIN)
#define  DB_INT16_ZERO   ((db_int16)0)

#define  DB_INT32_MAX    ((db_int32)INT32_MAX)
#define  DB_INT32_MIN    ((db_int32)INT32_MIN)
#define  DB_INT32_ZERO   ((db_int32)0)

#define  DB_INT64_MAX    ((db_int64)INT64_MAX)
#define  DB_INT64_MIN    ((db_int64)INT64_MIN)
#define  DB_INT64_ZERO   ((db_int64)0)

#define  DB_UINT16_MAX   ((db_uint16)UINT16_MAX)
#define  DB_UINT16_ZERO  ((db_uint16)0)

#define  DB_UINT32_MAX   ((db_uint32)UINT32_MAX)
#define  DB_UINT32_ZERO  ((db_uint32)0)

#define  DB_UINT64_MAX   ((db_uint64)UINT64_MAX)
#define  DB_UINT64_ZERO  ((db_uint64)0)


/*******************************************************************************
 dmoney, basemoney, db_dmoney and db_basemoney
 lmoney, lbasemoney, db_lmoney and db_lbasemoney
 decimal, basedecimal, db_decimal and db_basedecimal

 max, min and zero value
*******************************************************************************/

#if defined(TYPES_FOR_CODECHECK)
    typedef struct dmoney
          {
           char  a[8];
          } dmoney;

    typedef struct basemoney
          {
           char  a[8];
          } basemoney;

    typedef struct db_dmoney
          {
           char  a[8];
          } db_dmoney;

    typedef struct db_basemoney
          {
           char  a[8];
          } db_basemoney;

    typedef struct lmoney
          {
           char  a[10];
          } lmoney;

    typedef struct lbasemoney
          {
           char  a[10];
          } lbasemoney;

    typedef struct db_lmoney
          {
           char  a[10];
          } db_lmoney;

    typedef struct db_lbasemoney
          {
           char  a[10];
          } db_lbasemoney;

    typedef struct decimal
          {
           char  a[10];
          } decimal;

    typedef struct basedecimal
          {
           char  a[10];
          } basedecimal;

    typedef struct db_decimal
          {
           char  a[10];
          } db_decimal;

    typedef struct db_basedecimal
          {
           char  a[10];
          } db_basedecimal;

    typedef struct FDecimal_t
          {
           char  a[8];
          } FDecimal_t;

    typedef struct DBFDecimal_t
          {
           char  a[8];
          } DBFDecimal_t;


    #define  DMONEY_MAX    0
    #define  LMONEY_MAX    0
    #define  DECIMAL_MAX   0

    #define  DMONEY_MIN    0
    #define  LMONEY_MIN    0
    #define  DECIMAL_MIN   0

    #define  DMONEY_ZERO   0
    #define  LMONEY_ZERO   0
    #define  DECIMAL_ZERO  0

    #define  RSL_CAST_TO_DBL(val)           ((double)(val))
    #define  RSL_CAST_TO_LDBL(val)          ((long double)(val))

    #define  RSL_CAST_MON_TO_DBL(dmon)      ((double)(dmon))
    #define  RSL_CAST_LMON_TO_LDBL(lmon)    ((long double)(lmon))

    #define  RSL_CAST_DEC_TO_DBL(dec)       ((double)(dec))
    #define  RSL_CAST_DEC_TO_LDBL(dec)      ((long double)(dec))

    #define  RSL_CAST_MON_FROM_DBL(val)     ((dmoney)(val))
    #define  RSL_CAST_LMON_FROM_LDBL(val)   ((lmoney)(val))

    #define  RSL_CAST_DEC_FROM_DBL(val)     ((decimal)(val))
    #define  RSL_CAST_DEC_FROM_LDBL(val)    ((decimal)(val))

    #define  RSL_DECIMAL_CONST(n)  (n)

    #define  RSL_MONEY_DIGITS      16
    #define  RSL_MONEY_POINT        2

    #define  RSL_LMONEY_DIGITS     16
    #define  RSL_LMONEY_POINT       2

    #define  RSL_DECIMAL_DIGITS    18
    #define  RSL_DECIMAL_POINT      4

#elif defined(USE_FDECIMAL)

    #include "fdecimal.h"

    #if !defined(NUMERIC_AS_MONEY)
        //
        // Decimal Constant
        //
        #define  RSL_DECIMAL_CONST(n)  (FDecimal(#n))

        #define  RSL_MONEY_DIGITS      16
        #define  RSL_MONEY_POINT        2

        #define  RSL_LMONEY_DIGITS     16
        #define  RSL_LMONEY_POINT       2

        #define  RSL_DECIMAL_DIGITS    18
        #define  RSL_DECIMAL_POINT      4

        #ifdef __cplusplus
            typedef FMoney          dmoney;
            typedef BaseFMoney      basemoney;
            typedef DBFMoney        db_dmoney;
            typedef DBBaseFMoney    db_basemoney;

            typedef FMoney          lmoney;
            typedef BaseFMoney      lbasemoney;
            typedef DBFMoney        db_lmoney;
            typedef DBBaseFMoney    db_lbasemoney;

            typedef FDecimal        decimal;
            typedef BaseFDecimal    basedecimal;
            typedef DBFDecimal      db_decimal;
            typedef DBBaseFDecimal  db_basedecimal;
        #else
            typedef FDecimal_t      dmoney;
            typedef FDecimal_t      basemoney;
            typedef DBFDecimal_t    db_dmoney;
            typedef DBFDecimal_t    db_basemoney;

            typedef FDecimal_t      lmoney;
            typedef FDecimal_t      lbasemoney;
            typedef DBFDecimal_t    db_lmoney;
            typedef DBFDecimal_t    db_lbasemoney;

            typedef FDecimal_t      decimal;
            typedef FDecimal_t      basedecimal;
            typedef DBFDecimal_t    db_decimal;
            typedef DBFDecimal_t    db_basedecimal;
        #endif

        //
        // Max/min/zero values for money types
        //
        #define  DMONEY_MAX    (FMoney_MaxValue())
        #define  LMONEY_MAX    (FMoney_MaxValue())
        #define  DECIMAL_MAX   (FDecimal_MaxValue())

        #define  DMONEY_MIN    (FMoney_MinValue())
        #define  LMONEY_MIN    (FMoney_MinValue())
        #define  DECIMAL_MIN   (FDecimal_MinValue())

        #define  DMONEY_ZERO   (FMoney_ZeroValue())
        #define  LMONEY_ZERO   (FMoney_ZeroValue())
        #define  DECIMAL_ZERO  (FDecimal_ZeroValue())

        //
        // Convert to double and long double
        //

        #ifdef __cplusplus
            inline double        cvtToDouble(double                 v)  { return (double)v; }
            inline double        cvtToDouble(long double            v)  { return (double)v; }

            inline long double   cvtToLDouble(double                v)  { return (long double)v; }
            inline long double   cvtToLDouble(long double           v)  { return (long double)v; }

            inline double        cvtToDouble(const FDecimal        &v)  { return v.castDouble(); }
            inline double        cvtToDouble(const BaseFDecimal    &v)  { return v.castDouble(); }
            inline double        cvtToDouble(const DBBaseFDecimal  &v)  { return v.castDouble(); }

            inline double        cvtToDouble(const FMoney          &v)  { return v.castDouble(); }
            inline double        cvtToDouble(const BaseFMoney      &v)  { return v.castDouble(); }
            inline double        cvtToDouble(const DBBaseFMoney    &v)  { return v.castDouble(); }

            inline long double   cvtToLDouble(const FDecimal       &v)  { return v.castLDouble(); }
            inline long double   cvtToLDouble(const BaseFDecimal   &v)  { return v.castLDouble(); }
            inline long double   cvtToLDouble(const DBBaseFDecimal &v)  { return v.castLDouble(); }

            inline long double   cvtToLDouble(const FMoney         &v)  { return v.castLDouble(); }
            inline long double   cvtToLDouble(const BaseFMoney     &v)  { return v.castLDouble(); }
            inline long double   cvtToLDouble(const DBBaseFMoney   &v)  { return v.castLDouble(); }
        #endif   //#ifdef __cplusplus

        #ifndef SQLBUILD
            //
            // Casting to double and long double
            //
            #define  RSL_CAST_TO_DBL(val)        (cvtToDouble((val)))
            #define  RSL_CAST_TO_LDBL(val)       (cvtToLDouble((val)))

            #define  RSL_CAST_MON_TO_DBL(dmon)   (cvtToDouble((dmon)))
            #define  RSL_CAST_LMON_TO_LDBL(lmon) (cvtToLDouble((lmon)))

            #define  RSL_CAST_DEC_TO_DBL(dec)    (cvtToDouble((dec)))
            #define  RSL_CAST_DEC_TO_LDBL(dec)   (cvtToLDouble((dec)))

        #else // SQLBUILD

            #include "apptldef.h"

            #ifdef __cplusplus

                _APPTLFUNC void traceDecimalCvt(double          v, const char *event, const char *file, int line);
                _APPTLFUNC void traceDecimalCvt(long double     v, const char *event, const char *file, int line);
                _APPTLFUNC void traceDecimalCvt(const FMoney   &v, const char *event, const char *file, int line);
                _APPTLFUNC void traceDecimalCvt(const FDecimal &v, const char *event, const char *file, int line);

            #endif //#ifdef __cplusplus

            //
            // Casting to double and long double
            //
            #define  RSL_CAST_TO_DBL(val)        (traceDecimalCvt(val, "CastToDouble", __FILE__, __LINE__), cvtToDouble((val)))
            #define  RSL_CAST_TO_LDBL(val)       (traceDecimalCvt(val, "CastToLongDouble", __FILE__, __LINE__), cvtToLDouble((val)))

            #define  RSL_CAST_MON_TO_DBL(dmon)   (traceDecimalCvt(dmon, "CastMoneyToDouble", __FILE__, __LINE__), cvtToDouble((dmon)))
            #define  RSL_CAST_LMON_TO_LDBL(lmon) (traceDecimalCvt(lmon, "CastMoneyToLongDouble", __FILE__, __LINE__), cvtToLDouble((lmon)))

            #define  RSL_CAST_DEC_TO_DBL(dec)    (traceDecimalCvt(dec, "CastDecimalToDouble", __FILE__, __LINE__), cvtToDouble((dec)))
            #define  RSL_CAST_DEC_TO_LDBL(dec)   (traceDecimalCvt(dec, "CastDecimalToLongDouble", __FILE__, __LINE__), cvtToLDouble((dec)))

        #endif // SQLBUILD

        //
        // Casting from double and long double
        //
        #define  RSL_CAST_MON_FROM_DBL(val)     ((dmoney)(val))
        #define  RSL_CAST_LMON_FROM_LDBL(val)   ((lmoney)(val))

        #define  RSL_CAST_DEC_FROM_DBL(val)     ((decimal)(val))
        #define  RSL_CAST_DEC_FROM_LDBL(val)    ((decimal)(val))

    #else //#if !defined(NUMERIC_AS_MONEY)

    #endif //#if !defined(NUMERIC_AS_MONEY)

#else
    #if !defined(NUMERIC_AS_MONEY)

        typedef double          dmoney;
        typedef double          basemoney;
        typedef db_double       db_dmoney;
        typedef db_basedouble   db_basemoney;

        typedef long double     lmoney;
        typedef long double     lbasemoney;

        #if !defined(NO_DB_LDBL_TYPES)
            typedef db_ldouble      db_lmoney;
            typedef db_lbasedouble  db_lbasemoney;
        #endif

        typedef long double     decimal;
        typedef long double     basedecimal;

        #if !defined(NO_DB_LDBL_TYPES)
            typedef db_ldouble      db_decimal;
            typedef db_lbasedouble  db_basedecimal;
        #endif

        //
        // Max/min/zero values for money types
        //
        #define  DMONEY_MAX    DOUBLE_MAX
        #define  LMONEY_MAX    LDOUBLE_MAX
        #define  DECIMAL_MAX   LDOUBLE_MAX

        #define  DMONEY_MIN    (- DOUBLE_MAX)
        #define  LMONEY_MIN    (-LDOUBLE_MAX)
        #define  DECIMAL_MIN   (-LDOUBLE_MAX)

        #define  DMONEY_ZERO   DOUBLE_ZERO
        #define  LMONEY_ZERO   LDOUBLE_ZERO
        #define  DECIMAL_ZERO  LDOUBLE_ZERO

        //
        // Casting values for money types
        //
        #define  RSL_CAST_TO_DBL(val)          ((double) (val))
        #define  RSL_CAST_TO_LDBL(val)         ((long double) (val))

        #define  RSL_CAST_MON_TO_DBL(dmon)     ((double) (dmon))
        #define  RSL_CAST_LMON_TO_LDBL(lmon)   ((long double) (lmon))

        #define  RSL_CAST_DEC_TO_DBL(dec)      ((double) (dec))
        #define  RSL_CAST_DEC_TO_LDBL(dec)     ((long double) (dec))

        //
        // Casting from double and long double
        //
        #define  RSL_CAST_MON_FROM_DBL(val)    ((val))
        #define  RSL_CAST_LMON_FROM_LDBL(val)  ((val))

        #define  RSL_CAST_DEC_FROM_DBL(val)    ((val))
        #define  RSL_CAST_DEC_FROM_LDBL(val)   ((val))

        //
        // Decimal Constant
        //
        #define  RSL_DECIMAL_CONST(n)  (n)

        #define  RSL_MONEY_DIGITS      RSL_DOUBLE_DIGITS
        #define  RSL_MONEY_POINT       2

        #define  RSL_LMONEY_DIGITS     RSL_LDOUBLE_DIGITS
        #define  RSL_LMONEY_POINT      2

        #define  RSL_DECIMAL_DIGITS    RSL_LDOUBLE_DIGITS
        #define  RSL_DECIMAL_POINT     4

    #endif// !defined(NUMERIC_AS_MONEY)
#endif

// -----------------------------------------------------------------------------
#if !defined(NUMERIC_AS_MONEY)

    #define  DB_DMONEY_MAX    ((db_dmoney)DMONEY_MAX)
    #define  DB_DMONEY_MIN    ((db_dmoney)DMONEY_MIN)
    #define  DB_DMONEY_ZERO   ((db_dmoney)DMONEY_ZERO)

    #define  DB_LMONEY_MAX    ((db_lmoney)LMONEY_MAX)
    #define  DB_LMONEY_MIN    ((db_lmoney)LMONEY_MIN)
    #define  DB_LMONEY_ZERO   ((db_lmoney)LMONEY_ZERO)

    #define  DB_DECIMAL_MAX   ((db_decimal)DECIMAL_MAX)
    #define  DB_DECIMAL_MIN   ((db_decimal)DECIMAL_MIN)
    #define  DB_DECIMAL_ZERO  ((db_decimal)DECIMAL_ZERO)

#endif //defined(NUMERIC_AS_MONEY)


/*******************************************************************************
 Numeric, Numeric_t and DBNumeric

 max, min and zero value
*******************************************************************************/

#if defined(TYPES_FOR_CODECHECK)
    typedef struct Numeric
          {
           char  a[16];
          } Numeric;

    typedef struct Numeric_t
          {
           char  a[16];
          } Numeric_t;

    typedef struct DBNumeric
          {
           char  a[16];
          } DBNumeric;

    //
    // Numeric Constant
    //
    #define  RSL_NUMERIC_DIGITS     28

    #define  RSL_DB_NUMERIC_DIGITS  28
    #define  RSL_DB_NUMERIC_POINT   12

    #define  NUMERIC_MAX             0
    #define  NUMERIC_MIN             0
    #define  NUMERIC_ZERO            0

    #define  DB_NUMERIC_MAX          0
    #define  DB_NUMERIC_MIN          0
    #define  DB_NUMERIC_ZERO         0

#elif defined(USE_NUMERIC)

    #ifndef __RSNUMERIC_H
        #if !defined(RSL_RESTRICT_NUMERIC)
            #define RSL_RESTRICT_NUMERIC
        #endif

        #include "rsnumeric.h"
    #endif // __RSNUMERIC_H

    typedef Numeric96_t  Numeric_t;

    #ifdef __cplusplus

        typedef Numeric96InexactErr    NumericInexactErr;
        typedef Numeric96OverflowErr   NumericOverflowErr;
        typedef Numeric96DivByZeroErr  NumericDivByZeroErr;
        typedef Numeric96              Numeric;


        //-@H------ BEGIN _ DBBtrDecimal __________________________________ --------@@-

        _TOOLEXP void BtrDec2Numeric(const unsigned char *ptr, Numeric *num, int p);
        _TOOLEXP void Numeric2BtrDec(unsigned char *ptr, const Numeric *num, int p);
        _TOOLEXP Numeric GetLimitBtrDec(int p, bool positive);

        // 'Родные' функции преобразования Numeric <-> DBNumeric
        void Native_BtrDec2Numeric(const unsigned char *ptr, Numeric *num, int p);
        void Native_Numeric2BtrDec(unsigned char *ptr, const Numeric *num, int p);

        // 'Быстрые' функции преобразования Numeric <-> DBNumeric
        void Quick_BtrDec2Numeric(const unsigned char *ptr, Numeric *num, int p);
        void Quick_Numeric2BtrDec(unsigned char *ptr, const Numeric *num, int p);

        template <int decPoint>
        class DBBtrDecimal : public BtrDecimal_t
            {
             public:
               operator Numeric() const
               {
                Numeric  val;

                BtrDec2Numeric(data, &val, decPoint);

                return val;
               }

               DBBtrDecimal<decPoint> &operator = (const Numeric &val)
               {
                Numeric2BtrDec(data, &val, decPoint);

                return *this;
               }

               Numeric maxValue() const
               {
                return GetLimitBtrDec(decPoint, true);
               }

               Numeric minValue() const
               {
                return GetLimitBtrDec(decPoint, false);
               }

            // Arithmetic assignment operators -----------------------------------------------------------
               DBBtrDecimal<decPoint> &operator += (const Numeric &n)
               {
                *this = *this + n;

                return *this;
               }

               DBBtrDecimal<decPoint> &operator -= (const Numeric &n)
               {
                *this = *this - n;

                return *this;
               }

               DBBtrDecimal<decPoint> &operator *= (const Numeric &n)
               {
                *this = *this * n;

                return *this;
               }

               DBBtrDecimal<decPoint> &operator /= (const Numeric &n)
               {
                *this = *this / n;

                return *this;
               }
            };

        //-@@------ END ___________________________________________________ --------@E-

        #ifdef SQLBUILD
            typedef DBNumeric96      DBNumeric;
        #else
            typedef DBBtrDecimal<4>  DBNumeric;
        #endif

    #endif //__cplusplus

    //
    // Numeric Constant
    //
    #define  RSL_NUMERIC_DIGITS     28

    #define  RSL_DB_NUMERIC_DIGITS  28
    #define  RSL_DB_NUMERIC_POINT   12

    //
    // Max/min/zero values
    //
    #define  NUMERIC_MAX   (Numeric::maxValue())
    #define  NUMERIC_MIN   (Numeric::minValue())
    #define  NUMERIC_ZERO  (Numeric::zeroValue())

    #ifdef SQLBUILD
        #define  DB_NUMERIC_MAX      (DBNumeric::maxValue())
        #define  DB_NUMERIC_MIN      (DBNumeric::minValue())
        #define  DB_NUMERIC_ZERO     (DBNumeric::zeroValue())
    #else
        #define  DB_NUMERIC_MAXN(p)  (GetLimitBtrDec(p, true))
        #define  DB_NUMERIC_MINN(p)  (GetLimitBtrDec(p, false))
        #define  DB_NUMERIC_MAX      (GetLimitBtrDec(4, true))
        #define  DB_NUMERIC_MIN      (GetLimitBtrDec(4, false))
        #define  DB_NUMERIC_ZERO     (Numeric::zeroValue())
    #endif
#endif //USE_NUMERIC


// -----------------------------------------------------------------------------
#define  MKFVP(btrp, scrp)  ((btrp << 8) | scrp)


/*******************************************************************************
 dmoney, lmoney, decimal -> Numeric
*******************************************************************************/

#if defined(NUMERIC_AS_MONEY)

    #ifdef SQLBUILD
        typedef Numeric         dmoney;
        typedef Numeric_t       basemoney;

        typedef DBNumeric       db_dmoney;
        typedef DBNumeric       db_basemoney;

        typedef Numeric         lmoney;
        typedef Numeric_t       lbasemoney;

        typedef DBNumeric       db_lmoney;
        typedef DBNumeric       db_lbasemoney;

        typedef Numeric         decimal;
        typedef Numeric_t       basedecimal;

        typedef DBNumeric       db_decimal;
        typedef DBNumeric       db_basedecimal;
    #else //!SQLBUILD
        typedef Numeric          dmoney;
        typedef Numeric_t        basemoney;

        typedef DBBtrDecimal<4>  db_dmoney;
        typedef DBBtrDecimal<4>  db_basemoney;

        typedef Numeric          lmoney;
        typedef Numeric_t        lbasemoney;

        typedef DBBtrDecimal<4>  db_lmoney;
        typedef DBBtrDecimal<4>  db_lbasemoney;

        typedef Numeric          decimal;
        typedef Numeric_t        basedecimal;

        typedef DBBtrDecimal<4>  db_decimal;    
        typedef DBBtrDecimal<4>  db_basedecimal;
    #endif //!SQLBUILD

    //
    // Decimal Constant
    //
    #define  RSL_MONEY_DIGITS      RSL_DB_NUMERIC_DIGITS
    #define  RSL_LMONEY_DIGITS     RSL_DB_NUMERIC_DIGITS
    #define  RSL_DECIMAL_DIGITS    RSL_DB_NUMERIC_DIGITS

    #define  RSL_MONEY_POINT       RSL_DB_NUMERIC_POINT
    #define  RSL_LMONEY_POINT      RSL_DB_NUMERIC_POINT
    #define  RSL_DECIMAL_POINT     RSL_DB_NUMERIC_POINT

    //
    // Max/min/zero values
    //
    #define  DMONEY_MAX    NUMERIC_MAX
    #define  LMONEY_MAX    NUMERIC_MAX
    #define  DECIMAL_MAX   NUMERIC_MAX

    #define  DMONEY_MIN    NUMERIC_MIN
    #define  LMONEY_MIN    NUMERIC_MIN
    #define  DECIMAL_MIN   NUMERIC_MIN

    #define  DMONEY_ZERO   NUMERIC_ZERO
    #define  LMONEY_ZERO   NUMERIC_ZERO
    #define  DECIMAL_ZERO  NUMERIC_ZERO

    #define  DB_DMONEY_MAX   DB_NUMERIC_MAX
    #define  DB_DMONEY_MIN   DB_NUMERIC_MIN
    #define  DB_DMONEY_ZERO  DB_NUMERIC_ZERO

    #define  DB_LMONEY_MAX   DB_NUMERIC_MAX
    #define  DB_LMONEY_MIN   DB_NUMERIC_MIN
    #define  DB_LMONEY_ZERO  DB_NUMERIC_ZERO

    #define  DB_DECIMAL_MAX  DB_NUMERIC_MAX
    #define  DB_DECIMAL_MIN  DB_NUMERIC_MIN
    #define  DB_DECIMAL_ZERO DB_NUMERIC_ZERO

    //
    // Casting to double and long double
    //
    #define  RSL_CAST_TO_DBL(val)        (toDouble((val)))
    #define  RSL_CAST_TO_LDBL(val)       (toLongDouble((val)))

    #define  RSL_CAST_MON_TO_DBL(dmon)   (toDouble((dmon)))
    #define  RSL_CAST_LMON_TO_LDBL(lmon) (toLongDouble((lmon)))

    #define  RSL_CAST_DEC_TO_DBL(dec)    (toDouble((dec)))
    #define  RSL_CAST_DEC_TO_LDBL(dec)   (toLongDouble((dec)))

    //
    // Casting from double and long double
    //
    #define  RSL_CAST_MON_FROM_DBL(val)     (Numeric::from((val)))
    #define  RSL_CAST_LMON_FROM_LDBL(val)   (Numeric::from((val)))

    #define  RSL_CAST_DEC_FROM_DBL(val)     (Numeric::from((val)))
    #define  RSL_CAST_DEC_FROM_LDBL(val)    (Numeric::from((val)))

#endif //defined(NUMERIC_AS_MONEY)


/*******************************************************************************
 miscellaneous typedefs and defines
*******************************************************************************/

typedef unsigned char  uchar;
typedef signed char    r_coord;
typedef short          coord;


#ifndef _INTPTR_T_DEFINED
    #ifdef  RSL_PL_WIN64
        typedef __int64  intptr_t;
    #else
        typedef int      intptr_t;
    #endif

    #define _INTPTR_T_DEFINED
#endif

#ifndef _UINTPTR_T_DEFINED
    #ifdef  RSL_PL_WIN64
        typedef unsigned __int64  uintptr_t;
    #else
        typedef unsigned int      uintptr_t;
    #endif

    #define _UINTPTR_T_DEFINED
#endif

#if defined(TYPES_FOR_CODECHECK)
    #define  NULL  RSL_VOID_NULL
#endif

#define  UCHAR_MIN   0
#define  USHRT_MIN   0
#define  UINT_MIN    0
#define  ULONG_MIN   0

#define  UINT8_MIN   0
#define  UINT16_MIN  0
#define  UINT32_MIN  0
#define  UINT64_MIN  0

#endif   // __RSTYPES_H

/* <-- EOF --> */
