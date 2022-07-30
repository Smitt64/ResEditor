/*-----------------------------------------------------------------------*-

 File Name   : dlmdef.h  

 Sybsystem   :   
 Description : 

 Source      : 

 Library(s)  : 


 Copyright (c) 1991 - 1994 by R-Style Software Lab.
 All Rights Reserved.

-*- History -------------------------------------------------------------*-
 December 25,1996  Sergei Kubrin (K78) - Create file
-*-----------------------------------------------------------------------*/

#ifndef __DLMDEF_H
#define __DLMDEF_H

// -----------------------------------------------------------------------------
#if defined (RSL_PL_WIN32)
    #define  EXP
    #define  IMP
    #define  EXP32     __declspec(dllexport)
    #define  IMP32     __declspec(dllimport)   

    #define  DLMAPI    __stdcall
    #define  DLMAPI    __stdcall
    #define  DLMAPIC   __cdecl
    #define  DLMAPIC   __cdecl
#else
    #define  EXP
    #define  IMP 
    #define  EXP32
    #define  IMP32 

    #define  DLMAPI   
    #define  DLMAPI   
    #define  DLMAPIC  
    #define  DLMAPIC  
#endif

#endif  // __DLMDEF_H

/* <-- EOF --> */
