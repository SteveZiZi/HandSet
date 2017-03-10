/**
 * \file
 *      GooseBaseDlg.cpp
 *
 * \brief
 *      Goose页面基类实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/5/8
 */

// GooseBaseDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GooseBaseDlg.h"

#include "src/main/eventproc.h"
#include "src/main/mgr/statusmgr.h"
#include "src/main/pagedata/pagedata.h"
#include "src/utils/utility/dsm_utility.h"



// CGooseBaseDlg 对话框

IMPLEMENT_DYNAMIC(CGooseBaseDlg, DSM_BASEDLG) 


CGooseBaseDlg::CGooseBaseDlg( UINT nIDTemplate, UINT nIDCaption, CWnd* pParent /*= NULL*/ )
: DSM_BASEDLG(nIDTemplate, nIDCaption, pParent)
, m_nTimerID(UINT_MAX)
, m_nCaptionID(nIDCaption)
, m_nTemplateID(nIDTemplate)
, m_nCtrlBlkLastMenuID(ID_MENU_GS_CTRLBLK_BEGIN)
, m_nHighLightCtrlBlk(0)
, m_pPageDataExtra(NULL)
{
    m_nMenuID           = IDR_MENU_GOOSE;
    m_pEventProc        = CEventProc::GetInstance();
    m_pDataLayer        = CDsmDataLayer::getInstance();
    m_pCurrentCtrlBlk   = m_pDataLayer->GetCurrentCtrlBlk();
}

CGooseBaseDlg::~CGooseBaseDlg()
{
    m_pDataLayer->SetCurrentMode(DSM_MODE_QUIT_CURRENT_PAGE);

    m_mapCtrlBlk.clear();
}

void CGooseBaseDlg::DoDataExchange(CDataExchange* pDX)
{
    DSM_BASEDLG::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CGooseBaseDlg, DSM_BASEDLG)
    ON_COMMAND_RANGE(ID_MENU_GOOSE_F1, ID_MENU_GOOSE_F6, &CGooseBaseDlg::OnNaviMenuF1T6)
    ON_COMMAND_RANGE(ID_MENU_GOOSE_REALTIME, ID_MENU_GOOSE_MONITOR, &CGooseBaseDlg::OnNaviMenuF1)
    ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CGooseBaseDlg::OnInitDialog()
{
    CBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

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

    // 加载控制块列表
    _loadCtrlBlk();

    _updateCaption();

    // 高亮当前业务菜单
    _highlightMenu();

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}



// CGooseBaseDlg 消息处理程序

void CGooseBaseDlg::OnNaviMenuF1( UINT nID )
{
    switch(nID)
    {
        // 实时值
    case ID_MENU_GOOSE_REALTIME:
        m_pEventProc->ShowPage(IDD_GOOSE_REALTIME_DLG);
        break;

        // 变位列表
    case ID_MENU_GOOSE_SHIFT:
        m_pEventProc->ShowPage(IDD_GOOSE_SHIFT_DLG);
        break;

        // 报文统计
    case ID_MENU_GOOSE_STATISTICS:
        m_pEventProc->ShowPage(IDD_GOOSE_STATISTICS_DLG);
        break;

        // 报文监测
    case ID_MENU_GOOSE_MONITOR:
        m_pEventProc->ShowPage(IDD_GOOSE_MONITOR_DLG);
        break;

    default:
        break;
    }
}


void CGooseBaseDlg::_modifyMenu()
{
    // 修改F1字符串为『m_nCaptionID』
    m_menuMain.SetMenuItemCaption(0, dsm_stringMgr::Get(m_nCaptionID));
	m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);
	m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);

//     m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_CTRLBLK_NORMAL), NULL, NULL);
//     m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_MENU), NULL, NULL);
}


void CGooseBaseDlg::_highlightMenu()
{
    int nIndex = 0;

    switch(m_nTemplateID)
    {
    case IDD_GOOSE_REALTIME_DLG:
        nIndex = 0;
        break;

    case IDD_GOOSE_SHIFT_DLG:
        nIndex = 1;
        break;

    case IDD_GOOSE_STATISTICS_DLG:
        nIndex = 2;
        break;

    case IDD_GOOSE_MONITOR_DLG:
        nIndex = 3;
        break;

    default:
        return;
    }

    CNaviBarMgr::GetInstance()->HighlightMenuItem(0, nIndex);
}


void CGooseBaseDlg::OnDestroy()
{
    CBaseDlg::OnDestroy();

    // TODO: 在此处添加消息处理程序代码

    _stopTimer();
}

void CGooseBaseDlg::_beginTimer( UINT nTimeout /*= DSM_GOOSE_TIMER_INTERVAL*/ )
{
    m_nTimerID = SetTimer(DSM_GOOSE_TIMER_ID, nTimeout, NULL);
}

void CGooseBaseDlg::_stopTimer()
{
    if(m_nTimerID != 0)
    {
        KillTimer(m_nTimerID);
        m_nTimerID = 0;
    }
}

void CGooseBaseDlg::_updateCaption()
{
    ASSERT(m_pCurrentCtrlBlk);

    CString csDesc;
    CString csCaption;

    CNetDetectItem* pBlk = dynamic_cast<CNetDetectItem*>(m_pCurrentCtrlBlk);

    if(pBlk)
    {
        // 标题 ---->  业务 - AppID [描述]
        csCaption.Format(L"%s->[0x%04X : %s]", dsm_stringMgr::Get(m_nIDCaption), pBlk->appId, _getDesc(pBlk));

        CStatusMgr::SetTitle(csCaption);
    }
}


void CGooseBaseDlg::_loadCtrlBlk()
{
    CMenu  menu;
    VERIFY(menu.CreatePopupMenu());

    CString csDesc;
    UINT_PTR nItemID            = ID_MENU_GS_CTRLBLK_BEGIN;
    CNetDetectItem* pCtrlBlk    = dynamic_cast<CNetDetectItem*>(m_pDataLayer->GetFirstSelectCtrlBlk());

    ASSERT(m_pDataLayer->GetSelectedCtrlBlkCount() <= ID_MENU_GS_CTRLBLK_END - ID_MENU_GS_CTRLBLK_BEGIN + 1);

    // 添加选中控制块到F1菜单
    while(pCtrlBlk)
    {
        csDesc.Format(L"0x%04X [%s]", pCtrlBlk->appId, _getDesc(pCtrlBlk));

        // 添加到菜单
        menu.AppendMenu(MF_STRING, nItemID, csDesc);

        if(m_pCurrentCtrlBlk->nID == pCtrlBlk->nID)
        {
            m_nHighLightCtrlBlk = nItemID - ID_MENU_GS_CTRLBLK_BEGIN;
        }

        // 控制块加入菜单ID映射表
        _setMapping(nItemID, pCtrlBlk);
        pCtrlBlk = dynamic_cast<CNetDetectItem*>(m_pDataLayer->GetNextSelectCtrlBlk(pCtrlBlk));

        nItemID++;
    }

    m_nCtrlBlkLastMenuID = nItemID;

    CNaviMenu* pSub = new CNaviMenu;

    pSub->LoadMenu(&menu);
    menu.DestroyMenu();
    m_pNaviBar->GetNaviMenu()->SetMenuSubItem(1, pSub);
    CNaviBarMgr::GetInstance()->HighlightMenuItem(1, m_nHighLightCtrlBlk);
}


bool CGooseBaseDlg::_switchCtrlBlk( UINT nCmdID )
{
    MapCtrlBlk::iterator iter = m_mapCtrlBlk.find(nCmdID);

    if(m_mapCtrlBlk.end() == iter || !iter->second)
    {
        return false;
    }

    if(iter->second == m_pCurrentCtrlBlk)
    {
        return false;
    }

    // 若选中上次的菜单则返回
    if(m_nHighLightCtrlBlk == nCmdID - ID_MENU_GS_CTRLBLK_BEGIN)
    {
        return true;
    }

    // 先取消上一个菜单的高亮，在设置选中菜单的高亮
    CNaviBarMgr::GetInstance()->HighlightMenuItem(1, m_nHighLightCtrlBlk, FALSE);
    m_nHighLightCtrlBlk = nCmdID - ID_MENU_GS_CTRLBLK_BEGIN;
    CNaviBarMgr::GetInstance()->HighlightMenuItem(1, m_nHighLightCtrlBlk);

    m_pCurrentCtrlBlk = iter->second;
    m_pDataLayer->SetCurrentCtrlBlk(m_pCurrentCtrlBlk);

    _updateCaption();

    return true;
}


void CGooseBaseDlg::_setMapping( UINT nCmdID, CBaseDetectItem* pCtrlBlk )
{
    ASSERT(pCtrlBlk);

    MapCtrlBlk::iterator iter = m_mapCtrlBlk.find(nCmdID);

    if(m_mapCtrlBlk.end() == iter)
    {
        m_mapCtrlBlk.erase(nCmdID);
    }

    // 设置命令菜单ID到控制块映射信息
    m_mapCtrlBlk.insert(std::pair<UINT, CBaseDetectItem*>(nCmdID, pCtrlBlk));
}


BOOL CGooseBaseDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    // TODO: 在此添加专用代码和/或调用基类

    if(nCode == CN_UPDATE_COMMAND_UI && nID >= ID_MENU_GS_CTRLBLK_BEGIN && nID < m_nCtrlBlkLastMenuID)
    {
        CCmdUI* pCmdUI = (CCmdUI*)pExtra;
        ASSERT(pCmdUI);

        pCmdUI->Enable(TRUE);

        return TRUE;
    }

    return CBaseDlg::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CGooseBaseDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    // TODO: 在此添加专用代码和/或调用基类

    int nMenuID = LOWORD(wParam);

    if(nMenuID >= ID_MENU_GS_CTRLBLK_BEGIN && (DWORD)nMenuID < m_nCtrlBlkLastMenuID)
    {
        // 切换控制块
        _switchCtrlBlk(nMenuID);

        return TRUE;
    }

    return CBaseDlg::OnCommand(wParam, lParam);
}

void CGooseBaseDlg::OnNaviMenuF1T6(UINT nID)
{
	//int i =0;
     /*switch(nID)
     {
     default:
         break;
     }*/
}

CString CGooseBaseDlg::_getDesc( CNetDetectItem* pCtrlBlk )
{
    ASSERT(pCtrlBlk);

    CString csDesc;

    // 获取goose控制块描述信息
    if(FRAME_TYPE_NET_GOOSE == pCtrlBlk->enFrameType)
    {
        CGooseCb* pCB = m_pDataLayer->GetSclManager()->GetGooseCb(pCtrlBlk->appId);

        if(pCB)
        {
            csDesc = pCB->GetDescription().c_str();
        }
    }

//     // 描述信息为空则取MAC地址信息
//     if(csDesc.IsEmpty())
//     {
//         csDesc.Format(L"%02X-%02X-%02X-%02X-%02X-%02X", 
//             pCtrlBlk->macAddr[0], pCtrlBlk->macAddr[1], pCtrlBlk->macAddr[2], 
//             pCtrlBlk->macAddr[3], pCtrlBlk->macAddr[4], pCtrlBlk->macAddr[5]);
//     }

    CString csPort = DSM_GetPortDesc(pCtrlBlk->enPortName);

    // 描述信息为空则获取端口的描述字符串
    if(csDesc.IsEmpty())
    {
        csDesc = csPort;
    }
    else
    {
        csDesc.Format(L"%s : %s", csDesc, csPort);
    }

    return csDesc;
}
