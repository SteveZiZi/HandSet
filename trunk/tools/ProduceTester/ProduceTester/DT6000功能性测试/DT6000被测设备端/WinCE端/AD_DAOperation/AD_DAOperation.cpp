#include <Windows.h>
#include "AD_DAOperation.h"

//AD/DA驱动句柄
static HANDLE      g_hADDAFile=INVALID_HANDLE_VALUE;
#define   IOCTL_SET_ADC_CHANNEL                 2055
#define   IOCTL_SET_DAC_CHANNEL                 2056
#define   IOCTL_ADC_READ                        0x00000002

BOOL WINAPI DllMain(HANDLE hinstDLL, DWORD dwReason, LPVOID lpvReserved)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
        DisableThreadLibraryCalls((HMODULE)hinstDLL);
        g_hADDAFile=INVALID_HANDLE_VALUE;
        return TRUE;
    case  DLL_PROCESS_DETACH:
        if (g_hADDAFile !=INVALID_HANDLE_VALUE)
        {
            CloseHandle(g_hADDAFile);
            g_hADDAFile=INVALID_HANDLE_VALUE;
        }
        return TRUE;
    }

    return FALSE;
}

void epcAdDaCloseDriver()
{
    if (g_hADDAFile !=INVALID_HANDLE_VALUE)
    {
        CloseHandle(g_hADDAFile);
        g_hADDAFile=INVALID_HANDLE_VALUE;
    }
}


BOOL epcAdDaOpenDriver()
{
    if (g_hADDAFile !=INVALID_HANDLE_VALUE)
    {
        return TRUE;
    }

    g_hADDAFile=CreateFile(TEXT("ADC1:"),                        //打开ADC驱动
        GENERIC_READ | GENERIC_WRITE, 
        0, NULL, OPEN_EXISTING, 0, 0);

    if (g_hADDAFile == INVALID_HANDLE_VALUE) {       
        return FALSE; 
    }

    return TRUE;
}

BOOL epcAdGetSampleValue(DWORD *pValueArray,DWORD dwBeginChannel,DWORD dwEndChannel)
{
    BOOL bRet=TRUE;
    DWORD i,dwReturn;
    UCHAR ucChannel;

    for(i=dwBeginChannel;i<=dwEndChannel;i++)
    {
        ucChannel=(UCHAR)i;
        DeviceIoControl(g_hADDAFile,IOCTL_ADC_READ,(PBYTE)&ucChannel,
            sizeof(ucChannel),&pValueArray[i-dwBeginChannel],
            sizeof(DWORD),&dwReturn,NULL);      
    }    
    return TRUE;
}

BOOL epcDASetOutputValue(DWORD *pValueArray,DWORD dwBeginChannel,DWORD dwEndChannel)
{
    BOOL bRet=TRUE;
    DWORD i,dwReturn;
    DWORD ucChannel;
    UINT16 usTemp;
    for(i=dwBeginChannel;i<=dwEndChannel;i++)
    {
        ucChannel=(DWORD )i;
        DeviceIoControl(g_hADDAFile,IOCTL_SET_DAC_CHANNEL,(PBYTE)&ucChannel,
            sizeof(ucChannel),NULL,0,&dwReturn,NULL);

        usTemp=(UINT16)pValueArray[i-dwBeginChannel];
        WriteFile(g_hADDAFile,&usTemp,sizeof(usTemp),&dwReturn,NULL);
       
    }    
    return TRUE;
}

