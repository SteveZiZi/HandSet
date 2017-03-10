#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "FunctionTestCE.h"
#include "KbdTest.h"
#include "KbdTestDlg.h"
#include "iniFile.h"
#include "UsuallyCommon.h"
CKbdTest::CKbdTest()
    :m_pDlg(NULL)
{

}

CKbdTest::~CKbdTest()
{
    
}

BOOL CKbdTest::Init(const WCHAR *strIniFile, int index, 
                    UINT16 *uipCmdIndex,CTestManager *pManager/* =NULL */)
{

    //获取应该读取的配置项目名称
    WCHAR strSection[]=L"KbdTest_\0\0";
    strSection[8]=L'1'+index;

    //读取测试名称
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"键盘",m_strTestName,31,strIniFile);

    //初始化要用到的命令索引码
    m_uiCmd=index+CMD_TEST_KEYBORAD;
    *uipCmdIndex=m_uiCmd;
    m_pTestManager = pManager;
    //获取当前应用程序的目录
    int iDirLen =GetExeDirectory(m_strExtFilePath,MAX_PATH*2);
    return TRUE;
}

TEST_STATUS CKbdTest::ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen, const BYTE *pData,CConnect *pConnect)
{

    CKbdTestDlg         dlg(CWnd::FromHandle(m_pTestManager->GetMainWnd()));
    DWORD               dwRet=1 ;

    PROCESS_INFORMATION pi;

    if (dlg.DoModal()==IDOK )
    {
        dwRet = 0;
    }
       

    int nRet = pConnect->SendAPacket(m_uiCmd,param,sizeof(DWORD),(BYTE *)&dwRet);
    if (nRet <=0)
    {
        return TEST_CONNECTDOWN;
    }
    return dwRet ==0?TEST_NORMAL:TEST_ERROR;
}



