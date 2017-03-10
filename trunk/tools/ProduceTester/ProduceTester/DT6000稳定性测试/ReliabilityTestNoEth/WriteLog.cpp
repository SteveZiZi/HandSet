#include "stdafx.h"
#include "WriteLog.h"
#include "UsuallyCommon.h"
#include "assertMsg.h"
#include <xstring>
static BOOL         g_bInitSucess=FALSE;
CRITICAL_SECTION    g_csLog;                        //����ͬ��д��־�Ĳ���

#define     RESERVE_LOGSTR_NUM      32
//������ļ��У�ά��һ����¼��־���ַ�����ѭ�����У��Է���ͼ�ν�����ʾ
static WCHAR        strBuf[RESERVE_LOGSTR_NUM][2048];
static volatile int        g_iHeader=0;
static volatile int        g_iTail=0;
HANDLE              g_hSemaphore=NULL;                  //��¼ѭ����������Ч��־�ַ����ĸ���
HANDLE              g_hLogFile=INVALID_HANDLE_VALUE;    //��־�ļ����

BOOL InitWriteLog(const WCHAR *strLogPath,HWND hMainWnd)
{    
    if (g_bInitSucess)
    {
        return TRUE;
    }

    //������־�ļ�
    DeleteFile(strLogPath);
    g_hLogFile=CreateFile(strLogPath,GENERIC_WRITE,0,NULL,
        OPEN_ALWAYS,FILE_FLAG_WRITE_THROUGH,NULL);
    if (g_hLogFile ==INVALID_HANDLE_VALUE)
    {
        MessageBox(hMainWnd,L"������־�ļ�����",L"����",MB_OK|MB_ICONERROR);
        goto Init_error;
    }

    g_hSemaphore=CreateSemaphore(NULL,0,RESERVE_LOGSTR_NUM,NULL);
    if (g_hSemaphore ==NULL)
    {
        goto Init_error;
    }

    //д��UNICODE�ı��ļ�ͷ
    unsigned char bChar[2]={0xFF,0xFE};
    DWORD dwWrite;
    WriteFile(g_hLogFile,bChar,2,&dwWrite,NULL);

    InitializeCriticalSection(&g_csLog);
    g_iHeader=0;
    g_iTail=0;
    g_bInitSucess=TRUE;
    return TRUE;
Init_error:
    if (g_hLogFile !=INVALID_HANDLE_VALUE)
    {
        CloseHandle(g_hLogFile);
        g_hLogFile=INVALID_HANDLE_VALUE;
    }
   
    g_bInitSucess=FALSE;
    return FALSE;
}


int WriteLog(const WCHAR *strFormat,... )
{
 
    EnterCriticalSection(&g_csLog);
    va_list args;

    va_start(args,strFormat);
    int cnt=vswprintf(strBuf[g_iHeader],strFormat,args);
    DWORD dwWrite;
    WriteFile(g_hLogFile,strBuf[g_iHeader],cnt*sizeof(WCHAR),&dwWrite,NULL);
    va_end(args);

    //����ѭ������
    int k=(g_iHeader+1)%RESERVE_LOGSTR_NUM;
    if (k!=g_iTail)
    {
        //���ѭ������δ����������־����ѭ�����У��Է���ͼ�ν�����ʾ
        //���ѭ������������������־������ѭ�����У�ͼ�ν�����ʧ����������־����ʾ
        ReleaseSemaphore(g_hSemaphore,1,NULL);
        g_iHeader=k;
    }

    LeaveCriticalSection(&g_csLog);
    return 0;
}

void DeInitWritelog()
{
    if (g_hSemaphore)
    {
        CloseHandle(g_hSemaphore);
        g_hSemaphore=NULL;
    }
    if (g_bInitSucess)
    {
        g_bInitSucess=FALSE;
        DeleteCriticalSection(&g_csLog);
    }
    
    if (g_hLogFile !=INVALID_HANDLE_VALUE)
    {
        CloseHandle(g_hLogFile);
        g_hLogFile=INVALID_HANDLE_VALUE;
    }
    

}

DWORD GetLastLog(const WCHAR *&pLogStr)
{
    DWORD dwRet;
    dwRet =WaitForSingleObject(g_hSemaphore,500);
    if (dwRet ==WAIT_OBJECT_0)
    {
        pLogStr=strBuf[g_iTail];
    }

    pLogStr=NULL;
    return dwRet;
}

BOOL ReleaseLastLog()
{
    g_iTail=(g_iTail+1)%RESERVE_LOGSTR_NUM;
    return TRUE;
}