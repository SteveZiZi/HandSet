#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "gpioTest.h"
#include "iniFile.h"
#include "epcIoLib.h"


CGpioTest::CGpioTest()
{

}

CGpioTest::~CGpioTest()
{
}

BOOL CGpioTest::Init(const WCHAR *strIniFile, int index,UINT16 *uipCmdIndex,CTestManager *pManager)
{
    WCHAR strSection[]=L"GPIOTest_\0\0";
    strSection[9]=L'1'+index;

    //读取测试项的名字
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"GPIO",
        m_strTestName,31,strIniFile); 
    //载入gpio操作dll
    if (!LoadEpcIoLibrary() )
    {
        MessageBox(NULL,L"加载epcIoLib.dll失败",L"串口",MB_OK);
        return FALSE;
    }    
    m_uiCmd=index+CMD_TEST_GPIO;
    *uipCmdIndex=m_uiCmd;
    return TRUE;
}

TEST_STATUS CGpioTest::ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen, 
                                        const BYTE *pData,CConnect *pConnect)
{
    UINT32 uiOut;
    memcpy(&uiOut,pData,sizeof(uiOut) );
    epcIoWrite((DWORD)uiOut );
    Sleep(5);
    DWORD  dwIn=0;
    epcIoRead(&dwIn);
    
    UINT32 uiIn=(UINT32)dwIn ;

    int nRet=pConnect->SendAPacket(m_uiCmd,param,sizeof(UINT32 ),(BYTE *)&uiIn);
    if (nRet <=0)
    {
        return TEST_ERROR;
    }

    return TEST_NORMAL;
}