/**
 * \file
 *      SMVMonitorDlg.cpp
 *
 * \brief
 *      监视页面实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/4/21
 */

// SMVMonitorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SMVMonitorDlg.h"

#include "src/main/eventproc.h"
#include "src/main/mgr/statusmgr.h"
#include "src/main/smv/smvdualphasedlg.h"
#include "src/utils/utility/dsm_utility.h"

#if SU_FEI
//////////////////////////////////////////////////////////////////////////
// 状态监视
#define DSM_MONITOR_RCSTATUS                CRect(0, 390, 640, DSM_P_BODY_BOTTOM)     ///< 92底部MU状态区域
#define DSM_MONITOR_RCSTATUS_92_MU          CRect(470, 0, 620, 24)      ///< 92底部MU状态区域
#define DSM_MONITOR_RCSTATUS_FT3_WARN       CRect(20, 0, 170, 24)       ///< FT3底部告警状态区域
#define DSM_MONITOR_RCSTATUS_FT3_FIX        CRect(170, 0, 320, 24)      ///< FT3底部检修状态区域
#define DSM_MONITOR_RCSTATUS_FT3_WAKE       CRect(320, 0, 470, 24)      ///< FT3底部唤醒状态区域
#define DSM_MONITOR_RCSTATUS_FT3_MU         CRect(470, 0, 620, 24)      ///< FT3底部MU状态区域

#define DSM_MONITOR_92_INDEX                91          ///< 索引
#define DSM_MONITOR_92_DESC                 145         ///< 描述
#define DSM_MONITOR_92_STATUS               91          ///< 状态
#define DSM_MONITOR_92_FIX                  91          ///< 检修
#define DSM_MONITOR_92_COMPOUND             91          ///< 合成
#define DSM_MONITOR_92_QUALITY              91          ///< 品质

#define DSM_MONITOR_FT3_INDEX               182         ///< 索引
#define DSM_MONITOR_FT3_DESC                236         ///< 描述
#define DSM_MONITOR_FT3_STATUS              182         ///< 状态




#define DSM_SMV_MONITOR_PROTO_UNKNOWN       0x00            ///< 未知
#define DSM_SMV_MONITOR_PROTO_92            0x01            ///< 92
#define DSM_SMV_MONITOR_PROTO_91FT3         0x02            ///< 91 FT3(国网，南电)


#define DRAWTEXT_MODE       DT_VCENTER | DT_CENTER | DT_SINGLELINE




/**
 *      SMV接收状态监视  92 通道状态记录
 *
 *  \note
 *      通道序号、描述、状态、检修、合成量、品质
 */
class CDsmSmvMonitor92Record : public CELRecord
{
public:
    CDsmSmvMonitor92Record(CSmvRecvChannelInfo* pInfo);

    virtual ~CDsmSmvMonitor92Record() {}

public:
    /**
     *      更新通道状态信息
     */
    void Update(CStatusWatchChannelInfo info);
};

/**
 *      SMV接收状态监视  91 FT3 通道状态记录
 *
 *  \note
 *      通道序号、描述、状态
 */
class CDsmSmvMonitorFT3Record : public CELRecord
{
public:
    CDsmSmvMonitorFT3Record(CSmvRecvChannelInfo* pInfo);

    virtual ~CDsmSmvMonitorFT3Record() {}

public:
    /**
     *      更新通道状态信息
     */
    void Update(CStatusWatchChannelInfo info);
};



CDsmSmvMonitor92Record::CDsmSmvMonitor92Record( CSmvRecvChannelInfo* pInfo )
{
    if(pInfo)
    {
        AddItem(new CDsmRecordItem(pInfo->nIndex + 1));
        AddItem(new CDsmRecordItem(pInfo->strChannelDesc.c_str()));
        AddItem(new CDsmRecordItem(g_csBlankValue));
        AddItem(new CDsmRecordItem(g_csBlankValue));
        AddItem(new CDsmRecordItem(g_csBlankValue));
        AddItem(new CDsmRecordItem(g_csBlankValue));
    }
}

void CDsmSmvMonitor92Record::Update( CStatusWatchChannelInfo info )
{
    CString csQuality;
    CString csStatus    = dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_EFFECTIVE);
    CString csFix       = dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_YES);
    CString csCompound  = dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_YES);

    csQuality.Format(L"0x%04X", info.m_nQuality);

    if(!info.m_bChannelValid)
    {
        csStatus = dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_NOTEFFECTIVE);
    }

    if(!info.m_bChannelTestFlag)
    {
        csFix = dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_NO);
    }

    if(!info.m_bChannelDerived)
    {
        csCompound = dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_NO);
    }

    GetItem(2)->SetCaption(csStatus);
    GetItem(3)->SetCaption(csFix);
    GetItem(4)->SetCaption(csCompound);
    GetItem(5)->SetCaption(csQuality);
}

CDsmSmvMonitorFT3Record::CDsmSmvMonitorFT3Record( CSmvRecvChannelInfo* pInfo )
{
    if(pInfo)
    {
        AddItem(new CDsmRecordItem(pInfo->nIndex + 1));
        AddItem(new CDsmRecordItem(pInfo->strChannelDesc.c_str()));
        AddItem(new CDsmRecordItem(g_csBlankValue));
    }
}

void CDsmSmvMonitorFT3Record::Update( CStatusWatchChannelInfo info )
{
    if(info.m_bChannelValid)
    {
        GetItem(2)->SetCaption(dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_EFFECTIVE));
    }
    else
    {
        GetItem(2)->SetCaption(dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_NOTEFFECTIVE));
    }
}




// CSMVMonitorDlg 对话框

IMPLEMENT_DYNAMIC(CSMVMonitorDlg, DSM_SMVBASEDLG)

CSMVMonitorDlg::CSMVMonitorDlg(CWnd* pParent /*=NULL*/)
	: DSM_SMVBASEDLG(CSMVMonitorDlg::IDD, DSM_STR_SMV_MONITOR_CAPTION, pParent)
    , m_dwTag(DSM_SMV_MONITOR_PROTO_UNKNOWN)
    , m_nBegin(0)
    , m_nEnd(0)
    , m_rc92MU(DSM_MONITOR_RCSTATUS_92_MU)
    , m_rcFT3MU(DSM_MONITOR_RCSTATUS_FT3_MU)
    , m_rcFT3Warn(DSM_MONITOR_RCSTATUS_FT3_WARN)
    , m_rcFT3Fix(DSM_MONITOR_RCSTATUS_FT3_FIX)
    , m_rcFT3Wake(DSM_MONITOR_RCSTATUS_FT3_WAKE)
    , m_rcStatus(DSM_MONITOR_RCSTATUS)
    , m_ePage(PAGE_NORMAL)
    , m_pCurrentReport(NULL)
    , m_pCurCtrlBlk(NULL)
    , m_pCtrlBlk1(NULL)
    , m_pCtrlBlk2(NULL)
    , m_modeCtrl(CTRL_ONE)
{
    m_nMenuID = IDR_MENU_SMV_MONITOR;
}

CSMVMonitorDlg::~CSMVMonitorDlg()
{
    m_panelStatus.DeleteObject();

    CStatusMgr::SetTips(L"", 0);
}

void CSMVMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	DSM_SMVBASEDLG::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSMVMonitorDlg, DSM_SMVBASEDLG)
    ON_WM_TIMER()
    ON_COMMAND(ID_SMV_MONITOR_F1, &CSMVMonitorDlg::OnMenuF1)
END_MESSAGE_MAP()


// CSMVMonitorDlg 消息处理程序


BOOL CSMVMonitorDlg::OnInitDialog()
{
    DSM_SMVBASEDLG::OnInitDialog();

    // TODO:  在此添加额外的初始化

    CPageData* m_pPageData = CEventProc::GetInstance()->GetPreviousPageData();
    ASSERT(m_pPageData);

    // 核相页面
    if(m_pPageData && m_pPageData->m_nPageID == CSMVDualphaseDlg::IDD)
    {
        m_ePage = PAGE_DUALPHASE;

        CPageDataExtraDualPhase* pPageData = dynamic_cast<CPageDataExtraDualPhase*>(m_pPageData->m_pPageDataExtra);
        ASSERT(pPageData);

        m_pCtrlBlk1 = m_pDataLayer->GetSelectCtrlBlkByID(pPageData->nSmv1);
        m_pCtrlBlk2 = m_pDataLayer->GetSelectCtrlBlkByID(pPageData->nSmv2);

        m_pCurCtrlBlk = m_pCtrlBlk1;
    }
    else
    {
        m_pCtrlBlk1     = m_pCurrentCtrlBlk;
        m_pCurCtrlBlk   = m_pCtrlBlk1;
    }

    // 更新菜单
    _updateMenu();

    if(m_pCurCtrlBlk)
    {
        // 设置当前控制块的协议模式

        // 92协议
        if(FRAME_TYPE_NET_9_2 == m_pCurCtrlBlk->enFrameType)
        {
            m_dwTag = DSM_SMV_MONITOR_PROTO_92;
        }
        // FT3与91
        else if(FRAME_TYPE_NET_9_1 == m_pCurCtrlBlk->enFrameType || 
            FRAME_TYPE_FT3_NANRUI == m_pCurCtrlBlk->enFrameType || 
            FRAME_TYPE_FT3_GUOWANG == m_pCurCtrlBlk->enFrameType)
        {
            m_dwTag = DSM_SMV_MONITOR_PROTO_91FT3;
        }
        else
        {
            m_dwTag = DSM_SMV_MONITOR_PROTO_UNKNOWN;
        }

        m_pDataLayer->SetCurrentMode(DSM_MODE_SMV_RECV_STATUS_WATCH);

        _beginTimer();
    }

    _updateCtrlBlkCaption();

    m_pCurrentReport = &m_reportChannel;

    _createReport();

    m_pCurrentReport->SetFocus();

    m_csMUSync.Format(L"%s：%s", dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_MU), dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_SYNC));
    m_csMUNotSync.Format(L"%s：%s", dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_MU), dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_UNSYNC));
    m_csWarnGood.Format(L"%s：%s", dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_WARN), dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_YES));
    m_csWarnBad.Format(L"%s：%s", dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_WARN), dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_NO));
    m_csFixConn.Format(L"%s：%s", dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_FIX), dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_YES));
    m_csFixTest.Format(L"%s：%s", dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_FIX), dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_NO));
    m_csWakeValid.Format(L"%s：%s", dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_WAKE), dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_YES));
    m_csWakeInvalid.Format(L"%s：%s", dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_WAKE), dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_NO));

    // 空数据
    m_csMUNULL.Format(L"%s：%s", dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_MU), g_csBlankValue);
    m_csWarnNULL.Format(L"%s：%s", dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_WARN), g_csBlankValue);
    m_csFixNULL.Format(L"%s：%s", dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_FIX), g_csBlankValue);
    m_csWakeNULL.Format(L"%s：%s", dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_WAKE), g_csBlankValue);


    return FALSE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

/**
 *      更新底部状态信息
 */
void CSMVMonitorDlg::_updateStatus( bool bNull /* = false */ )
{
    CString csMU(m_csMUNULL), csWarn(m_csWarnNULL), csFix(m_csFixNULL), csWakeup(m_csWakeNULL);

    if(!bNull)
    {
        csWarn      = (m_data.GetFt3FaultAlarm() == 0) ? m_csWarnGood : m_csWarnBad;
        csFix       = (m_data.GetFt3TestFlag() == 0) ? m_csFixConn : m_csFixTest;
        csWakeup    = (m_data.GetFt3WakeupValid() == 0) ? m_csWakeValid : m_csWakeInvalid;
        csMU        = m_data.GetMuIsSync() ? m_csMUSync : m_csMUNotSync;
    }

    CDC* pDC = GetDC();

    CDC dcMem;
    dcMem.CreateCompatibleDC(pDC);
    CRect rcStatus(DSM_MONITOR_RCSTATUS);

    if(!m_panelStatus.GetSafeHandle())
    {
        m_panelStatus.CreateCompatibleBitmap(pDC, rcStatus.Width(), rcStatus.Height());
    }

    int nOri = dcMem.SaveDC();

    dcMem.SelectObject(m_panelStatus);
    dcMem.SetBkMode(TRANSPARENT);
    dcMem.SetTextColor(ELT_T1_TEXTCOLOR);
    dcMem.SelectObject(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    dcMem.FillSolidRect(0, 0, rcStatus.Width(), rcStatus.Height(), ELT_T1_BKCOLOR);

    if(DSM_SMV_MONITOR_PROTO_92 == m_dwTag)
    {
        dcMem.DrawText(csMU, m_rc92MU, DRAWTEXT_MODE);
    }
    else if(DSM_SMV_MONITOR_PROTO_91FT3 == m_dwTag)
    {
        dcMem.DrawText(csWarn, m_rcFT3Warn, DRAWTEXT_MODE);
        dcMem.DrawText(csFix, m_rcFT3Fix, DRAWTEXT_MODE);
        dcMem.DrawText(csWakeup, m_rcFT3Wake, DRAWTEXT_MODE);
        dcMem.DrawText(csMU, m_rcFT3MU, DRAWTEXT_MODE);
    }

    pDC->BitBlt(0, m_rcStatus.top, rcStatus.Width(), rcStatus.Height(), &dcMem, 0, 0, SRCCOPY);
    dcMem.RestoreDC(nOri);
    dcMem.DeleteDC();
    pDC->DeleteDC();
}

/**
 *      创建通道状态报表
 */
void CSMVMonitorDlg::_createReport()
{
    ASSERT(!m_pCurrentReport->GetSafeHwnd());

    CRect rcClient;
    GetClientRect(rcClient);

    rcClient.DeflateRect(DSM_MARGIN_IN, DSM_MARGIN_IN, DSM_MARGIN_IN, m_rcStatus.Height());

    int nCounts = (rcClient.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcClient.bottom = rcClient.top + ELT_T1_HDRHEIGHT + ELT_T1_ROWHEIGHT * nCounts;

    m_rcStatus.DeflateRect(DSM_MARGIN_IN, 0, DSM_MARGIN_IN, 0);
    m_rcStatus.bottom   = rcClient.bottom + m_rcStatus.Height();
    m_rcStatus.top      = rcClient.bottom;

    m_pCurrentReport->Create(DSM_WINSTYLE_REPORT_DEFAULT, rcClient, this, IDC_SMV_MONITOR_REPORT);

    // 设置字体
    m_pCurrentReport->SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_pCurrentReport->SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    m_pCurrentReport->PostInitTemplateStyle();

    // 92
    if(DSM_SMV_MONITOR_PROTO_92 == m_dwTag)
    {
        _insertColumn92();
    }
    // 91 FT3
    else if(DSM_SMV_MONITOR_PROTO_91FT3 == m_dwTag)
    {
        _insertColumnFT3();
    }
    else
    {
        return;
    }

    // 设置选择焦点样式
    m_pCurrentReport->SetRowSelectionVisible(FALSE);
    m_pCurrentReport->SetRowFocusEnable(FALSE);

    _initReport();
}


/**
 *      初次显示当前控制块通道状态信息
 */
void CSMVMonitorDlg::_initReport()
{
    if(!m_pCurCtrlBlk || !m_pCurrentReport->GetSafeHwnd())
    {
        return;
    }

    CSmvRecvConfig* pCfg = m_pCurCtrlBlk->pRecvConfig;

    if(!pCfg)
    {
        return;
    }

    size_t nCounts = pCfg->GetChannelCount();

    // 92
    if(DSM_SMV_MONITOR_PROTO_92 == m_dwTag)
    {
        for(size_t i = 0; i < nCounts; i++)
        {
            m_pCurrentReport->InsertRecordRow(i, new CDsmSmvMonitor92Record(pCfg->GetOneChannelInfo(i)));
        }
    }
    // 91 FT3
    else if(DSM_SMV_MONITOR_PROTO_91FT3 == m_dwTag)
    {
        for(size_t i = 0; i < nCounts; i++)
        {
            m_pCurrentReport->InsertRecordRow(i, new CDsmSmvMonitorFT3Record(pCfg->GetOneChannelInfo(i)));
        }
    }

    m_nBegin    = m_pCurrentReport->GetTopRowIndex();
    m_nEnd      = m_pCurrentReport->GetCountPerPage() + m_nBegin;
}

/**
 *      创建92列
 */
void CSMVMonitorDlg::_insertColumn92()
{
    int nIndex = 0;

    // 插入列元素
    m_pCurrentReport->InsertColumn(nIndex++, L"", EL_CENTER, DSM_MONITOR_92_INDEX);
    m_pCurrentReport->InsertColumn(nIndex++, dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_9X_DESC), EL_LEFT, DSM_MONITOR_92_DESC);
    m_pCurrentReport->InsertColumn(nIndex++, dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_9X_STATUS), EL_CENTER, DSM_MONITOR_92_STATUS);
    m_pCurrentReport->InsertColumn(nIndex++, dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_9X_FIX), EL_CENTER, DSM_MONITOR_92_FIX);
    m_pCurrentReport->InsertColumn(nIndex++, dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_9X_COMPOUND), EL_CENTER, DSM_MONITOR_92_COMPOUND);
    m_pCurrentReport->InsertColumn(nIndex++, dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_9X_QUALITY), EL_CENTER, DSM_MONITOR_92_QUALITY);
}

/**
 *      创建91与FT3列
 */
void CSMVMonitorDlg::_insertColumnFT3()
{
    int nIndex = 0;

    // 插入列元素
    m_pCurrentReport->InsertColumn(nIndex++, L"", EL_CENTER, DSM_MONITOR_FT3_INDEX);
    m_pCurrentReport->InsertColumn(nIndex++, dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_FT3_DESC), EL_LEFT, DSM_MONITOR_FT3_DESC);
    m_pCurrentReport->InsertColumn(nIndex++, dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_FT3_STATUS), EL_CENTER, DSM_MONITOR_FT3_STATUS);
}


void CSMVMonitorDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    if(nIDEvent == m_nTimerID)
    {
        _updateReport();
    }

    CSMVBaseDlg::OnTimer(nIDEvent);
}


/**
 *      更新当前显示页面的信息
 */
void CSMVMonitorDlg::_updateReport()
{
    ASSERT(m_pDataLayer);

    // 检查通道数是否变化
    if(_checkChannel())
    {
        return;
    }

    if(!m_pDataLayer->GetStatusWatchData(&m_data))
    {
        // 清空数据
        _updateNullUI();

        return;
    }

    int nCounts = (int)m_data.GetChannelCount();

    // 92
    if(DSM_SMV_MONITOR_PROTO_92 == m_dwTag)
    {
        for(int i = m_nBegin; i < m_nEnd && i < nCounts; i++)
        {
            CDsmSmvMonitor92Record* pRecord92 = dynamic_cast<CDsmSmvMonitor92Record*>(m_pCurrentReport->GetRecordRow(i));

            if(pRecord92)
            {
                pRecord92->Update(m_data.GetOneChannelInfo(i));
            }
        }
    }
    // 91 FT3
    else if(DSM_SMV_MONITOR_PROTO_91FT3 == m_dwTag)
    {
        for(int i = m_nBegin; i < m_nEnd && i < nCounts; i++)
        {
            CDsmSmvMonitorFT3Record* pRecordFT3 = dynamic_cast<CDsmSmvMonitorFT3Record*>(m_pCurrentReport->GetRecordRow(i));

            if(pRecordFT3)
            {
                pRecordFT3->Update(m_data.GetOneChannelInfo(i));;
            }
        }
    }

    m_pCurrentReport->Invalidate(TRUE);

    _updateStatus();
}

BOOL CSMVMonitorDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if(WM_KEYDOWN == pMsg->message)
    {
        switch(pMsg->wParam)
        {
        case VK_UP:
            m_pCurrentReport->ScrollPageUp();
            m_nBegin    = m_pCurrentReport->GetTopRowIndex();
            m_nEnd      = m_pCurrentReport->GetCountPerPage() + m_nBegin;
            return TRUE;

        case VK_DOWN:
            m_pCurrentReport->ScrollPageDown();
            m_nBegin    = m_pCurrentReport->GetTopRowIndex();
            m_nEnd      = m_pCurrentReport->GetCountPerPage() + m_nBegin;
            return TRUE;
        }
    }

    return CSMVBaseDlg::PreTranslateMessage(pMsg);
}

void CSMVMonitorDlg::OnMenuF1()
{
    if(PAGE_DUALPHASE == m_ePage)
    {
        if(m_pCurrentReport->GetSafeHwnd())
        {
            m_pCurrentReport->ShowWindow(SW_HIDE);
        }

        m_modeCtrl       = m_modeCtrl == CTRL_ONE ? CTRL_TWO : CTRL_ONE;
        m_pCurCtrlBlk    = m_modeCtrl == CTRL_ONE ? m_pCtrlBlk1: m_pCtrlBlk2;
        m_pCurrentReport = m_modeCtrl == CTRL_ONE ? &m_reportChannel : &m_reportChannel2;

        m_pDataLayer->SetCurrentCtrlBlk(m_pCurCtrlBlk);

        _updateCtrlBlkCaption();
        _updateMenu();

        if(CTRL_ONE == m_modeCtrl)
        {
            if(m_reportChannel.GetSafeHwnd())
            {
                m_reportChannel.ShowWindow(SW_SHOW);
            }
            else
            {
                _createReport();
            }
        }
        else
        {
            if(m_reportChannel2.GetSafeHwnd())
            {
                m_reportChannel2.ShowWindow(SW_SHOW);
            }
            else
            {
                _createReport();
            }
        }

        // 立即更新一次数据
        _updateReport();
    }
}

/**
 *      更新标题
 */
void CSMVMonitorDlg::_updateCtrlBlkCaption()
{
    ASSERT(m_pCurCtrlBlk);

    if(m_pCurCtrlBlk)
    {
        CString csCaption;

        csCaption.Format(L"%s - [%s]", dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_CAPTION), DSM_GetCtrlBlkInfo(m_pCurCtrlBlk));
        CStatusMgr::SetTitle(csCaption);
    }
}

/**
 *      更新菜单
 */
void CSMVMonitorDlg::_updateMenu()
{
    if(PAGE_DUALPHASE == m_ePage)
    {
        // 切换菜单
        if(CTRL_ONE == m_modeCtrl)
        {
            m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV1SMV2_SMV1), NULL, NULL);
        }
        else
        {
            m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV1SMV2_SMV2), NULL, NULL);
        }

        m_pNaviBar->UpdateNaviMenu();
    }
}


/**
 *      清空界面的数据显示，将数据项设置为 "--"
 */
void CSMVMonitorDlg::_updateNullUI()
{
    int nCounts = m_pCurrentReport->GetRecordRowCount();

    // 92
    if(DSM_SMV_MONITOR_PROTO_92 == m_dwTag)
    {
        for(int i = m_nBegin; i < m_nEnd && i < nCounts; i++)
        {
            m_pCurrentReport->SetRowItemText(i, 2, g_csBlankValue);
            m_pCurrentReport->SetRowItemText(i, 3, g_csBlankValue);
            m_pCurrentReport->SetRowItemText(i, 4, g_csBlankValue);
            m_pCurrentReport->SetRowItemText(i, 5, g_csBlankValue);
        }
    }
    // 91 FT3
    else if(DSM_SMV_MONITOR_PROTO_91FT3 == m_dwTag)
    {
        for(int i = m_nBegin; i < m_nEnd && i < nCounts; i++)
        {
            m_pCurrentReport->SetRowItemText(i, 2, g_csBlankValue);
        }
    }

    _updateStatus(true);
}

/**
 *      检查通道数是否变化
 */
bool CSMVMonitorDlg::_checkChannel()
{
    if(m_pCurCtrlBlk->nChannelCount != m_pDataLayer->GetCurrentCtrlBlkRealTimeChannelCounts())
    {
        if(CStatusMgr::ShowTipsStyle() == 0x00)
        {
            // 界面tips提示
            CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_CHANNEL_NUM_CHANGE));
        }

        if(!m_bChannelChange)
        {
            m_bChannelChange = true;

            // 清空数据
            _updateNullUI();
        }

        return true;
    }
    else
    {
        if(m_bChannelChange)
        {
            m_bChannelChange = false;
            CStatusMgr::SetTips(L"", 0);
        }
    }

    return false;
}

#endif