//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this sample source code is subject to the terms of the Microsoft
// license agreement under which you licensed this sample source code. If
// you did not accept the terms of the license agreement, you are not
// authorized to use this sample source code. For the terms of the license,
// please see the license agreement between you and Microsoft or, if applicable,
// see the LICENSE.RTF on your install media or the root of your tools installation.
// THE SAMPLE SOURCE CODE IS PROVIDED "AS IS", WITH NO WARRANTIES.
//
#ifndef _USBFNIOCTL_H_
#define _USBFNIOCTL_H_

#include <windows.h>
#include <ceddk.h>


// IOCTLs with a Function value between 0x200 and 0x2FF are reserved 
// for the OEM.
#define _UFN_ACCESS_CTL_CODE(_Function)  \
    CTL_CODE(FILE_DEVICE_UNKNOWN, _Function, METHOD_BUFFERED, FILE_ANY_ACCESS)


#define UFN_CLIENT_NAME_MAX_CHARS 128
#define UFN_CLIENT_DESCRIPTION_MAX_CHARS 250

typedef struct _UFN_CLIENT_INFO {
    TCHAR szName[UFN_CLIENT_NAME_MAX_CHARS];
    TCHAR szDescription[UFN_CLIENT_DESCRIPTION_MAX_CHARS];
} UFN_CLIENT_INFO, *PUFN_CLIENT_INFO;

typedef struct _UFN_CLIENT_NAME {
    TCHAR szName[UFN_CLIENT_NAME_MAX_CHARS];
} UFN_CLIENT_NAME, *PUFN_CLIENT_NAME;


// Mdd IOCTLs

// Setup for enumerating the available clients.
// Call with no arguments. Must be called before 
// IOCTL_UFN_ENUMERATE_AVAILABLE_CLIENTS.
#define IOCTL_UFN_ENUMERATE_AVAILABLE_CLIENTS_SETUP _UFN_ACCESS_CTL_CODE(1)

// Enumerate through the available clients.
// Pass UFN_CLIENT_INFO in the out buffer.
#define IOCTL_UFN_ENUMERATE_AVAILABLE_CLIENTS       _UFN_ACCESS_CTL_CODE(2)

// Get information about the current client.
// Pass UFN_CLIENT_INFO in the out buffer.
#define IOCTL_UFN_GET_CURRENT_CLIENT                _UFN_ACCESS_CTL_CODE(3)

// Change the current client.
// Pass _UFN_CLIENT_NAME in the out buffer.
#define IOCTL_UFN_CHANGE_CURRENT_CLIENT             _UFN_ACCESS_CTL_CODE(4)

// Get information about the default client.
// Pass UFN_CLIENT_INFO in the out buffer.
#define IOCTL_UFN_GET_DEFAULT_CLIENT                _UFN_ACCESS_CTL_CODE(5)

// Change the default client.
// Pass _UFN_CLIENT_NAME in the out buffer.
#define IOCTL_UFN_CHANGE_DEFAULT_CLIENT             _UFN_ACCESS_CTL_CODE(6)


// Pdd IOCTLs

typedef struct _UFN_PDD_INFO {
    INTERFACE_TYPE InterfaceType;
    ULONG BusNumber;
    DWORD dwAlignment;
} UFN_PDD_INFO, *PUFN_PDD_INFO;
typedef const UFN_PDD_INFO * PCUFN_PDD_INFO;

// Get desired physical buffer characteristics.
// Pass UFN_PDD_INFO structure as pBufOut.
#define IOCTL_UFN_GET_PDD_INFO                      _UFN_ACCESS_CTL_CODE(101)


#endif // _USBFN_IOCTL_H_

