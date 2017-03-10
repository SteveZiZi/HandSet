// DsmDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Dsm.h"
#include "DsmDlg.h"

#include "dbt.h"
#include "resourcebmp.h"
#include "src/main/utils/utility.h"
#include "src/main/mgr/StatusMgr.h"
#include "src/main/mgr/navibarmgr.h"
#include "src/include/p_wndusermsg_def.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/main/popdlg/progressbardlg.h"
#include "src/service/datalayer/dsmdatalayer.h"


// navibar
#if CHUN_HUA
#define DSM_NAVIBAR_WIDTH               107                         ///< �����˵�item�Ŀ��
#define DSM_NAVIBAR_NUMS                6                           ///< �����˵�item��Ŀ
#define DSM_NAVIBAR_RECT                CRect(0, 0, 107, 480)     ///< �����˵�����
#define DSM_NAVIBAR_COLOR_BK            RGB(0x0, 0x0, 0x0)
#define DSM_NAVIBAR_COLOR_TEXT          RGB(255, 255, 255)
#else
#define DSM_NAVIBAR_HEIGHT              68                         ///< �����˵�item�ĸ߶�
#define DSM_NAVIBAR_NUMS                6                           ///< �����˵�item��Ŀ
#define DSM_NAVIBAR_RECT                CRect(0, 37, 110, 444)     ///< �����˵�����
#define DSM_NAVIBAR_COLOR_BK            RGB(112, 115, 133)
#define DSM_NAVIBAR_COLOR_TEXT          RGB(227, 232, 239)
#endif

#if !SHENG_ZE
#include <devload.h>
#include "src/main/cregedit.h"
#endif


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define DSM_UPDATE_BOTTOMBAR_TIME       1000            ///< �����·�״̬��������
#define DSM_UPDATE_CONFIG_TIME          10000           ///< 10���ѯһ���Ƿ���Ҫ���������ļ�
//#define DSM_UPDATE_PARAM_CONFIG_TIME    31000           ///< 31���ѯһ���Ƿ���Ҫ���������ļ�
#define DSM_UPDATE_SYNC_FPGA_TIME       60000           ///< 1���Ӵ�FPGAͬ��ʱ��

#define DSM_POWER_PRESS_TIME            3000            ///< ����4000ms

#define WM_DSM_LOAD_SCL                 PRIVATE_WND_USERID + 450        ///< ����SCL


// CDsmDlg �Ի���

CDsmDlg::CDsmDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDsmDlg::IDD, pParent)
    , m_nTimerStatus(0)
    , m_nTimerSaveCfg(0)
    , m_nTimerFPGA(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    m_pEventProc = CEventProc::GetInstance();
}

CDsmDlg::~CDsmDlg()
{
    CDsmDataLayer::getInstance()->UnInit();
}

void CDsmDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDsmDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	//}}AFX_MSG_MAP
    ON_WM_ERASEBKGND()
    ON_WM_TIMER()
    ON_MESSAGE(PWID_UPDATE_INPUTMODE, &CDsmDlg::OnUpdateInputMode)
    ON_MESSAGE(WM_DSM_LOAD_SCL, &CDsmDlg::OnLoadSCL)
    ON_MESSAGE(WM_DSM_SYNC_NOW, &CDsmDlg::OnSyncTime)
#if !SU_FEI
	ON_WM_PAINT()
#endif
#if !SHENG_ZE
	ON_WM_DEVICECHANGE()
#endif
END_MESSAGE_MAP()


// CDsmDlg ��Ϣ�������

BOOL CDsmDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ
    // ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

    // ���ȴ���������ײ�״̬���ؼ�
    _createUICtl();

    if(!CDsmDataLayer::getInstance()->Init())
    {
        ASSERT(FALSE);
        EndDialog(IDCANCEL);
    }

    // ����FPGAʱ��
    _setFPGATime();

    // ����SCL�ļ�
    PostMessage(WM_DSM_LOAD_SCL, 0, 0);

    CRect rcDSM(0, 0, dsm_config::GetScreenWidth(), dsm_config::GetScreenHeight());

    MoveWindow(rcDSM, FALSE);
    ::SetForegroundWindow(m_hWnd);

    m_nTimerStatus      = SetTimer(1, DSM_UPDATE_BOTTOMBAR_TIME, NULL);
    m_nTimerSaveCfg     = SetTimer(2, DSM_UPDATE_CONFIG_TIME, NULL);
    m_nTimerFPGA        = SetTimer(3, DSM_UPDATE_SYNC_FPGA_TIME, NULL);

    ShowCursor(FALSE);

    // ��ʼ���˵�
    _initNavibar();
#if !SHENG_ZE
	m_tUsbStorageInfo.bActive = FALSE;
	m_tSDStorageInfo.bActive = FALSE;
	// ö��U�� �� SD��
	EnumActiveStorage(TRUE);
	if (m_tUsbStorageInfo.bActive) {
		CStatusMgr::SetUSBDisk(DSM_STATU_CONNECT_UCARD);
	}
	else {
		CStatusMgr::SetUSBDisk(DSM_STATU_DISCONNECT_UCARD);
	}
	if (m_tSDStorageInfo.bActive) {
		CStatusMgr::SetSD(DSM_STATU_CONNECT_SD);
	}
	else {
		CStatusMgr::SetSD(DSM_STATU_DISCONNECT_SD);
	}
#endif
    m_pEventProc->ShowWizard();

    ::SetProp(m_hWnd, DSM_PROP_VAL, this);

	return FALSE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CDsmDlg::OnPaint()
{

}


#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CDsmDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	if (AfxIsDRAEnabled())
	{
		DRA::RelayoutDialog(
			AfxGetResourceHandle(), 
			this->m_hWnd, 
			DRA::GetDisplayMode() != DRA::Portrait ? 
			MAKEINTRESOURCE(IDD_DSM_DIALOG_WIDE) : 
			MAKEINTRESOURCE(IDD_DSM_DIALOG));
	}
}
#endif


BOOL CDsmDlg::DestroyWindow()
{
    // TODO: �ڴ����ר�ô����/����û���

    ::RemoveProp(m_hWnd, DSM_PROP_VAL);

    m_bitmapDefNavi.Detach();

    if(m_nTimerStatus != 0)
    {
        this->KillTimer(m_nTimerStatus);
        m_nTimerStatus = 0;
    }

    if(m_nTimerSaveCfg != 0)
    {
        this->KillTimer(m_nTimerSaveCfg);
        m_nTimerSaveCfg = 0;
    }

    if(m_nTimerFPGA != 0)
    {
        this->KillTimer(m_nTimerFPGA);
        m_nTimerFPGA = 0;
    }

    return CDialog::DestroyWindow();
}


BOOL CDsmDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: �ڴ����ר�ô����/����û���

    static bool bFirstPress = true;
    static int nPressTime = 0;

    if(WM_KEYUP == pMsg->message)
    {
        switch(pMsg->wParam)
        {
        case DSM_KEY_POWER:
            {
                if(DSM_POWER_PRESS_TIME > GetTickCount() - nPressTime)
                {
                    m_pEventProc->OnKeyPower(DSM_KEYEVT_LOCK);
                }

                nPressTime = 0;
                bFirstPress = true;

                return TRUE;
            }
            break;

        default:
            break;
        }
    }

    if(WM_KEYDOWN == pMsg->message)
    {
        if(DSM_KEY_POWER == pMsg->wParam && bFirstPress)
        {
            nPressTime = GetTickCount();
            bFirstPress = false;
        }

        switch(pMsg->wParam)
        {
        case VK_ESCAPE:
            m_pEventProc->OnKeyEsc();
            return TRUE;

        case VK_F7:
            m_pEventProc->SaveConfig();
            m_pEventProc->Exit();
            CDialog::OnCancel();
            return TRUE;

        case DSM_KEY_POWER:
            if(DSM_POWER_PRESS_TIME < (GetTickCount() - nPressTime))
            {
                m_pEventProc->OnKeyPower(DSM_KEYEVT_POWEROFF);
                bFirstPress = true;
            }
            return TRUE;

        case DSM_KEY_HOME:
            m_pEventProc->OnKeyHome();
            return TRUE;

        case DSM_KEY_SNAPSHOT:
            m_pEventProc->SnapShot();
            return TRUE;

        default:
            break;
        }
    }

    return CDialog::PreTranslateMessage(pMsg);
}

BOOL CDsmDlg::OnEraseBkgnd(CDC*  pDC  )
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    return TRUE;
}


void CDsmDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

    if(nIDEvent == m_nTimerStatus)
    {
        // ��ʱˢ�½���״̬
        _updateUI();
    }
    else if(nIDEvent == m_nTimerSaveCfg)
    {
        // ��ʱ��������
        m_pEventProc->SaveConfig();
    }
    else if(nIDEvent == m_nTimerFPGA && dsm_config::GetTimeAutoSync())
    {
        // ͬ������ʱ��
        _setLocalTime();
    }

    // �����ʱ����
    // �ο�kb 839859  105286 819635
    AfxGetApp()->OnIdle(1);

    CDialog::OnTimer(nIDEvent);
}



void CDsmDlg::_initNavibar()
{   
	//�ײ��˵���
#if CHUN_HUA
	m_ctlNaviBar.Create(DSM_NAVIBAR_WIDTH, DSM_NAVIBAR_NUMS, 
        DSM_NAVIBAR_RECT, this, IDC_NAVIBAR, TRUE);
#else
	m_ctlNaviBar.Create(DSM_NAVIBAR_HEIGHT, DSM_NAVIBAR_NUMS, 
		DSM_NAVIBAR_RECT, this, IDC_NAVIBAR, TRUE);
#endif
    // ������ʽ
    m_ctlNaviBar.SetHasGridline(FALSE, FALSE);
    m_ctlNaviBar.SetBkColor(DSM_NAVIBAR_COLOR_BK);
    m_ctlNaviBar.SetTextColor(DSM_NAVIBAR_COLOR_TEXT);

    m_ctlNaviBar.SetFont(dsm_fontmgr::GetInstance()->GetFont(DSM_FTSIZE_NAVIGATION));

	m_bitmapDefNavi.Attach(dsm_bmpmgr::Get(DSM_BMP_DEFNAVIBK));
	m_ctlNaviBar.SetDefaultItemBkImage(&m_bitmapDefNavi);

    CNaviBarMgr::GetInstance()->SetNaviBar(&m_ctlNaviBar);

}

LRESULT CDsmDlg::OnUpdateInputMode( WPARAM /* wParam */, LPARAM /* lParam */ )
{
    m_ctlTopbar._InvalidateICON();
    return 0;
}

#if SHENG_ZE
LRESULT CDsmDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    // TODO: �ڴ����ר�ô����/����û���

    if(WM_DEVICECHANGE == message)
    {
        DEV_BROADCAST_HDR* dhr = NULL;
        PDEV_BROADCAST_PORT lpdbp = NULL;

        switch(wParam)
        {
            case DBT_DEVICEARRIVAL:
                dhr = (DEV_BROADCAST_HDR *)lParam;
                lpdbp = (PDEV_BROADCAST_PORT)dhr;

                // DBT_DEVTYP_PORT����Ĭ��������DSKx
                if(lpdbp->dbcp_devicetype == DBT_DEVTYP_PORT)
                {
                    InvalidateRect(DSM_RCTOP_ICONRECT, TRUE);
                }
                break;

            case DBT_DEVICEREMOVECOMPLETE:
                dhr = (DEV_BROADCAST_HDR *)lParam;
                lpdbp = (PDEV_BROADCAST_PORT)dhr;

                if(lpdbp->dbcp_devicetype == DBT_DEVTYP_PORT)
                {
                    InvalidateRect(DSM_RCTOP_ICONRECT, TRUE);
                }
                break;
        }
    }

    return CDialog::DefWindowProc(message, wParam, lParam);
}
#else
LRESULT CDsmDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return CDialog::DefWindowProc(message, wParam, lParam);
}

BOOL CDsmDlg::OnDeviceChange(UINT nEventType, DWORD_PTR dwData)
{
	switch(nEventType) 
	{ 
	case DBT_DEVICEARRIVAL: 
		EnumActiveStorage(FALSE);
		break; 
	case DBT_DEVICEREMOVECOMPLETE:
		{
			PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)dwData;
			if (pHdr->dbch_devicetype == DBT_DEVTYP_PORT) {
				PDEV_BROADCAST_PORT dbt = (PDEV_BROADCAST_PORT)dwData;
				if (wcscmp(dbt->dbcp_name, m_tSDStorageInfo.szDriverName) == 0) {
					m_tSDStorageInfo.bActive = FALSE;
				}
				else if (wcscmp(dbt->dbcp_name, m_tUsbStorageInfo.szDriverName) == 0) {
					m_tUsbStorageInfo.bActive = FALSE;
				}
			}
		}
		break;
	}
	if (m_tUsbStorageInfo.bActive) {
		CStatusMgr::SetUSBDisk(DSM_STATU_CONNECT_UCARD);
	}
	else {
		CStatusMgr::SetUSBDisk(DSM_STATU_DISCONNECT_UCARD);
	}
	if (m_tSDStorageInfo.bActive) {
		CStatusMgr::SetSD(DSM_STATU_CONNECT_SD);
	}
	else {
		CStatusMgr::SetSD(DSM_STATU_DISCONNECT_SD);
	}

	return TRUE;
}
#endif
void CDsmDlg::_createUICtl()
{
    // ����������ײ�״̬��
    m_ctlTopbar.Create(NULL, WS_CHILD | WS_VISIBLE, DSM_RCTOP_STATUBAR, this, IDC_DSMDLG_CTL_TOPBAR);
    m_ctlBottom.Create(NULL, WS_CHILD | WS_VISIBLE, DSM_RCBOTTOM_STATUBAR, this, IDC_DSMDLG_CTL_BOTTOMBAR);
}

/**
 *      ����FPGAʱ��
 */
void CDsmDlg::_setFPGATime()
{
    UTC_TIME_t t = DM_GetUTC();
    if(!CDsmDataLayer::getInstance()->SetFpgaUtcTime(&t))
    {
        ASSERT(FALSE);
        RETAILMSG(true, (L"dsm: SetFpgaUtcTime failed!\n"));
    }
}

/**
 *      ʹ��FPGAʱ��ͬ������ʱ��
 */
void CDsmDlg::_setLocalTime()
{
    UTC_TIME_t t;

    //switch(CDsmDataLayer::getInstance()->GetFpgaTimeSyncMode())
    switch(CDsmDataLayer::getInstance()->GetParamConfigBasic()->GetTimeSyncMode())
    {
        // B���ʱ
    case FPGA_TIME_SYNC_IRIGB_NEG:
    case FPGA_TIME_SYNC_IRIGB_POS:
        //if(CDsmDataLayer::getInstance()->IsPortActive(LIGHT_PORT_IRIG_B) && CDsmDataLayer::getInstance()->GetFpgaUtcTime(&t))
        // ���Ӳ�����ͬ���ź�ʱ��Ҳ����FPGA�뱾��ʱ���ͬ��
        if(CDsmDataLayer::getInstance()->GetFpgaUtcTime(&t))
        {
            SYSTEMTIME st = DM_GetLocalTimeFromUTC(t);
            SetLocalTime(&st);
        }
        break;

        // 1588��ʱ
    case FPGA_TIME_SYNC_IEEE1588_A:
    case FPGA_TIME_SYNC_IEEE1588_B:
    case FPGA_TIME_SYNC_IEEE1588_C:
#pragma message(__FILE__ "(" DSM_STRING(__LINE__) "): 1588��ʱ������ʱû��")
        //if(CDsmDataLayer::getInstance()->IsPortLinked(NET_PORT_ELECTRONIC) && CDsmDataLayer::getInstance()->GetFpgaUtcTime(&t))
        // ���Ӳ�����ͬ���ź�ʱ��Ҳ����FPGA�뱾��ʱ���ͬ��
        if(CDsmDataLayer::getInstance()->GetFpgaUtcTime(&t))
        {
            SYSTEMTIME st = DM_GetLocalTimeFromUTC(t);
            SetLocalTime(&st);
        }
        break;

    default:
        break;
    }
}

/**
 *      ���½���״̬
 */
void CDsmDlg::_updateUI()
{
    m_ctlTopbar._InvalidateICON();
    m_ctlBottom.Invalidate(TRUE);

    ::PostMessage(m_pEventProc->GetCurrentPageHwnd(), WM_DSM_TIME_TICK, 0, 0);

    int nBattery = CStatusMgr::GetBattery();

    // �رյ�Դ
    if(nBattery == 0 && !CStatusMgr::GetCharge())
    {
        m_pEventProc->Close();
    }
    // �����ʾ
    else if(nBattery <= DSM_BATTERY_LOW_WARNNING && !CStatusMgr::GetCharge())
    {
        static int nCount = 0;

        if(nCount++ % 5 == 0)
        {
            nCount = 0;
            CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_BATTERY_LOW_WARNNING), 2000);
        }
    }
}

LRESULT CDsmDlg::OnLoadSCL( WPARAM, LPARAM )
{
    // ��ȡ������SCL���ļ���
    CString csSclFile(CDsmDataLayer::getInstance()->GetParamConfigBasic()->GetSclFile().c_str());
    CString csSCLPath;

    if(_checkSCL(csSclFile, csSCLPath))
    {
        HWND hWnd = ::GetFocus();

        CProgressBarDlg dlgPro(this);

        // ����SCL
        _SCLTaskArgs arg;
        arg.csFile = csSCLPath;
        arg.m_pWnd = &dlgPro;

        CString csTips;

        dlgPro._SetTips(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_LOADSCL_TIPS));
        dlgPro._SetTitle(dsm_stringMgr::Get(DSM_STR_BASIC_PARAM_LOADSCL));
        dlgPro._SetTask(_LoadSCLTask, &arg);

        CRect rc(dlgPro._GetPicRC());
        rc.OffsetRect(-26, 0);
        dlgPro._SetPicRC(rc);
        rc = dlgPro._GetTextRC();
        rc.OffsetRect(-26, 0);
        dlgPro._SetTextRC(rc);

        if(IDOK == dlgPro.DoModal())
        {
            // ����SCL���óɹ�
            csTips.Format(dsm_stringMgr::Get(DSM_STR_INFO_LOAD_SCL_SUCCESS), csSclFile);
        }
        else
        {
            csTips.Format(dsm_stringMgr::Get(DSM_STR_INFO_LOAD_SCL_FAILEDF), csSclFile);
            DSM_DEBUG(L"dsm: CDsmDlg::OnInitDialog: load scl file failure.\n");
        }

        if(IsWindow(hWnd))
        {
            ::SetFocus(hWnd);
        }
        else
        {
            DSM_SetZFocus(CEventProc::GetInstance()->GetCurrentPageHwnd());
        }

        CStatusMgr::SetTips(csTips, 5000);
    }

    return 1;
}

bool CDsmDlg::_checkSCL(const CString csScl, CString& csSCLPath)
{
    CString csSDPath;       // SDĿ¼

    if(!csScl.IsEmpty())
    {
        // �ж�SD��״̬
        if(DSM_GetSDDriver(csSDPath))
        {
            csSCLPath = csSDPath + DSM_CFG_SCL_FILE_FOLDER;         // SCLĿ¼

            // �ж�SCLĿ¼�Ƿ����
            if(DSM_SDPathIsExist(csSCLPath))
            {
                csSCLPath += csScl;

                // ���SCl�ļ��Ƿ����
                if(DSM_IsFileExist(csSCLPath))
                {
                    return true;
                }
                else
                {
                    CString csTips;
                    csTips.Format(dsm_stringMgr::Get(DSM_STR_INFO_LOAD_SCL_FAILED), csScl);;
                    CStatusMgr::SetTips(csTips);
                }
            }
            else
            {
                // ����SClĿ¼
                if(DSM_CreateSDPath(csSCLPath))
                {
                    return true;
                }
            }
        }
        else
        {
            CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_SD_NOT_FOUND_SCL));
        }
    }

    return false;
}

LRESULT CDsmDlg::OnSyncTime( WPARAM wParam, LPARAM )
{
    if(wParam == DSM_SYNC_LOCAL_TO_FPGA)
    {
        // ����ʱ��ͬ����FPGA
        _setFPGATime();
    }
    else
    {
        // FPGAʱ��ͬ��������ʱ��
        _setLocalTime();
    }

    return 1;
}

#if !SHENG_ZE
void CDsmDlg::EnumActiveStorage(BOOL bEnumAll)
{
	CRegistryEdit reg(HKEY_LOCAL_MACHINE, _T("Drivers\\Active"));
#define SD_DRIVER_KEY_NAME      TEXT("Drivers\\SDCARD\\ClientDrivers\\Class\\SDMemory_Class")
#define USB_DRIVER_KEY_NAME     TEXT("Drivers\\USB\\ClientDrivers\\Mass_Storage_Class")

	DWORD i = 0;
	WCHAR szKeyName[DEVKEY_LEN];
	WCHAR szSubKeyName[DEVKEY_LEN];
	while(1) {
		DWORD dwKeyNameLength = DEVKEY_LEN;
		if (!reg.RegEnumKeyEx(i, szKeyName, &dwKeyNameLength, 0, NULL, NULL, NULL)) {
			break;
		}

		CRegistryEdit subReg(reg.GetHKey(), szKeyName);
		DWORD dwType = REG_SZ;
		DWORD dwSubKeyLength = DEVKEY_LEN;
		subReg.RegQueryValueEx(_T("Key"), &dwType, (LPBYTE)szSubKeyName, &dwSubKeyLength);
		if (wcsstr(szSubKeyName, SD_DRIVER_KEY_NAME)) {
			m_tSDStorageInfo.bActive = TRUE;
			dwSubKeyLength = DEVKEY_LEN;
			subReg.RegQueryValueEx(_T("Name"), &dwType, (LPBYTE)m_tSDStorageInfo.szDriverName, &dwSubKeyLength);
            if (!bEnumAll) {
                break;
            }
		}
		else if (wcsstr(szSubKeyName, USB_DRIVER_KEY_NAME)) {
			m_tUsbStorageInfo.bActive = TRUE;
			dwSubKeyLength = DEVKEY_LEN;
            subReg.RegQueryValueEx(_T("Name"), &dwType, (LPBYTE)m_tUsbStorageInfo.szDriverName, &dwSubKeyLength);
            if (!bEnumAll) {
                break;
            }
		}

        /*
         *  SD����U�̶�ö�ٳɹ���ֱ�����������豸
         */
        if (m_tUsbStorageInfo.bActive && m_tSDStorageInfo.bActive) {
            break;
        }

		i++;
	}
}
#endif
