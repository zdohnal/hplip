#ifndef _SANE_ORBLITE_INTERFACE_H
#define _SANE_ORBLITE_INTERFACE_H

#include <stdbool.h>
#include "sane.h"
#include "saneopts.h"       /* well-known options */
#include "../../io/hpmud/hpmud.h"
#include "../../ip/hpip.h"
#include "OrbliteScan/MacCommon.h"

#define max(x,y) ((x)>(y))?x:y
#define min(x,y) ((x)>(y))?y:x

#define COLOR_STR	SANE_I18N("color")
#define GRAY_STR	SANE_I18N("gray")
#define LINEART_STR	SANE_I18N("lineart")

#define FLATBED_STR		SANE_I18N("Flatbed")
#define ADF_SINGLE_STR		SANE_I18N("ADF-SinglePage")
#define ADF_STR		SANE_I18N("ADF")
#define ADF_MULTI_STR		SANE_I18N("ADF-MultiPage")
#define ADF_MULTI_SIMPLEX_STR	SANE_I18N("ADF-MultiPage-Simplex")
#define ADF_MULTI_DUPLEX_STR	SANE_I18N("ADF-MultiPage-Duplex")
#define NEGATIVE_STR		SANE_I18N("Negative")
#define SLIDE_STR		SANE_I18N("Slide")

#define CUSTOM_STR	SANE_I18N("Custom")
#define DETECT_STR	SANE_I18N("Auto-Detect")
#define LETTER_STR	SANE_I18N("Letter")
#define LEGAL_STR	SANE_I18N("Legal")
#define A4_STR		SANE_I18N("A4")
#define A5_STR		SANE_I18N("A5")
#define A6_STR		SANE_I18N("A6")
#define A7_STR		SANE_I18N("A7")
#define B5_STR		SANE_I18N("B5")
#define B6_STR		SANE_I18N("B6")
#define B7_STR		SANE_I18N("B7")
#define C5_STR		SANE_I18N("C5")
#define C6_STR		SANE_I18N("C6")
#define C7_STR		SANE_I18N("C7")
#define JIS_B5_STR	SANE_I18N("JIS-B5")
#define JIS_B6_STR	SANE_I18N("JIS-B6")
#define JIS_B7_STR	SANE_I18N("JIS-B7")

#define SANE_NAME_PAPER_SIZE	SANE_I18N("paper-size")
#define SANE_TITLE_PAPER_SIZE	SANE_I18N("Paper size")
#define SANE_DESC_PAPER_SIZE \
	SANE_I18N("Sets scan area to selected paper size")
#if 0
static SANE_Range SANE_rangeLeft;
static SANE_Range SANE_rangeRight;
static SANE_Range SANE_rangeTop;
static SANE_Range SANE_rangeBottom;

#define MAX_MODES	7
static SANE_String SANE_modes[MAX_MODES+1];

#define MAX_SOURCES	15
static SANE_String SANE_sources[MAX_SOURCES+1];

#define MAX_RESOLUTIONS	31
static SANE_Word SANE_resolutions[MAX_RESOLUTIONS+1];

#define MAX_PAPER_SIZES	63
static SANE_String SANE_paper_sizes[MAX_PAPER_SIZES+1];
#endif

/* options enumerator */
typedef enum
{
	optCount = 0,
	optTLX, optTLY, optBRX, optBRY,           // geometry group
	optResolution,                               // resolution group
	optMode,                                  // color/depth group
	optSource,
	optPaperSize,
	optMultipick,
	optLast,

	/* disabled options after optLast */

	optGammaTableRed,             /* Gamma Tables */
	optGammaTableGreen,
	optGammaTableBlue,
}
EOptionIndex;

extern SANE_Option_Descriptor	DefaultOrbOptions[];

struct t_SANE {
        char *tag;
        SANE_Option_Descriptor* Options;
//        CHPScan*              m_pHPScan;
        struct SCANNER_PARAMETERS    m_scannerParameters;
        struct BUFFER_INFO           m_bufferInfo;
        SCAN_SOURCE           m_scanSource;
        DWORD                 dwRemainingScanLines;
        DWORD                 dwBytesRead;
        DWORD                 dwBytesWritten;
        DWORD                 dwBufferSize;
        BYTE*                 pBuffer;
        BOOLEAN               bUseXPA;
        BOOLEAN               bUseADF;
        bool                  bDetectPageSize;
        bool                  bEndOfPage;
        bool                  bCancelled;
        bool                  invert;        // invert image
        SANE_Fixed            left;
	SANE_Fixed	      right;
	SANE_Fixed	      top;
	SANE_Fixed	      bottom;
        SANE_String           mode;
        SANE_String	      source;
	SANE_String	      paper_size;

	char uri[HPMUD_LINE_SIZE]; /* device uri */
	HPMUD_DEVICE dd;                /* hpiod device descriptor */
	HPMUD_CHANNEL cd;               /* hpiod eSCL channel descriptor */
	char model[HPMUD_LINE_SIZE];

	void *hpmud_handle;         /* returned by dlopen */
	void *math_handle;         /* returned by dlopen */
	void *bb_handle;            /* returned by dlopen */

	SANE_Status (*bb_orblite_init) (SANE_Int * version_code, SANE_Auth_Callback authorize);
	SANE_Status (*bb_orblite_get_devices) (const SANE_Device *** device_list, SANE_Bool local_only);
	void (*bb_orblite_exit)(void);
	SANE_Status (*bb_orblite_open) (SANE_String_Const devicename, SANE_Handle * handle);
	void (*bb_orblite_close) (SANE_Handle handle);
	const SANE_Option_Descriptor * (*bb_orblite_get_option_descriptor) (SANE_Handle handle, SANE_Int option);
	SANE_Status (*bb_orblite_control_option) (SANE_Handle handle, SANE_Int option, SANE_Action action, void *value, SANE_Int * info);
	SANE_Status (*bb_orblite_start) (SANE_Handle handle);
	SANE_Status (*bb_orblite_get_parameters) (SANE_Handle handle, SANE_Parameters * params);
	SANE_Status (*bb_orblite_read) (SANE_Handle handle, SANE_Byte * data, SANE_Int max_length, SANE_Int * length);
	void (*bb_orblite_cancel) (SANE_Handle handle);
	SANE_Status (*bb_orblite_set_io_mode) (SANE_Handle h, SANE_Bool non_blocking);
	SANE_Status (*bb_orblite_get_select_fd) (SANE_Handle h, SANE_Int * fd);

};

typedef struct t_SANE * SANE_THandle;


#endif
