#if !defined(_SKLHDAUDBUS_FDO_H_)
#define _SKLHDAUDBUS_FDO_H_

union baseaddr {
    PVOID Base;
    UINT8* baseptr;
};

typedef struct _PCI_BAR {
    union baseaddr Base;
    ULONG Len;
} PCI_BAR, * PPCI_BAR;

#include "adsp.h"

struct _FDO_CONTEXT;
struct _PDO_DEVICE_DATA;

typedef struct _FDO_CONTEXT
{
    WDFDEVICE WdfDevice;

    UINT8 revId;

    PCI_BAR m_BAR0; //required
    BUS_INTERFACE_STANDARD BusInterface; //PCI Bus Interface
    WDFINTERRUPT Interrupt;

    BOOLEAN ControllerEnabled;

    PADSP_INTERRUPT_CALLBACK dspInterruptCallback;
    PVOID dspInterruptContext;
    PVOID sofTplg;
    UINT64 sofTplgSz;
} FDO_CONTEXT, * PFDO_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(FDO_CONTEXT, Fdo_GetContext)

NTSTATUS
Fdo_Create(
	_Inout_ PWDFDEVICE_INIT DeviceInit
);

#endif
