#if !defined(_SKLHDAUDBUS_H_)
#define _SKLHDAUDBUS_H_

#define POOL_ZERO_DOWN_LEVEL_SUPPORT

#pragma warning(disable:4200)  // suppress nameless struct/union warning
#pragma warning(disable:4201)  // suppress nameless struct/union warning
#pragma warning(disable:4214)  // suppress bit field types other than int warning
#include <ntddk.h>
#include <initguid.h>

#include <wdm.h>
#include <wdmguid.h>
#include <wdf.h>
#include <ntintsafe.h>
#include <ntstrsafe.h>
#include <hdaudio.h>
#include <portcls.h>

#include "fdo.h"
#include "buspdo.h"

#define DRIVERNAME "acpbus.sys: "
#define ACPBUS_POOL_TAG 'DPCA'

//
// Helper macros
//

#define DEBUG_LEVEL_ERROR   1
#define DEBUG_LEVEL_INFO    2
#define DEBUG_LEVEL_VERBOSE 3

#define DBG_INIT  1
#define DBG_PNP   2
#define DBG_IOCTL 4

static ULONG ACPBusDebugLevel = 100;
static ULONG ACPBusDebugCatagories = DBG_INIT || DBG_PNP || DBG_IOCTL;

#if 0
#define ACPBusPrint(dbglevel, dbgcatagory, fmt, ...) {          \
    if (ACPBusDebugLevel >= dbglevel &&                         \
        (ACPBusDebugCatagories && dbgcatagory))                 \
		    {                                                           \
        DbgPrint(DRIVERNAME);                                   \
        DbgPrint(fmt, __VA_ARGS__);                             \
		    }                                                           \
}
#else
#define ACPBusPrint(dbglevel, fmt, ...) {                       \
}
#endif
#endif