/**
 * \file
 *      GooseShiftDlg.cpp
 *
 * \brief
 *      goose��λҳ��ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/5/10
 */

// GooseShiftDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "GooseShiftDlg.h"

#include "src/main/mgr/statusmgr.h"
#include "src/main/utils/utility.h"
#include "src/main/pagedata/pagedata.h"
#include "src/main/goose/utility_goose.h"
#include "src/utils/utility/dsm_utility.h"


//////////////////////////////////////////////////////////////////////////
// goose - ��λ��Ϣ
//
#define DSM_GOOSE_SHIFT_HEAD_INDEX          48          ///< ����

#if CHUN_HUA
#define DSM_GOOSE_SHIFT_HEAD_TIME           200         ///< ʱ��
#define DSM_GOOSE_SHIFT_HEAD_INFO           352         ///< ��Ϣ
#else
#define DSM_GOOSE_SHIFT_HEAD_TIME           200         ///< ʱ��
#define DSM_GOOSE_SHIFT_HEAD_INFO           281         ///< ��Ϣ
#endif


#define DSM_GOOSE_SHIFT_HEAD_IND            60          ///< ��Ϣ
#define DSM_GOOSE_SHIFT_HEAD_TYPE           110         ///< ����
#define DSM_GOOSE_SHIFT_HEAD_OLD            190         ///< ��λǰֵ
#define DSM_GOOSE_SHIFT_HEAD_NEW            240         ///< ��λ��ֵ

#define SHIFT_FILTER_FIRST                  45000       ///< ͨ�������Ĳ˵�ID��ʼֵ
#define SHIFT_FILTER_LAST                   45100       ///< ͨ�������Ĳ˵�ID���ֵ



/**
 *      goose���� - ��ʷ��λ�б�
 *
 *  \note
 *      ������ʱ�䣨����/��ԣ�����λ��Ϣ
 */
class CDsmGooseShiftRecord : public CELRecord
{
public:
    CDsmGooseShiftRecord(int nIndex, CString csTime, CString csShift)
        : m_nIndex(nIndex)
    {
        AddItem(new CDsmRecordItem(m_nIndex));
        AddItem(new CDsmRecordItem(csTime));
        AddItem(new CDsmRecordItem(csShift));
    }

    virtual ~CDsmGooseShiftRecord() {}

public:

    /**
     *      ��ȡ��¼�ĵڼ��α�λ����
     */
    uint32_t GetIndex()
    {
        return m_nIndex;
    }

private:
    uint32_t        m_nIndex;           ///< �ڼ��α�λ����
};

/**
 *      goose���� - ��ʷ��λ��ϸ��Ϣ
 *
 *  \note
 *      ���������͡���ֵ����ֵ
 */
class CDsmGooseShiftDetailRecord : public CELRecord
{
public:
    CDsmGooseShiftDetailRecord()
    {
        AddItem(new CDsmRecordItem(L""));
        AddItem(new CDsmRecordItem(L""));
        AddItem(new CDsmRecordItem(L""));
        AddItem(new CDsmRecordItem(L""));
    }

    virtual ~CDsmGooseShiftDetailRecord() {}
};



/**
 *      goose��λ���ƿ�����������
 */
class CCtrlBlkDataGooseShift : public ICtrlBlkData
{
public:

    CCtrlBlkDataGooseShift()
        : m_eMode(TIME_REL)
        , m_nChann(0)
        , m_nFocus(0)
        , m_nRows(0)
    {

    }

    virtual ~CCtrlBlkDataGooseShift() {}

public:
    ModeTime            m_eMode;            ///< ҳ��ģʽ
    int                 m_nChann;           ///< ��ͨ������
    int                 m_nFocus;           ///< ��λ�б��focus�к�
    int                 m_nRows;            ///< ��������
};


// CGooseShiftDlg �Ի���

IMPLEMENT_DYNAMIC(CGooseShiftDlg, DSM_GOOSEASEDLG)

CGooseShiftDlg::CGooseShiftDlg(CWnd* pParent /* =NULL */)
	: DSM_GOOSEASEDLG(CGooseShiftDlg::IDD, DSM_STR_GOOSE_SHIFT_CAPTION, pParent)
    , m_eTime(TIME_REL)
    , m_ePage(PAGE_LIST)
    , m_dwLast(0)
    , m_nChannel(0)
    , m_pMenuDetail(NULL)
    , m_pMenuList(NULL)
    , m_eShift(SHIFT_ALL)
    , m_nDetailIndex(0)
    , m_nRows(0)
    , m_nParentChann(0)
    , m_nSubChann(-1)
    , m_bRecounter(true)
    , m_nLastFocus(0)
    , m_nShiftIndex(0)
    , m_nShiftSubIndex(-1)
{

}

CGooseShiftDlg::~CGooseShiftDlg()
{
    if(m_pMenuDetail)
    {
        delete m_pMenuDetail;
    }
}

void CGooseShiftDlg::DoDataExchange(CDataExchange* pDX)
{
	DSM_GOOSEASEDLG::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CGooseShiftDlg, DSM_GOOSEASEDLG)
    ON_WM_TIMER()
    ON_COMMAND(ID_MENU_GOOSE_F3, &CGooseShiftDlg::OnMenuF3)
    ON_COMMAND(ID_MENU_GOOSE_F6, &CGooseShiftDlg::OnMenuF6)
    ON_COMMAND(ID_MENU_GOOSE_SHIFT_DETAIL_F6, &CGooseShiftDlg::OnMenuShiftF6)
    ON_NOTIFY(ELNM_OKCLICK, IDC_GOOSE_SHIFT_REPORT, &CGooseShiftDlg::OnClickOK)
END_MESSAGE_MAP()


// CGooseShiftDlg ��Ϣ�������


BOOL CGooseShiftDlg::OnInitDialog()
{
    CGooseBaseDlg::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��

    // ��������
    _createReport();

    // �ָ�ҳ��״̬
    _restorePage();

    if(m_pCurrentCtrlBlk)
    {
        m_pDataLayer->SetCurrentMode(DSM_MODE_GOOSE_RECV_CHANGE_LIST);

        _beginTimer();
    }

    return FALSE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}


/**
 *      ������λ��Ϣ����
 */
void CGooseShiftDlg::_createReport()
{
    CRect rcReport;
    GetClientRect(rcReport);
    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);

    int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

    m_reportGoose.Create(DSM_WINSTYLE_REPORT_DEFAULT, rcReport, this, IDC_GOOSE_SHIFT_REPORT);

    // ������ʽ
    m_reportGoose.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_reportGoose.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_reportGoose.PostInitTemplateStyle();

    CString csCaption(dsm_stringMgr::Get(DSM_STR_GOOSE_SHIFT_HEAD_TIME));

    if(m_eTime == TIME_REL)
    {
        csCaption += L"(s)";
    }

    // ������Ԫ��
    m_reportGoose.InsertColumn(0, L"", EL_RIGHT, DSM_GOOSE_SHIFT_HEAD_INDEX);               // ����
    m_reportGoose.InsertColumn(1, csCaption, EL_LEFT, DSM_GOOSE_SHIFT_HEAD_TIME);           // ��λʱ��
    m_reportGoose.InsertColumn(2, dsm_stringMgr::Get(DSM_STR_GOOSE_SHIFT_HEAD_INFO), EL_LEFT, DSM_GOOSE_SHIFT_HEAD_INFO);       // ��λ��Ϣ

    // ����ѡ�񽹵���ʽ
    m_reportGoose.SetRowSelectionVisible(TRUE);
    m_reportGoose.SetRowFocusEnable(TRUE);

    m_reportGoose.SetFocus();
}

/**
 *      ������ϸ��Ϣ����
 */
void CGooseShiftDlg::_createReportDetail()
{
    CRect rcReport;
    GetClientRect(rcReport);
    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);

    m_nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + m_nRows * ELT_T1_ROWHEIGHT;

    m_reportDetail.Create(DSM_WINSTYLE_REPORT_DEFAULT, rcReport, this, IDC_GOOSE_SHIFT_DETAIL_REPORT);

    // ������ʽ
    m_reportDetail.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_reportDetail.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_reportDetail.PostInitTemplateStyle();

    // ������Ԫ��
    m_reportDetail.InsertColumn(0, L"", EL_CENTER, DSM_GOOSE_SHIFT_HEAD_IND);               // ����
    m_reportDetail.InsertColumn(1, dsm_stringMgr::Get(DSM_STR_GOOSE_SHIFT_HEAD_TYPE), EL_CENTER, DSM_GOOSE_SHIFT_HEAD_TYPE);    // ����
    m_reportDetail.InsertColumn(2, dsm_stringMgr::Get(DSM_STR_GOOSE_SHIFT_HEAD_OLD), EL_CENTER, DSM_GOOSE_SHIFT_HEAD_OLD);      // ��ֵ
    m_reportDetail.InsertColumn(3, dsm_stringMgr::Get(DSM_STR_GOOSE_SHIFT_HEAD_NEW), EL_CENTER, DSM_GOOSE_SHIFT_HEAD_NEW);      // ��ֵ

    // ����ѡ�񽹵���ʽ
    m_reportDetail.SetRowSelectionVisible(FALSE);
    m_reportDetail.SetRowFocusEnable(FALSE);

    // �������������������ɵ�����
    for(int i = 0; i < m_nRows; i++)
    {
        m_reportDetail.InsertRecordRow(i, new CDsmGooseShiftDetailRecord);
    }

    m_reportDetail.SetFocus();
}


void CGooseShiftDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

    if(nIDEvent == m_nTimerID)
    {
        _updateData();
    }

    CGooseBaseDlg::OnTimer(nIDEvent);
}


/**
 *      ˢ��goose��λ��Ϣ
 */
void CGooseShiftDlg::_updateData()
{
    ASSERT(m_pDataLayer);
    ASSERT(m_pCurrentCtrlBlk);

    if(!m_pDataLayer->GetGooseChangeList(&m_data))
    {
        return;
    }

    // �ж��Ƿ����µı�λ����

    uint32_t nNewShift = m_data.GetNewChangeTimes();
    if(!m_bRecounter && nNewShift <= 0)
    {
        return;
    }

    int nFocus      = m_reportGoose.GetFocusedRowIndex();
    int nRowCounts  = m_reportGoose.GetRecordRowCount();

    // �жϱ�λ��Ŀ�Ƿ񳬹�����DSM_MAX_GOOSE_CHANGELIST_SIZE������������Ҫ��ͷɾ����λrecord
    if(nNewShift + nRowCounts > DSM_MAX_GOOSE_CHANGELIST_SIZE)
    {
        int nDel = nNewShift + nRowCounts - DSM_MAX_GOOSE_CHANGELIST_SIZE;
        int nNewFocus = nFocus - nDel;

        // ��ͷ��ɾ������record
        for(int i = 0; i < nDel && m_reportGoose.GetRecordRowCount() > 0; i++)
        {
            m_reportGoose.DeleteRecordRow(0);
        }

        // ����֮ǰ������
        if(nNewFocus >= 0 && nNewFocus < m_reportGoose.GetRecordRowCount())
        {
            m_reportGoose.SetFocusedSelectionRowIndex(nNewFocus);
            m_reportGoose.EnsureRowVisibleScrollRow(nNewFocus);
        }
        // ֮ǰ�����б�ɾ�������һ�л�ý���
        else if(nNewFocus == -1)
        {
            m_reportGoose.SetFocusedSelectionRowIndex(0);
            m_reportGoose.EnsureRowVisibleScrollRow(0);
        }
        // ���������һ�л�ý���
        else
        {
            m_reportGoose.SetFocusedSelectionRowIndex(m_reportGoose.GetRecordRowCount() - 1);
            m_reportGoose.EnsureRowVisibleScrollRow(m_reportGoose.GetRecordRowCount() - 1);
        }
    }

    int nChannel;
    int nIndex              = 0;
    bool bInsert            = false;       // �Ƿ�����ͨ������
    uint32_t nShiftCounts   = m_data.GetChangeTimes();

    CGooseOneTimeChange data;
    CString csTime, csChannel, csFmt, csDesc;
    csFmt = dsm_stringMgr::Get(DSM_STR_GOOSE_SHIFT_CHANNEL);

    // �жϽ������Ƿ���ĩ�У����ǣ�����󽹵��лᱣ�����һ��
    bool bEnd = m_reportGoose.GetFocusedRowIndex() == m_reportGoose.GetRecordRowCount() - 1 ? true : false;

    ASSERT(nShiftCounts <= DSM_MAX_GOOSE_CHANGELIST_SIZE);

    // ���¹���ͨ�������еı�λ�����µ�
    if(m_bRecounter)
    {
        nNewShift = nShiftCounts;
    }

    m_bRecounter = false;
    GOOSE_UTC_TIME timeBef;
    GOOSE_UTC_TIME timeAft;

    // β������������
    for(uint32_t i = nShiftCounts - nNewShift; i < nShiftCounts; i++)
    {
        csDesc = L"";
        data = m_data.GetGooseChangeInfo(i);

        if(!data.GetDataAfterChange() || !data.GetDataBeforeChange())
        {
            continue;
        }

        // ��ʽ����λͨ����Ϣ
        uint32_t nCount = data.GetChangedChannelCounts();
        for(uint32_t i = 0; i < nCount; i++)
        {
            nChannel = data.GetChangedChannelIndex(i);

            // �ҵ����˵�ͨ�������ñ�־λ
            if(nChannel == m_nChannel - 1)
            {
                bInsert = true;
            }

            csChannel.Format(L"��%s%d", csFmt, nChannel + 1);
            csDesc += csChannel;
        }

        timeAft = data.GetDataAfterChange()->GetEventTime();
        timeBef = data.GetDataBeforeChange()->GetEventTime();

        // ���ʱ��
        if(m_eTime == TIME_REL)
        {
            // ��ȡ����ʱ����һ�α�λ�Ļ�׼ʱ��
            if(m_nChannel != 0 && bInsert)
            {
                // ��ȡ��һ����λ��Ϣ
                if(m_reportGoose.GetRecordRowCount() > 0)
                {
                    CDsmGooseShiftRecord* pRecord = dynamic_cast<CDsmGooseShiftRecord*>(m_reportGoose.GetRecordRow(m_reportGoose.GetRecordRowCount() - 1));
                    ASSERT(pRecord);

                    uint32_t nPrev      = pRecord->GetIndex();
                    uint32_t nChCount   = m_data.GetChangeTimes();

                    for(uint32_t k = 0; k < nChCount; k++)
                    {
                        CGooseOneTimeChange ch = m_data.GetGooseChangeInfo(k);
                        if(ch.GetChangeIndex() == nPrev)
                        {
                            timeBef = ch.GetDataAfterChange()->GetEventTime();
                            break;
                        }
                    }
                }
                // ��ȡ��һ�α�λ��Ϣ
                else
                {
                    timeBef = m_data.GetGooseChangeInfo(0).GetDataBeforeChange()->GetEventTime();
                }
            }

            csTime = DSM_GetDValue(timeAft.t_s, timeAft.t_ns, timeBef.t_s, timeBef.t_ns);
        }
        // ����ʱ��
        else
        {
            UTC_TIME_t ut = {data.GetDataAfterChange()->GetEventTime().t_s, data.GetDataAfterChange()->GetEventTime().t_ns / 1000};
            csTime = DM_GetSystemTimeStrFromUTC(ut);
        }

        nIndex = m_reportGoose.GetRecordRowCount();
        csDesc = csDesc.Right(csDesc.GetLength() - 1);

        // ȫ��ͨ��
        if(m_nChannel == 0)
        {
            m_reportGoose.InsertRecordRow(nIndex, new CDsmGooseShiftRecord(data.GetChangeIndex(), csTime, csDesc));
        }
        // ������ʾָ��ͨ��
        else
        {
            if(bInsert)
            {
                m_reportGoose.InsertRecordRow(nIndex, new CDsmGooseShiftRecord(data.GetChangeIndex(), csTime, csDesc));
            }

            bInsert = false;
        }
    }

    int nCounts = m_reportGoose.GetRecordRowCount();

    // �ָ�һ�����ʵĽ���
    if(nCounts > 0)
    {
        if(m_reportGoose.GetFocusedRowIndex() < 0)
        {
            m_reportGoose.SetFocusedSelectionRowIndex(0);
        }
        else if(bEnd)
        {
            m_reportGoose.SetFocusedSelectionRowIndex(nCounts - 1);
            m_reportGoose.EnsureRowVisibleScrollRow(nCounts - 1);
        }
    }
}


void CGooseShiftDlg::OnMenuF3()
{
    if(PAGE_DETAIL == m_ePage)
    {
        return;
    }

    CString csCaption;
    m_eTime = (m_eTime == TIME_REL) ? TIME_ABS : TIME_REL;

    if(TIME_REL == m_eTime)
    {
        // ����F3���ͼ��
        m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_M_GS_ABSRELA_RELA), NULL, NULL);
        csCaption = dsm_stringMgr::Get(DSM_STR_GOOSE_SHIFT_HEAD_TIME) + L"(s)";
    }
    else
    {
        // ����F3����ͼ��
        m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_M_GS_ABSRELA_ABS), NULL, NULL);
        csCaption = dsm_stringMgr::Get(DSM_STR_GOOSE_SHIFT_HEAD_TIME);
    }

    m_pNaviBar->UpdateNaviMenu();

    LVCOLUMN colum;
    ::ZeroMemory(&colum, sizeof(LVCOLUMN));
    if(m_reportGoose.GetColumn(1, &colum))
    {
        colum.pszText       = csCaption.GetBuffer(csCaption.GetLength());
        colum.mask          = LVCF_TEXT;
        colum.cchTextMax    = csCaption.GetLength();
        m_reportGoose.SetColumn(1, &colum);
        csCaption.ReleaseBuffer(csCaption.GetLength());
    }

    _switchAbsoRel();
}

void CGooseShiftDlg::OnMenuF6()
{
//     if(PAGE_DETAIL != m_ePage)
//     {
//         return;
//     }
#if CHUN_HUA
	int i =0;
#endif
}


void CGooseShiftDlg::OnClickOK( NMHDR* /* pNotifyStruct */, LRESULT* pResult )
{
    *pResult = 1;

    // ��ȡѡ����
    m_nLastFocus = m_reportGoose.GetFocusedRowIndex();
    if(m_reportGoose.GetRecordRowCount() <= 0 || m_nLastFocus < 0 || m_nLastFocus >= m_reportGoose.GetRecordRowCount())
    {
        return;
    }

    // ��ȡѡ�б�λ��Ϣ������
    CDsmGooseShiftRecord* pRecord = dynamic_cast<CDsmGooseShiftRecord*>(m_reportGoose.GetRecordRow(m_nLastFocus));
    ASSERT(pRecord);
    m_nDetailIndex = pRecord->GetIndex();

    CGooseOneTimeChange change;
    // ��ȡָ����λ��Ϣʧ��
    if(!_getChange(m_nDetailIndex, change))
    {
        return;
    }

    // ����ʵ��λ��Ϣ��ֻ��ͨ�����������ͷ����仯���������
    CGooseInfo* pBefore = change.GetDataBeforeChange();
    CGooseInfo* pAfter  = change.GetDataAfterChange();

    // ������ͬ
    if(!pBefore || !pAfter || pBefore->GetChannelCounts() != pAfter->GetChannelCounts())
    {
        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_GOOSE_CHANNEL_CHANGE));
        return;
    }

    CGooseChannel* pBef = NULL;
    CGooseChannel* pAft = NULL;
    uint32_t nCounts    = pBefore->GetChannelCounts();

    // �ж�����
    for(uint32_t i = 0; i < nCounts; i++)
    {
        pBef = pBefore->GetChanneData(i);
        pAft = pAfter->GetChanneData(i);

        if(!pBef || !pAft || pBef->GetChannelData()->type != pAft->GetChannelData()->type)
        {
            CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_GOOSE_CHANNEL_CHANGE));
            return;
        }

        // �жϽṹ��ͨ��
        if(pBef->GetChannelData()->type == GSEDT_STRUCT)
        {
            if(pBef->GetSubChannelCount() != pAft->GetSubChannelCount())
            {
                CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_GOOSE_CHANNEL_CHANGE));
                return;
            }

            uint32_t nSubCounts     = pBef->GetSubChannelCount();
            CGooseChannel* pSubBef  = NULL;
            CGooseChannel* pSubAft  = NULL;

            for(uint32_t k = 0; k < nSubCounts; k++)
            {
                pSubBef = pBef->GetSubChannelData(k);
                pSubAft = pAft->GetSubChannelData(k);

                if(!pSubAft || !pSubBef || pSubBef->GetChannelData()->type != pSubAft->GetChannelData()->type)
                {
                    CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_GOOSE_CHANNEL_CHANGE));
                    return;
                }
            }
        }
    }

    // ���ý���λ����Ϣ
    m_nShiftIndex    = 0;
    m_nShiftSubIndex = -1;

    m_nParentChann   = 0;
    m_nSubChann      = -1;

    m_eShift        = SHIFT_ALL;

    // �鿴��λ����ϸ��Ϣ
    _showDetail();
}

/**
 *      ����ҳ��
 */
void CGooseShiftDlg::_SavePage()
{
    ASSERT(m_pCurrentCtrlBlk);

    // ���浱ǰҳ��״̬

    CCtrlBlkDataGooseShift* pData = dynamic_cast<CCtrlBlkDataGooseShift*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));

    if(!pData)
    {
        pData = new CCtrlBlkDataGooseShift;
        m_pPageDataExtra->Set(m_pCurrentCtrlBlk->nID, pData);
    }

    pData->m_eMode  = m_eTime;
    pData->m_nChann = m_nChannel;
    pData->m_nFocus = m_reportGoose.GetFocusedRowIndex();
    pData->m_nRows  = m_reportGoose.GetRecordRowCount();
}

/**
 *      �޸Ĳ˵�HMENU
 */
void CGooseShiftDlg::_modifyMenu()
{
    __super::_modifyMenu();

    // ����F3�������ͼ��
    //m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(m_eTime == TIME_REL ? DSM_BMP_M_GS_ABSRELA_RELA : DSM_BMP_M_GS_ABSRELA_ABS), NULL, NULL);
#ifdef ZY_PRJ
	// �޸�F6�ַ���Ϊ�����ˡ�
	m_menuMain.SetMenuItemCaption(5, dsm_stringMgr::Get(DSM_STR_GOOSE_SHIFT_FILTER));
#else
	// �޸�F3�ַ���Ϊ��ͨ��ѡ��
	m_menuMain.SetMenuItemCaption(2, L"ͨ��ѡ��");
	m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);

#endif
    
}

/**
 *      ���ݲ˵�ID�л���ָ���Ŀ��ƿ�
 *
 *  \param[in]
 *      nCmdID          �˵�������ID
 */
bool CGooseShiftDlg::_switchCtrlBlk( UINT nCmdID )
{
    CBaseDetectItem* pPreCtrlBlk = m_pCurrentCtrlBlk;

    if(__super::_switchCtrlBlk(nCmdID))
    {
        // �����л�ǰ���ƿ�ҳ��״̬��Ϣ

        CCtrlBlkDataGooseShift* pData = dynamic_cast<CCtrlBlkDataGooseShift*>(m_pPageDataExtra->Get(pPreCtrlBlk->nID));

        if(!pData)
        {
            pData = new CCtrlBlkDataGooseShift;
            m_pPageDataExtra->Set(m_pCurrentCtrlBlk->nID, pData);
        }

        pData->m_eMode      = m_eTime;
        pData->m_nChann     = m_nChannel;
        pData->m_nRows      = m_reportGoose.GetRecordRowCount();
        pData->m_nFocus     = m_reportGoose.GetFocusedRowIndex();

        m_nShiftIndex       = 0;
        m_nShiftSubIndex    = -1;

        return _restorePage();
    }

    return false;
}

/**
 *      ��ԭҳ������
 */
bool CGooseShiftDlg::_restorePage()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(CGooseShiftDlg::IDD == m_pPageDataExtra->GetPageData()->m_nPageID);

    // ��ȡ��ǰ�Ŀ��ƿ�����
    CCtrlBlkDataGooseShift* pData = dynamic_cast<CCtrlBlkDataGooseShift*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));

    // ��ǰ���ƿ���û�������Ϣ
    // ����Ĭ����ʾ
    if(!pData)
    {
        pData = new CCtrlBlkDataGooseShift;
        m_pPageDataExtra->Set(m_pCurrentCtrlBlk->nID, pData);;
    }

    m_nRows         = pData->m_nRows;
    m_eTime         = pData->m_eMode;
    m_nChannel      = pData->m_nChann;

    _resetF6Channel();

    // ��ʾ��λ��Ϣ
    _filter(m_nChannel + SHIFT_FILTER_FIRST);

    int nCounts = m_reportGoose.GetRecordRowCount();
    if(nCounts > 0)
    {
        if(pData->m_nFocus < nCounts && pData->m_nFocus >= 0 && pData->m_nFocus < pData->m_nRows)
        {
            m_reportGoose.SetFocusedSelectionRowIndex(pData->m_nFocus);
            m_reportGoose.SetFocusedSelectionRowIndex(pData->m_nFocus);
        }
        else
        {
            m_reportGoose.SetFocusedSelectionRowIndex(nCounts - 1);
            m_reportGoose.EnsureRowVisibleScrollRow(nCounts - 1);
        }
    }

    return true;
}

/**
 *      ˢ�½�����ʾ�ؼ�Ԫ�أ������ݣ�
 */
void CGooseShiftDlg::_switchMenu()
{
    // ��λ��ϸ��Ϣ
    if(PAGE_DETAIL == m_ePage)
    {
        if(!m_pMenuDetail)
        {
            CMenu menu;
            menu.LoadMenu(IDR_MENU_GOOSE_SHIFT_DETAIL);

            m_pMenuDetail = new CNaviMenu;
            m_pMenuDetail->LoadMenu(&menu);
            menu.DestroyMenu();
        }

        if(SHIFT_ALL == m_eShift)
        {
           m_pMenuDetail->SetMenuItemCaption(5, dsm_stringMgr::Get(DSM_STR_GOOSE_SHIFT_ONLY_SHIFT));
        }
        else
        {
            m_pMenuDetail->SetMenuItemCaption(5, dsm_stringMgr::Get(DSM_STR_GOOSE_SHIFT_ALL));
        }

        m_pMenuList = m_pNaviBar->GetNaviMenu();
        m_pNaviBar->SetNaviMenu(m_pMenuDetail);
    }
    // ��λ�б�
    else
    {
        if(m_pMenuList)
        {
            m_pNaviBar->SetNaviMenu(m_pMenuList);
        }
    }

    m_pNaviBar->UpdateNaviMenu();
}


/**
 *      ����F6ͨ���˵�
 */
void CGooseShiftDlg::_resetF6Channel()
{
    ASSERT(m_pCurrentCtrlBlk);

    CMenu  menu;
    VERIFY(menu.CreatePopupMenu());

    UINT_PTR nItemID = SHIFT_FILTER_FIRST;
    menu.AppendMenu(MF_STRING, nItemID++, L"ȫ��");

    CString csDesc;
    uint32_t nCounts = m_pCurrentCtrlBlk->nChannelCount;

    //m_dwLast = SHIFT_FILTER_FIRST + nCounts;
    ASSERT(nCounts < SHIFT_FILTER_LAST - SHIFT_FILTER_FIRST - 1);

    for(uint32_t i = 0; i < nCounts; i++)
    {
        csDesc.Format(L"ͨ��%02u", i + 1);
        menu.AppendMenu(MF_STRING, nItemID++, csDesc);
    }

    m_dwLast = nItemID;

    CNaviMenu* pSub = new CNaviMenu;

    pSub->LoadMenu(&menu);
    menu.DestroyMenu();

#ifdef ZY_PRJ
	m_pNaviBar->GetNaviMenu()->SetMenuSubItem(5, pSub);  
	if((uint32_t)m_nChannel > nCounts + 1)
	{
		m_nChannel = 0;
	}

	m_pNaviBar->GetNaviMenu()->GetSubMenu(5)->HighlightMenuItem(m_nChannel, TRUE);
#else
	m_pNaviBar->GetNaviMenu()->SetMenuSubItem(2, pSub); //������ͨ�������˵��ĵ�F3��ť
	if((uint32_t)m_nChannel > nCounts + 1)
	{
		m_nChannel = 0;
	}

	m_pNaviBar->GetNaviMenu()->GetSubMenu(2)->HighlightMenuItem(m_nChannel, TRUE);
#endif
    

    
}


BOOL CGooseShiftDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    // TODO: �ڴ����ר�ô����/����û���

    int nMenuID = LOWORD(wParam);

    if(nMenuID >= SHIFT_FILTER_FIRST && (DWORD)nMenuID < m_dwLast)
    {
        // ����F6ͨ�����˹���

        // ����
        if(m_nChannel != nMenuID - SHIFT_FILTER_FIRST)
        {
            _filter(nMenuID);
        }

        return TRUE;
    }

    return CGooseBaseDlg::OnCommand(wParam, lParam);
}


/**
 *      ����ͨ��
 *
 *  \param[in]
 *      nItem       ͨ���Ŷ�Ӧ�Ĳ˵�ID
 */
void CGooseShiftDlg::_filter( int nItem )
{
#ifdef ZY_PRJ
    // ȡ����һ���������Ĳ˵�
    CNaviBarMgr::GetInstance()->HighlightMenuItem(5, m_nChannel, FALSE);
    m_nChannel = nItem - SHIFT_FILTER_FIRST;
    // �����˵�
    CNaviBarMgr::GetInstance()->HighlightMenuItem(5, m_nChannel);
#else
	// ȡ����һ���������Ĳ˵�
	CNaviBarMgr::GetInstance()->HighlightMenuItem(2, m_nChannel, FALSE);
	m_nChannel = nItem - SHIFT_FILTER_FIRST;
	// �����˵�
	CNaviBarMgr::GetInstance()->HighlightMenuItem(2, m_nChannel);
#endif

    m_reportGoose.DeleteAllRecordRow();

    // ���±�����Ϊ�б�
    m_bRecounter = true;

    // ���±���
    _updateData();
}

BOOL CGooseShiftDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    // TODO: �ڴ����ר�ô����/����û���

    if(nCode == CN_UPDATE_COMMAND_UI && nID >= SHIFT_FILTER_FIRST && nID < m_dwLast)
    {
        CCmdUI* pCmdUI = (CCmdUI*)pExtra;
        ASSERT(pCmdUI);

        pCmdUI->Enable(TRUE);

        return TRUE;
    }

    return CGooseBaseDlg::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


/**
 *      �л���ʾ����ʱ�������ʱ��
 */
void CGooseShiftDlg::_switchAbsoRel()
{
    CString csTime;
    CGooseOneTimeChange info;
    CGooseInfo* pInfoBef        = NULL;
    CGooseInfo* pInfoAft        = NULL;
    uint32_t nCounts            = m_reportGoose.GetRecordRowCount();

    for(uint32_t i = 0; i < nCounts; i++)
    {
        // ��ȡ��λ��Ϣ
        info = m_data.GetGooseChangeInfo(i);

        pInfoBef = info.GetDataBeforeChange();
        pInfoAft = info.GetDataAfterChange();

        ASSERT(pInfoBef && pInfoAft);

        // ���ʱ��
        if(TIME_REL == m_eTime)
        {
            csTime = DSM_GetDValue(pInfoAft->GetEventTime().t_s, pInfoAft->GetEventTime().t_ns, pInfoBef->GetEventTime().t_s, pInfoBef->GetEventTime().t_ns);
        }
        // ����ʱ��
        else
        {
            UTC_TIME_t ut = {pInfoAft->GetEventTime().t_s, pInfoAft->GetEventTime().t_ns / 1000};
            csTime = DM_GetSystemTimeStrFromUTC(ut);
        }

        m_reportGoose.SetRowItemText(i, 1, csTime);
    }
}

BOOL CGooseShiftDlg::PreTranslateMessage( MSG* pMsg )
{
    if(PAGE_DETAIL == m_ePage)
    {
        if(WM_KEYDOWN == pMsg->message)
        {
            switch(pMsg->wParam)
            {
            case VK_ESCAPE:
                m_reportDetail.ShowWindow(SW_HIDE);
                m_reportGoose.ShowWindow(SW_SHOW);

                m_reportGoose.SetFocus();

                if(m_nLastFocus < m_reportGoose.GetRecordRowCount())
                {
                    m_reportGoose.SetFocusedSelectionRowIndex(m_nLastFocus);
                }
                else if(m_reportGoose.GetRecordRowCount() > 0)
                {
                    m_reportGoose.SetFocusedSelectionRowIndex(0);
                }

                m_ePage = PAGE_LIST;
                _switchMenu();

                return TRUE;

            case VK_UP:
                _channelDetailUp();
                break;

            case  VK_DOWN:
                _channelDetailDown();
                break;

            default:
                break;
            }
        }
    }

    return CGooseBaseDlg::PreTranslateMessage(pMsg);
}

void CGooseShiftDlg::OnMenuShiftF6()
{
    if(PAGE_LIST == m_ePage)
    {
        return;
    }

    m_eShift = m_eShift == SHIFT_ALL ? SHIFT_ONLY : SHIFT_ALL;

    // �л���ʾȫ����λ
    if(SHIFT_ALL == m_eShift)
    {
        m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(5, dsm_stringMgr::Get(DSM_STR_GOOSE_SHIFT_ONLY_SHIFT));
        m_pNaviBar->UpdateNaviMenu();

        _updateDetail();
    }
    // �л�����λ��Ϣ
    else
    {
        m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(5, dsm_stringMgr::Get(DSM_STR_GOOSE_SHIFT_ALL));
        m_pNaviBar->UpdateNaviMenu();

        _updateShiftDetail();
    }
}


/**
 *      ��ʾ��ϸ��λ��Ϣ
 */
void CGooseShiftDlg::_showDetail( )
{
    m_ePage = PAGE_DETAIL;

    // ��ʾ��ϸ��λ��Ϣ
    if(!m_reportDetail.GetSafeHwnd())
    {
        _createReportDetail();
    }

    _switchMenu();

    m_reportGoose.ShowWindow(SW_HIDE);
    m_reportDetail.ShowWindow(SW_SHOW);

    m_reportDetail.SetFocus();

    // ��ʾָ����λ��Ϣ
    _updateDetail();
}


/**
 *      ������ϸ��λ��Ϣ
 */
void CGooseShiftDlg::_updateDetail()
{
    CGooseOneTimeChange change;

    if(!_getChange(m_nDetailIndex, change))
    {
        _fillDetailReport(L"");
        ASSERT(FALSE);
        return;
    }

    CGooseInfo* pBefore = change.GetDataBeforeChange();
    CGooseInfo* pAfter  = change.GetDataAfterChange();

    if(!pBefore || !pAfter || pBefore->GetChannelCounts() != pAfter->GetChannelCounts())
    {
        _fillDetailReport(g_csBlankValue);
        ASSERT(FALSE);
        return;
    }

    uint32_t nChann             = m_nParentChann;   // ��һ�м�¼��ͨ�����
    int nSubChann               = m_nSubChann;      // ��һ�м�¼ͨ����ŵ���ͨ�����
    CGooseChannel* pChannBef    = NULL;
    CGooseChannel* pChannAft    = NULL;

    for(int i = 0; i < m_nRows; nChann++, nSubChann = -1)
    {
        pChannBef = pBefore->GetChanneData(nChann);
        pChannAft = pAfter->GetChanneData(nChann);

        if(!pChannAft || !pChannBef || pChannAft->GetChannelType() != pChannBef->GetChannelType() 
            || pChannAft->GetSubChannelCount() != pChannBef->GetSubChannelCount())
        {
            CString csText = g_csBlankValue;

            if(nChann >= pBefore->GetChannelCounts())
            {
                csText = L"";
            }

            m_reportDetail.SetRowItemText(i, 0, csText);
            m_reportDetail.SetRowItemText(i, 1, csText);
            m_reportDetail.SetRowItemText(i, 2, csText);
            m_reportDetail.SetRowItemText(i, 3, csText);

            i++;

            continue;
        }

        // �����ͨ�����Ϊ-1������븸ͨ�����ṹ���ͣ�
        if(-1 == nSubChann)
        {
            if(_insertRecord(pChannBef, pChannAft, nChann, nSubChann, i))
            {
                i++;
            }
        }

        // ��Ϊ��ͨ�����ṹ���ͣ�����ö����ͨ��
        if(pChannAft->GetChannelData()->type == GSEDT_STRUCT && i < m_nRows)
        {
            uint32_t nCounts        = pChannAft->GetSubChannelCount();      // ��ͨ������
            nSubChann               = (nSubChann == -1) ? 0 : nSubChann;    // ����ö����ͨ����λ��

            // ö�ٲ�����ͨ��
            for(uint32_t j = nSubChann; j < nCounts && i < m_nRows; j++)
            {
                if(_insertRecord(pChannBef->GetSubChannelData(j), pChannAft->GetSubChannelData(j), nChann, j, i))
                {
                    i++;
                }
            }
        }
    }
}


/**
 *      ����һ����ϸ��λ��¼
 *
 *  \param[in]
 *      pChanBef        ��λǰͨ��
 *  \param[in]
 *      pChannAft       ��λ��ͨ��
 *  \param[in]
 *      nChannel        ͨ����
 *  \param[in]
 *      nSubChannel     ��ͨ����
 *  \param[in]
 *      nRow            �����к�
 */
bool CGooseShiftDlg::_insertRecord( CGooseChannel* pChanBef, CGooseChannel* pChannAft, int nChannel, int nSubChannel, int nRow )
{
    ASSERT(pChannAft && pChanBef);

    GOOSE_DATA* pGsDataBef      = pChanBef->GetChannelData();
    GOOSE_DATA* pGsDataAft      = pChannAft->GetChannelData();

    if(!pGsDataBef || !pGsDataAft)
    {
        DSM_DEBUG(L"dsm: goose shift detal: null goose data.\n");
        ASSERT(FALSE);
        return false;
    }

    CString csIndex, csType, csValBef, csValAft;

    // ��ʽ������
    if(pChanBef->GetParentChannel())
    {
        csIndex.Format(L"%d.%d", nChannel + 1, nSubChannel + 1);
    }
    else
    {
        csIndex.Format(L"%d", nChannel + 1);
    }

    // ��ȡgoose������Ϣ
    DSM_GetGooseDataInfo(pChanBef, csType, csValBef);
    DSM_GetGooseDataInfo(pChannAft, csType, csValAft);

    m_reportDetail.SetRowItemText(nRow, 0, csIndex);
    m_reportDetail.SetRowItemText(nRow, 1, csType);
    m_reportDetail.SetRowItemText(nRow, 2, csValBef);
    m_reportDetail.SetRowItemText(nRow, 3, csValAft);

    return true;
}

/**
 *      �������Ϸ�ҳ�ڵ�һ����ʾ��ͨ����ź���ͨ�����
 *
 *  \return
 *      bool            ������ҳ����true�����򷵻�false
 */
bool CGooseShiftDlg::_calcUp()
{
    uint32_t nCount = m_data.GetChannelCounts();

    CGooseOneTimeChange change;

    // ���ұ�λ��Ϣ
    uint32_t nCounts = m_data.GetChangeTimes();
    for(uint32_t i = 0; i < nCounts; i++)
    {
        change = m_data.GetGooseChangeInfo(i);

        if(change.GetChangeIndex() == m_nDetailIndex)
        {
            break;
        }
    }

    if(nCount == 0 || !change.GetDataBeforeChange() || !change.GetDataAfterChange())
    {
        return false;
    }

    int nRow                    = 0;                    // �ۼ���ʾ������
    int nChan                   = m_nParentChann;       // ��ǰ��ʾͨ������
    GOOSE_DATA* pGooseData      = NULL;
    GOOSE_STRUCT* pGooseSt      = NULL;
    CGooseChannel* pChannel     = change.GetDataBeforeChange()->GetChanneData(nChan);

    ASSERT(pChannel);

    while(pChannel)
    {
        pGooseData = pChannel->GetChannelData();
        ASSERT(pGooseData);

        if(GSEDT_STRUCT == pGooseData->type)
        {
            pGooseSt        = (GOOSE_STRUCT*)pGooseData;
            size_t nSize    = pGooseSt->member.size();

            if(nChan == m_nParentChann)
            {
                // m_nSubChannΪδ��ʾ����ͨ��
                // ���m_nSubChann֮ǰ��ͨ��δ��ʾ������
                nRow = nRow + m_nSubChann + 1;
            }
            else
            {
                // ��ͨ��ȫ��δ��ʾnsize����ͨ��+1�Ǹ�ͨ������
                nRow = nRow + nSize + 1;
            }

            // ����ʾ��ͨ��������ÿҳ�����ʾͨ����
            // ��Ҫ����ĳ�����ӣ�ͨ��������������ʾΪ��һ��
            if(nRow >= m_nRows)
            {
                m_nParentChann      = nChan;                    // ĳͨ������
                m_nSubChann         = nRow - m_nRows - 1;       // ����ͨ�������������������

                return true;
            }
        }
        else
        {
            if(nChan != m_nParentChann)
            {
                nRow++;
            }

            // ����ʾ��ͨ��
            if(nRow == m_nRows)
            {
                m_nParentChann      = nChan;
                m_nSubChann         = -1;

                return true;
            }
        }

        // ö����һ��ͨ��
        pChannel = change.GetDataBeforeChange()->GetChanneData(--nChan);
    }

    return false;
}
 
/**
 *      �������·�ҳ�ڵ�һ����ʾ��ͨ����ź���ͨ�����
 *
 *  \return
 *      bool            ������ҳ����true�����򷵻�false
 */
bool CGooseShiftDlg::_calcDown()
{
    uint32_t nCount = m_data.GetChannelCounts();

    CGooseOneTimeChange change;

    // ���ұ�λ��Ϣ
    uint32_t nCounts = m_data.GetChangeTimes();
    for(uint32_t i = 0; i < nCounts; i++)
    {
        change = m_data.GetGooseChangeInfo(i);

        if(change.GetChangeIndex() == m_nDetailIndex)
        {
            break;
        }
    }

    if(nCount == 0 || !change.GetDataBeforeChange() || !change.GetDataAfterChange())
    {
        return false;
    }

    int nRow                    = 0;                // �ۼ���ʾ������
    int nChan                   = m_nParentChann;   // ��ǰ�����ͨ������
    int nSubChan                = m_nSubChann;      // ��ǰ����ͨ������ͨ������
    GOOSE_DATA* pGooseData      = NULL;
    GOOSE_STRUCT* pGooseSt      = NULL;
    CGooseChannel* pChannel     = change.GetDataBeforeChange()->GetChanneData(nChan);

    ASSERT(pChannel);

    while(pChannel)
    {
        pGooseData = pChannel->GetChannelData();
        ASSERT(pGooseData);

        // ��ǰͨ���ǽṹ�����ͣ�����ͨ�����ͣ�
        if(GSEDT_STRUCT == pGooseData->type)
        {
            pGooseSt        = (GOOSE_STRUCT*)pGooseData;
            size_t nSize    = pGooseSt->member.size();

            // �����ͨ������ʾ������
            nRow = nRow + nSize - nSubChan;

            // �ж����һҳ�����
            if(nRow == m_nRows && !change.GetDataBeforeChange()->GetChanneData(nChan + 1))
            {
                return false;
            }

            // �����ͨ���ڱ�ҳ����ʾ���꣬����һҳ������ʾ��ͨ��
            if(nRow >= m_nRows)
            {
                m_nParentChann  = nRow > m_nRows ? nChan : nChan + 1;
                m_nSubChann     = nRow > m_nRows ? nSize - (nRow - m_nRows) : -1;

                return true;
            }
        }
        else
        {
            nRow++;
            if(nRow > m_nRows)
            {
                m_nParentChann  = nChan;
                m_nSubChann     = nSubChan;

                return true;
            }
        }

        // ��ͨ���ڱ�ҳ�ܱ���ȫ��ʾ����ö����һͨ�������ָ���ͨ������Ϊ-1
        nSubChan = -1;

        pChannel = change.GetDataBeforeChange()->GetChanneData(++nChan);
    }

    return false;
}

/**
 *      ͨ����ҳ - ��һҳ
 */
void CGooseShiftDlg::_channelDetailUp()
{
    if(m_eShift == SHIFT_ALL)
    {
        if(_calcUp())
        {
            _updateDetail();
        }
    }
    else if(m_eShift == SHIFT_ONLY)
    {
        if(_calcUpOnly())
        {
            _updateShiftDetail();
        }
    }
}

/**
 *      ͨ����ҳ - ��һҳ
 */
void CGooseShiftDlg::_channelDetailDown()
{
    if(m_eShift == SHIFT_ALL)
    {
        if(_calcDown())
        {
            _updateDetail();
        }
    }
    else if(m_eShift == SHIFT_ONLY)
    {
        if(_calcDownOnly())
        {
            _updateShiftDetail();
        }
    }
}

/**
 *      ���㵥����λ��Ϣ�Ϸ�ҳ
 *
 *  \return
 *      bool        ������ҳ����true�����򷵻�false
 */
bool CGooseShiftDlg::_calcUpOnly()
{
    CGooseOneTimeChange change;

    if(!_getChange(m_nDetailIndex, change) || !change.GetDataBeforeChange() || !change.GetDataAfterChange())
    {
        return false;
    }

    int nRow                    = 0;                    // �ۼ���ʾ������
    uint32_t nShift             = m_nShiftIndex;        // ��ǰ��ʾ��λ����
    uint32_t nFirstIndex        = change.GetChangedChannelIndex(m_nShiftIndex);
    uint32_t nChanIndex         = nFirstIndex;
    CGooseChannel* pChanBef     = change.GetDataBeforeChange()->GetChanneData(nChanIndex);
    CGooseChannel* pChanAft     = change.GetDataAfterChange()->GetChanneData(nChanIndex);
    GOOSE_DATA* pDataBef        = NULL;
    GOOSE_DATA* pDataAft        = NULL;

    while(pChanBef || pChanAft)
    {
        pDataBef = pChanBef->GetChannelData();
        pDataAft = pChanAft->GetChannelData();
        ASSERT(pDataBef && pDataAft &&  pDataBef->type == pDataAft->type);

        // ����֮һ��struct
        if(GSEDT_STRUCT == pDataBef->type)
        {
            size_t nSize = ((GOOSE_STRUCT*)pDataBef)->member.size();

            if(nChanIndex == nFirstIndex)
            {
                // m_nSubChannΪδ��ʾ����ͨ��
                // ���m_nSubChann֮ǰ��ͨ��δ��ʾ������
                nRow = nRow + m_nShiftSubIndex + 1;
            }
            else
            {
                // ��ͨ��ȫ��δ��ʾnsize����ͨ��+1�Ǹ�ͨ������
                nRow = nRow + nSize + 1;
            }

            // ����ʾ��ͨ��������ÿҳ�����ʾͨ����
            // ��Ҫ����ĳ�����ӣ�ͨ��������������ʾΪ��һ��
            if(nRow >= m_nRows)
            {
                m_nShiftIndex       = nShift;                   // ĳ��λ����
                m_nShiftSubIndex    = nRow - m_nRows - 1;       // ����ͨ�������������������

                return true;
            }
        }
        else
        {
            if(nChanIndex != nFirstIndex)
            {
                nRow++;
            }

            // ����ʾ��ͨ��
            if(nRow == m_nRows)
            {
                m_nShiftIndex       = nShift;
                m_nShiftSubIndex    = -1;

                return true;
            }
        }

        // ö����һ����λͨ��

        nChanIndex = change.GetChangedChannelIndex(--nShift);

        if(nChanIndex >= 0)
        {
            pChanBef = change.GetDataBeforeChange()->GetChanneData(nChanIndex);
            pChanAft = change.GetDataAfterChange()->GetChanneData(nChanIndex);
        }
        else
        {
            return false;
        }
    }

    return false;
}

/**
 *      ���㵥����λ��Ϣ�·�ҳ
 *
 *  \return
 *      bool        ������ҳ����true�����򷵻�false
 */
bool CGooseShiftDlg::_calcDownOnly()
{
    CGooseOneTimeChange change;

    if(!_getChange(m_nDetailIndex, change) || !change.GetDataBeforeChange() || !change.GetDataAfterChange())
    {
        return false;
    }

    int nRow                    = 0;                    // �ۼ���ʾ������
    uint32_t nShift             = m_nShiftIndex;        // ��ǰ��ʾ��λ����
    uint32_t nFirstIndex        = change.GetChangedChannelIndex(m_nShiftIndex);
    uint32_t nChanIndex         = nFirstIndex;
    int nChanSubIndex           = m_nShiftSubIndex;
    CGooseChannel* pChanBef     = change.GetDataBeforeChange()->GetChanneData(nChanIndex);
    CGooseChannel* pChanAft     = change.GetDataAfterChange()->GetChanneData(nChanIndex);
    GOOSE_DATA* pDataBef        = NULL;
    GOOSE_DATA* pDataAft        = NULL;

    while(pChanBef || pChanAft)
    {
        pDataBef = pChanBef->GetChannelData();
        pDataAft = pChanAft->GetChannelData();
        ASSERT(pDataBef && pDataAft && pDataAft->type == pDataBef->type);

        // ����֮һ��struct
        if(GSEDT_STRUCT == pDataBef->type)
        {
            size_t nSize = ((GOOSE_STRUCT*)pDataBef)->member.size();

            // �����ͨ������ʾ������
            nRow = nRow + nSize - nChanSubIndex;

            // �ж����һҳ�����
            if(nRow == m_nRows && !change.GetDataBeforeChange()->GetChanneData(nChanIndex + 1))
            {
                return false;
            }

            // �����ͨ���ڱ�ҳ����ʾ���꣬����һҳ������ʾ��ͨ��
            if(nRow >= m_nRows)
            {

                m_nShiftIndex       = nRow > m_nRows ? nShift : nShift + 1;
                m_nShiftSubIndex    = nRow > m_nRows ? nSize - (nRow - m_nRows) : -1;

                return true;
            }
        }
        else
        {
            nRow++;
            if(nRow > m_nRows)
            {
                m_nShiftIndex       = nShift;
                m_nShiftSubIndex    = nChanSubIndex;

                return true;
            }
        }

        nChanSubIndex = -1;

        // ö����һ����λͨ��

        nChanIndex = change.GetChangedChannelIndex(++nShift);

        if(nChanIndex < change.GetChangedChannelCounts())
        {
            pChanBef = change.GetDataBeforeChange()->GetChanneData(nChanIndex);
            pChanAft = change.GetDataAfterChange()->GetChanneData(nChanIndex);
        }
        else
        {
            return false;
        }
    }

    return false;
}

/**
 *      ��ʾ������λ��Ϣ
 */
void CGooseShiftDlg::_updateShiftDetail()
{
    CGooseOneTimeChange change;

    bool bFind          = _getChange(m_nDetailIndex, change);
    uint32_t nCounts    = change.GetChangedChannelCounts();

    // �Ҳ�����λ��Ϣ���߱�λ�������ڱ�λ����
    if(!bFind || m_nShiftIndex >= nCounts)
    {
        _fillDetailReport(L"");
        ASSERT(FALSE);
        return;
    }

    CGooseInfo* pBefore = change.GetDataBeforeChange();
    CGooseInfo* pAfter  = change.GetDataAfterChange();
    ASSERT(pBefore && pAfter);

//     // ͨ����������쳣
//     if(pBefore->GetChannelCounts() != pAfter->GetChannelCounts())
//     {
//         _fillDetailReport(g_csBlankValue);
//         ASSERT(FALSE);
//         return;
//     }

    uint32_t nIndex             = m_nShiftIndex;            // ��һ�б�λͨ������
    int nChannelIndex           = -1;                       // ��һ�б�λͨ����
    int nChannelSubIndex        = m_nShiftSubIndex;         // ��һ�б�λ��ͨ����
    CGooseChannel* pChannBef    = NULL;
    CGooseChannel* pChannAft    = NULL;

    for(int i = 0; i < m_nRows;)
    {
        nChannelIndex = change.GetChangedChannelIndex(nIndex++);

        if(nChannelIndex == -1)
        {
            m_reportDetail.SetRowItemText(i, 0, L"");
            m_reportDetail.SetRowItemText(i, 1, L"");
            m_reportDetail.SetRowItemText(i, 2, L"");
            m_reportDetail.SetRowItemText(i, 3, L"");
            i++;

            continue;
        }

        pChannBef = pBefore->GetChanneData(nChannelIndex);
        pChannAft = pAfter->GetChanneData(nChannelIndex);

        // �����ͨ�����Ϊ-1������븸ͨ�����ṹ���ͣ�
        if(nChannelSubIndex == -1)
        {
            _insertRecord(pChannBef, pChannAft, nChannelIndex, nChannelSubIndex, i++);
        }

        // ��Ϊ��ͨ�����ṹ���ͣ�����ö����ͨ��
        if(pChannAft->GetChannelData()->type == GSEDT_STRUCT && i < m_nRows)
        {
            uint32_t nSubCounts     = pChannAft->GetSubChannelCount();                      // ��ͨ������
            nChannelSubIndex        = (nChannelSubIndex == -1) ? 0 : nChannelSubIndex;      // ����ö����ͨ����λ��

            // ö�ٲ�����ͨ��
            for(uint32_t j = nChannelSubIndex; j < nSubCounts && i < m_nRows; j++)
            {
                _insertRecord(pChannBef->GetSubChannelData(j), pChannAft->GetSubChannelData(j), nChannelIndex, j, i++);
            }
        }
    } // for
}

/**
 *      ��ȡָ�������ı�λ����
 *
 *  \param[in]
 *      nIndex      ��λ��������
 *  \param[out]
 *      change      ��λ����
 *  \return
 *      bool        ��ȡ���ݳɹ�����true��ʧ�ܷ���false
 */
bool CGooseShiftDlg::_getChange( uint32_t nIndex, CGooseOneTimeChange& change )
{
    // ���ұ�λ��Ϣ

    uint32_t nCounts    = m_data.GetChangeTimes();

    for(uint32_t i = 0; i < nCounts; i++)
    {
        change = m_data.GetGooseChangeInfo(i);

        if(change.GetChangeIndex() == nIndex)
        {
            return true;
        }
    }

    return false;
}


/**
 *      ���detal����
 *
 *  \param[in]
 *      csContent       ����ַ�������
 */
void CGooseShiftDlg::_fillDetailReport( CString csContent )
{
    // ��ձ���
    for(int i = 0; i < m_nRows; i++)
    {
        m_reportDetail.SetRowItemText(i, 0, csContent);
        m_reportDetail.SetRowItemText(i, 1, csContent);
        m_reportDetail.SetRowItemText(i, 2, csContent);
        m_reportDetail.SetRowItemText(i, 3, csContent);
    }
}


