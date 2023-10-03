/*****************************************************************************\
  
   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston,
   MA 02111-1307, USA.

\
\*****************************************************************************/
#ifndef PCLM_COMMON_DEFINES
#define PCLM_COMMON_DEFINES

#define PCLM_Ver 0.93
#define STANDALONE

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
