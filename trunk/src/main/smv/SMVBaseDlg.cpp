/**
 * \file
 *      SMVBaseDlg.h
 *
 * \brief
 *      smvҳ�����ʵ���ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/4/21
 */


// SMVBaseDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SMVBaseDlg.h"

#include "src/main/eventproc.h"
#include "src/main/resourcebmp.h"
#include "src/main/mgr/statusmgr.h"
#include "src/main/pagedata/pagedata.h"
#include "src/utils/utility/dsm_utility.h"



// CSMVBaseDlg �Ի���

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
    // �뿪ҳ��ʱ�����ݲ������뿪ģʽ
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

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��

    // ���õ�ǰ���ƿ�
    _initCurrentCtrlBlk();

    // ������ǰҵ��˵�
    _highlightMenu();

#if !SU_FEI
	_beginTimer(100);
#endif

    return TRUE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}



// CSMVBaseDlg ��Ϣ�������

void CSMVBaseDlg::OnNaviMenuF1( UINT nID )
{
    switch(nID)
    {
        // ��Чֵ
    case ID_MENU_SMV_EFFECTVAL:
        m_pEventProc->ShowPage(IDD_SMV_EFFECTIVE_VALUE_DLG);
        break;

        // ����
    case ID_MENU_SMV_WAVEFORM:
        m_pEventProc->ShowPage(IDD_SMV_WAVEFORM_DLG);
        break;

        // ����
    case ID_MENU_SMV_PHASOR:
        m_pEventProc->ShowPage(IDD_SMV_PHASOR_DLG);
        break;

        // ����
    case ID_MENU_SMV_PHASESEQU:
        m_pEventProc->ShowPage(IDD_SMV_PHASESEQU_DLG);
        break;

        // ����
    case ID_MENU_SMV_POWER:
        m_pEventProc->ShowPage(IDD_SMV_POWER_DLG);
        break;

        // г��
    case ID_MENU_SMV_HARMONIC:
        m_pEventProc->ShowPage(IDD_SMV_HARMONIC_DLG);
        break;

        // ˫AD
    case ID_MENU_SMV_DUALAD:
        m_pEventProc->ShowPage(IDD_SMV_DUALAD_DLG);
        break;

        // ����
    case ID_MENU_SMV_DUALPHASE:
        m_pEventProc->ShowPage(IDD_SMV_DUALPHASE_DLG);
        break;

        // ����
    case ID_MENU_SMV_POLAR:
        m_pEventProc->ShowPage(IDD_SMV_POLARITY_DLG);
        break;

        // ��ɢֵ
    case ID_MENU_SMV_STATISTICS:
        m_pEventProc->ShowPage(IDD_SMV_STATISTICS_DLG);
        break;

        // ���ļ��
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
		// ��ͣ
    case ID_MENU_SMV_F3:


        if(!m_bChannelChange)
        {
            // ������ʱ��
            if(0 == m_nTimerID)
            {
                // ������ʱ�������ò˵�����ʾ
                m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, dsm_stringMgr::Get(DSM_STR_TIMER_STOP));
                m_pNaviBar->UpdateNaviMenu();
                _beginTimer();
            }
            // ֹͣ��ʱ��
            else
            {
                // ֹͣ��ʱ�������ò˵�����ʾ
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
				//�����ǰ����ʾһ��ֵ�������óɶ���ֵ
				pCfg->SetSmvDispType(SMV_VALUE_TYPE_SECONDARY);
				m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_SECOND_SEL), NULL, NULL);
				
			}
			else if (pCfg->GetSmvDispType() == SMV_VALUE_TYPE_SECONDARY)
			{
				//�����ǰ����ʾ����ֵ�������ó�һ��ֵ
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
        // ����
    case ID_MENU_SMV_EX_MONITOR:
        m_pEventProc->ShowPage(IDD_SMV_MONITOR_DLG);
        break;

        // ����
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
    // TODO: �ڴ����ר�ô����/����û���

    int nMenuID = LOWORD(wParam);

    if(nMenuID >= ID_MENU_SMV_CTRLBLK_BEGIN && (DWORD)nMenuID < m_nCtrlBlkLastMenuID)
    {
        if(m_bEnableTimer)
        {
            // �л����ƿ�
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
    // TODO: �ڴ����ר�ô����/����û���

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
 *      �޸Ĳ˵�HMENU
 */
void CSMVBaseDlg::_modifyMenu()
{
    // �޸�F1�ַ���Ϊҵ������m_nCaptionID��
 //   m_menuMain.SetMenuItemCaption(0, dsm_stringMgr::Get(m_nCaptionID));
	//m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);
	//m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);
//     m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_CTRLBLK_NORMAL), NULL, NULL);
//     m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_MENU), NULL, NULL);

}

/**
 *      ������ǰҵ��Ĳ˵�
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

    // TODO: �ڴ˴������Ϣ����������

    // ֹͣ��ʱ��
    _stopTimer();
}


/**
 *      ����Ĭ�϶�ʱ��
 */
void CSMVBaseDlg::_beginTimer( UINT nTimeout /*= DSM_SMV_TIMER_INTERVAL*/ )
{
    m_nTimerID      = SetTimer(DSM_SMV_TIMER_ID, nTimeout, NULL);
    m_bEnableTimer  = true;
}

/**
 *      ֹͣĬ�϶�ʱ��
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
 *      ���±���
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
 *      ���ؿ��ƿ鵽F2�˵���
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
        // ��ȡ���ƿ�������Ϣ
        csDesc = DSM_GetCtrlBlkInfo(pItem);

        // ��ӵ��˵�
        menu.AppendMenu(MF_STRING, nItemID, csDesc);

        if(m_pCurrentCtrlBlk->nID == pItem->nID)
        {
            m_nHighLightCtrlBlk = nItemID - ID_MENU_SMV_CTRLBLK_BEGIN;
        }

        // ���ƿ����˵�IDӳ���
        _setMapping(nItemID, pItem);
        pItem = m_pDataLayer->GetNextSelectCtrlBlk(pItem);

        nItemID++;
    }

    m_nCtrlBlkLastMenuID    = nItemID;
    CNaviMenu* pSub         = new CNaviMenu;

    pSub->LoadMenu(&menu);
    menu.DestroyMenu();

    // ���ÿ��ƿ�˵�����������ǰѡ�п��ƿ�Ĳ˵���
    m_pNaviBar->GetNaviMenu()->SetMenuSubItem(1, pSub);
    CNaviBarMgr::GetInstance()->HighlightMenuItem(1, m_nHighLightCtrlBlk);
}

/**
 *      �����˵�ID����ƿ飨��ͻ���ǣ�
 *
 *  \param[in]
 *      nCmdID          �˵�����ID
 *  \param[in]
 *      pCtrlBlk        �˵�ID��Ӧ�Ŀ��ƿ�
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
 *      ���ݲ˵�ID�л���ָ���Ŀ��ƿ�
 *
 *  \note
 *      ����ѡ����ƿ���Ҫ���ظ÷�����һЩ�������
 */
bool CSMVBaseDlg::_switchCtrlBlk( UINT nCmdID )
{
    MapCtrlBlk::iterator iter = m_mapCtrlBlk.find(nCmdID);

    if(m_mapCtrlBlk.end() == iter || !iter->second)
    {
        return false;
    }

    // ��ѡ���ϴεĲ˵��򷵻�
    if(iter->second == m_pCurrentCtrlBlk || m_nHighLightCtrlBlk == (int)(nCmdID - ID_MENU_SMV_CTRLBLK_BEGIN))
    {
        return true;
    }

    // ���õ�ǰ���ƿ�
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
 *      ����ѡ�еĿ��ƿ�˵�
 *
 *  \param[in]
 *      pSelCtrlBlk         ��Ҫ�����Ŀ��ƿ����
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
            // �������ƿ�˵�����
            _highlightCtrlBlk(itor->first - ID_MENU_SMV_CTRLBLK_BEGIN);

            return;
        }
    }
}

/**
 *      ����ָ�������Ŀ��ƿ�˵�
 *
 *  \param[in]
 *      nIndex          �����Ŀ��ƿ�˵�������
 */
void CSMVBaseDlg::_highlightCtrlBlk( int nIndex )
{
    // ��ѡ���ϴεĲ˵��򷵻�
    if(m_nHighLightCtrlBlk == nIndex)
    {
        return;
    }

    // ��ȡ����һ�εĸ����˵����ٸ�����ǰѡ�еĲ˵�
    CNaviBarMgr::GetInstance()->HighlightMenuItem(1, m_nHighLightCtrlBlk, FALSE);
    m_nHighLightCtrlBlk = nIndex;
    CNaviBarMgr::GetInstance()->HighlightMenuItem(1, m_nHighLightCtrlBlk);

    // �л����ƿ����±���
    _updateCaption();
}

/**
 *      ��ʼ����ǰ���ƿ�
 */
void CSMVBaseDlg::_initCurrentCtrlBlk()
{
    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();
    bool bInited = false;

    // �½�ҳ�棬����ҳ������
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

    // ����ҳ�棬ȡ���ɵ�ҳ������
    if((pData->m_eAction == PageBack && pData->m_pPageDataExtra) || bInited)
    {
        m_pPageDataExtra = dynamic_cast<CPageDataExtra*>(pData->m_pPageDataExtra);
        ASSERT(m_pPageDataExtra);
    }

    ASSERT(m_pCurrentCtrlBlk);

    if(m_pCurrentCtrlBlk)
    {
        // ���ؿ��ƿ��б�
        _loadCtrlBlk();

        // ���±���
        _updateCaption();

        // ����ҵ��ģʽ
        _setMode();

        // ����Ĭ�϶�ʱ��
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
        // ��Чֵ
    case IDD_SMV_EFFECTIVE_VALUE_DLG:
        enDsmMode = DSM_MODE_SMV_RECV_EFFECTIVE_VALUE;
        break;

        // ����ͼ
    case IDD_SMV_WAVEFORM_DLG:
        enDsmMode = DSM_MODE_SMV_RECV_OSCILLOGRAM;
        break;

        // ����
    case IDD_SMV_PHASOR_DLG:
        enDsmMode = DSM_MODE_SMV_RECV_PHASOR;
        break;

        // ����
    case IDD_SMV_PHASESEQU_DLG:
        enDsmMode = DSM_MODE_SMV_RECV_XU_LIANG;
        break;

        // ����
    case IDD_SMV_POWER_DLG:
        enDsmMode = DSM_MODE_SMV_RECV_POWER;
        break;

        // г��
    case IDD_SMV_HARMONIC_DLG:
        enDsmMode = DSM_MODE_SMV_RECV_HARMONIC;
        break;

        // ˫AD
    case IDD_SMV_DUALAD_DLG:
        enDsmMode = DSM_MODE_SMV_RECV_DOUBLE_AD;
        break;

        // ����
    case IDD_SMV_DUALPHASE_DLG:
        enDsmMode = DSM_MODE_SMV_RECV_PHASE_CHECK;
        break;

        // ����
    case IDD_SMV_POLARITY_DLG:
        enDsmMode = DSM_MODE_SMV_RECV_POLARITY;
        break;

        // ����ͳ�Ƽ���ɢ��
    case IDD_SMV_STATISTICS_DLG:
        // ͳ�ƺ���ɢ�ȷֿ�����ģʽ����
        // DSM_MODE_SMV_RECV_DISPERSION
        enDsmMode = DSM_MODE_SMV_RECV_FRAME_STATIC;
        break;

        // ���ļ�⼰¼��
    case IDD_SMV_MSGMONITOR_DLG:
        enDsmMode = DSM_MODE_SMV_RECV_FRAME_MONITOR;
        break;

    default:
        break;
    }

    // ����ҵ��ģʽ
    m_pDataLayer->SetCurrentMode(enDsmMode);
}



