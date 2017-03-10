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
// 全局内存信息
//-----------------------------------------------------------------------------
//
struct GlobalMemoryUsageInfo
{

    DWORD               dwMemoryLoad;           // 本机器内存使用百分比

    DWORD               dwTotalPhys;            // 本机器总物理内存字节数
    DWORD               dwAvailPhys;            // 本机器可用物理内存字节数
    DWORD               dwUsedPhys;             // 本机器已被使用的物理内存字节数

    DWORD               dwTotalVirtual;         // 本进程的总虚拟内存字节数
    DWORD               dwAvailVirtual;         // 本进程的可用虚拟内存字节数(没有被保留或提交的)
    DWORD               dwUsedVirtual;          // 本进程已被使用的虚拟内存字节数(被保留或被提交的)

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

