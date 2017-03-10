#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "SerialPortTest.h"
#include "ledTest.h"
#include "assertMsg.h"
#include "iniFile.h"

CLedTest::CLedTest():
m_hInstLedDll(NULL),
epcGetLedMask(NULL),
epcSetLedMask(NULL),
m_dwTestCount(0)
{
}

CLedTest::~CLedTest()
{
    if (m_hInstLedDll!=NULL)
    {
        CloseHandle(m_hInstLedDll);
        m_hInstLedDll=NULL;
    }
}

BOOL CLedTest::Init(const WCHAR *strIniFile, int index,UINT16 *uipCmdIndex,CTestManager *pManager)
{
    WCHAR strSection[]=L"LEDTest_\0\0";
    strSection[8]=L'1'+index;

    //读取测试项的名字
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"LED",
        m_strTestName,31,strIniFile); 
    //载入gpio操作dll
    m_hInstLedDll=LoadLibrary(_T("epcLedTestLib.dll"));
    if (m_hInstLedDll==NULL)
    {
        MessageBox(NULL,_T("加载epcLedTestLib.dll失败，请与测试软件的维护人员联系"),
            _T("严重错误"),MB_OK|MB_ICONERROR);
        return FALSE;
    }  
    m_iLedNum=GetPrivateProfileInt(strSection,L"LedNum",1,strIniFile);

    epcSetLedMask=(void(*)(DWORD) ) GetProcAddress(m_hInstLedDll,TEXT("epcSetLedMask"));
    epcGetLedMask=( void (*)(DWORD *) )GetProcAddress(m_hInstLedDll,
                                                    _T("epcGetLedMask"));

    if (epcSetLedMask ==NULL)
    {
        return FALSE;
    }
    if (epcGetLedMask ==NULL)
    {
        return FALSE;
    }

    m_uiCmd=index+CMD_TEST_LED;
    *uipCmdIndex=m_uiCmd;
    return TRUE;
}

#define  LED1   1
#define  LED2   2
TEST_STATUS CLedTest::ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen, 
                                       const BYTE *pData,CConnect *pConnect)
{
    DWORD   dwStatus=0;
    BOOL    bError=FALSE;

    m_ti.bError=0;
    m_ti.uiErrorLen=0;

    epcSetLedMask(1<<m_dwTestCount);   
    Sleep(250);
    epcGetLedMask(&dwStatus);
    if (dwStatus != 1 <<m_dwTestCount)
    {
        m_ti.bError =1;
        m_ti.uiErrorLen = swprintf(m_ti.strErr,L"LED状态校验失败");

    }
    epcSetLedMask(0);
    Sleep(200);    
    

     m_dwTestCount = (m_dwTestCount+1)%m_iLedNum;
    TEST_INFO_TRANSFER tit;
    TestInfoLocalToTransfer(&tit,&m_ti);
    int nRet;
    nRet =pConnect->SendAPacket(m_uiCmd,0,sizeof(tit),(BYTE *)&tit);
    
    return nRet >0?TEST_NORMAL:TEST_CONNECTDOWN;

}