/*******************************************************************************
 FILE         :   PACKPSHN.H

 COPYRIGHT    :   R-Style Software Lab, 1998

 DESCRIPTION  :   Push Pack Native - set platform-native alignment for structures

 PROGRAMMED BY:   Yakov E. Markovitch

 CREATION DATE:   12 Oct 1998

 VERSION INFO :   $Archive: /RS-Tools/Old_tools/Tools/H/packpshn.h $
                  $Revision: 2 $
                  $Date: 27.01.00 18:51 $
                  $Author: Lebedev $
                  $Modtime: 27.01.00 14:09 $
*******************************************************************************/

#include <platform.h>

#if (RSL_MIN_SAFE_ALIGNMENT >= 8)
    #include <packpsh8.h>
#elif (RSL_MIN_SAFE_ALIGNMENT >= 4)
    #include <packpsh4.h>
#elif (RSL_MIN_SAFE_ALIGNMENT >= 2)
    #include <packpsh2.h>
#else
    #include <packpsh1.h>
#endif

/* <-- EOF --> */
