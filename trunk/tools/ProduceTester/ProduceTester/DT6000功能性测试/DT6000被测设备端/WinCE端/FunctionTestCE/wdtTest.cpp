#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "wdtTest.h"
#include "assertMsg.h"
#include "iniFile.h"
#include "pkfuncs.h"

CWdtTest::CWdtTest()
{

}

CWdtTest::~CWdtTest()
{
   
}

BOOL CWdtTest::Init(const WCHAR *strIniFile, int index,UINT16 *uipCmdIndex,CTestManager *pManager)
{
    //获取应该读取的配置项目名称
    WCHAR strSection[]=L"WdtTest_\0\0";
    strSection[8]=L'1'+index;

    //读取测试名称
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"看门狗",
        m_strTestName,31,strIniFile);

    //初始化要用到的命令索引码
    m_uiCmd=index+CMD_TEST_WATCH_DOG;
    *uipCmdIndex=m_uiCmd;

    return TRUE;
}

TEST_STATUS CWdtTest::ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen, 
                                       const BYTE *pData,CConnect *pConnect)
{
    
    if(!KernelIoControl(IOCTL_HAL_REBOOT, NULL, 0, NULL, 0, NULL))
    {
        return TEST_ERROR;
    }
    else
    {
        return TEST_NORMAL;
    }
    
}