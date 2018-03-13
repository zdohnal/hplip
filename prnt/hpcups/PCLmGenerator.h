/*
*******************************************************************************
*
* File:         PCLmGenerator.h
* Author:       Steve Claiborne
* Purpose:      Class/Object interface to genPCLm
* Status:
*
* (C) Copyright 2010, HP Company, all rights reserved.
*
*******************************************************************************
*/
#ifndef _PCLM_GENERATOR
#define _PCLM_GENERATOR
#define SUPPORT_WHITE_STRIPS

#include "common_defines.h"

const int OUT_STR_SIZE = 256;

typedef enum
{
  simplex,
  duplex_longEdge,
  duplex_shortEdge
} duplexDispositionEnum;

typedef enum
{
  job_open,
  job_closed,
  job_errored
} jobStateEnum;

typedef enum
{
  deviceRGB,
  adobeRGB,
  grayScale,
  blackonly
} colorSpaceDisposition;

typedef enum
{
	default_colortheme,
	vividsRGB,
	photosRGB,
	photoAdobeRGB,
	none
}colorThemes;

typedef enum
{
  debugOn,
  debugOff
} debugDisposition;

typedef enum
{
  compressRLE,
  compressDCT,
  compressFlate,
  compressDefault,
  compressNone
} compressionDisposition;

typedef enum
{
  portraitOrientation,
  landscapeOrientation
} mediaOrientationDisposition;

typedef enum
{
  res300,
  res600,
  res1200
} renderResolution;

typedef enum
{
  top_left,
  bottom_right
} pageOriginType;

typedef enum 
{
  color_content,
  gray_content,
  unknown_content
} pageCromaticContent;

typedef enum
{
  draft,
  normal,
  best,
} pageOutputQuality;

typedef enum
{
  alternate = 0,
  alternate_roll = 1,
  auto_select = 2,
  bottom = 3,
  center = 4 ,
  disc = 5,
  envelope =6 ,
  hagaki = 7,
  large_capacity = 8,
  left = 9,
  main_tray = 10,
  main_roll = 11,
  manual = 12,
  middle = 13,
  photo = 14,
  rear = 15,
  right = 16,
  side = 17,
  top = 18,
  tray_1 = 19,
  tray_2 =20,
  tray_3 = 21,
  tray_4 =22,
  tray_5 = 23,
  tray_N = 24,
} jobInputBin;

typedef enum
{
  top_output,
  middle_output,
  bottom_output,
  side_output,
  center_output,
  rear_output,
  face_up,
  face_down,
  large_capacity_output,
  stacker_N,
  mailbox_N,
  tray_1_output,
  tray_2_output,
  tray_3_output,
  tray_4_output,
} jobOutputBin;

typedef struct 
{
  char                        userPageQuality[32];
  mediaOrientationDisposition userOrientation;
  char                        userMediaType[256];
  jobInputBin                 userInputBin; 
  int                         userCopies;
  char                        userDocumentName[256];
  jobOutputBin                userOutputBin;
  colorThemes				  colorTheme;
  char						  datetime[256];
  int						  jobid;
  char                        user_name[32];
} PCLmSUserSettingsType;

typedef struct
{
  char mediaSizeName[256];
  //char mediaTypeName[256];
  char clientLocale [256];
  float mediaHeight;
  float mediaWidth;
  float sourceHeight;
  float sourceWidth;
  float mediaWidthOffset;
  float mediaHeightOffset;
  pageCromaticContent colorContent;          // Did the page contain any "real" color
  pageOriginType pageOrigin;
  compressionDisposition compTypeRequested;
  colorSpaceDisposition srcColorSpaceSpefication;
  colorSpaceDisposition dstColorSpaceSpefication;
  int stripHeight;
  renderResolution destinationResolution;
  
  duplexDispositionEnum duplexDisposition; // SJC
  int scaleFactor; // sjc
  bool genExtraPage; // SJC
  bool mirrorBackside;
} PCLmPageSetup;

typedef struct 
{
  PCLmPageSetup         *PCLmPageContent;
  PCLmSUserSettingsType *PCLmSUserSettings;
} PCLmSSetup;

typedef enum
{
  success=0,
  genericFailure=-1,
  callAgain=1
} PCLmGenerator_returnType;

class PCLmGenerator
{
public:

    PCLmGenerator();
    ~PCLmGenerator();

#ifdef STANDALONE
    int StartJob    (void **pOutBuffer, int *iOutBufferSize, bool debug);
#else
    int StartJob    (void **pOutBuffer, int *iOutBufferSize);
#endif
    int EndJob      (void **pOutBUffer, int *iOutBifferSize);
    int StartPage   (PCLmPageSetup *PCLmPageContent, void **pOutBuffer, int *iOutBufferSize);
    int StartPage   (PCLmSSetup *PCLmSPageContent, bool generatePCLmS, void **pOutBuffer, int *iOutBufferSize);
    int EndPage     (void **pOutBuffer, int *iOutBufferSize);
    int Encapsulate (void *pInBuffer, int inBufferSize, int numLines, void **pOutBuffer, int *iOutBufferSize);
    int SkipLines   (int iSkipLines);
    void FreeBuffer (void *pBuffer);

private:
    bool colorConvertSource(colorSpaceDisposition srcCS, colorSpaceDisposition dstCS, ubyte *strip, sint32 stripWidth, sint32 stripHeight);
    void writePDFGrammarPage(int imageWidth, int imageHeight, int numStrips, colorSpaceDisposition destColorSpace);
    void writePDFGrammarHeader();
    int injectRLEStrip(ubyte *RLEBuffer, int numBytes, int imageWidth, int imageHeight, colorSpaceDisposition destColorSpace, bool);
    int injectLZStrip(ubyte *LZBuffer, int numBytes, int imageWidth, int imageHeight, colorSpaceDisposition destColorSpace, bool);
    int injectJPEG(char *jpeg_Buff, int imageWidth, int imageHeight, int numCompBytes, colorSpaceDisposition destColorSpace, bool);
    void initOutBuff(char *buff, sint32 size);
    void writeStr2OutBuff(char *str);
    void write2Buff(ubyte *buff, int buffSize);
    int statOutputFileSize();
    void writePDFGrammarTrailer(int imageWidth, int imageHeight);
    bool injectAdobeRGBCS();
    bool addKids(sint32 kidObj);
    bool addXRef(sint32 xRefObj);
    int errorOutAndCleanUp();
	void Cleanup(void); /*Called in errorOutAndCleanUp and Destructor*/
	void writeJobTicket(void);
#ifdef SUPPORT_WHITE_STRIPS
    bool isWhiteStrip(void *, int);
#endif
	char* getInputBinString(jobInputBin bin);
	char* getOutputBin(jobOutputBin bin);
	char* getOrientationString(mediaOrientationDisposition Orientation);
	char* getDuplexString(duplexDispositionEnum duplex);
	char* getColorThemesString(colorThemes colortheme);

    sint32                      currStripHeight;
    char                        currMediaName[256];
    duplexDispositionEnum       currDuplexDisposition;
    colorSpaceDisposition       currColorSpaceDisposition;
    debugDisposition            currDebugDisposition;
    compressionDisposition      currCompressionDisposition;
    mediaOrientationDisposition currMediaOrientationDisposition;
    renderResolution            currRenderResolution;
    int                         currRenderResolutionInteger;
    void                        *allocatedOutputBuffer;
    void                        *leftoverScanlineBuffer;

    int                         mediaWidth;
    int                         mediaHeight;
    int                         mediaWidthInPixels;
    int                         mediaHeightInPixels;
    colorSpaceDisposition       destColorSpace;
    colorSpaceDisposition       sourceColorSpace;
    int                         scaleFactor;
    bool                        genExtraPage;
    jobStateEnum                jobOpen;
    int                         currSourceWidth;
    int                         currSourceHeight;
    int                         srcNumComponents;
    int                         dstNumComponents;
    int                         numLeftoverScanlines;
    ubyte                       *scratchBuffer;
    int                         pageCount;
    bool                        reverseOrder;
    int                         outBuffSize;
    int                         currOutBuffSize;
    int                         totalBytesWrittenToPCLmFile;
    int                         totalBytesWrittenToCurrBuff;
    char                        *outBuffPtr;
    char                        *currBuffPtr;
    sint32                      currResolution;
    float                       STANDARD_SCALE;
    sint32                      DebugIt;
    sint32                      DebugIt2;
    sint32                      objCounter;

    sint32                      yPosition;
    sint32                      infoObj;
    sint32                      pageOrigin;
    sint32                      *KidsArray;
    sint32                      numKids;
// XRefTable storage 
    sint32                      *xRefTable;
    sint32                      xRefIndex;
    sint32                      xRefStart;
    char                        pOutStr[OUT_STR_SIZE];
    bool                        adobeRGBCS_firstTime;
    bool                        mirrorBackside;
    sint32                      topMarginInPix;
    sint32                      leftMarginInPix;
    bool                        firstStrip;
    sint32                      numFullInjectedStrips;
    sint32                      numFullScanlinesToInject;
    sint32                      numPartialScanlinesToInject;

	PCLmSUserSettingsType *m_pPCLmSSettings;
	char returnStr[256];
};

#endif /* _PCLM_PARSER_ */


