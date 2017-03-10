#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "AudioTest.h"
#include "FunctionTest.h"
#include "testManager.h"
#include "assertMsg.h"
#include "AudioTestDlg.h"

CAudioTest::CAudioTest():
m_bTestRecord(TRUE)
{

}

CAudioTest::~CAudioTest()
{

}

BOOL CAudioTest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    //获取应该读取的配置项目名称
    WCHAR strSection[]=L"AudioTest_\0\0";
    strSection[10]=L'1'+index;

    //读取测试名称
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"音频",m_strTestName,31,strIniFile);
    m_bTestRecord=GetPrivateProfileInt(strSection,L"TestRecord",
        1,strIniFile);

    //初始化要用到的命令索引码
    m_uiCmd=index+CMD_TEST_AUDIO;
    //保存CTestManager对象指针
    assertMsg(pManager,L"pManager指针不能为空!");
    m_pManager=pManager;
    return TRUE;

    return TRUE;
}

int  CAudioTest::GetShouldTestTimes()const
{
    if (m_bTestRecord)
    {
        return 2;
    }
    return 1;
}

TEST_STATUS CAudioTest::OnBeginTest(CConnect *pConnect)
{
    return TEST_NORMAL;
}

TEST_STATUS CAudioTest::OnEndTest(CConnect *pConnect)
{
    return TEST_NORMAL;
}


TEST_STATUS CAudioTest::ExcuteOneTest(CConnect *pConnect)
{
    UINT16      param=PARAM_BEGIN_PLAY;
    int         nRet;

    PACKET      pa;
    if (m_uiTestCount ==0)
    {
        if (m_bTestRecord)
        {
            param=PARAM_BEGIN_RECORD;
        }
    }
    if (param ==PARAM_BEGIN_PLAY)
    {
        m_pManager->MessageBox(L"下面测试放音,请注意倾听耳机或喇叭是否有声音",
            L"音频测试",MB_OK);
    }
    else
    {
        m_pManager->MessageBox(L"下面测试录音,请注对准咪头说话",
            L"音频测试",MB_OK);
    }


    //启动工控板上的录音或者放音
    nRet =pConnect->SendAPacket(m_uiCmd,param,0,NULL);
    if (nRet >0)
    {
        nRet =pConnect->RecvAPacket(&pa,10000);
        if (nRet >0)
        {
            TEST_INFO_TRANSFER *pInfoT=(TEST_INFO_TRANSFER *)pa.pData;
            if (pInfoT ->bError)
            {
                TEST_INFO ti;
                TestInfoTransferToLocal(&ti,pInfoT);
                m_pManager->MessageBox(ti.strErr,L"音频测试",MB_OK|MB_ICONERROR);
                return TEST_ABORT;
            }
        }
    }

    if (nRet <=0)
    {
        return TEST_CONNECTDOWN;
    }   

    //显示录音或放音的过程信息
    m_uiTestCount++;
    CAudioTestDlg dlg(CWnd::FromHandle(m_pManager->GetMainWnd()));
    dlg.uiParamCmd= param==PARAM_BEGIN_PLAY?PARAM_GET_POS_PLAY:PARAM_GET_POS_RECORD;
    dlg.uiCmd=m_uiCmd;
    dlg.pConnect=pConnect;

    dlg.DoModal();

    if (dlg.status ==TEST_CONNECTDOWN)
    {
        return TEST_CONNECTDOWN;
    }

    if (dlg.status ==TEST_ERROR)
    {
        m_uiTestErrCount++;
    }
  
    param++;
    nRet =pConnect->SendAPacket(m_uiCmd,param,0,NULL);
    if (nRet >0)
    {
        nRet =pConnect->RecvAPacket(&pa,10000);
        if (nRet >0)
        {
            return dlg.status;
        }
    }

    return TEST_CONNECTDOWN;
}