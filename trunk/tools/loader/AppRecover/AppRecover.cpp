// AppRecover.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <afxwin.h>
#include <windows.h>
#include <commctrl.h>


#define DIRECTORY_NULL 18       ///<空文件夹标记

bool CopyFileFolder(CString , CString );

int _tmain(int argc, _TCHAR* argv[])
{
    //如果命令行参数小于2个则退出
    if(argc < 3)
    {
        return 0;
    }
    
    //检测键盘的次数
    int nTryTimes = 0;

    //是否进行恢复
    bool bRecover = FALSE;

    //检测键盘的频率
    int nFlashTime = 50;

    //总共检测的时间
    int nTotalTime = 300;

    //开始检测键盘的状态
    while(nTryTimes++ < nTotalTime / nFlashTime)
    {

        //检测F3有没有被按下
        if((GetAsyncKeyState(VK_F9) & 0x8000))
        {
            bRecover = TRUE;
            break;
        }

        //睡眠一段时间
        Sleep(nFlashTime);

    }

    //如果没检测到F3按键被按下则退出程序
    if(!bRecover)
    {
        return 0;
    }

    //提示用户是否要恢复初始程序
    if(IDYES == MessageBoxW(NULL,  L"是否要恢复初始程序？", L"提示", MB_YESNO | MB_DEFBUTTON2))
    {

        //把备份的程序复制到当前的程序目录
        CopyFileFolder(argv[1], argv[2]);

    }

	return 0;
}

///
/// @brief
///     把源文件的内容全部复制到目标文件夹
/// @param[CString]
///     strSrcPath - 要进行复制的源文件夹路径
/// @param[CString]
///     strDstPath - 复制后的目标文件夹路径
/// @return
///     如果复制成功返回TRUE，否则返回FALSE
///
bool CopyFileFolder(CString strSrcPath, CString strDstPath)
{
    WIN32_FIND_DATA fd;
    HANDLE hFind;
    int nError = 0;

    //寻找第一个文件
    hFind = FindFirstFile(strSrcPath + L"\\" + L"*.*", &fd);

    //如果没有找到文件
    if(hFind == INVALID_HANDLE_VALUE)
    {
        nError = GetLastError();
        //如果是空文件夹
        if(nError == DIRECTORY_NULL)
        {
            //创建一个空文件夹
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
        //判断是否为文件夹
        if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            //如果是文件夹则继续遍历文件夹下的内容
            CopyFileFolder(strSrcPath + L"\\" + fd.cFileName, strDstPath + L"\\" + fd.cFileName);
        }
        else
        {

            //如果是文件则直接复制到目标文件夹下
            if(!CopyFile(strSrcPath + L"\\" + fd.cFileName,
                strDstPath + L"\\" + fd.cFileName, FALSE))
            {
                FindClose(hFind);
                return FALSE;
            }
        }
    } while (FindNextFile(hFind, &fd));         //查找是否存在下一个文件

    //关闭句柄
    FindClose(hFind);

    return TRUE;
}