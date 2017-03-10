/**
 * \file
 *      GooseShiftDlg.cpp
 *
 * \brief
 *      goose变位页面头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/5/10
 */

// GooseShiftDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GooseShiftDlg.h"

#include "src/main/mgr/statusmgr.h"
#include "src/main/utils/utility.h"
#include "src/main/pagedata/pagedata.h"
#include "src/main/goose/utility_goose.h"
#include "src/utils/utility/dsm_utility.h"


//////////////////////////////////////////////////////////////////////////
// goose - 变位信息
//
#define DSM_GOOSE_SHIFT_HEAD_INDEX          48          ///< 索引

#if CHUN_HUA
#define DSM_GOOSE_SHIFT_HEAD_TIME           200         ///< 时间
#define DSM_GOOSE_SHIFT_HEAD_INFO           352         ///< 信息
#else
#define DSM_GOOSE_SHIFT_HEAD_TIME           200         ///< 时间
#define DSM_GOOSE_SHIFT_HEAD_INFO           281         ///< 信息
#endif


#define DSM_GOOSE_SHIFT_HEAD_IND            60          ///< 信息
#define DSM_GOOSE_SHIFT_HEAD_TYPE           110         ///< 类型
#define DSM_GOOSE_SHIFT_HEAD_OLD            190         ///< 变位前值
#define DSM_GOOSE_SHIFT_HEAD_NEW            240         ///< 变位后值

#define SHIFT_FILTER_FIRST                  45000       ///< 通道关联的菜单ID起始值
#define SHIFT_FILTER_LAST                   45100       ///< 通道关联的菜单ID最大值



/**
 *      goose接收 - 历史变位列表
 *
 *  \note
 *      索引、时间（绝对/相对）、变位信息
 */
class CDsmGooseShiftRecord : public CELRecord
{
public:
    CDsmGooseShiftRecord(int nIndex, CString csTime, CString csShift)
        : m_nIndex(nIndex)
    {
        AddItem(new CDsmRecordItem(m_nIndex));
        AddItem(new CDsmRecordItem(csTime));
        AddItem(new CDsmRecordItem(csShift));
    }

    virtual ~CDsmGooseShiftRecord() {}

public:

    /**
     *      获取记录的第几次变位索引
     */
    uint32_t GetIndex()
    {
        return m_nIndex;
    }

private:
    uint32_t        m_nIndex;           ///< 第几次变位索引
};

/**
 *      goose接收 - 历史变位详细信息
 *
 *  \note
 *      索引、类型、新值、旧值
 */
class CDsmGooseShiftDetailRecord : public CELRecord
{
public:
    CDsmGooseShiftDetailRecord()
    {
        AddItem(new CDsmRecordItem(L""));
        AddItem(new CDsmRecordItem(L""));
        AddItem(new CDsmRecordItem(L""));
        AddItem(new CDsmRecordItem(L""));
    }

    virtual ~CDsmGooseShiftDetailRecord() {}
};



/**
 *      goose变位控制块管理的数据类
 */
class CCtrlBlkDataGooseShift : public ICtrlBlkData
{
public:

    CCtrlBlkDataGooseShift()
        : m_eMode(TIME_REL)
        , m_nChann(0)
        , m_nFocus(0)
        , m_nRows(0)
    {

    }

    virtual ~CCtrlBlkDataGooseShift() {}

public:
    ModeTime            m_eMode;            ///< 页面模式
    int                 m_nChann;           ///< 父通道索引
    int                 m_nFocus;           ///< 变位列表的focus行号
    int                 m_nRows;            ///< 报表行数
};


// CGooseShiftDlg 对话框

IMPLEMENT_DYNAMIC(CGooseShiftDlg, DSM_GOOSEASEDLG)

CGooseShiftDlg::CGooseShiftDlg(CWnd* pParent /* =NULL */)
	: DSM_GOOSEASEDLG(CGooseShiftDlg::IDD, DSM_STR_GOOSE_SHIFT_CAPTION, pParent)
    , m_eTime(TIME_REL)
    , m_ePage(PAGE_LIST)
    , m_dwLast(0)
    , m_nChannel(0)
    , m_pMenuDetail(NULL)
    , m_pMenuList(NULL)
    , m_eShift(SHIFT_ALL)
    , m_nDetailIndex(0)
    , m_nRows(0)
    , m_nParentChann(0)
    , m_nSubChann(-1)
    , m_bRecounter(true)
    , m_nLastFocus(0)
    , m_nShiftIndex(0)
    , m_nShiftSubIndex(-1)
{

}

CGooseShiftDlg::~CGooseShiftDlg()
{
    if(m_pMenuDetail)
    {
        delete m_pMenuDetail;
    }
}

void CGooseShiftDlg::DoDataExchange(CDataExchange* pDX)
{
	DSM_GOOSEASEDLG::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CGooseShiftDlg, DSM_GOOSEASEDLG)
    ON_WM_TIMER()
    ON_COMMAND(ID_MENU_GOOSE_F3, &CGooseShiftDlg::OnMenuF3)
    ON_COMMAND(ID_MENU_GOOSE_F6, &CGooseShiftDlg::OnMenuF6)
    ON_COMMAND(ID_MENU_GOOSE_SHIFT_DETAIL_F6, &CGooseShiftDlg::OnMenuShiftF6)
    ON_NOTIFY(ELNM_OKCLICK, IDC_GOOSE_SHIFT_REPORT, &CGooseShiftDlg::OnClickOK)
END_MESSAGE_MAP()


// CGooseShiftDlg 消息处理程序


BOOL CGooseShiftDlg::OnInitDialog()
{
    CGooseBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    // 创建报表
    _createReport();

    // 恢复页面状态
    _restorePage();

    if(m_pCurrentCtrlBlk)
    {
        m_pDataLayer->SetCurrentMode(DSM_MODE_GOOSE_RECV_CHANGE_LIST);

        _beginTimer();
    }

    return FALSE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


/**
 *      创建变位信息报表
 */
void CGooseShiftDlg::_createReport()
{
    CRect rcReport;
    GetClientRect(rcReport);
    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);

    int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

    m_reportGoose.Create(DSM_WINSTYLE_REPORT_DEFAULT, rcReport, this, IDC_GOOSE_SHIFT_REPORT);

    // 设置样式
    m_reportGoose.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_reportGoose.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_reportGoose.PostInitTemplateStyle();

    CString csCaption(dsm_stringMgr::Get(DSM_STR_GOOSE_SHIFT_HEAD_TIME));

    if(m_eTime == TIME_REL)
    {
        csCaption += L"(s)";
    }

    // 插入列元素
    m_reportGoose.InsertColumn(0, L"", EL_RIGHT, DSM_GOOSE_SHIFT_HEAD_INDEX);               // 索引
    m_reportGoose.InsertColumn(1, csCaption, EL_LEFT, DSM_GOOSE_SHIFT_HEAD_TIME);           // 变位时间
    m_reportGoose.InsertColumn(2, dsm_stringMgr::Get(DSM_STR_GOOSE_SHIFT_HEAD_INFO), EL_LEFT, DSM_GOOSE_SHIFT_HEAD_INFO);       // 变位信息

    // 设置选择焦点样式
    m_reportGoose.SetRowSelectionVisible(TRUE);
    m_reportGoose.SetRowFocusEnable(TRUE);

    m_reportGoose.SetFocus();
}

/**
 *      创建详细信息报表
 */
void CGooseShiftDlg::_createReportDetail()
{
    CRect rcReport;
    GetClientRect(rcReport);
    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);

    m_nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + m_nRows * ELT_T1_ROWHEIGHT;

    m_reportDetail.Create(DSM_WINSTYLE_REPORT_DEFAULT, rcReport, this, IDC_GOOSE_SHIFT_DETAIL_REPORT);

    // 设置样式
    m_reportDetail.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_reportDetail.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_reportDetail.PostInitTemplateStyle();

    // 插入列元素
    m_reportDetail.InsertColumn(0, L"", EL_CENTER, DSM_GOOSE_SHIFT_HEAD_IND);               // 索引
    m_reportDetail.InsertColumn(1, dsm_stringMgr::Get(DSM_STR_GOOSE_SHIFT_HEAD_TYPE), EL_CENTER, DSM_GOOSE_SHIFT_HEAD_TYPE);    // 类型
    m_reportDetail.InsertColumn(2, dsm_stringMgr::Get(DSM_STR_GOOSE_SHIFT_HEAD_OLD), EL_CENTER, DSM_GOOSE_SHIFT_HEAD_OLD);      // 旧值
    m_reportDetail.InsertColumn(3, dsm_stringMgr::Get(DSM_STR_GOOSE_SHIFT_HEAD_NEW), EL_CENTER, DSM_GOOSE_SHIFT_HEAD_NEW);      // 新值

    // 设置选择焦点样式
    m_reportDetail.SetRowSelectionVisible(FALSE);
    m_reportDetail.SetRowFocusEnable(FALSE);

    // 创建报表区域所能容纳的行数
    for(int i = 0; i < m_nRows; i++)
    {
        m_reportDetail.InsertRecordRow(i, new CDsmGooseShiftDetailRecord);
    }

    m_reportDetail.SetFocus();
}


void CGooseShiftDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    if(nIDEvent == m_nTimerID)
    {
        _updateData();
    }

    CGooseBaseDlg::OnTimer(nIDEvent);
}


/**
 *      刷新goose变位信息
 */
void CGooseShiftDlg::_updateData()
{
    ASSERT(m_pDataLayer);
    ASSERT(m_pCurrentCtrlBlk);

    if(!m_pDataLayer->GetGooseChangeList(&m_data))
    {
        return;
    }

    // 判断是否有新的变位数据

    uint32_t nNewShift = m_data.GetNewChangeTimes();
    if(!m_bRecounter && nNewShift <= 0)
    {
        return;
    }

    int nFocus      = m_reportGoose.GetFocusedRowIndex();
    int nRowCounts  = m_reportGoose.GetRecordRowCount();

    // 判断变位数目是否超过限制DSM_MAX_GOOSE_CHANGELIST_SIZE，超过限制需要从头删除变位record
    if(nNewShift + nRowCounts > DSM_MAX_GOOSE_CHANGELIST_SIZE)
    {
        int nDel = nNewShift + nRowCounts - DSM_MAX_GOOSE_CHANGELIST_SIZE;
        int nNewFocus = nFocus - nDel;

        // 从头部删除多余record
        for(int i = 0; i < nDel && m_reportGoose.GetRecordRowCount() > 0; i++)
        {
            m_reportGoose.DeleteRecordRow(0);
        }

        // 保持之前焦点行
        if(nNewFocus >= 0 && nNewFocus < m_reportGoose.GetRecordRowCount())
        {
            m_reportGoose.SetFocusedSelectionRowIndex(nNewFocus);
            m_reportGoose.EnsureRowVisibleScrollRow(nNewFocus);
        }
        // 之前焦点行被删除，则第一行获得焦点
        else if(nNewFocus == -1)
        {
            m_reportGoose.SetFocusedSelectionRowIndex(0);
            m_reportGoose.EnsureRowVisibleScrollRow(0);
        }
        // 其他则最后一行获得焦点
        else
        {
            m_reportGoose.SetFocusedSelectionRowIndex(m_reportGoose.GetRecordRowCount() - 1);
            m_reportGoose.EnsureRowVisibleScrollRow(m_reportGoose.GetRecordRowCount() - 1);
        }
    }

    int nChannel;
    int nIndex              = 0;
    bool bInsert            = false;       // 是否插入该通道数据
    uint32_t nShiftCounts   = m_data.GetChangeTimes();

    CGooseOneTimeChange data;
    CString csTime, csChannel, csFmt, csDesc;
    csFmt = dsm_stringMgr::Get(DSM_STR_GOOSE_SHIFT_CHANNEL);

    // 判断焦点行是否是末行，若是，则最后焦点行会保持最后一行
    bool bEnd = m_reportGoose.GetFocusedRowIndex() == m_reportGoose.GetRecordRowCount() - 1 ? true : false;

    ASSERT(nShiftCounts <= DSM_MAX_GOOSE_CHANGELIST_SIZE);

    // 重新过滤通道，所有的变位都是新的
    if(m_bRecounter)
    {
        nNewShift = nShiftCounts;
    }

    m_bRecounter = false;
    GOOSE_UTC_TIME timeBef;
    GOOSE_UTC_TIME timeAft;

    // 尾部插入新数据
    for(uint32_t i = nShiftCounts - nNewShift; i < nShiftCounts; i++)
    {
        csDesc = L"";
        data = m_data.GetGooseChangeInfo(i);

        if(!data.GetDataAfterChange() || !data.GetDataBeforeChange())
        {
            continue;
        }

        // 格式化变位通道信息
        uint32_t nCount = data.GetChangedChannelCounts();
        for(uint32_t i = 0; i < nCount; i++)
        {
            nChannel = data.GetChangedChannelIndex(i);

            // 找到过滤的通道，设置标志位
            if(nChannel == m_nChannel - 1)
            {
                bInsert = true;
            }

            csChannel.Format(L"，%s%d", csFmt, nChannel + 1);
            csDesc += csChannel;
        }

        timeAft = data.GetDataAfterChange()->GetEventTime();
        timeBef = data.GetDataBeforeChange()->GetEventTime();

        // 相对时间
        if(m_eTime == TIME_REL)
        {
            // 获取过滤时的上一次变位的基准时间
            if(m_nChannel != 0 && bInsert)
            {
                // 获取上一个变位信息
                if(m_reportGoose.GetRecordRowCount() > 0)
                {
                    CDsmGooseShiftRecord* pRecord = dynamic_cast<CDsmGooseShiftRecord*>(m_reportGoose.GetRecordRow(m_reportGoose.GetRecordRowCount() - 1));
                    ASSERT(pRecord);

                    uint32_t nPrev      = pRecord->GetIndex();
                    uint32_t nChCount   = m_data.GetChangeTimes();

                    for(uint32_t k = 0; k < nChCount; k++)
                    {
                        CGooseOneTimeChange ch = m_data.GetGooseChangeInfo(k);
                        if(ch.GetChangeIndex() == nPrev)
                        {
                            timeBef = ch.GetDataAfterChange()->GetEventTime();
                            break;
                        }
                    }
                }
                // 获取第一次变位信息
                else
                {
                    timeBef = m_data.GetGooseChangeInfo(0).GetDataBeforeChange()->GetEventTime();
                }
            }

            csTime = DSM_GetDValue(timeAft.t_s, timeAft.t_ns, timeBef.t_s, timeBef.t_ns);
        }
        // 绝对时间
        else
        {
            UTC_TIME_t ut = {data.GetDataAfterChange()->GetEventTime().t_s, data.GetDataAfterChange()->GetEventTime().t_ns / 1000};
            csTime = DM_GetSystemTimeStrFromUTC(ut);
        }

        nIndex = m_reportGoose.GetRecordRowCount();
        csDesc = csDesc.Right(csDesc.GetLength() - 1);

        // 全部通道
        if(m_nChannel == 0)
        {
            m_reportGoose.InsertRecordRow(nIndex, new CDsmGooseShiftRecord(data.GetChangeIndex(), csTime, csDesc));
        }
        // 过滤显示指定通道
        else
        {
            if(bInsert)
            {
                m_reportGoose.InsertRecordRow(nIndex, new CDsmGooseShiftRecord(data.GetChangeIndex(), csTime, csDesc));
            }

            bInsert = false;
        }
    }

    int nCounts = m_reportGoose.GetRecordRowCount();

    // 恢复一个合适的焦点
    if(nCounts > 0)
    {
        if(m_reportGoose.GetFocusedRowIndex() < 0)
        {
            m_reportGoose.SetFocusedSelectionRowIndex(0);
        }
        else if(bEnd)
        {
            m_reportGoose.SetFocusedSelectionRowIndex(nCounts - 1);
            m_reportGoose.EnsureRowVisibleScrollRow(nCounts - 1);
        }
    }
}


void CGooseShiftDlg::OnMenuF3()
{
    if(PAGE_DETAIL == m_ePage)
    {
        return;
    }

    CString csCaption;
    m_eTime = (m_eTime == TIME_REL) ? TIME_ABS : TIME_REL;

    if(TIME_REL == m_eTime)
    {
        // 设置F3相对图标
        m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_M_GS_ABSRELA_RELA), NULL, NULL);
        csCaption = dsm_stringMgr::Get(DSM_STR_GOOSE_SHIFT_HEAD_TIME) + L"(s)";
    }
    else
    {
        // 设置F3绝对图标
        m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_M_GS_ABSRELA_ABS), NULL, NULL);
        csCaption = dsm_stringMgr::Get(DSM_STR_GOOSE_SHIFT_HEAD_TIME);
    }

    m_pNaviBar->UpdateNaviMenu();

    LVCOLUMN colum;
    ::ZeroMemory(&colum, sizeof(LVCOLUMN));
    if(m_reportGoose.GetColumn(1, &colum))
    {
        colum.pszText       = csCaption.GetBuffer(csCaption.GetLength());
        colum.mask          = LVCF_TEXT;
        colum.cchTextMax    = csCaption.GetLength();
        m_reportGoose.SetColumn(1, &colum);
        csCaption.ReleaseBuffer(csCaption.GetLength());
    }

    _switchAbsoRel();
}

void CGooseShiftDlg::OnMenuF6()
{
//     if(PAGE_DETAIL != m_ePage)
//     {
//         return;
//     }
#if CHUN_HUA
	int i =0;
#endif
}


void CGooseShiftDlg::OnClickOK( NMHDR* /* pNotifyStruct */, LRESULT* pResult )
{
    *pResult = 1;

    // 获取选中行
    m_nLastFocus = m_reportGoose.GetFocusedRowIndex();
    if(m_reportGoose.GetRecordRowCount() <= 0 || m_nLastFocus < 0 || m_nLastFocus >= m_reportGoose.GetRecordRowCount())
    {
        return;
    }

    // 获取选中变位信息的索引
    CDsmGooseShiftRecord* pRecord = dynamic_cast<CDsmGooseShiftRecord*>(m_reportGoose.GetRecordRow(m_nLastFocus));
    ASSERT(pRecord);
    m_nDetailIndex = pRecord->GetIndex();

    CGooseOneTimeChange change;
    // 获取指定变位信息失败
    if(!_getChange(m_nDetailIndex, change))
    {
        return;
    }

    // 非真实变位信息，只是通道数量或类型发生变化后引起计数
    CGooseInfo* pBefore = change.GetDataBeforeChange();
    CGooseInfo* pAfter  = change.GetDataAfterChange();

    // 数量不同
    if(!pBefore || !pAfter || pBefore->GetChannelCounts() != pAfter->GetChannelCounts())
    {
        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_GOOSE_CHANNEL_CHANGE));
        return;
    }

    CGooseChannel* pBef = NULL;
    CGooseChannel* pAft = NULL;
    uint32_t nCounts    = pBefore->GetChannelCounts();

    // 判断类型
    for(uint32_t i = 0; i < nCounts; i++)
    {
        pBef = pBefore->GetChanneData(i);
        pAft = pAfter->GetChanneData(i);

        if(!pBef || !pAft || pBef->GetChannelData()->type != pAft->GetChannelData()->type)
        {
            CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_GOOSE_CHANNEL_CHANGE));
            return;
        }

        // 判断结构子通道
        if(pBef->GetChannelData()->type == GSEDT_STRUCT)
        {
            if(pBef->GetSubChannelCount() != pAft->GetSubChannelCount())
            {
                CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_GOOSE_CHANNEL_CHANGE));
                return;
            }

            uint32_t nSubCounts     = pBef->GetSubChannelCount();
            CGooseChannel* pSubBef  = NULL;
            CGooseChannel* pSubAft  = NULL;

            for(uint32_t k = 0; k < nSubCounts; k++)
            {
                pSubBef = pBef->GetSubChannelData(k);
                pSubAft = pAft->GetSubChannelData(k);

                if(!pSubAft || !pSubBef || pSubBef->GetChannelData()->type != pSubAft->GetChannelData()->type)
                {
                    CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_GOOSE_CHANNEL_CHANGE));
                    return;
                }
            }
        }
    }

    // 重置焦点位置信息
    m_nShiftIndex    = 0;
    m_nShiftSubIndex = -1;

    m_nParentChann   = 0;
    m_nSubChann      = -1;

    m_eShift        = SHIFT_ALL;

    // 查看变位的详细信息
    _showDetail();
}

/**
 *      保存页面
 */
void CGooseShiftDlg::_SavePage()
{
    ASSERT(m_pCurrentCtrlBlk);

    // 保存当前页面状态

    CCtrlBlkDataGooseShift* pData = dynamic_cast<CCtrlBlkDataGooseShift*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));

    if(!pData)
    {
        pData = new CCtrlBlkDataGooseShift;
        m_pPageDataExtra->Set(m_pCurrentCtrlBlk->nID, pData);
    }

    pData->m_eMode  = m_eTime;
    pData->m_nChann = m_nChannel;
    pData->m_nFocus = m_reportGoose.GetFocusedRowIndex();
    pData->m_nRows  = m_reportGoose.GetRecordRowCount();
}

/**
 *      修改菜单HMENU
 */
void CGooseShiftDlg::_modifyMenu()
{
    __super::_modifyMenu();

    // 设置F3绝对相对图标
    //m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(m_eTime == TIME_REL ? DSM_BMP_M_GS_ABSRELA_RELA : DSM_BMP_M_GS_ABSRELA_ABS), NULL, NULL);
#ifdef ZY_PRJ
	// 修改F6字符串为『过滤』
	m_menuMain.SetMenuItemCaption(5, dsm_stringMgr::Get(DSM_STR_GOOSE_SHIFT_FILTER));
#else
	// 修改F3字符串为『通道选择』
	m_menuMain.SetMenuItemCaption(2, L"通道选择");
	m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);

#endif
    
}

/**
 *      根据菜单ID切换到指定的控制块
 *
 *  \param[in]
 *      nCmdID          菜单的命令ID
 */
bool CGooseShiftDlg::_switchCtrlBlk( UINT nCmdID )
{
    CBaseDetectItem* pPreCtrlBlk = m_pCurrentCtrlBlk;

    if(__super::_switchCtrlBlk(nCmdID))
    {
        // 保存切换前控制块页面状态信息

        CCtrlBlkDataGooseShift* pData = dynamic_cast<CCtrlBlkDataGooseShift*>(m_pPageDataExtra->Get(pPreCtrlBlk->nID));

        if(!pData)
        {
            pData = new CCtrlBlkDataGooseShift;
            m_pPageDataExtra->Set(m_pCurrentCtrlBlk->nID, pData);
        }

        pData->m_eMode      = m_eTime;
        pData->m_nChann     = m_nChannel;
        pData->m_nRows      = m_reportGoose.GetRecordRowCount();
        pData->m_nFocus     = m_reportGoose.GetFocusedRowIndex();

        m_nShiftIndex       = 0;
        m_nShiftSubIndex    = -1;

        return _restorePage();
    }

    return false;
}

/**
 *      还原页面设置
 */
bool CGooseShiftDlg::_restorePage()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(CGooseShiftDlg::IDD == m_pPageDataExtra->GetPageData()->m_nPageID);

    // 获取当前的控制块索引
    CCtrlBlkDataGooseShift* pData = dynamic_cast<CCtrlBlkDataGooseShift*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));

    // 当前控制块若没绑定相关信息
    // 则按照默认显示
    if(!pData)
    {
        pData = new CCtrlBlkDataGooseShift;
        m_pPageDataExtra->Set(m_pCurrentCtrlBlk->nID, pData);;
    }

    m_nRows         = pData->m_nRows;
    m_eTime         = pData->m_eMode;
    m_nChannel      = pData->m_nChann;

    _resetF6Channel();

    // 显示变位信息
    _filter(m_nChannel + SHIFT_FILTER_FIRST);

    int nCounts = m_reportGoose.GetRecordRowCount();
    if(nCounts > 0)
    {
        if(pData->m_nFocus < nCounts && pData->m_nFocus >= 0 && pData->m_nFocus < pData->m_nRows)
        {
            m_reportGoose.SetFocusedSelectionRowIndex(pData->m_nFocus);
            m_reportGoose.SetFocusedSelectionRowIndex(pData->m_nFocus);
        }
        else
        {
            m_reportGoose.SetFocusedSelectionRowIndex(nCounts - 1);
            m_reportGoose.EnsureRowVisibleScrollRow(nCounts - 1);
        }
    }

    return true;
}

/**
 *      刷新界面显示控件元素（非数据）
 */
void CGooseShiftDlg::_switchMenu()
{
    // 变位详细信息
    if(PAGE_DETAIL == m_ePage)
    {
        if(!m_pMenuDetail)
        {
            CMenu menu;
            menu.LoadMenu(IDR_MENU_GOOSE_SHIFT_DETAIL);

            m_pMenuDetail = new CNaviMenu;
            m_pMenuDetail->LoadMenu(&menu);
            menu.DestroyMenu();
        }

        if(SHIFT_ALL == m_eShift)
        {
           m_pMenuDetail->SetMenuItemCaption(5, dsm_stringMgr::Get(DSM_STR_GOOSE_SHIFT_ONLY_SHIFT));
        }
        else
        {
            m_pMenuDetail->SetMenuItemCaption(5, dsm_stringMgr::Get(DSM_STR_GOOSE_SHIFT_ALL));
        }

        m_pMenuList = m_pNaviBar->GetNaviMenu();
        m_pNaviBar->SetNaviMenu(m_pMenuDetail);
    }
    // 变位列表
    else
    {
        if(m_pMenuList)
        {
            m_pNaviBar->SetNaviMenu(m_pMenuList);
        }
    }

    m_pNaviBar->UpdateNaviMenu();
}


/**
 *      重置F6通道菜单
 */
void CGooseShiftDlg::_resetF6Channel()
{
    ASSERT(m_pCurrentCtrlBlk);

    CMenu  menu;
    VERIFY(menu.CreatePopupMenu());

    UINT_PTR nItemID = SHIFT_FILTER_FIRST;
    menu.AppendMenu(MF_STRING, nItemID++, L"全部");

    CString csDesc;
    uint32_t nCounts = m_pCurrentCtrlBlk->nChannelCount;

    //m_dwLast = SHIFT_FILTER_FIRST + nCounts;
    ASSERT(nCounts < SHIFT_FILTER_LAST - SHIFT_FILTER_FIRST - 1);

    for(uint32_t i = 0; i < nCounts; i++)
    {
        csDesc.Format(L"通道%02u", i + 1);
        menu.AppendMenu(MF_STRING, nItemID++, csDesc);
    }

    m_dwLast = nItemID;

    CNaviMenu* pSub = new CNaviMenu;

    pSub->LoadMenu(&menu);
    menu.DestroyMenu();

#ifdef ZY_PRJ
	m_pNaviBar->GetNaviMenu()->SetMenuSubItem(5, pSub);  
	if((uint32_t)m_nChannel > nCounts + 1)
	{
		m_nChannel = 0;
	}

	m_pNaviBar->GetNaviMenu()->GetSubMenu(5)->HighlightMenuItem(m_nChannel, TRUE);
#else
	m_pNaviBar->GetNaviMenu()->SetMenuSubItem(2, pSub); //将过滤通道弹出菜单改到F3按钮
	if((uint32_t)m_nChannel > nCounts + 1)
	{
		m_nChannel = 0;
	}

	m_pNaviBar->GetNaviMenu()->GetSubMenu(2)->HighlightMenuItem(m_nChannel, TRUE);
#endif
    

    
}


BOOL CGooseShiftDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    // TODO: 在此添加专用代码和/或调用基类

    int nMenuID = LOWORD(wParam);

    if(nMenuID >= SHIFT_FILTER_FIRST && (DWORD)nMenuID < m_dwLast)
    {
        // 处理F6通道过滤功能

        // 不变
        if(m_nChannel != nMenuID - SHIFT_FILTER_FIRST)
        {
            _filter(nMenuID);
        }

        return TRUE;
    }

    return CGooseBaseDlg::OnCommand(wParam, lParam);
}


/**
 *      过滤通道
 *
 *  \param[in]
 *      nItem       通道号对应的菜单ID
 */
void CGooseShiftDlg::_filter( int nItem )
{
#ifdef ZY_PRJ
    // 取消上一个个高亮的菜单
    CNaviBarMgr::GetInstance()->HighlightMenuItem(5, m_nChannel, FALSE);
    m_nChannel = nItem - SHIFT_FILTER_FIRST;
    // 高亮菜单
    CNaviBarMgr::GetInstance()->HighlightMenuItem(5, m_nChannel);
#else
	// 取消上一个个高亮的菜单
	CNaviBarMgr::GetInstance()->HighlightMenuItem(2, m_nChannel, FALSE);
	m_nChannel = nItem - SHIFT_FILTER_FIRST;
	// 高亮菜单
	CNaviBarMgr::GetInstance()->HighlightMenuItem(2, m_nChannel);
#endif

    m_reportGoose.DeleteAllRecordRow();

    // 重新遍历变为列表
    m_bRecounter = true;

    // 更新报表
    _updateData();
}

BOOL CGooseShiftDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    // TODO: 在此添加专用代码和/或调用基类

    if(nCode == CN_UPDATE_COMMAND_UI && nID >= SHIFT_FILTER_FIRST && nID < m_dwLast)
    {
        CCmdUI* pCmdUI = (CCmdUI*)pExtra;
        ASSERT(pCmdUI);

        pCmdUI->Enable(TRUE);

        return TRUE;
    }

    return CGooseBaseDlg::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


/**
 *      切换显示绝对时间与相对时间
 */
void CGooseShiftDlg::_switchAbsoRel()
{
    CString csTime;
    CGooseOneTimeChange info;
    CGooseInfo* pInfoBef        = NULL;
    CGooseInfo* pInfoAft        = NULL;
    uint32_t nCounts            = m_reportGoose.GetRecordRowCount();

    for(uint32_t i = 0; i < nCounts; i++)
    {
        // 获取变位信息
        info = m_data.GetGooseChangeInfo(i);

        pInfoBef = info.GetDataBeforeChange();
        pInfoAft = info.GetDataAfterChange();

        ASSERT(pInfoBef && pInfoAft);

        // 相对时间
        if(TIME_REL == m_eTime)
        {
            csTime = DSM_GetDValue(pInfoAft->GetEventTime().t_s, pInfoAft->GetEventTime().t_ns, pInfoBef->GetEventTime().t_s, pInfoBef->GetEventTime().t_ns);
        }
        // 绝对时间
        else
        {
            UTC_TIME_t ut = {pInfoAft->GetEventTime().t_s, pInfoAft->GetEventTime().t_ns / 1000};
            csTime = DM_GetSystemTimeStrFromUTC(ut);
        }

        m_reportGoose.SetRowItemText(i, 1, csTime);
    }
}

BOOL CGooseShiftDlg::PreTranslateMessage( MSG* pMsg )
{
    if(PAGE_DETAIL == m_ePage)
    {
        if(WM_KEYDOWN == pMsg->message)
        {
            switch(pMsg->wParam)
            {
            case VK_ESCAPE:
                m_reportDetail.ShowWindow(SW_HIDE);
                m_reportGoose.ShowWindow(SW_SHOW);

                m_reportGoose.SetFocus();

                if(m_nLastFocus < m_reportGoose.GetRecordRowCount())
                {
                    m_reportGoose.SetFocusedSelectionRowIndex(m_nLastFocus);
                }
                else if(m_reportGoose.GetRecordRowCount() > 0)
                {
                    m_reportGoose.SetFocusedSelectionRowIndex(0);
                }

                m_ePage = PAGE_LIST;
                _switchMenu();

                return TRUE;

            case VK_UP:
                _channelDetailUp();
                break;

            case  VK_DOWN:
                _channelDetailDown();
                break;

            default:
                break;
            }
        }
    }

    return CGooseBaseDlg::PreTranslateMessage(pMsg);
}

void CGooseShiftDlg::OnMenuShiftF6()
{
    if(PAGE_LIST == m_ePage)
    {
        return;
    }

    m_eShift = m_eShift == SHIFT_ALL ? SHIFT_ONLY : SHIFT_ALL;

    // 切换显示全部变位
    if(SHIFT_ALL == m_eShift)
    {
        m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(5, dsm_stringMgr::Get(DSM_STR_GOOSE_SHIFT_ONLY_SHIFT));
        m_pNaviBar->UpdateNaviMenu();

        _updateDetail();
    }
    // 切换仅变位信息
    else
    {
        m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(5, dsm_stringMgr::Get(DSM_STR_GOOSE_SHIFT_ALL));
        m_pNaviBar->UpdateNaviMenu();

        _updateShiftDetail();
    }
}


/**
 *      显示详细变位信息
 */
void CGooseShiftDlg::_showDetail( )
{
    m_ePage = PAGE_DETAIL;

    // 显示详细变位信息
    if(!m_reportDetail.GetSafeHwnd())
    {
        _createReportDetail();
    }

    _switchMenu();

    m_reportGoose.ShowWindow(SW_HIDE);
    m_reportDetail.ShowWindow(SW_SHOW);

    m_reportDetail.SetFocus();

    // 显示指定变位信息
    _updateDetail();
}


/**
 *      更新详细变位信息
 */
void CGooseShiftDlg::_updateDetail()
{
    CGooseOneTimeChange change;

    if(!_getChange(m_nDetailIndex, change))
    {
        _fillDetailReport(L"");
        ASSERT(FALSE);
        return;
    }

    CGooseInfo* pBefore = change.GetDataBeforeChange();
    CGooseInfo* pAfter  = change.GetDataAfterChange();

    if(!pBefore || !pAfter || pBefore->GetChannelCounts() != pAfter->GetChannelCounts())
    {
        _fillDetailReport(g_csBlankValue);
        ASSERT(FALSE);
        return;
    }

    uint32_t nChann             = m_nParentChann;   // 第一行记录的通道序号
    int nSubChann               = m_nSubChann;      // 第一行记录通道序号的子通道序号
    CGooseChannel* pChannBef    = NULL;
    CGooseChannel* pChannAft    = NULL;

    for(int i = 0; i < m_nRows; nChann++, nSubChann = -1)
    {
        pChannBef = pBefore->GetChanneData(nChann);
        pChannAft = pAfter->GetChanneData(nChann);

        if(!pChannAft || !pChannBef || pChannAft->GetChannelType() != pChannBef->GetChannelType() 
            || pChannAft->GetSubChannelCount() != pChannBef->GetSubChannelCount())
        {
            CString csText = g_csBlankValue;

            if(nChann >= pBefore->GetChannelCounts())
            {
                csText = L"";
            }

            m_reportDetail.SetRowItemText(i, 0, csText);
            m_reportDetail.SetRowItemText(i, 1, csText);
            m_reportDetail.SetRowItemText(i, 2, csText);
            m_reportDetail.SetRowItemText(i, 3, csText);

            i++;

            continue;
        }

        // 如果子通道序号为-1，则插入父通道（结构类型）
        if(-1 == nSubChann)
        {
            if(_insertRecord(pChannBef, pChannAft, nChann, nSubChann, i))
            {
                i++;
            }
        }

        // 若为父通道（结构类型），则枚举子通道
        if(pChannAft->GetChannelData()->type == GSEDT_STRUCT && i < m_nRows)
        {
            uint32_t nCounts        = pChannAft->GetSubChannelCount();      // 子通道个数
            nSubChann               = (nSubChann == -1) ? 0 : nSubChann;    // 计算枚举子通道的位置

            // 枚举插入子通道
            for(uint32_t j = nSubChann; j < nCounts && i < m_nRows; j++)
            {
                if(_insertRecord(pChannBef->GetSubChannelData(j), pChannAft->GetSubChannelData(j), nChann, j, i))
                {
                    i++;
                }
            }
        }
    }
}


/**
 *      插入一条详细变位记录
 *
 *  \param[in]
 *      pChanBef        变位前通道
 *  \param[in]
 *      pChannAft       变位后通道
 *  \param[in]
 *      nChannel        通道号
 *  \param[in]
 *      nSubChannel     子通道号
 *  \param[in]
 *      nRow            报表行号
 */
bool CGooseShiftDlg::_insertRecord( CGooseChannel* pChanBef, CGooseChannel* pChannAft, int nChannel, int nSubChannel, int nRow )
{
    ASSERT(pChannAft && pChanBef);

    GOOSE_DATA* pGsDataBef      = pChanBef->GetChannelData();
    GOOSE_DATA* pGsDataAft      = pChannAft->GetChannelData();

    if(!pGsDataBef || !pGsDataAft)
    {
        DSM_DEBUG(L"dsm: goose shift detal: null goose data.\n");
        ASSERT(FALSE);
        return false;
    }

    CString csIndex, csType, csValBef, csValAft;

    // 格式化索引
    if(pChanBef->GetParentChannel())
    {
        csIndex.Format(L"%d.%d", nChannel + 1, nSubChannel + 1);
    }
    else
    {
        csIndex.Format(L"%d", nChannel + 1);
    }

    // 获取goose数据信息
    DSM_GetGooseDataInfo(pChanBef, csType, csValBef);
    DSM_GetGooseDataInfo(pChannAft, csType, csValAft);

    m_reportDetail.SetRowItemText(nRow, 0, csIndex);
    m_reportDetail.SetRowItemText(nRow, 1, csType);
    m_reportDetail.SetRowItemText(nRow, 2, csValBef);
    m_reportDetail.SetRowItemText(nRow, 3, csValAft);

    return true;
}

/**
 *      计算向上翻页在第一行显示的通道序号和子通道序号
 *
 *  \return
 *      bool            发生翻页返回true，否则返回false
 */
bool CGooseShiftDlg::_calcUp()
{
    uint32_t nCount = m_data.GetChannelCounts();

    CGooseOneTimeChange change;

    // 查找变位信息
    uint32_t nCounts = m_data.GetChangeTimes();
    for(uint32_t i = 0; i < nCounts; i++)
    {
        change = m_data.GetGooseChangeInfo(i);

        if(change.GetChangeIndex() == m_nDetailIndex)
        {
            break;
        }
    }

    if(nCount == 0 || !change.GetDataBeforeChange() || !change.GetDataAfterChange())
    {
        return false;
    }

    int nRow                    = 0;                    // 累计显示的行数
    int nChan                   = m_nParentChann;       // 当前显示通道索引
    GOOSE_DATA* pGooseData      = NULL;
    GOOSE_STRUCT* pGooseSt      = NULL;
    CGooseChannel* pChannel     = change.GetDataBeforeChange()->GetChanneData(nChan);

    ASSERT(pChannel);

    while(pChannel)
    {
        pGooseData = pChannel->GetChannelData();
        ASSERT(pGooseData);

        if(GSEDT_STRUCT == pGooseData->type)
        {
            pGooseSt        = (GOOSE_STRUCT*)pGooseData;
            size_t nSize    = pGooseSt->member.size();

            if(nChan == m_nParentChann)
            {
                // m_nSubChann为未显示的子通道
                // 算出m_nSubChann之前该通道未显示的行数
                nRow = nRow + m_nSubChann + 1;
            }
            else
            {
                // 该通道全部未显示nsize是子通道+1是该通道本身
                nRow = nRow + nSize + 1;
            }

            // 已显示的通道数大于每页最大显示通道数
            // 需要计算某个（子）通道索引，让其显示为第一行
            if(nRow >= m_nRows)
            {
                m_nParentChann      = nChan;                    // 某通道索引
                m_nSubChann         = nRow - m_nRows - 1;       // 其子通道索引，算出首行索引

                return true;
            }
        }
        else
        {
            if(nChan != m_nParentChann)
            {
                nRow++;
            }

            // 已显示满通道
            if(nRow == m_nRows)
            {
                m_nParentChann      = nChan;
                m_nSubChann         = -1;

                return true;
            }
        }

        // 枚举上一个通道
        pChannel = change.GetDataBeforeChange()->GetChanneData(--nChan);
    }

    return false;
}
 
/**
 *      计算向下翻页在第一行显示的通道序号和子通道序号
 *
 *  \return
 *      bool            发生翻页返回true，否则返回false
 */
bool CGooseShiftDlg::_calcDown()
{
    uint32_t nCount = m_data.GetChannelCounts();

    CGooseOneTimeChange change;

    // 查找变位信息
    uint32_t nCounts = m_data.GetChangeTimes();
    for(uint32_t i = 0; i < nCounts; i++)
    {
        change = m_data.GetGooseChangeInfo(i);

        if(change.GetChangeIndex() == m_nDetailIndex)
        {
            break;
        }
    }

    if(nCount == 0 || !change.GetDataBeforeChange() || !change.GetDataAfterChange())
    {
        return false;
    }

    int nRow                    = 0;                // 累计显示的行数
    int nChan                   = m_nParentChann;   // 当前计算的通道索引
    int nSubChan                = m_nSubChann;      // 当前计算通道的子通道索引
    GOOSE_DATA* pGooseData      = NULL;
    GOOSE_STRUCT* pGooseSt      = NULL;
    CGooseChannel* pChannel     = change.GetDataBeforeChange()->GetChanneData(nChan);

    ASSERT(pChannel);

    while(pChannel)
    {
        pGooseData = pChannel->GetChannelData();
        ASSERT(pGooseData);

        // 当前通道是结构体类型（即父通道类型）
        if(GSEDT_STRUCT == pGooseData->type)
        {
            pGooseSt        = (GOOSE_STRUCT*)pGooseData;
            size_t nSize    = pGooseSt->member.size();

            // 算出该通道已显示的行数
            nRow = nRow + nSize - nSubChan;

            // 判断最后一页的情况
            if(nRow == m_nRows && !change.GetDataBeforeChange()->GetChanneData(nChan + 1))
            {
                return false;
            }

            // 如果该通道在本页面显示不完，则下一页继续显示该通道
            if(nRow >= m_nRows)
            {
                m_nParentChann  = nRow > m_nRows ? nChan : nChan + 1;
                m_nSubChann     = nRow > m_nRows ? nSize - (nRow - m_nRows) : -1;

                return true;
            }
        }
        else
        {
            nRow++;
            if(nRow > m_nRows)
            {
                m_nParentChann  = nChan;
                m_nSubChann     = nSubChan;

                return true;
            }
        }

        // 该通道在本页能被完全显示，则枚举下一通道，并恢复子通道索引为-1
        nSubChan = -1;

        pChannel = change.GetDataBeforeChange()->GetChanneData(++nChan);
    }

    return false;
}

/**
 *      通道翻页 - 上一页
 */
void CGooseShiftDlg::_channelDetailUp()
{
    if(m_eShift == SHIFT_ALL)
    {
        if(_calcUp())
        {
            _updateDetail();
        }
    }
    else if(m_eShift == SHIFT_ONLY)
    {
        if(_calcUpOnly())
        {
            _updateShiftDetail();
        }
    }
}

/**
 *      通道翻页 - 下一页
 */
void CGooseShiftDlg::_channelDetailDown()
{
    if(m_eShift == SHIFT_ALL)
    {
        if(_calcDown())
        {
            _updateDetail();
        }
    }
    else if(m_eShift == SHIFT_ONLY)
    {
        if(_calcDownOnly())
        {
            _updateShiftDetail();
        }
    }
}

/**
 *      计算单独变位信息上翻页
 *
 *  \return
 *      bool        发生翻页返回true，否则返回false
 */
bool CGooseShiftDlg::_calcUpOnly()
{
    CGooseOneTimeChange change;

    if(!_getChange(m_nDetailIndex, change) || !change.GetDataBeforeChange() || !change.GetDataAfterChange())
    {
        return false;
    }

    int nRow                    = 0;                    // 累计显示的行数
    uint32_t nShift             = m_nShiftIndex;        // 当前显示变位索引
    uint32_t nFirstIndex        = change.GetChangedChannelIndex(m_nShiftIndex);
    uint32_t nChanIndex         = nFirstIndex;
    CGooseChannel* pChanBef     = change.GetDataBeforeChange()->GetChanneData(nChanIndex);
    CGooseChannel* pChanAft     = change.GetDataAfterChange()->GetChanneData(nChanIndex);
    GOOSE_DATA* pDataBef        = NULL;
    GOOSE_DATA* pDataAft        = NULL;

    while(pChanBef || pChanAft)
    {
        pDataBef = pChanBef->GetChannelData();
        pDataAft = pChanAft->GetChannelData();
        ASSERT(pDataBef && pDataAft &&  pDataBef->type == pDataAft->type);

        // 两者之一是struct
        if(GSEDT_STRUCT == pDataBef->type)
        {
            size_t nSize = ((GOOSE_STRUCT*)pDataBef)->member.size();

            if(nChanIndex == nFirstIndex)
            {
                // m_nSubChann为未显示的子通道
                // 算出m_nSubChann之前该通道未显示的行数
                nRow = nRow + m_nShiftSubIndex + 1;
            }
            else
            {
                // 该通道全部未显示nsize是子通道+1是该通道本身
                nRow = nRow + nSize + 1;
            }

            // 已显示的通道数大于每页最大显示通道数
            // 需要计算某个（子）通道索引，让其显示为第一行
            if(nRow >= m_nRows)
            {
                m_nShiftIndex       = nShift;                   // 某变位索引
                m_nShiftSubIndex    = nRow - m_nRows - 1;       // 其子通道索引，算出首行索引

                return true;
            }
        }
        else
        {
            if(nChanIndex != nFirstIndex)
            {
                nRow++;
            }

            // 已显示满通道
            if(nRow == m_nRows)
            {
                m_nShiftIndex       = nShift;
                m_nShiftSubIndex    = -1;

                return true;
            }
        }

        // 枚举上一个变位通道

        nChanIndex = change.GetChangedChannelIndex(--nShift);

        if(nChanIndex >= 0)
        {
            pChanBef = change.GetDataBeforeChange()->GetChanneData(nChanIndex);
            pChanAft = change.GetDataAfterChange()->GetChanneData(nChanIndex);
        }
        else
        {
            return false;
        }
    }

    return false;
}

/**
 *      计算单独变位信息下翻页
 *
 *  \return
 *      bool        发生翻页返回true，否则返回false
 */
bool CGooseShiftDlg::_calcDownOnly()
{
    CGooseOneTimeChange change;

    if(!_getChange(m_nDetailIndex, change) || !change.GetDataBeforeChange() || !change.GetDataAfterChange())
    {
        return false;
    }

    int nRow                    = 0;                    // 累计显示的行数
    uint32_t nShift             = m_nShiftIndex;        // 当前显示变位索引
    uint32_t nFirstIndex        = change.GetChangedChannelIndex(m_nShiftIndex);
    uint32_t nChanIndex         = nFirstIndex;
    int nChanSubIndex           = m_nShiftSubIndex;
    CGooseChannel* pChanBef     = change.GetDataBeforeChange()->GetChanneData(nChanIndex);
    CGooseChannel* pChanAft     = change.GetDataAfterChange()->GetChanneData(nChanIndex);
    GOOSE_DATA* pDataBef        = NULL;
    GOOSE_DATA* pDataAft        = NULL;

    while(pChanBef || pChanAft)
    {
        pDataBef = pChanBef->GetChannelData();
        pDataAft = pChanAft->GetChannelData();
        ASSERT(pDataBef && pDataAft && pDataAft->type == pDataBef->type);

        // 两者之一是struct
        if(GSEDT_STRUCT == pDataBef->type)
        {
            size_t nSize = ((GOOSE_STRUCT*)pDataBef)->member.size();

            // 算出该通道已显示的行数
            nRow = nRow + nSize - nChanSubIndex;

            // 判断最后一页的情况
            if(nRow == m_nRows && !change.GetDataBeforeChange()->GetChanneData(nChanIndex + 1))
            {
                return false;
            }

            // 如果该通道在本页面显示不完，则下一页继续显示该通道
            if(nRow >= m_nRows)
            {

                m_nShiftIndex       = nRow > m_nRows ? nShift : nShift + 1;
                m_nShiftSubIndex    = nRow > m_nRows ? nSize - (nRow - m_nRows) : -1;

                return true;
            }
        }
        else
        {
            nRow++;
            if(nRow > m_nRows)
            {
                m_nShiftIndex       = nShift;
                m_nShiftSubIndex    = nChanSubIndex;

                return true;
            }
        }

        nChanSubIndex = -1;

        // 枚举下一个变位通道

        nChanIndex = change.GetChangedChannelIndex(++nShift);

        if(nChanIndex < change.GetChangedChannelCounts())
        {
            pChanBef = change.GetDataBeforeChange()->GetChanneData(nChanIndex);
            pChanAft = change.GetDataAfterChange()->GetChanneData(nChanIndex);
        }
        else
        {
            return false;
        }
    }

    return false;
}

/**
 *      显示单独变位信息
 */
void CGooseShiftDlg::_updateShiftDetail()
{
    CGooseOneTimeChange change;

    bool bFind          = _getChange(m_nDetailIndex, change);
    uint32_t nCounts    = change.GetChangedChannelCounts();

    // 找不到变位信息或者变位索引大于变位总数
    if(!bFind || m_nShiftIndex >= nCounts)
    {
        _fillDetailReport(L"");
        ASSERT(FALSE);
        return;
    }

    CGooseInfo* pBefore = change.GetDataBeforeChange();
    CGooseInfo* pAfter  = change.GetDataAfterChange();
    ASSERT(pBefore && pAfter);

//     // 通道数不相等异常
//     if(pBefore->GetChannelCounts() != pAfter->GetChannelCounts())
//     {
//         _fillDetailReport(g_csBlankValue);
//         ASSERT(FALSE);
//         return;
//     }

    uint32_t nIndex             = m_nShiftIndex;            // 第一行变位通道索引
    int nChannelIndex           = -1;                       // 第一行变位通道号
    int nChannelSubIndex        = m_nShiftSubIndex;         // 第一行变位子通道号
    CGooseChannel* pChannBef    = NULL;
    CGooseChannel* pChannAft    = NULL;

    for(int i = 0; i < m_nRows;)
    {
        nChannelIndex = change.GetChangedChannelIndex(nIndex++);

        if(nChannelIndex == -1)
        {
            m_reportDetail.SetRowItemText(i, 0, L"");
            m_reportDetail.SetRowItemText(i, 1, L"");
            m_reportDetail.SetRowItemText(i, 2, L"");
            m_reportDetail.SetRowItemText(i, 3, L"");
            i++;

            continue;
        }

        pChannBef = pBefore->GetChanneData(nChannelIndex);
        pChannAft = pAfter->GetChanneData(nChannelIndex);

        // 如果子通道序号为-1，则插入父通道（结构类型）
        if(nChannelSubIndex == -1)
        {
            _insertRecord(pChannBef, pChannAft, nChannelIndex, nChannelSubIndex, i++);
        }

        // 若为父通道（结构类型），则枚举子通道
        if(pChannAft->GetChannelData()->type == GSEDT_STRUCT && i < m_nRows)
        {
            uint32_t nSubCounts     = pChannAft->GetSubChannelCount();                      // 子通道个数
            nChannelSubIndex        = (nChannelSubIndex == -1) ? 0 : nChannelSubIndex;      // 计算枚举子通道的位置

            // 枚举插入子通道
            for(uint32_t j = nChannelSubIndex; j < nSubCounts && i < m_nRows; j++)
            {
                _insertRecord(pChannBef->GetSubChannelData(j), pChannAft->GetSubChannelData(j), nChannelIndex, j, i++);
            }
        }
    } // for
}

/**
 *      获取指定索引的变位数据
 *
 *  \param[in]
 *      nIndex      变位数据索引
 *  \param[out]
 *      change      变位数据
 *  \return
 *      bool        获取数据成功返回true，失败返回false
 */
bool CGooseShiftDlg::_getChange( uint32_t nIndex, CGooseOneTimeChange& change )
{
    // 查找变位信息

    uint32_t nCounts    = m_data.GetChangeTimes();

    for(uint32_t i = 0; i < nCounts; i++)
    {
        change = m_data.GetGooseChangeInfo(i);

        if(change.GetChangeIndex() == nIndex)
        {
            return true;
        }
    }

    return false;
}


/**
 *      填充detal报表
 *
 *  \param[in]
 *      csContent       填充字符串内容
 */
void CGooseShiftDlg::_fillDetailReport( CString csContent )
{
    // 清空报表
    for(int i = 0; i < m_nRows; i++)
    {
        m_reportDetail.SetRowItemText(i, 0, csContent);
        m_reportDetail.SetRowItemText(i, 1, csContent);
        m_reportDetail.SetRowItemText(i, 2, csContent);
        m_reportDetail.SetRowItemText(i, 3, csContent);
    }
}


