// ReliabilityTestNoEthDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ReliabilityTestNoEth.h"
#include "ReliabilityTestNoEthDlg.h"
#include "iniFile.h"
#include "WriteLog.h"
#include "AboutDlg.h"
#include "UsuallyCommon.h"
#include "winioctl.h"
#include ".\app_src\fiber.h"
#include "cregedit.h"
#define     ID_TIMER_REPORT             1               //��ʱ��¼���Ա���Ķ�ʱ��ID 

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CReliabilityTestNoEthDlg dialog
//������ʾ�����豸��Ϣ�Ĵ�����������Ϣ
#define     LIST_COL_TEST_NAME              0
#define     LIST_COL_TEST_STATUS            1
#define     LIST_COL_TEST_NUM               2
#define     LIST_COL_TEST_ERR_NUM           3
#define     LIST_COL_TEST_ERR_RATE          4

CReliabilityTestNoEthDlg::CReliabilityTestNoEthDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CReliabilityTestNoEthDlg::IDD, pParent),
    m_pTestManager(NULL),
    m_bTesting(FALSE)
{
    m_hDevice = INVALID_HANDLE_VALUE;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CReliabilityTestNoEthDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_INFO, m_ctrlListInfo);
}

BEGIN_MESSAGE_MAP(CReliabilityTestNoEthDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
    ON_BN_CLICKED(IDC_BUTTON1_START_STOP_TEST, &CReliabilityTestNoEthDlg::OnBnClickedButton1StartStopTest)
    ON_MESSAGE  (UM_TEST_COUNT_CHANGE,OnTestCountChange)
    ON_MESSAGE  (UM_TEST_STATUS_CHANGE,OnTestStatusChange)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_BUTTON_ABOUT, &CReliabilityTestNoEthDlg::OnBnClickedButtonAbout)
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_BUTTON3, &CReliabilityTestNoEthDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CReliabilityTestNoEthDlg message handlers

BOOL CReliabilityTestNoEthDlg::OnInitDialog()
{
	CDialog::OnInitDialog(); 
	
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

    //������ʾ������ϢListCtrl�ķ��
    DWORD   dwStyle;   
    dwStyle = m_ctrlListInfo.GetStyle();
    dwStyle |= LVS_EX_FULLROWSELECT;		
    m_ctrlListInfo.SetExtendedStyle(dwStyle);
    //��ʼ��List�������ʾ
    m_ctrlListInfo.InsertColumn(LIST_COL_TEST_NAME, _T("������"), LVCFMT_CENTER, 130);
    m_ctrlListInfo.InsertColumn(LIST_COL_TEST_STATUS, _T("����״̬"), LVCFMT_CENTER, 110);    
    m_ctrlListInfo.InsertColumn(LIST_COL_TEST_NUM, _T("���Դ���"), LVCFMT_CENTER, 110);   
    m_ctrlListInfo.InsertColumn(LIST_COL_TEST_ERR_NUM, _T("�������"), LVCFMT_CENTER, 110);
    m_ctrlListInfo.InsertColumn(LIST_COL_TEST_ERR_RATE, _T("������"),   LVCFMT_CENTER, 100);  


    if (!LoadGlobalConfig()) 
    {
        MessageBox(TEXT("����Ӧ�ó���������Ϣʧ��"),TEXT("����"),MB_OK|MB_ICONERROR);
        PostMessage(WM_CLOSE);
        return FALSE;
    }

    if (!InitLog())
    {
        MessageBox(L"��ʼ����־ʧ��");
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

    OpenBackLight();

    //��ʼ�����Թ�����
    m_pTestManager=new CTestManager();
    if (m_pTestManager ==NULL)
    {
        MessageBox(L"�����ڴ�ʧ��");
        PostMessage(WM_CLOSE);
        return FALSE;        
    }
    
    if (!m_pTestManager ->Init(m_strDeviveType,m_strServerIP,this->m_hWnd) )
    {
        MessageBox(L"��ʼ�����Թ�����ʧ��");
        PostMessage(WM_CLOSE);
        return FALSE;
    }

    //���ֲ������Ѿ�������ϣ���������������ʾ��������ȥ
    int count=m_pTestManager->GetTestObjCount();
    for (int i=0;i<count;i++)
    {     
        m_ctrlListInfo.InsertItem(i,
            m_pTestManager->GetTestObj(i)->GetTestObjName()) ;
        m_ctrlListInfo.SetItemText(i,LIST_COL_TEST_STATUS,L"������");
        m_ctrlListInfo.SetItemText(i,LIST_COL_TEST_NUM,L"--");
        m_ctrlListInfo.SetItemText(i,LIST_COL_TEST_ERR_NUM,L"--");
        m_ctrlListInfo.SetItemText(i,LIST_COL_TEST_ERR_RATE,L"--");
    }


    SYSTEMTIME st;
    GetLocalTime(&st);
    WriteLog(L"���Կ�ʼ��%d-%d-%d %d:%d:%d\r\n",st.wYear,st.wMonth,st.wDay,
        st.wHour,st.wMinute,st.wSecond); 

    //��߽����̵߳���Ӧ�ٶ�
//     CeSetThreadPriority(GetCurrentThread(),0);
// 	OnBnClickedButton1StartStopTest();
    OnBnClickedButton1StartStopTest();
    //������¼���Ա���Ķ�ʱ��
    SetTimer(ID_TIMER_REPORT,60000,NULL);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CReliabilityTestNoEthDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	if (AfxIsDRAEnabled())
	{
		DRA::RelayoutDialog(
			AfxGetResourceHandle(), 
			this->m_hWnd, 
			DRA::GetDisplayMode() != DRA::Portrait ? 
			MAKEINTRESOURCE(IDD_RELIABILITYTESTNOETH_DIALOG_WIDE) : 
			MAKEINTRESOURCE(IDD_RELIABILITYTESTNOETH_DIALOG));
	}
}
#endif

BOOL CReliabilityTestNoEthDlg::LoadGlobalConfig()
{
    //��ȡȫ�������ļ���·��
    CString   strIniFilePath;

    GetExeDirectory(strIniFilePath.GetBuffer(MAX_PATH*2),MAX_PATH*2);
    strIniFilePath.ReleaseBuffer();
    strIniFilePath+=_T("config.ini");

    //��ȡ�豸����
    int bAssign=GetPrivateProfileInt(L"global",L"AssignDeviceType",0,strIniFilePath);
    if (bAssign)
    {
        WCHAR *p;
        p=m_strDeviveType.GetBuffer(64);
        GetPrivateProfileString(L"global",L"DeviceType",L"EPC-8000",p,64,strIniFilePath);
        m_strDeviveType.ReleaseBuffer();
    }
    else
    {
        WCHAR buf[64];
        CString str1,str2;
        BOOL (*epcGetCustomerName) (WCHAR *pwcName, DWORD dwLen, DWORD *pdwActLen)=NULL;
        BOOL (*epcGetPlatFormName) (WCHAR *pwcName, DWORD dwLen, DWORD *pdwActLen)=NULL;
        HINSTANCE   hInst=NULL;

        hInst=LoadLibrary(L"\\windows\\epcSysInfo.dll");
        if (hInst ==NULL)
        {
            MessageBox(L"����epcSysInfo.dllʧ��");
            return FALSE;
        }
        epcGetCustomerName=( BOOL (*)(WCHAR *,DWORD,DWORD*) )
            GetProcAddress(hInst,L"epcGetCustomerName");
        epcGetPlatFormName=( BOOL (*)(WCHAR *,DWORD,DWORD*) )
            GetProcAddress(hInst,L"epcGetPlatFormName");
        if (epcGetPlatFormName==NULL ||epcGetCustomerName ==NULL )
        {
            MessageBox(L"����epcSysInfo.dllʧ��");
            FreeLibrary(hInst);
            return FALSE;
        }
        //��ȡƽ̨����

        DWORD dwActLen;
        epcGetPlatFormName(buf,64,&dwActLen);
        buf[dwActLen]=0;

        m_strDeviveType=buf;
        
        epcGetCustomerName(buf,64,&dwActLen);
        buf[dwActLen]=0;
        str1=buf;
        str1.MakeLower();
        if (str1!=_T("general"))
        {
            m_strDeviveType+=_T("_");
            m_strDeviveType+=buf;
        }
        
        FreeLibrary(hInst);
    }

    //��ȡ��¼��־�ļ��Ĵ��̵�·��
    WCHAR *p=m_strLogDiskDir.GetBuffer(MAX_PATH);

    GetPrivateProfileString(L"log",L"logDiskDir",L"\0",
        p,MAX_PATH,strIniFilePath);

    m_strLogDiskDir.ReleaseBuffer();
    p = m_strServerIP.GetBuffer(64);
    GetPrivateProfileString(L"global",L"ServerIP",L"192.168.0.254",p,64,strIniFilePath);
    m_strServerIP.ReleaseBuffer();

    if (m_strLogDiskDir.GetAt(0)==0)
    {
        //��ȡ��־���ڴ����ļ���ʧ��
        return FALSE;
    } 
    
    WCHAR   strTitle[100];
    GetPrivateProfileString(L"global",L"AppTitle",L"����̫���ȶ��Բ���",
        strTitle,1000,strIniFilePath);
    SetWindowTextW(strTitle);
    return TRUE;
}

BOOL CReliabilityTestNoEthDlg::InitLog()
{
    //���Ȳ��Ҳ�ȷ����һ����ŵ�Ŀ¼������д��־

    //���ȴ������ڴ����־��Ŀ¼
    m_strLogDir=m_strLogDiskDir+L"Reports\\";
    CreateDirectory(m_strLogDir,NULL);

    //���Ҿֲ�����ŵ��ļ���
    int         nMax=0;
    CString     strPrex=L"Test";
    HANDLE      hFind=INVALID_HANDLE_VALUE ;
    CString     strTmp=m_strLogDir+L"*.*";
    WIN32_FIND_DATA wfd;
    hFind=FindFirstFile(strTmp,&wfd);

    if (hFind !=INVALID_HANDLE_VALUE)
    {
        BOOL bFind;
        do 
        {
            int t=0;
            _stscanf(wfd.cFileName,L"Test%d",&t);
            if (t>nMax)
            {
                nMax=t;
            }
            bFind=FindNextFile(hFind,&wfd);
        } while (bFind);
        FindClose(hFind);
    }

    //�������δ洢Ҫ�õ����ļ���
    nMax++;
    strTmp.Empty();
    strTmp.Format(_T("Test%03d"),nMax);
    m_strLogDir+=strTmp;
    CreateDirectory(m_strLogDir,NULL);
    m_strLogDir+=_T("\\");
    strTmp=m_strLogDir+L"testLog.txt";   

    return InitWriteLog(strTmp,this->m_hWnd);
}


void CReliabilityTestNoEthDlg::OnBnClickedButton1StartStopTest()
{
    CString strText;
    GetDlgItem(IDC_BUTTON1_START_STOP_TEST)->GetWindowText(strText);

    if (strText ==L"��ʼ����")
    {
        
        m_bTesting=TRUE;
        m_pTestManager->BeginTest();
        GetDlgItem(IDC_BUTTON1_START_STOP_TEST)->SetWindowText(L"ֹͣ����");
    }
    else
    {
        m_bTesting=FALSE;
        if (m_pTestManager )
        {
            m_pTestManager->StopTest(); 
        }
        m_bTesting=FALSE;
        Sleep(500);

        //����������һ���򵥵���Ϣѭ��������ɶ�������Ϣ�Ĵ���
        MSG msg;
        while(PeekMessage(&msg,NULL,0,0,PM_REMOVE)!=0 )
        {
            TranslateMessage (&msg) ;
            DispatchMessage (&msg) ;
        }
        if (m_pTestManager)
        {
            m_pTestManager->WaitForStopComplete();
        }          

        int count=m_pTestManager->GetTestObjCount();
        for (int i=0;i<count;i++)
        {
            m_ctrlListInfo.SetItemText(i,LIST_COL_TEST_STATUS,L"������");
        }
      
        GetDlgItem(IDC_BUTTON1_START_STOP_TEST)->SetWindowText(L"��ʼ����");
        CString strReportFile=m_strLogDir+_T("report.html");
        m_pTestManager->BuildTestReport(strReportFile);
    }
}

void CReliabilityTestNoEthDlg::OnDestroy()
{

    KillTimer(ID_TIMER_REPORT);
    if (m_pTestManager )
    {
        m_pTestManager->StopTest(); 
    }
    m_bTesting=FALSE;
    Sleep(500);

    //����������һ���򵥵���Ϣѭ��������ɶ�������Ϣ�Ĵ���
    MSG msg;
    while(PeekMessage(&msg,NULL,0,0,PM_REMOVE)!=0 )
    {
        TranslateMessage (&msg) ;
        DispatchMessage (&msg) ;
    }

   
    if (m_pTestManager)
    {
        m_pTestManager->WaitForStopComplete();

        CString strReportFile=m_strLogDir+_T("report.html");
        m_pTestManager->BuildTestReport(strReportFile);

        delete m_pTestManager;
        m_pTestManager=NULL;
    }

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

    CDialog::OnDestroy();
}

HRESULT CReliabilityTestNoEthDlg::OnTestCountChange(WPARAM wParam,LPARAM lParam)
{
    if (m_bTesting)
    {
        int index=(int )wParam;
        const CTestBase *pTest=(const CTestBase *)lParam;
        CString str;

        unsigned int iCount=pTest->GetTestCount();
        unsigned int iErrCount=pTest->GetErrCount();
        str.Format(_T("%d"), iCount);
        m_ctrlListInfo.SetItemText(index,LIST_COL_TEST_NUM,str);
        str.Format(_T("%d"),iErrCount );
        m_ctrlListInfo.SetItemText(index,LIST_COL_TEST_ERR_NUM,str);

        if (iCount>0)
        {
            double t;
            t=(double )iErrCount/iCount *100;

            str.Format(_T("%.2f%%"),t);
            m_ctrlListInfo.SetItemText(index,LIST_COL_TEST_ERR_RATE,str);
        }
    }



    return 0;
}

HRESULT CReliabilityTestNoEthDlg::OnTestStatusChange(WPARAM wParam,LPARAM lParam)
{
    if (m_bTesting)
    {
        CString str;
        TEST_STATUS status=(TEST_STATUS)(int)lParam;
        
        switch(status)
        {
        case STATUS_WAITING:
            str=_T("������");
            break;
        case STATUS_TESTING:
            str=_T("���ڲ���");
            break;
        case STATUS_STOP:
            str=_T("ֹͣ");
            break;
        case STATUS_BE_STOPPED:
            str=_T("��ֹͣ");
            break;
        case STATUS_DEAD:
            str=_T("������");
            break;
        default:
            str=_T("�Ƿ�");
            break;
        }
        m_ctrlListInfo.SetItemText((int)wParam,LIST_COL_TEST_STATUS,str);

    }
    return 0;
}

void CReliabilityTestNoEthDlg::OnBnClickedButtonAbout()
{
    CAboutDlg dlg;
    dlg.DoModal();
}

void CReliabilityTestNoEthDlg::OnTimer(UINT_PTR nIDEvent)
{
    static BOOL flag=FALSE;
    CString strReportFile;

    flag=!flag;
    if (flag )
    {
        strReportFile=m_strLogDir+_T("report1.html");
    }
    else
    {
        strReportFile=m_strLogDir+_T("report2.html");
    }


    if (m_pTestManager)
    {        
        m_pTestManager->BuildTestReport(strReportFile);
    }

    CDialog::OnTimer(nIDEvent);
}

void CReliabilityTestNoEthDlg::OnBnClickedButton3()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
}

HANDLE CReliabilityTestNoEthDlg::GetDeviceHandle()
{
    return m_hDevice;
}


void CReliabilityTestNoEthDlg::OpenBackLight()
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
