#include "stdafx.h"
#include <stdlib.h>
#include "testBase.h"
#include "DiskTest.h"
#include "iniFile.h"

CDiskTest::CDiskTest():
m_devInitError(TRUE)
{
    wcscpy(m_reportNape[0].strNape,L"ÿ��д���ֽ�");
    wcscpy(m_reportNape[1].strNape,L"���Դ���");
    wcscpy(m_reportNape[2].strNape,L"�������");
    wcscpy(m_reportNape[3].strNape,L"��ȷ��");
    wcscpy(m_reportNape[4].strNape,L"����ʱ��");   
}

CDiskTest::~CDiskTest()
{
    //����ɾ�����в����ļ�
    WCHAR       strTestFile[64];            //���̲��Թ����ж�д���ļ��ĺ�·������
    wcsncpy(strTestFile,m_strTestFolder,32);
    strTestFile[m_iTestFolderLen+1]=0;    
    //ÿ����10�Σ�ɾ�������ļ�        
    for(int i=0;i<10;i++)
    {
        strTestFile[m_iTestFolderLen]=L'0'+i;
        DeleteFile(strTestFile);
    } 
}

BOOL CDiskTest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    WCHAR strSection[]=L"DiskTest_\0\0";
    strSection[9]=L'1'+index;

    //��ȡ�����������
    int nRet;
     nRet=GetPrivateProfileString(strSection,L"TestName",L"����1",
         m_strTestName,31,strIniFile);    

     //��ȡҪ���Ե��ļ��У����̲���ר�У�
     m_iTestFolderLen=GetPrivateProfileString(strSection,L"TestFolder",L"\0",
         m_strTestFolder,31,strIniFile);

     if (m_strTestFolder[0]==0)
     {
         //��ȡ���̲����ļ���ʧ��         
         return FALSE;
     }   

     m_devInitError=TRUE;
     m_pManager = pManager;
     srand(GetTickCount());
     return TRUE;
}

BOOL CDiskTest::CheckDiskAvaliable()
{
    WCHAR   strTmp[64];

    //ȷ�����ڲ��Ҵ����ļ��е�����
    wcsncpy(strTmp,m_strTestFolder,32);
    strTmp[m_iTestFolderLen]=L'*';
    strTmp[m_iTestFolderLen+1]=0;

    //���Ҵ����ļ����Ƿ����
    WIN32_FIND_DATA wfd;
    HANDLE hFind;

    hFind=FindFirstFile(strTmp,&wfd);
    if (hFind !=INVALID_HANDLE_VALUE)
    {
        FindClose(hFind);
        m_devInitError=FALSE;
        return TRUE;
    }

    swprintf(m_strLastError,L"δ����%s",m_strTestName );
    m_devInitError=TRUE;
    return FALSE;
}


TEST_SITUATION CDiskTest::ExcuteOneDiskTest()
{
    if (m_devInitError)
    {
        CheckDiskAvaliable();
        if (m_devInitError)
        {
            Sleep(1000);
            //���������Ȼ�����ã��򷵻ش���
            return SITUATION_ERROR;
        }
    }
   

    //��������в���
    int         index=(int)((m_uiTestCount)%10); 

    WCHAR       strTestFile[64];            //���̲��Թ����ж�д���ļ��ĺ�·������
    wcsncpy(strTestFile,m_strTestFolder,32);
    strTestFile[m_iTestFolderLen+1]=0;
    if (index ==0)
    {
        //ÿ����10�Σ�ɾ�������ļ�        
        for(int i=0;i<10;i++)
        {
            strTestFile[m_iTestFolderLen]=L'0'+i;
            DeleteFile(strTestFile);
        }
        strTestFile[m_iTestFolderLen]=0;
    }


    strTestFile[m_iTestFolderLen]=L'0'+index;

    m_uiTestCount++;
    UINT32 uiWriteVerify=0,uiReadVerify=0;
    
    if (WriteTestFile(strTestFile,&uiWriteVerify))
    {
        if (ReadTestFile(strTestFile,&uiReadVerify) )
        {
            //����дУ����Ƿ�һ��
            if (uiWriteVerify !=uiReadVerify)
            {
                swprintf(m_strLastError,L"�����ļ�%sʱ����дУ��Ͳ�һ��",strTestFile);
                m_uiErrCount++;
                return SITUATION_ERROR;
            }
            return SITUATION_NORMAL;
        }
    }

    //ִ�е����˵����д���̳����´β���Ӧ���³�ʼ���豸
    m_devInitError=TRUE;
    m_uiErrCount++;
 
     return SITUATION_ERROR;
}

BOOL CDiskTest::WriteTestFile(const WCHAR *strTestFile,UINT32 *uiVerifySumWrite)
{   
    register UINT32 uiSum=0;
    HANDLE      hTestFile=INVALID_HANDLE_VALUE;
    //���������ļ�
    DeleteFile(strTestFile);
    hTestFile=CreateFile(strTestFile,GENERIC_WRITE,0,
        NULL,OPEN_ALWAYS,FILE_FLAG_WRITE_THROUGH,INVALID_HANDLE_VALUE);
    
    if (hTestFile==INVALID_HANDLE_VALUE)
    {
        swprintf(m_strLastError,L"���������ļ�%sʧ��",strTestFile);       
        return FALSE;
    }
  

    //д���ļ����������д�����ݵ�У���
    int     j;
    UINT32 *pui=(UINT32 *)buf;
    BOOL    bSucess =TRUE;
    DWORD  dwReturn =0;

    //���ɲ����ļ�����
    for(j=0;j<sizeof(buf);j++)
    {
        buf[j]=rand();
    }
    //����У���
    for (j=0;j<__DISK_TEST_BUF_UINT32_NUM;j++)
    {
        uiSum^=pui[j];
    }
    bSucess=WriteFile(hTestFile,buf,sizeof(buf),&dwReturn,NULL);
    if (bSucess)
    {
        if (dwReturn<sizeof(buf))
        {
            //������У���Ƿ�д�����㹻���ֽ���
            swprintf(m_strLastError,L"д���ļ�%sʱ���ִ�������",strTestFile);
            goto Write_Failed;                
        }
    }   

    if (!bSucess)
    {
        swprintf(m_strLastError,L"д���ļ�%sʱ����",strTestFile);
        goto Write_Failed;
    }

    *uiVerifySumWrite=uiSum;
    CloseHandle(hTestFile);
    return TRUE;

Write_Failed:
    CloseHandle(hTestFile);
    return FALSE;
}

BOOL CDiskTest::ReadTestFile(const WCHAR *strTestFile,UINT32 *uiVerifySumRead)
{
    register UINT32 uiSum=0;
    HANDLE      hTestFile=INVALID_HANDLE_VALUE;
    //���������ļ�
    hTestFile=CreateFile(strTestFile,GENERIC_READ,0,
        NULL,OPEN_EXISTING,0,INVALID_HANDLE_VALUE);
    if (hTestFile==INVALID_HANDLE_VALUE)
    {
        swprintf(m_strLastError,L"�򿪲����ļ�%sʧ��",strTestFile);
        return FALSE;
    }

    //��ȡ�ļ������������ȡ�ļ����ݵ�У���
    int     j;
    UINT32 *pui=(UINT32 *)buf;
    BOOL    bSucess =TRUE;
    DWORD  dwReturn =0;

    bSucess=ReadFile(hTestFile,buf,sizeof(buf),&dwReturn,NULL);
    if (bSucess)
    {
        if (dwReturn<sizeof(buf))
        {
            bSucess=FALSE;
        }
    }

    //����У���
    for (j=0;j<__DISK_TEST_BUF_UINT32_NUM;j++)
    {
        uiSum^=pui[j];
    }              
    
    if (!bSucess)
    {
        //��ȡʧ�ܵĴ���
        swprintf(m_strLastError,L"��ȡ�ļ�%sʱ����",strTestFile);
        CloseHandle(hTestFile);
        return FALSE;
    }

    CloseHandle(hTestFile);
    *uiVerifySumRead=uiSum;
    return TRUE;
}

const SReportNape *CDiskTest::GetReportNapes(int &count)
{
    unsigned int h,m,s;
    UINT64 t=m_uiTestTime;
    count=5;

    swprintf(m_reportNape[0].strNapeDescriptor,L"%d�ֽ�",
        __DISK_TEST_BUF_UINT32_NUM*4 );
    m_reportNape[0].bHighLight=FALSE;  

    swprintf(m_reportNape[1].strNapeDescriptor,L"%d��",m_uiTestCount);
    m_reportNape[1].bHighLight=FALSE;

    swprintf(m_reportNape[2].strNapeDescriptor,L"%d��",m_uiErrCount);    
    if (m_uiTestCount >0)
    {
        swprintf(m_reportNape[3].strNapeDescriptor,L"%.2f%% ",
            100 *(double)(m_uiTestCount-m_uiErrCount)/m_uiTestCount );
    }
    else
    {
        swprintf(m_reportNape[3].strNapeDescriptor,L"0.0%% ");
    }

    if (m_uiErrCount >0)
    {
        m_reportNape[2].bHighLight=TRUE;
        m_reportNape[3].bHighLight=TRUE;
    }
    else
    {
        m_reportNape[2].bHighLight=FALSE;
        m_reportNape[3].bHighLight=FALSE;
    }  

    h= (unsigned int)( t/(1000*3600));
    t=t%(1000*3600);
    m=(unsigned int) (t/(1000 *60));
    t=t%(1000 *60);
    s=(unsigned int)(t/1000);

    swprintf(m_reportNape[4].strNapeDescriptor,L"%dʱ%d��%d��",h,m,s);
    m_reportNape[4].bHighLight=FALSE;

    return m_reportNape;
}