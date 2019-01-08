/*###########################################################
*
*  Copyright (c) HP Company, 2006.
*  All rights reserved.  Copying or other reproduction of this
*  program except for archival purposes is prohibited without
*  the prior written consent of HP Company.
*
*  File: genPCLm.c
*
*  HP COMPANY
*  11311 Chinden Boulevard
*  Boise, Idaho  83714
*
*##########################################################*/

/**********************************************************************************************
* File: genPCLm.c
*
* Author(s): Steve Claiborne
*
* To Do:								Status
*   - Error generation
*   - Backside duplex flip/mirror capability
*   - Opportunity to scale input image to device resolution
*   - Support Adobe RGB color space
*   - Skip white strips
*
*====  COMPLETED TASKS ===========================================================
*   - Generate output file name to reflect media and input params       12/20/2010
*   - Support 300 device resolution                                     12/17/2010
*   - Support 1200 device resolution                                    12/17/2010
*   - Media size support                                                12/17/2010
*   - Other compression technologies: delta, taos                       11/17/2010
*     - zlib 								11/17/2010
*     - RLE (Hi)					                12/13/2010
*   - Margin support                                                    N/A?
*   - Strip height programmability                                      11/18/2010
*   - Multiple pages                                                    11/23/2010
*   - Source image scaling 						11/09/2010
*   - Debug option							11/09/2010
*   - Add comment job ticket                                            12/02/2010
*   - Added grayscale                                                   12/20/2010
*   - Scaled source width to full-width of media                        12/17/2010
*   - Implemented PCLmGen OO Interface					02/10/2011
*   - AdobeRGB                                                          02/01/2011
*   - Support odd-page duplex for InkJet                                02/01/2011
*   - JPEG markers to reflect resolution                                02/16/2011
*   - JPEG markers for strip height are stuck at 16                     02/16/2011
*   - KidsArray, xRefTable, KidsString, pOutBuffer are static sized     02/23/2011
*   - Rewrite the logic for handling the **bufPtr                       02/24/2011
*   - Support short-edge duplex                                         03/04/2011
*   - Need to implement error handling correctly    			03/04/2011
*   - Fixed adobeRGB multi-job issue                                    03/08/2011
*   - Color convert crash fix                                           03/08/2011
*   - Added abilty to pass debug setting to genPCLm			03/08/2011
*   - Added top-margin capabilities to shift image right and down	04/12/2011
*   - Add ability to use PNG as input                                   04/01/2011
**********************************************************************************
*   - eliminate the flate_colorspace.bin file
*   - Change compression types intra-page
*   - Assert that CS does not change       
*   - Change CS type on page boundaries
*   - Implement the media*Offset functionality
*   - Leftover lines in a strip are not supported
*   - Need to implement debug -> logfile
*
*====  Log of issues / defects  ==================================================
* 0.54: programmable strip height                                       11/23/2010
* 0.53: run-time crash of large source images                           11/17/2010
*       switched to getopt for parsing                                  11/17/2010
* 0.55: Add multi-page support						11/23/2010
* 0.56: Fixing /Length issue & removed leading comment                  11/29/2010
* 0.57: Fixing scaling and image position issues                        12/01/2010
* 0.58: Fixing white space at bottom of page                            12/01/2010
* 0.58: Fixing floating point error by switching to dev coordinates     12/02/2010
* 0.59: Added comment job-ticket                                        12/03/2010
* 0.60: Fixed xref issues that caused performance degradation           12/08/2010
*       Added support for -h 0 (generates 1 strip)                      12/08/2010
*       Added JPEG compression into JFIF header                         12/08/2010
* 0.63  Fixed media-padding issue for mediaHeight			12/20/2010
*       Fixed media-padding issue for non-600 resolutions		12/20/2010
* 0.65  Added ability to inject blank page for duplex			02/02/2011
*
* Known Issues:
*   - Can't convert large images to PDF					Fixed 11/18(0.53)
*   - 1200 dpi images are rendered to PDF at 600 dpi                    Fixed 12/17(0.61)
*
**********************************************************************************************/

/**********************************************************************************************
* JPEG image pointers:
*   - myImageBuffer: allocated source image buffer
*   - destBuffer:    current buffer pointer for put_scanline_someplace
*
**********************************************************************************************/

/**********************************************************************************************
* zlib parameters
* compress2 (dest, destLen, source, sourceLen)
*  Compresses the source buffer into the destination buffer.  sourceLen is the byte
*  length of the source buffer. Upon entry, destLen is the total size of the
*  destination buffer, which must be at least 0.1% larger than sourceLen plus
*  12 bytes. Upon exit, destLen is the actual size of the compressed buffer.

*  compress returns Z_OK if success, Z_MEM_ERROR if there was not enough
*  memory, Z_BUF_ERROR if there was not enough room in the output buffer,
*  Z_STREAM_ERROR if the level parameter is invalid.
*    #define Z_OK            0
*    #define Z_STREAM_END    1
*    #define Z_NEED_DICT     2
*    #define Z_ERRNO        (-1)
*    #define Z_STREAM_ERROR (-2)
*    #define Z_DATA_ERROR   (-3)
*    #define Z_MEM_ERROR    (-4)
*    #define Z_BUF_ERROR    (-5)
*    #define Z_VERSION_ERROR (-6)
*
**********************************************************************************************/
#define STAND_ALLONE

#include "PCLmGenerator.h"
#include "CommonDefinitions.h"
#include "./../../common/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <math.h>
#include <zlib.h>
//#include <unistd.h>

extern "C"
{
#include "jpeglib.h"
}
#include "RunLenEncoding.h"

#include "common_defines.h"
#include "genPCLm.h"
#include "ctype.h"

#include "flate_colorspace.h"
/*
Defines
*/

int debugIt = 0;

#define STRIP_HEIGHT 16
#define JPEG_QUALITY 100

#define XREF_SIZE 10000
#define TEMP_BUFF_SIZE 100000
#define DEFAULT_OUTBUFF_SIZE 64*5120*3

#define STANDARD_SCALE_FOR_PDF 72.0

#define KID_STRING_SIZE 1000

#define CATALOG_OBJ_NUMBER 1
#define PAGES_OBJ_NUMBER   2

/*
Local Variables
*/

/*
Defines
*/
#define rgb_2_gray(r,g,b) (ubyte)(0.299*(double)r+0.587*(double)g+0.114*(double)b)

// Note: this is required for debugging
boolean writeOutputFile(int numBytes, ubyte *ptr, char *user_name);

/* 
********************************************* Helper Routines **************************
*/

/*
Function: shiftStripByLeftMargin
Purpose:  To shift the strip image right in the strip buffer by leftMargin pixels
Assumptions: The strip buffer was allocated large enough to handle the shift; if not
then the image data on the right will get clipped.
Details:
We allocate a full strip (height and width), but then only copy numLinesThisCall from the original buffer
to the newly allocated buffer.  This pads the strips for JPEG processing.
*/
ubyte *shiftStripByLeftMargin(ubyte *ptrToStrip,sint32 currSourceWidth,sint32 currStripHeight, sint32 numLinesThisCall,
							  sint32 currMediaWidth, sint32 leftMargin, colorSpaceDisposition destColorSpace)
{
	ubyte *fromPtr=ptrToStrip, *toPtr, *newStrip;
	sint32 scanLineWidth;

	assert(currSourceWidth+(2*leftMargin)<=currMediaWidth);

	// writeOutputFile(currSourceWidth*3*numLinesThisCall, ptrToStrip,"");

	if(destColorSpace==grayScale)
	{
		scanLineWidth=currMediaWidth;
		// Allocate a full strip
		newStrip=(ubyte*)malloc(scanLineWidth*currStripHeight);
		memset(newStrip,0xff,scanLineWidth*currStripHeight);

		for(int i=0;i<numLinesThisCall;i++)
		{
			toPtr=newStrip+leftMargin+(i*currMediaWidth);
			fromPtr=ptrToStrip+(i*currSourceWidth);
			memcpy(toPtr,fromPtr,currSourceWidth);
		}

	}
	else
	{
		scanLineWidth=currMediaWidth*3;
		sint32 srcScanlineWidth=currSourceWidth*3;
		sint32 shiftAmount=leftMargin*3;
		newStrip=(ubyte*)malloc(scanLineWidth*currStripHeight);
		memset(newStrip,0xff,scanLineWidth*currStripHeight);
		for(int i=0;i<numLinesThisCall;i++)
		{
			toPtr=newStrip+shiftAmount+(i*scanLineWidth);
			fromPtr=ptrToStrip+(i*srcScanlineWidth);
			memcpy(toPtr,fromPtr,srcScanlineWidth);
			// memset(toPtr,0xe0,srcScanlineWidth);
		}
	}

	return(newStrip);
}

#ifdef SUPPORT_WHITE_STRIPS
bool PCLmGenerator::isWhiteStrip(void *pInBuffer, int inBufferSize)
{
	uint32 *ptr=(uint32*)pInBuffer;
	for(int i=0;i<inBufferSize/4;i++,ptr++)
	{
		if(*ptr!=0xffffffff)
			return(false);
	}
	return(true);
}
#endif

/*Called in errorOutAndCleanUp and Destructor*/
void PCLmGenerator::Cleanup(void)
{
	if(allocatedOutputBuffer)
	{
		free(allocatedOutputBuffer);
		allocatedOutputBuffer = NULL;
		currOutBuffSize = 0;
	}

	if(leftoverScanlineBuffer)
	{
		free(leftoverScanlineBuffer);
		leftoverScanlineBuffer=NULL;
	}
	if(scratchBuffer)
	{
		free(scratchBuffer);
		scratchBuffer = NULL;
	}
	if(xRefTable)
	{
		free(xRefTable);
		xRefTable=NULL;
	}
	if(KidsArray)
	{
		free(KidsArray);
		KidsArray=NULL;
	}
}

int PCLmGenerator::errorOutAndCleanUp()
{
	Cleanup();
	jobOpen=job_errored;
	return(genericFailure);
}

bool PCLmGenerator::addXRef(sint32 xRefObj)
{
#define XREF_ARRAY_SIZE 100
	sint32      *tmpxRefTable = NULL;
	if(!xRefTable)
	{
		xRefTable=(sint32*)malloc(XREF_ARRAY_SIZE*sizeof(sint32)); 
		assert(xRefTable);
		if(NULL == xRefTable)
		{
			return false;
		}
		xRefTable[0]=0;
		xRefIndex++;
	}

	xRefTable[xRefIndex]=xRefObj;
	xRefIndex++;

	if(!(xRefIndex%XREF_ARRAY_SIZE))
	{
		tmpxRefTable=(sint32*)realloc(xRefTable,(((xRefIndex+XREF_ARRAY_SIZE)*sizeof(sint32))));
		if( NULL != tmpxRefTable)
		{
			xRefTable = tmpxRefTable;
		}
		/* if(DebugIt)
		printf("Realloc xRef: 0x%lx\n",(unsigned long int)xRefTable);*/
	}
	return(true);
}


bool PCLmGenerator::addKids(sint32 kidObj)
{
#define KID_ARRAY_SIZE 20
	sint32      *tmpKidsArray = NULL;
	if(!KidsArray)
	{
		KidsArray=(sint32*)malloc(KID_ARRAY_SIZE*sizeof(sint32)); 
		assert(KidsArray);
		if(NULL == KidsArray)
		{
			return false;
		}
	}

	KidsArray[numKids]=kidObj;
	numKids++;

	if(!(numKids%KID_ARRAY_SIZE))
	{
		tmpKidsArray=(sint32*)realloc(KidsArray,((numKids+KID_ARRAY_SIZE)*sizeof(sint32)));
		if( NULL != tmpKidsArray)
		{
			KidsArray = tmpKidsArray;
		}
	}
	return(true);
}

boolean writeOutputFile(int numBytes, ubyte *ptr, char *user_name)
{
	FILE *outputFile;
	char outFileName[MAX_FILE_PATH_LEN];
	static int fileCntr=0;

	if(1)
		return TRUE;
	snprintf(outFileName,sizeof(outFileName),"%s/hp_%s_cups_outfile_%04d",CUPS_TMP_DIR, user_name, fileCntr);
 
	fileCntr++;

	// Open output PDF file
	if (!(outputFile = fopen (outFileName, "w")))
	{
		dbglog ("Could not open the output file out.\n");
		exit (-1);
		//return(true);
	}
	fwrite(ptr,numBytes,1,outputFile);
	fclose(outputFile); 
	return(TRUE);
}

void PCLmGenerator::initOutBuff(char *buff, sint32 size)
{
	currBuffPtr=outBuffPtr=buff;
	outBuffSize=size;
	totalBytesWrittenToCurrBuff=0;
	memset(buff,0,size);
}


void PCLmGenerator::writeStr2OutBuff(char *str)
{
	sint32 strSize=strlen(str);
	// Make sure we have enough room for the copy
	char *maxSize=currBuffPtr+strSize;
	assert(maxSize-outBuffPtr < outBuffSize);
	memcpy(currBuffPtr,str,strSize);
	currBuffPtr+=strSize;
	totalBytesWrittenToCurrBuff+=strSize;
	totalBytesWrittenToPCLmFile+=strSize;
}

void PCLmGenerator::write2Buff(ubyte *buff, int buffSize)
{
	char *maxSize=currBuffPtr+buffSize;
	assert(maxSize-outBuffPtr < outBuffSize);
	memcpy(currBuffPtr,buff,buffSize);
	currBuffPtr+=buffSize;
	totalBytesWrittenToCurrBuff+=buffSize;
	totalBytesWrittenToPCLmFile+=buffSize;
}

int PCLmGenerator::statOutputFileSize()
{
	addXRef(totalBytesWrittenToPCLmFile);
	return(1);
}

void PCLmGenerator::writePDFGrammarTrailer(int imageWidth, int imageHeight)
{
	int i;
	char KidsString[KID_STRING_SIZE];
	if(DebugIt2)
	{
		dbglog("imageWidth=%d\n",imageWidth);
		dbglog("imageHeight=%d\n",imageHeight);
	}

	snprintf(pOutStr,OUT_STR_SIZE,"%%============= PCLm: FileBody: Object 1 - Catalog\n"); writeStr2OutBuff(pOutStr);
	statOutputFileSize();
	snprintf(pOutStr,OUT_STR_SIZE,"%d 0 obj\n", CATALOG_OBJ_NUMBER); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"<<\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"/Type /Catalog\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"/Pages %d 0 R\n",PAGES_OBJ_NUMBER); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,">>\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"endobj\n"); writeStr2OutBuff(pOutStr);

	snprintf(pOutStr,OUT_STR_SIZE,"%%============= PCLm: FileBody: Object 2 - page tree \n"); writeStr2OutBuff(pOutStr);
	statOutputFileSize();
	snprintf(pOutStr,OUT_STR_SIZE,"%d 0 obj\n", PAGES_OBJ_NUMBER); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"<<\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"/Count %d\n",numKids); writeStr2OutBuff(pOutStr);

	// Define the Kids for this document as an indirect array
	snprintf(KidsString,KID_STRING_SIZE,"/Kids [ ");writeStr2OutBuff(KidsString);
	for(i=0;i<numKids && KidsArray;i++)
	{
		//spot=strlen(KidsString);
		snprintf(KidsString,KID_STRING_SIZE,"%d 0 R ",KidsArray[i]);
		writeStr2OutBuff(KidsString);
	}

	snprintf(KidsString,KID_STRING_SIZE,"]\n");
	writeStr2OutBuff(KidsString);


	snprintf(pOutStr,OUT_STR_SIZE,"/Type /Pages\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,">>\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"endobj\n"); writeStr2OutBuff(pOutStr);

	snprintf(pOutStr,OUT_STR_SIZE,"%%============= PCLm: cross-reference section: object 0, 6 entries\n"); writeStr2OutBuff(pOutStr);
	statOutputFileSize();
	xRefStart=xRefIndex-1;
	infoObj=xRefIndex;

	snprintf(pOutStr,OUT_STR_SIZE,"xref\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"0 %d\n",1); writeStr2OutBuff(pOutStr);

	snprintf(pOutStr,OUT_STR_SIZE,"0000000000 65535 f\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"%d %d\n",PAGES_OBJ_NUMBER+1,xRefIndex-4); writeStr2OutBuff(pOutStr);
	for(i=1;i<xRefIndex-3 && xRefTable;i++)
	{
		snprintf(pOutStr,OUT_STR_SIZE,"%010d %05d n\n",xRefTable[i],0); writeStr2OutBuff(pOutStr);
	}

#ifdef PIECEINFO_SUPPORTED
	// HP PieceInfo Structure
	snprintf(pOutStr,OUT_STR_SIZE,"9996 0 obj\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"<</HPDefine1 1\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"/Private 9997 0 R>>\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"9997 0 obj\n"); writeStr2OutBuff(pOutStr);
#endif
	//snprintf(pOutStr,OUT_STR_SIZE,"<</AIMetaData 32 0 R/AIPDFPrivateData1 33 0 R/AIPDFPrivateData10 34 0\n");

	// Now add the catalog and page object
	snprintf(pOutStr,OUT_STR_SIZE,"%d 2\n",CATALOG_OBJ_NUMBER); writeStr2OutBuff(pOutStr);
	if(xRefTable)
	{
		snprintf(pOutStr,OUT_STR_SIZE,"%010d %05d n\n",xRefTable[xRefIndex-3],0); writeStr2OutBuff(pOutStr);
		snprintf(pOutStr,OUT_STR_SIZE,"%010d %05d n\n",xRefTable[xRefIndex-2],0); writeStr2OutBuff(pOutStr);
	}

	snprintf(pOutStr,OUT_STR_SIZE,"%%============= PCLm: File Trailer\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"trailer\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"<<\n"); writeStr2OutBuff(pOutStr);
	// snprintf(pOutStr,OUT_STR_SIZE,"/Info %d 0\n", infoObj); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"/Size %d\n", xRefIndex-1); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"/Root %d 0 R\n",CATALOG_OBJ_NUMBER); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,">>\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"startxref\n"); writeStr2OutBuff(pOutStr);
	if(xRefTable)
	{
		snprintf(pOutStr,OUT_STR_SIZE,"%d\n",xRefTable[xRefStart]); writeStr2OutBuff(pOutStr);
	}
	snprintf(pOutStr,OUT_STR_SIZE,"%%%%EOF\n"); writeStr2OutBuff(pOutStr);
}

bool PCLmGenerator::injectAdobeRGBCS()
{
	if(adobeRGBCS_firstTime)
	{
		// We need to inject the ICC object for AdobeRGB
		snprintf(pOutStr,OUT_STR_SIZE,"%%============= PCLm: ICC Profile\n"); writeStr2OutBuff(pOutStr);
		statOutputFileSize();
		snprintf(pOutStr,OUT_STR_SIZE,"%d 0 obj\n", objCounter); objCounter++; writeStr2OutBuff(pOutStr);
		snprintf(pOutStr,OUT_STR_SIZE,"[/ICCBased %d 0 R]\n",objCounter); writeStr2OutBuff(pOutStr);

		snprintf(pOutStr,OUT_STR_SIZE,"endobj\n"); writeStr2OutBuff(pOutStr);
		statOutputFileSize();
		snprintf(pOutStr,OUT_STR_SIZE,"%d 0 obj\n", objCounter); objCounter++; writeStr2OutBuff(pOutStr);
		snprintf(pOutStr,OUT_STR_SIZE,"<<\n"); writeStr2OutBuff(pOutStr);
		snprintf(pOutStr,OUT_STR_SIZE,"/N 3\n"); writeStr2OutBuff(pOutStr);
		snprintf(pOutStr,OUT_STR_SIZE,"/Alternate /DeviceRGB\n"); writeStr2OutBuff(pOutStr);
		snprintf(pOutStr,OUT_STR_SIZE,"/Length %u\n",ADOBE_RGB_SIZE+1); writeStr2OutBuff(pOutStr);
		snprintf(pOutStr,OUT_STR_SIZE,"/Filter /FlateDecode\n"); writeStr2OutBuff(pOutStr);
		snprintf(pOutStr,OUT_STR_SIZE,">>\n"); writeStr2OutBuff(pOutStr);
		snprintf(pOutStr,OUT_STR_SIZE,"stream\n"); writeStr2OutBuff(pOutStr);

		write2Buff(flateBuffer,ADOBE_RGB_SIZE);

		snprintf(pOutStr,OUT_STR_SIZE,"\nendstream\n"); writeStr2OutBuff(pOutStr);
		snprintf(pOutStr,OUT_STR_SIZE,"endobj\n"); writeStr2OutBuff(pOutStr);
	}

	adobeRGBCS_firstTime=false;
	return(true);
}

/****************************************************************************************
* Function: colorConvertSource
* Purpose: to convert an image from one color space to another.
* Limitations:
*   - Currently, only supports RGB->GRAY
*****************************************************************************************/
bool PCLmGenerator::colorConvertSource(colorSpaceDisposition srcCS, colorSpaceDisposition dstCS, ubyte *strip, sint32 stripWidth, sint32 stripHeight)
{
	if(srcCS==deviceRGB && dstCS==grayScale)
	{
		// Do an inplace conversion from RGB -> 8 bpp gray
		ubyte *srcPtr=strip;
		ubyte *dstPtr=strip;
		for(int h=0;h<stripHeight;h++)
		{
			for(int w=0;w<stripWidth;w++,dstPtr++,srcPtr+=3)
			{
				//*dstPtr=(ubyte)((0.299*((double)r))+(0.587*((double)g)+0.114)*((double)b));
				*dstPtr=(ubyte)rgb_2_gray(*srcPtr,*(srcPtr+1),*(srcPtr+2));
			}
		}
		dstNumComponents=1;
		// sourceColorSpace=grayScale; // We don't want to change this, as we are only changing the
		// current strip.
	}
	else
		assert(1);

	//writeOutputFile(stripWidth*stripHeight, strip, m_pPCLmSSettings->user_name);
	return(true);
}

int PCLmGenerator::injectRLEStrip(ubyte *RLEBuffer, int numBytes, int imageWidth, int imageHeight, colorSpaceDisposition destColorSpace, bool whiteStrip)
{
	//unsigned char c;
	int strLength;
	char str[512];
	char fileStr[25];
	static int stripCount=0;

	if(DebugIt2)
	{
		dbglog("Injecting RLE compression stream into PDF\n");
		dbglog("  numBytes=%d, imageWidth=%d, imageHeight=%d\n",numBytes,imageWidth,imageHeight);
		
		snprintf(fileStr,25,"rleFile.%d",stripCount);
		stripCount++;
		write2Buff(RLEBuffer,numBytes);
	}

	if(destColorSpace==adobeRGB)
	{ 
		injectAdobeRGBCS();
	}

	// Inject LZ compressed image into PDF file
	snprintf(pOutStr,OUT_STR_SIZE,"%%============= PCLm: FileBody: Strip Stream: RLE Image \n"); writeStr2OutBuff(pOutStr);
	statOutputFileSize();
	snprintf(pOutStr,OUT_STR_SIZE,"%d 0 obj\n", objCounter); objCounter++; writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"<<\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"/Width %d\n", imageWidth); writeStr2OutBuff(pOutStr);
	if(destColorSpace==deviceRGB)
	{
		snprintf(pOutStr,OUT_STR_SIZE,"/ColorSpace /DeviceRGB\n"); writeStr2OutBuff(pOutStr);
	}
	else if(destColorSpace==adobeRGB)
	{
		snprintf(pOutStr,OUT_STR_SIZE,"/ColorSpace 5 0 R\n"); writeStr2OutBuff(pOutStr);
	}
	else if(destColorSpace==blackonly)
	{
		snprintf(pOutStr,OUT_STR_SIZE,"/ColorSpace /DeviceRGB\n"); writeStr2OutBuff(pOutStr);
	}
	else
	{
		snprintf(pOutStr,OUT_STR_SIZE,"/ColorSpace /DeviceGray\n"); writeStr2OutBuff(pOutStr);
	}
	snprintf(pOutStr,OUT_STR_SIZE,"/Height %d\n", imageHeight); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"/Filter /RunLengthDecode\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"/Subtype /Image\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"/Length %d\n",numBytes); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"/Type /XObject\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"/BitsPerComponent 8\n"); writeStr2OutBuff(pOutStr);
#ifdef SUPPORT_WHITE_STRIPS
	if(whiteStrip)
	{
		snprintf(pOutStr,OUT_STR_SIZE,"/Name /WhiteStrip\n"); writeStr2OutBuff(pOutStr);
	}
	else
	{
		snprintf(pOutStr,OUT_STR_SIZE,"/Name /ColorStrip\n"); writeStr2OutBuff(pOutStr);
	}
#endif

	snprintf(pOutStr,OUT_STR_SIZE,">>\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"stream\n"); writeStr2OutBuff(pOutStr);

	// Write the zlib compressed strip to the PDF output file
	write2Buff(RLEBuffer,numBytes);
	snprintf(pOutStr,OUT_STR_SIZE,"\nendstream\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"endobj\n"); writeStr2OutBuff(pOutStr);

	// Output image transformation information 
	snprintf(pOutStr,OUT_STR_SIZE,"%%============= PCLm: Object - Image Transformation \n"); writeStr2OutBuff(pOutStr);
	statOutputFileSize();
	snprintf(pOutStr,OUT_STR_SIZE,"%d 0 obj\n", objCounter); objCounter++; writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"<<\n"); writeStr2OutBuff(pOutStr);

	snprintf(str,512,"q /image Do Q\n");
	strLength=strlen(str);

	snprintf(pOutStr,OUT_STR_SIZE,"/Length %d\n",strLength); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,">>\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"stream\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"%s",str); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"endstream\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"endobj\n"); writeStr2OutBuff(pOutStr);

	return(1);
}

int PCLmGenerator::injectLZStrip(ubyte *LZBuffer, int numBytes, int imageWidth, int imageHeight, colorSpaceDisposition destColorSpace, bool whiteStrip)
{
	//unsigned char c;
	int strLength;
	char str[512];

	if(DebugIt2)
	{
		dbglog("Injecting LZ compression stream into PDF\n");
		dbglog("  numBytes=%d, imageWidth=%d, imageHeight=%d\n",numBytes,imageWidth,imageHeight);
	}

	if(destColorSpace==adobeRGB)
	{ 
		injectAdobeRGBCS();
	}

	// Inject LZ compressed image into PDF file
	snprintf(pOutStr,OUT_STR_SIZE,"%%============= PCLm: FileBody: Strip Stream: zlib Image \n"); writeStr2OutBuff(pOutStr);
	statOutputFileSize();
	snprintf(pOutStr,OUT_STR_SIZE,"%d 0 obj\n", objCounter); objCounter++; writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"<<\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"/Width %d\n", imageWidth); writeStr2OutBuff(pOutStr);
	if(destColorSpace==deviceRGB)
	{
		snprintf(pOutStr,OUT_STR_SIZE,"/ColorSpace /DeviceRGB\n"); writeStr2OutBuff(pOutStr);
	}
	else if(destColorSpace==adobeRGB)
	{
		snprintf(pOutStr,OUT_STR_SIZE,"/ColorSpace 5 0 R\n"); writeStr2OutBuff(pOutStr);
	}
	else if(destColorSpace==blackonly)
	{
		snprintf(pOutStr,OUT_STR_SIZE,"/ColorSpace /DeviceRGB\n"); writeStr2OutBuff(pOutStr);
	}
	else
	{
		snprintf(pOutStr,OUT_STR_SIZE,"/ColorSpace /DeviceGray\n"); writeStr2OutBuff(pOutStr);
	}
	snprintf(pOutStr,OUT_STR_SIZE,"/Height %d\n", imageHeight); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"/Filter /FlateDecode\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"/Subtype /Image\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"/Length %d\n",numBytes); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"/Type /XObject\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"/BitsPerComponent 8\n"); writeStr2OutBuff(pOutStr);
#ifdef SUPPORT_WHITE_STRIPS
	if(whiteStrip)
	{
		snprintf(pOutStr,OUT_STR_SIZE,"/Name /WhiteStrip\n"); writeStr2OutBuff(pOutStr);
	}
	else
	{
		snprintf(pOutStr,OUT_STR_SIZE,"/Name /ColorStrip\n"); writeStr2OutBuff(pOutStr);
	}
#endif

	snprintf(pOutStr,OUT_STR_SIZE,">>\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"stream\n"); writeStr2OutBuff(pOutStr);

	// Write the zlib compressed strip to the PDF output file
	write2Buff(LZBuffer,numBytes);
	snprintf(pOutStr,OUT_STR_SIZE,"\nendstream\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"endobj\n"); writeStr2OutBuff(pOutStr);

	// Output image transformation information 
	snprintf(pOutStr,OUT_STR_SIZE,"%%============= PCLm: Object - Image Transformation \n"); writeStr2OutBuff(pOutStr);
	statOutputFileSize();
	snprintf(pOutStr,OUT_STR_SIZE,"%d 0 obj\n", objCounter); objCounter++; writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"<<\n"); writeStr2OutBuff(pOutStr);

	snprintf(str,512,"q /image Do Q\n");
	strLength=strlen(str);

	snprintf(pOutStr,OUT_STR_SIZE,"/Length %d\n",strLength); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,">>\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"stream\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"%s",str); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"endstream\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"endobj\n"); writeStr2OutBuff(pOutStr);

	return(1);
}

int PCLmGenerator::injectJPEG(char *jpeg_Buff, int imageWidth, int imageHeight, int numCompBytes, colorSpaceDisposition destColorSpace, bool whiteStrip)
{
	// int fd, bytesRead;
	int strLength;
	char str[512];

	if(DebugIt2)
	{
		dbglog("Injecting jpegBuff into PDF\n");
	}

	yPosition+=imageHeight;

	if(destColorSpace==adobeRGB)
	{
		injectAdobeRGBCS();
	}

	// Inject PDF JPEG into output file
	snprintf(pOutStr,OUT_STR_SIZE,"%%============= PCLm: FileBody: Strip Stream: jpeg Image \n"); writeStr2OutBuff(pOutStr);
	statOutputFileSize();
	snprintf(pOutStr,OUT_STR_SIZE,"%d 0 obj\n", objCounter); objCounter++; writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"<<\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"/Width %d\n", imageWidth); writeStr2OutBuff(pOutStr);
	if(destColorSpace==deviceRGB)
	{
		snprintf(pOutStr,OUT_STR_SIZE,"/ColorSpace /DeviceRGB\n"); writeStr2OutBuff(pOutStr);
	}
	else if(destColorSpace==adobeRGB)
	{
		snprintf(pOutStr,OUT_STR_SIZE,"/ColorSpace 5 0 R\n"); writeStr2OutBuff(pOutStr);
	}
	else if(destColorSpace==blackonly)
	{
		snprintf(pOutStr,OUT_STR_SIZE,"/ColorSpace /DeviceRGB\n"); writeStr2OutBuff(pOutStr);
	}
	else
	{
		snprintf(pOutStr,OUT_STR_SIZE,"/ColorSpace /DeviceGray\n"); writeStr2OutBuff(pOutStr);
	}
	snprintf(pOutStr,OUT_STR_SIZE,"/Height %d\n", imageHeight); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"/Filter /DCTDecode\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"/Subtype /Image\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"/Length %d\n",numCompBytes); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"/Type /XObject\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"/BitsPerComponent 8\n"); writeStr2OutBuff(pOutStr);
#ifdef SUPPORT_WHITE_STRIPS
	if(whiteStrip)
	{
		snprintf(pOutStr,OUT_STR_SIZE,"/Name /WhiteStrip\n"); writeStr2OutBuff(pOutStr);
	}
	else
	{
		snprintf(pOutStr,OUT_STR_SIZE,"/Name /ColorStrip\n"); writeStr2OutBuff(pOutStr);
	}
#endif
	snprintf(pOutStr,OUT_STR_SIZE,">>\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"stream\n"); writeStr2OutBuff(pOutStr);

	// Inject JPEG image into stream
	//fd = open(jpeg_filename, O_RDWR);
	//if(fd==-1)
	//{
	//  fprintf (stderr, "Could not open the image file %s.\n", jpeg_filename);
	//  exit(-1);
	//}

	//inPtr=(unsigned char *)malloc(fileSize);
	//bytesRead=read(fd, inPtr, fileSize);
	//write(stdout, inPtr, bytesRead);
	write2Buff((ubyte*)jpeg_Buff,numCompBytes);
	snprintf(pOutStr,OUT_STR_SIZE,"\nendstream\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"endobj\n"); writeStr2OutBuff(pOutStr);

	snprintf(str,512,"q /image Do Q\n");
	strLength=strlen(str);

	// Output image transformation information 
	snprintf(pOutStr,OUT_STR_SIZE,"%%============= PCLm: Object - Image Transformation \n"); writeStr2OutBuff(pOutStr);
	statOutputFileSize();
	snprintf(pOutStr,OUT_STR_SIZE,"%d 0 obj\n", objCounter); objCounter++; writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"<<\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"/Length %d\n",strLength); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,">>\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"stream\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"%s",str); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"endstream\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"endobj\n"); writeStr2OutBuff(pOutStr);

	return(1);
}


void writeStr2Buff(char *buffer,char *str)
{
	int buffSize;
	char *buffPos;

	buffSize=strlen(buffer)+strlen(str);
	if(buffSize>TEMP_BUFF_SIZE)
		assert(0);

	buffSize=strlen(buffer);
	buffPos=buffer+buffSize;
	snprintf(buffPos,TEMP_BUFF_SIZE - buffSize,"%s",str);

	buffSize=strlen(buffer);
	if(buffSize>TEMP_BUFF_SIZE)
	{
		dbglog("tempBuff size exceeded: buffSize=%d\n",buffSize);
		assert(0);
	}
}


/**********************************************************************************************
* Function:       writePDFGrammarPage
* Purpose:        to generate the PDF page construct(s), which includes the image information.
* Implementation: the /Length definition is required for PDF, so we write the stream to a RAM buffer
*      first, then calculate the Buffer size, insert the PDF /Length construct, then write the
*      buffer to the PDF file.  I used the RAM buffer instead of a temporary file, as a driver
*      implementation won't be able to use a disk file.
***********************************************************************************************/

void PCLmGenerator::writePDFGrammarPage(int imageWidth, int imageHeight, int numStrips, colorSpaceDisposition destColorSpace)
{
	int i, imageRef=objCounter+2, buffSize;
	int yAnchor;
	char str[512];
	char *tempBuffer = NULL;
	int startImageIndex=0;
	int numLinesLeft = 0;

	if(destColorSpace==adobeRGB && 1 == pageCount)
	{
		imageRef+=2; // Add 2 for AdobeRGB
	}

	tempBuffer=(char *)malloc(TEMP_BUFF_SIZE);
	assert(tempBuffer);

	if (NULL == tempBuffer)/*Fix for Coverity CID: 63764*/
	{
		return;
	}
	if(DebugIt2)
		dbglog("Allocated %d bytes for tempBuffer\n",TEMP_BUFF_SIZE);
	memset(tempBuffer,0x0,TEMP_BUFF_SIZE);

	snprintf(pOutStr,OUT_STR_SIZE,"%%============= PCLm: FileBody: Object 3 - page object\n"); writeStr2OutBuff(pOutStr);
	statOutputFileSize();
	snprintf(pOutStr,OUT_STR_SIZE,"%d 0 obj\n", objCounter); writeStr2OutBuff(pOutStr);
	addKids(objCounter);
	objCounter++;
	snprintf(pOutStr,OUT_STR_SIZE,"<<\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"/Type /Page\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"/Parent %d 0 R\n",PAGES_OBJ_NUMBER); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"/Resources <<\n"); writeStr2OutBuff(pOutStr);
	// snprintf(pOutStr,OUT_STR_SIZE,"/ProcSet [ /PDF /ImageC ]\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"/XObject <<\n"); writeStr2OutBuff(pOutStr);

	if(topMarginInPix)
	{
		for(i=0;i<numFullInjectedStrips;i++,startImageIndex++)
		{
			snprintf(str,512,"/Image%d %d 0 R\n",startImageIndex,imageRef);
			snprintf(pOutStr,OUT_STR_SIZE,"%s",str); writeStr2OutBuff(pOutStr);
			imageRef+=2;
		}
		if(numPartialScanlinesToInject)
		{
			snprintf(str,512,"/Image%d %d 0 R\n",startImageIndex,imageRef);
			snprintf(pOutStr,OUT_STR_SIZE,"%s",str); writeStr2OutBuff(pOutStr);
			imageRef+=2;
			startImageIndex++;
		}
	}

	for(i=startImageIndex;i<numStrips+startImageIndex;i++)
	{
		snprintf(str,512,"/Image%d %d 0 R\n",i,imageRef);
		// snprintf(pOutStr,OUT_STR_SIZE,"/ImageA 4 0 R /ImageB 6 0 R >>\n"); writeStr2OutBuff(pOutStr);
		snprintf(pOutStr,OUT_STR_SIZE,"%s",str); writeStr2OutBuff(pOutStr);
		imageRef+=2;
	}
	snprintf(pOutStr,OUT_STR_SIZE,">>\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,">>\n"); writeStr2OutBuff(pOutStr);
	if(currMediaOrientationDisposition==landscapeOrientation)
	{
		pageOrigin=mediaWidth;
		snprintf(pOutStr,OUT_STR_SIZE,"/MediaBox [ 0 0 %d %d ]\n", mediaHeight, mediaWidth); writeStr2OutBuff(pOutStr);
	}
	else
	{
		pageOrigin=mediaHeight;
		snprintf(pOutStr,OUT_STR_SIZE,"/MediaBox [ 0 0 %d %d ]\n", mediaWidth, mediaHeight); writeStr2OutBuff(pOutStr);
	}
	snprintf(pOutStr,OUT_STR_SIZE,"/Contents [ %d 0 R ]\n",objCounter); writeStr2OutBuff(pOutStr);
#ifdef PIECEINFO_SUPPORTED
	snprintf(pOutStr,OUT_STR_SIZE,"/PieceInfo <</HPAddition %d 0 R >> \n",9997); writeStr2OutBuff(pOutStr);
#endif
	snprintf(pOutStr,OUT_STR_SIZE,">>\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"endobj\n"); writeStr2OutBuff(pOutStr);

	// Create the FileBody stream first, so we know the Length of the stream
	if(reverseOrder)
	{
		yAnchor=0;
	}
	else
	{
		yAnchor=(int)((pageOrigin*STANDARD_SCALE)+0.99); // Round up
	}

	// Setup the CTM so that we can send device-resolution coordinates
	snprintf(pOutStr,OUT_STR_SIZE,"%%Image Transformation Matrix: width, skewX, skewY, height, xAnchor, yAnchor\n"); writeStr2OutBuff(pOutStr);
	snprintf(str,512,"%f 0 0 %f 0 0 cm\n",STANDARD_SCALE_FOR_PDF/currRenderResolutionInteger,STANDARD_SCALE_FOR_PDF/currRenderResolutionInteger);
	writeStr2Buff(tempBuffer,str);

	startImageIndex=0;
	if(topMarginInPix)
	{
		for(i=0;i<numFullInjectedStrips;i++)
		{
			if(reverseOrder)
				yAnchor+=numFullScanlinesToInject;
			else
				yAnchor-=numFullScanlinesToInject;

			snprintf(str,512,"/P <</MCID 0>> BDC q\n");
			writeStr2Buff(tempBuffer,str);

			snprintf(str,512,"%%Image Transformation Matrix: width, skewX, skewY, height, xAnchor, yAnchor\n");
			writeStr2Buff(tempBuffer,str);

			snprintf(str,512,"%d 0 0 %d 0 %d cm\n",imageWidth*scaleFactor,numFullScanlinesToInject*scaleFactor,yAnchor*scaleFactor);
			writeStr2Buff(tempBuffer,str);

			snprintf(str,512,"/Image%d Do Q\n",startImageIndex);
			writeStr2Buff(tempBuffer,str);

			startImageIndex++; 
		}
		if(numPartialScanlinesToInject)
		{
			if(reverseOrder)
				yAnchor+=numPartialScanlinesToInject;
			else
				yAnchor-=numPartialScanlinesToInject;

			snprintf(str,512,"/P <</MCID 0>> BDC q\n");
			writeStr2Buff(tempBuffer,str);

			snprintf(str,512,"%%Image Transformation Matrix: width, skewX, skewY, height, xAnchor, yAnchor\n");
			writeStr2Buff(tempBuffer,str);

			snprintf(str,512,"%d 0 0 %d 0 %d cm\n",imageWidth*scaleFactor,numPartialScanlinesToInject*scaleFactor,yAnchor*scaleFactor);
			writeStr2Buff(tempBuffer,str);

			snprintf(str,512,"/Image%d Do Q\n",startImageIndex);
			writeStr2Buff(tempBuffer,str);

			startImageIndex++; 
		}
	}

	for(i=startImageIndex;i<numStrips+startImageIndex;i++)
	{
		//last strip may have less lines than currStripHeight. So update yAnchor using left over lines
		if(i == (numStrips+startImageIndex-1))
		{
			numLinesLeft = currSourceHeight - ((numStrips-1) * currStripHeight);

			if(reverseOrder)
				yAnchor+=numLinesLeft;
			else
				yAnchor-=numLinesLeft;
		}
		else
		{
			if(reverseOrder)
				yAnchor+=currStripHeight;
			else
				yAnchor-=currStripHeight;
		}

		snprintf(str,512,"/P <</MCID 0>> BDC q\n");
		writeStr2Buff(tempBuffer,str);

		snprintf(str,512,"%%Image Transformation Matrix: width, skewX, skewY, height, xAnchor, yAnchor\n");
		writeStr2Buff(tempBuffer,str);

		if(i == (numStrips+startImageIndex-1)) //last strip may have less lines than currStripHeight
		{
			snprintf(str,512,"%d 0 0 %d 0 %d cm\n",imageWidth*scaleFactor,numLinesLeft*scaleFactor,yAnchor*scaleFactor);
			writeStr2Buff(tempBuffer,str);
		}
		else if(yAnchor<0)
		{
			sint32 newH=currStripHeight+yAnchor;
			snprintf(str,512,"%d 0 0 %d 0 %d cm\n",imageWidth*scaleFactor,newH*scaleFactor,0*scaleFactor);
			writeStr2Buff(tempBuffer,str);
		}
		else
		{
			snprintf(str,512,"%d 0 0 %d 0 %d cm\n",imageWidth*scaleFactor,currStripHeight*scaleFactor,yAnchor*scaleFactor);
			writeStr2Buff(tempBuffer,str);
		}

		snprintf(str,512,"/Image%d Do Q\n",i);
		writeStr2Buff(tempBuffer,str);

	}

	// Resulting buffer size
	buffSize=strlen(tempBuffer);

	snprintf(pOutStr,OUT_STR_SIZE,"%%============= PCLm: FileBody: Page Content Stream object\n"); writeStr2OutBuff(pOutStr);
	statOutputFileSize();
	snprintf(pOutStr,OUT_STR_SIZE,"%d 0 obj\n",objCounter); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"<<\n"); writeStr2OutBuff(pOutStr);

	snprintf(pOutStr,OUT_STR_SIZE,"/Length %d\n",buffSize); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,">>\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"stream\n"); writeStr2OutBuff(pOutStr);

	// Now write the FileBody stream
	write2Buff((ubyte*)tempBuffer,buffSize);

	snprintf(pOutStr,OUT_STR_SIZE,"endstream\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"endobj\n"); writeStr2OutBuff(pOutStr);
	objCounter++;
	if(tempBuffer)
	{
		free(tempBuffer);
		tempBuffer=NULL;
	}
}


/****************************************************************************************
* Function: prepImageForBacksideDuplex
* Purpose:  To mirror the source image in preperation for backside duplex support
* Limitations:
*   - 
*****************************************************************************************/
boolean prepImageForBacksideDuplex(ubyte *imagePtr, sint32 imageHeight, sint32 imageWidth, sint32 numComponents)
{
	sint32 numBytes=imageHeight*imageWidth*numComponents;
	ubyte *head, *tail, t0, t1, t2; 

	if(numComponents==3)
	{
		for(head=imagePtr,tail=imagePtr+numBytes-1;tail>head;)
		{
			t0=*head;
			t1=*(head+1);
			t2=*(head+2);

			*head=    *(tail-2);
			*(head+1)=*(tail-1);
			*(head+2)=*(tail-0);
			*tail=    t2;
			*(tail-1)=t1;
			*(tail-2)=t0;

			head+=3;
			tail-=3;
		}
	}
	else
	{
		for(head=imagePtr,tail=imagePtr+numBytes;tail>head;)
		{
			t0=*head;
			*head=*tail;
			*tail=t0;
			head++;
			tail--;
		}
	}
	//origTail++;
	return(TRUE);
}

char* PCLmGenerator::getInputBinString(jobInputBin bin)
{
	memset(returnStr,0,sizeof(returnStr));
	switch (bin)
	{
	case alternate:      strncpy(returnStr,"alternate", sizeof(returnStr)); break;
	case alternate_roll: strncpy(returnStr,"alternate_roll", sizeof(returnStr)); break;
	case auto_select:    strncpy(returnStr,"auto_select", sizeof(returnStr)); break;
	case bottom:         strncpy(returnStr,"bottom", sizeof(returnStr)); break;
	case center:         strncpy(returnStr,"center", sizeof(returnStr)); break;
	case disc:           strncpy(returnStr,"disc", sizeof(returnStr)); break;
	case envelope:       strncpy(returnStr,"envelope", sizeof(returnStr)); break;
	case hagaki:         strncpy(returnStr,"hagaki", sizeof(returnStr)); break;
	case large_capacity: strncpy(returnStr,"large_capacity", sizeof(returnStr)); break;
	case left:           strncpy(returnStr,"left", sizeof(returnStr)); break;
	case main_tray:      strncpy(returnStr,"main_tray", sizeof(returnStr)); break;
	case main_roll:      strncpy(returnStr,"main_roll", sizeof(returnStr)); break;
	case manual:         strncpy(returnStr,"manual", sizeof(returnStr)); break;
	case middle:         strncpy(returnStr,"middle", sizeof(returnStr)); break;
	case photo:          strncpy(returnStr,"photo", sizeof(returnStr)); break;
	case rear:           strncpy(returnStr,"rear", sizeof(returnStr)); break;
	case right:          strncpy(returnStr,"right", sizeof(returnStr)); break;
	case side:           strncpy(returnStr,"side", sizeof(returnStr)); break;
	case top:            strncpy(returnStr,"top", sizeof(returnStr)); break;
	case tray_1:         strncpy(returnStr,"tray_1", sizeof(returnStr)); break;
	case tray_2:         strncpy(returnStr,"tray_2", sizeof(returnStr)); break;
	case tray_3:         strncpy(returnStr,"tray_3", sizeof(returnStr)); break;
	case tray_4:         strncpy(returnStr,"tray_4", sizeof(returnStr)); break;
	case tray_5:         strncpy(returnStr,"tray_5", sizeof(returnStr)); break;
	case tray_N:         strncpy(returnStr,"tray_N", sizeof(returnStr)); break;
	default:			 strncpy(returnStr,"auto_select", sizeof(returnStr));  break; /*Use Auto for wrong option*/
	}
	return(returnStr);
}

char* PCLmGenerator::getOutputBin(jobOutputBin bin)
{
	memset(returnStr,0,sizeof(returnStr));
	switch(bin)
	{
	case top_output:               strncpy(returnStr,"top_output", sizeof(returnStr)); break;
	case middle_output:            strncpy(returnStr,"middle_output", sizeof(returnStr)); break;
	case bottom_output:            strncpy(returnStr,"bottom_output", sizeof(returnStr)); break;
	case side_output:              strncpy(returnStr,"side_output", sizeof(returnStr)); break;
	case center_output:            strncpy(returnStr,"center_output", sizeof(returnStr)); break;
	case rear_output:              strncpy(returnStr,"rear_output", sizeof(returnStr)); break;
	case face_up:                  strncpy(returnStr,"face_up", sizeof(returnStr)); break;
	case face_down:                strncpy(returnStr,"face_down", sizeof(returnStr)); break;
	case large_capacity_output:    strncpy(returnStr,"large_capacity_output", sizeof(returnStr)); break;
	case stacker_N:                strncpy(returnStr,"stacker_N", sizeof(returnStr)); break;
	case mailbox_N:                strncpy(returnStr,"mailbox_N", sizeof(returnStr)); break;
	case tray_1_output:            strncpy(returnStr,"tray_1_output", sizeof(returnStr)); break;
	case tray_2_output:            strncpy(returnStr,"tray_2_output", sizeof(returnStr)); break;
	case tray_3_output:            strncpy(returnStr,"tray_3_output", sizeof(returnStr)); break;
	case tray_4_output:            strncpy(returnStr,"tray_4_output", sizeof(returnStr)); break;
	default:					   strncpy(returnStr,"top_output", sizeof(returnStr));  break;/*Use top for wrong option*/
	}
	return(returnStr);
}
char* PCLmGenerator::getOrientationString(mediaOrientationDisposition Orientation)
{
	memset(returnStr,0,sizeof(returnStr));
	switch (Orientation)
	{
	case portraitOrientation:
		strncpy(returnStr,"portrait",sizeof(returnStr));
		break;

	case landscapeOrientation:
		strncpy(returnStr,"landscape",sizeof(returnStr));
		break;
	
	default:
		assert(0);
		break;
	}
	return(returnStr);
}
char* PCLmGenerator::getDuplexString(duplexDispositionEnum duplex)
{
	memset(returnStr,0,sizeof(returnStr));
	switch (duplex)
	{
	case simplex:
		strncpy(returnStr,"one-sided",sizeof(returnStr));
		break;
	case duplex_shortEdge:
		strncpy(returnStr,"two-sided-short-edge",sizeof(returnStr));
		break;

	case duplex_longEdge:
		strncpy(returnStr,"two-sided-long-edge",sizeof(returnStr));
		break;

	default:
		assert(0);
		break;
	}
	return(returnStr);
}

char* PCLmGenerator::getColorThemesString(colorThemes colortheme)
{
    memset(returnStr,0,sizeof(returnStr));
    switch(colortheme)
    {
        case default_colortheme:  strncpy(returnStr,"auto",sizeof(returnStr));break;
        case vividsRGB:           strncpy(returnStr,"saturation",sizeof(returnStr));break;
        case photosRGB:           strncpy(returnStr,"perception",sizeof(returnStr));break;
        case photoAdobeRGB:       strncpy(returnStr,"perception",sizeof(returnStr));break;
        default:                  strncpy(returnStr,"none",sizeof(returnStr));break;
    }
    return (returnStr);
}


void PCLmGenerator::writeJobTicket()
{
	// Write JobTicket
	char inputBin[256];
	char outputBin[256];
	char orientation[256];
	char duplex[256];
	char colorthemes[256];
	strncpy(colorthemes,getColorThemesString(m_pPCLmSSettings->colorTheme),256);
	strncpy(inputBin,getInputBinString(m_pPCLmSSettings->userInputBin),256);
	strncpy(outputBin,getOutputBin(m_pPCLmSSettings->userOutputBin),256);
	strncpy(orientation,getOrientationString(m_pPCLmSSettings->userOrientation),256);
	strncpy(duplex,getDuplexString(currDuplexDisposition),256);

	snprintf(pOutStr,OUT_STR_SIZE,"%%  genPCLm (Ver: %f)\n",PCLM_Ver); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"%%============= Job Ticket =============\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"%%  PCLmS-Job-Ticket\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"%%      job-ticket-version: 0.1\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"%%      epcl-version: 1.01\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"%%    JobSection\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"%%      job-id: %d\n",m_pPCLmSSettings->jobid); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"%%      job-start-time: %s\n",m_pPCLmSSettings->datetime); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"%%    MediaHandlingSection\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"%%      media-size-name: %s\n",currMediaName); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"%%      media-type: %s\n",m_pPCLmSSettings->userMediaType); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"%%      media-source: %s\n",inputBin); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"%%      sides: %s\n",duplex); writeStr2OutBuff(pOutStr);
	//snprintf(pOutStr,OUT_STR_SIZE,"%%      finishings: xxx\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"%%      output-bin: %s\n",outputBin); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"%%    RenderingSection\n"); writeStr2OutBuff(pOutStr);
	if(currCompressionDisposition==compressDCT)
	{
		snprintf(pOutStr,OUT_STR_SIZE,"%%      pclm-compression-method: JPEG\n"); writeStr2OutBuff(pOutStr);
	}
	else if(currCompressionDisposition==compressFlate)
	{
		snprintf(pOutStr,OUT_STR_SIZE,"%%      pclm-compression-method: FLATE\n"); writeStr2OutBuff(pOutStr);
	}
	else
	{
		snprintf(pOutStr,OUT_STR_SIZE,"%%      pclm-compression-method: RLE\n"); writeStr2OutBuff(pOutStr);
	}
	snprintf(pOutStr,OUT_STR_SIZE,"%%      strip-height: %d\n",currStripHeight); writeStr2OutBuff(pOutStr);

	if(destColorSpace==deviceRGB)
	{
		snprintf(pOutStr,OUT_STR_SIZE,"%%      print-color-mode: color\n"); writeStr2OutBuff(pOutStr);
	}
	else if(destColorSpace==adobeRGB)
	{
		snprintf(pOutStr,OUT_STR_SIZE,"%%      print-color-mode: color\n"); writeStr2OutBuff(pOutStr);
	}
	else if(destColorSpace==grayScale)
	{
		snprintf(pOutStr,OUT_STR_SIZE,"%%      print-color-mode: monochrome\n"); writeStr2OutBuff(pOutStr);
	}
	else if(destColorSpace==blackonly)
	{
		snprintf(pOutStr,OUT_STR_SIZE,"%%      print-color-mode: color\n"); writeStr2OutBuff(pOutStr);
	}
	snprintf(pOutStr,OUT_STR_SIZE,"%%      print-rendering-intent: %s\n",colorthemes); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"%%      print-quality: %s\n",m_pPCLmSSettings->userPageQuality); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"%%      printer-resolution: %d\n",currRenderResolutionInteger); writeStr2OutBuff(pOutStr);
	//snprintf(pOutStr,OUT_STR_SIZE,"%%      print-content-optimized: xxx\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"%%      orientation-requested: %d\n",m_pPCLmSSettings->userOrientation); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"%%      copies: %d\n",m_pPCLmSSettings->userCopies); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"%%      pclm-raster-back-side: xxx\n"); writeStr2OutBuff(pOutStr);
	if(currRenderResolutionInteger)
	{
		snprintf(pOutStr,OUT_STR_SIZE,"%%      margins-pre-applied: TRUE\n"); writeStr2OutBuff(pOutStr);
	}
	else
	{
		snprintf(pOutStr,OUT_STR_SIZE,"%%      margins-pre-applied: FALSE\n"); writeStr2OutBuff(pOutStr);
	}
	snprintf(pOutStr,OUT_STR_SIZE,"%%  PCLmS-Job-Ticket-End\n"); writeStr2OutBuff(pOutStr);
}

void PCLmGenerator::writePDFGrammarHeader()
{
	// snprintf(pOutStr,OUT_STR_SIZE,"%%============= PCLm: File Header \n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"%%PDF-1.7\n"); writeStr2OutBuff(pOutStr);
	snprintf(pOutStr,OUT_STR_SIZE,"%%PCLm 1.0\n"); writeStr2OutBuff(pOutStr);
}

/* PCLmGenerator Constructor
* 
* /desc
* /param  
* /return 
*/
PCLmGenerator::PCLmGenerator()
{
	strncpy(currMediaName,"na_letter_8.5x11in",256);
	currDuplexDisposition=simplex;
	currColorSpaceDisposition=deviceRGB;
	currDebugDisposition=debugOff;
	currCompressionDisposition=compressDCT;
	currMediaOrientationDisposition=portraitOrientation;
	currRenderResolution=res600;
	currStripHeight=STRIP_HEIGHT;

	// Default media h/w to letter specification
	mediaWidthInPixels=0;
	mediaHeightInPixels=0;
	mediaWidth=612;
	mediaHeight=792;
	destColorSpace=deviceRGB;
	sourceColorSpace=deviceRGB;
	scaleFactor=1;
	genExtraPage=false;
	jobOpen=job_closed;
	currSourceWidth = 5100;
	currSourceHeight = 6600;
	srcNumComponents = 3;
	dstNumComponents = 3;
	numLeftoverScanlines = 0;
	scratchBuffer=NULL;
	pageCount=0;
	reverseOrder = false;
	outBuffSize = 0;
	currOutBuffSize = 0;
	
	currRenderResolutionInteger=600;
	currResolution = 600;
	STANDARD_SCALE=(float)currRenderResolutionInteger/(float)STANDARD_SCALE_FOR_PDF;
	yPosition=0;
	infoObj=0;
	pageOrigin = 0;
	numKids=0;
	// XRefTable storage 
	xRefIndex=0;
	xRefStart = 0;

	DebugIt=0;
	DebugIt2=0;

	objCounter=PAGES_OBJ_NUMBER+1;
	totalBytesWrittenToPCLmFile=0;
	totalBytesWrittenToCurrBuff = 0;

	// Initialize first index in xRefTable
	xRefTable=NULL;
	KidsArray=NULL;

	// Initialize the output Buffer
	allocatedOutputBuffer=NULL;

	// Initialize the leftover scanline logic
	leftoverScanlineBuffer=NULL;

	adobeRGBCS_firstTime=true;
	mirrorBackside=false;

	topMarginInPix=0;
	leftMarginInPix=0;

	m_pPCLmSSettings = NULL;

	outBuffPtr = NULL;
	currBuffPtr = NULL;
	memset(pOutStr,0,OUT_STR_SIZE);
	firstStrip = true;
	numFullInjectedStrips = 0;
	numFullScanlinesToInject = 0;
	numPartialScanlinesToInject = 0;
	memset(returnStr,0,sizeof(returnStr));
}

/* PCLmGenerator Destructor
* 
* /desc
* /param  
* /return 
*/
PCLmGenerator::~PCLmGenerator()
{
	Cleanup();
}

/* StartJob
* 
* /desc
* /param  
* /return 
*/
#ifdef STANDALONE
int  PCLmGenerator::StartJob(void **pOutBuffer, int *iOutBufferSize, bool debug)
#else
int  PCLmGenerator::StartJob(void **pOutBuffer, int *iOutBufferSize)
#endif
{
	DebugIt=debug;
	DebugIt2=debug;

	if(DebugIt)
		dbglog("genPCLm::StartJob\n");

	// Allocate the output buffer; we don't know much at this point, so make the output buffer size
	// the worst case dimensions; when we get a startPage, we will resize it appropriately
	outBuffSize=DEFAULT_OUTBUFF_SIZE;
	*iOutBufferSize=outBuffSize;
	*pOutBuffer=(ubyte*)malloc(outBuffSize*10);

	if(NULL == *pOutBuffer)
	{
		return(errorOutAndCleanUp());
	}

	currOutBuffSize=outBuffSize*10;
	if(DebugIt2)
	    	dbglog("Allocated %d for myOutBufferSize\n",outBuffSize);
	allocatedOutputBuffer=*pOutBuffer;
	initOutBuff((char*)*pOutBuffer,outBuffSize);
	writePDFGrammarHeader();
	*iOutBufferSize=totalBytesWrittenToCurrBuff;
	jobOpen=job_open;

	return(success);  
}

/* EndJob
* 
* /desc
* /param  
* /return 
*/
int  PCLmGenerator::EndJob(void **pOutBuffer, int *iOutBufferSize)
{
	//int result; 

	if(DebugIt)
    	dbglog("genPCLm::EndJob\n");
    if(NULL==allocatedOutputBuffer)
    {
      return(errorOutAndCleanUp());
    }
	*pOutBuffer = allocatedOutputBuffer;

	initOutBuff((char*)*pOutBuffer,outBuffSize);

	// Write PDF trailer
	writePDFGrammarTrailer(currSourceWidth, currSourceHeight);

	/*if(!DebugIt && (currCompressionDisposition==compressDCT))
	{
		result=remove("jpeg_chunk.jpg");
	}*/

	*iOutBufferSize=totalBytesWrittenToCurrBuff;

	jobOpen=job_closed;

	if(xRefTable)
	{
		free(xRefTable);
		xRefTable=NULL;
	}
	if(KidsArray)
	{
		free(KidsArray);
		KidsArray=NULL;
	}

	return(success);  
}

/* PCLmSStartPage
* 
* /desc
* /param  
* /return 
*/
int  PCLmGenerator::StartPage(PCLmSSetup *PCLmSPageContent, bool generatePCLmS, void **pOutBuffer, int *iOutBufferSize)
{
	m_pPCLmSSettings=PCLmSPageContent->PCLmSUserSettings;
	return(StartPage(PCLmSPageContent->PCLmPageContent,pOutBuffer,iOutBufferSize));
}

/* StartPage
* 
* /desc
* /param  
* /return 
*/
int  PCLmGenerator::StartPage(PCLmPageSetup *PCLmPageContent, void **pOutBuffer, int *iOutBufferSize)
{
	int numImageStrips;
	// Save the resolution information
	currRenderResolution=PCLmPageContent->destinationResolution;

	*pOutBuffer = allocatedOutputBuffer;

	if(currRenderResolution==res300)
		currRenderResolutionInteger=300;
	else if(currRenderResolution==res600)
		currRenderResolutionInteger=600;
	else if(currRenderResolution==res1200)
		currRenderResolutionInteger=1200;
	else
		assert(0);

	// Recalculate STANDARD_SCALE to reflect the job resolution
	STANDARD_SCALE=(float)currRenderResolutionInteger/(float)STANDARD_SCALE_FOR_PDF;

	// Media and source sizes are in 72 DPI; convert media information to native resolutions: 
	//   Add 0.5 to force rounding
	currSourceWidth=(int)(PCLmPageContent->sourceWidth+0.50);
	currSourceHeight=(int)(PCLmPageContent->sourceHeight+0.50);

	// Save off the media information
    mediaWidth=(int)(PCLmPageContent->mediaWidth + 0.50);
    mediaHeight=(int)(PCLmPageContent->mediaHeight + 0.50);
	mediaWidthInPixels =(int)(((PCLmPageContent->mediaWidth/STANDARD_SCALE_FOR_PDF)*currRenderResolutionInteger)+0.50);
	mediaHeightInPixels=(int)(((PCLmPageContent->mediaHeight/STANDARD_SCALE_FOR_PDF)*currRenderResolutionInteger)+0.50);
	topMarginInPix=(int)(PCLmPageContent->mediaHeightOffset+0.50);
	leftMarginInPix=(int)(PCLmPageContent->mediaWidthOffset+0.50);
	currCompressionDisposition=PCLmPageContent->compTypeRequested;

	if(DebugIt)
	{
		dbglog("genPCLm::StartPage\n");
		dbglog("  mediaName=%s\n",   PCLmPageContent->mediaSizeName);
		dbglog("  clientLocale=%s\n",PCLmPageContent->mediaSizeName);
		dbglog("  mediaHeight=%f\n", PCLmPageContent->mediaHeight);
		dbglog("  mediaWidth=%f\n",  PCLmPageContent->mediaWidth);
		dbglog("  topMargin=%d\n",   topMarginInPix);
		dbglog("  leftMargin=%d\n",  leftMarginInPix);

		dbglog("  topLeftMargin=%f,%f\n",PCLmPageContent->mediaWidthOffset,PCLmPageContent->mediaHeightOffset);
		dbglog("  sourceHeight=%f\n",PCLmPageContent->sourceHeight);
		dbglog("  sourceWidth=%f\n", PCLmPageContent->sourceWidth);
		dbglog("  stripHeight=%d\n", PCLmPageContent->stripHeight);
		dbglog("  scaleFactor=%d\n", PCLmPageContent->scaleFactor);
		dbglog("  genExtraPage=%d\n",PCLmPageContent->genExtraPage);

		if(PCLmPageContent->colorContent==color_content){
			dbglog("  colorContent=color_content\n");
			}
        else if(PCLmPageContent->colorContent==gray_content){
            dbglog("  colorContent=gray_content\n");
            }
        else{
            dbglog("  colorContent=unknown_content\n");
            }

		if(PCLmPageContent->pageOrigin==top_left){
			dbglog("  pageOrigin=top_left\n");
			}
			else{
			dbglog("  pageOrigin=bottom_right\n");
			}
		if(PCLmPageContent->compTypeRequested==compressRLE){
			dbglog("compTypeRequested=RLE\n");
			}
		else if(PCLmPageContent->compTypeRequested==compressDCT){
			dbglog("compTypeRequested=DCT\n");
			}
		else if(PCLmPageContent->compTypeRequested==compressFlate){
			dbglog("compTypeRequested=Flate\n");
			}
		else if(PCLmPageContent->compTypeRequested==compressDefault){
			dbglog("compTypeRequested=Flate\n");
			}
		else if(PCLmPageContent->compTypeRequested==compressNone){
			dbglog("compTypeRequested=None\n");
			}

		if(PCLmPageContent->dstColorSpaceSpefication==deviceRGB){
			dbglog("colorSpaceSpefication=deviceRGB\n");
			}
		else if(PCLmPageContent->dstColorSpaceSpefication==adobeRGB){
			dbglog("colorSpaceSpefication=adobeRGB\n");
			}
		else if(PCLmPageContent->dstColorSpaceSpefication==grayScale){
			dbglog("colorSpaceSpefication=grayScale\n");
			}
        else{ //if blackonly
			dbglog("colorSpaceSpefication=blackonly\n");
			}
        

		if(PCLmPageContent->destinationResolution==res300){
			dbglog("destinationResolution Requested=300 DPI\n");
			}
		else if(PCLmPageContent->destinationResolution==res600){
			dbglog("destinationResolution Requested=600 DPI\n");
			}
		else if(PCLmPageContent->destinationResolution==res1200){
			dbglog("destinationResolution Requested=1200 DPI\n");
			}

		if(PCLmPageContent->duplexDisposition==simplex){
			dbglog("duplex disposition=Simplex\n");
			}
		else if(PCLmPageContent->duplexDisposition==duplex_longEdge){
			dbglog("duplex disposition=Duplex_longEdge\n");
			}
		else if(PCLmPageContent->duplexDisposition==duplex_shortEdge){
			dbglog("duplex disposition=Duplex_shortEdge\n");
			}


	}

	if(strlen(PCLmPageContent->mediaSizeName))
		strncpy(currMediaName,PCLmPageContent->mediaSizeName,256);

	currStripHeight=PCLmPageContent->stripHeight;
	if(!currStripHeight)
	{
		numImageStrips=1;
		currStripHeight=currSourceHeight;
	}
	else
	{
		float numImageStripsReal=ceil((float)currSourceHeight/(float)currStripHeight); // Need to know how many strips will be inserted into PDF file
		numImageStrips=(int)numImageStripsReal;
		//if(topMarginInPix) // We will inject an extra image for the topMargin offset
		//  numImageStrips++;
	}

	if(PCLmPageContent->srcColorSpaceSpefication==grayScale)
		srcNumComponents=1;
	else
		srcNumComponents=3;

	if(PCLmPageContent->dstColorSpaceSpefication==grayScale)
		dstNumComponents=1;
	else
		dstNumComponents=3;

	currDuplexDisposition=PCLmPageContent->duplexDisposition;

	destColorSpace=PCLmPageContent->dstColorSpaceSpefication;

	// Calculate how large the output buffer needs to be based upon the page specifications
	int tmp_outBuffSize=mediaWidthInPixels*currStripHeight*dstNumComponents;

	if(tmp_outBuffSize>currOutBuffSize)
	{
		*pOutBuffer=realloc(*pOutBuffer,tmp_outBuffSize);  // Realloc the pOutBuffer to the correct size

		if(*pOutBuffer==NULL)                              //realloc failed and prev buffer not freed
		{
			return errorOutAndCleanUp();
		}

		outBuffSize=currOutBuffSize=tmp_outBuffSize;
		allocatedOutputBuffer=*pOutBuffer;
		if(NULL == allocatedOutputBuffer)
		{
			return (errorOutAndCleanUp());
		}
		/*if(DebugIt2)
		printf("pOutBuffer: allocated %d bytes at 0x%lx\n",tmp_outBuffSize, (long int)*pOutBuffer);*/
	}

	initOutBuff((char*)*pOutBuffer,outBuffSize);

	if(DebugIt2)
        dbglog("Allocated %d for myOutBufferSize\n",outBuffSize);



	if(DebugIt)
        dbglog("numImageStrips=%d\n",numImageStrips);
	// Keep track of the page count
	pageCount++;

	// If we are on a backside and doing duplex, prep for reverse strip order
	if(currDuplexDisposition==duplex_longEdge && !(pageCount%2))
	{
		if(DebugIt2)
			dbglog("genPCLm.cpp: setting reverseOrder\n");
		reverseOrder=true;
	}
	else
		reverseOrder=false;

	// Calculate the number of injected strips, if any
	if(topMarginInPix)
	{
		if(topMarginInPix<=currStripHeight)
		{
			numFullInjectedStrips=1;
			numFullScanlinesToInject=topMarginInPix;
			numPartialScanlinesToInject=0;
		}
		else
		{
			numFullInjectedStrips=topMarginInPix/currStripHeight;
			numFullScanlinesToInject=currStripHeight;
			numPartialScanlinesToInject=topMarginInPix - (numFullInjectedStrips*currStripHeight);
		}
	}

	writeJobTicket();
	writePDFGrammarPage(mediaWidthInPixels, mediaHeightInPixels, numImageStrips, destColorSpace);
	*iOutBufferSize=totalBytesWrittenToCurrBuff;

	if(!scratchBuffer)
	{
		// We need to pad the scratchBuffer size to allow for compression expansion (RLE can create
		// compressed segments that are slightly larger than the source.
		scratchBuffer=(ubyte*)malloc(currStripHeight*mediaWidthInPixels*srcNumComponents*2);
		if(!scratchBuffer)
			return(errorOutAndCleanUp());
		/*if(DebugIt2)
		printf("scrachBuffer: Allocated %d bytes at 0x%lx\n",currStripHeight*currSourceWidth*srcNumComponents, (long int) scratchBuffer);*/
	}

	mirrorBackside=PCLmPageContent->mirrorBackside;
	firstStrip=true;

	return(success);  
}

/* EndPage
* 
* /desc
* /param  
* /return 
*/
int  PCLmGenerator::EndPage(void **pOutBuffer, int *iOutBufferSize)
{
	*pOutBuffer = allocatedOutputBuffer;
	initOutBuff((char*)*pOutBuffer,outBuffSize);
	*iOutBufferSize=totalBytesWrittenToCurrBuff;

	// Free up the scratchbuffer at endpage, to allow the next page to be declared with a different
	// size.
	if(scratchBuffer)
	{
		free(scratchBuffer);
		scratchBuffer=NULL;
	}

	return(success);  
}

/* SkipLines
* 
* /desc
* /param  
* /return 
*/
int  PCLmGenerator::SkipLines(int iSkipLines)
{
	return(success);  
}

/* Encapsulate
* 
* /desc
* /param  
* /return 
*/
int  PCLmGenerator::Encapsulate(void *pInBuffer, int inBufferSize, int thisHeight, void **pOutBuffer, int *iOutBufferSize)
{
	int result=0, numCompBytes;
	int scanlineWidth=mediaWidthInPixels*srcNumComponents;
	int compSize;
	// int numLinesThisCall=inBufferSize/(currSourceWidth*srcNumComponents);
	int numLinesThisCall=thisHeight;
	//void *savedInBufferPtr=NULL;
	//void *tmpBuffer=NULL;
	void *localInBuffer;
	ubyte *newStripPtr=NULL;

	/*Coverity CID: 63762: The two blocks in #if 0 here are related to buffering the scan lines if they are more 
	than strip height. The memory access is with mediaWidth whereas it should be currSourceWidth(19thApr11 mail) 
	plus coverity reported leaks. Since the number of lines passed from PCLmS.cpp will always be <= strip height, 
	disabling them and returning fail for that case. */

	// writeOutputFile(currSourceWidth*3*currStripHeight, (ubyte*)pInBuffer, m_pPCLmSSettings->user_name);
#if 0
	if(leftoverScanlineBuffer)
	{
		ubyte *whereAreWe;
		sint32 scanlinesThisTime;
		// The leftover scanlines have already been processed (color-converted and flipped), so just
		// put them into the output buffer.
		// Allocate a temporary buffer to copy leftover and new data into
		tmpBuffer=malloc(scanlineWidth*currStripHeight);
		if(!tmpBuffer)
			return(errorOutAndCleanUp());

		// Copy leftover scanlines into tmpBuffer
		memcpy(tmpBuffer,leftoverScanlineBuffer,scanlineWidth*numLeftoverScanlines);

		whereAreWe=(ubyte*)tmpBuffer+(scanlineWidth*numLeftoverScanlines);

		scanlinesThisTime=currStripHeight-numLeftoverScanlines;

		// Copy enough scanlines from the real inBuffer to fill out the tmpBuffer
		memcpy(whereAreWe,pInBuffer,scanlinesThisTime*scanlineWidth);

		// Now copy the remaining scanlines from pInBuffer to the leftoverBuffer
		if(DebugIt)
			dbglog("Leftover scanlines: numLinesThisCall=%d, currStripHeight=%d\n",numLinesThisCall,currStripHeight);
		numLeftoverScanlines=thisHeight-scanlinesThisTime;
		assert(leftoverScanlineBuffer);
		whereAreWe=(ubyte*)pInBuffer+(scanlineWidth*numLeftoverScanlines);
		memcpy(leftoverScanlineBuffer,whereAreWe,scanlineWidth*numLeftoverScanlines);
		numLinesThisCall=thisHeight=currStripHeight;

		savedInBufferPtr=pInBuffer;
		localInBuffer=tmpBuffer;
	}
	else
#endif
		localInBuffer=pInBuffer;


	//Not handling strip > strip height; see comment above
	if(thisHeight > currStripHeight)
	{
		return ((int)(genericFailure));
#if 0
		// Copy raw raster into leftoverScanlineBuffer
		ubyte *ptr;
		if(DebugIt)
			dbglog("Leftover scanlines: numLinesThisCall=%d, currStripHeight=%d\n",numLinesThisCall,currStripHeight);
		numLeftoverScanlines=thisHeight-currStripHeight;
		leftoverScanlineBuffer=malloc(scanlineWidth*numLeftoverScanlines);
		if(!leftoverScanlineBuffer)
			return(errorOutAndCleanUp());
		ptr=(ubyte *)localInBuffer+scanlineWidth*numLeftoverScanlines; 
		memcpy(leftoverScanlineBuffer,ptr,scanlineWidth*numLeftoverScanlines);
		thisHeight=currStripHeight;
#endif
	}

	if(NULL == allocatedOutputBuffer)
	{
		return (errorOutAndCleanUp());
	}
	*pOutBuffer = allocatedOutputBuffer;
	initOutBuff((char*)*pOutBuffer,outBuffSize);

	if(currDuplexDisposition==duplex_longEdge && !(pageCount%2))
	{
		if(mirrorBackside)
			prepImageForBacksideDuplex((ubyte*)localInBuffer, numLinesThisCall, currSourceWidth, srcNumComponents);
	}

	if((destColorSpace==grayScale) && (sourceColorSpace==deviceRGB || sourceColorSpace==adobeRGB))
	{
		colorConvertSource(sourceColorSpace, grayScale, (ubyte*)localInBuffer, currSourceWidth, numLinesThisCall);
		// Adjust the scanline width accordingly
		scanlineWidth = mediaWidthInPixels * dstNumComponents;
	}

	if(leftMarginInPix)
	{
		newStripPtr=shiftStripByLeftMargin((ubyte*)localInBuffer, currSourceWidth, currStripHeight, numLinesThisCall, mediaWidthInPixels, leftMarginInPix, destColorSpace);
#if 0
		if(sourceColorSpace==grayScale)
			writeOutputFile(currStripHeight*mediaWidthInPixels,(ubyte*)newStripPtr, m_pPCLmSSettings->user_name);
		else
			writeOutputFile(3*currStripHeight*mediaWidthInPixels,(ubyte*)newStripPtr, m_pPCLmSSettings->user_name);
#endif
	}

#ifdef SUPPORT_WHITE_STRIPS
	bool whiteStrip=isWhiteStrip(pInBuffer, thisHeight*currSourceWidth*srcNumComponents);
	if(DebugIt2)
	{
		if(whiteStrip){
			dbglog("Found white strip\n");
			}
		else{
			dbglog("Found non-white strip\n");
			}
	}
#else
	bool whiteStrip=false;
#endif

	if(currCompressionDisposition==compressDCT)
	{
		if(firstStrip && topMarginInPix)
		{
			ubyte whitePt=0xff;

			ubyte *tmpStrip=(ubyte*)malloc(scanlineWidth*topMarginInPix);
			memset(tmpStrip,whitePt,scanlineWidth*topMarginInPix);


			for(sint32 stripCntr=0; stripCntr<numFullInjectedStrips;stripCntr++)
			{
				write_JPEG_Buff (scratchBuffer, JPEG_QUALITY, mediaWidthInPixels, (sint32)numFullScanlinesToInject, (JSAMPLE*)tmpStrip, currRenderResolutionInteger, destColorSpace, &numCompBytes);
				injectJPEG((char*)scratchBuffer, mediaWidthInPixels, (sint32)numFullScanlinesToInject, numCompBytes, destColorSpace, true /*white*/ );
			}

			if(numPartialScanlinesToInject)
			{
				// Handle the leftover strip
				write_JPEG_Buff (scratchBuffer, JPEG_QUALITY, mediaWidthInPixels, numPartialScanlinesToInject, (JSAMPLE*)tmpStrip, currRenderResolutionInteger, destColorSpace, &numCompBytes);
				injectJPEG((char*)scratchBuffer, mediaWidthInPixels, numPartialScanlinesToInject, numCompBytes, destColorSpace, true /*white*/ );
			}

			free(tmpStrip);
			firstStrip=false;
		}

		/*The "if 1" block below pads the incoming buffer from numLinesThisCall to strip_height assuming that it has 
		that much extra space.
		Also "else" block after this "if 1" block was used before; when for JPEG also, there was no padding to strip_height.
		Retaining it just in case, in future, the padding has to be removed*/

#if 1
		// We are always going to compress the full strip height, even though the image may be less;
		// this allows the compressed images to be symetric
		if(numLinesThisCall<currStripHeight)
		{
			sint32 numLeftoverBytes=(currStripHeight-numLinesThisCall)*currSourceWidth*3;
			sint32 numImagedBytes  =numLinesThisCall*currSourceWidth*3;
			// End-of-page: we have to white-out the unused section of the source image
			memset((ubyte*)localInBuffer+numImagedBytes, 0xff, numLeftoverBytes);
		}

		if(newStripPtr)
		{
			write_JPEG_Buff (scratchBuffer, JPEG_QUALITY, mediaWidthInPixels, currStripHeight, (JSAMPLE*)newStripPtr, currRenderResolutionInteger, destColorSpace, &numCompBytes);
			free(newStripPtr);
			newStripPtr = NULL;
		}
		else
			write_JPEG_Buff (scratchBuffer, JPEG_QUALITY, mediaWidthInPixels, currStripHeight, (JSAMPLE*)localInBuffer, currRenderResolutionInteger, destColorSpace, &numCompBytes);

		if(DebugIt2)
			writeOutputFile(numCompBytes, scratchBuffer, m_pPCLmSSettings->user_name);
		injectJPEG((char*)scratchBuffer, mediaWidthInPixels, currStripHeight, numCompBytes, destColorSpace, whiteStrip );

#else
		if(newStripPtr)
		{
			write_JPEG_Buff (scratchBuffer, JPEG_QUALITY, mediaWidthInPixels, numLinesThisCall, (JSAMPLE*)newStripPtr, currRenderResolutionInteger, destColorSpace, &numCompBytes);
			free(newStripPtr);
			newStripPtr = NULL;
		}
		else
			write_JPEG_Buff (scratchBuffer, JPEG_QUALITY, mediaWidthInPixels, numLinesThisCall, (JSAMPLE*)localInBuffer, currRenderResolutionInteger, destColorSpace, &numCompBytes);
		if(DebugIt2)
			writeOutputFile(numCompBytes, scratchBuffer, m_pPCLmSSettings->user_name);
		injectJPEG((char*)scratchBuffer, mediaWidthInPixels, numLinesThisCall, numCompBytes, destColorSpace, whiteStrip );
#endif
	}
	else if(currCompressionDisposition==compressFlate)
	{
		uint32 len=numLinesThisCall*scanlineWidth;
		uLongf destSize=len;

		if(firstStrip && topMarginInPix)
		{
			ubyte whitePt=0xff;

			// We need to inject a blank image-strip with a height==topMarginInPix
			ubyte *tmpStrip=(ubyte*)malloc(scanlineWidth*topMarginInPix);
			uLongf tmpDestSize=destSize;
			memset(tmpStrip,whitePt,scanlineWidth*topMarginInPix);

			for(sint32 stripCntr=0; stripCntr<numFullInjectedStrips;stripCntr++)
			{
				result=compress((Bytef*)scratchBuffer,&tmpDestSize, (const Bytef*)tmpStrip, scanlineWidth*numFullScanlinesToInject);
				injectLZStrip((ubyte*)scratchBuffer, tmpDestSize, mediaWidthInPixels, numFullScanlinesToInject, destColorSpace, true /*white*/ );
			}
			if(numPartialScanlinesToInject)
			{
				result=compress((Bytef*)scratchBuffer,&tmpDestSize, (const Bytef*)tmpStrip, scanlineWidth*numPartialScanlinesToInject);
				injectLZStrip((ubyte*)scratchBuffer, tmpDestSize, mediaWidthInPixels, numPartialScanlinesToInject, destColorSpace, true /*white*/ );
			}
			free(tmpStrip);
			firstStrip=false;
		}

		if(newStripPtr)
		{
        result=compress((Bytef*)scratchBuffer,&destSize,(const Bytef*)newStripPtr,scanlineWidth*numLinesThisCall);
        if(DebugIt2)
          writeOutputFile(destSize, scratchBuffer, m_pPCLmSSettings->user_name);
        if(DebugIt2)
        {
          dbglog("Allocated zlib dest buffer of size %d\n",numLinesThisCall*scanlineWidth);
          dbglog("zlib compression return result=%d, compSize=%d\n",result,(int)destSize);
        }
			free(newStripPtr);
			newStripPtr = NULL;
		}
		else
      {
        result=compress((Bytef*)scratchBuffer, &destSize, (const Bytef*)localInBuffer, scanlineWidth*numLinesThisCall);
        if(DebugIt2)
          writeOutputFile(destSize, scratchBuffer, m_pPCLmSSettings->user_name);
        if(DebugIt2)
        {
          dbglog("Allocated zlib dest buffer of size %d\n",numLinesThisCall*scanlineWidth);
          dbglog("zlib compression return result=%d, compSize=%d\n",result,(int)destSize);
        }
		}
		injectLZStrip(scratchBuffer,destSize, mediaWidthInPixels, numLinesThisCall, destColorSpace, whiteStrip);
	}

	else if(currCompressionDisposition==compressRLE)
	{
		if(firstStrip && topMarginInPix)
		{
			ubyte whitePt=0xff;

			// We need to inject a blank image-strip with a height==topMarginInPix

			ubyte *tmpStrip=(ubyte*)malloc(scanlineWidth*topMarginInPix);
			memset(tmpStrip,whitePt,scanlineWidth*topMarginInPix);

			for(sint32 stripCntr=0; stripCntr<numFullInjectedStrips;stripCntr++)
			{
				compSize=HPRunLen_Encode((ubyte*)tmpStrip, scratchBuffer, scanlineWidth*numFullScanlinesToInject);
				injectRLEStrip((ubyte*)scratchBuffer, compSize, mediaWidthInPixels, numFullScanlinesToInject, destColorSpace, true /*white*/);
			}

			if(numPartialScanlinesToInject)
			{
				compSize=HPRunLen_Encode((ubyte*)tmpStrip, scratchBuffer, scanlineWidth*numPartialScanlinesToInject);
				injectRLEStrip((ubyte*)scratchBuffer, compSize, mediaWidthInPixels, numPartialScanlinesToInject, destColorSpace, true /*white*/);
			}

			free(tmpStrip);
			firstStrip=false;
		}

		if(newStripPtr)
		{
			compSize=HPRunLen_Encode((ubyte*)newStripPtr, scratchBuffer, scanlineWidth*numLinesThisCall);
			free(newStripPtr);
			newStripPtr = NULL;
		}
		else
			compSize=HPRunLen_Encode((ubyte*)localInBuffer, scratchBuffer, scanlineWidth*numLinesThisCall);

		if(DebugIt2)
		{
			dbglog("Allocated rle dest buffer of size %d\n",numLinesThisCall*scanlineWidth);
			dbglog("rle compression return size=%d=%d\n",result,(int)compSize);
		}
		injectRLEStrip(scratchBuffer, compSize, mediaWidthInPixels, numLinesThisCall, destColorSpace, whiteStrip);
	}
	else
		assert(0);

	*iOutBufferSize=totalBytesWrittenToCurrBuff;

	/*if(savedInBufferPtr)
		pInBuffer=savedInBufferPtr;

	if(tmpBuffer)
		free(tmpBuffer);*/

	if(newStripPtr)
	{
		free(newStripPtr);
		newStripPtr = NULL;
	}
	return(success);  
}

/* FreeBuffer
* 
* /desc
* /param  
* /return 
*/
void PCLmGenerator::FreeBuffer(void *pBuffer)
{
	if(jobOpen==job_closed && pBuffer)
	{

		if(pBuffer == allocatedOutputBuffer)
		{
			allocatedOutputBuffer = NULL;
		}
		free(pBuffer);
	}
	pBuffer=NULL;
}


