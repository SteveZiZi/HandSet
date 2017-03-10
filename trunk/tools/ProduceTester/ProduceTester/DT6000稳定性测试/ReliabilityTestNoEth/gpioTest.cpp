#include "stdafx.h"
#include "testBase.h"
#include "testManager.h"
#include "gpioTest.h"
#include "iniFile.h"


CGpioTest::CGpioTest():
epcIoRead(NULL),
epcIoWrite(NULL),
m_hInstGpioDll(NULL)
{
    m_strTestName[0]=0;
    wcscpy(m_reportNape[0].strNape,L"测试次数");
    wcscpy(m_reportNape[1].strNape,L"错误次数");
    wcscpy(m_reportNape[2].strNape,L"正确率");
    wcscpy(m_reportNape[3].strNape,L"持续时间");
}

CGpioTest::~CGpioTest()
{
    if (m_hInstGpioDll !=NULL)
    {
        CloseHandle(m_hInstGpioDll);
        m_hInstGpioDll=NULL;
    }
    epcIoRead=NULL;
    epcIoWrite=NULL;
}

BOOL CGpioTest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    WCHAR strSection[]=L"GPIOTest_\0\0";
    strSection[9]=L'1'+index;

    //读取测试项的名字
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"GPIO",
        m_strTestName,31,strIniFile); 
    //载入gpio操作dll
    m_hInstGpioDll=LoadLibrary(_T("epcIoLib.dll"));
    if (m_hInstGpioDll==NULL)
    {
        MessageBox(NULL,_T("加载epcIoLib.dll失败，请与测试软件的维护人员联系"),
            _T("严重错误"),MB_OK|MB_ICONERROR);
        return FALSE;
    }
    m_iInPinNum=GetPrivateProfileInt(strSection,L"InPinNum",8,strIniFile);
    m_iOutPinNum=GetPrivateProfileInt(strSection,L"OutPinNum",8,strIniFile);
    epcIoRead=(int ( *)(DWORD *))GetProcAddress(m_hInstGpioDll,TEXT("epcIoRead"));
    epcIoWrite=(int (*)(DWORD)) GetProcAddress(m_hInstGpioDll,TEXT("epcIoWrite"));
    if (epcIoRead ==NULL)
    {
        return FALSE;
    }
    if (epcIoWrite ==NULL)
    {
        return FALSE;
    }

    return TRUE;
}

TEST_SITUATION      CGpioTest::ExcuteOneTest()
{
    TEST_SITUATION  siRet=SITUATION_NORMAL; 
    DWORD dwWrite=rand();
    DWORD dwRead=0;
    DWORD dwWriteMask=-1;
    DWORD dwReadMask=-1;
    DWORD dwMask=-1;

    dwWriteMask =dwWriteMask <<m_iOutPinNum;
    dwWriteMask =~dwWriteMask;
    dwReadMask =dwReadMask <<m_iInPinNum;
    dwReadMask =~dwReadMask;

    dwWrite &=dwWriteMask;
    (*epcIoWrite)(dwWrite); 
    Sleep(5);
    epcIoRead(&dwRead); 
    dwRead &=dwReadMask;

    int     t1=m_iInPinNum,t;
    DWORD   dwTmp,dwReadTmp=dwRead;

    BOOL bError=FALSE;
    while(t1 >0)
    {
       if (t1 >=m_iOutPinNum)
       {
           t=m_iOutPinNum;
           t1-=m_iOutPinNum;
       }
       else
       {
           t=t1;
           t1=0;
       }

       dwMask=-1;
       dwMask =dwMask<<t;
       dwMask=~dwMask;
       dwTmp=dwRead &dwMask;
       dwRead =dwRead>>m_iOutPinNum;
       
       if (dwTmp !=(dwWrite &dwMask))
       {
           bError=TRUE;
       }
    }

    //校正读出和写入的值

    if (bError)
    {
        siRet=SITUATION_ERROR;
        swprintf(m_strLastError,
            L"读写校验失败:写入0x%02x，读出0x%02x",dwWrite,dwReadTmp );
    }
    else
    {
        siRet=SITUATION_NORMAL;
    }

    m_uiTestCount++;
    if (siRet !=SITUATION_NORMAL)
    {
        m_uiErrCount++;
    }

    return siRet;
  
}

const SReportNape *CGpioTest::GetReportNapes(int &count)
{
    count =4;
    unsigned int h,m,s;
    UINT64 t=m_uiTestTime;

    swprintf(m_reportNape[0].strNapeDescriptor,L"%d次",m_uiTestCount);
    m_reportNape[0].bHighLight=FALSE;

    swprintf(m_reportNape[1].strNapeDescriptor,L"%d次",m_uiErrCount);    
    if (m_uiTestCount >0)
    {
        swprintf(m_reportNape[2].strNapeDescriptor,L"%.2f%% ",
            100 *(double)(m_uiTestCount-m_uiErrCount)/m_uiTestCount );
    }
    else
    {
        swprintf(m_reportNape[2].strNapeDescriptor,L"0.0%% ");
    }

    if (m_uiErrCount >0)
    {
        m_reportNape[1].bHighLight=TRUE;
        m_reportNape[2].bHighLight=TRUE;
    }
    else
    {
        m_reportNape[1].bHighLight=FALSE;
        m_reportNape[2].bHighLight=FALSE;
    }  

    h= (unsigned int)( t/(1000*3600));
    t=t%(1000*3600);
    m=(unsigned int) (t/(1000 *60));
    t=t%(1000 *60);
    s=(unsigned int)(t/1000);

    swprintf(m_reportNape[3].strNapeDescriptor,L"%d时%d分%d秒",h,m,s);
    m_reportNape[3].bHighLight=FALSE;

    return m_reportNape;
}