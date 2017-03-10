#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "ScreenTest.h"
#include "assertMsg.h"

BOOL CSreenTest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    //��ȡӦ�ö�ȡ��������Ŀ����
    WCHAR strSection[]=L"ScreenTest_\0\0";
    strSection[11]=L'1'+index;

    //��ȡ��������
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"��Ļ",m_strTestName,31,strIniFile);

    //��ʼ��Ҫ�õ�������������
    m_uiCmd=index+CMD_TEST_SCREEN;
    //����CTestManager����ָ��
    assertMsg(pManager,L"pManagerָ�벻��Ϊ��!");
    m_pManager=pManager;
    return TRUE;
}

static const WCHAR *strQuestion[]={L"���ذ���Ļɫ����ʾ�Ƿ�������",
                                    L"���ذ���Ļͼ����ʾ�Ƿ�����?"};


TEST_STATUS CSreenTest::ExcuteOneTest(CConnect *pConnect)
{
    PACKET      pa;
    int         nRet;   

    //���Ͳ�������   
    nRet=pConnect->SendAPacket(m_uiCmd,SCREEN_PARAM_TEST_COLOR_LEVEL+m_uiTestCount,
        0,NULL );
   
    if (nRet >0)
    {
        nRet=pConnect->RecvAPacket(&pa,10000);
        if(nRet >0)
        {
            Sleep(500);
            int nRet=m_pManager->MessageBox(strQuestion[m_uiTestCount],
                        L"����",MB_YESNO|MB_ICONQUESTION);
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
        m_pManager->WriteLog(this,L"�ͻ���ͨ�����൱����ʱ���ڶ�û�л�Ӧ");
    }
    else
    {
        m_pManager->WriteLog(this,L"����ͨ�ų��ִ���");
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
        m_pManager->WriteLog(this,L"�ͻ���ͨ�����൱����ʱ���ڶ�û�л�Ӧ");
    }
    else
    {
        m_pManager->WriteLog(this,L"����ͨ�ų��ִ���");
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
        m_pManager->WriteLog(this,L"�ͻ���ͨ�����൱����ʱ���ڶ�û�л�Ӧ");
    }
    else
    {
        m_pManager->WriteLog(this,L"����ͨ�ų��ִ���");
    }
    return TEST_CONNECTDOWN;
}
