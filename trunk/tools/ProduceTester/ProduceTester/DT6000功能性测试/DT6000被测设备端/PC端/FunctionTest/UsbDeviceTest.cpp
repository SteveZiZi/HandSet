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
    //��ȡӦ�ö�ȡ��������Ŀ����
    WCHAR strSection[]=L"ActiveSyncTest_\0\0";
    strSection[9]=L'1'+index;

    //��ȡ��������
    int nRet;
    nRet=GetPrivateProfileString(strSection,L"TestName",L"USB ActiveSync",m_strTestName,31,strIniFile);

    //��ʼ��Ҫ�õ�������������
    m_uiCmd=index+CMD_TEST_ACTIVE;
    //����CTestManager����ָ��
    assertMsg(pManager,L"pManagerָ�벻��Ϊ��!");
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
            //�����ڴ�ʧ��
            delete []pBufRead;
            delete []pBufWrite;

            pBufRead=NULL;
            pBufWrite=NULL;

            m_pManager->WriteLog(this,L"�����ڴ�ʧ��");
            m_uiTestErrCount++;
            return TEST_ERROR;
        }

        //���д������
        for (int i=0;i<ACTIVESYNC_TEST_FILE_LEN;i++)
        {
            pBufWrite[i]=rand();
        }

        TEST_STATUS status;

        //��д��У��Զ���ļ�
        status =WriteRemoteTestFile(pBufWrite);
        if (status ==TEST_NORMAL)
        {
            status=ReadRemoteTestFile(pBufRead);
            if (status ==TEST_NORMAL)
            {
                m_pManager->showTestStatus(L"����У��Զ���ļ�");
                if (memcmp(pBufRead,pBufWrite,ACTIVESYNC_TEST_FILE_LEN )!=0 )
                {
                    m_pManager->WriteLog(this,L"Զ���ļ���дУ��ʧ��");
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
        m_pManager->showTestStatus(L"����ɾ�������ļ�");
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
    //д�����ļ�
    BOOL        bRet;
    DWORD       dwActLen;

    CeDeleteFile(ACTIVESYNC_TEST_FILE_NAME);
    m_pManager->showTestStatus(L"����д��Զ���ļ�");
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
        m_pManager->WriteLog(this,L"����Զ�̲����ļ�ʧ��");
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
        m_pManager->WriteLog(this, L"д��Զ���ļ�ʧ��");
    }
    CeCloseHandle(hFile);
    return TEST_NORMAL;
ActiveSyncDown:
    m_pManager->MessageBox(L"ActiveSyncԶ�������ԶϿ�,��������ֹ!",
        L"ActiveSync����", MB_ICONERROR|MB_OK);
     return TEST_ABORT;
}

TEST_STATUS CActiveSyncTest::ReadRemoteTestFile(BYTE *pReadBuf)
{
    //�������ļ�
    BOOL        bRet;
    DWORD       dwActLen;
 
    m_pManager->showTestStatus(L"���ڶ�ȡԶ���ļ�");
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
        m_pManager->WriteLog(this,L"��Զ�̲����ļ�ʧ��");
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
        m_pManager->WriteLog(this, L"��ȡԶ�̲����ļ�ʧ��");
    }
    CeCloseHandle(hFile);
    return TEST_NORMAL;
ActiveSyncDown:
    m_pManager->MessageBox(L"ActiveSyncԶ�������ԶϿ�,��������ֹ!",
        L"ActiveSync����", MB_ICONERROR|MB_OK);
    return TEST_ABORT;
}

TEST_STATUS CActiveSyncTest::OnBeginTest(CConnect *pConnect)
{

    RAPIINIT ri;
    ri.cbSize = sizeof(ri);
    CeRapiInitEx(&ri);
    DWORD ret = WaitForSingleObject(ri.heRapiInit, 1000);
    if((ret != S_OK) || (ri.hrRapiInit != S_OK)) {
        m_pManager->MessageBox(_T("RAPI������ʼ��ʧ�ܣ�����ActiveSync�Ƿ�������"),
            _T("����"),MB_OK|MB_ICONERROR);          
        return TEST_ERROR;
    } 
    return TEST_NORMAL;
}

TEST_STATUS CActiveSyncTest::OnEndTest(CConnect *pConnect)
{
    CeRapiUninit();
    return TEST_NORMAL;
}
