/**
 * \file
 *      GooseMonitorDlg.cpp
 *
 * \brief
 *      goose���ļ���ҳ��ʵ���ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/5/10
 */

// GooseMonitorDlg.cpp : ʵ���ļ�
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



// CGooseMonitorDlg �Ի���

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



// CGooseMonitorDlg ��Ϣ�������

BOOL CGooseMonitorDlg::OnInitDialog()
{
    CGooseBaseDlg::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��

    if(!m_pCurrentCtrlBlk && m_pDataLayer->GetSelectedCtrlBlkCount() > 0)
    {
        m_pDataLayer->SetCurrentCtrlBlk(m_pDataLayer->GetFirstSelectCtrlBlk());
        m_pCurrentCtrlBlk = m_pDataLayer->GetCurrentCtrlBlk();
    }

    if(m_pCurrentCtrlBlk)
    {
        m_pDataLayer->SetCurrentMode(DSM_MODE_GOOSE_RECV_REAL_TIME_MSG);
    }

    // ��ʾʵʱ���Ŀؼ�
    _showFrame();

    return FALSE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}

//void CGooseMonitorDlg::OnMenuF3()
//{
//    ASSERT(m_pDataLayer);
//
//    // ʵʱˢ�����ݣ����ݲ���ܻ�������һ�ε�����
//    if(m_pDataLayer->GetRealTimeMsg(&m_data))
//    {
//        // ��õ�һ��ʵʱ���ݺ�رն�ʱ��
//        if(m_nRealTimer == 0x01)
//        {
//            KillTimer(m_nRealTimer);
//            m_nRealTimer = 0;
//        }
//
//        int nSel    = m_treeFrame.GetCurSel();
//        int nVisual = m_treeFrame.GetTopIndex();
//
//        // ����������Ϣ
//        _updateFrame(m_data.m_enFrameType, &m_data.m_nDataBuffer[0], m_data.m_nFrameLen);
//
//        // �ָ������У���֮ǰ�нڵ㱻����������ܽ���λ�û���ƫ��
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
//        // ˢ�¿����ݣ��������ʹ�ñ�ķ�ʽ��ʾˢ�º�����û����
//        m_treeFrame.DeleteAllTreeItems();
//    }
//}


void CGooseMonitorDlg::OnMenuF3()
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

    CString csPcapFolder = csSDPath + DSM_PCAP_GOOSE_FILE_FOLDER;     // pcapĿ¼

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




void CGooseMonitorDlg::OnMenuF4()
{
    // ¼��ǰ�����SD��Ŀ¼�����������Ƿ�����
    if(!_checkforRecordGoose())
    {
        return;
    }

    // �����ֳ�
    CString csTitle = CStatusMgr::GetTitle();
    LBT_HTREEITEM item = m_treeFrame.GetTreeSelectedItem();

    // goose¼��
    bool bRet = _recordGoose();
    m_pDataLayer->SetCurrentMode(DSM_MODE_GOOSE_RECV_REAL_TIME_MSG);

    INT_PTR nRet = IDOK;

#if !CHUN_HUA

	CStatusMgr::SetTips(L"");

	// SD��Ŀ¼
	CString csSDPath;

	// �ж�SD���Ƿ����
	if(!DSM_GetSDDriver(csSDPath))
	{
		CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_SD_NOT_FOUND_PCAP));
		return;
	}

	CString csPcapFolder = csSDPath + DSM_PCAP_GOOSE_FILE_FOLDER+m_csRecordPath;     // pcapĿ¼

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

	 
	HWND hWnd       = ::GetFocus();

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
		// �ָ��ֳ�
		CStatusMgr::SetTitle(csTitle);
		m_treeFrame.SetFocus();
		m_treeFrame.SelectTreeItem(item);
	}
	else
	{
		// ʹ��home�˵�������ҳ��ʱ��������Ӧ����
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
        // �ָ��ֳ�
        CStatusMgr::SetTitle(csTitle);
        m_treeFrame.SetFocus();
        m_treeFrame.SelectTreeItem(item);
    }
    else
    {
        // ʹ��home�˵�������ҳ��ʱ��������Ӧ����
        DSM_SetZFocus(CEventProc::GetInstance()->GetCurrentPageHwnd());
    }
#endif
}


/**
 *      �޸Ĳ˵�HMENU
 */
void CGooseMonitorDlg::_modifyMenu()
{
    __super::_modifyMenu();

#ifdef ZY_PRJ
	// ����F6����ʽ ����ʼ��¼��
    m_menuMain.SetMenuSubItem(5, NULL);
    m_menuMain.SetMenuItemId(5, ID_MENU_GOOSE_F6);

    // ʵʱ��Ϣ������pcap�ļ���¼��
    m_menuMain.SetMenuItemCaption(2, dsm_stringMgr::Get(DSM_STR_GOOSE_MONITOR_REFRESH));
    m_menuMain.SetMenuItemCaption(4, dsm_stringMgr::Get(DSM_STR_GOOSE_MONITOR_LOADFILE));
    m_menuMain.SetMenuItemCaption(5, dsm_stringMgr::Get(DSM_STR_GOOSE_MONITOR_BEGIN_REC));
#else
	// ����F6����ʽ ����ʼ��¼��
	m_menuMain.SetMenuSubItem(5, NULL);
	m_menuMain.SetMenuItemId(2, ID_MENU_GOOSE_F3);
	m_menuMain.SetMenuItemId(3, ID_MENU_GOOSE_F4);
	
	// ʵʱ��Ϣ������pcap�ļ���¼��
	m_menuMain.SetMenuItemCaption(2, L"����ļ�");
	//m_menuMain.SetMenuItemCaption(4, dsm_stringMgr::Get(DSM_STR_GOOSE_MONITOR_LOADFILE));
	m_menuMain.SetMenuItemCaption(3, L"¼��");
#endif
    
}

bool CGooseMonitorDlg::_switchCtrlBlk( UINT nCmdID )
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

/**
 *      �������ؼ�
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

    // ��������ʽ
    m_treeFrame.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_treeFrame.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_treeFrame.PostInitTemplateStyle();

    m_treeFrame.InsertTreeColumn(0, dsm_stringMgr::Get(DSM_STR_GOOSE_MONITOR_ITEM), EL_LEFT, 280);
    m_treeFrame.InsertTreeColumn(1, dsm_stringMgr::Get(DSM_STR_GOOSE_MONITOR_VALUE), EL_CENTER, 250);

    m_treeFrame.SetFocus();
}


/**
 *      ��ʾgoose֡����
 */
void CGooseMonitorDlg::_showFrame()
{
    if(!::IsWindow(m_treeFrame.GetSafeHwnd()))
    {
        _createTree();
    }

    m_treeFrame.ShowWindow(SW_SHOW);
    m_treeFrame.SetFocus();

    // ��ȡ�µ�һ֡����
    if(m_pDataLayer->GetRealTimeMsg(&m_data))
    {
        _updateFrame(m_data.m_enFrameType, &m_data.m_nDataBuffer[0], m_data.m_nFrameLen);
    }
    else
    {
        // ��ȡʧ�ܣ���򿪶�ʱ��ֱ����ȡһ����Ч���ݺ��ֹͣ
        m_nRealTimer = SetTimer(0x01, 200, NULL);
    }
}


/**
 *      ����֡����
 *
 *  \param[in]
 *      type            ֡����
 *  \param[in]
 *      pBuff           ֡���ݻ�����
 *  \param[in]
 *      bufflen         ֡���ݻ���������
 */
void CGooseMonitorDlg::_updateFrame(enumFrameType type, const uint8_t* pBuff, int bufflen)
{
    // ����gooseЭ������
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
 *      goose¼��
 */
bool CGooseMonitorDlg::_recordGoose()
{
    // ¼����������
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
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

    // �Զ���ȡһ��ʵʱ����
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
    // ��ʱ��ѯ״̬
    else if(m_nDetectTimer == 0x02)
    {
        // ���¼����״̬��SD��������
        if(CStatusMgr::GetSD() == DSM_STATU_DISCONNECT_SD)
        {
            if(m_pMsgDlg->GetSafeHwnd())
            {
                m_pMsgDlg->EndDialog(DSM_SMV_RECORD_ERR_SD_INJECTED);
            }
        }

//             // ��·δ����
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
	m_csRecordPath.Format(L"%s%04d%02d%02d_%02d%02d%02d\\", L"GOOSE_", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	// �ж�·���Ƿ���ڣ��������򴴽�
	if(!DSM_CreateSDPath(csSDPath + DSM_PCAP_GOOSE_FILE_FOLDER+m_csRecordPath))
	{
		CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_RECORD_PATH_ERROR));
		return false;
	}
#else
    // �ж�·���Ƿ���ڣ��������򴴽�
    if(!DSM_CreateSDPath(csSDPath + DSM_PCAP_GOOSE_FILE_FOLDER))
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

    //         // �����·�Ƿ�����
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

    // ����¼�����ƿ�
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
    CString csSDPath;       // SD��Ŀ¼

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
    // ¼��״̬������
    INT_PTR nRet = _showRecordDlg();

    // ����¼���ļ���״ֵ̬
    uint32_t nSaveRet = 0;

    // SD�����γ�
    if(nRet == 201 && m_pDataLayer->CancelRecordWave())
    {
        RETAILMSG(1, (L"dsm: cancel record goose, sd card injected.\n"));
        DM_ShowReordErrorInfo(nRet);

        return false;
    }

    // ֹͣ¼��
    if(!m_pDataLayer->StopRecordWave(nSaveRet))
    {
        RETAILMSG(1, (L"dsm: stop record goose error\n"));
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

INT_PTR CGooseMonitorDlg::_showRecordDlg()
{
    // ��ʾ¼��״̬
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
	m_pMsgDlg->_ResetF1Text(_T("ֹͣ¼��"));
#endif
    m_nDetectTimer = SetTimer(0x02, 500, NULL);

    // ��ʾ¼��״̬
    INT_PTR nRet = m_pMsgDlg->DoModal();

    if(m_nDetectTimer == 0x02)
    {
        KillTimer(m_nDetectTimer);
        m_nDetectTimer = 0;
    }

    // �ͷ�¼��״̬����Դ
    m_pMsgDlg->_ResetMenu();
    delete m_pMsgDlg;
    m_pMsgDlg = NULL;

    return nRet;
}

