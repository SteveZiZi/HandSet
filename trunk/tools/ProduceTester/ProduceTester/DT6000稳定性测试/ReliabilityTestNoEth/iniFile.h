#ifndef __INI_FILE_H
#define __INI_FILE_H





#ifdef GetPrivateProfileString
#undef GetPrivateProfileString
#endif

DWORD WINAPI GetPrivateProfileString( LPCWSTR lpAppName, LPCWSTR lpKeyName,   LPCWSTR lpDefault,
                                        LPWSTR lpReturnedString,  DWORD nSize, LPCWSTR lpFileName );

#ifdef GetPrivateProfileInt
#undef GetPrivateProfileInt
#endif
UINT WINAPI GetPrivateProfileInt(LPCTSTR lpSectionName,LPCTSTR lpKeyName,  int nDefault,LPCTSTR lpFileName);
double WINAPI GetPrivateProfileDouble(LPCTSTR lpSectionName,LPCTSTR lpKeyName, double dDefault,LPCTSTR lpFileName);


#ifdef WritePrivateProfileString
#undef WritePrivateProfileString
#endif
BOOL WINAPI WritePrivateProfileString(LPCWSTR lpSectionName,LPCWSTR lpKeyName,LPCWSTR lpString,LPCWSTR lpFileName);
BOOL WINAPI WritePrivateProfileInt(LPCWSTR lpSectionName,LPCWSTR lpKeyName,int iValue,LPCWSTR lpFileName);
BOOL WINAPI WritePrivateProfileDouble(LPCWSTR lpSectionName,LPCWSTR lpKeyName,double dValue,LPCWSTR lpFileName);

#endif