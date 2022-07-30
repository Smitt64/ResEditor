/*++

Module Name:

    pshpack1.h

Abstract:

    This file turns 1 byte packing of structures on.  (That is, it disables
    automatic alignment of structure fields.)  An include file is needed
    because various compilers do this in different ways.  For Microsoft
    compatible compilers, this file uses the push option to the pack pragma
    so that the poppack.h include file can restore the previous packing
    reliably.

    The file packpop.h is the complement to this file.

--*/

/*
 VERSION INFO :   $Archive: /RS-Tools/Old_tools/Tools/H/packpsh1.h $
                  $Revision: 2 $
                  $Date: 30.06.98 18:09 $
                  $Author: Kubrin $
                  $Modtime: 30.06.98 18:07 $
 */

#if ! (defined(lint) || defined(_lint))
    #ifndef RC_INVOKED
        #if ( _MSC_VER >= 800 )
            #pragma warning(disable:4103)

            #if !(defined( MIDL_PASS )) || defined( __midl )
                #pragma pack(push)
            #endif

            #pragma pack(1)
        #else
            #pragma pack(1)
        #endif
    #endif // ndef RC_INVOKED
#endif // ! (defined(lint) || defined(_lint))

/* <-- EOF --> */