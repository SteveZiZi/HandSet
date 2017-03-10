#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "spiFlashTest.h"
#include "assertMsg.h"

BOOL CSpiFlash::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{

    //获取应该读取的配置项目名称
    WCHAR strSection[]=L"SpiFlashTest_\0\0";
    strSection[13]=L'1'+index;

    //读取测试名称
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"SPI Flash",m_strTestName,31,strIniFile);

    //初始化要用到的命令索引码
    m_uiCmd=index+CMD_TEST_SPI_FLASH;
    //保存CTestManager对象指针
    assertMsg(pManager,L"pManager指针不能为空!");
    m_pManager=pManager;

    return TRUE;
}

TEST_STATUS CSpiFlash::ExcuteOneTest(CConnect *pConnect)
{
    BYTE    bStatus=m_uiTestCount%2;
    BYTE    bStatusRet=0;

    PACKET      pa;
    int         nRet;
    nRet=pConnect->SendAPacket(m_uiCmd,m_uiTestCount,0,NULL);
    if (nRet >0)
    {
        nRet=pConnect->RecvAPacket(&pa,100000);
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