#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "KbdTest.h"
#include "assertMsg.h"

CKbdTest::CKbdTest()
{

}

CKbdTest::~CKbdTest()
{

}

BOOL CKbdTest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    //��ȡӦ�ö�ȡ��������Ŀ����
    WCHAR strSection[]=L"KbdTest_\0\0";
    strSection[9]=L'1'+index;

    //��ȡ��������
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"����",m_strTestName,31,strIniFile);

    //��ʼ��Ҫ�õ�������������
    m_uiCmd=index+CMD_TEST_KEYBORAD;
    //����CTestManager����ָ��
    assertMsg(pManager,L"pManagerָ�벻��Ϊ��!");
    m_pManager=pManager;
    return TRUE;
}

TEST_STATUS CKbdTest::OnBeginTest(CConnect *pConnect)
{
    return TEST_NORMAL;
}

TEST_STATUS CKbdTest::OnEndTest(CConnect *pConnect)
{
    return TEST_NORMAL;
}

TEST_STATUS CKbdTest::ExcuteOneTest(CConnect *pConnect)
{
    PACKET      pa;
    int         nRet;
    m_uiTestCount ++;
    nRet=pConnect->SendAPacket(m_uiCmd,0,0,NULL);
    if (nRet <=0)
    {
        return TEST_CONNECTDOWN;
    }
    m_pManager->MessageBox(L"����һ���¼��������еİ�������ע�⹤�ػ���Ļ�ϵ���ʾ��"
        L"������а��������¶�\"���̲���\"�Ի���û�йرգ����ֹ��ر���",L"��ʾ",MB_OK|MB_ICONINFORMATION);

    nRet=pConnect->RecvAPacket(&pa,150000);
    if (nRet <=0)
    {
        return TEST_CONNECTDOWN;
    }

    DWORD bError =*(DWORD *)pa.pData;

    if (bError)
    {
        m_uiTestErrCount++;
        //��¼������־
        WCHAR   strLog[512];
        swprintf(strLog,L"���ڼ��̰�������Ӧ�����");

        m_pManager->WriteLog(this,strLog);
        return TEST_ERROR;
    }

    return TEST_NORMAL;
}