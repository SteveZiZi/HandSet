/// @file
///
/// @brief
///     SMV通道映射 实现文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.7.18
///
/// 修改历史：
///

#include "stdafx.h"
#include "ManualSmvChanMapDlg.h"

#include "src/main/mgr/StatusMgr.h"
#include "src/service/datalayer/config/ParamconfigManualTest.h"
#include "src/service/datalayer/config/BaseCb.h"


//////////////////////////////////////////////////////////////////////////
// 手动试验- SMV映射表
//
#define DSM_MANUAL_SMV_MAP_WIDTH_CHAN            100
#define DSM_MANUAL_SMV_MAP_WIDTH_MAP_TO_SMV      229
#define DSM_MANUAL_SMV_MAP_WIDTH_TRANS_RATIO     200



// CManualSmvChanMapDlg 对话框

IMPLEMENT_DYNAMIC(CManualSmvChanMapDlg, CBaseDlg)

CManualSmvChanMapDlg::CManualSmvChanMapDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CManualSmvChanMapDlg::IDD, DSM_STR_MANUAL_SET_CAPTION, pParent)
    , m_pConfigManualTest(NULL)
{
    
    m_nMenuID = IDR_MENU_EMPTY;
}

CManualSmvChanMapDlg::~CManualSmvChanMapDlg()
{
}

void CManualSmvChanMapDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CManualSmvChanMapDlg, CBaseDlg)
    ON_NOTIFY(NM_RETURN, IDC_SMV_CHAN_MAP_TREE, OnTreeBoxClick)
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CManualSmvChanMapDlg 消息处理程序

BOOL CManualSmvChanMapDlg::OnInitDialog()
{
    CBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    m_pConfigManualTest = CParamconfigManualTest::getInstance();
    if (NULL == m_pConfigManualTest)
    {
        return FALSE;
    }

    SetTimer(1, PARAM_CFG_SAVE_TIMER, NULL);

    _createSmvChanMapTree();

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


void CManualSmvChanMapDlg::_createSmvChanMapTree()
{
    CRect rcTree;
    GetClientRect(rcTree);

    rcTree.DeflateRect(DSM_MARGIN_IN_RECT);

    int nRows = (rcTree.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcTree.bottom = rcTree.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

    m_ctlSmvChanMapTree.Create(DSM_WINSTYLE_TREE_DEFAULT, rcTree, this, IDC_SMV_CHAN_MAP_TREE);

    m_ctlSmvChanMapTree.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_ctlSmvChanMapTree.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    m_ctlSmvChanMapTree.PostInitTemplateStyle();

    m_ctlSmvChanMapTree.InsertTreeColumn(0, 
        dsm_stringMgr::Get(DSM_STR_MANUAL_CHAN), EL_LEFT, DSM_MANUAL_SMV_MAP_WIDTH_CHAN);
    m_ctlSmvChanMapTree.InsertTreeColumn(1,
        dsm_stringMgr::Get(DSM_STR_MANUAL_MAP_TO_SMV), EL_LEFT, DSM_MANUAL_SMV_MAP_WIDTH_MAP_TO_SMV);
    m_ctlSmvChanMapTree.InsertTreeColumn(2, 
        dsm_stringMgr::Get(DSM_STR_MANUAL_TRANS_RATIO), EL_LEFT, DSM_MANUAL_SMV_MAP_WIDTH_TRANS_RATIO);

    if (m_pConfigManualTest == NULL)
    {
        return;
    }    

    LBT_HTREEITEM  hDIItem = NULL;
    LBT_HTREEITEM  hChanItem = NULL;
    CChannel* pChannel = NULL;
    int nIndex = 0;
    int nSelIndex = 0;
    CString strTemp = _T("");

    // 遍历虚拟通道
    CSMVSendVirtualChannel* pVirChannel = m_pConfigManualTest->FirstSMVSendVirtualChannel();
    while(pVirChannel)
    {
        hDIItem = m_ctlSmvChanMapTree.InsertTreeItem(pVirChannel->GetName().c_str());
        m_ctlSmvChanMapTree.SetTreeItemData(hDIItem,(DWORD_PTR)pVirChannel);
        
        nIndex = 0;
        nSelIndex = pVirChannel->GetSelectedIndex();

        pChannel = pVirChannel->FirstChannel();
        while(pChannel)
        {
            strTemp.Format(_T("%d"), nIndex + 1);
            //通道
            hChanItem = m_ctlSmvChanMapTree.InsertTreeItem(strTemp, hDIItem);
            m_ctlSmvChanMapTree.SetTreeItemData(hChanItem, nIndex);
            m_ctlSmvChanMapTree.SetTreeHasCheckbox(hChanItem, TRUE);
            if (nIndex == nSelIndex)
            {
                m_ctlSmvChanMapTree.SetTreeCheck(hChanItem, TRUE);
            }
            //映射到SMV
            CString strIndex = _T("");
            if (pChannel->GetParentChannel() != NULL)
            {
                if (pChannel->GetParentChannel()->GetIndex() >= 0 && pChannel->GetIndex() >=0)
                {
                    strIndex.Format(_T("%d.%d"), pChannel->GetParentChannel()->GetIndex() + 1, pChannel->GetIndex()+1);
                }
                else
                {
                    strIndex = _T("_._");
                }
            }
            else
            {
                if (pChannel->GetIndex() >= 0)
                {
                    strIndex.Format(_T("%d"), pChannel->GetIndex()+1);
                }
                else
                {
                    strIndex = _T("_");
                }
            }
            strTemp.Format(_T("0x%04X-%s-%s"), pChannel->GetParentBaseCb()->GetAPPID(), strIndex, pChannel->GetDescription().c_str());
            m_ctlSmvChanMapTree.SetTreeItemText(hChanItem, 1, strTemp);

            //变比
            switch(pChannel->GetChannelType())
            {
            case CHANNEL_TYPE_SMV_CURRENT:
                {
                    strTemp.Format(_T("%.03lf A : %.03lf A"), pChannel->GetOnceRatings(), pChannel->GetSecondaryRatings());
                }
                break;

            case CHANNEL_TYPE_SMV_VOLTAGE:
                {
                    double dOnceValue = pChannel->GetOnceRatings();
                    dOnceValue/=1000.0;

                    strTemp.Format(_T("%.03lf kV : %.03lf V"), dOnceValue, pChannel->GetSecondaryRatings());
                }
                break;
            case CHANNEL_TYPE_SMV_TIME:
                {
                    strTemp.Format(_T("%.03lf μs : %.03lf μs"), pChannel->GetOnceRatings(), pChannel->GetSecondaryRatings());
                }
                break;
            default:
                {
                    strTemp.Format(_T("%.03lf : %.03lf "), pChannel->GetOnceRatings(), pChannel->GetSecondaryRatings());
                }
                break;
            }
            
            m_ctlSmvChanMapTree.SetTreeItemText(hChanItem, 2, strTemp);

            ++nIndex;
            pChannel = pVirChannel->NextChannel(pChannel);
        }
        pVirChannel = m_pConfigManualTest->NextSMVSendVirtualChannel(pVirChannel);
    }
    if (m_ctlSmvChanMapTree.GetCount() > 0)
    {
        m_ctlSmvChanMapTree.SetCurSel(0);
    }
    m_ctlSmvChanMapTree.ShowWindow(SW_SHOW);

#if !SU_FEI
	m_ctlSmvChanMapTree.SetTreeCheckImage(dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL),
		dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_UNSEL));
#endif
}

void CManualSmvChanMapDlg::OnTreeBoxClick(NMHDR* /*pNotifyStruct*/, LRESULT* pResult)
{
    LBT_HTREEITEM  hItem = m_ctlSmvChanMapTree.GetTreeSelectedItem();
    
    if (m_ctlSmvChanMapTree.GetTreeParentItem(hItem) == m_ctlSmvChanMapTree.GetTreeRootItem())
    {
        //
        //映射通道
        //
        //m_ctlSmvChanMapTree.ExpandTreeItem(hItem, !hItem->isExpand);
    }
    else
    {
        //
        //真实通道
        //
        if (!m_ctlSmvChanMapTree.GetTreeCheck(hItem))
        {
            CSMVSendVirtualChannel* pVirChannel = NULL;
            int nSelIndex = 0;

            LBT_HTREEITEM hParentItem = m_ctlSmvChanMapTree.GetTreeParentItem(hItem);
            pVirChannel = (CSMVSendVirtualChannel*)m_ctlSmvChanMapTree.GetTreeItemData(hParentItem);
            if (pVirChannel == NULL)
            {
                return;
            }
            nSelIndex = m_ctlSmvChanMapTree.GetTreeItemData(hItem);
            pVirChannel->SetSelectedIndex(nSelIndex);

            LBT_HTREEITEM hChildItem = m_ctlSmvChanMapTree.GetTreeChildItem(hParentItem);

            while(hChildItem != NULL)
            {
                if (hChildItem == hItem)
                {
                    m_ctlSmvChanMapTree.SetTreeCheck(hChildItem, TRUE);
                }
                else if(hChildItem != hItem && m_ctlSmvChanMapTree.GetTreeCheck(hChildItem))
                {
                    m_ctlSmvChanMapTree.SetTreeCheck(hChildItem, FALSE);
                }
                hChildItem = m_ctlSmvChanMapTree.GetTreeNextSiblingItem(hChildItem);
            }
        }
    }
    ;
    *pResult = TRUE;
}

BOOL CManualSmvChanMapDlg::DestroyWindow()
{
    // TODO: 在此添加专用代码和/或调用基类
    if (m_pConfigManualTest != NULL)
    {
        m_pConfigManualTest->SaveConfig();
    }    
    return CBaseDlg::DestroyWindow();
}

void CManualSmvChanMapDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (nIDEvent == 1)
    {    
        if (m_pConfigManualTest != NULL)
        {
            m_pConfigManualTest->SaveConfig();
        }
    }
    CBaseDlg::OnTimer(nIDEvent);
}

