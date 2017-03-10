#include "stdafx.h"

#include "filemapping.h"


CShareMemory::CShareMemory()
{
	m_hShareMemory = NULL;
	m_pMapBuffer = NULL;
	m_hAccessMutex =NULL;
}

CShareMemory::~CShareMemory()
{
	Close();
}

DWORD CShareMemory::Open(LPCTSTR szMapName,DWORD dwSize)
{
	DWORD dwRet = 1;
	if(szMapName == NULL)
		return 0;

	if(m_hShareMemory)
	{
		Close();
	} 

	// 创建内存映射文件对象
	m_hShareMemory = CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,0,dwSize,szMapName);
	if(!m_hShareMemory)
	{
        Close();
		return 0;
	}
    // 内存映射文件对象已存在
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
        dwRet = 2;
	}
	// 获取内存映射文件指针
	m_pMapBuffer = MapViewOfFile(m_hShareMemory,FILE_MAP_ALL_ACCESS,0,0,0);
	if(!m_pMapBuffer)
	{
		Close();
		return 0;
	}

	// 创建互斥体，用于读写同步   
    TCHAR szMutexName[MAX_PATH];   
    _tcscpy(szMutexName, szMapName);      
    _tcscat(szMutexName, _T("_Mutex"));     
    m_hAccessMutex=CreateMutex(NULL, FALSE, szMutexName);   
	if(!m_hAccessMutex)
	{
		Close();
		return 0;
	}	
	return dwRet;
}

BOOL CShareMemory::Read(void* pBuf,DWORD dwSize,DWORD dwOffset)
{
	BOOL bRet;
	if(!m_pMapBuffer) return FALSE;

	if(WaitForSingleObject(m_hAccessMutex,INFINITE)==WAIT_OBJECT_0)
	{
		memcpy(pBuf,(BYTE*)m_pMapBuffer+dwOffset,dwSize);	
		bRet = TRUE;
	}
	ReleaseMutex(m_hAccessMutex);
	return bRet;
}

BOOL CShareMemory::Write(const void* pBuf,DWORD dwSize,DWORD dwOffset)
{
	BOOL bRet;
	if(!m_pMapBuffer) return FALSE;

	if(WaitForSingleObject(m_hAccessMutex,INFINITE)==WAIT_OBJECT_0)
	{
		memcpy((BYTE*)m_pMapBuffer+dwOffset,pBuf,dwSize);
		bRet = TRUE;
	}
	ReleaseMutex(m_hAccessMutex);
	return TRUE;  
}

void CShareMemory::Close(void)
{
	if(m_hShareMemory)
	{
		UnmapViewOfFile(m_pMapBuffer);
		CloseHandle(m_hShareMemory);
		m_pMapBuffer = NULL;
		m_hShareMemory = NULL;
		
	}
	if(m_hAccessMutex)
	{
		CloseHandle(m_hAccessMutex);
		m_hAccessMutex = NULL;
	}
}