/*###########################################################
*
*  Copyright (c) HP Company, 2006.
*  All rights reserved.  Copying or other reproduction of this
*  program except for archival purposes is prohibited without
*  the prior written consent of HP Company.
*
*  File: Print2/GE/src/mpression.cpp
*
*  HP COMPANY
*  11311 Chinden Boulevard
*  Boise, Idaho  83714
*
*##########################################################*/

#include <jpeglib.h>

int read_JPEG_file (char * filename);

// #define OLDWAY
#ifdef OLDWAY
typedef enum
{
  deviceRGB,
  adobeRGB,
  grayScale,
} colorSpaceDisposition;
#endif


extern void write_JPEG_Buff (ubyte * outBuff, int quality, int image_width, int image_height, JSAMPLE *imageBuffer, int resolution, colorSpaceDisposition, int *numCompBytes);
//extern void write_JPEG_file (char * filename, int quality, int image_width, int image_height, JSAMPLE *imageBuffer, int resolution, colorSpaceEnum destCS);


extern int image_width;
extern int image_height;
extern int image_numComponents;
extern JSAMPLE * image_buffer; /* Points to large array of R,G,B-order data */
extern unsigned char *myImageBuffer;
extern int LZWEncodeFile(char *inBuff, int inBuffSize, char *outFile) ;
