// xcopy.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <afxwin.h>
#include <windows.h>

#define DIRECTORY_NULL 18       ///<���ļ��б��
enum FILE_RESULT
{
    IS_FILE = 0,                ///<�ļ�
    IS_DIRECTORY = 1,           ///<�ļ���
    IS_ERROR = 2                ///<�ļ�·������
};

bool CopyFileFolder(CString , CString );
FILE_RESULT JudgeIsFile(CString);
void DirectoryInit(LPCTSTR);

int _tmain(int argc, _TCHAR* argv[])
{
    //������������������3���򷵻�
    if(argc < 3) return -1;

    /* argv[1] = L"\\test.bmp";
    argv[2] = L"\\xcopyt\\test.bmp";*/
    
    _TCHAR chFullPath[256];
    bool bSucc;
    FILE_RESULT file_result;
    file_result = JudgeIsFile(argv[1]);
    //���������������ļ�
    if(IS_FILE == file_result)
    {
        UINT nIndex = 0;
        //Ϊ�˻�ȡ�ļ�����ʼλ�����ַ����е�λ��
        for(int i = 0; argv[2][i] != NULL; i++)
        {
            if(argv[2][i] == L'\\')
            {
                nIndex = i;
            }
        }

        //ֻ����ǰ����ļ�����Ϣ���������ļ���
        wcsncpy(chFullPath, argv[2], nIndex);

        chFullPath[nIndex] = 0;

        //�����ļ��еĳ�ʼ��
        DirectoryInit(chFullPath);

        //�����ļ��ĸ���
        bSucc = (0 != CopyFile(argv[1], argv[2], FALSE));
    }
    //������ļ���
    else if(IS_DIRECTORY == file_result)
    {
        DirectoryInit(argv[2]);  

        //���Ƹ��������в��������ļ���
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
///     �����ļ��еĳ�ʼ����
/// @param[LPCTSTR]
///     lpszFullString - Ҫ�����ļ��е�·��
/// @return
///
///
void DirectoryInit(LPCTSTR lpszFullString)
{
    CString strDirectory, strFileName;
    UINT nSubString = 0;

    //�Է���\Ϊ�ָ����ѭ����ȡ�ļ�������
    while(AfxExtractSubString(strFileName, lpszFullString, nSubString++, L'\\'))
    {
        if(strFileName != L"")
        {
            strDirectory += "\\";
            strDirectory += strFileName;

            //����ļ��в�������ᴴ��
            CreateDirectory(strDirectory, NULL);
        }
    }
}

///
/// @brief
///     �����ļ�·�����ж����ļ������ļ���
/// @param[CString]
///     strFilePath - �ļ���·����
/// @return
///     
///
FILE_RESULT JudgeIsFile(CString strFilePath)
{
    WIN32_FIND_DATA fd;
    //����ҵ����ļ�·������Ч
    if(FindFirstFile(strFilePath, &fd) != INVALID_HANDLE_VALUE)
    {
        //������ļ���
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