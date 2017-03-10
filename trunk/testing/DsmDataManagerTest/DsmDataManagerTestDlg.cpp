// DsmDataManagerTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DsmDataManagerTest.h"
#include "DsmDataManagerTestDlg.h"
#include "ParamConfigBasicDlg.h"
#include "SmvRecvConfigPubDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const std::wstring SCL_PAHT = L"\\FlashDisk\\GuoWangMUTest.SCD";


// CDsmDataManagerTestDlg 对话框

CDsmDataManagerTestDlg::CDsmDataManagerTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDsmDataManagerTestDlg::IDD, pParent)
    , m_csTestResult(_T(""))
    , m_pDsmDataLayer(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDsmDataManagerTestDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_TEST_RESULT, m_csTestResult);
}

BEGIN_MESSAGE_MAP(CDsmDataManagerTestDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_BUTTON_BEGIN_DETECT, &CDsmDataManagerTestDlg::OnBnClickedButtonBeginDetect)
    ON_BN_CLICKED(IDC_BUTTON_GET_DETECT_RESULT, &CDsmDataManagerTestDlg::OnBnClickedButtonGetDetectResult)
    ON_BN_CLICKED(IDC_BUTTON_LOAD_SCL, &CDsmDataManagerTestDlg::OnBnClickedButtonLoadScl)
    ON_BN_CLICKED(IDC_BUTTON_STOP_RECV, &CDsmDataManagerTestDlg::OnBnClickedButtonStopRecv)
    ON_BN_CLICKED(IDC_BUTTON_SET_CUR_CB, &CDsmDataManagerTestDlg::OnBnClickedButtonSetCurCb)
    ON_BN_CLICKED(IDC_BUTTON_SET_RECV, &CDsmDataManagerTestDlg::OnBnClickedButtonSetRecv)
    ON_BN_CLICKED(IDC_BUTTON_PARAM_CONFIG_BASIC, &CDsmDataManagerTestDlg::OnBnClickedButtonParamConfigBasic)
    ON_BN_CLICKED(IDC_BUTTON_RECV_CONFIG_PUB, &CDsmDataManagerTestDlg::OnBnClickedButtonRecvConfigPub)
    ON_BN_CLICKED(IDC_BUTTON_SET_FT3_CUR_BLK, &CDsmDataManagerTestDlg::OnBnClickedButtonSetFt3CurBlk)
    ON_BN_CLICKED(IDC_BUTTON_SET_MODE, &CDsmDataManagerTestDlg::OnBnClickedButtonSetMode)
    ON_BN_CLICKED(IDC_BUTTON_ADD_FILTER, &CDsmDataManagerTestDlg::OnBnClickedButtonAddFilter)
    ON_BN_CLICKED(IDC_BUTTON_BEGAIN_RECORD, &CDsmDataManagerTestDlg::OnBnClickedButtonBegainRecord)
    ON_BN_CLICKED(IDC_BUTTON_STOP_RECORD, &CDsmDataManagerTestDlg::OnBnClickedButtonStopRecord)
    ON_BN_CLICKED(IDC_BUTTON_RECV_INIT, &CDsmDataManagerTestDlg::OnBnClickedButtonRecvInit)
    ON_BN_CLICKED(IDC_BUTTON_RECV_UNINIT, &CDsmDataManagerTestDlg::OnBnClickedButtonRecvUninit)
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_BUTTON_SET_TIME, &CDsmDataManagerTestDlg::OnBnClickedButtonSetTime)
END_MESSAGE_MAP()


// CDsmDataManagerTestDlg 消息处理程序

BOOL CDsmDataManagerTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

    reinterpret_cast<CComboBox*>(GetDlgItem(IDC_COMBO_DETECT_ITEM))->SetCurSel(0);
    reinterpret_cast<CComboBox*>(GetDlgItem(IDC_COMBO_DSM_MODE))->SetCurSel(0);
    CString csRecordWavePath = L"\\FlashDisk2\\RecordWave\\A.pcap";
    ((CEdit*)(GetDlgItem(IDC_EDIT_RECORD_WAVE_PATH)))->SetWindowText(csRecordWavePath);


    m_pDsmDataLayer = CDsmDataLayer::getInstance();
    m_pDsmDataLayer->Init();

    SetTimer(1, 500, NULL);

    {
        SYSTEMTIME sysTime;
        GetLocalTime(&sysTime);
        CString strSysTime;
        strSysTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"), sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
        GetDlgItem(IDC_EDIT_TIME)->SetWindowText(strSysTime) ;
    }

    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

CDsmDataManagerTestDlg::~CDsmDataManagerTestDlg()
{
    if (m_pDsmDataLayer)
    {
        m_pDsmDataLayer->UnInit();
    }
}

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CDsmDataManagerTestDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	if (AfxIsDRAEnabled())
	{
		DRA::RelayoutDialog(
			AfxGetResourceHandle(), 
			this->m_hWnd, 
			DRA::GetDisplayMode() != DRA::Portrait ? 
			MAKEINTRESOURCE(IDD_DSMDATAMANAGERTEST_DIALOG_WIDE) : 
			MAKEINTRESOURCE(IDD_DSMDATAMANAGERTEST_DIALOG));
	}
}
#endif


void CDsmDataManagerTestDlg::OnBnClickedButtonBeginDetect()
{
    // TODO: 在此添加控件通知处理程序代码
    if (NULL != m_pDsmDataLayer)
    {
        m_pDsmDataLayer->BeginDetect(static_cast<NetFt3RecvFrameType>(reinterpret_cast<CComboBox*>(GetDlgItem(IDC_COMBO_DETECT_ITEM))->GetCurSel()));
    }
    m_csTestResult = L"";
    UpdateData(FALSE);
}

void CDsmDataManagerTestDlg::OnBnClickedButtonGetDetectResult()
{
    // TODO: 在此添加控件通知处理程序代码
    if (NULL != m_pDsmDataLayer)
    {
        NetFt3RecvFrameType enDetectType = m_pDsmDataLayer->GetCurrentRecvType();
        uint32_t nCount = m_pDsmDataLayer->GetNetDetectItemsCount();
        switch(enDetectType)
        {
        case NET_RECV_TYPE_SMV:
            m_csTestResult.Format(L"Net Detect Smv Detect Count %d ",nCount);
            break;
        case NET_RECV_TYPE_GOOSE:
            m_csTestResult.Format(L"Net Detect Goose Detect Count %d ",nCount);
            break;
        case NET_RECV_TYPE_IEEE_1588:
            m_csTestResult.Format(L"Net Detect Type 1588 Count %d ",nCount);
            break;
        case FT3_RECV_TYPE_SMV:
            m_csTestResult.Format(L"Ft3 Detect Count %d ",nCount);
            break;
        default:
            m_csTestResult.Format(L"Detect Type Unkown.");
            break;
        }
    }
    UpdateData(FALSE);
}

void CDsmDataManagerTestDlg::OnBnClickedButtonLoadScl()
{
    // TODO: 在此添加控件通知处理程序代码
    if (NULL != m_pDsmDataLayer)
    {
        CSCLManager* pSclMgr = m_pDsmDataLayer->GetSclManager();
        DWORD dStart, dStop;
        dStart = GetTickCount();
        pSclMgr->Load(SCL_PAHT);
        dStop = GetTickCount();
        m_csTestResult.Format(L"Scl (%s) Load Time : %d ms", SCL_PAHT.c_str(), dStop-dStart);
        RETAILMSG(TRUE, (m_csTestResult));
    }
    UpdateData(FALSE);
}

void CDsmDataManagerTestDlg::OnBnClickedButtonStopRecv()
{
    // TODO: 在此添加控件通知处理程序代码
    if(NULL != m_pDsmDataLayer)
    {
        m_pDsmDataLayer->StopRecvData();
    }
}

void CDsmDataManagerTestDlg::OnBnClickedButtonSetCurCb()
{
    // TODO: 在此添加控件通知处理程序代码
    if (NULL != m_pDsmDataLayer)
    {
        uint32_t nDetectItems = m_pDsmDataLayer->GetNetDetectItemsCount();
        if (nDetectItems > 0)
        {
            CNetDetectItem* pItem = m_pDsmDataLayer->GetNetDetectItem(0);
            m_pDsmDataLayer->SetCurrentCtrlBlk(pItem);
        }
    }
}

void CDsmDataManagerTestDlg::OnBnClickedButtonSetRecv()
{
    // TODO: 在此添加控件通知处理程序代码
    if (NULL != m_pDsmDataLayer)
    {
        DSM_OPERATE_MODE enMode = m_pDsmDataLayer->GetCurrentMode();
        switch(enMode)
        {
        case DSM_MODE_UNKOWN:
        case DSM_MODE_MAIN_PAGE:
        case DSM_MODE_QUIT_CURRENT_PAGE:
        case DSM_MODE_DETECT:
        case DSM_MODE_FILTER:
            {

            }
            break;
        case DSM_MODE_SMV_RECV_OSCILLOGRAM:
            {
                CDataOscillogram data;
                m_pDsmDataLayer->GetOscillogramData(&data);
                uint32_t nChannelCounts = data.GetChannelCount();
            }
            break;
        case DSM_MODE_SMV_RECV_EFFECTIVE_VALUE:
            {
                CDataEffectiveVal data;
                m_pDsmDataLayer->GetEffectiveVal(&data);
                uint32_t nChannelCounts = data.GetChannelCount();
            }
            break;
        case DSM_MODE_SMV_RECV_PHASOR:
            {
                CDataPhasor data;
                m_pDsmDataLayer->GetPhasorData(&data);
                uint32_t nChannelCounts = data.GetChannelCount();
            }
            break;
        case DSM_MODE_SMV_RECV_XU_LIANG:
            {
                CDataXuliang data;
                m_pDsmDataLayer->GetXuliangData(&data);
                uint32_t nChannelCounts = data.GetChannelCount();
            }
            break;
        case DSM_MODE_SMV_RECV_POWER:
            {
                CDataPower data;
                m_pDsmDataLayer->GetPowerData(&data);
                uint32_t nChannelCounts = data.GetChannelCount();
            }
            break;
        case DSM_MODE_SMV_RECV_HARMONIC:
            {
                CDataHarmonic data;
                m_pDsmDataLayer->GetHarmonicData(&data);
                uint32_t nChannelCounts = data.GetChannelCount();
            }
            break;
        case DSM_MODE_SMV_RECV_DOUBLE_AD:
            {
                CDataDoubleAD data;
                m_pDsmDataLayer->GetDoubleAdData(&data);
                uint32_t nChannelCounts = data.GetChannelCount();
            }
            break;
        case DSM_MODE_SMV_RECV_PHASE_CHECK:
            {
                CDataPhaseCheck data;
                m_pDsmDataLayer->GetPhaseCheckData(&data);
                uint32_t nChannelCounts = data.GetChannelCount();
            }
            break;
        case DSM_MODE_SMV_RECV_POLARITY:
            {
                CDataPolarity data;
                m_pDsmDataLayer->GetPolarityData(&data);
                uint32_t nChannelCounts = data.GetChannelCount();
            }
            break;
        case DSM_MODE_SMV_RECV_FRAME_MONITOR:
        case DSM_MODE_SMV_RECV_STATUS_WATCH:
            {
                CDataStatusWatch data;
                m_pDsmDataLayer->GetStatusWatchData(&data);
                uint32_t nChannelCounts = data.GetChannelCount();
            }
            break;
        case DSM_MODE_SMV_RECV_DISPERSION:
        case DSM_MODE_SMV_RECV_FRAME_STATIC:
            {
                CDataFrameStatistics data;
                m_pDsmDataLayer->GetFrameStatistics(&data);
                //uint32_t nChannelCounts = data.GetChannelCount();
            }
            break;
        case DSM_MODE_SMV_RECV_RECORD_WAVE:
            break;
        case DSM_MODE_SMV_RECV_REAL_TIME_MSG:
            {
                CDataRealTimeMsgData data;
                m_pDsmDataLayer->GetRealTimeMsg(&data);
                uint32_t nLen = data.m_nFrameLen;
            }
            break;

        case DSM_MODE_GOOSE_RECV_REALTIME_VAL:
            {
                CGooseInfo data;
                m_pDsmDataLayer->GetGooseRealTimeValue(&data);
            }
            break;
        case DSM_MODE_GOOSE_RECV_CHANGE_LIST:
            {
                CDataGooseChangeList data;
                m_pDsmDataLayer->GetGooseChangeList(&data);
            }
            break;
        case DSM_MODE_GOOSE_RECV_FRAME_STATIC:
            {
                CGooseFrameStatistics data;
                m_pDsmDataLayer->GetGooseFrameStatistics(&data);
            }
            break;
        case DSM_MODE_GOOSE_RECV_FRAME_MONITOR:
        case DSM_MODE_GOOSE_RECV_REAL_TIME_MSG:
            {
                CDataRealTimeMsgData data;
                m_pDsmDataLayer->GetRealTimeMsg(&data);
                uint32_t nLen = data.m_nFrameLen;
            }
            break;
        case DSM_MODE_GOOSE_RECV_RECORD_WAVE:
            {

            }
            break;
        default:
            break;
        }
    }
}

void CDsmDataManagerTestDlg::OnBnClickedButtonParamConfigBasic()
{
    // TODO: 在此添加控件通知处理程序代码
    CParamConfigBasicDlg dlg;
    dlg.DoModal();
}

void CDsmDataManagerTestDlg::OnBnClickedButtonRecvConfigPub()
{
    // TODO: 在此添加控件通知处理程序代码
    CSmvRecvConfigPubDlg dlg;
    dlg.DoModal();
}

void CDsmDataManagerTestDlg::OnBnClickedButtonSetFt3CurBlk()
{
    // TODO: 在此添加控件通知处理程序代码
    if (NULL != m_pDsmDataLayer)
    {
        uint32_t nDetectItems = m_pDsmDataLayer->GetFt3DetectItemsCount();
        if (nDetectItems > 0)
        {
            CFt3DetectItem* pItem = m_pDsmDataLayer->GetFt3DetectItem(0);
            m_pDsmDataLayer->SetCurrentCtrlBlk(pItem);
        }
    }
}

void CDsmDataManagerTestDlg::OnBnClickedButtonSetMode()
{
    // TODO: 在此添加控件通知处理程序代码
    if (m_pDsmDataLayer)
    {
        DSM_OPERATE_MODE enMode = static_cast<DSM_OPERATE_MODE>(((CComboBox*)(GetDlgItem(IDC_COMBO_DSM_MODE)))->GetCurSel());
        m_pDsmDataLayer->SetCurrentMode(enMode);
    }
}

void CDsmDataManagerTestDlg::OnBnClickedButtonAddFilter()
{
    // TODO: 在此添加控件通知处理程序代码
    uint32_t n = ((CComboBox*)(GetDlgItem(IDC_COMBO_DETECT_ITEM)))->GetCurSel();
    if (NULL != m_pDsmDataLayer)
    {
        //smv,goose
        if (n == 0 || n == 1)
        {
            uint32_t nDetectItems = m_pDsmDataLayer->GetNetDetectItemsCount();
            for (uint32_t i = 0; i < /*nDetectItems*/1; ++i)
            {
                CNetDetectItem* pItem = m_pDsmDataLayer->GetNetDetectItem(i);
                m_pDsmDataLayer->AddOneSelectedCtrlBlk(pItem);
            }
        }
        //Ft3
        else if (n == 3)
        {
            uint32_t nDetectItems = m_pDsmDataLayer->GetFt3DetectItemsCount();
            for (uint32_t i = 0; i < nDetectItems; ++i)
            {
                CFt3DetectItem* pItem = m_pDsmDataLayer->GetFt3DetectItem(0);
                m_pDsmDataLayer->AddOneSelectedCtrlBlk(pItem);
            }
        }
        
        m_pDsmDataLayer->AddSelectedCtrlBlkFinished();
    }
}

void CDsmDataManagerTestDlg::OnBnClickedButtonBegainRecord()
{
    // TODO: 在此添加控件通知处理程序代码
    if (!m_pDsmDataLayer)
    {
        return;
    }
    m_pDsmDataLayer->AddRecordItem(m_pDsmDataLayer->GetCurrentCtrlBlk());
    NetFt3RecvFrameType enType = static_cast<NetFt3RecvFrameType>(reinterpret_cast<CComboBox*>(GetDlgItem(IDC_COMBO_DETECT_ITEM))->GetCurSel());
    CString csStr;
    ((CEdit*)(GetDlgItem(IDC_EDIT_RECORD_WAVE_PATH)))->GetWindowText(csStr);
    m_pDsmDataLayer->SetRecordWavePath(csStr.GetString());
    switch(enType)
    {
    case NET_RECV_TYPE_GOOSE:
        m_pDsmDataLayer->StartRecordGoose();
        break;
    case NET_RECV_TYPE_SMV:
        m_pDsmDataLayer->StartRecordNetSmv();
        break;
    case FT3_RECV_TYPE_SMV:
        m_pDsmDataLayer->StartRecordFt3();
        break;
    }

}

void CDsmDataManagerTestDlg::OnBnClickedButtonStopRecord()
{
    // TODO: 在此添加控件通知处理程序代码
    uint32_t nRet;
    m_pDsmDataLayer->StopRecordWave(nRet);
}


void CDsmDataManagerTestDlg::OnBnClickedButtonRecvInit()
{
    // TODO: 在此添加控件通知处理程序代码
    m_pDsmDataLayer->Init();
}

void CDsmDataManagerTestDlg::OnBnClickedButtonRecvUninit()
{
    // TODO: 在此添加控件通知处理程序代码
    m_pDsmDataLayer->UnInit();
}

void CDsmDataManagerTestDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (nIDEvent == 1)
    {
        UpdateTime();
    }

    CDialog::OnTimer(nIDEvent);
}

void CDsmDataManagerTestDlg::UpdateTime()
{
    //系统时间
    SYSTEMTIME sysTime;
    GetLocalTime(&sysTime);
    CString strSysTime;
    strSysTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"), sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
    GetDlgItem(IDC_STATIC_SYS_TIME)->SetWindowText(strSysTime) ;

    //FpGa时间
    FpgaTimeSyncMode enFpgaSyncMode = CDsmDataLayer::getInstance()->GetFpgaTimeSyncMode();
    CString strDispTime(L"");
    switch(enFpgaSyncMode)
    {
    case FPGA_TIME_SYNC_IRIGB_NEG:
    case FPGA_TIME_SYNC_IRIGB_POS:
        strDispTime += L"IRIG_B:";
        break;
    case FPGA_TIME_SYNC_IEEE1588_A:
        strDispTime += L"1588:";
        break;
    default:
        break;
    }

    UTC_TIME_t utc_time;
    if(!CDsmDataLayer::getInstance()->GetFpgaUtcTime(&utc_time))
    {
        RETAILMSG(1,(L"Get Fpga Time Failed !\r\n"));
        return;
    }

    CTime tm(utc_time.uSecondSinceEpoch);
    CString strFpgaTime;
    strFpgaTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"), tm.GetYear(), tm.GetMonth(), tm.GetDay(), tm.GetHour(), tm.GetMinute(), tm.GetSecond());
    strDispTime += strFpgaTime;

    GetDlgItem(IDC_STATIC_FPGA_TIME)->SetWindowText(strDispTime);

}

void CDsmDataManagerTestDlg::OnBnClickedButtonSetTime()
{
    // TODO: 在此添加控件通知处理程序代码
    //系统时间
    SYSTEMTIME sysTime;
    CString strTime;
    GetDlgItem(IDC_EDIT_TIME)->GetWindowText(strTime);
    swscanf_s(strTime, L"%04d-%02d-%02d %02d:%02d:%02d", &sysTime.wYear, &sysTime.wMonth, &sysTime.wDay, &sysTime.wHour, &sysTime.wMinute, &sysTime.wSecond);

    if (!SetLocalTime(&sysTime))
    {
        MessageBox(L"Set SysTime Failed \r\n");
    }

    if (((CButton*)GetDlgItem(IDC_CHECK_SET_FPGA_TIME))->GetCheck() == BST_CHECKED)
    {
        CTime tm(sysTime);
        UTC_TIME_t utc_time;
        utc_time.uSecondSinceEpoch  = tm.GetTime();
        utc_time.uFractionOfSecond = sysTime.wMilliseconds * 1000000;

        if (!m_pDsmDataLayer->SetFpgaUtcTime(&utc_time))
        {
            MessageBox(L"Set Fpga Time Failed ") ;
        }
    }
}
