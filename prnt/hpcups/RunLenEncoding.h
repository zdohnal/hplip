/***************************************************************************
*            Header for Run Length Encoding and Decoding Library
*
*   File    : rle.h
*   Purpose : Provides prototypes for functions that use run length coding
*             to encode/decode files.
*
****************************************************************************/


#ifndef _RLE_H_
#define _RLE_H_

/* RLE encodeing */
int HPRunLen_Encode(ubyte *inFile, ubyte *outFile, int destSize);
/* RLE encodeing/decoding */
//int HPRunLen_Decode(ubyte *inFile, ubyte *outFile);


#endif  /* ndef _RLE_H_ */
