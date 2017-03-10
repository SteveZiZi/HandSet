/// @file
///
/// @brief
///     状态序列状态列表 实现文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.7.18
///
/// 修改历史：
///

#include "stdafx.h"
#include "StateSequenceDlg.h"
#include "StateSetDlg.h"
#include "src/main/eventproc.h"
#include "src/main/mgr/statusmgr.h"
#include "src/main/resourcebmp.h"
#include "src/service/datalayer/config/ParamConfigStateSequence.h"
#include "src/service/datalayer/dsmdatalayer.h"
#include "src/utils/utility/dsm_utility.h"
//#include "StateSwitchStatusConfirmDlg.h"
#include "src/main/popdlg/MsgBtnDlg.h"


//////////////////////////////////////////////////////////////////////////
//状态序列 - 状态序列
//
#if SHENG_ZE
#define DSM_STATE_STATE_WIDTH_INDEX            20
#define DSM_STATE_STATE_WIDTH_STATE_SET        160
#define DSM_STATE_STATE_WIDTH_STATE_DATA       349
#else
#define DSM_STATE_STATE_WIDTH_INDEX            27
#define DSM_STATE_STATE_WIDTH_STATE_SET        153
#define DSM_STATE_STATE_WIDTH_STATE_DATA       349
#endif
// msg 页面
#define DSM_STATE_TIP_DLG_RC          CRect(0, 333, 640, 441)     ///< 页面区域
#if SHENG_ZE
#define DSM_STATE_TIP_DLG_RCTEXT      CRect(100, 15, 620, 95)     ///< 文字区域
#else
#define DSM_STATE_TIP_DLG_RCTEXT      CRect(75, 15, 620, 95)     ///< 文字区域
#endif
// CStateSequenceDlg 对话框

IMPLEMENT_DYNAMIC(CStateSequenceDlg, CBaseDlg)

CStateSequenceDlg::CStateSequenceDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CStateSequenceDlg::IDD, DSM_STR_STATE_STATE_CAPTION, pParent)
    , m_pDIDOStateDlg(NULL)
    , m_bTesting(FALSE)
    , m_nTestStateIndex(0)
    , m_pConfigStateSequ(NULL)
{
    m_nMenuID = IDR_MENU_STATE_SEQUENCE;
}

CStateSequenceDlg::~CStateSequenceDlg()
{

}

void CStateSequenceDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CStateSequenceDlg, CBaseDlg)
    ON_COMMAND_RANGE(ID_MENU_STATE_SEQUENCE_CMD_MIN,   ID_MENU_STATE_SEQUENCE_CMD_MAX, &CStateSequenceDlg::OnNaviMenuStateSequence)
    ON_UPDATE_COMMAND_UI_RANGE(ID_MENU_STATE_SEQUENCE_CMD_MIN,   ID_MENU_STATE_SEQUENCE_CMD_MAX, &CStateSequenceDlg::OnUpdateNaviMenuStateSequence)
    ON_NOTIFY(ELNM_OKCLICK, IDC_STATE_SEQUENCE_RET, &CStateSequenceDlg::OnStateOkClick)
    ON_MESSAGE(WM_DI_CHANGED, &CStateSequenceDlg::OnDIChanged)
    ON_MESSAGE(WM_WHETHER_SWITCH, &CStateSequenceDlg::OnWhetherSwitch)
    ON_MESSAGE(WM_STATUS_CHANGED, &CStateSequenceDlg::OnStatusChanged)
    ON_MESSAGE(WM_STATUS_SEQUENCE_RESULT, &CStateSequenceDlg::OnTestCompleted)
    ON_WM_TIMER()
    ON_WM_DESTROY()
END_MESSAGE_MAP()


// CStateSequenceDlg 消息处理程序

BOOL CStateSequenceDlg::OnInitDialog()
{
    CBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化
    
    m_pConfigStateSequ = CParamConfigStateSequence::getInstance();
    if (m_pConfigStateSequ == NULL)
    {
        return FALSE;
    }


    SetTimer(1, PARAM_CFG_SAVE_TIMER, NULL);
    _createReport();

    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();
    if(!pData || CStateSequenceDlg::IDD != pData->m_nPageID)
    {
        return FALSE;
    }
    if(pData->m_eAction == PageBack)
    {
        _restorePage();
    }
    else
    {
        if (m_ctlStateSequReport.GetRecordRowCount() > 0)
        {
            m_ctlStateSequReport.SetFocusedSelectionRowIndex(0);
        }
    }


    if (m_pConfigStateSequ->GetIsShowDI())
    {
        _showDIDO(TRUE);
    }

	if (_isDIDOShowing())
	{
		m_pNaviBar->GetNaviMenu()->GetSubMenu(5)->SetMenuItemCaption(3,_T("隐藏开入量"));
	}
	else
	{
		m_pNaviBar->GetNaviMenu()->GetSubMenu(5)->SetMenuItemCaption(3,_T("显示开入量"));
	}
    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


void CStateSequenceDlg::_createReport()
{
    ASSERT(m_pConfigStateSequ);

    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

    m_ctlStateSequReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_STATE_SEQUENCE_RET);
    
    // 设置字体
    m_ctlStateSequReport.SetFont (dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_ctlStateSequReport.SetHeaderFont (dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    
    m_ctlStateSequReport.PostInitTemplateStyle();

    // 插入列元素
    m_ctlStateSequReport.InsertColumn(0, _T(""),                                       EL_CENTER, DSM_STATE_STATE_WIDTH_INDEX);
    m_ctlStateSequReport.InsertColumn(1, dsm_stringMgr::Get(DSM_STR_STATE_STATE_SET),  EL_LEFT, DSM_STATE_STATE_WIDTH_STATE_SET);
    m_ctlStateSequReport.InsertColumn(2, dsm_stringMgr::Get(DSM_STR_STATE_STATE_DATA), EL_LEFT, DSM_STATE_STATE_WIDTH_STATE_DATA);

    // 设置选择焦点样式
    m_ctlStateSequReport.SetRowSelectionVisible(TRUE);
    m_ctlStateSequReport.SetRowFocusEnable(TRUE);

    int nIndex = 0;

    CStatus* pStatus = m_pConfigStateSequ->FirstStatus();
    while(pStatus)
    {
        m_ctlStateSequReport.InsertRecordRow(nIndex, new CDsmStateSequenceRecord(nIndex + 1, pStatus));
        m_ctlStateSequReport.SetRecordRowData(nIndex, (DWORD_PTR)pStatus);

        pStatus = m_pConfigStateSequ->NextStatus(pStatus);
        ++nIndex;
    }

    m_ctlStateSequReport.GetColumnExAttr(2)->GetEditOptions()->SetAllowEdit(TRUE);
    m_ctlStateSequReport.SetFocus();
    m_ctlStateSequReport.ShowWindow(SW_SHOW);

	m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(5, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);
	m_pNaviBar->UpdateNaviMenu();
}

void CStateSequenceDlg::_updateReport()
{
    
    //状态设置
    CString strSet = _T("");
    //状态数据
    CString strStateData = _T("");
    CString strTemp =  _T("");
    CString strChanName = _T("");
    CELRecordItemText* pItem = NULL;
    CStatus* pStatus = NULL;
    for (int i = 0; i < m_ctlStateSequReport.GetRecordRowCount(); ++i)
    {
        pStatus = (CStatus*)m_ctlStateSequReport.GetRecordRowData(i);
        if (pStatus == NULL)
        {
            continue;
        }
        strSet = _T("");
        strStateData = _T("");
        //
        //更新设置
        //
        eSwitchType switchType = pStatus->GetSwitchType();
        switch (switchType)
        {
        case SWITCH_TYPE_TIME:
            {
                strSet.Format(_T("%s : %d ms"),
                    dsm_stringMgr::Get(DSM_STR_STATE_SET_CT_TIME_CHANGE),
                    pStatus->GetDelay());
            };
            break;
        case SWITCH_TYPE_DI:
            {
                CString strDIChangeType = _T("");
                switch(pStatus->GetLogicalOperator())
                {
                case LOGICAL_AND:
                    {
                        strDIChangeType = dsm_stringMgr::Get(DSM_STR_STATE_SET_DI_CT_AND);
                    }
                    break;
                case LOGICAL_OR:
                    {
                        strDIChangeType = dsm_stringMgr::Get(DSM_STR_STATE_SET_DI_CT_OR);
                    }
                    break;
                default:
                    strDIChangeType = dsm_stringMgr::Get(DSM_STR_STATE_UNKNOWN);
                    break;
                }
                strSet.Format(_T("%s:%s"), dsm_stringMgr::Get(DSM_STR_STATE_SET_CT_DI_CHANGE),
                    strDIChangeType);
            };
            break;
        case SWITCH_TYPE_MANUAL:
            {
                strSet.Format(_T("%s"), dsm_stringMgr::Get(DSM_STR_STATE_SET_CT_MANUAL_CHANGE));
            };
            break;
        }
        pItem = dynamic_cast<CELRecordItemText*>(m_ctlStateSequReport.GetRecordRow(i)->GetItem(1));
        if (pItem)
        {
            pItem->SetValue(strSet);
        }

        //
        //
        //
        CSMVSendVirtualChannel* pVirChannel = pStatus->FirstSMVSendVirtualChannel();
        while(pVirChannel)
        {
            strChanName = pVirChannel->GetName().c_str();
            if (strChanName.GetLength() > 0)
            {
                if (strChanName[0] == _T('U'))
                {
                    //电压
                    strTemp.Format(_T("%s=%.03lf V,"), strChanName, pVirChannel->GetAmplitude());
                }
                else if (strChanName[0] == _T('I'))
                {
                    //电流
                    strTemp.Format(_T("%s=%.03lf A,"), strChanName, pVirChannel->GetAmplitude());
                }
                else if (strChanName[0] == _T('t'))
                {
                    //时间
                    strTemp.Format(_T("%s=%.03lf μs,"), strChanName, pVirChannel->GetAmplitude());
                }
                strStateData += strTemp;
            }
            pVirChannel = pStatus->NextSMVSendVirtualChannel(pVirChannel);
        }
        pItem = dynamic_cast<CELRecordItemText*>(m_ctlStateSequReport.GetRecordRow(i)->GetItem(2));
        if (pItem)
        {
            pItem->SetValue(strStateData);
        }
        m_ctlStateSequReport.RedrawRowItem(i, 1);
        m_ctlStateSequReport.RedrawRowItem(i, 2);
    }
}


void CStateSequenceDlg::_addStatus()
{
    ASSERT(m_pConfigStateSequ);

    if (m_pConfigStateSequ->GetStatusCount() >= g_nSSStateSequenceCountMax)
    {
        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_STATE_COUNT_MAX), 3000);
        return;
    }
    CStatus* pStatus = m_pConfigStateSequ->AddStatus();
    if (pStatus != NULL)
    {
        int nIndex = m_ctlStateSequReport.GetRecordRowCount();
        m_ctlStateSequReport.InsertRecordRow(nIndex, new CDsmStateSequenceRecord(nIndex + 1, pStatus));
        m_ctlStateSequReport.SetRecordRowData(nIndex, (DWORD_PTR)pStatus);
        m_ctlStateSequReport.SetFocusedSelectionRowIndex(nIndex);
        m_ctlStateSequReport.EnsureVisible(nIndex, FALSE);
    }
}

void CStateSequenceDlg::_editStatus()
{
	
	int nRowIndex = m_ctlStateSequReport.GetFocusedRowIndex();
	if (nRowIndex < 0)
	{
		return;
	}
	CStatus* pStatus = (CStatus*)m_ctlStateSequReport.GetRecordRowData(nRowIndex);
	if (pStatus)
	{
		if (!m_bTesting)
		{
			//试验中不能进入编辑状态页面
			CEventProc::GetInstance()->ShowPage(IDD_STATE_SET_DLG);
		}
	}
}

void CStateSequenceDlg::_clearStatus()
{

	ASSERT(m_pConfigStateSequ);

//	CMsgDlg msgdlg(DSM_STR_INFO_REFACTORY_CONFIRM, 0, this);
	CMsgBtnDlg msgdlg(DSM_STR_INFO_REFACTORY_CONFIRM, DSM_BMP_WARNING_ICON, this);

	msgdlg._SetMsg(_T("警告：删除所有状态？"));
#if SHENG_ZE
	msgdlg._SetBitmapDisp(false);
		msgdlg._SetTextRC( CRect(90, 45, 320, 95));
#else
	msgdlg._SetBitmapDisp(true);
	msgdlg._SetTextRC(CRect(151, 51, 314, 71));
#endif

	//msgdlg._ResetF5Text(_T("取消"));
	//msgdlg._ResetF6Text(_T("确定"));
	HWND hWnd = ::GetFocus();
	INT_PTR nRet = msgdlg.DoModal();
	msgdlg._ResetMenu();

	if(nRet == IDOK)
	{
		m_pConfigStateSequ->ClearAllStatus();
		m_ctlStateSequReport.DeleteAllRecordRow();
	}
	::SetFocus(hWnd);
	
}

void CStateSequenceDlg::_deleteStatus()
{
    ASSERT(m_pConfigStateSequ);
    
    m_ctlStateSequReport.SetRedraw(FALSE);
    int nSelIndex = m_ctlStateSequReport.GetFocusedRowIndex();
    if (nSelIndex >=0)
    {
        CStatus* pStatus = (CStatus*)m_ctlStateSequReport.GetRecordRowData(nSelIndex);
        if (pStatus != NULL)
        {
            m_pConfigStateSequ->DeleteStatus(pStatus);
            m_ctlStateSequReport.DeleteRecordRow(nSelIndex);
            
            CString strIndex;
            for (int i = 0; i < m_ctlStateSequReport.GetRecordRowCount(); ++i)
            {
                strIndex.Format(_T("%d"), i +1);
                CELRecordItemText* pItem = dynamic_cast<CELRecordItemText*>(m_ctlStateSequReport.GetRecordRow(i)->GetItem(0));
                if (pItem)
                {
                    pItem->SetValue(strIndex);
                }
                m_ctlStateSequReport.RedrawRowItem(i, 0);
            }
        }
    }
    m_ctlStateSequReport.SetRedraw(TRUE);
}

void CStateSequenceDlg::_updateTestIndex()
{ 
    int nRowCount = m_ctlStateSequReport.GetRecordRowCount();
    CString strIndex = _T("");
    CELRecordItemText* pItem = NULL;

    if (m_bTesting)
    {
        for (int i = 0; i < nRowCount; ++i)
        {
            if (i != m_nTestStateIndex)
            {
                //
                //执行完的状态索引改为正常格式，去掉->
                //
                pItem = dynamic_cast<CELRecordItemText*>(m_ctlStateSequReport.GetRecordRow(i)->GetItem(0));
                if (pItem)
                {
                    strIndex.Format(_T("%d"), i + 1);
                    pItem->SetValue(strIndex);
                    m_ctlStateSequReport.RedrawRowItem(i, 0);
                }
            }
            else
            {
                //
                //待执行的下一状态索引改为 ->3 格式
                //
                m_ctlStateSequReport.SetFocusedSelectionRowIndex(m_nTestStateIndex);

                pItem = dynamic_cast<CELRecordItemText*>(m_ctlStateSequReport.GetRecordRow(m_nTestStateIndex)->GetItem(0));
                if (pItem)
                {
                    strIndex.Format(_T("%s%d"), dsm_stringMgr::Get(DSM_STR_STATE_CUR_STATE_FLAG), m_nTestStateIndex + 1);
                    pItem->SetValue(strIndex);
                    m_ctlStateSequReport.EnsureVisible(m_nTestStateIndex, FALSE);
                    m_ctlStateSequReport.RedrawRowItem(m_nTestStateIndex, 0);
                }
                _updateDOStatus(m_nTestStateIndex);
            }
        }
    }
    else
    {
        //
        //执行完的状态索引改为正常格式
        //
        if (m_nTestStateIndex >= 0 && m_nTestStateIndex < nRowCount)
        {
            pItem = dynamic_cast<CELRecordItemText*>(m_ctlStateSequReport.GetRecordRow(m_nTestStateIndex)->GetItem(0));
            if (pItem)
            {
                strIndex.Format(_T("%d"), m_nTestStateIndex + 1);
                pItem->SetValue(strIndex);
                m_ctlStateSequReport.RedrawRowItem(m_nTestStateIndex, 0);
            }
        }
    }
}
void CStateSequenceDlg::_resetMenu()
{
    if ( m_bTesting)
    {
        m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(0, dsm_stringMgr::Get(DSM_STR_STATE_STOP_TEST));
		m_pNaviBar->GetNaviMenu()->GetSubMenu(5)->SetMenuItemCaption(3,_T("隐藏开入量"));
    }
    else
    {
        m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(0, dsm_stringMgr::Get(DSM_STR_STATE_START_TEST));
    }

}

void CStateSequenceDlg::OnNaviMenuStateSequence(UINT nID)
{
    switch (nID)
    {
    case ID_MENU_STATE_START_TEST:
        {

            ASSERT(m_pConfigStateSequ);
            CDsmDataLayer* pDataLayer = CDsmDataLayer::getInstance();
            if (pDataLayer == NULL)
            {
                break;
            }
            m_nTestStateIndex = 0;
            m_bTesting = !m_bTesting;


            if (m_bTesting)
            {
                if (m_pConfigStateSequ->GetStatusCount() == 0)
                {
                    CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_STATE_NO_STATE_TIPS));
                    m_bTesting = FALSE;
                    return;
                }
                if (!_isDIDOShowing())
                {
                    _showDIDO(TRUE);
                }
                _resetMenu();
                pDataLayer->StartStateSequenceTest(this->GetSafeHwnd());
                _updateTestIndex();
            }
            else
            {
                pDataLayer->StopStateSequenceTest();
            }
        }
        break;
    case ID_MENU_STATE_ADD_STATE:
        {
            _addStatus();
        }
        break;
	case ID_MENU_STATE_EDIT_STATE:
		{
			_editStatus();
		}
		break;
    case ID_MENU_STATE_DELETE_STATE:
        {
            _deleteStatus();
        }
        break;
	case ID_MENU_STATE_CLEAR_STATE:
		{
			_clearStatus();
		}
		break;
    case ID_MENU_STATE_SHOW_SMV_REP:
        {
            CEventProc::GetInstance()->ShowPage(IDD_MANUAL_SMV_CHAN_MAP_DLG);
        }
        break;
    case ID_MENU_STATE_SHOW_DI_REP:
        {
            CEventProc::GetInstance()->ShowPage(IDD_BASIC_PARAM_DI_MAP_DLG);
        }
        break;
    case ID_MENU_STATE_SHOW_RESULT:
        {
            CEventProc::GetInstance()->ShowPage(IDD_STATE_RESULT_LIST_DLG);
        }
        break;
    case ID_MENU_STATE_SHOW_DI_STATE:
        {
            //显示开入映射

            if (_isDIDOShowing())
            {
                _showDIDO(FALSE);
                m_pConfigStateSequ->SetIsShowDI(FALSE);
				m_pNaviBar->GetNaviMenu()->GetSubMenu(5)->SetMenuItemCaption(3,_T("显示开入量"));
            }
            else
            {
                _showDIDO(TRUE);
                m_pConfigStateSequ->SetIsShowDI(TRUE);
				m_pNaviBar->GetNaviMenu()->GetSubMenu(5)->SetMenuItemCaption(3,_T("隐藏开入量"));
            }
        }
        break;
    default :
        {

        }
        break;
    }
}

//改变按键的使能状态
void CStateSequenceDlg::OnUpdateNaviMenuStateSequence(CCmdUI* pCmdUI)
{
    switch (pCmdUI->m_nID)
    {
    case ID_MENU_STATE_ADD_STATE:
    case ID_MENU_STATE_DELETE_STATE:
    case ID_MENU_STATE_SHOW_SMV_REP:
    case ID_MENU_STATE_SHOW_DI_REP:
    case ID_MENU_STATE_SHOW_RESULT:
	case ID_MENU_STATE_EDIT_STATE:
	case ID_MENU_STATE_CLEAR_STATE:
        {
            pCmdUI->Enable(!m_bTesting);
        }
        break;
    default :
        {

        }
        break;
    }
}



DWORD WINAPI DSM_STATE_SEQUENCE_END_TASK(void* arg)
{
    CDialog* pDlg = (CDialog*)arg;

    pDlg->EndDialog(IDOK);

    return 0;
}

void CStateSequenceDlg::OnStateOkClick(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    UNREFERENCED_PARAMETER(pNotifyStruct);
    
    int nRowIndex = m_ctlStateSequReport.GetFocusedRowIndex();
    if (nRowIndex < 0)
    {
        return;
    }
    CStatus* pStatus = (CStatus*)m_ctlStateSequReport.GetRecordRowData(nRowIndex);
    if (pStatus)
    {
        if (!m_bTesting)
        {
            //试验中不能进入编辑状态页面
            CEventProc::GetInstance()->ShowPage(IDD_STATE_SET_DLG);
        }
    }
    *pResult = TRUE;
}


LRESULT CStateSequenceDlg::OnDIChanged( WPARAM wParam, LPARAM lParam )
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    //
    //DI数据更新
    //
       
    if (m_pDIDOStateDlg && m_pDIDOStateDlg->GetSafeHwnd())
    {
        ::SendMessage(m_pDIDOStateDlg->GetSafeHwnd(), WM_DI_CHANGED, wParam, lParam);
    }
    return 1;
}

DWORD WINAPI StateToNextProc(WPARAM wParam, LPARAM lParam, void* arg)
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    CDialog* pDlg = (CDialog*)arg;
    pDlg->EndDialog(IDOK);
    return 0;
}

LRESULT CStateSequenceDlg::OnWhetherSwitch( WPARAM wParam, LPARAM lParam )
{
    
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    //
    //手动切换确认
    //
    RETAILMSG(OUTPUT_LOG_MSG, (L"CStateSequenceDlg::OnWhetherSwitch before DoModal\n"));
    
#if SHENG_ZE
    if (_isDIDOShowing())
    {
        m_pDIDOStateDlg->MoveWindowTop();
    }
#endif

    CMsgDlg msgDlg(DSM_STR_STATE_CONFIRM_TO_NEXT_STATE, 0, this);

    msgDlg._SetMenu(IDR_MENU_STATE_TO_NEXT);
    msgDlg._SetTextRC(DSM_STATE_TIP_DLG_RCTEXT);
    msgDlg._SetBitmapDisp(false);
    msgDlg._SetCommandRange(ID_MENU_STATE_TO_NEXT_F1, ID_MENU_STATE_TO_NEXT_F1, StateToNextProc, (void*)&msgDlg);
#if !SHENG_ZE
	msgDlg._ResetF1Text(_T("确认"));
#endif
    HWND hWnd = ::GetFocus();
    msgDlg.DoModal();
    ::SetFocus(hWnd);

    if (_isDIDOShowing())
    {
        m_pDIDOStateDlg->MoveWindowBottom();
    }

    return 1;
}

LRESULT CStateSequenceDlg::OnStatusChanged( WPARAM wParam, LPARAM lParam )
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    m_nTestStateIndex = (int)wParam;
    _updateTestIndex();
    return 1;
}

LRESULT CStateSequenceDlg::OnTestCompleted( WPARAM wParam, LPARAM lParam )
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    
    _updateDOStatus(-1);
    m_pConfigStateSequ->SetIsShowDI(false);
    m_bTesting = FALSE;

    enumErrorCode errCode = (enumErrorCode)wParam;
    CString strTips = _T("");
    switch(errCode)
    {
    case ERROR_CODE_SUCCESS:
    case ERROR_CODE_CANCEL:
        {
            CEventProc::GetInstance()->ShowPage(IDD_STATE_RESULT_LIST_DLG);
        }
        break;
    case ERROR_SMV_TOO_MANY_CHANNEL_ASDU:
        {
            strTips =  dsm_stringMgr::Get(DSM_STR_MANUAL_SMV_TOO_MANY_CHANNEL_ASDU);
            CStatusMgr::SetTips(strTips);
            _updateTestIndex();
            _resetMenu();
        }
        break;
    case ERROR_GOOSE_TOO_MANY_CHANNEL:
        {
            strTips =  dsm_stringMgr::Get(DSM_STR_MANUAL_GOOSE_TOO_MANY_CHANNEL);
            CStatusMgr::SetTips(strTips);
            _updateTestIndex();
            _resetMenu();
        }
        break;
    default:
        break;
    }

    return 1;
}

BOOL CStateSequenceDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类

    if(WM_KEYDOWN == pMsg->message)
    {
        switch(pMsg->wParam)
        {
        case VK_ESCAPE:
            {
                if (m_bTesting)
                {
                    //退出测试
                    CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_STATE_STOP_TESTING_TIPS), 3000);
                    return TRUE;
                }
                else
                {
                    CStatusMgr::SetTips(_T(""));
                    return CBaseDlg::PreTranslateMessage(pMsg);
                }
            }
            break;
        case DSM_KEY_HOME:
            {
                if (m_bTesting)
                {
                    //退出测试
                    CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_STATE_STOP_TESTING_TIPS), 3000);
                    return TRUE;
                }
                else
                {
                    CStatusMgr::SetTips(_T(""));
                    return CBaseDlg::PreTranslateMessage(pMsg);
                }
            }
            break;
        case VK_F7:
            {
                _showDIDO(FALSE);
                return CBaseDlg::PreTranslateMessage(pMsg);
            }
            break;
        //case VK_UP:
        //    {
        //        //
        //        //调节开入量状态框 显示位置
        //        //
        //        if (m_pDIDOStateDlg &&
        //            m_pDIDOStateDlg->GetSafeHwnd() &&
        //            m_pDIDOStateDlg->IsWindowVisible() )
        //        {
        //            int nRowSel = m_ctlStateSequReport.GetFocusedRowIndex();
        //            
        //            if (nRowSel == 0)
        //            {
        //                //
        //                //如果当前行是第0行，当再按 向上时，report默认处理为选中最后一行，
        //                //所以需要判断是否需要把DI移动到top
        //                //
        //                if(m_ctlStateSequReport.GetRecordRowCount() > 8)
        //                {
        //                    m_pDIDOStateDlg->MoveWindowTop();
        //                }
        //            }
        //            else if (nRowSel < 3)
        //            {
        //                m_pDIDOStateDlg->MoveWindowBottom();
        //            }
        //        }
        //    }
        //    break;
        //case VK_DOWN:
        //    {

        //        //
        //        //调节开入量状态框 显示位置
        //        //
        //        if (m_pDIDOStateDlg &&
        //            m_pDIDOStateDlg->GetSafeHwnd() &&
        //            m_pDIDOStateDlg->IsWindowVisible() )
        //        {
        //            int nRowSel = m_ctlStateSequReport.GetFocusedRowIndex();
        //            if (nRowSel == m_ctlStateSequReport.GetRecordRowCount() - 1)
        //            {
        //                //
        //                //如果当前行是最后行，当再按 向上时，report默认处理为选中第0行，
        //                //所以需要判断是否需要把DI移动到bottom
        //                //
        //                m_pDIDOStateDlg->MoveWindowBottom();
        //            }
        //            else if (nRowSel > 7)
        //            {
        //                m_pDIDOStateDlg->MoveWindowTop();
        //            }
        //        }
        //    }
        //    break;
        default:
            break;
        }
    }
    return CBaseDlg::PreTranslateMessage(pMsg);
}
void CStateSequenceDlg::_SavePage()
{
    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

    if(!pData || CStateSequenceDlg::IDD != pData->m_nPageID)
    {
        return;
    }
    
    if (pData->m_pPageDataExtra == NULL)
    {
        pData->m_pPageDataExtra = new CStateSequenceDataExtra(pData);
    }

    CStateSequenceDataExtra* pDataExtra = dynamic_cast<CStateSequenceDataExtra*>(pData->m_pPageDataExtra);
    ASSERT(pDataExtra != NULL);
    if (pDataExtra == NULL)
    {
        return;
    }
    int nFocusRowIndex = m_ctlStateSequReport.GetFocusedRowIndex();
    pDataExtra->m_nFocusRowIndex = nFocusRowIndex;
    if (nFocusRowIndex >= 0)
    {
        pDataExtra->m_pStatus = (CStatus*)m_ctlStateSequReport.GetRecordRowData(nFocusRowIndex);
    }
    else
    {
        pDataExtra = NULL;
    }

    m_pConfigStateSequ->SetIsShowDI(_isDIDOShowing());
    _showDIDO(FALSE);
    //
    //保存参数配置
    //

    m_pConfigStateSequ->SaveConfig();
}
void CStateSequenceDlg::_restorePage()
{
    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();

    if(!pData || CStateSequenceDlg::IDD != pData->m_nPageID)
    {
        return;
    }

    if(pData->m_eAction == PageBack)
    {
        //从其他页面返回
        CStateSequenceDataExtra* pDataExtra = dynamic_cast<CStateSequenceDataExtra*>(pData->m_pPageDataExtra);
        ASSERT(pDataExtra != NULL);
        if (pDataExtra == NULL)
        {
            return;
        }

        if (pDataExtra->m_nFocusRowIndex >= 0 && pDataExtra->m_nFocusRowIndex < m_ctlStateSequReport.GetRecordRowCount())
        {
            m_ctlStateSequReport.SetFocusedSelectionRowIndex(pDataExtra->m_nFocusRowIndex);
            m_ctlStateSequReport.EnsureVisible(pDataExtra->m_nFocusRowIndex, FALSE);
        }
    }
}

BOOL CStateSequenceDlg::_isDIDOShowing()
{
    if (m_pDIDOStateDlg &&
        m_pDIDOStateDlg->GetSafeHwnd() &&
        m_pDIDOStateDlg->IsWindowVisible())
    {
        //退出开入量显示
        return TRUE;
    }
    return FALSE;

}

void CStateSequenceDlg::_showDIDO(BOOL bShow)
{

    if (bShow)
    {
        if (!m_pDIDOStateDlg)
        {
            m_pDIDOStateDlg = new CDIDOStateDlg;
            m_pDIDOStateDlg->Create(CDIDOStateDlg::IDD, this);
        }
        m_pDIDOStateDlg->ShowWindow(SW_SHOWNOACTIVATE);
    }
    else
    {
        if (m_pDIDOStateDlg)
        {
            m_pDIDOStateDlg->ShowWindow(SW_HIDE);
        }
    }
}

void CStateSequenceDlg::_updateDOStatus(int nRowIndex)
{
    if (m_pDIDOStateDlg &&
        m_pDIDOStateDlg->GetSafeHwnd())
    {
        if (nRowIndex < 0 || nRowIndex >= m_ctlStateSequReport.GetRecordRowCount())
        {
            m_pDIDOStateDlg->UpdateDOStatus(NULL);
            return;
        }
        else
        {
            CStatus* pStatus = (CStatus*)m_ctlStateSequReport.GetRecordRowData(nRowIndex); 
            m_pDIDOStateDlg->UpdateDOStatus(pStatus);
        }
    }
}

void CStateSequenceDlg::_loadNaviMenu()
{
    ASSERT(-1 != m_nMenuID);

    // 添加底部菜单
    m_menu.LoadMenu(m_nMenuID);
    m_menuMain.LoadMenu(&m_menu);
    m_menu.DestroyMenu();

    // 调用虚函数修改菜单
    _modifyMenu();

    m_pNaviBar = CNaviBarMgr::GetInstance()->GetNaviBar();

    if(m_pNaviBar)
    {
        // 设置菜单目标对象和数据对象
        m_pNaviBar->SetCommandTarget(this);
        m_pNaviBar->SetNaviMenu(&m_menuMain, TRUE);
    }
}
void CStateSequenceDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    
    if (nIDEvent == 1)
    {
        m_pConfigStateSequ->SaveConfig();
    }
    
    CBaseDlg::OnTimer(nIDEvent);
}

void CStateSequenceDlg::OnDestroy()
{
    
    if (m_pDIDOStateDlg)
    {
        ::SendMessage(m_pDIDOStateDlg->GetSafeHwnd(), WM_CLOSE, 0, 0);
    }

    CBaseDlg::OnDestroy();
}