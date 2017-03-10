// DownLoadDlg.cpp : implementation file
//

#include "stdafx.h"
#include <process.h>
#include <WinSock2.h>
#include "UsuallyCommon.h"
#include "rapi.h"
#include "FunctionTest.h"
#include "TSocket.h"
#include "DownLoadDlg.h"
#include <stack>

using namespace std;
#pragma comment(lib,"rapi.lib")


#define UM_SHOW_STATUS              WM_USER +100
#define UM_INIT_FINISHED            WM_USER +101
#define UM_CONFIG_NET_FINISHED      WM_USER +102
#define UM_DOWNLOAD_FINISHED        WM_USER +103

IMPLEMENT_DYNAMIC(CDownLoadDlg, CDialog)

CDownLoadDlg::CDownLoadDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDownLoadDlg::IDD, pParent)
    , m_strStatus(_T(""))
{

}

CDownLoadDlg::~CDownLoadDlg()
{
}

void CDownLoadDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PROGRESS_DOWNLOAD, m_ctrlProcess);
    DDX_Text(pDX, IDC_STATIC_STATUS, m_strStatus);
    DDX_Control(pDX, IDC_IPADDRESS_IP1, m_ctrlIp1);
    DDX_Control(pDX, IDC_IPADDRESS_IP2, m_ctrlIp2);
    DDX_Control(pDX, IDC_BUTTON_DOWNLOAD, m_btnDownLoad);
    DDX_Control(pDX, IDC_BUTTON_CONFIG_NET, m_btnConfigNet);
}


BEGIN_MESSAGE_MAP(CDownLoadDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_DOWNLOAD, &CDownLoadDlg::OnBnClickedButtonDownload)
    ON_MESSAGE(UM_SHOW_STATUS,OnShowStatus)
    ON_MESSAGE(UM_INIT_FINISHED,OnInitFinished)
    ON_MESSAGE(UM_CONFIG_NET_FINISHED,OnConfigNetFinished)
    ON_MESSAGE(UM_DOWNLOAD_FINISHED,OnDownloadFinished)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_BUTTON_CONFIG_NET, &CDownLoadDlg::OnBnClickedButtonConfigNet)
END_MESSAGE_MAP()


// CDownLoadDlg message handlers
BOOL CDownLoadDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    CMenu *pSysMenu=GetSystemMenu(FALSE);
    pSysMenu->EnableMenuItem( SC_CLOSE, MF_BYCOMMAND|MF_GRAYED);
    m_strStatus=_T("正在读取配置信息...");
    UpdateData(FALSE);
    m_ctrlIp1.SetAddress(192,168,0,2);
    m_ctrlIp2.SetAddress(192,168,0,3);
    m_ctrlIp1.EnableWindow(FALSE);
    m_ctrlIp2.EnableWindow(FALSE);

    //使按钮无效
    m_btnConfigNet.EnableWindow(FALSE);
    m_btnDownLoad.EnableWindow(FALSE);


    //设置显示进度条
    m_ctrlProcess.SetRange(0,100);
    m_ctrlProcess.SetPos(0);

    //获取当前可执行文件所在的目录
    WCHAR strPath[MAX_PATH *2];
    strPath[0]=0;
    int len=GetExeDirectory(strPath,MAX_PATH*2);
    m_strExePath=strPath;

    //创建初始化线程
    HANDLE h=(HANDLE)_beginthreadex(NULL,0,InitProc,this,0,NULL);
    CloseHandle(h);
    return TRUE;  
  
}

unsigned int CDownLoadDlg::InitProc(void *pVoid)
{
    CDownLoadDlg *pDlg=(CDownLoadDlg *)pVoid;
    Sleep(100);
    CString     strIniFilePath=pDlg->m_strExePath;
    WCHAR       t[128];
    strIniFilePath+=L"FunctionTest.ini";

    int num=GetPrivateProfileInt(L"download",L"EthNum",1,strIniFilePath);
    pDlg->m_iClientIpNum=num;
    if (num>0)
    {
        GetPrivateProfileString(L"download",L"RegIP1",L"Comm\\AX88796B1\\Parms\\TcpIp",
            t,128,strIniFilePath);
        pDlg->m_strIp1SubKey=t;
    }

    if (num>1)
    {
        GetPrivateProfileString(L"download",L"RegIP2",L"Comm\\AX88796B_21\\Parms\\TcpIp",
            t,128,strIniFilePath);
        pDlg->m_strIp2SubKey=t;
    }


    //初始化RAPI
    RAPIINIT ri;
    ri.cbSize = sizeof(ri);
    CeRapiInitEx(&ri);
    DWORD ret = WaitForSingleObject(ri.heRapiInit, 1000);
    if((ret != S_OK) || (ri.hrRapiInit != S_OK)) {
        pDlg->MessageBox(_T("RAPI函数初始化失败，请检查ActiveSync是否已连接"),
            _T("错误"),MB_OK|MB_ICONERROR);
        pDlg->PostMessage(WM_CLOSE);
        return 0;
    }

    //读取以太网1的IP地址
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
        //读取以太网1IP地址出错
        pDlg->m_strIp1.Empty();
        pDlg->m_strIp1SubKey.Empty();
        pDlg->SendMessage(UM_SHOW_STATUS,0,(LPARAM)L"读取以太网1IP地址出错" );
    }

    //读取以太网2的IP地址
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
        //读取以太网2IP地址出错
        pDlg->m_strIp2.Empty();
        pDlg->m_strIp2SubKey.Empty();
        pDlg->SendMessage(UM_SHOW_STATUS,0,(LPARAM)L"读取以太网2IP地址出错" );
    }
    //读取配置信息完成
    pDlg->SendMessage(UM_INIT_FINISHED,0,0);

    return 0;
}
BOOL CDownLoadDlg::PreTranslateMessage(MSG* pMsg)
{
    if(pMsg->message == WM_KEYDOWN)
    {
        switch(pMsg->wParam) 
        {
        case VK_RETURN: //回车
            return TRUE;
        case VK_ESCAPE: //ESC
            return TRUE;
        }
    }
    return CDialog::PreTranslateMessage(pMsg);
}

HRESULT CDownLoadDlg::OnShowStatus(WPARAM wParam,LPARAM lParam)
{
    m_strStatus=(LPCWSTR)lParam;
    m_ctrlProcess.SetPos((int)wParam);
    UpdateData(FALSE);

    return 0;
}

HRESULT CDownLoadDlg::OnInitFinished(WPARAM wParam,LPARAM lParam)
{
    BYTE b[4];
    UINT32 uiIP;
    //根据读取的设备的以太网的IP地址来显示
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


    m_strStatus=L"读取配置信息完成";
    UpdateData(FALSE);
    //使按钮无效
    m_btnConfigNet.EnableWindow(TRUE);
    m_btnDownLoad.EnableWindow(TRUE);
    CMenu *pSysMenu=GetSystemMenu(FALSE);
    pSysMenu->EnableMenuItem( SC_CLOSE, MF_BYCOMMAND|MF_ENABLED);

    return 0;
}
void CDownLoadDlg::OnDestroy()
{
    CDialog::OnDestroy();
    CeRapiUninit();
}

void CDownLoadDlg::OnBnClickedButtonConfigNet()
{
    UpdateData(TRUE);
    m_strStatus=_T("正在配置网络");
    m_btnConfigNet.EnableWindow(FALSE);
    m_btnDownLoad.EnableWindow(FALSE);
    CMenu *pSysMenu=GetSystemMenu(FALSE);
    pSysMenu->EnableMenuItem( SC_CLOSE, MF_BYCOMMAND|MF_GRAYED);
    UpdateData(FALSE);

    BYTE b1,b2,b3,b4;
    m_ctrlIp1.GetAddress(b1,b2,b3,b4);
    m_strIp1.Format(_T("%d.%d.%d.%d"),b1,b2,b3,b4 );
    m_ctrlIp2.GetAddress(b1,b2,b3,b4);
    m_strIp2.Format(_T("%d.%d.%d.%d"),b1,b2,b3,b4 );

    HANDLE h=(HANDLE)_beginthreadex(NULL,0,ConfigNetProc,this,0,NULL);
    CloseHandle(h);
}

unsigned int CDownLoadDlg::ConfigNetProc(void *pVoid)
{
    CDownLoadDlg *pDlg=(CDownLoadDlg *)pVoid;

    //读取以太网1的IP地址
    HKEY hSubKey = NULL;
    DWORD dwResult = 0;
    DWORD dwType = REG_SZ;
    WCHAR szValue[256] = {0};
    DWORD dwValueLen = sizeof(szValue);
    if (! pDlg->m_strIp1SubKey.IsEmpty())
    {
        wcscpy(szValue,pDlg->m_strIp1);
        dwValueLen=sizeof(WCHAR)*(pDlg->m_strIp1.GetLength() +1);
        dwResult = ::CeRegOpenKeyEx(HKEY_LOCAL_MACHINE, pDlg->m_strIp1SubKey, 0, KEY_READ|KEY_WRITE, &hSubKey);
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

    if (! pDlg->m_strIp2SubKey.IsEmpty())
    {
        wcscpy(szValue,pDlg->m_strIp2);
        dwValueLen=sizeof(WCHAR)*(pDlg->m_strIp2.GetLength() +1);
        dwResult = ::CeRegOpenKeyEx(HKEY_LOCAL_MACHINE, pDlg->m_strIp2SubKey, 0, KEY_READ|KEY_WRITE, &hSubKey);
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
    
    pDlg->SendMessage(UM_CONFIG_NET_FINISHED,0,0);

    return 0;
}

HRESULT CDownLoadDlg::OnConfigNetFinished(WPARAM wParam,LPARAM lParam)
{
    MessageBox(_T("配置完成，但要重启设备以后才会生效"),_T("配置完成"),MB_OK|MB_ICONERROR );
    m_strStatus=_T("配置网络完成");
    m_btnConfigNet.EnableWindow(TRUE);
    m_btnDownLoad.EnableWindow(TRUE);

    CMenu *pSysMenu=GetSystemMenu(FALSE);
    pSysMenu->EnableMenuItem( SC_CLOSE, MF_BYCOMMAND|MF_ENABLED);
    
    return 0;
}

void CDownLoadDlg::OnBnClickedButtonDownload()
{
    m_strStatus=_T("正在复制...");
    m_btnConfigNet.EnableWindow(FALSE);
    m_btnDownLoad.EnableWindow(FALSE);
    CMenu *pSysMenu=GetSystemMenu(FALSE);
    pSysMenu->EnableMenuItem( SC_CLOSE, MF_BYCOMMAND|MF_GRAYED);
    UpdateData(FALSE);

    //创建下载线程   
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

    //在设备上创建文件
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

unsigned int CDownLoadDlg::DownLoadProc(void *pVoid)
{
    CDownLoadDlg *pDlg=(CDownLoadDlg *)pVoid;

    stack<CString>      stackCopyDirectory;
    CString             strDevCpyBaseDir= L"\\";
    CString             strlocalCopyBaseDir=pDlg->m_strExePath;

     //复制稳定性测试客户端所有文件到测试设备上
    CString             strOppositeCpyDir=_T("");           //复制的相对路径,相对于本地和设备
    //复制的根路径
    int                 iFileCount=GetDirFileCountAll(strlocalCopyBaseDir+_T("CEDll\\"))      //要复制的文件总数
                                +GetDirFileCountAll(strlocalCopyBaseDir+_T("CopyFiles\\"))
                                +GetDirFileCountAll(strlocalCopyBaseDir+_T("config\\"));  
    int                 iCount=0;                                              //已复制的文件计数

    //将配置文件夹复制到到"\"目录下    
    stackCopyDirectory.push(_T("config\\"));
    while( !stackCopyDirectory.empty() )
    {
        strOppositeCpyDir=stackCopyDirectory.top();
        stackCopyDirectory.pop();
        CString     strLocalCpyDir=strlocalCopyBaseDir+strOppositeCpyDir;
        CString     strDevCpyDir=strDevCpyBaseDir+strOppositeCpyDir;

        //在设备上创建要复制的目录
        CeCreateDirectory(strDevCpyDir,NULL);
        CString     strFind=strLocalCpyDir +_T("*.*");
        //遍历要复制的目录的文件并且将其复制的设备上
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
                    strShow =L"正在复制"+strShow;

                    pDlg->SendMessage(UM_SHOW_STATUS,90*iCount/iFileCount,
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

    //拷贝CopyFiles文件夹下的文件到"\"目录下
    stackCopyDirectory.push(_T(""));
    strDevCpyBaseDir= L"\\";
    strlocalCopyBaseDir=pDlg->m_strExePath+_T("CopyFiles\\");
    while( !stackCopyDirectory.empty() )
    {
        strOppositeCpyDir=stackCopyDirectory.top();
        stackCopyDirectory.pop();
        CString     strLocalCpyDir=strlocalCopyBaseDir+strOppositeCpyDir;
        CString     strDevCpyDir=strDevCpyBaseDir+strOppositeCpyDir;

        //在设备上创建要复制的目录
        CeCreateDirectory(strDevCpyDir,NULL);
        CString     strFind=strLocalCpyDir +_T("*.*");
        //遍历要复制的目录的文件并且将其复制的设备上
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
                    strShow =L"正在复制"+strShow;

                    pDlg->SendMessage(UM_SHOW_STATUS,90*iCount/iFileCount,
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


    //拷贝所有动态库文件到"windows"目录下
    stackCopyDirectory.push(_T(""));
    strDevCpyBaseDir= L"\\windows\\";
    strlocalCopyBaseDir=pDlg->m_strExePath+_T("CEDll\\");
    while( !stackCopyDirectory.empty() )
    {
        strOppositeCpyDir=stackCopyDirectory.top();
        stackCopyDirectory.pop();
        CString     strLocalCpyDir=strlocalCopyBaseDir+strOppositeCpyDir;
        CString     strDevCpyDir=strDevCpyBaseDir+strOppositeCpyDir;

        //在设备上创建要复制的目录
        CeCreateDirectory(strDevCpyDir,NULL);
        CString     strFind=strLocalCpyDir +_T("*.*");
        //遍历要复制的目录的文件并且将其复制的设备上
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
                    strShow =L"正在复制"+strShow;

                    pDlg->SendMessage(UM_SHOW_STATUS,90*iCount/iFileCount,
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

    pDlg->SendMessage(UM_SHOW_STATUS,95,(LPARAM)L"启动测试程序WinCe端");
    BOOL    bRet;


    PROCESS_INFORMATION pi;    
    
    WCHAR strCmd[]=_T("\\FunctionTestCE.exe");
    bRet=CeCreateProcess(strCmd,strCmd,NULL,
        NULL,FALSE,CREATE_NEW_CONSOLE,NULL,NULL,NULL,&pi );    

    pDlg->SendMessage(UM_SHOW_STATUS,100,(LPARAM)L"下载完成");
    pDlg->SendMessage(UM_DOWNLOAD_FINISHED,0,0);
    
    return 0;
}

HRESULT CDownLoadDlg::OnDownloadFinished(WPARAM wParam,LPARAM lParam)
{
    Sleep(100);
    OnOK();
    return 0;
}