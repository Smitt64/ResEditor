#ifndef RSRESCORE_GLOBAL_H
#define RSRESCORE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(RSRESCORE_LIBRARY)
#  define RSRESCORE_EXPORT Q_DECL_EXPORT
#else
#  define RSRESCORE_EXPORT Q_DECL_IMPORT
#endif

#endif // RSRESCORE_GLOBAL_H
