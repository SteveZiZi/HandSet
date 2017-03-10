#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "LedTest.h"
#include "assertMsg.h"

BOOL CLedTest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    //��ȡӦ�ö�ȡ��������Ŀ����
    WCHAR strSection[]=L"LedTest_\0\0";
    strSection[8]=L'1'+index;

    //��ȡ��������
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"LED",m_strTestName,31,strIniFile);
    m_iLedNum=GetPrivateProfileInt(strSection,L"LedNum",1,strIniFile);

    //��ʼ��Ҫ�õ�������������
    m_uiCmd=index+CMD_TEST_LED;
    //����CTestManager����ָ��
    assertMsg(pManager,L"pManagerָ�벻��Ϊ��!");
    m_pManager=pManager;

    return TRUE;
}

TEST_STATUS CLedTest::ExcuteOneTest(CConnect *pConnect)
{
    int         nRet;
    PACKET      pa;

    nRet=pConnect->SendAPacket(m_uiCmd,0,0,NULL);
    if (nRet >0)
    {
        nRet =pConnect->RecvAPacket(&pa,5000);
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

    return TEST_CONNECTDOWN;
}

TEST_STATUS CLedTest::OnEndTest(CConnect *pConnect)
{
    CString str;
    str.Format(L"�Ƿ񿴵����ػ�����%d��LED�ƽ�����˸ ?",m_iLedNum);
    int nRet=m_pManager->MessageBox(str,
        L"LED����",MB_YESNO|MB_ICONQUESTION);

    if (nRet !=IDYES)
    {
        m_uiTestErrCount++;
    }
    return TEST_NORMAL;
}