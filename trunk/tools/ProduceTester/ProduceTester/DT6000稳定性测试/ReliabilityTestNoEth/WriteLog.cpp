#include "stdafx.h"
#include "WriteLog.h"
#include "UsuallyCommon.h"
#include "assertMsg.h"
#include <xstring>
static BOOL         g_bInitSucess=FALSE;
CRITICAL_SECTION    g_csLog;                        //用于同步写日志的操作

#define     RESERVE_LOGSTR_NUM      32
//在这个文件中，维护一个记录日志的字符串的循环队列，以方便图形界面显示
static WCHAR        strBuf[RESERVE_LOGSTR_NUM][2048];
static volatile int        g_iHeader=0;
static volatile int        g_iTail=0;
HANDLE              g_hSemaphore=NULL;                  //记录循环队列中有效日志字符串的个数
HANDLE              g_hLogFile=INVALID_HANDLE_VALUE;    //日志文件句柄

BOOL InitWriteLog(const WCHAR *strLogPath,HWND hMainWnd)
{    
    if (g_bInitSucess)
    {
        return TRUE;
    }

    //创建日志文件
    DeleteFile(strLogPath);
    g_hLogFile=CreateFile(strLogPath,GENERIC_WRITE,0,NULL,
        OPEN_ALWAYS,FILE_FLAG_WRITE_THROUGH,NULL);
    if (g_hLogFile ==INVALID_HANDLE_VALUE)
    {
        MessageBox(hMainWnd,L"创建日志文件错误",L"错误",MB_OK|MB_ICONERROR);
        goto Init_error;
    }

    g_hSemaphore=CreateSemaphore(NULL,0,RESERVE_LOGSTR_NUM,NULL);
    if (g_hSemaphore ==NULL)
    {
        goto Init_error;
    }

    //写入UNICODE文本文件头
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

    //处理循环队列
    int k=(g_iHeader+1)%RESERVE_LOGSTR_NUM;
    if (k!=g_iTail)
    {
        //如果循环队列未满，将新日志插入循环队列，以方便图形界面显示
        //如果循环队列已满，则新日志不插入循环队列，图形界面则丢失掉对这条日志的显示
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