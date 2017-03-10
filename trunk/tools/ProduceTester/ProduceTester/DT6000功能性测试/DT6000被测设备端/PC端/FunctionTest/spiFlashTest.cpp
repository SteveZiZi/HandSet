#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "spiFlashTest.h"
#include "assertMsg.h"

BOOL CSpiFlash::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{

    //��ȡӦ�ö�ȡ��������Ŀ����
    WCHAR strSection[]=L"SpiFlashTest_\0\0";
    strSection[13]=L'1'+index;

    //��ȡ��������
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"SPI Flash",m_strTestName,31,strIniFile);

    //��ʼ��Ҫ�õ�������������
    m_uiCmd=index+CMD_TEST_SPI_FLASH;
    //����CTestManager����ָ��
    assertMsg(pManager,L"pManagerָ�벻��Ϊ��!");
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
        m_pManager->WriteLog(this,L"�ͻ���ͨ�����൱����ʱ���ڶ�û�л�Ӧ");
    }
    else
    {
        m_pManager->WriteLog(this,L"����ͨ�ų��ִ���");
    }

    return TEST_CONNECTDOWN;
}