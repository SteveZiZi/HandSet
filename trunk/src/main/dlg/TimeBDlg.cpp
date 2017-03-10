/**
 * \file
 *      TimeBDlg.cpp
 *
 * \brief
 *      对时业务的实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/6/13
 */

// TimeBDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TimeBDlg.h"

#include "src/main/resourcebmp.h"
#include "src/main/mgr/statusmgr.h"
#include "src/main/utils/utility.h"
#include "src/main/utils/rnavimenu.h"
#include "src/utils/utility/dsm_utility.h"


//////////////////////////////////////////////////////////////////////////
// timetick - 对时
//
#if SU_FEI
#define DSM_TIMETICK_B_RET_SUB_CAPTION      130
#define DSM_TIMETICK_B_RET_VALUE            170
#define DSM_TIMETICK_1588_RET_MAC           200
#define DSM_TIMETICK_1588_RET_CLOCK         300
#define DSM_TIMETICK_1588_RET_PORT          100

#define DSM_TIMETICK_1588_DETAIL_ITEM       220
#define DSM_TIMETICK_1588_DETAIL_VALUE      380
#else

#define DSM_TIMETICK_B_RET_SUB_CAPTION      110
#define DSM_TIMETICK_B_RET_VALUE            136

#define DSM_TIMETICK_1588_RET_MAC           200
#define DSM_TIMETICK_1588_RET_CLOCK         243
#define DSM_TIMETICK_1588_RET_PORT          87

#define DSM_TIMETICK_1588_DETAIL_ITEM       220
#define DSM_TIMETICK_1588_DETAIL_VALUE      380

#endif

/**
 *      对时记录 - B码
*
*   \note
*       子标题、值、子标题、值
 */
class CDsmTimeTickRecord : public CELRecord
{
public:
    CDsmTimeTickRecord(CString csSubCaptionA, CString csSubCaptionB)
    {
        AddItem(new CDsmRecordItem(csSubCaptionA));
        AddItem(new CDsmRecordItem(L""));
        AddItem(new CDsmRecordItem(csSubCaptionB));
        AddItem(new CDsmRecordItem(L""));
    }

    virtual ~CDsmTimeTickRecord() {}
};


class CDsmTimeTick1588Record : public CELRecord
{
public:
    CDsmTimeTick1588Record(uint32_t nIndex, const CString csMac, const CString csClock, const CString csPort)
        : m_nIndex(nIndex)
    {
        AddItem(new CDsmRecordItem(csMac));
        AddItem(new CDsmRecordItem(csClock));
        AddItem(new CDsmRecordItem(csPort));
    }

    virtual ~CDsmTimeTick1588Record()
    {
    }

public:

    /**
     *      获取记录关联的时钟源索引
     */
    inline uint32_t _GetIndex()
    {
        return m_nIndex;
    }

private:
    uint32_t            m_nIndex;           ///< 时钟源索引
};

/**
 *      1588详细时间信息
*
*   \note
*       子标题、值
 */
class CDsmTime1588DetailRecord : public CELRecord
{
public:
    CDsmTime1588DetailRecord(const CString csItem, const CString csValue)
    {
        AddItem(new CDsmRecordItem(csItem));
        AddItem(new CDsmRecordItem(csValue));
    }

    virtual ~CDsmTime1588DetailRecord() {}
};




// CTimeBDlg 对话框

IMPLEMENT_DYNAMIC(CTimeBDlg, DSM_BASEDLG)

CTimeBDlg::CTimeBDlg(CWnd* pParent /*=NULL*/)
	: DSM_BASEDLG(CTimeBDlg::IDD, DSM_STR_TIMETICK_CAPTION_B, pParent)
    , m_eTimeMode(TIME_B)
    , m_pDataLayer(NULL)
    , m_csDetailMac(L"")
    , m_csDetailID(L"")
    , m_csPort(L"")
    , m_pRNaviMenu(NULL)
#if !SHENG_ZE
	, m_BtimeFlag(false)
#endif
{
    m_nMenuID           = IDR_MENU_TIMETICK;
}

CTimeBDlg::~CTimeBDlg()
{
    if(m_pRNaviMenu)
    {
        delete m_pRNaviMenu;
        m_pRNaviMenu = NULL;
    }
}

void CTimeBDlg::DoDataExchange(CDataExchange* pDX)
{
	DSM_BASEDLG::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTimeBDlg, DSM_BASEDLG)
    ON_COMMAND(ID_MENU_TIME_F1, &CTimeBDlg::OnMenuF1)
    ON_COMMAND(ID_MENU_TIME_F2, &CTimeBDlg::OnMenuF2)
    ON_COMMAND(ID_MENU_TIME_F6, &CTimeBDlg::OnMenuF6)
    ON_WM_TIMER()
    ON_WM_DESTROY()
    ON_NOTIFY(ELNM_OKCLICK, IDC_TIME_1588_RET, &CTimeBDlg::OnSelectClock)
END_MESSAGE_MAP()


// CTimeBDlg 消息处理程序

BOOL CTimeBDlg::OnInitDialog()
{
    CBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    m_pDataLayer = CDsmDataLayer::getInstance();
    ASSERT(m_pDataLayer);

    if(TIME_B == m_eTimeMode)
    {
        _createReportB();
    }
    else
    {
        _createReport1588();
    }

    _beginTimer();

    return FALSE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


void CTimeBDlg::_createReportB()
{
    CRect rcReport;
    GetClientRect(rcReport);
    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);

    m_reportB.Create(DSM_WINSTYLE_REPORT_DEFAULT, rcReport, this, IDC_TIME_B_RET);

    // 设置字体
    m_reportB.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    // 设置样式
    m_reportB.PostInitTemplateStyle();
    m_reportB.SetHeaderHeight(0);

    // 插入列元素
    m_reportB.InsertColumn(0, L"", EL_CENTER, DSM_TIMETICK_B_RET_SUB_CAPTION);      // 子标题
    m_reportB.InsertColumn(1, L"", EL_CENTER, DSM_TIMETICK_B_RET_VALUE);            // 值
    m_reportB.InsertColumn(2, L"", EL_CENTER, DSM_TIMETICK_B_RET_SUB_CAPTION);      // 子标题
    m_reportB.InsertColumn(3, L"", EL_CENTER, DSM_TIMETICK_B_RET_VALUE);            // 值

    // 设置选择焦点样式
    m_reportB.SetRowSelectionVisible(FALSE);
    m_reportB.SetRowFocusEnable(FALSE);

    m_reportB.InsertRecordRow(0, new CDsmTimeTickRecord(dsm_stringMgr::Get(DSM_STR_TIMETICK_DATE), dsm_stringMgr::Get(DSM_STR_TIMETICK_TIME_OFFSET)));
    m_reportB.InsertRecordRow(1, new CDsmTimeTickRecord(dsm_stringMgr::Get(DSM_STR_TIMETICK_TIME), dsm_stringMgr::Get(DSM_STR_TIMETICK_TIME_QUALITY)));
    m_reportB.InsertRecordRow(2, new CDsmTimeTickRecord(dsm_stringMgr::Get(DSM_STR_TIMETICK_LEAP_NOTICE), dsm_stringMgr::Get(DSM_STR_TIMETICK_SUMMER_NOTICE)));
    m_reportB.InsertRecordRow(3, new CDsmTimeTickRecord(dsm_stringMgr::Get(DSM_STR_TIMETICK_LEAP_TAG), dsm_stringMgr::Get(DSM_STR_TIMETICK_SUMMER_TAG)));

    m_reportB.SetFocus();
}

void CTimeBDlg::_createReport1588()
{
    CRect rcReport;
    GetClientRect(rcReport);
    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);

    m_report1588.Create(DSM_WINSTYLE_REPORT_DEFAULT, rcReport, this, IDC_TIME_1588_RET);

    // 设置字体
    m_report1588.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_report1588.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    // 设置样式
    m_report1588.PostInitTemplateStyle();

    // 插入列元素
    m_report1588.InsertColumn(0, dsm_stringMgr::Get(DSM_STR_TIMETICK_MAC_ADDR), EL_CENTER, DSM_TIMETICK_1588_RET_MAC);      // MAC地址
    m_report1588.InsertColumn(1, dsm_stringMgr::Get(DSM_STR_TIMETICK_CLOCK_ID), EL_CENTER, DSM_TIMETICK_1588_RET_CLOCK);    // 时钟ID
    m_report1588.InsertColumn(2, dsm_stringMgr::Get(DSM_STR_SCANSMV_9XRPT_PORT), EL_CENTER, DSM_TIMETICK_1588_RET_PORT);    // 端口

    // 设置选择焦点样式
    m_report1588.SetRowSelectionVisible(TRUE);
    m_report1588.SetRowFocusEnable(TRUE);

    m_report1588.SetFocus();
}

/**
 *      创建1588时钟信息报表
 */
void CTimeBDlg::_createReportTime()
{
    CRect rcReport;
    GetClientRect(rcReport);
    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);

    m_reportTime.Create(DSM_WINSTYLE_REPORT_DEFAULT, rcReport, this, IDC_TIME_1588TIME);

    // 设置字体
    m_reportTime.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_reportTime.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    // 设置样式
    m_reportTime.PostInitTemplateStyle();

    // 插入列元素
    m_reportTime.InsertColumn(0, dsm_stringMgr::Get(DSM_STR_TIMETICK_ITEM), EL_LEFT, DSM_TIMETICK_1588_DETAIL_ITEM);      // 项目
    m_reportTime.InsertColumn(1, dsm_stringMgr::Get(DSM_STR_TIMETICK_VALUE), EL_LEFT, DSM_TIMETICK_1588_DETAIL_VALUE);   // 值

    int i = 0;
    m_reportTime.InsertRecordRow(i++, new CDsmTime1588DetailRecord(dsm_stringMgr::Get(DSM_STR_TIMETICK_MAC_ADDR), L""));
    m_reportTime.InsertRecordRow(i++, new CDsmTime1588DetailRecord(dsm_stringMgr::Get(DSM_STR_TIMETICK_CLOCK_ID), L""));
    m_reportTime.InsertRecordRow(i++, new CDsmTime1588DetailRecord(dsm_stringMgr::Get(DSM_STR_SCANSMV_9XRPT_PORT), L""));
    m_reportTime.InsertRecordRow(i++, new CDsmTime1588DetailRecord(dsm_stringMgr::Get(DSM_STR_TIMETICK_DATE), L""));
    m_reportTime.InsertRecordRow(i++, new CDsmTime1588DetailRecord(dsm_stringMgr::Get(DSM_STR_TIMETICK_TIME), L""));

    // 设置选择焦点样式
    m_reportTime.SetRowSelectionVisible(FALSE);
    m_reportTime.SetRowFocusEnable(FALSE);

    m_reportTime.SetFocus();
}

void CTimeBDlg::OnMenuF1()
{
    if(TIME_B != m_eTimeMode)
    {
        m_eTimeMode = TIME_B;

        _switchReport();
    }
}

void CTimeBDlg::OnMenuF2()
{
    if(m_eTimeMode != TIME_1588)
    {
        m_eTimeMode = TIME_1588;

        _switchReport();
    }
}


void CTimeBDlg::OnMenuF6()
{
    if(m_eTimeMode == TIME_1588)
    {
        m_report1588.DeleteAllRecordRow();
        m_pDataLayer->BeginDetect(NET_RECV_TYPE_IEEE_1588);
    }
#if !SHENG_ZE
	else if(m_eTimeMode == TIME_B)
	{
		if (true != m_BtimeFlag)
		{
			m_BtimeFlag = true;
			// 开始授时
			CDsmDataLayer::getInstance()->StartIRIGBTx();
			_updateDisp();
		} 
		else
		{
			m_BtimeFlag = false;
			// 停止授时
			CDsmDataLayer::getInstance()->StopIRIGBTx();
			_updateDisp();
		}
		
	}
#endif
}


void CTimeBDlg::_switchReport()
{
    // 切换B码对时报表
    if(m_eTimeMode == TIME_B)
    {
        if(!IsWindow(m_reportB.GetSafeHwnd()))
        {
            _createReportB();
        }

        if(m_report1588.GetSafeHwnd() && m_report1588.IsWindowVisible())
        {
            m_report1588.ShowWindow(SW_HIDE);
        }
#if !SHENG_ZE
		m_BtimeFlag = false;
#endif
        m_reportB.SetFocus();
        m_reportB.ShowWindow(SW_SHOW);
    }
    // 切换1588对时报表
    else if(m_eTimeMode == TIME_1588)
    {
        if(!IsWindow(m_report1588.GetSafeHwnd()))
        {
            _createReport1588();
        }

        if(m_reportB.GetSafeHwnd() && m_reportB.IsWindowVisible())
        {
            m_reportB.ShowWindow(SW_HIDE);
        }

        m_report1588.DeleteAllRecordRow();
        m_pDataLayer->BeginDetect(NET_RECV_TYPE_IEEE_1588);
        m_report1588.SetFocus();
        m_report1588.ShowWindow(SW_SHOW);
    }
    else if(m_eTimeMode == TIME_1588_DETAIL)
    {
        if(!IsWindow(m_reportTime.GetSafeHwnd()))
        {
            _createReportTime();
        }

        if(m_reportB.GetSafeHwnd() && m_reportB.IsWindowVisible())
        {
            m_reportB.ShowWindow(SW_HIDE);
        }

        if(m_report1588.GetSafeHwnd() && m_report1588.IsWindowVisible())
        {
            m_report1588.ShowWindow(SW_HIDE);
        }

        m_reportTime.SetRowItemText(0, 1, m_csDetailMac);
        m_reportTime.SetRowItemText(1, 1, m_csDetailID);
        m_reportTime.SetRowItemText(2, 1, m_csPort);

        m_pDataLayer->SetCurrentMode(DSM_MODE_NET_RECV_1588);

        m_reportTime.ShowWindow(SW_SHOW);
    }

    _updateDisp();
}


void CTimeBDlg::_updateDisp()
{
    CString csCaption;

    if(m_pRNaviMenu)
    {
        delete m_pRNaviMenu;
        m_pRNaviMenu = NULL;
    }

    // 更新B码对时的菜单
    if(m_eTimeMode == TIME_B)
    {
        csCaption = dsm_stringMgr::Get(DSM_STR_TIMETICK_CAPTION_B);

        m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_TIME_B_SEL), NULL, NULL);
        m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_TIME_1588_UNSEL), NULL, NULL);
#if SHENG_ZE
        m_menuMain.SetMenuItemCaption(5, L"");
#else
		if (false == m_BtimeFlag)
		{
			m_menuMain.SetMenuItemCaption(5, L"开始授时");
		}
		else
		{
			m_menuMain.SetMenuItemCaption(5, L"停止授时");
		}
        m_pNaviBar->UpdateNaviMenu();
    }
#endif
    // 更新1588对时的菜单
    else if(m_eTimeMode == TIME_1588)
    {
        csCaption = dsm_stringMgr::Get(DSM_STR_TIMETICK_CAPTION_1588);

        m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_TIME_B_UNSEL), NULL, NULL);
        m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_TIME_1588_SEL), NULL, NULL);

        m_menuMain.SetMenuItemCaption(5, dsm_stringMgr::Get(DSM_STR_TIMETICK_RESCAN));
        m_pNaviBar->UpdateNaviMenu();
    }
    else if(m_eTimeMode == TIME_1588_DETAIL)
    {
        csCaption = dsm_stringMgr::Get(DSM_STR_TIMETICK_1588DETAIL);
        m_pRNaviMenu = new CRNaviMenu(this, IDR_MENU_NULL);
    }

    CStatusMgr::SetTitle(csCaption);
}


void CTimeBDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    if(nIDEvent == m_nTimerID)
    {
        _updateData();
    }

    CBaseDlg::OnTimer(nIDEvent);
}

void CTimeBDlg::_beginTimer( UINT nTimeout /*= DSM_TIMETICK_TIMER_INTERVAL*/ )
{
    // 设置对时页面的定时器
    m_nTimerID = SetTimer(DSM_TIMETICK_TIMER_ID, nTimeout, NULL);
}

void CTimeBDlg::_stopTimer()
{
    if(m_nTimerID != 0)
    {
        KillTimer(m_nTimerID);
        m_nTimerID = 0;
    }
}

void CTimeBDlg::OnDestroy()
{
    CBaseDlg::OnDestroy();

    // TODO: 在此处添加消息处理程序代码

    // 关闭定时器
    _stopTimer();
}

void CTimeBDlg::_modifyMenu()
{
    // 进入页面前设置B码对时的菜单图标样式
    if(TIME_B == m_eTimeMode)
    {
        m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_TIME_B_SEL), NULL, NULL);
        m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_TIME_1588_UNSEL), NULL, NULL);
#if !SHENG_ZE
		m_menuMain.SetMenuItemCaption(5, L"开始授时");
#endif
    }
    // 进入页面前设置1588对时的菜单图标样式
    else
    {
        m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_TIME_B_UNSEL), NULL, NULL);
        m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_TIME_1588_SEL), NULL, NULL);

        m_menuMain.SetMenuItemCaption(5, dsm_stringMgr::Get(DSM_STR_TIMETICK_RESCAN));
    }
}

void CTimeBDlg::_updateData()
{
    // 刷新B码数据
    if(m_eTimeMode == TIME_B)
    {
        _updateIRIGB();
    }
    // 刷新1588 数据
    else if(m_eTimeMode == TIME_1588)
    {
        _update1588Scan();
    }
    // 刷新1588详细信息
    else if(m_eTimeMode == TIME_1588_DETAIL)
    {
        _update1588Detail();
    }
}

void CTimeBDlg::_updateIRIGB()
{
    // 更新B码对时的数据到ui
    if(m_pDataLayer->GetTimeSyncIRIGB(&m_dataIRIGB))
    {
        CString csDate, csTime, csLsp, csLs, csDsp, csDst, csQuality, csOffset;

        SYSTEMTIME st = m_dataIRIGB.GetDateTime();

        csDate.Format(L"%04d-%02d-%02d%", st.wYear, st.wMonth, st.wDay);
        csTime.Format(L"%02d:%02d:%02d.%03d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

        csLsp.Format(L"%u", m_dataIRIGB.GetLSP());
        csLs.Format(L"%u", m_dataIRIGB.GetLS());

        csDsp.Format(L"%u", m_dataIRIGB.GetDSP());
        csDst.Format(L"%u", m_dataIRIGB.GetDST());

        csQuality.Format(L"%u", m_dataIRIGB.GetTimeQuality());
        csOffset.Format(L"%u", m_dataIRIGB.GetTimeOffset());

        m_reportB.SetRowItemText(0, 1, csDate);
        m_reportB.SetRowItemText(0, 3, csOffset);
        m_reportB.SetRowItemText(1, 1, csTime);
        m_reportB.SetRowItemText(1, 3, csQuality);
        m_reportB.SetRowItemText(2, 1, csLsp);
        m_reportB.SetRowItemText(2, 3, csDsp);
        m_reportB.SetRowItemText(3, 1, csLs);
        m_reportB.SetRowItemText(3, 3, csDst);
    }
    else
    {
        m_reportB.SetRowItemText(0, 1, g_csBlankValue);
        m_reportB.SetRowItemText(0, 3, g_csBlankValue);
        m_reportB.SetRowItemText(1, 1, g_csBlankValue);
        m_reportB.SetRowItemText(1, 3, g_csBlankValue);
        m_reportB.SetRowItemText(2, 1, g_csBlankValue);
        m_reportB.SetRowItemText(2, 3, g_csBlankValue);
        m_reportB.SetRowItemText(3, 1, g_csBlankValue);
        m_reportB.SetRowItemText(3, 3, g_csBlankValue);
    }
}


/**
 *      刷新1588详细时间信息
 */
void CTimeBDlg::_update1588Detail()
{
    if(m_reportTime.GetSafeHwnd() && m_reportTime.IsWindowVisible())
    {
        IEEE1588_TIMESTAMP info = m_pDataLayer->GetNet1588FrameTime();
        SYSTEMTIME st = DM_GetSystemTimeFromUTC(info.t_s, info.t_ns / 1000);

        CString csDate, csTime;

        csDate.Format(L"%04d-%02d-%02d", st.wYear, st.wMonth, st.wDay);
        csTime.Format(L"%02d:%02d:%02d.%03d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

        m_reportTime.SetRowItemText(3, 1, csDate);
        m_reportTime.SetRowItemText(4, 1, csTime);
    }
}

/**
 *      刷新1588探测时钟源数据
 */
void CTimeBDlg::_update1588Scan()
{
    if(!m_report1588.GetSafeHwnd() || !m_report1588.IsWindowVisible())
    {
        return;
    }

    int nCounts = (int)m_pDataLayer->GetNet1588DetectItemsCount();

    if(nCounts == m_report1588.GetRecordRowCount())
    {
        return;
    }

    CString csMac, csID, csPort;
    CNet1588DetectItem* pItem = NULL;

    for(int i = m_report1588.GetRecordRowCount(); i != nCounts; i++)
    {
        pItem = m_pDataLayer->GetNet1588DetectItem(i);

        if(pItem)
        {
            DWORD dwHigh = (DWORD)(pItem->stClockID.clockIdentity >> 32);
            DWORD dwLow  = (DWORD)(pItem->stClockID.clockIdentity & 0x00000000FFFFFFFF);

            csMac.Format(L"%02X-%02X-%02X-%02X-%02X-%02X", pItem->macAddr[0], pItem->macAddr[1], pItem->macAddr[2], pItem->macAddr[3], pItem->macAddr[4], pItem->macAddr[5]);
            csID.Format(L"V%u: 0x%08X%08X - 0x%04X",  pItem->n1588Version, dwHigh, dwLow, pItem->stClockID.sourcePortID);
        }
        else
        {
            csMac   = g_csBlankValue;
            csID    = g_csBlankValue;
        }

        csPort = DSM_GetPortDesc(pItem->enPortName);

        m_report1588.InsertRecordRow(m_report1588.GetRecordRowCount(), new CDsmTimeTick1588Record(i, csMac, csID, csPort));
    }

    if(m_report1588.GetFocusedRowIndex() > 0)
    {
        m_report1588.SetFocusedSelectionRowIndex(0);
    }
}

void CTimeBDlg::OnSelectClock( NMHDR* /*pNotifyStruct*/, LRESULT* /*pResult*/ )
{
    int nRowIndex = m_report1588.GetFocusedRowIndex();
    if (nRowIndex < 0)
    {
        return;
    }

    CDsmTimeTick1588Record* pRecord = dynamic_cast<CDsmTimeTick1588Record*>(m_report1588.GetRecordRow(nRowIndex));
    if(!pRecord)
    {
        return;
    }

    CNet1588DetectItem* pItem = m_pDataLayer->GetNet1588DetectItem(pRecord->_GetIndex());

    if(pItem)
    {
        DWORD dwHigh = (DWORD)(pItem->stClockID.clockIdentity >> 32);
        DWORD dwLow  = (DWORD)(pItem->stClockID.clockIdentity & 0x00000000FFFFFFFF);

        m_csDetailMac.Format(L"%02X-%02X-%02X-%02X-%02X-%02X", pItem->macAddr[0], pItem->macAddr[1], pItem->macAddr[2], pItem->macAddr[3], pItem->macAddr[4], pItem->macAddr[5]);
        m_csDetailID.Format(L"0x%08X%08X - 0x%04X",  dwHigh, dwLow, pItem->stClockID.sourcePortID);
        m_csPort = DSM_GetPortDesc(pItem->enPortName);

        m_pDataLayer->SetCurrentCtrlBlk(pItem);
        m_eTimeMode = TIME_1588_DETAIL;

        _switchReport();
    }
}

BOOL CTimeBDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if(m_eTimeMode == TIME_1588_DETAIL)
    {
        if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
        {
            m_eTimeMode = TIME_1588;
            m_pDataLayer->SetCurrentMode(DSM_MODE_QUIT_CURRENT_PAGE);
            _switchReport();
            return TRUE;
        }
    }
#if !SHENG_ZE
	if (true == m_BtimeFlag && TIME_B == m_eTimeMode)
	{
		if(WM_KEYDOWN == pMsg->message)
		{
			switch(pMsg->wParam)
			{
			case VK_F1:
			case VK_F2:
			case VK_ESCAPE:
				{
					CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_BTIME_ING), 3000);
					return TRUE;
				}
				break;
			}
		}
	}
#endif
    return CBaseDlg::PreTranslateMessage(pMsg);
}


