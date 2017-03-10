/**
 * \file
 *      StatusMgr.cpp
 *
 * \brief
 *      状态管理器
 *          获取DSTester的软硬件状态信息
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/3/25
 */
#include "StdAfx.h"
#include "StatusMgr.h"

#include "src/main/dsmdlg.h"
#include "src/main/common.h"
#include "src/main/resourcebmp.h"
#include "src/main/resourcestr.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/service/datalayer/dsmdatalayer.h"


#define IOCTL_GPIO_GETBIT       \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0303, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_BKL_SETDUTYCYCLE       \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0301, METHOD_BUFFERED, FILE_ANY_ACCESS)     ///< 设置背光亮度
#define IOCTL_BKL_GETDUTYCYCLE       \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0302, METHOD_BUFFERED, FILE_ANY_ACCESS)     ///< 获取背光亮度
#define IOCTL_BKL_SETDUTYCYCLEREG    \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0303, METHOD_BUFFERED, FILE_ANY_ACCESS)     ///< 设置背光亮度到注册表
#define IOCTL_BKL_SETBACKLINGTTIME  \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0304, METHOD_BUFFERED, FILE_ANY_ACCESS)     ///< 设置背光时间
#define IOCTL_BKL_GETBACKLINGTTIME  \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0305, METHOD_BUFFERED, FILE_ANY_ACCESS)     ///< 获取背光时间


#define BATTERY_100P                100         ///< 100%电量
#define BATTERY_80P                 80          ///< 80%电量
#define BATTERY_60P                 60          ///< 60%电量
#define BATTERY_40P                 40          ///< 40%电量
#define BATTERY_10P                 10          ///< 10%电量  提醒充电
#define BATTERY_0P                  0           ///< 0%电量   即将关机

HBITMAP CStatusMgr::m_hSnapPanel = NULL;
SYSTEM_POWER_STATUS_EX2 CStatusMgr::m_sStatus;
#if !SHENG_ZE
int     CStatusMgr::m_nSDStatu = DSM_STATU_DISCONNECT_USB;            ///< SD接入状态
int     CStatusMgr::m_nUCardStatu = DSM_STATU_DISCONNECT_SD;          ///< U盘接入状态
#endif

CStatusMgr::CStatusMgr(void)
: m_csTitle(L"")
, m_bLock(false)
, m_bAuthority(false)
, m_nTimerAuth(0)
, m_bShowInputMode(false)
, m_hBrightFile(INVALID_HANDLE_VALUE)
, m_bTips(false)
, m_csTips(L"")
, m_uTimeOut(0)
, m_crTips(RGB(0xFF, 0xFF, 0xFF))
, m_dwStyle(0x0)
{
    m_mapBatteryResID.SetAt(BATTERY_0P, DSM_BMP_TOPBAR_BATTERY_0);
    m_mapBatteryResID.SetAt(BATTERY_10P, DSM_BMP_TOPBAR_BATTERY_20);
    m_mapBatteryResID.SetAt(BATTERY_40P, DSM_BMP_TOPBAR_BATTERY_40);
    m_mapBatteryResID.SetAt(BATTERY_60P, DSM_BMP_TOPBAR_BATTERY_60);
    m_mapBatteryResID.SetAt(BATTERY_80P, DSM_BMP_TOPBAR_BATTERY_80);
    m_mapBatteryResID.SetAt(BATTERY_100P, DSM_BMP_TOPBAR_BATTERY_100);
}

CStatusMgr::~CStatusMgr(void)
{
    CloseHandle(m_hBrightFile);

    m_hBrightFile = NULL;

    m_sfileMapping.Close();
}

int CStatusMgr::GetBattery()
{
    ::ZeroMemory(&m_sStatus, sizeof(m_sStatus));
    GetSystemPowerStatusEx2(&m_sStatus, sizeof(SYSTEM_POWER_STATUS_EX2), FALSE);

    return m_sStatus.BatteryLifePercent;
}

int CStatusMgr::GetSD()
{
	return m_nSDStatu;
}

#if !SHENG_ZE
void CStatusMgr::SetSD(int nSDStatu)
{
	ASSERT(nSDStatu == DSM_STATU_CONNECT_SD || nSDStatu == DSM_STATU_DISCONNECT_SD);
	m_nSDStatu = nSDStatu;
}
#endif
int CStatusMgr::GetUSB()
{
//     DWORD dwId = 132;           // GPIO132 USB插入检测引脚
//     DWORD dwVal = 1;
// 
//     if (FALSE == DeviceIoControl(getInstance().m_hSDFile, IOCTL_GPIO_GETBIT, &dwId, sizeof(dwId), &dwVal, sizeof(dwVal), NULL, NULL)) {
//         RETAILMSG(1, (L"dsm: IOCTL_GPIO_GETBIT failed, read usb status.\r\n"));
//     }
// 
//     // 高电平 表示USB已经插入
//     return (dwVal == 1) ? DSM_STATU_CONNECT_USB : DSM_STATU_DISCONNECT_USB;

    return DSM_STATU_DISCONNECT_USB;
}

#if !SHENG_ZE

// 检查U盘是否插入
int CStatusMgr::GetUSBDisk()
{
	return m_nUCardStatu;
}

/**
 *      设置U盘状态
 *
 *  \param
 *      int         DSM_STATU_CONNECT_UCARD:      接入USB
 *                  DSM_STATU_DISCONNECT_UCARD:   拔出USB
 */
void CStatusMgr::SetUSBDisk(int nUCardStatu)
{
    ASSERT(nUCardStatu == DSM_STATU_CONNECT_UCARD || nUCardStatu == DSM_STATU_DISCONNECT_UCARD);
    m_nUCardStatu = nUCardStatu;
}

#endif
bool CStatusMgr::GetScreenLock()
{
    return getInstance().m_bLock;
}

void CStatusMgr::SetScreenLock( bool bLock )
{
    getInstance().m_bLock = bLock;

    ((CDsmDlg*)AfxGetMainWnd())->_InvalidateStatusRect(DSM_RCTOP_RIGHTICON);
}


// int CStatusMgr::GetExPower()
// {
//     ::ZeroMemory(&m_sStatus, sizeof(m_sStatus));
//     GetSystemPowerStatusEx2(&m_sStatus, sizeof(SYSTEM_POWER_STATUS_EX2), FALSE);
// 
//     return m_sStatus.ACLineStatus;
// }

CString CStatusMgr::GetTitle()
{
    return getInstance().m_csTitle;
}

void CStatusMgr::SetTitle( const CString& csTitle )
{
    getInstance().m_csTitle = csTitle;

    ((CDsmDlg*)AfxGetMainWnd())->_SetTitle(csTitle);
}

void CStatusMgr::SetTips( const CString& csTips, UINT uTimeOut /*= 3000 */, COLORREF color /*= RGB(0xFF, 0xFF, 0xFF) */ )
{
    CStatusMgr& mgr     = getInstance();

    mgr.m_csTips        = csTips;
    mgr.m_uTimeOut      = GetTickCount() + uTimeOut;
    mgr.m_crTips        = color;
    mgr.m_bTips         = true;
    mgr.m_dwStyle       = 0x01;

    ((CDsmDlg*)AfxGetMainWnd())->_UpdateBottomBar();
}

CString CStatusMgr::GetTips()
{
    return getInstance().m_csTips;
}

COLORREF CStatusMgr::GetTipsColor()
{
    return getInstance().m_crTips;
}

DWORD CStatusMgr::ShowTipsStyle()
{
    CStatusMgr& mgr = getInstance();

    if(mgr.m_bTips)
    {
        if(GetTickCount() > mgr.m_uTimeOut)
        {
            mgr.m_csTips    = L"";
            mgr.m_bTips     = false;
            mgr.m_crTips    = RGB(0xFF, 0xFF, 0xFF);
            mgr.m_dwStyle   = 0x00;
        }

        return mgr.m_dwStyle;
    }

    return 0x00;
}


HBITMAP CStatusMgr::GetSDBitmap()
{
    int nResID = DSM_BMP_TOPBAR_DCON_SD;

    if(DSM_STATU_CONNECT_SD == CStatusMgr::GetSD())
    {
        nResID = DSM_BMP_TOPBAR_CON_SD;
    }

    return dsm_bmpmgr::Get(nResID);
}

HBITMAP CStatusMgr::GetUSBBitmap()
{
    int nResID = DSM_BMP_TOPBAR_DCON_USB;

    if(DSM_STATU_CONNECT_USB == CStatusMgr::GetUSB())
    {
        nResID = DSM_BMP_TOPBAR_CON_USB;
    }
	
    return dsm_bmpmgr::Get(nResID);
}

#if !SHENG_ZE
HBITMAP CStatusMgr::GetUSBDiskBitmap()
{
	int nResID = DSM_BMP_TOPBAR_DCON_USB;

	if(DSM_STATU_CONNECT_UCARD == CStatusMgr::GetUSBDisk())
	{
		nResID = DSM_BMP_TOPBAR_CON_USB;
	}

	return dsm_bmpmgr::Get(nResID);
}
#endif

/**
 *      根据电量值返回相应的电量档值
 *
 *  \return
 *      int         返回0，10，40，60，80，100之一
 *  \note
 *      电量有100，80，60，40，10，0共六档
 *      <=0     --> 0           关机
 *      1~10    --> 10          充电
 *      11~40   --> 40
 *      41~60   --> 60
 *      61~80   --> 80
 *      >=81~100--> 100
 */
static int g_GetRangeByBattery(int nBattery)
{
    if(nBattery <= 0)
    {
        return BATTERY_0P;
    }
    else if(nBattery < 11)
    {
        return BATTERY_10P;
    }
    else if(nBattery < 41)
    {
        return BATTERY_40P;
    }
    else if(nBattery < 61)
    {
        return BATTERY_60P;
    }
    else if(nBattery < 81)
    {
        return BATTERY_80P;
    }

    return BATTERY_100P;
}

HBITMAP CStatusMgr::GetBatteryBitmap()
{
    // 获取电量值相应电量状态的图片资源ID
    int nResID;
    if(!getInstance().m_mapBatteryResID.Lookup(g_GetRangeByBattery(CStatusMgr::GetBattery()), nResID))
    {
        nResID = DSM_BMP_TOPBAR_BATTERY_0;
    }

    // 电量从0~100%动态增长
    static int nMod = 0;

    if(CStatusMgr::GetCharge())
    {
    
		if(nMod > DSM_BMP_TOPBAR_BATTERY_PW_100 - DSM_BMP_TOPBAR_BATTERY_PW_0)
        {
            nMod = 0;
        }
        nResID = DSM_BMP_TOPBAR_BATTERY_PW_0 + nMod++;

    }

    return dsm_bmpmgr::Get(nResID);
}

void CStatusMgr::Snapshot()
{
    CRect rcClient;
    CWnd* pWnd = AfxGetMainWnd();

    pWnd->GetClientRect(rcClient);
    pWnd->ClientToScreen(rcClient);

    HDC hAppDC;
    HDC hMemoryDC;

    hAppDC      = CreateDC(L"DISPLAY", NULL, NULL, NULL);
    hMemoryDC   = CreateCompatibleDC(hAppDC);

    int nWidth  = GetDeviceCaps(hAppDC, HORZRES);
    int nHeight = GetDeviceCaps(hAppDC, VERTRES);

    nWidth = nWidth > rcClient.Width() ? rcClient.Width() : nWidth;
    nHeight = nHeight > rcClient.Height() ? rcClient.Height() : nHeight;

    m_hSnapPanel        = CreateCompatibleBitmap(hAppDC, nWidth, nHeight);
    HBITMAP hOldPanel   = (HBITMAP)SelectObject(hMemoryDC, m_hSnapPanel);

    BitBlt(hMemoryDC, 0, 0, nWidth, nHeight, hAppDC, rcClient.left, rcClient.top, SRCCOPY);

    m_hSnapPanel = (HBITMAP)SelectObject(hMemoryDC, hOldPanel);

    DeleteDC(hAppDC);
    DeleteDC(hMemoryDC);
}

void CStatusMgr::ClearSnapshot()
{
    if(m_hSnapPanel)
    {
        DeleteObject(m_hSnapPanel);
        m_hSnapPanel = NULL;
    }
}


bool CStatusMgr::SaveSnapshot( CString csFile )
{
    if(!m_hSnapPanel)
    {
        DSM_DEBUG(L"dsm: svaesnapshot: m_hSnapPanel null handle");
        return false;
    }

    if(csFile.IsEmpty())
    {
        SYSTEMTIME st;
        GetLocalTime(&st);

        csFile.Format(L"%04d%02d%02d_%02d%02d%02d_%03d.bmp", 
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond,
            st.wMilliseconds);
    }

    // 获取SD目录

    CString csSDPath;

    if(!DSM_GetSDDriver(csSDPath))
    {
        // SD卡未插入
        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_SD_NOT_FOUND_SNAP));
        return false;
    }

    //CString csPath = csSDPath + DSM_SNAPSHOT_FOLDER + csFile;
	CString csPath = csSDPath  + csFile;

    bool bRet = false;
    PBITMAPINFO pInfo = _createBitmapInfoStruct();
    if(pInfo)
    {
        bRet = _createBMPFile(csPath, pInfo);
    }

    CString csMsg;

    if(bRet)
    {
        csMsg.Format(dsm_stringMgr::Get(DSM_STR_INFO_SNAPSHOT_SAVE_PATH), csFile);
        SetTips(csMsg);
    }
    else
    {
        csMsg.Format(dsm_stringMgr::Get(DSM_STR_INFO_SNAPSHOT_SAVE_SAVE_ERROR), csFile);
        SetTips(csMsg);
    }

    return bRet;
}

PBITMAPINFO CStatusMgr::_createBitmapInfoStruct()
{
    BITMAP bmp;
    PBITMAPINFO pbmi;
    WORD    cClrBits;

    // Retrieve the bitmap color format, width, and height.
    if (!GetObject(m_hSnapPanel, sizeof(BITMAP), (LPSTR)&bmp))
    {
        return NULL;
    }

    // Convert the color format to a count of bits.
    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
    if (cClrBits == 1)
        cClrBits = 1;
    else if (cClrBits <= 4)
        cClrBits = 4;
    else if (cClrBits <= 8)
        cClrBits = 8;
    else if (cClrBits <= 16) 
        cClrBits = 16;
    else if (cClrBits <= 24) 
        cClrBits = 24;
    else cClrBits = 32;

    // Allocate memory for the BITMAPINFO structure. (This structure
    // contains a BITMAPINFOHEADER structure and an array of RGBQUAD
    // data structures.)

    if(cClrBits < 24)
        pbmi = (PBITMAPINFO) LocalAlloc(LPTR,
        sizeof(BITMAPINFOHEADER) +
        sizeof(RGBQUAD) * (1<< cClrBits));

    // There is no RGBQUAD array for these formats: 24-bit-per-pixel or 32-bit-per-pixel

    else
        pbmi = (PBITMAPINFO) LocalAlloc(LPTR,
        sizeof(BITMAPINFOHEADER));

    // Initialize the fields in the BITMAPINFO structure.

    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pbmi->bmiHeader.biWidth = bmp.bmWidth;
    pbmi->bmiHeader.biHeight = bmp.bmHeight;
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes;
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel;
    if(cClrBits < 24)
        pbmi->bmiHeader.biClrUsed = (1<<cClrBits);

    // If the bitmap is not compressed, set the BI_RGB flag.
    pbmi->bmiHeader.biCompression = BI_RGB;

    // Compute the number of bytes in the array of color
    // indices and store the result in biSizeImage.
    // The width must be DWORD aligned unless the bitmap is RL
    // compressed.
    pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) /8
        * pbmi->bmiHeader.biHeight;
    // Set biClrImportant to 0, indicating that all of the
    // device colors are important.
    pbmi->bmiHeader.biClrImportant = 0;

    return pbmi;
}

bool CStatusMgr::_createBMPFile( CString csFile, PBITMAPINFO pbi )
{
    HANDLE hf;                  // file handle
    BITMAPFILEHEADER hdr;       // bitmap file-header
    PBITMAPINFOHEADER pbih;     // bitmap info-header
    LPBYTE lpBits;              // memory pointer
    DWORD dwTotal;              // total count of bytes
    DWORD cb;                   // incremental count of bytes
    BYTE *hp;                   // byte pointer
    DWORD dwTmp;
    CString csMsg;
    bool bRet = false;

    pbih = (PBITMAPINFOHEADER) pbi;
    lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

    if(!lpBits)
    {
        return false;
    }

    CRect rcClient;

    HDC hDC = GetDC(AfxGetMainWnd()->GetSafeHwnd());
    int nWidth  = GetDeviceCaps(hDC, HORZRES);
    int nHeight = GetDeviceCaps(hDC, VERTRES);
    AfxGetMainWnd()->GetClientRect(rcClient);

    nWidth  = nWidth > rcClient.Width() ? rcClient.Width() : nWidth;
    nHeight = nHeight > rcClient.Height() ? rcClient.Height() : nHeight;

    HDC hMemDC = CreateCompatibleDC(hDC);
    HGDIOBJ hOriObj = SelectObject(hMemDC, m_hSnapPanel);

    HDC hMemDIBDC = CreateCompatibleDC(hDC);
    HBITMAP hDIBPanel = CreateDIBSection(hMemDIBDC, pbi,DIB_RGB_COLORS,(void **)&lpBits,NULL,0);
    HGDIOBJ hOriDIBObj = SelectObject(hMemDIBDC, hDIBPanel);
    BitBlt(hMemDIBDC, 0, 0, nWidth, nHeight, hMemDC, 0, 0, SRCCOPY);

    // Create the .BMP file.
    hf = CreateFile(csFile, 
        GENERIC_READ | GENERIC_WRITE, 
        (DWORD) 0, 
        NULL, 
        CREATE_ALWAYS, 
        FILE_ATTRIBUTE_NORMAL, 
        (HANDLE) NULL); 
    if(hf == INVALID_HANDLE_VALUE)
    {
        csMsg.Format(dsm_stringMgr::Get(DSM_STR_INFO_SNAPSHOT_SAVE_CREATE_ERROR), csFile);
        SetTips(csMsg);
        DSM_DEBUG(L"dsm: _createBMPFile: CreateFile: error 0x%08X.\n", GetLastError());
        goto Exit_createBMPFile;
    }

    hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"
    // Compute the size of the entire file.
    hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + 
        pbih->biSize + pbih->biClrUsed 
        * sizeof(RGBQUAD) + pbih->biSizeImage);
    hdr.bfReserved1 = 0;
    hdr.bfReserved2 = 0;

    // Compute the offset to the array of color indices.
    hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + 
        pbih->biSize + pbih->biClrUsed 
        * sizeof (RGBQUAD); 

    // Copy the BITMAPFILEHEADER into the .BMP file.
    if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), 
        (LPDWORD) &dwTmp,  NULL)) 
    {
        DSM_DEBUG(L"dsm: _createBMPFile: WriteFile: error 0x%08X.\n", GetLastError());
        goto Exit_createBMPFile;
    }

    // Copy the BITMAPINFOHEADER and RGBQUAD array into the file.
    if (!WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER) 
        + pbih->biClrUsed * sizeof (RGBQUAD), 
        (LPDWORD) &dwTmp, ( NULL)))
    {
        DSM_DEBUG(L"dsm: _createBMPFile: WriteFile: error 0x%08X.\n", GetLastError());
        goto Exit_createBMPFile;
    }

    // Copy the array of color indices into the .BMP file.
    dwTotal = cb = pbih->biSizeImage;
    hp = lpBits;
    if (!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL))
    {
        DSM_DEBUG(L"dsm: _createBMPFile: WriteFile: error 0x%08X.\n", GetLastError());
        goto Exit_createBMPFile;
    }

    // Close the .BMP file.
    if(!CloseHandle(hf))
    {
        DSM_DEBUG(L"dsm: _createBMPFile: CloseHandle: error 0x%08X.\n", GetLastError());
        goto Exit_createBMPFile;
    }

    bRet = true;

Exit_createBMPFile:

    // Free memory
    GlobalFree((HGLOBAL)lpBits);

    SelectObject(hMemDC, hOriObj);
    SelectObject(hMemDIBDC, hOriDIBObj);
    DeleteObject(hDIBPanel);
    DeleteDC(hMemDC);
    DeleteDC(hMemDIBDC);
    DeleteDC(hDC);

    return bRet;
}

bool CStatusMgr::IsAuthorized()
{
    return getInstance().m_bAuthority;
}


VOID CALLBACK DSM_AUTHORITY_TIMER(HWND, UINT, UINT, DWORD)
{
    CStatusMgr::ExpireAuthority();
}

bool CStatusMgr::GetAuthority( CString csPassword, unsigned int nTimeCycle /*= 120000*/ )
{
    if(!dsm_config::VerifyPasswrod(csPassword))
    {
        getInstance().m_bAuthority = false;
        ::KillTimer(NULL, getInstance().m_nTimerAuth);
        getInstance().m_nTimerAuth = 0;

        return false;
    }

    getInstance().m_bAuthority = true;

    getInstance().m_nTimerAuth = ::SetTimer(NULL, 0, nTimeCycle, DSM_AUTHORITY_TIMER);

    return true;
}

void CStatusMgr::ExpireAuthority()
{
    if(getInstance().m_bAuthority)
    {
        getInstance().m_bAuthority = false;

        ::KillTimer(NULL, getInstance().m_nTimerAuth);

        getInstance().m_nTimerAuth = 0;
    }
}


void CStatusMgr::SetShowInputMode( bool bShow /*= true*/ )
{
    getInstance().m_bShowInputMode = bShow;

    ((CDsmDlg*)AfxGetMainWnd())->_InvalidateStatusRect(DSM_RCTOP_RIGHTICON);
}

bool CStatusMgr::GetShowInputMode()
{
    return getInstance().m_bShowInputMode;
}



// 初始化
CShareMemory CStatusMgr::m_sfileMapping;

bool CStatusMgr::GetDSM800APPInfo( DSM800AppInfo* retInfo )
{
    ASSERT(retInfo);

    static DWORD dwRet = m_sfileMapping.Open(DSM800_APPINFOEXCHG_MAPPING, sizeof(DSM800AppInfo));

    if(0 == dwRet)
    {
        false;
    }

    return m_sfileMapping.Read(retInfo, sizeof(DSM800AppInfo)) == TRUE ? true : false;
}

APP_KBInputMode CStatusMgr::GetDSM800InputMode()
{
    DSM800AppInfo info;

    if(GetDSM800APPInfo(&info))
    {
        return info.inputMode;
    }

    return APP_KB_MODE_UNDEF;
}

BYTE CStatusMgr::GetBrightness()
{
    return (BYTE)dsm_config::GetBrightness();
}

void CStatusMgr::SetBrightness( BYTE byteBrightness )
{
    if(getInstance().m_hBrightFile == INVALID_HANDLE_VALUE)
    {
        getInstance().m_hBrightFile = CreateFile(L"BKL1:", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, NULL, NULL);

        if(getInstance().m_hBrightFile == INVALID_HANDLE_VALUE)
        {
            RETAILMSG(true, (L"dsm: statusmgr: Open brightness device Failed\n"));
            return;
        }
        else
        {
            RETAILMSG(true, (L"dsm: statusmgr: Open brightness device Success\n"));
        }
    }

    DeviceIoControl(getInstance().m_hBrightFile, IOCTL_BKL_SETDUTYCYCLE, &byteBrightness, sizeof(BYTE), NULL, NULL, NULL, NULL);
    DeviceIoControl(getInstance().m_hBrightFile, IOCTL_BKL_SETDUTYCYCLEREG, &byteBrightness, sizeof(BYTE), NULL, NULL, NULL, NULL);

    dsm_config::SetBrightness(byteBrightness);
}

DWORD CStatusMgr::GetBrightTime()
{
    return (DWORD)dsm_config::GetBrightTime();
}

void CStatusMgr::SetBrightTime( DWORD dwTime )
{
    //_setDefaultBKLight(dwTime);

    HKEY hKey;
    if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, L"ControlPanel\\Backlight", 0, 0, &hKey))
    {
        ASSERT(FALSE);
        return ;
    }

    // 设置系统背光时间
    VERIFY(ERROR_SUCCESS == RegSetValueEx(hKey, L"ACTimeout", 0, REG_DWORD, (LPBYTE)&dwTime, sizeof(dwTime)));
    VERIFY(ERROR_SUCCESS == RegSetValueEx(hKey, L"BatteryTimeout", 0, REG_DWORD, (LPBYTE)&dwTime, sizeof(dwTime)));

    RegCloseKey(hKey);

    // 重新时能电源管理驱动加载超时时间设置
    HANDLE hevReloadActivityTimeouts = OpenEvent(EVENT_ALL_ACCESS, FALSE, _T("PowerManager/ReloadActivityTimeouts"));
    if (hevReloadActivityTimeouts)
    {
        SetEvent(hevReloadActivityTimeouts);
        CloseHandle(hevReloadActivityTimeouts);
    }
    else
    {
        RETAILMSG(true, (L"dsm: modifier brighttime failed. Open \"PowerManager/ReloadActivityTimeouts\" Event Failed\r\n"));
    }

   dsm_config::SetBrightTime(dwTime);
}

statusmgr::SyncMode CStatusMgr::GetSyncMode()
{
    using namespace statusmgr;
    SyncMode eRet;

    switch(CDsmDataLayer::getInstance()->GetFpgaTimeSyncMode())
    {
    case FPGA_TIME_SYNC_IEEE1588_A:
    case FPGA_TIME_SYNC_IEEE1588_B:
    case FPGA_TIME_SYNC_IEEE1588_C:
        eRet = Mode_1588;
        break;

    case FPGA_TIME_SYNC_IRIGB_POS:
        eRet = Mode_IRIGB;
        break;

    case FPGA_TIME_SYNC_IRIGB_NEG:
        eRet = Mode_IRIGBN;
        break;

    default:
        eRet = Mode_UNSYNC;
        break;
    }

    return eRet;
}

/**
 *      设置默认降低背光
 */
// void CStatusMgr::_setDefaultBKLight( DWORD dwTime /*= 60*/ )
// {
//     HKEY hKey;
//     if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_CURRENT_USER, L"ControlPanel\\Backlight", 0, 0, &hKey))
//     {
//         ASSERT(FALSE);
//         return ;
//     }
// 
//     DWORD dwType;
//     DWORD dwValue1, dwValue2;
//     DWORD dwLen1(sizeof(dwValue1)), dwLen2(sizeof(dwValue2));
// 
//     // 获取用户空闲时间
//     VERIFY(ERROR_SUCCESS == RegQueryValueEx(hKey, L"BatteryTimeout", 0, &dwType, (LPBYTE)&dwValue1, &dwLen1));
//     VERIFY(ERROR_SUCCESS == RegQueryValueEx(hKey, L"ACTimeout", 0, &dwType, (LPBYTE)&dwValue2, &dwLen2));
// 
//     if(dwValue1 != dwTime || dwValue2 != dwTime)
//     {
//         dwValue1 = dwTime;
//         dwValue2 = dwTime;
// 
//         VERIFY(ERROR_SUCCESS == RegSetValueEx(hKey, L"BatteryTimeout", 0, REG_DWORD, (LPBYTE)&dwValue1, sizeof(dwValue1)));
//         VERIFY(ERROR_SUCCESS == RegSetValueEx(hKey, L"ACTimeout", 0, REG_DWORD, (LPBYTE)&dwValue2, sizeof(dwValue2)));
//     }
// 
//     RegCloseKey(hKey);
// }

bool CStatusMgr::GetCharge()
{
    ::ZeroMemory(&m_sStatus, sizeof(m_sStatus));
    GetSystemPowerStatusEx2(&m_sStatus, sizeof(SYSTEM_POWER_STATUS_EX2), FALSE);

    return (m_sStatus.BatteryFlag & BATTERY_FLAG_CHARGING) == BATTERY_FLAG_CHARGING;
}

void CStatusMgr::FixDefault()
{
    _fixBkLight();
}

void CStatusMgr::_fixBkLight()
{
    HKEY hKey;
    if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Power\\Timeouts", 0, 0, &hKey))
    {
        ASSERT(FALSE);
        return ;
    }

    DWORD dwValue = 0;

    VERIFY(ERROR_SUCCESS == RegSetValueEx(hKey, L"ACUserIdle", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(dwValue)));
    VERIFY(ERROR_SUCCESS == RegSetValueEx(hKey, L"ACSystemIdle", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(dwValue)));
    VERIFY(ERROR_SUCCESS == RegSetValueEx(hKey, L"BattUserIdle", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(dwValue)));
    VERIFY(ERROR_SUCCESS == RegSetValueEx(hKey, L"BattSystemIdle", 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(dwValue)));

    RegCloseKey(hKey);
}



