#ifndef __UNIXIO_H
#define __UNIXIO_H

#include <platform.h>

// -----------------------------------------------------------------------------
#ifdef RSL_PL_MS

    #include <io.h>
    #include <fcntl.h>
    #include <share.h>
    #include <direct.h>
    #include <sys/stat.h>

    #ifdef _MSC_VER
        #include <sys/utime.h>
        typedef struct _utimbuf  rstimebuf;
    #else
        #include <utime.h>
        typedef struct utimbuf  rstimebuf;
    #endif

    #define  rstime(fname, buf)  (utime((fname), (buf)))

    #define  ftruncate(file, size)  chsize((file), (size))

    #ifndef O_SHARE_RDONLY
        #define  O_SHARE_RDONLY   SH_DENYWR
    #endif

    #ifndef O_SHARE_WRONLY
        #define  O_SHARE_WRONLY   SH_DENYRD
    #endif

    #ifndef O_SHARE_RDWR
        #define  O_SHARE_RDWR     SH_DENYNO
    #endif

    #ifndef O_SHARE_NONE
        #define  O_SHARE_NONE     SH_DENYRW
    #endif

    #ifdef USE_UFILE

        intptr_t _findfirst_rs(const char *filespec, struct _finddata_t *fileinfo);
        int _findnext_rs(intptr_t handle, struct _finddata_t *fileinfo);

        #if defined(_MSC_VER) && (_MSC_VER >= 1400)
            intptr_t _findfirst32i64_rs(const char *filespec, struct _finddata32i64_t *fileinfo);
            int _findnext32i64_rs(intptr_t handle, struct _finddata32i64_t *fileinfo);
        #endif

    #else

        #define  _findfirst_rs(p1, p2)   _findfirst(p1, p2)
        #define  _findnext_rs(p1, p2)    _findnext(p1, p2)

    #endif  // USE_UFILE

#else

    #include <unistd.h>
    #include <fcntl.h>
    #include <sys/stat.h>
    #include <utime.h>

    typedef struct utimbuf  rstimebuf;

    /*
       There are no sopen(), tell() and filelength() in POSIX
       rstime() instead of utime()
    */

    #ifndef __cplusplus

        #define  sopen(name, access, share)  (open((name), (access)|(share)))
        #define  filelength(handle)          (lseek((handle), 0, SEEK_END))
        #define  rstime(fname, buf)          (utime((fname), (buf)))

    #else  //__cplusplus

        inline int sopen(const char *name, int access, int share)
        {
         return open(name, access|share);
        }

        inline int sopen(const char *name, int access, int share, int mode)
        {
         return open(name, access|share, mode);
        }

        inline long filelength(int handle)
        {
         return lseek((handle), 0, SEEK_END);
        }

        inline int rstime(const char *fname, const rstimebuf *buf)
        {
         return uname(fname, buf);
        }

    #endif //__cplusplus

    #ifndef O_SHARE_RDONLY
        #define  O_SHARE_RDONLY  0x00
    #endif

    #ifndef O_SHARE_WRONLY
        #define  O_SHARE_WRONLY  0x00
    #endif

    #ifndef O_SHARE_RDWR
        #define  O_SHARE_RDWR    0x00
    #endif

    #ifndef O_SHARE_NONE
        #define  O_SHARE_NONE    0x00
    #endif

    /*
       Access rights
    */

    #ifndef S_IREAD
        #define  S_IREAD  S_IRUSR
    #endif

    #ifndef S_IWRITE
        #define  S_IWRITE  S_IWUSR
    #endif

#endif

#ifndef SH_COMPAT
    #define  SH_COMPAT      O_SHARE_RDWR    /* compatibility mode */
#endif

#ifndef SH_DENYRW
    #define  SH_DENYRW      O_SHARE_NONE    /* deny read/write mode */
#endif

#ifndef SH_DENYWR
    #define  SH_DENYWR      O_SHARE_RDONLY    /* deny write mode */
#endif

#ifndef SH_DENYRD
    #define  SH_DENYRD      O_SHARE_WRONLY    /* deny read mode */
#endif

#ifndef SH_DENYNO
    #define  SH_DENYNO      O_SHARE_RDWR     /* deny none mode */
#endif

#ifndef SH_DENYNONE
    #define  SH_DENYNONE    SH_DENYNO
#endif

#ifndef O_BINARY
    #define  O_BINARY       0x00
#endif

#ifndef O_TEXTDATA
    #define  O_TEXTDATA     0x00
#endif

#ifndef O_CODEPAGE
    #define  O_CODEPAGE     0x00
#endif

#ifndef O_TEXT
    #define  O_TEXT         O_TEXTDATA
#endif

#endif // __UNIXIO_H

/* <-- EOF --> */
