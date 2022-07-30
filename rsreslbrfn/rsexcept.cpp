#include "rsexcept.h"
//#include "rsnexcep.h"
#include "platform.h"

/*#include "netlock.h"    // for CNum
#include "statvars.h"   // for oper

#include "getbdt.h"     // for bdate btime

#include "btserver.h"   //AV 17.03.2011 check for setting in rsreq.ini "MINIDUMP"
*/

// -----------------------------------------------------------------------------
static RSFatalHandler  handler;
static RSFatalFilter   filter;

// -----------------------------------------------------------------------------
RSL_CFUNC RSFatalHandler SetFatalHandler(RSFatalHandler newHandler)
{
 RSFatalHandler  oldHandler = handler;


 handler = newHandler;

 return oldHandler;
}

// -----------------------------------------------------------------------------
RSL_CFUNC RSFatalFilter SetFatalFilter(RSFatalFilter newFilter)
{
 RSFatalFilter  oldFilter = filter;


 filter = newFilter;

 return oldFilter;
}

/******************************************************************************
   RSSystemException
******************************************************************************/

// -----------------------------------------------------------------------------
RSSystemException::RSSystemException(int code, int nativeCode) : RSException("RSSystemException")
{
 _code       = code;
 _nativeCode = nativeCode;

 _str        = "";
 _address    = NULL;

 switch(_code)
      {
       // Illegal instruction
       case Illegal_Operation:       _str = "Illegal operation";                      break;
       case Privileged_Operation:    _str = "Privileged operation";                   break;
       case Stack_Overflow:          _str = "Stack overflow";                         break;

       // Unmapped, protected, or bad memory
       case Access_Violation:        _str = "Access violation";                       break;
       case Missing_Alignment:       _str = "Missing alignment";                      break;
       case Address_Error:           _str = "Address error";                          break;

       // Arithmetic exception
       case Float_DivideByZero:      _str = "Floating-point division by zero";        break;
       case Float_Overflow:          _str = "Floating-point overflow";                break;
       case Float_InexactResult:     _str = "Floating-point inexact result";          break;
       case Float_Underflow:         _str = "Floating-point underflow";               break;
       case Float_InvalidOperation:  _str = "Floating-point invalid operation";       break;
       case Float_PlatformSpecific:  _str = "Floating-point platform-specific error"; break;
       case Binary_DivideByZero:     _str = "Integer division by zero";               break;
       case Binary_Overflow:         _str = "Integer overflow";                       break;

       case OSSpecific_Exception:    _str = "OS-specific exception";                  break;
      }
}

// -----------------------------------------------------------------------------
void RSSystemException::executeHandler() const
{
 if(handler)
  {
   RSFatalHandler  safeHandler = handler;


   handler = NULL;

   (*safeHandler)(_code, _nativeCode, _str);
  }
}


// -----------------------------------------------------------------------------
char  RSSystemException::_dumpName[16];           //имя dump-файла
char  RSSystemException::_dumpDir[256] = "dump";  //каталог для dump-файлов

// -----------------------------------------------------------------------------
void RSSystemException::setDumpName(const char *dumpName)
{
 strncpy(RSSystemException::_dumpName, dumpName, sizeof(RSSystemException::_dumpName) - 1);
 RSSystemException::_dumpName[sizeof(RSSystemException::_dumpName) - 1] = '\0';
}

// -----------------------------------------------------------------------------
void RSSystemException::setDumpDir(const char *dumpDir)
{
 strncpy(RSSystemException::_dumpDir, dumpDir, sizeof(RSSystemException::_dumpDir) - 1);
 RSSystemException::_dumpDir[sizeof(RSSystemException::_dumpDir) - 1] = '\0';
}


// -----------------------------------------------------------------------------
bool  RSSystemException::_dumpDisabled = false;

// -----------------------------------------------------------------------------
void RSSystemException::disableDump()
{
 _dumpDisabled = true;
}

// -----------------------------------------------------------------------------
void RSSystemException::enableDump()
{
 _dumpDisabled = false;
}

// -----------------------------------------------------------------------------
#ifdef RSL_PL_WIN32
    // -----------------------------------------------------------------------------
    #include <stdio.h>

    /******************************************************************************
       RSNativeException
    ******************************************************************************/

    // -----------------------------------------------------------------------------
   /* RSNativeException::RSNativeException(int code, int nativeCode, const EXCEPTION_RECORD *ExceptionRecord)
                      : RSSystemException(code, nativeCode)
    {
     _ExceptionRecord = *ExceptionRecord;

     _address = _ExceptionRecord.ExceptionAddress;

     switch(_nativeCode)
          {
           case EXCEPTION_ACCESS_VIOLATION:
                {
                 void  *address = _ExceptionRecord.ExceptionAddress;
                 void  *pointer = (void *)_ExceptionRecord.ExceptionInformation[1];
                 char  *mode    = _ExceptionRecord.ExceptionInformation[0] ? "written" : "read";


                 _str = _buf;

                 sprintf((char *)_str, "The instruction at 0x%08X referenced memory at 0x%08X, the memory could not be %s",
                         (uint32)(uintptr_t)address, (uint32)(uintptr_t)pointer, mode);
                }
                break;

           case EXCEPTION_DATATYPE_MISALIGNMENT:
                _str = "Alignment Fault: A datatype misalignment was detected in a load or store instruction";
                break;

           case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
                _str = "Array bounds exceeded";
                break;

           case EXCEPTION_FLT_DENORMAL_OPERAND:
                _str = "Floating-point denormal operand";
                break;

           case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                _str = "Floating-point division by zero";
                break;

           case EXCEPTION_FLT_INEXACT_RESULT:
                _str = "Floating-point inexact result";
                break;

           case EXCEPTION_FLT_INVALID_OPERATION:
                _str = "Floating-point invalid operation";
                break;

           case EXCEPTION_FLT_OVERFLOW:
                _str = "Floating-point overflow";
                break;

           case EXCEPTION_FLT_STACK_CHECK:
                _str = "Floating-point stack check";
                break;

           case EXCEPTION_FLT_UNDERFLOW:
                _str = "Floating-point underflow";
                break;

           case EXCEPTION_INT_DIVIDE_BY_ZERO:
                _str = "Integer division by zero";
                break;

           case EXCEPTION_INT_OVERFLOW:
                _str = "Integer overflow";
                break;

           case EXCEPTION_PRIV_INSTRUCTION:
                _str = "Privileged instruction";
                break;

           case EXCEPTION_IN_PAGE_ERROR:
                _str = "The required data was not placed into memory because of an I/O error";
                break;

           case EXCEPTION_ILLEGAL_INSTRUCTION:
                _str = "An attempt was made to execute an illegal instruction";
                break;

           case EXCEPTION_NONCONTINUABLE_EXCEPTION:
                _str = "Cannot Continue: Windows NT cannot continue from this exception";
                break;

           case EXCEPTION_STACK_OVERFLOW:
                _str = "Stack Overflow";
                break;

           case EXCEPTION_INVALID_DISPOSITION:
                _str = "An invalid exception disposition was returned by an exception handler";
                break;

           case EXCEPTION_GUARD_PAGE:
                _str = "Guard Page Exception: A page of memory that marks the end of "
                       "a data structure, such as a stack or an array, has been accessed";
                break;
          }
    }

    // -----------------------------------------------------------------------------
    RSNativeException::RSNativeException(const RSNativeException &x)
                      : RSSystemException(x._code, x._nativeCode)
    {
     _ExceptionRecord = x._ExceptionRecord;
     _nativeCode      = x._nativeCode;

     if(x._str == x._buf)
      {
       _str = _buf;

       strcpy((char *)_str, x._str);
      }
     else
       _str = x._str;
    }
*/
    //AV 19.10.2010 create minidump for exception
    #pragma warning(disable:4091) // typedef : Їа®ЇгбЄ Ґвбп б«Ґў  ®в "<unnamed-enum-hdBase>", Є®Ј¤  ­Ё ®¤­  ЇҐаҐ¬Ґ­­ п ­Ґ ®Ўкпў«Ґ­
    /*#include <dbghelp.h>

    // -----------------------------------------------------------------------------
    static bool GenerateDump(EXCEPTION_POINTERS *pExceptionPointers)
    {
     BOOL   bMiniDumpSuccessful;
     char   szPath[MAX_PATH];
     char   szFileName[MAX_PATH];

     bdate  curdate;
     btime  curtime;

     HANDLE                          hDumpFile;
     MINIDUMP_EXCEPTION_INFORMATION  ExpParam;


     GetTempPath(MAX_PATH, szPath);

     strcat(szPath, "rstyle_exceptions_dmp");

     CreateDirectory(szPath, NULL);

     getbdate(&curdate);
     getbtime(&curtime);

     sprintf(szFileName, "%s\\rsrtlwm-%04hu%02hu%02hu-%02hu%02hu%02hu-%ld-%ld.dmp",
             szPath,
             (short)curdate.year, (short)curdate.mon, (short)curdate.day,
             (short)curtime.hour, (short)curtime.min, (short)curtime.sec,
             oper, CNum);

     hDumpFile = CreateFile(szFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);

     ExpParam.ThreadId          = GetCurrentThreadId();
     ExpParam.ExceptionPointers = pExceptionPointers;
     ExpParam.ClientPointers    = TRUE;

     bMiniDumpSuccessful = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpWithDataSegs, &ExpParam, NULL, NULL);

     return bMiniDumpSuccessful ? true : false;
    }*/

    // -----------------------------------------------------------------------------
   /* int RSNativeException::ExceptionFilter(DWORD ExceptionCode, EXCEPTION_POINTERS *ExceptionInfo)
    {
     if(BtServerGetConfig()->miniDump) // AV 17.03.2011 add check setting
       GenerateDump(ExceptionInfo);    // AV 19.10.2010 create minidump for exception

     static struct
          {
           int  ExceptionCode;
           int  code;
          } exceptionTable[] = {
                                { (int)EXCEPTION_ACCESS_VIOLATION,         Access_Violation },
                                { (int)EXCEPTION_DATATYPE_MISALIGNMENT,    Missing_Alignment },
                                { (int)EXCEPTION_ARRAY_BOUNDS_EXCEEDED,    Address_Error },

                                { (int)EXCEPTION_FLT_DENORMAL_OPERAND,     Float_PlatformSpecific },
                                { (int)EXCEPTION_FLT_DIVIDE_BY_ZERO,       Float_DivideByZero },
                                { (int)EXCEPTION_FLT_INEXACT_RESULT,       Float_InexactResult },
                                { (int)EXCEPTION_FLT_INVALID_OPERATION,    Float_InvalidOperation },
                                { (int)EXCEPTION_FLT_OVERFLOW,             Float_Overflow },
                                { (int)EXCEPTION_FLT_STACK_CHECK,          Float_PlatformSpecific },
                                { (int)EXCEPTION_FLT_UNDERFLOW,            Float_Underflow },

                                { (int)EXCEPTION_INT_DIVIDE_BY_ZERO,       Binary_DivideByZero },
                                { (int)EXCEPTION_INT_OVERFLOW,             Binary_Overflow },

                                { (int)EXCEPTION_PRIV_INSTRUCTION,         Privileged_Operation },
                                { (int)EXCEPTION_IN_PAGE_ERROR,            OSSpecific_Exception },
                                { (int)EXCEPTION_ILLEGAL_INSTRUCTION,      Illegal_Operation },
                                { (int)EXCEPTION_NONCONTINUABLE_EXCEPTION, OSSpecific_Exception },
                                { (int)EXCEPTION_STACK_OVERFLOW,           Stack_Overflow },

                                { (int)EXCEPTION_INVALID_DISPOSITION,      OSSpecific_Exception },
                                { (int)EXCEPTION_GUARD_PAGE,               OSSpecific_Exception }
                               };

     int  i;
     int  code = -1;


     for(i = 0; i < sizeof(exceptionTable)/sizeof(exceptionTable[0]); i++)
        if((int)ExceptionCode == exceptionTable[i].ExceptionCode)
         {
          code = exceptionTable[i].code;

          break;
         }

     if(code == -1)
       return EXCEPTION_CONTINUE_SEARCH;

     if(filter && (*filter)(code, ExceptionCode, ExceptionInfo) == -1)
       return EXCEPTION_CONTINUE_SEARCH;

     throw RSNativeException(code, ExceptionCode, ExceptionInfo->ExceptionRecord);
    }*/
#endif //RSL_PL_WIN32

/* <-- EOF --> */
