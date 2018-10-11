#ifndef H_MacCommon
#define H_MacCommon

#ifndef __linux__
#include <CoreFoundation/CFPlugInCOM.h>
#define __CFPlugInCOM_Included__
#endif

#ifdef __linux__
#    include "LinuxCommon.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#ifndef MAX_PATH
#define MAX_PATH		260
#endif

#ifndef _MAX_PATH
#define _MAX_PATH	1024
#endif

#define OUT
#define IN

#ifndef OKAY
#define OKAY			0
#endif

#define MAXDWORD		0xffffffff

#define __cdecl

#define ATLASSERT	assert
#define ASSERT		assert
#define _ASSERT		assert

#define IDOK                1
#define IDCANCEL            2
#define IDABORT             3
#define IDRETRY             4
#define IDIGNORE            5
#define IDYES               6
#define IDNO                7

#define TEXT(s) s
#define _TEXT(s) s
#define _T(s) s

#define _tzset tzset

//#ifndef __MACH__
//typedef	long			HRESULT;
//#endif
#define STDMETHOD(x) virtual HRESULT x
#define STDMETHODIMP HRESULT
#define STDMETHODIMP_(x)	x

typedef void*			HBITMAP;
typedef void*			PTIMERAPCROUTINE;
typedef void*			HANDLE;
typedef void*			HMODULE;
typedef HANDLE			*LPHANDLE;
typedef HANDLE			HINSTANCE;
typedef void*			HGLOBAL;
typedef HANDLE			HKEY;
typedef HKEY			*PHKEY;

#define DECLARE_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name
DECLARE_HANDLE(HDC);

typedef long			LONG;

#ifdef _HP_SANE_ORBLITE_

typedef long                    HRESULT;
typedef int                             BOOL;
typedef u_int32_t                       DWORD;
typedef u_int16_t                       WORD;
//typedef int64_t                         __int64;
typedef unsigned char   BYTE;
typedef unsigned long   ULONG;
typedef unsigned short  USHORT;
typedef unsigned int    UINT;
typedef void*                   PVOID;
typedef void*                   LPVOID;
typedef DWORD                   *LPDWORD;
typedef BYTE*                                   LPBYTE;
typedef BYTE*                                   PBYTE;
typedef WORD*                                   LPWORD;

typedef struct tagBITMAPINFOHEADER {
        DWORD   biSize;
        LONG    biWidth;
        LONG    biHeight;
        WORD    biPlanes;
        WORD    biBitCount;
        DWORD   biCompression;
        DWORD   biSizeImage;
        LONG    biXPelsPerMeter;
        LONG    biYPelsPerMeter;
        DWORD   biClrUsed;
        DWORD   biClrImportant;
} BITMAPINFOHEADER;

/*typedef struct tagRGBQUAD {
        BYTE    rgbBlue;
        BYTE    rgbGreen;
        BYTE    rgbRed;
        BYTE    rgbReserved;
} RGBQUAD;


typedef struct tagBITMAPINFO {
        BITMAPINFOHEADER        bmiHeader;
        RGBQUAD                         bmiColors[1];
} BITMAPINFO;
*/

typedef void IUnknown;
typedef unsigned long   ULONG;
typedef void*                   LPVOID;


#endif
typedef BYTE                    BOOLEAN;

#ifndef __MACH__
typedef unsigned long	REFIID;
#endif
typedef char			CHAR;
#ifdef __BOOL_DEFINED_AS_CHAR 
typedef signed char		BOOL;
#else
//typedef int				BOOL;
#endif
//typedef u_int32_t			DWORD;
//typedef u_int16_t			WORD;
typedef u_int64_t			UINT64;
typedef u_int64_t			DWORD64;
typedef unsigned long long	ULONGLONG;
typedef long long			LONGLONG;
typedef int64_t				INT64;
typedef int64_t				_int64;
//typedef int64_t				__int64;
//typedef unsigned char	BYTE;
typedef int				boolean;
typedef LONG*			PLONG;
#ifndef __MACH__
//typedef unsigned long	ULONG;
#endif
typedef ULONG*			ULONG_PTR;
//typedef unsigned short	USHORT;
typedef unsigned char	UCHAR;
typedef UCHAR*			PUCHAR;
typedef int				INT;
//typedef unsigned int	UINT;
typedef signed char     INT8;
typedef unsigned char	UINT8;
typedef unsigned short	UINT16;
typedef unsigned int	UINT32;
typedef signed int		INT32;
typedef signed short	INT16;
//typedef void			VOID;
//#define VOID			void
//typedef void*			PVOID;
//#ifndef __MACH__
//typedef void*			LPVOID;
//#endif
//typedef DWORD			*LPDWORD;
typedef DWORD			*DWORD_PTR;
typedef wchar_t			WCHAR;
typedef const wchar_t	*LPCWSTR;
typedef WCHAR *LPWSTR, *PWSTR;
typedef float			FLOAT;
typedef double          FLOAT64;
typedef double 			DOUBLE;
typedef size_t			SIZE_T;

//typedef BYTE*					LPBYTE;
//typedef BYTE*					PBYTE;
//typedef WORD*					LPWORD;

typedef char *              HPINT8;
typedef unsigned char * 	HPUINT8;
typedef short *     	    HPINT16;
typedef unsigned short *	HPUINT16;
typedef long *				HPINT32;
typedef unsigned long *		HPUINT32;
typedef float *				HPFLOAT32;
typedef double *			HPFLOAT64;

typedef char*			PINT8;
typedef unsigned char*	PUINT8;
typedef short*			PINT16;
typedef unsigned short*	PUINT16;
typedef INT32*			PINT32;
typedef UINT32*			PUINT32;

typedef char*			LPSTR;
typedef const char*		LPCSTR;
typedef const char*		LPCTSTR;
typedef char*			LPTSTR;
typedef char			TCHAR;
#define A2T
#define T2CA
#define T2OLE A2W
#define T2COLE A2W
#define OLE2T W2A
typedef wchar_t			OLECHAR;
typedef OLECHAR			*LPOLESTR;
typedef const LPOLESTR	LPCOLESTR;
typedef LPOLESTR 		BSTR;
typedef char**			HWND;
typedef UINT WPARAM;
typedef LONG LPARAM;
typedef LONG LRESULT;

typedef int				PSTIDEVICE;
typedef int				SCI;
typedef int				PSTI;
typedef long			REGSAM;
typedef FILE*			HFILE;

#define HFILE_ERROR		0

#define REG_NONE                    ( 0 )   // No value type
#define REG_SZ                      ( 1 )   // Unicode nul terminated string
#define REG_EXPAND_SZ               ( 2 )   // Unicode nul terminated string
                                            // (with environment variable references)
#define REG_BINARY                  ( 3 )   // Free form binary
#define REG_DWORD                   ( 4 )   // 32-bit number
#define REG_DWORD_LITTLE_ENDIAN     ( 4 )   // 32-bit number (same as REG_DWORD)
#define REG_DWORD_BIG_ENDIAN        ( 5 )   // 32-bit number
#define REG_LINK                    ( 6 )   // Symbolic Link (unicode)
#define REG_MULTI_SZ                ( 7 )   // Multiple Unicode strings
#define REG_RESOURCE_LIST           ( 8 )   // Resource list in the resource map
#define REG_FULL_RESOURCE_DESCRIPTOR ( 9 )  // Resource list in the hardware description
#define REG_RESOURCE_REQUIREMENTS_LIST ( 10 )

#define REG_CREATED_NEW_KEY         (0x00000001L)   // New Registry Key created
#define REG_OPENED_EXISTING_KEY     (0x00000002L)   // Existing Key opened
#define REG_OPTION_NON_VOLATILE		(0x00000000L)	// This key is not volatile; this is the default.
#define REG_OPTION_VOLATILE			(0x00000001L)	// All keys created by the function are volatile.

#define CALLBACK

typedef struct _GUID {
	ULONG   Data1;
	unsigned short Data2;
	unsigned short Data3;
	unsigned char Data4[8];
} GUID;

typedef GUID	CLSID;

#ifndef __linux__
const extern CLSID CLSID_NULL;
const extern GUID GUID_NULL;
#else
extern const CLSID CLSID_NULL;
#endif

typedef struct tagPOINT
{
    LONG  x;
    LONG  y;
} POINT, *PPOINT;

typedef struct tagRECT
{
	LONG	left;
	LONG	top;
	LONG	right;
	LONG	bottom;
} RECT;
typedef RECT	*LPRECT;

typedef struct
{
	DWORD	dwLowDateTime;
	DWORD	dwHighDateTime;
} FILETIME;
typedef FILETIME	*PFILETIME;

typedef struct
{
	DWORD	dwFileAttributes;
	FILETIME	ftCreationTime;
	FILETIME	ftLastAccessTime;
	FILETIME	ftLastWriteTime;
	DWORD	nFileSizeHigh;
	DWORD	nFileSizeLow;
	DWORD	dwReserved0;
	DWORD	dwReserved1;
	TCHAR	cFileName[MAX_PATH];
	TCHAR	cAlternateFileName[14];
} WIN32_FIND_DATA;
typedef WIN32_FIND_DATA		*LPWIN32_FIND_DATA;

typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

typedef struct _SECURITY_ATTRIBUTES {
	DWORD	nLenght;
	LPVOID	lpSecurityDescriptor;
	BOOL	bInheritHandle;
} SECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

#define DLL_PROCESS_ATTACH	1
#define DLL_THREAD_ATTACH	2
#define DLL_THREAD_DETACH	3
#define DLL_PROCESS_DETACH	0

#define FILE_DEVICE_PARALLEL_PORT       0x00000016

#define FILE_FLAG_WRITE_THROUGH         0x80000000
#define FILE_FLAG_OVERLAPPED            0x40000000
#define FILE_FLAG_NO_BUFFERING          0x20000000
#define FILE_FLAG_RANDOM_ACCESS         0x10000000
#define FILE_FLAG_SEQUENTIAL_SCAN       0x08000000
#define FILE_FLAG_DELETE_ON_CLOSE       0x04000000
#define FILE_FLAG_BACKUP_SEMANTICS      0x02000000
#define FILE_FLAG_POSIX_SEMANTICS       0x01000000
#define FILE_FLAG_OPEN_REPARSE_POINT    0x00200000
#define FILE_FLAG_OPEN_NO_RECALL        0x00100000

#define FILE_BEGIN                  SEEK_SET
#define FILE_CURRENT                SEEK_CUR
#define FILE_END                    SEEK_END

#define INVALID_HANDLE_VALUE        ((HANDLE)(-1))
#define INVALID_FILE_SIZE ((DWORD)0xFFFFFFFF)
#define INVALID_SET_FILE_POINTER ((DWORD)-1)
#define DELETE                      (0x00010000L)
#define READ_CONTROL                (0x00020000L)
#define WRITE_DAC                   (0x00040000L)
#define WRITE_OWNER                 (0x00080000L)
#define SYNCHRONIZE                 (0x00100000L)
#define GENERIC_READ                (0x80000000) /* from WINNT.H */
#define GENERIC_WRITE               (0x40000000) /* from WINNT.H */
#define FILE_SHARE_READ             (0x00000001) /* from WINNT.H */
#define FILE_SHARE_WRITE            (0x00000002) /* from WINNT.H */

/* Ternary raster operations */
#define SRCCOPY             (DWORD)0x00CC0020 /* dest = source                   */
#define SRCPAINT            (DWORD)0x00EE0086 /* dest = source OR dest           */
#define SRCAND              (DWORD)0x008800C6 /* dest = source AND dest          */
#define SRCINVERT           (DWORD)0x00660046 /* dest = source XOR dest          */
#define SRCERASE            (DWORD)0x00440328 /* dest = source AND (NOT dest )   */
#define NOTSRCCOPY          (DWORD)0x00330008 /* dest = (NOT source)             */
#define NOTSRCERASE         (DWORD)0x001100A6 /* dest = (NOT src) AND (NOT dest) */
#define MERGECOPY           (DWORD)0x00C000CA /* dest = (source AND pattern)     */
#define MERGEPAINT          (DWORD)0x00BB0226 /* dest = (NOT source) OR dest     */
#define PATCOPY             (DWORD)0x00F00021 /* dest = pattern                  */
#define PATPAINT            (DWORD)0x00FB0A09 /* dest = DPSnoo                   */
#define PATINVERT           (DWORD)0x005A0049 /* dest = pattern XOR dest         */
#define DSTINVERT           (DWORD)0x00550009 /* dest = (NOT dest)               */
#define BLACKNESS           (DWORD)0x00000042 /* dest = BLACK                    */
#define WHITENESS           (DWORD)0x00FF0062 /* dest = WHITE                    */

/* DIB color table identifiers */

#define DIB_RGB_COLORS      0 /* color table in RGBs */
#define DIB_PAL_COLORS      1 /* color table in palette indices */

#define CREATE_NEW          1
#define CREATE_ALWAYS       2
#define OPEN_EXISTING       3
#define OPEN_ALWAYS         4
#define TRUNCATE_EXISTING   5

#define FILE_ATTRIBUTE_READONLY         0x00000001
#define FILE_ATTRIBUTE_HIDDEN           0x00000002
#define FILE_ATTRIBUTE_SYSTEM           0x00000004
#define FILE_ATTRIBUTE_DIRECTORY        0x00000010
#define FILE_ATTRIBUTE_ARCHIVE          0x00000020
#define FILE_ATTRIBUTE_NORMAL           0x00000080
#define FILE_ATTRIBUTE_TEMPORARY        0x00000100

#define FILE_FLAG_WRITE_THROUGH     0x80000000
#define FILE_FLAG_RANDOM_ACCESS     0x10000000

#define METHOD_BUFFERED		0
#define METHOD_IN_DIRECT	1
#define METHOD_OUT_DIRECT	2
#define METHOD_NEITHER		3

#define FILE_ANY_ACCESS		0
#define FILE_READ_ACCESS	( 0x0001 )
#define FILE_WRITE_ACCESS	( 0x0002 )
#define FILE_ALL_ACCESS		( 0x0003 )

#define ERROR_OPERATION_ABORTED          995L
#define ERROR_IO_INCOMPLETE              996L
#define ERROR_IO_PENDING                 997L    // dderror

#define ERROR_NO_MORE_FILES              18L

#define WAIT_OBJECT_0	0
#define WAIT_ABANDONED	4
#define WAIT_TIMEOUT                     0x00000102L

#define CreateMutex(attr, owner, name)	((HANDLE)1)
#define ReleaseMutex(hObj)              ((BOOL)1)
#define CloseMutexHandle(hObj)          TRUE
#define ReleaseSemaphore(a, b, c)	1
#define TerminateThread(a, b);

// Define the facility codes
//
#define FACILITY_WINDOWS                 8
#define FACILITY_STORAGE                 3
#define FACILITY_SSPI                    9
#define FACILITY_SETUPAPI                15
#define FACILITY_RPC                     1
#define FACILITY_WIN32                   7
#define FACILITY_CONTROL                 10
#define FACILITY_NULL                    0
#define FACILITY_MSMQ                    14
#define FACILITY_MEDIASERVER             13
#define FACILITY_INTERNET                12
#define FACILITY_ITF                     4
#define FACILITY_DISPATCH                2
#define FACILITY_CERT                    11

#define PAGE_NOACCESS          0x01     
#define PAGE_READONLY          0x02     
#define PAGE_READWRITE         0x04     
#define PAGE_WRITECOPY         0x08     
#define PAGE_EXECUTE           0x10     
#define PAGE_EXECUTE_READ      0x20     
#define PAGE_EXECUTE_READWRITE 0x40     
#define PAGE_EXECUTE_WRITECOPY 0x80     
#define PAGE_GUARD            0x100     
#define PAGE_NOCACHE          0x200     
#define PAGE_WRITECOMBINE     0x400     
#define MEM_COMMIT           0x1000     
#define MEM_RESERVE          0x2000     
#define MEM_DECOMMIT         0x4000     
#define MEM_RELEASE          0x8000     
#define MEM_FREE            0x10000     
#define MEM_PRIVATE         0x20000     
#define MEM_MAPPED          0x40000     
#define MEM_RESET           0x80000     
#define MEM_TOP_DOWN       0x100000     
#define MEM_WRITE_WATCH    0x200000     
#define MEM_PHYSICAL       0x400000     
#define MEM_4MB_PAGES    0x80000000     
#define SEC_FILE           0x800000     
#define SEC_IMAGE         0x1000000     
#define SEC_RESERVE       0x4000000     
#define SEC_COMMIT        0x8000000     
#define SEC_NOCACHE      0x10000000     
#define MEM_IMAGE         SEC_IMAGE     
#define WRITE_WATCH_FLAG_RESET 0x01

#define MB_ICONEXCLAMATION	0
#define MB_SYSTEMMODAL		1
#define MB_ICONSTOP			2
#define MB_OK				3
#define MB_ICONERROR		4
#define MB_ICONWARNING		5
#define MB_TOPMOST			6

#define VER_PLATFORM_WIN32_NT	0
#define VER_PLATFORM_MACOS		1

#define HRESULT_FROM_WIN32(x)   (x ? ((HRESULT) (((x) & 0x0000FFFF) | (FACILITY_WIN32 << 16) | 0x80000000)) : 0 )

#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)
	
#define MAKE_HRESULT(sev,fac,code) \
    ((HRESULT) (((unsigned long)(sev)<<31) | ((unsigned long)(fac)<<16) | ((unsigned long)(code))) )

typedef struct _OVERLAPPED {
	DWORD	Internal;
	DWORD	InternalHigh;
	DWORD	Offset;
	DWORD	OffsetHigh;
	HANDLE	hEvent;
} OVERLAPPED;

typedef OVERLAPPED	*LPOVERLAPPED;

#if PRAGMA_STRUCT_ALIGN
#pragma options align=mac68k
#endif

typedef struct tagBITMAPFILEHEADER {
	WORD	bfType;
	DWORD	bfSize;
	WORD	bfReserved1;
	WORD	bfReserved2;
	DWORD	bfOffBits;
} BITMAPFILEHEADER;
/*
typedef struct tagBITMAPINFOHEADER {
	DWORD	biSize;
	LONG	biWidth;
	LONG	biHeight;
	WORD	biPlanes;
	WORD	biBitCount;
	DWORD	biCompression;
	DWORD	biSizeImage;
	LONG	biXPelsPerMeter;
	LONG	biYPelsPerMeter;
	DWORD	biClrUsed;
	DWORD	biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagRGBQUAD {
	BYTE	rgbBlue;
	BYTE	rgbGreen;
	BYTE	rgbRed;
	BYTE	rgbReserved;
} RGBQUAD;

typedef struct tagBITMAPINFO {
	BITMAPINFOHEADER	bmiHeader;
	RGBQUAD				bmiColors[1];
} BITMAPINFO;
*/
#if PRAGMA_STRUCT_ALIGN
#pragma options align=reset
#endif

typedef struct 
{
	BOOL	Allocated;
} FLAGS;

typedef struct 
{
	FLAGS	Flags;
} BITMAPHANDLE;

typedef struct {
	DWORD	dwOSVersionInfoSize;
	DWORD	dwMajorVersion;
	DWORD	dwMinorVersion;
	DWORD	dwBuildNumber;
	DWORD	dwPlatformId;
	TCHAR	szCSDVersion[128];
} OSVERSIONINFO;

typedef struct tagPALETTEENTRY {
    BYTE        peRed;
    BYTE        peGreen;
    BYTE        peBlue;
    BYTE        peFlags;
} PALETTEENTRY, *PPALETTEENTRY;//, FAR *LPPALETTEENTRY;

/* Logical Palette */
typedef struct tagLOGPALETTE {
    WORD        palVersion;
    WORD        palNumEntries;
    PALETTEENTRY        palPalEntry[1];
} LOGPALETTE, *PLOGPALETTE;//, NEAR *NPLOGPALETTE, FAR *LPLOGPALETTE;

typedef union _LARGE_INTEGER {
	struct {
#ifdef __LITTLE_ENDIAN__
		DWORD	LowPart;
		LONG	HighPart;
#else
		LONG	HighPart;
		DWORD	LowPart;
#endif
	};
	LONGLONG	QuadPart;
} LARGE_INTEGER;

typedef union _ULARGE_INTEGER {
	struct {
#ifdef __LITTLE_ENDIAN__
		DWORD	LowPart;
		DWORD	HighPart;
#else
		DWORD	HighPart;
		DWORD	LowPart;
#endif
	};
	ULONGLONG	QuadPart;
} ULARGE_INTEGER;

typedef struct RawFileHeader
{
	UInt16		fHeaderSize;
	UInt16		fImageFmt;
	UInt16		fComponentsPerPixel;
	UInt16		fBitsPerComponent;
	UInt32		fResDPI;
	UInt32		fWidth;
	UInt32		fHeight;
	
} RawFileHeader;	// total size 20 bytes

#ifndef FALSE
#define FALSE			0
#endif
#ifndef TRUE
#define TRUE			1
#endif

#define BI_RGB		0L

#define GetFocus()	1

#ifndef LOBYTE
#define LOBYTE(b) ((WORD)b&0x00ff)
#endif
#ifndef HIBYTE
#define HIBYTE(b) (((WORD)b>>8)&0x00ff)
#endif
#ifndef HIWORD
#define HIWORD(b) (((DWORD)b>>16)&0x0000ffff)
#endif
#ifndef LOWORD
#define LOWORD(b) ((DWORD)b&0x0000ffff)
#endif

#define	_tcspbrk		strpbrk
#define _tcsinc(_pc)    ((_pc)+1)
#define _tcsrchr		strrchr
#define _tcschr			strchr
#define _tcsstr			strstr
#define _tcsncmp		strncmp
#define _tcscmp			strcmp
#define _tcscpy			strcpy
#define _tcsicmp		strcmp
#define _tcslen			strlen
#define _tcsnicmp		strncmp
#define _tcsncpy		strncpy
#define _tcscat			strcat
#define lstrcat			strcat
#define lstrlen			strlen
#define lstrcpy			strcpy
#define wsprintf		sprintf
#define wvsprintf		vsprintf
#define _vstprintf_s	vsnprintf
#define _stprintf_s		sprintf_s
#define _tprintf		printf
#define _countof		sizeof
#define strnicmp		strncmp
#define _strnicmp		strnicmp
#define strncpy_s		strncpy
#define _tstoi			atoi
#define _ttol			atol
#define _tfopen			fopen
#define _fgetts			fgets
#define _stscanf		sscanf

#define CoTaskMemAlloc	malloc
#define CoTaskMemFree(a) free(a)

#define INFINITE				0xFFFFFFFF
#define SUCCESS 0
#define FAILURE 1
#ifndef  __CFPlugInCOM_Included__ //Defined in CFPlugInCOM.h
//typedef void IUnknown;
#define SUCCEEDED(Status)	((HRESULT)(Status) >= 0)
#define FAILED(Status)		((HRESULT)(Status) < 0)
#define S_OK				((HRESULT)0x00000000L)
#define S_FALSE				((HRESULT)0x00000001L)
#define E_UNEXPECTED ((HRESULT)0x8000FFFFL)
#define E_NOTIMPL ((HRESULT)0x80004001L)
#define E_OUTOFMEMORY ((HRESULT)0x8007000EL)
#define E_INVALIDARG ((HRESULT)0x80070057L)
#define E_NOINTERFACE ((HRESULT)0x80004002L)
#define E_POINTER ((HRESULT)0x80004003L)
#define E_HANDLE ((HRESULT)0x80070006L)
#define E_ABORT ((HRESULT)0x80004004L)
#define E_FAIL ((HRESULT)0x80004005L)
#define E_ACCESSDENIED ((HRESULT)0x80070005L)
#endif

#define WINAPI
#define USES_CONVERSION
#define ATL_NO_VTABLE
#define DECLARE_REGISTRY_RESOURCEID(a)
#define DECLARE_PROTECT_FINAL_CONSTRUCT()
#define BEGIN_COM_MAP(a)
#define COM_INTERFACE_ENTRY(a)
#define END_COM_MAP()

/* Spooler Error Codes */
#define SP_NOTREPORTED               0x4000
#define SP_ERROR                     (-1)
#define SP_APPABORT                  (-2)
#define SP_USERABORT                 (-3)
#define SP_OUTOFDISK                 (-4)
#define SP_OUTOFMEMORY               (-5)

#define STI_OK  S_OK
#define STI_ERROR_NO_ERROR          STI_OK
#define STIERR_UNSUPPORTED      	E_NOTIMPL

#define ZeroMemory(a, b) memset(a, 0, b)
#define CopyMemory(a, b, c) memcpy(a, b, c)

#ifndef MIN
#define MIN(a, b) (b < a ? b : a)
#endif
#define FAR
#define CONST	const

#define KEY_QUERY_VALUE			(0x0001)
#define KEY_SET_VALUE			(0x0002)
#define KEY_CREATE_SUB_KEY		(0x0004)
#define KEY_ENUMERATE_SUB_KEYS	(0x0008)
#define KEY_NOTIFY				(0x0010)
#define KEY_CREATE_LINK			(0x0020)
#define KEY_WOW64_64KEY			(0x0100)
#define KEY_WOW64_32KEY			(0x0200)

#define HKEY_CLASSES_ROOT           (( HKEY ) 0x80000000 )
#define HKEY_CURRENT_USER           (( HKEY ) 0x80000001 )
#define HKEY_LOCAL_MACHINE          (( HKEY ) 0x80000002 )
#define HKEY_USERS                  (( HKEY ) 0x80000003 )
#define HKEY_PERFORMANCE_DATA       (( HKEY ) 0x80000004 )
#define REG_OPTION_NON_VOLATILE     (0x00000000L)

#define STANDARD_RIGHTS_READ             (READ_CONTROL)
#define STANDARD_RIGHTS_WRITE            (READ_CONTROL)
#define STANDARD_RIGHTS_EXECUTE          (READ_CONTROL)
#define STANDARD_RIGHTS_ALL              (0x001F0000L)

#define ES_CONTINUOUS		0x80000000
#define ES_DISPLAY_REQUIRED	0x00000002
#define ES_SYSTEM_REQUIRED	0x00000001

#define SPECIFIC_RIGHTS_ALL              (0x0000FFFFL)

#define KEY_READ                ((STANDARD_RIGHTS_READ       |\
                                  KEY_QUERY_VALUE            |\
                                  KEY_ENUMERATE_SUB_KEYS     |\
                                  KEY_NOTIFY)                 \
                                  &                           \
                                 (~SYNCHRONIZE))


#define KEY_WRITE               ((STANDARD_RIGHTS_WRITE      |\
                                  KEY_SET_VALUE              |\
                                  KEY_CREATE_SUB_KEY)         \
                                  &                           \
                                 (~SYNCHRONIZE))

#define KEY_EXECUTE             ((KEY_READ)                   \
                                  &                           \
                                 (~SYNCHRONIZE))

#define KEY_ALL_ACCESS          ((STANDARD_RIGHTS_ALL        |\
                                  KEY_QUERY_VALUE            |\
                                  KEY_SET_VALUE              |\
                                  KEY_CREATE_SUB_KEY         |\
                                  KEY_ENUMERATE_SUB_KEYS     |\
                                  KEY_NOTIFY                 |\
                                  KEY_CREATE_LINK)            \
                                  &                           \
                                 (~SYNCHRONIZE))


#define NO_ERROR					0
#define ERROR_SUCCESS				0L
#define ERROR_INVALID_FUNCTION      1L
#define ERROR_FILE_NOT_FOUND        2L
#define ERROR_PATH_NOT_FOUND        3L
#define ERROR_TOO_MANY_OPEN_FILES   4L
#define ERROR_ACCESS_DENIED			5L
#define ERROR_INVALID_HANDLE        6L
#define ERROR_ARENA_TRASHED			7L
#define ERROR_NOT_ENOUGH_MEMORY     8L
#define ERROR_INVALID_BLOCK			9L
#define ERROR_BAD_ENVIRONMENT       10L
#define ERROR_BAD_FORMAT			11L
#define ERROR_INVALID_ACCESS        12L
#define ERROR_INVALID_DATA			13L
#define ERROR_OUTOFMEMORY			14L
#define ERROR_INVALID_DRIVE			15L
#define ERROR_WRITE_PROTECT			19L
#define ERROR_NOT_READY				21L
#define ERROR_READ_FAULT			30L
#define ERROR_GEN_FAILURE			31L
#define ERROR_HANDLE_DISK_FULL		39L
#define ERROR_CANNOT_MAKE			82L
#define ERROR_OPEN_FAILED			110L
#define ERROR_DISK_FULL				112L

#define SEVERITY_SUCCESS    0
#define SEVERITY_ERROR      1

/* Global Memory Flags */
#define GMEM_FIXED          0x0000
#define GMEM_MOVEABLE       0x0002
#define GMEM_NOCOMPACT      0x0010
#define GMEM_NODISCARD      0x0020
#define GMEM_ZEROINIT       0x0040
#define GMEM_MODIFY         0x0080
#define GMEM_DISCARDABLE    0x0100
#define GMEM_NOT_BANKED     0x1000
#define GMEM_SHARE          0x2000
#define GMEM_DDESHARE       0x2000
#define GMEM_NOTIFY         0x4000
#define GMEM_LOWER          GMEM_NOT_BANKED
#define GMEM_VALID_FLAGS    0x7F72
#define GMEM_INVALID_HANDLE 0x8000

#define GHND                (GMEM_MOVEABLE | GMEM_ZEROINIT)
#define GPTR                (GMEM_FIXED | GMEM_ZEROINIT)

#define SECTION_QUERY       0x0001
#define SECTION_MAP_WRITE   0x0002
#define SECTION_MAP_READ    0x0004
#define SECTION_MAP_EXECUTE 0x0008
#define SECTION_EXTEND_SIZE 0x0010
#define STANDARD_RIGHTS_REQUIRED         (0x000F0000L)

#define SECTION_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED|SECTION_QUERY|\
                            SECTION_MAP_WRITE |      \
                            SECTION_MAP_READ |       \
                            SECTION_MAP_EXECUTE |    \
                            SECTION_EXTEND_SIZE)

#define FILE_MAP_COPY       SECTION_QUERY
#define FILE_MAP_WRITE      SECTION_MAP_WRITE
#define FILE_MAP_READ       SECTION_MAP_READ
#define FILE_MAP_ALL_ACCESS SECTION_ALL_ACCESS

#define OF_READ             0x00000000
#define OF_WRITE            0x00000001
#define OF_READWRITE        0x00000002
#define OF_SHARE_COMPAT     0x00000000
#define OF_SHARE_EXCLUSIVE  0x00000010
#define OF_SHARE_DENY_WRITE 0x00000020
#define OF_SHARE_DENY_READ  0x00000030
#define OF_SHARE_DENY_NONE  0x00000040
#define OF_PARSE            0x00000100
#define OF_DELETE           0x00000200
#define OF_VERIFY           0x00000400
#define OF_CANCEL           0x00000800
#define OF_CREATE           0x00001000
#define OF_PROMPT           0x00002000
#define OF_EXIST            0x00004000
#define OF_REOPEN           0x00008000

struct TONEMAP_PARAMETERS
	{
		WORD	wGamma; // 1/1000 units (0.250 to 4.000)
		BYTE	byBrightness; // 0-255.  0=darkest, 128=neutral, 255=brightest
		BYTE	byContrast;	// 0-255.  0=least contrast, 128=neutral contrast, 255=most contrast.
		WORD	wHighlight; // range depends on dwGammaMaxEntryValue in SCANNER_PARAMETERS
		WORD	wShadow; // range depends on dwGammaMaxEntryValue in SCANNER_PARAMETERS
	};

struct SCANNER_PARAMETERS {
	DWORD dwLeft;			
	DWORD dwTop;			
	DWORD dwWidth;		
	DWORD dwLength;		
	DWORD dwScanMode;
	DWORD dwScanOptions;	
	BYTE  byBitsPerChannel;
	BYTE  byEdgeEraseTop;
	WORD  wXResolution;
	WORD  wYResolution;
	WORD  wGreyChannel;
	WORD  wScanMethod;
	// future expansion
	DWORD dwBWThreshold;
	WORD wFeedEdge;
	struct TONEMAP_PARAMETERS tonemap;  // 2 DWORD
	char cSharpSmoothLevel;
	BYTE byNoiseRemovalLevel;
	BYTE byContentType;
	BYTE byBlankPageSensitivity;
	BYTE byEdgeEraseBottom;
	BYTE byFillColor;
	DWORD dwPageWidth;
	DWORD dwPageHeight;
	BYTE byAutoCropType;
	BYTE byACDmapping;
	BYTE byACDsensitivity;
	BYTE byEdgeEraseLeft;
	BYTE byEdgeEraseRight;
	BYTE byAutoOrientQualityMode;
	BYTE byACDsensitivityBW;
	BYTE byAutoOrientLanguage;
	BYTE byAutoXclusionTop;
	BYTE byAutoXclusionLeft;
	BYTE byAutoXclusionRight;
	BYTE byAutoXclusionBottom;
	BYTE byStoreJobRequest;
	BYTE bymultipick;
	BYTE byStoredJobName[64];
	BYTE byStoredJobPIN[10];
	WORD wReserved[ 5 ];
};


struct BUFFER_INFO {
	DWORD dwPixelWidth;
	DWORD dwPixelHeight;
	DWORD dwBytesPerLine;
	// future expansion
	DWORD dwReserved[10];
};

typedef struct _SCAN_SOURCE 
{
	WORD 	scanMethod;
	DWORD	adfOptions;
	WORD	adfInputTray;
	WORD	xpaTemplate;
} SCAN_SOURCE;	

#endif
