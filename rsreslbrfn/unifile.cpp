//-*--------------------------------------------------------------------------*-
//
// File Name   : unifile.cpp
//
// Copyright (c) 1991 - 2007 by R-Style Softlab.
// All Rights Reserved.
//
//-*--------------------------------------------------------------------------*-
// December 19,2007  Sergei Kubrin (K78) - Create file
// Junuary 15, 2008 C interface added
//-*--------------------------------------------------------------------------*-
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <io.h>
#include <fcntl.h>
#include <share.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <mbstring.h>
#include <direct.h>
#include <time.h>
#include <errno.h>

//#include "tooldef.h"
#include "platform.h"
#include "unifile.hpp"

#include "rsexcept.h"


// -----------------------------------------------------------------------------
#ifdef USE_UFILE

// -----------------------------------------------------------------------------
#include "convertutf.h"
#include "rslocale.h"
#include "res.h"
#include "rsassert.h"
#include "unixio.h"

// -----------------------------------------------------------------------------
bool TUniFileBase::underflow(unsigned char *ptr)
{
 if(!isOpen())
   return false;

 int  res = readImpl(fileBuffer.getBuffer(), (int)fileBuffer.getBufferSize());


 if(res <= 0)
  {
   eofFlag = true;

   if(res < 0)
     errorFlag = true;

   return false;
  }

 if(res < 0)
   return false;

 curPtr = endPtr = fileBuffer.getBuffer();
 endPtr += res;
 *ptr = *curPtr++;

 return true;
}

// -----------------------------------------------------------------------------
bool TUniFileBase::flush()
{
 if(isOpen() && (oMode == UF_WRITE || oMode == UF_APPEND))
  {
   size_t  toWrite = curPtr - fileBuffer.getBuffer();
   int     res = writeImpl(fileBuffer.getBuffer(), (int)toWrite);


   if(res != toWrite)
    {
     errorFlag = true;

     return false;
    }

   curPtr = fileBuffer.getBuffer();
  }

 return true;
}

// -----------------------------------------------------------------------------
bool TUniFileBase::overflow(unsigned char byte)
{
 if(!isOpen())
   return false;

 if(flush())
  {
   *curPtr++ = byte;

   return true;
  }

 return false;
}

// -----------------------------------------------------------------------------
bool TUniFileBase::getByte(unsigned char *ptr)
{
 if(curPtr != endPtr)
  {
   *ptr = *curPtr++;

   return true;
  }

 return underflow(ptr);
}

// -----------------------------------------------------------------------------
bool TUniFileBase::putByte(unsigned char byte)
{
 if(curPtr != endPtr)
  {
   *curPtr++ = byte;

   return true;
  }

 return overflow(byte);
}

// -----------------------------------------------------------------------------
void TUniFileBase::unGetChar(char ch)
{
 unGetCharacter = ch;
}

// -----------------------------------------------------------------------------
bool TUniFileBase::getChar(char *ptr)
{
 char  ch;


 while(getCharImpl(&ch))
     {
      // #2782 Если флаг установлен, то читаем байт в байт. Такой режим используется при печати
      //       из макрофайлов с помощью Exit(2), иначе после печати строки не происходит возврат
      //       каретки.
      if(!flagReadAsIs)
       {
        if((ch == '\r' && lastNewLineChar == '\n') || (ch == '\n' && lastNewLineChar == '\r'))
         {
          lastNewLineChar = 0;
          *ptr = '\n';

          return true;
         }

        if(lastNewLineChar == '\r' || lastNewLineChar == '\n') // standalone symbol '\n' '\r'
         {
          unGetChar(ch);

          lastNewLineChar = 0;
          *ptr = '\n';

          return true;
         }

        if(ch == '\r' || ch == '\n')
         {
          lastNewLineChar = ch;

          continue;
         }
       }

      *ptr = ch;

      return true;
     }

 return false;
}

// -----------------------------------------------------------------------------
bool TUniFileBase::getCharImpl(char *ptr)
{
 if(unGetCharacter)
  {
   *ptr = unGetCharacter;

   unGetCharacter = 0;

   return true;
  }


 switch(fileEncode)
      {
       case UF_UTF16LE:
       case UF_UTF16BE:
       case UF_UTF8:
            {
             wchar_t  wch;


             if(!getWCharImpl(&wch))
               return false;

             LCW2RSCH(ptr, 1, &wch, 1, progEncode);
            }
            break;

       case UF_RSOEM:
       case UF_RSANSI:
       case UF_LCOEM:
       case UF_LCANSI:
            {
             unsigned char  ch;


             if(!getByte(&ch))
               return false;

             LCCnvChar(progEncode, ptr, 1, fileEncode, (const char *)&ch, 1);
            }
            break;

       default:
            return false;
      }

 return true;
}

// -----------------------------------------------------------------------------
bool TUniFileBase::putCharImpl(char ch)
{
 switch(fileEncode)
      {
       case UF_UTF16LE:
       case UF_UTF16BE:
       case UF_UTF8:
            {
             wchar_t  wch;


             LCRSCH2W(&wch, 1, &ch, 1, progEncode);

             if(!putWCharImpl(wch))
               return false;
            }
            break;

       case UF_RSOEM:
       case UF_RSANSI:
       case UF_LCOEM:
       case UF_LCANSI:
            {
             unsigned char  fch;


             LCCnvChar(fileEncode, (char *)&fch, 1, progEncode, &ch, 1);

             if(!putByte(fch))
               return false;
            }
            break;

       default:
            return false;
      }

 return true;
}

// -----------------------------------------------------------------------------
bool TUniFileBase::writeNewLine()
{
 if(eolMode == UF_EOL_DOSWIN || eolMode == UF_EOL_MAC)
   if(!putCharImpl('\r'))
     return false;

 if(eolMode == UF_EOL_DOSWIN || eolMode == UF_EOL_UNIX)
   if(!putCharImpl('\n'))
     return false;

 return true;
}

// -----------------------------------------------------------------------------
bool TUniFileBase::putChar(char ch)
{
 if(ch == '\r' || ch == '\n')
  {
   if((lastNewLineChar == '\n' && ch == '\r') || (lastNewLineChar == '\r' && ch == '\n'))
    {
     // receive pear symbol
     lastNewLineChar = 0;

     return true;
    }

   lastNewLineChar = ch;
   ++pendigNewLines;

   return true;
  }

 lastNewLineChar = 0;

 while(pendigNewLines)
     {
      if(!writeNewLine())
        return false;

      --pendigNewLines;
     }

 return putCharImpl(ch);
}

// -----------------------------------------------------------------------------
void TUniFileBase::unGetWChar(wchar_t ch)
{
 unGetWCharacter = ch;
}

// -----------------------------------------------------------------------------
bool TUniFileBase::getWChar(wchar_t *ptr)
{
 wchar_t  ch;


 while(getWCharImpl(&ch))
     {
      if((ch == '\r' && lastNewLineChar == '\n') || (ch == '\n' && lastNewLineChar == '\r'))
       {
        lastNewLineChar = 0;
        *ptr = '\n';

        return true;
       }

      if(lastNewLineChar == '\r' || lastNewLineChar == '\n') // standalone symbol '\n' '\r'
       {
        unGetWChar(ch);

        lastNewLineChar = 0;
        *ptr = '\n';

        return true;
       }

      if(ch == '\r' || ch == '\n')
       {
        lastNewLineChar = ch;

        continue;
       }

      *ptr = ch;

      return true;
     }

 return false;
}

// -----------------------------------------------------------------------------
bool TUniFileBase::getWCharImpl(wchar_t *ptr)
{
 if(unGetWCharacter)
  {
   *ptr = unGetWCharacter;
   unGetWCharacter = 0;

   return true;
  }

 unsigned char  *bytePtr = (unsigned char *)ptr;


 switch(fileEncode)
      {
       case UF_UTF16LE:
            if(!getByte(bytePtr) || !getByte(bytePtr + 1))
              return false;
            break;

       case UF_UTF16BE:
            if(!getByte(bytePtr + 1) || !getByte(bytePtr))
              return false;
            break;

       case UF_UTF8:
            {
             unsigned char  utf8[6];


             if(getByte (&utf8 [0]))
              {
               int  len = GetUTF8Lenght(utf8[0]);


               for(int i = 1; i < len; ++i)
                  if(!getByte(&utf8[i]))
                    return false;

               const UTF8  *sourceStart = utf8;
               UTF16       *targetStart = (UTF16 *)ptr;


               if(ConvertUTF8toUTF16(&sourceStart, sourceStart + len, &targetStart, targetStart + 1, lenientConversion) != conversionOK)
                {
                 errorFlag = true;

                 return false;
                }
              }
             else
               return false;
            }
            break;

       case UF_RSOEM:
       case UF_RSANSI:
       case UF_LCOEM:
       case UF_LCANSI:
            {
             unsigned char  ch;


             if(!getByte(&ch))
               return false;

             LCRSCH2W(ptr, 1, (const char *)&ch, 1, fileEncode);
            }
            break;

       default:
            return false;
      }

 return true;
}

// -----------------------------------------------------------------------------
bool TUniFileBase::putWCharImpl(wchar_t wch)
{
 unsigned char  *bytePtr = (unsigned char *)&wch;


 switch(fileEncode)
      {
       case UF_UTF16LE:
            if(!putByte(*bytePtr) || !putByte(*(bytePtr + 1)))
              return false;
            break;

       case UF_UTF16BE:
            if(!putByte(*(bytePtr + 1)) || !putByte(*bytePtr))
              return false;
            break;

       case UF_UTF8:
            {
             unsigned char   utf8[6];
             UTF8           *targetStart = utf8;
             const UTF16    *sourceStart = (const UTF16 *)&wch;


             if(ConvertUTF16toUTF8(&sourceStart, sourceStart + 1, &targetStart, targetStart + 6, lenientConversion) != conversionOK)
              {
               errorFlag = true;

               return false;
              }

             int  byteCount = (int)(targetStart - utf8);


             for(int i = 0; i < byteCount; ++i)
                if(!putByte(utf8[i]))
                  return false;
            }
            break;

       case UF_RSOEM:
       case UF_RSANSI:
       case UF_LCOEM:
       case UF_LCANSI:
            {
             unsigned char  ch;


             LCW2RSCH((char *)&ch, 1, &wch, 1, fileEncode);

             if(!putByte(ch))
               return false;
            }
            break;

       default:
            return false;
      }

 return true;
}

// -----------------------------------------------------------------------------
bool TUniFileBase::putWChar(wchar_t wch)
{
 if(wch == '\r' || wch == '\n')
  {
   if((lastNewLineChar == '\n' && wch == '\r') || (lastNewLineChar == '\r' && wch == '\n'))
    {
     // receive pear symbol
     lastNewLineChar = 0;

     return true;
    }

   lastNewLineChar = wch;
   ++pendigNewLines;

   return true;
  }

 lastNewLineChar = 0;

 while(pendigNewLines)
     {
      if(!writeNewLine())
        return false;

      --pendigNewLines;
     }

 return putWCharImpl(wch);
}

// -----------------------------------------------------------------------------
int TUniFileBase::write(const char *buff, int num)
{
 if(!checkWriteMode())
   return -1;

 int  written = 0;


 for(int i = 0; i < num; ++i)
   {
    if(!putChar(*buff++))
      break;

    ++written;
   }

 return written;
}

// -----------------------------------------------------------------------------
int TUniFileBase::read(char *buff, int num)
{
 if(!checkReadMode())
   return -1;

 int  read = 0;


 for(int i = 0; i < num; ++i)
   {
    if(!getChar(buff++))
      break;

    ++read;
   }

 return read;
}

// -----------------------------------------------------------------------------
bool TUniFileBase::writeLine(const char *str)
{
 if(!checkWriteMode())
   return false;

 while(*str)
      if(!putChar(*str++))
        break;

 if(*str)
   return false;

 return putChar('\n');
}


// -----------------------------------------------------------------------------
#define  DEF_LINE_SIZE  256

// -----------------------------------------------------------------------------
char *TUniFileBase::readLine()
{
 if(!checkReadMode())
   return false;

 bool        memStreamUsed = false;
 TMemStream  mem;
 char       *line = lineBuffer.getBuffer(DEF_LINE_SIZE);


 if(!line)
  {
   _set_errno(ENOMEM);

   errorFlag = true;

   return NULL;
  }

 char     ch;
 char    *ptr = line;
 size_t   num = lineBuffer.getBufferSize() - 1;


 MemStremInit(&mem);

 while(getChar(&ch))
     {
      if(ch == '\n')
        break;

      for(;;)
        {
         if(num--)
          {
           *ptr++ = ch;

           break;
          }
         else
          {
           MemStremWrite(&mem, line, ptr - line);

           num = lineBuffer.getBufferSize() - 1;
           ptr = line;

           memStreamUsed = true;
          }
        }
     }

 if(memStreamUsed)
  {
   MemStremWrite(&mem, line, ptr - line);

   size_t  memSz = MemStremGetSize(&mem);


   line = lineBuffer.getBuffer(memSz + sizeof(char));

   if(line)
    {
     MemStremSetPos(&mem, 0);
     MemStremRead(&mem, line, memSz);

     ptr = line + memSz;
    }
   else
    {
     _set_errno(ENOMEM);

     errorFlag = true;
     ptr = line;
    }
  }

 if(ptr)
   *ptr = '\0';

 MemStremDone(&mem);

 return (ptr == line && eofFlag) ? NULL : line;
}

// -----------------------------------------------------------------------------
bool TUniFileBase::writeBOM()
{
 switch(fileEncode)
      {
       case UF_UTF16LE:
            putByte(0xFF);
            putByte(0xFE);
            break;

       case UF_UTF16BE:
            putByte(0xFE);
            putByte(0xFF);
            break;

       case UF_UTF8:
            putByte(0xEF);
            putByte(0xBB);
            putByte(0xBF);
            break;
      }

 return true;
}

// -----------------------------------------------------------------------------
bool TUniFileBase::readBOM(bool *emptyFile)
{
 UFEncodeType   encode = UF_RSOEM;
 unsigned char  bom[3];


 if(emptyFile)
   *emptyFile = false;

 if(!getByte(&bom[0]))
  {
   if(eofFlag && !errorFlag)
     if(emptyFile)
       *emptyFile = true;
  }
 else if(getByte(&bom[1]))
  {
   if(bom[0] == 0xFF && bom[1] == 0xFE)
     encode = UF_UTF16LE;
   else if(bom[0] == 0xFE && bom[1] == 0xFF)
     encode = UF_UTF16BE;
   else if(bom[0] == 0xEF && bom[1] == 0xBB && getByte(&bom[2]) && bom[2] == 0xBF)
     encode = UF_UTF8;
  }

 if(encode == UF_RSOEM)
  {
   curPtr  = fileBuffer.getBuffer();
   eofFlag = errorFlag = false;
  }
 else
   fileEncode = encode;

 return true;
}

// -----------------------------------------------------------------------------
bool TUniFileBase::vprint(const char *fmt, va_list args)
{
 if(!checkWriteMode())
   return false;

 int  sz = _vscprintf(fmt, args);


 if(sz < 0)
  {
   errorFlag = true;

   return false;
  }

 char  *buff = lineBuffer.getBuffer((sz + 1)*sizeof(char));


 if(!buff)
  {
   _set_errno(ENOMEM);

   errorFlag = true;

   return false;
  }

 vsnprintf(buff, sz + 1, fmt, args);

 return (write(buff, sz) == sz) ? true : false;
}

// -----------------------------------------------------------------------------
bool TUniFileBase::print(const char *fmt, ...)
{
 va_list  args;


 va_start(args, fmt);

 bool  result = vprint(fmt, args);


 va_end(args);

 return result;
}

// -----------------------------------------------------------------------------
bool TUniFileBase::println(const char *fmt, ...)
{
 va_list  args;


 va_start(args, fmt);

 bool  result = vprint(fmt, args);


 va_end(args);

 if(result)
   return putChar('\n');

 return false;
}

// -----------------------------------------------------------------------------
bool TUniFileBase::rewind()
{
 if(checkReadMode() && seekImpl(0, SEEK_SET))
  {
   eofFlag = errorFlag = false;
   endPtr = curPtr = fileBuffer.getBuffer();

   readBOM();

   return true;
  }

 return false;
}

// -----------------------------------------------------------------------------
bool TUniFileBase::checkReadMode()
{
 if(isOpen() && (oMode == UF_READ || oMode == UF_READ_PLUS))
   return true;

 _set_errno(EPERM);

 errorFlag = true;

 return false;
}

// -----------------------------------------------------------------------------
bool TUniFileBase::checkWriteMode()
{
 if(isOpen() && oMode != UF_READ)
   return true;

 _set_errno(EPERM);

 errorFlag = true;

 return false;
}

// -----------------------------------------------------------------------------
bool TUniFileBase::reWrite(UFEncodeType encode)
{
 if(isOpen() && oMode == UF_READ_PLUS && seekImpl(0, SEEK_SET) && truncImpl(0))
  {
   fileEncode = encode;
   eofFlag    = errorFlag = false;
   curPtr     = fileBuffer.getBuffer ();
   endPtr     = curPtr + fileBuffer.getBufferSize();

   writeBOM();

   oMode = UF_WRITE;

   return true;
  }

 _set_errno(EPERM);

 errorFlag = true;

 return false;
}

// -----------------------------------------------------------------------------
bool TUniFileBase::openBase(UFOpenMode mode, UFShareMode share, size_t buffSize)
{
 curPtr = fileBuffer.getBuffer(buffSize);

 if(!curPtr)
  {
   _set_errno(ENOMEM);

   errorFlag = true;

   return false;
  }

 oMode    = mode;
 openFlag = true;

 switch(mode)
      {
       case UF_READ:
       case UF_READ_PLUS:
            endPtr = curPtr;
            readBOM();
            break;

       case UF_WRITE:
            endPtr = curPtr + fileBuffer.getBufferSize();
            writeBOM();
            break;

       case UF_APPEND:
            {
             bool  isNewFile;


             endPtr = curPtr;

             readBOM(&isNewFile);

             curPtr = fileBuffer.getBuffer(buffSize);
             endPtr = curPtr + fileBuffer.getBufferSize();

             if(isNewFile)
               writeBOM();
             else
               seekImpl(0, SEEK_END);
            }
            break;
      }

 return true;
}

// -----------------------------------------------------------------------------
bool TUniFileBase::commit()
{
 bool  stat = false;


 if(checkWriteMode())
  {
   if(flush())
    {
     stat = commitImpl();

     if(!stat)
       errorFlag = true;
    }
  }

 return stat;
}

// -----------------------------------------------------------------------------
bool TUniFileBase::close()
{
 if(isOpen())
  {
   // Всегда выводим хвостовые переводы строк. Иначе нарушается обратная совместимость.
   // Видимо надо добавить специальный режим, когда хвостовые переводы можно не выводить.
   if(oMode == UF_APPEND || oMode == UF_WRITE)
     while(pendigNewLines)
         {
          if(!writeNewLine())
            break;

          --pendigNewLines;
         }

   flush();
   closeImpl();

   openFlag = false;

   return true;
  }

 return false;
}

// -----------------------------------------------------------------------------
TUniFileBase::TUniFileBase(UFEncodeType fileEncode_, UFEncodeType progEncode_, UFEndOfLineMode eolMode_)
             : fileEncode(fileEncode_), progEncode(progEncode_), unGetWCharacter(0), unGetCharacter(0),
               eofFlag(false), errorFlag(false), openFlag(false), pendigNewLines(0), lastNewLineChar(0),
               curPtr(0), endPtr(0), eolMode(eolMode_), flagReadAsIs(false)

{
 if(progEncode == UF_USEDEFAULT)
   progEncode = (UFEncodeType)RslDefProgEncode;

 if(progEncode == UF_UTF8 || progEncode == UF_UTF16LE || progEncode == UF_UTF16BE)
   progEncode = UF_RSOEM;
}

// -----------------------------------------------------------------------------
TUniFileBase::~TUniFileBase()
{
}

// -----------------------------------------------------------------------------
TUniFile::TUniFile(UFEncodeType fileEncode, UFEncodeType progEncode, UFEndOfLineMode eolMode)
         : TUniFileBase(fileEncode, progEncode, eolMode), fd(-1)
#ifdef DEBUG
        , magic(782303)
#endif
{
}

// -----------------------------------------------------------------------------
TUniFile::~TUniFile()
{
 close();
}

// -----------------------------------------------------------------------------
bool TUniFile::open(const char *fileName, UFOpenMode mode, UFShareMode share, size_t buffSize, int *errcode)
{
 int  oflag     = _O_BINARY | _O_RDONLY;
 int  shareFlag = share == UF_DENYNO ? _SH_DENYNO : _SH_DENYWR;


 switch(mode)
      {
       case UF_READ:
            oflag = _O_BINARY | _O_RDONLY | _O_SEQUENTIAL;
            break;

       case UF_WRITE:
            oflag = _O_BINARY | _O_WRONLY | _O_CREAT | _O_TRUNC | _O_SEQUENTIAL;
            break;

       case UF_APPEND:
            oflag = _O_BINARY | _O_RDWR | _O_CREAT | _O_SEQUENTIAL;
            break;

       case UF_READ_PLUS:
            oflag = _O_BINARY | _O_RDWR;
            break;
      }

 if(IsRsLocaleLoaded)
   fd = _wsopen(RSCH2WEX<>(fileName, progEncode), oflag, shareFlag, _S_IREAD | _S_IWRITE);
 else
   fd = _sopen(fileName, oflag, shareFlag, _S_IREAD | _S_IWRITE);

 if(fd != -1 && openBase(mode, share, buffSize))
   return true;

 if((fd == -1) && errcode)
   *errcode = errno;

 close();

 errorFlag = true;

 return false;
}

// -----------------------------------------------------------------------------
void TUniFile::closeImpl()
{
 if(fd != -1)
   _close(fd);

 fd = -1;
}

// -----------------------------------------------------------------------------
int TUniFile::readImpl(unsigned char *buff, int sz)
{
 return _read(fd, buff, sz);
}

// -----------------------------------------------------------------------------
int TUniFile::writeImpl(const unsigned char *buff, int sz)
{
 return _write(fd, buff, sz);
}

// -----------------------------------------------------------------------------
bool TUniFile::seekImpl(int pos, int from)
{
 if(_lseek(fd, pos, from) != -1)
   return true;

 return false;
}

// -----------------------------------------------------------------------------
bool TUniFile::truncImpl(long sz)
{
 if(_chsize(fd, sz) != -1)
   return true;

 return false;
}

// -----------------------------------------------------------------------------
bool TUniFile::commitImpl()
{
 return _commit(fd) == 0;
}

//-@H------ BEGIN _________________________________________________ --------@@-
//
//  C interface

// -----------------------------------------------------------------------------
int ufGetEncodeFromStr(const char *encStr)
{
 if(!encStr)
   return UF_RSOEM;

 if(strcmp(encStr, RSANSI_TXT) == 0)
   return UF_RSANSI;

 if(strcmp(encStr, LCANSI_TXT) == 0)
   return UF_LCANSI;

 if(strcmp(encStr, LCOEM_TXT) == 0)
   return UF_LCOEM;

 if(strcmp(encStr, UTF8_TXT) == 0)
   return UF_UTF8;

 if(strcmp(encStr, UTF16LE_TXT) == 0)
   return UF_UTF16LE;

 if(strcmp(encStr, UTF16BE_TXT) == 0)
   return UF_UTF16BE;

 return UF_RSOEM;
}

// -----------------------------------------------------------------------------
int ufGetEncode(FILE *fl)
{
 if(fl)
  {
   TUniFile  *fPtr = reinterpret_cast<TUniFile *>(fl);


   RSASSERT(fPtr->magic == 782303);

   return (int)fPtr->getFileEncode();
  }

 return UF_RSOEM;
}

// -----------------------------------------------------------------------------
void ufSetEncode(FILE *fl, int enc)
{
 if(fl)
  {
   TUniFile  *fPtr = reinterpret_cast<TUniFile *>(fl);


   RSASSERT(fPtr->magic == 782303);

   UFEncodeType  curEnc = fPtr->getFileEncode();


   if(curEnc != UF_UTF8 && curEnc != UF_UTF16LE && curEnc != UF_UTF16BE)
     fPtr->setFileEncode((UFEncodeType)enc);
  }
}

// -----------------------------------------------------------------------------
FILE *ufOpenEx2(const char *fileName, int openMode, int fileEncode, int progEncode, int shareMode, int eol, int *errcode)
{
 TUniFile  *fPtr = new TUniFile((UFEncodeType)fileEncode, (UFEncodeType)progEncode, (UFEndOfLineMode)eol);


 if(fPtr->open(fileName, (UFOpenMode)openMode, (UFShareMode)shareMode, dUF_BUFF_SIZE, errcode))
   return reinterpret_cast<FILE *>(fPtr);

 delete fPtr;

 return NULL;
}

// -----------------------------------------------------------------------------
FILE *ufOpenEx(const char *fileName, int openMode, int fileEncode, int progEncode, int shareMode, int eol)
{
 return ufOpenEx2(fileName, openMode, fileEncode, progEncode, shareMode, eol, NULL);
}

// -----------------------------------------------------------------------------
FILE *ufOpen(const char *fileName, int openMode, int fileEncode, int progEncode, int shareMode)
{
 return ufOpenEx(fileName, openMode, fileEncode, progEncode, shareMode, UF_EOL_DOSWIN);
}

// -----------------------------------------------------------------------------
FILE *ufOpen2(const char *fileName, int openMode, int fileEncode, int progEncode, int shareMode, int *errcode)
{
 return ufOpenEx2(fileName, openMode, fileEncode, progEncode, shareMode, UF_EOL_DOSWIN, errcode);
}

// -----------------------------------------------------------------------------
void ufClose(FILE *fl)
{
 if(fl)
  {
   TUniFile  *fPtr = reinterpret_cast<TUniFile *>(fl);


   RSASSERT(fPtr->magic == 782303);

   fPtr->close();

   delete fPtr;
  }
}

// -----------------------------------------------------------------------------
int ufGetch(FILE *fl)
{
 if(fl)
  {
   char       ch;
   TUniFile  *fPtr = reinterpret_cast<TUniFile *>(fl);


   RSASSERT(fPtr->magic == 782303);

   if(fPtr->getChar(&ch))
     return (unsigned char)ch;
  }

 return EOF;
}

// -----------------------------------------------------------------------------
void ufUnGetch(FILE *fl, char ch)
{
 if(fl)
  {
   TUniFile  *fPtr = reinterpret_cast<TUniFile *>(fl);


   RSASSERT(fPtr->magic == 782303);

   fPtr->unGetChar(ch);
  }
}

// -----------------------------------------------------------------------------
bool ufPutch(FILE *fl, char ch)
{
 if(fl)
  {
   TUniFile  *fPtr = reinterpret_cast<TUniFile *>(fl);


   RSASSERT(fPtr->magic == 782303);

   return fPtr->putChar(ch);
  }

 return false;
}

// -----------------------------------------------------------------------------
void ufFlush(FILE *fl)
{
 if(fl)
  {
   TUniFile  *fPtr = reinterpret_cast<TUniFile *>(fl);


   RSASSERT(fPtr->magic == 782303);

   fPtr->flush();
  }
}

// -----------------------------------------------------------------------------
int ufWrite(FILE *fl, const char *buff, int sz)
{
 if(fl)
  {
   TUniFile  *fPtr = reinterpret_cast<TUniFile *>(fl);


   RSASSERT(fPtr->magic == 782303);

   return fPtr->write(buff, sz);
  }

 return -1;
}

// -----------------------------------------------------------------------------
int ufRead(FILE *fl, char *buff, int sz)
{
 if(fl)
  {
   TUniFile  *fPtr = reinterpret_cast<TUniFile *>(fl);


   RSASSERT(fPtr->magic == 782303);

   return fPtr->read(buff, sz);
  }

 return -1;
}

// -----------------------------------------------------------------------------
char *ufReadLine(FILE *fl)
{
 if(fl)
  {
   TUniFile  *fPtr = reinterpret_cast<TUniFile *>(fl);


   RSASSERT(fPtr->magic == 782303);

   char  *ret = fPtr->readLine();

   return ret;
  }

 return NULL;
}

// -----------------------------------------------------------------------------
bool ufError(FILE *fl)
{
 if(fl)
  {
   TUniFile  *fPtr = reinterpret_cast<TUniFile *>(fl);


   RSASSERT(fPtr->magic == 782303);

   return fPtr->isError();
  }

 return true;
}

// -----------------------------------------------------------------------------
bool ufWriteLine(FILE *fl, const char *line)
{
 if(fl)
  {
   TUniFile  *fPtr = reinterpret_cast<TUniFile *>(fl);


   RSASSERT(fPtr->magic == 782303);

   return fPtr->writeLine(line);
  }

 return false;
}

// -----------------------------------------------------------------------------
bool ufRewind(FILE *fl)
{
 if(fl)
  {
   TUniFile  *fPtr = reinterpret_cast<TUniFile *>(fl);


   RSASSERT(fPtr->magic == 782303);

   return fPtr->rewind();
  }

 return false;
}

// -----------------------------------------------------------------------------
bool ufReWrite(FILE *fl, int encode)
{
 if(fl)
  {
   TUniFile  *fPtr = reinterpret_cast<TUniFile *>(fl);


   RSASSERT(fPtr->magic == 782303);

   return fPtr->reWrite((UFEncodeType)encode);
  }

 return false;
}

// -----------------------------------------------------------------------------
char *ufGetTmpBuffer(FILE *fl, size_t sz)
{
 if(fl)
  {
   TUniFile  *fPtr = reinterpret_cast<TUniFile *>(fl);


   RSASSERT(fPtr->magic == 782303);

   return fPtr->getTmpBuffer(sz);
  }

 return NULL;
}

// -----------------------------------------------------------------------------
int ufPrintf(FILE *fl, const char *fmt, ...)
{
 int  retVal = -1;


 if(fl)
  {
   TUniFile  *fPtr = reinterpret_cast<TUniFile *>(fl);


   RSASSERT(fPtr->magic == 782303);

   va_list    args;


   va_start(args, fmt);


   if(fPtr->vprint(fmt, args))
     retVal = (int)strlen(fPtr->getTmpBuffer(0));

   va_end(args);
  }

 return retVal;
}

// -----------------------------------------------------------------------------
int ufFileno(FILE *fl)
{
 if(fl)
  {
   TUniFile  *fPtr = reinterpret_cast<TUniFile *>(fl);


   RSASSERT(fPtr->magic == 782303);

   return fPtr->getFileno();
  }

 return -1;
}

// -----------------------------------------------------------------------------
bool ufSetAsIsFlag(FILE *fl, bool newFlag)
{
 if(fl)
  {
   TUniFile  *fPtr = reinterpret_cast<TUniFile *>(fl);


   RSASSERT(fPtr->magic == 782303);

   return fPtr->setAsIsFlag(newFlag);
  }

 return false;
}

// -----------------------------------------------------------------------------
class OemBuffer : public RslTmpBuffer<char>
{
 public:
   // --------------------------------------------
   OemBuffer() : RslTmpBuffer<char>(0), curSize(0)
   {
   }

   // --------------------------------------------
   void setValue(wchar_t *env)
   {
    size_t  needSz = wcslen(env) + 1;


    if(curSize < needSz)
     {
      init(needSz);

      curSize = needSz;
     }

    LCW2RSOEM(*this, (int)needSz, env, (int)needSz);
   }

 private:
   size_t   curSize;
};

// -----------------------------------------------------------------------------
static OemBuffer  globalOemBuffer;

// -----------------------------------------------------------------------------
const char *getenv_rs(const char *key)
{
 if(IsRsLocaleLoaded)
  {
   wchar_t  *env = _wgetenv(RSCH2WEX<>(key, RslDefProgEncode));


   if(env)
    {
     globalOemBuffer.setValue(env);

     return globalOemBuffer;
    }

   return NULL;
  }

 return getenv(key);
}

// -----------------------------------------------------------------------------
int putenv_rs(const char *val)
{
 if(IsRsLocaleLoaded)
   return _wputenv(RSCH2WEX<>(val, RslDefProgEncode));

 return _putenv(val);
}

// -----------------------------------------------------------------------------
char *fullpath_rs(char *absPath, const char *relPath, size_t maxLen)
{
 if(IsRsLocaleLoaded)
  {
   RslTmpBuffer<wchar_t, 256>   tmpBuff(absPath ? maxLen : _MAX_PATH);
   wchar_t                     *p = _wfullpath(tmpBuff, RSCH2WEX<>(relPath, RslDefProgEncode), absPath ? maxLen : _MAX_PATH);


   if(p)
    {
     if(!absPath)
      {
       maxLen  = wcslen(p) + 1;
       absPath = (char *)malloc(maxLen);
      }

     if(absPath)
       LCW2RSCH(absPath, (int)maxLen, tmpBuff, (int)maxLen, RslDefProgEncode);

     return absPath;
    }

   return NULL;
  }

 return _fullpath(absPath, relPath, maxLen);
}

// -----------------------------------------------------------------------------
// #4151 - Обход ошибки MSVS 2010
//         Неаккуртаненько, но пишем костыль здесь, а не в отдельном файле,
//         поскольку unifile.cpp включается в несколько проектов :-(
// -----------------------------------------------------------------------------

// -------------------------------------------
// Утянуто из crt\src\msdos.h

// DOS file attributes
// Только те, которые здесь используются

#define  A_RO          0x1     /* read only */
#define  A_D           0x10    /* directory */


// -------------------------------------------

// -------------------------------------------
// Утянуто из crt\src\internal.h

// -------------------------------------------
#define  dISSLASHW(a)  ((a) == L'\\' || (a) == L'/')

// -----------------------------------------------------------------------------
typedef struct TSErrEntry
      {
       unsigned long  oscode;     /* OS return value */
       int            errnocode;  /* System V error code */
      } TSErrEntry;

// -----------------------------------------------------------------------------
// Утянуто из crt\src\dosmap.c
static struct TSErrEntry  loc_ErrTable[] = {
                                            { ERROR_INVALID_FUNCTION,      EINVAL    },  /*    1 */
                                            { ERROR_FILE_NOT_FOUND,        ENOENT    },  /*    2 */
                                            { ERROR_PATH_NOT_FOUND,        ENOENT    },  /*    3 */
                                            { ERROR_TOO_MANY_OPEN_FILES,   EMFILE    },  /*    4 */
                                            { ERROR_ACCESS_DENIED,         EACCES    },  /*    5 */
                                            { ERROR_INVALID_HANDLE,        EBADF     },  /*    6 */
                                            { ERROR_ARENA_TRASHED,         ENOMEM    },  /*    7 */
                                            { ERROR_NOT_ENOUGH_MEMORY,     ENOMEM    },  /*    8 */
                                            { ERROR_INVALID_BLOCK,         ENOMEM    },  /*    9 */
                                            { ERROR_BAD_ENVIRONMENT,       E2BIG     },  /*   10 */
                                            { ERROR_BAD_FORMAT,            ENOEXEC   },  /*   11 */
                                            { ERROR_INVALID_ACCESS,        EINVAL    },  /*   12 */
                                            { ERROR_INVALID_DATA,          EINVAL    },  /*   13 */
                                            { ERROR_INVALID_DRIVE,         ENOENT    },  /*   15 */
                                            { ERROR_CURRENT_DIRECTORY,     EACCES    },  /*   16 */
                                            { ERROR_NOT_SAME_DEVICE,       EXDEV     },  /*   17 */
                                            { ERROR_NO_MORE_FILES,         ENOENT    },  /*   18 */
                                            { ERROR_LOCK_VIOLATION,        EACCES    },  /*   33 */
                                            { ERROR_BAD_NETPATH,           ENOENT    },  /*   53 */
                                            { ERROR_NETWORK_ACCESS_DENIED, EACCES    },  /*   65 */
                                            { ERROR_BAD_NET_NAME,          ENOENT    },  /*   67 */
                                            { ERROR_FILE_EXISTS,           EEXIST    },  /*   80 */
                                            { ERROR_CANNOT_MAKE,           EACCES    },  /*   82 */
                                            { ERROR_FAIL_I24,              EACCES    },  /*   83 */
                                            { ERROR_INVALID_PARAMETER,     EINVAL    },  /*   87 */
                                            { ERROR_NO_PROC_SLOTS,         EAGAIN    },  /*   89 */
                                            { ERROR_DRIVE_LOCKED,          EACCES    },  /*  108 */
                                            { ERROR_BROKEN_PIPE,           EPIPE     },  /*  109 */
                                            { ERROR_DISK_FULL,             ENOSPC    },  /*  112 */
                                            { ERROR_INVALID_TARGET_HANDLE, EBADF     },  /*  114 */
                                            { ERROR_INVALID_HANDLE,        EINVAL    },  /*  124 */
                                            { ERROR_WAIT_NO_CHILDREN,      ECHILD    },  /*  128 */
                                            { ERROR_CHILD_NOT_COMPLETE,    ECHILD    },  /*  129 */
                                            { ERROR_DIRECT_ACCESS_HANDLE,  EBADF     },  /*  130 */
                                            { ERROR_NEGATIVE_SEEK,         EINVAL    },  /*  131 */
                                            { ERROR_SEEK_ON_DEVICE,        EACCES    },  /*  132 */
                                            { ERROR_DIR_NOT_EMPTY,         ENOTEMPTY },  /*  145 */
                                            { ERROR_NOT_LOCKED,            EACCES    },  /*  158 */
                                            { ERROR_BAD_PATHNAME,          ENOENT    },  /*  161 */
                                            { ERROR_MAX_THRDS_REACHED,     EAGAIN    },  /*  164 */
                                            { ERROR_LOCK_FAILED,           EACCES    },  /*  167 */
                                            { ERROR_ALREADY_EXISTS,        EEXIST    },  /*  183 */
                                            { ERROR_FILENAME_EXCED_RANGE,  ENOENT    },  /*  206 */
                                            { ERROR_NESTING_NOT_ALLOWED,   EAGAIN    },  /*  215 */
                                            { ERROR_NOT_ENOUGH_QUOTA,      ENOMEM    }   /* 1816 */
                                           };


// size of the table
#define  dERRTABLESIZE(et)       (sizeof(et)/sizeof(et[0]))

// The following two constants must be the minimum and maximum
// values in the (contiguous) range of Exec Failure errors.
#define  dMIN_EXEC_ERROR         ERROR_INVALID_STARTING_CODESEG
#define  dMAX_EXEC_ERROR         ERROR_INFLOOP_IN_RELOC_CHAIN

// These are the low and high value in the range of errors that are access violations */
#define  dMIN_EACCES_RANGE       ERROR_WRITE_PROTECT
#define  dMAX_EACCES_RANGE       ERROR_SHARING_BUFFER_EXCEEDED


// -----------------------------------------------------------------------------
// Аналог _get_errno_from_oserr (да, именно её, а не _dosmaperr - нам не особо интересна
// установка _doserrno)
static int __DosMapError(unsigned long oserrno)
{
 int  stat = 0;


 /* check the table for the OS error code */
 for(int  i = 0; i < dERRTABLESIZE(loc_ErrTable); i++)
    if(oserrno == loc_ErrTable[i].oscode)
     {
      stat = loc_ErrTable[i].errnocode;

      break;
     }

 // The error code wasn't in the table.  We check for a range of
 // EACCES errors or exec failure errors (ENOEXEC).  Otherwise
 // EINVAL is returned.

 if((oserrno >= dMIN_EACCES_RANGE) && (oserrno <= dMAX_EACCES_RANGE))
   stat = EACCES;
 else if((oserrno >= dMIN_EXEC_ERROR) && (oserrno <= dMAX_EXEC_ERROR))
   stat = ENOEXEC;
 else
   stat = EINVAL;

 _set_errno(stat);

 return stat;
}

// -----------------------------------------------------------------------------
// Аналог __loctotime32_t
static __time32_t __LocToTimeEx32(int wYear, int wMonth, int wDay, int wHour, int wMinute, int wSecond, int isdst = -1)
{
 __time32_t  t32;
 struct tm   ptm = { 0 };


 // Алгоритм утащен из _getsystime (crt\src\systime.c)

 ptm.tm_isdst = isdst;  /* mktime() computes whether this is */
                        /* during Standard or Daylight time. */
 ptm.tm_sec   = wSecond;
 ptm.tm_min   = wMinute;
 ptm.tm_hour  = wHour;
 ptm.tm_mday  = wDay;
 ptm.tm_mon   = wMonth - 1;
 ptm.tm_year  = wYear  - 1900;
 ptm.tm_wday  = 0; //wDayOfWeek;

 t32 = _mktime32(&ptm);

 return t32;
}

// -----------------------------------------------------------------------------
// Аналог __loctotime32_t
static __time32_t __LocToTime32(SYSTEMTIME &sysTime, int isdst = -1)
{
 return __LocToTimeEx32(sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, isdst);
}

// -----------------------------------------------------------------------------
// Конвертация времени из __time32_t в __time64_t
// Замечание: функция использует внутрисистемный буфер!
static __time64_t __CvtTime32to64(__time32_t t32)
{
 __time64_t  t64 = (__time64_t)-1;
 struct tm  *pTM = _localtime32(&t32);


 if(pTM)
   t64 = _mktime64(pTM);

 return t64;
}

// -----------------------------------------------------------------------------
// Конвертация времени из __time64_t в __time32_t
// Замечание: функция использует внутрисистемный буфер!
static __time32_t __CvtTime64to32(__time64_t t64)
{
 // В открытых хедерах этого значения я не нашел, утянуто из crt\src\ctime.h
 // Соответствует дефайну _MAX__TIME32_T
 #define  dMAX_TIME32  0x7fffd27f

 __time32_t  t32 = (__time32_t)-1;


 // Сначала делаем проверку: поместится-ли наше время 64 (максимальное значение 2038-01-18 23:59:59)
 // во время 32 (максимальное значение 3000-12-31 23:59:59).
 if(t64 != (__time64_t)-1)
   if(t64 <= (__time64_t)dMAX_TIME32)
    {
     struct tm  *pTM = _localtime64(&t64);


     if(pTM)
       t32 = _mktime32(pTM);
    }

 return t32;
}

// -----------------------------------------------------------------------------
// А это вообще выдрано из crt\src\stat.c
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
/*
 * IsRootUNCName - returns TRUE if the argument is a UNC name specifying
 *      a root share.  That is, if it is of the form \\server\share\.
 *      This routine will also return true if the argument is of the
 *      form \\server\share (no trailing slash) but Win32 currently
 *      does not like that form.
 *
 *      Forward slashes ('/') may be used instead of backslashes ('\').
 */
static int __IsRootUNCNameW(const wchar_t *path)
{
 // If a root UNC name, path will start with 2 (but not 3) slashes
 if((wcslen(path) >= 5) /* minimum string is "//x/y" */
     && dISSLASHW(path[0]) && dISSLASHW(path[1])
     && !dISSLASHW(path[2]))
  {
   const wchar_t  *p = path + 2;


   // find the slash between the server name and share name
   while(* ++p)
       {
        if(dISSLASHW(*p))
          break;
       }

   if(*p && p[1])
    {
     // is there a further slash?
     while(* ++p)
         {
          if(dISSLASHW(*p))
            break;
         }

     // just final slash (or no final slash)
     if(!*p || !p[1])
       return 1;
    }
  }

 return 0;
}

// -----------------------------------------------------------------------------
static wchar_t *__FullPath_HelperW(wchar_t *buf, const wchar_t *path, size_t sz, wchar_t **pBuf)
{
 wchar_t  *ret;
 errno_t   save_errno = errno;


 errno = 0;

 ret = _wfullpath(buf, path, sz);

 if(ret)
  {
   errno = save_errno;

   return ret;
  }

 // if _tfullpath fails because buf is too small, then we just call again _tfullpath and
 // have it allocate the appropriate buffer
 if(errno != ERANGE)
  {
   // _tfullpath is failing for another reason, just propagate the failure and keep the
   // failure code in errno

   return NULL;
  }

 errno = save_errno;

 *pBuf = _wfullpath(NULL, path, 0);

 return *pBuf;
}

// -----------------------------------------------------------------------------
static unsigned short __ToXModeW(int attr, const wchar_t *name)
{
 unsigned short   uxmode;
 unsigned         dosmode = attr & 0xff;
 const wchar_t   *p;


 if((p = name)[1] == L':')
   p += 2;

 // check to see if this is a directory - note we must make a special
 // check for the root, which DOS thinks is not a directory
 uxmode = (unsigned short)(((dISSLASHW(*p) && !p[1]) || (dosmode & A_D) || !*p) ? _S_IFDIR | _S_IEXEC : _S_IFREG);

 // If attribute byte does not have read-only bit, it is read-write
 uxmode |= (dosmode & A_RO) ? _S_IREAD : (_S_IREAD | _S_IWRITE);

 // see if file appears to be executable - check extension of name

 if(p = wcsrchr(name, L'.'))
   if(!_wcsicmp(p, L".exe") || !_wcsicmp(p, L".cmd") || !_wcsicmp(p, L".bat") || !_wcsicmp(p, L".com"))
     uxmode |= _S_IEXEC;

 /* propagate user read/write/execute bits to group/other fields */

 uxmode |= (uxmode & 0700) >> 3;
 uxmode |= (uxmode & 0700) >> 6;

 return uxmode;
}

// -----------------------------------------------------------------------------
// Аналог _wstat32i64
static int __Stat32i64W(const wchar_t *name, struct _stat32i64 *buf)
{
 wchar_t  *path;
 wchar_t   pathbuf[_MAX_PATH];
 int       drive; /* A: = 1, B: = 2, etc. */
 int       stat  = 0;


 if(name && buf)
  {
   memset(buf, 0, sizeof(*buf));

   // Don't allow wildcards to be interpreted by system
   if(!wcspbrk(name, L"?*"))
    {
     // Try to get disk from name.  If none, get current disk.
     if(name[1] == L':')
      {
       if(*name && !name[2])
        {
         _set_errno(ENOENT);

         stat = -1;
        }
       else
         drive = _mbctolower(*name) - L'a' + 1;
      }
     else
       drive = _getdrive();

     if(!stat)
      {
       bool              fillCommon = true;

       WIN32_FIND_DATAW  findbuf = { 0 };
       // Call Find Match File
       HANDLE            hFind   = FindFirstFileW(name, &findbuf);


       if(hFind == INVALID_HANDLE_VALUE)
        {
         wchar_t  *pBuf = NULL;


         if(!(wcspbrk(name, L"./\\") &&
             (path = __FullPath_HelperW(pathbuf, name, _MAX_PATH, &pBuf)) &&
             // root dir. ('C:\') or UNC root dir. ('\\server\share\')
             ((wcslen(path) == 3) || __IsRootUNCNameW(path)) && (GetDriveTypeW(path) > 1)))
          {
           if(pBuf)
             free(pBuf);

           _set_errno(ENOENT);

           stat = -1;
          }

         if(pBuf)
           free(pBuf);

         // Root directories (such as C:\ or \\server\share\ are fabricated.
         if(!stat)
          {
           findbuf.dwFileAttributes = A_D;
           findbuf.nFileSizeHigh    = 0;
           findbuf.nFileSizeLow     = 0;
           findbuf.cFileName[0]     = L'\0';

           buf->st_mtime = __LocToTimeEx32(1980, 1, 1, 0, 0, 0, -1);
           buf->st_atime = buf->st_mtime;
           buf->st_ctime = buf->st_mtime;
          }
        }
       else if((findbuf.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) && (findbuf.dwReserved0 == IO_REPARSE_TAG_SYMLINK))
        {
         // if the file is a symbolic link, then use fstat to fill the info in the _stat struct
         int      fd = -1;
         errno_t  e  = _wsopen_s(&fd, name, _O_RDONLY, _SH_DENYNO, 0 /* ignored */);


         if(fd != -1)
          {
           stat = _fstat32i64(fd, buf);

           _close(fd);
          }
         else
          {
           _set_errno(ENOENT);

           stat = -1;
          }

         fillCommon = false;
        }
       else
        {
         SYSTEMTIME  sysTime;
         FILETIME    locFTime;


         if(!stat)
          {
           if(findbuf.ftLastWriteTime.dwLowDateTime || findbuf.ftLastWriteTime.dwHighDateTime)
            {
             if(FileTimeToLocalFileTime(&findbuf.ftLastWriteTime, &locFTime) && FileTimeToSystemTime(&locFTime, &sysTime))
              {
               buf->st_mtime = __LocToTime32(sysTime, -1);
              }
             else
              {
               __DosMapError(GetLastError());

               stat = -1;
              }
            }
          }

         if(!stat)
          {
           if(findbuf.ftLastAccessTime.dwLowDateTime || findbuf.ftLastAccessTime.dwHighDateTime)
            {
             if(FileTimeToLocalFileTime(&findbuf.ftLastAccessTime, &locFTime) && FileTimeToSystemTime(&locFTime, &sysTime))
              {
               buf->st_atime = __LocToTime32(sysTime, -1);
              }
             else
              {
               __DosMapError(GetLastError());

               stat = -1;
              }
            }
           else
             buf->st_atime = buf->st_mtime;
          }

         if(!stat)
          {
           if(findbuf.ftCreationTime.dwLowDateTime || findbuf.ftCreationTime.dwHighDateTime)
            {
             if(FileTimeToLocalFileTime(&findbuf.ftCreationTime, &locFTime) && FileTimeToSystemTime(&locFTime, &sysTime))
              {
               buf->st_ctime = __LocToTime32(sysTime, -1);
              }
             else
              {
               __DosMapError(GetLastError());

               stat = -1;
              }
            }
           else
             buf->st_ctime = buf->st_mtime;
          }
        }

       // Fill in buf
       if(!stat && fillCommon)
        {
         buf->st_mode  = __ToXModeW(findbuf.dwFileAttributes, name);
         buf->st_nlink = 1;
         buf->st_size  = ((__int64)(findbuf.nFileSizeHigh))*(0x100000000i64) + (__int64)(findbuf.nFileSizeLow);

         // now set the common fields

         buf->st_rdev = buf->st_dev = (_dev_t)(drive - 1); // A=0, B=1, etc.
        }

       if(hFind != INVALID_HANDLE_VALUE)
         FindClose(hFind);
      }
    }
   else
    {
     _set_errno(ENOENT);

     stat = -1;
    }
  }
 else
  {
   _set_errno(EINVAL);

   stat = -1;
  }

 return stat;
}

// -----------------------------------------------------------------------------
// Работаем по "бразильской системе": конвертируем char в wchar_t
// и вызываем UNICODE-версию функции.
static int __Stat32i64(const char *name, struct _stat32i64 *buf)
{
 int  stat  = -2;


 if(name && buf)
  {
   wchar_t  wName[_MAX_PATH] = L"";


   MultiByteToWideChar(CP_OEMCP, MB_PRECOMPOSED, name, -1, wName, _MAX_PATH - 1);

   stat = __Stat32i64W(wName, buf);
  }

 return stat;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Дополнительная обёртка для _wstat (на самом деле, это дефайн для _wstat32/_wstat64i32,
// в зависимости от _USE_32BIT_TIME_T)
static int __StatW(const wchar_t *name, struct _stat *buf)
{
 int  stat = -2;


 if(name && buf)
  {
   struct _stat32i64  buf32i64 = { 0 };


   stat = __Stat32i64W(name, &buf32i64);

   if(!stat)
    {
     memset(buf, 0, sizeof(*buf));

     // Эту часть копируем как есть
     buf->st_dev   = buf32i64.st_dev;
     buf->st_ino   = buf32i64.st_ino;
     buf->st_mode  = buf32i64.st_mode;
     buf->st_nlink = buf32i64.st_nlink;
     buf->st_uid   = buf32i64.st_uid;
     buf->st_gid   = buf32i64.st_gid;
     buf->st_rdev  = buf32i64.st_rdev;

     // А вот тут в зависимости от разрядности
     // Поле buf32i64.st_size у нас всегда 64 бита, а поля времнни - 32 бита.

     // При попытке записать 64 в 32 проверяем значение. Если оно меньше
     // либо равно максимальному - записываем.
     if(buf32i64.st_size <= (__int64)LONG_MAX)
       buf->st_size = (_off_t)buf32i64.st_size;

     // Чтобы не завязываться на дефайны, завязываемся на фактическую размерность.

     // Для времени проверяем первое, поскольку они однотипны
     if(sizeof(buf->st_atime) == sizeof(buf32i64.st_atime))
      {
       buf->st_atime = buf32i64.st_atime;
       buf->st_mtime = buf32i64.st_mtime;
       buf->st_ctime = buf32i64.st_ctime;
      }
     else
      {
       // Записываем 32 в 64
       buf->st_atime = (time_t)buf32i64.st_atime;
       buf->st_mtime = (time_t)buf32i64.st_mtime;
       buf->st_ctime = (time_t)buf32i64.st_ctime;
      }
    }
  }

 return stat;
}

// -----------------------------------------------------------------------------
// Дополнительная обёртка для _stat (на самом деле, это дефайн для _stat32/_stat64i32,
// в зависимости от _USE_32BIT_TIME_T)
static int __Stat(const char *name, struct _stat *buf)
{
 int  stat  = -2;


 if(name && buf)
  {
   wchar_t  wName[_MAX_PATH] = L"";


   MultiByteToWideChar(CP_OEMCP, MB_PRECOMPOSED, name, -1, wName, _MAX_PATH - 1);

   stat = __StatW(wName, buf);
  }

 return stat;
}

// -----------------------------------------------------------------------------
int stat_rs(const char *path, struct _stat *buffer)
{
 if(IsRsLocaleLoaded)
// #4151
//   return _wstat(RSCH2WEX<>(path, RslDefProgEncode), buffer);
  return __StatW(RSCH2WEX<>(path, RslDefProgEncode), buffer);

// #4151
// return _stat(path, buffer);
 return __Stat(path, buffer);
}

// -----------------------------------------------------------------------------
// Дополнительная обёртка для _wstati64 (на самом деле, это дефайн для _wstat64/_wstat32i64,
// в зависимости от _USE_32BIT_TIME_T)
static int __Stat64W(const wchar_t *name, struct _stati64 *buf)
{
 int  stat = -2;


 if(name && buf)
  {
   struct _stat32i64  buf32i64 = { 0 };


   stat = __Stat32i64W(name, &buf32i64);

   if(!stat)
    {
     memset(buf, 0, sizeof(*buf));

     // Эту часть копируем как есть
     buf->st_dev   = buf32i64.st_dev;
     buf->st_ino   = buf32i64.st_ino;
     buf->st_mode  = buf32i64.st_mode;
     buf->st_nlink = buf32i64.st_nlink;
     buf->st_uid   = buf32i64.st_uid;
     buf->st_gid   = buf32i64.st_gid;
     buf->st_rdev  = buf32i64.st_rdev;

     // А вот тут в зависимости от разрядности
     // Поле buf32i64.st_size у нас всегда 64 бита, а поля времнни - 32 бита.

     // Чтобы не завязываться на дефайны, завязываемся на фактическую размерность.

     if(sizeof(buf->st_size) == sizeof(buf32i64.st_size))
       buf->st_size = buf32i64.st_size;
     else
      {
       // При попытке записать 64 в 32 проверяем значение. Если оно меньше
       // либо равно максимальному - записываем.
       if(buf32i64.st_size <= (__int64)LONG_MAX)
         buf->st_size = (long)buf32i64.st_size;
      }

     // Для времени проверяем первое, поскольку они однотипны
     if(sizeof(buf->st_atime) == sizeof(buf32i64.st_atime))
      {
       buf->st_atime = buf32i64.st_atime;
       buf->st_mtime = buf32i64.st_mtime;
       buf->st_ctime = buf32i64.st_ctime;
      }
     else
      {
       // Записываем 32 в 64
       buf->st_atime = (__time64_t)buf32i64.st_atime;
       buf->st_mtime = (__time64_t)buf32i64.st_mtime;
       buf->st_ctime = (__time64_t)buf32i64.st_ctime;
      }
    }
  }

 return stat;
}

// -----------------------------------------------------------------------------
// Дополнительная обёртка для _stati64 (на самом деле, это дефайн для _stat64/_stat32i64,
// в зависимости от _USE_32BIT_TIME_T)
static int __Stat64(const char *name, struct _stati64 *buf)
{
 int  stat  = -2;


 if(name && buf)
  {
   wchar_t  wName[_MAX_PATH] = L"";


   MultiByteToWideChar(CP_OEMCP, MB_PRECOMPOSED, name, -1, wName, _MAX_PATH - 1);

   stat = __Stat64W(wName, buf);
  }

 return stat;
}

// -----------------------------------------------------------------------------
#if _MSC_VER >= 1400
    // -------------------------------------------------------------------------
    int stati64_rs(const char *path, struct _stati64 *buffer)
    {
     if(IsRsLocaleLoaded)
// #4151
//       return _wstati64(RSCH2WEX<>(path, RslDefProgEncode), buffer);
       return __Stat64W(RSCH2WEX<>(path, RslDefProgEncode), buffer);

// #4151
//     return _stati64(path, buffer);
     return __Stat64(path, buffer);
    }

    // -------------------------------------------------------------------------
    int stat32i64_rs(const char *path, struct _stat32i64 *buffer)
    {
     if(IsRsLocaleLoaded)
// #4151
//       return _wstat32i64(RSCH2WEX<>(path, RslDefProgEncode), buffer);
       return __Stat32i64W(RSCH2WEX<>(path, RslDefProgEncode), buffer);

// #4151
//     return _stat32i64(path, buffer);
     return __Stat32i64(path, buffer);
    }
#endif

// -----------------------------------------------------------------------------
int utime_rs(const char *path, struct _utimbuf *times)
{
 if(IsRsLocaleLoaded)
   return _wutime(RSCH2WEX<>(path, RslDefProgEncode), times);

 return _utime(path, times);
}

// -----------------------------------------------------------------------------
FILE *fopen_rs(const char *fileName, const char *mode)
{
 FILE  *fp;


 if(IsRsLocaleLoaded)
   fp = _wfopen(RSCH2WEX<>(fileName, RslDefProgEncode), RSCH2WEX<>(mode, RslDefProgEncode));
 else
   fp = fopen(fileName, mode);

 return fp;
}

// -----------------------------------------------------------------------------
int access_rs(const char *fileName, int mode)
{
 if(IsRsLocaleLoaded)
   return _waccess(RSCH2WEX<>(fileName, RslDefProgEncode), mode);

 return _access(fileName, mode);
}

// -----------------------------------------------------------------------------
int access_s_rs(const char *fileName, int mode)
{
 if(IsRsLocaleLoaded)
   return _waccess_s(RSCH2WEX<>(fileName, RslDefProgEncode), mode);

 return _access_s(fileName, mode);
}

// -----------------------------------------------------------------------------
int sopen_rs(const char *fileName, int oflag, int shareFlag, int pmode)
{
 int  fd;


 if(IsRsLocaleLoaded)
   fd = _wsopen(RSCH2WEX<>(fileName, RslDefProgEncode), oflag, shareFlag, pmode);
 else
   fd = _sopen(fileName, oflag, shareFlag, pmode);

 return fd;
}

// -----------------------------------------------------------------------------
FILE *fdopen_rs(int fd, const char *mode)
{
 FILE  *fp;


 if(IsRsLocaleLoaded)
   fp = _wfdopen(fd, RSCH2WEX<>(mode, RslDefProgEncode));
 else
   fp = _fdopen(fd, mode);

 return fp;
}

// -----------------------------------------------------------------------------
int unlink_rs(const char *fileName)
{
 if(IsRsLocaleLoaded)
   return _wunlink(RSCH2WEX<>(fileName, RslDefProgEncode));

 return _unlink(fileName);
}

// -----------------------------------------------------------------------------
int rename_rs(const char *from, const char *to)
{
 if(IsRsLocaleLoaded)
   return _wrename(RSCH2WEX<>(from, RslDefProgEncode), RSCH2WEX<>(to, RslDefProgEncode));

 return rename(from, to);
}

// -----------------------------------------------------------------------------
int mkdir_rs(const char *dirName)
{
 if(IsRsLocaleLoaded)
   return _wmkdir(RSCH2WEX<>(dirName, RslDefProgEncode));

 return _mkdir(dirName);
}

// -----------------------------------------------------------------------------
int rmdir_rs(const char *dirName)
{
 if(IsRsLocaleLoaded)
   return _wrmdir(RSCH2WEX<>(dirName, RslDefProgEncode));

 return _rmdir(dirName);
}

// -----------------------------------------------------------------------------
static _wfinddata_t  wFF;

// -----------------------------------------------------------------------------
intptr_t _findfirst_rs(const char *filespec, struct _finddata_t *fileinfo)
{
 if(IsRsLocaleLoaded)
  {
   intptr_t  result = _wfindfirst(RSCH2WEX<>(filespec, RslDefProgEncode), &wFF);


   if(result != -1)
    {
     fileinfo->attrib      = wFF.attrib;
     fileinfo->time_create = wFF.time_create;    /* -1 for FAT file systems */
     fileinfo->time_access = wFF.time_access;    /* -1 for FAT file systems */
     fileinfo->time_write  = wFF.time_write;
     fileinfo->size        = wFF.size;

     LCW2RSCH(fileinfo->name, 260, wFF.name, 260, RslDefProgEncode);
    }

   return result;
  }

 return _findfirst(filespec, fileinfo);
}

// -----------------------------------------------------------------------------
int _findnext_rs(intptr_t handle, struct _finddata_t *fileinfo)
{
 if(IsRsLocaleLoaded)
  {
   intptr_t  result = _wfindnext(handle, &wFF);


   if(result != -1)
    {
     fileinfo->attrib      = wFF.attrib;
     fileinfo->time_create = wFF.time_create;    /* -1 for FAT file systems */
     fileinfo->time_access = wFF.time_access;    /* -1 for FAT file systems */
     fileinfo->time_write  = wFF.time_write;
     fileinfo->size        = wFF.size;

     LCW2RSCH(fileinfo->name, 260, wFF.name, 260, RslDefProgEncode);
    }

   return (int)result;
  }

 return _findnext(handle, fileinfo);
}

#if defined(_MSC_VER) && (_MSC_VER >= 1400)
    // -----------------------------------------------------------------------------
    static _wfinddata32i64_t  wFD;

    // -----------------------------------------------------------------------------
    intptr_t _findfirst32i64_rs(const char *filespec, struct _finddata32i64_t *fileinfo)
    {
     if(IsRsLocaleLoaded)
      {
       intptr_t  result = _wfindfirst32i64(RSCH2WEX<>(filespec, RslDefProgEncode), &wFD);


       if(result != -1)
        {
         fileinfo->attrib      = wFD.attrib;
         fileinfo->time_create = wFD.time_create;    /* -1 for FAT file systems */
         fileinfo->time_access = wFD.time_access;    /* -1 for FAT file systems */
         fileinfo->time_write  = wFD.time_write;
         fileinfo->size        = wFD.size;

         LCW2RSCH(fileinfo->name, 260, wFD.name, 260, RslDefProgEncode);
        }

       return result;
      }

     return _findfirst32i64(filespec, fileinfo);
    }

    // -----------------------------------------------------------------------------
    int _findnext32i64_rs(intptr_t handle, struct _finddata32i64_t *fileinfo)
    {
     if(IsRsLocaleLoaded)
      {
       intptr_t  result = _wfindnext32i64(handle, &wFD);


       if(result != -1)
        {
         fileinfo->attrib      = wFD.attrib;
         fileinfo->time_create = wFD.time_create;    /* -1 for FAT file systems */
         fileinfo->time_access = wFD.time_access;    /* -1 for FAT file systems */
         fileinfo->time_write  = wFD.time_write;
         fileinfo->size        = wFD.size;

         LCW2RSCH(fileinfo->name, 260, wFD.name, 260, RslDefProgEncode);
        }

       return (int)result;
      }

     return _findnext32i64(handle, fileinfo);
    }
#endif
//-@@------ END ___________________________________________________ --------@E-
#else  // USE_UFILE
    // -----------------------------------------------------------------------------
    #define  TMP_BUFF_SIZE  1024

    // -----------------------------------------------------------------------------
    static char  TmpBuff[TMP_BUFF_SIZE];

    // -----------------------------------------------------------------------------
    int ufGetEncodeFromStr(const char *encStr)
    {
     return UF_RSOEM;
    }

    // -----------------------------------------------------------------------------
    FILE *ufOpen2(const char *fileName, int openMode, int fileEncode, int progEncode, int shareMode, int *errcode)
    {
     const char  *mode = "r";
     FILE        *fp;


     switch(openMode)
          {
           case UF_READ:
                mode = "r";
                break;

           case UF_WRITE:
                mode = "w";
                break;

           case UF_APPEND:
                mode = "a";
                break;
          }

     fp =  fopen(fileName, mode);

     if(!fp && errcode)
       *errcode = errno;

     return fp;
    }

    // -----------------------------------------------------------------------------
    FILE *ufOpen(const char *fileName, int openMode, int fileEncode, int progEncode, int shareMode)
    {
     return ufOpen2(fileName, openMode, fileEncode, progEncode, shareMode, NULL);
    }

    // -----------------------------------------------------------------------------
    void ufClose(FILE *fl)
    {
     fclose(fl);
    }

    // -----------------------------------------------------------------------------
    int ufGetch(FILE *f)
    {
     return getc(f);
    }

    // -----------------------------------------------------------------------------
    void ufUnGetch(FILE *fl, char ch)
    {
     ungetc(ch, fl);
    }

    // -----------------------------------------------------------------------------
    bool ufPutch(FILE *fl, char ch)
    {
     return putc((unsigned char)ch, fl) != EOF;
    }

    // -----------------------------------------------------------------------------
    void ufFlush(FILE *fl)
    {
     fflush(fl);
    }

    // -----------------------------------------------------------------------------
    int ufWrite(FILE *fl, const char *buff, int sz)
    {
     return fwrite(buff, 1, sz, fl);
    }

    // -----------------------------------------------------------------------------
    int ufRead(FILE *fl, char *buff, int sz)
    {
     return fread(buff, 1, sz, fl);
    }

    // -----------------------------------------------------------------------------
    char* ufReadLine(FILE *fl)
    {
     return fgets(TmpBuff, TMP_BUFF_SIZE, fl);
    }

    // -----------------------------------------------------------------------------
    bool ufError(FILE *fl)
    {
     return ferror(fl) ? true : false;
    }

    // -----------------------------------------------------------------------------
    bool ufWriteLine(FILE *fl, const char *line)
    {
     fprintf(fl, "%s\n", line);

     return !ferror(fl);
    }

    // -----------------------------------------------------------------------------
    bool ufRewind(FILE *fl)
    {
     rewind(fl);

     return !ferror(fl);
    }

    // -----------------------------------------------------------------------------
    char* ufGetTmpBuffer(FILE *fl, size_t sz)
    {
     if(sz > TMP_BUFF_SIZE)
       return NULL;

     return TmpBuff;
    }

    // -----------------------------------------------------------------------------
    int ufPrintf(FILE *fl, const char *fmt, ...)
    {
     va_list  args;


     va_start(args, fmt);

     vfprintf(fl, fmt, args);

     va_end(args);

     return !ferror(fl);
    }

    // -----------------------------------------------------------------------------
    void ufSetEncode(FILE *fl, int enc)
    {
    }

    // -----------------------------------------------------------------------------
    int ufGetEncode(FILE *fl)
    {
     return UF_RSOEM;
    }

    // -----------------------------------------------------------------------------
    int ufFileno(FILE *fl)
    {
     return fileno(fl);
    }
#endif // USE_UFILE


// -----------------------------------------------------------------------------
#ifdef TEST
int main (int, char**)
   {
   TUniFile  fileOut (UF_UTF8);

   if (fileOut.open ("testUTF8.txt", UF_READ))
      {
      fileOut.println ("Hello\n %s", "friend!");
      fileOut.println ("Hello\r\n %s", "friend!");

      //fileOut.putWChar (L'H');
      //fileOut.putWChar (L'\n');
      //fileOut.putWChar (L'\n');
      //fileOut.putWChar (L'l');
      //fileOut.putWChar (L'l');
      //fileOut.putWChar (L'\n');

      //fileOut.writeLine ("He\n\n\nllo!");

      //fileOut.writeLine ("");
      //fileOut.writeLine ("Hello!, Привет!");
      //fileOut.writeLine ("Hello!, Привет!");

      fileOut.close ();
      }

   TUniFile  fileIn (UF_RSOEM);

   if (fileIn.open ("testUTF8.txt"))
   //if (fileIn.open ("testunix.txt"))
      {
      char* line;
      while ((line = fileIn.readLine ()) != NULL)
         printf ("%s\n", line);
      }

   /*
   if (file.open ("testUTF8.txt"))
      {
      wchar_t wch;
      while (file.getWChar (&wch))
         printf ("%04X\n", wch);
      }

   if (file.open ("testUTF16LE.txt"))
      {
      wchar_t wch;
      while (file.getWChar (&wch))
         printf ("%04X\n", wch);
      }

   if (file.open ("testUTF16BE.txt"))
      {
      wchar_t wch;
      while (file.getWChar (&wch))
         printf ("%04X\n", wch);
      }
   */

   /*
   printf ("----------------\n");

   TUniFile  fileOEM (TUniFile::LCOEM);

   if (fileOEM.open ("testOEM.txt"))
      {
      wchar_t wch;
      while (fileOEM.getWChar (&wch))
         printf ("%04X\n", wch);
      }


   TUniFile  fileANSI (TUniFile::LCANSI);

   if (fileANSI.open ("testANSI.txt"))
      {
      wchar_t wch;
      while (fileANSI.getWChar (&wch))
         printf ("%04X\n", wch);
      }
   */

   return 0;
   }
#endif

/* <-- EOF --> */
