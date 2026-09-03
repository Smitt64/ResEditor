#ifndef RESMENU_GLOBAL_H
#define RESMENU_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(RESMENU_LIBRARY)
#define RESMENU_EXPORT Q_DECL_EXPORT
#else
#define RESMENU_EXPORT Q_DECL_IMPORT
#endif

#endif // RESMENU_GLOBAL_H
