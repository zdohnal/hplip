/*****************************************************************************\
    hpcupscdmfax.h : HP Cups Fax Filter

    Copyright (c) 2001 - 2015, HP Co.
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. Neither the name of the Hewlett-Packard nor the names of its
       contributors may be used to endorse or promote products derived
       from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
    OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
    NOT LIMITED TO, PATENT INFRINGEMENT; PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
    STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
    IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
\*****************************************************************************/

#ifndef HPCUPSCDMFAX_H
#define HPCUPSCDMFAX_H
#include <cups/ppd.h>
#include "jetlib.h"

extern "C"
{
#ifndef JPEGLIB_H
#include "jpeglib.h"
#endif
}

extern "C"
{
#include <stddef.h>
#include <stdio.h>
#include <setjmp.h>
}

#define STORE_JPEG
#define PRINT_CUPS_HEADER

typedef unsigned char BYTE;
typedef BYTE *PBYTE;

#define CUPS_TMP_DIR   getenv("TMPDIR") ? : getenv("HOME") ?:"/tmp"

#define FILE_NAME_SIZE 128
#define MAX_FILE_PATH_LEN 128
#define MAX_BUFF_SIZE 409600

unsigned long m_ulCookie;
HP_StreamHandleType m_ptJetlibStream;
const unsigned int QTABLE_SIZE = 64;
const BYTE JREndPageSeq[] = {0x44};
const BYTE JREndSessionSeq[] = {0x42};
const BYTE JRCloseDataSourceSeq[] = {0x49};

const char pstrFaxGroupJAType[]                         = "GROUP";
const char pstrFaxStringJAType[]                        = "STRING";
const char pstrFaxEnumJAType[]                          = "ENUM";

const char pstrFaxDestGroupJANamePrefix[]               = "fax.faxDest";
const char pstrFaxDestJANameNumberSuffix[]              = "number";
const char pstrFaxDestJANameBillingCodeSuffix[]         = "billingCode";

const char pstrFaxDestJANameBillingCode[]               = "fax.faxDest.billingCode";
const char pstrFaxDestJANameResMode[]                   = "fax.attachment.resMode";
const char pstrFaxDestJANameNotificationType[]          = "fax.notification.notifyOn";
const char pstrFaxDestJANameNotificationMethod[]        = "fax.notification.notifyType";
const char pstrFaxEMailDestGroupJANamePrefix[]          = "fax.notification.emailDest";
const char pstrFaxEMailDestGroupJANameAddressSuffix[]   = "address";
const char pstrFaxThumbnailOnConfirmationPage[]         = "fax.thumbnailconfirmation";
const char PJLExit[]        = "\x1b%-12345X@PJL EOJ\012\x1b%-12345X";

void clearStream();
int send_data_to_stdout(int fromFD);
int createTempFile(char *szFileName, FILE **pFilePtr);

typedef struct _QTABLEINFO
{

   unsigned int qtable0[QTABLE_SIZE];
   unsigned int qtable1[QTABLE_SIZE];
   unsigned int qtable2[QTABLE_SIZE];
   int qFactor;

} QTABLEINFO, *PQTABLEINFO;

BYTE *fpJPEGBuffer;          // This is passed destination JPEG buffer

JSAMPROW fRow_array[1];      // JPEG processing routine expects an
                             //   array of pointers. We always send
                             //   1 row at a time.

unsigned long fBufferSize;
struct jpeg_compress_struct cinfo;
struct jpeg_decompress_struct dinfo;
struct jpeg_error_mgr jerr;
struct jpeg_destination_mgr dest;
jmp_buf setjmp_buffer; // for return to caller



#endif /* hpcupscdmfax_H */
