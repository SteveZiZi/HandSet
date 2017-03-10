/**
 * \file
 *      GooseStatisticsDlg.cpp
 *
 * \brief
 *      goose����ͳ��ҳ��ʵ���ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/5/10
 */

// GooseStatisticsDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "GooseStatisticsDlg.h"

#include "src/main/pagedata/pagedata.h"
#include "src/utils/utility/dsm_utility.h"

//////////////////////////////////////////////////////////////////////////
// goose - ����ͳ��
//
#define DSM_GOOSE_STATISTICS_HEAD_SUB       155             ///< ������
#define DSM_GOOSE_STATISTICS_HEAD_VALUE     110             ///< ֵ��

#define DSM_GOOSE_STATISTICS_RC_TOP         CRect(0, 0, 240, 25)



/**
 *      goose���� - ����ͳ��
 *
 *  \note
 *      �ӱ��⡢ֵ
 */
class CDsmGooseStatisticsRecord : public CELRecord
{
public:
    CDsmGooseStatisticsRecord(const CString csSubCaption1, const CString csSubCaption2)
    {
        AddItem(new CDsmRecordItem(csSubCaption1));
        AddItem(new CDsmRecordItem(L""));
        AddItem(new CDsmRecordItem(csSubCaption2));
        AddItem(new CDsmRecordItem(L""));
    }

    virtual ~CDsmGooseStatisticsRecord() {}
};



// CGooseStatisticsDlg �Ի���

IMPLEMENT_DYNAMIC(CGooseStatisticsDlg, DSM_GOOSEASEDLG)

CGooseStatisticsDlg::CGooseStatisticsDlg(CWnd* pParent /*=NULL*/)
	: DSM_GOOSEASEDLG(CGooseStatisticsDlg::IDD, DSM_STR_GOOSE_STATISTICS_CAPTION, pParent)
{

}

CGooseStatisticsDlg::~CGooseStatisticsDlg()
{

}

void CGooseStatisticsDlg::DoDataExchange(CDataExchange* pDX)
{
	DSM_GOOSEASEDLG::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CGooseStatisticsDlg, DSM_GOOSEASEDLG)
    ON_WM_TIMER()
    ON_COMMAND(ID_MENU_GOOSE_F3, &CGooseStatisticsDlg::OnMenuF3)
    ON_COMMAND(ID_MENU_GOOSE_F6, &CGooseStatisticsDlg::OnMenuF6)
END_MESSAGE_MAP()

// CGooseStatisticsDlg ��Ϣ�������

BOOL CGooseStatisticsDlg::OnInitDialog()
{
    CGooseBaseDlg::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��

    _createReport();

    if(m_pCurrentCtrlBlk)
    {
        m_pDataLayer->SetCurrentMode(DSM_MODE_GOOSE_RECV_FRAME_STATIC);

        _beginTimer();
    }

    // ����ͳ���쳣��Ϣ
    m_pDataLayer->ResetGooseFrameStatistics();

    return FALSE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}


/**
 *      �������ļ��ӱ���
 */
void CGooseStatisticsDlg::_createReport()
{
    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    m_reportGoose.Create(DSM_WINSTYLE_REPORT_DEFAULT, rcReport, this, IDC_GOOSE_STATISTICS_REPORT);

    // ��������
    m_reportGoose.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    // ������ʽ
    m_reportGoose.PostInitTemplateStyle();

    // ������Ԫ��
    m_reportGoose.InsertColumn(0, dsm_stringMgr::Get(DSM_STR_GOOSE_STATISTICS_HEAD_CAP), EL_LEFT, DSM_GOOSE_STATISTICS_HEAD_SUB);          // �ӱ���
    m_reportGoose.InsertColumn(1, dsm_stringMgr::Get(DSM_STR_GOOSE_STATISTICS_HEAD_VAL), EL_LEFT, DSM_GOOSE_STATISTICS_HEAD_VALUE);        // ֵ
    m_reportGoose.InsertColumn(2, dsm_stringMgr::Get(DSM_STR_GOOSE_STATISTICS_HEAD_CAP), EL_LEFT, DSM_GOOSE_STATISTICS_HEAD_SUB);          // �ӱ���
    m_reportGoose.InsertColumn(3, dsm_stringMgr::Get(DSM_STR_GOOSE_STATISTICS_HEAD_VAL), EL_LEFT, DSM_GOOSE_STATISTICS_HEAD_VALUE);        // ֵ

    // ����ѡ�񽹵���ʽ
    m_reportGoose.SetRowSelectionVisible(FALSE);
    m_reportGoose.SetRowFocusEnable(FALSE);

    m_reportGoose.InsertRecordRow(0, new CDsmGooseStatisticsRecord(dsm_stringMgr::Get(DSM_STR_GOOSE_STATISTICS_TOTAL), dsm_stringMgr::Get(DSM_STR_GOOSE_STATISTICS_LOSTDATA)));
    m_reportGoose.InsertRecordRow(1, new CDsmGooseStatisticsRecord(dsm_stringMgr::Get(DSM_STR_GOOSE_STATISTICS_LOSTFRAME), dsm_stringMgr::Get(DSM_STR_GOOSE_STATISTICS_COMMBREAK)));
    m_reportGoose.InsertRecordRow(2, new CDsmGooseStatisticsRecord(dsm_stringMgr::Get(DSM_STR_GOOSE_STATISTICS_MISORDER), dsm_stringMgr::Get(DSM_STR_GOOSE_STATISTICS_INVALIDTTL)));
    m_reportGoose.InsertRecordRow(3, new CDsmGooseStatisticsRecord(dsm_stringMgr::Get(DSM_STR_GOOSE_STATISTICS_RETRY), dsm_stringMgr::Get(DSM_STR_GOOSE_STATISTICS_TIMENOTSYNC)));
    m_reportGoose.InsertRecordRow(4, new CDsmGooseStatisticsRecord(dsm_stringMgr::Get(DSM_STR_GOOSE_STATISTICS_STLOST), dsm_stringMgr::Get(DSM_STR_GOOSE_STATISTICS_TIMEFAULT)));
    m_reportGoose.InsertRecordRow(5, new CDsmGooseStatisticsRecord(dsm_stringMgr::Get(DSM_STR_GOOSE_STATISTICS_STMIS), L""));

    m_reportGoose.SetFocus();
}


void CGooseStatisticsDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    if(nIDEvent == m_nTimerID)
    {
        _updateData();
    }

    CGooseBaseDlg::OnTimer(nIDEvent);
}


/**
 *      ���±���ͳ����Ϣ
 */
void CGooseStatisticsDlg::_updateData()
{
    ASSERT(m_pDataLayer);
    ASSERT(m_pCurrentCtrlBlk);

    // ������
    if(m_pDataLayer->GetGooseFrameStatistics(&m_data))
    {
        m_csTotal.Format(L"%u", m_data.GetTotalFrameCounts());
        m_csLostFrame.Format(L"%u", m_data.GetLostFrameCounts());
        m_csMisOrder.Format(L"%u", m_data.GetMisOrderFrameCount());
        m_csRetry.Format(L"%u", m_data.GetRetryFrameCount());
        m_csStLost.Format(L"%u", m_data.GetStLostFrameCounts());
        m_csStMis.Format(L"%u", m_data.GetStMisOrderFrameCount());
        m_csLostData.Format(L"%u", m_data.GetLostDataGramCount());
        m_csCommBreak.Format(L"%u", m_data.GetCommBreakCount());
        m_csInvalidTTL.Format(L"%u", m_data.GetInvalidTTLCount());
        m_csTimeNotSync.Format(L"%u", m_data.GetTimeQualityNotSyncCount());
        m_csTimeFault.Format(L"%u", m_data.GetTimeQualityFaultCount());
    }
    // ������
    else
    {
        m_csTotal       = g_csBlankValue;
        m_csLostFrame   = g_csBlankValue;
        m_csMisOrder    = g_csBlankValue;
        m_csRetry       = g_csBlankValue;
        m_csStLost      = g_csBlankValue;
        m_csStMis       = g_csBlankValue;
        m_csLostData    = g_csBlankValue;
        m_csCommBreak   = g_csBlankValue;
        m_csInvalidTTL  = g_csBlankValue;
        m_csTimeNotSync = g_csBlankValue;
        m_csTimeFault   = g_csBlankValue;
    }

    m_reportGoose.SetRowItemText(0, 1, m_csTotal);
    m_reportGoose.SetRowItemText(1, 1, m_csLostFrame);
    m_reportGoose.SetRowItemText(2, 1, m_csMisOrder);
    m_reportGoose.SetRowItemText(3, 1, m_csRetry);
    m_reportGoose.SetRowItemText(4, 1, m_csStLost);
    m_reportGoose.SetRowItemText(5, 1, m_csStMis);

    m_reportGoose.SetRowItemText(0, 3, m_csLostData);
    m_reportGoose.SetRowItemText(1, 3, m_csCommBreak);
    m_reportGoose.SetRowItemText(2, 3, m_csInvalidTTL);
    m_reportGoose.SetRowItemText(3, 3, m_csTimeNotSync);
    m_reportGoose.SetRowItemText(4, 3, m_csTimeFault);
}


/**
 *      ����ҳ��
 */
void CGooseStatisticsDlg::_SavePage()
{

}

void CGooseStatisticsDlg::OnMenuF3()
{
#ifdef ZY_PRJ
	
#else
	ASSERT(m_pDataLayer);

	// ����ͳ��
	m_pDataLayer->ResetGooseFrameStatistics();

	// ����ˢ�½���
	_updateData();
#endif
}

void CGooseStatisticsDlg::OnMenuF6()
{
#ifdef ZY_PRJ
    ASSERT(m_pDataLayer);

    // ����ͳ��
    m_pDataLayer->ResetGooseFrameStatistics();

    // ����ˢ�½���
    _updateData();
#else
#endif
}


/**
 *      �޸ĵײ��˵�
 */
void CGooseStatisticsDlg::_modifyMenu()
{
     
#ifdef ZY_PRJ
	__super::_modifyMenu();

	// ����ʾF3����
	m_menuMain.SetMenuItemCaption(2, L"");

	// ����F6���ַ��� ������ͳ�ơ�
	m_menuMain.SetMenuSubItem(5, NULL);
	m_menuMain.SetMenuItemId(5, ID_MENU_GOOSE_F6);
	m_menuMain.SetMenuItemCaption(5, dsm_stringMgr::Get(DSM_STR_GOOSE_STATISTICS_RESET));
#else
	__super::_modifyMenu();

	// ����ʾF3����
	//m_menuMain.SetMenuItemCaption(2, L"");

	// ����F3���ַ��� ������ͳ�ơ�
	m_menuMain.SetMenuSubItem(2, NULL);
	m_menuMain.SetMenuItemId(2, ID_MENU_GOOSE_F3);
	m_menuMain.SetMenuItemCaption(2, L"�쳣ͳ��");//dsm_stringMgr::Get(DSM_STR_GOOSE_STATISTICS_RESET));
#endif
}
