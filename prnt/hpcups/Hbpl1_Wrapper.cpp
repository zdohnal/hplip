/*****************************************************************************\
  Pclm_wrapper.h : Interface for the PCLm class

  Copyright (c) 1996 - 2015, HP Co.
  All rights reserved. Copying or other reproduction of this
  program except for archival purposes is prohibited without
  the prior written consent of HP Company.

  HP COMPANY
  11311 Chinden Boulevard
  Boise, Idaho  83714

  Author(s): Suma Byrappa
  Contributor(s): Sanjay Kumar, Goutam Kodu
\*****************************************************************************/



#include "Hbpl1.h"
#include "PCLmGenerator.h"

extern "C" Hbpl1Wrapper* create_object(Hbpl1* const m_Hbpl1)
{
    return new Hbpl1Wrapper(m_Hbpl1);
}

extern "C" void destroy_object( Hbpl1Wrapper* object )
{
    if(object)
        delete object;
}

PCLmGenerator *m_pPCLmGenerator;


Hbpl1Wrapper::Hbpl1Wrapper(Hbpl1* const m_Hbpl1)
{
    o_Hbpl1 = m_Hbpl1;
    o_Hbpl1->m_pbyStripData = NULL;

    m_pPCLmGenerator = new PCLmGenerator();
}

Hbpl1Wrapper::~Hbpl1Wrapper()
{
    delete m_pPCLmGenerator;
}


DRIVER_ERROR Hbpl1Wrapper::InitStripBuffer(long long NoOfBytes)
{
	DRIVER_ERROR err = NO_ERROR;
	o_Hbpl1->m_nStripSize = NoOfBytes;

	o_Hbpl1->m_pbyStripData = new BYTE[o_Hbpl1->m_nStripSize];
	if(NULL == o_Hbpl1->m_pbyStripData)
	{
		err = SYSTEM_ERROR;
	}
	memset(o_Hbpl1->m_pbyStripData,0xFF,o_Hbpl1->m_nStripSize);
	o_Hbpl1->m_numScanLines = 0;
	return err;
}


void Hbpl1Wrapper::FreeStripBuffer(void)
{
	if(o_Hbpl1->m_pbyStripData)
	{
		delete [] o_Hbpl1->m_pbyStripData;
		o_Hbpl1->m_pbyStripData = NULL;
	}
	o_Hbpl1->m_nStripSize = 0;
	o_Hbpl1->m_numScanLines = -1;
}


DRIVER_ERROR Hbpl1Wrapper::StartJob(void **pOutBuffer, int *pOutBufferSize)
{
    DRIVER_ERROR    err = NO_ERROR;

    m_pPCLmGenerator->StartJob(pOutBuffer,pOutBufferSize,false);
    return err;
}


DRIVER_ERROR Hbpl1Wrapper::EndJob(void **pOutBuffer, int *pOutBufferSize)
{
    DRIVER_ERROR    err = NO_ERROR;

    m_pPCLmGenerator->EndJob(pOutBuffer,pOutBufferSize);
    return err;
}


DRIVER_ERROR Hbpl1Wrapper::StartPage(void **pOutBuffer, int *pOutBufferSize)
{
    DRIVER_ERROR    err = NO_ERROR;
    PCLmPageSetup PCLmPageContent;
    PCLmSUserSettingsType  PCLmSSettings;
    PCLmSSetup PCLmSContent;
    PCLmSContent.PCLmPageContent = &PCLmPageContent;
    PCLmSContent.PCLmSUserSettings = &PCLmSSettings;
    PCLmPageContent.mediaHeight = o_Hbpl1->m_JA.media_attributes.physical_height;
    PCLmPageContent.mediaWidth =  o_Hbpl1->m_JA.media_attributes.physical_width;
    PCLmPageContent.mediaWidthOffset  = o_Hbpl1->m_JA.media_attributes.printable_start_x;
    PCLmPageContent.mediaHeightOffset  = o_Hbpl1->m_JA.media_attributes.printable_start_y;
    PCLmPageContent.sourceWidth = o_Hbpl1->m_JA.media_attributes.printable_width;
    PCLmPageContent.sourceHeight = o_Hbpl1->m_JA.media_attributes.printable_height;
    PCLmPageContent.stripHeight = o_Hbpl1->m_nStripHeight;
    PCLmPageContent.compTypeRequested = compressFlate;
    strncpy(PCLmPageContent.mediaSizeName, o_Hbpl1->m_JA.media_attributes.PageSizeName, sizeof(PCLmPageContent.mediaSizeName)-1);
    strncpy(PCLmSSettings.userMediaType, o_Hbpl1->m_JA.media_attributes.MediaTypeName, sizeof(PCLmSSettings.userMediaType)-1);
    PCLmSSettings.userInputBin = (jobInputBin)o_Hbpl1->m_JA.media_source;
    PCLmSSettings.jobid = o_Hbpl1->m_JA.job_id;
    strncpy(PCLmSSettings.user_name, o_Hbpl1->m_JA.user_name, sizeof(PCLmSSettings.user_name)-1);    
    strncpy(PCLmSSettings.datetime, o_Hbpl1->m_JA.job_start_time, sizeof(PCLmSSettings.datetime));

    switch (o_Hbpl1->m_ColorMode)
    {
        case COLORTYPE_COLOR:   PCLmPageContent.dstColorSpaceSpefication = deviceRGB;
                                PCLmPageContent.compTypeRequested = compressFlate;
                                break;
        case COLORTYPE_BLACK:   PCLmPageContent.dstColorSpaceSpefication = blackonly;
                                PCLmPageContent.compTypeRequested = compressFlate;
                                break;
        default:                PCLmPageContent.dstColorSpaceSpefication = grayScale;
                                PCLmPageContent.compTypeRequested = compressRLE;
                                break;
    }
    switch(o_Hbpl1->m_JA.quality_attributes.horizontal_resolution)
    {
	case 300:
		PCLmPageContent.destinationResolution = res300;
		break;
	case 1200:
		PCLmPageContent.destinationResolution = res1200;
		break;
	default:
		PCLmPageContent.destinationResolution = res600;
    }

    switch(o_Hbpl1->m_ColorTheme)
    {
        case DEFAULTSRGB    :   PCLmSSettings.colorTheme = default_colortheme; break;
        case PHOTOSRGB      :   PCLmSSettings.colorTheme = photosRGB; break;
        case ADOBERGB       :   PCLmSSettings.colorTheme = photoAdobeRGB; break;
        case VIVIDSRGB      :   PCLmSSettings.colorTheme = vividsRGB; break;
        default             :   PCLmSSettings.colorTheme = none;
    }

    //Output bin:Only default options is supported as of now.
    PCLmSSettings.userOutputBin = top_output;

	//userCopies: Must be 1 always as GS already gives those many copies
    PCLmSSettings.userCopies = 1;



	//Document Name
	strncpy(PCLmSSettings.userDocumentName,o_Hbpl1->m_JA.job_title,sizeof(PCLmSSettings.userDocumentName));


	//printQuality - As of now we support only normal,draft,photo
	if (o_Hbpl1->m_Economode) // Economode is ON
        strncpy(PCLmSSettings.userPageQuality, "draft", sizeof(PCLmSSettings.userPageQuality)-1);
    else
        strncpy(PCLmSSettings.userPageQuality, o_Hbpl1->m_JA.quality_attributes.hbpl1_print_quality, sizeof(PCLmSSettings.userPageQuality)-1);

	//Orientation: GS already does the orientation work.
	PCLmSSettings.userOrientation = portraitOrientation;


	PCLmPageContent.duplexDisposition = simplex;

	m_pPCLmGenerator->StartPage(&PCLmSContent,true,pOutBuffer,pOutBufferSize);


    return err;
}


DRIVER_ERROR Hbpl1Wrapper::EndPage(void **pOutBuffer, int *pOutBufferSize)
{
    DRIVER_ERROR    err = NO_ERROR;
    m_pPCLmGenerator->EndPage(pOutBuffer, pOutBufferSize);
    return err;
}


DRIVER_ERROR Hbpl1Wrapper::FormFeed()
{
     return NO_ERROR;
}

DRIVER_ERROR Hbpl1Wrapper::Encapsulate (void *pInBuffer, int inBufferSize, int numLines, void **pOutBuffer, int *pOutBufferSize)
{
    DRIVER_ERROR    err = NO_ERROR;
    m_pPCLmGenerator->Encapsulate(pInBuffer, inBufferSize, numLines, pOutBuffer, pOutBufferSize);
    return err;
}

DRIVER_ERROR Hbpl1Wrapper::SkipLines   (int iSkipLines)
{
    DRIVER_ERROR    err = NO_ERROR;
    return err;
}

void Hbpl1Wrapper::FreeBuffer (void *pBuffer, int bufSize)
{
    if (bufSize >= 0)
    {
        m_pPCLmGenerator->FreeBuffer(pBuffer);
    } else {
        // Error
    }
}

