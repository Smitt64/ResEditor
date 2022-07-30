/*-----------------------------------------------------------------------*-

 File Name   : rslcvt.h  

 Sybsystem   :   
 Description : 

 Source      : 

 Library(s)  : 

 VERSION INFO: $Archive: /RS-Tools/Old_tools/Tools/H/rslcvt.h $
               $Revision: 1 $
               $Date: 17.09.98 18:19 $
               $Author: Kubrin $
               $Modtime: 17.09.98 18:13 $

 Copyright (c) 1991 - 1998 by R-Style Software Lab.
 All Rights Reserved.

-*- History -------------------------------------------------------------*-
 September 17,1998  Sergei Kubrin (K78) - Create file
-*-----------------------------------------------------------------------*/

#ifndef __RSLCVT_H
#define __RSLCVT_H

#include "platform.h"
//#include "tooldef.h"


RSL_MAKE_HANDLE(HRSLCVT);

#define  RSL_DEF_CODEPAGE  866

#ifdef __cplusplus
extern "C" {
#endif

#define  RslOpenCvt(from, to)  0
#define  RslCloseCvt(par)
#define  RslCvtUString(hd, par)
#define  RslCvtUShort(hd, par)
#define  RslCvtULong(hd, par)
#define  RslCvtString(hd, par)
#define  RslCvtShort(hd, par)
#define  RslCvtLong(hd, par)
#define  RslCvtBlock(hd, par, par2)

#ifdef __cplusplus
}
#endif

#endif // __RSLCVT_H

/* <-- EOF --> */
