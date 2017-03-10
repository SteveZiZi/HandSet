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
    //��ȡӦ�ö�ȡ��������Ŀ����
    WCHAR strSection[]=L"AudioTest_\0\0";
    strSection[10]=L'1'+index;

    //��ȡ��������
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"��Ƶ",m_strTestName,31,strIniFile);
    m_bTestRecord=GetPrivateProfileInt(strSection,L"TestRecord",
        1,strIniFile);

    //��ʼ��Ҫ�õ�������������
    m_uiCmd=index+CMD_TEST_AUDIO;
    //����CTestManager����ָ��
    assertMsg(pManager,L"pManagerָ�벻��Ϊ��!");
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
        m_pManager->MessageBox(L"������Է���,��ע�����������������Ƿ�������",
            L"��Ƶ����",MB_OK);
    }
    else
    {
        m_pManager->MessageBox(L"�������¼��,��ע��׼��ͷ˵��",
            L"��Ƶ����",MB_OK);
    }


    //�������ذ��ϵ�¼�����߷���
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
                m_pManager->MessageBox(ti.strErr,L"��Ƶ����",MB_OK|MB_ICONERROR);
                return TEST_ABORT;
            }
        }
    }

    if (nRet <=0)
    {
        return TEST_CONNECTDOWN;
    }   

    //��ʾ¼��������Ĺ�����Ϣ
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