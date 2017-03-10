/**
 * \file
 *      GooseScanDlg.cpp
 *
 * \brief
 *      探测goose控制块页面实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/5/8
 */

// GooseScanDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GooseScanDlg.h"

#include "src/main/common.h"
#include "src/main/mgr/statusmgr.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/service/datalayer/dsmdatalayer.h"

//////////////////////////////////////////////////////////////////////////
// goose - 探测
//
#define DSM_GOOSE_SCAN_HEAD_CHECKBOX        48          ///< 选择控制块栏
#define DSM_GOOSE_SCAN_HEAD_INDEX           50//100         ///< 控制块索引
#define DSM_GOOSE_SCAN_HEAD_APPID           100         ///< 控制块AppID
#define DSM_GOOSE_SCAN_HEAD_DESC            200//261         ///< 控制块描述，没有描述信息则显示MAC地址
#define DSM_GOOSE_SCAN_HEAD_PORT            91          ///< 端口名


#define DSM_GOOSE_SCAN_TIMERID              1           ///< 刷新探测数据的定时器ID

#define DSM_GOOSE_CTRLBLK_MAX_NUM           10          ///< GOOSE最多可选控制块个数



/**
 *      GOOSE探测数据记录项
 *
 *  \note
 *      checkbox、序号、AppID、描述、端口
 */
class CDsmScanGooseRecord : public CELRecord
{
public:
    /**
     *      GOOSE探测记录项
     *
     *  \param[in]
     *      nIndex          索引
     *  \param[in]
     *      pInfo           控制块信息
     */
    CDsmScanGooseRecord(int nIndex, CNetDetectItem* pInfo);
    virtual ~CDsmScanGooseRecord() {}

public:
    /**
     *      获取goose记录返回的控制块信息
     */
    inline CNetDetectItem* _GetInfo()
    {
        return m_pInfo;
    }

private:
    CNetDetectItem*         m_pInfo;            ///< 记录关联的控制块信息
};


CDsmScanGooseRecord::CDsmScanGooseRecord( int nIndex, CNetDetectItem* pInfo)
: m_pInfo(pInfo)
{
    ASSERT(m_pInfo);

    CString csAppID;
    csAppID.Format(L"0x%04X", m_pInfo->appId);

    CELRecordItem* pItem = AddItem(new CDsmRecordItem(L""));
    AddItem(new CDsmRecordItem(nIndex));
    AddItem(new CDsmRecordItem(csAppID));
    AddItem(new CDsmRecordItem(DSM_GetNetCtrlBlkDesc(m_pInfo)));
    AddItem(new CDsmRecordItem(DSM_GetPortDesc(m_pInfo->enPortName)));

    pItem->SetEditable(TRUE);
    pItem->HasCheckbox(TRUE);
}



// CGooseScanDlg 对话框

IMPLEMENT_DYNAMIC(CGooseScanDlg, DSM_BASEDLG)

CGooseScanDlg::CGooseScanDlg(CWnd* pParent /*=NULL*/)
	: DSM_BASEDLG(CGooseScanDlg::IDD, DSM_STR_GOOSE_SCAN_CAPTION, pParent)
    , m_nScanTimerID(UINT_MAX)
    , m_pDataLayer(NULL)
{
    m_nMenuID = IDR_MENU_GOOSE_SCAN;

    m_pDataLayer = CDsmDataLayer::getInstance();
}

CGooseScanDlg::~CGooseScanDlg()
{
    _clearRes();
}

void CGooseScanDlg::DoDataExchange(CDataExchange* pDX)
{
	DSM_BASEDLG::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CGooseScanDlg, DSM_BASEDLG)
    ON_COMMAND_RANGE(ID_MENU_SCAN_SMV_DLG, ID_MENU_GOOSE_ENTER, &CGooseScanDlg::OnNaviMenuFn)
    ON_NOTIFY(ELNM_ITEM_CHECKED, IDC_GOOSE_SCAN_REPORT, &CGooseScanDlg::OnItemChecked)
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CGooseScanDlg 消息处理程序


BOOL CGooseScanDlg::OnInitDialog()
{
    CBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    _createReport();

    m_pDataLayer->BeginDetect(NET_RECV_TYPE_GOOSE);

    m_nScanTimerID = SetTimer(DSM_GOOSE_SCAN_TIMERID, DSM_GOOSE_SCAN_TIMER_INTERVAL, NULL);

    m_reportGoose.SetFocus();

    return FALSE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


void CGooseScanDlg::OnNaviMenuFn( UINT nID )
{
    switch(nID)
    {
        // 重新探测goose控制块
    case ID_MENU_GOOSE_RESCAN:
        _rescan();
        break;

        // 清除选中goose控制块
    case ID_MENU_GOOSE_CLSSEL:
        _clearSelect();
        break;

        // 进入goose业务
    case ID_MENU_GOOSE_ENTER:
        if(_setSelect())
        {
            // 关闭定时器
            if(UINT_MAX != m_nScanTimerID)
            {
                KillTimer(m_nScanTimerID);
                m_nScanTimerID = UINT_MAX;
            }

            CEventProc::GetInstance()->ShowPage(IDD_GOOSE_REALTIME_DLG);
        }

        break;
	case ID_MENU_SCAN_FT3_DLG:
		{
			// 进入FT3页面
			CEventProc::GetInstance()->ShowPage(IDD_SCAN_SMV_DLG);
		}
		break;
	case ID_MENU_SCAN_GOOSE_DLG:
		break;
	case ID_MENU_SCAN_SMV_DLG:
		{
			// 进入9-1/9-2 smv探测页面
			CEventProc::GetInstance()->ShowPage(IDD_SCAN_SMV_DLG);
		}
    default:
        break;
    }
}

void CGooseScanDlg::OnItemChecked( NMHDR* pNotifyStruct, LRESULT* pResult )
{
    ELNM_RECORDITEM_ARGS* pNS = (ELNM_RECORDITEM_ARGS*)pNotifyStruct;
    ASSERT(pNS);

    CDsmScanGooseRecord* pRecord = dynamic_cast<CDsmScanGooseRecord*>(m_reportGoose.GetRecordRow(pNS->rowIndex));
    ASSERT(pRecord);

    *pResult = TRUE;
    CNetDetectItem* pInfo = pRecord->_GetInfo();

    // 取消选择，将从m_lstSelects列表中移除已选控制块
    if(!pNS->pItem->IsChecked())
    {
        CNetDetectItem* pTempInfo = NULL;
        POSITION prepos;
        POSITION pos = m_lstSelects.GetHeadPosition();
        while(pos)
        {
            prepos = pos;
            pTempInfo = dynamic_cast<CNetDetectItem*>(m_lstSelects.GetNext(pos));
            ASSERT(pTempInfo);

            if(pTempInfo && pTempInfo->appId == pInfo->appId && pTempInfo->enPortName == pInfo->enPortName)
            {
                m_lstSelects.RemoveAt(prepos);
                break;
            }
        }

        return;
    }

#pragma message(__FILE__ "(" DSM_STRING(__LINE__) "): 是否需要过滤不同端口过来的相同控制块")

    // 控制块超过可选数目，取消选择
    if(m_lstSelects.GetCount() >= DSM_GOOSE_CTRLBLK_MAX_NUM)
    {
        CString csTip;
        CString csFmt(dsm_stringMgr::Get(DSM_STR_GOOSE_SCAN_NUMMAX));
        csTip.Format(csFmt, DSM_GOOSE_CTRLBLK_MAX_NUM);
        CStatusMgr::SetTips(csTip);

        pNS->pItem->SetChecked(FALSE);
        return;
    }

    m_lstSelects.AddTail(pRecord->_GetInfo());
}


void CGooseScanDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    if(nIDEvent == m_nScanTimerID)
    {
        _insertRecord();
    }

    CBaseDlg::OnTimer(nIDEvent);
}


void CGooseScanDlg::_createReport()
{
    CRect rcReport;
    GetClientRect(rcReport);
    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);

    int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

    m_reportGoose.Create(DSM_WINSTYLE_REPORT_DEFAULT, rcReport, this, IDC_GOOSE_SCAN_REPORT);

    // 设置字体
    m_reportGoose.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_reportGoose.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    // 设置样式
    m_reportGoose.PostInitTemplateStyle();

    // 插入列元素
    m_reportGoose.InsertColumn(0, L"", EL_CENTER, DSM_GOOSE_SCAN_HEAD_CHECKBOX);   // checkbox
    m_reportGoose.InsertColumn(1, dsm_stringMgr::Get(DSM_STR_GOOSE_SCAN_HEAD_INDEX), EL_CENTER, DSM_GOOSE_SCAN_HEAD_INDEX);     // 序号
    m_reportGoose.InsertColumn(2, dsm_stringMgr::Get(DSM_STR_GOOSE_SCAN_HEAD_APPID), EL_CENTER, DSM_GOOSE_SCAN_HEAD_APPID);     // APPID
    m_reportGoose.InsertColumn(3, dsm_stringMgr::Get(DSM_STR_GOOSE_SCAN_HEAD_DESC), EL_LEFT, DSM_GOOSE_SCAN_HEAD_DESC);         // 描述
    m_reportGoose.InsertColumn(4, dsm_stringMgr::Get(DSM_STR_GOOSE_SCAN_HEAD_PORT), EL_LEFT, DSM_GOOSE_SCAN_HEAD_PORT);       // 端口

    // 设置选择焦点样式
    m_reportGoose.SetRowSelectionVisible(TRUE);
    m_reportGoose.SetRowFocusEnable(TRUE);
    m_reportGoose.SetSubItemFocusEnable(TRUE);
    m_reportGoose.SetFreezeSelectionVisible(TRUE);
    m_reportGoose.SetFocusedColIndex(0);
}

void CGooseScanDlg::_rescan()
{
    // 清楚之前探测信息
    _clearRes();

    m_pDataLayer->BeginDetect(NET_RECV_TYPE_GOOSE);
}

void CGooseScanDlg::_clearSelect()
{
    CELRecordItem* pItem    = NULL;

    // 清空选中的控制块
    int nCounts = m_reportGoose.GetRecordRowCount();
    for(int i = 0; i < nCounts; i++)
    {
        pItem = m_reportGoose.GetRecordRow(i)->GetItem(0);

        if(pItem->IsChecked())
        {
            pItem->SetChecked(FALSE);
            m_reportGoose.RedrawRowItem(i, 0);
        }
    }

    m_lstSelects.RemoveAll();
}

bool CGooseScanDlg::_setSelect()
{
    CBaseDetectItem* pInfo  = NULL;
    POSITION pos            = m_lstSelects.GetHeadPosition();

    if(m_lstSelects.GetCount() <= 0)
    {
        return false;
    }

    while(pos)
    {
        // 设置选中的控制到数据层
        pInfo = m_lstSelects.GetNext(pos);
        if(!m_pDataLayer->AddOneSelectedCtrlBlk(pInfo))
        {
            TRACE(L"设置所选控制块失败");
            ASSERT(FALSE);
            return false;
        }
    }

    m_pDataLayer->AddSelectedCtrlBlkFinished();

    return true;
}

void CGooseScanDlg::_clearRes()
{
    if(m_reportGoose.GetSafeHwnd())
    {
        m_reportGoose.DeleteAllRecordRow();
    }

    m_lstDetects.RemoveAll();
    m_lstSelects.RemoveAll();
}

void CGooseScanDlg::_insertRecord()
{ 
    unsigned int nCounts        = m_reportGoose.GetRecordRowCount();
    unsigned int nDetectCounts  = m_pDataLayer->GetNetDetectItemsCount();

    // 若无新的探测结果则返回
    if(nDetectCounts == nCounts ||
        !m_reportGoose.GetSafeHwnd())
    {
        return;
    }

    CNetDetectItem* pInfo= NULL;

    // 插入新的记录
    for(unsigned int i = nCounts; i < nDetectCounts; i++)
    {
        pInfo = m_pDataLayer->GetNetDetectItem(i);

        if(!pInfo)
        {
            TRACE(L"dsm: get goose ctrlblk error\n");
            continue;
        }

        m_reportGoose.InsertRecordRow(i, new CDsmScanGooseRecord(i, pInfo));

        for(int nColumn = 1; nColumn < 5; nColumn++)
        {
            m_reportGoose.GetRecordRow(i)->GetItem(nColumn)->SetFocusable(FALSE);
        }

        if(0 == i)
        {
            m_reportGoose.SetFocusedSelectionRowIndex(0);
        }

        m_lstDetects.AddTail(pInfo);
    }
}

