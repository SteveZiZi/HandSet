/**
 * \file
 *      GooseMonitorDlg.cpp
 *
 * \brief
 *      goose报文监视页面实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/5/10
 */

// GooseMonitorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GooseMonitorDlg.h"

#include "src/main/resourcebmp.h"
#include "src/main/utils/decode.h"
#include "src/main/utils/utility.h"
#include "src/main/mgr/statusmgr.h"
#include "src/main/popdlg/msgdlg.h"
#include "src/main/dlg/dsmfiledlg.h"
#include "src/main/smv/common_smv.h"
#include "src/main/dlg/pcapbrowserdlg.h"
#include "src/utils/utility/dsm_utility.h"



// CGooseMonitorDlg 对话框

IMPLEMENT_DYNAMIC(CGooseMonitorDlg, DSM_GOOSEASEDLG)

CGooseMonitorDlg::CGooseMonitorDlg(CWnd* pParent /*=NULL*/)
	: DSM_GOOSEASEDLG(CGooseMonitorDlg::IDD, DSM_STR_GOOSE_MONITOR_CAPTION, pParent)
    , m_pGooseMsg(NULL)
    , m_csPcapFiles(L"")
    , m_nMaxRows(0)
    , m_nRealTimer(0)
    , m_nDetectTimer(0)
    , m_pMsgDlg(NULL)
	, m_csRecordPath(L"")
{

}

CGooseMonitorDlg::~CGooseMonitorDlg()
{
    ReleaseGooseMsgImpl(m_pGooseMsg);
}

void CGooseMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	DSM_GOOSEASEDLG::DoDataExchange(pDX);
}

#ifdef ZY_PRJ
BEGIN_MESSAGE_MAP(CGooseMonitorDlg, DSM_GOOSEASEDLG)
	ON_WM_TIMER()
	ON_COMMAND(ID_MENU_GOOSE_F3, &CGooseMonitorDlg::OnMenuF3)
	ON_COMMAND(ID_MENU_GOOSE_F5, &CGooseMonitorDlg::OnMenuF5)
	ON_COMMAND(ID_MENU_GOOSE_F6, &CGooseMonitorDlg::OnMenuF6)
	ON_MESSAGE(WM_DATA_LAYER_MESSAGE, &CGooseMonitorDlg::OnHandleRecordException)
END_MESSAGE_MAP()
#else
BEGIN_MESSAGE_MAP(CGooseMonitorDlg, DSM_GOOSEASEDLG)
	ON_WM_TIMER()
	//ON_COMMAND(ID_MENU_GOOSE_F3, &CGooseMonitorDlg::OnMenuF3)
	ON_COMMAND(ID_MENU_GOOSE_F3, &CGooseMonitorDlg::OnMenuF3)
	ON_COMMAND(ID_MENU_GOOSE_F4, &CGooseMonitorDlg::OnMenuF4)
	ON_MESSAGE(WM_DATA_LAYER_MESSAGE, &CGooseMonitorDlg::OnHandleRecordException)
END_MESSAGE_MAP()
#endif



// CGooseMonitorDlg 消息处理程序

BOOL CGooseMonitorDlg::OnInitDialog()
{
    CGooseBaseDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    if(!m_pCurrentCtrlBlk && m_pDataLayer->GetSelectedCtrlBlkCount() > 0)
    {
        m_pDataLayer->SetCurrentCtrlBlk(m_pDataLayer->GetFirstSelectCtrlBlk());
        m_pCurrentCtrlBlk = m_pDataLayer->GetCurrentCtrlBlk();
    }

    if(m_pCurrentCtrlBlk)
    {
        m_pDataLayer->SetCurrentMode(DSM_MODE_GOOSE_RECV_REAL_TIME_MSG);
    }

    // 显示实时报文控件
    _showFrame();

    return FALSE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

//void CGooseMonitorDlg::OnMenuF3()
//{
//    ASSERT(m_pDataLayer);
//
//    // 实时刷新数据，数据层可能缓存有上一次的数据
//    if(m_pDataLayer->GetRealTimeMsg(&m_data))
//    {
//        // 获得第一次实时数据后关闭定时器
//        if(m_nRealTimer == 0x01)
//        {
//            KillTimer(m_nRealTimer);
//            m_nRealTimer = 0;
//        }
//
//        int nSel    = m_treeFrame.GetCurSel();
//        int nVisual = m_treeFrame.GetTopIndex();
//
//        // 更新数据信息
//        _updateFrame(m_data.m_enFrameType, &m_data.m_nDataBuffer[0], m_data.m_nFrameLen);
//
//        // 恢复焦点行，若之前有节点被缩进，则可能焦点位置会有偏差
//        if(nVisual <= m_treeFrame.GetCount())
//        {
//            m_treeFrame.SetTopIndex(nVisual);
//        }
//
//        if(nSel <= m_treeFrame.GetCount())
//        {
//            m_treeFrame.SetCurSel(nSel);
//        }
//    }
//    else
//    {
//        // 刷新空数据，或许可以使用别的方式提示刷新后数据没更新
//        m_treeFrame.DeleteAllTreeItems();
//    }
//}


void CGooseMonitorDlg::OnMenuF3()
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

    CString csPcapFolder = csSDPath + DSM_PCAP_GOOSE_FILE_FOLDER;     // pcap目录

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




void CGooseMonitorDlg::OnMenuF4()
{
    // 录波前，检查SD卡目录、数据连接是否正常
    if(!_checkforRecordGoose())
    {
        return;
    }

    // 保存现场
    CString csTitle = CStatusMgr::GetTitle();
    LBT_HTREEITEM item = m_treeFrame.GetTreeSelectedItem();

    // goose录波
    bool bRet = _recordGoose();
    m_pDataLayer->SetCurrentMode(DSM_MODE_GOOSE_RECV_REAL_TIME_MSG);

    INT_PTR nRet = IDOK;

#if !CHUN_HUA

	CStatusMgr::SetTips(L"");

	// SD卡目录
	CString csSDPath;

	// 判断SD卡是否存在
	if(!DSM_GetSDDriver(csSDPath))
	{
		CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_SD_NOT_FOUND_PCAP));
		return;
	}

	CString csPcapFolder = csSDPath + DSM_PCAP_GOOSE_FILE_FOLDER+m_csRecordPath;     // pcap目录

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

	 
	HWND hWnd       = ::GetFocus();

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
		// 恢复现场
		CStatusMgr::SetTitle(csTitle);
		m_treeFrame.SetFocus();
		m_treeFrame.SelectTreeItem(item);
	}
	else
	{
		// 使用home菜单返回主页面时，设置相应焦点
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
        // 恢复现场
        CStatusMgr::SetTitle(csTitle);
        m_treeFrame.SetFocus();
        m_treeFrame.SelectTreeItem(item);
    }
    else
    {
        // 使用home菜单返回主页面时，设置相应焦点
        DSM_SetZFocus(CEventProc::GetInstance()->GetCurrentPageHwnd());
    }
#endif
}


/**
 *      修改菜单HMENU
 */
void CGooseMonitorDlg::_modifyMenu()
{
    __super::_modifyMenu();

#ifdef ZY_PRJ
	// 设置F6的样式 『开始记录』
    m_menuMain.SetMenuSubItem(5, NULL);
    m_menuMain.SetMenuItemId(5, ID_MENU_GOOSE_F6);

    // 实时信息、加载pcap文件、录波
    m_menuMain.SetMenuItemCaption(2, dsm_stringMgr::Get(DSM_STR_GOOSE_MONITOR_REFRESH));
    m_menuMain.SetMenuItemCaption(4, dsm_stringMgr::Get(DSM_STR_GOOSE_MONITOR_LOADFILE));
    m_menuMain.SetMenuItemCaption(5, dsm_stringMgr::Get(DSM_STR_GOOSE_MONITOR_BEGIN_REC));
#else
	// 设置F6的样式 『开始记录』
	m_menuMain.SetMenuSubItem(5, NULL);
	m_menuMain.SetMenuItemId(2, ID_MENU_GOOSE_F3);
	m_menuMain.SetMenuItemId(3, ID_MENU_GOOSE_F4);
	
	// 实时信息、加载pcap文件、录波
	m_menuMain.SetMenuItemCaption(2, L"浏览文件");
	//m_menuMain.SetMenuItemCaption(4, dsm_stringMgr::Get(DSM_STR_GOOSE_MONITOR_LOADFILE));
	m_menuMain.SetMenuItemCaption(3, L"录波");
#endif
    
}

bool CGooseMonitorDlg::_switchCtrlBlk( UINT nCmdID )
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

/**
 *      创建树控件
 */
void CGooseMonitorDlg::_createTree()
{
    ASSERT(!m_treeFrame.GetSafeHwnd());

    CRect rcTree;
    GetClientRect(rcTree);
    rcTree.DeflateRect(DSM_MARGIN_IN_RECT);

    int nRow = (rcTree.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcTree.bottom = rcTree.top + ELT_T1_HDRHEIGHT + ELT_T1_ROWHEIGHT * nRow;

    m_treeFrame.Create(WS_CHILD|WS_VISIBLE|WS_TABSTOP, rcTree, this, IDC_GOOSE_MONITOR_TREE);

    // 设置树样式
    m_treeFrame.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_treeFrame.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_treeFrame.PostInitTemplateStyle();

    m_treeFrame.InsertTreeColumn(0, dsm_stringMgr::Get(DSM_STR_GOOSE_MONITOR_ITEM), EL_LEFT, 280);
    m_treeFrame.InsertTreeColumn(1, dsm_stringMgr::Get(DSM_STR_GOOSE_MONITOR_VALUE), EL_CENTER, 250);

    m_treeFrame.SetFocus();
}


/**
 *      显示goose帧数据
 */
void CGooseMonitorDlg::_showFrame()
{
    if(!::IsWindow(m_treeFrame.GetSafeHwnd()))
    {
        _createTree();
    }

    m_treeFrame.ShowWindow(SW_SHOW);
    m_treeFrame.SetFocus();

    // 获取新的一帧数据
    if(m_pDataLayer->GetRealTimeMsg(&m_data))
    {
        _updateFrame(m_data.m_enFrameType, &m_data.m_nDataBuffer[0], m_data.m_nFrameLen);
    }
    else
    {
        // 获取失败，则打开定时器直到获取一次有效数据后才停止
        m_nRealTimer = SetTimer(0x01, 200, NULL);
    }
}


/**
 *      更新帧数据
 *
 *  \param[in]
 *      type            帧类型
 *  \param[in]
 *      pBuff           帧数据缓冲区
 *  \param[in]
 *      bufflen         帧数据缓冲区长度
 */
void CGooseMonitorDlg::_updateFrame(enumFrameType type, const uint8_t* pBuff, int bufflen)
{
    // 解码goose协议数据
    switch(type)
    {
    case FRAME_TYPE_NET_GOOSE:
        if(!m_pGooseMsg)
        {
            m_pGooseMsg = CreateGooseMsgImpl();
        }

        DSM_DecodeGoose(m_treeFrame, m_pGooseMsg, type, pBuff, bufflen);
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


DWORD WINAPI DSM_RECORD_GOOSE_TASK(WPARAM /* wParam */, LPARAM /* lParam */, void* arg)
{
    CDialog* pDlg = (CDialog*)arg;

    pDlg->EndDialog(IDOK);

    return 0;
}

/**
 *      goose录波
 */
bool CGooseMonitorDlg::_recordGoose()
{
    // 录波参数设置
    bool bRet = _recordGooseEx();;

    if(bRet)
    {
        bRet = _handleRecord();
        m_pDataLayer->RemoveRecordItem(m_pCurrentCtrlBlk);
    }

    return bRet;
}

void CGooseMonitorDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    // 自动获取一次实时报文
    if(m_nRealTimer == 0x01)
    {
        if(m_pDataLayer->GetRealTimeMsg(&m_data))
        {
            KillTimer(m_nRealTimer);
            m_nRealTimer = 0;

            ASSERT(m_data.m_enFrameType == m_pCurrentCtrlBlk->enFrameType);
            _updateFrame(m_data.m_enFrameType, &m_data.m_nDataBuffer[0], m_data.m_nFrameLen);
        }
    }
    // 定时轮询状态
    else if(m_nDetectTimer == 0x02)
    {
        // 检查录波的状态，SD卡、网络
        if(CStatusMgr::GetSD() == DSM_STATU_DISCONNECT_SD)
        {
            if(m_pMsgDlg->GetSafeHwnd())
            {
                m_pMsgDlg->EndDialog(DSM_SMV_RECORD_ERR_SD_INJECTED);
            }
        }

//             // 线路未连接
//             if(!m_pDataLayer->IsPortActive(m_pCurrentCtrlBlk->enPortName) && !m_pDataLayer->IsPortLinked(m_pCurrentCtrlBlk->enPortName))
//             {
//                 if(m_pMsgDlg->GetSafeHwnd())
//                 {
//                     m_pMsgDlg->EndDialog(DSM_SMV_RECORD_ERR_LINK);
//                 }
//             }

    }

    CGooseBaseDlg::OnTimer(nIDEvent);
}

LRESULT CGooseMonitorDlg::OnHandleRecordException( WPARAM wParam, LPARAM /* lParam */ )
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

BOOL CGooseMonitorDlg::DestroyWindow()
{
    if(m_nRealTimer == 0x01)
    {
        KillTimer(m_nRealTimer);
        m_nRealTimer = 0;
    }

    if(m_nDetectTimer == 0x02)
    {
        KillTimer(m_nDetectTimer);
        m_nDetectTimer = 0;
    }

    return DSM_GOOSEASEDLG::DestroyWindow();
}

bool CGooseMonitorDlg::_checkforRecordGoose()
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
	m_csRecordPath.Format(L"%s%04d%02d%02d_%02d%02d%02d\\", L"GOOSE_", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	// 判断路径是否存在，不存在则创建
	if(!DSM_CreateSDPath(csSDPath + DSM_PCAP_GOOSE_FILE_FOLDER+m_csRecordPath))
	{
		CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_RECORD_PATH_ERROR));
		return false;
	}
#else
    // 判断路径是否存在，不存在则创建
    if(!DSM_CreateSDPath(csSDPath + DSM_PCAP_GOOSE_FILE_FOLDER))
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

    //         // 检查线路是否正常
    //         if(!m_pDataLayer->IsPortActive(m_pCurrentCtrlBlk->enPortName) && 
    //              !m_pDataLayer->IsPortLinked(m_pCurrentCtrlBlk->enPortName))
    //         {
    //             CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_RECORD_LINK_ERROR));
    //             return;
    //         }
#if !CHUN_HUA

	m_pDataLayer->SetSDCardSize(nFree.QuadPart);
#endif

    return true;

}

bool CGooseMonitorDlg::_recordGooseEx()
{
    _genRecordFileName();

    // 设置录波控制块
    m_pDataLayer->AddRecordItem(m_pCurrentCtrlBlk);
    m_pDataLayer->SetRecordWavePath((LPCTSTR)m_csPcapFiles);
    m_pDataLayer->SetNotifyWnd(this);

    if(!m_pDataLayer->StartRecordGoose())
    {
        ASSERT(FALSE);
        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_RECORD_DATA_ERROR));
        return false;
    }

    return true;
}

void CGooseMonitorDlg::_genRecordFileName()
{
    CString csSDPath;       // SD卡目录

    if(!DSM_GetSDDriver(csSDPath))
    {
        ASSERT(FALSE);
    }

    SYSTEMTIME st;
    GetLocalTime(&st);
#if SHENG_ZE
    m_csPcapFiles.Format(L"%s%s%s%04d%02d%02d_%02d%02d%02d_%03d.pcap", csSDPath, DSM_PCAP_GOOSE_FILE_FOLDER, L"gs", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
#else

#if !CHUN_HUA
	m_csPcapFiles.Format(L"%s%s%s%s", csSDPath, DSM_PCAP_GOOSE_FILE_FOLDER,m_csRecordPath, L"GOOSE");

#else
	m_csPcapFiles.Format(L"%s%s%s_%04d%02d%02d_%02d%02d%02d.pcap", csSDPath, DSM_PCAP_GOOSE_FILE_FOLDER, L"GOOSE", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
#endif

#endif
}

bool CGooseMonitorDlg::_handleRecord()
{
    // 录波状态框设置
    INT_PTR nRet = _showRecordDlg();

    // 保存录波文件的状态值
    uint32_t nSaveRet = 0;

    // SD卡被拔出
    if(nRet == 201 && m_pDataLayer->CancelRecordWave())
    {
        RETAILMSG(1, (L"dsm: cancel record goose, sd card injected.\n"));
        DM_ShowReordErrorInfo(nRet);

        return false;
    }

    // 停止录波
    if(!m_pDataLayer->StopRecordWave(nSaveRet))
    {
        RETAILMSG(1, (L"dsm: stop record goose error\n"));
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

INT_PTR CGooseMonitorDlg::_showRecordDlg()
{
    // 显示录波状态
    m_pMsgDlg = new CMsgDlg(DSM_STR_MSG_RECORD_TIPS, 0, this);

    std::vector<HBITMAP> v;
    v.push_back(dsm_bmpmgr::Get(DSM_BMP_RECORDING_1));
    v.push_back(dsm_bmpmgr::Get(DSM_BMP_RECORDING_2));
	v.push_back(dsm_bmpmgr::Get(DSM_BMP_RECORDING_3));
	v.push_back(dsm_bmpmgr::Get(DSM_BMP_RECORDING_4));
	v.push_back(dsm_bmpmgr::Get(DSM_BMP_RECORDING_5));
	v.push_back(dsm_bmpmgr::Get(DSM_BMP_RECORDING_6));
    m_pMsgDlg->_SetBitmap(v);
//    m_pMsgDlg->_SetPicRC(DSM_SMV_MSGMONITOR_RCRECORD_TIP_ICO);
//	m_pMsgDlg->_SetTextRC(DSM_SMV_MSGMONITOR_RCRECORD_TIP_TEXT);
	m_pMsgDlg->_SetTextRC(DSM_SMV_MSGMONITOR_RCRECORD_TIP_TEXT);
    m_pMsgDlg->_SetMenu(IDR_MENU_FRAME_RECORD);

#ifdef ZY_PRJ
    m_pMsgDlg->_SetCommandRange(ID_MENU_FRAME_RECORD_F6, ID_MENU_FRAME_RECORD_F6, DSM_RECORD_GOOSE_TASK, (void*)m_pMsgDlg);
#else
	m_pMsgDlg->_SetCommandRange(ID_MENU_FRAME_RECORD_F1, ID_MENU_FRAME_RECORD_F1, DSM_RECORD_GOOSE_TASK, (void*)m_pMsgDlg);
#endif
    m_pMsgDlg->_GetCtl()->_SetTimeInterval(80);
#if !SHENG_ZE
	m_pMsgDlg->_ResetF1Text(_T("停止录波"));
#endif
    m_nDetectTimer = SetTimer(0x02, 500, NULL);

    // 显示录波状态
    INT_PTR nRet = m_pMsgDlg->DoModal();

    if(m_nDetectTimer == 0x02)
    {
        KillTimer(m_nDetectTimer);
        m_nDetectTimer = 0;
    }

    // 释放录波状态框资源
    m_pMsgDlg->_ResetMenu();
    delete m_pMsgDlg;
    m_pMsgDlg = NULL;

    return nRet;
}

