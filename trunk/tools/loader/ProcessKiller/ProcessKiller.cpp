// ProcessKiller.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <afxwin.h>
#include <windows.h>
#include <TlHelp32.h>
#include <commctrl.h>


int _tmain(int argc, _TCHAR* argv[])
{
    //���û�����������в������˳�����
    if(argc < 2)
    {
        return -1;
    }
    
    //��ȡ������Ϣ�Ŀ���
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    //��������Ч
    if(INVALID_HANDLE_VALUE == hSnapshot)
    {
        return -1;
    }

    PROCESSENTRY32 pe = { sizeof(pe) };
    HANDLE hProcess;
    BOOL bOk;
    bool bTerminate = TRUE;
    

    //����ϵͳ��ȫ������
    for(bOk = Process32First(hSnapshot, &pe); bOk; bOk = Process32Next(hSnapshot, &pe))
    {

        //����ҵ�������
        if(0 == _tcscmp(argv[1], pe.szExeFile))
        {

            //���ݽ���ID��ȡ���̾��
            hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);

            //�����ȡ�Ľ��̾����Ч���˳�ѭ��
            if(!TerminateProcess(hProcess, 4))
            {
                bTerminate = FALSE;
                break;
            }
            
            //�ȴ�ָ�����̽���
            DWORD dw = WaitForSingleObject(hProcess, 5000);

            if(dw != WAIT_OBJECT_0)
            {
                bTerminate = FALSE;
            }

            break;

        }
    }
    
    //�رս��̿���
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

