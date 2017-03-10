// FunctionTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include <WinSock2.h>
#include <IPHlpApi.h>
#include <process.h>
#include "TSocket.h"
#include "connect.h"
#include "NetConnect.h"
#include "ActiveSyncConnect.h"
#include "FunctionTest.h"
#include "FunctionTestDlg.h"
#include "DownLoadDlg.h"
#include "WriteLog.h"
#include "testCommon.h"
#include "UsuallyCommon.h"
#include "CANLib.h"

#pragma comment(lib,"Iphlpapi.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

    // Dialog Data
    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    // Implementation
protected:
    DECLARE_MESSAGE_MAP()
private:
    // ��¼�˳�������ʱ��
    CString m_strBuildTime;
public:
    virtual BOOL OnInitDialog();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
, m_strBuildTime(_T(""))
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_STATIC_BUILD_TIME, m_strBuildTime);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()
BOOL CAboutDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    m_strBuildTime+=__TIME__;
    m_strBuildTime+=L"   ";
    m_strBuildTime+=__DATE__;
    UpdateData(FALSE);
    return TRUE;  
}

CFunctionTestDlg::CFunctionTestDlg(CWnd* pParent /*=NULL*/)
: CDialog(CFunctionTestDlg::IDD, pParent)
,m_pManager(NULL)
,m_hTestWaitEvent(NULL)
,m_hTestThread(NULL)
,m_iTestItem(-1)
,m_bKillTestThread(FALSE)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);	
}

void CFunctionTestDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_TEST_INFO, m_ctrlTestInfo);
    DDX_Control(pDX, IDC_STATIC_TEST_STATUS, m_ctrlStatus);
    DDX_Control(pDX, IDC_COMBO_LOCAL_IP, m_ctrlComboLocalIP);
    DDX_Control(pDX, IDC_IPADDRESS_DEV_IP, m_ctrlDevIP);
    DDX_Control(pDX, IDC_BUTTON_CONNECT, m_btnConnect);
    DDX_Control(pDX, IDC_BUTTON_DOWNLOAD, m_btnDownLoad);
    DDX_Control(pDX, IDC_BUTTON_EXCUTE_TEST, m_btnExcute);
}

BEGIN_MESSAGE_MAP(CFunctionTestDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_BUTTON_EXCUTE_TEST, &CFunctionTestDlg::OnBnClickedButtonExcuteTest)
    ON_CBN_DROPDOWN(IDC_COMBO_LOCAL_IP, &CFunctionTestDlg::OnCbnDropdownComboLocalIp)    
    ON_CBN_CLOSEUP(IDC_COMBO_LOCAL_IP, &CFunctionTestDlg::OnCbnCloseupComboLocalIp)
    ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CFunctionTestDlg::OnBnClickedButtonConnect)

    ON_MESSAGE(UM_NEED_DISP,OnNeedDisp)
    ON_MESSAGE(UM_SHOW_STATUS,OnShowStatus)
    ON_MESSAGE(UM_TEST_BEGIN,OnTestBegin)
    ON_MESSAGE(UM_TEST_END  ,OnTestEnd)
    ON_MESSAGE(UM_CONECT_DOWN,OnConnectDown)
    ON_MESSAGE(UM_CONNECT_FINISH,OnConnectFinish)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD, &CFunctionTestDlg::OnBnClickedButtonDownload)
END_MESSAGE_MAP()


// CFunctionTestDlg message handlers
//������ʾ�����豸��Ϣ�Ĵ�����������Ϣ
#define     LIST_COL_TEST_NAME              0
#define     LIST_COL_TEST_STATUS            1
#define     LIST_COL_TEST_NUM               2
#define     LIST_COL_TEST_ERR_NUM           3
#define     LIST_COL_TEST_RESULT            4

BOOL CFunctionTestDlg::OnInitDialog()
{
    CDialog::OnInitDialog();


    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    //����ϵͳ�˵�
    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
    {
        CString strAboutMenu;
        strAboutMenu.LoadString(IDS_ABOUTBOX);
        if (!strAboutMenu.IsEmpty())
        {
            int nItemCount=pSysMenu->GetMenuItemCount();
            if (nItemCount >2)
            {
                pSysMenu->InsertMenu(nItemCount-2,MF_SEPARATOR|MF_BYPOSITION);
                pSysMenu->InsertMenu(nItemCount-1,MF_STRING|MF_BYPOSITION,IDM_ABOUTBOX,strAboutMenu);

            }
            else
            {
                pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
            }

        }
    }
    VCI_LoadLib();

    if (!LoadGlobalConfig())
    {
        MessageBox(_T("��ȡ�����ļ�ʧ��") );       
        PostMessage(WM_CLOSE);
        return FALSE;
    }

    if (!InitLog())
    {
        MessageBox(L"��ʼ����־ʧ��");
        PostMessage(WM_CLOSE);
        return FALSE;
    }


    //��ʼ����ʾ������Ϣ��CListCtrl�ؼ�
    m_ctrlTestInfo.InsertColumn(0, _T("������"), LVCFMT_LEFT, 140, -1);
    m_ctrlTestInfo.InsertColumn(LIST_COL_TEST_STATUS,   _T("״̬"), LVCFMT_CENTER, 75, -1);
    m_ctrlTestInfo.InsertColumn(LIST_COL_TEST_NUM,      _T("���Դ���"), LVCFMT_CENTER, 75, -1);
    m_ctrlTestInfo.InsertColumn(LIST_COL_TEST_ERR_NUM,  _T("�������"), LVCFMT_CENTER, 75, -1);
    m_ctrlTestInfo.InsertColumn(LIST_COL_TEST_RESULT, _T("���Խ��"), LVCFMT_CENTER, 80, -1);

    DWORD   dwStyle;
    dwStyle = m_ctrlTestInfo.GetStyle();
    dwStyle |= LVS_EX_FULLROWSELECT |LVS_EX_CHECKBOXES|LVS_SINGLESEL;   
    m_ctrlTestInfo.SetExtendedStyle(dwStyle);	

    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    //��ʼ����ʾ����IP��ַ�б��Combo�ؼ�
    struct in_addr LocalAddr[10];
    int count=GetLocalIps(LocalAddr,10);
    m_ctrlComboLocalIP.InsertString(0,_T("�Զ�"));
    CString str;
    for(int i=0;i<count;i++)
    {
        str.Format(_T("%d.%d.%d.%d"),LocalAddr[i].S_un.S_un_b.s_b1,
            LocalAddr[i].S_un.S_un_b.s_b2,LocalAddr[i].S_un.S_un_b.s_b3
            ,LocalAddr[i].S_un.S_un_b.s_b4);
        m_ctrlComboLocalIP.InsertString(i+1,str);
    }
    m_ctrlComboLocalIP.SetCurSel(0);

    //��ʼ���豸IP��ַ����ʾ
    m_ctrlDevIP.SetAddress(192,168,0,2);
    m_iCEEthCount=1;
    m_strCeEthIp[0]=L"192.168.0.2";

    //���ý��в��԰�ť
    GetDlgItem(IDC_BUTTON_EXCUTE_TEST)->EnableWindow(FALSE);
    if (m_connectWay == CONNECT_WAY_ACTIVE_SYNC)
    {
        m_ctrlComboLocalIP.EnableWindow(FALSE);
        m_ctrlDevIP.EnableWindow(FALSE);
    }

    //���������߳�
    m_hTestWaitEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
    m_bKillTestThread=FALSE;
    m_iTestItem=0;
    m_hTestThread=(HANDLE)_beginthreadex(NULL,0,TestThreadProc,this,0,NULL);
    if (m_hTestThread ==NULL)
    {
        MessageBox(_T("���������߳�ʧ��") );
        CloseHandle(m_hTestWaitEvent);
        m_hTestWaitEvent=NULL;
        PostMessage(WM_CLOSE);
    }

    memset(bTestSuc, 0, sizeof(bTestSuc));

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFunctionTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialog::OnSysCommand(nID, lParam);
    }
}


void CFunctionTestDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFunctionTestDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CFunctionTestDlg::OnBnClickedButtonExcuteTest()
{
    int iItem=m_ctrlTestInfo.GetNextItem(-1,LVNI_SELECTED);
    m_iTestItem=iItem;
    SetEvent(m_hTestWaitEvent);
    GetDlgItem(IDC_BUTTON_EXCUTE_TEST)->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTON_DOWNLOAD)->EnableWindow(FALSE);

}

void CFunctionTestDlg::OnCbnDropdownComboLocalIp()
{
    //�ڴ˴����»�ȡIP��ַ�б���������ʾ��������
    struct in_addr LocalAddr[10];
    int count=GetLocalIps(LocalAddr,10);
    m_ctrlComboLocalIP.ResetContent();
    m_ctrlComboLocalIP.InsertString(0,_T("�Զ�"));
    CString str;
    for(int i=0;i<count;i++)
    {
        str.Format(_T("%d.%d.%d.%d"),LocalAddr[i].S_un.S_un_b.s_b1,
            LocalAddr[i].S_un.S_un_b.s_b2,LocalAddr[i].S_un.S_un_b.s_b3
            ,LocalAddr[i].S_un.S_un_b.s_b4);
        m_ctrlComboLocalIP.InsertString(i+1,str);
    }
    m_ctrlComboLocalIP.SetCurSel(0);
}

void CFunctionTestDlg::OnCbnCloseupComboLocalIp()
{
    if (m_ctrlComboLocalIP.GetCurSel() <0 )
    {
        m_ctrlComboLocalIP.SetCurSel(0);
    }
}

void CFunctionTestDlg::OnBnClickedButtonConnect()
{
    CString     str;
    GetDlgItem(IDC_BUTTON_CONNECT)->GetWindowText(str);
    if (str==_T("����"))
    {
        GetDlgItem(IDC_BUTTON_CONNECT) ->EnableWindow(FALSE);
        CMenu *menu = GetSystemMenu(FALSE);
        if (menu)
        {
            menu->EnableMenuItem(SC_CLOSE,MF_BYCOMMAND|MF_GRAYED);
        }
        HANDLE      h= (HANDLE)_beginthreadex(NULL,0,ConnectThreadProc,this,0,NULL);
        CloseHandle(h);
    }
    else
    {
        CConnect *p=m_pManager->DettachConnect();
        if (p!=NULL)
        {
            delete p;
        }

        //������ؿؼ���״̬
        if (m_connectWay == CONNECT_WAY_ETNERNET)
        {
            m_ctrlDevIP.EnableWindow(TRUE);
            m_ctrlComboLocalIP.EnableWindow(TRUE);
        }
        GetDlgItem(IDC_BUTTON_DOWNLOAD)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_EXCUTE_TEST)->EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON_CONNECT)->SetWindowText(_T("����"));
        SetWindowText(m_strTitle);
    }

}

void CFunctionTestDlg::OnDestroy()
{
    CDialog::OnDestroy();
    //�����˳������߳�
    if (m_hTestThread !=NULL)
    {
        m_bKillTestThread=TRUE;
        SetEvent(m_hTestWaitEvent);

        DWORD nRet=WaitForSingleObject(m_hTestThread,500);
        while(nRet ==WAIT_TIMEOUT)
        {
            //����������һ���򵥵���Ϣѭ��������ɶ�������Ϣ�Ĵ���
            MSG msg;
            while(PeekMessage(&msg,NULL,0,0,PM_REMOVE)!=0 )
            {
                TranslateMessage (&msg) ;
                DispatchMessage (&msg) ;
            }
            nRet=WaitForSingleObject(m_hTestThread,500);
        }

        CloseHandle(m_hTestThread);
        CloseHandle(m_hTestWaitEvent);

        m_hTestThread=NULL;
        m_hTestWaitEvent=NULL;
    }

    //ɾ�����ܴ��ڵĲ��Թ���������
    if (m_pManager !=NULL)
    {
        CConnect *pConnect=m_pManager->DettachConnect();
        if (pConnect)
        {
            delete pConnect;
        }
        delete m_pManager;
        m_pManager=NULL;
    }
}

unsigned int CFunctionTestDlg::TestThreadProc(void *pVoid)
{
    CFunctionTestDlg *pDlg=(CFunctionTestDlg *)pVoid;

    while(1)
    {
        WaitForSingleObject(pDlg->m_hTestWaitEvent,INFINITE );
TEST_RETRY:
        if (pDlg->m_bKillTestThread)
        {
            break;
        }
        int count;  
        int nIndex = pDlg->m_iTestItem;
        CTestBase *pTest=pDlg->m_pManager->GetTestIndex(nIndex);
        count=pTest->GetShouldTestTimes();
        pTest->ResetTestCount();

        if(9 == nIndex)
        {
            if(!pDlg->bTestSuc[nIndex - 1] && !pDlg->bTestSuc[nIndex - 2])
            {
                AfxMessageBox(L"�ڽ��д������֮ǰFT3_RX1���պ�FT3_RX2���ղ���������һ��Ҫ�Ȳ���ͨ��");
                pDlg->GetDlgItem(IDC_BUTTON_EXCUTE_TEST)->EnableWindow(TRUE);
                continue;
            }
        }

        TEST_STATUS     status;
        CConnect        *pConnet=pDlg->m_pManager->GetConnect();

        //֪ͨͼ�ν��棬��һ�������ʼ�����ˣ���ʹͼ�ν�����ʾ��ȷ����Ϣ
        pDlg->SendMessage(UM_TEST_BEGIN,(WPARAM)pDlg->m_iTestItem,0);

        //��ȡ��ʼ����Ϣ
        status=pTest->OnBeginTest(pConnet);
        if (status ==TEST_CONNECTDOWN)
        {
            //���ӶϿ��Ĵ���                        
            goto TEST_CONNECTDOWN;
        }
        else if (status==TEST_ERROR)
        {
            //��ʼ��ʧ��
            pDlg->SendMessage(UM_NEED_DISP,(WPARAM)pDlg->m_iTestItem,0);
            pDlg->SendMessage(UM_TEST_END,(WPARAM)pDlg->m_iTestItem,0);
            continue;
        }

        int i=0;
        while(i<count)
        {
            if (pDlg->m_bKillTestThread)
            {
                break;
            }
            status=pTest->ExcuteOneTest(pConnet);
            if (pDlg->m_bKillTestThread)
            {
                break;
            }
            switch(status)
            {
            case TEST_NORMAL:
            case TEST_ERROR:
                pDlg->SendMessage(UM_NEED_DISP,(WPARAM)pDlg->m_iTestItem,0);
                break;
            case  TEST_CONNECTDOWN:       
                goto TEST_CONNECTDOWN;
            case TEST_ABORT:
                goto TEST_Proc_End;
            default:
                break;
            }
            i++;
        }
        //������ɺ�Ĵ���         
TEST_Proc_End:
        if (pDlg->m_bKillTestThread)
        {
            break;
        }
        status=pTest->OnEndTest(pConnet);
        if (status ==TEST_CONNECTDOWN)
        {
            //���ӶϿ��Ĵ���
            goto    TEST_CONNECTDOWN;
        }
        pDlg->SendMessage(UM_TEST_END,(WPARAM)pDlg->m_iTestItem,0);
        continue;
TEST_CONNECTDOWN:
        if (pDlg->m_connectWay == CONNECT_WAY_ETNERNET)
        {
            if (pDlg->m_iCEEthCount>1)
            {
                CString     str=L"��ǰ����";
                str+=pDlg->m_strDestIP;
                str+=L"�ѶϿ�,�����Զ��������ӵ���������";

                pDlg->SendMessage(UM_SHOW_STATUS,0,(LPARAM)(LPCWSTR)str);
                //���CE�����������������Զ����ӵ���������
                for (int i=0;i<pDlg->m_iCEEthCount;i++)
                {
                    if (pDlg->m_strCeEthIp[i]!=pDlg->m_strDestIP)
                    {
                        BOOL    bConnect;
                        CNetConnect *pConnect=new CNetConnect;
                        str.Format(L"���ڳ������ӵ�%s",(LPCWSTR)pDlg->m_strCeEthIp[i]) ;
                        pDlg->SendMessage(UM_SHOW_STATUS,0,(LPARAM)(LPCWSTR)str);
                        bConnect=pConnect->Connect(pDlg->m_strCeEthIp[i],SERVER_PORT,NULL);
                        if (bConnect)
                        {
                            //���ӳɹ�
                            str.Format(L"���ӵ�%s�ɹ�",(LPCWSTR)pDlg->m_strCeEthIp[i]) ;
                            pDlg->SendMessage(UM_SHOW_STATUS,0,(LPARAM)(LPCWSTR)str);
                            CConnect *p=pDlg->m_pManager->ReplaceConnect(pConnect);
                            if(p!=NULL)
                                delete p;
                            pDlg->m_strDestIP=pDlg->m_strCeEthIp[i];
                            goto TEST_RETRY;

                        }                   
                        delete pConnect;

                    }                
                }
            }
            {
                //�ٳ���һ��ԭ�ȵ�IP
                BOOL    bConnect;
                CString str;
                CNetConnect *pConnect=new CNetConnect;
                str.Format(L"���ڳ������ӵ�%s",(LPCWSTR)pDlg->m_strDestIP) ;
                pDlg->SendMessage(UM_SHOW_STATUS,0,(LPARAM)(LPCWSTR)str);
                bConnect=pConnect->Connect(pDlg->m_strDestIP,SERVER_PORT,NULL);
                if (bConnect)
                {
                    //���ӳɹ�
                    str.Format(L"���ӵ�%s�ɹ�",(LPCWSTR)pDlg->m_strDestIP) ;
                    pDlg->SendMessage(UM_SHOW_STATUS,0,(LPARAM)(LPCWSTR)str);
                    CConnect *p=pDlg->m_pManager->ReplaceConnect(pConnect);
                    if(p!=NULL)
                        delete p;
                    goto TEST_RETRY;

                }                   
                delete pConnect;
            }
        }
        pDlg->SendMessage(UM_CONECT_DOWN,(WPARAM)pDlg->m_iTestItem,0);         
    }

    return 0;
}

HRESULT CFunctionTestDlg::OnNeedDisp(WPARAM wParam,LPARAM lParam)
{
    int         index=(int )wParam;
    CString     str;
    const CTestBase   *pTest;

    pTest=m_pManager->GetTestIndex(index);
    str.Format(_T("%d"),pTest->GetTestedCount() );    
    m_ctrlTestInfo.SetItemText(index,LIST_COL_TEST_NUM,str);

    str.Format(_T("%d"),pTest->GetTestedErrCount() );    
    m_ctrlTestInfo.SetItemText(index,LIST_COL_TEST_ERR_NUM,str);
    return 0;
}

HRESULT CFunctionTestDlg::OnConnectDown(WPARAM wParam, LPARAM lParam)
{
    MessageBox(_T("�����Ѿ��Ͽ��������������Ӻ͹����Բ��Կͻ��˵Ĺ������"),
        _T("����"),MB_OK|MB_ICONERROR);
    if (m_pManager)
    {
        CConnect *p=m_pManager->DettachConnect();
        if (p!=NULL)
        {
            delete p;
        }
        //������ؿؼ���״̬
        if (m_connectWay == CONNECT_WAY_ETNERNET)
        {
            m_ctrlDevIP.EnableWindow(TRUE);
            m_ctrlComboLocalIP.EnableWindow(TRUE);
        }
        m_ctrlTestInfo.SetItemText((int)wParam,LIST_COL_TEST_STATUS,_T("���ӶϿ�") );
        m_ctrlTestInfo.SetItemColor((int)wParam,RGB(255,0,128));
        m_ctrlTestInfo.RedrawItems((int)wParam,(int)wParam );
    }
    GetDlgItem(IDC_BUTTON_DOWNLOAD)->EnableWindow(TRUE);
    GetDlgItem(IDC_BUTTON_EXCUTE_TEST)->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTON_CONNECT)->SetWindowText(_T("����"));
    GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(TRUE);    
    m_ctrlStatus.SetWindowText(_T("���ӶϿ�"));
    return 0;

}

HRESULT CFunctionTestDlg::OnShowStatus(WPARAM wParam,LPARAM lParam)
{
    m_ctrlStatus.SetWindowText((const WCHAR *)lParam );
    return 0;
}

HRESULT CFunctionTestDlg::OnTestBegin(WPARAM wParam,LPARAM lParam)
{
    m_ctrlTestInfo.SetItemText((int)wParam,LIST_COL_TEST_STATUS,_T("���ڲ���") );
    m_ctrlTestInfo.SetItemText( (int)wParam,LIST_COL_TEST_ERR_NUM,_T("0")  );
    m_ctrlTestInfo.SetItemText( (int)wParam,LIST_COL_TEST_NUM,_T("0")  );
    m_ctrlTestInfo.SetItemText( (int)wParam,LIST_COL_TEST_RESULT,_T("--")  );
    m_ctrlTestInfo.SetItemColor((int)wParam,RGB(0,0,0));
    m_ctrlTestInfo.RedrawItems((int)wParam,(int)wParam );
    m_ctrlStatus.SetWindowText(_T("���ڲ���..."));  
    return 0;
}

HRESULT CFunctionTestDlg::OnTestEnd(WPARAM wParm, LPARAM lParma)
{
    int         index=(int )wParm;
    int         iErrNum;

    OnNeedDisp(wParm,0);
    const CTestBase   *pTest;
    pTest=m_pManager->GetTestIndex(index);

    int iTotalCOunt=m_pManager->GetTestCount();
    iErrNum=pTest->GetTestedErrCount();

    if (iErrNum ==0)
    {
        m_ctrlTestInfo.SetItemColor(index,RGB(0,128,0));
        m_ctrlTestInfo.SetItemText( index,LIST_COL_TEST_RESULT,_T("��������")  );
        m_ctrlStatus.SetWindowText(_T("�������"));
        bTestSuc[index] = TRUE;
    }
    else
    {
        if (iErrNum <(int )pTest ->GetTestedCount())
        {
            m_ctrlTestInfo.SetItemColor(index,RGB(255,128,0));
        }
        else
        {
            m_ctrlTestInfo.SetItemColor(index,RGB(255,0,0));
        }

        m_ctrlTestInfo.SetItemText( index,LIST_COL_TEST_RESULT,_T("���Գ���")  );
        m_ctrlStatus.SetWindowText(_T("���Գ���"));
    }

    if ((int )pTest ->GetTestedCount() ==0)
    {
        //���Դ���Ϊ0��������ʼ��ʧ��
        m_ctrlTestInfo.SetItemColor(index,RGB(255,128,128));
        m_ctrlTestInfo.SetItemText( index,LIST_COL_TEST_RESULT,_T("���Գ���")  );
        m_ctrlStatus.SetWindowText(_T("���Գ���"));
    }

    m_ctrlTestInfo.SetItemText( index,LIST_COL_TEST_STATUS,_T("�������")  );
    for (int i=0;i<iTotalCOunt;i++)
    {
        m_ctrlTestInfo.SetCheck(i,FALSE);
        m_ctrlTestInfo.SetItemState(i,0,LVNI_SELECTED);
    }

    m_ctrlTestInfo.SetCheck(index,FALSE);
    m_ctrlTestInfo.SetItemState(index,0,LVNI_SELECTED);
    index=(index+1)%iTotalCOunt;

    m_ctrlTestInfo.SetCheck(index,TRUE);
    m_ctrlTestInfo.SetItemState(index,LVNI_SELECTED,LVNI_SELECTED);

    //������ؿؼ�
    GetDlgItem(IDC_BUTTON_EXCUTE_TEST)->EnableWindow(TRUE);
    GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(TRUE);
    GetDlgItem(IDC_BUTTON_DOWNLOAD)->EnableWindow(TRUE);
    ;
    return 0;
}

void CFunctionTestDlg::OnBnClickedButtonDownload()
{
    CDownLoadDlg dlg;
    if (dlg.DoModal() ==IDOK)
    {
        m_iCEEthCount=0;
        if (!dlg.m_strIp1.IsEmpty())
        {
            m_strCeEthIp[m_iCEEthCount++]=dlg.m_strIp1;
        }
        if (!dlg.m_strIp2.IsEmpty())
        { 
            m_strCeEthIp[m_iCEEthCount++]=dlg.m_strIp2;
        }
        if (m_iCEEthCount==0)
        {
            m_iCEEthCount=1;
            m_strCeEthIp[0]=L"192.168.0.2";
        }

        m_ctrlDevIP.SetAddress( htonl(inet_addrW(m_strCeEthIp[0]) ) );
    }       
}

BOOL CFunctionTestDlg::LoadGlobalConfig()
{
    //��ȡ��ǰ��ִ���ļ���Ŀ¼
    CString     strIniPath;

    GetExeDirectory(strIniPath.GetBuffer(MAX_PATH*2),MAX_PATH*2 );
    strIniPath.ReleaseBuffer();
    strIniPath+=_T("FunctionTest.ini");

    //����Ӧ�ó���ı���
    GetPrivateProfileString(L"Application",L"Title",L"���ܲ���",m_strTitle,256,strIniPath);
    SetWindowText(m_strTitle);

    //������ͻ������ӵķ�ʽ
    m_connectWay = (CONNECT_WAY)(int)GetPrivateProfileInt(L"Application",L"ConnectWay",0,strIniPath);
    if (m_connectWay!=CONNECT_WAY_ACTIVE_SYNC)
    {
        m_connectWay = CONNECT_WAY_ETNERNET;
    }
    return TRUE;
}

BOOL CFunctionTestDlg::InitLog()
{
    //���Ȳ��Ҳ�ȷ����һ����ŵ�Ŀ¼������д��־

    //���ȴ������ڴ����־��Ŀ¼
    CString         strLogDir;
    WCHAR           *p=strLogDir.GetBuffer(MAX_PATH*2);
    GetExeDirectory(p,MAX_PATH*2);
    strLogDir.ReleaseBuffer(m_iTestItem);
    strLogDir+=L"Reports\\";

    CreateDirectory(strLogDir,NULL);

    //���Ҿֲ�����ŵ��ļ���
    int         nMax=0;
    CString     strPrex=L"Test";
    HANDLE      hFind=INVALID_HANDLE_VALUE ;
    CString     strTmp=strLogDir+L"*.*";
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
    strLogDir+=strTmp;
    CreateDirectory(strLogDir,NULL);
    strLogDir+=_T("\\");
    strTmp=strLogDir+L"testError.log";   

    return InitWriteLog(strTmp,this->m_hWnd);
}

unsigned int CFunctionTestDlg::ConnectThreadProc(void *pVoid)
{
    CFunctionTestDlg            *pDlg = (CFunctionTestDlg *)pVoid;
    CConnect                    *pConnect = NULL;
    if (pDlg->m_connectWay == CONNECT_WAY_ETNERNET)
    {
        CNetConnect     *pNetConnect;
        pNetConnect=new CNetConnect();
        if (pNetConnect==NULL)
        {
            pDlg->MessageBox(_T("�����ڴ�ʧ��"));
            goto CONNECT_END;
        }

        CString strLocalIP,strDevIP;
        BYTE b1,b2,b3,b4;

        pDlg->m_ctrlDevIP.GetAddress(b1,b2,b3,b4);
        strDevIP.Format(_T("%d.%d.%d.%d"),b1,b2,b3,b4);
        //����CE��IP��ַ����
        {
            BOOL    bSame=FALSE;
            for (int i=0;i<pDlg->m_iCEEthCount;i++)
            {
                if (pDlg->m_strCeEthIp[i]==strDevIP)
                {
                    bSame=TRUE;
                    break;
                }
            }

            if (!bSame)
            {
                pDlg->m_strCeEthIp[0]=strDevIP;
            }
        }
        int index=pDlg->m_ctrlComboLocalIP.GetCurSel();
        if (index >=0)
        {
            pDlg->m_ctrlComboLocalIP.GetLBText(index,strLocalIP);
        }
        else
        {
            strLocalIP=_T("�Զ�");
        }

        BOOL bConnect;

        pDlg->m_ctrlStatus.SetWindowText(L"�������ӵ�"+strDevIP+L"...");
        if (strLocalIP==_T("�Զ�") )
        {
            bConnect=pNetConnect->Connect(strDevIP,SERVER_PORT,NULL);
        }
        else
        {
            bConnect=pNetConnect->Connect(strDevIP,SERVER_PORT,strLocalIP);
        }

        pDlg->m_strDestIP=strDevIP;
        if (!bConnect)
        {
            pDlg->m_ctrlStatus.SetWindowText(L"���ӵ�"+strDevIP+L"ʧ��");
            //pDlg->MessageBox(_T("����ʧ��"),_T("����"),MB_OK|MB_ICONERROR);
            delete pNetConnect;
            goto CONNECT_END;
        }
        pConnect = pNetConnect;
    }
    else if (pDlg->m_connectWay == CONNECT_WAY_ACTIVE_SYNC)
    {
        CActiveSyncConnect *pActiveConnect;
        pActiveConnect=new CActiveSyncConnect();
        BOOL bConnect= pActiveConnect->Connect();
        if (!bConnect)
        {
            pDlg->m_ctrlStatus.SetWindowText(L"���ӵ��豸ʧ��");
            //pDlg->MessageBox(_T("����ʧ��"),_T("����"),MB_OK|MB_ICONERROR);
            delete pActiveConnect;
            goto CONNECT_END;
        }

        pConnect = pActiveConnect;

    }

    //���ӳɹ�
    if (pDlg->m_pManager!=NULL)
    {
        CConnect *p=pDlg->m_pManager->DettachConnect();
        if (p)
        {
            delete p;
        }
        p =NULL;
        delete pDlg->m_pManager;
        pDlg->m_pManager=NULL;
    }

    pDlg->m_pManager=new CTestManager();
    if (pDlg->m_pManager->Init(pConnect,pDlg->m_hWnd))
    {
        
    }
    else
    {
        delete pConnect;
        delete pDlg->m_pManager;
        pDlg->m_pManager=NULL;
        pConnect = NULL;
    }


CONNECT_END:
    pDlg->PostMessage(UM_CONNECT_FINISH,0,(pConnect!=NULL?1:0));

    return 0;
}

HRESULT CFunctionTestDlg::OnConnectFinish(WPARAM wParm, LPARAM lParma)
{
    if (lParma)
    {
        m_ctrlTestInfo.DeleteAllItems();
        m_ctrlTestInfo.ResetAllItemsColor();

        //�ڽ�������ʾ�����еĲ�����
        int cnt=m_pManager->GetTestCount();
        for (int i=0;i<cnt;i++)
        {
            const CTestBase *pBase=m_pManager->GetTestIndex(i);
            if (pBase!=NULL)
            {
                m_ctrlTestInfo.InsertItem(i,pBase->GetTestObjName());
                m_ctrlTestInfo.SetItemText(i,LIST_COL_TEST_STATUS,L"������");
                m_ctrlTestInfo.SetItemText(i,LIST_COL_TEST_NUM,L"0");
                m_ctrlTestInfo.SetItemText(i,LIST_COL_TEST_ERR_NUM,L"0");
                m_ctrlTestInfo.SetItemText(i,LIST_COL_TEST_RESULT,L"--");
            }
        }

        SetWindowText((CString)m_strTitle + m_pManager->GetDeviceInfo()->strDeviceVer);

        //��ʾ���ӳɹ���״̬
        CString strStatus=_T("�����豸�ɹ�,�豸����Ϊ:");
        strStatus+=m_pManager->GetDeviceInfo()->strDeviceType;
        m_ctrlStatus.SetWindowText(strStatus);

        GetDlgItem(IDC_BUTTON_CONNECT)->SetWindowText(_T("�Ͽ�"));
        GetDlgItem(IDC_BUTTON_CONNECT) ->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON_EXCUTE_TEST)->EnableWindow(TRUE);
        //������ؿؼ���״̬
        m_ctrlDevIP.EnableWindow(FALSE);
        m_ctrlComboLocalIP.EnableWindow(FALSE);
        m_ctrlTestInfo.SetItemState(0,LVNI_SELECTED,LVNI_SELECTED);
        m_ctrlTestInfo.SetCheck(0,TRUE);
    }
    else
    {
        GetDlgItem(IDC_BUTTON_CONNECT) ->EnableWindow(TRUE);
        
    }
    CMenu *menu = GetSystemMenu(FALSE);
    if (menu)
    {
        menu->EnableMenuItem(SC_CLOSE,MF_BYCOMMAND|MF_ENABLED);
    }

    return 0;

}