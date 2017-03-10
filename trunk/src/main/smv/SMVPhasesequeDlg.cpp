/**
 * \file
 *      SMVPhasesequeDlg.cpp
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

// SMVPhasesequeDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SMVPhasesequeDlg.h"

#include "src/main/mgr/statusmgr.h"
#include "src/main/utils/utility.h"
#include "src/main/smv/common_smv.h"
#include "src/main/pagedata/pagedata.h"
#include "src/utils/utility/dsm_utility.h"


/**
 *      ����ҳ����ƿ��������
 */
class CCtrlBlkDataPhasaSeq  : public ICtrlBlkData
{
public:

    CCtrlBlkDataPhasaSeq()
    {
        for(int i = 0; i < 3; i++)
        {
            nChannelIndex[i] = UINT_MAX;
            bChannelValid[i] = false;
        }
    }


public:
    int nFoucs;                     ///< �����к�
    uint32_t nChannelIndex[3];      ///< ͨ����
    bool bChannelValid[3];          ///< ͨ����Ч��
    int nTimes;                     ///< ���ű���
};



#define PHASASEQ_MODE_SELECT            0x01        ///< ѡ��
#define PHASASEQ_MODE_ZOOM              0x02        ///< ����

#define PHASASEQ_TIME_MAX               4           ///< ���Ŵ���
#define PHASASEQ_TIME_MIN               -4          ///< ��С��С����

#if !CHUN_HUA
COLORREF m_sColor[] ={DSM_PHASE_COLOR_A,DSM_PHASE_COLOR_B,DSM_PHASE_COLOR_C,DSM_PHASE_COLOR_N};
#endif

// CSMVPhasesequeDlg �Ի���

IMPLEMENT_DYNAMIC(CSMVPhasesequeDlg, DSM_SMVBASEDLG)

CSMVPhasesequeDlg::CSMVPhasesequeDlg(CWnd* pParent /*=NULL*/)
	: DSM_SMVBASEDLG(CSMVPhasesequeDlg::IDD, DSM_STR_SMV_PHASESEQU_CAPTION, pParent)
    , m_dwMode(PHASASEQ_MODE_SELECT)
    , m_nTimes(0)
    , m_fScaleInterval(0.0f)
    , m_eDispType(SMV_VALUE_TYPE_PRIMARY)
    , m_eFrameType(SMV_VALUE_TYPE_PRIMARY)
{
    for(int i = 0; i < 3; i++)
    {
        m_fAngel[i] = 0.0f;
        m_fValue[i] = 0.0f;

        m_strDesc[i].Format(dsm_stringMgr::Get(DSM_STR_SMV_PHASESEQU_POS + i));
    }
}

CSMVPhasesequeDlg::~CSMVPhasesequeDlg()
{
    CStatusMgr::SetTips(L"", 200);
}

void CSMVPhasesequeDlg::DoDataExchange(CDataExchange* pDX)
{
	DSM_SMVBASEDLG::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSMVPhasesequeDlg, DSM_SMVBASEDLG)
    ON_WM_TIMER()
    ON_COMMAND(ID_MENU_SMV_F4, &CSMVPhasesequeDlg::OnMenuF4)
    ON_NOTIFY(ELNM_ITEM_CONSTRAINTCHANGED, IDC_SMV_PHASESEQU_REPORT, &CSMVPhasesequeDlg::OnItemSelectChanged)
    ON_NOTIFY(ELNM_INPLACEBUTTONDOWN, IDC_SMV_PHASESEQU_REPORT, &CSMVPhasesequeDlg::OnInplaceButtonDown)
END_MESSAGE_MAP()


// CSMVPhasesequeDlg ��Ϣ�������


BOOL CSMVPhasesequeDlg::OnInitDialog()
{
    DSM_SMVBASEDLG::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��

    m_eDispType      = m_pDataLayer->GetSmvConfigPublic()->GetSmvDispType();         // ��ȡͨ��ֵ����ʾ����
    m_eFrameType     = m_pDataLayer->GetParamConfigBasic()->GetFrameValueType();     // ��ȡͨ���ֵ����

    _createReport();
    _createPhaseGram();

    m_reportPhase.SetFocus();

    return FALSE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}

/**
 *      ������λ����
 */
void CSMVPhasesequeDlg::_createReport()
{
    CRect rcReport(DSM_PHASOR_REPORT_RECT);
    DSM_ParentToChild(this, rcReport);
    int nHeight = ELT_T1_ROWHEIGHT * 7;

    // �����߶�
    rcReport.top = (rcReport.Height() - nHeight) / 2;
    rcReport.bottom = rcReport.top + nHeight;
#if SU_FEI
    rcReport.OffsetRect(DSM_MARGIN_IN, 0);
#endif
    m_reportPhase.Create(DSM_WINSTYLE_REPORT_DEFAULT, rcReport, this, IDC_SMV_PHASESEQU_REPORT);

    // ��������
    m_reportPhase.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    // ������ʽ
    m_reportPhase.PostInitTemplateStyle();
    m_reportPhase.SetHeaderHeight(0);
	m_reportPhase.SetHasGridline(FALSE, TRUE);

    // ������Ԫ��
    m_reportPhase.InsertColumn(0, L"", EL_CENTER, DSM_PHASOR_REPORT_CAPTION);     // ����
    m_reportPhase.InsertColumn(1, L"", EL_LEFT, DSM_PHASOR_REPORT_VALUE);       // ����

    // ����ѡ�񽹵���ʽ
    m_reportPhase.SetRowSelectionVisible(TRUE);
    m_reportPhase.SetRowFocusEnable(TRUE);
    m_reportPhase.SetSubItemFocusEnable(TRUE);
    m_reportPhase.SetFreezeColumnsCount(1);
    m_reportPhase.SetFocusedColIndex(1);

    // ���豨��ֵ
    _resetReport();
}

/**
 *      ��������ͼ
 */
void CSMVPhasesequeDlg::_createPhaseGram()
{
    CRect rcPhasor(DSM_PHASOR_PHASORGRAM_RECT);
    DSM_ParentToChild(this, rcPhasor);

    m_ctrlPhasor.Create(NULL, WS_CHILD | WS_VISIBLE, rcPhasor, this, IDC_SMV_PHASESEQU_PHASESEQUGRAM);
    m_ctrlPhasor.ModifyStyle(0, SS_OWNERDRAW);

    // �������������ɫ
    m_ctrlPhasor.SetPhasorColor(0, DSM_PHASE_COLOR_A);
    m_ctrlPhasor.SetPhasorColor(1, DSM_PHASE_COLOR_B);
    m_ctrlPhasor.SetPhasorColor(2, DSM_PHASE_COLOR_C);

    m_ctrlPhasor.SetBkgndColor(DSM_COLOR_BK);
    CRect rcMargin(DSM_PHASOR_MARGIN);
    m_ctrlPhasor.SetMargin(rcMargin.left, rcMargin.right, rcMargin.top, rcMargin.bottom);
    m_ctrlPhasor.Init();

    m_ctrlPhasor.ShowWindow(SW_SHOW);
}

/**
 *      ���豨����ͨ������
 */
void CSMVPhasesequeDlg::_resetReport()
{
    ASSERT(m_pCurrentCtrlBlk);

    m_reportPhase.DeleteAllRecordRow();

    int nIndex = 0;

    // ����ABC����ͨ����
    m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(m_pCurrentCtrlBlk, dsm_stringMgr::Get(DSM_STR_SMV_PHASESEQU_A), g_csBlankValue));
    m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(m_pCurrentCtrlBlk, dsm_stringMgr::Get(DSM_STR_SMV_PHASESEQU_B), g_csBlankValue));
    m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(m_pCurrentCtrlBlk, dsm_stringMgr::Get(DSM_STR_SMV_PHASESEQU_C), g_csBlankValue));

    // ��������
  //  m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(NULL, L"", L""));
    m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(NULL, L"", L""));
   // m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(NULL, L"", L""));

    // ����ABC����ͨ����ֵ��
    m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(NULL, dsm_stringMgr::Get(DSM_STR_SMV_PHASESEQU_POSITIVE), g_csBlankValue));
    m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(NULL, dsm_stringMgr::Get(DSM_STR_SMV_PHASESEQU_NEGATIVE), g_csBlankValue));
    m_reportPhase.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(NULL, dsm_stringMgr::Get(DSM_STR_SMV_PHASESEQU_ZERO), g_csBlankValue));

    // ��ֹ��ͨ����ѡ��
    for(int i = 3; i < 7; i++)
    {
        m_reportPhase.GetRecordRow(i)->SetFocusable(FALSE);
    }

    // ����ҳ������������ѡͨ��
    if(!_restorePage())
    {
        // ��ҳ������û�������Ϣ�����Զ�ѡ�к��ʵ�ͨ��
        _adjustChannel();
    }

    // ����ͨ����ֵ�ĵ�λ��Ϣ
    _updateUnit();
}

/**
 *      ������ʼѡ�е�3��ͨ��������ѡ�к��ʵ�ABC����
 */
void CSMVPhasesequeDlg::_adjustChannel()
{
    ASSERT(m_pPageDataExtra);
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_reportPhase.GetSafeHwnd());

    // ��ȡ���ƿ�����ݣ���Ϊ�գ����½�һ������
    CCtrlBlkDataPhasaSeq* pData = dynamic_cast<CCtrlBlkDataPhasaSeq*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));

    if(!pData)
    {
        pData = new CCtrlBlkDataPhasaSeq;
        m_pPageDataExtra->Set(m_pCurrentCtrlBlk->nID, pData);
    }

    // Ѱ��3�����ʵ�ͨ��
    _calcChannel(pData);

    CSmvRecvChannelInfo* pInfo  = NULL;
    CSmvRecvConfig* pCfg        = m_pCurrentCtrlBlk->pRecvConfig;

    // ����3��ͨ����ֵ������
    for(int i = 0; i < 3; i++)
    {
        // ͨ����Ч����ʾL"--"
        if(!pData->bChannelValid[i])
        {
            m_reportPhase.SetRowItemText(i, 1, g_csBlankValue);
        }
        else
        {
            pInfo = pCfg->GetOneChannelInfo(pData->nChannelIndex[i]);

            if(pInfo)
            {
                m_reportPhase.SetRowItemText(i, 1, pInfo->strChannelDesc.c_str());
            }
            else
            {
                m_reportPhase.SetRowItemText(i, 1, g_csBlankValue);
            }
        }
    }

    m_reportPhase.SetFocusedSelectionRowIndex(0);

    pData->nFoucs = 0;
}

/**
 *      Ѱ��3�����ʵ�Ĭ����ʾͨ����
 *
 *  \note
 *      ѡ�е�����Ĭ��ͨ�������������ͨ��������ҳ�����ݶ�����
 */
void CSMVPhasesequeDlg::_calcChannel(CCtrlBlkDataPhasaSeq* pData)
{
    ASSERT(pData);

    CSmvRecvConfig* pCfg        = m_pCurrentCtrlBlk->pRecvConfig;
    size_t nCounts              = pCfg->GetChannelCount();
    CSmvRecvChannelInfo* pInfo  = NULL;

    // ö������ͨ��
    for(size_t i = 0; i < nCounts; i++)
    {
        pInfo = pCfg->GetOneChannelInfo(i);
        ASSERT(pInfo);

        // ѡȡ�״γ��ֵ�ѹ�������͵�����
        if(CHANNEL_TYPE_SMV_VOLTAGE != pInfo->enChannelType || 
            CHANNEL_TYPE_SMV_CURRENT != pInfo->enChannelType)
        {
            enumChannelType eType = pInfo->enChannelType;

            // ���������������ͨ������
            int nA = _findChannel(eType, CHANNEL_PHASE_A, i);
            int nB = _findChannel(eType, CHANNEL_PHASE_B, nA + 1);
            int nC = _findChannel(eType, CHANNEL_PHASE_C, nB + 1);

            // ��������һ��ͨ������δ�ҵ����������i��ʼ�����ҵ�һ�����ֵ�����ͨ��
            if(nA == UINT_MAX || nB == UINT_MAX || nC == UINT_MAX)
            {
                nA = _findChannel(eType, CHANNEL_PHASE_A, i);
                nB = _findChannel(eType, CHANNEL_PHASE_B, i);
                nC = _findChannel(eType, CHANNEL_PHASE_C, i);
            }

            // ���ABC����ͨ�����ҵ����ʵģ������õ����ƿ�����������в�����
            if(nA != UINT_MAX && nB != UINT_MAX && nC != UINT_MAX)
            {
                pData->nChannelIndex[0] = nA;
                pData->nChannelIndex[1] = nB;
                pData->nChannelIndex[2] = nC;

                for(int k = 0; k < 3; k++)
                {
                    pData->bChannelValid[k] = true;
                }

                // �����ݲ�������������
                m_data.SetXLMode(eType);
                m_data.SetABCChannles(nA, nB, nC);

                return;
            }
        }
    }

    // ������������ѡȡ��������
    for(size_t i = 0, j = 0; i < nCounts && j < 3; i++)
    {
        pInfo = pCfg->GetOneChannelInfo(i);
        ASSERT(pInfo);

        // ѡȡ�״γ��ֵ����������͵�����
        if(CHANNEL_TYPE_SMV_CURRENT != pInfo->enChannelType && CHANNEL_TYPE_SMV_VOLTAGE != pInfo->enChannelType)
        {
            continue;
        }

        pData->nChannelIndex[j++] = pInfo->nIndex;

        if(j == 3)
        {
            // Ĭ�����õ�ѹ����
            m_data.SetXLMode(CHANNEL_TYPE_SMV_VOLTAGE);
            m_data.SetABCChannles(pData->nChannelIndex[0], pData->nChannelIndex[1], pData->nChannelIndex[2]);
            return;
        }
    }

    // ����Ƿ���ͨ��û������
    if(pData->nChannelIndex[0] != UINT_MAX)
    {
        for(int i = 0; i < 3; i++)
        {
            if(pData->nChannelIndex[i] == UINT_MAX)
            {
                pData->nChannelIndex[i] = pData->nChannelIndex[0];
                pData->bChannelValid[i] = true;
            }
        }
    }
}


void CSMVPhasesequeDlg::OnTimer( UINT_PTR nIDEvent )
{
    if(nIDEvent == m_nTimerID)
    {
        // ��ʱˢ������ҳ������
        _updatePhasae();
    }

    CSMVBaseDlg::OnTimer(nIDEvent);
}


/**
 *      ˢ�½�������
 */
void CSMVPhasesequeDlg::_updatePhasae()
{
    ASSERT(m_pCurrentCtrlBlk);

    if(m_bEnableTimer)
    {
        if(_checkChannel())
        {
            return;
        }

        // ��ȡ��������
        if(!m_pDataLayer->GetXuliangData(&m_data))
        {
            // ���ͼ��
            m_ctrlPhasor.ErasePhasor();
            m_ctrlPhasor.Invalidate(TRUE);
            return;
        }
    }

    CString csValue;
    float fMaxLen = 0.0;
    m_data.GetPhaseSeqAngle(m_fAngel[2], m_fAngel[0], m_fAngel[1]);
    m_data.GetPhaseSeqContent(m_fValue[2], m_fValue[0], m_fValue[1]);

    // ��ȡ���ƿ������
    CCtrlBlkDataPhasaSeq* pData = dynamic_cast<CCtrlBlkDataPhasaSeq*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));
    ASSERT(pData);

    // ������������
    for(int i = 0; i < 3; i++)
    {
        if(!pData->bChannelValid[i])
        {
#if SU_FEI
            m_reportPhase.SetRowItemText(i + 6, 1, g_csBlankValue);
#else
			m_reportPhase.SetRowItemText(i + 4, 1, g_csBlankValue);
#endif
            continue;
        }

        // ����һ�ζ���ֵ����
        if(m_eDispType == SMV_VALUE_TYPE_PRIMARY || (m_eFrameType == SMV_VALUE_TYPE_PRIMARY && m_eDispType == SMV_VALUE_TYPE_ORIGINANL))
        {
            CSmvRecvChannelInfo* pChannel = m_pCurrentCtrlBlk->pRecvConfig->GetOneChannelInfo(pData->nChannelIndex[i]);
            ASSERT(pChannel);

            if(pChannel && pChannel->enChannelType == CHANNEL_TYPE_SMV_VOLTAGE)
            {
                m_fValue[i] = m_fValue[i] / 1000;
            }
        }

        csValue.Format(L"%0.3f %s/%0.3f��", m_fValue[i], m_csUnit[i], m_fAngel[i]);
#if SU_FEI
        m_reportPhase.SetRowItemText(i + 6, 1, csValue);
#else
		m_reportPhase.SetRowItemText(i + 4, 1, csValue);
#endif
        if(fMaxLen < m_fValue[i])
        {
            fMaxLen = m_fValue[i];
        }
    }

    // ����Ƶ������
    float fMaxTemp  = float(fMaxLen / 4.0 * pow(2, m_nTimes));
    float fMargin   = fMaxTemp * 0.1f;
    float fDiff     = m_fScaleInterval - fMaxTemp;

    if(fDiff < 0.0f || fDiff > fMargin)
    {
        m_fScaleInterval = fMaxTemp;
        m_ctrlPhasor.SetLenScaleInterval(m_fScaleInterval);
    }


#if !CHUN_HUA
	for (int i =0; i<3;i++)
	{
		CELRecord*  record = m_reportPhase.GetRecordRow(i);
		CELRecordItem*  curItem = record->GetItem(0);
		curItem->SetColor(m_sColor[i],EL_COLOR_DEFAULT);
		m_reportPhase.RedrawRowItem(i,0);
		CELRecordItem*  curItem1 = record->GetItem(1);
		curItem1->SetColor(m_sColor[i],EL_COLOR_DEFAULT);
		m_reportPhase.RedrawRowItem(i,1);

		CELRecord*  record1 = m_reportPhase.GetRecordRow(i+4);
		CELRecordItem*  curItem2 = record1->GetItem(0);
		curItem2->SetColor(m_sColor[i],EL_COLOR_DEFAULT);
		m_reportPhase.RedrawRowItem(i+4,0);
		CELRecordItem*  curItem3 = record1->GetItem(1);
		curItem3->SetColor(m_sColor[i],EL_COLOR_DEFAULT);
		m_reportPhase.RedrawRowItem(i+4,1);
	}

#endif

    // ��������ʾ������
    m_ctrlPhasor.ErasePhasor();
    m_ctrlPhasor.DrawPhasor(m_fValue, m_fAngel, m_strDesc, 3);
    m_ctrlPhasor.Invalidate(TRUE);
}


/**
 *      �޸Ĳ˵�HMENU
 */
void CSMVPhasesequeDlg::_modifyMenu()
{
    __super::_modifyMenu();

	m_menuMain.SetMenuItemCaption(0, dsm_stringMgr::Get(DSM_STR_SMV_PHASESEQU_CAPTION));
	m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);
	m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);


    // ����F4����ѡ��ͼ��
    m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(m_dwMode == PHASASEQ_MODE_ZOOM ?  DSM_BMP_M_SMV_SCAN_STRETCH_SEL : DSM_BMP_M_SMV_SCAN_SEL ), NULL, NULL);

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
}

/**
 *      ���ݲ˵�ID�л���ָ���Ŀ��ƿ�
 *
 *  \param[in]
 *      nCmdID          ����˵�ID���������һ��ӳ��Ĺ�ϵ
 */
bool CSMVPhasesequeDlg::_switchCtrlBlk( UINT nCmdID )
{
    ASSERT(m_pPageDataExtra);

    // �����л�ǰ�Ŀ��ƿ����
    CBaseDetectItem* pPreCtrlBlk = m_pCurrentCtrlBlk;

    if(!__super::_switchCtrlBlk(nCmdID))
    {
        return false;
    }

    // ��ѡ���ϴεĲ˵��򷵻�
    if(pPreCtrlBlk == m_pCurrentCtrlBlk && pPreCtrlBlk->nID == m_pCurrentCtrlBlk->nID)
    {
        return true;
    }

    CCtrlBlkDataPhasaSeq* pData = dynamic_cast<CCtrlBlkDataPhasaSeq*>(m_pPageDataExtra->Get(pPreCtrlBlk->nID));

    // ����ԭ���ƿ��ͨ�����������ű���
    if(pData)
    {
        pData->nFoucs = m_reportPhase.GetFocusedRowIndex();
        pData->nTimes = m_nTimes;
    }

    // �����µĿ��ƿ����豨������
    _resetReport();

    return true;
}


void CSMVPhasesequeDlg::OnItemSelectChanged( NMHDR* pNotifyStruct, LRESULT* pResult )
{
    ASSERT(m_pCurrentCtrlBlk);

    ELNM_INPLACELIST_ARGS* pItemNotify = (ELNM_INPLACELIST_ARGS*)pNotifyStruct;
    ASSERT(pItemNotify);
    ASSERT(pItemNotify->rowIndex >= 0 && pItemNotify->rowIndex < 3);

    if(ELNM_ITEM_CONSTRAINTCHANGED != pItemNotify->hdr.code)
    {
        pResult = FALSE;
        return;
    }

    *pResult = TRUE;

    // ��ȡ���ƿ�󶨵�����
    CCtrlBlkDataPhasaSeq* pData = dynamic_cast<CCtrlBlkDataPhasaSeq*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));
    ASSERT(pData);

    int nSelChann = (int)pItemNotify->pConstraint->GetConstraintData();

    // ������ѡ���ͨ����Ϣ
    pData->nChannelIndex[pItemNotify->rowIndex] = nSelChann;
    pData->bChannelValid[pItemNotify->rowIndex] = true;

    // �����һ��ѡ����A�����͵�ͨ�������Զ��ͽ�Ϊ������ѡ��BC��ͨ��
    if(0 == pItemNotify->rowIndex)
    {
        CSmvRecvConfig* pCfg = m_pCurrentCtrlBlk->pRecvConfig;
        ASSERT(pCfg);

        CSmvRecvChannelInfo* pInfo = pCfg->GetOneChannelInfo(nSelChann);
        ASSERT(pInfo);

        // ��һ�в�����A������ͨ��
        if(CHANNEL_PHASE_A == pInfo->enChannelPhase)
        {
            // �������BC��ͨ��
            uint32_t nA = nSelChann;
            uint32_t nB = _findChannel(pInfo->enChannelType, CHANNEL_PHASE_B, nA + 1);
            uint32_t nC = _findChannel(pInfo->enChannelType, CHANNEL_PHASE_C, nB + 1);

            if(nB != UINT_MAX && nC != UINT_MAX)
            {
                pData->nChannelIndex[1] = nB;
                pData->nChannelIndex[2] = nC;

                pData->bChannelValid[1] = true;
                pData->bChannelValid[2] = true;

                // �����ݲ���������ͨ����Ϣ
                m_data.SetXLMode(pInfo->enChannelType);
                m_data.SetABCChannles(nA, nB, nC);

                m_reportPhase.SetRowItemText(1, 1, pCfg->GetOneChannelInfo(nB)->strChannelDesc.c_str());
                m_reportPhase.SetRowItemText(2, 1, pCfg->GetOneChannelInfo(nC)->strChannelDesc.c_str());

                // ������ͨ������ʾ��λ
                _updateUnit();

                return;
            }
        }
    }

    // ����ͨ�����ж�ͨ���ĵ�ѹ����������Ϣ
    CSmvRecvConfig* pCfg = m_pCurrentCtrlBlk->pRecvConfig;
    ASSERT(pCfg);

    CSmvRecvChannelInfo* pInfoA = pCfg->GetOneChannelInfo(pData->nChannelIndex[0]);
    CSmvRecvChannelInfo* pInfoB = pCfg->GetOneChannelInfo(pData->nChannelIndex[1]);
    CSmvRecvChannelInfo* pInfoC = pCfg->GetOneChannelInfo(pData->nChannelIndex[2]);

    if(pInfoA && pInfoB && pInfoC && 
        pInfoA->enChannelType == pInfoB->enChannelType &&
        pInfoB->enChannelType == pInfoC->enChannelType)
    {
        // ��������ģʽ
        m_data.SetXLMode(pInfoA->enChannelType);
        m_data.SetABCChannles(pInfoA->nIndex, pInfoB->nIndex, pInfoC->nIndex);
    }

    // ������ͨ������ʾ��λ
    _updateUnit();
}


/**
 *      ���ط���������ͨ����
 *
 *  \param[in]
 *      eType           ָ����ͨ������
 *  \param[in]
 *      ePhase          ָ����ͨ�����
 *  \param[in]
 *      nStart          ͨ���Ĳ�ѯ��ʼλ��
 *  \return
 *      int             ���ط���������ͨ���ţ��Ҳ����򷵻� UINT_MAX
 */
uint32_t CSMVPhasesequeDlg::_findChannel( enumChannelType eType, ChannelPhase ePhase, int nStart )
{
    CSmvRecvConfig* pCfg        = m_pCurrentCtrlBlk->pRecvConfig;
    size_t nCounts              = pCfg->GetChannelCount();
    CSmvRecvChannelInfo* pInfo  = NULL;

    // ѡ3����Ч��ͨ����ABC
    for(size_t i = nStart; i < nCounts; i++)
    {
        pInfo = pCfg->GetOneChannelInfo(i);
        ASSERT(pInfo);

        // �ҵ���������λ���ʵ�ͨ������ ����
        if(pInfo && eType == pInfo->enChannelType && ePhase == pInfo->enChannelPhase)
        {
            return pInfo->nIndex;
        }
    }

    return UINT_MAX;
}

/**
 *      ����ҳ��
 */
void CSMVPhasesequeDlg::_SavePage()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pPageDataExtra);

    CCtrlBlkDataPhasaSeq* pData = dynamic_cast<CCtrlBlkDataPhasaSeq*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));

    if(pData)
    {
        // �����н�����Ŵ���
        pData->nFoucs = m_reportPhase.GetFocusedRowIndex();
        pData->nTimes = m_nTimes;
    }
}

/**
 *      ʹ��ҳ����������ͨ��
 */
bool CSMVPhasesequeDlg::_restorePage()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pPageDataExtra);
    ASSERT(CSMVPhasesequeDlg::IDD == m_pPageDataExtra->GetPageData()->m_nPageID);


    // ��ȡ��ǰ�Ŀ��ƿ�����
    CCtrlBlkDataPhasaSeq* pData = dynamic_cast<CCtrlBlkDataPhasaSeq*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));

    // ��ǰ���ƿ���û�������Ϣ���򷵻�
    if(!pData)
    {
        return false;
    }

    uint32_t nChannels[3]           = {UINT_MAX, UINT_MAX, UINT_MAX};
    m_nTimes                        = pData->nTimes;
    CELRecordItem* pItem            = NULL;
    CELRecordItemConstraint* pCon   = NULL;

    // ����ԭ����ͨ����Ϣ
    for(int i = 0; i < 3; i++ )
    {
        pItem = m_reportPhase.GetRecordRow(i)->GetItem(1);
        ASSERT(pItem);

        if(pItem)
        {
            // ����ָ��ͨ��������ͨ������
            pCon = pItem->GetEditOptions(NULL)->FindConstraint(pData->nChannelIndex[i]);

            if(pCon)
            {
                nChannels[i]            = pData->nChannelIndex[i];
                pData->bChannelValid[i] = true;

                // ����ѡ��ͨ��������������
                m_reportPhase.SetRowItemText(i, 1, pCon->GetConstraintCaption());
            }
            else
            {
                pData->bChannelValid[i] = false;

                m_reportPhase.SetRowItemText(i, 1, g_csBlankValue);
#if SU_FEI
                m_reportPhase.SetRowItemText(i + 6, 1, g_csBlankValue);
#else
				m_reportPhase.SetRowItemText(i+4, 1, g_csBlankValue);
#endif
            }
        }
    }

    CSmvRecvConfig* pCfg = m_pCurrentCtrlBlk->pRecvConfig;
    ASSERT(pCfg);

    CSmvRecvChannelInfo* pInfo = pCfg->GetOneChannelInfo(pData->nChannelIndex[0]);

    // ��������ģʽ�ļ��������
    if(pInfo)
    {
        m_data.SetXLMode(pInfo->enChannelType);
    }
    else
    {
        m_data.SetXLMode(CHANNEL_TYPE_SMV_VOLTAGE);
    }

    // ��������ģʽ��ABC����ͨ������
    m_data.SetABCChannles(nChannels[0], nChannels[1], nChannels[2]);

    if(pData->nFoucs >= 0 && pData->nFoucs < m_reportPhase.GetRecordRowCount())
    {
        m_reportPhase.SetFocusedSelectionRowIndex(pData->nFoucs);
    }

    return true;
}

void CSMVPhasesequeDlg::OnMenuF4()
{
    if(!m_bChannelChange)
    {
        // �л�����/ѡ��ģʽ
        m_dwMode = m_dwMode == PHASASEQ_MODE_ZOOM ? PHASASEQ_MODE_SELECT : PHASASEQ_MODE_ZOOM;

        // �л�ģʽΪ��ѡ��
        if(PHASASEQ_MODE_SELECT == m_dwMode)
        {
            m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_SEL), NULL, NULL);
        }
        // �л�ģʽΪ�����š�
        else
        {
            m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_STRETCH_SEL), NULL, NULL);
        }

        m_pNaviBar->UpdateNaviMenu();
    }
}


BOOL CSMVPhasesequeDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: �ڴ����ר�ô����/����û���
    if(WM_KEYDOWN == pMsg->message)
    {
        // ��������ģʽ
        if(!m_bChannelChange && PHASASEQ_MODE_ZOOM == m_dwMode)
        {
            switch(pMsg->wParam)
            {
                // �Ŵ�ͼ��
            case VK_UP:
                if(PHASASEQ_TIME_MIN < m_nTimes)
                {
                    m_nTimes--;
                    _updatePhasae();
                }

                return TRUE;

                // ��Сͼ��
            case VK_DOWN:
                if(PHASASEQ_TIME_MAX > m_nTimes)
                {
                    m_nTimes++;
                    _updatePhasae();
                }

                return TRUE;
            }
        }
    }

    return CSMVBaseDlg::PreTranslateMessage(pMsg);
}

/**
 *      ������ѡͨ���ĵ�λ
 */
void CSMVPhasesequeDlg::_updateUnit()
{
    CSmvRecvConfig* pCfg = m_pCurrentCtrlBlk->pRecvConfig;
    ASSERT(pCfg);

    // ��ȡ���ƿ�󶨵�����
    CCtrlBlkDataPhasaSeq* pData = dynamic_cast<CCtrlBlkDataPhasaSeq*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));
    ASSERT(pData);

    CSmvRecvChannelInfo* pInfo      = NULL;

    for(int i = 0; i < 3; i++)
    {
        // ͨ����Ч�����õ�λ
        if(pData->bChannelValid[i])
        {
            pInfo = pCfg->GetOneChannelInfo(pData->nChannelIndex[i]);

            if(pInfo)
            {
                // ����ͨ���ĵ�λ
                m_csUnit[i] = DM_GetUnitFromType(pInfo->enChannelType, m_eDispType, m_eFrameType);
            }
            else
            {
                m_csUnit[i] = L"";
            }
        }
        else
        {
            m_csUnit[i] = L"";
        }
    }
}

/**
 *      ���ͨ���Ƿ����仯
 */
bool CSMVPhasesequeDlg::_checkChannel()
{
    // ͨ�������仯��
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

            // �������ͼ
            m_ctrlPhasor.ErasePhasor();
            m_ctrlPhasor.Invalidate(TRUE);

            // ����ͨ��ѡ��
            m_reportPhase.GetRecordRow(0)->GetItem(1)->SetEditable(FALSE);
            m_reportPhase.GetRecordRow(1)->GetItem(1)->SetEditable(FALSE);
            m_reportPhase.GetRecordRow(2)->GetItem(1)->SetEditable(FALSE);
#if SU_FEI
            // ����ͨ������ЧֵitemΪL"--"
            m_reportPhase.SetRowItemText(6, 1, g_csBlankValue);
            m_reportPhase.SetRowItemText(7, 1, g_csBlankValue);
            m_reportPhase.SetRowItemText(8, 1, g_csBlankValue);
#else
			// ����ͨ������ЧֵitemΪL"--"
			m_reportPhase.SetRowItemText(4, 1, g_csBlankValue);
			m_reportPhase.SetRowItemText(5, 1, g_csBlankValue);
			m_reportPhase.SetRowItemText(6, 1, g_csBlankValue);
#endif
            // ���ز��ֲ˵�����
            //m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, L"");
            m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(3, NULL);
            m_pNaviBar->UpdateNaviMenu();
        }

        return true;
    }
    else
    {
        if(m_bChannelChange)
        {
            m_bChannelChange = false;
            CStatusMgr::SetTips(L"", 0);

            // �ָ�ͨ��ѡ��
            m_reportPhase.GetRecordRow(0)->GetItem(1)->SetEditable(TRUE);
            m_reportPhase.GetRecordRow(1)->GetItem(1)->SetEditable(TRUE);
            m_reportPhase.GetRecordRow(2)->GetItem(1)->SetEditable(TRUE);

            // �ָ����ֲ˵�����
           // m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, dsm_stringMgr::Get(DSM_STR_TIMER_STOP));
            m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(m_dwMode == PHASASEQ_MODE_ZOOM ? DSM_BMP_M_SMV_SCAN_STRETCH_SEL : DSM_BMP_M_SMV_SCAN_SEL), NULL, NULL);
            m_pNaviBar->UpdateNaviMenu();
        }
    }

    return false;
}

void CSMVPhasesequeDlg::OnInplaceButtonDown( NMHDR* , LRESULT* pResult )
{
    if(!m_bEnableTimer)
    {
        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_TIMER_SW_CHAN_TIPS));
        *pResult = TRUE;
    }
}


