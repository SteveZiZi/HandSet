/**
 * \file
 *      GooseScanDlg.cpp
 *
 * \brief
 *      ̽��goose���ƿ�ҳ��ʵ���ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/5/8
 */

// GooseScanDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "GooseScanDlg.h"

#include "src/main/common.h"
#include "src/main/mgr/statusmgr.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/service/datalayer/dsmdatalayer.h"

//////////////////////////////////////////////////////////////////////////
// goose - ̽��
//
#define DSM_GOOSE_SCAN_HEAD_CHECKBOX        48          ///< ѡ����ƿ���
#define DSM_GOOSE_SCAN_HEAD_INDEX           50//100         ///< ���ƿ�����
#define DSM_GOOSE_SCAN_HEAD_APPID           100         ///< ���ƿ�AppID
#define DSM_GOOSE_SCAN_HEAD_DESC            200//261         ///< ���ƿ�������û��������Ϣ����ʾMAC��ַ
#define DSM_GOOSE_SCAN_HEAD_PORT            91          ///< �˿���


#define DSM_GOOSE_SCAN_TIMERID              1           ///< ˢ��̽�����ݵĶ�ʱ��ID

#define DSM_GOOSE_CTRLBLK_MAX_NUM           10          ///< GOOSE����ѡ���ƿ����



/**
 *      GOOSE̽�����ݼ�¼��
 *
 *  \note
 *      checkbox����š�AppID���������˿�
 */
class CDsmScanGooseRecord : public CELRecord
{
public:
    /**
     *      GOOSE̽���¼��
     *
     *  \param[in]
     *      nIndex          ����
     *  \param[in]
     *      pInfo           ���ƿ���Ϣ
     */
    CDsmScanGooseRecord(int nIndex, CNetDetectItem* pInfo);
    virtual ~CDsmScanGooseRecord() {}

public:
    /**
     *      ��ȡgoose��¼���صĿ��ƿ���Ϣ
     */
    inline CNetDetectItem* _GetInfo()
    {
        return m_pInfo;
    }

private:
    CNetDetectItem*         m_pInfo;            ///< ��¼�����Ŀ��ƿ���Ϣ
};


CDsmScanGooseRecord::CDsmScanGooseRecord( int nIndex, CNetDetectItem* pInfo)
: m_pInfo(pInfo)
{
    ASSERT(m_pInfo);

    CString csAppID;
    csAppID.Format(L"0x%04X", m_pInfo->appId);

    CELRecordItem* pItem = AddItem(new CDsmRecordItem(L""));
    AddItem(new CDsmRecordItem(nIndex));
    AddItem(new CDsmRecordItem(csAppID));
    AddItem(new CDsmRecordItem(DSM_GetNetCtrlBlkDesc(m_pInfo)));
    AddItem(new CDsmRecordItem(DSM_GetPortDesc(m_pInfo->enPortName)));

    pItem->SetEditable(TRUE);
    pItem->HasCheckbox(TRUE);
}



// CGooseScanDlg �Ի���

IMPLEMENT_DYNAMIC(CGooseScanDlg, DSM_BASEDLG)

CGooseScanDlg::CGooseScanDlg(CWnd* pParent /*=NULL*/)
	: DSM_BASEDLG(CGooseScanDlg::IDD, DSM_STR_GOOSE_SCAN_CAPTION, pParent)
    , m_nScanTimerID(UINT_MAX)
    , m_pDataLayer(NULL)
{
    m_nMenuID = IDR_MENU_GOOSE_SCAN;

    m_pDataLayer = CDsmDataLayer::getInstance();
}

CGooseScanDlg::~CGooseScanDlg()
{
    _clearRes();
}

void CGooseScanDlg::DoDataExchange(CDataExchange* pDX)
{
	DSM_BASEDLG::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CGooseScanDlg, DSM_BASEDLG)
    ON_COMMAND_RANGE(ID_MENU_SCAN_SMV_DLG, ID_MENU_GOOSE_ENTER, &CGooseScanDlg::OnNaviMenuFn)
    ON_NOTIFY(ELNM_ITEM_CHECKED, IDC_GOOSE_SCAN_REPORT, &CGooseScanDlg::OnItemChecked)
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CGooseScanDlg ��Ϣ�������


BOOL CGooseScanDlg::OnInitDialog()
{
    CBaseDlg::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��

    _createReport();

    m_pDataLayer->BeginDetect(NET_RECV_TYPE_GOOSE);

    m_nScanTimerID = SetTimer(DSM_GOOSE_SCAN_TIMERID, DSM_GOOSE_SCAN_TIMER_INTERVAL, NULL);

    m_reportGoose.SetFocus();

    return FALSE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}


void CGooseScanDlg::OnNaviMenuFn( UINT nID )
{
    switch(nID)
    {
        // ����̽��goose���ƿ�
    case ID_MENU_GOOSE_RESCAN:
        _rescan();
        break;

        // ���ѡ��goose���ƿ�
    case ID_MENU_GOOSE_CLSSEL:
        _clearSelect();
        break;

        // ����gooseҵ��
    case ID_MENU_GOOSE_ENTER:
        if(_setSelect())
        {
            // �رն�ʱ��
            if(UINT_MAX != m_nScanTimerID)
            {
                KillTimer(m_nScanTimerID);
                m_nScanTimerID = UINT_MAX;
            }

            CEventProc::GetInstance()->ShowPage(IDD_GOOSE_REALTIME_DLG);
        }

        break;
	case ID_MENU_SCAN_FT3_DLG:
		{
			// ����FT3ҳ��
			CEventProc::GetInstance()->ShowPage(IDD_SCAN_SMV_DLG);
		}
		break;
	case ID_MENU_SCAN_GOOSE_DLG:
		break;
	case ID_MENU_SCAN_SMV_DLG:
		{
			// ����9-1/9-2 smv̽��ҳ��
			CEventProc::GetInstance()->ShowPage(IDD_SCAN_SMV_DLG);
		}
    default:
        break;
    }
}

void CGooseScanDlg::OnItemChecked( NMHDR* pNotifyStruct, LRESULT* pResult )
{
    ELNM_RECORDITEM_ARGS* pNS = (ELNM_RECORDITEM_ARGS*)pNotifyStruct;
    ASSERT(pNS);

    CDsmScanGooseRecord* pRecord = dynamic_cast<CDsmScanGooseRecord*>(m_reportGoose.GetRecordRow(pNS->rowIndex));
    ASSERT(pRecord);

    *pResult = TRUE;
    CNetDetectItem* pInfo = pRecord->_GetInfo();

    // ȡ��ѡ�񣬽���m_lstSelects�б����Ƴ���ѡ���ƿ�
    if(!pNS->pItem->IsChecked())
    {
        CNetDetectItem* pTempInfo = NULL;
        POSITION prepos;
        POSITION pos = m_lstSelects.GetHeadPosition();
        while(pos)
        {
            prepos = pos;
            pTempInfo = dynamic_cast<CNetDetectItem*>(m_lstSelects.GetNext(pos));
            ASSERT(pTempInfo);

            if(pTempInfo && pTempInfo->appId == pInfo->appId && pTempInfo->enPortName == pInfo->enPortName)
            {
                m_lstSelects.RemoveAt(prepos);
                break;
            }
        }

        return;
    }

#pragma message(__FILE__ "(" DSM_STRING(__LINE__) "): �Ƿ���Ҫ���˲�ͬ�˿ڹ�������ͬ���ƿ�")

    // ���ƿ鳬����ѡ��Ŀ��ȡ��ѡ��
    if(m_lstSelects.GetCount() >= DSM_GOOSE_CTRLBLK_MAX_NUM)
    {
        CString csTip;
        CString csFmt(dsm_stringMgr::Get(DSM_STR_GOOSE_SCAN_NUMMAX));
        csTip.Format(csFmt, DSM_GOOSE_CTRLBLK_MAX_NUM);
        CStatusMgr::SetTips(csTip);

        pNS->pItem->SetChecked(FALSE);
        return;
    }

    m_lstSelects.AddTail(pRecord->_GetInfo());
}


void CGooseScanDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

    if(nIDEvent == m_nScanTimerID)
    {
        _insertRecord();
    }

    CBaseDlg::OnTimer(nIDEvent);
}


void CGooseScanDlg::_createReport()
{
    CRect rcReport;
    GetClientRect(rcReport);
    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);

    int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

    m_reportGoose.Create(DSM_WINSTYLE_REPORT_DEFAULT, rcReport, this, IDC_GOOSE_SCAN_REPORT);

    // ��������
    m_reportGoose.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_reportGoose.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    // ������ʽ
    m_reportGoose.PostInitTemplateStyle();

    // ������Ԫ��
    m_reportGoose.InsertColumn(0, L"", EL_CENTER, DSM_GOOSE_SCAN_HEAD_CHECKBOX);   // checkbox
    m_reportGoose.InsertColumn(1, dsm_stringMgr::Get(DSM_STR_GOOSE_SCAN_HEAD_INDEX), EL_CENTER, DSM_GOOSE_SCAN_HEAD_INDEX);     // ���
    m_reportGoose.InsertColumn(2, dsm_stringMgr::Get(DSM_STR_GOOSE_SCAN_HEAD_APPID), EL_CENTER, DSM_GOOSE_SCAN_HEAD_APPID);     // APPID
    m_reportGoose.InsertColumn(3, dsm_stringMgr::Get(DSM_STR_GOOSE_SCAN_HEAD_DESC), EL_LEFT, DSM_GOOSE_SCAN_HEAD_DESC);         // ����
    m_reportGoose.InsertColumn(4, dsm_stringMgr::Get(DSM_STR_GOOSE_SCAN_HEAD_PORT), EL_LEFT, DSM_GOOSE_SCAN_HEAD_PORT);       // �˿�

    // ����ѡ�񽹵���ʽ
    m_reportGoose.SetRowSelectionVisible(TRUE);
    m_reportGoose.SetRowFocusEnable(TRUE);
    m_reportGoose.SetSubItemFocusEnable(TRUE);
    m_reportGoose.SetFreezeSelectionVisible(TRUE);
    m_reportGoose.SetFocusedColIndex(0);
}

void CGooseScanDlg::_rescan()
{
    // ���֮ǰ̽����Ϣ
    _clearRes();

    m_pDataLayer->BeginDetect(NET_RECV_TYPE_GOOSE);
}

void CGooseScanDlg::_clearSelect()
{
    CELRecordItem* pItem    = NULL;

    // ���ѡ�еĿ��ƿ�
    int nCounts = m_reportGoose.GetRecordRowCount();
    for(int i = 0; i < nCounts; i++)
    {
        pItem = m_reportGoose.GetRecordRow(i)->GetItem(0);

        if(pItem->IsChecked())
        {
            pItem->SetChecked(FALSE);
            m_reportGoose.RedrawRowItem(i, 0);
        }
    }

    m_lstSelects.RemoveAll();
}

bool CGooseScanDlg::_setSelect()
{
    CBaseDetectItem* pInfo  = NULL;
    POSITION pos            = m_lstSelects.GetHeadPosition();

    if(m_lstSelects.GetCount() <= 0)
    {
        return false;
    }

    while(pos)
    {
        // ����ѡ�еĿ��Ƶ����ݲ�
        pInfo = m_lstSelects.GetNext(pos);
        if(!m_pDataLayer->AddOneSelectedCtrlBlk(pInfo))
        {
            TRACE(L"������ѡ���ƿ�ʧ��");
            ASSERT(FALSE);
            return false;
        }
    }

    m_pDataLayer->AddSelectedCtrlBlkFinished();

    return true;
}

void CGooseScanDlg::_clearRes()
{
    if(m_reportGoose.GetSafeHwnd())
    {
        m_reportGoose.DeleteAllRecordRow();
    }

    m_lstDetects.RemoveAll();
    m_lstSelects.RemoveAll();
}

void CGooseScanDlg::_insertRecord()
{ 
    unsigned int nCounts        = m_reportGoose.GetRecordRowCount();
    unsigned int nDetectCounts  = m_pDataLayer->GetNetDetectItemsCount();

    // �����µ�̽�����򷵻�
    if(nDetectCounts == nCounts ||
        !m_reportGoose.GetSafeHwnd())
    {
        return;
    }

    CNetDetectItem* pInfo= NULL;

    // �����µļ�¼
    for(unsigned int i = nCounts; i < nDetectCounts; i++)
    {
        pInfo = m_pDataLayer->GetNetDetectItem(i);

        if(!pInfo)
        {
            TRACE(L"dsm: get goose ctrlblk error\n");
            continue;
        }

        m_reportGoose.InsertRecordRow(i, new CDsmScanGooseRecord(i, pInfo));

        for(int nColumn = 1; nColumn < 5; nColumn++)
        {
            m_reportGoose.GetRecordRow(i)->GetItem(nColumn)->SetFocusable(FALSE);
        }

        if(0 == i)
        {
            m_reportGoose.SetFocusedSelectionRowIndex(0);
        }

        m_lstDetects.AddTail(pInfo);
    }
}

