#ifndef PCLM_COMMON_DEFINES
#define PCLM_COMMON_DEFINES

#define PCLM_Ver 0.93
#define STANDALONE

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

typedef unsigned char   ubyte;          /* unsigned byte: 0..255           */
typedef signed   char   sbyte;          /* signed byte: -128..127          */
typedef unsigned char   uint8;          /* unsigned byte: 0..255           */
typedef unsigned short  uint16;         /* unsigned integer: 0..65535      */
typedef signed short    sint16;         /* signed integer: -32768..32767   */
typedef unsigned int    uint32;         /* unsigned long integer: 0..2^32-1*/
typedef signed int      sint32;         /* signed long integer: -2^31..2^31*/
typedef float           float32;        /* 32 bit floating point           */
typedef double          float64;        /* 64 bit floating point           */

typedef enum 
{
  RGB,
  AdobeRGB,
  GRAY, 
  unknown
} colorSpaceEnum;

typedef enum
{
  jpeg,
  zlib,
  rle
} compTypeEnum;
#endif
