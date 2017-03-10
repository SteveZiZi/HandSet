/**
 * \file
 *      SMVMsgMonitorDlg.cpp
 *
 * \brief
 *      报文监测及录波页面实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/5/11
 */

// SMVMsgMonitorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SMVMsgMonitorDlg.h"

#include "src/main/resourcebmp.h"
#include "src/main/utils/decode.h"
#include "src/main/utils/utility.h"
#include "src/main/mgr/statusmgr.h"
#include "src/main/popdlg/msgdlg.h"
#include "src/main/smv/common_smv.h"
#include "src/main/dlg/dsmfiledlg.h"
#include "src/main/popdlg/common_pop.h"
#include "src/main/dlg/pcapbrowserdlg.h"
#include "src/utils/utility/dsm_utility.h"


// CSMVMsgMonitorDlg 对话框

IMPLEMENT_DYNAMIC(CSMVMsgMonitorDlg, DSM_SMVBASEDLG)

CSMVMsgMonitorDlg::CSMVMsgMonitorDlg(CWnd* pParent /*=NULL*/)
	: DSM_SMVBASEDLG(CSMVMsgMonitorDlg::IDD, DSM_STR_SMV_MSGMONITOR_CAPTION, pParent)
    , m_pMsg91(NULL)
    , m_pMsg92(NULL)
    , m_pMsgNanRui(NULL)
    , m_pMsgGuoWang(NULL)
    , m_csPcapFiles(L"")
    , m_nTreeRows(0)
    , m_pMsgDlg(NULL)
    , m_nRealTimer(0)
    , m_nRecordTimer(0)
	, m_csPcapPath(L"")
{
    m_bEnableTimer = true;
}

CSMVMsgMonitorDlg::~CSMVMsgMonitorDlg()
{
    ReleaseSav91MsgImpl(m_pMsg91);
    ReleaseSav92MsgImpl(m_pMsg92);
    ReleaseNanRuiFT3MsgImpl(m_pMsgNanRui);
    ReleaseGuoWangFT3MsgImpl(m_pMsgGuoWang);
}

void CSMVMsgMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	DSM_SMVBASEDLG::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSMVMsgMonitorDlg, DSM_SMVBASEDLG)
    ON_COMMAND(ID_MENU_SMV_F3, &CSMVMsgMonitorDlg::OnMenuF3)
    ON_COMMAND(ID_MENU_SMV_F5, &CSMVMsgMonitorDlg::OnMenuF5)
    ON_COMMAND(ID_MENU_SMV_F6, &CSMVMsgMonitorDlg::OnMenuF6)
#if !SU_FEI

	ON_COMMAND(ID_MENU_SMV_F4, &CSMVMsgMonitorDlg::OnMenuF4)
	
#endif
    ON_MESSAGE(WM_DATA_LAYER_MESSAGE, &CSMVMsgMonitorDlg::OnHandleRecordException)
    ON_WM_TIMER()
END_MESSAGE_MAP()



// CSMVMsgMonitorDlg 消息处理程序

BOOL CSMVMsgMonitorDlg::OnInitDialog()
{
    CSMVBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    if(!m_pCurrentCtrlBlk && m_pDataLayer->GetSelectedCtrlBlkCount() > 0)
    {
        m_pDataLayer->SetCurrentCtrlBlk(m_pDataLayer->GetFirstSelectCtrlBlk());
        m_pCurrentCtrlBlk = m_pDataLayer->GetCurrentCtrlBlk();
    }

    if(m_pCurrentCtrlBlk)
    {
        // 加载控制块列表
        _loadCtrlBlk();
        m_pDataLayer->SetCurrentCtrlBlk(m_pCurrentCtrlBlk);
        _updateCaption();

        m_pDataLayer->SetCurrentMode(DSM_MODE_SMV_RECV_REAL_TIME_MSG);
    }

    // 显示实时报文控件
    _showFrame();

    return FALSE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


void CSMVMsgMonitorDlg::_modifyMenu()
{
    __super::_modifyMenu();

    m_menuMain.SetMenuSubItem(5, NULL);
    m_menuMain.SetMenuItemId(5, ID_MENU_SMV_F6);
	m_menuMain.SetMenuItemCaption(5, L"");
	

    m_menuMain.SetMenuItemCaption(2, dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_REFRESH));
#if !SU_FEI
	m_menuMain.SetMenuItemId(3, ID_MENU_SMV_F4);
	//m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_9X_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_9X_SEL), NULL);
	m_menuMain.SetMenuItemCaption(3, L"浏览文件");
	//m_menuMain.HighlightMenuItem(3, TRUE);
#endif
   // m_menuMain.SetMenuItemCaption(4, dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_LOADFILE));
#if SU_FEI
    m_menuMain.SetMenuItemCaption(5, dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_BEGIN_REC));
#else
	m_menuMain.SetMenuItemCaption(4, L"录波");
#endif

	m_menuMain.SetMenuItemCaption(0, dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_CAPTION));
	m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);
	m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);


}

bool CSMVMsgMonitorDlg::_switchCtrlBlk(UINT nCmdID)
{
    bool bRet = __super::_switchCtrlBlk(nCmdID);

    if(bRet)
    {
        m_treeFrame.DeleteAllTreeItems();

        // 打开定时器直到获取一次有效数据后才停止
        m_nRealTimer = SetTimer(0x01, 200, NULL);
    }

    return bRet;
}

void CSMVMsgMonitorDlg::OnMenuF5()
{
#if !SU_FEI
	if(!_checkforRecordSMV())
	{
		return;
	}

	CString csTitle = CStatusMgr::GetTitle();
	LBT_HTREEITEM item = m_treeFrame.GetTreeSelectedItem();

	// smv录波
	bool bRet = _recordSmv();
	m_pDataLayer->SetCurrentMode(DSM_MODE_SMV_RECV_REAL_TIME_MSG);

	INT_PTR nRet = IDOK;

#if !CHUN_HUA
	CStatusMgr::SetTips(L"");

	HWND hWnd       = ::GetFocus();
	// SD卡目录
	CString csSDPath;

	// 判断SD卡是否存在
	if(!DSM_GetSDDriver(csSDPath))
	{
		CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_SD_NOT_FOUND_PCAP));
		return;
	}

	CString csPcapFolder = csSDPath + DSM_PCAP_SMV_FILE_FOLDER+m_csPcapPath;     // pcap目录

	// 打开pcap文件夹失败
	if(!DSM_CreateSDPath(csPcapFolder))
	{
		CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_FOLDER_NOT_FOUND_PCAP));
		return;
	}

	// 加载pcap文件
	CString csExt;
	csExt.Format(L"*.%s|", DSM_PCAP_FILE_EXTENSION);
	CDsmFileDlg dlg(0, csPcapFolder, L"", csExt, 0, this, dsm_stringMgr::Get(DSM_STR_FILEBROWSER_TITLE));

	
	
	nRet    = dlg.DoModal();
	if(nRet == IDOK)
	{
		m_csPcapFiles = dlg._GetFilePath();

		// 加载pcap包
		CPcapBrowserDlg dlg(m_csPcapFiles);
		nRet = dlg.DoModal();
	}

	if(nRet == IDBACKTOMAIN)
	{
		return;
	}

	if(::IsWindow(m_treeFrame.GetSafeHwnd()) && item)
	{
		CStatusMgr::SetTitle(csTitle);
		m_treeFrame.SetFocus();
		m_treeFrame.SelectTreeItem(item);
	}
	else
	{
		DSM_SetZFocus(CEventProc::GetInstance()->GetCurrentPageHwnd());
	}
#else
	if(bRet)
	{
		// 加载pcap包
		CPcapBrowserDlg dlg(m_csPcapFiles);
		nRet = dlg.DoModal();
	}

	if(nRet == IDBACKTOMAIN)
	{
		return;
	}

	if(::IsWindow(m_treeFrame.GetSafeHwnd()) && item)
	{
		CStatusMgr::SetTitle(csTitle);
		m_treeFrame.SetFocus();
		m_treeFrame.SelectTreeItem(item);
	}
	else
	{
		DSM_SetZFocus(CEventProc::GetInstance()->GetCurrentPageHwnd());
	}
#endif
#else

    CStatusMgr::SetTips(L"");

    // SD卡目录
    CString csSDPath;

    // 判断SD卡是否存在
    if(!DSM_GetSDDriver(csSDPath))
    {
        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_SD_NOT_FOUND_PCAP));
        return;
    }

    CString csPcapFolder = csSDPath + DSM_PCAP_SMV_FILE_FOLDER;     // pcap目录

    // 打开pcap文件夹失败
    if(!DSM_CreateSDPath(csPcapFolder))
    {
        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_FOLDER_NOT_FOUND_PCAP));
        return;
    }

    // 加载pcap文件
    CString csExt;
    csExt.Format(L"*.%s|", DSM_PCAP_FILE_EXTENSION);
    CDsmFileDlg dlg(0, csPcapFolder, L"", csExt, 0, this, dsm_stringMgr::Get(DSM_STR_FILEBROWSER_TITLE));

    CString csTitle(CStatusMgr::GetTitle());

    HWND hWnd       = ::GetFocus();
    INT_PTR nRet    = dlg.DoModal();
    if(nRet == IDOK)
    {
        m_csPcapFiles = dlg._GetFilePath();

        CString csTitle = CStatusMgr::GetTitle();

        // 加载pcap包
        CPcapBrowserDlg dlg(m_csPcapFiles);
        nRet = dlg.DoModal();
    }

    if(nRet == IDBACKTOMAIN)
    {
        return;
    }

    if(IsWindow(hWnd))
    {
        CStatusMgr::SetTitle(csTitle);
        ::SetFocus(hWnd);
    }
    else
    {
        DSM_SetZFocus(CEventProc::GetInstance()->GetCurrentPageHwnd());
    }
#endif
}

void CSMVMsgMonitorDlg::OnMenuF3()
{
    // 刷新一帧报文
    _refreshFrame();
}

#if !SU_FEI

void CSMVMsgMonitorDlg::OnMenuF4()
{
	CStatusMgr::SetTips(L"");

	// SD卡目录
	CString csSDPath;

	// 判断SD卡是否存在
	if(!DSM_GetSDDriver(csSDPath))
	{
		CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_SD_NOT_FOUND_PCAP));
		return;
	}

	CString csPcapFolder = csSDPath + DSM_PCAP_SMV_FILE_FOLDER;     // pcap目录

	// 打开pcap文件夹失败
	if(!DSM_CreateSDPath(csPcapFolder))
	{
		CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_FOLDER_NOT_FOUND_PCAP));
		return;
	}

	// 加载pcap文件
	CString csExt;
	csExt.Format(L"*.%s|", DSM_PCAP_FILE_EXTENSION);
	CDsmFileDlg dlg(0, csPcapFolder, L"", csExt, 0, this, dsm_stringMgr::Get(DSM_STR_FILEBROWSER_TITLE));

	CString csTitle(CStatusMgr::GetTitle());

	HWND hWnd       = ::GetFocus();
	INT_PTR nRet    = dlg.DoModal();
	if(nRet == IDOK)
	{
		m_csPcapFiles = dlg._GetFilePath();

		// 加载pcap包
		CPcapBrowserDlg dlg(m_csPcapFiles);
		nRet = dlg.DoModal();
	}

	if(nRet == IDBACKTOMAIN)
	{
		return;
	}

	if(IsWindow(hWnd))
	{
		CStatusMgr::SetTitle(csTitle);
		::SetFocus(hWnd);
	}
	else
	{
		DSM_SetZFocus(CEventProc::GetInstance()->GetCurrentPageHwnd());
	}
}
#endif

void CSMVMsgMonitorDlg::OnMenuF6()
{
    //if(!_checkforRecordSMV())
    //{
    //    return;
    //}

    //CString csTitle = CStatusMgr::GetTitle();
    //LBT_HTREEITEM item = m_treeFrame.GetTreeSelectedItem();

    //// smv录波
    //bool bRet = _recordSmv();
    //m_pDataLayer->SetCurrentMode(DSM_MODE_SMV_RECV_REAL_TIME_MSG);

    //INT_PTR nRet = IDOK;

    //if(bRet)
    //{
    //    // 加载pcap包
    //    CPcapBrowserDlg dlg(m_csPcapFiles);
    //    nRet = dlg.DoModal();
    //}

    //if(nRet == IDBACKTOMAIN)
    //{
    //    return;
    //}

    //if(::IsWindow(m_treeFrame.GetSafeHwnd()) && item)
    //{
    //    CStatusMgr::SetTitle(csTitle);
    //    m_treeFrame.SetFocus();
    //    m_treeFrame.SelectTreeItem(item);
    //}
    //else
    //{
    //    DSM_SetZFocus(CEventProc::GetInstance()->GetCurrentPageHwnd());
    //}
}


void CSMVMsgMonitorDlg::_showFrame()
{
    if(FALSE == IsWindow(m_treeFrame.GetSafeHwnd()))
    {
        _createTree();
    }

    m_treeFrame.ShowWindow(SW_SHOW);
    m_treeFrame.SetFocus();

    // 获取新的一帧数据
    if(m_pDataLayer->GetRealTimeMsg(&m_data))
    {
        ASSERT(m_data.m_enFrameType == m_pCurrentCtrlBlk->enFrameType);
        _updateFrame(m_data.m_enFrameType, &m_data.m_nDataBuffer[0], m_data.m_nFrameLen);
    }
    else
    {
        // 获取失败，则打开定时器直到获取一次有效数据后才停止
        m_nRealTimer = SetTimer(0x01, 200, NULL);
    }
}


void CSMVMsgMonitorDlg::_updateFrame( enumFrameType type, const uint8_t* pBuff, int bufflen )
{
    // 根据数据帧类型转交给不同的解码接口
    switch(type)
    {
    case FRAME_TYPE_NET_9_1:
        if(!m_pMsg91)
        {
            m_pMsg91 = CreateSav91MsgImpl();
        }

        DSM_Decode91(m_treeFrame, m_pMsg91, type, pBuff, bufflen);
        break;

    case FRAME_TYPE_NET_9_2:
        if(!m_pMsg92)
        {
            m_pMsg92 = CreateSav92MsgImpl();
        }

        DSM_Decode92(m_treeFrame, m_pMsg92, type, pBuff, bufflen);
        break;

    case FRAME_TYPE_FT3_NANRUI:
        if(!m_pMsgNanRui)
        {
            m_pMsgNanRui = CreateNanRuiFT3MsgImpl();
        }

        DSM_DecodeNanruiFT3(m_treeFrame, m_pMsgNanRui, type,pBuff, bufflen);
        break;

    case FRAME_TYPE_FT3_GUOWANG:
        if(!m_pMsgGuoWang)
        {
            m_pMsgGuoWang = CreateGuoWangFT3MsgImpl();
        }

        DSM_DecodeGuowangFT3(m_treeFrame, m_pMsgGuoWang, type, pBuff, bufflen);
        break;

    default:
        break;
    }

    if(m_treeFrame.GetCount() > 0)
    {
        LBT_HTREEITEM item = m_treeFrame.GetTreeFirstVisibleItem();

        if(item)
        {
            m_treeFrame.SelectTreeItem(item);
        }
    }
}


void CSMVMsgMonitorDlg::_createTree()
{
    ASSERT(!m_treeFrame.GetSafeHwnd());

    CRect rcTree;
    GetClientRect(rcTree);
    rcTree.DeflateRect(DSM_MARGIN_IN_RECT);

    m_nTreeRows     = (rcTree.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcTree.bottom   = rcTree.top + ELT_T1_HDRHEIGHT + ELT_T1_ROWHEIGHT * m_nTreeRows;

    m_treeFrame.Create(WS_CHILD|WS_VISIBLE|WS_TABSTOP, rcTree, this, IDC_SMV_MSGMONITOR_TREE);

    // 设置树样式
    m_treeFrame.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_treeFrame.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_treeFrame.PostInitTemplateStyle();

    m_treeFrame.InsertTreeColumn(0, dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_ITEM), EL_LEFT, 280);
    m_treeFrame.InsertTreeColumn(1, dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_VALUE), EL_CENTER, 250);

    m_treeFrame.SetFocus();
}


DWORD WINAPI DSM_RECORD_SMV_TASK(WPARAM /* wParam */, LPARAM /* lParam */, void* arg)
{
    CDialog* pDlg = (CDialog*)arg;

    pDlg->EndDialog(IDOK);

    return 0;
}

bool CSMVMsgMonitorDlg::_recordSmv()
{
    bool bRet = false;

    // 录波参数设置
    switch(m_pCurrentCtrlBlk->enFrameType)
    {
    case FRAME_TYPE_NET_9_1:
    case FRAME_TYPE_NET_9_2:
        bRet = _recordSmv9x();
        break;

    case FRAME_TYPE_FT3_GUOWANG:
    case FRAME_TYPE_FT3_NANRUI:
        bRet = _recordSmvft3();
        break;
    }

    if(bRet)
    {
        bRet = _handleRecord();
        m_pDataLayer->RemoveRecordItem(m_pCurrentCtrlBlk);
    }

    return bRet;
}


LRESULT CSMVMsgMonitorDlg::OnHandleRecordException( WPARAM wParam, LPARAM /* lParam */ )
{
    if(!m_pMsgDlg)
    {
        return 0;
    }

    int nCounts = 20;       // 累计10次不能自动关闭对话框，则转为手动停止关闭

    while(nCounts-- > 0)
    {
        if(m_pMsgDlg->GetSafeHwnd())
        {
            m_pMsgDlg->EndDialog(wParam + 100);
            return 1;
        }

        Sleep(500);
    }

    return 0;
}

void CSMVMsgMonitorDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    // 第一次获取有效的实时报文
    if(m_nRealTimer == 0x01)
    {
        // 获取到有效的实时报文后关闭定时器
        if(m_pDataLayer->GetRealTimeMsg(&m_data))
        {
            KillTimer(m_nRealTimer);
            m_nRealTimer = 0;

            ASSERT(m_data.m_enFrameType == m_pCurrentCtrlBlk->enFrameType);
            _updateFrame(m_data.m_enFrameType, &m_data.m_nDataBuffer[0], m_data.m_nFrameLen);
        }
    }
    // 检查录波的状态，SD卡、网络
    else if(m_nRecordTimer == 0x02)
    {
        // SD卡被拔出
        if(CStatusMgr::GetSD() == DSM_STATU_DISCONNECT_SD)
        {
            if(m_pMsgDlg->GetSafeHwnd())
            {
                m_pMsgDlg->EndDialog(DSM_SMV_RECORD_ERR_SD_INJECTED);
            }
        }

        // 线路未连接
//         if(!m_pDataLayer->IsPortActive(m_pCurrentCtrlBlk->enPortName) && !m_pDataLayer->IsPortLinked(m_pCurrentCtrlBlk->enPortName))
//         {
//             if(m_pMsgDlg->GetSafeHwnd())
//             {
//                 m_pMsgDlg->EndDialog(DSM_SMV_RECORD_ERR_LINK);
//             }
//         }
   }

    CSMVBaseDlg::OnTimer(nIDEvent);
}

BOOL CSMVMsgMonitorDlg::DestroyWindow()
{
    if(m_nRealTimer == 0x01)
    {
        KillTimer(m_nRealTimer);
        m_nRealTimer = 0;
    }

    if(m_nRecordTimer == 0x02)
    {
        KillTimer(m_nRecordTimer);
        m_nRecordTimer = 0;
    }

    return DSM_SMVBASEDLG::DestroyWindow();
}

void CSMVMsgMonitorDlg::_refreshFrame()
{
    ASSERT(m_pDataLayer);

    // 获取新的一帧数据
    if(m_pDataLayer->GetRealTimeMsg(&m_data))
    {
        if(m_nRealTimer == 0x01)
        {
            KillTimer(m_nRealTimer);
            m_nRealTimer = 0;
        }

        int nSel    = m_treeFrame.GetCurSel();
        int nVisual = m_treeFrame.GetTopIndex();

        _updateFrame(m_data.m_enFrameType, &m_data.m_nDataBuffer[0], m_data.m_nFrameLen);

        if(nVisual <= m_treeFrame.GetCount())
        {
            m_treeFrame.SetTopIndex(nVisual);
        }

        if(nSel <= m_treeFrame.GetCount())
        {
            m_treeFrame.SetCurSel(nSel);
        }
    }
    else
    {
        // 刷新空数据，或许可以使用别的方式提示刷新后数据没更新
        m_treeFrame.DeleteAllTreeItems();
    }
}

bool CSMVMsgMonitorDlg::_checkforRecordSMV()
{
    CString csSDPath;

    // 检查电量是否充足
    if(CStatusMgr::GetBattery() <= DSM_BATTERY_LOW_WARNNING && !CStatusMgr::GetCharge())
    {
        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_RECORD_BATTERY_LOW));
        return false;
    }

    // 判断SD卡是否接入
    if(!DSM_GetSDDriver(csSDPath))
    {
        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_RECORD_SD_NOT_FOUND));
        return false;
    }

#if !CHUN_HUA
	SYSTEMTIME st;
	GetLocalTime(&st);
	m_csPcapPath.Format(L"%s%04d%02d%02d_%02d%02d%02d\\", L"SV_", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

    // 判断路径是否存在，不存在则创建
    if(!DSM_CreateSDPath(csSDPath + DSM_PCAP_SMV_FILE_FOLDER+m_csPcapPath))
    {
        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_RECORD_PATH_ERROR));
        return false;
    }
#else
	// 判断路径是否存在，不存在则创建
	if(!DSM_CreateSDPath(csSDPath + DSM_PCAP_SMV_FILE_FOLDER))
	{
		CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_RECORD_PATH_ERROR));
		return false;
	}
#endif

    // 获取SD卡剩余空间
    ULARGE_INTEGER nFree, nTotal;
    if(!GetDiskFreeSpaceEx(csSDPath, &nFree, &nTotal, NULL))
    {
        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_GET_SD_FREESPACE_FAILED));
        return false;
    }

    // 判断SD卡剩余空间是否足够
    if(nFree.QuadPart < m_pDataLayer->GetRecordWaveSize())
    {
        CString csText(dsm_stringMgr::Get(DSM_STR_INFO_GET_SD_FREESPACE_LACK));
        csText.Format(csText, m_pDataLayer->GetRecordWaveSize() / 1024 / 1024);

        CStatusMgr::SetTips(csText);
        return false;
    }

#if !CHUN_HUA

	m_pDataLayer->SetSDCardSize(nFree.QuadPart);
#endif
//     // 检查线路是否正常
//     if(!m_pDataLayer->IsPortActive(m_pCurrentCtrlBlk->enPortName) && !m_pDataLayer->IsPortLinked(m_pCurrentCtrlBlk->enPortName))
//     {
//         CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_RECORD_LINK_ERROR));
//         return false;
//     }

    return true;
}

bool CSMVMsgMonitorDlg::_recordSmv9x()
{
    ASSERT(m_pCurrentCtrlBlk->enFrameType == FRAME_TYPE_NET_9_1 || m_pCurrentCtrlBlk->enFrameType == FRAME_TYPE_NET_9_2);

    _genRecordFile();

    // 设置录波控制块
    m_pDataLayer->AddRecordItem(m_pCurrentCtrlBlk);
    m_pDataLayer->SetRecordWavePath((LPCTSTR)m_csPcapFiles);
    m_pDataLayer->SetNotifyWnd(this);

    if(!m_pDataLayer->StartRecordNetSmv())
    {
        ASSERT(FALSE);
        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_RECORD_DATA_ERROR));
        return false;
    }

    return true;
}

void CSMVMsgMonitorDlg::_genRecordFile()
{
    CString csPre, csSDPath;       // SD卡目录

    if(!DSM_GetSDDriver(csSDPath))
    {
        ASSERT(FALSE);
    }

    switch(m_pCurrentCtrlBlk->enFrameType)
    {
#if SHENG_ZE
    case FRAME_TYPE_NET_9_1:
        csPre = L"sv91_";
        break;

    case FRAME_TYPE_NET_9_2:
        csPre = L"sv92_";
        break;

    case FRAME_TYPE_FT3_GUOWANG:
        csPre = L"guowang_";
        break;

    case FRAME_TYPE_FT3_NANRUI:
        csPre = L"nanrui_";
        break;

    default:
        ASSERT(FALSE);
        csPre = L"unknown_";
        break;
#else
	case FRAME_TYPE_NET_9_1:
		csPre = L"SV91_";
		break;

	case FRAME_TYPE_NET_9_2:
		csPre = L"SV92_";
		break;

	case FRAME_TYPE_FT3_GUOWANG:
		csPre = L"GUOWANG_";
		break;

	case FRAME_TYPE_FT3_NANRUI:
		csPre = L"NANRUI_";
		break;

	default:
		ASSERT(FALSE);
		csPre = L"UNKNOW_";
		break;
#endif

    }

    SYSTEMTIME st;
    GetLocalTime(&st);
#if SHENG_ZE
    m_csPcapFiles.Format(L"%s%s%s%04d%02d%02d_%02d%02d%02d_%03d.pcap", csSDPath, DSM_PCAP_SMV_FILE_FOLDER, csPre, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
#else

#if !CHUN_HUA
	m_csPcapFiles.Format(L"%s%s%s%s", csSDPath, DSM_PCAP_SMV_FILE_FOLDER, m_csPcapPath,csPre);
	//m_csPcapFiles.Format(L"%s%s%s%04d%02d%02d_%02d%02d%02d\\%s", csSDPath, DSM_PCAP_SMV_FILE_FOLDER,csPre,st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, csPre);

#else
	m_csPcapFiles.Format(L"%s%s%s%04d%02d%02d_%02d%02d%02d.pcap", csSDPath, DSM_PCAP_SMV_FILE_FOLDER, csPre, st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

#endif

#endif

}

bool CSMVMsgMonitorDlg::_recordSmvft3()
{
    ASSERT(m_pCurrentCtrlBlk->enFrameType == FRAME_TYPE_FT3_GUOWANG || m_pCurrentCtrlBlk->enFrameType == FRAME_TYPE_FT3_NANRUI);

    _genRecordFile();

    // 设置录波控制块
    m_pDataLayer->AddRecordItem(m_pCurrentCtrlBlk);
    m_pDataLayer->SetRecordWavePath((LPCTSTR)m_csPcapFiles);
    m_pDataLayer->SetNotifyWnd(this);

    if(!m_pDataLayer->StartRecordFt3())
    {
        ASSERT(FALSE);
        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_RECORD_DATA_ERROR));
        return false;
    }

    return true;
}

INT_PTR CSMVMsgMonitorDlg::_showRecordDlg()
{
    // 初始化录波状态框
    m_pMsgDlg = new CMsgDlg(DSM_STR_MSG_RECORD_TIPS, 0, this);

    std::vector<HBITMAP> v;
	v.push_back(dsm_bmpmgr::Get(DSM_BMP_RECORDING_1));
	v.push_back(dsm_bmpmgr::Get(DSM_BMP_RECORDING_2));
	v.push_back(dsm_bmpmgr::Get(DSM_BMP_RECORDING_3));
	v.push_back(dsm_bmpmgr::Get(DSM_BMP_RECORDING_4));
	v.push_back(dsm_bmpmgr::Get(DSM_BMP_RECORDING_5));
	v.push_back(dsm_bmpmgr::Get(DSM_BMP_RECORDING_6));
    m_pMsgDlg->_SetBitmap(v);
	/*m_pMsgDlg->_SetPicRC(DSM_SMV_MSGMONITOR_RCRECORD_TIP_ICO);*/
	m_pMsgDlg->_SetTextRC(DSM_SMV_MSGMONITOR_RCRECORD_TIP_TEXT);

    m_pMsgDlg->_SetMenu(IDR_MENU_FRAME_RECORD);
    m_pMsgDlg->_SetCommandRange(ID_MENU_FRAME_RECORD_F1, ID_MENU_FRAME_RECORD_F1, DSM_RECORD_SMV_TASK, (void*)m_pMsgDlg);
#if !SHENG_ZE
	m_pMsgDlg->_ResetF1Text(_T("停止录波"));
#endif
	m_pMsgDlg->_GetCtl()->_SetTimeInterval(80);
    m_pMsgDlg->_EnableCountDown();
    CCountDownWnd* pCountdown = m_pMsgDlg->_GetCountDown();
    pCountdown->_SetBKColor(DSM_BASEPOP_CRBK);

    int nTotalSize  = 20 * 1024 * 1024 - sizeof(PCAP_FILE_HEADER);
    int nPieceSize  = m_pCurrentCtrlBlk->nSampleRate * (m_pCurrentCtrlBlk->nFrameLen + sizeof(PCAP_PACKET_HEADER)) / m_pCurrentCtrlBlk->nAsdu;
    int nSeconds     = (int)(nTotalSize / nPieceSize);
    int nMin        = nSeconds / 60;
    int nSecond     = nSeconds % 60;

    CString csTips;
    if(nMin > 0)
    {
        csTips.Format(dsm_stringMgr::Get(DSM_STR_INFO_RECORD_TOTAL), nMin, nSecond);
    }
    else
    {
        csTips.Format(dsm_stringMgr::Get(DSM_STR_INFO_RECORD_TOTAL1), nSecond);
    }
    
    csTips += dsm_stringMgr::Get(nMin > 0 ? DSM_STR_INFO_RECORD_LEFT : DSM_STR_INFO_RECORD_LEFT1);
    pCountdown->_SetSeconds(nSeconds);
    pCountdown->_SetTipsFmt(csTips);

    m_nRecordTimer = SetTimer(0x02, 500, NULL);

    // 显示录波状态
    INT_PTR nRet = m_pMsgDlg->DoModal();

    if(m_nRecordTimer == 0x02)
    {
        KillTimer(m_nRecordTimer);
        m_nRecordTimer = 0;
    }

    // 释放录波状态框资源
    m_pMsgDlg->_ResetMenu();
    delete m_pMsgDlg;
    m_pMsgDlg = NULL;

    return nRet;
}

bool CSMVMsgMonitorDlg::_handleRecord()
{
    // 录波状态框设置
    INT_PTR nRet = _showRecordDlg();

    // 保存录波文件的状态值
    uint32_t nSaveRet = 0;

    // SD卡被拔出
    if(nRet == DSM_SMV_RECORD_ERR_SD_INJECTED && m_pDataLayer->CancelRecordWave())
    {
        RETAILMSG(1, (L"dsm: cancel record smv, sd card injected.\n"));
        DM_ShowReordErrorInfo(nRet);

        return false;
    }

    // 停止录波
    if(!m_pDataLayer->StopRecordWave(nSaveRet))
    {
        RETAILMSG(1, (L"dsm: stop record smv error\n"));
        DM_ShowReordErrorInfo(nRet > 200 ? nRet : nSaveRet);
    }
    else
    {
        // 判断是否录波大小超过限定值(20M)
        if(nRet - 100 == MSG_RECORD_SIZE_REACHED)
        {
            CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_RECORD_SIZE_FULL));
        }
    }

    return true;
}
