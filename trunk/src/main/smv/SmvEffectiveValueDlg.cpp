// SmvEffectiveValueDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SmvEffectiveValueDlg.h"
#include "src/main/resourcestr.h"
#include "src/main/eventproc.h"
#include "src/service/datalayer/dsmdatalayer.h"

#include "src/main/mgr/statusmgr.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/main/pagedata/pagedata.h"


//////////////////////////////////////////////////////////////////////////
// smv - ��Чֵ����
// 
#if SU_FEI
#define DSM_EFF_VAL_WIDTH_CHANNEL_INDEX     50
#define DSM_EFF_VAL_WIDTH_CHANNEL           230
#define DSM_EFF_VAL_WIDTH_EFF_VAL           160
#define DSM_EFF_VAL_WIDTH_PHASE_ANGLE       160

#else
#define DSM_EFF_VAL_WIDTH_CHANNEL_INDEX     50
#define DSM_EFF_VAL_WIDTH_CHANNEL           199
#define DSM_EFF_VAL_WIDTH_EFF_VAL           140
#define DSM_EFF_VAL_WIDTH_PHASE_ANGLE       140

#endif

/**
 *     ��Чֵ
 *
 *  \note
 *      ͨ�� ��Чֵ ���
 */
class CDsmEffValRecord : public CELRecord
{
public:

    CDsmEffValRecord()
    {
        AddItem(new CDsmRecordItem(L""));
        AddItem(new CDsmRecordItem(L""));
        AddItem(new CDsmRecordItem(L""));
        AddItem(new CDsmRecordItem(L""));
    }

    ~CDsmEffValRecord() {}
};

/**
 *      ��Чֵ���ƿ��������
 */
class CCtrlBlkDataEffectiveValue : public ICtrlBlkData
{
public:
    CCtrlBlkDataEffectiveValue()
    {

    }

    virtual ~CCtrlBlkDataEffectiveValue() {}

public:
};


// CSmvEffectiveValueDlg �Ի���

IMPLEMENT_DYNAMIC(CSmvEffectiveValueDlg, DSM_SMVBASEDLG)

CSmvEffectiveValueDlg::CSmvEffectiveValueDlg(CWnd* pParent /*=NULL*/)
	: DSM_SMVBASEDLG(CSmvEffectiveValueDlg::IDD, DSM_STR_EFF_VAL_CAPTION, pParent)
{

}

CSmvEffectiveValueDlg::~CSmvEffectiveValueDlg()
{
}

void CSmvEffectiveValueDlg::DoDataExchange(CDataExchange* pDX)
{
	DSM_SMVBASEDLG::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSmvEffectiveValueDlg, DSM_SMVBASEDLG)
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CSmvEffectiveValueDlg ��Ϣ�������

BOOL CSmvEffectiveValueDlg::OnInitDialog()
{
    DSM_SMVBASEDLG::OnInitDialog();
    
    // TODO:  �ڴ���Ӷ���ĳ�ʼ��
    
    if (m_pDataLayer == NULL)
    {
        return FALSE;
    }

    if(m_pCurrentCtrlBlk)
    {
        _beginTimer();
    }
    else
    {
        return FALSE;
    }
    
    _createReport();
    _updateEffectiveValue();
    _restorePage();

    _updateCaption();

    return TRUE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}


void CSmvEffectiveValueDlg::_createReport()
{
    CRect rcReport;
    GetClientRect(rcReport);
    

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

    m_ctlEffValReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_SMV_EFF_VAL_RET);

    // ��������
    m_ctlEffValReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_ctlEffValReport.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    
    m_ctlEffValReport.PostInitTemplateStyle();

    // ������Ԫ��

    m_ctlEffValReport.InsertColumn(0, 
        _T(""),
        EL_CENTER, DSM_EFF_VAL_WIDTH_CHANNEL_INDEX);               //ͨ������

    m_ctlEffValReport.InsertColumn(1, 
        dsm_stringMgr::Get(DSM_STR_EFF_VAL_CHANNEL),
        EL_LEFT, DSM_EFF_VAL_WIDTH_CHANNEL);                     //ͨ��

    m_ctlEffValReport.InsertColumn(2, 
        dsm_stringMgr::Get(DSM_STR_EFF_VAL_EFF_VAL), 
        EL_LEFT, DSM_EFF_VAL_WIDTH_EFF_VAL);                     //��Чֵ

    m_ctlEffValReport.InsertColumn(3, 
        dsm_stringMgr::Get(DSM_STR_EFF_VAL_PHASE_ANGLE), 
        EL_LEFT, DSM_EFF_VAL_WIDTH_PHASE_ANGLE);                 //���

    // ����ѡ�񽹵���ʽ
    m_ctlEffValReport.SetRowSelectionVisible(FALSE);
    m_ctlEffValReport.SetRowFocusEnable(FALSE);
    //m_ctlEffValReport.SetSubItemFocusEnable(FALSE);
    m_ctlEffValReport.SetFreezeSelectionVisible(TRUE);


    m_ctlEffValReport.SetFocus();

    _resetReport();
}


void CSmvEffectiveValueDlg::_resetReport()
{
    if (m_pCurrentCtrlBlk)
    {
        m_ctlEffValReport.DeleteAllRecordRow();
        for (int i = 0; i < (int)m_pCurrentCtrlBlk->nChannelCount; ++i)
        {
            m_ctlEffValReport.InsertRecordRow(i, new CDsmEffValRecord());
        }
    }
}

void CSmvEffectiveValueDlg::_updateEffectiveValue()
{
    
    if (_checkChannel())
    {
        return;
    }

    CEffectiveChanelInfo *pEffChannelInfo = NULL;

    CDsmEffValRecord* pRecord = NULL;
    if(!m_pDataLayer->GetEffectiveVal(&m_dataEffVal))
    {
        return ;
    }

    int nChannelCount = m_dataEffVal.GetChannelCount();
    if (nChannelCount == 0)
    {
        return;
    }

    CString strUnknown = _T("");
    strUnknown = dsm_stringMgr::Get(DSM_STR_EFF_VAL_UNKNOWN);

    CSmvRecvConfig* pConfig =  m_pCurrentCtrlBlk->pRecvConfig;
    if (pConfig == NULL)
    {
        return;
    }

    CString strCaption  = _T("");
    CString strEffValue  = _T("");
    CString strPhase  = _T("");
    
    float fPhaseMark = 0.0f;


    // ���ñ����ʼֵ
    CSmvRecvConfigPublic* pCfg = m_pDataLayer->GetSmvConfigPublic();
    ASSERT(pCfg);

    // һ�ζ�����ʾֵ
    switch(pCfg->GetSmvDispType())
    {
    case SMV_VALUE_TYPE_PRIMARY:
        {
            //
            //һ��ֵ��ʾ
            //
            BOOL bFindFirstMark = FALSE;
            for (int i = 0; i < nChannelCount; ++i)
            {
                pEffChannelInfo = m_dataEffVal.GetOneChannelInfo(i);
                if (pEffChannelInfo != NULL)
                {
                    if (!bFindFirstMark)
                    {
                        if (pConfig->GetOneChannelInfo(i)->enChannelType == CHANNEL_TYPE_SMV_CURRENT ||
                            pConfig->GetOneChannelInfo(i)->enChannelType == CHANNEL_TYPE_SMV_VOLTAGE )
                        {
                            fPhaseMark = m_dataEffVal.GetOneChannelInfo(i)->m_fPhase;
                            bFindFirstMark = TRUE;
                        }
                    }
                    pRecord = (CDsmEffValRecord*)m_ctlEffValReport.GetRecordRow(i);
                    strCaption.Format(_T("%d"), i + 1);
                    pRecord->GetItem(0)->SetCaption(strCaption);
                    pRecord->GetItem(1)->SetCaption(m_dataEffVal.GetChannelDesc(i).c_str());

                    switch(pConfig->GetOneChannelInfo(i)->enChannelType)
                    {
                    case CHANNEL_TYPE_SMV_CURRENT:
                        strEffValue.Format(_T("%.4f A"), pEffChannelInfo->m_fEffectiveValue);
                        strPhase.Format(_T("%.4f��"), DSM_GetRelativePhase(fPhaseMark, pEffChannelInfo->m_fPhase));

                        break;
                    case CHANNEL_TYPE_SMV_VOLTAGE:
                        strEffValue.Format(_T("%.4f kV"), pEffChannelInfo->m_fEffectiveValue/1000.0);
                        strPhase.Format(_T("%.4f��"), DSM_GetRelativePhase(fPhaseMark, pEffChannelInfo->m_fPhase));

                        break;
                    case CHANNEL_TYPE_SMV_TIME:
                        strEffValue.Format(_T("%.4f ��s"), pEffChannelInfo->m_fEffectiveValue);
                        strPhase.Format(_T("%.4f��"), 0);

                        break;
                    default:

                        strEffValue.Format(_T("%.4f"), pEffChannelInfo->m_fEffectiveValue);
                        strPhase.Format(_T("%.4f��"), DSM_GetRelativePhase(fPhaseMark, pEffChannelInfo->m_fPhase));
                        break;
                    }
                    pRecord->GetItem(2)->SetCaption(strEffValue);
                    pRecord->GetItem(3)->SetCaption(strPhase);
                }
                else
                {
                    //��ͨ������
                    pRecord = (CDsmEffValRecord*)m_ctlEffValReport.GetRecordRow(i);
                    CString strFormat = _T("");
                    strFormat.Format(_T("%d"), i + 1);
                    pRecord->GetItem(0)->SetCaption(strFormat);
                    pRecord->GetItem(1)->SetCaption(m_dataEffVal.GetChannelDesc(i).c_str());
                    pRecord->GetItem(2)->SetCaption(strUnknown);
                    pRecord->GetItem(3)->SetCaption(strUnknown);
                }
            }
        }
        break;
    case SMV_VALUE_TYPE_SECONDARY:
        {
            //
            //����ֵ��ʾ
            //
            BOOL bFindFirstMark = FALSE;
            for (int i = 0; i < nChannelCount; ++i)
            {
                pEffChannelInfo = m_dataEffVal.GetOneChannelInfo(i);
                if (pEffChannelInfo != NULL)
                {
                    if (!bFindFirstMark)
                    {
                        if (pConfig->GetOneChannelInfo(i)->enChannelType == CHANNEL_TYPE_SMV_CURRENT ||
                            pConfig->GetOneChannelInfo(i)->enChannelType == CHANNEL_TYPE_SMV_VOLTAGE )
                        {
                            fPhaseMark = m_dataEffVal.GetOneChannelInfo(i)->m_fPhase;
                            bFindFirstMark = TRUE;
                        }
                    }
                    pRecord = (CDsmEffValRecord*)m_ctlEffValReport.GetRecordRow(i);
                    strCaption.Format(_T("%d"), i + 1);
                    pRecord->GetItem(0)->SetCaption(strCaption);
                    pRecord->GetItem(1)->SetCaption(m_dataEffVal.GetChannelDesc(i).c_str());

                    switch(pConfig->GetOneChannelInfo(i)->enChannelType)
                    {
                    case CHANNEL_TYPE_SMV_CURRENT:
                        strEffValue.Format(_T("%.4f A"), pEffChannelInfo->m_fEffectiveValue);
                        strPhase.Format(_T("%.4f��"), DSM_GetRelativePhase(fPhaseMark, pEffChannelInfo->m_fPhase));

                        break;
                    case CHANNEL_TYPE_SMV_VOLTAGE:

                        strEffValue.Format(_T("%.4f V"), pEffChannelInfo->m_fEffectiveValue);
                        strPhase.Format(_T("%.4f��"), DSM_GetRelativePhase(fPhaseMark, pEffChannelInfo->m_fPhase));

                        break;
                    case CHANNEL_TYPE_SMV_TIME:
                        strEffValue.Format(_T("%.4f ��s"), pEffChannelInfo->m_fEffectiveValue);
                        strPhase.Format(_T("%.4f��"), 0);
                        break;
                    default:

                        strEffValue.Format(_T("%.4f"), pEffChannelInfo->m_fEffectiveValue);
                        strPhase.Format(_T("%.4f��"), DSM_GetRelativePhase(fPhaseMark, pEffChannelInfo->m_fPhase));
                        break;
                    }
                    pRecord->GetItem(2)->SetCaption(strEffValue);
                    pRecord->GetItem(3)->SetCaption(strPhase);
                }
                else
                {
                    //��ͨ������
                    pRecord = (CDsmEffValRecord*)m_ctlEffValReport.GetRecordRow(i);
                    CString strFormat = _T("");
                    strFormat.Format(_T("%d"), i + 1);
                    pRecord->GetItem(0)->SetCaption(strFormat);
                    pRecord->GetItem(1)->SetCaption(m_dataEffVal.GetChannelDesc(i).c_str());
                    pRecord->GetItem(2)->SetCaption(strUnknown);
                    pRecord->GetItem(3)->SetCaption(strUnknown);
                }
            }
        }
        break;
    case SMV_VALUE_TYPE_ORIGINANL:
        {
            //
            //ԭʼֵ��ʾ��û�е�λ��
            //
            BOOL bFindFirstMark = FALSE;
            for (int i = 0; i < nChannelCount; ++i)
            {
                pEffChannelInfo = m_dataEffVal.GetOneChannelInfo(i);
                if (pEffChannelInfo != NULL)
                {
                    if (!bFindFirstMark)
                    {
                        if (pConfig->GetOneChannelInfo(i)->enChannelType == CHANNEL_TYPE_SMV_CURRENT ||
                            pConfig->GetOneChannelInfo(i)->enChannelType == CHANNEL_TYPE_SMV_VOLTAGE )
                        {
                            fPhaseMark = m_dataEffVal.GetOneChannelInfo(i)->m_fPhase;
                            bFindFirstMark = TRUE;
                        }
                    }
                    pRecord = (CDsmEffValRecord*)m_ctlEffValReport.GetRecordRow(i);
                    strCaption.Format(_T("%d"), i + 1);
                    pRecord->GetItem(0)->SetCaption(strCaption);
                    pRecord->GetItem(1)->SetCaption(m_dataEffVal.GetChannelDesc(i).c_str());

                    switch(pConfig->GetOneChannelInfo(i)->enChannelType)
                    {
                    case CHANNEL_TYPE_SMV_CURRENT:
                        strEffValue.Format(_T("%.4f"), pEffChannelInfo->m_fEffectiveValue);
                        strPhase.Format(_T("%.4f��"), DSM_GetRelativePhase(fPhaseMark, pEffChannelInfo->m_fPhase));

                        break;
                    case CHANNEL_TYPE_SMV_VOLTAGE:

                        strEffValue.Format(_T("%.4f"), pEffChannelInfo->m_fEffectiveValue);

                        strPhase.Format(_T("%.4f��"), DSM_GetRelativePhase(fPhaseMark, pEffChannelInfo->m_fPhase));

                        break;
                    case CHANNEL_TYPE_SMV_TIME:
                        strEffValue.Format(_T("%.4f"), pEffChannelInfo->m_fEffectiveValue);
                        strPhase.Format(_T("%.4f��"), 0);

                        break;
                    default:

                        strEffValue.Format(_T("%.4f"), pEffChannelInfo->m_fEffectiveValue);
                        strPhase.Format(_T("%.4f��"), DSM_GetRelativePhase(fPhaseMark, pEffChannelInfo->m_fPhase));
                        break;
                    }
                    pRecord->GetItem(2)->SetCaption(strEffValue);
                    pRecord->GetItem(3)->SetCaption(strPhase);
                }
                else
                {
                    //��ͨ������
                    pRecord = (CDsmEffValRecord*)m_ctlEffValReport.GetRecordRow(i);
                    CString strFormat = _T("");
                    strFormat.Format(_T("%d"), i + 1);
                    pRecord->GetItem(0)->SetCaption(strFormat);
                    pRecord->GetItem(1)->SetCaption(m_dataEffVal.GetChannelDesc(i).c_str());
                    pRecord->GetItem(2)->SetCaption(strUnknown);
                    pRecord->GetItem(3)->SetCaption(strUnknown);
                }
            }
        }
        break;
    default:
        break;
    }
    m_ctlEffValReport.RedrawWindow();

}
/**
 *      ���ݲ˵�ID�л���ָ���Ŀ��ƿ�
 *
 *  /param[in]
 *      nCmdID      ���ƿ�����Ĳ˵�����ID
 */
bool CSmvEffectiveValueDlg::_switchCtrlBlk( UINT nCmdID )
{
    // ������һ�����ƿ����
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

    // �����¿��ƿ�ҳ��״̬��Ϣ���豨��
    _resetReport();

    return true;
}
bool CSmvEffectiveValueDlg::_checkChannel()
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

            CDsmEffValRecord* pRecord = NULL;
            // ����ͨ��ѡ���������
            for(int i = 0; i < (int)m_pCurrentCtrlBlk->nChannelCount; i++)
            {
                //
                //����ͨ��ֵΪ ��--��
                //
                pRecord = (CDsmEffValRecord*)m_ctlEffValReport.GetRecordRow(i);

                if (pRecord)
                {
                    pRecord->GetItem(2)->SetCaption(g_csBlankValue);
                    pRecord->GetItem(3)->SetCaption(g_csBlankValue);
                }
            }
            m_ctlEffValReport.RedrawWindow();

            // ���ء���ͣ��ʼ���˵�����
            m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, L"");
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

            // �ָ���ͣ��ʼ�˵�����
            m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, dsm_stringMgr::Get(DSM_STR_TIMER_STOP));
            m_pNaviBar->UpdateNaviMenu();
        }
    }

    return false;
}


void CSmvEffectiveValueDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    

    if (m_nTimerID == nIDEvent)
    {
        _updateEffectiveValue();
    }
    CSMVBaseDlg::OnTimer(nIDEvent);
}


void CSmvEffectiveValueDlg::_SavePage()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pPageDataExtra);

    CCtrlBlkDataEffectiveValue* pData = dynamic_cast<CCtrlBlkDataEffectiveValue*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));
    if(pData == NULL)
    {
        pData = new CCtrlBlkDataEffectiveValue();
        m_pPageDataExtra->Set(m_pCurrentCtrlBlk->nID, pData);
    }
}

bool CSmvEffectiveValueDlg::_restorePage()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pPageDataExtra);
    ASSERT(CSmvEffectiveValueDlg::IDD == m_pPageDataExtra->GetPageData()->m_nPageID);

    // ��ȡ��ǰ�Ŀ��ƿ�����
    CCtrlBlkDataEffectiveValue* pData = dynamic_cast<CCtrlBlkDataEffectiveValue*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));

    // ��ǰ���ƿ���û�������Ϣ���򷵻�
    if(!pData)
    {
        return false;
    }

    return true;
}

#if !SU_FEI
void CSmvEffectiveValueDlg::_modifyMenu()
{
	m_menuMain.SetMenuItemCaption(0, dsm_stringMgr::Get(DSM_STR_EFF_VAL_CAPTION));
	m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);
	m_menuMain.SetMenuItemBitmaps(1, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_ARGS_SEL), NULL);

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
#endif