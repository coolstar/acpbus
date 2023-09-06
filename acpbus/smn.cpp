#include "driver.h"
#include <acpiioct.h>

NTSTATUS
SMNQuery(
    _In_ PVOID _context,
    _In_ UINT32 Addr,
    _In_ PUINT32 PData,
    _In_ BOOL Write
)
{
    if (!_context)
        return STATUS_NO_SUCH_DEVICE;

    PPDO_DEVICE_DATA devData = (PPDO_DEVICE_DATA)_context;
    if (!devData->FdoContext) {
        return STATUS_NO_SUCH_DEVICE;
    }

    WDFDEVICE FxDevice = devData->FdoContext->WdfDevice;

    NTSTATUS status = STATUS_ACPI_NOT_INITIALIZED;
    WDFMEMORY inputBufferMemory = NULL;
    WDFMEMORY outputBufferMemory = NULL;
    PACPI_EVAL_INPUT_BUFFER_COMPLEX_EX inputBuffer = NULL;
    PACPI_METHOD_ARGUMENT inputArgument = NULL;
    WDF_MEMORY_DESCRIPTOR inputBufferMemoryDescriptor;
    WDF_MEMORY_DESCRIPTOR outputBufferMemoryDescriptor;

    ULONG inputBufferSize =
        (ULONG)(
            FIELD_OFFSET(ACPI_EVAL_INPUT_BUFFER_COMPLEX_EX, Argument) +
            ACPI_METHOD_ARGUMENT_LENGTH(sizeof(ULONG)) +
            (Write ? ACPI_METHOD_ARGUMENT_LENGTH(sizeof(ULONG)) : 0)
            );

    WDF_OBJECT_ATTRIBUTES attributes;
    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
    attributes.ParentObject = FxDevice;

    status = WdfMemoryCreate(&attributes,
        PagedPool,
        ACPBUS_POOL_TAG,
        inputBufferSize,
        &inputBufferMemory,
        (PVOID*)&inputBuffer);
    if (!NT_SUCCESS(status)) {
        ACPBusPrint(DEBUG_LEVEL_ERROR, DBG_IOCTL, 
            "Failed to create input buffer\n");
        goto end;
    }
    RtlZeroMemory(inputBuffer, inputBufferSize);

    inputBuffer->Signature = ACPI_EVAL_INPUT_BUFFER_COMPLEX_SIGNATURE_V1_EX;
    status = RtlStringCchPrintfA(
        inputBuffer->MethodName,
        sizeof(inputBuffer->MethodName),
        Write ? "SMNW" : "SMNR"
    );
    if (!NT_SUCCESS(status)) {
        ACPBusPrint(DEBUG_LEVEL_ERROR, DBG_IOCTL, 
            "Failed to write method name\n");
        goto end;
    }

    inputBuffer->Size = inputBufferSize;
    inputBuffer->ArgumentCount = Write ? 2 : 1;
    inputArgument = inputBuffer->Argument;

    //arg 0
    ACPI_METHOD_SET_ARGUMENT_INTEGER(inputArgument, Addr);

    if (Write) {
        inputArgument = ACPI_METHOD_NEXT_ARGUMENT(inputArgument);
        ACPI_METHOD_SET_ARGUMENT_INTEGER(inputArgument, *PData);
    }

    WDF_MEMORY_DESCRIPTOR_INIT_HANDLE(&inputBufferMemoryDescriptor,
        inputBufferMemory,
        0);

    PACPI_EVAL_OUTPUT_BUFFER outputBuffer;
    size_t outputArgumentBufferSize = 32;
    size_t outputBufferSize = FIELD_OFFSET(ACPI_EVAL_OUTPUT_BUFFER, Argument) + outputArgumentBufferSize;

    status = WdfMemoryCreate(&attributes,
        PagedPool,
        ACPBUS_POOL_TAG,
        outputBufferSize,
        &outputBufferMemory,
        (PVOID*)&outputBuffer);
    if (!NT_SUCCESS(status)) {
        ACPBusPrint(DEBUG_LEVEL_ERROR, DBG_IOCTL, 
            "Failed to create output buffer\n");
        goto end;
    }

    RtlZeroMemory(outputBuffer, outputBufferSize);
    WDF_MEMORY_DESCRIPTOR_INIT_HANDLE(&outputBufferMemoryDescriptor, outputBufferMemory, NULL);

    status = WdfIoTargetSendIoctlSynchronously(WdfDeviceGetIoTarget(FxDevice),
        NULL,
        IOCTL_ACPI_EVAL_METHOD_EX,
        &inputBufferMemoryDescriptor,
        &outputBufferMemoryDescriptor,
        NULL,
        NULL);

    if (!NT_SUCCESS(status)) {
        ACPBusPrint(DEBUG_LEVEL_ERROR, DBG_IOCTL,
            "Failed ioctl 0x%x\n", status);
        goto end;
    }

    ACPBusPrint(DEBUG_LEVEL_ERROR, DBG_IOCTL, 
        "Ioctl succeeded for %d\n", Write);

    if (!Write) {
        if (outputBuffer->Signature != ACPI_EVAL_OUTPUT_BUFFER_SIGNATURE) {
            goto end;
        }

        if (outputBuffer->Count < 1) {
            goto end;
        }

        UINT32 Data = 0;
        if (outputBuffer->Argument[0].DataLength >= 4) {
            Data = *(UINT32*)outputBuffer->Argument->Data;
        }
        else {
            ACPBusPrint(DEBUG_LEVEL_ERROR, DBG_IOCTL, 
                "Invalid SMN Data length for read: %d\n", outputBuffer->Argument[0].DataLength);
            status = STATUS_ACPI_INVALID_DATA;
            goto end;
        }
        if (PData) {
            *PData = Data;
        }
        else {
            status = STATUS_ACPI_INVALID_ARGUMENT;
        }
    }

end:
    if (inputBufferMemory != NULL) {
        WdfObjectDelete(inputBufferMemory);
    }

    if (outputBufferMemory != NULL) {
        WdfObjectDelete(outputBufferMemory);
    }
    return status;
}