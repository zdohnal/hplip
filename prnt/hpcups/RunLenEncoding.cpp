#include "common_defines.h"
#include "RunLenEncoding.h"
#include <inttypes.h>
 
/*
This function encodes the 
*/

int HPRunLen_Encode(ubyte *in, ubyte *out, int inLength)
{
  // Overview
  //    compress input by identifying repeating bytes (not sequences)
  //    Compression ratio good for grayscale images, not great on RGB
  //    Output:
  //      1-127:   literal run
  //      128:     end of compression block
  //      129-256: repeating byte sequence
  //
  ubyte *imgPtr=in;
  ubyte *endPtr=in+inLength;
  ubyte *origOut=out;
  ubyte c;
  sint32 cnt=0;
  //unsigned __int64 lenLeft;
  uint64_t lenLeft;

  while((imgPtr+1)<endPtr)
  {
    lenLeft=endPtr-imgPtr;
    c=*imgPtr++;
    cnt=1;

    // Figure out how many repeating bytes are in the image
    while(cnt<lenLeft && *imgPtr==c)
    {
      if(imgPtr>endPtr)
        break;
      cnt++;
      imgPtr++;
    }

    // If cnt > 1, then output repeating byte specification
    //    The syntax is "byte-count repeateByte", where byte-count is 
    //    257-byte-count.
    //    Since the cnt value is a byte, if the repeateCnt is > 128
    //    then we need to put out multiple repeat-blocks
    //    Referred to as method 1, range is 128-256
    if(cnt>1)     
    {
      // Handle the repeat blocks of greater than 128 bytes
      while(cnt>128)
      {              // block of 128 repeating bytes
         *out++=129; // i.e. 257-129==128
         *out++=c;
         cnt-=128;
      }
      // Now handle the repeats that are < 128
      if(cnt)
      {
        *out++=(257-cnt); // i.e. cnt==2: 257-255=2
        *out++=c;
      }
    }
    // If cnt==1, then this is a literal run - no repeating bytes found.
    //    The syntax is "byte-count literal-run", where byte-count is < 128 and 
    //    literal-run is the non-repeating bytes of the input stream.
    //    Referred to as method 2, range is 0-127
    else   
    {
      ubyte *start, *p;
      sint32 i; 
      start=(imgPtr-1);  // The first byte of the literal run

      // Now find the end of the literal run
      for(cnt=1,p=start;(imgPtr<endPtr)  ;p++,imgPtr++,cnt++)
        if(*p==*imgPtr)
			break;
      if(!((imgPtr)==endPtr) && (cnt != 1))
        imgPtr--;    // imgPtr incremented 1 too many
      cnt--;
      // Blocks of literal bytes can't exceed 128 bytes, so output multiple
      //    literal-run blocks if > 128
      while(cnt>128)
      {
        *out++=127;
        for(i=0;i<128;i++)
          *out++=*start++;
        cnt-=128;
      }
      // Now output the leftover literal run 
      *out++=cnt-1;
      for(i=0;i<cnt;i++)
        *out++=*start++;
    }
  }
  // Now, write the end-of-compression marker (byte 128) into the output stream
  *out++=128;
  // Return the compressed size
  return((int)(out-origOut));
}

/*
This function decodes the 
*/

//int HPRunLen_Decode(ubyte *inFile, ubyte *outFile)
//{
//  return 1;
//}

