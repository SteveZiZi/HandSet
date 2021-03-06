#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "assertMsg.h"
#include "BcodeTest.h"

const WCHAR * strQuestion[] = {L"是否已将DT6000 RX2上的ST线拔出，并连接到IRIG_B码接口上？"};

CBcodeTest::CBcodeTest(void)
{
}

CBcodeTest::~CBcodeTest(void)
{
}

BOOL CBcodeTest::Init( const WCHAR *strIniFile, int index,CTestManager *pManager )
{
    //获取应该读取的配置项目名称
    WCHAR strSection[]=L"UseAppBcodeTest_\0\0";
    strSection[16]=L'1'+index;

    //读取测试名称
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"IRIG_B校时测试",m_strTestName,31,strIniFile);

    //初始化要用到的命令索引码
    m_uiCmd=index+CMD_TEST_USE_B_CODE;
    //保存CTestManager对象指针
    assertMsg(pManager,L"pManager指针不能为空!");
    m_pManager=pManager;
    return TRUE;
}

TEST_STATUS CBcodeTest::OnBeginTest( CConnect *pConnect )
{
    if(0 == m_uiTestCount)
    {
        int nRet=m_pManager->MessageBox(strQuestion[0],
            L"测试",MB_YESNO|MB_ICONQUESTION);
        if (nRet == IDNO)
        {
            return TEST_ERROR;
        }
    }
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

TEST_STATUS CBcodeTest::OnEndTest( CConnect *pConnect )
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

TEST_STATUS CBcodeTest::ExcuteOneTest( CConnect *pConnect )
{
    PACKET      pa;
    int nRet;
    pa.param=FRAME_TEST_CMD_EXT;
    pa.uiCmd=m_uiCmd;
    pa.uiDatalen=0;

    m_uiTestCount++;

    if(m_uiTestCount == 0)
    {
        nRet = TEST_NORMAL;
    }
    else
    {
        nRet=OnBeginTest(pConnect);
    }

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
        nRet=pConnect->RecvAPacket(&pa,20000);
        if(nRet >0)
        {
            TEST_INFO_TRANSFER *pInfoT=(TEST_INFO_TRANSFER *)pa.pData;
            if (pInfoT->bError)
            {
                //读取文件出错
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
        return TEST_CONNECTDOWN;
    }

    if (nRet == 0)
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
