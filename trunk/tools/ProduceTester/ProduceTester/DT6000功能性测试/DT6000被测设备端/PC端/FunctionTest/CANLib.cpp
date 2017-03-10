#include "stdafx.h"
#include "CanLib.h"

#undef  _T
#define _T(x) x

static HINSTANCE hDLL = NULL;
static DWORD (CALLBACK *pfnVCI_OpenDevice)(DWORD DeviceType,DWORD DeviceInd,DWORD Reserved) = NULL;
static DWORD (CALLBACK *pfnVCI_CloseDevice)(DWORD DeviceType,DWORD DeviceInd)=NULL;
static DWORD (CALLBACK *pfnVCI_InitCAN)(DWORD, DWORD, DWORD, PVCI_INIT_CONFIG) = NULL;
static DWORD (CALLBACK *pfnVCI_ReadErrInfo)(DWORD, DWORD, DWORD, PVCI_ERR_INFO) = NULL;
static DWORD (CALLBACK *pfnVCI_StartCAN)(DWORD, DWORD, DWORD) = NULL;
static DWORD (CALLBACK *pfnVCI_ResetCAN)(DWORD, DWORD, DWORD) = NULL;
static ULONG (CALLBACK *pfnVCI_Transmit)(DWORD, DWORD, DWORD, PVCI_CAN_OBJ, ULONG) = NULL;
static ULONG (CALLBACK *pfnVCI_Receive)(DWORD, DWORD, DWORD, PVCI_CAN_OBJ, ULONG, INT) = NULL;
static DWORD (CALLBACK *pfnVCI_ClearBuffer)(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd);

BOOL VCI_LoadLib()
{
    if (hDLL!=NULL)
    {
        return TRUE;
    }

	hDLL = LoadLibrary(TEXT("ControlCAN.dll"));
	if (!hDLL) return FALSE;

    pfnVCI_OpenDevice=(DWORD (CALLBACK *)(DWORD, DWORD, DWORD))
        ::GetProcAddress(hDLL, _T("VCI_OpenDevice"));
    if (pfnVCI_OpenDevice ==NULL)
    {
        goto Init_FAIL;
    }
    pfnVCI_CloseDevice=(DWORD (CALLBACK *)(DWORD, DWORD))
        ::GetProcAddress(hDLL, _T("VCI_CloseDevice"));
    if(pfnVCI_CloseDevice ==NULL)
    {
        goto Init_FAIL;
    }

    pfnVCI_InitCAN=(DWORD (CALLBACK *)(DWORD, DWORD, DWORD, PVCI_INIT_CONFIG))
        ::GetProcAddress(hDLL, _T("VCI_InitCAN"));
    if(pfnVCI_InitCAN ==NULL)
    {
        goto Init_FAIL;
    }

    pfnVCI_ClearBuffer=(DWORD (CALLBACK *)(DWORD ,DWORD ,DWORD ))
        ::GetProcAddress(hDLL, _T("VCI_ClearBuffer"));
    if(pfnVCI_ClearBuffer ==NULL)
    {
        goto Init_FAIL;
    }

    pfnVCI_ReadErrInfo=(DWORD (CALLBACK *)(DWORD, DWORD, DWORD, PVCI_ERR_INFO))
        ::GetProcAddress(hDLL, _T("VCI_ReadErrInfo"));
    if (pfnVCI_ReadErrInfo ==NULL)
    {
        goto Init_FAIL;
    }
    pfnVCI_StartCAN=(DWORD (CALLBACK *)(DWORD, DWORD, DWORD))
        ::GetProcAddress(hDLL, _T("VCI_StartCAN"));
    if (pfnVCI_StartCAN ==NULL)
    {
        goto Init_FAIL;
    }

    pfnVCI_ResetCAN=(DWORD (CALLBACK *)(DWORD, DWORD, DWORD))
        ::GetProcAddress(hDLL, _T("VCI_ResetCAN"));
    if (pfnVCI_ResetCAN ==NULL)
    {
        goto Init_FAIL;
    }    
    pfnVCI_Transmit= (ULONG (CALLBACK *)(DWORD, DWORD, DWORD, PVCI_CAN_OBJ, ULONG))
        ::GetProcAddress(hDLL, _T("VCI_Transmit"));
    if (pfnVCI_Transmit ==NULL)
    {
        goto Init_FAIL;
    }

    pfnVCI_Receive=(ULONG (CALLBACK *)(DWORD, DWORD, DWORD, PVCI_CAN_OBJ, ULONG, INT))
        ::GetProcAddress(hDLL, _T("VCI_Receive"));
    if (pfnVCI_Receive ==NULL)
    {
        goto Init_FAIL;
    }

	return TRUE;
Init_FAIL:
    CloseHandle(hDLL);
    hDLL=NULL;
    return FALSE;
}

void VCI_UnLoadLib()
{
	if (hDLL) FreeLibrary(hDLL);
    hDLL=NULL;
}

DWORD VCI_OpenDevice(DWORD DeviceType,DWORD DeviceInd,DWORD Reserved)
{
	return pfnVCI_OpenDevice(DeviceType, DeviceInd, Reserved);
}

DWORD VCI_CloseDevice(DWORD DeviceType,DWORD DeviceInd)
{
	if (!pfnVCI_CloseDevice) return 0;
	return pfnVCI_CloseDevice(DeviceType, DeviceInd);
}

DWORD VCI_InitCAN(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, PVCI_INIT_CONFIG pInitConfig)
{
	if (!pfnVCI_InitCAN) return 0;
	return pfnVCI_InitCAN(DeviceType, DeviceInd, CANInd, pInitConfig);
}

DWORD VCI_ReadErrInfo(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,PVCI_ERR_INFO pErrInfo)
{
	if (!pfnVCI_ReadErrInfo) return 0;
	return pfnVCI_ReadErrInfo(DeviceType, DeviceInd, CANInd, pErrInfo);
}

DWORD VCI_StartCAN(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd)
{
	if (!pfnVCI_StartCAN) return 0;
	return pfnVCI_StartCAN( DeviceType, DeviceInd, CANInd);
}

DWORD VCI_ResetCAN(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd)
{
	if (!pfnVCI_ResetCAN) return 0;
	return pfnVCI_ResetCAN(DeviceType, DeviceInd, CANInd);
}

ULONG VCI_Transmit(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,PVCI_CAN_OBJ pSend,ULONG Len)
{
    if (!pfnVCI_Transmit) return 0;
	return pfnVCI_Transmit(DeviceType, DeviceInd, CANInd, pSend, Len);

}

ULONG VCI_Receive(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,PVCI_CAN_OBJ pReceive,ULONG Len,INT WaitTime)
{
	if (!pfnVCI_Receive) return 0;
	return pfnVCI_Receive(DeviceType, DeviceInd, CANInd, pReceive, Len, WaitTime);
}

DWORD VCI_ClearBuffer(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd)
{
    if (!pfnVCI_ClearBuffer) return 0;
    return pfnVCI_ClearBuffer(DeviceType, DeviceInd, CANInd);
}