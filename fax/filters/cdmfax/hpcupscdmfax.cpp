/*****************************************************************************\
    hpcupscdmfax.cpp : HP CUPS fax filter

    Copyright (c) 2001 - 2010, HP Co.
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

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <syslog.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <math.h>
#include <cups/cups.h>
#include <cups/raster.h>
#include <string>
#ifdef FALSE
#undef FALSE
#endif
#ifdef TRUE
#undef TRUE
#endif

#include "hpcupscdmfax.h"
#include "bug.h"
using namespace std;

static int iLogLevel = 1;
char hpFileName[MAX_FILE_PATH_LEN];

#define DBG(args...) syslog(LOG_INFO, __FILE__ " " STRINGIZE(__LINE__) ": " args)

static void GetLogLevel()
{
    FILE *fp;
    char str[258];
    char *p;
    fp = fopen("/etc/cups/cupsd.conf", "r");
    if (fp == NULL)
        return;
    while (!feof(fp))
    {
        if (!fgets(str, 256, fp))
        {
            break;
        }
        if ((p = strstr(str, "hpLogLevel")))
        {
            p += strlen("hpLogLevel") + 1;
            iLogLevel = atoi(p);
            break;
        }
    }
    fclose(fp);
}

void PrintCupsHeader(cups_page_header2_t m_cupsHeader)
{
    if (iLogLevel == 0)
    {
        return;
    }
    BUG("DEBUG: HPFAX - startPage...\n");
    BUG("DEBUG: HPFAX - MediaClass = \"%s\"\n", m_cupsHeader.MediaClass);
    BUG("DEBUG: HPFAX - MediaColor = \"%s\"\n", m_cupsHeader.MediaColor);
    BUG("DEBUG: HPFAX - MediaType = \"%s\"\n", m_cupsHeader.MediaType);
    BUG("DEBUG: HPFAX - OutputType = \"%s\"\n", m_cupsHeader.OutputType);
    BUG("DEBUG: HPFAX - AdvanceDistance = %d\n", m_cupsHeader.AdvanceDistance);
    BUG("DEBUG: HPFAX - AdvanceMedia = %d\n", m_cupsHeader.AdvanceMedia);
    BUG("DEBUG: HPFAX - Collate = %d\n", m_cupsHeader.Collate);
    BUG("DEBUG: HPFAX - CutMedia = %d\n", m_cupsHeader.CutMedia);
    BUG("DEBUG: HPFAX - Duplex = %d\n", m_cupsHeader.Duplex);
    BUG("DEBUG: HPFAX - HWResolution = [ %d %d ]\n", m_cupsHeader.HWResolution[0], m_cupsHeader.HWResolution[1]);
    BUG("DEBUG: HPFAX - ImagingBoundingBox = [ %d %d %d %d ]\n",
        m_cupsHeader.ImagingBoundingBox[0], m_cupsHeader.ImagingBoundingBox[1],
        m_cupsHeader.ImagingBoundingBox[2], m_cupsHeader.ImagingBoundingBox[3]);
    BUG("DEBUG: HPFAX - InsertSheet = %d\n", m_cupsHeader.InsertSheet);
    BUG("DEBUG: HPFAX - Jog = %d\n", m_cupsHeader.Jog);
    BUG("DEBUG: HPFAX - LeadingEdge = %d\n", m_cupsHeader.LeadingEdge);
    BUG("DEBUG: HPFAX - Margins = [ %d %d ]\n", m_cupsHeader.Margins[0], m_cupsHeader.Margins[1]);
    BUG("DEBUG: HPFAX - ManualFeed = %d\n", m_cupsHeader.ManualFeed);
    BUG("DEBUG: HPFAX - MediaPosition = %d\n", m_cupsHeader.MediaPosition);
    BUG("DEBUG: HPFAX - MediaWeight = %d\n", m_cupsHeader.MediaWeight);
    BUG("DEBUG: HPFAX - MirrorPrint = %d\n", m_cupsHeader.MirrorPrint);
    BUG("DEBUG: HPFAX - NegativePrint = %d\n", m_cupsHeader.NegativePrint);
    BUG("DEBUG: HPFAX - NumCopies = %d\n", m_cupsHeader.NumCopies);
    BUG("DEBUG: HPFAX - Orientation = %d\n", m_cupsHeader.Orientation);
    BUG("DEBUG: HPFAX - OutputFaceUp = %d\n", m_cupsHeader.OutputFaceUp);
    BUG("DEBUG: HPFAX - PageSize = [ %d %d ]\n", m_cupsHeader.PageSize[0], m_cupsHeader.PageSize[1]);
    BUG("DEBUG: HPFAX - Separations = %d\n", m_cupsHeader.Separations);
    BUG("DEBUG: HPFAX - TraySwitch = %d\n", m_cupsHeader.TraySwitch);
    BUG("DEBUG: HPFAX - Tumble = %d\n", m_cupsHeader.Tumble);
    BUG("DEBUG: HPFAX - cupsWidth = %d\n", m_cupsHeader.cupsWidth);
    BUG("DEBUG: HPFAX - cupsHeight = %d\n", m_cupsHeader.cupsHeight);
    BUG("DEBUG: HPFAX - cupsMediaType = %d\n", m_cupsHeader.cupsMediaType);
    BUG("DEBUG: HPFAX - cupsRowStep = %d\n", m_cupsHeader.cupsRowStep);
    BUG("DEBUG: HPFAX - cupsBitsPerColor = %d\n", m_cupsHeader.cupsBitsPerColor);
    BUG("DEBUG: HPFAX - cupsBitsPerPixel = %d\n", m_cupsHeader.cupsBitsPerPixel);
    BUG("DEBUG: HPFAX - cupsBytesPerLine = %d\n", m_cupsHeader.cupsBytesPerLine);
    BUG("DEBUG: HPFAX - cupsColorOrder = %d\n", m_cupsHeader.cupsColorOrder);
    BUG("DEBUG: HPFAX - cupsColorSpace = %d\n", m_cupsHeader.cupsColorSpace);
    BUG("DEBUG: HPFAX - cupsCompression = %d\n", m_cupsHeader.cupsCompression);
    BUG("DEBUG: HPFAX - cupsPageSizeName = %s\n", m_cupsHeader.cupsPageSizeName);
}

static void Gjpeg_error(j_common_ptr cinfo)
{
    // The standard behavior is to send a message to stderr.
    // Since this is icky for Gumby, we change it to do nothing
}

bool CompressJPEG(cups_raster_t *cups_raster,
                  cups_page_header2_t cups_header,
                  QTABLEINFO *pQTable,
                  BYTE **oCompressedBuf,
                  unsigned long *oCompressedBufSize)

{
    //
    // To remove the ugly if elses all over in this function.
    //
    int iColorsUsed = 1;
    //----------------------------------------------------------------
    //  Setup for compression
    //----------------------------------------------------------------

    //----------------------------------------------------------------
    // JPEG Lib Step 1: Allocate and initialize JPEG compression object
    //----------------------------------------------------------------
    cinfo.err = jpeg_std_error(&jerr);

    // Fix the error handler to return when an error occurs,
    // the default exit()s which is nasty for a driver to do.
    jerr.error_exit = Gjpeg_error;
    // Set the return jump address. This must be done now since
    //  jpeg_create_compress could cause an error.
    if (setjmp(setjmp_buffer))
    {

        // If we get here, the JPEG code has signaled an error.
        //* We need to clean up the JPEG object, and return.

        jpeg_destroy_compress(&cinfo);
        return FALSE;
    }
    jpeg_create_compress(&cinfo);
    jpeg_set_defaults(&cinfo);
    jpeg_mem_dest(&cinfo, oCompressedBuf, oCompressedBufSize);
    cinfo.in_color_space = JCS_GRAYSCALE;
    
    cinfo.image_width = cups_header.cupsWidth;
    cinfo.image_height = cups_header.cupsHeight;
    cinfo.input_components = iColorsUsed; // change if bit depths others than 24bpp are ever needed
    cinfo.data_precision = 8;

    jpeg_default_colorspace(&cinfo);
    //jpeg_set_quality(&cinfo, 50, true);

    //
    // UGLY put the quant table addition to the JPEG in to a function.
    // Create a static quant table here.
    //
    static unsigned int mojave_quant_table1[64] = {2, 3, 4, 5, 5, 5, 5, 5,
                                                   3, 6, 5, 8, 5, 8, 5, 8,
                                                   4, 5, 5, 5, 5, 5, 5, 5,
                                                   5, 8, 5, 8, 5, 8, 5, 8,
                                                   5, 5, 5, 5, 5, 5, 5, 5,
                                                   5, 8, 5, 8, 5, 8, 5, 8,
                                                   5, 5, 5, 5, 5, 5, 5, 5,
                                                   5, 8, 5, 8, 5, 8, 5, 8};
    //
    // Use this variable for representing the scale_factor for now.
    //
    unsigned int iScaleFactor = pQTable->qFactor;

    //
    // Mojave specific Q-Tables will be added here. We do the following:
    //  1. Add three Q-Tables.
    //  2. Scale the Q-Table elemets with the given scale factor.
    //  3. Check to see if any of the element is in the table is greater than 255
    //     reser that elemet to 255.
    //  5. There is a specific scaling need to be done to the first 6
    //     elements in the matrix. This required to achieve the better
    //     compression ratio.
    //  4. Check to see if any the of recently modified element is
    //     greater than 255, reset that with 255.
    //  Following for loop implements the above logic.
    //
    //  Please refer to sRGBLaserHostBasedSoftwareERS.doc v9.0 section 5.2.5.3.1.1
    //  for more details.
    //
    //  [NOTE] These loop needs to be further optimized.
    //
    for (int i = 0; i < 3; i++)
    {
        //
        // Adding Q-Table.
        //
        jpeg_add_quant_table(&cinfo, i, mojave_quant_table1, 0, false);

        //
        // Scaling the Q-Table elements.
        // Reset the element to 255, if it is greater than 255.
        //
        for (int j = 1; j < 64; j++)
        {
            cinfo.quant_tbl_ptrs[i]->quantval[j] = (unsigned short)(mojave_quant_table1[j] * iScaleFactor);

            //
            // [PERF]Since we are using a scale_factor from 1 to 10 do we really need this.
            //
            if (cinfo.quant_tbl_ptrs[i]->quantval[j] > 255)
                cinfo.quant_tbl_ptrs[i]->quantval[j] = 255;
        }
        //
        // Special scaling for first 6 elements in the table.
        // Reset the specially scaled elements 255, if it is greater than 255.
        //

        //
        // 1st component in the table. Unchanged, I need not change anything here.
        //
        cinfo.quant_tbl_ptrs[i]->quantval[0] = (unsigned short)mojave_quant_table1[0];

        //
        // 2nd and 3rd components in the zig zag order
        //
        //
        // The following dTemp is being used  to ciel the vales: e.g 28.5 to 29
        //
        double dTemp = mojave_quant_table1[1] * (1 + 0.25 * (iScaleFactor - 1));
        dTemp = (dTemp + 0.5) / 1;
        cinfo.quant_tbl_ptrs[i]->quantval[1] = (unsigned short)dTemp;
        if (cinfo.quant_tbl_ptrs[i]->quantval[1] > 255)
            cinfo.quant_tbl_ptrs[i]->quantval[1] = 255;

        dTemp = mojave_quant_table1[8] * (1 + 0.25 * (iScaleFactor - 1));
        dTemp = (dTemp + 0.5) / 1;
        cinfo.quant_tbl_ptrs[i]->quantval[8] = (unsigned short)dTemp;
        if (cinfo.quant_tbl_ptrs[i]->quantval[8] > 255)
            cinfo.quant_tbl_ptrs[i]->quantval[8] = 255;

        //
        // 4th, 5th and 6th components in the zig zag order
        //
        dTemp = mojave_quant_table1[16] * (1 + 0.50 * (iScaleFactor - 1));
        dTemp = (dTemp + 0.5) / 1;
        cinfo.quant_tbl_ptrs[i]->quantval[16] = (unsigned short)dTemp;
        if (cinfo.quant_tbl_ptrs[i]->quantval[16] > 255)
            cinfo.quant_tbl_ptrs[i]->quantval[16] = 255;

        dTemp = mojave_quant_table1[9] * (1 + 0.50 * (iScaleFactor - 1));
        dTemp = (dTemp + 0.5) / 1;
        cinfo.quant_tbl_ptrs[i]->quantval[9] = (unsigned short)dTemp;
        if (cinfo.quant_tbl_ptrs[i]->quantval[9] > 255)
            cinfo.quant_tbl_ptrs[i]->quantval[9] = 255;

        dTemp = mojave_quant_table1[2] * (1 + 0.50 * (iScaleFactor - 1));
        dTemp = (dTemp + 0.5) / 1;
        cinfo.quant_tbl_ptrs[i]->quantval[2] = (unsigned short)dTemp;
        if (cinfo.quant_tbl_ptrs[i]->quantval[2] > 255)
            cinfo.quant_tbl_ptrs[i]->quantval[2] = 255;
    }
    //
    // Hard code to use sampling mode 4:4:4
    //
    cinfo.comp_info[0].h_samp_factor = 1;
    cinfo.comp_info[0].v_samp_factor = 1;
    cinfo.write_JFIF_header = cinfo.write_Adobe_marker = false;
    jpeg_suppress_tables(&cinfo, true); //b4 true

    //----------------------------------------------------------------
    // JPEG Lib Step 4: Start the compression cycle
    //  set destination to table file
    //  jpeg_write_tables(cinfo);
    //  set destination to image file
    //  jpeg_start_compress(cinfo, FALSE);
    //----------------------------------------------------------------
    jpeg_start_compress(&cinfo, true);

    //----------------------------------------------------------------
    // This completes the JPEG setup.
    //----------------------------------------------------------------

    // do the jpeg compression
    JSAMPROW pRowArray[1];
    unsigned char *buffer = NULL;

    buffer = (unsigned char *)malloc(cups_header.cupsBytesPerLine);
    /* read raster data */
    for (unsigned int y = 0; y < cups_header.cupsHeight; y++)
    {
        memset(buffer, 0, cups_header.cupsBytesPerLine);
        if (cupsRasterReadPixels(cups_raster, buffer, cups_header.cupsBytesPerLine) == 0)
            break;
        pRowArray[0] = buffer;
        jpeg_write_scanlines(&cinfo, pRowArray, 1);
    }
    if (buffer != NULL)
    {
        free(buffer);
    }
    //----------------------------------------------------------------
    // JPEG Lib Step 6: Finish compression
    //----------------------------------------------------------------
    // Tell the compressor about the extra buffer space for the trailer
    jpeg_finish_compress(&cinfo);
    //
    // Read the quantization tables used for the compression.
    // BUGBUG - it's probably better to use the jpeg_write_tables routine,
    // but this works for now. Hack alert!!
    //
    /*
    if (cinfo.quant_tbl_ptrs[0] != NULL)
    {
        for (int iI = 0; iI < QTABLE_SIZE; iI++)
        {
            pQTable->qtable0[iI] = (unsigned int)cinfo.quant_tbl_ptrs[0]->quantval[iI];
        }
    }

    if (cinfo.quant_tbl_ptrs[1] != NULL)
    {

        for (int iI = 0; iI < QTABLE_SIZE; iI++)
        {
            pQTable->qtable1[iI] = (unsigned int)cinfo.quant_tbl_ptrs[1]->quantval[iI];
        }
    }

    if (cinfo.quant_tbl_ptrs[2] != NULL)
    {

        for (int iI = 0; iI < QTABLE_SIZE; iI++)
        {
            pQTable->qtable2[iI] = (unsigned int)cinfo.quant_tbl_ptrs[2]->quantval[iI];
        }
    }
    */
    //----------------------------------------------------------------
    // JPEG Lib Step 7: Destroy the compression object
    //----------------------------------------------------------------
    jpeg_destroy_compress(&cinfo);

    return true;
}

int ProcessRasterData(cups_raster_t *cups_raster, int fdFax)
{
    int status = 0;

    cups_page_header2_t cups_header;
    int iPageNum = 0;

    unsigned char *pDataArray = NULL;
    unsigned char *pDataArray1 = NULL;
    unsigned char *pDataArrayMoving = NULL;
    unsigned short wCoordinate = 0;
    unsigned long dwNumBytesAligned = 0, dwRemainder = 0, dwNumBytes = 0;
    QTABLEINFO m_QTableInfo;
    memset(&m_QTableInfo, 0, sizeof(m_QTableInfo));
    m_QTableInfo.qFactor = 6;
    while (cupsRasterReadHeader2(cups_raster, &cups_header))
    {
        iPageNum++;
        clearStream();
#ifdef PRINT_CUPS_HEADER
        PrintCupsHeader(cups_header);
#endif

        fprintf(stderr, "b4 HP_BeginPage_1\n");
        HP_BeginPage_1(m_ptJetlibStream, HP_ePortraitOrientation, (HP_UByte)0);
        write(fdFax, m_ptJetlibStream->HP_OutBuffer, m_ptJetlibStream->HP_CurrentBufferLen);
        fprintf(stderr, "m_ptJetlibStream->HP_CurrentBufferLen = %ld \n", m_ptJetlibStream->HP_CurrentBufferLen);
        clearStream();
        fprintf(stderr, "b4 HP_JR3BeginImage_1\n");

        /*wCoordinate = (unsigned short)(((cups_header.cupsWidth * 200) / 254) - 200);
        dwNumBytesAligned = (unsigned long)wCoordinate * 3;
        dwRemainder = dwNumBytesAligned % 96;
        if (dwRemainder != 0)
        {
            dwNumBytes = dwNumBytesAligned + (96 - dwRemainder);
            dwNumBytesAligned = dwNumBytes / 3;
            wCoordinate = (unsigned short)dwNumBytesAligned;
        }
        wCoordinate = cups_header.cupsWidth;
        fprintf(stderr, "wCoordinate = %d \n", wCoordinate);*/
        HP_JR3BeginImage_1(m_ptJetlibStream, cups_header.cupsWidth, cups_header.cupsHeight, 1, cups_header.cupsHeight, hp_JetReadyVersion30, eJR_Gray, 0);
        write(fdFax, m_ptJetlibStream->HP_OutBuffer, m_ptJetlibStream->HP_CurrentBufferLen);
        fprintf(stderr, "m_ptJetlibStream->HP_CurrentBufferLen = %ld \n", m_ptJetlibStream->HP_CurrentBufferLen);

        fpJPEGBuffer = NULL;
        fprintf(stderr, "b4 CompressJPEG \n");
        CompressJPEG(cups_raster, cups_header, &m_QTableInfo, &fpJPEGBuffer, &fBufferSize);
        fprintf(stderr, "after CompressJPEG \n");
        fprintf(stderr, "fBufferSize = %ld \n", fBufferSize);
        if (fpJPEGBuffer != NULL && fBufferSize > 0)
        {

            long startLine = 0, blockHeight = 0, dataLength = 0, dwLocalCompressBmpBitsSize = 0;
            dataLength = fBufferSize + 6;
            fprintf(stderr, "dataLength = %ld \n", dataLength);
            // Create the compressed data array along with the header
            pDataArray = new BYTE[dataLength];
            // Zero-out the memory
            memset(pDataArray, 0, dataLength);
            pDataArrayMoving = pDataArray1 = pDataArray;
            // Hack Hack. TODO: Change later
            dwLocalCompressBmpBitsSize = fBufferSize;
            *pDataArrayMoving++ = 0x21;
            *pDataArrayMoving++ = 0x90;
            *pDataArrayMoving++ = (BYTE)(255 & (dwLocalCompressBmpBitsSize));
            *pDataArrayMoving++ = (BYTE)(255 & (dwLocalCompressBmpBitsSize = (dwLocalCompressBmpBitsSize >> 8)));
            *pDataArrayMoving++ = (BYTE)(255 & (dwLocalCompressBmpBitsSize = (dwLocalCompressBmpBitsSize >> 8)));
            *pDataArrayMoving++ = (BYTE)(255 & (dwLocalCompressBmpBitsSize = (dwLocalCompressBmpBitsSize >> 8)));

            memcpy(pDataArrayMoving, fpJPEGBuffer, fBufferSize);
            fprintf(stderr, "b4 HP_JR3ReadImage_1 \n");
            clearStream();
            HP_JR3ReadImage_1(m_ptJetlibStream, 0, cups_header.cupsHeight, hp_JetReadyVersion30, 0, dataLength);
            write(fdFax, m_ptJetlibStream->HP_OutBuffer, m_ptJetlibStream->HP_CurrentBufferLen);
            // Send the data using HP_RawUByteArray
            clearStream();
            fprintf(stderr, "b4 HP_RawUByteArray \n");
            HP_RawUByteArray(m_ptJetlibStream, pDataArray1, dataLength);
#ifdef STORE_JPEG
            char jepgFileName[MAX_FILE_PATH_LEN];
            memset(jepgFileName, 0, MAX_FILE_PATH_LEN);
            snprintf(jepgFileName, sizeof(jepgFileName), "%s/hp_fax_page%d.jpg", CUPS_TMP_DIR, iPageNum);
            FILE *pFile = fopen(jepgFileName, "w+b");
            if (pFile)
                fwrite(fpJPEGBuffer, sizeof(BYTE), fBufferSize, pFile);
            if (pFile)
                fclose(pFile);
#endif

            fprintf(stderr, "m_ptJetlibStream->HP_CurrentBufferLen = %ld \n", m_ptJetlibStream->HP_CurrentBufferLen);
            fprintf(stderr, "after HP_RawUByteArray \n");
            write(fdFax, m_ptJetlibStream->HP_OutBuffer, m_ptJetlibStream->HP_CurrentBufferLen);
            // write(fdFax, pDataArray1, dataLength);
            if (pDataArray != NULL)
            {
                free(pDataArray);
                pDataArray = NULL;
                pDataArray1 = NULL;
                pDataArrayMoving = NULL;
            }
            if (fpJPEGBuffer != NULL)
            {
                free(fpJPEGBuffer);
                fpJPEGBuffer = NULL;
            }
        }

        write(fdFax, JREndPageSeq, sizeof(JREndPageSeq));

    } /* end while (1) */

    //lseek(fdFax, 9, SEEK_SET);
    fprintf(stderr, "end of compress data \n");
BUGOUT:
    return status;
}

void clearStream()
{
    if (m_ptJetlibStream)
    {
        memset(m_ptJetlibStream->HP_OutBuffer, 0, m_ptJetlibStream->HP_OutBufferMaxSize);
        m_ptJetlibStream->HP_CurrentBufferLen = 0;
    }
}

int send_data_to_stdout(int fromFD)
{
    int iSize, i;
    int len;
    BYTE *pTmp = NULL;

    iSize = lseek(fromFD, 0, SEEK_END);
    lseek(fromFD, 0, SEEK_SET);

    // DBG("hpcupsfax: lseek(fromFD) returned %d", iSize);
    if (iSize > 0)
    {
        pTmp = (BYTE *)malloc(iSize);
    }
    if (pTmp == NULL)
    {
        iSize = 1024;
        pTmp = (BYTE *)malloc(iSize);
        if (pTmp == NULL)
        {
            return 1;
        }
    }

    while ((len = read(fromFD, pTmp, iSize)) > 0)
    {
        write(STDOUT_FILENO, pTmp, len);
    }
    free(pTmp);

    return 0;
}

int createTempFile(char *szFileName, FILE **pFilePtr)
{
    int iFD = -1;

    if (szFileName == NULL || szFileName[0] == '\0' || pFilePtr == NULL)
    {
        // BUG("Invalid Filename/ pointer\n");
        return 0;
    }

    if (strstr(szFileName, "XXXXXX") == NULL)
        strcat(szFileName, "_XXXXXX");

    iFD = mkstemp(szFileName);
    if (-1 == iFD)
    {
        // BUG("Failed to create the temp file Name[%s] errno[%d : %s]\n",szFileName,errno,strerror(errno));
        return 0;
    }
    else
    {
        *pFilePtr = fdopen(iFD, "w+");
    }

    return iFD;
}

int main(int argc, char **argv)
{
    int status = 0;
    int fd = 0;
    int fdFax = -1;
    int i = 0;
    FILE *pFilePtrFax;
    cups_raster_t *cups_raster;
    ppd_file_t *ppd;

    GetLogLevel();
    openlog("hpcupscdmfax", LOG_PID, LOG_DAEMON);

    if (argc < 6 || argc > 7)
    {
        BUG("ERROR: %s job-id user title copies options [file]\n", *argv);
        return 1;
    }

    if (argc == 7)
    {
        if ((fd = open(argv[6], O_RDONLY)) == -1)
        {
            BUG("ERROR: Unable to open raster file %s\n", argv[6]);
            return 1;
        }
    }

    while (argv[i] != NULL)
    {
        DBG("hpcupscdmfax: argv[%d] = %s\n", i, argv[i]);
        i++;
    }

    snprintf(hpFileName, sizeof(hpFileName), "%s/hp_%s_fax_Log_XXXXXX", CUPS_TMP_DIR, argv[2]);

    fdFax = createTempFile(hpFileName, &pFilePtrFax);
    if (fdFax < 0)
    {
        BUG("ERROR: Unable to open Fax output file - %s for writing\n", hpFileName);
        return 1;
    }
    else
    {
        chmod(hpFileName, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    }
    cups_raster = cupsRasterOpen(fd, CUPS_RASTER_READ);
    if (cups_raster == NULL)
    {
        // dbglog("cupsRasterOpen failed, fd = %d\n", fd);
        if (fd != 0)
        {
            close(fd);
        }
        // closeFilter();
        return 1;
    }
    char szEnumField[10] = "", szCountField[10] = "", *szStringField = NULL, szField[128] = "";
    m_ulCookie = 0;
    m_ptJetlibStream = HP_NewStream(MAX_BUFF_SIZE, m_ulCookie);
    //this part will be taken care in cdmfax.py
    /*
    clearStream();
    HP_BeginSession_4(m_ptJetlibStream, 200, 200, HP_eInch, HP_eBackChAndErrPage, HP_eSendToFax);
    write(fdFax, m_ptJetlibStream->HP_OutBuffer, (int)m_ptJetlibStream->HP_CurrentBufferLen);
    fprintf(stderr, "m_ptJetlibStream->HP_CurrentBufferLen = %ld \n", m_ptJetlibStream->HP_CurrentBufferLen);

    clearStream();
    HP_OpenDataSource_1(m_ptJetlibStream, HP_eDefaultDataSource, HP_eBinaryLowByteFirst);
    write(fdFax, m_ptJetlibStream->HP_OutBuffer, (int)m_ptJetlibStream->HP_CurrentBufferLen);
    fprintf(stderr, "m_ptJetlibStream->HP_CurrentBufferLen = %ld \n", m_ptJetlibStream->HP_CurrentBufferLen);
    */
    status = ProcessRasterData(cups_raster, fdFax);
    //this part will be taken care in cdmfax.py
    /*
    write(fdFax, JRCloseDataSourceSeq, sizeof(JRCloseDataSourceSeq));
    write(fdFax, JREndSessionSeq, sizeof(JREndSessionSeq));
    */
    //write(fdFax,PJLExit,sizeof(PJLExit));

    cupsRasterClose(cups_raster);

    // DBG("hpcupscdmfax: Send data to stdout \n");
    status = send_data_to_stdout(fdFax);

    if (fd != 0)
    {
        close(fd);
    }

    if (fdFax > 0)
    {
        close(fdFax);
        unlink(hpFileName);
    }

    return status;
}
