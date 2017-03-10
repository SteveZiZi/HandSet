// DownLoadActiveSyncDlg.cpp : implementation file
//

#include "stdafx.h"
#include <winsock2.h>
#include <process.h>
#include "DownLoadActiveSync.h"
#include "DownLoadActiveSyncDlg.h"
#include "rapi.h"
#include "UsuallyCommon.h"
#include <stack>
#include <string>
using namespace std;

#pragma comment(lib,"rapi.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

UINT32 inet_addrW(const WCHAR *cp)
{
    int i=0;
    UINT32 t[4]={0};
    int tPos=0;

    int     iNumCharNum=0; 

    if(cp==NULL)
        return 0;

    //���������ַ�����ͷ���ܵĿո�
    while(cp[i] ==L' ')
    {
        i++;
    }
    while(cp[i])
    {
        switch(cp[i])
        {
        case L'0':
        case L'1':
        case L'2':
        case L'3':
        case L'4':
        case L'5':
        case L'6':
        case L'7':
        case L'8':
        case L'9': 
            t[tPos]=t[tPos]*10+(cp[i]-L'0');
            if(t[tPos] >255)
            {
                return INADDR_NONE;
            }
            iNumCharNum++;
            break;
        case L'.':
            if (iNumCharNum ==0)
            {
                return INADDR_NONE;
            }
            iNumCharNum=0;

            tPos++;
            break;        
        default:
            if(tPos<3 || iNumCharNum ==0)
            {
                return INADDR_NONE;
            }
            goto sucessed;
            break;
        }
        i++;
    }


sucessed:
    UINT32 uiReturned;

    BYTE *p=(BYTE *)&uiReturned;
    p[0]=t[0];
    p[1]=t[1];
    p[2]=t[2];
    p[3]=t[3];

    return uiReturned;
}

//��ȡһ���ļ����°������ļ����е��ļ��ĸ���,���ce�ͻ��˵�Զ�̵���
int  CeGetDirFileCountAll(const WCHAR *strDirectory)
{
    wstring     strFindPath=strDirectory;
    wstring     strFind,strFindTemp;
    HANDLE      hFind;
    BOOL        bFind=FALSE;
    CE_FIND_DATA wfd;

    int         count=0;
    strFind=strFindPath+L"*.*";
    hFind=CeFindFirstFile(strFind.c_str(),&wfd);

    if (hFind !=INVALID_HANDLE_VALUE)
    {
        do 
        {
            if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY  )
            {
                if (wfd.cFileName[0]!=L'.' )
                {
                    strFindTemp=strFindPath+wfd.cFileName;
                    strFindTemp+=L"\\";
                    count+=CeGetDirFileCountAll(strFindTemp.c_str());
                }
            }
            else
            {
                count++;
            }

            bFind=CeFindNextFile(hFind,&wfd);
        } while (bFind);
        CeFindClose(hFind);
        hFind=INVALID_HANDLE_VALUE;
    }

    return count;
}

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
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CDownLoadActiveSyncDlg::CDownLoadActiveSyncDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDownLoadActiveSyncDlg::IDD, pParent)
    , m_strStatus(_T(""))
    , m_iClientIpNum(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDownLoadActiveSyncDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_IPADDRESS_SERVER1, m_ctrlServerIP);
    DDX_Text(pDX, IDC_STATIC_STATUS, m_strStatus);
    DDX_Control(pDX, IDC_IPADDRESS_IP1, m_ctrlIp1);
    DDX_Control(pDX, IDC_IPADDRESS_IP2, m_ctrlIp2);
    DDX_Control(pDX, IDC_BUTTON_DOWN, m_btnDownLoad);
    DDX_Control(pDX, IDC_BUTTON_CANCEL, m_btnCancel);
    DDX_Control(pDX, IDC_ALL_PROGRESS, m_ctrlProcess);
}

BEGIN_MESSAGE_MAP(CDownLoadActiveSyncDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_MESSAGE(UM_SHOW_STATUS,OnShowStatus)
    ON_MESSAGE(UM_INIT_FINISHED,OnInitFinished)
    ON_WM_DESTROY()  
    ON_BN_CLICKED(IDC_BUTTON_DOWN, &CDownLoadActiveSyncDlg::OnBnClickedButtonDown)
    ON_BN_CLICKED(IDC_BUTTON_REMOVE, &CDownLoadActiveSyncDlg::OnBnClickedButtonRemove)
END_MESSAGE_MAP()


// CDownLoadActiveSyncDlg message handlers

BOOL CDownLoadActiveSyncDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

    //��ʼ��������IP��ַ
    char szHostName[32] = {0};
    gethostname(szHostName, 32);
    HOSTENT *pIpList = gethostbyname(szHostName);
    if (pIpList->h_addr_list[0]) {
        m_ctrlServerIP.SetAddress(pIpList->h_addr_list[0][0], pIpList->h_addr_list[0][1], pIpList->h_addr_list[0][2], pIpList->h_addr_list[0][3]);
    }
    m_strStatus=_T("���ڶ�ȡ������Ϣ...");
    UpdateData(FALSE);

    //��ȡ��ǰ��ִ���ļ����ڵ�Ŀ¼
    WCHAR strPath[MAX_PATH *2];
    strPath[0]=0;
    int len=GetExeDirectory(strPath,MAX_PATH*2);
    m_strExePath=strPath;

    m_ctrlIp1.SetAddress(255,255,255,0);
    m_ctrlIp2.SetAddress(255,255,255,0);
    m_ctrlIp1.EnableWindow(FALSE);
    m_ctrlIp2.EnableWindow(FALSE);
    

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);
    m_ctrlProcess.SetRange(0,100);
    m_ctrlProcess.SetPos(0);
    //������ʼ���߳�
    HANDLE h=(HANDLE)_beginthreadex(NULL,0,InitProc,this,0,NULL);
    CloseHandle(h);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

unsigned int CDownLoadActiveSyncDlg::InitProc(void *pVoid)
{
    CDownLoadActiveSyncDlg *pDlg=(CDownLoadActiveSyncDlg *)pVoid;
    Sleep(100);
    CString     strIniFilePath=pDlg->m_strExePath;
    WCHAR       t[128];
    strIniFilePath+=L"DownLoadActiveSync.ini";

    //��ȡ�������ļ��е�·��
    int len=GetPrivateProfileString(L"global",L"AutoDir",L"\\FlashDisk\\StartUp\\",
        t,128,strIniFilePath);
    if (t[len-1]!=L'\\')
    {
        t[len+1]=0;
        t[len]=L'\\';
    }
    pDlg->m_strStartFolder=t;

    //��ȡ�ȶ��Բ��Կͻ������ڵ�·��
    len=GetPrivateProfileString(L"global",L"DevExeDir",L"\\FlashDisk\\ReliabilityTestCE\\",
        t,128,strIniFilePath);
    if (t[len-1]!=L'\\')
    {
        t[len+1]=0;
        t[len]=L'\\';
    }
    pDlg->m_strDevBaseDir=t;
  
    //�������ļ���ȡ��̫����������Ϣ
    if (!IsFileExist(strIniFilePath))
    {
        pDlg->MessageBox(_T("�����ļ�������"),
            _T("����"),MB_OK|MB_ICONERROR);
        pDlg->PostMessage(WM_CLOSE);
        return 0;
    }
    
    int num=GetPrivateProfileInt(L"global",L"EthNum",0,strIniFilePath);
    pDlg->m_iClientIpNum=num;
    if (num>0)
    {
        GetPrivateProfileString(L"global",L"RegIP1",L"Comm\\AX88796B1\\Parms\\TcpIp",
            t,128,strIniFilePath);
        pDlg->m_strIp1SubKey=t;
    }

    if (num>1)
    {
        GetPrivateProfileString(L"global",L"RegIP2",L"Comm\\AX88796B_21\\Parms\\TcpIp",
            t,128,strIniFilePath);
        pDlg->m_strIp2SubKey=t;
    }

    //����ͻ��˲����������������÷����IP�Ŀؼ�����
    

    //��ʼ��RAPI
    RAPIINIT ri;
    ri.cbSize = sizeof(ri);
    CeRapiInitEx(&ri);
    DWORD ret = WaitForSingleObject(ri.heRapiInit, 1000);
    if((ret != S_OK) || (ri.hrRapiInit != S_OK)) {
        pDlg->MessageBox(_T("RAPI������ʼ��ʧ�ܣ�����ActiveSync�Ƿ�������"),
            _T("����"),MB_OK|MB_ICONERROR);
        pDlg->PostMessage(WM_CLOSE);
        return 0;
    }    

    //��ȡ��̫��1��IP��ַ
    HKEY hSubKey = NULL;
    DWORD dwResult = 0;
    DWORD dwType = REG_SZ;
    WCHAR szValue[256] = {0};
    DWORD dwValueLen = sizeof(szValue);
    BOOL  bError=FALSE;
    if (num>0)
    {
        dwResult = ::CeRegOpenKeyEx(HKEY_LOCAL_MACHINE, pDlg->m_strIp1SubKey, 0, KEY_READ, &hSubKey);
        if (dwResult != ERROR_SUCCESS)
        {
            bError=TRUE;
            goto ETH1_FINISH;
        }
        dwResult = ::CeRegQueryValueEx(hSubKey, L"IpAddress", 0, &dwType, (BYTE *)szValue, &dwValueLen);
        if (dwResult != ERROR_SUCCESS)
        {
            bError=TRUE;
            goto ETH1_FINISH;
        }
        szValue[dwValueLen/sizeof(WCHAR)]=0;
        CeRegCloseKey(hSubKey);
        pDlg->m_strIp1=szValue;

    }
ETH1_FINISH:
    if (bError)
    {
        //��ȡ��̫��1IP��ַ����
        pDlg->m_strIp1.Empty();
        pDlg->m_strIp1SubKey.Empty();
        pDlg->SendMessage(UM_SHOW_STATUS,0,(LPARAM)L"��ȡ��̫��1IP��ַ����" );
    }

    //��ȡ��̫��2��IP��ַ
    hSubKey = NULL;
    dwResult = 0;
    dwType = REG_SZ;
    dwValueLen = sizeof(szValue);
    bError=FALSE;
    if (num>1)
    {
        dwResult = ::CeRegOpenKeyEx(HKEY_LOCAL_MACHINE, pDlg->m_strIp2SubKey, 0, KEY_READ, &hSubKey);
        if (dwResult != ERROR_SUCCESS)
        {
            bError=TRUE;
            goto ETH2_FINISH;
        }
        dwResult = ::CeRegQueryValueEx(hSubKey, L"IpAddress", 0, &dwType, (BYTE *)szValue, &dwValueLen);
        if (dwResult != ERROR_SUCCESS)
        {
            bError=TRUE;
            goto ETH2_FINISH;
        }
        szValue[dwValueLen/sizeof(WCHAR)]=0;
        CeRegCloseKey(hSubKey);
        pDlg->m_strIp2=szValue;

    }
ETH2_FINISH:
    if (bError)
    {
        //��ȡ��̫��2IP��ַ����
        pDlg->m_strIp2.Empty();
        pDlg->m_strIp2SubKey.Empty();
        pDlg->SendMessage(UM_SHOW_STATUS,0,(LPARAM)L"��ȡ��̫��2IP��ַ����" );
    }
    //��ȡ������Ϣ���
    pDlg->SendMessage(UM_INIT_FINISHED,0,0);

    return 0;
}

HRESULT CDownLoadActiveSyncDlg::OnShowStatus(WPARAM wParam,LPARAM lParam)
{
    m_strStatus=(LPCWSTR)lParam;
     m_ctrlProcess.SetPos((int)wParam);
    UpdateData(FALSE);
    return 0;
}

HRESULT CDownLoadActiveSyncDlg::OnInitFinished(WPARAM wParam,LPARAM lParam)
{
    BYTE b[4];
    UINT32 uiIP;
    //���ݶ�ȡ���豸����̫����IP��ַ����ʾ
    if (!m_strIp1.IsEmpty() )
    {   
        uiIP=inet_addrW(m_strIp1);
        *((UINT *)b)=uiIP;        
        m_ctrlIp1.SetAddress(b[0],b[1],b[2],b[3]);
        m_ctrlIp1.EnableWindow(TRUE);
    }
    if (!m_strIp2.IsEmpty() )
    {
        uiIP=inet_addrW(m_strIp2);
        *((UINT *)b)=uiIP;
        m_ctrlIp2.SetAddress(b[0],b[1],b[2],b[3]);
        m_ctrlIp2.EnableWindow(TRUE);
    }

    if (m_iClientIpNum <=0)
    {
        m_ctrlServerIP.EnableWindow(FALSE);
    }
    else
    {
         m_ctrlServerIP.EnableWindow(TRUE);
    }

    m_btnDownLoad.EnableWindow(TRUE);
    m_btnCancel.EnableWindow(TRUE);
    m_strStatus=L"��ȡ������Ϣ���";
    UpdateData(FALSE);
    return 0;
}

void CDownLoadActiveSyncDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDownLoadActiveSyncDlg::OnPaint()
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
HCURSOR CDownLoadActiveSyncDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDownLoadActiveSyncDlg::OnDestroy()
{
    CDialog::OnDestroy();
    CeRapiUninit();
}


void CDownLoadActiveSyncDlg::OnBnClickedButtonDown()
{
    //������ؿؼ�
    m_btnDownLoad.EnableWindow(FALSE);
    m_btnCancel.EnableWindow(FALSE);
    m_ctrlServerIP.EnableWindow(FALSE);
    m_ctrlIp1.EnableWindow(FALSE);
    m_ctrlIp2.EnableWindow(FALSE);

    //��ȡ�ͻ����õ�IP��ַ
    BYTE b1,b2,b3,b4;
    m_ctrlServerIP.GetAddress(b1,b2,b3,b4);
    m_strServerIP.Format(_T("%d.%d.%d.%d"),b1,b2,b3,b4);
    m_ctrlIp1.GetAddress(b1,b2,b3,b4);
    m_strIp1.Format(_T("%d.%d.%d.%d"),b1,b2,b3,b4);
    m_ctrlIp2.GetAddress(b1,b2,b3,b4);
    m_strIp2.Format(_T("%d.%d.%d.%d"),b1,b2,b3,b4);

    //���������߳�
    HANDLE h=(HANDLE)_beginthreadex(NULL,0,DownLoadProc,this,0,NULL);
    CloseHandle(h);
}

BOOL CECopyFile(LPCWSTR strSrcFilePath,LPCWSTR strDstFilePath)
{
    CFile file;
    if (!file.Open(strSrcFilePath,CFile::modeRead|CFile::typeBinary)  )
    {
        return FALSE;
    }

    //���豸�ϴ����ļ�
    HANDLE hFileCE = CeCreateFile(strDstFilePath, 
        GENERIC_WRITE, 
        0, 
        NULL, 
        CREATE_ALWAYS, 
        FILE_ATTRIBUTE_NORMAL, 
        NULL);   
    if (hFileCE== INVALID_HANDLE_VALUE) 
    {
        return FALSE;
    }

    BYTE        buf[4096];
    DWORD       lenFileRead=0;
    ULONGLONG   LenFile=0;
    DWORD       dwReturned=0;
    BOOL        bRet;

    LenFile=file.GetLength();

    lenFileRead=file.Read(buf,4096);
    while(lenFileRead >0)
    {
        bRet = CeWriteFile(hFileCE, 
            buf, 
            lenFileRead, 
            &dwReturned, 
            NULL);
        if (!bRet)
        {
            goto CopyFailed;
        }
        lenFileRead=file.Read(buf,4096);        
    }

    CeCloseHandle(hFileCE);
    return TRUE;
CopyFailed:
    CeCloseHandle(hFileCE);
    return FALSE;
}


unsigned int CDownLoadActiveSyncDlg::DownLoadProc(void *pVoid)
{
    CDownLoadActiveSyncDlg *pDlg=(CDownLoadActiveSyncDlg *)pVoid;
    stack<CString>      stackCopyDirectory;
    CString             strDevCpyBaseDir= pDlg->m_strDevBaseDir;
    CString             strlocalCopyBaseDir=pDlg->m_strExePath+_T("CopyFiles\\");

    //���÷�������IP
    CString str;
    pDlg->SendMessage(UM_SHOW_STATUS,0,(LPARAM)L"�����޸������ļ�");
    str=pDlg->m_strExePath;
    str+=L"CopyFiles\\config.ini";
    WritePrivateProfileString(L"global",L"ServerIP",pDlg->m_strServerIP,str);
    if (pDlg->m_iClientIpNum >0)
    {
        //�޸Ŀͻ��˵Ĳ���IP
        pDlg->SendMessage(UM_SHOW_STATUS,5,(LPARAM)L"�������ÿͻ���IP");
        pDlg->ConfigNet();
    }


    
    //�����ȶ��Բ��Կͻ��������ļ��������豸��
    CString             strOppositeCpyDir=_T("");           //���Ƶ����·��,����ڱ��غ��豸
                                                            //���Ƶĸ�·��
    int                 iFileCount=GetDirFileCountAll(strlocalCopyBaseDir);  //Ҫ���Ƶ��ļ�����
    int                 iCount=0;                                            //�Ѹ��Ƶ��ļ�����

    stackCopyDirectory.push(strOppositeCpyDir);
    while( !stackCopyDirectory.empty() )
    {
        strOppositeCpyDir=stackCopyDirectory.top();
        stackCopyDirectory.pop();
        CString     strLocalCpyDir=strlocalCopyBaseDir+strOppositeCpyDir;
        CString     strDevCpyDir=strDevCpyBaseDir+strOppositeCpyDir;

        //���豸�ϴ���Ҫ���Ƶ�Ŀ¼
        CeCreateDirectory(strDevCpyDir,NULL);
        CString     strFind=strLocalCpyDir +_T("*.*");
        //����Ҫ���Ƶ�Ŀ¼���ļ����ҽ��临�Ƶ��豸��
        HANDLE      hFind=INVALID_HANDLE_VALUE;
        WIN32_FIND_DATA wfd;
        BOOL        bFind=FALSE;
        hFind=FindFirstFile(strFind,&wfd);
        if (hFind !=INVALID_HANDLE_VALUE)
        {
            do 
            {
                if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY  )
                {
                    if (wfd.cFileName[0]!=L'.' )
                    {
                        CString     str;
                        str=wfd.cFileName;
                        str+=_T("\\");
                        str=strOppositeCpyDir+str;
                        stackCopyDirectory.push(str);
                    }
                }
                else
                {
                    CString strShow;
                    strShow=strOppositeCpyDir+wfd.cFileName;
                    strShow =L"���ڸ���"+strShow;

                    pDlg->SendMessage(UM_SHOW_STATUS,5+85*iCount/iFileCount,
                        (LPARAM)(LPCTSTR)strShow);
                    
                    CString     strSrcFile,strDstFile;
                    strSrcFile  =strLocalCpyDir+wfd.cFileName;
                    strDstFile  =strDevCpyDir+wfd.cFileName;
                    CECopyFile(strSrcFile,strDstFile);
                    iCount++; 
                }

                bFind=FindNextFile(hFind,&wfd);
            } while (bFind);

            FindClose(hFind);
            hFind=INVALID_HANDLE_VALUE;
        }        
    }

    //����start.bat�ļ������ذ���
    CString     strSrcFile=pDlg->m_strExePath+_T("Fixed\\start.bat");
    CString     strDstFile=pDlg->m_strStartFolder+_T("start.bat");
    pDlg->SendMessage(UM_SHOW_STATUS,90,
        (LPARAM)(LPCTSTR)L"���ڸ���start.bat");
    CECopyFile(strSrcFile,strDstFile);

    pDlg->SendMessage(UM_SHOW_STATUS,100,(LPARAM)L"�������");
    pDlg->MessageBox(L"���ز�����������ذ��ϳɹ��������豸����Ч",
        L"��ʾ",MB_OK|MB_ICONINFORMATION);
    pDlg->PostMessage(WM_CLOSE);
    return  0;
}
void CDownLoadActiveSyncDlg::OnBnClickedButtonRemove()
{
    GetDlgItem(IDC_BUTTON_DOWN)->EnableWindow(FALSE);
    //ɾ��start.bat
    SendMessage(UM_SHOW_STATUS,0,(LPARAM)L"����ɾ��start.bat" );
    
    CString strDelStart=m_strStartFolder+L"start.bat";
    CeDeleteFile(strDelStart);

    //ɾ�����Գ���CE�˵ĳ���
    CString     strExeDir=m_strDevBaseDir;
    int iDelFileCount=CeGetDirFileCountAll(strExeDir);
    int iCount=0;
    DelLoop(strExeDir,iCount,iDelFileCount);
    CeRemoveDirectory(strExeDir);
    SendMessage(UM_SHOW_STATUS,100,(LPARAM)L"ɾ�����" );
    GetDlgItem(IDC_BUTTON_DOWN)->EnableWindow(TRUE);
}


BOOL CDownLoadActiveSyncDlg::ConfigNet()
{
    //��ȡ��̫��1��IP��ַ
    HKEY hSubKey = NULL;
    DWORD dwResult = 0;
    DWORD dwType = REG_SZ;
    WCHAR szValue[256] = {0};
    DWORD dwValueLen = sizeof(szValue);
    if (! m_strIp1SubKey.IsEmpty())
    {
        wcscpy(szValue,m_strIp1);
        dwValueLen=sizeof(WCHAR)*(m_strIp1.GetLength() +1);
        dwResult = ::CeRegOpenKeyEx(HKEY_LOCAL_MACHINE, m_strIp1SubKey, 0, KEY_READ|KEY_WRITE, &hSubKey);
        if (dwResult != ERROR_SUCCESS)
        {          
            goto ETH1_FINISH;
        }
        dwResult = ::CeRegSetValueEx(hSubKey, L"IpAddress", 0, dwType, (BYTE *)szValue, dwValueLen);
                
        CeRegCloseKey(hSubKey);
    }
ETH1_FINISH:
    hSubKey = NULL;
    dwResult = 0;
    dwType = REG_SZ;
    szValue[0] = 0;
    
    if (! m_strIp2SubKey.IsEmpty())
    {
        wcscpy(szValue,m_strIp2);
        dwValueLen=sizeof(WCHAR)*(m_strIp1.GetLength() +1);
        dwResult = ::CeRegOpenKeyEx(HKEY_LOCAL_MACHINE, m_strIp2SubKey, 0, KEY_READ|KEY_WRITE, &hSubKey);
        if (dwResult != ERROR_SUCCESS)
        {          
            goto ETH2_FINISH;
        }
        dwResult = ::CeRegSetValueEx(hSubKey, L"IpAddress", 0, dwType,
            (BYTE *)szValue, dwValueLen);
        if (dwResult != ERROR_SUCCESS)
        {                
        }        
        CeRegCloseKey(hSubKey);
    }
ETH2_FINISH:

    return FALSE;
}

void  CDownLoadActiveSyncDlg::DelLoop(const WCHAR *strDir,int &count,const int iTotalCount)
{
    HANDLE              hFind;
    CE_FIND_DATA        cfd;
    BOOL                bFind=FALSE;
    CString strFind=strDir;
    strFind+=L"*.*";

    hFind=CeFindFirstFile(strFind,&cfd);

    if (hFind !=INVALID_HANDLE_VALUE)
    {
        do 
        {
            if (cfd.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY)
            {
                if (cfd.cFileName[0]!=L'.')
                {
                    CString str=strDir;
                    str+=cfd.cFileName;
                    str+=L"\\";
                    DelLoop(str,count,iTotalCount);
                }
            }
            else
            {
                CString str;
                str+=strDir;
                str+=cfd.cFileName;
                CString strDisp=_T("����ɾ��")+str;
                SendMessage(UM_SHOW_STATUS,5+95*count/iTotalCount,(LPARAM)(LPCTSTR)strDisp );  
                CeDeleteFile(str);                              
                count++;
            }

            bFind=CeFindNextFile(hFind,&cfd);
        } while (bFind);

        CeFindClose(hFind);
    }

    CeRemoveDirectory(strDir);
}