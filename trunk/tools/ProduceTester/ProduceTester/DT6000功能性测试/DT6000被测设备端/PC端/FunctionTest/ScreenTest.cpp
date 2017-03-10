#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "ScreenTest.h"
#include "assertMsg.h"

BOOL CSreenTest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    //获取应该读取的配置项目名称
    WCHAR strSection[]=L"ScreenTest_\0\0";
    strSection[11]=L'1'+index;

    //读取测试名称
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"屏幕",m_strTestName,31,strIniFile);

    //初始化要用到的命令索引码
    m_uiCmd=index+CMD_TEST_SCREEN;
    //保存CTestManager对象指针
    assertMsg(pManager,L"pManager指针不能为空!");
    m_pManager=pManager;
    return TRUE;
}

static const WCHAR *strQuestion[]={L"工控板屏幕色阶显示是否正常？",
                                    L"工控板屏幕图像显示是否正常?"};


TEST_STATUS CSreenTest::ExcuteOneTest(CConnect *pConnect)
{
    PACKET      pa;
    int         nRet;   

    //发送测试命令   
    nRet=pConnect->SendAPacket(m_uiCmd,SCREEN_PARAM_TEST_COLOR_LEVEL+m_uiTestCount,
        0,NULL );
   
    if (nRet >0)
    {
        nRet=pConnect->RecvAPacket(&pa,10000);
        if(nRet >0)
        {
            Sleep(500);
            int nRet=m_pManager->MessageBox(strQuestion[m_uiTestCount],
                        L"测试",MB_YESNO|MB_ICONQUESTION);
             m_uiTestCount++;
            if (nRet ==IDNO)
            {
                m_uiTestErrCount++;
                return TEST_ERROR;
            }

            return TEST_NORMAL;            
        }
    }

    if (nRet == 0)
    {
        m_pManager->WriteLog(this,L"客户端通信在相当长的时间内都没有回应");
    }
    else
    {
        m_pManager->WriteLog(this,L"连接通信出现错误");
    }

    return TEST_CONNECTDOWN;

}

TEST_STATUS CSreenTest::OnBeginTest(CConnect *pConnect)
{
    PACKET          pa;
    int             nRet;

    nRet=pConnect->SendAPacket(m_uiCmd,SCREEN_PARAM_INIT,0,NULL);
    if (nRet >0)
    {
        nRet =pConnect->RecvAPacket(&pa,10000);
        if (nRet >0)
        {
            TEST_INFO *pInfo=(TEST_INFO *)pa.pData;
            if (pInfo->bError)
            {
                m_pManager->WriteLog(this,pInfo->strErr);
                return TEST_ERROR;
            }
            return TEST_NORMAL;
        }
    }

    if (nRet ==0)
    {
        m_pManager->WriteLog(this,L"客户端通信在相当长的时间内都没有回应");
    }
    else
    {
        m_pManager->WriteLog(this,L"连接通信出现错误");
    }
    return TEST_CONNECTDOWN;
}

TEST_STATUS CSreenTest::OnEndTest(CConnect *pConnect)
{
    PACKET          pa;
    int             nRet;

    nRet=pConnect->SendAPacket(m_uiCmd,SCREEN_PARAM_END,0,NULL);
    if (nRet >0)
    {
        nRet =pConnect->RecvAPacket(&pa,10000);
        if (nRet >0)
        {
            TEST_INFO *pInfo=(TEST_INFO *)pa.pData;
            if (pInfo->bError)
            {
                m_pManager->WriteLog(this,pInfo->strErr);
                return TEST_ERROR;
            }
            Sleep(500);
            return TEST_NORMAL;
        }
    }

    if (nRet ==0)
    {
        m_pManager->WriteLog(this,L"客户端通信在相当长的时间内都没有回应");
    }
    else
    {
        m_pManager->WriteLog(this,L"连接通信出现错误");
    }
    return TEST_CONNECTDOWN;
}
