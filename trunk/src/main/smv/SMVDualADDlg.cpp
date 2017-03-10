/**
 * \file
 *      SMVDualADDlg.cpp
 *
 * \brief
 *      ˫ADҳ��ʵ���ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/4/22
 */


// SMVDualADDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SMVDualADDlg.h"

#include "src/main/mgr/statusmgr.h"
#include "src/main/pagedata/pagedata.h"

//////////////////////////////////////////////////////////////////////////
// smv - ˫AD
#if SU_FEI
#define DSM_SMV_DUALAD_AD1                  190
#define DSM_SMV_DUALAD_AD2                  190
#define DSM_SMV_DUALAD_AD1S2                110
#define DSM_SMV_DUALAD_AD2S1                110
#else
#define DSM_SMV_DUALAD_AD1                  160
#define DSM_SMV_DUALAD_AD2                  160
#define DSM_SMV_DUALAD_AD1S2                104
#define DSM_SMV_DUALAD_AD2S1                105
#endif

#define DUALAD_COUNTS           DOUBLE_AD_GROUP_COUNT       ///< ˫AD����


/**
 *      SMV���� - ˫AD
 *
 *  \note
 *      AD1��AD2��|AD1 - AD2|��|AD1| - |AD2|
 */
class CDsmSmvDualADRecord : public CELRecord
{
public:
    CDsmSmvDualADRecord(CBaseDetectItem* pCtrlBlk);

    virtual ~CDsmSmvDualADRecord() {}

};


CDsmSmvDualADRecord::CDsmSmvDualADRecord( CBaseDetectItem* pCtrlBlk )
{
    ASSERT(pCtrlBlk);

    CELRecordItem* pItemAD[2] = {NULL, NULL};

    pItemAD[0] = AddItem(new CDsmRecordItem);           // AD1
    pItemAD[1] = AddItem(new CDsmRecordItem);           // AD2

    AddItem(new CDsmRecordItem(g_csBlankValue))->SetFocusable(FALSE);    // |AD1 - AD2|
    AddItem(new CDsmRecordItem(g_csBlankValue))->SetFocusable(FALSE);    // |AD1| - |AD2|

    pItemAD[0]->SetEditable(TRUE);
    pItemAD[1]->SetEditable(TRUE);

    CSmvRecvConfig* pCfg = pCtrlBlk->pRecvConfig;
    ASSERT(pCfg);

    CSmvRecvChannelInfo* pInfo          = NULL;
    size_t nCounts                      = pCfg->GetChannelCount();
    CELRecordItemEditOptions* pOption   = NULL;

    for(int i = 0; i < 2; i++)
    {
        pOption = pItemAD[i]->GetEditOptions(NULL);
        pOption->AddComboButton();

        // ��ʼ����ѡͨ���������б�
        for(size_t j = 0; j < nCounts; j++)
        {
            pInfo = pCfg->GetOneChannelInfo(j);

            if(CHANNEL_TYPE_SMV_VOLTAGE != pInfo->enChannelType &&
                CHANNEL_TYPE_SMV_CURRENT != pInfo->enChannelType)
            {
                continue;
            }

            pOption->AddConstraint(pInfo->strChannelDesc.c_str(), pInfo->nIndex);
        }
    }
}



/**
 *      ˫ADҳ����ƿ����������
 */
class CCtrlBlkDataDualAD : public ICtrlBlkData
{
public:
    CCtrlBlkDataDualAD()
        : m_nFocusRow(0)
        , m_nFocusCol(0)
    {
        for(int i = 0; i < DUALAD_COUNTS; i++)
        {
            for(int j = 0; j < 2; j++)
            {
                m_arrDualAD[i][j] = -1;
            }
        }
    }

    virtual ~CCtrlBlkDataDualAD()
    {

    }

public:
    int         m_nFocusRow;                        ///< ������
    int         m_nFocusCol;                        ///< ������

    int         m_arrDualAD[DUALAD_COUNTS][2];      ///< ˫AD��ͨ����
};



// CSMVDualADDlg �Ի���

IMPLEMENT_DYNAMIC(CSMVDualADDlg, DSM_SMVBASEDLG)

CSMVDualADDlg::CSMVDualADDlg(CWnd* pParent /*=NULL*/)
	: DSM_SMVBASEDLG(CSMVDualADDlg::IDD, DSM_STR_SMV_DUALAD_CAPTION, pParent)
{

}

CSMVDualADDlg::~CSMVDualADDlg()
{
    CStatusMgr::SetTips(L"", 200);
}

void CSMVDualADDlg::DoDataExchange(CDataExchange* pDX)
{
	DSM_SMVBASEDLG::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSMVDualADDlg, DSM_SMVBASEDLG)
    ON_WM_TIMER()
    ON_NOTIFY(ELNM_ITEM_CONSTRAINTCHANGED, IDC_SMV_DUALAD_REPORT, &CSMVDualADDlg::OnItemSelectChange)
    ON_NOTIFY(ELNM_INPLACEBUTTONDOWN, IDC_SMV_DUALAD_REPORT, &CSMVDualADDlg::OnInplaceButtonDown)
END_MESSAGE_MAP()


// CSMVDualADDlg ��Ϣ�������


BOOL CSMVDualADDlg::OnInitDialog()
{
    CSMVBaseDlg::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��

    _createReport();

    return FALSE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}


/**
 *      ����˫AD����
 */
void CSMVDualADDlg::_createReport()
{
    CRect rcClient;
    GetClientRect(rcClient);
    rcClient.DeflateRect(DSM_MARGIN_IN, DSM_MARGIN_IN, DSM_MARGIN_IN, 0);
    rcClient.bottom = rcClient.top + ELT_T1_HDRHEIGHT + ELT_T1_ROWHEIGHT * (4 + 5);

    m_reportAD.Create(DSM_WINSTYLE_REPORT_DEFAULT, rcClient, this, IDC_SMV_DUALAD_REPORT);

    // ��������
    m_reportAD.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_reportAD.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    // ������ʽ
    m_reportAD.PostInitTemplateStyle();

    // ������Ԫ��
    m_reportAD.InsertColumn(0, L"         " + dsm_stringMgr::Get(DSM_STR_SMV_DUALAD_HEAD_AD1), EL_LEFT, DSM_SMV_DUALAD_AD1);         // AD1
    m_reportAD.InsertColumn(1, L"         " + dsm_stringMgr::Get(DSM_STR_SMV_DUALAD_HEAD_AD2), EL_LEFT, DSM_SMV_DUALAD_AD2);         // AD2
    m_reportAD.InsertColumn(2, dsm_stringMgr::Get(DSM_STR_SMV_DUALAD_HEAD_AD1S2), EL_CENTER, DSM_SMV_DUALAD_AD1S2);     // ��ľ���ֵ
    m_reportAD.InsertColumn(3, dsm_stringMgr::Get(DSM_STR_SMV_DUALAD_HEAD_AD2S1), EL_CENTER, DSM_SMV_DUALAD_AD2S1);     // ����ֵ�Ĳ�

    // ����ѡ�񽹵���ʽ
    m_reportAD.SetRowSelectionVisible(FALSE);
    m_reportAD.SetRowFocusEnable(TRUE);
    m_reportAD.SetSubItemFocusEnable(TRUE);
    m_reportAD.SetFreezeSelectionVisible(TRUE);
    m_reportAD.SetFocusedColIndex(0);

    // ���豨��ĸ���ֵ
    _resetReport();

    m_reportAD.SetFocus();
}


/**
 *      ��ʼ������
 */
void CSMVDualADDlg::_resetReport()
{
    ASSERT(m_pCurrentCtrlBlk);

    _updateCaption();

    m_reportAD.DeleteAllRecordRow();

    // ����˫AD��
    for(int i = 0; i < DUALAD_COUNTS; i++)
    {
        m_reportAD.InsertRecordRow(i, new CDsmSmvDualADRecord(m_pCurrentCtrlBlk));
    }

    // ����ҳ������������ѡͨ��
    // ��ҳ������û�������Ϣ�����Զ�����
    if(!_restorePage())
    {
        _adjustChannel();
    }
}

/**
 *      �ָ�ҳ��������Ϣ
 */
bool CSMVDualADDlg::_restorePage()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(CSMVDualADDlg::IDD == m_pPageDataExtra->GetPageData()->m_nPageID);

    // ��ȡ��ǰ�Ŀ��ƿ�����
    CCtrlBlkDataDualAD* pData = dynamic_cast<CCtrlBlkDataDualAD*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));

    // ��ǰ���ƿ���û�������Ϣ
    // ���޷��������ݻָ�ҳ��
    if(!pData)
    {
        return false;
    }

    CELRecordItem* pItem1               = NULL;
    CELRecordItem* pItem2               = NULL;
    CELRecordItemConstraint* pCon1      = NULL;
    CELRecordItemConstraint* pCon2      = NULL;

    // ����ԭ����ͨ����Ϣ
    for(int i = 0; i < DUALAD_COUNTS; i++ )
    {
        pItem1 = m_reportAD.GetRecordRow(i)->GetItem(0);
        pItem2 = m_reportAD.GetRecordRow(i)->GetItem(1);

        ASSERT(pItem1);
        ASSERT(pItem2);

        pCon1 = pItem1->GetEditOptions(NULL)->FindConstraint(pData->m_arrDualAD[i][0]);
        pCon2 = pItem2->GetEditOptions(NULL)->FindConstraint(pData->m_arrDualAD[i][1]);

        if(pCon1 && pCon2)
        {
            m_reportAD.SetRowItemText(i, 0, pCon1->GetConstraintCaption());
            m_reportAD.SetRowItemText(i, 1, pCon2->GetConstraintCaption());

            m_data.SetAD1Channel(i, pData->m_arrDualAD[i][0]);
            m_data.SetAD2Channel(i, pData->m_arrDualAD[i][1]);
        }
        else
        {
            m_reportAD.SetRowItemText(i, 0, g_csBlankValue);
            m_reportAD.SetRowItemText(i, 1, g_csBlankValue);

            m_data.SetAD1Channel(i, UINT_MAX);
            m_data.SetAD2Channel(i, UINT_MAX);
        }
    }

    if(pData->m_nFocusRow >= 0 && pData->m_nFocusRow < m_reportAD.GetRecordRowCount())
    {
        m_reportAD.SetFocusedSelectionRowIndex(pData->m_nFocusRow);
        m_reportAD.SetFocusedColIndex(pData->m_nFocusCol);
    }

    return true;
}


/**
 *      ����Ĭ����ʾͨ��
 */
void CSMVDualADDlg::_adjustChannel()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pPageDataExtra);

    // ��ȡ��ǰ�Ŀ��ƿ�����
    CCtrlBlkDataDualAD* pData = dynamic_cast<CCtrlBlkDataDualAD*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));

    // ��ȡ���ƿ�����ݣ���Ϊ�գ����½�һ������
    if(!pData)
    {
        pData = new CCtrlBlkDataDualAD;
        m_pPageDataExtra->Set(m_pCurrentCtrlBlk->nID, pData);

        pData = dynamic_cast<CCtrlBlkDataDualAD*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));
    }

    // Ѱ�Һ��ʵ�Ĭ����ʾͨ��
    _calcChannel(pData);

    CSmvRecvChannelInfo* pInfo1 = NULL;
    CSmvRecvChannelInfo* pInfo2 = NULL;
    CSmvRecvConfig* pCfg        = m_pCurrentCtrlBlk->pRecvConfig;

    // ����Ĭ��ͨ����ҳ��������
    for(int i = 0; i < DUALAD_COUNTS; i++)
    {
        pInfo1 = pCfg->GetOneChannelInfo(pData->m_arrDualAD[i][0]);
        pInfo2 = pCfg->GetOneChannelInfo(pData->m_arrDualAD[i][1]);

        if(pInfo1 && pInfo2)
        {
            m_reportAD.SetRowItemText(i, 0, pInfo1->strChannelDesc.c_str());
            m_reportAD.SetRowItemText(i, 1, pInfo2->strChannelDesc.c_str());
        }
        else
        {
            m_reportAD.SetRowItemText(i, 0, g_csBlankValue);
            m_reportAD.SetRowItemText(i, 1, g_csBlankValue);
        }
    }

    pData->m_nFocusRow = 0;
    pData->m_nFocusCol = 0;


    if(m_reportAD.GetRecordRowCount() > 0)
    {
        m_reportAD.SetFocusedSelectionRowIndex(pData->m_nFocusRow);
        m_reportAD.SetFocusedColIndex(pData->m_nFocusCol);
    }
}

/**
 *      Ѱ��Ĭ����ʾͨ��
 *
 *  \param[in]
 *      ���ƿ������ҳ��״̬���ݶ���
 *
 *  \note
 *      ���ڲ�����һ����Ϊһ��
 */
void CSMVDualADDlg::_calcChannel( CCtrlBlkDataDualAD* pData )
{
    ASSERT(pData);
    int nIndex                  = 0;
    CSmvRecvConfig* pCfg        = m_pCurrentCtrlBlk->pRecvConfig;
    size_t nCounts              = pCfg->GetChannelCount();
    CSmvRecvChannelInfo* pInfo1  = NULL;
    CSmvRecvChannelInfo* pInfo2  = NULL;

    // �򵥲���˫AD��
    for(size_t i = 0; i < nCounts && nIndex < DUALAD_COUNTS; i++)
    {
        pInfo1 = pCfg->GetOneChannelInfo(i);
        ASSERT(pInfo1);

        if(CHANNEL_TYPE_SMV_CURRENT == pInfo1->enChannelType || CHANNEL_TYPE_SMV_VOLTAGE == pInfo1->enChannelType)
        {
            pInfo2 = pCfg->GetOneChannelInfo(i + 1);

            // �ҵ�һ��
            if(pInfo2 && pInfo1->enChannelType == pInfo2->enChannelType)
            {
                pData->m_arrDualAD[nIndex][0] = pInfo1->nIndex;
                pData->m_arrDualAD[nIndex][1] = pInfo2->nIndex;

                // �����ݲ�����ͨ����Ϣ
                m_data.SetAD1Channel(nIndex, pInfo1->nIndex);
                m_data.SetAD2Channel(nIndex, pInfo2->nIndex);

                nIndex++;
                i++;
                continue;
            }
        }
    }

    // ����ʣ��������ͨ����δȷ�ϣ�����

    for(int i = nIndex; i < DUALAD_COUNTS; i++)
    {
        // �����ݲ�����ͨ����Ϣ
        m_data.SetAD1Channel(i, UINT_MAX);
        m_data.SetAD2Channel(i, UINT_MAX);
    }
}


/**
 *      ����ҳ��
 */
void CSMVDualADDlg::_SavePage()
{
    ASSERT(m_pCurrentCtrlBlk);
    ASSERT(m_pPageDataExtra);

    CCtrlBlkDataDualAD* pData = dynamic_cast<CCtrlBlkDataDualAD*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));

    // ���潹�������ֵ
    if(pData)
    {
        pData->m_nFocusRow = m_reportAD.GetFocusedRowIndex();
        pData->m_nFocusCol = m_reportAD.GetFocusedColIndex();
    }
}

/**
 *      ���½�������
 */
void CSMVDualADDlg::_update()
{
    ASSERT(m_pCurrentCtrlBlk);

    if(_checkChannel())
    {
        return;
    }

    // ��ȡ˫AD����
    if(!m_pDataLayer->GetDoubleAdData(&m_data))
    {
        for(int i = 0; i < DUALAD_COUNTS; i++)
        {
            m_reportAD.SetRowItemText(i, 2, g_csBlankValue);
            m_reportAD.SetRowItemText(i, 3, g_csBlankValue);
        }

        return;
    }

    float fVal1 = 0.0f;
    float fVal2 = 0.0f;
    CString csVal1, csVal2;

    for(int i = 0; i < DUALAD_COUNTS; i++)
    {
        if(m_data.GetADGroupResult(i, fVal1, fVal2))
        {
            csVal1.Format(L"%0.3f", fVal1);
            csVal2.Format(L"%0.3f", fVal2);

            m_reportAD.SetRowItemText(i, 2, csVal1);
            m_reportAD.SetRowItemText(i, 3, csVal2);
        }
        else
        {
            m_reportAD.SetRowItemText(i, 2, g_csBlankValue);
            m_reportAD.SetRowItemText(i, 3, g_csBlankValue);
        }
    }
}

void CSMVDualADDlg::OnTimer( UINT_PTR nIDEvent )
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    if(nIDEvent == m_nTimerID)
    {
        // ��ʱˢ�½���
        _update();
    }

    CSMVBaseDlg::OnTimer(nIDEvent);
}


void CSMVDualADDlg::OnItemSelectChange( NMHDR* pNotifyStruct, LRESULT * pResult )
{
    ASSERT(m_pPageDataExtra);

    ELNM_INPLACELIST_ARGS* pItemNotify = (ELNM_INPLACELIST_ARGS*)pNotifyStruct;
    ASSERT(pItemNotify);

    if(ELNM_ITEM_CONSTRAINTCHANGED == pItemNotify->hdr.code)
    {
        CCtrlBlkDataDualAD* pData = dynamic_cast<CCtrlBlkDataDualAD*>(m_pPageDataExtra->Get(m_pCurrentCtrlBlk->nID));
        ASSERT(pData);

        // ����AD1
        if(0 == pItemNotify->colIndex)
        {
            m_data.SetAD1Channel(pItemNotify->rowIndex, pItemNotify->pConstraint->GetConstraintData());
        }
        // ����AD2
        else if(1 == pItemNotify->colIndex)
        {
            m_data.SetAD2Channel(pItemNotify->rowIndex, pItemNotify->pConstraint->GetConstraintData());
        }
        else
        {
            *pResult = FALSE;
            return;
        }

        // ���ı���Ϣ������ƿ������ҳ��״̬������
        pData->m_arrDualAD[pItemNotify->rowIndex][pItemNotify->colIndex] = pItemNotify->pConstraint->GetConstraintData();

        *pResult = TRUE;
    }
}

/**
 *      ���ݲ˵�ID�л���ָ���Ŀ��ƿ�
 *
 *  \param[in]
 *      nCmdID          ���ƿ�ӳ�������˵�ID
 */
bool CSMVDualADDlg::_switchCtrlBlk( UINT nCmdID )
{
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

    // ����ԭ���ƿ��ͨ������
    CCtrlBlkDataDualAD* pData = dynamic_cast<CCtrlBlkDataDualAD*>(m_pPageDataExtra->Get(pPreCtrlBlk->nID));

    if(pData)
    {
        pData->m_nFocusRow = m_reportAD.GetFocusedRowIndex();
        pData->m_nFocusCol = m_reportAD.GetFocusedColIndex();
    }

    // �л����ƿ���������ñ�������ͨ��֮��ģ�
    _resetReport();

    return true;
}

/**
 *      ���ͨ���Ƿ����仯
 */
bool CSMVDualADDlg::_checkChannel()
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

            // ����ͨ��ѡ���������
            for(int i = 0; i < DUALAD_COUNTS; i++)
            {
                m_reportAD.SetRowItemText(i, 2, g_csBlankValue);
                m_reportAD.SetRowItemText(i, 3, g_csBlankValue);

                m_reportAD.GetRecordRow(i)->GetItem(0)->SetEditable(FALSE);
                m_reportAD.GetRecordRow(i)->GetItem(1)->SetEditable(FALSE);
            }

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

            // �ָ�ͨ��ѡ����
            for(int i = 0; i < DUALAD_COUNTS; i++)
            {
                m_reportAD.GetRecordRow(i)->GetItem(0)->SetEditable(TRUE);
                m_reportAD.GetRecordRow(i)->GetItem(1)->SetEditable(TRUE);
            }

            // �ָ���ͣ��ʼ�˵�����
            m_pNaviBar->GetNaviMenu()->SetMenuItemCaption(2, dsm_stringMgr::Get(DSM_STR_TIMER_STOP));
            m_pNaviBar->UpdateNaviMenu();
        }
    }

    return false;
}

void CSMVDualADDlg::OnInplaceButtonDown( NMHDR* , LRESULT *pResult )
{
    if(!m_bEnableTimer)
    {
        CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_TIMER_SW_CHAN_TIPS));
        *pResult = TRUE;
    }
}

void CSMVDualADDlg::_modifyMenu()
{

	m_menuMain.SetMenuItemCaption(0, dsm_stringMgr::Get(DSM_STR_SMV_DUALAD_CAPTION));
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