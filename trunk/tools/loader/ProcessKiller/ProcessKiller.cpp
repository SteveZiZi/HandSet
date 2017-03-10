// ProcessKiller.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <afxwin.h>
#include <windows.h>
#include <TlHelp32.h>
#include <commctrl.h>


int _tmain(int argc, _TCHAR* argv[])
{
    //如果没有输入命令行参数则退出程序
    if(argc < 2)
    {
        return -1;
    }
    
    //获取进程信息的快照
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    //如果句柄无效
    if(INVALID_HANDLE_VALUE == hSnapshot)
    {
        return -1;
    }

    PROCESSENTRY32 pe = { sizeof(pe) };
    HANDLE hProcess;
    BOOL bOk;
    bool bTerminate = TRUE;
    

    //遍历系统的全部进程
    for(bOk = Process32First(hSnapshot, &pe); bOk; bOk = Process32Next(hSnapshot, &pe))
    {

        //如果找到进程名
        if(0 == _tcscmp(argv[1], pe.szExeFile))
        {

            //根据进程ID获取进程句柄
            hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);

            //如果获取的进程句柄无效则退出循环
            if(!TerminateProcess(hProcess, 4))
            {
                bTerminate = FALSE;
                break;
            }
            
            //等待指定进程结束
            DWORD dw = WaitForSingleObject(hProcess, 5000);

            if(dw != WAIT_OBJECT_0)
            {
                bTerminate = FALSE;
            }

            break;

        }
    }
    
    //关闭进程快照
    CloseToolhelp32Snapshot(hSnapshot);

    if(bTerminate)
    {
        return 0;
    }
    else
    {
        return -1;
    }
	

}

