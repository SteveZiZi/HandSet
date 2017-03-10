#include "stdafx.h"
#include "connect.h"
#include "testBase.h"
#include "testManager.h"
#include "UsbDeviceTest.h"
#include "assertMsg.h"
#include "rapi.h"

#define  ACTIVESYNC_TEST_FILE_LEN           102400
#define  ACTIVESYNC_TEST_FILE_NAME          L"\\ActiveSyncTest.bin"

BOOL CActiveSyncTest::Init(const WCHAR *strIniFile, int index,CTestManager *pManager)
{
    //获取应该读取的配置项目名称
    WCHAR strSection[]=L"ActiveSyncTest_\0\0";
    strSection[9]=L'1'+index;

    //读取测试名称
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"USB ActiveSync",m_strTestName,31,strIniFile);

    //初始化要用到的命令索引码
    m_uiCmd=index+CMD_TEST_ACTIVE;
    //保存CTestManager对象指针
    assertMsg(pManager,L"pManager指针不能为空!");
    m_pManager=pManager;

    return TRUE;
}

TEST_STATUS CActiveSyncTest::ExcuteOneTest(CConnect *pConnect)
{
    BYTE *pBufRead=NULL;
    BYTE *pBufWrite=NULL;

    __try
    {
        m_uiTestCount++;
        pBufRead=new BYTE[ACTIVESYNC_TEST_FILE_LEN];
        pBufWrite=new BYTE[ACTIVESYNC_TEST_FILE_LEN];

        if ( !(pBufRead && pBufWrite)  )
        {
            //分配内存失败
            delete []pBufRead;
            delete []pBufWrite;

            pBufRead=NULL;
            pBufWrite=NULL;

            m_pManager->WriteLog(this,L"分配内存失败");
            m_uiTestErrCount++;
            return TEST_ERROR;
        }

        //填充写缓冲区
        for (int i=0;i<ACTIVESYNC_TEST_FILE_LEN;i++)
        {
            pBufWrite[i]=rand();
        }

        TEST_STATUS status;

        //读写并校验远程文件
        status =WriteRemoteTestFile(pBufWrite);
        if (status ==TEST_NORMAL)
        {
            status=ReadRemoteTestFile(pBufRead);
            if (status ==TEST_NORMAL)
            {
                m_pManager->showTestStatus(L"正在校验远程文件");
                if (memcmp(pBufRead,pBufWrite,ACTIVESYNC_TEST_FILE_LEN )!=0 )
                {
                    m_pManager->WriteLog(this,L"远程文件读写校验失败");
                    m_uiTestErrCount++;
                    return TEST_ERROR;               
                }
                return TEST_NORMAL;
            }
        }

        m_uiTestErrCount++;
        return status;
    }

    __finally
    {
        m_pManager->showTestStatus(L"正在删除测试文件");
        CeDeleteFile(ACTIVESYNC_TEST_FILE_NAME);
        if (pBufRead)
        {
            delete []pBufRead;
            pBufRead=NULL;
        }
        if (pBufWrite)
        {
            delete []pBufWrite;
            pBufWrite=NULL;
        }
    }    
}

TEST_STATUS CActiveSyncTest::WriteRemoteTestFile(BYTE *pWriteBuf)
{
    //写测试文件
    BOOL        bRet;
    DWORD       dwActLen;

    CeDeleteFile(ACTIVESYNC_TEST_FILE_NAME);
    m_pManager->showTestStatus(L"正在写入远程文件");
    HANDLE hFile = CeCreateFile(ACTIVESYNC_TEST_FILE_NAME, 
        GENERIC_WRITE, 
        0, 
        NULL, 
        OPEN_ALWAYS, 
        FILE_ATTRIBUTE_NORMAL, 
        NULL);
    if (hFile ==INVALID_HANDLE_VALUE)
    {
        if ((DWORD)CeRapiGetError() == 0x80072775) goto ActiveSyncDown;
        m_pManager->WriteLog(this,L"创建远程测试文件失败");
        return TEST_ERROR;
    }
    
    bRet = CeWriteFile(hFile, 
        pWriteBuf, 
        ACTIVESYNC_TEST_FILE_LEN, 
        &dwActLen, 
        NULL);

    if (!bRet) {       
        CeCloseHandle(hFile);
        if ((DWORD)CeRapiGetError() == 0x80072775) goto ActiveSyncDown; 
        m_pManager->WriteLog(this, L"写入远程文件失败");
    }
    CeCloseHandle(hFile);
    return TEST_NORMAL;
ActiveSyncDown:
    m_pManager->MessageBox(L"ActiveSync远程连接以断开,本测试终止!",
        L"ActiveSync测试", MB_ICONERROR|MB_OK);
     return TEST_ABORT;
}

TEST_STATUS CActiveSyncTest::ReadRemoteTestFile(BYTE *pReadBuf)
{
    //读测试文件
    BOOL        bRet;
    DWORD       dwActLen;
 
    m_pManager->showTestStatus(L"正在读取远程文件");
    HANDLE hFile = CeCreateFile(ACTIVESYNC_TEST_FILE_NAME, 
        GENERIC_READ, 
        0, 
        NULL, 
        OPEN_ALWAYS, 
        FILE_ATTRIBUTE_NORMAL, 
        NULL);
    if (hFile ==INVALID_HANDLE_VALUE)
    {
        if ((DWORD)CeRapiGetError() == 0x80072775) goto ActiveSyncDown;
        m_pManager->WriteLog(this,L"打开远程测试文件失败");
        return TEST_ERROR;
    }

    bRet = CeReadFile(hFile, 
        pReadBuf, 
        ACTIVESYNC_TEST_FILE_LEN, 
        &dwActLen, 
        NULL);

    if (!bRet) {       
        CeCloseHandle(hFile);
        if ((DWORD)CeRapiGetError() == 0x80072775) goto ActiveSyncDown; 
        m_pManager->WriteLog(this, L"读取远程测试文件失败");
    }
    CeCloseHandle(hFile);
    return TEST_NORMAL;
ActiveSyncDown:
    m_pManager->MessageBox(L"ActiveSync远程连接以断开,本测试终止!",
        L"ActiveSync测试", MB_ICONERROR|MB_OK);
    return TEST_ABORT;
}

TEST_STATUS CActiveSyncTest::OnBeginTest(CConnect *pConnect)
{

    RAPIINIT ri;
    ri.cbSize = sizeof(ri);
    CeRapiInitEx(&ri);
    DWORD ret = WaitForSingleObject(ri.heRapiInit, 1000);
    if((ret != S_OK) || (ri.hrRapiInit != S_OK)) {
        m_pManager->MessageBox(_T("RAPI函数初始化失败，请检查ActiveSync是否已连接"),
            _T("错误"),MB_OK|MB_ICONERROR);          
        return TEST_ERROR;
    } 
    return TEST_NORMAL;
}

TEST_STATUS CActiveSyncTest::OnEndTest(CConnect *pConnect)
{
    CeRapiUninit();
    return TEST_NORMAL;
}
