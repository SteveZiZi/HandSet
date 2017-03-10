// FunctionTestCEDlg.cpp : implementation file
//

#include "stdafx.h"
#include <IPHlpApi.h>
#include "FunctionTestCE.h"
#include "connect.h"
#include "iniFile.h"
#include "NetConnect.h"
#include "FunctionTestCEDlg.h"
#include "iniFile.h"
#include "UsuallyCommon.h"
#include "ActiveSyncConnect.h"
#include "KbdTestDlg.h"
#include "winioctl.h"
#include ".\app_src\fiber.h"
#include "cregedit.h"

#pragma comment(lib,"Iphlpapi.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CFunctionTestCEDlg dialog

CFunctionTestCEDlg::CFunctionTestCEDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CFunctionTestCEDlg::IDD, pParent)  
    ,m_hProcessPacket(NULL)
    ,m_hAccpeptThread(NULL)
    ,m_pServer(NULL)
{
    
    m_hDevice = INVALID_HANDLE_VALUE;
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

}

void CFunctionTestCEDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFunctionTestCEDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
    ON_WM_SIZE()
#endif
    //}}AFX_MSG_MAP
    ON_MESSAGE(UWM_PC_CONECT,OnPCConnect)
    ON_MESSAGE(UWM_CONNECT_DOWN,OnConnectDown)
    ON_WM_DESTROY()
END_MESSAGE_MAP()


// CFunctionTestCEDlg message handlers

BOOL CFunctionTestCEDlg::OnInitDialog()
{
    CDialog::OnInitDialog();    
    SetIcon(m_hIcon, TRUE);            
    SetIcon(m_hIcon, FALSE);
    //����ȫ�������ļ�
    if (!LoadGlobalConfig())
    {
        MessageBox(_T("���������ļ�ʧ��"),_T("����"),MB_OK|MB_ICONERROR );
        PostMessage(WM_CLOSE);
        return FALSE;
    }


    if (m_hDevice == INVALID_HANDLE_VALUE) {                            /*  ��GPM����                 */
        m_hDevice = CreateFile(L"GPM1:", GENERIC_READ|GENERIC_WRITE, 0, 0,OPEN_ALWAYS, 0, NULL);
        if (m_hDevice == INVALID_HANDLE_VALUE) {
            MessageBox(L"Open GPM Device failed");
            return FALSE;
        } else {
            RETAILMSG(1, (_T("Open GPM Device\r\n")));
        }
    }

    BOOL bFiberWork = TRUE; 
    if (FALSE == DeviceIoControl(m_hDevice, IOCTRL_START_FPGA, &bFiberWork, sizeof(bFiberWork), NULL,NULL,NULL,NULL) ) {
        MessageBox(L"DeviceIoControl:IOCTRL_START_FPGA failed");
        return FALSE;
    }

    UINT16 uBootVer[2];
    if (DeviceIoControl(m_hDevice, IOCTRL_GET_DRV_VERSION, NULL, NULL, &uBootVer,sizeof(uBootVer),NULL,NULL)) 
    {
        m_strDeviceVer.Format(_T("  GPM(V%x.%02x) FPGA(V%x.%02x)"), 
            (uBootVer[0]>>8)&0xF, (uBootVer[0]&0xFF), 
            (uBootVer[1]>>8)&0xF, (uBootVer[1]&0xFF)
            );
    } 
    else 
    {
        RETAILMSG(1, (_T("DeviceIoControl:IOCTRL_GET_DRV_VERSION failed\r\n")));
    }

    //����һ��TestManager����
    m_pTestManager=new CTestManager();
    if (m_pTestManager==NULL)
    {
        MessageBox(_T("�����ڴ�ʧ��"));
        PostMessage(WM_CLOSE);
        return 0;
    }
    //��ʼ��TestManager����
    if(!m_pTestManager->Init(m_strDeviceType,this->m_hWnd) )
    {
        MessageBox(_T("��ʼ������ʧ��"));
        PostMessage(WM_CLOSE);
        return 0;
    }

    if (m_connectWay == CONNECT_WAY_ACTIVE_SYNC)
    {
        CActiveSyncConnect *pConnect = new CActiveSyncConnect();
        if (!pConnect->Connect())
        {
            MessageBox(_T("��ʼ������ʧ��"));
            PostMessage(WM_CLOSE);
            return 0;
        }
        PostMessage(UWM_PC_CONECT,0,(LPARAM)pConnect);
    }
    else if (m_connectWay == CONNECT_WAY_ETNERNET)
    {
        //���ʹ����̫�����ӣ������������߳�
        CNetServer      *pNetServer =new CNetServer();
        if ( pNetServer->BuildServer((char *)NULL,SERVER_PORT,2))
        {
            m_pServer = pNetServer;
            m_hAccpeptThread =CreateThread(NULL,0,AcceptProc,this,0,NULL);
            if (m_hAccpeptThread ==NULL)
            {
                delete m_pServer;
                m_pServer=NULL;
                MessageBox(_T("�����������߳�ʧ��"));
                PostMessage(WM_CLOSE);
            }
        }
        else
        {
            delete pNetServer;
            MessageBox(_T("����������ʧ��"));
            PostMessage(WM_CLOSE);
            return 0;
        }
    }

    OpenBackLight();

    return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CFunctionTestCEDlg::LoadGlobalConfig()
{

    //�õ�ȫ�������ļ���·��
    CString     strIniFilePath;
    WCHAR       *p=strIniFilePath.GetBuffer(MAX_PATH *2);
    WCHAR       tmp[MAX_PATH];
    CString     str;
    GetExeDirectory(p,MAX_PATH*2);
    strIniFilePath.ReleaseBuffer();
    strIniFilePath+=L"config.ini";

   

    //����Ӧ�ó���ı���
    GetPrivateProfileString(L"global",L"AppTitle",L"FunctionTest",tmp,
        MAX_PATH,strIniFilePath);
    SetWindowText(tmp);
    

    //�����豸����
    p=m_strDeviceType.GetBuffer(100);
    GetPrivateProfileString(L"global",L"DeviceType",L"DAQ-1000",p,
        100,strIniFilePath);
    m_strDeviceType.ReleaseBuffer();

    //������PC�����ӵķ�ʽ
    m_connectWay = (CONNECT_WAY)(int)GetPrivateProfileInt(L"global",
        L"ConnectWay",0,strIniFilePath);
    if (m_connectWay!=CONNECT_WAY_ACTIVE_SYNC)
    {
        m_connectWay = CONNECT_WAY_ETNERNET;
    }


    return TRUE;
}

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CFunctionTestCEDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
    if (AfxIsDRAEnabled())
    {
        DRA::RelayoutDialog(
            AfxGetResourceHandle(), 
            this->m_hWnd, 
            DRA::GetDisplayMode() != DRA::Portrait ? 
            MAKEINTRESOURCE(IDD_FUNCTIONTESTCE_DIALOG_WIDE) : 
            MAKEINTRESOURCE(IDD_FUNCTIONTESTCE_DIALOG));
    }
}
#endif

DWORD CFunctionTestCEDlg::AcceptProc(void * pVoid)
{
//     int index=( (AcceptInitStruct *)pVoid)->index;
//     CFunctionTestCEDlg *pDlg=( (AcceptInitStruct *)pVoid)->pDlg;
// 
//     CNetServer  *pNetServer=NULL;
//     CString     str;
//     CStringA    strIP;
// 
//     PIP_ADAPTER_INFO  p=NULL,p1;
//     ULONG       size=0;
//     BOOL        bFind=FALSE;            //�Ƿ���ҵ����������Ӧ������
//     BOOL        bBuidServer;            //�Ƿ�ɹ�����������
// 
// 
// TRY_BUILD_SERVER:    
//     
//     //���Բ���ָ��������
//     size=0;
//     bFind=FALSE;
//     bBuidServer=FALSE;
//     GetAdaptersInfo(p,&size);
//     if (size >0)
//     {
//         p=(PIP_ADAPTER_INFO)malloc(size);
//         GetAdaptersInfo(p,&size); 
//         p1=p;
//         do 
//         {
//             str=p1->AdapterName;
//             str.MakeUpper();
//             if (str ==pDlg ->m_strEthNameList[index] )
//             {
//                 //���ҵ�ָ�����������Ի�ȡ��IP��ַ
//                 bFind=TRUE;
//                 strIP=p1->IpAddressList.IpAddress.String;
//                 break;
//             }
// 
//             p1=p1->Next;
//         } while (p1!=NULL);
// 
//         free(p);
//         p=NULL;
//     }
// 
//     if (bFind)
//     {
//         //�ɹ��ҵ�ָ������������֤��IP��ַ�Ƿ���Ч
//         if (inet_addr(strIP )==INADDR_NONE )
//         {
//             //IP��ַ��Ч,�������������Ȼ���ڵ�����û�в�ã��ȴ�300ms������
//             Sleep(300);
//             goto TRY_BUILD_SERVER;
//         }
//         
//     }
//     else
//     {
//         //û���ҵ�ָ������������������Ϊָ�������������ã��ȴ�300ms������
//         Sleep(300);
//         goto TRY_BUILD_SERVER;
//     }
//     
//     //�ɹ��ҵ�ָ��������������IP��ַ��Ч�����Խ���������
//     pNetServer=new CNetServer();
//     bBuidServer=pNetServer->BuildServer(strIP,SERVER_PORT,1);
//     if (!bBuidServer )
//     {
//         delete pNetServer;
//         pNetServer=NULL;
//     }
// 
    //�����������ɹ�����ʼAccept
    BOOL    bAccept;                    //Accept�Ƿ�ɹ� 
    CConnect *pConnect;
    CFunctionTestCEDlg      *pDlg =(CFunctionTestCEDlg *)pVoid;
    CServer                 *pServer =pDlg->m_pServer;
    do 
    {
        pConnect=new CNetConnect();
        bAccept=pServer->Accept(pConnect);
        if (bAccept)
        {
            pDlg->SendMessage(UWM_PC_CONECT,0,(LPARAM)pConnect);
            
        }
    } while (bAccept);

    delete pConnect;
    return  0;
}

HRESULT CFunctionTestCEDlg::OnPCConnect(WPARAM wParam,LPARAM lParam)
{
    if (m_hProcessPacket ==NULL)
    {
        m_pTestManager->OnConnect((CConnect *)lParam);    
        /*PACKET packet;
        char * pDeviceVer;
        UnicodeToMByte(pDeviceVer, m_strDeviceVer);
        memcpy(packet.pData, pDeviceVer, 100);
        ((CConnect *)lParam)->SendAPacket(&packet);*/
        m_bKillProcessPacket=FALSE;
        m_hProcessPacket=CreateThread(NULL,0,ProcessPacketProc,this,0,NULL);
        if (m_hProcessPacket==NULL)
        {
            //�����������ݰ��߳�ʧ�ܡ�
        }
    }
    else
    {
        //�������ӣ�ɾ��ԭ��������
        CConnect *p=m_pTestManager->GetConnect();
        p->Clear();
        while(m_hProcessPacket !=NULL)
        {
            Sleep(0);
            MSG msg;
            while(PeekMessage(&msg,NULL,0,0,PM_REMOVE)!=0 )
            {
                TranslateMessage (&msg) ;
                DispatchMessage (&msg) ;
            }
        }

        //�ȴ��������ݰ��߳�

        m_pTestManager->OnConnect((CConnect *)lParam);    

        m_bKillProcessPacket=FALSE;
        m_hProcessPacket=CreateThread(NULL,0,ProcessPacketProc,this,0,NULL);
        if (m_hProcessPacket==NULL)
        {
            //�����������ݰ��߳�ʧ�ܡ�
        }
    }

    return 0;
}

DWORD CFunctionTestCEDlg::ProcessPacketProc(void *pVoid)
{
    CFunctionTestCEDlg *pDlg=(CFunctionTestCEDlg *)pVoid;
    int nRet;
    /*CeSetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);*/
    while(!pDlg->m_bKillProcessPacket)
    {
        nRet=pDlg->m_pTestManager->ProcessOnePacket();
        if (nRet>0)
        {
            //��������
        }
        else if (nRet ==0)
        {
            //���ӳ�ʱ�����ƶ�һֱδ�����ݰ�           
            if ( pDlg->m_pTestManager->TestConnect() <0)
            {
                ::PostMessage(pDlg->m_hWnd,UWM_CONNECT_DOWN,0,0);
                break;
            }

        }
        else
        {             
            //���ӶϿ�,����һ�����ӶϿ���Ϣ���˳��������ݰ��߳�
            ::PostMessage(pDlg->m_hWnd,UWM_CONNECT_DOWN,0,0);
            break;
        }
    }

    return 0;
}

HRESULT CFunctionTestCEDlg::OnConnectDown(WPARAM wParam,LPARAM lParam)
{
    m_bKillProcessPacket=TRUE;

    m_pTestManager->OnConnectDown();
    if (m_hProcessPacket !=NULL)
    {
        WaitForSingleObject(m_hProcessPacket,INFINITE);
        CloseHandle(m_hProcessPacket);
        m_hProcessPacket=NULL;
    }
    //�ͷŲ��Թ�����������������Ӷ���
    CConnect *pConnect=m_pTestManager->DetachConnect();
    if (pConnect)
    {
        delete pConnect;
    }
    
    return 0;
}
void CFunctionTestCEDlg::OnDestroy()
{
    
    BOOL bFiberWork = FALSE; 
    if (m_hDevice != INVALID_HANDLE_VALUE) {
        if (FALSE == DeviceIoControl(m_hDevice, IOCTRL_START_FPGA, &bFiberWork, sizeof(bFiberWork), NULL,NULL,NULL,NULL) ) {
            MessageBox(L"DeviceIoControl:IOCTRL_START_FPGA failed");
            return;
        }
    }
    
    if (m_hDevice != INVALID_HANDLE_VALUE) {
        CloseHandle(m_hDevice);
        m_hDevice = INVALID_HANDLE_VALUE;
    }

    //ʹ�ڵȴ�����(Accept)���߳��˳�
    if (m_hAccpeptThread !=NULL)
    {
        m_pServer->CloseServer();
        while (WaitForSingleObject(m_hAccpeptThread,500)==WAIT_TIMEOUT)
        {
            MSG msg;
            while(PeekMessage(&msg,NULL,0,0,PM_REMOVE)!=0 )
            {
                TranslateMessage (&msg) ;
                DispatchMessage (&msg) ;
            }
        }
        delete m_pServer;
        m_pServer=NULL;
        CloseHandle(m_hAccpeptThread);
        m_hAccpeptThread = NULL; 
    }

    //�ȴ��������ݰ��߳��˳�
    m_bKillProcessPacket=TRUE;

    if (m_hProcessPacket !=NULL)
    {
        while (WaitForSingleObject(m_hProcessPacket,500)==WAIT_TIMEOUT)
        {
            MSG msg;
            while(PeekMessage(&msg,NULL,0,0,PM_REMOVE)!=0 )
            {
                TranslateMessage (&msg) ;
                DispatchMessage (&msg) ;
            }
        }
        
        CloseHandle(m_hProcessPacket);
        m_hProcessPacket=NULL;
    }

    if(m_pTestManager!=NULL)
    {
        //�ͷŲ��Թ�����������������Ӷ���
        CConnect *pConnect=m_pTestManager->DetachConnect();
        if (pConnect)
        {
            delete pConnect;
        }
        //�ͷŲ��Թ���������
        delete m_pTestManager;
        m_pTestManager=NULL;
    }
    CDialog::OnDestroy();
}

HANDLE CFunctionTestCEDlg::GetDeviceHandle()
{
    return m_hDevice;
}

CHAR * CFunctionTestCEDlg::UnicodeToMByte( CHAR* destStr, CString sourceStr )
{
    int len = sourceStr.GetLength();
    //��ȡCSring�ַ����еĻ�������������
    LPCTSTR buffer = sourceStr.GetBuffer(len);
    TCHAR* pch = new TCHAR[len + 1];
    ZeroMemory(pch, (len + 1) * sizeof(TCHAR));
    //�����ַ���
    lstrcpy (pch, buffer);
    int wcslen = ::WideCharToMultiByte( CP_ACP, 0, pch, -1, NULL, 0, NULL, NULL);
    //�����ֽ��ַ�ת��Ϊ���ֽ��ַ�
    ::WideCharToMultiByte( CP_ACP, 0, pch, -1, 
        destStr, wcslen, NULL, NULL);
    //�ͷŻ�ȡ���û�����
    sourceStr.ReleaseBuffer(len);
    delete[] pch;
    //���ػ�ȡ����char������ֵ
    return destStr;
}

void CFunctionTestCEDlg::OpenBackLight()
{
    DWORD dwTurnOffTime;
    BOOL dwTrans = FALSE;

    CRegistryEdit regBacklight(HKEY_CURRENT_USER, TEXT("ControlPanel\\Backlight"));

    dwTurnOffTime = 0;
    if (!regBacklight.RegSetValueEx(_T("BatteryTimeout"), REG_DWORD, (LPBYTE)&dwTurnOffTime, sizeof(dwTurnOffTime))) {
        RETAILMSG(1, (TEXT("[%s] %S RegSetValueEx(\"%s\" failed\r\n"), L"Light", __FUNCTION__, _T("BatteryTimeout")));
        return ;
    }
    if (!regBacklight.RegSetValueEx(_T("ACTimeout"), REG_DWORD, (LPBYTE)&dwTurnOffTime, sizeof(dwTurnOffTime))) {
        RETAILMSG(1, (TEXT("[%s] %S RegSetValueEx(\"%s\" failed\r\n"), L"Light", __FUNCTION__, _T("ACTimeout")));
        return ;
    }

    HANDLE hevReloadActivityTimeouts = 
        OpenEvent(EVENT_ALL_ACCESS, FALSE, _T("PowerManager/ReloadActivityTimeouts"));
    if (hevReloadActivityTimeouts) {
        SetEvent(hevReloadActivityTimeouts);
        CloseHandle(hevReloadActivityTimeouts);
    } else {
        RETAILMSG(1, (TEXT("Open \"PowerManager/ReloadActivityTimeouts\" Event Failed\r\n")));
    }

}
