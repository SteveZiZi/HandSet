//
//=============================================================================
// ʵ�ֱ������ڴ�����̽��
//=============================================================================
//

#include "lib_leakprobe.h"


#include <Psapi.h>

#include "Tlhelp32.h"
#pragma comment(lib,"Toolhelp.lib")




//
//-----------------------------------------------------------------------------
// ���cpuʹ����
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

    //����1�����¼���
    ::Sleep(1000);
    dwIdleEnd = GetIdleTime();
    dwStopTick = GetTickCount();
    CpuUsed = (100 * (dwIdleEnd - dwIdleStart)) / ((dwStopTick - dwStartTick));

    return (100-CpuUsed);
}


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

//
//-----------------------------------------------------------------------------
// ���ȫ���ڴ�ʹ����
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
// ���ָ�����̵�heapʹ�û�����Ϣ
//-----------------------------------------------------------------------------
//
static DWORD  GetProcessHeapUsageInfo (DWORD  processId)
{

    HANDLE  snapShot = INVALID_HANDLE_VALUE;
    DWORD  heapUsage = 0;


    // �������������������ģ��, Hook��Щģ�����ת��
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

                // �ۼ�heap���С
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
// �����ڴ�������Ϣ
//-----------------------------------------------------------------------------
//
struct VirtualUsageInfo
{

    DWORD               virtSize;               // �����е������ڴ��������ֽ�

    DWORD               virtCommit;             // �������Ѿ��ύ�������ڴ��ֽ�

    DWORD               virtPrivate;            // ������˽�����͵������ڴ��ֽ�

};

//
//-----------------------------------------------------------------------------
// ���ָ�����̵������ڴ�ʹ�û�����Ϣ
//-----------------------------------------------------------------------------
//
static void  GetProcessVirtualUsageInfo (DWORD  processId, VirtualUsageInfo*  retVirtUsage)
{
    assert(retVirtUsage);
    memset(retVirtUsage, 0, sizeof(VirtualUsageInfo));     // ����ֵ��0

    HANDLE  hProcess = OpenProcess(0, FALSE, processId);
    if (hProcess)
    {
        SYSTEM_INFO  sysInfo;
        GetSystemInfo(&sysInfo);

        //MODULEINFO  theModInfo;

        // ��ý�����ģ�����ַ��Ϣ
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
                    // ������ѭ�� ���Ѿ���ĩβ
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
// ��ӡ�ڴ�ʹ����Ϣ
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
// �ڴ�����̽���߳�
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
// ��ʼ̽���ڴ�����
//-----------------------------------------------------------------------------
//
void  Lkp_StartProbeMemUsage ()
{
    HANDLE  thread = ::CreateThread(NULL, 0, MemoryUsageProbeProc, NULL, 0, NULL);
    
    ::CloseHandle(thread);
}

//
//-----------------------------------------------------------------------------
// ����̽���ڴ�����
//-----------------------------------------------------------------------------
//
void  Lkp_EndProbeMemUsage ()
{


}

