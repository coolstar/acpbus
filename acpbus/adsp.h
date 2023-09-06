#ifndef __ADSP_INTERFACE
#define __ADSP_INTERFACE
#include <hdaudio.h>

//
// The GUID_ACPDSP_BUS_INTERFACE interface GUID
//
// {863DD2AC-5B54-4C57-8487-782F9ADFE8D5}
DEFINE_GUID(GUID_ACPDSP_BUS_INTERFACE,
    0x863dd2ac, 0x5b54, 0x4c57, 0x84, 0x87, 0x78, 0x2f, 0x9a, 0xdf, 0xe8, 0xd5);

typedef struct _TPLG_INFO {
    PVOID sofTplg;
    UINT64 sofTplgSz;
} TPLG_INFO, * PTPLG_INFO;

typedef _Must_inspect_result_ NTSTATUS(*PGET_ADSP_RESOURCES) (_In_ PVOID _context, _Out_ _PCI_BAR* acpBar, PTPLG_INFO tplgInfo, _Out_ BUS_INTERFACE_STANDARD* pciConfig);
typedef _Must_inspect_result_ NTSTATUS(*PDSP_SET_POWER_STATE) (_In_ PVOID _context, _In_ DEVICE_POWER_STATE newPowerState);
typedef _Must_inspect_result_ BOOL(*PADSP_INTERRUPT_CALLBACK)(PVOID context);
typedef _Must_inspect_result_ NTSTATUS(*PREGISTER_ADSP_INTERRUPT) (_In_ PVOID _context, _In_ PADSP_INTERRUPT_CALLBACK callback, _In_ PVOID callbackContext);
typedef _Must_inspect_result_ NTSTATUS(*PUNREGISTER_ADSP_INTERRUPT) (_In_ PVOID _context);
typedef _Must_inspect_result_ NTSTATUS (*PSMNQuery)(_In_ PVOID _context, _In_ UINT32 Addr, _In_ PUINT32 PData, _In_ BOOL Write);

typedef struct _ACPDSP_BUS_INTERFACE
{
    //
    // First we define the standard INTERFACE structure ...
    //
    USHORT                    Size;
    USHORT                    Version;
    PVOID                     Context;
    PINTERFACE_REFERENCE      InterfaceReference;
    PINTERFACE_DEREFERENCE    InterfaceDereference;

    //
    // Then we expand the structure with the ADSP_BUS_INTERFACE stuff.

    PGET_ADSP_RESOURCES           GetResources;
    PDSP_SET_POWER_STATE          SetDSPPowerState;
    PREGISTER_ADSP_INTERRUPT      RegisterInterrupt;
    PUNREGISTER_ADSP_INTERRUPT    UnregisterInterrupt;

    PSMNQuery                     SMNQuery;
} ACPDSP_BUS_INTERFACE, * PACPDSP_BUS_INTERFACE;

#ifndef ADSP_DECL
ACPDSP_BUS_INTERFACE ACPDSP_BusInterface(PVOID Context);
#endif
#endif