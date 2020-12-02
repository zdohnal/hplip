#ifndef _DISCOVERY_AVAHI_H
#define _DISCOVERY_AVAHI_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <syslog.h>
#include <stdarg.h>

#include <avahi-client/client.h>
#include <avahi-client/lookup.h>
#include <avahi-common/simple-watch.h>
#include <avahi-common/malloc.h>
#include <avahi-common/error.h>

#ifndef DBUS_TIMEOUT_INFINITE
#define DBUS_TIMEOUT_INFINITE ((int) 0x7fffffff)
#endif

#define NULL_CHAR_SIZE 1
#define HP_PLAIN_URI_LEN 24
#define HP_SKIP_MFG_NAME_SIZE 3
#define MFG_HP "HP"
#define MFG_HP_LEN 2
#define MFG_NAME "mfg"
#define MDL_NAME "mdl"
#define TYPE_NAME "ty"
#define HPMUD_LINE_SIZE 256
#define MAX_URI_LEN 512
#define HP_MAX_SCAN_BUFF 16384
#define HP_EXT_SCAN_BUFF 8192
#define MAX_NAME_LENGTH 256
#define AVAHI_NET_DISCOVERY 1
#define AVAHI_HOST_LOOKUP 2
#define MAX_IP_ADDR_LEN 16
#define APPEND_LOCAL_LEN 6

//Error Codes
#define AVAHI_STATUS_OK 0
#define AVAHI_STATUS_ERROR 1
#define AVAHI_STATUS_TIMEOUT 2
#define SYSTEMD_SERVICE_TIMEOUT 10000

#define POLKIT_AUTH_DBUS  "org.freedesktop.PolicyKit1"
#define POLKIT_AUTH_INTF  "org.freedesktop.PolicyKit1.Authority"
#define POLKIT_AUTH_PATH  "/org/freedesktop/PolicyKit1/Authority"
#define POLKIT_AUTH_METHOD_CALL "CheckAuthorization"

#define ACTION_ID "org.freedesktop.systemd1.manage-units"
#define SYSTEM_BUS_NAME "system-bus-name"
#define AVAHI_SERVICE_NAME "avahi-daemon.service"
#define AVHHI_SERVICE_MODE_REPLACE "replace"

#define SYSTEMD_DBUS_NAME "org.freedesktop.systemd1"
#define SYSTEMD_DBUS_PATH "/org/freedesktop/systemd1"
#define SYSTEMD_DBUS_INTF "org.freedesktop.systemd1.Manager"
#define SYSTEMD_START_SERVICE_METHOD "StartUnit"

extern char* aUriBuf;
extern char ipAddressBuff[MAX_IP_ADDR_LEN];

//#define MDNS_DEBUG

#define _STRINGIZE(x) #x
#define STRINGIZE(x) _STRINGIZE(x)

#define BUG(args...) syslog(LOG_ERR, __FILE__ " " STRINGIZE(__LINE__) ": " args)
#ifdef AVAHI_DEBUG
   #define DBG(args...) syslog(LOG_INFO, __FILE__ " " STRINGIZE(__LINE__) ": " args)
#else
   #define DBG(args...)
#endif

/*Function Prototypes*/

int   avahi_probe_nw_scanners();
int   avahi_lookup(const char* hostname);

#endif // _DISCOVERY_AVAHI_H

