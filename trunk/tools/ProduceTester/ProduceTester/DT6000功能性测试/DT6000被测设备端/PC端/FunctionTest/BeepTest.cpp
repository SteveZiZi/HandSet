#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "BeepTest.h"
#include "assertMsg.h"

CBeepTest::CBeepTest()
{

}

CBeepTest::~CBeepTest()
{

}

BOOL CBeepTest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    //获取应该读取的配置项目名称
    WCHAR strSection[]=L"BEEPTest_\0\0";
    strSection[9]=L'1'+index;

    //读取测试名称
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"蜂鸣器",m_strTestName,31,strIniFile);

    //初始化要用到的命令索引码
    m_uiCmd=index+CMD_TEST_BEEP;
    //保存CTestManager对象指针
    assertMsg(pManager,L"pManager指针不能为空!");
    m_pManager=pManager;

    return TRUE;
}

TEST_STATUS CBeepTest::ExcuteOneTest(CConnect *pConnect)
{
    BYTE    bStatus=m_uiTestCount%2;
    BYTE    bStatusRet=0;

    PACKET      pa;
    int         nRet;
    nRet=pConnect->SendAPacket(m_uiCmd,0,sizeof(BYTE),(BYTE *)&bStatus);
    if (nRet >0)
    {
        nRet=pConnect->RecvAPacket(&pa,100000);
        if(nRet >0)
        {
            bStatusRet=pa.pData[0]; 
            m_uiTestCount++;
            if (bStatus !=bStatusRet)
            {
                m_uiTestErrCount++;
                m_pManager->WriteLog(this,L"蜂鸣器状态校验错误");
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

TEST_STATUS CBeepTest::OnBeginTest(CConnect *pConnect)
{    
    return TEST_NORMAL;
}

TEST_STATUS CBeepTest::OnEndTest(CConnect *pConnect)
{  
    m_uiTestCount++;

    int nRet=m_pManager->MessageBox(_T("是否听到工控板蜂鸣器响声?"),_T("提问"),
        MB_YESNO|MB_ICONQUESTION);

    if (nRet !=IDYES)
    {
        m_uiTestErrCount++;
        m_pManager->WriteLog(this,L"工控板蜂鸣器没有响");
    }

    return TEST_NORMAL;
}

