#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "wdtTest.h"
#include "assertMsg.h"

BOOL CWdtTest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    //获取应该读取的配置项目名称
    WCHAR strSection[]=L"WdtTest_\0\0";
    strSection[9]=L'8'+index;

    //读取测试名称
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"看门狗",m_strTestName,31,strIniFile);

    //初始化要用到的命令索引码
    m_uiCmd=index+CMD_TEST_WATCH_DOG;
    //保存CTestManager对象指针
    assertMsg(pManager,L"pManager指针不能为空!");
    m_pManager=pManager;

    return TRUE;
}

TEST_STATUS CWdtTest::ExcuteOneTest(CConnect *pConnect)
{
    int         nRet;
    nRet=pConnect->SendAPacket(m_uiCmd,0,0,NULL);
    if(nRet > 0)
    {
        m_uiTestCount++;
        Sleep(1000);
        int ret=m_pManager->MessageBox(L"设备是否关机了？",L"看门狗测试"
            ,MB_YESNO|MB_ICONQUESTION) ;

        if (ret != IDYES)
        {
            m_uiTestErrCount++;
            return TEST_ERROR;
        }
        return TEST_NORMAL;
    }
    else
    {
        return TEST_CONNECTDOWN;
    }
        
}