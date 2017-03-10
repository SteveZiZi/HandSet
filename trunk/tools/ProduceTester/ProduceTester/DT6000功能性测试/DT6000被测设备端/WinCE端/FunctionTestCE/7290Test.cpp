#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "7290Test.h"
#include "iniFile.h"

C7290Test::C7290Test()
    :m_hTestThread(NULL)
    ,m_pTestDlg(NULL)
{

}

C7290Test::~C7290Test()
{

}

BOOL C7290Test::Init(const WCHAR *strIniFile, int index,UINT16 *uipCmdIndex,CTestManager *pManager)
{
    //获取应该读取的配置项目名称
    WCHAR strSection[]=L"7290Test_\0\0";
    strSection[9]=L'1'+index;

    //读取测试名称
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"ADC",m_strTestName,31,strIniFile);

    //初始化要用到的命令索引码
    m_uiCmd=index+CMD_TEST_7290;
    *uipCmdIndex=m_uiCmd;
    return TRUE;
    
}
#define  PARAM_7290_START       0
#define  PARAM_7290_END         1

TEST_STATUS C7290Test::ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen, 
                                        const BYTE *pData,CConnect *pConnect)
{
    if (param==PARAM_7290_START)
    {
        m_pTestDlg=new C7290TestDlg(NULL);
        m_hTestThread=CreateThread(NULL,0,TestProc,m_pTestDlg,0,NULL);      
        Sleep(500);

        int nRet=pConnect->SendAPacket(m_uiCmd,param,0,NULL);
        if (nRet <=0)
        {
            return TEST_CONNECTDOWN;
        }
        return TEST_NORMAL;
    }
    else
    {
        m_pTestDlg->PostMessage(WM_CLOSE);
        WaitForSingleObject(m_hTestThread,INFINITE);
        CloseHandle(m_hTestThread);
        m_hTestThread=NULL;
        delete m_pTestDlg;
        m_pTestDlg=NULL;

        int nRet=pConnect->SendAPacket(m_uiCmd,param,0,NULL);
        if (nRet <=0)
        {
            return TEST_CONNECTDOWN;
        }
        return TEST_NORMAL;
    }
}

TEST_STATUS C7290Test::OnConnectDown()
{
    if (m_hTestThread !=NULL)
    {
        m_pTestDlg->PostMessage(WM_CLOSE);
        WaitForSingleObject(m_hTestThread,INFINITE);
        CloseHandle(m_hTestThread);
        m_hTestThread=NULL;
        delete m_pTestDlg;
        m_pTestDlg=NULL;
    }
    return TEST_NORMAL;
}

DWORD C7290Test::TestProc(LPVOID pVoid)
{
    C7290TestDlg *pDlg=(C7290TestDlg *)pVoid;
    pDlg->DoModal();

    return 0;
}