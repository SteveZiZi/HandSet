//
//=============================================================================
// 实现本进程内存用量探测
//=============================================================================
//

#include "lib_leakprobe.h"


#include <Psapi.h>

#include "Tlhelp32.h"
#pragma comment(lib,"Toolhelp.lib")




//
//-----------------------------------------------------------------------------
// 获得cpu使用率
//-----------------------------------------------------------------------------
//
static int  GetGlobalCPUUsageInfo ()
{
    DWORD               dwStartTick = 0;
    DWORD               dwIdleStart = 0; 
    DWORD               dwIdleEnd   = 0;
    DWORD               dwStopTick  = 0;
    int                 CpuUsed     = 0;

    dwStartTick = GetTickCount();
    dwIdleStart = GetIdleTime();

    //休眠1秒重新计算
    ::Sleep(1000);
    dwIdleEnd = GetIdleTime();
    dwStopTick = GetTickCount();
    CpuUsed = (100 * (dwIdleEnd - dwIdleStart)) / ((dwStopTick - dwStartTick));

    return (100-CpuUsed);
}


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

//
//-----------------------------------------------------------------------------
// 获得全局内存使用率
//-----------------------------------------------------------------------------
//
static void  GetGlobalMemoryUsageInfo (GlobalMemoryUsageInfo*  retMemInfo)
{
    assert(retMemInfo);

    MEMORYSTATUS  memStat = {sizeof(MEMORYSTATUS)};
    ::GlobalMemoryStatus(&memStat);

    retMemInfo->dwMemoryLoad = memStat.dwMemoryLoad;

    retMemInfo->dwTotalPhys = memStat.dwTotalPhys;
    retMemInfo->dwAvailPhys = memStat.dwAvailPhys;
    retMemInfo->dwUsedPhys = memStat.dwTotalPhys - memStat.dwAvailPhys;

    retMemInfo->dwTotalVirtual = memStat.dwTotalVirtual;
    retMemInfo->dwAvailVirtual = memStat.dwAvailVirtual;
    retMemInfo->dwUsedVirtual = memStat.dwTotalVirtual - memStat.dwAvailVirtual;
}


//
//-----------------------------------------------------------------------------
// 获得指定进程的heap使用汇总信息
//-----------------------------------------------------------------------------
//
static DWORD  GetProcessHeapUsageInfo (DWORD  processId)
{

    HANDLE  snapShot = INVALID_HANDLE_VALUE;
    DWORD  heapUsage = 0;


    // 遍历本进程载入的所有模块, Hook这些模块的跳转表
    snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPHEAPLIST, processId);
    if (snapShot != INVALID_HANDLE_VALUE)
    {
        HEAPLIST32  heapList = {sizeof(HEAPLIST32)};

        BOOL  findHeapList = Heap32ListFirst(snapShot, &heapList);
        while (findHeapList == TRUE)
        {
            HEAPENTRY32  heapInfo = {sizeof(HEAPENTRY32)};

            BOOL  findHeapEntry = Heap32First(snapShot, &heapInfo, 
                heapList.th32ProcessID, heapList.th32HeapID);
            while (findHeapEntry)
            {

                // 累加heap块大小
                if (heapInfo.dwFlags & LF32_FREE || heapInfo.dwFlags & LF32_DECOMMIT)
                {
                }
                else
                {
                    heapUsage += heapInfo.dwBlockSize;
                }


                findHeapEntry = Heap32Next(snapShot, &heapInfo);
            }


            findHeapList = Heap32ListNext(snapShot, &heapList);
        }

        CloseToolhelp32Snapshot(snapShot);
    }

    return heapUsage;
}

//
//-----------------------------------------------------------------------------
// 虚拟内存用量信息
//-----------------------------------------------------------------------------
//
struct VirtualUsageInfo
{

    DWORD               virtSize;               // 进程中的虚拟内存总用量字节

    DWORD               virtCommit;             // 进程中已经提交的虚拟内存字节

    DWORD               virtPrivate;            // 进程中私有类型的虚拟内存字节

};

//
//-----------------------------------------------------------------------------
// 获得指定进程的虚拟内存使用汇总信息
//-----------------------------------------------------------------------------
//
static void  GetProcessVirtualUsageInfo (DWORD  processId, VirtualUsageInfo*  retVirtUsage)
{
    assert(retVirtUsage);
    memset(retVirtUsage, 0, sizeof(VirtualUsageInfo));     // 返回值清0

    HANDLE  hProcess = OpenProcess(0, FALSE, processId);
    if (hProcess)
    {
        SYSTEM_INFO  sysInfo;
        GetSystemInfo(&sysInfo);

        //MODULEINFO  theModInfo;

        // 获得进程主模块基地址信息
        //if (GetModuleInformation(hProcess, NULL, &theModInfo, sizeof(theModInfo)))
        {
            BYTE*  base = (BYTE*)sysInfo.lpMinimumApplicationAddress;
            BYTE*  end = (BYTE*)sysInfo.lpMaximumApplicationAddress;

            for (BYTE*  adr = base; adr < end; )
            {
                MEMORY_BASIC_INFORMATION  mbi = {0};
                DWORD  ret = VirtualQueryEx(hProcess, adr, &mbi, sizeof(mbi));

                if (ret == 0 || mbi.RegionSize == 0)
                {
                    // 避免死循环 或已经到末尾
                    break;
                }

                if (mbi.State == MEM_COMMIT)
                {
                    retVirtUsage->virtCommit += mbi.RegionSize;
                }
                if (mbi.State == MEM_COMMIT || mbi.State == MEM_RESERVE)
                {
                    retVirtUsage->virtSize += mbi.RegionSize;
                }
                if (mbi.State == MEM_COMMIT || mbi.State == MEM_RESERVE && mbi.Type == MEM_PRIVATE)
                {
                    retVirtUsage->virtPrivate += mbi.RegionSize;
                }

                adr += mbi.RegionSize;

            }
        }

        CloseHandle(hProcess);
    }

}

//
//-----------------------------------------------------------------------------
// 打印内存使用信息
//-----------------------------------------------------------------------------
//
static void  PrintMemoryUsage ()
{
    DWORD  pid = GetCurrentProcessId();

    int  cpuUsage = GetGlobalCPUUsageInfo();

    GlobalMemoryUsageInfo  globalMemUsage;
    GetGlobalMemoryUsageInfo(&globalMemUsage);

    DWORD  heapUsage = GetProcessHeapUsageInfo(pid);

    VirtualUsageInfo  virtUsage;
    memset(&virtUsage, 0, sizeof(VirtualUsageInfo));
    //GetProcessVirtualUsageInfo(pid, &virtUsage);



    LKP_DEBUGLOG(L"LKP_USAGE: cpu=%d%%, load=%u%%, usedPhys=%uK, heapUsage=%uK, virtSize=%uK/%uK, virtCommit=%uK, virtPrivate=%uK\n", 
        cpuUsage, globalMemUsage.dwMemoryLoad, globalMemUsage.dwUsedPhys/1024, heapUsage/1024,
        globalMemUsage.dwUsedVirtual/1024, virtUsage.virtSize/1024, virtUsage.virtCommit/1024, virtUsage.virtPrivate/1024);

}


//
//-----------------------------------------------------------------------------
// 内存用量探测线程
//-----------------------------------------------------------------------------
//
static DWORD WINAPI  MemoryUsageProbeProc (LPVOID)
{
    ::Sleep(5000);
    
    while (1)
    {
        PrintMemoryUsage();
        ::Sleep(4000);
    }
}


//
//-----------------------------------------------------------------------------
// 开始探测内存用量
//-----------------------------------------------------------------------------
//
void  Lkp_StartProbeMemUsage ()
{
    HANDLE  thread = ::CreateThread(NULL, 0, MemoryUsageProbeProc, NULL, 0, NULL);
    
    ::CloseHandle(thread);
}

//
//-----------------------------------------------------------------------------
// 结束探测内存用量
//-----------------------------------------------------------------------------
//
void  Lkp_EndProbeMemUsage ()
{


}

