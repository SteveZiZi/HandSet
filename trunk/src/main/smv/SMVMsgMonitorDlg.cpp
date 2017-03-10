/**
 * \file
 *      SMVMsgMonitorDlg.cpp
 *
 * \brief
 *      ���ļ�⼰¼��ҳ��ʵ���ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/5/11
 */

// SMVMsgMonitorDlg.cpp : ʵ���ļ�
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


// CSMVMsgMonitorDlg �Ի���

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



// CSMVMsgMonitorDlg ��Ϣ�������

BOOL CSMVMsgMonitorDlg::OnInitDialog()
{
    CSMVBaseDlg::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��

    if(!m_pCurrentCtrlBlk && m_pDataLayer->GetSelectedCtrlBlkCount() > 0)
    {
        m_pDataLayer->SetCurrentCtrlBlk(m_pDataLayer->GetFirstSelectCtrlBlk());
        m_pCurrentCtrlBlk = m_pDataLayer->GetCurrentCtrlBlk();
    }

    if(m_pCurrentCtrlBlk)
    {
        // ���ؿ��ƿ��б�
        _loadCtrlBlk();
        m_pDataLayer->SetCurrentCtrlBlk(m_pCurrentCtrlBlk);
        _updateCaption();

        m_pDataLayer->SetCurrentMode(DSM_MODE_SMV_RECV_REAL_TIME_MSG);
    }

    // ��ʾʵʱ���Ŀؼ�
    _showFrame();

    return FALSE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
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
	m_menuMain.SetMenuItemCaption(3, L"����ļ�");
	//m_menuMain.HighlightMenuItem(3, TRUE);
#endif
   // m_menuMain.SetMenuItemCaption(4, dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_LOADFILE));
#if SU_FEI
    m_menuMain.SetMenuItemCaption(5, dsm_stringMgr::Get(DSM_STR_SMV_MSGMONITOR_BEGIN_REC));
#else
	m_menuMain.SetMenuItemCaption(4, L"¼��");
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

        // �򿪶�ʱ��ֱ����ȡһ����Ч���ݺ��ֹͣ
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

	// smv¼��
	bool bRet = _recordSmv();
	m_pDataLayer->SetCurrentMode(DSM_MODE_SMV_RECV_REAL_TIME_MSG);

	INT_PTR nRet = IDOK;

#if !CHUN_HUA
	CStatusMgr::SetTips(L"");

	HWND hWnd       = ::GetFocus();
	// SD��Ŀ¼
	CString csSDPath;

	// �ж�SD���Ƿ����
	if(!DSM_GetSDDriver(csSDPath))
	{
		CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_SD_NOT_FOUND_PCAP));
		return;
	}

	CString csPcapFolder = csSDPath + DSM_PCAP_SMV_FILE_FOLDER+m_csPcapPath;     // pcapĿ¼

	// ��pcap�ļ���ʧ��
	if(!DSM_CreateSDPath(csPcapFolder))
	{
		CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_FOLDER_NOT_FOUND_PCAP));
		return;
	}

	// ����pcap�ļ�
	CString csExt;
	csExt.Format(L"*.%s|", DSM_PCAP_FILE_EXTENSION);
	CDsmFileDlg dlg(0, csPcapFolder, L"", csExt, 0, this, dsm_stringMgr::Get(DSM_STR_FILEBROWSER_TITLE));

	
	
	nRet    = dlg.DoModal();
	if(nRet == IDOK)
	{
		m_csPcapFiles = dlg._GetFilePath();

		// ����pcap��
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
		// ����pcap��
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

    // SD��Ŀ¼
    CString csSDPath;

    // �ж�SD���Ƿ����
    if(!DSM_GetSDDriver(csSDPath))
    {
        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_SD_NOT_FOUND_PCAP));
        return;
    }

    CString csPcapFolder = csSDPath + DSM_PCAP_SMV_FILE_FOLDER;     // pcapĿ¼

    // ��pcap�ļ���ʧ��
    if(!DSM_CreateSDPath(csPcapFolder))
    {
        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_FOLDER_NOT_FOUND_PCAP));
        return;
    }

    // ����pcap�ļ�
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

        // ����pcap��
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
    // ˢ��һ֡����
    _refreshFrame();
}

#if !SU_FEI

void CSMVMsgMonitorDlg::OnMenuF4()
{
	CStatusMgr::SetTips(L"");

	// SD��Ŀ¼
	CString csSDPath;

	// �ж�SD���Ƿ����
	if(!DSM_GetSDDriver(csSDPath))
	{
		CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_SD_NOT_FOUND_PCAP));
		return;
	}

	CString csPcapFolder = csSDPath + DSM_PCAP_SMV_FILE_FOLDER;     // pcapĿ¼

	// ��pcap�ļ���ʧ��
	if(!DSM_CreateSDPath(csPcapFolder))
	{
		CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_FOLDER_NOT_FOUND_PCAP));
		return;
	}

	// ����pcap�ļ�
	CString csExt;
	csExt.Format(L"*.%s|", DSM_PCAP_FILE_EXTENSION);
	CDsmFileDlg dlg(0, csPcapFolder, L"", csExt, 0, this, dsm_stringMgr::Get(DSM_STR_FILEBROWSER_TITLE));

	CString csTitle(CStatusMgr::GetTitle());

	HWND hWnd       = ::GetFocus();
	INT_PTR nRet    = dlg.DoModal();
	if(nRet == IDOK)
	{
		m_csPcapFiles = dlg._GetFilePath();

		// ����pcap��
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

    //// smv¼��
    //bool bRet = _recordSmv();
    //m_pDataLayer->SetCurrentMode(DSM_MODE_SMV_RECV_REAL_TIME_MSG);

    //INT_PTR nRet = IDOK;

    //if(bRet)
    //{
    //    // ����pcap��
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

    // ��ȡ�µ�һ֡����
    if(m_pDataLayer->GetRealTimeMsg(&m_data))
    {
        ASSERT(m_data.m_enFrameType == m_pCurrentCtrlBlk->enFrameType);
        _updateFrame(m_data.m_enFrameType, &m_data.m_nDataBuffer[0], m_data.m_nFrameLen);
    }
    else
    {
        // ��ȡʧ�ܣ���򿪶�ʱ��ֱ����ȡһ����Ч���ݺ��ֹͣ
        m_nRealTimer = SetTimer(0x01, 200, NULL);
    }
}


void CSMVMsgMonitorDlg::_updateFrame( enumFrameType type, const uint8_t* pBuff, int bufflen )
{
    // ��������֡����ת������ͬ�Ľ���ӿ�
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

    // ��������ʽ
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

    // ¼����������
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

    int nCounts = 20;       // �ۼ�10�β����Զ��رնԻ�����תΪ�ֶ�ֹͣ�ر�

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
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

    // ��һ�λ�ȡ��Ч��ʵʱ����
    if(m_nRealTimer == 0x01)
    {
        // ��ȡ����Ч��ʵʱ���ĺ�رն�ʱ��
        if(m_pDataLayer->GetRealTimeMsg(&m_data))
        {
            KillTimer(m_nRealTimer);
            m_nRealTimer = 0;

            ASSERT(m_data.m_enFrameType == m_pCurrentCtrlBlk->enFrameType);
            _updateFrame(m_data.m_enFrameType, &m_data.m_nDataBuffer[0], m_data.m_nFrameLen);
        }
    }
    // ���¼����״̬��SD��������
    else if(m_nRecordTimer == 0x02)
    {
        // SD�����γ�
        if(CStatusMgr::GetSD() == DSM_STATU_DISCONNECT_SD)
        {
            if(m_pMsgDlg->GetSafeHwnd())
            {
                m_pMsgDlg->EndDialog(DSM_SMV_RECORD_ERR_SD_INJECTED);
            }
        }

        // ��·δ����
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

    // ��ȡ�µ�һ֡����
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
        // ˢ�¿����ݣ��������ʹ�ñ�ķ�ʽ��ʾˢ�º�����û����
        m_treeFrame.DeleteAllTreeItems();
    }
}

bool CSMVMsgMonitorDlg::_checkforRecordSMV()
{
    CString csSDPath;

    // �������Ƿ����
    if(CStatusMgr::GetBattery() <= DSM_BATTERY_LOW_WARNNING && !CStatusMgr::GetCharge())
    {
        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_RECORD_BATTERY_LOW));
        return false;
    }

    // �ж�SD���Ƿ����
    if(!DSM_GetSDDriver(csSDPath))
    {
        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_RECORD_SD_NOT_FOUND));
        return false;
    }

#if !CHUN_HUA
	SYSTEMTIME st;
	GetLocalTime(&st);
	m_csPcapPath.Format(L"%s%04d%02d%02d_%02d%02d%02d\\", L"SV_", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

    // �ж�·���Ƿ���ڣ��������򴴽�
    if(!DSM_CreateSDPath(csSDPath + DSM_PCAP_SMV_FILE_FOLDER+m_csPcapPath))
    {
        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_RECORD_PATH_ERROR));
        return false;
    }
#else
	// �ж�·���Ƿ���ڣ��������򴴽�
	if(!DSM_CreateSDPath(csSDPath + DSM_PCAP_SMV_FILE_FOLDER))
	{
		CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_RECORD_PATH_ERROR));
		return false;
	}
#endif

    // ��ȡSD��ʣ��ռ�
    ULARGE_INTEGER nFree, nTotal;
    if(!GetDiskFreeSpaceEx(csSDPath, &nFree, &nTotal, NULL))
    {
        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_GET_SD_FREESPACE_FAILED));
        return false;
    }

    // �ж�SD��ʣ��ռ��Ƿ��㹻
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
//     // �����·�Ƿ�����
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

    // ����¼�����ƿ�
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
    CString csPre, csSDPath;       // SD��Ŀ¼

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

    // ����¼�����ƿ�
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
    // ��ʼ��¼��״̬��
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
	m_pMsgDlg->_ResetF1Text(_T("ֹͣ¼��"));
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

    // ��ʾ¼��״̬
    INT_PTR nRet = m_pMsgDlg->DoModal();

    if(m_nRecordTimer == 0x02)
    {
        KillTimer(m_nRecordTimer);
        m_nRecordTimer = 0;
    }

    // �ͷ�¼��״̬����Դ
    m_pMsgDlg->_ResetMenu();
    delete m_pMsgDlg;
    m_pMsgDlg = NULL;

    return nRet;
}

bool CSMVMsgMonitorDlg::_handleRecord()
{
    // ¼��״̬������
    INT_PTR nRet = _showRecordDlg();

    // ����¼���ļ���״ֵ̬
    uint32_t nSaveRet = 0;

    // SD�����γ�
    if(nRet == DSM_SMV_RECORD_ERR_SD_INJECTED && m_pDataLayer->CancelRecordWave())
    {
        RETAILMSG(1, (L"dsm: cancel record smv, sd card injected.\n"));
        DM_ShowReordErrorInfo(nRet);

        return false;
    }

    // ֹͣ¼��
    if(!m_pDataLayer->StopRecordWave(nSaveRet))
    {
        RETAILMSG(1, (L"dsm: stop record smv error\n"));
        DM_ShowReordErrorInfo(nRet > 200 ? nRet : nSaveRet);
    }
    else
    {
        // �ж��Ƿ�¼����С�����޶�ֵ(20M)
        if(nRet - 100 == MSG_RECORD_SIZE_REACHED)
        {
            CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_RECORD_SIZE_FULL));
        }
    }

    return true;
}
