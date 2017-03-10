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
    //��ȡӦ�ö�ȡ��������Ŀ����
    WCHAR strSection[]=L"BEEPTest_\0\0";
    strSection[9]=L'1'+index;

    //��ȡ��������
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"������",m_strTestName,31,strIniFile);

    //��ʼ��Ҫ�õ�������������
    m_uiCmd=index+CMD_TEST_BEEP;
    //����CTestManager����ָ��
    assertMsg(pManager,L"pManagerָ�벻��Ϊ��!");
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
                m_pManager->WriteLog(this,L"������״̬У�����");
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

TEST_STATUS CBeepTest::OnBeginTest(CConnect *pConnect)
{    
    return TEST_NORMAL;
}

TEST_STATUS CBeepTest::OnEndTest(CConnect *pConnect)
{  
    m_uiTestCount++;

    int nRet=m_pManager->MessageBox(_T("�Ƿ��������ذ����������?"),_T("����"),
        MB_YESNO|MB_ICONQUESTION);

    if (nRet !=IDYES)
    {
        m_uiTestErrCount++;
        m_pManager->WriteLog(this,L"���ذ������û����");
    }

    return TEST_NORMAL;
}

