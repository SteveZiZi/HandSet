#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "assertMsg.h"
#include "Ft3TxTest.h"

CFt3TxTest::CFt3TxTest(void)
{
}

CFt3TxTest::~CFt3TxTest(void)
{
}

BOOL CFt3TxTest::Init( const WCHAR *strIniFile, int index,CTestManager *pManager )
{
    //获取应该读取的配置项目名称
    WCHAR strSection[]=L"UseAppCFt3TxTest_\0\0";
    strSection[16]=L'1'+index;

    //读取测试名称
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"FT3_TX发送",m_strTestName,31,strIniFile);

    //初始化要用到的命令索引码
    m_uiCmd=index+CMD_TEST_USE_FT3_TX1;
    //保存CTestManager对象指针
    assertMsg(pManager,L"pManager指针不能为空!");
    m_pManager=pManager;
    return TRUE;
}

TEST_STATUS CFt3TxTest::OnBeginTest( CConnect *pConnect )
{
    PACKET      pa;
    pa.param=FRAME_TEST_CMD_INI;
    pa.uiCmd=m_uiCmd;
    pa.uiDatalen=0;
    int nRet=pConnect->SendAPacket(&pa);

    if(nRet > 0)
    {
        nRet=pConnect->RecvAPacket(&pa,10000);
        if(nRet >0)
        {

            TEST_INFO *pInfo = (TEST_INFO *)pa.pData;
            if (pInfo->bError)
            {
                //结束FT3测试出错
                m_pManager->WriteLog(this,pInfo->strErr);
                return TEST_ERROR;
            }
            return TEST_NORMAL;            
        }
    }
    else
    {
        return TEST_CONNECTDOWN;
    }
}

TEST_STATUS CFt3TxTest::OnEndTest( CConnect *pConnect )
{
    PACKET      pa;
    pa.param=FRAME_TEST_CMD_END;
    pa.uiCmd=m_uiCmd;
    pa.uiDatalen=0;
    int nRet=pConnect->SendAPacket(&pa);
    if(nRet > 0)
    {
        nRet=pConnect->RecvAPacket(&pa,10000);
        if(nRet >0)
        {

            TEST_INFO *pInfo = (TEST_INFO *)pa.pData;
            if (pInfo->bError)
            {
                //结束FT3测试出错
                m_pManager->WriteLog(this,pInfo->strErr);
                return TEST_ERROR;
            }
            return TEST_NORMAL;            
        }
    }
    else
    {
        return TEST_CONNECTDOWN;
    }
}

TEST_STATUS CFt3TxTest::ExcuteOneTest( CConnect *pConnect )
{
    PACKET      pa;
    int nRet;
    pa.param=FRAME_TEST_CMD_EXT;
    pa.uiCmd=m_uiCmd;
    pa.uiDatalen=0;

    m_uiTestCount++;
    nRet=OnBeginTest(pConnect);
    if (nRet == TEST_CONNECTDOWN)
    {
        return TEST_CONNECTDOWN;
    }

    if (nRet ==TEST_ERROR)
    {
        m_uiTestErrCount++;
        return TEST_ABORT;
    }

    //发送测试命令 
    nRet=pConnect->SendAPacket(&pa);


    if (nRet >0)
    {
        nRet=pConnect->RecvAPacket(&pa,10000);
        if(nRet >0)
        {

            TEST_INFO_TRANSFER *pInfoT=(TEST_INFO_TRANSFER *)pa.pData;
            if (pInfoT->bError)
            {
               
                TEST_INFO ti;
                TestInfoTransferToLocal(&ti,pInfoT);
                m_pManager->WriteLog(this,ti.strErr);
                m_uiTestErrCount++;
                OnEndTest(pConnect);
                return TEST_ERROR;
            }


        }
        else
        {
            return TEST_CONNECTDOWN;
        }
    }
    else
    {
        TEST_CONNECTDOWN;
    }

    if (nRet ==0)
    {
        m_pManager->WriteLog(this,L"客户端通信在相当长的时间内都没有回应");
    }

    nRet = OnEndTest(pConnect);
    if(nRet == TEST_CONNECTDOWN)
    {
        return TEST_CONNECTDOWN;
    }

    return TEST_NORMAL;
}
