#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "wdtTest.h"
#include "assertMsg.h"

BOOL CWdtTest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    //��ȡӦ�ö�ȡ��������Ŀ����
    WCHAR strSection[]=L"WdtTest_\0\0";
    strSection[9]=L'8'+index;

    //��ȡ��������
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"���Ź�",m_strTestName,31,strIniFile);

    //��ʼ��Ҫ�õ�������������
    m_uiCmd=index+CMD_TEST_WATCH_DOG;
    //����CTestManager����ָ��
    assertMsg(pManager,L"pManagerָ�벻��Ϊ��!");
    m_pManager=pManager;

    return TRUE;
}

TEST_STATUS CWdtTest::ExcuteOneTest(CConnect *pConnect)
{
    int         nRet;
    nRet=pConnect->SendAPacket(m_uiCmd,0,0,NULL);
    if(nRet > 0)
    {
        m_uiTestCount++;
        Sleep(1000);
        int ret=m_pManager->MessageBox(L"�豸�Ƿ�ػ��ˣ�",L"���Ź�����"
            ,MB_YESNO|MB_ICONQUESTION) ;

        if (ret != IDYES)
        {
            m_uiTestErrCount++;
            return TEST_ERROR;
        }
        return TEST_NORMAL;
    }
    else
    {
        return TEST_CONNECTDOWN;
    }
        
}