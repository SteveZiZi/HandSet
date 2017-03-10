// DT6000_FiberTestToolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DT6000_FiberTestTool.h"
#include "DT6000_FiberTestToolDlg.h"

#include "winioctl.h"
//#include "pkfuncs.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CDT6000_FiberTestToolDlg 对话框

#define FT3_NANRUI_PATH  L"pcap\\NanRui.txt"
#define FT3_GUOWANG_PATH     L"pcap\\GuoWang.txt"
#define SMV_9_2_PATH     L"pcap\\sv92_test.pcap"
//#define SMV_9_2_PATH     L".\\FlashDisk2\\1.pcap"
#define GOOSE_PATH     L"pcap\\goose.pcap"

CDT6000_FiberTestToolDlg::CDT6000_FiberTestToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDT6000_FiberTestToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_hDevice = INVALID_HANDLE_VALUE;
    m_pFt3Tx = NULL;
    m_pFt3Rx1 = NULL;
    m_pFt3Rx2 = NULL;
    for(int i = 0; i < DEVICE_FIBER_NUM; i++)
    {
        m_pFiber[i] = NULL;
    }
    m_bSendBcode = FALSE;
}

void CDT6000_FiberTestToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDT6000_FiberTestToolDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_BUTTON_START_SEV, &CDT6000_FiberTestToolDlg::OnBnClickedButtonStartSev)
    ON_BN_CLICKED(IDC_BUTTON_STOP_SEV, &CDT6000_FiberTestToolDlg::OnBnClickedButtonStopSev)
    ON_MESSAGE(WM_CHANGE_TX, &CDT6000_FiberTestToolDlg::OnChangeTx)
    ON_MESSAGE(WM_RECOVER_TX, &CDT6000_FiberTestToolDlg::OnRecoverTx)
    ON_MESSAGE(WM_STOP_TX, &CDT6000_FiberTestToolDlg::OnStopTx)
    ON_MESSAGE(WM_SEND_BCODE, &CDT6000_FiberTestToolDlg::OnSendBcode)
END_MESSAGE_MAP()


// CDT6000_FiberTestToolDlg 消息处理程序

BOOL CDT6000_FiberTestToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

    DeviceInit();
	DataInit();
  //  Ft3Tx();
    OnBnClickedButtonStartSev();
    
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CDT6000_FiberTestToolDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	if (AfxIsDRAEnabled())
	{
		DRA::RelayoutDialog(
			AfxGetResourceHandle(), 
			this->m_hWnd, 
			DRA::GetDisplayMode() != DRA::Portrait ? 
			MAKEINTRESOURCE(IDD_DT6000_FiberTestTool_DIALOG_WIDE) : 
			MAKEINTRESOURCE(IDD_DT6000_FiberTestTool_DIALOG));
	}
}
#endif


///
/// @brief
///     数据的初始化
///
/// @return
///
///
void CDT6000_FiberTestToolDlg::DataInit()
{
    UINT32 uMuTxFrameGroup = 1;
    UINT32 uMuTxAsduNum = 1;

    m_tFiberParam.uDeviceItem = 0;

    memset(m_tFiberParam.uInBuf, 0, sizeof(m_tFiberParam.uInBuf));

    //0:Smv 1:goose
    m_tFiberParam.uInBuf[0] = 0;

    m_tFiberParam.uInBuf[1] = uMuTxFrameGroup|(uMuTxAsduNum << 16);
    m_tFiberParam.uInBuf[2] = 4000; 

    //采样率
    m_tFiberParam.uInBuf[3] = 1;   
    m_tFiberParam.uInBuf[5] = 39;

    m_tCtrlW.wData = 0;
    m_tCtrlW.CtrlBit.bBaudRate = 1;
    m_tCtrlW.CtrlBit.bFrameStyle = _GUO_WANG;
    m_tCtrlW.CtrlBit.bSampleRate = 1;

    //获取当前应用程序的目录
    int iDirLen =GetExeDirectory(m_strExtFilePath,MAX_PATH*2);
    
}


BOOL CDT6000_FiberTestToolDlg::DeviceInit()
{
    if (m_hDevice == INVALID_HANDLE_VALUE) {   
        //打开GPM驱动                                          
        m_hDevice = CreateFile(L"GPM1:", GENERIC_READ|GENERIC_WRITE, 0, 0,OPEN_ALWAYS, 0, NULL);
        if (m_hDevice == INVALID_HANDLE_VALUE) {
            MessageBox(L"Open GPM Device failed");
            return FALSE;
        } else {
            RETAILMSG(ZONE_ERROR, (_T("Open GPM Device\r\n")));
        }
    }

#if ZONE_FT3_TX
    m_pFt3Tx = new _FT3_c(m_hDevice, DEVICE_FT3_TX);
    if (m_pFt3Tx->Init( L"DSM_FT3_TX1", L"ShareFT3_TX1", FT3_TX_MAP_FILE_LEN) == FALSE) {
        RETAILMSG(1, (_T("APP: m_Ft3Tx.Init failed\r\n")));
        return FALSE;
    }
#endif
#if ZONE_FT3_RX1
    m_pFt3Rx1 = new _FT3_c(m_hDevice, DEVICE_FT3_RX1);
    if (m_pFt3Rx1->Init( L"DSM_FT3_RX1", L"ShareFT3_RX1", FT3_MAP_FILE_LEN, ft3ThreadProcRx1) == FALSE) {
        RETAILMSG(ZONE_ERROR, (_T("APP: m_Ft3Rx1.Init failed\r\n")));
        return FALSE;
    }
#endif
#if ZONE_FT3_RX2
    m_pFt3Rx2 = new _FT3_c(m_hDevice, DEVICE_FT3_RX2);
    if (m_pFt3Rx2->Init( L"DSM_FT3_RX2", L"ShareFT3_RX2", FT3_MAP_FILE_LEN, ft3ThreadProcRx2) == FALSE) {
        RETAILMSG(ZONE_ERROR, (_T("APP: m_Ft3Rx2.Init failed\r\n")));
        return FALSE;
    }
#endif
#if ZONE_FIBER_A
    m_pFiber[0] = new _SMV_92_c(m_hDevice, DEVICE_FIBER_A);
    if (m_pFiber[0]->Init(L"DSM_FIBER_A", L"ShareFIBER_A", FIBER_MAP_FILE_LEN, FiberThreadProcRx1) == FALSE) {
        RETAILMSG(ZONE_ERROR, (_T("APP: m_pFiberA.Init failed\r\n")));
        return FALSE;
    }

#endif

#if ZONE_FIBER_B
    m_pFiber[1] = new _SMV_92_c(m_hDevice, DEVICE_FIBER_B);
    if (m_pFiber[1]->Init(L"DSM_FIBER_B", L"ShareFIBER_B", FIBER_MAP_FILE_LEN, FiberThreadProcRx2) == FALSE) {
        RETAILMSG(ZONE_ERROR, (_T("APP: m_pFiberB.Init failed\r\n")));
        return FALSE;
    }
#endif
#if ZONE_FIBER_C
    m_pFiber[2] = new _SMV_92_c(m_hDevice, DEVICE_FIBER_C);
    if (m_pFiber[2]->Init(L"DSM_FIBER_C", L"ShareFIBER_C", FIBER_MAP_FILE_LEN, FiberThreadProcRx3) == FALSE) {
        RETAILMSG(ZONE_ERROR, (_T("APP: m_pFiberC.Init failed\r\n")));
        return FALSE;
    }
#endif

    BOOL bFiberWork = TRUE; 
    if (FALSE == DeviceIoControl(m_hDevice, IOCTRL_START_FPGA, &bFiberWork, sizeof(bFiberWork), NULL,NULL,NULL,NULL) ) {
        MessageBox(L"DeviceIoControl:IOCTRL_START_FPGA failed");
        return FALSE;
    }


    return TRUE;
}


///
/// @brief
///     资源的释放
///
/// @return
///
///
void CDT6000_FiberTestToolDlg::DeviceDeinit()
{
    BOOL bFiberWork = FALSE; 
    if (m_hDevice != INVALID_HANDLE_VALUE) {
        if (FALSE == DeviceIoControl(m_hDevice, IOCTRL_START_FPGA, &bFiberWork, sizeof(bFiberWork), NULL,NULL,NULL,NULL) ) {
            MessageBox(L"DeviceIoControl:IOCTRL_START_FPGA failed");
            return;
        }
    }

#if ZONE_FT3_TX
    if(m_pFt3Tx != NULL) {
        m_pFt3Tx->DeInit();
        delete m_pFt3Tx;
        m_pFt3Tx = NULL;
    }
#endif
#if ZONE_FT3_RX1
    if(m_pFt3Rx1 != NULL) {
        m_pFt3Rx1->DeInit();
        delete m_pFt3Rx1;
    }
#endif
#if ZONE_FT3_RX2
    if(m_pFt3Rx2 != NULL) {
        m_pFt3Rx2->DeInit();
        delete m_pFt3Rx2;
    }
#endif
#if ZONE_FIBER_A
    if (m_pFiber[0] != NULL) {
        m_pFiber[0]->DeInit();
        delete m_pFiber[0];
    }
#endif
#if ZONE_FIBER_B
    if (m_pFiber[1] != NULL) {
        m_pFiber[1]->DeInit();
        delete m_pFiber[1];
    }
#endif
#if ZONE_FIBER_C
    if (m_pFiber[2] != NULL) {
        m_pFiber[2]->DeInit();
        delete m_pFiber[2];
    }
#endif
    if (m_hDevice != INVALID_HANDLE_VALUE) {
        CloseHandle(m_hDevice);
        m_hDevice = INVALID_HANDLE_VALUE;
    }
}

void CDT6000_FiberTestToolDlg::OnDestroy()
{
    DeviceDeinit();
    CDialog::OnDestroy();
    
}



void CDT6000_FiberTestToolDlg::OnBnClickedButtonStartSev()
{
    // 发送SMV
    for(int i = 0; i < DEVICE_FIBER_NUM; i++)
    {
        m_tFiberParam.uDeviceItem =  i;
        m_pFiber[i]->SetFilePath((CString)(m_strExtFilePath)+ SMV_9_2_PATH);
        m_pFiber[i]->SetTxParam(m_tFiberParam);
        m_pFiber[i]->MuTx();
        m_pFiber[i]->StartMuDetect();
    }


    m_pFt3Tx->SetFilePath((CString)m_strExtFilePath + FT3_GUOWANG_PATH);
    m_pFt3Tx->SetFT3TxInfo(m_tCtrlW);
    m_pFt3Tx->SetFt3TxFrame();

    //发送FT3报文
    m_pFt3Tx->Ft3Tx();

    //接收FT3报文
    m_pFt3Rx1->Ft3Rx();
    m_pFt3Rx2->Ft3Rx();


    GetDlgItem(IDC_BUTTON_STOP_SEV)->EnableWindow(TRUE);
    GetDlgItem(IDC_BUTTON_START_SEV)->EnableWindow(FALSE);

    GetDlgItem(IDC_STATIC_TEST_INFO)->SetWindowText(L"测试中...");
    UpdateData(FALSE);
}


void CDT6000_FiberTestToolDlg::OnBnClickedButtonStopSev()
{
    //停止发送和探测SMV报文
    for(int i = 0; i < DEVICE_FIBER_NUM; i++)
    {
        m_pFiber[i]->MuStop();
        m_pFiber[i]->StopMuDetect();
    }

    //停止发送和探测FT3报文
    m_pFt3Tx->Ft3StopTx();
    m_pFt3Rx1->Ft3RxStop();
    m_pFt3Rx2->Ft3RxStop();

    GetDlgItem(IDC_BUTTON_STOP_SEV)->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTON_START_SEV)->EnableWindow(TRUE);

    GetDlgItem(IDC_STATIC_TEST_INFO)->SetWindowText(L"测试已停止...");
    UpdateData(FALSE);


}

///
/// @brief
///     改变当前发送的报文格式
/// @param[WPARAM wParam]
///     0表示FT3,1表示SMV
/// @param[LPARAM lParam]
///     表示第几个口
/// @return
///     
///
LRESULT CDT6000_FiberTestToolDlg::OnChangeTx( WPARAM wParam, LPARAM lParam )
{
    if(wParam == 0)
    {
        //获取报文格式
        if(lParam == 0)
        {
            m_pFt3Rx1->GetFT3TxInfo(m_tCtrlW);
        }
        else
        {
            m_pFt3Rx2->GetFT3TxInfo(m_tCtrlW);     
        }

        //发送接收到的FT3报文
        m_pFt3Tx->Ft3Tx();
        GetDlgItem(IDC_STATIC_TEST_INFO)->SetWindowText(L"发送接收到的FT3报文...");
        UpdateData(FALSE);
    }
    else
    {
        m_tFiberParam.uDeviceItem = lParam;
        
        //发送GOOSE报文
        m_tFiberParam.uInBuf[0] = 1;

        //T0和T1
        m_tFiberParam.uInBuf[1] = 2;
        m_tFiberParam.uInBuf[2] = 2;
       
        m_pFiber[lParam]->SetFilePath((CString)m_strExtFilePath + GOOSE_PATH);
        m_pFiber[lParam]->SetTxParam(m_tFiberParam);
        m_pFiber[lParam]->MuTx();

        GetDlgItem(IDC_STATIC_TEST_INFO)->SetWindowText(L"发送GOOSE报文...");
        UpdateData(FALSE);
      
    }
    return 0;
}

///
/// @brief
///     恢复之前的报文发送
/// @param[WPARAM wParam]
///     0表示FT3,1表示SMV
/// @param[LPARAM lParam]
///
/// @return
///
///
LRESULT CDT6000_FiberTestToolDlg::OnRecoverTx( WPARAM wParam, LPARAM lParam )
{
    if(wParam == 0)
    {
        //停止当前的FT3报文发送
        m_pFt3Tx->Ft3StopTx();
        m_pFt3Tx->SetFilePath((CString)m_strExtFilePath + FT3_GUOWANG_PATH);

        m_tCtrlW.wData = 0;
        m_tCtrlW.CtrlBit.bBaudRate = _4K;
        m_tCtrlW.CtrlBit.bFrameStyle = _GUO_WANG;
        m_tCtrlW.CtrlBit.bSampleRate = 1;

        m_pFt3Tx->SetFT3TxInfo(m_tCtrlW);
        m_pFt3Tx->SetFt3TxFrame();
        m_pFt3Tx->Ft3Tx();

        //停止接收
        m_pFt3Rx1->Ft3RxStop();
        m_pFt3Rx2->Ft3RxStop();

        //重新开始接收
        m_pFt3Rx1->Ft3Rx();
        m_pFt3Rx2->Ft3Rx();
    }
    else
    {
        m_tFiberParam.uDeviceItem = lParam;
        m_tFiberParam.uInBuf[0] = 0;
        m_tFiberParam.uInBuf[1] = 1|(1 << 16);
        //发送的帧数
        m_tFiberParam.uInBuf[2] = 4000;
        //采样率
        m_tFiberParam.uInBuf[3] = 1;    
        m_tFiberParam.uInBuf[5] = 39;

        m_pFiber[lParam]->SetFilePath((CString)m_strExtFilePath + SMV_9_2_PATH);
        m_pFiber[lParam]->SetTxParam(m_tFiberParam);
        m_pFiber[lParam]->MuTx();
        m_pFiber[lParam]->StopMuDetect();
        m_pFiber[lParam]->StartMuDetect();

        if(m_bSendBcode)
        {
            //停止B码发送
            m_pFt3Tx->StopBcode();
            m_bSendBcode = FALSE;
            m_pFt3Tx->Ft3Tx();
        }
    }

    GetDlgItem(IDC_STATIC_TEST_INFO)->SetWindowText(L"已经恢复报文发送...");
    UpdateData(FALSE);
    return 0;
}

///
/// @brief
///     停止发送当前报文
/// @param[WPARAM wParam]
///     0表示FT3,1表示SMV
/// @param[LPARAM lParam]
///
/// @return
///
///
LRESULT CDT6000_FiberTestToolDlg::OnStopTx( WPARAM wParam, LPARAM lParam )
{
    if(0 == wParam)
    { 
        m_pFt3Tx->Ft3StopTx();
        if(0 == lParam)
        {
            //把接收到的报文转为发送报文的格式
            m_pFt3Rx1->SetFt3TxAddress(m_pFt3Tx->m_dataBuf + FT3_TX_LOOP_BUF0_POS);
            SetEvent(m_pFt3Rx1->m_hStopTxEvent);
        }
        else
        {         
            m_pFt3Rx2->SetFt3TxAddress(m_pFt3Tx->m_dataBuf + FT3_TX_LOOP_BUF0_POS);
            SetEvent(m_pFt3Rx2->m_hStopTxEvent);
        }
    }

    GetDlgItem(IDC_STATIC_TEST_INFO)->SetWindowText(L"停止发送当前报文...");
    UpdateData(FALSE);
    return 0;
}

///
/// @brief
///     发送B码
/// @param[WPARAM wParam]
/// @param[LPARAM lParam]
///
/// @return
///
///
LRESULT CDT6000_FiberTestToolDlg::OnSendBcode( WPARAM wParam, LPARAM lParam )
{
    m_bSendBcode = TRUE;
    m_pFt3Tx->Ft3StopTx();

    //设置UTC时间
    m_pFiber[lParam]->SetUtcTime();
    m_pFt3Tx->SendBcode();

    GetDlgItem(IDC_STATIC_TEST_INFO)->SetWindowText(L"发送B码...");
    UpdateData(FALSE);
    return 0;
}

///
/// @brief
///     获取应用程序的当前目录
/// @param[WCHAR * strPath]
/// @param[int len]
///
/// @return
///
///
int CDT6000_FiberTestToolDlg::GetExeDirectory(WCHAR *strPath, int len)
{
    int         i;
    WCHAR       strFilePath[MAX_PATH*2];

    //获取可执行文件所在的路径
    i=GetModuleFileName(NULL,strFilePath,MAX_PATH);
    while(i>=0 && strFilePath[i]!=L'\\' )
    {
        i--;
    }
    if (i<0)
        return FALSE;
    i++;
    strFilePath[i]=0;

    if (len <i+1)
    {
        return 0;
    }

    memcpy(strPath,strFilePath,(i+i)*sizeof(WCHAR));    
    return i;
}