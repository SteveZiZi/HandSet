#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "KbdTest.h"
#include "assertMsg.h"

CKbdTest::CKbdTest()
{

}

CKbdTest::~CKbdTest()
{

}

BOOL CKbdTest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    //获取应该读取的配置项目名称
    WCHAR strSection[]=L"KbdTest_\0\0";
    strSection[9]=L'1'+index;

    //读取测试名称
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"按键",m_strTestName,31,strIniFile);

    //初始化要用到的命令索引码
    m_uiCmd=index+CMD_TEST_KEYBORAD;
    //保存CTestManager对象指针
    assertMsg(pManager,L"pManager指针不能为空!");
    m_pManager=pManager;
    return TRUE;
}

TEST_STATUS CKbdTest::OnBeginTest(CConnect *pConnect)
{
    return TEST_NORMAL;
}

TEST_STATUS CKbdTest::OnEndTest(CConnect *pConnect)
{
    return TEST_NORMAL;
}

TEST_STATUS CKbdTest::ExcuteOneTest(CConnect *pConnect)
{
    PACKET      pa;
    int         nRet;
    m_uiTestCount ++;
    nRet=pConnect->SendAPacket(m_uiCmd,0,0,NULL);
    if (nRet <=0)
    {
        return TEST_CONNECTDOWN;
    }
    m_pManager->MessageBox(L"请逐一按下键盘上所有的按键，并注意工控机屏幕上的显示，"
        L"如果所有按键都按下而\"键盘测试\"对话框没有关闭，请手工关闭它",L"提示",MB_OK|MB_ICONINFORMATION);

    nRet=pConnect->RecvAPacket(&pa,150000);
    if (nRet <=0)
    {
        return TEST_CONNECTDOWN;
    }

    DWORD bError =*(DWORD *)pa.pData;

    if (bError)
    {
        m_uiTestErrCount++;
        //记录错误日志
        WCHAR   strLog[512];
        swprintf(strLog,L"存在键盘按键不响应的情况");

        m_pManager->WriteLog(this,strLog);
        return TEST_ERROR;
    }

    return TEST_NORMAL;
}