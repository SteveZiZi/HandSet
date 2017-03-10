#include "stdafx.h"
#include "ProcessLoadingTool.h"
#include "ProcessHandle.h"
#include <afxwin.h>

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR    lpCmdLine,
                   int       nCmdShow)
{
    /////<事件对象的句柄
    //HANDLE  g_hEvent;  

    ////创建一个命名的事件对象
    //g_hEvent = CreateEvent(NULL, FALSE, FALSE, L"ProcessLoading");

    //if(g_hEvent)
    //{
    //    //如果事件对象句柄已经存在
    //    if(ERROR_ALREADY_EXISTS == GetLastError())
    //    {
    //        return 0;
    //    }
    //}
    //

    //创建CProcessHandle对象
    CProcessHandle * pProcessHandle = new CProcessHandle;

    //如果全部进程启动成功
    if(pProcessHandle->StartProcess())
    {
        //开始监听保护进程
        while(pProcessHandle->StartMonitoring());
    }

    //关闭事件对象句柄
    //CloseHandle(g_hEvent);

    //释放资源
    delete pProcessHandle;

    return 0;
}
