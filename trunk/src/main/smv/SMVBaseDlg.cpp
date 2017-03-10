/**
 * \file
 *      SMVBaseDlg.h
 *
 * \brief
 *      smv页面基类实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/4/21
 */


// SMVBaseDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SMVBaseDlg.h"

#include "src/main/eventproc.h"
#include "src/main/resourcebmp.h"
#include "src/main/mgr/statusmgr.h"
#include "src/main/pagedata/pagedata.h"
#include "src/utils/utility/dsm_utility.h"



// CSMVBaseDlg 对话框

IMPLEMENT_DYNAMIC(CSMVBaseDlg, DSM_BASEDLG)


CSMVBaseDlg::CSMVBaseDlg( UINT nIDTemplate, UINT nIDCaption, CWnd* pParent /*= NULL*/ )
    :DSM_BASEDLG(nIDTemplate, nIDCaption, pParent)
    , m_pSMVRecvConfig(NULL)
    , m_nTimerID(UINT_MAX)
    , m_nCtrlBlkLastMenuID(ID_MENU_SMV_CTRLBLK_BEGIN)
    , m_nCaptionID(nIDCaption)
    , m_nTemplateID(nIDTemplate)
    , m_nHighLightCtrlBlk(0)
    , m_pPageDataExtra(NULL)
    , m_bChannelChange(false)
    , m_bEnableTimer(false)
{
    m_nMenuID           = IDR_MENU_SMV_COMMON;
    m_pEventProc        = CEventProc::GetInstance();
    m_pDataLayer        = CDsmDataLayer::getInstance();
    m_pCurrentCtrlBlk   = m_pDataLayer->GetCurrentCtrlBlk();
}

CSMVBaseDlg::~CSMVBaseDlg()
{
    // 离开页面时向数据层设置离开模式
    m_pDataLayer->SetCurrentMode(DSM_MODE_QUIT_CURRENT_PAGE);

    m_mapCtrlBlk.clear();
}

void CSMVBaseDlg::DoDataExchange(CDataExchange* pDX)
{
	DSM_BASEDLG::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSMVBaseDlg, DSM_BASEDLG)
    ON_COMMAND_RANGE(ID_MENU_SMV_EFFECTVAL, ID_MENU_SMV_MSGMONITOR, &CSMVBaseDlg::OnNaviMenuF1)
    ON_COMMAND_RANGE(ID_MENU_SMV_F1, ID_MENU_SMV_F6, &CSMVBaseDlg::OnNaviMenuF3T6)
#if SU_FEI
    ON_COMMAND_RANGE(ID_MENU_SMV_EX_MONITOR, ID_MENU_SMV_EX_CONFIG, &CSMVBaseDlg::OnNaviMenuF6X)
#endif
    ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CSMVBaseDlg::OnInitDialog()
{
    CBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    // 设置当前控制块
    _initCurrentCtrlBlk();

    // 高亮当前业务菜单
    _highlightMenu();

#if !SU_FEI
	_beginTimer(100);
#endif

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}



// CSMVBaseDlg 消息处理程序

void CSMVBaseDlg::OnNaviMenuF1( UINT nID )
{
    switch(nID)
    {
        // 有效值
    case ID_MENU_SMV_EFFECTVAL:
        m_pEventProc->ShowPage(IDD_SMV_EFFECTIVE_VALUE_DLG);
        break;

        // 波形
    case ID_MENU_SMV_WAVEFORM:
        m_pEventProc->ShowPage(IDD_SMV_WAVEFORM_DLG);
        break;

        // 相量
    case ID_MENU_SMV_PHASOR:
        m_pEventProc->ShowPage(IDD_SMV_PHASOR_DLG);
        break;

        // 序量
    case ID_MENU_SMV_PHASESEQU:
        m_pEventProc->ShowPage(IDD_SMV_PHASESEQU_DLG);
        break;

        // 功率
    case ID_MENU_SMV_POWER:
        m_pEventProc->ShowPage(IDD_SMV_POWER_DLG);
        break;

        // 谐波
    case ID_MENU_SMV_HARMONIC:
        m_pEventProc->ShowPage(IDD_SMV_HARMONIC_DLG);
        break;

        // 双AD
    case ID_MENU_SMV_DUALAD:
        m_pEventProc->ShowPage(IDD_SMV_DUALAD_DLG);
        break;

        // 核相
    case ID_MENU_SMV_DUALPHASE:
        m_pEventProc->ShowPage(IDD_SMV_DUALPHASE_DLG);
        break;

        // 极性
    case ID_MENU_SMV_POLAR:
        m_pEventProc->ShowPage(IDD_SMV_POLARITY_DLG);
        break;

        // 离散值
    case ID_MENU_SMV_STATISTICS:
        m_pEventProc->ShowPage(IDD_SMV_STATISTICS_DLG);
        break;

        // 报文监测
    case ID_MENU_SMV_MSGMONITOR:
        m_pEventProc->ShowPage(IDD_SMV_MSGMONITOR_DLG);
        break;

    default:
        break;
    }
}

void CSMVBaseDlg::OnNaviMenuF3T6( UINT nID )
{
    switch(nID)
    {
#if SU_FEI       
		// 暂停
    case ID_MENU_SMV_F3:


        if(!m_bChannelChange)
        {
            // 启动定时器
            if(0 == m_nTimerID)
            {
                // 启动定时器并设置菜单的显示
                m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, dsm_stringMgr::Get(DSM_STR_TIMER_STOP));
                m_pNaviBar->UpdateNaviMenu();
                _beginTimer();
            }
            // 停止定时器
            else
            {
                // 停止定时器并设置菜单的显示
                m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, dsm_stringMgr::Get(DSM_STR_TIMER_START));
                m_pNaviBar->UpdateNaviMenu();
                _stopTimer();
            }
        }

		
        break;
#else

	case ID_MENU_SMV_F3:
		{
			CSmvRecvConfigPublic* pCfg = m_pDataLayer->GetSmvConfigPublic();
			ASSERT(pCfg);		
			
			
			if (pCfg->GetSmvDispType() == SMV_VALUE_TYPE_PRIMARY)
			{
				//如果当前是显示一次值，就设置成二次值
				pCfg->SetSmvDispType(SMV_VALUE_TYPE_SECONDARY);
				m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_SECOND_SEL), NULL, NULL);
				
			}
			else if (pCfg->GetSmvDispType() == SMV_VALUE_TYPE_SECONDARY)
			{
				//如果当前是显示二次值，就设置成一次值
				pCfg->SetSmvDispType(SMV_VALUE_TYPE_PRIMARY);	
				m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_FIRST_SEL), NULL, NULL);
			}

			pCfg->SaveToConfigFile();

			m_pNaviBar->UpdateNaviMenu();
		}

		break;

#endif
	case ID_MENU_SMV_F6:
		m_pEventProc->ShowPage(IDD_SMV_CONFIG_DLG);
		break;
    default:
        break;
    }
}

#if SU_FEI
void CSMVBaseDlg::OnNaviMenuF6X( UINT nID )
{
    switch(nID)
    {
        // 监视
    case ID_MENU_SMV_EX_MONITOR:
        m_pEventProc->ShowPage(IDD_SMV_MONITOR_DLG);
        break;

        // 设置
    case ID_MENU_SMV_EX_CONFIG:
        m_pEventProc->ShowPage(IDD_SMV_CONFIG_DLG);
        break;

    default:
        break;
    }
}
#endif

BOOL CSMVBaseDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    // TODO: 在此添加专用代码和/或调用基类

    int nMenuID = LOWORD(wParam);

    if(nMenuID >= ID_MENU_SMV_CTRLBLK_BEGIN && (DWORD)nMenuID < m_nCtrlBlkLastMenuID)
    {
        if(m_bEnableTimer)
        {
            // 切换控制块
            _switchCtrlBlk(nMenuID);
        }
        else
        {
            MapCtrlBlk::iterator iter = m_mapCtrlBlk.find(nMenuID);

            if(m_mapCtrlBlk.end() != iter && iter->second && iter->second != m_pCurrentCtrlBlk && m_nHighLightCtrlBlk != (int)(nMenuID - ID_MENU_SMV_CTRLBLK_BEGIN))
            {
                CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_TIMER_SWITCH_CTRL_TIPS));
            }
        }

        return TRUE;
    }

    return CBaseDlg::OnCommand(wParam, lParam);
}

BOOL CSMVBaseDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    // TODO: 在此添加专用代码和/或调用基类

    if(nCode == CN_UPDATE_COMMAND_UI && nID >= ID_MENU_SMV_CTRLBLK_BEGIN && nID < m_nCtrlBlkLastMenuID)
    {
        CCmdUI* pCmdUI = (CCmdUI*)pExtra;
        ASSERT(pCmdUI);

        pCmdUI->Enable(TRUE);

        return TRUE;
    }

    return CBaseDlg::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

/**
 *      修改菜单HMENU
 */
void CSMVBaseDlg::_modifyMenu()
{
    // 修改F1字符串为业务名『m_nCaptionID』
 //   m_menuMain.SetMenuItemCaption(0, dsm_stringMgr::Get(m_nCaptionID));
	//m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);
	//m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);
//     m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_CTRLBLK_NORMAL), NULL, NULL);
//     m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_MENU), NULL, NULL);

}

/**
 *      高亮当前业务的菜单
 */
void CSMVBaseDlg::_highlightMenu()
{
    int nIndex = 0;

    switch(m_nTemplateID)
    {
    case IDD_SMV_EFFECTIVE_VALUE_DLG:
        nIndex = 0;
        break;

    case IDD_SMV_WAVEFORM_DLG:
        nIndex = 1;
        break;

    case IDD_SMV_PHASOR_DLG:
        nIndex = 2;
        break;

    case IDD_SMV_PHASESEQU_DLG:
        nIndex = 3;
        break;

    case IDD_SMV_POWER_DLG:
        nIndex = 4;
        break;

    case IDD_SMV_HARMONIC_DLG:
        nIndex = 5;
        break;

    case IDD_SMV_DUALAD_DLG:
        nIndex = 6;
        break;

    case IDD_SMV_DUALPHASE_DLG:
        nIndex = 7;
        break;

    case IDD_SMV_POLARITY_DLG:
        nIndex = 8;
        break;

    case IDD_SMV_STATISTICS_DLG:
        nIndex = 9;
        break;

    case IDD_SMV_MSGMONITOR_DLG:
        nIndex = 10;
        break;

    default:
        return;
    }

     CNaviBarMgr::GetInstance()->HighlightMenuItem(0, nIndex);
}


void CSMVBaseDlg::OnDestroy()
{
    CBaseDlg::OnDestroy();

    // TODO: 在此处添加消息处理程序代码

    // 停止定时器
    _stopTimer();
}


/**
 *      启动默认定时器
 */
void CSMVBaseDlg::_beginTimer( UINT nTimeout /*= DSM_SMV_TIMER_INTERVAL*/ )
{
    m_nTimerID      = SetTimer(DSM_SMV_TIMER_ID, nTimeout, NULL);
    m_bEnableTimer  = true;
}

/**
 *      停止默认定时器
 */
void CSMVBaseDlg::_stopTimer()
{
    if(m_nTimerID != 0)
    {
        KillTimer(m_nTimerID);
        m_nTimerID = 0;

        m_bEnableTimer = false;
    }
}

/**
 *      更新标题
 */
void CSMVBaseDlg::_updateCaption()
{
    ASSERT(m_pCurrentCtrlBlk);

    if(m_pCurrentCtrlBlk)
    {
        CString csCaption;

        csCaption.Format(L"%s->[%s]", dsm_stringMgr::Get(m_nIDCaption), DSM_GetCtrlBlkInfo(m_pCurrentCtrlBlk));
        CStatusMgr::SetTitle(csCaption);
    }
}

/**
 *      加载控制块到F2菜单中
 */
void CSMVBaseDlg::_loadCtrlBlk()
{
    CMenu  menu;
    VERIFY(menu.CreatePopupMenu());

    CString csDesc;
    UINT_PTR nItemID        = ID_MENU_SMV_CTRLBLK_BEGIN;
    CBaseDetectItem* pItem  = m_pDataLayer->GetFirstSelectCtrlBlk();

    ASSERT(m_pDataLayer->GetSelectedCtrlBlkCount() <= ID_MENU_SMV_CTRLBLK_END - ID_MENU_SMV_CTRLBLK_BEGIN + 1);

    while(pItem)
    {
        // 获取控制块描述信息
        csDesc = DSM_GetCtrlBlkInfo(pItem);

        // 添加到菜单
        menu.AppendMenu(MF_STRING, nItemID, csDesc);

        if(m_pCurrentCtrlBlk->nID == pItem->nID)
        {
            m_nHighLightCtrlBlk = nItemID - ID_MENU_SMV_CTRLBLK_BEGIN;
        }

        // 控制块加入菜单ID映射表
        _setMapping(nItemID, pItem);
        pItem = m_pDataLayer->GetNextSelectCtrlBlk(pItem);

        nItemID++;
    }

    m_nCtrlBlkLastMenuID    = nItemID;
    CNaviMenu* pSub         = new CNaviMenu;

    pSub->LoadMenu(&menu);
    menu.DestroyMenu();

    // 设置控制块菜单，并高亮当前选中控制块的菜单项
    m_pNaviBar->GetNaviMenu()->SetMenuSubItem(1, pSub);
    CNaviBarMgr::GetInstance()->HighlightMenuItem(1, m_nHighLightCtrlBlk);
}

/**
 *      关联菜单ID与控制块（冲突覆盖）
 *
 *  \param[in]
 *      nCmdID          菜单命令ID
 *  \param[in]
 *      pCtrlBlk        菜单ID对应的控制块
 */
void CSMVBaseDlg::_setMapping( UINT nCmdID, CBaseDetectItem* pCtrlBlk )
{
    ASSERT(pCtrlBlk);

    MapCtrlBlk::iterator iter = m_mapCtrlBlk.find(nCmdID);

    if(m_mapCtrlBlk.end() == iter)
    {
        m_mapCtrlBlk.erase(nCmdID);
    }

    m_mapCtrlBlk.insert(std::pair<UINT, CBaseDetectItem*>(nCmdID, pCtrlBlk));
}


/**
 *      根据菜单ID切换到指定的控制块
 *
 *  \note
 *      子类选择控制块需要重载该方法做一些补充操作
 */
bool CSMVBaseDlg::_switchCtrlBlk( UINT nCmdID )
{
    MapCtrlBlk::iterator iter = m_mapCtrlBlk.find(nCmdID);

    if(m_mapCtrlBlk.end() == iter || !iter->second)
    {
        return false;
    }

    // 若选中上次的菜单则返回
    if(iter->second == m_pCurrentCtrlBlk || m_nHighLightCtrlBlk == (int)(nCmdID - ID_MENU_SMV_CTRLBLK_BEGIN))
    {
        return true;
    }

    // 设置当前控制块
    m_pCurrentCtrlBlk = iter->second;
    m_pDataLayer->SetCurrentCtrlBlk(m_pCurrentCtrlBlk);

    _highlightCtrlBlk(nCmdID - ID_MENU_SMV_CTRLBLK_BEGIN);

    if(m_pCurrentCtrlBlk->nChannelCount > DSM_CHANNEL_MAX)
    {
        CString csTips;
        csTips.Format(dsm_stringMgr::Get(DSM_STR_INFO_CHANNEL_EXCEED_MAX), DSM_CHANNEL_MAX);
        CStatusMgr::SetTips(csTips, 5000);
    }

    return true;
}


/**
 *      高亮选中的控制块菜单
 *
 *  \param[in]
 *      pSelCtrlBlk         需要高亮的控制块对象
 */
void CSMVBaseDlg::_highlightCtrlBlk( CBaseDetectItem* pSelCtrlBlk )
{
    if(!pSelCtrlBlk)
    {
        ASSERT(pSelCtrlBlk);
        return;
    }

    MapCtrlBlk::iterator itor  = m_mapCtrlBlk.begin();
    MapCtrlBlk::iterator itEnd = m_mapCtrlBlk.end();

    for(; itor != itEnd; itor++)
    {
        if(pSelCtrlBlk->nID == itor->second->nID)
        {
            // 高亮控制块菜单索引
            _highlightCtrlBlk(itor->first - ID_MENU_SMV_CTRLBLK_BEGIN);

            return;
        }
    }
}

/**
 *      高亮指定索引的控制块菜单
 *
 *  \param[in]
 *      nIndex          高亮的控制块菜单索引号
 */
void CSMVBaseDlg::_highlightCtrlBlk( int nIndex )
{
    // 若选中上次的菜单则返回
    if(m_nHighLightCtrlBlk == nIndex)
    {
        return;
    }

    // 先取消上一次的高亮菜单，再高亮当前选中的菜单
    CNaviBarMgr::GetInstance()->HighlightMenuItem(1, m_nHighLightCtrlBlk, FALSE);
    m_nHighLightCtrlBlk = nIndex;
    CNaviBarMgr::GetInstance()->HighlightMenuItem(1, m_nHighLightCtrlBlk);

    // 切换控制块后更新标题
    _updateCaption();
}

/**
 *      初始化当前控制块
 */
void CSMVBaseDlg::_initCurrentCtrlBlk()
{
    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();
    bool bInited = false;

    // 新进页面，创建页面数据
    if(pData->m_eAction == PageEnter)
    {
        if(!pData->m_pPageDataExtra)
        {
            m_pPageDataExtra = new CPageDataExtra(pData);
            pData->m_pPageDataExtra = m_pPageDataExtra;

            if(!m_pCurrentCtrlBlk && m_pDataLayer->GetSelectedCtrlBlkCount() > 0)
            {
                m_pDataLayer->SetCurrentCtrlBlk(m_pDataLayer->GetFirstSelectCtrlBlk());
                m_pCurrentCtrlBlk = m_pDataLayer->GetCurrentCtrlBlk();
                ASSERT(m_pCurrentCtrlBlk);
            }
        }
        else
        {
            bInited = true;
        }
    }

    // 返回页面，取出旧的页面数据
    if((pData->m_eAction == PageBack && pData->m_pPageDataExtra) || bInited)
    {
        m_pPageDataExtra = dynamic_cast<CPageDataExtra*>(pData->m_pPageDataExtra);
        ASSERT(m_pPageDataExtra);
    }

    ASSERT(m_pCurrentCtrlBlk);

    if(m_pCurrentCtrlBlk)
    {
        // 加载控制块列表
        _loadCtrlBlk();

        // 更新标题
        _updateCaption();

        // 设置业务模式
        _setMode();

        // 启用默认定时器
        _beginTimer();

        if(m_pCurrentCtrlBlk->nChannelCount > DSM_CHANNEL_MAX)
        {
            CString csTips;
            csTips.Format(dsm_stringMgr::Get(DSM_STR_INFO_CHANNEL_EXCEED_MAX), DSM_CHANNEL_MAX);
            CStatusMgr::SetTips(csTips, 5000);
        }
    }
}

void CSMVBaseDlg::_setMode()
{
    DSM_OPERATE_MODE enDsmMode = DSM_MODE_UNKOWN;

    switch(m_nTemplateID)
    {
        // 有效值
    case IDD_SMV_EFFECTIVE_VALUE_DLG:
        enDsmMode = DSM_MODE_SMV_RECV_EFFECTIVE_VALUE;
        break;

        // 波形图
    case IDD_SMV_WAVEFORM_DLG:
        enDsmMode = DSM_MODE_SMV_RECV_OSCILLOGRAM;
        break;

        // 相量
    case IDD_SMV_PHASOR_DLG:
        enDsmMode = DSM_MODE_SMV_RECV_PHASOR;
        break;

        // 序量
    case IDD_SMV_PHASESEQU_DLG:
        enDsmMode = DSM_MODE_SMV_RECV_XU_LIANG;
        break;

        // 功率
    case IDD_SMV_POWER_DLG:
        enDsmMode = DSM_MODE_SMV_RECV_POWER;
        break;

        // 谐波
    case IDD_SMV_HARMONIC_DLG:
        enDsmMode = DSM_MODE_SMV_RECV_HARMONIC;
        break;

        // 双AD
    case IDD_SMV_DUALAD_DLG:
        enDsmMode = DSM_MODE_SMV_RECV_DOUBLE_AD;
        break;

        // 核相
    case IDD_SMV_DUALPHASE_DLG:
        enDsmMode = DSM_MODE_SMV_RECV_PHASE_CHECK;
        break;

        // 极性
    case IDD_SMV_POLARITY_DLG:
        enDsmMode = DSM_MODE_SMV_RECV_POLARITY;
        break;

        // 报文统计及离散度
    case IDD_SMV_STATISTICS_DLG:
        // 统计和离散度分开两个模式？？
        // DSM_MODE_SMV_RECV_DISPERSION
        enDsmMode = DSM_MODE_SMV_RECV_FRAME_STATIC;
        break;

        // 报文监测及录波
    case IDD_SMV_MSGMONITOR_DLG:
        enDsmMode = DSM_MODE_SMV_RECV_FRAME_MONITOR;
        break;

    default:
        break;
    }

    // 设置业务模式
    m_pDataLayer->SetCurrentMode(enDsmMode);
}



