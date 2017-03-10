/**
 * \file
 *      EventProc.cpp
 *
 * \brief
 *      页面消息处理器
 *          处理页面间跳转以及各按键消息分发
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/3/21
 *
 *  \note
 *      该类负责处理按键消息，切换窗口，发送更新状态命令
 */

#include "StdAfx.h"
#include "EventProc.h"
#include "resource.h"

#include <pm.h>
#include <wdm.h>
#include <winioctl.h>


#include "src/main/dlg/MainDlg.h"
#include "src/main/mgr/statusmgr.h"
#include "src/main/dlg/BasicCfgDlg.h"
#include "src/utils/utility/dsm_utility.h"


#include "src/main/paramcfg/BasicParamDlg.h"
#include "src/main/paramcfg/BasicParamAddGooseFromSclDlg.h"
#include "src/main/paramcfg/BasicParamAddGooseFromScanDlg.h"
#include "src/main/paramcfg/BasicParamEditGooseCtrlDlg.h"
#include "src/main/paramcfg/BasicParamViewGooseCtrlDlg.h"
#include "src/main/paramcfg/BasicParamDIMapDlg.h"
#include "src/main/paramcfg/BasicParamACDCDlg.h"
#include "src/main/paramcfg/SelSmvFromSclDlg.h"
#include "src/main/paramcfg/SelSmvFromScanDlg.h"
#include "src/main/paramcfg/EditSmvSendDlg.h"
#include "src/main/paramcfg/StatusWordSetDlg.h"

#if !SU_FEI
#include "src/main/paramcfg/BasicParamSmvTxDlg.h"
#include "src/main/paramcfg/BasicParamGooseTxDlg.h"
#include "src/main/paramcfg/BasicParamGooseRxDlg.h"
//#include "src/main/lightpower/BasicAssistedTestingDlg.h"

#include "src/main/paramcfg/BasicParamAddGooseRxFromSclDlg.h"
#include "src/main/paramcfg/BasicParamAddGooseRxFromScanDlg.h"

#include "src/main/manual/MainSendDlg.h"

#include "src/main/dlg/BasicAboutDlg.h"
#include "src/main/dlg/BasicSettingDlg.h"
#include "src/main/dlg/MainDlg.h"
#endif

#include "src/main/popdlg/HomeDlg.h"
#include "src/main/popdlg/passworddlg.h"
#include "src/main/smv/ScanSmvDlg.h"
#include "src/main/smv/smvwaveformdlg.h"
#include "src/main/smv/smvphasordlg.h"
#include "src/main/smv/smvphasesequedlg.h"
#include "src/main/smv/smvdualphasedlg.h"
#include "src/main/smv/smvmonitordlg.h"
#include "src/main/smv/smvconfigdlg.h"
#include "src/main/smv/smvdualaddlg.h"
#include "src/main/smv/smvpowerdlg.h"
#include "src/main/smv/smvmsgmonitordlg.h"
#include "src/main/smv/smvstatisticsdlg.h"


#include "src/main/smv/smveffectivevaluedlg.h"
#include "src/main/smv/smvpolaritydlg.h"
#include "src/main/smv/smvpolaritycfgdlg.h"
#include "src/main/smv/smvharmonicdlg.h"

// goose
#include "src/main/goose/goosescandlg.h"
#include "src/main/goose/gooseshiftdlg.h"
#include "src/main/goose/goosemonitordlg.h"
#include "src/main/goose/gooserealtimedlg.h"
#include "src/main/goose/goosestatisticsdlg.h"

//光功率
#include "src/main/lightpower/lplightpowerdlg.h"


#include "src/main/dlg/timebdlg.h"

#include "src/main/popdlg/lockdlg.h"
#include "src/main/popdlg/msgdlg.h"
#include "src/main/popdlg/MsgBtnDlg.h"


//
//manual
//
#include "src/main/manual/manualsmvgoosesend.h"
#include "src/main/manual/manualsetdlg.h"
#include "src/main/manual/manualsmvchanmapdlg.h"
#include "src/main/manual/manualresultlistdlg.h"
#include "src/main/manual/manualresultdiactiondlg.h"
#include "src/main/manual/manualresultdetaildlg.h"
#include "src/main/manual/manualsmvctrllistdlg.h"
#include "src/main/manual/manualsmvstatusworddlg.h"
#include "src/main/manual/manualsmvchanqualitydlg.h"
#include "src/main/manual/manualqualitysetdlg.h"
#include "src/main/manual/manualstepsetdlg.h"
//
//状态序列
//
#include "src/main/statesequence/statesequencedlg.h"
#include "src/main/statesequence/statesetdlg.h"
#include "src/main/statesequence/stateResultListdlg.h"
#include "src/main/statesequence/stateResultDIActiondlg.h"
#include "src/main/statesequence/stateHarmonicDlg.h"
#include "src/main/statesequence/stateFaultCalcDlg.h"
#include "src/main/statesequence/stateFaultCompensateDlg.h"
#include "src/main/statesequence/stateFaultImpedanceDlg.h"


#include "src/main/resourcebmp.h"




CEventProc::CEventProc(void)
: m_pCurPage(NULL)
{
}

CEventProc::~CEventProc(void)
{
    clearSource();
}

void CEventProc::OnKeyEsc()
{
    pageBack();
}


void CEventProc::OnKeyPower(DWORD dwStatu)
{
    saveFocusHwnd(::GetFocus());

    // 锁屏按钮
    if(DSM_KEYEVT_LOCK == dwStatu)
    {
        CLockDlg lockdlg(AfxGetMainWnd());

        if(IDOK == lockdlg.DoModal())
        {

        }
    }
    // 关机按钮
    else if(DSM_KEYEVT_POWEROFF == dwStatu)
    {
        CMsgBtnDlg msgdlg(DSM_STR_MSG_SHUTDOWN_TIPS, DSM_BMP_DLG_MSG_SHUTDOWN_ICON, AfxGetMainWnd());

        INT_PTR nRet = msgdlg.DoModal();
        msgdlg._ResetMenu();

        if(nRet == IDOK)
        {
            CEventProc::GetInstance()->Close();
        }
    }

    restoreFocusHwnd();
}
#if !CHUN_HUA
UINT CEventProc::OnKeyHome( bool bModal /* = false */ )
{
	if(m_pCurPage->_AllowInterrup() == FALSE)
	{
		return DSM_HOME_RETVAL_NONE;
	}
	if (IDD_MAIN_DLG == m_stackData.GetTail()->m_nPageID)
	{
		return DSM_HOME_RETVAL_NONE;
	}
	if(!bModal)
	{
		if(IDD_MAIN_DLG == m_stackData.GetTail()->m_nPageID)
		{
			restoreFocusHwnd();
		}
		else
		{
			CEventProc::GetInstance()->ShowMain();
		}
	}

	CStatusMgr::ClearSnapshot();
	return DSM_HOME_RETVAL_MAIN;
}
#else
UINT CEventProc::OnKeyHome( bool bModal /* = false */ )
{
    /******************************************************************/
    if(!bModal)
    {
        if(IDD_MAIN_DLG == m_stackData.GetTail()->m_nPageID)
        {
            restoreFocusHwnd();
        }
        else
        {
            CEventProc::GetInstance()->ShowMain();
        }
    }

    CStatusMgr::ClearSnapshot();
    return DSM_HOME_RETVAL_MAIN;

    /*****************************************************************************/
    if(m_pCurPage->_AllowInterrup() == FALSE)
    {
        return DSM_HOME_RETVAL_NONE;
    }

    CStatusMgr::Snapshot();

    CHomeDlg dlg(m_pCurPage);

    saveFocusHwnd(::GetFocus());

    INT_PTR nRet = dlg.DoModal();

    switch(nRet)
    {
    case DSM_HOME_RETVAL_MAIN:

        if(!bModal)
        {
            if(IDD_MAIN_DLG == m_stackData.GetTail()->m_nPageID)
            {
                restoreFocusHwnd();
            }
            else
            {
                CEventProc::GetInstance()->ShowMain();
            }
        }

        CStatusMgr::ClearSnapshot();
        return DSM_HOME_RETVAL_MAIN;

    case DSM_HOME_RETVAL_SNAP:


        // 检查SD的截图目录是否存在
        CString csSDPath;
        if(!DSM_GetSDDriver(csSDPath))
#if SHENG_ZE
		{
            if(DSM_CreateSDPath(csSDPath + DSM_SNAPSHOT_FOLDER))
            {
                CStatusMgr::SaveSnapshot();
            }
            else
            {
                CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_SNAPSHOT_PATH_ERROR));
            }
        }
        else
#endif

        {
            // SD卡未插入
            CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_SD_NOT_FOUND_SNAP));
        }

        CStatusMgr::ClearSnapshot();
        restoreFocusHwnd();
        return DSM_HOME_RETVAL_SNAP;
    }

    CStatusMgr::ClearSnapshot();

    restoreFocusHwnd();

    return DSM_HOME_RETVAL_NONE;
}
#endif

CBaseDlg* CEventProc::pageCreate( UINT nPageID )
{
    CBaseDlg* pPage = NULL;

    switch(nPageID)
    {
    case IDD_MAIN_DLG:
        pPage = new CMainDlg;		
        break;

    case IDD_BASIC_CFG_DLG:
        pPage = new CBasicCfgDlg;
        break;

    case IDD_BASIC_PARAM_DLG:
        pPage = new CBasicParamDlg;
        break;

    case IDD_BASIC_PARAM_ADD_GOOSE_FROM_SCL_DLG:
        pPage = new CBasicParamAddGooseFromSclDlg;
        break;

    case IDD_BASIC_PARAM_ADD_GOOSE_FROM_SCAN_DLG:
        pPage = new CBasicParamAddGooseFromScanDlg;
        break;

#if !SU_FEI
	case IDD_BASIC_PARAM_ADD_GOOSE_RX_FROM_SCL_DLG:
		pPage = new CBasicParamAddGooseRxFromSclDlg;
		break;

	case IDD_BASIC_PARAM_ADD_GOOSE_RX_FROM_SCAN_DLG:
		pPage = new CBasicParamAddGooseRxFromScanDlg;
		break;

	case IDD_BASIC_SETTING_DLG:
		pPage = new CBasicSettingDlg;
		break;
		
	case IDD_BASIC_ABOUT_DLG:
		pPage = new CBasicAboutDlg;
		break;
#endif
    case IDD_BASIC_PARAM_EDIT_GOOSE_CTRL_DLG:
        pPage = new CBasicParamEditGooseCtrlDlg;
        break;

    case IDD_BASIC_PARAM_VIEW_GOOSE_CTRL_DLG:
        pPage = new CBasicParamViewGooseCtrlDlg;
        break;

    case IDD_BASIC_PARAM_DI_MAP_DLG:
        pPage = new CBasicParamDIMapDlg;
        break;

    case IDD_SEL_SMV_FROM_SCL_DLG:
        pPage = new CSelSmvFromSclDlg;
        break;

    case IDD_SEL_SMV_FROM_SCAN_DLG:
        pPage = new CSelSmvFromScanDlg;
        break;

    case IDD_EDIT_SMV_SEND_DLG:
        pPage = new CEditSmvSendDlg;
        break;

    case IDD_BASIC_PARAM_ACDC_DLG:
        pPage = new CBasicParamACDCDlg;
        break;

    case IDD_STATUS_WORD_SET_DLG:
        pPage = new CStatusWordSetDlg;
        break;
#if !SU_FEI
	case IDD_PARAM_SMV_TX_DLG:
		pPage = new CBasicParamSmvTxDlg;
		break;	
	case IDD_PARAM_GOOSE_TX_DLG:
		pPage = new CBasicParamGooseTxDlg;
		break;
	case IDD_PARAM_GOOSE_RX_DLG:
		pPage = new CBasicParamGooseRxDlg;
		break;
#endif

//////////////////////////////////////////////////////////////////////////
    // manual
    case IDD_MANUAL_SMV_GOOSE_SEND_DLG:
        pPage = new CManualSmvGooseSendDlg;
        break;

    case IDD_MANUAL_SMV_CHAN_MAP_DLG:
        pPage = new CManualSmvChanMapDlg;
        break;

    case IDD_MANUAL_SET_DLG:
        pPage = new CManualSetDlg;
        break;

    case IDD_MANUAL_RESULT_LIST_DLG:
        pPage = new CManualResultListDlg;
        break;

    case IDD_MANUAL_RESULT_DI_ACTION_DLG:
        pPage = new CManualResultDIActionDlg;
        break;

    case IDD_MANUAL_RESULT_DETAIL_DLG:
        pPage = new CManualResultDetailDlg;
        break;

    case IDD_MANUAL_SMV_CTRL_LIST_DLG:
        pPage = new CManualSmvCtrlListDlg;
        break;

    case IDD_MANUAL_SMV_STATUS_WORD_DLG:
        pPage = new CManualSmvStatusWordDlg;
        break;

    case IDD_MANUAL_SMV_CHAN_QUALITY_DLG:
        pPage = new CManualSmvChanQualityDlg;
        break;

    case IDD_MANUAL_QUALITY_SET_DLG:
        pPage = new CManualQualitySetDlg;
        break;

    case IDD_MANUAL_STEP_SET_DLG:
        pPage = new CManualStepSetDlg;
        break;
//////////////////////////////////////////////////////////////////////////
    // 状态序列
#if !SU_FEI
	case IDD_MAIN_SEND_DLG:
		pPage = new CMainSendDlg;
		break;
#endif
    case IDD_STATE_SEQUENCE_DLG:
        pPage = new CStateSequenceDlg;
        break;

    case IDD_STATE_SET_DLG:
        pPage = new CStateSetDlg;
        break;

    case IDD_STATE_RESULT_LIST_DLG:
        pPage = new CStateResultListDlg;
        break;

    case IDD_STATE_RESULT_DI_ACTION_DLG:
        pPage = new CStateResultDIActionDlg;
        break;

    case IDD_STATE_HARMONIC_DLG:
        pPage = new CStateHarmonicDlg;
        break;

    case IDD_STATE_FAULT_CALC_DLG:
        pPage = new CStateFaultCalcDlg;
        break;

    case IDD_STATE_FAULT_COMPENSATE_DLG:
        pPage = new CStateFaultCompensateDlg;
        break;

    case IDD_STATE_FAULT_IMPEDANCE_DLG:
        pPage = new CStateFaultImpedanceDlg;
        break;

//////////////////////////////////////////////////////////////////////////
    // smv
    case IDD_SCAN_SMV_DLG:
        pPage = new CScanSmvDlg;
        break;

    case IDD_SMV_WAVEFORM_DLG:
        pPage = new CSMVWaveformDlg;
        break;

    case IDD_SMV_PHASOR_DLG:
        pPage = new CSMVPhasorDlg;
        break;

    case IDD_SMV_PHASESEQU_DLG:
        pPage = new CSMVPhasesequeDlg;
        break;

    case IDD_SMV_DUALPHASE_DLG:
        pPage = new CSMVDualphaseDlg;
        break;
#if SU_FEI
	/************************************************************************/
	/* 取消了单独状态监视页面                                               */
	/************************************************************************/
    case IDD_SMV_MONITOR_DLG:
        pPage = new CSMVMonitorDlg;
        break;
#endif
    case IDD_SMV_CONFIG_DLG:
        pPage = new CSMVConfigDlg;
        break;

    case IDD_SMV_DUALAD_DLG:
        pPage = new CSMVDualADDlg;
        break;

    case IDD_SMV_POWER_DLG:
        pPage = new CSMVPowerDlg;
        break;

    case IDD_SMV_EFFECTIVE_VALUE_DLG:
        pPage = new CSmvEffectiveValueDlg;
        break;

    case IDD_SMV_POLARITY_DLG:
        pPage = new CSmvPolarityDlg;
        break;

    case IDD_SMV_POLARITY_CFG_DLG:
        pPage = new CSmvPolarityCfgDlg;
        break;

    case IDD_SMV_HARMONIC_DLG:
        pPage = new CSmvHarmonicDlg;
        break;

    case IDD_SMV_MSGMONITOR_DLG:
        pPage = new CSMVMsgMonitorDlg;
        break;

    case IDD_SMV_STATISTICS_DLG:
        pPage = new CSMVStatisticsDlg;
        break;

//////////////////////////////////////////////////////////////////////////
    // goose

    case IDD_GOOSE_SCAN_DLG:
        pPage = new CGooseScanDlg;
        break;

    case IDD_GOOSE_REALTIME_DLG:
        pPage = new CGooseRealTimeDlg;
        break;

    case IDD_GOOSE_SHIFT_DLG:
        pPage = new CGooseShiftDlg;
        break;

    case IDD_GOOSE_STATISTICS_DLG:
        pPage = new CGooseStatisticsDlg;
        break;

    case IDD_GOOSE_MONITOR_DLG:
        pPage = new CGooseMonitorDlg;
        break;
    
//////////////////////////////////////////////////////////////////////////
    case IDD_TIME_B_DLG:
        pPage = new CTimeBDlg;
        break;

//////////////////////////////////////////////////////////////////////////
//光功率
#if !SU_FEI
    case IDD_LP_LIGHT_POWER_DLG:
        pPage = new CLpLightPowerDlg;
        break;
#else
	///< 辅助测试
	case IDD_ASSISTED_TESTING_DLG:
		pPage = new CBasicAssistedTestingDlg;	
		break;
#endif
	default:
        ASSERT(FALSE);
    }

    return pPage;
}


void CEventProc::ShowMain()
{
    pageForward(IDD_MAIN_DLG);
}

void CEventProc::ShowWizard()
{
    if(dsm_config::GetSystemInit())
    {
        ShowMain();
    }
    else
    {
        pageForward(IDD_BASIC_CFG_DLG);
    }
}

void CEventProc::Exit()
{
    if(m_pCurPage)
    {
        // 保存页面状态数据
        m_pCurPage->_SavePage();

        m_pCurPage->DestroyWindow();
        delete m_pCurPage;
        m_pCurPage = NULL;
    }

    clearSource();
}

void CEventProc::ShowPage( UINT nPageID )
{
    pageForward(nPageID);
}

void CEventProc::Close()
{
    // 保存配置
    SaveConfig();

#define IOCTL_BATTERY_SET_SYSTEM_POWEROFF CTL_CODE(FILE_DEVICE_BATTERY, 0x112, METHOD_BUFFERED, FILE_ANY_ACCESS)

    HANDLE g_file = CreateFile(_T("BAT1:"),GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,OPEN_EXISTING,0, 0);
    if (g_file == INVALID_HANDLE_VALUE){
        return;
    }

    DeviceIoControl(g_file,IOCTL_BATTERY_SET_SYSTEM_POWEROFF,NULL,0,NULL,0,NULL,NULL);
}

bool CEventProc::isBusinessPage( UINT nPageID )
{
    switch(nPageID)
    {
        // smv
    case IDD_SMV_EFFECTIVE_VALUE_DLG:
    case IDD_SMV_WAVEFORM_DLG:
    case IDD_SMV_PHASOR_DLG:
    case IDD_SMV_PHASESEQU_DLG:
    case IDD_SMV_POWER_DLG:
    case IDD_SMV_HARMONIC_DLG:
    case IDD_SMV_DUALAD_DLG:
    case IDD_SMV_DUALPHASE_DLG:
    case IDD_SMV_POLARITY_DLG:
    case IDD_SMV_STATISTICS_DLG:
    case IDD_SMV_MSGMONITOR_DLG:
        return true;

        // goose
    case IDD_GOOSE_MONITOR_DLG:
    case IDD_GOOSE_REALTIME_DLG:
    case IDD_GOOSE_SHIFT_DLG:
    case IDD_GOOSE_STATISTICS_DLG:
        return true;

    default:
        break;
    }

    return false;
}

void CEventProc::saveFocusHwnd( HWND hWnd )
{
    m_stackAncestor.RemoveAll();

    CWnd* pWnd      = NULL;
    HWND hWndEnum   = hWnd;
    HWND hWndMain   = AfxGetMainWnd()->GetSafeHwnd();

    while(hWndMain != hWndEnum)
    {
        if(FALSE == IsWindow(hWndEnum))
        {
            break;
        }

        m_stackAncestor.AddHead(hWndEnum);

        pWnd        = CWnd::FromHandle(hWndEnum);
        hWndEnum    = pWnd->GetOwner()->GetSafeHwnd();
    }

    m_stackAncestor.AddHead(hWndMain);
}

void CEventProc::restoreFocusHwnd()
{
    HWND hWndFocus = NULL;

    while(m_stackAncestor.GetCount() > 0)
    {
        hWndFocus = m_stackAncestor.GetTail();

        if(IsWindow(hWndFocus))
        {
            ::SetFocus(hWndFocus);
            return;
        }

        m_stackAncestor.RemoveTail();
    }

    if(m_pCurPage && IsWindow(m_pCurPage->GetSafeHwnd()))
    {
        hWndFocus = m_pCurPage->GetSafeHwnd();
    }
    else
    {
        hWndFocus = AfxGetMainWnd()->GetSafeHwnd();
    }

    ::SetFocus(hWndFocus);
}

CPageData* CEventProc::GetCurrentPageData()
{
    return m_stackData.GetTail();
}

CPageData* CEventProc::GetPreviousPageData()
{
    int nPrevious = m_stackData.GetCount() - 2;

    if(0 <= nPrevious)
    {
        return m_stackData.GetAt(m_stackData.FindIndex(nPrevious));
    }

    return NULL;
}

bool DSM_PASSWORD_PROC(CString csPassword, DWORD)
{
    return dsm_config::VerifyPasswrod(csPassword);
}

bool CEventProc::ShowPassword()
{
    saveFocusHwnd(::GetFocus());

    CPasswordDlg dlg;
    dlg._SetProc(DSM_PASSWORD_PROC, NULL);

    INT_PTR nRet = dlg.DoModal();

    restoreFocusHwnd();

    if(IDOK == nRet)
    {
        return true;
    }

    return false;
}

void CEventProc::clearPageStack()
{
    CPageData* pData    = NULL;
    POSITION pos        = m_stackData.GetHeadPosition();

    while(pos)
    {
        pData = m_stackData.GetNext(pos);

        if(pData)
        {
            if(pData->m_pPageDataExtra)
            {
                delete pData->m_pPageDataExtra;
            }

            delete pData;
        }
    }

    m_stackData.RemoveAll();
}

void CEventProc::clearMapBussData()
{
    UINT nID            = 0xFFFFFFFF;
    CPageData* pData    = NULL;
    POSITION pos        = m_mapBussData.GetStartPosition();

    while(pos)
    {
        m_mapBussData.GetNextAssoc(pos, nID, pData);

        if(pData)
        {
            if(pData->m_pPageDataExtra)
            {
                delete pData->m_pPageDataExtra;
            }

            delete pData;
        }
    }

    m_mapBussData.RemoveAll();
}

void CEventProc::clearSource()
{
    m_pCurPage = NULL;

    clearMapBussData();
    clearPageStack();
    m_stackAncestor.RemoveAll();
}

void CEventProc::ResetFactory()
{
    CString     csSrcPathName;
    CString     csDstPathName;

    dsm_config::Reset();
    CDsmDataLayer::getInstance()->GetSmvConfigPublic()->Reset();
    CDsmDataLayer::getInstance()->GetParamConfigBasic()->Reset();

    //
    // 恢复SVM发送配置文件
    //
    {
        // SMV发送配置管理器
        CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
        if (pParamConfigSMVSend)
        {
            // 拷贝备份的配置文件
            csSrcPathName.Format(L"%s%s%s", DSM_GetAppDir(), DEFAULT_CFG_BACKUP.c_str(), PARAM_CONFIG_SMV_SEND);
            csDstPathName = DSM_GetRuntimeDir() + PARAM_CONFIG_SMV_SEND;
            if(CopyFile(csSrcPathName, csDstPathName, FALSE))
            {
                // 重新加载配置文件
                pParamConfigSMVSend->LoadConfig(csDstPathName.GetString());
            }
        }
    }

    //
    // 恢复GOOSE发送配置文件
    //
    {
        // GOOSE发送配置管理器
        CParamConfigGOOSESend* pParamConfigGOOSESend = CParamConfigGOOSESend::getInstance();
        if (pParamConfigGOOSESend)
        {
            // 拷贝备份的配置文件
            csSrcPathName.Format(L"%s%s%s", DSM_GetAppDir(), DEFAULT_CFG_BACKUP.c_str(), PARAM_CONFIG_GOOSE_SEND);
            csDstPathName = DSM_GetRuntimeDir() + PARAM_CONFIG_GOOSE_SEND;
            if(CopyFile(csSrcPathName, csDstPathName, FALSE))
            {
                // 重新加载配置文件
                pParamConfigGOOSESend->LoadConfig(csDstPathName.GetString());
            }
        }
    }

    //
    // 恢复GOOSE接收配置文件
    //
    {
        // GOOSE接收配置管理器
        CParamConfigGOOSERecv* pParamConfigGOOSERecv = CParamConfigGOOSERecv::getInstance();
        if (pParamConfigGOOSERecv)
        {
            // 拷贝备份的配置文件
            csSrcPathName.Format(L"%s%s%s", DSM_GetAppDir(), DEFAULT_CFG_BACKUP.c_str(), PARAM_CONFIG_GOOSE_RECV);
            csDstPathName = DSM_GetRuntimeDir() + PARAM_CONFIG_GOOSE_RECV;
            if(CopyFile(csSrcPathName, csDstPathName, FALSE))
            {
                // 重新加载配置文件
                pParamConfigGOOSERecv->LoadConfig(csDstPathName.GetString());
            }
        }
    }

    //
    // 恢复手动试验配置文件
    //
    {
        // 手动试验配置管理器
        CParamconfigManualTest* pParamConfigManualTest = CParamconfigManualTest::getInstance();
        if (pParamConfigManualTest)
        {
            // 拷贝备份的配置文件
            csSrcPathName.Format(L"%s%s%s", DSM_GetAppDir(), DEFAULT_CFG_BACKUP.c_str(), PARAM_CONFIG_MANUAL_TEST);
            csDstPathName = DSM_GetRuntimeDir() + PARAM_CONFIG_MANUAL_TEST;
            if(CopyFile(csSrcPathName, csDstPathName, FALSE))
            {
                // 重新加载配置文件
                pParamConfigManualTest->LoadConfig(csDstPathName.GetString());
            }
        }
    }

    //
    // 恢复状态序列配置文件
    //
    {
        // 状态序列配置管理器
        CParamConfigStateSequence* pParamConfigStateSequence = CParamConfigStateSequence::getInstance();
        if (pParamConfigStateSequence)
        {
            // 拷贝备份的配置文件
            csSrcPathName.Format(L"%s%s%s", DSM_GetAppDir(), DEFAULT_CFG_BACKUP.c_str(), PARAM_CONFIG_STATE_SEQUENCE);
            csDstPathName = DSM_GetRuntimeDir() + PARAM_CONFIG_STATE_SEQUENCE;
            if(CopyFile(csSrcPathName, csDstPathName, FALSE))
            {
                pParamConfigStateSequence->LoadConfig(csDstPathName.GetString());
            }
        }
    }

    // 重置后重新设置背光时间与亮度
    CStatusMgr::SetBrightness(CStatusMgr::GetBrightness());
    CStatusMgr::SetBrightTime(CStatusMgr::GetBrightTime());

    if(!dsm_config::GetSystemInit())
    {
        // 关闭当前页面并清理资源
        Exit();

        ShowWizard();
    }
}


/**
 *      保存配置文件
 */
void CEventProc::SaveConfig()
{
    dsm_config::Save();
    CDsmDataLayer::getInstance()->GetSmvConfigPublic()->SaveToConfigFile();
    CDsmDataLayer::getInstance()->GetParamConfigBasic()->SaveConfig();
    CParamConfigSMVSend::getInstance()->SaveConfig();
    CParamConfigGOOSESend::getInstance()->SaveConfig();
    CParamConfigGOOSERecv::getInstance()->SaveConfig();
}

/**
 *      截图保存
 */
void CEventProc::SnapShot()
{
    CStatusMgr::Snapshot();

	CString csSDPath;
#if SHENG_ZE
    // 检查SD的截图目录是否存在
    CString csSDPath;
    if(DSM_GetSDDriver(csSDPath))
    {
        if(DSM_CreateSDPath(csSDPath + DSM_SNAPSHOT_FOLDER))
        {
            CStatusMgr::SaveSnapshot();
        }
        else
        {
            CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_SNAPSHOT_PATH_ERROR));
        }
    }
    else
#endif
	if(!DSM_GetSDDriver(csSDPath))
    {
        // SD卡未插入
        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_SD_NOT_FOUND_SNAP));
    }

    CStatusMgr::ClearSnapshot();
}

HWND CEventProc::GetCurrentPageHwnd()
{
    return m_pCurPage ? m_pCurPage->GetSafeHwnd() : NULL;
}

void CEventProc::pageForward( UINT nPageID )
{
    // 打开相同的页面则返回
    if(m_stackData.GetCount() > 0 && nPageID == m_stackData.GetTail()->m_nPageID)
    {
        return;
    }

    pageShow(nPageID);
}

void CEventProc::pageBack()
{
    if(m_stackData.GetCount() >= 2 && m_pCurPage)
    {
        POSITION pos = m_stackData.FindIndex(m_stackData.GetCount() - 2);
        pageShow(m_stackData.GetPrev(pos)->m_nPageID, PageBack);
    }
}

void CEventProc::pageShow( UINT nPageID, DsmPageAction eAction /*= PageEnter*/)
{
    // 当前窗口是否愿意退出
    if(m_pCurPage && !m_pCurPage->_AllowExit())
    {
        return;
    }

    CBaseDlg* pNewPage = pageCreate(nPageID);

    if(m_pCurPage)
    {
        m_pCurPage->_SavePage();
        m_pCurPage->DestroyWindow();

        delete m_pCurPage;
    }

    m_pCurPage = pNewPage;

    updatePageStack(nPageID, eAction);

    m_pCurPage->Create(nPageID, AfxGetMainWnd());
}

void CEventProc::updatePageStack( UINT nPageID, DsmPageAction eAction )
{
    // 主页面
    if(IDD_MAIN_DLG == nPageID)
    {
        processPageMain();     // 清空页面栈数据
    }
    // 业务页面
    else if(eAction == PageEnter && isBusinessPage(nPageID) && isBusinessPage(m_stackData.GetTail()->m_nPageID))
    {
        CPageData* pData = NULL;

        // 单独保存同级业务页面数据
        if(!m_mapBussData.Lookup(m_stackData.GetTail()->m_nPageID, pData))
        {
            m_stackData.GetTail()->m_eAction = PageBack;
            m_mapBussData.SetAt(m_stackData.GetTail()->m_nPageID, m_stackData.GetTail());
        }

        // 从栈中移除上一业务数据
        m_stackData.RemoveTail();
    }
    else if(eAction == PageBack && nPageID != IDD_MAIN_DLG)
    {
        processPageBack();
    }

    // 添加页面栈数据
    if(!lookupPageDataByID(nPageID))
    {
        CPageData* pData = NULL;

        if(!m_mapBussData.Lookup(nPageID, pData))
        {
            pData = new CPageData(nPageID);
        }

        m_stackData.AddTail(pData);
    }
}

void CEventProc::processPageMain()
{
    if(m_stackData.GetCount() > 0)
    {
        CPageData* pData = m_stackData.GetHead();

        if(pData && pData->m_nPageID == IDD_MAIN_DLG)
        {
            m_stackData.RemoveHead();
            clearPageStack();
            m_stackData.AddTail(pData);

            pData->m_eAction = PageBack;
        }
        else
        {
            clearPageStack();
        }
    }

    clearMapBussData();
}

void CEventProc::processPageBack()
{
    ASSERT(m_stackData.GetCount() >=  2);

    // 弹出并清理最后页面的数据
    CPageData* pData = m_stackData.RemoveTail();

    if(pData)
    {
        if(isBusinessPage(pData->m_nPageID))
        {
            clearMapBussData();
        }

        if(pData->m_pPageDataExtra)
        {
            delete pData->m_pPageDataExtra;
        }

        delete pData;
    }

    // 修改返回页的action状态为返回
    pData = m_stackData.GetTail();
    if(pData)
    {
        pData->m_eAction = PageBack;
    }
}

CPageData* CEventProc::lookupPageDataByID( UINT nPageID )
{
    CPageData* pData    = NULL;
    POSITION pos        = m_stackData.GetHeadPosition();

    while(pos)
    {
        pData = m_stackData.GetNext(pos);

        if(pData && pData->m_nPageID == nPageID)
        {
            return pData;
        }
    }

    return NULL;
}

