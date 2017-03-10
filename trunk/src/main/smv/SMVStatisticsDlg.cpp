/**
 * \file
 *      SMVStatisticsDlg.cpp
 *
 * \brief
 *      ����ͳ������ɢ��ʵ���ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/7/11
 */

// SMVStatisticsDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SMVStatisticsDlg.h"

#include "src/main/eventproc.h"
#include "src/main/mgr/statusmgr.h"
#include "src/main/pagedata/pagedata.h"
#include "src/utils/utility/dsm_utility.h"



//////////////////////////////////////////////////////////////////////////
// smv - ����ͳ��
//
#if SU_FEI
#define DSM_SMV_STATISTICS_HEAD_SUB         120
#define DSM_SMV_STATISTICS_HEAD_VALUE       100

#define DSM_SVM_STATISTICS_RC_TOP           CRect(0, 0, 640, ELT_T1_HDRHEIGHT)
#define DSM_SMV_STATISTICS_RC_TOP1          CRect(0, 0, 240, ELT_T1_HDRHEIGHT)
#define DSM_SMV_STATISTICS_RC_TOP2          CRect(261, 0, 640, ELT_T1_HDRHEIGHT)
//#define DSM_SMV_STATISTICS_RC_LINE          CRect(240, 0, 241, ELT_T1_HDRHEIGHT)
#define DSM_SMV_STATISTICS_RC_REPROT        CRect(0, 25, 240, 414)
//#define DSM_SMV_STATISTICS_RC_CTRL          CRect(241, 25, 640, 414)

#else

#define DSM_SMV_STATISTICS_HEAD_SUB         180
#define DSM_SMV_STATISTICS_HEAD_VALUE       349

#define DSM_SVM_STATISTICS_RC_TOP           CRect(0, 0, 640, ELT_T1_HDRHEIGHT)
#define DSM_SMV_STATISTICS_RC_TOP1          CRect(0, 0, 640-110, ELT_T1_HDRHEIGHT)
#define DSM_SMV_STATISTICS_RC_TOP2          CRect(0, 0, 640-110, ELT_T1_HDRHEIGHT)
#define DSM_SMV_STATISTICS_RC_REPROT        CRect(0, 25, 640, 444)



//////////////////////////////////////////////////////////////////////////
// ״̬����
#define DSM_MONITOR_RCSTATUS                CRect(110, 416, 640, DSM_P_BODY_BOTTOM)     ///< 92�ײ�MU״̬����
#define DSM_MONITOR_RCSTATUS_92_MU          CRect(496-110, 0, 629-110, 24)      ///< 92�ײ�MU״̬����
#define DSM_MONITOR_RCSTATUS_FT3_WARN       CRect(127-110, 0, 250-110, 24)       ///< FT3�ײ��澯״̬����
#define DSM_MONITOR_RCSTATUS_FT3_FIX        CRect(250-110, 0, 373-110, 24)      ///< FT3�ײ�����״̬����
#define DSM_MONITOR_RCSTATUS_FT3_WAKE       CRect(373-110, 0, 496-110, 24)      ///< FT3�ײ�����״̬����
#define DSM_MONITOR_RCSTATUS_FT3_MU         CRect(496-110, 0, 629-110, 24)      ///< FT3�ײ�MU״̬����

#define DSM_MONITOR_92_INDEX                60          ///< ����
#define DSM_MONITOR_92_DESC                 160         ///< ����
#define DSM_MONITOR_92_STATUS               71          ///< ״̬
#define DSM_MONITOR_92_FIX                  71          ///< ����
#define DSM_MONITOR_92_COMPOUND             71          ///< �ϳ�
#define DSM_MONITOR_92_QUALITY              96         ///< Ʒ��

#define DSM_MONITOR_FT3_INDEX               128         ///< ����
#define DSM_MONITOR_FT3_DESC                274         ///< ����
#define DSM_MONITOR_FT3_STATUS              127         ///< ״̬




#define DSM_SMV_MONITOR_PROTO_UNKNOWN       0x00            ///< δ֪
#define DSM_SMV_MONITOR_PROTO_92            0x01            ///< 92
#define DSM_SMV_MONITOR_PROTO_91FT3         0x02            ///< 91 FT3(�������ϵ�)


#define DRAWTEXT_MODE       DT_VCENTER | DT_CENTER | DT_SINGLELINE
#endif

// #define DSM_SMV_STATISTICS_CR_UN            RGB(0x00, 0x99, 0xFF)
// #define DSM_SMV_STATISTICS_CR_0             RGB(0x00, 0x80, 0x00)
// #define DSM_SMV_STATISTICS_CR_1             RGB(0x33, 0x99, 0x66)
// #define DSM_SMV_STATISTICS_CR_2_10          RGB(0x00, 0xCC, 0xFF)
// #define DSM_SMV_STATISTICS_CR_10_50         RGB(0x33, 0x66, 0xFF)
// #define DSM_SMV_STATISTICS_CR_50_250        RGB(0xFF, 0x99, 0x00)
// #define DSM_SMV_STATISTICS_CR_250           RGB(0xFF, 0x00, 0x00)
// #define DSM_SMV_STATISTICS_CR_UN            RGB(0x00, 0x99, 0xFF)
// #define DSM_SMV_STATISTICS_CR_0             RGB(0x23, 0x7C, 0xD6)
// #define DSM_SMV_STATISTICS_CR_1             RGB(0x26, 0xC1, 0x97)
// #define DSM_SMV_STATISTICS_CR_2_10          RGB(0x83, 0xB6, 0x2E)
// #define DSM_SMV_STATISTICS_CR_10_50         RGB(0xCE, 0x9D, 0x1E)
// #define DSM_SMV_STATISTICS_CR_50_250        RGB(0xEA, 0x56, 0x3C)
// #define DSM_SMV_STATISTICS_CR_250           RGB(0xB8, 0x24, 0x1D)

//�޸���ɫ
/*************************************************************************/
#define DSM_SMV_STATISTICS_CR_UN            RGB(0xb4, 0xee, 0x51)
#define DSM_SMV_STATISTICS_CR_0             RGB(0x82, 0xe9, 0x56)
#define DSM_SMV_STATISTICS_CR_1             RGB(0x66, 0xe6, 0x52)
#define DSM_SMV_STATISTICS_CR_2_10          RGB(0x4a, 0xdf, 0x59)
#define DSM_SMV_STATISTICS_CR_10_50         RGB(0x4a, 0xdf, 0x59)
#define DSM_SMV_STATISTICS_CR_50_250        RGB(0x4a, 0xdf, 0x59)
#define DSM_SMV_STATISTICS_CR_250           RGB(0x4a, 0xdf, 0x59)


#if !SU_FEI
/************************************************************************/
/* ���Monitorģ�����                                                                     */
/************************************************************************/

/**
 *      SMV����״̬����  92 ͨ��״̬��¼
 *
 *  \note
 *      ͨ����š�������״̬�����ޡ��ϳ�����Ʒ��
 */
class CDsmSmvMonitor92Record : public CELRecord
{
public:
    CDsmSmvMonitor92Record(CSmvRecvChannelInfo* pInfo);

    virtual ~CDsmSmvMonitor92Record() {}

public:
    /**
     *      ����ͨ��״̬��Ϣ
     */
    void Update(CStatusWatchChannelInfo info);
};



/**
 *      SMV����״̬����  91 FT3 ͨ��״̬��¼
 *
 *  \note
 *      ͨ����š�������״̬
 */
class CDsmSmvMonitorFT3Record : public CELRecord
{
public:
    CDsmSmvMonitorFT3Record(CSmvRecvChannelInfo* pInfo);

    virtual ~CDsmSmvMonitorFT3Record() {}

public:
    /**
     *      ����ͨ��״̬��Ϣ
     */
    void Update(CStatusWatchChannelInfo info);
};



CDsmSmvMonitor92Record::CDsmSmvMonitor92Record( CSmvRecvChannelInfo* pInfo )
{
	if(pInfo)
	{
		AddItem(new CDsmRecordItem(pInfo->nIndex + 1));
		AddItem(new CDsmRecordItem(pInfo->strChannelDesc.c_str()));
		AddItem(new CDsmRecordItem(g_csBlankValue));
		AddItem(new CDsmRecordItem(g_csBlankValue));
		AddItem(new CDsmRecordItem(g_csBlankValue));
		AddItem(new CDsmRecordItem(g_csBlankValue));
	}
}


void CDsmSmvMonitor92Record::Update( CStatusWatchChannelInfo info )
{
	CString csQuality;
	CString csStatus    = dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_EFFECTIVE);
	CString csFix       = dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_YES);
	CString csCompound  = dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_YES);

	csQuality.Format(L"0x%04X", info.m_nQuality);

	if(!info.m_bChannelValid)
	{
		csStatus = dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_NOTEFFECTIVE);
	}

	if(!info.m_bChannelTestFlag)
	{
		csFix = dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_NO);
	}

	if(!info.m_bChannelDerived)
	{
		csCompound = dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_NO);
	}

	GetItem(2)->SetCaption(csStatus);
	GetItem(3)->SetCaption(csFix);
	GetItem(4)->SetCaption(csCompound);
	GetItem(5)->SetCaption(csQuality);
}


CDsmSmvMonitorFT3Record::CDsmSmvMonitorFT3Record( CSmvRecvChannelInfo* pInfo )
{
	if(pInfo)
	{
		AddItem(new CDsmRecordItem(pInfo->nIndex + 1));
		AddItem(new CDsmRecordItem(pInfo->strChannelDesc.c_str()));
		AddItem(new CDsmRecordItem(g_csBlankValue));
	}
}


void CDsmSmvMonitorFT3Record::Update( CStatusWatchChannelInfo info )
{
	if(info.m_bChannelValid)
	{
		GetItem(2)->SetCaption(dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_EFFECTIVE));
	}
	else
	{
		GetItem(2)->SetCaption(dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_NOTEFFECTIVE));
	}
}

#endif
/**
 *      SMV���� - ����ͳ��
 *  \note
 *      �ӱ��⡢ֵ
 */
class CDsmSmvStatisticsRecord : public CELRecord
{
public:
    CDsmSmvStatisticsRecord(CString csSubCaption)
    {
        AddItem(new CDsmRecordItem(csSubCaption));
        AddItem(new CDsmRecordItem(L""));
    }

    virtual ~CDsmSmvStatisticsRecord() {}
};



// CSMVStatisticsDlg �Ի���

IMPLEMENT_DYNAMIC(CSMVStatisticsDlg, DSM_SMVBASEDLG)

CSMVStatisticsDlg::CSMVStatisticsDlg(CWnd* pParent /*=NULL*/)
	: DSM_SMVBASEDLG(CSMVStatisticsDlg::IDD, DSM_STR_SMV_STATISTICS_CAPTION, pParent)
    , m_csSampleInter(L"")
    , m_csSample(L"")
#if !SU_FEI
	, m_modeCtr(REPORT_MODE)
	, m_dwTag(DSM_SMV_MONITOR_PROTO_UNKNOWN)
	, m_nBegin(0)
	, m_nEnd(0)
	, m_rc92MU(DSM_MONITOR_RCSTATUS_92_MU)
	, m_rcFT3MU(DSM_MONITOR_RCSTATUS_FT3_MU)
	, m_rcFT3Warn(DSM_MONITOR_RCSTATUS_FT3_WARN)
	, m_rcFT3Fix(DSM_MONITOR_RCSTATUS_FT3_FIX)
	, m_rcFT3Wake(DSM_MONITOR_RCSTATUS_FT3_WAKE)
	, m_rcStatus(DSM_MONITOR_RCSTATUS)
#endif
{
    for(int i = 0; i < STATISTICS_PILLARS; i++)
    {
        m_etypes[i] = (MSG_DISP_PERIOD)(MSG_DISP_PERIOD_0 + i);
    }



}

CSMVStatisticsDlg::~CSMVStatisticsDlg()
{
#if !SU_FEI
	m_panelStatus.DeleteObject();
#endif
}

void CSMVStatisticsDlg::DoDataExchange(CDataExchange* pDX)
{
	DSM_SMVBASEDLG::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSMVStatisticsDlg, DSM_SMVBASEDLG)
    ON_WM_TIMER()
    ON_WM_PAINT()
    ON_COMMAND(ID_MENU_SMV_F3, &CSMVStatisticsDlg::OnMenuF3)
    ON_COMMAND(ID_MENU_SMV_F6, &CSMVStatisticsDlg::OnMenuF6)

#if !SU_FEI
	ON_COMMAND(ID_MENU_SMV_F4, &CSMVStatisticsDlg::OnMenuF4)
	ON_COMMAND(ID_MENU_SMV_F5, &CSMVStatisticsDlg::OnMenuF5)
#endif

END_MESSAGE_MAP()


// CSMVStatisticsDlg ��Ϣ�������

BOOL CSMVStatisticsDlg::OnInitDialog()
{
    CSMVBaseDlg::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��

    // ���ò���Ƶ�ʲ˵�
    //_setSampleRateMenu();

#if SU_FEI
   	// ������ɢ��ͼ��
    _createHorHistogram();

    // ��������ͳ�Ʊ���
	 _createReport();
#else 
	
	if (m_modeCtr == REPORT_MODE)
	{
		_createReport();
	}
	else if (m_modeCtr == STATISTICS_MODE)
	{
		_createHorHistogram();
	}
	
	if(m_pCurrentCtrlBlk)
	{
		// ���õ�ǰ���ƿ��Э��ģʽ

		// 92Э��
		if(FRAME_TYPE_NET_9_2 == m_pCurrentCtrlBlk->enFrameType)
		{
			m_dwTag = DSM_SMV_MONITOR_PROTO_92;
		}
		// FT3��91
		else if(FRAME_TYPE_NET_9_1 == m_pCurrentCtrlBlk->enFrameType || 
			FRAME_TYPE_FT3_NANRUI == m_pCurrentCtrlBlk->enFrameType || 
			FRAME_TYPE_FT3_GUOWANG == m_pCurrentCtrlBlk->enFrameType)
		{
			m_dwTag = DSM_SMV_MONITOR_PROTO_91FT3;
		}
		else
		{
			m_dwTag = DSM_SMV_MONITOR_PROTO_UNKNOWN;
		}

		

		//_beginTimer();
	}

	m_csMUSync.Format(L"%s��%s", dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_MU), dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_SYNC));
	m_csMUNotSync.Format(L"%s��%s", dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_MU), dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_UNSYNC));
	m_csWarnGood.Format(L"%s��%s", dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_WARN), dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_YES));
	m_csWarnBad.Format(L"%s��%s", dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_WARN), dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_NO));
	m_csFixConn.Format(L"%s��%s", dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_FIX), dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_YES));
	m_csFixTest.Format(L"%s��%s", dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_FIX), dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_NO));
	m_csWakeValid.Format(L"%s��%s", dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_WAKE), dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_YES));
	m_csWakeInvalid.Format(L"%s��%s", dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_WAKE), dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_NO));

	// ������
	m_csMUNULL.Format(L"%s��%s", dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_MU), g_csBlankValue);
	m_csWarnNULL.Format(L"%s��%s", dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_WARN), g_csBlankValue);
	m_csFixNULL.Format(L"%s��%s", dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_FIX), g_csBlankValue);
	m_csWakeNULL.Format(L"%s��%s", dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_ST_WAKE), g_csBlankValue);

#endif



    m_csSample.Format(L" - %s(%d)", dsm_stringMgr::Get(DSM_STR_SMV_STATISTICS_SMPRAT), m_pCurrentCtrlBlk->nSampleRate);
    CStatusMgr::SetTitle(CStatusMgr::GetTitle() + m_csSample);

    // ����ͳ��
    m_pDataLayer->ResetSmvFrameStatistics();

    return FALSE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}

void CSMVStatisticsDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    if(nIDEvent == m_nTimerID)
    {
        _update();
    }

    CSMVBaseDlg::OnTimer(nIDEvent);
}

void CSMVStatisticsDlg::OnPaint()
{

    CPaintDC dc(this); // device context for painting
    // TODO: �ڴ˴������Ϣ����������
    // ��Ϊ��ͼ��Ϣ���� CSMVBaseDlg::OnPaint()

    // ������ɢ����״ͼ�ı���
    CRect rcTop2 = m_rcTop2;
#if SU_FEI
    rcTop2.DeflateRect(DSM_MARGIN_IN, 0, 0, 0);
#else
	rcTop2.DeflateRect(20, 0, DSM_MARGIN_IN, 0);
#endif
    int nOriDC = dc.SaveDC();
    dc.SetBkMode(TRANSPARENT);
    dc.SetTextColor(ELT_T1_HDRTEXTCOLOR);
    dc.SelectObject(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    dc.FillSolidRect(m_rcTop2, ELT_T1_HDRBKCOLOR);

    CString csSub2;
    csSub2 = dsm_stringMgr::Get(DSM_STR_SMV_STATISTICS_SUB2_CAPTION) + m_csSampleInter;
    dc.DrawText(csSub2, rcTop2, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    dc.RestoreDC(nOriDC);
}

void CSMVStatisticsDlg::_createHorHistogram()
{
    CRect rcClient;
    GetClientRect(rcClient);

    m_rcTop2 = DSM_SMV_STATISTICS_RC_TOP2;
    m_rcTop2.OffsetRect(0, DSM_MARGIN_IN);
    m_rcTop2.DeflateRect(DSM_MARGIN_IN, 0, DSM_MARGIN_IN, 0);

    rcClient.DeflateRect(m_rcTop2.left, m_rcTop2.bottom, DSM_MARGIN_IN, DSM_MARGIN_IN);

    m_ctrlHistogram.Create(L"", WS_CHILD | WS_VISIBLE, rcClient, this, IDC_SMV_STATISTICS_CTRL);
    m_ctrlHistogram.ModifyStyle(0, SS_OWNERDRAW);
    m_ctrlHistogram.SetBkgndColor(DSM_COLOR_BK);

    CFont* pFont = dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT);
    m_ctrlHistogram.SetTitleFont(pFont);
    m_ctrlHistogram.SetScaleValFont(pFont);
    m_ctrlHistogram.SetPillarValFont(pFont);

    // ��״ͼ��ɫ
    COLORREF crPillar[STATISTICS_PILLARS] = {
        DSM_SMV_STATISTICS_CR_0, 
        DSM_SMV_STATISTICS_CR_1, 
        DSM_SMV_STATISTICS_CR_2_10, 
        DSM_SMV_STATISTICS_CR_10_50, 
        DSM_SMV_STATISTICS_CR_50_250, 
        DSM_SMV_STATISTICS_CR_250, 
    };

    // ������״ͼ����
    m_ctrlHistogram.SetPillarCount(STATISTICS_PILLARS);

    // ������״ͼ��ɫ�ͱ���
    for(int i = 0; i < STATISTICS_PILLARS; i++)
    {
        m_ctrlHistogram.SetPillarColor(i, crPillar[i]);
    }

    // ������ɢ�ȱ���
    _resetPillarTitle();

    m_ctrlHistogram.SetPillarValColor(RGB(0xFF, 0xFF, 0xFF));
    m_ctrlHistogram.SetScaleValColor(RGB(0xFF, 0xFF, 0xFF));
    m_ctrlHistogram.SetPillarAreaBorderColor(ELT_T1_HDRGRIDCOLOR);
    m_ctrlHistogram.SetSpaceFromLeftBorderToPillar(10);


    // ����10������ÿ������10
    m_ctrlHistogram.SetPillarAreaGrid(10, 0);
    m_ctrlHistogram.SetXScaleInterval(10.0);

    m_ctrlHistogram.SetPillarBetween(10);           ///< ��״ͼ���
    m_ctrlHistogram.SetPillarHeight(30);            ///< ��״ͼ���
    m_ctrlHistogram.SetMargin(0, 78, 0, 0);         ///< �ؼ����
#if SU_FEI
    m_ctrlHistogram.SetYScaleValLen(55);            ///< ��������
#else
	m_ctrlHistogram.SetYScaleValLen(116);
#endif
    m_ctrlHistogram.SetTitleVisible(TRUE);
    m_ctrlHistogram.SetBorderVisible(0, TRUE);
    m_ctrlHistogram.SetXScaleValVisible(TRUE);
    m_ctrlHistogram.SetYScaleValVisible(TRUE);
    m_ctrlHistogram.SetVerGridVisible(FALSE);

    m_ctrlHistogram.Init();
    m_ctrlHistogram.ShowWindow(TRUE);
}


void CSMVStatisticsDlg::_createReport()
{
    ASSERT(m_pCurrentCtrlBlk);

    CRect rcClient;
    GetClientRect(rcClient);

    m_rcTop1 = DSM_SMV_STATISTICS_RC_TOP1;
    m_rcTop1.OffsetRect(0, DSM_MARGIN_IN);

    m_rcTop1.DeflateRect( 0, 0, DSM_MARGIN_IN, 0);

    // ��1�����ر����ͷ�ߣ����Ի�����غ�
    rcClient.DeflateRect(0, DSM_MARGIN_IN , rcClient.right - m_rcTop1.right, DSM_MARGIN_IN);

    m_reportStatistics.Create(DSM_WINSTYLE_REPORT_DEFAULT, rcClient, this, IDC_SMV_STATISTICS_REPORT);

    // ��������
    m_reportStatistics.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    // ������ʽ
    m_reportStatistics.PostInitTemplateStyle();

    // ������Ԫ��
    m_reportStatistics.InsertColumn(0, L"  " + dsm_stringMgr::Get(DSM_STR_SMV_STATISTICS_SUB1_CAPTION), EL_LEFT, DSM_SMV_STATISTICS_HEAD_SUB);
    m_reportStatistics.InsertColumn(1, L"", EL_LEFT, DSM_SMV_STATISTICS_HEAD_VALUE);

    // ����ѡ�񽹵���ʽ
    m_reportStatistics.SetRowSelectionVisible(FALSE);
    m_reportStatistics.SetRowFocusEnable(FALSE);

    m_reportStatistics.InsertRecordRow(0, new CDsmSmvStatisticsRecord(dsm_stringMgr::Get(DSM_STR_SMV_STATISTICS_TOTAL)));
    m_reportStatistics.InsertRecordRow(1, new CDsmSmvStatisticsRecord(dsm_stringMgr::Get(DSM_STR_SMV_STATISTICS_LOSE)));
    m_reportStatistics.InsertRecordRow(2, new CDsmSmvStatisticsRecord(dsm_stringMgr::Get(DSM_STR_SMV_STATISTICS_ERROR)));
    m_reportStatistics.InsertRecordRow(3, new CDsmSmvStatisticsRecord(dsm_stringMgr::Get(DSM_STR_SMV_STATISTICS_RETRY)));
    m_reportStatistics.InsertRecordRow(4, new CDsmSmvStatisticsRecord(dsm_stringMgr::Get(DSM_STR_SMV_STATISTICS_LOSTSYNC)));
    m_reportStatistics.InsertRecordRow(5, new CDsmSmvStatisticsRecord(dsm_stringMgr::Get(DSM_STR_SMV_STATISTICS_TEST)));
    m_reportStatistics.InsertRecordRow(6, new CDsmSmvStatisticsRecord(dsm_stringMgr::Get(DSM_STR_SMV_STATISTICS_SHAKE)));

    if(m_pCurrentCtrlBlk->enFrameType == FRAME_TYPE_NET_9_2)
    {
        m_reportStatistics.InsertRecordRow(7, new CDsmSmvStatisticsRecord(dsm_stringMgr::Get(DSM_STR_SMV_STATISTICS_QUALITY)));
    }
    m_reportStatistics.SetFocus();

}


#if !SU_FEI
/************************************************************************/
/* ����״̬���ӱ��                                                    */
/************************************************************************/
void CSMVStatisticsDlg::_createMonitorReport()
{
	ASSERT(!m_reportMonitor.GetSafeHwnd());

	CRect rcClient;
	GetClientRect(rcClient);

	rcClient.DeflateRect(DSM_MARGIN_IN, DSM_MARGIN_IN, DSM_MARGIN_IN, m_rcStatus.Height());

	int nCounts = (rcClient.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
	rcClient.bottom = rcClient.top + ELT_T1_HDRHEIGHT + ELT_T1_ROWHEIGHT * nCounts;

	
	m_rcStatus.bottom   = rcClient.bottom + m_rcStatus.Height();
	m_rcStatus.top      = rcClient.bottom;
 
	m_rcStatus.DeflateRect(DSM_MARGIN_IN, 0, DSM_MARGIN_IN, 0);

	m_reportMonitor.Create(DSM_WINSTYLE_REPORT_DEFAULT, rcClient, this, IDC_SMV_MONITOR_REPORT);
	
	// ��������
	m_reportMonitor.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
	m_reportMonitor.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

	m_reportMonitor.PostInitTemplateStyle();

	// 92
	if(DSM_SMV_MONITOR_PROTO_92 == m_dwTag)
	{
		_insertColumn92();
	}
	// 91 FT3
	else if(DSM_SMV_MONITOR_PROTO_91FT3 == m_dwTag)
	{
		_insertColumnFT3();
	}
	else
	{
		return;
	}

	// ����ѡ�񽹵���ʽ
	m_reportMonitor.SetRowSelectionVisible(FALSE);
	m_reportMonitor.SetRowFocusEnable(FALSE);

	_initMonitorReport();
}

void CSMVStatisticsDlg::_initMonitorReport()
{
	if(!m_pCurrentCtrlBlk || !m_reportMonitor.GetSafeHwnd())
	{
		return;
	}

	CSmvRecvConfig* pCfg = m_pCurrentCtrlBlk->pRecvConfig;

	if(!pCfg)
	{
		return;
	}


	size_t nCounts = pCfg->GetChannelCount();

	// 92
	if(DSM_SMV_MONITOR_PROTO_92 == m_dwTag)
	{
		for(size_t i = 0; i < nCounts; i++)
		{
			m_reportMonitor.InsertRecordRow(i, new CDsmSmvMonitor92Record(pCfg->GetOneChannelInfo(i)));
		}
	}
	// 91 FT3
	else if(DSM_SMV_MONITOR_PROTO_91FT3 == m_dwTag)
	{
		for(size_t i = 0; i < nCounts; i++)
		{
			m_reportMonitor.InsertRecordRow(i, new CDsmSmvMonitorFT3Record(pCfg->GetOneChannelInfo(i)));
		}
	}

	m_nBegin    = m_reportMonitor.GetTopRowIndex();
	m_nEnd      = m_reportMonitor.GetCountPerPage() + m_nBegin;
}


void CSMVStatisticsDlg::_insertColumn92()
{
	int nIndex = 0;

	// ������Ԫ��
	m_reportMonitor.InsertColumn(nIndex++, L"", EL_CENTER, DSM_MONITOR_92_INDEX);
	m_reportMonitor.InsertColumn(nIndex++, dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_9X_DESC), EL_LEFT, DSM_MONITOR_92_DESC);
	m_reportMonitor.InsertColumn(nIndex++, L"  " + dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_9X_STATUS), EL_CENTER, DSM_MONITOR_92_STATUS);
	m_reportMonitor.InsertColumn(nIndex++, L"  " +  dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_9X_FIX), EL_CENTER, DSM_MONITOR_92_FIX);
	m_reportMonitor.InsertColumn(nIndex++, L"  " + dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_9X_COMPOUND), EL_CENTER, DSM_MONITOR_92_COMPOUND);
	m_reportMonitor.InsertColumn(nIndex++, L"  " + dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_9X_QUALITY), EL_CENTER, DSM_MONITOR_92_QUALITY);
}


void CSMVStatisticsDlg::_insertColumnFT3()
{
	int nIndex = 0;

	// ������Ԫ��
	m_reportMonitor.InsertColumn(nIndex++, L"", EL_CENTER, DSM_MONITOR_FT3_INDEX);
	m_reportMonitor.InsertColumn(nIndex++, dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_FT3_DESC), EL_LEFT, DSM_MONITOR_FT3_DESC);
	m_reportMonitor.InsertColumn(nIndex++, L"     " + dsm_stringMgr::Get(DSM_STR_SMV_MONITOR_FT3_STATUS), EL_CENTER, DSM_MONITOR_FT3_STATUS);
}

/**
 *      ���ͨ�����Ƿ�仯
 */
bool CSMVStatisticsDlg::_checkChannel()
{
	if(m_pCurrentCtrlBlk->nChannelCount != m_pDataLayer->GetCurrentCtrlBlkRealTimeChannelCounts())
	{
		if(CStatusMgr::ShowTipsStyle() == 0x00)
		{
			// ����tips��ʾ
			CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_CHANNEL_NUM_CHANGE));
		}

		if(!m_bChannelChange)
		{
			m_bChannelChange = true;

			// �������
			_updateNullUI();
		}

		return true;
	}
	else
	{
		if(m_bChannelChange)
		{
			m_bChannelChange = false;
			CStatusMgr::SetTips(L"", 0);
		}
	}

	return false;
}


/**
 *      ��ս����������ʾ��������������Ϊ "--"
 */
void CSMVStatisticsDlg::_updateNullUI()
{
    int nCounts = m_reportMonitor.GetRecordRowCount();

    // 92
    if(DSM_SMV_MONITOR_PROTO_92 == m_dwTag)
    {
        for(int i = m_nBegin; i < m_nEnd && i < nCounts; i++)
        {
            m_reportMonitor.SetRowItemText(i, 2, g_csBlankValue);
            m_reportMonitor.SetRowItemText(i, 3, g_csBlankValue);
            m_reportMonitor.SetRowItemText(i, 4, g_csBlankValue);
            m_reportMonitor.SetRowItemText(i, 5, g_csBlankValue);
        }
    }
    // 91 FT3
    else if(DSM_SMV_MONITOR_PROTO_91FT3 == m_dwTag)
    {
        for(int i = m_nBegin; i < m_nEnd && i < nCounts; i++)
        {
            m_reportMonitor.SetRowItemText(i, 2, g_csBlankValue);
        }
    }

    _updateStatus(true);
}
/**
 *      ���µײ�״̬��Ϣ
 */
void CSMVStatisticsDlg::_updateStatus( bool bNull /* = false */ )
{
    CString csMU(m_csMUNULL), csWarn(m_csWarnNULL), csFix(m_csFixNULL), csWakeup(m_csWakeNULL);

    if(!bNull)
    {
        csWarn      = (m_monitordata.GetFt3FaultAlarm() == 0) ? m_csWarnGood : m_csWarnBad;
        csFix       = (m_monitordata.GetFt3TestFlag() == 0) ? m_csFixConn : m_csFixTest;
        csWakeup    = (m_monitordata.GetFt3WakeupValid() == 0) ? m_csWakeValid : m_csWakeInvalid;
        csMU        = m_monitordata.GetMuIsSync() ? m_csMUSync : m_csMUNotSync;
    }

    CDC* pDC = GetDC();

    CDC dcMem;
    dcMem.CreateCompatibleDC(pDC);
    CRect rcStatus(DSM_MONITOR_RCSTATUS);

    if(!m_panelStatus.GetSafeHandle())
    {
        m_panelStatus.CreateCompatibleBitmap(pDC, rcStatus.Width(), rcStatus.Height());
    }

    int nOri = dcMem.SaveDC();

    dcMem.SelectObject(m_panelStatus);
    dcMem.SetBkMode(TRANSPARENT);
    dcMem.SetTextColor(ELT_T1_TEXTCOLOR);
    dcMem.SelectObject(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    dcMem.FillSolidRect(0, 0, rcStatus.Width(), rcStatus.Height(), ELT_T1_BKCOLOR);

    if(DSM_SMV_MONITOR_PROTO_92 == m_dwTag)
    {
        dcMem.DrawText(csMU, m_rc92MU, DRAWTEXT_MODE);
    }
    else if(DSM_SMV_MONITOR_PROTO_91FT3 == m_dwTag)
    {
        dcMem.DrawText(csWarn, m_rcFT3Warn, DRAWTEXT_MODE);
        dcMem.DrawText(csFix, m_rcFT3Fix, DRAWTEXT_MODE);
        dcMem.DrawText(csWakeup, m_rcFT3Wake, DRAWTEXT_MODE);
        dcMem.DrawText(csMU, m_rcFT3MU, DRAWTEXT_MODE);
    }

    pDC->BitBlt(0, m_rcStatus.top, rcStatus.Width(), rcStatus.Height(), &dcMem, 0, 0, SRCCOPY);
    dcMem.RestoreDC(nOri);
    dcMem.DeleteDC();
    pDC->DeleteDC();
}

BOOL CSMVStatisticsDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: �ڴ����ר�ô����/����û���
    if(WM_KEYDOWN == pMsg->message)
    {
        switch(pMsg->wParam)
        {
        case VK_UP:
			
            m_reportMonitor.ScrollPageUp();
            m_nBegin    = m_reportMonitor.GetTopRowIndex();
            m_nEnd      = m_reportMonitor.GetCountPerPage() + m_nBegin;
            return TRUE;

        case VK_DOWN:
            m_reportMonitor.ScrollPageDown();
            m_nBegin    = m_reportMonitor.GetTopRowIndex();
            m_nEnd      = m_reportMonitor.GetCountPerPage() + m_nBegin;
            return TRUE;
        }
    }

    return CSMVBaseDlg::PreTranslateMessage(pMsg);
}
#endif


void CSMVStatisticsDlg::_update()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pDataLayer);



    if(!m_pDataLayer->GetFrameStatistics(&m_data))
    {
        // �������

        m_reportStatistics.SetRowItemText(0, 1, g_csBlankValue);
        m_reportStatistics.SetRowItemText(1, 1, g_csBlankValue);
        m_reportStatistics.SetRowItemText(2, 1, g_csBlankValue);
        m_reportStatistics.SetRowItemText(3, 1, g_csBlankValue);
        m_reportStatistics.SetRowItemText(4, 1, g_csBlankValue);
        m_reportStatistics.SetRowItemText(5, 1, g_csBlankValue);
        m_reportStatistics.SetRowItemText(6, 1, g_csBlankValue);

        if(m_pCurrentCtrlBlk->enFrameType == FRAME_TYPE_NET_9_2)
        {
            m_reportStatistics.SetRowItemText(7, 1, g_csBlankValue);
        }
#if !SU_FEI
		_updateNullUI();
#endif
        return;
    }
#if SU_FEI
    CString csTotalCount;           // �����ܼ���
    CString csLostCount;            // �������
    CString csErrorCount;           // �������
    CString csTimeoutCount;         // ��ʱ����
    CString csJitterCount;          // ��������
    CString csRetry;                // �ط�����
    CString csLostSync;             // ʧ������
    CString csQuality;              // Ʒ����Ч����
    CString csTest;                 // ���޼���

    // ���ñ�������
    csTotalCount.Format(L"%u", m_data.GetTotalFrameCount());
    csLostCount.Format(L"%u", m_data.GetLostFrameCount());
    csErrorCount.Format(L"%u", m_data.GetMisOrderFrameCount());
    csRetry.Format(L"%u", m_data.GetRetryFrameCount());
    csLostSync.Format(L"%u", m_data.GetLostSyncFrameCount());
    csTest.Format(L"%u", m_data.GetTestFrameCount());
    csJitterCount.Format(L"%u", m_data.GetJitterFrameCount());

    m_reportStatistics.SetRowItemText(0, 1, csTotalCount);
    m_reportStatistics.SetRowItemText(1, 1, csLostCount);
    m_reportStatistics.SetRowItemText(2, 1, csErrorCount);
    m_reportStatistics.SetRowItemText(3, 1, csRetry);
    m_reportStatistics.SetRowItemText(4, 1, csLostSync);
    m_reportStatistics.SetRowItemText(5, 1, csTest);
    m_reportStatistics.SetRowItemText(6, 1, csJitterCount);

    if(m_pCurrentCtrlBlk->enFrameType == FRAME_TYPE_NET_9_2)
    {
        csQuality.Format(L"%u", m_data.GetQualityInvalidFrameCount());
        m_reportStatistics.SetRowItemText(7, 1, csQuality);
    }

    // ˢ�±�����ɢ����״ͼ
    for ( int i = 0; i < STATISTICS_PILLARS; i++ )
    {
        m_fdata[i] = m_data.GetDispPercent(m_etypes[i]);
    }

    m_ctrlHistogram.ErasePillar();
    m_ctrlHistogram.DrawHorHistoGram(m_fdata,sizeof(m_fdata)/sizeof(float), _T("%"));

    m_ctrlHistogram.Invalidate();



#else
	if (m_modeCtr == REPORT_MODE)
	{
		
		//����top2��������
		InvalidateRect(m_rcTop2, TRUE);

		CString csTotalCount;           // �����ܼ���
		CString csLostCount;            // �������
		CString csErrorCount;           // �������
		CString csTimeoutCount;         // ��ʱ����
		CString csJitterCount;          // ��������
		CString csRetry;                // �ط�����
		CString csLostSync;             // ʧ������
		CString csQuality;              // Ʒ����Ч����
		CString csTest;                 // ���޼���

		// ���ñ�������
		csTotalCount.Format(L"%u", m_data.GetTotalFrameCount());
		csLostCount.Format(L"%u", m_data.GetLostFrameCount());
		csErrorCount.Format(L"%u", m_data.GetMisOrderFrameCount());
		csRetry.Format(L"%u", m_data.GetRetryFrameCount());
		csLostSync.Format(L"%u", m_data.GetLostSyncFrameCount());
		csTest.Format(L"%u", m_data.GetTestFrameCount());
		csJitterCount.Format(L"%u", m_data.GetJitterFrameCount());

		m_reportStatistics.SetRowItemText(0, 1, csTotalCount);
		m_reportStatistics.SetRowItemText(1, 1, csLostCount);
		m_reportStatistics.SetRowItemText(2, 1, csErrorCount);
		m_reportStatistics.SetRowItemText(3, 1, csRetry);
		m_reportStatistics.SetRowItemText(4, 1, csLostSync);
		m_reportStatistics.SetRowItemText(5, 1, csTest);
		m_reportStatistics.SetRowItemText(6, 1, csJitterCount);

		if(m_pCurrentCtrlBlk->enFrameType == FRAME_TYPE_NET_9_2)
		{
			csQuality.Format(L"%u", m_data.GetQualityInvalidFrameCount());
			m_reportStatistics.SetRowItemText(7, 1, csQuality);
		}

	}
	else if (m_modeCtr == STATISTICS_MODE)
	{
		
	
		// ˢ�±�����ɢ����״ͼ
		for ( int i = 0; i < STATISTICS_PILLARS; i++ )
		{
			m_fdata[i] = m_data.GetDispPercent(m_etypes[i]);
		}

		m_ctrlHistogram.ErasePillar();
		m_ctrlHistogram.DrawHorHistoGram(m_fdata,sizeof(m_fdata)/sizeof(float), _T("%"));

		m_ctrlHistogram.Invalidate();
	}
	else if (m_modeCtr == MONITOR_MODE)
	{
		//����top2��������
		InvalidateRect(m_rcTop2, TRUE);

		

		if(_checkChannel())
		{
			return;
		}

		_updateStatus();

		//int nCounts = (int)m_monitordata.GetChannelCount();
		
		int nCounts = m_pCurrentCtrlBlk->nChannelCount;

		// 92
		if(DSM_SMV_MONITOR_PROTO_92 == m_dwTag)
		{
			for(int i = m_nBegin; i < m_nEnd && i < nCounts; i++)
			{
				CDsmSmvMonitor92Record* pRecord92 = dynamic_cast<CDsmSmvMonitor92Record*>(m_reportMonitor.GetRecordRow(i));

				if(pRecord92)
				{
					pRecord92->Update(m_monitordata.GetOneChannelInfo(i));
				}
			}
		}
		// 91 FT3
		else if(DSM_SMV_MONITOR_PROTO_91FT3 == m_dwTag)
		{
			for(int i = m_nBegin; i < m_nEnd && i < nCounts; i++)
			{
				CDsmSmvMonitorFT3Record* pRecordFT3 = dynamic_cast<CDsmSmvMonitorFT3Record*>(m_reportMonitor.GetRecordRow(i));

				if(pRecordFT3)
				{
					pRecordFT3->Update(m_monitordata.GetOneChannelInfo(i));;
				}
			}
		}

		m_reportMonitor.Invalidate(TRUE);

		
	}

#endif


}

/**
 *      ����ҳ��
 */
void CSMVStatisticsDlg::_SavePage()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pPageDataExtra);
}

/**
 *      �޸Ĳ˵�HMENU
 */
void CSMVStatisticsDlg::_modifyMenu()
{
    __super::_modifyMenu();

    // ����ʾF3����
#if SU_FEI
    m_menuMain.SetMenuItemCaption(2, L"");
#else
	
	m_menuMain.SetMenuItemCaption(2, L"");
	m_menuMain.SetMenuSubItem(2, NULL);
	m_menuMain.SetMenuItemId(2, ID_MENU_SMV_F3);

	m_menuMain.HighlightMenuItem(2, TRUE);

	m_menuMain.SetMenuItemId(3, ID_MENU_SMV_F4);

	m_menuMain.SetMenuItemId(4, ID_MENU_SMV_F5);


	m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_STATISTICS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_STATISTICS_SEL), NULL);
	m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_PILLAR_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_PILLAR_SEL), NULL);
	m_menuMain.SetMenuItemBitmaps(4, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_MONITOR_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_MONITOR_SEL), NULL);
	
#endif
    // ����F6����ʽ ������ͳ�ơ�
    m_menuMain.SetMenuSubItem(5, NULL);
    m_menuMain.SetMenuItemId(5, ID_MENU_SMV_F6);
    m_menuMain.SetMenuItemCaption(5, dsm_stringMgr::Get(DSM_STR_SMV_STATISTICS_RECOUNT));


	m_menuMain.SetMenuItemCaption(0, dsm_stringMgr::Get(DSM_STR_SMV_STATISTICS_CAPTION));
	m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);
	m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);

}


void CSMVStatisticsDlg::OnMenuF6()
{
    ASSERT(m_pDataLayer);

    // ����ͳ��
    m_pDataLayer->ResetSmvFrameStatistics();


    // ����ˢ�½���
    _update();
}

void CSMVStatisticsDlg::OnMenuF3()
{

#if !SU_FEI
	m_pNaviBar->GetNaviMenu()->HighlightMenuItem(2, TRUE);
	m_pNaviBar->GetNaviMenu()->HighlightMenuItem(3, FALSE);
	m_pNaviBar->GetNaviMenu()->HighlightMenuItem(4, FALSE);

	m_pNaviBar->UpdateNaviMenu();


	

	if (m_ctrlHistogram.GetSafeHwnd())
	{
		m_ctrlHistogram.ShowWindow(FALSE);
	}

	if (m_reportMonitor.GetSafeHwnd())
	{
		m_reportMonitor.ShowWindow(SW_HIDE);
	}


	if (m_reportStatistics.GetSafeHwnd())
	{
		m_reportStatistics.ShowWindow(SW_SHOW);
		m_reportStatistics.SetFocus();
	}
	else
	{
		_createReport();
	}

	m_modeCtr = REPORT_MODE;

	

#endif

}


#if !SU_FEI
void CSMVStatisticsDlg::OnMenuF4()
{
	m_pNaviBar->GetNaviMenu()->HighlightMenuItem(2, FALSE);
	m_pNaviBar->GetNaviMenu()->HighlightMenuItem(3, TRUE);
	m_pNaviBar->GetNaviMenu()->HighlightMenuItem(4, FALSE);

	m_pNaviBar->UpdateNaviMenu();

	

	if (m_reportStatistics.GetSafeHwnd())
	{
		m_reportStatistics.ShowWindow(SW_HIDE);
	}

	if (m_reportMonitor.GetSafeHwnd())
	{
		m_reportMonitor.ShowWindow(SW_HIDE);
	}


	if (m_ctrlHistogram.GetSafeHwnd())
	{
		m_ctrlHistogram.SetFocus();
		m_ctrlHistogram.Init();
		m_ctrlHistogram.ShowWindow(TRUE);
	}
	else
	{
		_createHorHistogram();
	}
	m_modeCtr = STATISTICS_MODE;

}

void CSMVStatisticsDlg::OnMenuF5()
{
	m_pNaviBar->GetNaviMenu()->HighlightMenuItem(2, FALSE);
	m_pNaviBar->GetNaviMenu()->HighlightMenuItem(3, FALSE);
	m_pNaviBar->GetNaviMenu()->HighlightMenuItem(4, TRUE);

	m_pNaviBar->UpdateNaviMenu();

	if (m_ctrlHistogram.GetSafeHwnd())
	{
		m_ctrlHistogram.ShowWindow(FALSE);
	}

	if (m_reportStatistics.GetSafeHwnd())
	{
		m_reportStatistics.ShowWindow(SW_HIDE);
	}

	if (m_reportMonitor.GetSafeHwnd())
	{
		m_reportMonitor.ShowWindow(SW_SHOW);
		m_reportMonitor.SetFocus();
	}
	else
	{
		_createMonitorReport();
	}
	m_modeCtr = MONITOR_MODE;

}

#endif


/**
 *      ���ݲ˵�ID�л���ָ���Ŀ��ƿ�
 *
 *  /param[in]
 *      nCmdID      ���ƿ�����Ĳ˵�����ID
 */
bool CSMVStatisticsDlg::_switchCtrlBlk( UINT nCmdID )
{
    // �л����ƿ�
    if(!__super::_switchCtrlBlk(nCmdID))
    {
        return false;
    }

	
#if SU_FEI
    // ����asdu�����������
    _resetPillarTitle();
#else
	if (m_ctrlHistogram.GetSafeHwnd())
	{
		// ����asdu�����������
		_resetPillarTitle();
	}
	
#endif
    // �������Ƶ�ʲ˵�
    //_setSampleRateMenu();

    m_csSample.Format(L" - %s(%d)", dsm_stringMgr::Get(DSM_STR_SMV_STATISTICS_SMPRAT), m_pCurrentCtrlBlk->nSampleRate);
    CStatusMgr::SetTitle(CStatusMgr::GetTitle() + m_csSample);

    return true;
}

/**
 *      ������ɢ�ȱ���
 */
void CSMVStatisticsDlg::_resetPillarTitle()
{
    // ������ɢ���������
    CString csRegion4, csRegion5;

    csRegion4.Format(L"%u��s", m_pCurrentCtrlBlk->nAsdu * 50);
    csRegion5.Format(L"%u��s", m_pCurrentCtrlBlk->nAsdu * 250);

    CString csPillarTitle[STATISTICS_PILLARS] = {
        L"0��s", 
        L"1��s", 
        L"2��s", 
        L"10��s", 
        csRegion4,
        csRegion5,
    };

    uint32_t nSamInt = m_pCurrentCtrlBlk->nAsdu * 1000000 / m_pCurrentCtrlBlk->nSampleRate;
    m_csSampleInter.Format(L" - %s(%u ��s)", dsm_stringMgr::Get(DSM_STR_SMV_STATISTICS_SAMPLEINTERVAL), nSamInt);

    // ������״ͼ��ɫ�ͱ���
    for(int i = 0; i < STATISTICS_PILLARS; i++)
    {
        m_ctrlHistogram.SetYScaleVal(i, csPillarTitle[i]);
    }

    m_ctrlHistogram.Init();

    Invalidate();
}

// void CSMVStatisticsDlg::_setSampleRateMenu()
// {
//     if(m_pCurrentCtrlBlk->enFrameType == FRAME_TYPE_NET_9_2)
//     {
//         // 92Э�����Ӳ��������ò˵�
//         CMenu  menu;
//         VERIFY(menu.CreatePopupMenu());
// 
//         CString cs4K, cs12p8K;
//         cs4K.Format(L"4K Hz");
//         cs12p8K.Format(L"12.8K Hz");
// 
//         menu.AppendMenu(MF_STRING, ID_MENU_SMV_STATISTICS_SAMPLE_4K, cs4K);
//         menu.AppendMenu(MF_STRING, ID_MENU_SMV_STATISTICS_SAMPLE_12P8K, cs12p8K);
// 
//         CNaviMenu* pSub         = new CNaviMenu;
//         pSub->LoadMenu(&menu);
//         menu.DestroyMenu();
// 
//         int nHightLihgt = 0;
// 
//         if(m_pDataLayer->GetSmvConfigPublic()->GetSmv92SampleRate() == SMV92_SAMPLE_RATE_12P8K)
//         {
//             nHightLihgt = 1;
//         }
// 
//         // ���ò����ʲ˵���������ǰ�����ʲ˵���
//         CNaviBarMgr::GetInstance()->GetNaviBar()->GetNaviMenu()->SetMenuSubItem(4, pSub);
//         CNaviBarMgr::GetInstance()->GetNaviBar()->GetNaviMenu()->SetMenuItemCaption(4, dsm_stringMgr::Get(DSM_STR_SMV_STATISTICS_SMPRAT));
//         CNaviBarMgr::GetInstance()->HighlightMenuItem(4, nHightLihgt);
//     }
//     else
//     {
//         CNaviMenu* pMenu = CNaviBarMgr::GetInstance()->GetNaviBar()->GetNaviMenu();
//         ASSERT(pMenu);
// 
//         // ȡ�����������ò˵�
//         pMenu->SetMenuSubItem(4, NULL);
//         pMenu->SetMenuItemId(4, ID_MENU_SMV_F5);
//         pMenu->SetMenuItemCaption(4, L" ");
//     }
// }
// 
// BOOL CSMVStatisticsDlg::OnCommand(WPARAM wParam, LPARAM lParam)
// {
//     // TODO: �ڴ����ר�ô����/����û���
// 
//     int nMenuID = LOWORD(wParam);
// 
//     if(nMenuID >= ID_MENU_SMV_STATISTICS_SAMPLE_4K && nMenuID < ID_MENU_SMV_STATISTICS_SAMPLE_END)
//     {
//         _setSampleRate92(nMenuID);
// 
//         return TRUE;
//     }
// 
//     return CSMVBaseDlg::OnCommand(wParam, lParam);
// }
// 
// BOOL CSMVStatisticsDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
// {
//     // TODO: �ڴ����ר�ô����/����û���
// 
//     if(nCode == CN_UPDATE_COMMAND_UI && nID >= ID_MENU_SMV_STATISTICS_SAMPLE_4K && nID < ID_MENU_SMV_STATISTICS_SAMPLE_END)
//     {
//         CCmdUI* pCmdUI = (CCmdUI*)pExtra;
//         ASSERT(pCmdUI);
// 
//         pCmdUI->Enable(TRUE);
// 
//         return TRUE;
//     }
// 
//     return CSMVBaseDlg::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
// }
// 
// void CSMVStatisticsDlg::_setSampleRate92( UINT nCmdID )
// {
//     UINT nPrevSmpRat = ID_MENU_SMV_STATISTICS_SAMPLE_4K;
//     SMV92_SAMPLE_RATE ePrevSmpRat = m_pDataLayer->GetSmvConfigPublic()->GetSmv92SampleRate();
// 
//     // ��ȡ֮ǰ�����Ĳ����ʲ˵�ID
//     if(ePrevSmpRat == SMV92_SAMPLE_RATE_12P8K)
//     {
//         nPrevSmpRat = ID_MENU_SMV_STATISTICS_SAMPLE_12P8K;
//     }
// 
//     // ѡ����ͬ�˵��򷵻�
//     if(nCmdID == nPrevSmpRat)
//     {
//         return;
//     }
// 
//     // �����µĲ�����
//     m_pDataLayer->GetSmvConfigPublic()->SetSmv92SampleRate(ePrevSmpRat == SMV92_SAMPLE_RATE_4K ? SMV92_SAMPLE_RATE_12P8K : SMV92_SAMPLE_RATE_4K);
// 
//     // ��ȡ����һ�εĸ����˵����ٸ�����ǰѡ�еĲ˵�
//     CNaviBarMgr::GetInstance()->HighlightMenuItem(4, nPrevSmpRat - ID_MENU_SMV_STATISTICS_SAMPLE_4K, FALSE);
//     CNaviBarMgr::GetInstance()->HighlightMenuItem(4, nCmdID - ID_MENU_SMV_STATISTICS_SAMPLE_4K);
// 
//     // ������״ͼ�ؼ�����
//     _resetPillarTitle();
// 
//     // ����ͳ��
//     m_pDataLayer->ResetSmvFrameStatistics();
// }
