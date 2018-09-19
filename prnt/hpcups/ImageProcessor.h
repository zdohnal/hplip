#ifndef IMAGE_PROCESSOR_H
#define IMAGE_PROCESSOR_H

#include <cups/raster.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char     BYTE;

typedef struct _image_processor_handle image_processor_t; // opaque image processor handle type

typedef enum
{
// general or system errors
    IPE_SUCCESS          =  0x00,    //!< everything okay
    IPE_CANCEL           =  0x01,    //!< CANCEL chosen by user
    IPE_GENERAL_ERROR    =  0x02,    //!< something bad that should not have happened
    IPE_MEM_ERROR        =  0x03,    //!< failed to allocate memory
    IPE_NULL_PTR         =  0x04,    //!< null pointer passed as parameter is not allowed

//  Runtime related
    IPE_LIBRARY_MISSING  = 0x30,   //!< a required dynamic library is missing

} IMAGE_PROCESSOR_ERROR; //DRIVER_ERROR

extern  image_processor_t* imageProcessorCreate();
extern  void imageProcessorDestroy(image_processor_t*);
extern IMAGE_PROCESSOR_ERROR imageProcessorStartPage (image_processor_t*, cups_page_header2_t* cups_header);
extern IMAGE_PROCESSOR_ERROR imageProcessorProcessLine(image_processor_t*, BYTE* pLineBuffer, unsigned bufferSize);
extern IMAGE_PROCESSOR_ERROR imageProcessorEndPage (image_processor_t*);

#ifdef __cplusplus
} // extern c
#endif

#endif // IMAGE_PROCESSOR_H