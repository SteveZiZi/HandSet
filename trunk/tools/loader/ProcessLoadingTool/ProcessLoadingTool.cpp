#include "stdafx.h"
#include "ProcessLoadingTool.h"
#include "ProcessHandle.h"
#include <afxwin.h>

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPTSTR    lpCmdLine,
                   int       nCmdShow)
{
    /////<�¼�����ľ��
    //HANDLE  g_hEvent;  

    ////����һ���������¼�����
    //g_hEvent = CreateEvent(NULL, FALSE, FALSE, L"ProcessLoading");

    //if(g_hEvent)
    //{
    //    //����¼��������Ѿ�����
    //    if(ERROR_ALREADY_EXISTS == GetLastError())
    //    {
    //        return 0;
    //    }
    //}
    //

    //����CProcessHandle����
    CProcessHandle * pProcessHandle = new CProcessHandle;

    //���ȫ�����������ɹ�
    if(pProcessHandle->StartProcess())
    {
        //��ʼ������������
        while(pProcessHandle->StartMonitoring());
    }

    //�ر��¼�������
    //CloseHandle(g_hEvent);

    //�ͷ���Դ
    delete pProcessHandle;

    return 0;
}
