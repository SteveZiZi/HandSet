// SMVPhasorDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SMVPhasorDlg.h"

#include "src/main/utils/utility.h"
#include "src/main/mgr/statusmgr.h"
#include "src/main/smv/common_smv.h"
#include "src/main/pagedata/pagedata.h"
#include "src/utils/utility/dsm_utility.h"



#define PHASOR_MODE_SELECT          0x01        ///< ѡ��
#define PHASOR_MODE_ZOOM            0x02        ///< ����


#define PHASOR_TIME_MAX             4           ///< ���Ŵ���
#define PHASOR_TIME_MIN             -4          ///< ��С��С����

#if !CHUN_HUA
COLORREF m_Color[] ={DSM_PHASE_COLOR_A,DSM_PHASE_COLOR_B,DSM_PHASE_COLOR_C,DSM_PHASE_COLOR_N};
#endif

/**
 *      ����ҳ����ƿ��������
 */
class CCtrlBlkDataPhasor : public ICtrlBlkData
{
public:

    CCtrlBlkDataPhasor()
        : nFoucs(0)
        , nTimes(0)
    {
        for(int i = 0; i < 4; i++)
        {
            nChannelIndex[i] = UINT_MAX;
            bChannelValid[i] = false;
        }
    }

    virtual ~CCtrlBlkDataPhasor()
    {

    }


public:
    int nFoucs;                 ///< �����к�
    uint32_t nChannelIndex[4];  ///< ͨ����
    bool bChannelValid[4];      ///< ͨ����Ч��
    int nTimes;                 ///< �Ŵ���
};


// CSMVPhasorDlg �Ի���

IMPLEMENT_DYNAMIC(CSMVPhasorDlg, DSM_SMVBASEDLG)

CSMVPhasorDlg::CSMVPhasorDlg(CWnd* pParent /*=NULL*/)
	: DSM_SMVBASEDLG(CSMVPhasorDlg::IDD, DSM_STR_SMV_PHASOR_CAPTION, pParent)
    , m_dwMode(PHASOR_MODE_SELECT)
    , m_fScaleInterval(0.0f)
    , m_nTimes(0)
    , m_pCurCtrlBlkData(NULL)
    , m_eDispType(SMV_VALUE_TYPE_PRIMARY)
    , m_eFrameType(SMV_VALUE_TYPE_PRIMARY)
{
    for(int i = 0; i < 4; i++)
    {
        m_fLen[i]       = 0.0f;
        m_fValue[i]     = 0.0f;

        m_strDesc[i].Format(L"%d", i + 1);
    }
}

CSMVPhasorDlg::~CSMVPhasorDlg()
{
    CStatusMgr::SetTips(L"", 200);
}

void CSMVPhasorDlg::DoDataExchange(CDataExchange* pDX)
{
	DSM_SMVBASEDLG::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSMVPhasorDlg, DSM_SMVBASEDLG)
    ON_WM_TIMER()
    ON_COMMAND(ID_MENU_SMV_F4, &CSMVPhasorDlg::OnMenuF4)
    ON_NOTIFY(ELNM_ITEM_CONSTRAINTCHANGED, IDC_SMV_PHASOR_REPROT, &CSMVPhasorDlg::OnItemSelectChange)
END_MESSAGE_MAP()


// CSMVPhasorDlg ��Ϣ�������

BOOL CSMVPhasorDlg::OnInitDialog()
{
    DSM_SMVBASEDLG::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��

    m_eDispType      = m_pDataLayer->GetSmvConfigPublic()->GetSmvDispType();         // ��ȡͨ��ֵ����ʾ����
    m_eFrameType     = m_pDataLayer->GetParamConfigBasic()->GetFrameValueType();     // ��ȡͨ���ֵ����

    _createReport();
    _createPhasorGram();

    m_reportChannel.SetFocus();

    return FALSE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}


void CSMVPhasorDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

    if(nIDEvent == m_nTimerID)
    {
        // ˢ����������
        _updatePhasor();
    }

    CSMVBaseDlg::OnTimer(nIDEvent);
}

/**
 *      ������������
 */
void CSMVPhasorDlg::_createReport()
{
    CRect rcReport(DSM_PHASOR_REPORT_RECT);
    DSM_ParentToChild(this, rcReport);
    int nHeight = ELT_T1_ROWHEIGHT * 9;

    // �����߶�
    rcReport.top = (rcReport.Height() - nHeight) / 2;
    rcReport.bottom = rcReport.top + nHeight;
#if SU_FEI
    rcReport.OffsetRect(DSM_MARGIN_IN, 0);
#endif
    m_reportChannel.Create(DSM_WINSTYLE_REPORT_DEFAULT, rcReport, this, IDC_SMV_PHASOR_REPROT);

    // ��������
    m_reportChannel.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    // ������ʽ
    m_reportChannel.PostInitTemplateStyle();
    m_reportChannel.SetHeaderHeight(0);
	m_reportChannel.SetHasGridline(FALSE, TRUE);

    // ������Ԫ��
    m_reportChannel.InsertColumn(0, L"", EL_CENTER, DSM_PHASOR_REPORT_CAPTION);   // ����
    m_reportChannel.InsertColumn(1, L"", EL_LEFT, DSM_PHASOR_REPORT_VALUE);     // ����

    // ����ѡ�񽹵���ʽ
    m_reportChannel.SetRowSelectionVisible(TRUE);
    m_reportChannel.SetRowFocusEnable(TRUE);
    m_reportChannel.SetSubItemFocusEnable(TRUE);
    m_reportChannel.SetFreezeColumnsCount(1);
    m_reportChannel.SetFocusedColIndex(1);

    // �����������������ֵ
    _resetReport();
}


/**
 *      ��������ͼ
 */
void CSMVPhasorDlg::_createPhasorGram()
{
    CRect rcPhasor(DSM_PHASOR_PHASORGRAM_RECT);
    DSM_ParentToChild(this, rcPhasor);

    m_ctrlPhasor.Create(NULL, WS_CHILD | WS_VISIBLE, rcPhasor, this, IDC_SMV_PHASOR_PHASORGRAM);

    m_ctrlPhasor.ModifyStyle(0, SS_OWNERDRAW);

    // �������������ɫ
    m_ctrlPhasor.SetPhasorColor(0, DSM_PHASE_COLOR_A);
    m_ctrlPhasor.SetPhasorColor(1, DSM_PHASE_COLOR_B);
    m_ctrlPhasor.SetPhasorColor(2, DSM_PHASE_COLOR_C);
    m_ctrlPhasor.SetPhasorColor(3, DSM_PHASE_COLOR_N);

    m_ctrlPhasor.SetBkgndColor(DSM_COLOR_BK);
    CRect rcMargin(DSM_PHASOR_MARGIN);
    m_ctrlPhasor.SetMargin(rcMargin.left, rcMargin.right, rcMargin.top, rcMargin.bottom);
    m_ctrlPhasor.Init();
}

/**
 *      ���豨����ͨ������
 */
void CSMVPhasorDlg::_resetReport()
{
    ASSERT(m_pCurrentCtrlBlk);

    m_reportChannel.DeleteAllRecordRow();

    int nIndex = 0;

    // ����ͨ���� ABCN��
    m_reportChannel.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(m_pCurrentCtrlBlk, dsm_stringMgr::Get(DSM_STR_SMV_PHASOR_CHANN1), g_csBlankValue));
    m_reportChannel.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(m_pCurrentCtrlBlk, dsm_stringMgr::Get(DSM_STR_SMV_PHASOR_CHANN2), g_csBlankValue));
    m_reportChannel.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(m_pCurrentCtrlBlk, dsm_stringMgr::Get(DSM_STR_SMV_PHASOR_CHANN3), g_csBlankValue));
    m_reportChannel.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(m_pCurrentCtrlBlk, dsm_stringMgr::Get(DSM_STR_SMV_PHASOR_CHANN4), g_csBlankValue));

    // ����
     m_reportChannel.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(NULL, L"", L""));

     // ����ͨ��ֵ��
    m_reportChannel.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(NULL, dsm_stringMgr::Get(DSM_STR_SMV_PHASOR_CHANN1_VAL), g_csBlankValue));
    m_reportChannel.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(NULL, dsm_stringMgr::Get(DSM_STR_SMV_PHASOR_CHANN2_VAL), g_csBlankValue));
    m_reportChannel.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(NULL, dsm_stringMgr::Get(DSM_STR_SMV_PHASOR_CHANN3_VAL), g_csBlankValue));
    m_reportChannel.InsertRecordRow(nIndex++, new CDsmSmvPhasorRecord(NULL, dsm_stringMgr::Get(DSM_STR_SMV_PHASOR_CHANN4_VAL), g_csBlankValue));

    // ����ͨ��ֵ�в���ѡ��
    for(int i = 4; i < 9; i++)
    {
        m_reportChannel.GetRecordRow(i)->SetFocusable(FALSE);
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
void CSMVPhasorDlg::_adjustChannel()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pPageDataExtra);

    // ��ȡ���ƿ�����ݣ���Ϊ�գ����½�һ������
    m_pCurCtrlBlkData = dynamic_cast<CCtrlBlkDataPhasor*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));

    if(!m_pCurCtrlBlkData)
    {
        m_pCurCtrlBlkData = new CCtrlBlkDataPhasor;
        m_pPageDataExtra->Set(m_pCurrentCtrlBlk->nID, m_pCurCtrlBlkData);
    }

    // Ѱ��3�����ʵ�ͨ��
    _calcChannel();

    CSmvRecvChannelInfo* pInfo  = NULL;
    CSmvRecvConfig* pCfg        = m_pCurrentCtrlBlk->pRecvConfig;

    // ����3��ͨ����ֵ������
    for(int i = 0; i < 3; i++)
    {
        pInfo = pCfg->GetOneChannelInfo(m_pCurCtrlBlkData->nChannelIndex[i]);

        if(pInfo)
        {
            m_reportChannel.SetRowItemText(i, 1, pInfo->strChannelDesc.c_str());
        }
        else
        {
            m_reportChannel.SetRowItemText(i, 1, g_csBlankValue);
        }
    }

    // Ĭ��ûѡ��ͨ����ʾ L"--"
    m_reportChannel.SetRowItemText(3, 1, g_csBlankValue);
    m_reportChannel.SetRowItemText(8, 1, g_csBlankValue);

    m_reportChannel.SetFocusedSelectionRowIndex(0);

    m_pCurCtrlBlkData->nFoucs = 0;
}

/**
 *      Ѱ��3�����ʵ�Ĭ����ʾͨ����
 *
 *  \note
 *      ѡ�е�����Ĭ��ͨ�������������ͨ��������ҳ�����ݶ�����
 */
void CSMVPhasorDlg::_calcChannel()
{
    ASSERT(m_pCurCtrlBlkData);

    CSmvRecvConfig* pCfg        = m_pCurrentCtrlBlk->pRecvConfig;
    size_t nCounts              = pCfg->GetChannelCount();
    CSmvRecvChannelInfo* pInfo  = NULL;

    // ö������ͨ��
    for(size_t i = 0; i < nCounts; i++)
    {
        pInfo = pCfg->GetOneChannelInfo(i);
        ASSERT(pInfo);

        // ѡȡ�״γ��ֵ�ѹ���͵�����
        if(CHANNEL_TYPE_SMV_VOLTAGE == pInfo->enChannelType || 
            CHANNEL_TYPE_SMV_CURRENT == pInfo->enChannelType)
        {
            enumChannelType eType = pInfo->enChannelType;

            // ���������������ͨ������
            uint32_t nA = _findChannel(eType, CHANNEL_PHASE_A, i);
            uint32_t nB = _findChannel(eType, CHANNEL_PHASE_B, nA + 1);
            uint32_t nC = _findChannel(eType, CHANNEL_PHASE_C, nB + 1);

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
                m_pCurCtrlBlkData->nChannelIndex[0] = nA;
                m_pCurCtrlBlkData->nChannelIndex[1] = nB;
                m_pCurCtrlBlkData->nChannelIndex[2] = nC;

                for(int k = 0; k < 3; k++)
                {
                    m_pCurCtrlBlkData->bChannelValid[k] = true;
                }

                return;
            }
        }
    }

    // ������������ѡȡ��������
    for(size_t i = 0, j = 0; i < nCounts && j < 3; i++)
    {
        pInfo = pCfg->GetOneChannelInfo(i);
        ASSERT(pInfo);

        // ֻҪ�����ǵ�ѹ�����������
        if(CHANNEL_TYPE_SMV_CURRENT == pInfo->enChannelType || CHANNEL_TYPE_SMV_VOLTAGE == pInfo->enChannelType)
        {
            m_pCurCtrlBlkData->nChannelIndex[j] = pInfo->nIndex;
            m_pCurCtrlBlkData->bChannelValid[j] = true;
            j++;
        }
    }

    // ����Ƿ���ͨ��û������
    if(m_pCurCtrlBlkData->nChannelIndex[0] != UINT_MAX)
    {
        for(int i = 0; i < 3; i++)
        {
            if(m_pCurCtrlBlkData->nChannelIndex[i] == UINT_MAX)
            {
                m_pCurCtrlBlkData->nChannelIndex[i] = m_pCurCtrlBlkData->nChannelIndex[0];
                m_pCurCtrlBlkData->bChannelValid[i] = true;
            }
        }
    }
}

/**
 *      ˢ�½����ֵ
 */
void CSMVPhasorDlg::_updatePhasor()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pCurCtrlBlkData);

    // �����ö�ʱ�������ȡ��������
    if(m_bEnableTimer)
    {
        if(_checkChannel())
        {
            return;
        }

        // ��ȡ��������
        if(!m_pDataLayer->GetPhasorData(&m_data))
        {
            // �������ͼ
            m_ctrlPhasor.ErasePhasor();
            m_ctrlPhasor.Invalidate(TRUE);
            return;
        }
    }

    CString csValue;
    int nCounts = 3;
    float fMaxLen = 0.0f;
    float fPhaseA = 0.0f;
    CEffectiveChanelInfo*       pInfo = NULL;           ///< ͨ��������Ϣ

    for(int i = 0; i < 4; i++)
    {
        // ��Чͨ��
        if(!m_pCurCtrlBlkData->bChannelValid[i])
        {
            m_reportChannel.SetRowItemText(i + 5, 1, g_csBlankValue);

            m_fLen[i]       = 0.0f;
            m_fValue[i]     = 0.0f;

            continue;
        }

        // ��ȡѡ��ͨ������Ϣ
        pInfo = m_data.GetOneChannelInfo(m_pCurCtrlBlkData->nChannelIndex[i]);

        if(!pInfo)
        {
            continue;
        }

        // ����һ�ζ���ֵ����
        if(m_eDispType == SMV_VALUE_TYPE_PRIMARY || (m_eFrameType == SMV_VALUE_TYPE_PRIMARY && m_eDispType == SMV_VALUE_TYPE_ORIGINANL))
        {
            CSmvRecvChannelInfo* pChannel = m_pCurrentCtrlBlk->pRecvConfig->GetOneChannelInfo(m_pCurCtrlBlkData->nChannelIndex[i]);
            ASSERT(pChannel);

            if(pChannel && pChannel->enChannelType == CHANNEL_TYPE_SMV_VOLTAGE)
            {
                pInfo->m_fEffectiveValue = pInfo->m_fEffectiveValue / 1000;
            }
        }

        // ���ĸ�ͨ����Ч����������
        if(i == 3)
        {
            nCounts = 4;
        }

        // ����A����λ������A��Ϊ0��ʼֵ
        if(0 == i)
        {
            fPhaseA = pInfo->m_fPhase;
        }

        m_fLen[i]   = pInfo->m_fEffectiveValue;

        // ����ͨ����ͨ��1����λ��Ϊ��׼
        m_fValue[i] = DSM_GetRelativePhase(fPhaseA, pInfo->m_fPhase);

        csValue.Format(L"%0.2f %s/%0.2f��", m_fLen[i], m_csUnit[i], m_fValue[i]);

        m_reportChannel.SetRowItemText(i + 5, 1, csValue);

        // ��ȡ���ֵ
        if(fMaxLen < m_fLen[i])
        {
            fMaxLen = m_fLen[i];
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
	for (int i =0; i<4;i++)
	{
		CELRecord*  record = m_reportChannel.GetRecordRow(i);
		CELRecordItem*  curItem = record->GetItem(0);
		curItem->SetColor(m_Color[i],EL_COLOR_DEFAULT);
		m_reportChannel.RedrawRowItem(i,0);
		CELRecordItem*  curItem1 = record->GetItem(1);
		curItem1->SetColor(m_Color[i],EL_COLOR_DEFAULT);
		m_reportChannel.RedrawRowItem(i,1);

		CELRecord*  record1 = m_reportChannel.GetRecordRow(i+5);
		CELRecordItem*  curItem2 = record1->GetItem(0);
		curItem2->SetColor(m_Color[i],EL_COLOR_DEFAULT);
		m_reportChannel.RedrawRowItem(i+5,0);
		CELRecordItem*  curItem3 = record1->GetItem(1);
		curItem3->SetColor(m_Color[i],EL_COLOR_DEFAULT);
		m_reportChannel.RedrawRowItem(i+5,1);
	}
	
#endif

    // ��������ʾ������
    m_ctrlPhasor.ErasePhasor();
    m_ctrlPhasor.DrawPhasor(m_fLen, m_fValue, m_strDesc, nCounts);
    m_ctrlPhasor.Invalidate(TRUE);
}

void CSMVPhasorDlg::OnMenuF4()
{
    if(!m_bChannelChange)
    {
        // �л�����/ѡ��ģʽ
        m_dwMode = m_dwMode == PHASOR_MODE_ZOOM ? PHASOR_MODE_SELECT : PHASOR_MODE_ZOOM;

        // �л�ģʽΪ��ѡ��
        if(PHASOR_MODE_SELECT == m_dwMode)
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


BOOL CSMVPhasorDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: �ڴ����ר�ô����/����û���
    if(WM_KEYDOWN == pMsg->message)
    {
        // ��������ģʽ
        if(!m_bChannelChange && PHASOR_MODE_ZOOM == m_dwMode)
        {
            switch(pMsg->wParam)
            {
            case VK_UP:
                // �Ŵ�����ͼ
                if(PHASOR_TIME_MIN < m_nTimes)
                {
                    m_nTimes--;

                    // ���½���
                    _updatePhasor();
                }

                return TRUE;

            case VK_DOWN:
                // �Ŵ�����ͼ
                if(PHASOR_TIME_MAX > m_nTimes)
                {
                    m_nTimes++;

                    // ���½���
                    _updatePhasor();
                }

                return TRUE;
            }
        }
    }

    return CSMVBaseDlg::PreTranslateMessage(pMsg);
}

/**
 *      ����ҳ��״̬��Ϣ
 */
void CSMVPhasorDlg::_SavePage()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pPageDataExtra);
    ASSERT(m_pCurCtrlBlkData);

    // �����н�����Ŵ���
    m_pCurCtrlBlkData->nFoucs = m_reportChannel.GetFocusedRowIndex();
    m_pCurCtrlBlkData->nTimes = m_nTimes;
}

/**
 *      �޸Ĳ˵�HMENU
 */
void CSMVPhasorDlg::_modifyMenu()
{
    __super::_modifyMenu();

	m_menuMain.SetMenuItemCaption(0, dsm_stringMgr::Get(DSM_STR_SMV_PHASOR_CAPTION));
	m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);
	m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);


    // ����F4����ѡ��ͼ��
    m_menuMain.SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(m_dwMode == PHASOR_MODE_ZOOM ? DSM_BMP_M_SMV_SCAN_STRETCH_SEL : DSM_BMP_M_SMV_SCAN_SEL ), NULL, NULL);


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
 *      nCmdID          ���ƿ�ӳ�������˵�ID
 */
bool CSMVPhasorDlg::_switchCtrlBlk( UINT nCmdID )
{
    // �����л�ǰ�Ŀ��ƿ����
    CBaseDetectItem* pPreCtrlBlk = m_pCurrentCtrlBlk;

    // �л����ƿ�
    if(!__super::_switchCtrlBlk(nCmdID))
    {
        return false;
    }

    // ��ѡ���ϴεĲ˵��򷵻�
    if(pPreCtrlBlk == m_pCurrentCtrlBlk && pPreCtrlBlk->nID == m_pCurrentCtrlBlk->nID)
    {
        return true;
    }

    // ����ԭ���ƿ��ͨ������
    CCtrlBlkDataPhasor* pData = dynamic_cast<CCtrlBlkDataPhasor*>(m_pPageDataExtra->Get(pPreCtrlBlk->nID));

    // ����ԭ���ƿ�����ı����������ű���
    if(pData)
    {
        pData->nFoucs = m_reportChannel.GetFocusedRowIndex();
        pData->nTimes = m_nTimes;
    }

    // �����µĿ��ƿ����豨��
    _resetReport();

    return true;
}


/**
 *      ʹ��ҳ����������ͨ��
 */
bool CSMVPhasorDlg::_restorePage()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pPageDataExtra);
    ASSERT(CSMVPhasorDlg::IDD == m_pPageDataExtra->GetPageData()->m_nPageID);

    // ��ȡ��ǰ�Ŀ��ƿ������ҳ������
    CCtrlBlkDataPhasor* pData = dynamic_cast<CCtrlBlkDataPhasor*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));

    // ��ǰ���ƿ���û�������Ϣ���򷵻�
    if(!pData)
    {
        return false;
    }

    m_pCurCtrlBlkData               = pData;
    m_nTimes                        = m_pCurCtrlBlkData->nTimes;
    CELRecordItem* pItem            = NULL;
    CELRecordItemConstraint* pCon   = NULL;

    // ����ԭ����ͨ����Ϣ
    for(int i = 0; i < 4; i++ )
    {
        pItem = m_reportChannel.GetRecordRow(i)->GetItem(1);
        ASSERT(pItem);

        if(pItem)
        {
            // ����ָ��ͨ��������ͨ������
            pCon = pItem->GetEditOptions(NULL)->FindConstraint(m_pCurCtrlBlkData->nChannelIndex[i]);

            if(pCon)
            {
                m_pCurCtrlBlkData->bChannelValid[i] = true;         // ����ͨ����Ч

                // ����ѡ��ͨ��������������
                m_reportChannel.SetRowItemText(i, 1, pCon->GetConstraintCaption());
            }
            else
            {
                m_pCurCtrlBlkData->bChannelValid[i] = false;        // ����ͨ����Ч

                m_reportChannel.SetRowItemText(i, 1, g_csBlankValue);
                m_reportChannel.SetRowItemText(i + 5, 1, g_csBlankValue);
            }
        }
    }

    if(m_pCurCtrlBlkData->nFoucs >= 0 && m_pCurCtrlBlkData->nFoucs < m_reportChannel.GetRecordRowCount())
    {
        m_reportChannel.SetFocusedSelectionRowIndex(m_pCurCtrlBlkData->nFoucs);
    }

    return true;
}

void CSMVPhasorDlg::OnItemSelectChange( NMHDR* pNotifyStruct, LRESULT *pResult )
{
    ASSERT(m_pPageDataExtra);
    ASSERT(m_pCurrentCtrlBlk);

    ELNM_INPLACELIST_ARGS* pItemNotify = (ELNM_INPLACELIST_ARGS*)pNotifyStruct;
    ASSERT(pItemNotify);
    ASSERT(pItemNotify->rowIndex >= 0 && pItemNotify->rowIndex < 3);

    if(ELNM_ITEM_CONSTRAINTCHANGED != pItemNotify->hdr.code)
    {
        pResult = FALSE;
        return;
    }

    uint32_t nSelChan = (uint32_t)pItemNotify->pConstraint->GetConstraintData();

    // ������ѡ���ͨ����
    m_pCurCtrlBlkData->nChannelIndex[pItemNotify->rowIndex] = nSelChan;
    m_pCurCtrlBlkData->bChannelValid[pItemNotify->rowIndex] = true;

    // �����һ��ѡ����A�����͵�ͨ�������Զ��ͽ�Ϊ������ѡ��BC��ͨ��
    if(0 == pItemNotify->rowIndex)
    {
        CSmvRecvConfig* pCfg = m_pCurrentCtrlBlk->pRecvConfig;
        ASSERT(pCfg);

        CSmvRecvChannelInfo* pInfo = pCfg->GetOneChannelInfo(nSelChan);
        ASSERT(pInfo);

        if(CHANNEL_PHASE_A == pInfo->enChannelPhase)
        {
            // �������BC��ͨ��
            int nB = _findChannel(pInfo->enChannelType, CHANNEL_PHASE_B, nSelChan + 1);
            int nC = _findChannel(pInfo->enChannelType, CHANNEL_PHASE_C, nB + 1);

            // �ҵ����ʵ�BC��ͨ���������õ����ƿ���������ݶ�����
            if(nB != UINT_MAX && nC != UINT_MAX)
            {
                m_pCurCtrlBlkData->nChannelIndex[1] = nB;
                m_pCurCtrlBlkData->nChannelIndex[2] = nC;

                m_pCurCtrlBlkData->bChannelValid[1] = true;
                m_pCurCtrlBlkData->bChannelValid[2] = true;

                m_reportChannel.SetRowItemText(1, 1, pCfg->GetOneChannelInfo(nB)->strChannelDesc.c_str());
                m_reportChannel.SetRowItemText(2, 1, pCfg->GetOneChannelInfo(nC)->strChannelDesc.c_str());
            }
        }
    }

    // ����ͨ���ĵ�λ
    _updateUnit();

    // ˢ�¸���ͨ�����ֵ
    _updatePhasor();

    *pResult = TRUE;
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
 *      uint32_t             ���ط���������ͨ���ţ��Ҳ����򷵻� UINT_MAX
 */
uint32_t CSMVPhasorDlg::_findChannel( enumChannelType eType, ChannelPhase ePhase, int nStart )
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
 *      ������ѡͨ���ĵ�λ
 */
void CSMVPhasorDlg::_updateUnit()
{
    CSmvRecvConfig* pCfg = m_pCurrentCtrlBlk->pRecvConfig;
    ASSERT(pCfg);

    CSmvRecvChannelInfo* pInfo      = NULL;

    for(int i = 0; i < 4; i++)
    {
        // ͨ����Ч�����õ�λ
        if(m_pCurCtrlBlkData->bChannelValid[i])
        {
            pInfo = pCfg->GetOneChannelInfo(m_pCurCtrlBlkData->nChannelIndex[i]);

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
 *      ���ͨ���Ƿ�仯
 *
 *  \return
 *      bool        �����仯����true��û�仯����false
 */
bool CSMVPhasorDlg::_checkChannel()
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
            m_reportChannel.GetRecordRow(0)->GetItem(1)->SetEditable(FALSE);
            m_reportChannel.GetRecordRow(1)->GetItem(1)->SetEditable(FALSE);
            m_reportChannel.GetRecordRow(2)->GetItem(1)->SetEditable(FALSE);
            m_reportChannel.GetRecordRow(3)->GetItem(1)->SetEditable(FALSE);

            // ����ͨ������ЧֵitemΪL"--"
            m_reportChannel.SetRowItemText(5, 1, g_csBlankValue);
            m_reportChannel.SetRowItemText(6, 1, g_csBlankValue);
            m_reportChannel.SetRowItemText(7, 1, g_csBlankValue);
            m_reportChannel.SetRowItemText(8, 1, g_csBlankValue);

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
            m_reportChannel.GetRecordRow(0)->GetItem(1)->SetEditable(TRUE);
            m_reportChannel.GetRecordRow(1)->GetItem(1)->SetEditable(TRUE);
            m_reportChannel.GetRecordRow(2)->GetItem(1)->SetEditable(TRUE);
            m_reportChannel.GetRecordRow(3)->GetItem(1)->SetEditable(TRUE);

            // �ָ����ֲ˵�����
         //   m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, dsm_stringMgr::Get(DSM_STR_TIMER_STOP));
            m_pNaviBar->GetNaviMenu()->SetMenuItemBitmaps(3, dsm_bmpmgr::GetCB(m_dwMode == PHASOR_MODE_ZOOM ? DSM_BMP_M_SMV_SCAN_STRETCH_SEL : DSM_BMP_M_SMV_SCAN_SEL), NULL, NULL);
            m_pNaviBar->UpdateNaviMenu();
        }
    }

    return false;
}

