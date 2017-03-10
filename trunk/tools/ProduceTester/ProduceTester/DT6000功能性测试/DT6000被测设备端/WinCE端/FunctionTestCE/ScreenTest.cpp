#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "ScreenTest.h"
#include "iniFile.h"
#include "resource.h"
#include "FullScreenDlg.h"


CScreenTest::CScreenTest():
m_pFullDlgWnd(NULL)
{

}

CScreenTest::~CScreenTest()
{
    if (m_pFullDlgWnd)
    {
        ((CFullScreenDlg *)m_pFullDlgWnd)->SendMessage(WM_DESTROY);
        
        delete  ( (CFullScreenDlg *)m_pFullDlgWnd);
    }
}




BOOL CScreenTest::Init(const WCHAR *strIniFile, int index,UINT16 *uipCmdIndex,CTestManager *pManager)
{
    WCHAR strSection[]=L"ScreenTest_\0\0";
    strSection[9]=L'1'+index;

    //读取测试项的名字
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"屏幕",
        m_strTestName,31,strIniFile);      

    m_uiCmd=index+CMD_TEST_SCREEN;
    *uipCmdIndex=m_uiCmd;
    CFullScreenDlg *pDlg;
    pDlg=new CFullScreenDlg(NULL);
    m_pFullDlgWnd=pDlg;
    HANDLE h=CreateThread(NULL,0,ScreenTestThread,this,0,NULL);
   
    if (h==NULL)
    { 
        MessageBox(NULL,_T("创建屏幕测试线程失败"),
            _T("严重错误"),MB_OK|MB_ICONERROR);
        return FALSE;
    }
    CloseHandle(h);
    while(pDlg->m_hWnd ==NULL)
    {
        Sleep(100);
    }
    pDlg->ShowWindow(SW_HIDE);
    pDlg->UpdateWindow();

    return TRUE;
}

TEST_STATUS CScreenTest::ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen,
                                   const BYTE *pData,CConnect *pConnect)
{
    m_ti.bError=0;

    switch(param)
    {
    case  SCREEN_PARAM_INIT:
        InitScreenTest();
        
        break;
    case  SCREEN_PARAM_TEST_COLOR_LEVEL:
        ((CFullScreenDlg *)m_pFullDlgWnd)->DisplayColorLevel();        \
        break;
    case  SCREEN_PARAM_END:
        ((CFullScreenDlg *)m_pFullDlgWnd)->ShowWindow(SW_HIDE);  
        break;
    case  SCREEN_PARAM_TEST_IMAGE:
        ((CFullScreenDlg *)m_pFullDlgWnd)->DisplayTestImage();
        break;
    default:
        break;
    }

    int nRet=pConnect->SendAPacket(m_uiCmd,param,sizeof(TEST_INFO),(BYTE *)&m_ti);
    if (nRet >0)
    {
        if (m_ti.bError)
        {
            return TEST_ERROR;
        }
        return TEST_NORMAL;
    }
    
    return TEST_CONNECTDOWN;
}


BOOL CScreenTest::InitScreenTest()
{
    ((CFullScreenDlg *)m_pFullDlgWnd)->DisplayStartInterface();
    Sleep(500);
    return TRUE;
}

DWORD CScreenTest::ScreenTestThread(void *pVoid)
{
    CScreenTest *pTest=(CScreenTest *)pVoid;
    CFullScreenDlg *pDlg=(CFullScreenDlg *)pTest->m_pFullDlgWnd;
    pDlg->DoModal();
    return 0;
}

TEST_STATUS CScreenTest::OnConnectDown()
{
    ((CFullScreenDlg *)m_pFullDlgWnd)->ShowWindow(SW_HIDE); 
    return TEST_NORMAL;
}