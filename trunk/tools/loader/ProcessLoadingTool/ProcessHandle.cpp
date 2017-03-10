#include "stdafx.h"
#include "ProcessHandle.h"
#include <dbgapi.h>
#include <afxwin.h>
#include <atlimage.h>

const int WINDOWS_WIDE = 640;                                   ///<标准窗口的宽度

const int WINDOWS_HEIGHT = 480;                                 ///<标准窗口的高度

const CString CUR_PROCESS_NAME = L"ProcessLoadingTool.exe";     ///<当前进程名

const UINT MAX_FNAME_LEN = 256;                                 ///<文件路径长度的最大值

const CString XML_FILE_NAME = L"ProcessLoad.xml";               ///<XML文件名

std::map<DWORD, CString> g_mapError;                            ///<保存异常信息



CProcessHandle::CProcessHandle(void)
{ 
    m_nProcessCnt = 0;

    //进行错误信息的初始化
    g_mapError[EXCEPTION_ACCESS_VIOLATION]          =    _T("EXCEPTION_ACCESS_VIOLATION"); 
    g_mapError[EXCEPTION_DATATYPE_MISALIGNMENT]     =    _T("EXCEPTION_DATATYPE_MISALIGNMENT");
    g_mapError[EXCEPTION_BREAKPOINT]                =    _T("EXCEPTION_BREAKPOINT"); 
    g_mapError[EXCEPTION_SINGLE_STEP]               =    _T("EXCEPTION_SINGLE_STEP"); 
    g_mapError[EXCEPTION_ARRAY_BOUNDS_EXCEEDED]     =    _T("EXCEPTION_ARRAY_BOUNDS_EXCEEDED"); 
    g_mapError[EXCEPTION_FLT_DENORMAL_OPERAND]      =    _T("EXCEPTION_FLT_DENORMAL_OPERAND"); 
    g_mapError[EXCEPTION_FLT_DIVIDE_BY_ZERO]        =    _T("EXCEPTION_FLT_DIVIDE_BY_ZERO"); 
    g_mapError[EXCEPTION_FLT_INEXACT_RESULT]        =    _T("EXCEPTION_FLT_INEXACT_RESULT"); 
    g_mapError[EXCEPTION_FLT_INVALID_OPERATION]     =    _T("EXCEPTION_FLT_INVALID_OPERATION"); 
    g_mapError[EXCEPTION_FLT_OVERFLOW]              =    _T("EXCEPTION_FLT_OVERFLOW"); 
    g_mapError[EXCEPTION_FLT_STACK_CHECK]           =    _T("EXCEPTION_FLT_STACK_CHECK"); 
    g_mapError[EXCEPTION_FLT_UNDERFLOW]             =    _T("EXCEPTION_FLT_UNDERFLOW"); 
    g_mapError[EXCEPTION_INT_DIVIDE_BY_ZERO]        =    _T("EXCEPTION_INT_DIVIDE_BY_ZERO"); 
    g_mapError[EXCEPTION_INT_OVERFLOW]              =    _T("EXCEPTION_INT_OVERFLOW"); 
    g_mapError[EXCEPTION_PRIV_INSTRUCTION]          =    _T("EXCEPTION_PRIV_INSTRUCTION"); 
    g_mapError[EXCEPTION_IN_PAGE_ERROR]             =    _T("EXCEPTION_IN_PAGE_ERROR"); 
    g_mapError[EXCEPTION_ILLEGAL_INSTRUCTION]       =    _T("EXCEPTION_ILLEGAL_INSTRUCTION"); 
    g_mapError[EXCEPTION_NONCONTINUABLE_EXCEPTION]  =    _T("EXCEPTION_NONCONTINUABLE_EXCEPTION"); 
    g_mapError[EXCEPTION_STACK_OVERFLOW]            =    _T("EXCEPTION_STACK_OVERFLOW"); 
    g_mapError[EXCEPTION_INVALID_DISPOSITION]       =    _T("EXCEPTION_INVALID_DISPOSITION"); 
    g_mapError[EXCEPTION_GUARD_PAGE]                =    _T("EXCEPTION_GUARD_PAGE"); 
    g_mapError[EXCEPTION_INVALID_HANDLE]            =    _T("EXCEPTION_INVALID_HANDLE");  
}

CProcessHandle::~CProcessHandle(void)
{
    if(m_pReadProcess != NULL)
    {
        delete m_pReadProcess;
    }

    //关闭保护进程句柄
    for(UINT i = 0; i < m_nProcessCnt; i++)
    {
        CloseHandle(m_hProcess[i]);
    }

}

///
/// @brief
///     创建一个子进程
/// @param[_PROCESS_INFO]
///     process_info - 进程信息
/// @return
///     启动线程成功则返回TRUE，否则返回FALSE
///
bool CProcessHandle::RunProcess( _PROCESS_INFO process_info )
{
    PROCESS_INFORMATION pi;
    CString strImageName;
    
    //判断路径名是绝对路径还是相对路径
    if(process_info.bAbsolutePath)
    {
        strImageName = process_info.strProcessName;
    }
    else
    {
        //如果是相对路径则须加上当前应用程序所在目录的父目录
        strImageName = strProcessPath + L"\\" + process_info.strProcessName;
    }

    //创建一个新的子进程
    if(!CreateProcess(strImageName, process_info.strProcessParam, NULL, NULL, 
        FALSE, CREATE_NEW_CONSOLE, NULL, NULL,NULL, &pi))
    {
        return FALSE;
    }

    //如果属于监视线程
    if(process_info.isProtected)
    {
        //如果进程数大于默认值
        if(m_nProcessCnt >= PROCESS_SIZE)
        {
            return FALSE;
        }

        //保存进程的句柄
        m_hProcess[m_nProcessCnt++] = pi.hProcess;
    }
    else
    {
        //替换当前最新进程句柄
        m_hPreProcess = pi.hProcess;
    }

    return TRUE;
}

///
/// @brief
///     开始子进程的启动
///
/// @return
///     进程全部创建成功则返回TRUE，否则返回FALSE
///
bool CProcessHandle::StartProcess()
{
    WCHAR wProcessPath[MAX_FNAME_LEN];
    
    //如果获取当前应用程序的目录名失败
    if(!GetModuleFileName(NULL, wProcessPath, MAX_FNAME_LEN))
    {
        RETAILMSG(TRUE,  (CUR_PROCESS_NAME + TEXT(" Error: can't get the process path\n"), NULL));
        return FALSE;
    };

    //获取当前应用程序所在目录的父目录
    for(int i = wcslen(wProcessPath); i >= 0; i--)
    {
        //如果从后面开始找到目录分隔标记
        if(wProcessPath[i] == '\\')
        {
            wProcessPath[i] = 0;
            break;
        }
    }

    strProcessPath.Format(L"%s", wProcessPath);

    //xml文件用绝对路径来表示
    m_pReadProcess = new CReadProcess(strProcessPath + L"\\" + XML_FILE_NAME);

    //如果获取进程列表失败
    if(!m_pReadProcess->GetProcessList(m_vecProcessInfo))
    {
        return FALSE;
    };
    
    CString strLogoName;

    //获取logo图片路径
    strLogoName = m_pReadProcess->GetLogoName();

    if(strLogoName[0] != '\\')
    {
        strLogoName  = strProcessPath + L"\\" + strLogoName;
    }

    //显示logo图片
    if(!DisplayLogo(strLogoName))
    {
        RETAILMSG(TRUE, (CUR_PROCESS_NAME + TEXT(" Error: can't display the logo\n"),NULL));
    }

    for(UINT i = 0; i < m_vecProcessInfo.size(); i++)
    {
        //如果延迟时间是小于0
        if(m_vecProcessInfo[i].uDelayTime < 0)
        {
           
            if(m_hPreProcess != INVALID_HANDLE_VALUE)
            {   
                //一直等待上一个进程结束
                WaitForSingleObject(m_hPreProcess, INFINITE);
            }
            
        }
        else
        {
            //相对于上一个进程要睡眠的时间
            Sleep(m_vecProcessInfo[i].uDelayTime);

        }
      

        if(!RunProcess(m_vecProcessInfo[i]))
        {
            //输出启动进程的失败信息
            RETAILMSG(TRUE, (CUR_PROCESS_NAME + TEXT(" Error: %s start fail\n"),
                m_vecProcessInfo[i].strProcessName));

            return FALSE;
        }
        
        //如果是保护进程
        if(m_vecProcessInfo[i].isProtected)
        {
            //进程句柄到进程数组下标的映射
            m_mapHandle[m_hProcess[m_nProcessCnt - 1]] = i;
        }

    }

    return TRUE;
}

///
/// @brief
///     开始监听保护进程
///
/// @return
///     如果全部进程都正常退出则返回TRUE，发生错误返回FALSE
///
bool CProcessHandle::StartMonitoring()
{
    //一直等待直到有一个保护进程退出或发生异常
    DWORD dwResult = WaitForMultipleObjects(m_nProcessCnt, m_hProcess, FALSE, INFINITE);

    //如果进程句柄发生错误
    if(dwResult == WAIT_FAILED)
    {
        return FALSE;
    }

    //计算出第几个对象变为已通知状态，即第几个进程退出了
    DWORD dwIndex = dwResult - WAIT_OBJECT_0;

    ASSERT(dwIndex < m_nProcessCnt);

    DWORD dwExitCode, dwMap;

    //获取进程退出代码
    if(!GetExitCodeProcess(m_hProcess[dwIndex], &dwExitCode))
    {
        RemoveProcessHandle(dwIndex);
        return TRUE; 
    }

    //先获取退出的进程在进程列表中的下标
    dwMap = m_mapHandle[m_hProcess[dwIndex]];

    //在映射中删除这个句柄到下标的映射
    m_mapHandle.erase(m_mapHandle.find(m_hProcess[dwIndex]));

    //移除要监控的进程句柄
    RemoveProcessHandle(dwIndex);

    //重新启动该进程
    RunProcess(m_vecProcessInfo[dwMap]);

    m_mapHandle[m_hProcess[m_nProcessCnt - 1]] = dwMap;
    
    //如果是发生异常退出
    if(0 != dwExitCode)
    {
        //输出异常信息
        RETAILMSG(TRUE, (CUR_PROCESS_NAME + TEXT(" Process: %s Exception: %s\n"),
                  m_vecProcessInfo[dwMap].strProcessName, g_mapError[dwExitCode]));
    }

    return TRUE;
}

///
/// @brief
///     根据下标移除进程句柄
/// @param[DWORD]
///     dwIndex - 进程句柄的下标
/// @return
///     删除成功则返回TRUE，否则返回FALSE
///
void CProcessHandle::RemoveProcessHandle( DWORD dwIndex )
{
    m_nProcessCnt--;
    
    //如果是最后一个，则不用进行移位直接返回
    if(dwIndex == m_nProcessCnt)
    {
        return ;
    }

    //把下标后面的数据往前移
    for(UINT i = dwIndex; i < m_nProcessCnt; i++)
    {
        m_hProcess[i] = m_hProcess[i + 1];
    }

}

///
/// @brief
///      
/// @param[CString]
///     strLogoName - logo图片路径
/// @return
///     如果加载图片成功返回TRUE，否则返回FALSE
///
bool CProcessHandle::DisplayLogo( CString strLogoName )
{
    HBITMAP hOldBitmap, hBitmap;
    HDC hMemDC, hDC;
    CBitmap bBitmap;

    //获取整个屏幕的宽
    int nWidth = GetSystemMetrics(SM_CXSCREEN);

    //获取整个屏幕的高
    int nHeight = GetSystemMetrics(SM_CYSCREEN); 

    //显示图片的起始横坐标
    int nStartX = (nWidth - WINDOWS_WIDE) / 2;

    //如果值小于0则赋值为0
    nStartX = nStartX < 0 ? 0 : nStartX;

    //从文件中加载位图
    hBitmap =  SHLoadDIBitmap(strLogoName);

    //如果加载失败则返回
    if(hBitmap == NULL)
    {
        return FALSE;
    }

    //把HBITMAP转为CBitmap
    bBitmap.Attach (hBitmap);

    hDC = GetDC(NULL);

    //创建内存设备DC
    hMemDC = CreateCompatibleDC(hDC);

    hOldBitmap = (HBITMAP)SelectObject(hMemDC, bBitmap);

    //将内存设备DC的内容复制到当前DC
    BitBlt(hDC, nStartX, 0, nWidth , nHeight, hMemDC, 0, 0, SRCCOPY);

    //恢复原来的位图设置
    SelectObject(hDC, hOldBitmap);

    //释放DC资源
    DeleteDC(hMemDC);
    ReleaseDC(NULL, hDC);
    return TRUE;
}

