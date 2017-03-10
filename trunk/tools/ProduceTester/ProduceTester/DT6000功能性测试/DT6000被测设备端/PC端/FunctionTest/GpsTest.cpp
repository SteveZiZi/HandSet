#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "GpsTest.h"
#include "assertMsg.h"

CGpsTest::CGpsTest()
{

}

CGpsTest::~CGpsTest()
{

}

BOOL CGpsTest::Init(const WCHAR *strIniFile, int index,
                    CTestManager *pManager)
{
    //��ȡӦ�ö�ȡ��������Ŀ����
    WCHAR strSection[]=L"GPSTest_\0\0";
    strSection[8]=L'1'+index;

    //��ȡ��������
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"GPS",m_strTestName,31,strIniFile);

    //��ʼ��Ҫ�õ�������������
    m_uiCmd=index+CMD_TEST_GPS;
    //����CTestManager����ָ��
    assertMsg(pManager,L"pManagerָ�벻��Ϊ��!");
    m_pManager=pManager;

    return TRUE;
}

TEST_STATUS CGpsTest::ExcuteOneTest(CConnect *pConnect)
{
    int         nRet;
    PACKET      pa;

    nRet=pConnect->SendAPacket(m_uiCmd,0,0,NULL);
    if (nRet >0)
    {
        nRet =pConnect->RecvAPacket(&pa,30000);
        if (nRet >0)
        {
            m_uiTestCount++;
            TEST_INFO_TRANSFER *pInfoT=(TEST_INFO_TRANSFER *)pa.pData;
            TEST_INFO ti;
            TestInfoTransferToLocal(&ti,pInfoT);
            if (pInfoT ->bError)
            {
                
                m_pManager->WriteLog(this,ti.strErr);
                m_pManager->MessageBox(ti.strErr,NULL,MB_OK|MB_ICONERROR);
                m_uiTestErrCount++;
                return TEST_ERROR;
            }

            if (ti.uiErrorLen >0)
            {
                m_pManager->WriteLog(this,ti.strErr);
                m_pManager->MessageBox(ti.strErr,NULL,MB_OK|MB_ICONERROR);
            }

            return TEST_NORMAL;
        }
    }

    return TEST_CONNECTDOWN;
}