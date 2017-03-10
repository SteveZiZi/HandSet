#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "RelayTest.h"
#include "assertMsg.h"
#include "iniFile.h"

CRelayTest::CRelayTest()
    :m_iRelayNum(0)
    ,m_iOpenInputNum(0)
    ,epcRelayInit(NULL)
    ,epcRelayOff(NULL)
    ,epcRelayOn(NULL)
    ,epcGetOpenInputState(NULL)
{

}

BOOL CRelayTest::Init(const WCHAR *strIniFile, int index, UINT16 *uipCmdIndex,CTestManager *pManager/* =NULL */)
{
    WCHAR strSection[]=L"RelayTest_\0\0";
    strSection[10]=L'1'+index;

    //��ȡ�����������
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"�̵���",
        m_strTestName,31,strIniFile); 
    
    m_iRelayNum = GetPrivateProfileInt(strSection,L"RelayNum",0,strIniFile);
    m_iOpenInputNum = GetPrivateProfileInt(strSection,L"OpenInputNum",0,strIniFile);

    //��ʼ��Ҫ�õ�������������
    m_uiCmd=index+CMD_TEST_RELAY;
    *uipCmdIndex =m_uiCmd;
    m_pTestManager = pManager;

    if (LoadRelayTestLib())
    {
        if (epcRelayInit())
        {
            return TRUE;
        }
        MessageBox(NULL,L"��ʼ���̵���ʧ��",m_strTestName,MB_OK|MB_ICONERROR);
    }

    return FALSE;

}

BOOL CRelayTest::LoadRelayTestLib()
{
    HMODULE      hInst =NULL;

    hInst = LoadLibrary(L"epcRelayTestLib.dll");
    if (hInst == NULL)
    {
        MessageBox(NULL,L"����epcRelayTestLib.dllʧ��",m_strTestName,MB_OK|MB_ICONERROR);
        return FALSE;
    }

    //��ȡ�������ĵ�ַ
    epcRelayInit=(BOOL (*)())GetProcAddress(hInst,_T("epcRelayInit"));
    epcRelayOn=( BOOL (*)(DWORD) )
        GetProcAddress(hInst,_T("epcRelayOn"));

    epcRelayOff=( BOOL (__cdecl *)(DWORD) )
        GetProcAddress(hInst,_T("epcRelayOff"));
    epcGetOpenInputState=( DWORD (*)(DWORD) )
        GetProcAddress(hInst,_T("epcGetOpenInputState"));

    if (!(epcRelayInit && epcRelayOn &&
        epcRelayOff  &&epcGetOpenInputState) )
    {
        MessageBox(NULL,L"epcRelayTestLib.dll",L"����",MB_OK|MB_ICONERROR);
        CloseHandle(hInst);
        return FALSE;
    } 

    return TRUE;
}

TEST_STATUS CRelayTest::ProcessExcuteCmd(UINT16 param,
                                         UINT16 uiDatalen, const BYTE *pData,CConnect *pConnect)
{
    
    int nRet;
    

    TEST_INFO_TRANSFER tit;
    tit.bError =0;
    tit.uiErrorLen =0;

    DWORD   dwStatus =0,dwStatus1 =0;

    for (unsigned int i=0;i<m_iRelayNum;i++)
    {
        epcRelayOff(i);
    }
    
    Sleep(500);

    for (unsigned int i=0;i<m_iOpenInputNum;i++)
    {
        dwStatus |= epcGetOpenInputState(i)<<i;
    }

    for (unsigned int i=0;i<m_iRelayNum;i++)
    {
        epcRelayOn(i);
    }
    Sleep(500);

    for (unsigned int i=0;i<m_iOpenInputNum;i++)
    {
        dwStatus1 |= epcGetOpenInputState(i)<<i;
    }

    if (dwStatus1!=0 ||(dwStatus !=((1<<m_iRelayNum)-1) ) )
    {
        TEST_INFO           ti;
        ti.bError =1;
        ti.uiErrorLen =swprintf(ti.strErr,L"�̵���״̬У��ʧ��,��:0x%x,��0x%x",dwStatus,dwStatus1);
        TestInfoLocalToTransfer(&tit,&ti);
    }


   

    nRet=pConnect->SendAPacket(m_uiCmd,0,
        sizeof(tit),
        (const BYTE *)&tit);

    if (nRet>0)
    {
        return TEST_NORMAL;
    }

    return TEST_CONNECTDOWN;



    return TEST_NORMAL;
}
