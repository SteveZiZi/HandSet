#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "LedTest.h"
#include "assertMsg.h"

BOOL CLedTest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    //获取应该读取的配置项目名称
    WCHAR strSection[]=L"LedTest_\0\0";
    strSection[8]=L'1'+index;

    //读取测试名称
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"LED",m_strTestName,31,strIniFile);
    m_iLedNum=GetPrivateProfileInt(strSection,L"LedNum",1,strIniFile);

    //初始化要用到的命令索引码
    m_uiCmd=index+CMD_TEST_LED;
    //保存CTestManager对象指针
    assertMsg(pManager,L"pManager指针不能为空!");
    m_pManager=pManager;

    return TRUE;
}

TEST_STATUS CLedTest::ExcuteOneTest(CConnect *pConnect)
{
    int         nRet;
    PACKET      pa;

    nRet=pConnect->SendAPacket(m_uiCmd,0,0,NULL);
    if (nRet >0)
    {
        nRet =pConnect->RecvAPacket(&pa,5000);
        if (nRet >0)
        {
            m_uiTestCount++;
            TEST_INFO_TRANSFER *pInfoT=(TEST_INFO_TRANSFER *)pa.pData;
            if (pInfoT ->bError)
            {
                TEST_INFO ti;
                TestInfoTransferToLocal(&ti,pInfoT);
                m_pManager->WriteLog(this,ti.strErr);
                m_uiTestErrCount++;
                return TEST_ERROR;
            }
            
            return TEST_NORMAL;
        }
    }

    return TEST_CONNECTDOWN;
}

TEST_STATUS CLedTest::OnEndTest(CConnect *pConnect)
{
    CString str;
    str.Format(L"是否看到工控机板上%d个LED灯交替闪烁 ?",m_iLedNum);
    int nRet=m_pManager->MessageBox(str,
        L"LED测试",MB_YESNO|MB_ICONQUESTION);

    if (nRet !=IDYES)
    {
        m_uiTestErrCount++;
    }
    return TEST_NORMAL;
}