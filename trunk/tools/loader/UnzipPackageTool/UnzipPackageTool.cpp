// UnzipPackageTool.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <afxwin.h>
#include <windows.h>
#include <commctrl.h>
#include "tar.h"

///< 删除文件的标记
const WCHAR m_gDeleteSign[] = L"-d";

bool UnPackage( const TCHAR *, const TCHAR *);
void DirectoryInit(LPCTSTR);
bool JudgeToZip(const LPCTSTR, const LPCTSTR);

int _tmain(int argc, _TCHAR* argv[])
{
    //如果没有传入命令参数则退出
    if(argc < 2) return 0;

    bool bDelete = FALSE;
    
    //如果只有两个命令参数或三个命令参数中最后一个参数是"-d"
    if(argc == 2 || (argc == 3 && 
        (bDelete = (wcscmp(argv[2], m_gDeleteSign) == 0 ))))
    {
        TCHAR tchTemp[256];

        //复制字符串
        wcscpy(tchTemp, argv[1]);

        //获取压缩文件的父目录
        for(int i = wcslen(tchTemp); i >= 0; i--)
        {
            if(tchTemp[i] == '\\')
            {
                tchTemp[i] = 0;
                break;
            }
        }
        
        //如果不符合解压条件则退出程序
        if(!JudgeToZip(argv[1], tchTemp))
        {
            return 0;
        }

        //进行文件解压
        bool bSucc = UnPackage(tchTemp, argv[1]); 
        int a = 0;

    }
    //如果有三个或者四个命令参数
    else if(argc == 3 || (argc == 4 && 
        (bDelete = (wcscmp(argv[3], m_gDeleteSign) == 0 ))))
    {
        
        DirectoryInit(argv[2]);

        if(!JudgeToZip(argv[1], argv[2]))
        {
            return 0;
        }
        
        UnPackage(argv[2], argv[1]);

    }

    //根据得到的标记决定是否删除源文件
    if(bDelete)
    {
        DeleteFile(argv[1]);
    }

	return 0;
}


///
/// @brief
///     判断解压文件是否存在以及解压后的目标文件容量是否足够
/// @param[const LPCTSTR]
///     srcFile - 要解压的源文件
/// @param[const LPCTSTR]
///     dstPath - 解压后的文件存放路径
/// @return
///     如果可以继续解压返回TRUE，不符合解压条件返回FALSE
///
bool JudgeToZip(const LPCTSTR srcFile, const LPCTSTR dstPath)
{
    int nTryTimes = 0;
    bool isExist = FALSE;

    //循环3次来检测解压文件是否存在
    while(nTryTimes++ < 3)
    {
        //判断要解压的源文件是否存在
        isExist = (::GetFileAttributes(srcFile) != 0xFFFFFFFF);

        //如果已经检测出解压文件则退出循环
        if(isExist)
        {
            break;
        }

        //如果没检测到则睡眠1s后继续检测
        Sleep(1000);
    }

    //如果不存在则退出
    if(!isExist)
    {
        return FALSE;
    }
    
    ULARGE_INTEGER iToalFreeBytes;

    //获取目标文件路径的剩余容量
    if(!GetDiskFreeSpaceEx(dstPath, NULL, NULL, &iToalFreeBytes))
    {
        return FALSE;
    }

    //如果解压的目标路径剩余容量小于100M则不进行解压操作
    if(iToalFreeBytes.LowPart < 1024 * 1024 * 100
        && iToalFreeBytes.HighPart == 0)
    {
        return FALSE;
    }

    return TRUE;
}


///
/// @brief
///     把源文件路径进行解压后放到目标文件路径
/// @param[const TCHAR * ]
///     desPath - 解压后的文件存放路径
/// @param[const TCHAR * ]
///     srcFile - 需要解包的文件
/// @return
///     如果解压成功返回TRUE，否则返回FALSE
///
bool UnPackage( const TCHAR * desPath, const TCHAR * srcFile)
{
    //如果解压格式为.zip的文件成功
    if(unzip(desPath, srcFile) == 0)
    {
        return TRUE;
    }

    //如果解压格式为.tar的文件成功
    else if(untar(desPath, srcFile) == 0)
    {
        return TRUE;
    }

    //如果解压格式为.tar、.tgz和.tar.gz的文件成功
    else if(untgz(desPath, srcFile) == 0)
    {
        return TRUE;
    }

    //如果文件都不满足上述格式则返回FALSE
    else 
    {
        return FALSE;
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
