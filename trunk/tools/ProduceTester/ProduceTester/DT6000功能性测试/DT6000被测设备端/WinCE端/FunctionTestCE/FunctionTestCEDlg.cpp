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
    //加载全局配置文件
    if (!LoadGlobalConfig())
    {
        MessageBox(_T("加载配置文件失败"),_T("错误"),MB_OK|MB_ICONERROR );
        PostMessage(WM_CLOSE);
        return FALSE;
    }


    if (m_hDevice == INVALID_HANDLE_VALUE) {                            /*  打开GPM驱动                 */
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

    //分配一个TestManager对象
    m_pTestManager=new CTestManager();
    if (m_pTestManager==NULL)
    {
        MessageBox(_T("分配内存失败"));
        PostMessage(WM_CLOSE);
        return 0;
    }
    //初始化TestManager对象
    if(!m_pTestManager->Init(m_strDeviceType,this->m_hWnd) )
    {
        MessageBox(_T("初始化测试失败"));
        PostMessage(WM_CLOSE);
        return 0;
    }

    if (m_connectWay == CONNECT_WAY_ACTIVE_SYNC)
    {
        CActiveSyncConnect *pConnect = new CActiveSyncConnect();
        if (!pConnect->Connect())
        {
            MessageBox(_T("初始化连接失败"));
            PostMessage(WM_CLOSE);
            return 0;
        }
        PostMessage(UWM_PC_CONECT,0,(LPARAM)pConnect);
    }
    else if (m_connectWay == CONNECT_WAY_ETNERNET)
    {
        //如果使用以太网连接，则建立服务器线程
        CNetServer      *pNetServer =new CNetServer();
        if ( pNetServer->BuildServer((char *)NULL,SERVER_PORT,2))
        {
            m_pServer = pNetServer;
            m_hAccpeptThread =CreateThread(NULL,0,AcceptProc,this,0,NULL);
            if (m_hAccpeptThread ==NULL)
            {
                delete m_pServer;
                m_pServer=NULL;
                MessageBox(_T("建立服务器线程失败"));
                PostMessage(WM_CLOSE);
            }
        }
        else
        {
            delete pNetServer;
            MessageBox(_T("建立服务器失败"));
            PostMessage(WM_CLOSE);
            return 0;
        }
    }

    OpenBackLight();

    return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CFunctionTestCEDlg::LoadGlobalConfig()
{

    //得到全局配置文件的路径
    CString     strIniFilePath;
    WCHAR       *p=strIniFilePath.GetBuffer(MAX_PATH *2);
    WCHAR       tmp[MAX_PATH];
    CString     str;
    GetExeDirectory(p,MAX_PATH*2);
    strIniFilePath.ReleaseBuffer();
    strIniFilePath+=L"config.ini";

   

    //加载应用程序的标题
    GetPrivateProfileString(L"global",L"AppTitle",L"FunctionTest",tmp,
        MAX_PATH,strIniFilePath);
    SetWindowText(tmp);
    

    //加载设备类型
    p=m_strDeviceType.GetBuffer(100);
    GetPrivateProfileString(L"global",L"DeviceType",L"DAQ-1000",p,
        100,strIniFilePath);
    m_strDeviceType.ReleaseBuffer();

    //加载与PC机连接的方式
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
//     BOOL        bFind=FALSE;            //是否查找到本索引项对应的网卡
//     BOOL        bBuidServer;            //是否成功建立服务器
// 
// 
// TRY_BUILD_SERVER:    
//     
//     //尝试查找指定的网卡
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
//                 //查找到指定网卡，尝试获取其IP地址
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
//         //成功找到指定的网卡，验证其IP地址是否有效
//         if (inet_addr(strIP )==INADDR_NONE )
//         {
//             //IP地址无效,表明这块网卡虽然存在但网线没有插好，等待300ms后重试
//             Sleep(300);
//             goto TRY_BUILD_SERVER;
//         }
//         
//     }
//     else
//     {
//         //没有找到指定的网卡，可能是因为指定的网卡被禁用，等待300ms后重试
//         Sleep(300);
//         goto TRY_BUILD_SERVER;
//     }
//     
//     //成功找到指定的网卡，并且IP地址有效，尝试建立服务器
//     pNetServer=new CNetServer();
//     bBuidServer=pNetServer->BuildServer(strIP,SERVER_PORT,1);
//     if (!bBuidServer )
//     {
//         delete pNetServer;
//         pNetServer=NULL;
//     }
// 
    //建立服务器成功，开始Accept
    BOOL    bAccept;                    //Accept是否成功 
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
            //创建处理数据包线程失败。
        }
    }
    else
    {
        //有新连接，删除原来的连接
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

        //等待处理数据包线程

        m_pTestManager->OnConnect((CConnect *)lParam);    

        m_bKillProcessPacket=FALSE;
        m_hProcessPacket=CreateThread(NULL,0,ProcessPacketProc,this,0,NULL);
        if (m_hProcessPacket==NULL)
        {
            //创建处理数据包线程失败。
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
            //测试正常
        }
        else if (nRet ==0)
        {
            //连接超时，控制端一直未发数据包           
            if ( pDlg->m_pTestManager->TestConnect() <0)
            {
                ::PostMessage(pDlg->m_hWnd,UWM_CONNECT_DOWN,0,0);
                break;
            }

        }
        else
        {             
            //连接断开,寄送一个连接断开消息，退出处理数据包线程
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
    //释放测试管理器中所保存的连接对象
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

    //使在等待连接(Accept)的线程退出
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

    //等待处理数据包线程退出
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
        //释放测试管理器中所保存的连接对象
        CConnect *pConnect=m_pTestManager->DetachConnect();
        if (pConnect)
        {
            delete pConnect;
        }
        //释放测试管理器对象
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
    //获取CSring字符串中的缓冲区，并锁定
    LPCTSTR buffer = sourceStr.GetBuffer(len);
    TCHAR* pch = new TCHAR[len + 1];
    ZeroMemory(pch, (len + 1) * sizeof(TCHAR));
    //拷贝字符串
    lstrcpy (pch, buffer);
    int wcslen = ::WideCharToMultiByte( CP_ACP, 0, pch, -1, NULL, 0, NULL, NULL);
    //将宽字节字符转换为多字节字符
    ::WideCharToMultiByte( CP_ACP, 0, pch, -1, 
        destStr, wcslen, NULL, NULL);
    //释放获取到得缓冲区
    sourceStr.ReleaseBuffer(len);
    delete[] pch;
    //返回获取到的char型数组值
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
