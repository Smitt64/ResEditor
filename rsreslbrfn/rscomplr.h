/*******************************************************************************
 FILE         :   RSCOMPLR.H

 COPYRIGHT    :   R-Style Software Lab, 1998

 DESCRIPTION  :   Compiler configuration header for RS-Tools libraries

 PROGRAMMED BY:   Yakov E. Markovitch

 CREATION DATE:   29 Jun 1998

 VERSION INFO :   $Archive: /RS-Tools/Old_tools/tools/h/rscomplr.h $
                  $Revision: 19 $
                  $Date: 5.09.00 14:50 $
                  $Author: Kubrin $
                  $Modtime: 5.09.00 14:47 $
*******************************************************************************/

#ifndef __RSCOMPLR_H
#define __RSCOMPLR_H


#if defined(_MSC_VER)

    #define  RSL_NONSTD_UNIX_IO               1
    #define  RSL_NO_GLOBAL_ARGS               1
    #define  RSL_NO_LDOUBLE                   1
    #define  RSL_NO_LDOUBLE_CVT_FUN           1
    #define  RSL_NO_EX_RANDOM                 1
    #define  RSL_USE_CAST                     1
    #define  RSL_UNDERSCORE_CVT_FUN           1
    #define  RSL_STANDARD_TPL_SPECIALIZATION  1

#elif defined(__IBMC__) || defined(__IBMCPP__)

    #define  RSL_NO_GLOBAL_ARGS         1
    #define  RSL_NO_LDOUBLE             1
    #define  RSL_NO_LDOUBLE_CVT_FUN     1
    #define  RSL_NO_LDOUBLE_MATHLIB     1
    #define  RSL_NO_LOCALE_CONV         1
    #define  RSL_UNDERSCORE_CVT_FUN     1

    #ifdef __TOS_OS400__
        #define  RSL_NO_STRCASE_CONV    1
        #define  RSL_NO_EXPLICIT        1
        #define  RSL_NO_NAMESPACE       1
        #define  RSL_NO_NAMESPACE_STD   1
        #define  RSL_NO_NAMESPACE_STL   1
    #endif

    #define  RSL_NO_EX_RANDOM           1

    #if (__IBMCPP__ <= 300)
        #define  RSL_NO_NAMESPACE       1
        #define  RSL_NO_NAMESPACE_STD   1
        #define  RSL_NO_NAMESPACE_STL   1
    #endif

#elif defined(__SUNPRO_CC) || defined(__GNUC__)

    #define  RSL_NO_GLOBAL_ARGS               1
    #define  RSL_NO_LDOUBLE_CVT_FUN           1
    #define  RSL_NO_LDOUBLE_MATHLIB           1
    #define  RSL_NO_CVT_FUN                   1
    #define  RSL_NO_STRCASE_CONV              1
    #define  RSL_NO_LOCALE_CONV               1
    #define  RSL_NO_EX_RANDOM                 1
    #define  RSL_STANDARD_TPL_SPECIALIZATION  1

#else

    #error Unknown C/C++ compiler!

#endif


#ifdef __cplusplus
    #define   _rs_inline_  inline
#else
    #ifdef RSL_USE_CAST
        #undef  RSL_USE_CAST
    #endif

    #define  _rs_inline_
#endif

#ifndef _p_inline_
    #define  _p_inline_  _rs_inline_
#endif

#if !defined(RSL_NO_NAMESPACE_STD) && (defined(RSL_NO_NAMESPACE) || defined(_OLD_IOSTREAM_))
    #define  RSL_NO_NAMESPACE_STD  1
#endif

#endif // __RSCOMPLR_H

/* <-- EOF --> */