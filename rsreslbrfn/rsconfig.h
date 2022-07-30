/*******************************************************************************
 FILE         :   RSCONFIG.H

 COPYRIGHT    :   R-Style Software Lab, 1998

 DESCRIPTION  :   Compatibility support for different compilers. Parts are moved
                  from mssup.h

 PROGRAMMED BY:   Yakov E. Markovitch

 CREATION DATE:   25 Jun 1998

 VERSION INFO :   $Archive: /RS-Tools/Old_tools/tools/h/rsconfig.h $
                  $Revision: 27 $
                  $Date: 3.08.00 15:09 $
                  $Author: Kubrin $
                  $Modtime: 3.08.00 12:17 $
*******************************************************************************/

#ifndef __RSCONFIG_H
#define __RSCONFIG_H

#include <rscomplr.h>
#include <platform.h>
//#include <rscdefs.h>
#include <stddef.h>
#include <fcntl.h>
#include <corecrt_share.h>

#ifdef _MSC_VER

    #include <time.h>
    #include <search.h>

    #ifdef __cplusplus
    extern "C" {
    #endif

    // -----------------------------------------------------------------------------
    #ifndef RSL_FTIME_DEFINED
    #define RSL_FTIME_DEFINED

        // -----------------------------------------------------------------------------
        struct ftime
             {
              unsigned    ft_tsec  : 5;   /* Two second interval */
              unsigned    ft_min   : 6;   /* Minutes */
              unsigned    ft_hour  : 5;   /* Hours */
              unsigned    ft_day   : 5;   /* Days */
              unsigned    ft_month : 4;   /* Months */
              unsigned    ft_year  : 7;   /* Year */
             };
    #endif

    // -----------------------------------------------------------------------------
    #ifndef RSL_NO_IO
        #define  read(h, b, c)   _read((h), (b), (c))
        #define  write(h, b, c)  _write((h), (b), (c))
        #define  open(p, o, m)   _open((p), (o), (m))
        #define  close(h)        _close(h)
    #endif


    #define  strdup(p)            _strdup(p)
    #define  tzset                _tzset

    #define  access(p, m)         _access((p), (m))
    #define  dup(h)               _dup(h)
    #define  dup2(h1, h2)         _dup2((h1), (h2))
    #define  lseek(h, offs, org)  _lseek((h), (offs), (org))
    #define  tell(h)              _tell(h)
    #define  chsize(h, sz)        _chsize((h), (sz))
    #define  unlink(f)            _unlink(f)
    #define  strupr(s)            _strupr(s)
    #define  strlwr(s)            _strlwr(s)
    #define  sopen                _sopen
    #define  fileno(f)            _fileno(f)
    #define  fstat(f, p)          _fstat((f), (struct _stat*)(p))
    #define  rs_macro_stat(f,p)   _stat((f),(struct _stat*)(p))
    #define  stricmp(s1, s2)      _stricmp((s1), (s2))
    #define  strnicmp(s1, s2, n)  _strnicmp((s1), (s2), (n))
    #define  filelength           _filelength
    #define  fdopen               _fdopen
    #define  delay                JDELAY
    #define  spawnl               _spawnl
    #define  spawnle              _spawnle
    #define  spawnlp              _spawnlp
    #define  spawnlpe             _spawnlpe
    #define  spawnv               _spawnv
    #define  spawnve              _spawnve
    #define  spawnvp              _spawnvp
    #define  spawnvpe             _spawnvpe
    #define  getdisk()            (_getdrive() - 1)
    #define  setdisk(n)           (_chdrive((n) + 1))
    #define  getcwd               _getcwd
    #define  chdir                _chdir
    #define  mkdir                _mkdir
    #define  rmdir                _rmdir
    #define  getcurdir            RslGetDir
    #define  memccpy              _memccpy
    #define  hopen                _open
    #define  cwait(a, b, c)       _cwait(a, b, c)
    #define  getpid               _getpid
    #define  utime(a, b)          _utime(a, b)
    #define  putenv(a)            _putenv(a)
    #define  lfind                _lfind


    #define  O_RDONLY     _O_RDONLY
    #define  O_WRONLY     _O_WRONLY
    #define  O_RDWR       _O_RDWR
    #define  O_TRUNC      _O_TRUNC
    #define  S_IREAD      _S_IREAD
    #define  S_IWRITE     _S_IWRITE
    #define  O_CREAT      _O_CREAT
    #define  O_TRUNC      _O_TRUNC
    #define  O_BINARY     _O_BINARY
    #define  O_APPEND     _O_APPEND
    #define  O_EXCL       _O_EXCL
    #define  O_TEXT       _O_TEXT

    #ifdef __cplusplus
    } ;
    #endif

#else   // _MSC_VER

    #define  _getcwd(a, b)  getcwd(a, b)
    #define  hopen          open

#endif  // _MSC_VER

#ifdef RSL_NO_EX_RANDOM
    #include <time.h>

    #define  randomize()  srand((unsigned)time(NULL))
    #define  random(num)  (int)(((long)rand()*(num))/(RAND_MAX + 1))
#endif

#define  pow10(y)            pow((double)10, (double)(y))
#define  pow10l(y)           powl((long double)10, (long double)(y))

#ifdef RSL_NO_LDOUBLE_CVT_FUN

    #define  atold(s)              atof(s)
    #define  _atold(s)             atof(s)
    #define  _strtold(sbeg, send)  strtod(sbeg, send)

#endif

// -----------------------------------------------------------------------------
#ifdef RSL_NO_LDOUBLE_MATHLIB

    #define  acosl(x)             acos(x)
    #define  asinl(x)             asin(x)
    #define  atan2l(x, y)         atan2((x), (y))
    #define  atanl(x)             atan(x)
    #define  ceill(x)             ceil(x)
    #define  coshl(x)             cosh(x)
    #define  cosl(x)              cos(x)
    #define  expl(x)              exp(x)
    #define  fabsl(x)             fabs(x)
    #define  floorl(x)            floor(x)
    #define  fmodl(x, y)          fmod((x), (y))
    #define  frexpl(x, exponent)  frexp((x), (exponent))
    #define  ldexpl(x, exponent)  ldexp((x), (exponent))
    #define  log10l(x)            log10(x)
    #define  logl(x)              log(x)
    #define  modfl(x, ipart)      modf((x), (ipart))
    #define  powl(x, y)           pow((x), (y))
    #define  sinhl(x)             sinh(x)
    #define  sinl(x)              sin(x)
    #define  sqrtl(x)             sqrt(x)
    #define  tanhl(x)             tanh(x)
    #define  tanl(x)              tan(x)

#endif

// -----------------------------------------------------------------------------
#if defined(RSL_NO_UNDERSCORE_CVT_FUN)

    #define  _itoa(val, str, rad)   itoa((val), (str), (rad))
    #define  _ltoa(val, str, rad)   ltoa((val), (str), (rad))
    #define  _ultoa(val, str, rad)  ultoa((val), (str), (rad))

#elif defined(RSL_UNDERSCORE_CVT_FUN)

    #define  itoa(val, str, rad)   _itoa((val), (str), (rad))
    #define  ltoa(val, str, rad)   _ltoa((val), (str), (rad))
    #define  ultoa(val, str, rad)  _ultoa((val), (str), (rad))

#else

    #ifdef __cplusplus
     extern "C" {
    #endif

        char *rs_itoa(int value, char *string, int radix);
        char *rs_ltoa(long value, char *string, int radix);
        char *rs_ultoa(unsigned long value, char *string, int radix);

    #ifdef __cplusplus
     };
    #endif

    #define  itoa(val, str, rad)    rs_itoa((val), (str), (rad))
    #define  ltoa(val, str, rad)    rs_ltoa((val), (str), (rad))
    #define  ultoa(val, str, rad)   rs_ultoa((val), (str), (rad))

    #define  _itoa(val, str, rad)   rs_itoa((val), (str), (rad))
    #define  _ltoa(val, str, rad)   rs_ltoa((val), (str), (rad))
    #define  _ultoa(val, str, rad)  rs_ultoa((val), (str), (rad))

#endif

// -----------------------------------------------------------------------------
#define  rs_max(a, b)  ((a) < (b) ? (b) : (a))
#define  rs_min(a, b)  ((b) < (a) ? (b) : (a))

// -----------------------------------------------------------------------------
#ifndef RSL_PL_MS
    #define  _fpreset()
    #define  _control87()
    #define  _clear87()
#endif

#ifdef RSL_PL_MS
    #ifdef __cplusplus
       extern "C" {
    #endif
        void ToolsSetLocale(void);
        void ToolsRestLocale(void);
    #ifdef __cplusplus
     };
    #endif

    #define  TLSAVELOCALE()  ToolsSetLocale()
    #define  TLRESTLOCALE()  ToolsRestLocale()

#else

    #define  TLSAVELOCALE()
    #define  TLRESTLOCALE()

#endif //RSL_PL_MS

#endif // __RSCONFIG_H

/* <-- EOF --> */
