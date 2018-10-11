//
// Linux/GNU SANE (Scanner Access Now Easy) driver
// - www.sane-project.org
//
// (c) 2004 Hewlett-Packard Company, All Rights Reserved.
//
// derived from SimpleScan.cpp,
//   which was factored into the initial SANE API framework
//



#ifdef __cplusplus
 extern "C" {
#endif

#include "sane.h"           /* SANE API */
#define DEBUG_NOT_STATIC
#undef NDEBUG
#include "orblitei.h"
#include "orblite.h"//Added New
#include <math.h>
#include "utils.h"
#include "io.h"

#ifdef __cplusplus
 }
#endif 
static SANE_Range SANE_rangeLeft;
static SANE_Range SANE_rangeRight;
static SANE_Range SANE_rangeTop;
static SANE_Range SANE_rangeBottom;

#define MAX_MODES       7 
static SANE_String SANE_modes[MAX_MODES+1];

#define MAX_SOURCES     15
static SANE_String SANE_sources[MAX_SOURCES+1];

#define MAX_RESOLUTIONS 31
static SANE_Word SANE_resolutions[MAX_RESOLUTIONS+1];

#define MAX_PAPER_SIZES 63
static SANE_String SANE_paper_sizes[MAX_PAPER_SIZES+1];

static struct t_SANE *g_handle = NULL;

SANE_Option_Descriptor	DefaultOrbOptions[] = {
	/* name, title, desc,
	 * type, unit, size, cap,
	 * constraint_type, constraint */

	// optCount
	{
		"", SANE_TITLE_NUM_OPTIONS, SANE_DESC_NUM_OPTIONS,
		SANE_TYPE_INT, SANE_UNIT_NONE, sizeof(SANE_Word), 
		SANE_CAP_SOFT_DETECT,
		SANE_CONSTRAINT_NONE, NULL
	},
    
	// optTLX, optTLY, optBRX, optBRY,                  // geometry group
	{
		SANE_NAME_SCAN_TL_X, SANE_TITLE_SCAN_TL_X, SANE_DESC_SCAN_TL_X,  // name, title, desc
		SANE_TYPE_FIXED, SANE_UNIT_MM, sizeof(SANE_Fixed),// type, unit, size
		SANE_CAP_SOFT_SELECT | SANE_CAP_SOFT_DETECT,      // cap(ability)
		SANE_CONSTRAINT_RANGE, (SANE_Char**)&SANE_rangeLeft  // constraint_type, constraint
	},

	{
		SANE_NAME_SCAN_TL_Y, SANE_TITLE_SCAN_TL_Y, SANE_DESC_SCAN_TL_Y,  // name, title, desc
		SANE_TYPE_FIXED, SANE_UNIT_MM, sizeof(SANE_Fixed),// type, unit, size
		SANE_CAP_SOFT_SELECT | SANE_CAP_SOFT_DETECT,      // cap(ability)
		SANE_CONSTRAINT_RANGE, (SANE_Char**)&SANE_rangeTop  // constraint_type, constraint
	},
  
	{
		SANE_NAME_SCAN_BR_X, SANE_TITLE_SCAN_BR_X, SANE_DESC_SCAN_BR_X,  // name, title, desc
		SANE_TYPE_FIXED, SANE_UNIT_MM, sizeof(SANE_Fixed),// type, unit, size
		SANE_CAP_SOFT_SELECT | SANE_CAP_SOFT_DETECT,      // cap(ability)
		SANE_CONSTRAINT_RANGE, (SANE_Char**)&SANE_rangeRight  // constraint_type, constraint
	},

	{
		SANE_NAME_SCAN_BR_Y, SANE_TITLE_SCAN_BR_Y, SANE_DESC_SCAN_BR_Y,  // name, title, desc
		SANE_TYPE_FIXED, SANE_UNIT_MM, sizeof(SANE_Fixed),// type, unit, size
		SANE_CAP_SOFT_SELECT | SANE_CAP_SOFT_DETECT,      // cap(ability)
		SANE_CONSTRAINT_RANGE, (SANE_Char**)&SANE_rangeBottom  // constraint_type, constraint
	},
   
	// optResolution,                                       // resolution group  
	{
		SANE_NAME_SCAN_RESOLUTION, SANE_TITLE_SCAN_RESOLUTION, SANE_DESC_SCAN_RESOLUTION, // name, title, desc
		SANE_TYPE_INT, SANE_UNIT_DPI, sizeof(SANE_Word),  // type, unit, size,
		SANE_CAP_SOFT_SELECT | SANE_CAP_SOFT_DETECT,      // cap(ability)
		SANE_CONSTRAINT_WORD_LIST, (SANE_Char**)SANE_resolutions // constraint type, constraint
	},

	// optMode,                                            // color/depth group
	{
		SANE_NAME_SCAN_MODE, SANE_TITLE_SCAN_MODE, SANE_DESC_SCAN_MODE, // name, title, desc
		SANE_TYPE_STRING, SANE_UNIT_NONE, 256,  // type, unit, size,
		SANE_CAP_SOFT_SELECT | SANE_CAP_SOFT_DETECT,      // cap(ability)
		SANE_CONSTRAINT_STRING_LIST, (SANE_Char**)SANE_modes   // constraint type, constraint
	},
 
	// optSource, 
	{
		SANE_NAME_SCAN_SOURCE, SANE_TITLE_SCAN_SOURCE, SANE_DESC_SCAN_SOURCE, // name, title, desc
		SANE_TYPE_STRING, SANE_UNIT_NONE, 256,  // type, unit, size,
		SANE_CAP_SOFT_SELECT | SANE_CAP_SOFT_DETECT,      // cap(ability)
		SANE_CONSTRAINT_STRING_LIST, (SANE_Char**)SANE_sources   // constraint type, constraint
	},
 
	// optPaperSize, 
	{
		SANE_NAME_PAPER_SIZE, SANE_TITLE_PAPER_SIZE, SANE_DESC_PAPER_SIZE, // name, title, desc
		SANE_TYPE_STRING, SANE_UNIT_NONE, 256,  // type, unit, size,
		SANE_CAP_SOFT_SELECT | SANE_CAP_SOFT_DETECT,      // cap(ability)
		SANE_CONSTRAINT_STRING_LIST, (SANE_Char**)SANE_paper_sizes   // constraint type, constraint
	},
 
	// optPaperSize, 
	{
		SANE_NAME_PAPER_SIZE, SANE_TITLE_PAPER_SIZE, SANE_DESC_PAPER_SIZE, // name, title, desc
		SANE_TYPE_INT, SANE_UNIT_DPI, sizeof(SANE_Word),  // type, unit, size,
		SANE_CAP_SOFT_SELECT | SANE_CAP_SOFT_DETECT,      // cap(ability)
		SANE_CONSTRAINT_WORD_LIST, (SANE_Char**)SANE_resolutions // constraint type, constraint
	},
#ifdef NOTDEF
// default template
	{
		"", "", "",                       // name, title, desc
		SANE_TYPE_INT, SANE_UNIT_NONE, sizeof(SANE_Word),    // type, unit, size,
		0,                                // cap(ability)
		SANE_CONSTRAINT_NONE, 0           // constraint type, constraint
	},
#endif
};


static int bb_load(SANE_THandle ps, const char* so)
{
	int stat = 1;
	/* Load hpmud manually with symbols exported. Otherwise the plugin will not find it. */
	if ((ps->hpmud_handle = load_library("libhpmud.so.0")) == NULL)
	{
	   if ((ps->hpmud_handle = load_library("libhpmud.so.0")) == NULL)
	   goto bugout;
	}

	if ((ps->bb_handle = load_plugin_library(UTILS_SCAN_PLUGIN_LIBRARY, so)) == NULL)
	{
		SendScanEvent(ps->uri, EVENT_PLUGIN_FAIL);
		goto bugout;
	} 
	if ((ps->bb_orblite_init = get_library_symbol(ps->bb_handle, "bb_orblite_init")) == NULL)
		goto bugout;

	if ((ps->bb_orblite_get_devices = get_library_symbol(ps->bb_handle, "bb_orblite_get_devices")) == NULL)
		goto bugout;
	
	if ((ps->bb_orblite_exit = get_library_symbol(ps->bb_handle, "bb_orblite_exit")) == NULL)
		goto bugout;


	if ((ps->bb_orblite_open = get_library_symbol(ps->bb_handle, "bb_orblite_open")) == NULL)
		goto bugout;
	if ((ps->bb_orblite_close = get_library_symbol(ps->bb_handle, "bb_orblite_close")) == NULL)
		goto bugout;
	if ((ps->bb_orblite_get_option_descriptor = get_library_symbol(ps->bb_handle, "bb_orblite_get_option_descriptor")) == NULL)
		goto bugout;
	if ((ps->bb_orblite_control_option = get_library_symbol(ps->bb_handle, "bb_orblite_control_option")) == NULL)
		goto bugout;

	if ((ps->bb_orblite_start = get_library_symbol(ps->bb_handle, "bb_orblite_start")) == NULL)
		goto bugout;
	if ((ps->bb_orblite_get_parameters = get_library_symbol(ps->bb_handle, "bb_orblite_get_parameters")) == NULL)
		goto bugout;
	if ((ps->bb_orblite_read = get_library_symbol(ps->bb_handle, "bb_orblite_read")) == NULL)
		goto bugout;

	if ((ps->bb_orblite_cancel = get_library_symbol(ps->bb_handle, "bb_orblite_cancel")) == NULL)
		goto bugout;

	if ((ps->bb_orblite_set_io_mode = get_library_symbol(ps->bb_handle, "bb_orblite_set_io_mode")) == NULL)
		goto bugout;

	if ((ps->bb_orblite_get_select_fd = get_library_symbol(ps->bb_handle, "bb_orblite_get_select_fd")) == NULL)
		goto bugout;
	stat = 0;
bugout:
	return stat;

}

static int bb_unload(SANE_THandle ps)
{
   _DBG("Calling orblite bb_unload: \n");
   if (ps->bb_handle)
   {
      dlclose(ps->bb_handle);
      ps->bb_handle = NULL;
   }
   if (ps->hpmud_handle)
   {
      dlclose(ps->hpmud_handle);
      ps->hpmud_handle = NULL;
   }
//   if (ps->math_handle)
//   { 
//      dlclose(ps->math_handle);
//      ps->math_handle = NULL;
//   }
   return 0;
}

SANE_Status 
orblite_init (SANE_Int * version_code, SANE_Auth_Callback authorize)
{
/*
	// Allocate handle, set all handle values to zero
	g_handle = (SANE_THandle) calloc( 1, sizeof(struct t_SANE) );
	if (!g_handle)
		return SANE_STATUS_NO_MEM;

	// Allocate option descriptors.
	g_handle->Options = (SANE_Option_Descriptor *) calloc( optLast, sizeof(SANE_Option_Descriptor) );
	if (!g_handle->Options)
		return SANE_STATUS_NO_MEM;
	
	memcpy( g_handle->Options, DefaultOrbOptions, optLast*sizeof(SANE_Option_Descriptor) );
	g_handle->tag = (char*)malloc(strlen("ORBLITE") + 1);
        strcpy(g_handle->tag, "ORBLITE");

	if (bb_load(g_handle, SCAN_PLUGIN_ORBLITE))
	{
		printf("orblite_init failed: %s %d\n", __FILE__, __LINE__);
		return SANE_STATUS_IO_ERROR;
	}

	return g_handle->bb_orblite_init(version_code, authorize);
*/

	return SANE_STATUS_GOOD;
}



SANE_Status 
orblite_get_devices (const SANE_Device *** device_list, SANE_Bool local_only)
{
        //return g_handle->bb_orblite_get_devices(device_list, local_only);
	return SANE_STATUS_GOOD;
}



void 
orblite_exit (void)
{
	return g_handle->bb_orblite_exit();
}


/////////////////////// sane_open, close //////////////////////////////////////


SANE_Status 
orblite_open (SANE_String_Const devicename, SANE_Handle * handle)
{
	SANE_Status stat;
	SANE_Int * version_code;
	SANE_Auth_Callback authorize;
	const SANE_Device *** device_list;
	SANE_Bool local_only;
	
	
	// Allocate handle, set all handle values to zero
	g_handle = (SANE_THandle) calloc( 1, sizeof(struct t_SANE) );
	if (!g_handle)
		return SANE_STATUS_NO_MEM;

	// Allocate option descriptors.
	g_handle->Options = (SANE_Option_Descriptor *) calloc( optLast, sizeof(SANE_Option_Descriptor) );
	if (!g_handle->Options)
		return SANE_STATUS_NO_MEM;
	
	memcpy( g_handle->Options, DefaultOrbOptions, optLast*sizeof(SANE_Option_Descriptor) );
	g_handle->tag = (char*)malloc(strlen("ORBLITE") + 1);
        strcpy(g_handle->tag, "ORBLITE");


	if (bb_load(g_handle, SCAN_PLUGIN_ORBLITE))
	{
		printf("orblite_init failed: %s %d\n", __FILE__, __LINE__);
		return SANE_STATUS_IO_ERROR;
	}

	stat = g_handle->bb_orblite_init(version_code, authorize);
	if (stat != SANE_STATUS_GOOD)
		return stat;

	stat = g_handle->bb_orblite_get_devices(device_list, local_only);
	if (stat != SANE_STATUS_GOOD)
		return stat;

	stat = g_handle->bb_orblite_open(devicename, &g_handle);
	if (stat == SANE_STATUS_GOOD)
		*handle = g_handle;

	return stat;	
}


void
orblite_close (SANE_Handle handle)
{
	return g_handle->bb_orblite_close(g_handle);
}


////// sane_get_option_descriptor, control_option, start, cancel //////////////



const SANE_Option_Descriptor *
orblite_get_option_descriptor (SANE_Handle handle, SANE_Int option)
{
if (option >= optCount && option < optLast)
	{
		return &g_handle->Options[option];
	}
	//return g_handle->bb_orblite_get_option_descriptor(g_handle, option);
}


SANE_Status
orblite_control_option (SANE_Handle handle, 
		     SANE_Int option,
		     SANE_Action action, 
		     void *value,
		     SANE_Int * info)
{
	return g_handle->bb_orblite_control_option(handle,option,action, value, info);

}



SANE_Status
orblite_start (SANE_Handle handle)
{
	return g_handle->bb_orblite_start(g_handle);
}


SANE_Status
orblite_get_parameters (SANE_Handle handle, SANE_Parameters * params)
{
	return g_handle->bb_orblite_get_parameters(g_handle, params);
}



SANE_Status
orblite_read (SANE_Handle handle, SANE_Byte * data, SANE_Int max_length, SANE_Int * length)
{
	return g_handle->bb_orblite_read (g_handle, data, max_length, length);
}


void
orblite_cancel (SANE_Handle handle)
{
	return g_handle->bb_orblite_cancel(g_handle);
}

////// sane_set_io_mode, get_select_fd, strstatus - UNIMPLEMENTED /////////////


SANE_Status
orblite_set_io_mode (SANE_Handle h, SANE_Bool non_blocking)
{
	return g_handle->bb_orblite_set_io_mode(g_handle, non_blocking);
}


SANE_Status
orblite_get_select_fd (SANE_Handle h, SANE_Int * fd)
{
	return g_handle->bb_orblite_get_select_fd(g_handle, fd);
}


