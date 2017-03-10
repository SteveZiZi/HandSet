// USBFunctionSwitch.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#include "svsutil.hxx"
#include "usbfnioctl.h"

#include "usb_function_switch.h"

HANDLE GetUfnController()
{
	int		iErr;
	HANDLE	hUfn = NULL;
	HANDLE	hf;
	BYTE	bGuidBuffer[sizeof(GUID) + 4];
	LPGUID	pGuidBus;
	LPCTSTR	pszBusGuid = _T("E2BDC372-598F-4619-BC50-54B3F7848D35");
	

	DEVMGR_DEVICE_INFORMATION	devInfo;

	// Parse the GUID
	pGuidBus = (LPGUID)bGuidBuffer;
	iErr = _stscanf(pszBusGuid, SVSUTIL_GUID_FORMAT, SVSUTIL_PGUID_ELEMENTS(&pGuidBus));
	if (iErr == 0 || iErr == EOF)
	{
		return INVALID_HANDLE_VALUE;
	}

	// Get a handle to the bus driver
	memset(&devInfo, 0, sizeof(devInfo));
	devInfo.dwSize = sizeof(devInfo);
	hf = FindFirstDevice(DeviceSearchByGuid, pGuidBus, &devInfo);
	if (hf != INVALID_HANDLE_VALUE)
	{
		hUfn = CreateFile(devInfo.szBusName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		CloseHandle(hf);
	}
	
	return hUfn;
}

BOOL ChangeClient(HANDLE hUfn, LPCTSTR pszNewClient)
{
    BOOL	bSuccess = FALSE;
    UFN_CLIENT_NAME	uClientName;

    _tcscpy(uClientName.szName, pszNewClient);
    bSuccess = DeviceIoControl(hUfn, IOCTL_UFN_CHANGE_CURRENT_CLIENT, &uClientName, sizeof(uClientName), NULL, 0, NULL, NULL);

    return bSuccess;
}

BOOL GetClientName(HANDLE hUfn, LPTSTR pszNewClient)
{
    BOOL	bSuccess = FALSE;

    UFN_CLIENT_INFO uClient;
    bSuccess = DeviceIoControl(hUfn, IOCTL_UFN_GET_CURRENT_CLIENT, NULL, 0, &uClient, sizeof(uClient), NULL, NULL);
    if (bSuccess == FALSE) {
        RETAILMSG(1, (_T("Error Code (%d)\r\n"), GetLastError()));
    }
    _tcscpy(pszNewClient, uClient.szName);

    return bSuccess;
}



BOOL SetClientMode(UINT mode)
{
    BOOL dwRet = TRUE;
    UFN_CLIENT_NAME uClientName;
    int  iErr;
    HANDLE hUfn = NULL;
    HANDLE hf;
    BYTE bGuidBuffer[sizeof(GUID) + 4];
    LPGUID pGuidBus;
    LPCTSTR pszBusGuid = _T("E2BDC372-598F-4619-BC50-54B3F7848D35");
    DEVMGR_DEVICE_INFORMATION devInfo;
    LPCTSTR szNewDriverName = NULL;
    static const TCHAR c_szRndisFnName[]       = TEXT("RNDIS");
    static const TCHAR c_szUsbSerialFnName[]   = TEXT("Serial_Class");
    static const TCHAR c_szMassStorageFnName[] = TEXT("Mass_Storage_Class");
    HKEY hKey;
    DWORD dwSize, dwValue;
    TCHAR strValue[MAX_PATH] = {0};
    // save to registry
    if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Drivers\\USB\\FunctionDrivers"), 0, 0, &hKey))
    {
        switch(mode)
        {
        case USB_RNDIS:
            dwSize = (wcslen( c_szRndisFnName)+1)*sizeof(WCHAR);
            RegSetValueEx(hKey, TEXT("DefaultClientDriver"), NULL, REG_SZ, (BYTE *)c_szRndisFnName, dwSize);
            szNewDriverName = c_szRndisFnName;
            RegCloseKey(hKey); 
            // to avoid the system error "Comunicatoin Error", disable the flowing setting:
            if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("ControlPanel\\Comm"), 0, 0, &hKey))
            {
                dwSize = sizeof(DWORD);
                dwValue = 0;
                RegSetValueEx(hKey, TEXT("AutoCnct"), 0, REG_DWORD, (LPBYTE)&dwValue, dwSize);
                RegCloseKey(hKey); 
            }
            break;
        case USB_Serial:
            dwSize = (wcslen( c_szUsbSerialFnName)+1)*sizeof(WCHAR);
            RegSetValueEx(hKey, TEXT("DefaultClientDriver"), NULL, REG_SZ, (BYTE *)c_szUsbSerialFnName, dwSize);
            szNewDriverName = c_szUsbSerialFnName;
            RegCloseKey(hKey); 
            // to avoid the system error "Comunicatoin Error", disable the flowing setting, and now reset it
            if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("ControlPanel\\Comm"), 0, 0, &hKey))
            {
                dwSize = sizeof(DWORD);
                dwValue = 1;
                RegSetValueEx(hKey, TEXT("AutoCnct"), 0, REG_DWORD, (LPBYTE)&dwValue, dwSize);
                RegCloseKey(hKey); 
            }
            break;
        case USB_MSF:
            dwSize = (wcslen( c_szMassStorageFnName)+1)*sizeof(WCHAR);
            RegSetValueEx(hKey, TEXT("DefaultClientDriver"), NULL, REG_SZ, (BYTE *)c_szMassStorageFnName, dwSize);
            szNewDriverName = c_szMassStorageFnName;
            RegCloseKey(hKey); 
            // to avoid the system error "Comunicatoin Error", disable the flowing setting:
            if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("ControlPanel\\Comm"), 0, 0, &hKey))
            {
                dwSize = sizeof(DWORD);
                dwValue = 0;
                RegSetValueEx(hKey, TEXT("AutoCnct"), 0, REG_DWORD, (LPBYTE)&dwValue, dwSize);
                RegCloseKey(hKey); 
            }
            break;
        }

    }

    // Change mode
    // Parse the GUID
    pGuidBus = (LPGUID)bGuidBuffer;
    iErr = _stscanf(pszBusGuid, SVSUTIL_GUID_FORMAT, SVSUTIL_PGUID_ELEMENTS(&pGuidBus));
    if (iErr == 0 || iErr == EOF)
    {
        return FALSE;
    }
    // Get a handle to the bus driver
    memset(&devInfo, 0, sizeof(devInfo));
    devInfo.dwSize = sizeof(devInfo);
    hf = FindFirstDevice(DeviceSearchByGuid, pGuidBus, &devInfo);
    if (hf != INVALID_HANDLE_VALUE)
    {
        hUfn = CreateFile(devInfo.szBusName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if( hUfn != INVALID_HANDLE_VALUE)
        {
            _tcscpy(uClientName.szName, szNewDriverName);
            dwRet = DeviceIoControl(hUfn, IOCTL_UFN_CHANGE_CURRENT_CLIENT, &uClientName, sizeof(uClientName), NULL, 0, NULL, NULL);

            CloseHandle(hUfn);
        }
        CloseHandle(hf);
    }
    return dwRet;
}
