#pragma once




//-----------------------------------------------------------------------------
// common
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <new>
#include <windows.h>

#include "undoc.h"


#define LKP_DEBUGLOG  NKDbgPrintfW

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif



//
//-----------------------------------------------------------------------------
// ȫ���ڴ���Ϣ
//-----------------------------------------------------------------------------
//
struct GlobalMemoryUsageInfo
{

    DWORD               dwMemoryLoad;           // �������ڴ�ʹ�ðٷֱ�

    DWORD               dwTotalPhys;            // �������������ڴ��ֽ���
    DWORD               dwAvailPhys;            // ���������������ڴ��ֽ���
    DWORD               dwUsedPhys;             // �������ѱ�ʹ�õ������ڴ��ֽ���

    DWORD               dwTotalVirtual;         // �����̵��������ڴ��ֽ���
    DWORD               dwAvailVirtual;         // �����̵Ŀ��������ڴ��ֽ���(û�б��������ύ��)
    DWORD               dwUsedVirtual;          // �������ѱ�ʹ�õ������ڴ��ֽ���(���������ύ��)

};



//-----------------------------------------------------------------------------
// libLeakProbe
//-----------------------------------------------------------------------------


void  Lkp_StartProbe ();
void  Lkp_EndProbe ();




//-----------------------------------------------------------------------------
// api_hook
//-----------------------------------------------------------------------------


PROC  Lkp_HookApi (LPCWSTR  calleeModuleName, LPCWSTR  currentFunctionName, PROC  newFunction);
PROC  Lkp_UnHookApi (LPCWSTR  calleeModuleName, LPCWSTR  currentFunctionName, PROC  newFunction);

LPCWSTR  Lkp_LookupAddressOwner (void*  addr);


//-----------------------------------------------------------------------------
// memleak_probe
//-----------------------------------------------------------------------------


void  Lkp_StartProbeMemLeak ();
void  Lkp_EndProbeMemLeak ();

void  Lkp_BreakAtAllocCount (DWORD64  cnt);
void  Lkp_BreakAtAllocHandle (void*  handle);


//-----------------------------------------------------------------------------
// memusage_probe
//-----------------------------------------------------------------------------

void  Lkp_StartProbeMemUsage ();
void  Lkp_EndProbeMemUsage ();
void  Lkp_GetCurrentUsageInfo (int* retCpuUsage, GlobalMemoryUsageInfo* retMemUsage);

