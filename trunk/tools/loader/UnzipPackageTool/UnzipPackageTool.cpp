// UnzipPackageTool.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <afxwin.h>
#include <windows.h>
#include <commctrl.h>
#include "tar.h"

///< ɾ���ļ��ı��
const WCHAR m_gDeleteSign[] = L"-d";

bool UnPackage( const TCHAR *, const TCHAR *);
void DirectoryInit(LPCTSTR);
bool JudgeToZip(const LPCTSTR, const LPCTSTR);

int _tmain(int argc, _TCHAR* argv[])
{
    //���û�д�������������˳�
    if(argc < 2) return 0;

    bool bDelete = FALSE;
    
    //���ֻ�������������������������������һ��������"-d"
    if(argc == 2 || (argc == 3 && 
        (bDelete = (wcscmp(argv[2], m_gDeleteSign) == 0 ))))
    {
        TCHAR tchTemp[256];

        //�����ַ���
        wcscpy(tchTemp, argv[1]);

        //��ȡѹ���ļ��ĸ�Ŀ¼
        for(int i = wcslen(tchTemp); i >= 0; i--)
        {
            if(tchTemp[i] == '\\')
            {
                tchTemp[i] = 0;
                break;
            }
        }
        
        //��������Ͻ�ѹ�������˳�����
        if(!JudgeToZip(argv[1], tchTemp))
        {
            return 0;
        }

        //�����ļ���ѹ
        bool bSucc = UnPackage(tchTemp, argv[1]); 
        int a = 0;

    }
    //��������������ĸ��������
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

    //���ݵõ��ı�Ǿ����Ƿ�ɾ��Դ�ļ�
    if(bDelete)
    {
        DeleteFile(argv[1]);
    }

	return 0;
}


///
/// @brief
///     �жϽ�ѹ�ļ��Ƿ�����Լ���ѹ���Ŀ���ļ������Ƿ��㹻
/// @param[const LPCTSTR]
///     srcFile - Ҫ��ѹ��Դ�ļ�
/// @param[const LPCTSTR]
///     dstPath - ��ѹ����ļ����·��
/// @return
///     ������Լ�����ѹ����TRUE�������Ͻ�ѹ��������FALSE
///
bool JudgeToZip(const LPCTSTR srcFile, const LPCTSTR dstPath)
{
    int nTryTimes = 0;
    bool isExist = FALSE;

    //ѭ��3��������ѹ�ļ��Ƿ����
    while(nTryTimes++ < 3)
    {
        //�ж�Ҫ��ѹ��Դ�ļ��Ƿ����
        isExist = (::GetFileAttributes(srcFile) != 0xFFFFFFFF);

        //����Ѿ�������ѹ�ļ����˳�ѭ��
        if(isExist)
        {
            break;
        }

        //���û��⵽��˯��1s��������
        Sleep(1000);
    }

    //������������˳�
    if(!isExist)
    {
        return FALSE;
    }
    
    ULARGE_INTEGER iToalFreeBytes;

    //��ȡĿ���ļ�·����ʣ������
    if(!GetDiskFreeSpaceEx(dstPath, NULL, NULL, &iToalFreeBytes))
    {
        return FALSE;
    }

    //�����ѹ��Ŀ��·��ʣ������С��100M�򲻽��н�ѹ����
    if(iToalFreeBytes.LowPart < 1024 * 1024 * 100
        && iToalFreeBytes.HighPart == 0)
    {
        return FALSE;
    }

    return TRUE;
}


///
/// @brief
///     ��Դ�ļ�·�����н�ѹ��ŵ�Ŀ���ļ�·��
/// @param[const TCHAR * ]
///     desPath - ��ѹ����ļ����·��
/// @param[const TCHAR * ]
///     srcFile - ��Ҫ������ļ�
/// @return
///     �����ѹ�ɹ�����TRUE�����򷵻�FALSE
///
bool UnPackage( const TCHAR * desPath, const TCHAR * srcFile)
{
    //�����ѹ��ʽΪ.zip���ļ��ɹ�
    if(unzip(desPath, srcFile) == 0)
    {
        return TRUE;
    }

    //�����ѹ��ʽΪ.tar���ļ��ɹ�
    else if(untar(desPath, srcFile) == 0)
    {
        return TRUE;
    }

    //�����ѹ��ʽΪ.tar��.tgz��.tar.gz���ļ��ɹ�
    else if(untgz(desPath, srcFile) == 0)
    {
        return TRUE;
    }

    //����ļ���������������ʽ�򷵻�FALSE
    else 
    {
        return FALSE;
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
