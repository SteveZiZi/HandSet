#include <Windows.h>
#include <atlstr.h>
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "DiskTest.h"
#include "assertMsg.h"

BOOL CDiskTest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    //��ȡӦ�ö�ȡ��������Ŀ����
    WCHAR strSection[]=L"DiskTest_\0\0";
    strSection[9]=L'1'+index;

    //��ȡ��������
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"����1",m_strTestName,31,strIniFile);

    m_bRemoveable=GetPrivateProfileInt(strSection,L"bRemoveable",0,
        strIniFile);
    m_lSlotNum=GetPrivateProfileInt(strSection,L"SlotNum",1,strIniFile);

    if (m_bRemoveable && m_lSlotNum <1)
    {
        pManager->MessageBox(L"���������",L"���̲���",MB_OK|MB_ICONERROR);
        return FALSE;
    }

    //��ʼ��Ҫ�õ�������������
    m_uiCmd=index+CMD_TEST_DISK1;
    //����CTestManager����ָ��
    assertMsg(pManager,L"pManagerָ�벻��Ϊ��!");
    m_pManager=pManager;
    m_lSlotNumTested=0;

    return TRUE;
}

TEST_STATUS CDiskTest::OnBeginTest(CConnect *pConnect)
{
    TEST_STATUS statusRet=TEST_NORMAL;
    int nRet;
    PACKET      pa;
    pa.param=PARAM_GET_INIT_INFO;
    pa.uiCmd=m_uiCmd;
    pa.uiDatalen=0;

    if(m_bRemoveable&&m_lSlotNumTested==0)
    {
        CString  str;
        str.Format(L"�뽫%s����%s�ӿ�1",m_strTestName,m_strTestName);
        m_pManager->MessageBox(str,L"���̲���",MB_OK|MB_ICONINFORMATION);
    }    

    nRet =pConnect->SendAPacket(&pa);
    if (nRet <=0)
    {
        return TEST_CONNECTDOWN;
    }
    
    nRet=pConnect->RecvAPacket(&pa,10000);
    if (nRet <=0)
    {
        return TEST_CONNECTDOWN;
    }

    TEST_INFO *pInfo=(TEST_INFO *)pa.pData;
    if (pInfo->bError)
    {
        m_pManager->MessageBox(pInfo->strErr,L"���̲���",MB_OK|MB_ICONERROR);
        return TEST_ERROR;
    }

    return TEST_NORMAL;
}

TEST_STATUS CDiskTest::OnEndTest(CConnect *pConnect)
{
    TEST_STATUS statusRet=TEST_NORMAL;
    int nRet;
    PACKET      pa;
    pa.param=DISK_TEST_CMD_DELETE;
    pa.uiCmd=m_uiCmd;
    pa.uiDatalen=0;

    m_pManager->showTestStatus(L"����ɾ�������ļ�...");
    nRet =pConnect->SendAPacket(&pa);
    if (nRet <=0)
    {
        return TEST_CONNECTDOWN;
    }

    nRet=pConnect->RecvAPacket(&pa,10000);
    if (nRet <=0)
    {
        return TEST_CONNECTDOWN;
    }

    if (m_lSlotNumTested==m_lSlotNum)
    {
        m_lSlotNumTested=0;
    }

    return TEST_NORMAL;
}

TEST_STATUS CDiskTest::ExcuteOneTest(CConnect *pConnect)
{
    UINT32 uiVerifySumWrite=0,uiVerifySumRead=0;
    int nRet;

    if (m_uiTestCount%3==0 &&m_uiTestCount !=0 )
    {       
        if (m_lSlotNumTested <m_lSlotNum)
        {
            CString str;
            str.Format(L"�뽫%s������һ���ӿڣ��Ѿ�����%d���ӿڣ���ʣ%d���ӿ�",
                m_strTestName,m_lSlotNumTested,m_lSlotNum-m_lSlotNumTested);
            m_pManager->MessageBox(str,L"���̲���",MB_OK|MB_ICONERROR);

            Sleep(2000);
            nRet=OnBeginTest(pConnect);
            if (nRet ==TEST_CONNECTDOWN)
            {
                return TEST_CONNECTDOWN;
            }

            if (nRet ==TEST_ERROR)
            {
                m_uiTestCount++;
                m_uiTestErrCount++;
                return TEST_ABORT;
            }
        }
    }


    m_uiTestCount++;
    m_pManager->showTestStatus(L"���ڴ�����д������ļ�...");
    TEST_STATUS statusRet=TEST_NORMAL;    
    PACKET      pa;
    pa.param=DISK_TEST_CMD_WRITE;
    pa.uiCmd=m_uiCmd;
    pa.uiDatalen=0;

    nRet =pConnect->SendAPacket(&pa);
    if (nRet <=0)
    {
        return TEST_CONNECTDOWN;
    }

    nRet=pConnect->RecvAPacket(&pa,10000);
    if (nRet <=0)
    {
        return TEST_CONNECTDOWN;
    }

    TEST_INFO *pInfo=(TEST_INFO *)pa.pData;
    if (pInfo->bError)
    {
        //����д���ļ�����
        m_pManager->WriteLog(this,pInfo->strErr);
        m_uiTestErrCount++;
        return TEST_ERROR;
    }
    uiVerifySumWrite=*(UINT32 *)pInfo->bAppend;

    m_pManager->showTestStatus(L"���ڶ�ȡ�����ļ�...");
    pa.param=DISK_TEST_CMD_READ;
    pa.uiCmd=m_uiCmd;
    pa.uiDatalen=0;

    nRet =pConnect->SendAPacket(&pa);
    if (nRet <=0)
    {
        return TEST_CONNECTDOWN;
    }

    nRet=pConnect->RecvAPacket(&pa,10000);
    if (nRet <=0)
    {
        return TEST_CONNECTDOWN;
    }

    pInfo=(TEST_INFO *)pa.pData;
    if (pInfo->bError)
    {
        //��ȡ�ļ�����
        m_pManager->WriteLog(this,pInfo->strErr);
        m_uiTestErrCount++;
        return TEST_ERROR;
    }
    uiVerifySumRead=*(UINT32 *)pInfo->bAppend;

    if (uiVerifySumRead !=uiVerifySumWrite)
    {
        //�ļ�У�����
        m_pManager->WriteLog(this,L"�ļ���дУ��Ͳ�һ��");
        m_uiTestErrCount++;
        return TEST_ERROR;
    }

    if (m_uiTestCount%3==0 &&m_uiTestCount !=0 )
    {
        m_lSlotNumTested++;
        if (m_lSlotNumTested <m_lSlotNum)
        {
            nRet=OnEndTest(pConnect);
            if (nRet ==TEST_CONNECTDOWN)
            {
                return TEST_CONNECTDOWN;
            }
        }
    }

    return TEST_NORMAL;
}

int CDiskTest::GetShouldTestTimes()const
{
    if (m_bRemoveable)
    {
        return m_lSlotNum*3;
    }
    return 3;
}
