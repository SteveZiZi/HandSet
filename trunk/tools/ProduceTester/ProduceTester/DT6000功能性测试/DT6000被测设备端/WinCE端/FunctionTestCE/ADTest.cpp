#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "ADTest.h"
#include "assertMsg.h"
#include "iniFile.h"

CADTest::~CADTest()
{
    if (m_hInstAdDADll)
    {
        CloseHandle(m_hInstAdDADll);
        m_hInstAdDADll=NULL;
    }
}

BOOL CADTest::Init(const WCHAR *strIniFile, int index,UINT16 *uipCmdIndex,CTestManager *pManager)
{

    //获取应该读取的配置项目名称
    WCHAR strSection[]=L"ADTest_\0\0";
    strSection[7]=L'1'+index;

    //读取测试名称
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"ADC",m_strTestName,31,strIniFile);

    //初始化要用到的命令索引码
    m_uiCmd=index+CMD_TEST_AD;
    *uipCmdIndex=m_uiCmd;

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

    return LoadAdDaDll();
}

BOOL CADTest::LoadAdDaDll()
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

TEST_STATUS CADTest::ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen, 
                                      const BYTE *pData,CConnect *pConnect)
{
    int     nRet;
    DWORD   buf[64];
    DWORD   i;
    BOOL    bErr=FALSE;


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
    WCHAR  *strBuf=m_ti.strErr;
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
    if (bErr )
    {
        m_ti.bError=1;
        m_ti.uiErrorLen=iStrLen+1;
    }
    else
    {
        m_ti.bError=0;
        m_ti.uiErrorLen=0;
    }

    TEST_INFO_TRANSFER tit;

    TestInfoLocalToTransfer(&tit,&m_ti);

    nRet=pConnect->SendAPacket(m_uiCmd,0,
        sizeof(tit),
        (const BYTE *)&tit);

    if (nRet>0)
    {
        return TEST_NORMAL;
    }

    return TEST_CONNECTDOWN;
}