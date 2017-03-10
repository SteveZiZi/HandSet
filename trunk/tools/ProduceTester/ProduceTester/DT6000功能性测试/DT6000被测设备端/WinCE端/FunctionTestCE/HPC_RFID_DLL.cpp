#include "stdafx.h"
#include "HPC_RFID_DLL.h"

typedef void (*PFN_RfidModulePowerOn)(void);
typedef void (*PFN_RfidModulePowerOff)(void);
typedef BYTE (*PFN_RfidModuleOpenPort)(BYTE ucPort);
typedef BYTE (*PFN_RfidModuleClosePort)(void);
typedef BYTE (*PFN_ISO14443A_ReadCardSn)(BYTE *pucCardSn);

static PFN_RfidModulePowerOn    pfnRfidModulePowerOn    = NULL;
static PFN_RfidModulePowerOff   pfnRfidModulePowerOff   = NULL;
static PFN_RfidModuleOpenPort   pfnRfidModuleOpenPort   = NULL;
static PFN_RfidModuleClosePort  pfnRfidModuleClosePort  = NULL;
static PFN_ISO14443A_ReadCardSn pfnISO14443A_ReadCardSn = NULL;
static HMODULE                  g_hRfidDll;

BOOL LoadRfidDll()
{
    g_hRfidDll=LoadLibrary(L"\\windows\\HPC_RFID_DLL.dll");
    DWORD   dw=GetLastError();
    if (g_hRfidDll == NULL)
    {
        return FALSE;
    }

    pfnRfidModulePowerOn = (PFN_RfidModulePowerOn)
        GetProcAddress(g_hRfidDll,L"RfidModulePowerOn");
    if (pfnRfidModulePowerOn == NULL)
    {
        return FALSE;
    }

    pfnRfidModulePowerOff = (PFN_RfidModulePowerOff)
        GetProcAddress(g_hRfidDll,L"RfidModulePowerOff");
    if (pfnRfidModulePowerOn == NULL)
    {
        return FALSE;
    }
    pfnRfidModuleOpenPort = (PFN_RfidModuleOpenPort)
        GetProcAddress(g_hRfidDll,L"RfidModuleOpenPort");
    if (pfnRfidModulePowerOn == NULL)
    {
        return FALSE;
    }
    pfnRfidModuleClosePort = (PFN_RfidModuleClosePort)
        GetProcAddress(g_hRfidDll,L"RfidModuleClosePort");
    if (pfnRfidModulePowerOn == NULL)
    {
        return FALSE;
    }

    pfnISO14443A_ReadCardSn = (PFN_ISO14443A_ReadCardSn)
        GetProcAddress(g_hRfidDll,L"ISO14443A_ReadCardSn");
    if (pfnRfidModulePowerOn == NULL)
    {
        return FALSE;
    }

    return TRUE;
}

void RfidModulePowerOff()
{
    pfnRfidModulePowerOff();
}

void RfidModulePowerOn()
{
    pfnRfidModulePowerOn();
}

BYTE RfidModuleClosePort()
{
    return pfnRfidModuleClosePort();
}

BYTE RfidModuleOpenPort(BYTE ucPort)
{
    return pfnRfidModuleOpenPort(ucPort);
}

BYTE ISO14443A_ReadCardSn(BYTE *pucCardSn)
{
    return pfnISO14443A_ReadCardSn(pucCardSn);
}

