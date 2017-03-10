// AppRecover.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <afxwin.h>
#include <windows.h>
#include <commctrl.h>


#define DIRECTORY_NULL 18       ///<���ļ��б��

bool CopyFileFolder(CString , CString );

int _tmain(int argc, _TCHAR* argv[])
{
    //��������в���С��2�����˳�
    if(argc < 3)
    {
        return 0;
    }
    
    //�����̵Ĵ���
    int nTryTimes = 0;

    //�Ƿ���лָ�
    bool bRecover = FALSE;

    //�����̵�Ƶ��
    int nFlashTime = 50;

    //�ܹ�����ʱ��
    int nTotalTime = 300;

    //��ʼ�����̵�״̬
    while(nTryTimes++ < nTotalTime / nFlashTime)
    {

        //���F3��û�б�����
        if((GetAsyncKeyState(VK_F9) & 0x8000))
        {
            bRecover = TRUE;
            break;
        }

        //˯��һ��ʱ��
        Sleep(nFlashTime);

    }

    //���û��⵽F3�������������˳�����
    if(!bRecover)
    {
        return 0;
    }

    //��ʾ�û��Ƿ�Ҫ�ָ���ʼ����
    if(IDYES == MessageBoxW(NULL,  L"�Ƿ�Ҫ�ָ���ʼ����", L"��ʾ", MB_YESNO | MB_DEFBUTTON2))
    {

        //�ѱ��ݵĳ����Ƶ���ǰ�ĳ���Ŀ¼
        CopyFileFolder(argv[1], argv[2]);

    }

	return 0;
}

///
/// @brief
///     ��Դ�ļ�������ȫ�����Ƶ�Ŀ���ļ���
/// @param[CString]
///     strSrcPath - Ҫ���и��Ƶ�Դ�ļ���·��
/// @param[CString]
///     strDstPath - ���ƺ��Ŀ���ļ���·��
/// @return
///     ������Ƴɹ�����TRUE�����򷵻�FALSE
///
bool CopyFileFolder(CString strSrcPath, CString strDstPath)
{
    WIN32_FIND_DATA fd;
    HANDLE hFind;
    int nError = 0;

    //Ѱ�ҵ�һ���ļ�
    hFind = FindFirstFile(strSrcPath + L"\\" + L"*.*", &fd);

    //���û���ҵ��ļ�
    if(hFind == INVALID_HANDLE_VALUE)
    {
        nError = GetLastError();
        //����ǿ��ļ���
        if(nError == DIRECTORY_NULL)
        {
            //����һ�����ļ���
            CreateDirectory(strDstPath,NULL);
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        CreateDirectory(strDstPath,NULL);
    }

    do 
    {
        //�ж��Ƿ�Ϊ�ļ���
        if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            //������ļ�������������ļ����µ�����
            CopyFileFolder(strSrcPath + L"\\" + fd.cFileName, strDstPath + L"\\" + fd.cFileName);
        }
        else
        {

            //������ļ���ֱ�Ӹ��Ƶ�Ŀ���ļ�����
            if(!CopyFile(strSrcPath + L"\\" + fd.cFileName,
                strDstPath + L"\\" + fd.cFileName, FALSE))
            {
                FindClose(hFind);
                return FALSE;
            }
        }
    } while (FindNextFile(hFind, &fd));         //�����Ƿ������һ���ļ�

    //�رվ��
    FindClose(hFind);

    return TRUE;
}