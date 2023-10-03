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
