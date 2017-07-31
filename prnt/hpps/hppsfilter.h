#ifndef _HPPSFILTER_H
#define _HPPSFILTER_H



#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <cups/cups.h>
#include <cups/ppd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_PJL    10
#define MAX_BUFFER 256

#define PJL_HEADER "\x1B%-12345X@PJL\x0A"
#define BOD_PJL_FIXED "0400040101020D10100115"
#define BOD_PPD_ATR "HPBOD"
#define HPDIGT_ATR "HPDigit"
#define BOD_PJL "@PJL DMINFO ASCIIHEX=\"%s%s\"\012"
#define BOD_DATETIME_FORMAT "%04d%02d%02d%02d%02d%02d"
#define DBG_PSFILE "hpps_job"
#define DBG_TMP_FOLDER "/var/log/hp/tmp/"

#define LINE_SIZE                    258
#define FILE_NAME_SIZE               128

#define SAVE_PS_FILE                 2

#define HPBOD		        "HPBOD"
#define HPACCOUNTINGINFO        "HPAccountingInfo"
#define HPPJLECONOMODE		"HPPJLEconoMode"
#define HPPJLECONOMODE2		"HPPJLEconoMode2"
#define HPPJLPRINTQUALITY	"HPPJLPrintQuality"
#define HPPJLOUTPUTMODE		"HPPJLOutputMode"
#define HPPJLDRYTIME		"HPPJLDryTime"
#define HPPJLSATURATION		"HPPJLSaturation"
#define HPPJLINKBLEED		"HPPJLInkBleed"
#define HPPJLCOLORASGRAY	"HPPJLColorAsGray"
#define HPPJLTRUEBLACK		"HPPJLTrueBlack"


#endif
