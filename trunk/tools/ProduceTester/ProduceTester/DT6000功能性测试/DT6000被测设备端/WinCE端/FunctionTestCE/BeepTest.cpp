#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "BeepTest.h"
#include "assertMsg.h"
#include "iniFile.h"

CBeepTest::CBeepTest()
{

}

CBeepTest::~CBeepTest()
{
    if (m_hInstBeepDll )
    {
        CloseHandle(m_hInstBeepDll);
        m_hInstBeepDll=NULL;
    }
}

BOOL CBeepTest::Init(const WCHAR *strIniFile, int index,UINT16 *uipCmdIndex,CTestManager *pManager)
{
    //获取应该读取的配置项目名称
    WCHAR strSection[]=L"BEEPTest_\0\0";
    strSection[9]=L'1'+index;

    //读取测试名称
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"蜂鸣器",m_strTestName,31,strIniFile);

    //初始化要用到的命令索引码
    m_uiCmd=index+CMD_TEST_BEEP;
    *uipCmdIndex=m_uiCmd;

    //载入蜂鸣器操作dll
    m_hInstBeepDll=LoadLibrary(_T("epcBuzzerLib.dll"));
    if (m_hInstBeepDll==NULL)
    {
        MessageBox(NULL,_T("加载epcBuzzerLib.dll失败，请与测试软件的维护人员联系"),
            _T("严重错误"),MB_OK|MB_ICONERROR);
        return FALSE;
    }

    epcBuzzerOn=(BOOL (*)(DWORD) )GetProcAddress(m_hInstBeepDll,TEXT("epcBuzzerOn"));
    epcBuzzerOff=( BOOL (*)(void) ) GetProcAddress(m_hInstBeepDll,TEXT("epcBuzzerOff"));
    epcBuzzerGetStatus=( DWORD  (*) (void) ) GetProcAddress
        (m_hInstBeepDll,TEXT("epcBuzzerGetStatus"));

    if (epcBuzzerOn ==NULL ||
        epcBuzzerOff==NULL ||
        epcBuzzerGetStatus ==NULL)
    {
        return FALSE;
    }

    return TRUE;
}

TEST_STATUS CBeepTest::ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen, const BYTE *pData,CConnect *pConnect)
{
    BYTE    bStatusRet=0;
    if (pData[0])
    {
        epcBuzzerOff();
    }
    else
    {
        epcBuzzerOn(0);
    }

    Sleep(100);

    bStatusRet=(BYTE)epcBuzzerGetStatus();
    int nRet;
    nRet=pConnect->SendAPacket(m_uiCmd,0,sizeof(BYTE),(BYTE *)&bStatusRet);  

    return nRet >0?TEST_NORMAL:TEST_CONNECTDOWN;
}
