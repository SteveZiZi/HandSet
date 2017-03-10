#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "assertMsg.h"
#include "SmvTest.h"
CSmvTest::CSmvTest(void)
{
}

CSmvTest::~CSmvTest(void)
{
}

BOOL CSmvTest::Init( const WCHAR *strIniFile, int index,CTestManager *pManager )
{
    //��ȡӦ�ö�ȡ��������Ŀ����
    WCHAR strSection[]=L"UseAppSmvTest_\0\0";
    strSection[14]=L'1'+index;

    //��ȡ��������
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"����̫��1����",m_strTestName,31,strIniFile);

    //��ʼ��Ҫ�õ�������������
    m_uiCmd=index+CMD_TEST_USE_MU1;
    //����CTestManager����ָ��
    assertMsg(pManager,L"pManagerָ�벻��Ϊ��!");
    m_pManager=pManager;
    return TRUE;
}

TEST_STATUS CSmvTest::OnBeginTest( CConnect *pConnect )
{
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
                //����FT3���Գ���
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

TEST_STATUS CSmvTest::OnEndTest( CConnect *pConnect )
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
                //����FT3���Գ���
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

TEST_STATUS CSmvTest::ExcuteOneTest( CConnect *pConnect )
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

    //���Ͳ������� 
    nRet=pConnect->SendAPacket(&pa);

    

    if (nRet >0)
    {
        nRet=pConnect->RecvAPacket(&pa,20000);
        if(nRet >0)
        {
            TEST_INFO_TRANSFER *pInfoT=(TEST_INFO_TRANSFER *)pa.pData;
            if (pInfoT->bError)
            {
                //��ȡ�ļ�����
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
        m_pManager->WriteLog(this,L"�ͻ���ͨ�����൱����ʱ���ڶ�û�л�Ӧ");
    }

    nRet = OnEndTest(pConnect);
    if(nRet == TEST_CONNECTDOWN)
    {
        return TEST_CONNECTDOWN;
    }

    return TEST_NORMAL;
}
