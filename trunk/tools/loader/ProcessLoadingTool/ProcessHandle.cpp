#include "stdafx.h"
#include "ProcessHandle.h"
#include <dbgapi.h>
#include <afxwin.h>
#include <atlimage.h>

const int WINDOWS_WIDE = 640;                                   ///<��׼���ڵĿ��

const int WINDOWS_HEIGHT = 480;                                 ///<��׼���ڵĸ߶�

const CString CUR_PROCESS_NAME = L"ProcessLoadingTool.exe";     ///<��ǰ������

const UINT MAX_FNAME_LEN = 256;                                 ///<�ļ�·�����ȵ����ֵ

const CString XML_FILE_NAME = L"ProcessLoad.xml";               ///<XML�ļ���

std::map<DWORD, CString> g_mapError;                            ///<�����쳣��Ϣ



CProcessHandle::CProcessHandle(void)
{ 
    m_nProcessCnt = 0;

    //���д�����Ϣ�ĳ�ʼ��
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

    //�رձ������̾��
    for(UINT i = 0; i < m_nProcessCnt; i++)
    {
        CloseHandle(m_hProcess[i]);
    }

}

///
/// @brief
///     ����һ���ӽ���
/// @param[_PROCESS_INFO]
///     process_info - ������Ϣ
/// @return
///     �����̳߳ɹ��򷵻�TRUE�����򷵻�FALSE
///
bool CProcessHandle::RunProcess( _PROCESS_INFO process_info )
{
    PROCESS_INFORMATION pi;
    CString strImageName;
    
    //�ж�·�����Ǿ���·���������·��
    if(process_info.bAbsolutePath)
    {
        strImageName = process_info.strProcessName;
    }
    else
    {
        //��������·��������ϵ�ǰӦ�ó�������Ŀ¼�ĸ�Ŀ¼
        strImageName = strProcessPath + L"\\" + process_info.strProcessName;
    }

    //����һ���µ��ӽ���
    if(!CreateProcess(strImageName, process_info.strProcessParam, NULL, NULL, 
        FALSE, CREATE_NEW_CONSOLE, NULL, NULL,NULL, &pi))
    {
        return FALSE;
    }

    //������ڼ����߳�
    if(process_info.isProtected)
    {
        //�������������Ĭ��ֵ
        if(m_nProcessCnt >= PROCESS_SIZE)
        {
            return FALSE;
        }

        //������̵ľ��
        m_hProcess[m_nProcessCnt++] = pi.hProcess;
    }
    else
    {
        //�滻��ǰ���½��̾��
        m_hPreProcess = pi.hProcess;
    }

    return TRUE;
}

///
/// @brief
///     ��ʼ�ӽ��̵�����
///
/// @return
///     ����ȫ�������ɹ��򷵻�TRUE�����򷵻�FALSE
///
bool CProcessHandle::StartProcess()
{
    WCHAR wProcessPath[MAX_FNAME_LEN];
    
    //�����ȡ��ǰӦ�ó����Ŀ¼��ʧ��
    if(!GetModuleFileName(NULL, wProcessPath, MAX_FNAME_LEN))
    {
        RETAILMSG(TRUE,  (CUR_PROCESS_NAME + TEXT(" Error: can't get the process path\n"), NULL));
        return FALSE;
    };

    //��ȡ��ǰӦ�ó�������Ŀ¼�ĸ�Ŀ¼
    for(int i = wcslen(wProcessPath); i >= 0; i--)
    {
        //����Ӻ��濪ʼ�ҵ�Ŀ¼�ָ����
        if(wProcessPath[i] == '\\')
        {
            wProcessPath[i] = 0;
            break;
        }
    }

    strProcessPath.Format(L"%s", wProcessPath);

    //xml�ļ��þ���·������ʾ
    m_pReadProcess = new CReadProcess(strProcessPath + L"\\" + XML_FILE_NAME);

    //�����ȡ�����б�ʧ��
    if(!m_pReadProcess->GetProcessList(m_vecProcessInfo))
    {
        return FALSE;
    };
    
    CString strLogoName;

    //��ȡlogoͼƬ·��
    strLogoName = m_pReadProcess->GetLogoName();

    if(strLogoName[0] != '\\')
    {
        strLogoName  = strProcessPath + L"\\" + strLogoName;
    }

    //��ʾlogoͼƬ
    if(!DisplayLogo(strLogoName))
    {
        RETAILMSG(TRUE, (CUR_PROCESS_NAME + TEXT(" Error: can't display the logo\n"),NULL));
    }

    for(UINT i = 0; i < m_vecProcessInfo.size(); i++)
    {
        //����ӳ�ʱ����С��0
        if(m_vecProcessInfo[i].uDelayTime < 0)
        {
           
            if(m_hPreProcess != INVALID_HANDLE_VALUE)
            {   
                //һֱ�ȴ���һ�����̽���
                WaitForSingleObject(m_hPreProcess, INFINITE);
            }
            
        }
        else
        {
            //�������һ������Ҫ˯�ߵ�ʱ��
            Sleep(m_vecProcessInfo[i].uDelayTime);

        }
      

        if(!RunProcess(m_vecProcessInfo[i]))
        {
            //����������̵�ʧ����Ϣ
            RETAILMSG(TRUE, (CUR_PROCESS_NAME + TEXT(" Error: %s start fail\n"),
                m_vecProcessInfo[i].strProcessName));

            return FALSE;
        }
        
        //����Ǳ�������
        if(m_vecProcessInfo[i].isProtected)
        {
            //���̾�������������±��ӳ��
            m_mapHandle[m_hProcess[m_nProcessCnt - 1]] = i;
        }

    }

    return TRUE;
}

///
/// @brief
///     ��ʼ������������
///
/// @return
///     ���ȫ�����̶������˳��򷵻�TRUE���������󷵻�FALSE
///
bool CProcessHandle::StartMonitoring()
{
    //һֱ�ȴ�ֱ����һ�����������˳������쳣
    DWORD dwResult = WaitForMultipleObjects(m_nProcessCnt, m_hProcess, FALSE, INFINITE);

    //������̾����������
    if(dwResult == WAIT_FAILED)
    {
        return FALSE;
    }

    //������ڼ��������Ϊ��֪ͨ״̬�����ڼ��������˳���
    DWORD dwIndex = dwResult - WAIT_OBJECT_0;

    ASSERT(dwIndex < m_nProcessCnt);

    DWORD dwExitCode, dwMap;

    //��ȡ�����˳�����
    if(!GetExitCodeProcess(m_hProcess[dwIndex], &dwExitCode))
    {
        RemoveProcessHandle(dwIndex);
        return TRUE; 
    }

    //�Ȼ�ȡ�˳��Ľ����ڽ����б��е��±�
    dwMap = m_mapHandle[m_hProcess[dwIndex]];

    //��ӳ����ɾ�����������±��ӳ��
    m_mapHandle.erase(m_mapHandle.find(m_hProcess[dwIndex]));

    //�Ƴ�Ҫ��صĽ��̾��
    RemoveProcessHandle(dwIndex);

    //���������ý���
    RunProcess(m_vecProcessInfo[dwMap]);

    m_mapHandle[m_hProcess[m_nProcessCnt - 1]] = dwMap;
    
    //����Ƿ����쳣�˳�
    if(0 != dwExitCode)
    {
        //����쳣��Ϣ
        RETAILMSG(TRUE, (CUR_PROCESS_NAME + TEXT(" Process: %s Exception: %s\n"),
                  m_vecProcessInfo[dwMap].strProcessName, g_mapError[dwExitCode]));
    }

    return TRUE;
}

///
/// @brief
///     �����±��Ƴ����̾��
/// @param[DWORD]
///     dwIndex - ���̾�����±�
/// @return
///     ɾ���ɹ��򷵻�TRUE�����򷵻�FALSE
///
void CProcessHandle::RemoveProcessHandle( DWORD dwIndex )
{
    m_nProcessCnt--;
    
    //��������һ�������ý�����λֱ�ӷ���
    if(dwIndex == m_nProcessCnt)
    {
        return ;
    }

    //���±�����������ǰ��
    for(UINT i = dwIndex; i < m_nProcessCnt; i++)
    {
        m_hProcess[i] = m_hProcess[i + 1];
    }

}

///
/// @brief
///      
/// @param[CString]
///     strLogoName - logoͼƬ·��
/// @return
///     �������ͼƬ�ɹ�����TRUE�����򷵻�FALSE
///
bool CProcessHandle::DisplayLogo( CString strLogoName )
{
    HBITMAP hOldBitmap, hBitmap;
    HDC hMemDC, hDC;
    CBitmap bBitmap;

    //��ȡ������Ļ�Ŀ�
    int nWidth = GetSystemMetrics(SM_CXSCREEN);

    //��ȡ������Ļ�ĸ�
    int nHeight = GetSystemMetrics(SM_CYSCREEN); 

    //��ʾͼƬ����ʼ������
    int nStartX = (nWidth - WINDOWS_WIDE) / 2;

    //���ֵС��0��ֵΪ0
    nStartX = nStartX < 0 ? 0 : nStartX;

    //���ļ��м���λͼ
    hBitmap =  SHLoadDIBitmap(strLogoName);

    //�������ʧ���򷵻�
    if(hBitmap == NULL)
    {
        return FALSE;
    }

    //��HBITMAPתΪCBitmap
    bBitmap.Attach (hBitmap);

    hDC = GetDC(NULL);

    //�����ڴ��豸DC
    hMemDC = CreateCompatibleDC(hDC);

    hOldBitmap = (HBITMAP)SelectObject(hMemDC, bBitmap);

    //���ڴ��豸DC�����ݸ��Ƶ���ǰDC
    BitBlt(hDC, nStartX, 0, nWidth , nHeight, hMemDC, 0, 0, SRCCOPY);

    //�ָ�ԭ����λͼ����
    SelectObject(hDC, hOldBitmap);

    //�ͷ�DC��Դ
    DeleteDC(hMemDC);
    ReleaseDC(NULL, hDC);
    return TRUE;
}

