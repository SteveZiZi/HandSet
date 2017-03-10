#include <windows.h>
#include <atlstr.h>
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "UseAppTest.h"
#include "assertMsg.h"

CUseAppTest::CUseAppTest()
{
    m_bShowInfo=FALSE;
}


BOOL CUseAppTest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    //获取应该读取的配置项目名称
    WCHAR strSection[]=L"UseAppTest_\0\0";
    strSection[11]=L'1'+index;

    //读取测试名称
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"ISA",m_strTestName,31,strIniFile);

    m_bShowInfo=GetPrivateProfileInt(strSection,L"bShowInformation",0,strIniFile);
    if (m_bShowInfo)
    {
        GetPrivateProfileString(strSection,L"ShowInformation",L"123",
            m_wszInfo,256,strIniFile);
    }
    GetPrivateProfileString(strSection,L"AskQuestion",L"123",m_wszQuestion,256,strIniFile);
    m_bAskQuesion=GetPrivateProfileInt(strSection,L"bAskQuestion",1,strIniFile);
    m_dwCorrectExitCode= GetPrivateProfileInt(strSection,L"CorrectExitCode",0,strIniFile);

    //初始化要用到的命令索引码
    m_uiCmd=index+0;
    //保存CTestManager对象指针
    assertMsg(pManager,L"pManager指针不能为空!");
    m_pManager=pManager;

    return TRUE;
}

TEST_STATUS CUseAppTest::OnBeginTest(CConnect *pConnect)
{
    return TEST_NORMAL;
}

TEST_STATUS CUseAppTest::OnEndTest(CConnect *pConnect)
{ 
    return TEST_NORMAL;
}

TEST_STATUS CUseAppTest::ExcuteOneTest(CConnect *pConnect)
{
    PACKET      pa;
    int         nRet;
    nRet=pConnect->SendAPacket(m_uiCmd,0,0,NULL);
    if (nRet <=0)
    {
        return TEST_CONNECTDOWN;
    }

    if (m_bShowInfo)
    {
        m_pManager->MessageBox(m_wszInfo,L"提示",MB_OK|MB_ICONINFORMATION);
    }

    nRet=pConnect->RecvAPacket(&pa,20000);
    if (nRet <=0)
    {
        return TEST_CONNECTDOWN;
    }
    m_uiTestCount++;
    if (pa.param)
    {
        //客户端demo启动正常
        if(m_bAskQuesion)
        {
            int ret=m_pManager->MessageBox(m_wszQuestion,m_strTestName,MB_YESNO|MB_ICONQUESTION);
            if (ret !=IDYES)
            {
                m_uiTestErrCount++;
            }
        }
        //客户端demo程序运行完毕，命令客户端结束程序的运行
        nRet=pConnect->SendAPacket(m_uiCmd,1,0,NULL);
        if (nRet <=0)
        {
            return TEST_CONNECTDOWN;
        }
        nRet=pConnect->RecvAPacket(&pa,20000);
        if (nRet <=0)
        {
            return TEST_CONNECTDOWN;
        }

        if (!m_bAskQuesion)
        {
            DWORD   dwExitCode=0;
            dwExitCode = *((DWORD *)pa.pData);
            if (dwExitCode!=m_dwCorrectExitCode)
            {
                m_uiTestErrCount++;
                CString     str;
                str.Format(L"the demo\'s exit code is %d",dwExitCode);
                m_pManager->WriteLog(this,str);
                return TEST_ERROR;
            }
        }


    }
    else
    {
        m_pManager->MessageBox(L"客户端程序启动失败",m_strTestName,MB_ICONQUESTION|MB_OK);
        m_uiTestErrCount++;
        return TEST_ERROR;
    }

    return TEST_NORMAL;
}