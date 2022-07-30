/*-----------------------------------------------------------------------*-

 File Name   : platform.h

 Sybsystem   : TOOLS
 Description : Определение независимых от компилятора макросов
               для различных платформ.

               1) Платформы (различие по системе адресации,
               call-интерфейсу).

               Макросы платформ:

               RSL_PL_OS2     (Os/2 2.x)
               RSL_PL_WIN64   (Win64)
               RSL_PL_WIN32   (Win64,Win32,Win32 Console,Win32s,DPMI32)
               RSL_PL_WIN16   (Win16,DPMI16)
               RSL_PL_DOS     (Dos,Dos Overlay)
               RSL_PL_MS      (All Microsoft platforms)
               RSL_PL_AS400   (IBM AS/400)
               RSL_PL_UNIX    (All UNIX platforms)

               2) Модификаторы платформ (различия внутри платформы)

               Макросы модификаторы:

               RSL_MD_FLAT       (плоская адресация Win32,OS2,DPMI32,Os/2)
               RSL_MD_CONSOLE    (символьное приложение)
               RSL_MD_GUI        (графическое приложение)
               RSL_MD_DPMI16     (Borland extender 16 bit)
               RSL_MD_DPMI32     (Borland extender 32 bit)
               RSL_MD_OVERLAY    (оверлейная DOS программа)
               RSL_MD_MT         (многопотоковое приложение)
               RSL_MD_DLL        (динамическая библиотека)

               RSL_MD_SUN
               RSL_MD_HP

               3) Макросы размера указателей и др.

               RSL_NEAR          (near иказатель для 16 бит)
               RSL_FAR           (far указатель или объект 16 бит)
               RSL_LOCAL         (модификатор локальной функции)

               4) Макросы для исправления ошибки в DPMI32

               RSL_PUSHEBX
               RSL_POPEBX

               5) Определение хэндлов

               RSL_MAKE_HANDLE


               6) Processor type

               RSL_PRC_LITTLE_ENDIAN
               RSL_PRC_BIG_ENDIAN

               7) Character coding

               RSL_CHAR_ASCII
               RSL_CHAR_EBCDIC

               8) Platform standard alignment boundary

               RSL_STD_ALIGNMENT

               9) Platform standard path delimiters

               RSL_PATH_DELIMS
               RSL_PATH_NATIVE_DELIM

               10) RSFINALLY Use as:
                  try {
                      ...
                      }
                  RSFINALLY(
                      instruction;
                      instruction;
                      ....
                      )

               11) Function modifiers
                  RSL_CFUNC
                  extern "C" in C++ environment
                  empty in C environment

                  RSL_CFUNC_(proto_proc)
                  extern "C" { proto_proc; }   in C++ environment

                  RSL_CDECL
                  __cdecl on MS platform
                  empty in all others

               12) RSL_VOID_NULL

 Source      : platform.c

 Library(s)  :


 Version Info:   $Archive: /RS-Tools/Old_tools/tools/h/platform.h $
                 $Revision: 48 $
                 $Date: 7.09.00 20:06 $
                 $Author: Lebedev $
                 $Modtime: 7.09.00 20:04 $

 Copyright (c) 1991 - 1994 by R-Style JSC
 All Rights Reserved.

-*- History -------------------------------------------------------------*-
 March 16,1995  Sergei Kubrin (K78) - Create file
 May  16,1995 Serg Kubrin (K78)
    Поддержка убогая IBM C
 May 30,1996 Serg Kubrin (K78)
    Add GetSystemForWin32
 December 30,1996
    Add   RSL_MAKE_HANDLE
 February 24,1998 (K78)
    Add RSL_PL_AS400, RSL_PL_UNIX, RSL_MD_SUN, RSL_MD_HP

 13 Mar 1998 (Yakov E. Markovitch)
   RSL_PRC_LITTLE_ENDIAN
   RSL_PRC_BIG_ENDIAN

 9 Sep 1998 (Yakov E. Markovitch)
   RSL_STD_ALIGNMENT
   RSL_PACKED_SIZE()
   RSL_STD_PACKED_SIZE()

 2 Oct 1998 (Yakov E. Markovitch)
   RSL_PATH_DELIMS
   RSL_PATH_NATIVE_DELIM

 8 Oct 1998 (Yakov E. Markovitch)
   RSL_CFUNC
   RSL_CDECL
-*-----------------------------------------------------------------------*/

#ifndef __PLATFORM_H
#define __PLATFORM_H

#include <rscomplr.h>

#define  RSL_PL_CAPACITY  32

#if defined (_MSC_VER)  // Microsoft Visual C++ compiler

    #if defined (_BLDDLL) || defined (_WINDLL) || defined (_USRDLL)
        #define RSL_MD_DLL
    #endif

    #define RSL_PRC_LITTLE_ENDIAN    // Always on Intel processors

    #if defined (_WIN32)

        #define RSL_PL_WIN32
        #define RSL_MD_FLAT

        #if defined(_WIN64)
            #define RSL_PL_WIN64

            #undef   RSL_PL_CAPACITY       // Под Win64 выдает предупреждение о переопределении
            #define  RSL_PL_CAPACITY  64
        #endif


        #define  __EXPORT2  __declspec(dllexport)
        #define  __IMPORT2  __declspec(dllimport)
        #define  __THREAD2  __declspec(thread)

        #define  __EXPORT 
        #define  __IMPORT 
        #define  __THREAD 

        #if defined (_DLL)
            #define _RTLDLL
        #endif

        #if defined (_MT)
            #define RSL_MD_MT
        #endif

        #if defined (_CONSOLE)
            #define RSL_MD_CONSOLE
        #else
            #define RSL_MD_GUI
        #endif

    #elif defined (_WINDOWS)

        #define RSL_MD_GUI

        #define  __EXPORT __export
        #define  __IMPORT
        #define  __THREAD

        #if defined (_DLL)
            #define _RTLDLL
        #endif

        #if defined (_WINDLL)
            #define RSL_MD_DLL
        #endif

    #else   // DOS

        #define  RSL_PL_DOS
        #define  __EXPORT
        #define  __IMPORT
        #define  __THREAD

    #endif

          
    #if !defined(DEBUG) && defined(_DEBUG)
        #define DEBUG
    #endif

    #if defined(DEBUG) && !defined(_DEBUG)
        #define _DEBUG
    #endif

#elif defined (__IBMC__) || defined (__IBMCPP__) // IBM C++  compiler

    #ifdef __TOS_OS400__

        #define RSL_MD_FLAT
        #define RSL_PRC_BIG_ENDIAN

    #elif defined (_WIN32)

        #define RSL_PL_WIN32
        #define RSL_MD_FLAT

        #define  __EXPORT2  __declspec(dllexport)
        #define  __IMPORT2  __declspec(dllimport)
        #define  __THREAD2  __declspec(thread)

        #if defined (__DLL__)
            #define RSL_MD_DLL
        #endif

        #if defined (_MT)
            #define RSL_MD_MT
        #endif

        #if defined (__IMPORTLIB__)
            #define _RTLDLL
        #endif

    #else
        #define RSL_PL_OS2
        #define RSL_MD_FLAT
    #endif

#elif defined (__unix)

    #define RSL_MD_FLAT

    #if defined (__sun)
        #define RSL_MD_SUN
        #define RSL_PRC_BIG_ENDIAN
    #endif

    #if defined (__hpux)
        #define RSL_MD_HP
        #define RSL_PRC_BIG_ENDIAN
    #endif

#else  // _MSC_VER  (not defined)
    #error This compiler is not supported
#endif

// -----------------------------------------------------------------------------
#define RSL_CHAR_ASCII

#define  _RSPACKED

// -----------------------------------------------------------------------------
#ifdef RSL_MD_FLAT
    #define  RSL_LOCAL  _pascal
    #define  RSL_FAR
    #define  RSL_NEAR
#else
    #define  RSL_LOCAL  _near _pascal
    #define  RSL_FAR    _far
    #define  RSL_NEAR   _near
#endif

// -----------------------------------------------------------------------------
#ifdef RSL_MD_MT
    #define  _THREAD __THREAD
#else
    #define  _THREAD
#endif

// -----------------------------------------------------------------------------
#if defined(RSL_PL_WIN32) || defined(RSL_PL_DOS)
    #define RSL_PL_MS
#endif

// -----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

    typedef enum
          {
           PL_DPMI32 = 1,
           PL_WIN32,
           PL_WIN32S,
           PL_WINNT,
           PL_WIN95,
           PL_AS400  = 0x1000,
           PL_SUN
          } PlatformType;


    #ifdef RSL_PL_WIN32

        PlatformType GetPlatform(void);
        PlatformType GetSystemForWin32(void);
        void ForceLinkBorlandUnresolvedEntryHook(void);
        void FixLocaleForWindows7RC(void);

    #elif defined(RSL_MD_SUN)

        #define  GetPlatform()  (PL_SUN)

    #endif // RSL_PL_WIN32

#ifdef __cplusplus
}
#endif


// -----------------------------------------------------------------------------
#if !defined(RSL_BOOL_DEFINED) && (!defined(__cplusplus) || (defined(_MSC_VER) && (_MSC_VER < 1100)) || defined(__IBMCPP__))

    #ifdef ENUMINTSIZE
        #pragma option -b-
    #endif

    typedef char  bool;

    enum TBool
       {
        bool_false,
        bool_true
       };

    #ifdef true
        #undef true
    #endif

    #ifdef false
        #undef false
    #endif

    #define  true   bool_true
    #define  false  bool_false
    #define  RSL_BOOL_DEFINED

    #ifdef ENUMINTSIZE
        #pragma option -b
    #endif

#endif

// -----------------------------------------------------------------------------
#ifdef DYNAMIC_CAST
    #undef DYNAMIC_CAST
#endif

#ifdef STATIC_CAST
    #undef STATIC_CAST
#endif

#ifdef CONST_CAST
    #undef CONST_CAST
#endif

#ifdef REINTERPRET_CAST
    #undef REINTERPRET_CAST
#endif

// -----------------------------------------------------------------------------
#define  DYNAMIC_CAST(toClass, object)  dynamic_cast<toClass *>(object)

// -----------------------------------------------------------------------------
#ifdef  RSL_USE_CAST

    #define  STATIC_CAST(targetType, object)       static_cast<targetType>(object)
    #define  CONST_CAST(targetType, object)        const_cast<targetType>(object)
    #define  REINTERPRET_CAST(targetType, object)  reinterpret_cast<targetType>(object)

#else

    #define  STATIC_CAST(targetType, object)       ((targetType)(object))
    #define  CONST_CAST(targetType, object)        ((targetType)(object))
    #define  REINTERPRET_CAST(targetType, object)  ((targetType)(object))

#endif // RSL_USE_CAST

// -----------------------------------------------------------------------------         
#ifndef NO_GLOCAL
    #define  glocal  _THREAD
#else
    #define  glocal
#endif

#define  RSL_MAKE_HANDLE(name)  struct name##__ { int unused; }; typedef struct name##__ *name

// -----------------------------------------------------------------------------
#define  RSFINALLY(code)  catch (...) { code throw; } code

/*
 * Alignment constants and macros
 */

#if defined(RSL_PL_WIN64)
    #define  RSL_STD_ALIGNMENT           8
    #define  RSL_MIN_SAFE_ALIGNMENT      1
#elif defined(RSL_PL_OS2) || defined(RSL_PL_WIN32)
    #define  RSL_STD_ALIGNMENT           4
    #define  RSL_MIN_SAFE_ALIGNMENT      1
#elif defined(RSL_PL_DOS)
    #define  RSL_STD_ALIGNMENT           1
    #define  RSL_MIN_SAFE_ALIGNMENT      1
#else
    #error Unknown platform!
#endif

// -----------------------------------------------------------------------------
#define  RSL_PACKED_SIZE(sz, alg)    ((((sz) + (alg) - 1)/(alg))*(alg))
#define  RSL_STD_PACKED_SIZE(sz)     RSL_PACKED_SIZE((sz), RSL_STD_ALIGNMENT)
#define  RSL_SAFE_PACKED_SIZE(sz)    RSL_PACKED_SIZE((sz), RSL_MIN_SAFE_ALIGNMENT)


// -----------------------------------------------------------------------------
#define  RSL_PATH_DELIMS  "\\/"

#if defined(RSL_PL_MS)
    #define  RSL_PATH_NATIVE_DELIM  '\\'
#else // UNIX, etc.
    #define  RSL_PATH_NATIVE_DELIM  '/'
#endif

// -----------------------------------------------------------------------------
#ifdef __cplusplus
    #define  RSL_CFUNC               extern "C"
    #define  RSL_CFUNC_(proto_proc)  extern "C" { proto_proc; }
#else
    #define  RSL_CFUNC
    #define  RSL_CFUNC_(proto_proc)  proto_proc
#endif

#ifdef RSL_PL_MS
    #define  RSL_CDECL  __cdecl
#else
    #define  RSL_CDECL
#endif


/*
   NULL void pointer, regardless of C or C++
   The problem is that in C NULL is void *, in C++ NULL is int.
   So we need additional NULL pointer that is invariantly _pointer_
   (for passing as NULL parameter in ... part of functions with variable number
   of parameters)
*/
#define  RSL_VOID_NULL  ((void *)0)

// -----------------------------------------------------------------------------
#if !defined(__cplusplus) || defined(RSL_NO_NAMESPACE) || defined(RSL_NO_NAMESPACE_STD)
    #define  RSL_STD
    #define  RSL_USING_NAMESPACE_STD
#else
    #define  RSL_STD                  std
    #define  RSL_USING_NAMESPACE_STD  using namespace std
#endif

// -----------------------------------------------------------------------------
#if !defined(__cplusplus) || defined(RSL_NO_NAMESPACE) || defined(RSL_NO_NAMESPACE_STL)
    #define  RSL_STL
    #define  RSL_USING_NAMESPACE_STL
#else
    #define  RSL_STL                  std
    #define  RSL_USING_NAMESPACE_STL  using namespace std
#endif

// -----------------------------------------------------------------------------
#if !defined(__cplusplus) || defined(RSL_NO_NAMESPACE)
    #define RSL_BEGIN_NAMESPACE(nsname)
    #define RSL_END_NAMESPACE
    #define RSL_USING_NAMESPACE(nsname)
    #define RSL_NS(nsname)
#else
    #define RSL_BEGIN_NAMESPACE(nsname)  namespace nsname {
    #define RSL_END_NAMESPACE            }
    #define RSL_USING_NAMESPACE(nsname)  using namespace nsname
    #define RSL_NS(nsname)               nsname::
#endif

// -----------------------------------------------------------------------------
#if defined(RSL_NO_EXPLICIT)
    #define explicit
#endif

// -----------------------------------------------------------------------------
#if defined(__cplusplus) && defined(RSL_STANDARD_TPL_SPECIALIZATION)
    #define  RSL_TPL_SPECIALIZATION  template<>
#else
    #define  RSL_TPL_SPECIALIZATION
#endif

#endif  // __PLATFORM_H

/* <-- EOF --> */