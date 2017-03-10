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
/*++
THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.

Module Name:  

Abstract:

    Platform dependent PCMCIA definitions for Intel Ti1250 compatible socket
    controller.

Notes: 
--*/
#ifndef __CREGEDIT_H_
#define __CREGEDIT_H_
#include <winreg.h>
#include <ddkreg.h>

class CRegistryEdit
{
public:
    CRegistryEdit( LPCTSTR RegPath )
    {
        m_hDevKey = NULL;
        if( RegPath )
        {
            m_hDevKey = OpenDeviceKey( RegPath );
        }
    }
    
    CRegistryEdit( HKEY hKey, LPCTSTR pszRegPath )
    {
        m_hDevKey = NULL;
        if( pszRegPath )
        {
            if( RegOpenKeyEx( hKey, pszRegPath, 0, 0, &m_hDevKey ) != ERROR_SUCCESS )
            {
                m_hDevKey = NULL;
            }
        }
    }
    
    CRegistryEdit( HKEY hKey,
                           LPCTSTR pszRegPath,
                           PDWORD pdwDisposition )
    {
        m_hDevKey = NULL;
        if( pszRegPath )
        {
            if( RegCreateKeyEx( hKey,
                                pszRegPath,
                                0,
                                NULL,
                                0,
                                0,
                                NULL,
                                &m_hDevKey,
                                pdwDisposition ) != ERROR_SUCCESS )
            {
                m_hDevKey = NULL;
            }
        }
    }
    
    ~CRegistryEdit( void )
    {
        if( m_hDevKey )
        {
            RegCloseKey( m_hDevKey );
        }
    }
    
    BOOL    GetRegValue( LPCWSTR lpcName, __out_bcount(dwDataLen) LPBYTE lpData, DWORD dwDataLen )
    {
        DWORD dwType = 0;
        LONG status = ::RegQueryValueEx( m_hDevKey,
                                           lpcName,
                                           NULL,
                                           &dwType,
                                           lpData,
                                           &dwDataLen );
        return ( status == ERROR_SUCCESS );
    }

    BOOL    RegQueryValueEx( LPCWSTR lpcName,
                             PDWORD pdwType,
                             __out_bcount_opt(*pdwDataLen) LPBYTE lpData,
                             PDWORD pdwDataLen )
    {
        LONG status = ::RegQueryValueEx( m_hDevKey,
                                           lpcName,
                                           NULL,
                                           pdwType,
                                           lpData,
                                           pdwDataLen );
        return ( status == ERROR_SUCCESS );
    };
    
    BOOL    RegSetValueEx( LPCWSTR lpcName,
                           DWORD dwType,
                           __in_bcount(dwDataLen) LPBYTE lpData,
                           DWORD dwDataLen )

    {
        LONG status = ::RegSetValueEx( m_hDevKey,
                                         lpcName,
                                         NULL,
                                         dwType,
                                         lpData,
                                         dwDataLen );
        return ( status == ERROR_SUCCESS );
    }
    
    BOOL IsKeyOpened()
    {
        return ( m_hDevKey != NULL );
    }
    
    HKEY GetHKey()
    {
        return m_hDevKey;
    }
    
    BOOL RegQueryInfoKey( LPWSTR lpClass,
                          LPDWORD lpcbClass,
                          LPDWORD lpReserved,
                          LPDWORD lpcSubKeys,
                          LPDWORD lpcbMaxSubKeyLen,
                          LPDWORD lpcbMaxClassLen,
                          LPDWORD lpcValues,
                          LPDWORD lpcbMaxValueNameLen,
                          LPDWORD lpcbMaxValueLen,
                          LPDWORD lpcbSecurityDescriptor,
                          PFILETIME lpftLastWriteTime )
    {
        return ( ::RegQueryInfoKey( m_hDevKey,
                                    lpClass,
                                    lpcbClass,
                                    lpReserved,
                                    lpcSubKeys,
                                    lpcbMaxSubKeyLen,
                                    lpcbMaxClassLen,
                                    lpcValues,
                                    lpcbMaxValueNameLen,
                                    lpcbMaxValueLen,
                                    lpcbSecurityDescriptor,
                                    lpftLastWriteTime ) == ERROR_SUCCESS );
    }
    
    BOOL RegEnumKeyEx( DWORD dwIndex,
                       __out_ecount(*lpcbName) LPWSTR lpName,
                       LPDWORD lpcbName,
                       LPDWORD lpReserved,
                       LPWSTR lpClass,
                       LPDWORD lpcbClass,
                       PFILETIME lpftLastWriteTime )
    {
        PREFAST_SUPPRESS(386, "Buffer overrun while writing to 'lpClass': the writable size is 'lpcbClass' bytes, but 'lpcbClass*2' bytes may be written.");
        return ( ::RegEnumKeyEx( m_hDevKey,
                                 dwIndex,
                                 lpName,
                                 lpcbName,
                                 lpReserved,
                                 lpClass,
                                 lpcbClass,
                                 lpftLastWriteTime ) == ERROR_SUCCESS );
    }
    
    BOOL RegEnumValue( DWORD dwIndex,
                       LPWSTR lpValueName,
                       LPDWORD lpcbValueName,
                       LPDWORD lpReserved,
                       LPDWORD lpType,
                       LPBYTE lpData,
                       LPDWORD lpcbData )
    {
        return ( ::RegEnumValue( m_hDevKey,
                                 dwIndex,
                                 lpValueName,
                                 lpcbValueName,
                                 lpReserved,
                                 lpType,
                                 lpData,
                                 lpcbData ) == ERROR_SUCCESS );
    }
    
    BOOL RegDeleteValue( LPCWSTR lpValueName )
    {
        return ( ::RegDeleteValue( m_hDevKey, lpValueName ) == ERROR_SUCCESS );
    }

    BOOL    RegGetList( LPCWSTR ValName, PDWORD pNum, DWORD List[] )
    {
        union {
            WCHAR ValList[2 * DEVKEY_LEN];

            // ValList is cast to a DWORD* so we put it in a union
            // with a DWORD to guarantee proper alignment;
            DWORD dwToForceFourByteAlignment;
        };
        
        DWORD ValLen = sizeof( ValList );
        DWORD ValType;
        if( pNum == NULL || List == NULL )
        {
            return FALSE;
        }

        DWORD MaxItems = *pNum;
        DWORD Status = ::RegQueryValueEx( m_hDevKey,
                                          ValName,
                                          NULL,
                                          &ValType,
                                          ( PBYTE ) ValList,
                                          &ValLen );

        *pNum = 0;

        if( Status != ERROR_SUCCESS )
        {
            return FALSE;
        }

        if( ValType == REG_MULTI_SZ )
        {
            // Registry entry is multiple strings
            DWORD i;
            PWCHAR bp;

            for( bp = ValList, i = 0; i < MaxItems; i++ )
            {
                PWCHAR ep;

                if( *bp == ( WCHAR ) '\0' )
                    break;

                List[i] = wcstoul( bp, &ep, 16 );

                if( bp == ep )
                    break;

                ( *pNum )++;
                bp = ++ep;
            }

            if( ( i == MaxItems ) && ( *bp != ( WCHAR ) '\0' ) )
            {
                // There are more strings in the registry entry than there are locations on the List
                return FALSE;
            }
        }
        else if( ValType == REG_DWORD )
        {
            // Registry entry is DWORD
            List[0] = *( DWORD * ) ValList;
            *pNum = 1;
        }
        else
        {
            // Registry entry is some other type; ignore
            return FALSE;
        }

        return TRUE;
    }

    BOOL    RegSetList( LPCWSTR ValName, DWORD Num, DWORD List[] )
    {
        WCHAR ValList[2 * DEVKEY_LEN];
        DWORD Status;

        if( Num == 0 )
        {
            return TRUE;
        }
        else if( Num == 1 )
        {
            Status = ::RegSetValueEx( m_hDevKey,
                                      ValName,
                                      0,
                                      REG_DWORD,
                                      ( PBYTE ) List,
                                      sizeof( List[0] ) );
        }
        else
        {
            DWORD i;
            PWCHAR bp = ValList;
            WCHAR Item[11];          // Largest number of characters produced by _ultow is 10 + termination character
            DWORD ItemSize, Size = 0;

            for( i = 0; i < Num; i++ )
            {
                _ultow( List[i], Item, 16 );
                ItemSize = wcslen( Item ) + 1;

                Size += ItemSize;
                if( Size < ItemSize || Size >= 2 * DEVKEY_LEN )
                {
                    // List too big
                    return FALSE;
                }

                wcscpy( bp, Item );
                bp += ItemSize;
            }

            *bp = ( WCHAR ) '\0';
            Size++;

            Status = ::RegSetValueEx( m_hDevKey,
                                      ValName,
                                      0,
                                      REG_MULTI_SZ,
                                      ( PBYTE ) ValList,
                                      Size * sizeof( WCHAR ) );
        }

        if( Status == ERROR_SUCCESS )
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }

private:
    HKEY    m_hDevKey;
};

#endif
