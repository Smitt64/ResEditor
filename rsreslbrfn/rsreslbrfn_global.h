#ifndef RSRESLBRFN_GLOBAL_H
#define RSRESLBRFN_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(RSRESLBRFN_LIBRARY)
#  define RSRESLBRFN_EXPORT Q_DECL_EXPORT
#else
#  define RSRESLBRFN_EXPORT Q_DECL_IMPORT
#endif

//#define ftruncate _chsize
//#define fileno _fileno

#endif // RSRESLBRFN_GLOBAL_H
