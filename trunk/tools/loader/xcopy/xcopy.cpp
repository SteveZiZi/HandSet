// xcopy.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <afxwin.h>
#include <windows.h>

#define DIRECTORY_NULL 18       ///<空文件夹标记
enum FILE_RESULT
{
    IS_FILE = 0,                ///<文件
    IS_DIRECTORY = 1,           ///<文件夹
    IS_ERROR = 2                ///<文件路径错误
};

bool CopyFileFolder(CString , CString );
FILE_RESULT JudgeIsFile(CString);
void DirectoryInit(LPCTSTR);

int _tmain(int argc, _TCHAR* argv[])
{
    //如果命令参数个数少于3个则返回
    if(argc < 3) return -1;

    /* argv[1] = L"\\test.bmp";
    argv[2] = L"\\xcopyt\\test.bmp";*/
    
    _TCHAR chFullPath[256];
    bool bSucc;
    FILE_RESULT file_result;
    file_result = JudgeIsFile(argv[1]);
    //如果是命令参数是文件
    if(IS_FILE == file_result)
    {
        UINT nIndex = 0;
        //为了获取文件名开始位置在字符串中的位置
        for(int i = 0; argv[2][i] != NULL; i++)
        {
            if(argv[2][i] == L'\\')
            {
                nIndex = i;
            }
        }

        //只复制前面的文件夹信息，不复制文件名
        wcsncpy(chFullPath, argv[2], nIndex);

        chFullPath[nIndex] = 0;

        //进行文件夹的初始化
        DirectoryInit(chFullPath);

        //进行文件的复制
        bSucc = (0 != CopyFile(argv[1], argv[2], FALSE));
    }
    //如果是文件夹
    else if(IS_DIRECTORY == file_result)
    {
        DirectoryInit(argv[2]);  

        //复制根据命令行参数复制文件夹
        bSucc = CopyFileFolder(argv[1], argv[2]);
    }
    else
    {
        return -1;
    }

    if(!bSucc)
    {
        return -1;
    }
    else
    {
        return 0;
    }
	
}

///
/// @brief
///     进行文件夹的初始创建
/// @param[LPCTSTR]
///     lpszFullString - 要创建文件夹的路径
/// @return
///
///
void DirectoryInit(LPCTSTR lpszFullString)
{
    CString strDirectory, strFileName;
    UINT nSubString = 0;

    //以符号\为分隔标记循环读取文件夹名字
    while(AfxExtractSubString(strFileName, lpszFullString, nSubString++, L'\\'))
    {
        if(strFileName != L"")
        {
            strDirectory += "\\";
            strDirectory += strFileName;

            //如果文件夹不存在则会创建
            CreateDirectory(strDirectory, NULL);
        }
    }
}

///
/// @brief
///     根据文件路径名判断是文件还是文件夹
/// @param[CString]
///     strFilePath - 文件的路径名
/// @return
///     
///
FILE_RESULT JudgeIsFile(CString strFilePath)
{
    WIN32_FIND_DATA fd;
    //如果找到的文件路径名有效
    if(FindFirstFile(strFilePath, &fd) != INVALID_HANDLE_VALUE)
    {
        //如果是文件夹
        if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            return IS_DIRECTORY;
        }
        else
        {
            return IS_FILE;
        }
    }
    return IS_ERROR;
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