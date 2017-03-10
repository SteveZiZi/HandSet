#include "stdafx.h"
#include "testBase.h"
#include "testManager.h"
#include "ADDATest.h"
#include "iniFile.h"

CAdDaTest::CAdDaTest()
    :m_hInstAdDADll(NULL)
{

}

CAdDaTest::~CAdDaTest()
{
    if (m_hInstAdDADll !=NULL)
    {
        epcAdDaCloseDriver();
        CloseHandle(m_hInstAdDADll);
        m_hInstAdDADll=NULL;
    }
}

BOOL CAdDaTest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    WCHAR strSection[]=L"ADDATest_\0\0";
    strSection[9]=L'1'+index;

    //读取测试项的名字
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"AD/DA",
        m_strTestName,31,strIniFile); 

    //读取AD的通道数
    m_dwADChannelNum=GetPrivateProfileInt(strSection,L"ADChannelNum",0,
        strIniFile);    
    if (m_dwADChannelNum<=0)
    {
        MessageBox(NULL,L"无可用的ADC通道",L"错误",MB_OK|MB_ICONERROR);
        return FALSE;
    }
    //读取AD测试的参考数值
    m_dwTestValue=GetPrivateProfileInt(strSection,L"TestValue",0,
        strIniFile);
    //读取AD/DA输入输出最大允许的值
    m_dwMaxValue=GetPrivateProfileInt(strSection,L"AD_DAMaxValue",0,
        strIniFile);
    //读取DA的通道数
    m_dwDAChannelNum=GetPrivateProfileInt(strSection,L"DAChannelNum",0,
        strIniFile);
    if (m_dwDAChannelNum >0)
    {
        m_dwTestValue=0;
    }

    m_pManager = pManager;
    return LoadAdDaDll();
}

BOOL CAdDaTest::LoadAdDaDll()
{
    //加载AD/DA动态库
    m_hInstAdDADll=LoadLibrary(_T("AD_DAOperation.dll"));
    if (m_hInstAdDADll ==NULL)
    {
        MessageBox(NULL,_T("加载AD_DAOperation.dll失败，请与测试软件的维护人员联系"),
            _T("严重错误"),MB_OK|MB_ICONERROR);
        return FALSE;
    }
    //获取各函数的地址
    epcAdDaOpenDriver=(BOOL (*)())GetProcAddress(m_hInstAdDADll,_T("epcAdDaOpenDriver"));
    epcAdDaCloseDriver=( void (*)() )
        GetProcAddress(m_hInstAdDADll,_T("epcAdDaCloseDriver"));

    epcAdGetSampleValue=( BOOL (*)(DWORD *,DWORD,DWORD) )
        GetProcAddress(m_hInstAdDADll,_T("epcAdGetSampleValue"));
    epcDASetOutputValue=( BOOL (*) (DWORD *,DWORD,DWORD) )
        GetProcAddress(m_hInstAdDADll,_T("epcDASetOutputValue"));

    if (!(epcAdDaOpenDriver && epcAdDaCloseDriver &&
        epcAdGetSampleValue  &&epcDASetOutputValue) )
    {
        MessageBox(NULL,L"AD_DAOperation.dll版本不匹配",L"错误",MB_OK|MB_ICONERROR);
        CloseHandle(m_hInstAdDADll);
        m_hInstAdDADll=NULL;
        return FALSE;
    }

    if (! epcAdDaOpenDriver() )
    {
        MessageBox(NULL,L"打开AD/DA驱动失败",L"错误",MB_OK|MB_ICONERROR);
        CloseHandle(m_hInstAdDADll);
        m_hInstAdDADll=NULL;
        return FALSE;
    }

    return TRUE;
}

TEST_SITUATION CAdDaTest::ExcuteOneTest()
{   
    DWORD   buf[64];
    DWORD   i;
    BOOL    bErr=FALSE;

    TEST_SITUATION      siRet=SITUATION_NORMAL;
    if (m_dwDAChannelNum <=0)
    {
        //只有AD

    }
    else
    {
        //既有AD，又有DA 
        //置DA输出为统一的值
        m_dwTestValue+=100;
        if (m_dwTestValue >=m_dwMaxValue)
        {
            m_dwTestValue=0;
        }

        DWORD dwValue[64];
        for (DWORD i=0;i<m_dwDAChannelNum;i++)
        {
            dwValue[i]=m_dwTestValue;
        }
        epcDASetOutputValue(dwValue,0,m_dwDAChannelNum-1);
    }



    epcAdGetSampleValue(buf,0,m_dwADChannelNum-1);
    WCHAR  *strBuf=m_strLastError;
    int     iStrLen=0;

    for (i=0;i<m_dwADChannelNum;i++)
    {
        if ( abs( (int )(buf[i]-m_dwTestValue) )  >(int ) (m_dwMaxValue)/20 )
        {
            iStrLen+=swprintf(strBuf+iStrLen,L"通道%d:0x%x",i,buf[i]);
            bErr=TRUE;
        }
    }
    iStrLen+=swprintf(strBuf+iStrLen,L"预计的ADC采样值为0x%x",m_dwTestValue);

    return bErr?SITUATION_ERROR:SITUATION_NORMAL;
}

const SReportNape *CAdDaTest::GetReportNapes(int &count)
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