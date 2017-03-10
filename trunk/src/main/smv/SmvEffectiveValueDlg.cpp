// SmvEffectiveValueDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SmvEffectiveValueDlg.h"
#include "src/main/resourcestr.h"
#include "src/main/eventproc.h"
#include "src/service/datalayer/dsmdatalayer.h"

#include "src/main/mgr/statusmgr.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/main/pagedata/pagedata.h"


//////////////////////////////////////////////////////////////////////////
// smv - 有效值报表
// 
#if SU_FEI
#define DSM_EFF_VAL_WIDTH_CHANNEL_INDEX     50
#define DSM_EFF_VAL_WIDTH_CHANNEL           230
#define DSM_EFF_VAL_WIDTH_EFF_VAL           160
#define DSM_EFF_VAL_WIDTH_PHASE_ANGLE       160

#else
#define DSM_EFF_VAL_WIDTH_CHANNEL_INDEX     50
#define DSM_EFF_VAL_WIDTH_CHANNEL           199
#define DSM_EFF_VAL_WIDTH_EFF_VAL           140
#define DSM_EFF_VAL_WIDTH_PHASE_ANGLE       140

#endif

/**
 *     有效值
 *
 *  \note
 *      通道 有效值 相角
 */
class CDsmEffValRecord : public CELRecord
{
public:

    CDsmEffValRecord()
    {
        AddItem(new CDsmRecordItem(L""));
        AddItem(new CDsmRecordItem(L""));
        AddItem(new CDsmRecordItem(L""));
        AddItem(new CDsmRecordItem(L""));
    }

    ~CDsmEffValRecord() {}
};

/**
 *      有效值控制块相关数据
 */
class CCtrlBlkDataEffectiveValue : public ICtrlBlkData
{
public:
    CCtrlBlkDataEffectiveValue()
    {

    }

    virtual ~CCtrlBlkDataEffectiveValue() {}

public:
};


// CSmvEffectiveValueDlg 对话框

IMPLEMENT_DYNAMIC(CSmvEffectiveValueDlg, DSM_SMVBASEDLG)

CSmvEffectiveValueDlg::CSmvEffectiveValueDlg(CWnd* pParent /*=NULL*/)
	: DSM_SMVBASEDLG(CSmvEffectiveValueDlg::IDD, DSM_STR_EFF_VAL_CAPTION, pParent)
{

}

CSmvEffectiveValueDlg::~CSmvEffectiveValueDlg()
{
}

void CSmvEffectiveValueDlg::DoDataExchange(CDataExchange* pDX)
{
	DSM_SMVBASEDLG::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSmvEffectiveValueDlg, DSM_SMVBASEDLG)
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CSmvEffectiveValueDlg 消息处理程序

BOOL CSmvEffectiveValueDlg::OnInitDialog()
{
    DSM_SMVBASEDLG::OnInitDialog();
    
    // TODO:  在此添加额外的初始化
    
    if (m_pDataLayer == NULL)
    {
        return FALSE;
    }

    if(m_pCurrentCtrlBlk)
    {
        _beginTimer();
    }
    else
    {
        return FALSE;
    }
    
    _createReport();
    _updateEffectiveValue();
    _restorePage();

    _updateCaption();

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


void CSmvEffectiveValueDlg::_createReport()
{
    CRect rcReport;
    GetClientRect(rcReport);
    

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

    m_ctlEffValReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_SMV_EFF_VAL_RET);

    // 设置字体
    m_ctlEffValReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_ctlEffValReport.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    
    m_ctlEffValReport.PostInitTemplateStyle();

    // 插入列元素

    m_ctlEffValReport.InsertColumn(0, 
        _T(""),
        EL_CENTER, DSM_EFF_VAL_WIDTH_CHANNEL_INDEX);               //通道索引

    m_ctlEffValReport.InsertColumn(1, 
        dsm_stringMgr::Get(DSM_STR_EFF_VAL_CHANNEL),
        EL_LEFT, DSM_EFF_VAL_WIDTH_CHANNEL);                     //通道

    m_ctlEffValReport.InsertColumn(2, 
        dsm_stringMgr::Get(DSM_STR_EFF_VAL_EFF_VAL), 
        EL_LEFT, DSM_EFF_VAL_WIDTH_EFF_VAL);                     //有效值

    m_ctlEffValReport.InsertColumn(3, 
        dsm_stringMgr::Get(DSM_STR_EFF_VAL_PHASE_ANGLE), 
        EL_LEFT, DSM_EFF_VAL_WIDTH_PHASE_ANGLE);                 //相角

    // 设置选择焦点样式
    m_ctlEffValReport.SetRowSelectionVisible(FALSE);
    m_ctlEffValReport.SetRowFocusEnable(FALSE);
    //m_ctlEffValReport.SetSubItemFocusEnable(FALSE);
    m_ctlEffValReport.SetFreezeSelectionVisible(TRUE);


    m_ctlEffValReport.SetFocus();

    _resetReport();
}


void CSmvEffectiveValueDlg::_resetReport()
{
    if (m_pCurrentCtrlBlk)
    {
        m_ctlEffValReport.DeleteAllRecordRow();
        for (int i = 0; i < (int)m_pCurrentCtrlBlk->nChannelCount; ++i)
        {
            m_ctlEffValReport.InsertRecordRow(i, new CDsmEffValRecord());
        }
    }
}

void CSmvEffectiveValueDlg::_updateEffectiveValue()
{
    
    if (_checkChannel())
    {
        return;
    }

    CEffectiveChanelInfo *pEffChannelInfo = NULL;

    CDsmEffValRecord* pRecord = NULL;
    if(!m_pDataLayer->GetEffectiveVal(&m_dataEffVal))
    {
        return ;
    }

    int nChannelCount = m_dataEffVal.GetChannelCount();
    if (nChannelCount == 0)
    {
        return;
    }

    CString strUnknown = _T("");
    strUnknown = dsm_stringMgr::Get(DSM_STR_EFF_VAL_UNKNOWN);

    CSmvRecvConfig* pConfig =  m_pCurrentCtrlBlk->pRecvConfig;
    if (pConfig == NULL)
    {
        return;
    }

    CString strCaption  = _T("");
    CString strEffValue  = _T("");
    CString strPhase  = _T("");
    
    float fPhaseMark = 0.0f;


    // 设置报表初始值
    CSmvRecvConfigPublic* pCfg = m_pDataLayer->GetSmvConfigPublic();
    ASSERT(pCfg);

    // 一次二次显示值
    switch(pCfg->GetSmvDispType())
    {
    case SMV_VALUE_TYPE_PRIMARY:
        {
            //
            //一次值显示
            //
            BOOL bFindFirstMark = FALSE;
            for (int i = 0; i < nChannelCount; ++i)
            {
                pEffChannelInfo = m_dataEffVal.GetOneChannelInfo(i);
                if (pEffChannelInfo != NULL)
                {
                    if (!bFindFirstMark)
                    {
                        if (pConfig->GetOneChannelInfo(i)->enChannelType == CHANNEL_TYPE_SMV_CURRENT ||
                            pConfig->GetOneChannelInfo(i)->enChannelType == CHANNEL_TYPE_SMV_VOLTAGE )
                        {
                            fPhaseMark = m_dataEffVal.GetOneChannelInfo(i)->m_fPhase;
                            bFindFirstMark = TRUE;
                        }
                    }
                    pRecord = (CDsmEffValRecord*)m_ctlEffValReport.GetRecordRow(i);
                    strCaption.Format(_T("%d"), i + 1);
                    pRecord->GetItem(0)->SetCaption(strCaption);
                    pRecord->GetItem(1)->SetCaption(m_dataEffVal.GetChannelDesc(i).c_str());

                    switch(pConfig->GetOneChannelInfo(i)->enChannelType)
                    {
                    case CHANNEL_TYPE_SMV_CURRENT:
                        strEffValue.Format(_T("%.4f A"), pEffChannelInfo->m_fEffectiveValue);
                        strPhase.Format(_T("%.4f°"), DSM_GetRelativePhase(fPhaseMark, pEffChannelInfo->m_fPhase));

                        break;
                    case CHANNEL_TYPE_SMV_VOLTAGE:
                        strEffValue.Format(_T("%.4f kV"), pEffChannelInfo->m_fEffectiveValue/1000.0);
                        strPhase.Format(_T("%.4f°"), DSM_GetRelativePhase(fPhaseMark, pEffChannelInfo->m_fPhase));

                        break;
                    case CHANNEL_TYPE_SMV_TIME:
                        strEffValue.Format(_T("%.4f μs"), pEffChannelInfo->m_fEffectiveValue);
                        strPhase.Format(_T("%.4f°"), 0);

                        break;
                    default:

                        strEffValue.Format(_T("%.4f"), pEffChannelInfo->m_fEffectiveValue);
                        strPhase.Format(_T("%.4f°"), DSM_GetRelativePhase(fPhaseMark, pEffChannelInfo->m_fPhase));
                        break;
                    }
                    pRecord->GetItem(2)->SetCaption(strEffValue);
                    pRecord->GetItem(3)->SetCaption(strPhase);
                }
                else
                {
                    //无通道数据
                    pRecord = (CDsmEffValRecord*)m_ctlEffValReport.GetRecordRow(i);
                    CString strFormat = _T("");
                    strFormat.Format(_T("%d"), i + 1);
                    pRecord->GetItem(0)->SetCaption(strFormat);
                    pRecord->GetItem(1)->SetCaption(m_dataEffVal.GetChannelDesc(i).c_str());
                    pRecord->GetItem(2)->SetCaption(strUnknown);
                    pRecord->GetItem(3)->SetCaption(strUnknown);
                }
            }
        }
        break;
    case SMV_VALUE_TYPE_SECONDARY:
        {
            //
            //二次值显示
            //
            BOOL bFindFirstMark = FALSE;
            for (int i = 0; i < nChannelCount; ++i)
            {
                pEffChannelInfo = m_dataEffVal.GetOneChannelInfo(i);
                if (pEffChannelInfo != NULL)
                {
                    if (!bFindFirstMark)
                    {
                        if (pConfig->GetOneChannelInfo(i)->enChannelType == CHANNEL_TYPE_SMV_CURRENT ||
                            pConfig->GetOneChannelInfo(i)->enChannelType == CHANNEL_TYPE_SMV_VOLTAGE )
                        {
                            fPhaseMark = m_dataEffVal.GetOneChannelInfo(i)->m_fPhase;
                            bFindFirstMark = TRUE;
                        }
                    }
                    pRecord = (CDsmEffValRecord*)m_ctlEffValReport.GetRecordRow(i);
                    strCaption.Format(_T("%d"), i + 1);
                    pRecord->GetItem(0)->SetCaption(strCaption);
                    pRecord->GetItem(1)->SetCaption(m_dataEffVal.GetChannelDesc(i).c_str());

                    switch(pConfig->GetOneChannelInfo(i)->enChannelType)
                    {
                    case CHANNEL_TYPE_SMV_CURRENT:
                        strEffValue.Format(_T("%.4f A"), pEffChannelInfo->m_fEffectiveValue);
                        strPhase.Format(_T("%.4f°"), DSM_GetRelativePhase(fPhaseMark, pEffChannelInfo->m_fPhase));

                        break;
                    case CHANNEL_TYPE_SMV_VOLTAGE:

                        strEffValue.Format(_T("%.4f V"), pEffChannelInfo->m_fEffectiveValue);
                        strPhase.Format(_T("%.4f°"), DSM_GetRelativePhase(fPhaseMark, pEffChannelInfo->m_fPhase));

                        break;
                    case CHANNEL_TYPE_SMV_TIME:
                        strEffValue.Format(_T("%.4f μs"), pEffChannelInfo->m_fEffectiveValue);
                        strPhase.Format(_T("%.4f°"), 0);
                        break;
                    default:

                        strEffValue.Format(_T("%.4f"), pEffChannelInfo->m_fEffectiveValue);
                        strPhase.Format(_T("%.4f°"), DSM_GetRelativePhase(fPhaseMark, pEffChannelInfo->m_fPhase));
                        break;
                    }
                    pRecord->GetItem(2)->SetCaption(strEffValue);
                    pRecord->GetItem(3)->SetCaption(strPhase);
                }
                else
                {
                    //无通道数据
                    pRecord = (CDsmEffValRecord*)m_ctlEffValReport.GetRecordRow(i);
                    CString strFormat = _T("");
                    strFormat.Format(_T("%d"), i + 1);
                    pRecord->GetItem(0)->SetCaption(strFormat);
                    pRecord->GetItem(1)->SetCaption(m_dataEffVal.GetChannelDesc(i).c_str());
                    pRecord->GetItem(2)->SetCaption(strUnknown);
                    pRecord->GetItem(3)->SetCaption(strUnknown);
                }
            }
        }
        break;
    case SMV_VALUE_TYPE_ORIGINANL:
        {
            //
            //原始值显示（没有单位）
            //
            BOOL bFindFirstMark = FALSE;
            for (int i = 0; i < nChannelCount; ++i)
            {
                pEffChannelInfo = m_dataEffVal.GetOneChannelInfo(i);
                if (pEffChannelInfo != NULL)
                {
                    if (!bFindFirstMark)
                    {
                        if (pConfig->GetOneChannelInfo(i)->enChannelType == CHANNEL_TYPE_SMV_CURRENT ||
                            pConfig->GetOneChannelInfo(i)->enChannelType == CHANNEL_TYPE_SMV_VOLTAGE )
                        {
                            fPhaseMark = m_dataEffVal.GetOneChannelInfo(i)->m_fPhase;
                            bFindFirstMark = TRUE;
                        }
                    }
                    pRecord = (CDsmEffValRecord*)m_ctlEffValReport.GetRecordRow(i);
                    strCaption.Format(_T("%d"), i + 1);
                    pRecord->GetItem(0)->SetCaption(strCaption);
                    pRecord->GetItem(1)->SetCaption(m_dataEffVal.GetChannelDesc(i).c_str());

                    switch(pConfig->GetOneChannelInfo(i)->enChannelType)
                    {
                    case CHANNEL_TYPE_SMV_CURRENT:
                        strEffValue.Format(_T("%.4f"), pEffChannelInfo->m_fEffectiveValue);
                        strPhase.Format(_T("%.4f°"), DSM_GetRelativePhase(fPhaseMark, pEffChannelInfo->m_fPhase));

                        break;
                    case CHANNEL_TYPE_SMV_VOLTAGE:

                        strEffValue.Format(_T("%.4f"), pEffChannelInfo->m_fEffectiveValue);

                        strPhase.Format(_T("%.4f°"), DSM_GetRelativePhase(fPhaseMark, pEffChannelInfo->m_fPhase));

                        break;
                    case CHANNEL_TYPE_SMV_TIME:
                        strEffValue.Format(_T("%.4f"), pEffChannelInfo->m_fEffectiveValue);
                        strPhase.Format(_T("%.4f°"), 0);

                        break;
                    default:

                        strEffValue.Format(_T("%.4f"), pEffChannelInfo->m_fEffectiveValue);
                        strPhase.Format(_T("%.4f°"), DSM_GetRelativePhase(fPhaseMark, pEffChannelInfo->m_fPhase));
                        break;
                    }
                    pRecord->GetItem(2)->SetCaption(strEffValue);
                    pRecord->GetItem(3)->SetCaption(strPhase);
                }
                else
                {
                    //无通道数据
                    pRecord = (CDsmEffValRecord*)m_ctlEffValReport.GetRecordRow(i);
                    CString strFormat = _T("");
                    strFormat.Format(_T("%d"), i + 1);
                    pRecord->GetItem(0)->SetCaption(strFormat);
                    pRecord->GetItem(1)->SetCaption(m_dataEffVal.GetChannelDesc(i).c_str());
                    pRecord->GetItem(2)->SetCaption(strUnknown);
                    pRecord->GetItem(3)->SetCaption(strUnknown);
                }
            }
        }
        break;
    default:
        break;
    }
    m_ctlEffValReport.RedrawWindow();

}
/**
 *      根据菜单ID切换到指定的控制块
 *
 *  /param[in]
 *      nCmdID      控制块关联的菜单命令ID
 */
bool CSmvEffectiveValueDlg::_switchCtrlBlk( UINT nCmdID )
{
    // 保存上一个控制块对象
    CBaseDetectItem* pPreCtrlBlk = m_pCurrentCtrlBlk;

    // 切换控制块
    if(!__super::_switchCtrlBlk(nCmdID))
    {
        return false;
    }

    // 若选中上次的菜单则返回
    if(pPreCtrlBlk == m_pCurrentCtrlBlk && pPreCtrlBlk->nID == m_pCurrentCtrlBlk->nID)
    {
        return true;
    }

    // 根据新控制块页面状态信息重设报表
    _resetReport();

    return true;
}
bool CSmvEffectiveValueDlg::_checkChannel()
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

            CDsmEffValRecord* pRecord = NULL;
            // 禁用通道选择并清空数据
            for(int i = 0; i < (int)m_pCurrentCtrlBlk->nChannelCount; i++)
            {
                //
                //设置通道值为 “--”
                //
                pRecord = (CDsmEffValRecord*)m_ctlEffValReport.GetRecordRow(i);

                if (pRecord)
                {
                    pRecord->GetItem(2)->SetCaption(g_csBlankValue);
                    pRecord->GetItem(3)->SetCaption(g_csBlankValue);
                }
            }
            m_ctlEffValReport.RedrawWindow();

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

            // 恢复暂停开始菜单标题
            m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, dsm_stringMgr::Get(DSM_STR_TIMER_STOP));
            m_pNaviBar->UpdateNaviMenu();
        }
    }

    return false;
}


void CSmvEffectiveValueDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    

    if (m_nTimerID == nIDEvent)
    {
        _updateEffectiveValue();
    }
    CSMVBaseDlg::OnTimer(nIDEvent);
}


void CSmvEffectiveValueDlg::_SavePage()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pPageDataExtra);

    CCtrlBlkDataEffectiveValue* pData = dynamic_cast<CCtrlBlkDataEffectiveValue*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));
    if(pData == NULL)
    {
        pData = new CCtrlBlkDataEffectiveValue();
        m_pPageDataExtra->Set(m_pCurrentCtrlBlk->nID, pData);
    }
}

bool CSmvEffectiveValueDlg::_restorePage()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pPageDataExtra);
    ASSERT(CSmvEffectiveValueDlg::IDD == m_pPageDataExtra->GetPageData()->m_nPageID);

    // 获取当前的控制块索引
    CCtrlBlkDataEffectiveValue* pData = dynamic_cast<CCtrlBlkDataEffectiveValue*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));

    // 当前控制块若没绑定相关信息，则返回
    if(!pData)
    {
        return false;
    }

    return true;
}

#if !SU_FEI
void CSmvEffectiveValueDlg::_modifyMenu()
{
	m_menuMain.SetMenuItemCaption(0, dsm_stringMgr::Get(DSM_STR_EFF_VAL_CAPTION));
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
#endif