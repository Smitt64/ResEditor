//-*--------------------------------------------------------------------------*-
//
// File Name   : rstring.h
//
// Copyright (c) 1991 - 2005 by R-Style Softlab.
// All Rights Reserved.
//
//-*--------------------------------------------------------------------------*-
// March 5, 1994 Sergei Kubrin (K78) - Create file
//-*--------------------------------------------------------------------------*-

//
// Char and string case conversion and string/char
// comparison routines using national locale
//

#ifndef __RSTRING_H
#define __RSTRING_H

#include "rsreslbrfn_global.h"
#include <stddef.h>
#include <string.h>

#include <rscomplr.h>
#include <platform.h>
/*#include "tooldef.h"
#include "rsfname.h"*/


#ifdef RSL_NO_STRCASE_CONV

    #define  strupr(s)  struprR(s)
    #define  strlwr(s)  strlwrR(s)

#endif // RSL_NO_STRCASE_CONV


#define  toupzs(s)  (struprR(s))

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 Conversion/comparison functions
*******************************************************************************/
typedef unsigned char  TStrCp;

#define  RSL_CPDOS  0
#define  RSL_CPWIN  1


#ifdef RSL_CHAR_ASCII

    #define  ccmpR(c1, c2)              ((unsigned char)(c1) - (unsigned char)(c2))
    #define  cicmpR(c1, c2)             ((unsigned char)(toupc(c1)) - (unsigned char)(toupc(c2)))
    #define  strcmpR(str1, str2)        (strcmp((str1), (str2)))
    #define  strncmpR(str1, str2, len)  (strncmp((str1), (str2), (len)))

#else
    //
    // Compare characters using unique-weight collating sequence for current locale
    //
    RSRESLBRFN_EXPORT int ccmpR(char c1, char c2);

    //
    // Compare characters using shared-weight collating sequence for current locale
    // (case-independent comparison)
    //
    RSRESLBRFN_EXPORT int cicmpR(char c1, char c2);

    //
    // Compare strings using unique-weight collating sequence for current locale
    //
    RSRESLBRFN_EXPORT int strcmpR(const char *str1, const char *str2);

    //
    // Compare strings up to given length using unique-weight collating
    // sequence for current locale
    //
    RSRESLBRFN_EXPORT int strncmpR(const char *str1, const char *str2, size_t len);

#endif // RSL_CHAR_ASCII

//
// Convert char to uppercase using current charset
//
RSRESLBRFN_EXPORT int toupc  (int ch);
RSRESLBRFN_EXPORT int toupcEx(int ch, TStrCp codePage);

//
// Convert char to lowercase using current charset
//
RSRESLBRFN_EXPORT int toDownc  (int ch);
RSRESLBRFN_EXPORT int toDowncEx(int ch, TStrCp codePage);

//
// Compare strings using shared-weight collating sequence for current locale
// (case-independent comparison)
//
RSRESLBRFN_EXPORT int stricmpR  (const char *str1, const char *str2);
RSRESLBRFN_EXPORT int stricmpREx(const char *str1, const char *str2, TStrCp codePage);

//
// Compare strings up to given length using shared-weight collating sequence
// for current locale (case-independent comparison)
//
RSRESLBRFN_EXPORT int strnicmpR  (const char *str1, const char *str2, size_t len);
RSRESLBRFN_EXPORT int strnicmpREx(const char *str1, const char *str2, size_t len, TStrCp codePage);

//
// Поиск подстроки без учета регистра
//
RSRESLBRFN_EXPORT const char *stristrR  (const char *str1, const char *str2);
RSRESLBRFN_EXPORT const char *stristrREx(const char *str1, const char *str2, TStrCp codePage);

//
// Convert string to uppercase using current charset
//
RSRESLBRFN_EXPORT char * struprR (char *str);
RSRESLBRFN_EXPORT char *struprREx(char *str, TStrCp codePage);

//
// Convert string to uppercase up to given length using current charset
//
RSRESLBRFN_EXPORT char * strnuprR (char *str, size_t len);
RSRESLBRFN_EXPORT char *strnuprREx(char *str, size_t len, TStrCp codePage);

//
// Convert string to lowercase using current charset
//
RSRESLBRFN_EXPORT char *strlwrR  (char *str);
RSRESLBRFN_EXPORT char *strlwrREx(char *str, TStrCp codePage);

//
// Convert string to lowercase up to given length using current charset
//
RSRESLBRFN_EXPORT char *strnlwrR  (char *str, size_t len);
RSRESLBRFN_EXPORT char *strnlwrREx(char *str, size_t len, TStrCp codePage);

RSRESLBRFN_EXPORT int isalphaR  (int ch);
RSRESLBRFN_EXPORT int isalphaREx(int ch, TStrCp codePage);
RSRESLBRFN_EXPORT int isalnumR  (int ch);
RSRESLBRFN_EXPORT int isalnumREx(int ch, TStrCp codePage);

/*******************************************************************************
 String/memory copy functions
*******************************************************************************/

RSRESLBRFN_EXPORT char *rsmemstrstripcpy (char *dest, const char *src, size_t sizetMaxD, size_t sizetMaxS);

RSRESLBRFN_EXPORT char *rsmempadcpy(char *dest, const char *src, size_t sizetMaxD, size_t sizetMaxS);

#define  szTrunCpy(dest, src, sizetMaxD, sizetMaxS)  rsmemstrstripcpy((dest), (src), (sizetMaxD), (sizetMaxS))
#define  TxtSCpy(dest, src, sizetMaxD, sizetMaxS)    rsmempadcpy((dest), (src), (sizetMaxD), (sizetMaxS))

//
// Remove trailing spaces
// dest and src can overlap (and can be the same)
//
RSRESLBRFN_EXPORT char *rstrimr(char *dest, const char *src);
RSRESLBRFN_EXPORT char *remrb(char *c);

//
// Remove leading spaces
// dest and src can overlap (and can be the same)
//
RSRESLBRFN_EXPORT char *rstriml(char *dest, const char *src);

#define  remlb(src, dest)  rstriml((dest), (src))


/*******************************************************************************
 Oem <-> Ansi conversion functions
*******************************************************************************/

RSRESLBRFN_EXPORT char *RslToOEM     (char *buff, const char *ansi);
RSRESLBRFN_EXPORT char *RslToANSI    (char *buff, const char *oem);
RSRESLBRFN_EXPORT char *RslToOEMBuff (char *buff, const char *ansi, size_t len);
RSRESLBRFN_EXPORT char *RslToANSIBuff(char *buff, const char *oem,  size_t len);

RSRESLBRFN_EXPORT char *RslCvtChar  (unsigned dstCp, char *dst, int dstSz, unsigned srcCp, const char *src, int srcSz);
RSRESLBRFN_EXPORT int   RslWide2Char(char *outStr, int outSz, const wchar_t *inStr, int inSz, unsigned cp);
RSRESLBRFN_EXPORT int   RslChar2Wide(wchar_t *outStr, int outSz, const char *inStr, int inSz, unsigned cp);

RSRESLBRFN_EXPORT bool     IsRsLocaleUsed(void);
RSRESLBRFN_EXPORT unsigned SetRslDefProgEncode(unsigned newEncode);


#ifdef __cplusplus
}
#endif

#endif //#ifndef __RSTRING_H

/* <-- EOF --> */
