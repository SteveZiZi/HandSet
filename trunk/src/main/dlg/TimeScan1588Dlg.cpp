/**
 * \file
 *      TimeScan1588Dlg.cpp
 *
 * \brief
 *      1588时钟源探测页面实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/10/9
 */

// TimeScan1588Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TimeScan1588Dlg.h"

#include "src/main/common.h"
#include "src/main/eventproc.h"
#include "src/main/resourcestr.h"
#include "src/main/mgr/statusmgr.h"
#include "src/main/mgr/navibarmgr.h"
#include "src/utils/utility/dsm_utility.h"

//////////////////////////////////////////////////////////////////////////
// 1588时钟源探测
//
#define DSM_1588_SCAN_HEAD_CHECKBOX         48          ///< 选择时钟源栏
#define DSM_1588_SCAN_HEAD_SID              100         ///< 时钟的地址
#define DSM_1588_SCAN_HEAD_PID              100         ///< 时钟地址的端口
#define DSM_1588_SCAN_HEAD_MAC              261         ///< 时钟源MAC地址
#define DSM_1588_SCAN_HEAD_PORT             91          ///< 接收时钟源端口

#define DSM_1588_SCAN_TIMERID               1           ///< 刷新探测数据的定时器ID

/**
 *      1588时钟源探测数据记录项
 *
 *  \note
 *      check,mac,port
 */
class CDsmScan1588Record : public CELRecord
{
public:
    CDsmScan1588Record(CNet1588DetectItem* pInfo);
    virtual ~CDsmScan1588Record() {}

public:
    /**
     *      获取时钟源记录
     */
    inline CNet1588DetectItem* _GetInfo()
    {
        return m_pInfo;
    }

private:
    CNet1588DetectItem*         m_pInfo;        ///< 记录关联的时钟源信息
};


CDsmScan1588Record::CDsmScan1588Record(CNet1588DetectItem* pInfo)
: m_pInfo(pInfo)
{
    ASSERT(m_pInfo);

    CString csID;
    csID.Format(L"%ld", pInfo->stClockID.clockIdentity);

    CString csMac;
    csMac.Format(L"%02X-%02X-%02X-%02X-%02X-%02X", 
        pInfo->macAddr[0], 
        pInfo->macAddr[1], 
        pInfo->macAddr[2], 
        pInfo->macAddr[3], 
        pInfo->macAddr[4], 
        pInfo->macAddr[5]);

    CELRecordItem* pItem = AddItem(new CDsmRecordItem(L""));
    AddItem(new CDsmRecordItem(csID));
    AddItem(new CDsmRecordItem(pInfo->stClockID.sourcePortID));
    AddItem(new CDsmRecordItem(csMac));
    AddItem(new CDsmRecordItem(DSM_GetPortDesc(m_pInfo->enPortName)));

    pItem->SetEditable(TRUE);
    pItem->HasCheckbox(TRUE);
}



// CTimeScan1588Dlg 对话框

IMPLEMENT_DYNAMIC(CTimeScan1588Dlg, CDialog)

CTimeScan1588Dlg::CTimeScan1588Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTimeScan1588Dlg::IDD, pParent)
    , m_nScanTimerID(UINT_MAX)
    , m_pDataLayer(NULL)
    , m_pHwndPrev(NULL)
    , m_pMenuPrev(NULL)
    , m_pNaviBar(NULL)
{
    m_pNaviBar      = CNaviBarMgr::GetInstance()->GetNaviBar();
    m_pDataLayer    = CDsmDataLayer::getInstance();
}

CTimeScan1588Dlg::~CTimeScan1588Dlg()
{
    if(IsWindow(m_pHwndPrev))
    {
        CNaviBarMgr::GetInstance()->RestoreMenu(CWnd::FromHandle(m_pHwndPrev), m_pMenuPrev);
    }
}

void CTimeScan1588Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTimeScan1588Dlg, CDialog)
    ON_WM_TIMER()
    ON_WM_PAINT()
    ON_COMMAND(ID_MENU_SCAN_1588_F6, &CTimeScan1588Dlg::OnMenuF6)
END_MESSAGE_MAP()


// CTimeScan1588Dlg 消息处理程序

BOOL CTimeScan1588Dlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化

    CStatusMgr::SetTitle(dsm_stringMgr::Get(DSM_STR_1588D_CAPTION));

    CRect rcRect(DSM_RCBODY);
    DSM_ParentToChild(this, rcRect);

    MoveWindow(rcRect);

    m_pMenuPrev = CNaviBarMgr::GetInstance()->GetNaviBar()->GetNaviMenu();
    m_pHwndPrev = CNaviBarMgr::GetInstance()->GetNaviBar()->GetCommandTarget()->GetSafeHwnd();

    CNaviBarMgr::GetInstance()->ShowMenu(this, IDR_MENU_SCAN_1588);

    _createReport();

    m_pDataLayer->BeginDetect(NET_RECV_TYPE_IEEE_1588);

    m_nScanTimerID = SetTimer(DSM_1588_SCAN_TIMERID, 400, NULL);

    m_report1588.SetFocus();

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void CTimeScan1588Dlg::_createReport()
{
    CRect rcReport;
    GetClientRect(rcReport);
    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);

    int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

    m_report1588.Create(DSM_WINSTYLE_REPORT_DEFAULT, rcReport, this, IDC_1588_SCAN_REPORT);

    // 设置字体
    m_report1588.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_report1588.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    // 设置样式
    m_report1588.PostInitTemplateStyle();

    // 插入列元素
    m_report1588.InsertColumn(0, L"", EL_CENTER, DSM_1588_SCAN_HEAD_CHECKBOX);   // checkbox
    m_report1588.InsertColumn(1, dsm_stringMgr::Get(DSM_STR_1588D_HEAD_SID), EL_CENTER, DSM_1588_SCAN_HEAD_SID);        // SID
    m_report1588.InsertColumn(2, dsm_stringMgr::Get(DSM_STR_1588D_HEAD_PID), EL_CENTER, DSM_1588_SCAN_HEAD_PID);        // PID
    m_report1588.InsertColumn(3, dsm_stringMgr::Get(DSM_STR_1588D_HEAD_MAC), EL_CENTER, DSM_1588_SCAN_HEAD_MAC);        // MAC
    m_report1588.InsertColumn(4, dsm_stringMgr::Get(DSM_STR_1588D_HEAD_PORT), EL_CENTER, DSM_1588_SCAN_HEAD_PORT);      // PORT

    // 设置选择焦点样式
    m_report1588.SetRowSelectionVisible(TRUE);
    m_report1588.SetRowFocusEnable(TRUE);
    m_report1588.SetSubItemFocusEnable(TRUE);
    m_report1588.SetFreezeSelectionVisible(TRUE);
    m_report1588.SetFocusedColIndex(0);
}

void CTimeScan1588Dlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    if(nIDEvent == m_nScanTimerID)
    {
        _insertRecord();
    }
    CDialog::OnTimer(nIDEvent);
}

void CTimeScan1588Dlg::_rescan()
{
    // 清楚之前探测信息
    _clearRes();

    m_pDataLayer->BeginDetect(NET_RECV_TYPE_IEEE_1588);
}

void CTimeScan1588Dlg::_clearSelect()
{
    CELRecordItem* pItem    = NULL;

    // 清空选中的控制块
    int nCounts = m_report1588.GetRecordRowCount();
    for(int i = 0; i < nCounts; i++)
    {
        pItem = m_report1588.GetRecordRow(i)->GetItem(0);

        if(pItem->IsChecked())
        {
            pItem->SetChecked(FALSE);
            m_report1588.RedrawRowItem(i, 0);
        }
    }
}


void CTimeScan1588Dlg::_clearRes()
{
    if(m_report1588.GetSafeHwnd())
    {
        m_report1588.DeleteAllRecordRow();
    }
}

void CTimeScan1588Dlg::_insertRecord()
{
    unsigned int nCounts        = m_report1588.GetRecordRowCount();
    unsigned int nDetectCounts  = m_pDataLayer->GetNet1588DetectItemsCount();

    // 若无新的探测结果则返回
    if(nDetectCounts == nCounts || !m_report1588.GetSafeHwnd())
    {
        return;
    }

    CNet1588DetectItem* pInfo= NULL;

    // 插入新的记录
    for(unsigned int i = nCounts; i < nDetectCounts; i++)
    {
        pInfo = m_pDataLayer->GetNet1588DetectItem(i);

        if(!pInfo)
        {
            TRACE(L"dsm: get 1588 clock source error\n");
            continue;
        }

        m_report1588.InsertRecordRow(i, new CDsmScan1588Record(pInfo));

        for(int nColumn = 1; nColumn < 5; nColumn++)
        {
            m_report1588.GetRecordRow(i)->GetItem(nColumn)->SetFocusable(FALSE);
        }

        if(0 == i)
        {
            m_report1588.SetFocusedSelectionRowIndex(0);
        }
    }
}

void CTimeScan1588Dlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO: 在此处添加消息处理程序代码
    // 不为绘图消息调用 CDialog::OnPaint()

    CRect rcClient;
    GetClientRect(rcClient);

    dc.FillSolidRect(rcClient, DSM_COLOR_BK);
}

BOOL CTimeScan1588Dlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类

    // 传给导航菜单处理消息
    
    if (m_pNaviBar && m_pNaviBar->PreTranslateMessage(pMsg))
    {
        return TRUE;
    }

    if(pMsg->message == WM_KEYDOWN)
    {
        switch(pMsg->wParam)
        {
        case DSM_KEY_HOME:
            if(CEventProc::GetInstance()->OnKeyHome() == DSM_HOME_RETVAL_MAIN)
            {
                EndDialog(IDCANCEL);
            }
            return TRUE;

        case DSM_KEY_SNAPSHOT:
            CEventProc::GetInstance()->SnapShot();
            return TRUE;

        default:
            break;
        }
    }

    return CDialog::PreTranslateMessage(pMsg);
}

void CTimeScan1588Dlg::OnMenuF6()
{
    CELRecordItem* pItem    = NULL;

    // 清空选中的控制块
    int nCounts = m_report1588.GetRecordRowCount();
    for(int i = 0; i < nCounts; i++)
    {
        pItem = m_report1588.GetRecordRow(i)->GetItem(0);

        if(pItem->IsChecked())
        {
            //CDsmScan1588Record* pRecord = dynamic_cast<CDsmScan1588Record*>(m_report1588.GetRecordRow(i));
#pragma message(__FILE__ "(" DSM_STRING(__LINE__) "): 设置1588时钟源")
        }
    }

    EndDialog(IDOK);
}
