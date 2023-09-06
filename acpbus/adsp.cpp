#include "driver.h"
#define ADSP_DECL 1
#include "adsp.h"

NTSTATUS
SMNQuery(
	_In_ PVOID _context,
	_In_ UINT32 Addr,
	_In_ PUINT32 PData,
	_In_ BOOL Write
);

NTSTATUS ADSPGetResources(_In_ PVOID _context, _Out_ _PCI_BAR* acpBar, PTPLG_INFO tplgInfo, _Out_ BUS_INTERFACE_STANDARD* pciConfig) {
	if (!_context)
		return STATUS_NO_SUCH_DEVICE;

	PPDO_DEVICE_DATA devData = (PPDO_DEVICE_DATA)_context;
	if (!devData->FdoContext) {
		return STATUS_NO_SUCH_DEVICE;
	}

	if (acpBar) {
		*acpBar = devData->FdoContext->m_BAR0;
	}

	if (tplgInfo) {
		if (devData->FdoContext->sofTplg) {
			tplgInfo->sofTplg = devData->FdoContext->sofTplg;
			tplgInfo->sofTplgSz = devData->FdoContext->sofTplgSz;
		}
	}

	if (pciConfig) {
		*pciConfig = devData->FdoContext->BusInterface;
	}

	return STATUS_SUCCESS;
}

NTSTATUS ADSPSetPowerState(_In_ PVOID _context, _In_ DEVICE_POWER_STATE powerState) {
	if (!_context)
		return STATUS_NO_SUCH_DEVICE;

	PPDO_DEVICE_DATA devData = (PPDO_DEVICE_DATA)_context;
	if (!devData->FdoContext) {
		return STATUS_NO_SUCH_DEVICE;
	}

	NTSTATUS status = STATUS_SUCCESS;
	if (powerState == PowerDeviceD3) {
		WdfDeviceResumeIdle(devData->FdoContext->WdfDevice);
	} else if (powerState == PowerDeviceD0) {
		status = WdfDeviceStopIdle(devData->FdoContext->WdfDevice, TRUE);
	}
	return status;
}

NTSTATUS ADSPRegisterInterrupt(_In_ PVOID _context, _In_ PADSP_INTERRUPT_CALLBACK callback, _In_ PVOID callbackContext) {
	if (!_context)
		return STATUS_NO_SUCH_DEVICE;

	PPDO_DEVICE_DATA devData = (PPDO_DEVICE_DATA)_context;
	if (!devData->FdoContext) {
		return STATUS_NO_SUCH_DEVICE;
	}

	devData->FdoContext->dspInterruptCallback = callback;
	devData->FdoContext->dspInterruptContext = callbackContext;
	return STATUS_SUCCESS;
}

NTSTATUS ADSPUnregisterInterrupt(_In_ PVOID _context) {
	if (!_context)
		return STATUS_NO_SUCH_DEVICE;

	PPDO_DEVICE_DATA devData = (PPDO_DEVICE_DATA)_context;
	if (!devData->FdoContext) {
		return STATUS_NO_SUCH_DEVICE;
	}

	devData->FdoContext->dspInterruptCallback = NULL;
	devData->FdoContext->dspInterruptContext = NULL;
	return STATUS_SUCCESS;
}

ACPDSP_BUS_INTERFACE ACPDSP_BusInterface(PVOID Context) {
	ACPDSP_BUS_INTERFACE busInterface;
	RtlZeroMemory(&busInterface, sizeof(ACPDSP_BUS_INTERFACE));

	PPDO_DEVICE_DATA devData = (PPDO_DEVICE_DATA)Context;

	busInterface.Size = sizeof(ACPDSP_BUS_INTERFACE);
	busInterface.Version = 1;
	busInterface.Context = Context;
	busInterface.InterfaceReference = WdfDeviceInterfaceReferenceNoOp;
	busInterface.InterfaceDereference = WdfDeviceInterfaceDereferenceNoOp;
	busInterface.GetResources = ADSPGetResources;
	busInterface.SetDSPPowerState = ADSPSetPowerState;
	busInterface.RegisterInterrupt = ADSPRegisterInterrupt;
	busInterface.UnregisterInterrupt = ADSPUnregisterInterrupt;
	busInterface.SMNQuery = SMNQuery;

	return busInterface;
}