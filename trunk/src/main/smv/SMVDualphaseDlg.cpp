/**
 * \file
 *      SMVDualphaseDlg.cpp
 *
 * \brief
 *      ����ҳ��ʵ���ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/4/21
 */


//  SMVDualphaseDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SMVDualphaseDlg.h"

#include "src/main/mgr/statusmgr.h"
#include "src/main/utils/utility.h"
#include "src/main/smv/common_smv.h"
#include "src/main/pagedata/pagedata.h"
#include "src/utils/utility/dsm_utility.h"


#define DSM_PHASOR_REPORT_VAL_RECT          CRect(316, DSM_P_BODY_TOP, 640, DSM_P_BODY_BOTTOM)      ///< ��Чֵ��������

#define RECT_SETV(rc,l,t,r,b)   rc.left = l; \
                                rc.top = t; \
                                rc.right = r; \
                                rc.bottom = b;

#define GRAM_TIPS_COLOR         RGB(128,128,135)      ///< ů��ɫ������ͼ���ı�������ɫ

#define PHASOR_COL0             0               ///< ��Чֵ������
#define PHASOR_COL1             1               ///< ͬ��/��������

#define PHASOR_A                1               ///< SMV��1��1��ͨ������
#define PHASOR_B                2               ///< SMV��1��2��ͨ������
#define PHASOR_C                3               ///< SMV��1��3��ͨ������
#define PHASOR_a                6               ///< SMV��2��1��ͨ������
#define PHASOR_b                7               ///< SMV��2��2��ͨ������
#define PHASOR_c                8               ///< SMV��2��3��ͨ������

#define PHASOR_FREQ1            0               ///< SMV��1Ƶ������
#define PHASOR_FREQ2            5               ///< SMV��2Ƶ������

#if !CHUN_HUA
extern COLORREF m_Color[];// ={DSM_PHASE_COLOR_A,DSM_PHASE_COLOR_B,DSM_PHASE_COLOR_C,DSM_PHASE_COLOR_N};
#endif

namespace SMV_DUALPHASOR
{
    /**
     *      ���ݸ�ʽ���ַ����������ȡ��ʽ���ַ���
     *
     *  \param[in]
     *      csFmt           ��ʽ���ַ���
     *  \param[in]
     *      fvalue          ��ʽ������
     *  \return
     *      CString         ���ظ�ʽ���õ��ַ���
     */
    inline CString g_GetResultString(const CString csFmt, float fvalue)
    {
        CString csRet;

        csRet.Format(csFmt, fvalue);

        return csRet;
    }

    /**
     *      ���ݸ�ʽ���ַ����������ȡ��ʽ���ַ���
     *
     *  \param[in]
     *      csFmt           ��ʽ���ַ���
     *  \param[in]
     *      csUnit          ��λ
     *  \param[in]
     *      fvalue1         ��ʽ������1
     *  \param[in]
     *      fvalue2         ��ʽ������2
     *  \return
     *      CString         ���ظ�ʽ���õ��ַ���
     */
    inline CString g_GetResultString(const CString csFmt, const CString csUnit, float fvalue1, float fvalue2)
    {
        CString csRet;

        csRet.Format(csFmt, fvalue1, csUnit, fvalue2);

        return csRet;
    }
}


/**
 *      SMV���� - ���� ���ƿ��¼
 */

class CDsmSmvDualPhaseCtrlBlkRecord : public CELRecord
{
public:
    CDsmSmvDualPhaseCtrlBlkRecord(CDsmDataLayer* pDataLayer, CString csCaption, CString csSubCaption);
    virtual ~CDsmSmvDualPhaseCtrlBlkRecord() {}
};

/**
 *      SMV���� - ���� ͨ����¼
 */
class CDsmSmvDualPhaseChannelRecord : public CELRecord
{
public:
    CDsmSmvDualPhaseChannelRecord(CBaseDetectItem* pCtrlBlk, CString csCaption);
    virtual ~CDsmSmvDualPhaseChannelRecord() {}

public:

    /**
     *      ����ָ�����ƿ����ͨ���б�
     */
    void UpdateChannels(CBaseDetectItem* pCtrlBlk);

private:

    /**
     *      ����ָ�����ƿ��ͨ���б�
     */
    void _loadChannels(CBaseDetectItem* pCtrlBlk, CELRecordItem* pItem);
};


CDsmSmvDualPhaseChannelRecord::CDsmSmvDualPhaseChannelRecord( CBaseDetectItem* pCtrlBlk, CString csCaption )
{
    AddItem(new CDsmRecordItem(csCaption));
    CELRecordItem* pItem = AddItem(new CDsmRecordItem(g_csBlankValue));

    if(pCtrlBlk)
    {
        pItem->SetEditable(TRUE);
        pItem->GetEditOptions(NULL)->AddComboButton();

        // ����ͨ���б�
        _loadChannels(pCtrlBlk, pItem);
    }
}

void CDsmSmvDualPhaseChannelRecord::UpdateChannels( CBaseDetectItem* pCtrlBlk )
{
    CELRecordItem* pItem = GetItem(1);

    if(pCtrlBlk && pItem)
    {
        // ����ͨ���б�
        _loadChannels(pCtrlBlk, pItem);
    }
}

void CDsmSmvDualPhaseChannelRecord::_loadChannels( CBaseDetectItem* pCtrlBlk, CELRecordItem* pItem )
{
    ASSERT(pCtrlBlk);
    ASSERT(pItem);

    CSmvRecvConfig* pConfig         = pCtrlBlk->pRecvConfig;
    CSmvRecvChannelInfo* pInfo      = NULL;
    size_t nCounts                  = pConfig->GetChannelCount();
    CELRecordItemEditOptions* pEdit = pItem->GetEditOptions(NULL);
    ASSERT(pEdit);

    // �Ƴ���ͨ������
    pEdit->GetConstraints()->RemoveAll();

    for(size_t i = 0; i < nCounts; i++)
    {
        pInfo = pConfig->GetOneChannelInfo(i);

        if(CHANNEL_TYPE_SMV_VOLTAGE != pInfo->enChannelType &&
            CHANNEL_TYPE_SMV_CURRENT != pInfo->enChannelType)
        {
            continue;
        }

        pEdit->AddConstraint(pInfo->strChannelDesc.c_str(), pInfo->nIndex);
    }
}

CDsmSmvDualPhaseCtrlBlkRecord::CDsmSmvDualPhaseCtrlBlkRecord( CDsmDataLayer* pDataLayer, CString csCaption, CString csSubCaption )
{
    ASSERT(pDataLayer);

    AddItem(new CDsmRecordItem(csCaption));
    CELRecordItem* pItem = AddItem(new CDsmRecordItem(g_csBlankValue));

    CBaseDetectItem* pCtrlBlkBase       = pDataLayer->GetFirstSelectCtrlBlk();;
    CELRecordItemEditOptions* pOptions  = pItem->GetEditOptions(NULL);

    pItem->SetEditable(TRUE);
    pOptions->AddComboButton();

    CString csCtrlBlk;

    while(pCtrlBlkBase)
    {
        csCtrlBlk = DSM_GetCtrlBlkInfo(pCtrlBlkBase);
        pOptions->AddConstraint(csCtrlBlk, pCtrlBlkBase->nID);

        pCtrlBlkBase    = pDataLayer->GetNextSelectCtrlBlk(pCtrlBlkBase);
    }
}




// CSMVDualphaseDlg �Ի���

IMPLEMENT_DYNAMIC(CSMVDualphaseDlg, DSM_SMVBASEDLG)

CSMVDualphaseDlg::CSMVDualphaseDlg(CWnd* pParent /*=NULL*/)
	: DSM_SMVBASEDLG(CSMVDualphaseDlg::IDD, DSM_STR_SMV_DUALPHASE_CAPTION, pParent)
    , m_pCtrlBlkA(NULL)
    , m_pCtrlBlkB(NULL)
    , m_pPageDataExtraDual(NULL)
    , m_bGram(true)
    , m_bSamePhase(true)
    , m_bLastStatus(true)
     , m_fScaleInterval(0.0f)
    , m_csFmtFreq(L"Freq = %0.3f Hz")
    , m_csFmtEffect(L"%0.3f%s/%0.3f��")
{
    for(int i = 0; i < 3; i++)
    {
        m_fInPhaseAngleDiff[i]    = 0.0f;

        m_stPhaseVal1[i].m_fPhase = 0.0f;
        m_stPhaseVal1[i].m_fValue = 0.0f;

        m_stPhaseVal2[i].m_fPhase = 0.0f;
        m_stPhaseVal2[i].m_fValue = 0.0f;

        m_stPhaseData1[i].m_fPhase = 0.0f;
        m_stPhaseData1[i].m_fValue = 0.0f;

        m_stPhaseData2[i].m_fPhase = 0.0f;
        m_stPhaseData2[i].m_fValue = 0.0f;
    }

    for(int i = 0; i < 6; i++)
    {
        m_csDesc[i] = dsm_stringMgr::Get(DSM_STR_SMV_DUALPHASE_PHASE_A1 + i);
    }
}

CSMVDualphaseDlg::~CSMVDualphaseDlg()
{
    CStatusMgr::SetTips(L"", 0);
}

void CSMVDualphaseDlg::DoDataExchange(CDataExchange* pDX)
{
	DSM_SMVBASEDLG::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSMVDualphaseDlg, DSM_SMVBASEDLG)
    ON_COMMAND(ID_MENU_SMV_F2, &CSMVDualphaseDlg::OnMenuF2)
   // ON_COMMAND(ID_MENU_SMV_F3, &CSMVDualphaseDlg::OnMenuF3)
    ON_COMMAND(ID_MENU_SMV_F4, &CSMVDualphaseDlg::OnMenuF4)
    ON_COMMAND(ID_MENU_SMV_F5, &CSMVDualphaseDlg::OnMenuF5)
    ON_NOTIFY(ELNM_ITEM_CONSTRAINTCHANGED, IDC_SMV_DULAPHASE_REPORT, &CSMVDualphaseDlg::OnItemSelectChange)
    ON_NOTIFY(ELNM_INPLACEBUTTONDOWN, IDC_SMV_DULAPHASE_REPORT, &CSMVDualphaseDlg::OnInplaceButtonDown)
    ON_WM_TIMER()
END_MESSAGE_MAP()


BOOL CSMVDualphaseDlg::OnInitDialog()
{
    DSM_SMVBASEDLG::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��

    // ֵ�����������Ż�
    _createReportVal();
    _createReport();
    _createDualphaseGram();

    // û���麯��������
    _updateMenuNavi();

    // ����ҳ��״̬���ݵ�״̬ģʽ��Ϣ���л�״̬
    _switchF4();        // ͼ��/����
    _switchF5();        // ���/ͬ��

    // ������ʱ��
    _beginTimer(800);

    m_reportPhase.SetFocus();

    return FALSE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}


// CSMVDualphaseDlg ��Ϣ�������


/**
 *      ��������ͼ
 */
void CSMVDualphaseDlg::_createDualphaseGram()
{
    CRect rcPhasor(DSM_PHASOR_PHASORGRAM_RECT);
    DSM_ParentToChild(this, rcPhasor);

    m_ctrlPhaseGram.Create(NULL, WS_CHILD | WS_VISIBLE, rcPhasor, this, IDC_SMV_DUALPHASE_PHASEGRAM);

    m_ctrlPhaseGram.ModifyStyle(0, SS_OWNERDRAW);

    // �������������ɫ
    m_ctrlPhaseGram.SetPhasorColor(0, DSM_PHASE_COLOR_A);
    m_ctrlPhaseGram.SetPhasorColor(1, DSM_PHASE_COLOR_B);
    m_ctrlPhaseGram.SetPhasorColor(2, DSM_PHASE_COLOR_C);
    m_ctrlPhaseGram.SetPhasorColor(3, DSM_PHASE_COLOR_A);
    m_ctrlPhaseGram.SetPhasorColor(4, DSM_PHASE_COLOR_B);
    m_ctrlPhaseGram.SetPhasorColor(5, DSM_PHASE_COLOR_C);

    m_ctrlPhaseGram.GetClientRect(rcPhasor);

    // ����ͼ����Ϣ����
    RECT_SETV(m_rcAa, rcPhasor.right - 110, rcPhasor.top + 10, rcPhasor.right - 10, rcPhasor.top + 34)
    RECT_SETV(m_rcBb, rcPhasor.left + 30, rcPhasor.top + 10, rcPhasor.left + 130, rcPhasor.top + 34)
    RECT_SETV(m_rcCc, rcPhasor.left + 30, rcPhasor.bottom - 34, rcPhasor.left + 130, rcPhasor.bottom -10)
    RECT_SETV(m_rcF1, rcPhasor.right - 110, rcPhasor.bottom - 58, rcPhasor.right - 10, rcPhasor.bottom - 34)
    RECT_SETV(m_rcF2, rcPhasor.right - 110, rcPhasor.bottom - 34, rcPhasor.right - 10, rcPhasor.bottom - 10)

    // ��������ͼ��ʽ
    m_ctrlPhaseGram.SetPhasorExtDescFont(dsm_fontmgr::GetInstance()->GetFont(DSM_ELEC_CTRL_DEFT_FT));
    m_ctrlPhaseGram.SetPhasorDescFont(dsm_fontmgr::GetInstance()->GetFont(DSM_ELEC_CTRL_DEFT_FT));
    m_ctrlPhaseGram.SetBkgndColor(DSM_COLOR_BK);
    CRect rcMargin(DSM_PHASOR_MARGIN);
    m_ctrlPhaseGram.SetMargin(rcMargin.left, rcMargin.right, rcMargin.top, rcMargin.bottom);
    m_ctrlPhaseGram.Init();
}

/**
 *      ��ʼ������ҵ���smv1��smv2���ƿ��Լ���ѡ�е�ABC����ͨ��
 */
void CSMVDualphaseDlg::_initPageStatus()
{
    ASSERT(m_pPageDataExtraDual);

    // ��ȡ����ҳ��״̬����
    size_t nCounts  = m_pDataLayer->GetSelectedCtrlBlkCount();
    m_pCtrlBlkA     = m_pDataLayer->GetSelectCtrlBlkByID(m_pPageDataExtraDual->nSmv1);      // ��ȡsmv1
    m_pCtrlBlkB     = m_pDataLayer->GetSelectCtrlBlkByID(m_pPageDataExtraDual->nSmv2);      // ��ȡsmv2
    m_bGram         = m_pPageDataExtraDual->bGram;                                          // ��ȡͼ��ģʽ
    m_bSamePhase    = m_pPageDataExtraDual->bSame;                                          // ��ȡ���ģʽ

    // ��smv1��Ч������б�ѡһ��
    if(!m_pCtrlBlkA)
    {
        m_pCtrlBlkA = m_pDataLayer->GetFirstSelectCtrlBlk();

        // ѡ�п��ƿ���������1�����ѡ��һ����smv2����ͬ�Ŀ��ƿ�
        if(nCounts > 1)
        {
            for(size_t i = 0; i < nCounts; i++)
            {
                if(!m_pCtrlBlkB || m_pCtrlBlkA->nID != m_pCtrlBlkB->nID)
                {
                    break;
                }

                m_pCtrlBlkA = m_pDataLayer->GetNextSelectCtrlBlk(m_pCtrlBlkA);
            }
        }

        // ָ���Ŀ��ƿ�Ѱ�Һ��ʵ�ABC����ͨ����ʾ
        m_pPageDataExtraDual->nSmv1 = m_pCtrlBlkA->nID;
        _calcChannel(m_pCtrlBlkA, ORDER_PHASE_A);
    }

    if(!m_pCtrlBlkB)
    {
        m_pCtrlBlkB = m_pDataLayer->GetFirstSelectCtrlBlk();

        // ѡ�п��ƿ���������1�����ѡ��һ����smv1����ͬ�Ŀ��ƿ�
        if(nCounts > 1)
        {
            for(size_t i = 0; i < nCounts; i++)
            {
                if(!m_pCtrlBlkA || m_pCtrlBlkA->nID != m_pCtrlBlkB->nID)
                {
                    break;
                }

                m_pCtrlBlkB = m_pDataLayer->GetNextSelectCtrlBlk(m_pCtrlBlkB);
            }
        }

        // ָ���Ŀ��ƿ�Ѱ�Һ��ʵ�ABC����ͨ����ʾ
        m_pPageDataExtraDual->nSmv2 = m_pCtrlBlkB->nID;
        _calcChannel(m_pCtrlBlkB, ORDER_PHASE_B);
    }

    // ���ú����smv1��smv2
    m_pDataLayer->SetPhaseCheckSmv1CtrBlk(m_pCtrlBlkA);
    m_pDataLayer->SetPhaseCheckSmv2CtrBlk(m_pCtrlBlkB);

    uint32_t nChannelA[3] = {UINT_MAX, UINT_MAX, UINT_MAX};
    uint32_t nChannelB[3] = {UINT_MAX, UINT_MAX, UINT_MAX};

    for(int i = 0; i < 3; i++)
    {
        if(m_pPageDataExtraDual->bChannelValidA[i])
        {
            nChannelA[i] = m_pPageDataExtraDual->nChannelA[i];
        }

        if(m_pPageDataExtraDual->bChannelValidB[i])
        {
            nChannelB[i] = m_pPageDataExtraDual->nChannelB[i];
        }
    }

    // ���ú���ҵ���smv1��smv2ѡ�е�ͨ����
    m_pDataLayer->SetSmv1ABCChannles(nChannelA[0], nChannelA[1], nChannelA[2]);
    m_pDataLayer->SetSmv2ABCChannles(nChannelB[0], nChannelB[1], nChannelB[2]);
}

/**
 *      ���������౨��
 */
void CSMVDualphaseDlg::_createReport()
{
    CRect rcReport(DSM_PHASOR_REPORT_RECT);
    DSM_ParentToChild(this, rcReport);
    int nHeight = ELT_T1_ROWHEIGHT * 9;

    // �����߶�
    rcReport.top = (rcReport.Height() - nHeight) / 2;
    rcReport.bottom = rcReport.top + nHeight;

    //rcReport.OffsetRect(DSM_MARGIN_IN, 0);
	rcReport.DeflateRect(0, 0, 0, 0);

    m_reportPhase.Create(DSM_WINSTYLE_REPORT_DEFAULT, rcReport, this, IDC_SMV_DULAPHASE_REPORT);

    // ��������
    m_reportPhase.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    // ������ʽ
    m_reportPhase.PostInitTemplateStyle();
    m_reportPhase.SetHeaderHeight(0);
	m_reportPhase.SetHasGridline(FALSE, TRUE);

    // ������Ԫ��
    m_reportPhase.InsertColumn(PHASOR_COL0, L"", EL_CENTER, DSM_PHASOR_REPORT_CAPTION);     // ����
    m_reportPhase.InsertColumn(PHASOR_COL1, L"", EL_CENTER, DSM_PHASOR_REPORT_VALUE);       // ����

    // ����ѡ�񽹵���ʽ
    m_reportPhase.SetRowSelectionVisible(FALSE);
    m_reportPhase.SetRowFocusEnable(TRUE);
    m_reportPhase.SetSubItemFocusEnable(TRUE);
    m_reportPhase.SetFreezeColumnsCount(1);
    m_reportPhase.SetFocusedColIndex(1);

    // ���ñ����ֵ
    _initReport();
}

/**
 *      ������Чֵ��
 */
void CSMVDualphaseDlg::_createReportVal()
{
    CRect rcReport(DSM_PHASOR_REPORT_VAL_RECT);
    DSM_ParentToChild(this, rcReport);

    int nHeight = ELT_T1_ROWHEIGHT * 9;

    // �����߶�
    rcReport.top = (rcReport.Height() - nHeight) / 2;
    rcReport.bottom = rcReport.top + nHeight;

   // rcReport.DeflateRect(DSM_MARGIN_IN, 0, DSM_MARGIN_IN, 0);
	rcReport.DeflateRect(0, 0, 0, 0);
	
    m_reportValue.Create(WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL , 
        rcReport, this, IDC_SMV_DULAPHASE_VALUE_REPORT);

    // ������ʽ
    m_reportValue.PostInitTemplateStyle();

    m_reportValue.SetHeaderHeight(0);
	m_reportValue.SetHasGridline(FALSE, TRUE);

    // ��������
    m_reportValue.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    // ������Ԫ��
    m_reportValue.InsertColumn(PHASOR_COL0, L"", EL_CENTER, DSM_PHASOR_REPORT_EFFECTIVE);
    m_reportValue.InsertColumn(PHASOR_COL1, L"", EL_CENTER, DSM_PHASOR_REPORT_ANGEL);

    // ����ѡ�񽹵���ʽ
    m_reportValue.SetRowSelectionVisible(FALSE);
    m_reportValue.SetRowFocusEnable(FALSE);

    for(int i = 0; i < 9; i++)
    {
        m_reportValue.InsertRecordRow(i, new CDsmSmvDualPhaseChannelRecord(NULL, L""));
    }
	m_reportValue.SetRowItemText(4, 1, L"");
    
	

    // ��ʼ��item����
    m_arrayItem[0] = m_reportValue.GetRecordRow(PHASOR_FREQ1)->GetItem(PHASOR_COL0);
    m_arrayItem[1] = m_reportValue.GetRecordRow(PHASOR_FREQ2)->GetItem(PHASOR_COL0);
    m_arrayItem[2] = m_reportValue.GetRecordRow(PHASOR_A)->GetItem(PHASOR_COL0);
    m_arrayItem[3] = m_reportValue.GetRecordRow(PHASOR_B)->GetItem(PHASOR_COL0);
    m_arrayItem[4] = m_reportValue.GetRecordRow(PHASOR_C)->GetItem(PHASOR_COL0);
    m_arrayItem[5] = m_reportValue.GetRecordRow(PHASOR_a)->GetItem(PHASOR_COL0);
    m_arrayItem[6] = m_reportValue.GetRecordRow(PHASOR_b)->GetItem(PHASOR_COL0);
    m_arrayItem[7] = m_reportValue.GetRecordRow(PHASOR_c)->GetItem(PHASOR_COL0);
    m_arrayItem[8] = m_reportValue.GetRecordRow(PHASOR_A)->GetItem(PHASOR_COL1);
    m_arrayItem[9] = m_reportValue.GetRecordRow(PHASOR_B)->GetItem(PHASOR_COL1);
    m_arrayItem[10] = m_reportValue.GetRecordRow(PHASOR_C)->GetItem(PHASOR_COL1);
    m_arrayItem[11] = m_reportValue.GetRecordRow(PHASOR_a)->GetItem(PHASOR_COL1);
    m_arrayItem[12] = m_reportValue.GetRecordRow(PHASOR_b)->GetItem(PHASOR_COL1);
    m_arrayItem[13] = m_reportValue.GetRecordRow(PHASOR_c)->GetItem(PHASOR_COL1);
}


void CSMVDualphaseDlg::_initReport()
{
    int nIndex = 0;

    // ����smv1�Ŀ��ƿ���ͨ����
    m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvDualPhaseCtrlBlkRecord(m_pDataLayer, dsm_stringMgr::Get(DSM_STR_SMV_DUALPHASE_SMV1), dsm_stringMgr::Get(DSM_STR_SMV_DUALPHASE_SAMEANGDIFF)));
    m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvDualPhaseChannelRecord(m_pCtrlBlkA, dsm_stringMgr::Get(DSM_STR_SMV_DUALPHASE_A)));
    m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvDualPhaseChannelRecord(m_pCtrlBlkA, dsm_stringMgr::Get(DSM_STR_SMV_DUALPHASE_B)));
    m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvDualPhaseChannelRecord(m_pCtrlBlkA, dsm_stringMgr::Get(DSM_STR_SMV_DUALPHASE_C)));

    int nFocusDisable = m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvDualPhaseChannelRecord(NULL, L""));

    m_reportPhase.SetRowItemText(nFocusDisable, 1, L"");

    // ����smv�Ŀ��ƿ���ͨ����
    m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvDualPhaseCtrlBlkRecord(m_pDataLayer, dsm_stringMgr::Get(DSM_STR_SMV_DUALPHASE_SMV2), dsm_stringMgr::Get(DSM_STR_SMV_DUALPHASE_SAMEDIFF)));
    m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvDualPhaseChannelRecord(m_pCtrlBlkB, dsm_stringMgr::Get(DSM_STR_SMV_DUALPHASE_a)));
    m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvDualPhaseChannelRecord(m_pCtrlBlkB, dsm_stringMgr::Get(DSM_STR_SMV_DUALPHASE_b)));
    m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvDualPhaseChannelRecord(m_pCtrlBlkB, dsm_stringMgr::Get(DSM_STR_SMV_DUALPHASE_c)));

    // �м�Ŀհ��н�ֹѡ��
    m_reportPhase.GetRecordRow(nFocusDisable)->SetFocusable(FALSE);

    // ����ҳ��״̬��Ϣ�Զ�����smv���ƿ���ѡ�еı���
    _adjustChannel();
}


/**
 *      ����ҳ��״̬��Ϣ�Զ�����smv���ƿ���ѡ�еı���
 */
void CSMVDualphaseDlg::_adjustChannel()
{
    ASSERT(m_pPageDataExtraDual);

    _setChannel(ORDER_PHASE_A);         // ����smv1
    _setChannel(ORDER_PHASE_B);         // ����smv2

    // ���ý���
    if(m_pPageDataExtraDual->nFoucs >= 0 && m_pPageDataExtraDual->nFoucs < m_reportPhase.GetRecordRowCount())
    {
        m_reportPhase.SetFocusedSelectionRowIndex(m_pPageDataExtraDual->nFoucs);
    }
}

/**
 *      ����ָ����Ϣ����һ��ͨ�����������ÿ��ƿ��Լ�ABC����ͨ��
 */
void CSMVDualphaseDlg::_setChannel( ePhaseOrder order )
{
    ASSERT(m_pDataLayer);
    ASSERT(m_pPageDataExtraDual);

    int nIndex                  = 0;
    bool* pValid                = NULL;
    uint32_t* pChannels         = NULL;
    CBaseDetectItem* pCtrlBlk   = NULL;

    // ����ͨ�ñ���
    if(ORDER_PHASE_A == order)
    {
        pCtrlBlk        = m_pCtrlBlkA;
        nIndex          = 0;
        pValid          = &m_pPageDataExtraDual->bChannelValidA[0];
        pChannels       = &m_pPageDataExtraDual->nChannelA[0];
    }
    else
    {
        pCtrlBlk        = m_pCtrlBlkB;
        nIndex          = 5;
        pValid          = &m_pPageDataExtraDual->bChannelValidB[0];
        pChannels       = &m_pPageDataExtraDual->nChannelB[0];
    }

    //
    // ����ҳ�����ݻָ�����ѡ��
    //

    // ����ѡ�п��ƿ������
    CELRecordItemConstraint* pCons = m_reportPhase.GetRecordRow(nIndex)->GetItem(PHASOR_COL1)->GetEditOptions(NULL)->FindConstraint(pCtrlBlk->nID);
    ASSERT(pCons);

    m_reportPhase.SetRowItemText(nIndex, PHASOR_COL1, pCons->GetConstraintCaption());

    nIndex++;   // �ۼӵ�ͨ��row

    // ���ÿ��ƿ�ѡ�е�ͨ������
    for(int i = 0; i < 3; i++)
    {
        if(pChannels[i] != UINT_MAX)
        {
            pCons = m_reportPhase.GetRecordRow(nIndex)->GetItem(PHASOR_COL1)->GetEditOptions(NULL)->FindConstraint(pChannels[i]);

            if(pCons)
            {
                pValid[i] = true;

                m_reportPhase.SetRowItemText(nIndex, PHASOR_COL1, pCons->GetConstraintCaption());
            }
            else
            {
                pValid[i] = false;

                m_reportPhase.SetRowItemText(nIndex, PHASOR_COL1, g_csBlankValue);
            }
        }

        // ������ͨ���ĵ�λ
        _updateUnit(pCtrlBlk, nIndex++, pChannels[i]);
    }
}

/**
 *      Ϊָ���Ŀ��ƿ�Ѱ�Һ��ʵ�ABC����ͨ����ʾ
 *
 *  \param[in]
 *      pCtrlBlk         ָ���Ŀ��ƿ����
 *  \param[in]
 *      order           smv����smv1����smv2
 */
void CSMVDualphaseDlg::_calcChannel( CBaseDetectItem* pCtrlBlk, ePhaseOrder order )
{
    ASSERT(pCtrlBlk);
    ASSERT(m_pPageDataExtraDual);

    CSmvRecvConfig* pCfg        = pCtrlBlk->pRecvConfig;
    ASSERT(pCfg);

    CSmvRecvChannelInfo* pInfo  = NULL;
    size_t nCounts              = pCfg->GetChannelCount();

    for(size_t i = 0; i < nCounts; i++)
    {
        pInfo = pCfg->GetOneChannelInfo(i);
        ASSERT(pInfo);

        // ѡȡ�״γ��ֵ�ѹ���͵�����
        if(CHANNEL_TYPE_SMV_VOLTAGE == pInfo->enChannelType || 
            CHANNEL_TYPE_SMV_CURRENT == pInfo->enChannelType)
        {
            enumChannelType eType = pInfo->enChannelType;

            // ����˳���ABC����ͨ��
            int nA = _findChannel(pCtrlBlk, eType, CHANNEL_PHASE_A, i);
            int nB = _findChannel(pCtrlBlk, eType, CHANNEL_PHASE_B, nA + 1);
            int nC = _findChannel(pCtrlBlk, eType, CHANNEL_PHASE_C, nB + 1);

            // ��û�У����i��ʼ�ҷ�˳���ABC����ͨ��
            if(nA == UINT_MAX || nB == UINT_MAX || nC == UINT_MAX)
            {
                nA = _findChannel(pCtrlBlk, eType, CHANNEL_PHASE_A, i);
                nB = _findChannel(pCtrlBlk, eType, CHANNEL_PHASE_B, i);
                nC = _findChannel(pCtrlBlk, eType, CHANNEL_PHASE_C, i);
            }

            // ���ҵ����ABC����ͨ���������õ�ҳ��״̬������
            if(nA != UINT_MAX && nB != UINT_MAX && nC != UINT_MAX)
            {
                if(ORDER_PHASE_A == order)
                {
                    m_pPageDataExtraDual->nChannelA[0] = nA;
                    m_pPageDataExtraDual->nChannelA[1] = nB;
                    m_pPageDataExtraDual->nChannelA[2] = nC;

                    for(int k = 0; k < 3; k++)
                    {
                        m_pPageDataExtraDual->bChannelValidA[k] = true;
                    }
                }
                else
                {
                    m_pPageDataExtraDual->nChannelB[0] = nA;
                    m_pPageDataExtraDual->nChannelB[1] = nB;
                    m_pPageDataExtraDual->nChannelB[2] = nC;

                    for(int k = 0; k < 3; k++)
                    {
                        m_pPageDataExtraDual->bChannelValidB[k] = true;
                    }
                }

                // ������ÿ��ƿ��µ�ͨ��ֵ���˳�
                return;
            }
            else
            {
                // �뿪ѭ��
                break;
            }
        }
    }

    // û�ҵ����ʵ�ͨ��ֵ�������ѡ�������͵�ͨ��

    bool* pValids           = &m_pPageDataExtraDual->bChannelValidA[0];
    uint32_t* pChannels     = &m_pPageDataExtraDual->nChannelA[0];

    if(ORDER_PHASE_B == order)
    {
        pValids     = &m_pPageDataExtraDual->bChannelValidB[0];
        pChannels   = &m_pPageDataExtraDual->nChannelB[0];
    }


    // ���ѡȡ��������
    for(size_t i = 0, j = 0; i < nCounts && j < 3; i++)
    {
        pInfo = pCfg->GetOneChannelInfo(i);
        ASSERT(pInfo);

        if(CHANNEL_TYPE_SMV_CURRENT == pInfo->enChannelType || CHANNEL_TYPE_SMV_VOLTAGE == pInfo->enChannelType)
        {
            pValids[j]      = true;
            pChannels[j]    = pInfo->nIndex;
        }
    }

    // ����Ƿ�����ͨ����������ͨ����

    if(pChannels[0] != UINT_MAX)
    {
        for(int i = 0; i < 3; i++)
        {
            if(pChannels[i] == UINT_MAX)
            {
                pValids[i]      = true;
                pChannels[i]    = pChannels[0];
            }
        }
    }
}


/**
 *      ���ط���������ͨ����
 *
 *  \param[in]
 *      pCtrlBlk        ָ���Ŀ��ƿ�
 *  \param[in]
 *      eType           ָ����ͨ������
 *  \param[in]
 *      ePhase          ָ����ͨ�����
 *  \param[in]
 *      nStart          ͨ���Ĳ�ѯ��ʼλ��
 *  \return
 *      uint32_t        ���ط���������ͨ���ţ��Ҳ����򷵻� UINT_MAX
 */
uint32_t CSMVDualphaseDlg::_findChannel( CBaseDetectItem* pCtrlBlk, enumChannelType eType, ChannelPhase ePhase, int nStart )
{
    ASSERT(pCtrlBlk);

    CSmvRecvConfig* pCfg        = pCtrlBlk->pRecvConfig;
    size_t nCounts              = pCfg->GetChannelCount();
    CSmvRecvChannelInfo* pInfo  = NULL;

    // ��ָ��λ�ÿ�ʼ���Һ���������ͨ��
    for(size_t i = nStart; i < nCounts; i++)
    {
        pInfo = pCfg->GetOneChannelInfo(i);

        if(pInfo && 
            eType == pInfo->enChannelType && 
            ePhase == pInfo->enChannelPhase)
        {
            return pInfo->nIndex;
        }
    }

    return UINT_MAX;
}


/**
 *      ����ҳ��
 */
void CSMVDualphaseDlg::_SavePage()
{
    ASSERT(m_pPageDataExtraDual);

    m_pPageDataExtraDual->nFoucs = m_reportPhase.GetFocusedRowIndex();

    m_pPageDataExtraDual->bGram = m_bGram;          // ����ͼ�α��ģʽ
    m_pPageDataExtraDual->bSame = m_bSamePhase;     // ���ñ������ģʽ
}

void CSMVDualphaseDlg::OnItemSelectChange( NMHDR* pNotifyStruct, LRESULT * pResult )
{
    ASSERT(m_pPageDataExtraDual);

    ELNM_INPLACELIST_ARGS* pItemNotify = (ELNM_INPLACELIST_ARGS*)pNotifyStruct;
    ASSERT(pItemNotify);

    if(ELNM_ITEM_CONSTRAINTCHANGED != pItemNotify->hdr.code)
    {
        *pResult = FALSE;
        return;
    }

    uint32_t nChannel[3] = {UINT_MAX, UINT_MAX, UINT_MAX};
    uint32_t nSelChan = (uint32_t)pItemNotify->pConstraint->GetConstraintData();

    switch(pItemNotify->rowIndex)
    {
        // �ı�smv1��smv2���ƿ�
    case 0:
        _adjustCtrlBlk(ORDER_PHASE_A, nSelChan);
        break;

    case 5:
        _adjustCtrlBlk(ORDER_PHASE_B, nSelChan);
        break;

    case 1:
        // A��ͨ���ı䣬��������BC��ͨ����һ��ı䣨��ѯһ��ABCͨ����
        _adjustABC(ORDER_PHASE_A, m_pCtrlBlkA, nSelChan);

    case 2:
    case 3:

        // �����ı�ABCͨ��
        m_pPageDataExtraDual->bChannelValidA[pItemNotify->rowIndex - 1] = true;
        m_pPageDataExtraDual->nChannelA[pItemNotify->rowIndex - 1]      = nSelChan;

        for(int i = 0; i < 3; i++)
        {
            if(m_pPageDataExtraDual->bChannelValidA[i])
            {
                nChannel[i] = m_pPageDataExtraDual->nChannelA[i];
            }
        }

        m_pDataLayer->SetSmv1ABCChannles(nChannel[0], nChannel[1], nChannel[2]);
        _updateUnit(m_pCtrlBlkA, pItemNotify->rowIndex, nSelChan);

        break;

    case 6:

        // A��ͨ���ı䣬��������BC��ͨ����һ��ı䣨��ѯһ��ABCͨ����
        _adjustABC(ORDER_PHASE_B, m_pCtrlBlkB, nSelChan);

    case 7:
    case 8:
        // �����ı�ABCͨ��
        m_pPageDataExtraDual->bChannelValidB[pItemNotify->rowIndex - 6] = true;
        m_pPageDataExtraDual->nChannelB[pItemNotify->rowIndex - 6]      = nSelChan;

        for(int i = 0; i < 3; i++)
        {
            if(m_pPageDataExtraDual->bChannelValidB[i])
            {
                nChannel[i] = m_pPageDataExtraDual->nChannelB[i];
            }
        }

        m_pDataLayer->SetSmv2ABCChannles(nChannel[0], nChannel[1], nChannel[2]);
        _updateUnit(m_pCtrlBlkB, pItemNotify->rowIndex, nSelChan);

        break;

    default:
        break;
    }

    *pResult = TRUE;
}


void CSMVDualphaseDlg::OnMenuF2()
{

}


void CSMVDualphaseDlg::OnMenuF4()
{
    if(m_bChannelChange)
    {
        return;
    }

    m_bGram = m_bGram ? false : true;

    // ˢ�²˵�
    _updateMenuNavi();

    // �л�������ʾ
    _switchF4();
}

void CSMVDualphaseDlg::OnMenuF5()
{
    if(m_bChannelChange)
    {
        return;
    }

    if(!m_bGram)
    {
        m_bSamePhase = m_bSamePhase ? false : true;

        _updateMenuNavi();

        // �л�������ʾͬ����������
        _switchF5();
    }
}

//void CSMVDualphaseDlg::OnMenuF3()
//{
//    if(m_bChannelChange)
//    {
//        return;
//    }
//
//#if SU_FEI
//    // ������ʱ��
//    if(0 == m_nTimerID)
//    {
//        m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, dsm_stringMgr::Get(DSM_STR_TIMER_STOP));
//        m_pNaviBar->UpdateNaviMenu();
//        _beginTimer();
//    }
//    // ֹͣ��ʱ��
//    else
//    {
//        m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, dsm_stringMgr::Get(DSM_STR_TIMER_START));
//        m_pNaviBar->UpdateNaviMenu();
//        _stopTimer();
//
//        // ��ͣʱ�������ͼ�ε�����Ҫ����һ��
//
//        // ͼ��ģʽʱ����Ҫˢ�±��������
//        if(m_bGram)
//        {
//            _updateDataReport();
//        }
//        // ����ģʽʱ����Ҫˢ��ͼ�ε�����
//        else
//        {
//            _updateDataGram();
//        }
//    }
//#endif
//}

void CSMVDualphaseDlg::_modifyMenu()
{
    __super::_modifyMenu();

	m_menuMain.SetMenuItemCaption(0, dsm_stringMgr::Get(DSM_STR_SMV_DUALPHASE_CAPTION));
	m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);
	//m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);


    // ����F4���ͼ��ͼ��
    //m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(m_bGram ? DSM_BMP_M_TABLEGRAPH_GRAPH : DSM_BMP_M_TABLEGRAPH_TABLE), NULL, NULL);
	m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(m_bGram ? DSM_BMP_M_SMV_SCAN_REPORT_SEL : DSM_BMP_M_SMV_SCAN_GRAPHPIC_SEL), NULL, NULL);
    // �޸�F5�ַ���Ϊ����
    m_menuMain.SetMenuItemCaption(4, L"");
}


/**
 *      F4�л����ܣ����������ͼ���л���ʾ
 */
void CSMVDualphaseDlg::_switchF4()
{
    // �л���ʾ��Чֵ����������ͼ

    // ͼ��/���ģʽ�л�
    if(m_bGram)
    {
        m_reportValue.ShowWindow(SW_HIDE);
        m_ctrlPhaseGram.ShowWindow(SW_SHOW);
    }
    else
    {
        m_ctrlPhaseGram.ShowWindow(SW_HIDE);
        m_reportValue.ShowWindow(SW_SHOW);
    }
}

/**
 *      F5�л����ܣ��ڱ��ģʽ�£��л���ʾͬ����ǲ�����������
 */
void CSMVDualphaseDlg::_switchF5()
{
    // �л�ͬ����书�ܣ���Ҫ���ñ���

    // ͬ��  ����ͬ�����
    if(m_bSamePhase)
    {
        m_reportValue.SetRowItemText(0, PHASOR_COL1, dsm_stringMgr::Get(DSM_STR_SMV_DUALPHASE_SAMEANGDIFF));
        m_reportValue.SetRowItemText(5, PHASOR_COL1, dsm_stringMgr::Get(DSM_STR_SMV_DUALPHASE_SAMEDIFF));
    }
    // ���  ����������
    else
    {
        m_reportValue.SetRowItemText(0, PHASOR_COL1, dsm_stringMgr::Get(DSM_STR_SMV_DUALPHASE_BETWDIFF));
        m_reportValue.SetRowItemText(5, PHASOR_COL1, dsm_stringMgr::Get(DSM_STR_SMV_DUALPHASE_BETWDIFF));
    }

    // �������±�������
    _updateData();
}


/**
 *      ���²˵�����
 */
void CSMVDualphaseDlg::_updateMenuNavi()
{
    m_menuMain.SetMenuItemCaption(1, L"");

    // ͼ��ģʽ
    if(m_bGram)
    {
        m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_GRAPHPIC_SEL), NULL, NULL);

        // ����F5����
        m_menuMain.SetMenuItemBitmaps(4, NULL, NULL, NULL);
    }
    // ���ģʽ
    else
    {
        m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_REPORT_SEL), NULL, NULL);

        // ͬ��
        if(m_bSamePhase)
        {
            m_menuMain.SetMenuItemBitmaps(4, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_PHASE_SEL), NULL, NULL);
        }
        // ���
        else
        {
            m_menuMain.SetMenuItemBitmaps(4, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ALTERNATE_SEL), NULL, NULL);
        }
    }

	//��Ҫ����������ѡ�е�һ��ֵ���Ƕ���ֵ�����жϣ�����������ʾ
	CSmvRecvConfigPublic* pCfg = m_pDataLayer->GetSmvConfigPublic();

	if (pCfg->GetSmvDispType() == SMV_VALUE_TYPE_SECONDARY)
	{
		m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_SECOND_SEL), NULL, NULL);
	}
	else if (pCfg->GetSmvDispType() == SMV_VALUE_TYPE_PRIMARY)
	{
		m_menuMain.SetMenuItemBitmaps(2, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_FIRST_SEL), NULL, NULL);
	}

    m_pNaviBar->UpdateNaviMenu();
}

/**
 *      ��һ��ѡ��ͨ������A���������Զ�Ϊ2��3ͨ��ѡ����ӵ�BC�����͵�ͨ��
 *
 *  \param[in]
 *      eOrder          smv����smv1��smv2
 *  \param[in]
 *      pCtrlBlk        ѡ�еĿ��ƿ����
 *  \param[in]
 *      nSel            ���ƿ���ѡ�е�A��ͨ������
 */
void CSMVDualphaseDlg::_adjustABC( ePhaseOrder eOrder, CBaseDetectItem* pCtrlBlk, uint32_t nSel )
{
    ASSERT(pCtrlBlk);

    CSmvRecvConfig* pCfg = pCtrlBlk->pRecvConfig;
    ASSERT(pCfg);

    CSmvRecvChannelInfo* pInfo = pCfg->GetOneChannelInfo(nSel);

    if(!pInfo)
    {
        ASSERT(pInfo);
        return;
    }

    // ��ѡ�е���A�����ͣ�����ѡ�����ڵ�BC������ͨ��
    if(CHANNEL_PHASE_A == pInfo->enChannelPhase)
    {
        int nB = _findChannel(pCtrlBlk, pInfo->enChannelType, CHANNEL_PHASE_B, nSel + 1);
        int nC = _findChannel(pCtrlBlk, pInfo->enChannelType, CHANNEL_PHASE_C, nB + 1);

        if(nB != UINT_MAX && nC != UINT_MAX)
        {
            ASSERT(m_pPageDataExtraDual);

            int nRedrawA = 1;

            if(eOrder == ORDER_PHASE_A)
            {
                m_pPageDataExtraDual->nChannelA[0] = nSel;
                m_pPageDataExtraDual->nChannelA[1] = nB;
                m_pPageDataExtraDual->nChannelA[2] = nC;

                for(int k = 0; k < 3; k++)
                {
                    m_pPageDataExtraDual->bChannelValidA[k] = true;
                }
            }
            else
            {
                m_pPageDataExtraDual->nChannelB[0] = nSel;
                m_pPageDataExtraDual->nChannelB[1] = nB;
                m_pPageDataExtraDual->nChannelB[2] = nC;

                for(int k = 0; k < 3; k++)
                {
                    m_pPageDataExtraDual->bChannelValidB[k] = true;
                }

                nRedrawA = 6;
            }

            m_reportPhase.SetRowItemText(nRedrawA + 1, PHASOR_COL1, pCfg->GetOneChannelInfo(nB)->strChannelDesc.c_str());
            m_reportPhase.SetRowItemText(nRedrawA + 2, PHASOR_COL1, pCfg->GetOneChannelInfo(nC)->strChannelDesc.c_str());

            // ����BCͨ���ĵ�λ
            _updateUnit(pCtrlBlk, nRedrawA + 1, nB);
            _updateUnit(pCtrlBlk, nRedrawA + 2, nC);
        }
    }
}

/**
 *      ѡ���¿��ƿ�󣬵����ÿ��ƿ���ABCͨ��
 *
 *  \param[in]
 *      eOrder          smv����smv1��smv2
 *  \param[in]
 *      nSel            ���ƿ���������������ABC����ͨ��������ѡ��
 */
void CSMVDualphaseDlg::_adjustCtrlBlk( ePhaseOrder eOrder, uint32_t nSel )
{
    ASSERT(m_pDataLayer);

    CBaseDetectItem* pCtrlBlk = m_pDataLayer->GetSelectCtrlBlkByID(nSel);
    ASSERT(pCtrlBlk);

    CSmvRecvConfig* pCfg = pCtrlBlk->pRecvConfig;
    ASSERT(pCfg);

    int nRow            = 1;            // ABCͨ����ʼ�к�
    uint32_t* pChannels = NULL;

    // ͨ�ñ�������������
    if(eOrder == ORDER_PHASE_A)
    {
        m_pCtrlBlkA = pCtrlBlk;
        pChannels   = &m_pPageDataExtraDual->nChannelA[0];
    }
    else
    {
        m_pCtrlBlkB = pCtrlBlk;
        pChannels   = &m_pPageDataExtraDual->nChannelB[0];
        nRow        = 6;
    }

    CDsmSmvDualPhaseChannelRecord* pRecord = NULL;

    // �����µĿ��ƿ�ΪABC�����б����ͨ��
    for(int i = 0; i < 3; i++)
    {
        pRecord = dynamic_cast<CDsmSmvDualPhaseChannelRecord*>(m_reportPhase.GetRecordRow(nRow + i));
        ASSERT(pRecord);

        pRecord->UpdateChannels(pCtrlBlk);
    }

    // ���жϾɿ��ƿ��µ�ABCͨ���Ƿ���Ч
    // ��Ч��Ϊ�¿��ƿ�ѡ��Ĭ�ϵ�ABC
    // ��Ч�򲻸Ķ�

    bool bEffect                = true;
    CSmvRecvChannelInfo* pInfo  = NULL;

    for(int i = 0; i < 3; i++)
    {
        pInfo = pCfg->GetOneChannelInfo(pChannels[i]);

        if(!pInfo ||
            pInfo->enChannelType != CHANNEL_TYPE_SMV_CURRENT || 
            pInfo->enChannelType != CHANNEL_TYPE_SMV_VOLTAGE)
        {
            bEffect = false;
            break;
        }
    }

    // ����ͨ������Ч
    if(!bEffect)
    {
        // Ϊ�¿��ƿ�ѡ��Ĭ��ͨ��
        _calcChannel(pCtrlBlk, eOrder);

        // ���õ�����
        _setChannel(eOrder);
    }

    uint32_t nChannel[3] = {UINT_MAX, UINT_MAX, UINT_MAX};

    // �������õ����ݲ�
    if(eOrder == ORDER_PHASE_A)
    {
        for(int i = 0; i < 3; i++)
        {
            if(m_pPageDataExtraDual->bChannelValidA[i])
            {
                nChannel[i] = m_pPageDataExtraDual->nChannelA[i];
            }
        }

        m_pDataLayer->SetPhaseCheckSmv1CtrBlk(m_pCtrlBlkA);
        m_pDataLayer->SetSmv1ABCChannles(nChannel[0], nChannel[1], nChannel[2]);
    }
    else
    {
        for(int i = 0; i < 3; i++)
        {
            if(m_pPageDataExtraDual->bChannelValidB[i])
            {
                nChannel[i] = m_pPageDataExtraDual->nChannelB[i];
            }
        }

        m_pDataLayer->SetPhaseCheckSmv2CtrBlk(m_pCtrlBlkB);
        m_pDataLayer->SetSmv2ABCChannles(nChannel[0], nChannel[1], nChannel[2]);
    }

    // ���µ�λ
    for(int i = 0; i < 3; i++)
    {
        _updateUnit(pCtrlBlk, nRow + i, pChannels[i]);
    }
}


void CSMVDualphaseDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

    if(nIDEvent == m_nTimerID)
    {
        // ��ʱˢ�º���ҳ������
        _updateData();
    }

    CSMVBaseDlg::OnTimer(nIDEvent);
}


/**
 *      ˢ��ҳ������
 */
void CSMVDualphaseDlg::_updateData()
{
    ASSERT(m_pDataLayer);

    if(m_bEnableTimer)
    {
        if(_checkChannel())
        {
            return;
        }

        if(!m_pDataLayer->GetPhaseCheckData(&m_data))
        {
            // �������ͼ
            if(m_bGram)
            {
                m_ctrlPhaseGram.ErasePhasor();
                m_ctrlPhaseGram.Invalidate(TRUE);
            }
            // ��ձ���
            else
            {
                _clearReport();
            }

            return;
        }
    }

    // ��������ͼ
    if(m_bGram)
    {
        _updateDataGram();
    }
    // ���±�������
    else
    {
        _updateDataReport();
    }
}


/**
 *      ˢ������ͼ
 */
void CSMVDualphaseDlg::_updateDataGram()
{
    // Ƶ��
    CString csFreq1, csFreq2;
    CString csAa, csBb, csCc;
    csFreq1.Format(L"f1=%0.3f Hz", m_data.GetSmv1Freq());
    csFreq2.Format(L"f2=%0.3f Hz", m_data.GetSmv2Freq());

    // ��ȡ����
    if(!m_data.GetSmv1VaulePhase(&m_stPhaseVal1[0], &m_stPhaseVal1[1], &m_stPhaseVal1[2]) || 
        !m_data.GetSmv2VaulePhase(&m_stPhaseVal2[0], &m_stPhaseVal2[1], &m_stPhaseVal2[2]) || 
        !m_data.GetInPhaseAngleDiff(&m_fInPhaseAngleDiff[0], &m_fInPhaseAngleDiff[1], &m_fInPhaseAngleDiff[2]))
    {
        return;
    }

    float fEffect[6] = {
        m_pPageDataExtraDual->bChannelValidA[0] ? m_stPhaseVal1[0].m_fValue : 0.0f, 
        m_pPageDataExtraDual->bChannelValidA[1] ? m_stPhaseVal1[1].m_fValue : 0.0f, 
        m_pPageDataExtraDual->bChannelValidA[2] ? m_stPhaseVal1[2].m_fValue : 0.0f,
        m_pPageDataExtraDual->bChannelValidB[0] ? m_stPhaseVal2[0].m_fValue : 0.0f, 
        m_pPageDataExtraDual->bChannelValidB[1] ? m_stPhaseVal2[1].m_fValue : 0.0f, 
        m_pPageDataExtraDual->bChannelValidB[2] ? m_stPhaseVal2[2].m_fValue : 0.0f};

    // �Ե�һ��A��ͨ����λ��Ϊ��׼
    float fPhaseA = m_stPhaseVal1[0].m_fPhase;

    float fAngel[6] = {
        m_pPageDataExtraDual->bChannelValidA[0] ? DSM_GetRelativePhase(fPhaseA, m_stPhaseVal1[0].m_fPhase) : 0.0f, 
        m_pPageDataExtraDual->bChannelValidA[1] ? DSM_GetRelativePhase(fPhaseA, m_stPhaseVal1[1].m_fPhase) : 0.0f, 
        m_pPageDataExtraDual->bChannelValidA[2] ? DSM_GetRelativePhase(fPhaseA, m_stPhaseVal1[2].m_fPhase) : 0.0f, 
        m_pPageDataExtraDual->bChannelValidB[0] ? DSM_GetRelativePhase(fPhaseA, m_stPhaseVal2[0].m_fPhase) : 0.0f, 
        m_pPageDataExtraDual->bChannelValidB[1] ? DSM_GetRelativePhase(fPhaseA, m_stPhaseVal2[1].m_fPhase) : 0.0f, 
        m_pPageDataExtraDual->bChannelValidB[2] ? DSM_GetRelativePhase(fPhaseA, m_stPhaseVal2[2].m_fPhase) : 0.0f};

    float fMaxLen = fEffect[0];

    for(int i = 1; i < 6; i++)
    {
        fMaxLen = max(fMaxLen, fEffect[i]);
    }

    csAa.Format(L"Aa��%0.3f��", DSM_GetRelativePhase(0.0f, m_fInPhaseAngleDiff[0]));
    csBb.Format(L"Bb��%0.3f��", DSM_GetRelativePhase(0.0f, m_fInPhaseAngleDiff[1]));
    csCc.Format(L"Cc��%0.3f��", DSM_GetRelativePhase(0.0f, m_fInPhaseAngleDiff[2]));

    // ����Ƶ������
    float fMaxTemp  = float(fMaxLen / 4.0);
    float fMargin   = fMaxTemp * 0.1f;
    float fDiff     = m_fScaleInterval - fMaxTemp;

    if(fDiff < 0.0f || fDiff > fMargin)
    {
        m_fScaleInterval = fMaxTemp;
        m_ctrlPhaseGram.SetLenScaleInterval(m_fScaleInterval);
    }

#if !CHUN_HUA
	for (int i =1; i<4;i++)
	{
		CELRecord*  record = m_reportPhase.GetRecordRow(i);
		CELRecordItem*  curItem = record->GetItem(0);
		curItem->SetColor(m_Color[i-1],EL_COLOR_DEFAULT);
		m_reportPhase.RedrawRowItem(i,0);
		CELRecordItem*  curItem1 = record->GetItem(1);
		curItem1->SetColor(m_Color[i-1],EL_COLOR_DEFAULT);
		m_reportPhase.RedrawRowItem(i,1);

		CELRecord*  record1 = m_reportPhase.GetRecordRow(i+5);
		CELRecordItem*  curItem2 = record1->GetItem(0);
		curItem2->SetColor(m_Color[i-1],EL_COLOR_DEFAULT);
		m_reportPhase.RedrawRowItem(i+5,0);
		CELRecordItem*  curItem3 = record1->GetItem(1);
		curItem3->SetColor(m_Color[i-1],EL_COLOR_DEFAULT);
		m_reportPhase.RedrawRowItem(i+5,1);
	}

#endif

    m_ctrlPhaseGram.ErasePhasor();

    m_ctrlPhaseGram.DrawExtDesc(csAa, m_rcAa, GRAM_TIPS_COLOR, DT_VCENTER | DT_RIGHT | DT_SINGLELINE);
    m_ctrlPhaseGram.DrawExtDesc(csBb, m_rcBb, GRAM_TIPS_COLOR);
    m_ctrlPhaseGram.DrawExtDesc(csCc, m_rcCc, GRAM_TIPS_COLOR);

    m_ctrlPhaseGram.DrawExtDesc(csFreq1, m_rcF1, GRAM_TIPS_COLOR, DT_VCENTER | DT_RIGHT | DT_SINGLELINE);
    m_ctrlPhaseGram.DrawExtDesc(csFreq2, m_rcF2, GRAM_TIPS_COLOR, DT_VCENTER | DT_RIGHT | DT_SINGLELINE);

    m_ctrlPhaseGram.DrawPhasor(fEffect, fAngel, m_csDesc, 6);
    m_ctrlPhaseGram.Invalidate(TRUE);
}

void CSMVDualphaseDlg::_updateDataReport()
{
    using namespace SMV_DUALPHASOR;

    // ��ʾsmv1��smv2Ƶ��
//     m_reportValue.SetRowItemText(PHASOR_FREQ1, PHASOR_COL0, g_GetResultString(m_csFmtFreq, m_data.GetSmv1Freq()));
//     m_reportValue.SetRowItemText(PHASOR_FREQ2, PHASOR_COL0, g_GetResultString(m_csFmtFreq, m_data.GetSmv2Freq()));
    m_arrayItem[0]->SetCaption(g_GetResultString(m_csFmtFreq, m_data.GetSmv1Freq()));
    m_arrayItem[1]->SetCaption(g_GetResultString(m_csFmtFreq, m_data.GetSmv2Freq()));


    bool bSmv1 = false;

    /// smv1��Чֵ
    if(m_data.GetSmv1VaulePhase(&m_stPhaseVal1[0], &m_stPhaseVal1[1], &m_stPhaseVal1[2]))
    {
//         m_reportValue.SetRowItemText(PHASOR_A, PHASOR_COL0, g_GetResultString(m_csFmtEffect, m_csUnit[0], m_stPhaseVal1[0].m_fValue, DSM_GetRelativePhase(m_stPhaseVal1[0].m_fPhase, m_stPhaseVal1[0].m_fPhase)));
//         m_reportValue.SetRowItemText(PHASOR_B, PHASOR_COL0, g_GetResultString(m_csFmtEffect, m_csUnit[1], m_stPhaseVal1[1].m_fValue, DSM_GetRelativePhase(m_stPhaseVal1[0].m_fPhase, m_stPhaseVal1[1].m_fPhase)));
//         m_reportValue.SetRowItemText(PHASOR_C, PHASOR_COL0, g_GetResultString(m_csFmtEffect, m_csUnit[2], m_stPhaseVal1[2].m_fValue, DSM_GetRelativePhase(m_stPhaseVal1[0].m_fPhase, m_stPhaseVal1[2].m_fPhase)));

        m_arrayItem[2]->SetCaption(g_GetResultString(m_csFmtEffect, m_csUnit[0], m_stPhaseVal1[0].m_fValue, DSM_GetRelativePhase(m_stPhaseVal1[0].m_fPhase, m_stPhaseVal1[0].m_fPhase)));
        m_arrayItem[3]->SetCaption(g_GetResultString(m_csFmtEffect, m_csUnit[1], m_stPhaseVal1[1].m_fValue, DSM_GetRelativePhase(m_stPhaseVal1[0].m_fPhase, m_stPhaseVal1[1].m_fPhase)));
        m_arrayItem[4]->SetCaption(g_GetResultString(m_csFmtEffect, m_csUnit[2], m_stPhaseVal1[2].m_fValue, DSM_GetRelativePhase(m_stPhaseVal1[0].m_fPhase, m_stPhaseVal1[2].m_fPhase)));

        bSmv1 = true;
    }

    // smv2��Чֵ
    if(m_data.GetSmv2VaulePhase(&m_stPhaseVal2[0], &m_stPhaseVal2[1], &m_stPhaseVal2[2]))
    {
        float fBase = m_stPhaseVal1[0].m_fPhase;

        if(!bSmv1)
        {
            fBase = m_stPhaseVal2[0].m_fPhase;
        }

//         m_reportValue.SetRowItemText(PHASOR_a, PHASOR_COL0, g_GetResultString(m_csFmtEffect, m_csUnit[3], m_stPhaseVal2[0].m_fValue, DSM_GetRelativePhase(fBase, m_stPhaseVal2[0].m_fPhase)));
//         m_reportValue.SetRowItemText(PHASOR_b, PHASOR_COL0, g_GetResultString(m_csFmtEffect, m_csUnit[4], m_stPhaseVal2[1].m_fValue, DSM_GetRelativePhase(fBase, m_stPhaseVal2[1].m_fPhase)));
//         m_reportValue.SetRowItemText(PHASOR_c, PHASOR_COL0, g_GetResultString(m_csFmtEffect, m_csUnit[5], m_stPhaseVal2[2].m_fValue, DSM_GetRelativePhase(fBase, m_stPhaseVal2[2].m_fPhase)));

        m_arrayItem[5]->SetCaption(g_GetResultString(m_csFmtEffect, m_csUnit[3], m_stPhaseVal2[0].m_fValue, DSM_GetRelativePhase(fBase, m_stPhaseVal2[0].m_fPhase)));
        m_arrayItem[6]->SetCaption(g_GetResultString(m_csFmtEffect, m_csUnit[4], m_stPhaseVal2[1].m_fValue, DSM_GetRelativePhase(fBase, m_stPhaseVal2[1].m_fPhase)));
        m_arrayItem[7]->SetCaption(g_GetResultString(m_csFmtEffect, m_csUnit[5], m_stPhaseVal2[2].m_fValue, DSM_GetRelativePhase(fBase, m_stPhaseVal2[2].m_fPhase)));
    }

    // ͬ��
    if(m_bSamePhase)
    {
        // ��ȡͬ����ǲ�
        if(m_data.GetInPhaseAngleDiff(&m_fInPhaseAngleDiff[0], &m_fInPhaseAngleDiff[1], &m_fInPhaseAngleDiff[2]))
        {
//             m_reportValue.SetRowItemText(PHASOR_A, PHASOR_COL1, g_GetResultString(L"Aa: %0.3f��", m_fInPhaseAngleDiff[0]));
//             m_reportValue.SetRowItemText(PHASOR_B, PHASOR_COL1, g_GetResultString(L"Bb: %0.3f��", m_fInPhaseAngleDiff[1]));
//             m_reportValue.SetRowItemText(PHASOR_C, PHASOR_COL1, g_GetResultString(L"Cc: %0.3f��", m_fInPhaseAngleDiff[2]));

            m_arrayItem[8]->SetCaption(g_GetResultString(L"Aa: %0.3f��", DSM_GetRelativePhase(0.0f, m_fInPhaseAngleDiff[0])));
            m_arrayItem[9]->SetCaption(g_GetResultString(L"Bb: %0.3f��", DSM_GetRelativePhase(0.0f, m_fInPhaseAngleDiff[1])));
            m_arrayItem[10]->SetCaption(g_GetResultString(L"Cc: %0.3f��", DSM_GetRelativePhase(0.0f, m_fInPhaseAngleDiff[2])));
        }

        // ��ȡͬ��������
        if(m_data.GetInPhasePhasorDiff(&m_stPhaseData2[0], &m_stPhaseData2[1], &m_stPhaseData2[2]))
        {
//             m_reportValue.SetRowItemText(PHASOR_a, PHASOR_COL1, g_GetResultString(L"Aa: %0.3f %s ��%0.3f��", m_csUnit[3], m_stPhaseData2[0].m_fValue, m_stPhaseData2[0].m_fPhase));
//             m_reportValue.SetRowItemText(PHASOR_b, PHASOR_COL1, g_GetResultString(L"Bb: %0.3f %s ��%0.3f��", m_csUnit[4], m_stPhaseData2[1].m_fValue, m_stPhaseData2[1].m_fPhase));
//             m_reportValue.SetRowItemText(PHASOR_c, PHASOR_COL1, g_GetResultString(L"Cc: %0.3f %s ��%0.3f��", m_csUnit[5], m_stPhaseData2[2].m_fValue, m_stPhaseData2[2].m_fPhase));

            m_arrayItem[11]->SetCaption(g_GetResultString(L"Aa: %0.3f%s/%0.3f��", m_csUnit[3], m_stPhaseData2[0].m_fValue, m_stPhaseData2[0].m_fPhase));
            m_arrayItem[12]->SetCaption(g_GetResultString(L"Bb: %0.3f%s/%0.3f��", m_csUnit[4], m_stPhaseData2[1].m_fValue, m_stPhaseData2[1].m_fPhase));
            m_arrayItem[13]->SetCaption(g_GetResultString(L"Cc: %0.3f%s/%0.3f��", m_csUnit[5], m_stPhaseData2[2].m_fValue, m_stPhaseData2[2].m_fPhase));
        }
    }
    // ���
    else
    {
        // ��ȡ���������
        if(m_data.GetSmv1BetweenPhasePhasorDiff(&m_stPhaseData1[0], &m_stPhaseData1[1], &m_stPhaseData1[2]))
        {
//             m_reportValue.SetRowItemText(PHASOR_A, PHASOR_COL1, g_GetResultString(L"AB: %0.3f %s ��%0.3f��", m_csUnit[0], m_stPhaseData1[0].m_fValue, m_stPhaseData1[0].m_fPhase));
//             m_reportValue.SetRowItemText(PHASOR_B, PHASOR_COL1, g_GetResultString(L"BC: %0.3f %s ��%0.3f��", m_csUnit[1], m_stPhaseData1[1].m_fValue, m_stPhaseData1[1].m_fPhase));
//             m_reportValue.SetRowItemText(PHASOR_C, PHASOR_COL1, g_GetResultString(L"CA: %0.3f %s ��%0.3f��", m_csUnit[2], m_stPhaseData1[2].m_fValue, m_stPhaseData1[2].m_fPhase));

            m_arrayItem[8]->SetCaption(g_GetResultString(L"AB: %0.3f%s/%0.3f��", m_csUnit[0], m_stPhaseData1[0].m_fValue, m_stPhaseData1[0].m_fPhase));
            m_arrayItem[9]->SetCaption(g_GetResultString(L"BC: %0.3f%s/%0.3f��", m_csUnit[1], m_stPhaseData1[1].m_fValue, m_stPhaseData1[1].m_fPhase));
            m_arrayItem[10]->SetCaption(g_GetResultString(L"CA: %0.3f%s/%0.3f��", m_csUnit[2], m_stPhaseData1[2].m_fValue, m_stPhaseData1[2].m_fPhase));
        }
        // ��ȡ���������
        if(m_data.GetSmv2BetweenPhasePhasorDiff(&m_stPhaseData2[0], &m_stPhaseData2[1], &m_stPhaseData2[2]))
        {
//             m_reportValue.SetRowItemText(PHASOR_a, PHASOR_COL1, g_GetResultString(L"ab: %0.3f %s ��%0.3f��", m_csUnit[3], m_stPhaseData2[0].m_fValue, m_stPhaseData2[0].m_fPhase));
//             m_reportValue.SetRowItemText(PHASOR_b, PHASOR_COL1, g_GetResultString(L"bc: %0.3f %s ��%0.3f��", m_csUnit[4], m_stPhaseData2[1].m_fValue, m_stPhaseData2[1].m_fPhase));
//             m_reportValue.SetRowItemText(PHASOR_c, PHASOR_COL1, g_GetResultString(L"ca: %0.3f %s ��%0.3f��", m_csUnit[5], m_stPhaseData2[2].m_fValue, m_stPhaseData2[2].m_fPhase));

            m_arrayItem[11]->SetCaption(g_GetResultString(L"ab: %0.3f%s/%0.3f��", m_csUnit[3], m_stPhaseData2[0].m_fValue, m_stPhaseData2[0].m_fPhase));
            m_arrayItem[12]->SetCaption(g_GetResultString(L"bc: %0.3f%s/%0.3f��", m_csUnit[4], m_stPhaseData2[1].m_fValue, m_stPhaseData2[1].m_fPhase));
            m_arrayItem[13]->SetCaption(g_GetResultString(L"ca: %0.3f%s/%0.3f��", m_csUnit[5], m_stPhaseData2[2].m_fValue, m_stPhaseData2[2].m_fPhase));
        }
    }

    // ��ˢ��report���ɵ�����ˢ��
    m_reportValue.Invalidate(TRUE);
}


/**
 *      ����ѡ��ͨ���ĵ�λ
 *
 *  \param[in]
 *      pCtrlBlk        ͨ�������Ŀ��ƿ����
 *  \param[in]
 *      nRow            ͨ�����ڱ����к�
 *  \param[in]
 *      nChannel        ͨ����
 */
void CSMVDualphaseDlg::_updateUnit( CBaseDetectItem* pCtrlBlk, int nRow, uint32_t nChannel )
{
    ASSERT(pCtrlBlk);

    enumSmvValueType eDispType      = CDsmDataLayer::getInstance()->GetSmvConfigPublic()->GetSmvDispType();         // ��ȡͨ��ֵ����ʾ����
    enumSmvValueType eFrameType     = CDsmDataLayer::getInstance()->GetParamConfigBasic()->GetFrameValueType();     // ��ȡһ�ζ��ζֵ����
    CSmvRecvConfig* pCfg            = pCtrlBlk->pRecvConfig;
    ASSERT(pCfg);

    CSmvRecvChannelInfo* pInfo      = pCfg->GetOneChannelInfo(nChannel);

    // ����ָ��ͨ�����͵ĵ�λ
    m_csUnit[nRow > 3 ? nRow - 3 : nRow - 1] = pInfo ? DM_GetUnitFromType(pInfo->enChannelType, eDispType, eFrameType) : L"";
}



/**
 *      ��ʼ����ǰ�����ҳ������
 */
void CSMVDualphaseDlg::_initCurrentCtrlBlk()
{
    CPageData* pData = CEventProc::GetInstance()->GetCurrentPageData();
    bool bInited = true;

    // �½�ҳ�棬����ҳ������
    if(pData->m_eAction == PageEnter)
    {
        if(!pData->m_pPageDataExtra)
        {
            bInited = false;
            m_pPageDataExtraDual = new CPageDataExtraDualPhase(pData);
            pData->m_pPageDataExtra = m_pPageDataExtraDual;
        }
    }

    // ����ҳ�棬ȡ���ɵ�ҳ������
    if((pData->m_eAction == PageBack && pData->m_pPageDataExtra) || bInited)
    {
        m_pPageDataExtraDual = dynamic_cast<CPageDataExtraDualPhase*>(pData->m_pPageDataExtra);
        ASSERT(m_pPageDataExtraDual);
    }

    // ��ʼ���������ƿ�
    _initPageStatus();

    // ���ú���ģʽ
    m_pDataLayer->SetCurrentMode(DSM_MODE_SMV_RECV_PHASE_CHECK);
}

/**
 *      ��ձ������
 */
void CSMVDualphaseDlg::_clearReport()
{
    m_reportValue.SetRowItemText(PHASOR_A, PHASOR_COL0, g_csBlankValue);
    m_reportValue.SetRowItemText(PHASOR_B, PHASOR_COL0, g_csBlankValue);
    m_reportValue.SetRowItemText(PHASOR_C, PHASOR_COL0, g_csBlankValue);
    m_reportValue.SetRowItemText(PHASOR_a, PHASOR_COL0, g_csBlankValue);
    m_reportValue.SetRowItemText(PHASOR_b, PHASOR_COL0, g_csBlankValue);
    m_reportValue.SetRowItemText(PHASOR_c, PHASOR_COL0, g_csBlankValue);

    m_reportValue.SetRowItemText(PHASOR_A, PHASOR_COL1, g_csBlankValue);
    m_reportValue.SetRowItemText(PHASOR_B, PHASOR_COL1, g_csBlankValue);
    m_reportValue.SetRowItemText(PHASOR_C, PHASOR_COL1, g_csBlankValue);
    m_reportValue.SetRowItemText(PHASOR_a, PHASOR_COL1, g_csBlankValue);
    m_reportValue.SetRowItemText(PHASOR_b, PHASOR_COL1, g_csBlankValue);
    m_reportValue.SetRowItemText(PHASOR_c, PHASOR_COL1, g_csBlankValue);

    m_reportValue.SetRowItemText(PHASOR_FREQ1, PHASOR_COL0, L"Freq = -- Hz");
    m_reportValue.SetRowItemText(PHASOR_FREQ2, PHASOR_COL0, L"Freq = -- Hz");
}

/**
 *      ���ͨ���Ƿ����仯
 */
bool CSMVDualphaseDlg::_checkChannel()
{
    uint32_t nCtrlBlkA, nCtrlBlkB;

    // ͨ���������仯��
    if(!m_pDataLayer->GetPhaseCheckRealTimeChannelCounts(nCtrlBlkA, nCtrlBlkB) || 
        m_pCtrlBlkA->nChannelCount != nCtrlBlkA ||
        m_pCtrlBlkB->nChannelCount != nCtrlBlkB)
    {
        if(m_bChannelChange && CStatusMgr::ShowTipsStyle() == 0x00)
        {
            // ����tips��ʾ
            CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_CHANNEL_NUM_CHANGE));
        }

        if(!m_bChannelChange)
        {
            m_bChannelChange = true;

            // �������ͼ
            if(m_bGram)
            {
                m_ctrlPhaseGram.ErasePhasor();
                m_ctrlPhaseGram.Invalidate(TRUE);
            }
            else
            {
                _clearReport();
            }

            // ��ֹͨ��ѡ��
            for(int i = 0; i < 9; i++)
            {
                if(i == 4)
                {
                    continue;
                }

                m_reportPhase.GetRecordRow(i)->GetItem(1)->SetEditable(FALSE);
            }

            if(m_pNaviBar->GetCommandTarget() == this)
            {
                // ���ز��ֲ˵�����
#if SU_FEI
                m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, L"");
#endif
                m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(3, NULL);
                m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(4, NULL);
                m_pNaviBar->UpdateNaviMenu();
            }
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

        // �ָ�ͨ��ѡ��
        for(int i = 0; i < 9; i++)
        {
            if(i == 4)
            {
                continue;
            }

            m_reportPhase.GetRecordRow(i)->GetItem(1)->SetEditable(TRUE);
        }

        if(m_pNaviBar->GetCommandTarget() == this)
        {
            // �ָ��˵�����
#if SU_FEI
            m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, dsm_stringMgr::Get(DSM_STR_TIMER_STOP));
#endif
            m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(m_bGram ? DSM_BMP_M_SMV_SCAN_GRAPHPIC_SEL : DSM_BMP_M_SMV_SCAN_REPORT_SEL));
            m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(4, m_bGram ? NULL : dsm_bmpmgr::GetCB(m_bSamePhase ? DSM_BMP_M_SMV_SCAN_PHASE_SEL : DSM_BMP_M_SMV_SCAN_ALTERNATE_SEL));
            m_pNaviBar->UpdateNaviMenu();
        }
    }

    return false;
}

void CSMVDualphaseDlg::OnInplaceButtonDown( NMHDR* pNotifyStruct, LRESULT *pResult )
{
    if(!m_bEnableTimer)
    {
        ELNM_INPLACELIST_ARGS* pItemNotify = (ELNM_INPLACELIST_ARGS*)pNotifyStruct;
        ASSERT(pItemNotify);

        switch(pItemNotify->rowIndex)
        {
        case 0:
        case 5:
            CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_TIMER_SWITCH_CTRL_TIPS));
            break;

        case 1:
        case 2:
        case 3:
        case 6:
        case 7:
        case 8:
            CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_TIMER_SW_CHAN_TIPS));
            break;

        default:
            break;
        }

        *pResult = TRUE;
    }
}

