#include "stdafx.h"
#include <stdlib.h>
#include "testBase.h"
#include "DiskTest.h"
#include "iniFile.h"

CDiskTest::CDiskTest():
m_devInitError(TRUE)
{
    wcscpy(m_reportNape[0].strNape,L"每次写入字节");
    wcscpy(m_reportNape[1].strNape,L"测试次数");
    wcscpy(m_reportNape[2].strNape,L"错误次数");
    wcscpy(m_reportNape[3].strNape,L"正确率");
    wcscpy(m_reportNape[4].strNape,L"持续时间");   
}

CDiskTest::~CDiskTest()
{
    //尝试删除所有测试文件
    WCHAR       strTestFile[64];            //磁盘测试过程中读写的文件的含路径名称
    wcsncpy(strTestFile,m_strTestFolder,32);
    strTestFile[m_iTestFolderLen+1]=0;    
    //每测试10次，删除测试文件        
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

    //读取测试项的名字
    int nRet;
     nRet=GetPrivateProfileString(strSection,L"TestName",L"磁盘1",
         m_strTestName,31,strIniFile);    

     //读取要测试的文件夹（磁盘测试专有）
     m_iTestFolderLen=GetPrivateProfileString(strSection,L"TestFolder",L"\0",
         m_strTestFolder,31,strIniFile);

     if (m_strTestFolder[0]==0)
     {
         //读取磁盘测试文件夹失败         
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

    //确定用于查找磁盘文件夹的名字
    wcsncpy(strTmp,m_strTestFolder,32);
    strTmp[m_iTestFolderLen]=L'*';
    strTmp[m_iTestFolderLen+1]=0;

    //查找磁盘文件夹是否存在
    WIN32_FIND_DATA wfd;
    HANDLE hFind;

    hFind=FindFirstFile(strTmp,&wfd);
    if (hFind !=INVALID_HANDLE_VALUE)
    {
        FindClose(hFind);
        m_devInitError=FALSE;
        return TRUE;
    }

    swprintf(m_strLastError,L"未发现%s",m_strTestName );
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
            //如果磁盘依然不可用，则返回错误
            return SITUATION_ERROR;
        }
    }
   

    //在这里进行测试
    int         index=(int)((m_uiTestCount)%10); 

    WCHAR       strTestFile[64];            //磁盘测试过程中读写的文件的含路径名称
    wcsncpy(strTestFile,m_strTestFolder,32);
    strTestFile[m_iTestFolderLen+1]=0;
    if (index ==0)
    {
        //每测试10次，删除测试文件        
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
            //检查读写校验和是否一致
            if (uiWriteVerify !=uiReadVerify)
            {
                swprintf(m_strLastError,L"测试文件%s时，读写校验和不一致",strTestFile);
                m_uiErrCount++;
                return SITUATION_ERROR;
            }
            return SITUATION_NORMAL;
        }
    }

    //执行到这里，说明读写过程出错，下次测试应重新初始化设备
    m_devInitError=TRUE;
    m_uiErrCount++;
 
     return SITUATION_ERROR;
}

BOOL CDiskTest::WriteTestFile(const WCHAR *strTestFile,UINT32 *uiVerifySumWrite)
{   
    register UINT32 uiSum=0;
    HANDLE      hTestFile=INVALID_HANDLE_VALUE;
    //创建测试文件
    DeleteFile(strTestFile);
    hTestFile=CreateFile(strTestFile,GENERIC_WRITE,0,
        NULL,OPEN_ALWAYS,FILE_FLAG_WRITE_THROUGH,INVALID_HANDLE_VALUE);
    
    if (hTestFile==INVALID_HANDLE_VALUE)
    {
        swprintf(m_strLastError,L"创建测试文件%s失败",strTestFile);       
        return FALSE;
    }
  

    //写入文件，并计算出写入数据的校验和
    int     j;
    UINT32 *pui=(UINT32 *)buf;
    BOOL    bSucess =TRUE;
    DWORD  dwReturn =0;

    //生成测试文件数据
    for(j=0;j<sizeof(buf);j++)
    {
        buf[j]=rand();
    }
    //计算校验和
    for (j=0;j<__DISK_TEST_BUF_UINT32_NUM;j++)
    {
        uiSum^=pui[j];
    }
    bSucess=WriteFile(hTestFile,buf,sizeof(buf),&dwReturn,NULL);
    if (bSucess)
    {
        if (dwReturn<sizeof(buf))
        {
            //在这里校验是否写入了足够的字节数
            swprintf(m_strLastError,L"写入文件%s时发现磁盘已满",strTestFile);
            goto Write_Failed;                
        }
    }   

    if (!bSucess)
    {
        swprintf(m_strLastError,L"写入文件%s时出错",strTestFile);
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
    //创建测试文件
    hTestFile=CreateFile(strTestFile,GENERIC_READ,0,
        NULL,OPEN_EXISTING,0,INVALID_HANDLE_VALUE);
    if (hTestFile==INVALID_HANDLE_VALUE)
    {
        swprintf(m_strLastError,L"打开测试文件%s失败",strTestFile);
        return FALSE;
    }

    //读取文件，并计算出读取文件数据的校验和
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

    //计算校验和
    for (j=0;j<__DISK_TEST_BUF_UINT32_NUM;j++)
    {
        uiSum^=pui[j];
    }              
    
    if (!bSucess)
    {
        //读取失败的处理
        swprintf(m_strLastError,L"读取文件%s时出错",strTestFile);
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

    swprintf(m_reportNape[0].strNapeDescriptor,L"%d字节",
        __DISK_TEST_BUF_UINT32_NUM*4 );
    m_reportNape[0].bHighLight=FALSE;  

    swprintf(m_reportNape[1].strNapeDescriptor,L"%d次",m_uiTestCount);
    m_reportNape[1].bHighLight=FALSE;

    swprintf(m_reportNape[2].strNapeDescriptor,L"%d次",m_uiErrCount);    
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

    swprintf(m_reportNape[4].strNapeDescriptor,L"%d时%d分%d秒",h,m,s);
    m_reportNape[4].bHighLight=FALSE;

    return m_reportNape;
}