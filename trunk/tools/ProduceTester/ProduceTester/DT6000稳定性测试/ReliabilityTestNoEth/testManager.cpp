#include "stdafx.h"
#include <stdio.h>
#include "testManager.h"
#include "ethernetTest.h"
#include "DiskTest.h"
#include "gpioTest.h"
#include "iniFile.h"
#include "UsuallyCommon.h"
#include "assertMsg.h"
#include "WriteLog.h"
#include "serialporttest.h"
#include "TestReport.h"
#include "ISATest.h"
#include "LedTest.h"
#include "CANTest.h"
#include "BeepTest.h"
#include "ADDATest.h"
#include "rtcTest.h"
#include "wifiTest.h"
#include "GPSTest.h"
#include "spiFlashTest.h"
#include "gprsTest.h"
#include "SmvTest.h"
#include "Ft3Test.h"
#include "ReliabilityTestNoEthDlg.h"
//#pragma comment(lib,"Mmtimer.lib")
//extern     int     g_iFlag;
CTestManager::CTestManager():
m_iTestObjNum(0),
m_hTimmerThread(NULL),
m_hStopTimmerThread(NULL)
{
    //������в�����ָ��   
    memset(m_pTestObj,0,MAX_TEST_OBJ*sizeof(CTestBase *) );
    memset(m_pbTest,0,MAX_TEST_OBJ *sizeof(BOOL) );
    memset(&m_stBegin,0,sizeof(m_stBegin));
    CReliabilityTestNoEthDlg * pDlg = (CReliabilityTestNoEthDlg *)AfxGetApp()->m_pMainWnd;
    m_pFt3Tx = new CFt3Tx(pDlg->GetDeviceHandle(), DEVICE_FT3_TX);
    m_bSendFt3 = FALSE;
}

CTestManager::~CTestManager()
{
    if (m_hStopTimmerThread !=NULL)
    {
        CloseHandle(m_hStopTimmerThread);
        m_hStopTimmerThread=NULL;
    }
    //ɾ�����в��������
    for (int i=0;i<m_iTestObjNum;i++ )
    {
        if (m_pTestObj[i])
        {
            delete m_pTestObj[i];
            m_pTestObj[i]=NULL;
        }
    }
    m_iTestObjNum=0;

    if(m_pFt3Tx != NULL)
    {
        delete m_pFt3Tx;
    }
}

BOOL CTestManager::Init(const WCHAR *strDeviceType,const WCHAR *strServerIP,HWND hMainWnd)
{
    int         i;
    int         k;
    WCHAR       strIniFilePath[MAX_PATH*2];

    //��ȡ��ִ���ļ����ڵ�·��
    i=GetExeDirectory(strIniFilePath,MAX_PATH*2);
    //��������ļ����ڵ�·��
    memcpy(strIniFilePath+i,L"config\\",8*sizeof(WCHAR) );
    i+=7;
    wcscpy(strIniFilePath+i,strDeviceType);
    wcscat(strIniFilePath,L".ini");

    //�鿴�����ļ��Ƿ����
    if (! IsFileExist(strIniFilePath))
    {
        return FALSE;
    }  
    m_hStopTimmerThread=CreateEvent(NULL,FALSE,FALSE,NULL);
    if (m_hStopTimmerThread ==NULL)
    {
        MessageBox(NULL,L"����ֹͣ��ʱ���¼�����ʧ��",L"����",MB_OK|MB_ICONERROR);
        return FALSE;
    }


    //������̫������Ե���Ϣ
    i=GetPrivateProfileInt(L"product",L"EthernetTestNum",0,strIniFilePath);   
    k=0;
    while(k<i)
    {
        CEthernetTest *pTest=new CEthernetTest();
        if( pTest->Init(strIniFilePath,k,this) )
        {
            AddTestObj(pTest);
        }
        else
        {
            delete pTest;
        }
        k++;
    }  

    //���ش�������Ե���Ϣ
    i=GetPrivateProfileInt(L"product",L"SERIALTestNum",0,strIniFilePath);   
    k=0;
    while(k<i)
    {
        CSerialPortTest *pTest=new CSerialPortTest();
        if( pTest->Init(strIniFilePath,k,this) )
        {
            AddTestObj(pTest);
        }
        else
        {
            delete pTest;
        }
        k++;
    }  

    //����RTC����Ե���Ϣ
    i=GetPrivateProfileInt(L"product",L"RTCTestNum",0,strIniFilePath);   
    k=0;
    while(k<i)
    {
        CRTCTest *pTest=new CRTCTest();
        if( pTest->Init(strIniFilePath,k,this) )
        {
            AddTestObj(pTest);
        }
        else
        {
            delete pTest;
        }
        k++;
    }  


    //����CAN���Ե���Ϣ
    i=GetPrivateProfileInt(L"product",L"CANTestNum",0,strIniFilePath);   
    k=0;
    while(k<i)
    {
        CCANTest *pTest=new CCANTest();
        if( pTest->Init(strIniFilePath,k,this) )
        {
            AddTestObj(pTest);
        }
        else
        {
            delete pTest;
        }
        k++;
    }  

    //����WIFI���Ե���Ϣ
    i=GetPrivateProfileInt(L"product",L"WIFITestNum",0,strIniFilePath);   
    k=0;
    while(k<i)
    {
        CWifiTest *pTest=new CWifiTest();
        if( pTest->Init(strIniFilePath,k,this) )
        {
            AddTestObj(pTest);
        }
        else
        {
            delete pTest;
        }
        k++;
    }  

    //����GPS���Ե���Ϣ
    i=GetPrivateProfileInt(L"product",L"GPSTestNum",0,strIniFilePath);   
    k=0;
    while(k<i)
    {
        CGpsTest *pTest=new CGpsTest();
        if( pTest->Init(strIniFilePath,k,this) )
        {
            AddTestObj(pTest);
        }
        else
        {
            delete pTest;
        }
        k++;
    }

    //����GPRS���Ե���Ϣ
    i=GetPrivateProfileInt(L"product",L"GPRSTestNum",0,strIniFilePath);   
    k=0;
    while(k<i)
    {
        CGPRSTest *pTest=new CGPRSTest();
        if( pTest->Init(strIniFilePath,k,this) )
        {
            AddTestObj(pTest);
        }
        else
        {
            delete pTest;
        }
        k++;
    }

    //����ISA���������ļ�
    i=GetPrivateProfileInt(L"product",L"ISATestNum",0,strIniFilePath);    
    k=0;  
    while(k<i)
    {
        CISATest *pTest=new CISATest();
        if (pTest->Init(strIniFilePath,k,this) )
        {
            AddTestObj(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        k++;
    }

    //���ش��̲��������ļ�
    i=GetPrivateProfileInt(L"product",L"DiskTestNum",0,strIniFilePath);
    //��ʼ����Ҫ�Ĵ��̲���
    k=0;  
    while(k<i)
    {
        CDiskTest *pTest=new CDiskTest();
        if (pTest->Init(strIniFilePath,k,this) )
        {
            AddTestObj(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        k++;
    }

    //����LED���������ļ�
    i=GetPrivateProfileInt(L"product",L"LEDTestNum",0,strIniFilePath);    
    k=0;  
    while(k<i)
    {
        CLedTest *pTest=new CLedTest();
        if (pTest->Init(strIniFilePath,k,this) )
        {
            AddTestObj(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        k++;
    }

    //���ط��������������ļ�
    i=GetPrivateProfileInt(L"product",L"BEEPTestNum",0,strIniFilePath);    
    k=0;  
    while(k<i)
    {
        CBeepTest *pTest=new CBeepTest();
        if (pTest->Init(strIniFilePath,k,this) )
        {
            AddTestObj(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        k++;
    }

    //����GPIO���������ļ�
    i=GetPrivateProfileInt(L"product",L"GPIOTestNum",0,strIniFilePath);    
    k=0;  
    while(k<i)
    {
        CGpioTest *pTest=new CGpioTest();
        if (pTest->Init(strIniFilePath,k,this) )
        {
            AddTestObj(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        k++;
    }

    //����AD/DA���������ļ�
    i=GetPrivateProfileInt(L"product",L"ADDATestNum",0,strIniFilePath);    
    k=0;  
    while(k<i)
    {
        CAdDaTest *pTest=new CAdDaTest();
        if (pTest->Init(strIniFilePath,k,this) )
        {
            AddTestObj(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        k++;
    }

    
    //����spiflash���������ļ�
    i=GetPrivateProfileInt(L"product",L"SpiFlashTestNum",0,strIniFilePath);    
    k=0;  
    while(k<i)
    {
        CSpiFlashTest *pTest=new CSpiFlashTest();
        if (pTest->Init(strIniFilePath,k,this) )
        {
            AddTestObj(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        k++;
    }

    //����FT3���������ļ�
    CReliabilityTestNoEthDlg * pDlg = (CReliabilityTestNoEthDlg *)AfxGetApp()->m_pMainWnd;
    i=GetPrivateProfileInt(L"product",L"Ft3TestNum",0,strIniFilePath); 
    if(i > 0)
    {
        m_pFt3Tx->Ft3Send();
        m_bSendFt3 = TRUE;
    }
    k=0;  
    while(k<i)
    {
        CFt3Test *pTest=new CFt3Test(pDlg->GetDeviceHandle(), k + DEVICE_FT3_RX1);
        if (pTest->Init(strIniFilePath,k,this) )
        {
            AddTestObj(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        k++;
    }

    //���ع����ڲ��������ļ�
    i=GetPrivateProfileInt(L"product",L"SmvTestNum",0,strIniFilePath);    
    k=0;  
    while(k<i)
    {
        CSmvTest *pTest=new CSmvTest(pDlg->GetDeviceHandle());
        if (pTest->Init(strIniFilePath,k,this) )
        {
            AddTestObj(pTest);
        }
        else
        {
            //��ʼ��ʧ����ɾ��������Զ���
            delete pTest;
        }
        k++;
    }

    m_hMainWnd=hMainWnd;

    //�����Ƿ��в���
    
    m_bParallelTest=GetPrivateProfileInt(L"product",L"parallelTest",0,strIniFilePath);    
   m_strServerIP = strServerIP;

    return TRUE;
}


BOOL CTestManager::AddTestObj(CTestBase *pTest)
{
    if (m_iTestObjNum >=MAX_TEST_OBJ ) return FALSE;
    m_pbTest[m_iTestObjNum]=TRUE;
    m_pTestObj[m_iTestObjNum]=pTest;
    m_iTestObjNum++;

    return TRUE;
}

BOOL CTestManager::DisableTestObj(int index)
{
    assertMsg(index >=0 &&index <MAX_TEST_OBJ,L"index��ֵ�����˷�Χ");
    m_pbTest[index]=FALSE;
    return TRUE;
}

struct ParalleTestInfo
{
    CTestManager   *pManager;
    int             iTestObj;
};

BOOL CTestManager::BeginTest()
{
    m_bStopTest=FALSE;

    //��ʼ����ʼʱ��
    if (m_stBegin.wYear ==0)
    {
        GetLocalTime(&m_stBegin);
    }


    if (m_bParallelTest)
    {
        for (int i=0;i<m_iTestObjNum;i++)
        {
            ParalleTestInfo     *pInfo =new ParalleTestInfo;
            pInfo->iTestObj=i;
            pInfo->pManager=this;
            HANDLE h=CreateThread(NULL,0,ParallelTestProc,pInfo,0,NULL);
            m_phParallelHandle[i] = h;
        }
        return TRUE;
    }
    else
    {
        //������ʱ���߳�
        ResetEvent(m_hStopTimmerThread);
        m_hTimmerThread=CreateThread(NULL,0,TimerThreadProc,this,0,NULL);
        if (m_hTimmerThread ==NULL)
        {
            return FALSE;
        }

        //���������߳�
        m_hTestThread=CreateThread(NULL,0,TestProc,this,0,NULL);
        if (m_hTestThread !=NULL)
        {
            return TRUE;
        }
        //���������߳�ʧ�ܵĴ���
        SetEvent(m_hStopTimmerThread);
        WaitForSingleObject(m_hTimmerThread,INFINITE);
        CloseHandle(m_hTimmerThread);
        m_hTimmerThread=NULL;
        return FALSE;
    }
}

BOOL CTestManager::StopTest()
{
    m_bStopTest=TRUE;
    if (m_hTimmerThread !=NULL)
    {
        SetEvent(m_hStopTimmerThread);
        WaitForSingleObject(m_hTimmerThread,INFINITE);
        CloseHandle(m_hTimmerThread);
        m_hTimmerThread=NULL;
    }
    if(m_bSendFt3)
    {
        m_pFt3Tx->Ft3Stop();
        m_bSendFt3 = FALSE;
    }
   
    return TRUE;
}

BOOL CTestManager::WaitForStopComplete()
{
    if (m_hTestThread!=NULL)
    {
        DWORD dwRet=WaitForSingleObject(m_hTestThread,500);
        while (dwRet == WAIT_TIMEOUT)
        {
            //����������һ���򵥵���Ϣѭ��������ɶ�������Ϣ�Ĵ���
            MSG msg;
            while(PeekMessage(&msg,NULL,0,0,PM_REMOVE)!=0 )
            {
                TranslateMessage (&msg) ;
                DispatchMessage (&msg) ;
            }
            dwRet = WaitForSingleObject(m_hTestThread,500);
        }
        CloseHandle(m_hTestThread);
        m_hTestThread=NULL;
    }

    if (m_bParallelTest)
    {
        for (int i=0;i<m_iTestObjNum;i++)
        {
            if (m_phParallelHandle[i])
            {
                DWORD dwRet=WaitForSingleObject(m_phParallelHandle[i],500);
                while (dwRet == WAIT_TIMEOUT)
                {
                    //����������һ���򵥵���Ϣѭ��������ɶ�������Ϣ�Ĵ���
                    MSG msg;
                    while(PeekMessage(&msg,NULL,0,0,PM_REMOVE)!=0 )
                    {
                        TranslateMessage (&msg) ;
                        DispatchMessage (&msg) ;
                    }
                    dwRet = WaitForSingleObject(m_phParallelHandle[i],500);
                }
                CloseHandle(m_phParallelHandle[i]);
                m_phParallelHandle[i]=NULL;
            }
        }
    }

    return FALSE;
}

DWORD CTestManager::TestProc(void *pVoid)
{
    CTestManager *pManager=(CTestManager *)pVoid;
    HWND          hMainWnd=pManager->m_hMainWnd;

    pManager->m_bChangTest=FALSE;
    pManager->m_iCurTestObj=0;
    //�������ڵ�����ǰ������Ķ�ʱ��
//     pManager->m_uiTimerId=timeSetEvent(20000,10,ChangeTestTimeProc,(DWORD_PTR)pVoid,
//         TIME_CALLBACK_FUNCTION|TIME_PERIODIC);

//     if(pManager ->m_uiTimerId ==NULL)
//     {
//         MessageBox(pManager->m_hMainWnd,_T("�������ڵ���������Ķ�ʱ��ʧ��"),
//             _T("����"),MB_OK|MB_ICONERROR );
//         return 1;
//     }

    SendMessage(hMainWnd,UM_TEST_STATUS_CHANGE,
        (WPARAM)pManager->m_iCurTestObj,(LPARAM)(int)STATUS_TESTING );
    DWORD   dwStart,dwEnd;
    dwStart=GetTickCount();
    dwEnd=dwStart;
    bool bIsBegin = FALSE;
    
    while (!pManager ->m_bStopTest)
    {
        CTestBase *pTest;
        
        if (pManager->m_bChangTest)
        {
            pTest->EndOneTest();
            bIsBegin = FALSE;
            dwEnd=GetTickCount();
            if (dwEnd>dwStart)
            {
                pManager->m_pTestObj[ pManager->m_iCurTestObj]->AddTestTime(dwEnd-dwStart);
            }

            SendMessage(hMainWnd,UM_TEST_STATUS_CHANGE,
                (WPARAM)pManager->m_iCurTestObj,(LPARAM)(int)STATUS_WAITING );
            if (!pManager-> ChangeTest())
            {
                //�Ѿ��Ҳ������õĲ�����˳����Գ���
                pManager->WriteLog(NULL,L"���в������ƺ�����������");
                return 0;
            } 
            SendMessage(hMainWnd,UM_TEST_STATUS_CHANGE,
                (WPARAM)pManager->m_iCurTestObj,(LPARAM)(int)STATUS_TESTING );
            dwStart=GetTickCount();
        }

        TEST_SITUATION ts;
        pTest=pManager->m_pTestObj[ pManager->m_iCurTestObj];
        if(!bIsBegin)
        {
            pTest->BeginOneTest();
            bIsBegin = TRUE;
        }
        ts= pTest->ExcuteOneTest();
        switch(ts)
        {
        case SITUATION_DEAD:
            if (!pManager ->m_bStopTest)
            {
                SendMessage(hMainWnd,UM_TEST_STATUS_CHANGE,
                    (WPARAM)pManager->m_iCurTestObj,(LPARAM)(int)STATUS_DEAD );
            }
            pManager->WriteLog(pTest,L"�������Ѿ�����");

            break;
        case SITUATION_ERROR: 
            //��¼�������־
            pManager->WriteLog(pTest,pTest->GetLastErrorMsg());
        case  SITUATION_NORMAL:
            if (!pManager ->m_bStopTest)
            {
                 SendMessage(hMainWnd,UM_TEST_COUNT_CHANGE,
                     (WPARAM)pManager->m_iCurTestObj,(LPARAM)pTest);
            }
            break;
        }
        
    }

    //�����Ѿ��˳�
    //�������Ĳ���ʱ��
    dwEnd=GetTickCount();
    if (dwEnd>dwStart)
    {
        pManager->m_pTestObj[ pManager->m_iCurTestObj]->AddTestTime(dwEnd-dwStart);
    }
    
    return 0;
}

DWORD CTestManager::ParallelTestProc(void *pVoid)
{
    ParalleTestInfo     *pInfo=(ParalleTestInfo *)pVoid;

    CTestManager *pManager=pInfo ->pManager;
    int           iTestObj =pInfo->iTestObj;
    HWND          hMainWnd=pManager->m_hMainWnd;   

    delete pInfo;
    pInfo =NULL;
    //�������ڵ�����ǰ������Ķ�ʱ��
    //     pManager->m_uiTimerId=timeSetEvent(20000,10,ChangeTestTimeProc,(DWORD_PTR)pVoid,
    //         TIME_CALLBACK_FUNCTION|TIME_PERIODIC);

    //     if(pManager ->m_uiTimerId ==NULL)
    //     {
    //         MessageBox(pManager->m_hMainWnd,_T("�������ڵ���������Ķ�ʱ��ʧ��"),
    //             _T("����"),MB_OK|MB_ICONERROR );
    //         return 1;
    //     }

    SendMessage(hMainWnd,UM_TEST_STATUS_CHANGE,
        iTestObj,(LPARAM)(int)STATUS_TESTING );
    DWORD   dwStart,dwEnd;
    dwStart=GetTickCount();
    dwEnd=dwStart;

    while (!pManager ->m_bStopTest)
    {
       
        dwEnd=GetTickCount();
        if (dwEnd>dwStart)
        {
            pManager->m_pTestObj[ iTestObj]->AddTestTime(dwEnd-dwStart);
        }

        dwStart=GetTickCount();        

        TEST_SITUATION ts;
        CTestBase *pTest=pManager->m_pTestObj[ iTestObj];
        ts= pTest->ExcuteOneTest();
        switch(ts)
        {
        case SITUATION_DEAD:
            if (!pManager ->m_bStopTest)
            {
                SendMessage(hMainWnd,UM_TEST_STATUS_CHANGE,
                    (WPARAM)pManager->m_iCurTestObj,(LPARAM)(int)STATUS_DEAD );
            }
            pManager->WriteLog(pTest,L"�������Ѿ�����");

            break;
        case SITUATION_ERROR: 
            //��¼�������־
            pManager->WriteLog(pTest,pTest->GetLastErrorMsg());
        case  SITUATION_NORMAL:
            if (!pManager ->m_bStopTest)
            {
                SendMessage(hMainWnd,UM_TEST_COUNT_CHANGE,
                    (WPARAM)iTestObj,(LPARAM)pTest);
            }
            break;
        }
        Sleep(10);
    }

    //�����Ѿ��˳�
    //�������Ĳ���ʱ��
    dwEnd=GetTickCount();
    if (dwEnd>dwStart)
    {
        pManager->m_pTestObj[ iTestObj]->AddTestTime(dwEnd-dwStart);
    }

    return 0;
}


BOOL CTestManager::ChangeTest()
{
    int i=m_iCurTestObj;
    int tryTime=0;
    
RETRY:
    i=(i+1)%m_iTestObjNum;
    /*if(i == 0)
    {
        g_iFlag ^= 1;
    }*/
    if (m_pbTest[i])
    {
        m_iCurTestObj=i;
        m_bChangTest=FALSE;
        return TRUE;
    }
    tryTime++;

    if (tryTime >m_iTestObjNum)
    {
        return FALSE;
    }
    goto RETRY;
}

DWORD   CTestManager::TimerThreadProc(void *pVoid)
{
    CTestManager *pManager=(CTestManager *)pVoid;
    DWORD          dwRet;

    while(1)
    {
        dwRet=WaitForSingleObject(pManager ->m_hStopTimmerThread,20000);
        if (dwRet ==WAIT_TIMEOUT)
        {
            pManager->m_bChangTest=TRUE;
        }
        else
        {
            break;
        }
    }

    return 0;
}


BOOL CTestManager::WriteLog(CTestBase *pTest,const WCHAR *strLog)
{
    SYSTEMTIME st;
    GetLocalTime(&st);
//     if (pTest ==NULL)
//     {
//         ::WriteLog(L"(%u) %s\r\n",GetTickCount(),strLog);
//     }
//     else
//     {
//         ::WriteLog(L"(%u)[%s] %s\r\n",GetTickCount(),pTest->GetTestObjName(),strLog);
    //     }
    CString strTmp;
    strTmp.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
    if (pTest ==NULL)
    {
        ::WriteLog(L"(%s) %s\r\n",strTmp,strLog);
    }
    else
    {
        ::WriteLog(L"(%s)[%s] %s\r\n",strTmp,pTest->GetTestObjName(),strLog);
    }

    return TRUE;
}

BOOL CTestManager::BuildTestReport(LPCWSTR strReprotFile)
{
    CTestReport report;
    SYSTEMTIME  st;
    SReportNape srt[]={
        {L"�豸����",L"",FALSE}, 
        {L"�豸��ʶ",L"",TRUE},
        {L"������ʼʱ��",L"",FALSE},
        {L"���Խ���ʱ��",L"",FALSE}

    };
    
    WCHAR   strReportTitle[128];
    GetLocalTime(&st);
    swprintf(strReportTitle,L"�ȶ��Բ��Ա���(%d-%d-%d %d:%d:%d)",
        st.wYear,st.wMinute,st.wDay,st.wHour,st.wMinute,st.wSecond);

    //���ϵͳ��Ϣ��ı���
    wcsncpy(srt[0].strNapeDescriptor,m_strDeviceType,127);
    wcsncpy(srt[1].strNapeDescriptor,m_strDeviceType,127);
    swprintf(srt[2].strNapeDescriptor,L"%d��%d��%d�� %dʱ%d��%d��",
        m_stBegin.wYear,m_stBegin.wMonth,m_stBegin.wDay,m_stBegin.wHour,
        m_stBegin.wMinute,m_stBegin.wSecond);
    swprintf(srt[3].strNapeDescriptor,L"%d��%d��%d�� %dʱ%d��%d��",
        st.wYear,st.wMonth,st.wDay,st.wHour,
        st.wMinute,st.wSecond);

    report.InitTestReport(strReprotFile,strReportTitle);
   // report.AddReportItem(L"ϵͳ������Ϣ",srt,4);

    for (int i=0;i<m_iTestObjNum;i++)
    {
        int count;
        const SReportNape *p=m_pTestObj[i]->GetReportNapes(count);
        report.AddReportItem(m_pTestObj[i]->GetTestObjName(),p,count);        
    }

    return TRUE;
}