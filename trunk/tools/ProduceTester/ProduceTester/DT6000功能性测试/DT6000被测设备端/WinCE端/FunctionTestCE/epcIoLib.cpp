#include "stdafx.h"
#include <assert.h>
#include "epcIoLib.h"


static  HINSTANCE   g_hInstGPIO=NULL;
static int (*pfn_epcIoRead)(DWORD *dwPinState)=NULL;
static int (*pfn_epcIoWrite)(DWORD dwPinState)=NULL;


BOOL LoadEpcIoLibrary()
{
    if (g_hInstGPIO !=NULL)
    {
        return TRUE;
    }

    HINSTANCE h=LoadLibrary(L"\\windows\\epcIoLib.dll");
    if (h==NULL)
    {
        return FALSE;
    }

    pfn_epcIoRead=(int (*)(DWORD *) )GetProcAddress(h,L"epcIoRead");
    pfn_epcIoWrite=(int (*)(DWORD) )GetProcAddress(h,L"epcIoWrite");
    if (pfn_epcIoRead==NULL ||pfn_epcIoWrite ==NULL)
    {
        goto fail;
    }
    g_hInstGPIO=h;
    return TRUE;

fail:
    if (h!=NULL)
    {
        CloseHandle(h);
    }
    g_hInstGPIO=NULL;
    return FALSE;
}

int epcIoRead(DWORD *dwPinState)
{
    return pfn_epcIoRead(dwPinState);
}

int epcIoWrite(DWORD dwPinState)
{
    return pfn_epcIoWrite(dwPinState);
}