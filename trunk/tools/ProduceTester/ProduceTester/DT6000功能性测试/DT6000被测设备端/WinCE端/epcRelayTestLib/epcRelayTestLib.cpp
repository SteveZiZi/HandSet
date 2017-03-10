#include <windows.h>
#include "ceddkex.h"
#include "pm.h"
#include "windev.h"
#include "epcRelayTestLib.h"

#define IOCTL_GPIO_SETBIT       \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0300, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_GPIO_CLRBIT       \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0301, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_GPIO_UPDATEBIT    \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0302, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_GPIO_GETBIT       \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0303, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_GPIO_SETMODE      \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0304, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_GPIO_GETMODE      \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0305, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_GPIO_GETIRQ       \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0306, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_GPIO_SET_POWER_STATE  \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0307, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_GPIO_GET_POWER_STATE  \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0308, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_GPIO_SET_DEBOUNCE_TIME  \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0309, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_GPIO_GET_DEBOUNCE_TIME  \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0310, METHOD_BUFFERED, FILE_ANY_ACCESS)


HANDLE          g_hRelayFile = INVALID_HANDLE_VALUE;
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call,LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(hModule);
    UNREFERENCED_PARAMETER(lpReserved);

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls((HMODULE)hModule);
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        if (g_hRelayFile != INVALID_HANDLE_VALUE) 
        {
            CloseHandle(g_hRelayFile);                                  /*  ¹Ø±Õ LED Çý¶¯               */
            g_hRelayFile = INVALID_HANDLE_VALUE;
        }
        break;
    }
    return TRUE;
}

BOOL epcRelayInit()
{
    if (g_hRelayFile == INVALID_HANDLE_VALUE)
    {
        g_hRelayFile = CreateFile(L"GIO1:", GENERIC_READ | GENERIC_WRITE, 
            FILE_SHARE_READ | FILE_SHARE_WRITE, 
            NULL, OPEN_EXISTING, 0, 0);
        if (g_hRelayFile == INVALID_HANDLE_VALUE)
        {
            return FALSE;
        }

        //Çå¿ÕGPIO×´Ì¬
        DWORD dwData =105;
        DeviceIoControl(g_hRelayFile, IOCTL_GPIO_CLRBIT, &dwData, sizeof(dwData), NULL, 0, NULL, NULL);
        dwData =106;
        DeviceIoControl(g_hRelayFile, IOCTL_GPIO_CLRBIT, &dwData, sizeof(dwData), NULL, 0, NULL, NULL);
        dwData =101;
        DeviceIoControl(g_hRelayFile, IOCTL_GPIO_CLRBIT, &dwData, sizeof(dwData), NULL, 0, NULL, NULL);
        dwData =102;
        DeviceIoControl(g_hRelayFile, IOCTL_GPIO_CLRBIT, &dwData, sizeof(dwData), NULL, 0, NULL, NULL);
    }

    return TRUE;
}

BOOL epcRelayOn(DWORD dwIndex)
{
    dwIndex = 106-dwIndex;
    DWORD outdata = 0;
    DeviceIoControl(g_hRelayFile, IOCTL_GPIO_SETBIT, &dwIndex, sizeof(dwIndex), 
        &outdata, sizeof(outdata), NULL, NULL);

    return TRUE;
}

BOOL epcRelayOff(DWORD dwIndex)
{
    dwIndex = 106-dwIndex;
    DWORD outdata = 0;
    DeviceIoControl(g_hRelayFile, IOCTL_GPIO_CLRBIT, &dwIndex, sizeof(dwIndex), 
        &outdata, sizeof(outdata), NULL, NULL);

    return TRUE;
}

DWORD epcGetOpenInputState(DWORD dwIndex)
{
    dwIndex = 102-dwIndex;
    DWORD outdata = 0;
    DeviceIoControl(g_hRelayFile, IOCTL_GPIO_GETBIT, &dwIndex, sizeof(dwIndex), \
        &outdata, sizeof(outdata), NULL, NULL);
    if (outdata)
    {
        return 1;
    }

    return 0;
}