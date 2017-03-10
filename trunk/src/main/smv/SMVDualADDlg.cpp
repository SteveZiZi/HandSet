/**
 * \file
 *      SMVDualADDlg.cpp
 *
 * \brief
 *      双AD页面实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/4/22
 */


// SMVDualADDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SMVDualADDlg.h"

#include "src/main/mgr/statusmgr.h"
#include "src/main/pagedata/pagedata.h"

//////////////////////////////////////////////////////////////////////////
// smv - 双AD
#if SU_FEI
#define DSM_SMV_DUALAD_AD1                  190
#define DSM_SMV_DUALAD_AD2                  190
#define DSM_SMV_DUALAD_AD1S2                110
#define DSM_SMV_DUALAD_AD2S1                110
#else
#define DSM_SMV_DUALAD_AD1                  160
#define DSM_SMV_DUALAD_AD2                  160
#define DSM_SMV_DUALAD_AD1S2                104
#define DSM_SMV_DUALAD_AD2S1                105
#endif

#define DUALAD_COUNTS           DOUBLE_AD_GROUP_COUNT       ///< 双AD组数


/**
 *      SMV接收 - 双AD
 *
 *  \note
 *      AD1、AD2、|AD1 - AD2|、|AD1| - |AD2|
 */
class CDsmSmvDualADRecord : public CELRecord
{
public:
    CDsmSmvDualADRecord(CBaseDetectItem* pCtrlBlk);

    virtual ~CDsmSmvDualADRecord() {}

};


CDsmSmvDualADRecord::CDsmSmvDualADRecord( CBaseDetectItem* pCtrlBlk )
{
    ASSERT(pCtrlBlk);

    CELRecordItem* pItemAD[2] = {NULL, NULL};

    pItemAD[0] = AddItem(new CDsmRecordItem);           // AD1
    pItemAD[1] = AddItem(new CDsmRecordItem);           // AD2

    AddItem(new CDsmRecordItem(g_csBlankValue))->SetFocusable(FALSE);    // |AD1 - AD2|
    AddItem(new CDsmRecordItem(g_csBlankValue))->SetFocusable(FALSE);    // |AD1| - |AD2|

    pItemAD[0]->SetEditable(TRUE);
    pItemAD[1]->SetEditable(TRUE);

    CSmvRecvConfig* pCfg = pCtrlBlk->pRecvConfig;
    ASSERT(pCfg);

    CSmvRecvChannelInfo* pInfo          = NULL;
    size_t nCounts                      = pCfg->GetChannelCount();
    CELRecordItemEditOptions* pOption   = NULL;

    for(int i = 0; i < 2; i++)
    {
        pOption = pItemAD[i]->GetEditOptions(NULL);
        pOption->AddComboButton();

        // 初始化可选通道的下拉列表
        for(size_t j = 0; j < nCounts; j++)
        {
            pInfo = pCfg->GetOneChannelInfo(j);

            if(CHANNEL_TYPE_SMV_VOLTAGE != pInfo->enChannelType &&
                CHANNEL_TYPE_SMV_CURRENT != pInfo->enChannelType)
            {
                continue;
            }

            pOption->AddConstraint(pInfo->strChannelDesc.c_str(), pInfo->nIndex);
        }
    }
}



/**
 *      双AD页面控制块关联数据类
 */
class CCtrlBlkDataDualAD : public ICtrlBlkData
{
public:
    CCtrlBlkDataDualAD()
        : m_nFocusRow(0)
        , m_nFocusCol(0)
    {
        for(int i = 0; i < DUALAD_COUNTS; i++)
        {
            for(int j = 0; j < 2; j++)
            {
                m_arrDualAD[i][j] = -1;
            }
        }
    }

    virtual ~CCtrlBlkDataDualAD()
    {

    }

public:
    int         m_nFocusRow;                        ///< 焦点行
    int         m_nFocusCol;                        ///< 焦点列

    int         m_arrDualAD[DUALAD_COUNTS][2];      ///< 双AD组通道号
};



// CSMVDualADDlg 对话框

IMPLEMENT_DYNAMIC(CSMVDualADDlg, DSM_SMVBASEDLG)

CSMVDualADDlg::CSMVDualADDlg(CWnd* pParent /*=NULL*/)
	: DSM_SMVBASEDLG(CSMVDualADDlg::IDD, DSM_STR_SMV_DUALAD_CAPTION, pParent)
{

}

CSMVDualADDlg::~CSMVDualADDlg()
{
    CStatusMgr::SetTips(L"", 200);
}

void CSMVDualADDlg::DoDataExchange(CDataExchange* pDX)
{
	DSM_SMVBASEDLG::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSMVDualADDlg, DSM_SMVBASEDLG)
    ON_WM_TIMER()
    ON_NOTIFY(ELNM_ITEM_CONSTRAINTCHANGED, IDC_SMV_DUALAD_REPORT, &CSMVDualADDlg::OnItemSelectChange)
    ON_NOTIFY(ELNM_INPLACEBUTTONDOWN, IDC_SMV_DUALAD_REPORT, &CSMVDualADDlg::OnInplaceButtonDown)
END_MESSAGE_MAP()


// CSMVDualADDlg 消息处理程序


BOOL CSMVDualADDlg::OnInitDialog()
{
    CSMVBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    _createReport();

    return FALSE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


/**
 *      创建双AD报表
 */
void CSMVDualADDlg::_createReport()
{
    CRect rcClient;
    GetClientRect(rcClient);
    rcClient.DeflateRect(DSM_MARGIN_IN, DSM_MARGIN_IN, DSM_MARGIN_IN, 0);
    rcClient.bottom = rcClient.top + ELT_T1_HDRHEIGHT + ELT_T1_ROWHEIGHT * (4 + 5);

    m_reportAD.Create(DSM_WINSTYLE_REPORT_DEFAULT, rcClient, this, IDC_SMV_DUALAD_REPORT);

    // 设置字体
    m_reportAD.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_reportAD.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    // 设置样式
    m_reportAD.PostInitTemplateStyle();

    // 插入列元素
    m_reportAD.InsertColumn(0, L"         " + dsm_stringMgr::Get(DSM_STR_SMV_DUALAD_HEAD_AD1), EL_LEFT, DSM_SMV_DUALAD_AD1);         // AD1
    m_reportAD.InsertColumn(1, L"         " + dsm_stringMgr::Get(DSM_STR_SMV_DUALAD_HEAD_AD2), EL_LEFT, DSM_SMV_DUALAD_AD2);         // AD2
    m_reportAD.InsertColumn(2, dsm_stringMgr::Get(DSM_STR_SMV_DUALAD_HEAD_AD1S2), EL_CENTER, DSM_SMV_DUALAD_AD1S2);     // 差的绝对值
    m_reportAD.InsertColumn(3, dsm_stringMgr::Get(DSM_STR_SMV_DUALAD_HEAD_AD2S1), EL_CENTER, DSM_SMV_DUALAD_AD2S1);     // 绝对值的差

    // 设置选择焦点样式
    m_reportAD.SetRowSelectionVisible(FALSE);
    m_reportAD.SetRowFocusEnable(TRUE);
    m_reportAD.SetSubItemFocusEnable(TRUE);
    m_reportAD.SetFreezeSelectionVisible(TRUE);
    m_reportAD.SetFocusedColIndex(0);

    // 重设报表的各项值
    _resetReport();

    m_reportAD.SetFocus();
}


/**
 *      初始化报表
 */
void CSMVDualADDlg::_resetReport()
{
    ASSERT(m_pCurrentCtrlBlk);

    _updateCaption();

    m_reportAD.DeleteAllRecordRow();

    // 插入双AD行
    for(int i = 0; i < DUALAD_COUNTS; i++)
    {
        m_reportAD.InsertRecordRow(i, new CDsmSmvDualADRecord(m_pCurrentCtrlBlk));
    }

    // 根据页面数据设置所选通道
    // 若页面数据没有相关信息，则自动调节
    if(!_restorePage())
    {
        _adjustChannel();
    }
}

/**
 *      恢复页面数据信息
 */
bool CSMVDualADDlg::_restorePage()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(CSMVDualADDlg::IDD == m_pPageDataExtra->GetPageData()->m_nPageID);

    // 获取当前的控制块索引
    CCtrlBlkDataDualAD* pData = dynamic_cast<CCtrlBlkDataDualAD*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));

    // 当前控制块若没绑定相关信息
    // 则无法根据数据恢复页面
    if(!pData)
    {
        return false;
    }

    CELRecordItem* pItem1               = NULL;
    CELRecordItem* pItem2               = NULL;
    CELRecordItemConstraint* pCon1      = NULL;
    CELRecordItemConstraint* pCon2      = NULL;

    // 设置原来的通道信息
    for(int i = 0; i < DUALAD_COUNTS; i++ )
    {
        pItem1 = m_reportAD.GetRecordRow(i)->GetItem(0);
        pItem2 = m_reportAD.GetRecordRow(i)->GetItem(1);

        ASSERT(pItem1);
        ASSERT(pItem2);

        pCon1 = pItem1->GetEditOptions(NULL)->FindConstraint(pData->m_arrDualAD[i][0]);
        pCon2 = pItem2->GetEditOptions(NULL)->FindConstraint(pData->m_arrDualAD[i][1]);

        if(pCon1 && pCon2)
        {
            m_reportAD.SetRowItemText(i, 0, pCon1->GetConstraintCaption());
            m_reportAD.SetRowItemText(i, 1, pCon2->GetConstraintCaption());

            m_data.SetAD1Channel(i, pData->m_arrDualAD[i][0]);
            m_data.SetAD2Channel(i, pData->m_arrDualAD[i][1]);
        }
        else
        {
            m_reportAD.SetRowItemText(i, 0, g_csBlankValue);
            m_reportAD.SetRowItemText(i, 1, g_csBlankValue);

            m_data.SetAD1Channel(i, UINT_MAX);
            m_data.SetAD2Channel(i, UINT_MAX);
        }
    }

    if(pData->m_nFocusRow >= 0 && pData->m_nFocusRow < m_reportAD.GetRecordRowCount())
    {
        m_reportAD.SetFocusedSelectionRowIndex(pData->m_nFocusRow);
        m_reportAD.SetFocusedColIndex(pData->m_nFocusCol);
    }

    return true;
}


/**
 *      调整默认显示通道
 */
void CSMVDualADDlg::_adjustChannel()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pPageDataExtra);

    // 获取当前的控制块索引
    CCtrlBlkDataDualAD* pData = dynamic_cast<CCtrlBlkDataDualAD*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));

    // 获取控制块绑定数据，若为空，则新建一个数据
    if(!pData)
    {
        pData = new CCtrlBlkDataDualAD;
        m_pPageDataExtra->Set(m_pCurrentCtrlBlk->nID, pData);

        pData = dynamic_cast<CCtrlBlkDataDualAD*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));
    }

    // 寻找合适的默认显示通道
    _calcChannel(pData);

    CSmvRecvChannelInfo* pInfo1 = NULL;
    CSmvRecvChannelInfo* pInfo2 = NULL;
    CSmvRecvConfig* pCfg        = m_pCurrentCtrlBlk->pRecvConfig;

    // 设置默认通道到页面数据中
    for(int i = 0; i < DUALAD_COUNTS; i++)
    {
        pInfo1 = pCfg->GetOneChannelInfo(pData->m_arrDualAD[i][0]);
        pInfo2 = pCfg->GetOneChannelInfo(pData->m_arrDualAD[i][1]);

        if(pInfo1 && pInfo2)
        {
            m_reportAD.SetRowItemText(i, 0, pInfo1->strChannelDesc.c_str());
            m_reportAD.SetRowItemText(i, 1, pInfo2->strChannelDesc.c_str());
        }
        else
        {
            m_reportAD.SetRowItemText(i, 0, g_csBlankValue);
            m_reportAD.SetRowItemText(i, 1, g_csBlankValue);
        }
    }

    pData->m_nFocusRow = 0;
    pData->m_nFocusCol = 0;


    if(m_reportAD.GetRecordRowCount() > 0)
    {
        m_reportAD.SetFocusedSelectionRowIndex(pData->m_nFocusRow);
        m_reportAD.SetFocusedColIndex(pData->m_nFocusCol);
    }
}

/**
 *      寻找默认显示通道
 *
 *  \param[in]
 *      控制块关联的页面状态数据对象
 *
 *  \note
 *      相邻并相序一样则为一组
 */
void CSMVDualADDlg::_calcChannel( CCtrlBlkDataDualAD* pData )
{
    ASSERT(pData);
    int nIndex                  = 0;
    CSmvRecvConfig* pCfg        = m_pCurrentCtrlBlk->pRecvConfig;
    size_t nCounts              = pCfg->GetChannelCount();
    CSmvRecvChannelInfo* pInfo1  = NULL;
    CSmvRecvChannelInfo* pInfo2  = NULL;

    // 简单查找双AD组
    for(size_t i = 0; i < nCounts && nIndex < DUALAD_COUNTS; i++)
    {
        pInfo1 = pCfg->GetOneChannelInfo(i);
        ASSERT(pInfo1);

        if(CHANNEL_TYPE_SMV_CURRENT == pInfo1->enChannelType || CHANNEL_TYPE_SMV_VOLTAGE == pInfo1->enChannelType)
        {
            pInfo2 = pCfg->GetOneChannelInfo(i + 1);

            // 找到一组
            if(pInfo2 && pInfo1->enChannelType == pInfo2->enChannelType)
            {
                pData->m_arrDualAD[nIndex][0] = pInfo1->nIndex;
                pData->m_arrDualAD[nIndex][1] = pInfo2->nIndex;

                // 向数据层设置通道信息
                m_data.SetAD1Channel(nIndex, pInfo1->nIndex);
                m_data.SetAD2Channel(nIndex, pInfo2->nIndex);

                nIndex++;
                i++;
                continue;
            }
        }
    }

    // 还有剩余组数的通道号未确认，留空

    for(int i = nIndex; i < DUALAD_COUNTS; i++)
    {
        // 向数据层设置通道信息
        m_data.SetAD1Channel(i, UINT_MAX);
        m_data.SetAD2Channel(i, UINT_MAX);
    }
}


/**
 *      保存页面
 */
void CSMVDualADDlg::_SavePage()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pPageDataExtra);

    CCtrlBlkDataDualAD* pData = dynamic_cast<CCtrlBlkDataDualAD*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));

    // 保存焦点的行列值
    if(pData)
    {
        pData->m_nFocusRow = m_reportAD.GetFocusedRowIndex();
        pData->m_nFocusCol = m_reportAD.GetFocusedColIndex();
    }
}

/**
 *      更新界面数据
 */
void CSMVDualADDlg::_update()
{
    ASSERT(m_pCurrentCtrlBlk);

    if(_checkChannel())
    {
        return;
    }

    // 获取双AD数据
    if(!m_pDataLayer->GetDoubleAdData(&m_data))
    {
        for(int i = 0; i < DUALAD_COUNTS; i++)
        {
            m_reportAD.SetRowItemText(i, 2, g_csBlankValue);
            m_reportAD.SetRowItemText(i, 3, g_csBlankValue);
        }

        return;
    }

    float fVal1 = 0.0f;
    float fVal2 = 0.0f;
    CString csVal1, csVal2;

    for(int i = 0; i < DUALAD_COUNTS; i++)
    {
        if(m_data.GetADGroupResult(i, fVal1, fVal2))
        {
            csVal1.Format(L"%0.3f", fVal1);
            csVal2.Format(L"%0.3f", fVal2);

            m_reportAD.SetRowItemText(i, 2, csVal1);
            m_reportAD.SetRowItemText(i, 3, csVal2);
        }
        else
        {
            m_reportAD.SetRowItemText(i, 2, g_csBlankValue);
            m_reportAD.SetRowItemText(i, 3, g_csBlankValue);
        }
    }
}

void CSMVDualADDlg::OnTimer( UINT_PTR nIDEvent )
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if(nIDEvent == m_nTimerID)
    {
        // 定时刷新界面
        _update();
    }

    CSMVBaseDlg::OnTimer(nIDEvent);
}


void CSMVDualADDlg::OnItemSelectChange( NMHDR* pNotifyStruct, LRESULT * pResult )
{
    ASSERT(m_pPageDataExtra);

    ELNM_INPLACELIST_ARGS* pItemNotify = (ELNM_INPLACELIST_ARGS*)pNotifyStruct;
    ASSERT(pItemNotify);

    if(ELNM_ITEM_CONSTRAINTCHANGED == pItemNotify->hdr.code)
    {
        CCtrlBlkDataDualAD* pData = dynamic_cast<CCtrlBlkDataDualAD*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));
        ASSERT(pData);

        // 更改AD1
        if(0 == pItemNotify->colIndex)
        {
            m_data.SetAD1Channel(pItemNotify->rowIndex, pItemNotify->pConstraint->GetConstraintData());
        }
        // 更改AD2
        else if(1 == pItemNotify->colIndex)
        {
            m_data.SetAD2Channel(pItemNotify->rowIndex, pItemNotify->pConstraint->GetConstraintData());
        }
        else
        {
            *pResult = FALSE;
            return;
        }

        // 将改变信息存入控制块关联的页面状态数据中
        pData->m_arrDualAD[pItemNotify->rowIndex][pItemNotify->colIndex] = pItemNotify->pConstraint->GetConstraintData();

        *pResult = TRUE;
    }
}

/**
 *      根据菜单ID切换到指定的控制块
 *
 *  \param[in]
 *      nCmdID          控制块映射的命令菜单ID
 */
bool CSMVDualADDlg::_switchCtrlBlk( UINT nCmdID )
{
    // 保存切换前的控制块对象
    CBaseDetectItem* pPreCtrlBlk = m_pCurrentCtrlBlk;

    if(!__super::_switchCtrlBlk(nCmdID))
    {
        return false;
    }

    // 若选中上次的菜单则返回
    if(pPreCtrlBlk == m_pCurrentCtrlBlk && pPreCtrlBlk->nID == m_pCurrentCtrlBlk->nID)
    {
        return true;
    }

    // 设置原控制块的通道焦点
    CCtrlBlkDataDualAD* pData = dynamic_cast<CCtrlBlkDataDualAD*>(m_pPageDataExtra->Get(pPreCtrlBlk->nID));

    if(pData)
    {
        pData->m_nFocusRow = m_reportAD.GetFocusedRowIndex();
        pData->m_nFocusCol = m_reportAD.GetFocusedColIndex();
    }

    // 切换控制块后，重新设置报表（加载通道之类的）
    _resetReport();

    return true;
}

/**
 *      检查通道是否发生变化
 */
bool CSMVDualADDlg::_checkChannel()
{
    // 通道发生变化，
    if(m_pCurrentCtrlBlk->nChannelCount != m_pDataLayer->GetCurrentCtrlBlkRealTimeChannelCounts())
    {
        if(CStatusMgr::ShowTipsStyle() == 0x00)
        {
            // 界面tips提示
            CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_CHANNEL_NUM_CHANGE));
        }

        if(!m_bChannelChange)
        {
            m_bChannelChange = true;

            // 禁用通道选择并清空数据
            for(int i = 0; i < DUALAD_COUNTS; i++)
            {
                m_reportAD.SetRowItemText(i, 2, g_csBlankValue);
                m_reportAD.SetRowItemText(i, 3, g_csBlankValue);

                m_reportAD.GetRecordRow(i)->GetItem(0)->SetEditable(FALSE);
                m_reportAD.GetRecordRow(i)->GetItem(1)->SetEditable(FALSE);
            }

            // 隐藏『暂停开始』菜单标题
            m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, L"");
            m_pNaviBar->UpdateNaviMenu();
        }

        return true;
    }
    else
    {
        if(m_bChannelChange)
        {
            m_bChannelChange = false;
            CStatusMgr::SetTips(L"", 0);

            // 恢复通道选择功能
            for(int i = 0; i < DUALAD_COUNTS; i++)
            {
                m_reportAD.GetRecordRow(i)->GetItem(0)->SetEditable(TRUE);
                m_reportAD.GetRecordRow(i)->GetItem(1)->SetEditable(TRUE);
            }

            // 恢复暂停开始菜单标题
            m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, dsm_stringMgr::Get(DSM_STR_TIMER_STOP));
            m_pNaviBar->UpdateNaviMenu();
        }
    }

    return false;
}

void CSMVDualADDlg::OnInplaceButtonDown( NMHDR* , LRESULT *pResult )
{
    if(!m_bEnableTimer)
    {
        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_TIMER_SW_CHAN_TIPS));
        *pResult = TRUE;
    }
}

void CSMVDualADDlg::_modifyMenu()
{

	m_menuMain.SetMenuItemCaption(0, dsm_stringMgr::Get(DSM_STR_SMV_DUALAD_CAPTION));
	m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);
	m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);

	//需要根据设置中选中的一次值还是二次值进行判断，才能正常显示
	CSmvRecvConfigPublic* pCfg = m_pDataLayer->GetSmvConfigPublic();

	if (pCfg->GetSmvDispType() == SMV_VALUE_TYPE_SECONDARY)
	{
		m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_SECOND_SEL), NULL, NULL);
	}
	else if (pCfg->GetSmvDispType() == SMV_VALUE_TYPE_PRIMARY)
	{
		m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_FIRST_SEL), NULL, NULL);
	}
}