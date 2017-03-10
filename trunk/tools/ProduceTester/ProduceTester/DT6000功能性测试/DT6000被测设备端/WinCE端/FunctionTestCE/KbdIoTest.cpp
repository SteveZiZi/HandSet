#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "KbdIoTest.h"
#include "iniFile.h"



CKbdIoTest::CKbdIoTest():
epcIoRead(NULL),
epcIoWrite(NULL),
m_hInstGpioDll(NULL)
{

}

CKbdIoTest::~CKbdIoTest()
{
    if (m_hInstGpioDll !=NULL)
    {
        CloseHandle(m_hInstGpioDll);
        m_hInstGpioDll=NULL;
    }
    epcIoRead=NULL;
    epcIoWrite=NULL;
}

BOOL CKbdIoTest::Init(const WCHAR *strIniFile, int index,UINT16 *uipCmdIndex,CTestManager *pManager)
{
    WCHAR strSection[]=L"KbdIOTest_\0\0";
    strSection[10]=L'1'+index;

    //读取测试项的名字
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"GPIO",
        m_strTestName,31,strIniFile); 
    //载入gpio操作dll
    m_hInstGpioDll=LoadLibrary(_T("epcKbdIoLib.dll"));
    if (m_hInstGpioDll==NULL)
    {
        MessageBox(NULL,_T("加载epcKbdIoLib.dll失败，请与测试软件的维护人员联系"),
            _T("严重错误"),MB_OK|MB_ICONERROR);
        return FALSE;
    }    
    epcIoRead=(int ( *)(DWORD *))GetProcAddress(m_hInstGpioDll,TEXT("epcKbdIoRead"));
    epcIoWrite=(int (*)(DWORD)) GetProcAddress(m_hInstGpioDll,TEXT("epcKbdIoWrite"));
    if (epcIoRead ==NULL)
    {
        return FALSE;
    }
    if (epcIoWrite ==NULL)
    {
        return FALSE;
    }

    m_uiCmd=index+CMD_TEST_KBD_IO;
    *uipCmdIndex=m_uiCmd;
    return TRUE;
}

TEST_STATUS CKbdIoTest::ProcessExcuteCmd(UINT16 param,UINT16 uiDatalen, 
                                        const BYTE *pData,CConnect *pConnect)
{
    UINT32 uiOut;
    memcpy(&uiOut,pData,sizeof(uiOut) );
    epcIoWrite((DWORD)uiOut );
    Sleep(5);
    DWORD  dwIn=0;
    epcIoRead(&dwIn);

    UINT32 uiIn=(UINT32)dwIn ;

    int nRet=pConnect->SendAPacket(m_uiCmd,param,sizeof(UINT32 ),(BYTE *)&uiIn);
    if (nRet <=0)
    {
        return TEST_ERROR;
    }

    return TEST_NORMAL;
}