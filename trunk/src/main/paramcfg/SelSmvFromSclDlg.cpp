/// @file
///
/// @brief
///     ��ȫվ���������smv���ͶԻ��� ʵ���ļ�
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    lqx  2013.5.28
///
/// �޸���ʷ��
///


#include "stdafx.h"

#include "SelSmvFromSclDlg.h"
#include "src/main/mgr/StatusMgr.h"
#include "src/main/paramcfg/common_cfg.h"
#include "src/service/datalayer/scl/SCLManager.h"
#include "src/service/datalayer/config/ParamConfigSMVSend.h"

// CAddSmvFromSclDlg �Ի���



IMPLEMENT_DYNAMIC(CSelSmvFromSclDlg, CBaseDlg)

CSelSmvFromSclDlg::CSelSmvFromSclDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CSelSmvFromSclDlg::IDD, DSM_STR_SEL_SMV_FROM_SCL_CAPTION, pParent)
{
    m_nMenuID = IDR_MENU_EMPTY;
}

CSelSmvFromSclDlg::~CSelSmvFromSclDlg()
{
}

void CSelSmvFromSclDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSelSmvFromSclDlg, CBaseDlg)
    ON_NOTIFY(ELNM_ITEM_CHECKED, IDC_SEL_SMV_FROM_SCL_RET, &CSelSmvFromSclDlg::OnSmvSelItemValueChanged)
END_MESSAGE_MAP()


// CAddSmvFromSclDlg ��Ϣ�������

BOOL CSelSmvFromSclDlg::OnInitDialog()
{
    CBaseDlg::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��

    _createReport();

    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        return FALSE;
    }

    CString strSmvType = _T("");
    enumFrameType smvType = pParamConfigSMVSend->GetSMVType();
    if (smvType == FRAME_TYPE_NET_9_1 || smvType == FRAME_TYPE_NET_9_2)
    {
        strSmvType = _T("9-1/2");
    }
    else
    {
        strSmvType = _T("FT3");
    }

    CString csCaption;
    csCaption.Format(_T("%s-%s"), 
        dsm_stringMgr::Get(DSM_STR_SEL_SMV_FROM_SCL_CAPTION),
        strSmvType);
    CStatusMgr::SetTitle(csCaption);

    return TRUE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}



void CSelSmvFromSclDlg::_createReport()
{
    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

    m_ctlSmvReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_SEL_SMV_FROM_SCL_RET);

    // ��������
    m_ctlSmvReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    m_ctlSmvReport.SetHeaderFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));
    
    m_ctlSmvReport.PostInitTemplateStyle();
    // ������Ԫ��

   
    m_ctlSmvReport.InsertColumn(0, L"   " + dsm_stringMgr::Get(DSM_STR_SEL_SMV_FROM_SCL_APPID),      EL_CENTER, DSM_SEL_SMV_FROM_SCL_WIDTH_APPID);
    m_ctlSmvReport.InsertColumn(1, L"  " + dsm_stringMgr::Get(DSM_STR_SEL_SMV_FROM_SCL_CHAN_COUNT), EL_CENTER, DSM_SEL_SMV_FROM_SCL_WIDTH_CHAN_COUNT);
    m_ctlSmvReport.InsertColumn(2, dsm_stringMgr::Get(DSM_STR_SEL_SMV_FROM_SCL_DESC),       EL_CENTER, DSM_SEL_SMV_FROM_SCL_WIDTH_CTRL_DESC);
    m_ctlSmvReport.InsertColumn(3, L"  " + dsm_stringMgr::Get(DSM_STR_SEL_SMV_FROM_SCL_PORT),       EL_CENTER, DSM_SEL_SMV_FROM_SCL_WIDTH_PORT);
	m_ctlSmvReport.InsertColumn(4, _T(""),                                                  EL_CENTER, DSM_SEL_SMV_FROM_SCL_WIDTH_CHECKBOX);

    // ����ѡ�񽹵���ʽ
    m_ctlSmvReport.SetRowSelectionVisible(TRUE);
    m_ctlSmvReport.SetRowFocusEnable(TRUE);
    m_ctlSmvReport.SetSubItemFocusEnable(TRUE);
    m_ctlSmvReport.SetFreezeSelectionVisible(TRUE);
    m_ctlSmvReport.SetFocusedColIndex(4);

    m_ctlSmvReport.SetFocus();
    m_ctlSmvReport.ShowWindow(SW_SHOW);

#if !SU_FEI
	m_ctlSmvReport.SetCheckImage(dsm_bmpmgr::GetCB(DSM_BMP_CHECK_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_UNSEL),
		dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_SEL), dsm_bmpmgr::GetCB(DSM_BMP_CHECK_FOCUS_UNSEL));
#endif

    
    CSCLManager* pSCLManager = CSCLManager::getInstance();
    if (NULL == pSCLManager)
    {
        return ;
    }

    //
    //��ʼ��ȫվ�����ļ��е�smv���ƿ�
    //
    int i = 0;
    

    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        return ;
    }
    //ȡ��ԭ��ѡ�еĿ��ƿ�
    pSCLManager->UnSelectAllIEC9_2Cb();
    enumFrameType smvType = pParamConfigSMVSend->GetSMVType();
    switch (smvType)
    {
    case FRAME_TYPE_NET_9_1:
        {
            CIEC9_1Cb* pIEC91Cb = pParamConfigSMVSend->FirstIEC9_1Cb();
            while(pIEC91Cb)
            {
                pSCLManager->SelectIEC9_2Cb(pIEC91Cb->GetAPPID());
                pIEC91Cb = pParamConfigSMVSend->NextIEC9_1Cb(pIEC91Cb);
            }
        }
        break;
    case FRAME_TYPE_NET_9_2:
        {
            CIEC9_2Cb* pIEC92Cb = pParamConfigSMVSend->FirstIEC9_2Cb();
            while(pIEC92Cb)
            {
                pSCLManager->SelectIEC9_2Cb(pIEC92Cb->GetAPPID());
                pIEC92Cb = pParamConfigSMVSend->NextIEC9_2Cb(pIEC92Cb);
            }
        }
        break;
    case FRAME_TYPE_FT3_NANRUI:
        {
            CFT3Cb* pFt3Cb = pParamConfigSMVSend->FirstFt3Cb();
            while(pFt3Cb)
            {
                pSCLManager->SelectIEC9_2Cb(pFt3Cb->GetAPPID());
                pFt3Cb = pParamConfigSMVSend->NextFt3Cb(pFt3Cb);
            }
        }
        break;
    case FRAME_TYPE_FT3_GUOWANG:
        {
            CFT3ExpandCb* pFt3ExtCb = pParamConfigSMVSend->FirstFt3ExpandCb();
            while(pFt3ExtCb)
            {
                pSCLManager->SelectIEC9_2Cb(pFt3ExtCb->GetAPPID());
                pFt3ExtCb = pParamConfigSMVSend->NextFt3ExpandCb(pFt3ExtCb);
            }
        }
        break;
    }
    
    //ѡ��֮ǰ���еĿ��ƿ�
    CIEC9_2Cb* pIEC92Cb = pSCLManager->FirstIEC9_2Cb();
    while(pIEC92Cb)
    {
        m_ctlSmvReport.InsertRecordRow(i, new CDsmSelSmvCtrlFromSclRecord(pIEC92Cb));
        m_ctlSmvReport.SetRecordRowData(i, (DWORD_PTR)pIEC92Cb);

        ++i;
        pIEC92Cb = pSCLManager->NextIEC9_2Cb(pIEC92Cb);
    }
    
    m_ctlSmvReport.GetColumnExAttr(4)->GetEditOptions()->SetAllowEdit(TRUE);

    if (m_ctlSmvReport.GetRecordRowCount() > 0)
    {
        m_ctlSmvReport.SetFocusedSelectionRowIndex(0);
    }
}

void CSelSmvFromSclDlg::OnSmvSelItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult)
{
    UNREFERENCED_PARAMETER(pResult);
    *pResult = FALSE;
    ELNM_RECORDITEM_ARGS* pItemNotify = (ELNM_RECORDITEM_ARGS*)pNotifyStruct;
    if (pItemNotify == NULL)
    {
        return ;
    }
    
    CIEC9_2Cb* pIEC92Cb = (CIEC9_2Cb*) m_ctlSmvReport.GetRecordRowData(pItemNotify->rowIndex);
    
    ASSERT(pIEC92Cb);
    if (pIEC92Cb == NULL)
    {
        return;
    }
    
    if(pItemNotify->pItem->GetHasCheckbox())
    {
        //
        //�����Ƿ�ѡ�п��ƿ�
        //
        pIEC92Cb->Selected(pItemNotify->pItem->IsChecked() == TRUE ? true : false);
    }
    *pResult = TRUE;
}

void CSelSmvFromSclDlg::_SavePage()
{

    CParamConfigSMVSend* pParamConfigSMVSend = CParamConfigSMVSend::getInstance();
    if (NULL == pParamConfigSMVSend)
    {
        return ;
    }
    CIEC9_2Cb* pIEC92Cb = NULL;
    enumFrameType smvType = pParamConfigSMVSend->GetSMVType();
    switch (smvType)
    {
    case FRAME_TYPE_NET_9_1:
        {
            for (int i = 0; i < m_ctlSmvReport.GetRecordRowCount(); ++i)
            {
                pIEC92Cb = (CIEC9_2Cb*) m_ctlSmvReport.GetRecordRowData(i);
                ASSERT(pIEC92Cb);
                if (pIEC92Cb != NULL)
                {
                    if (pIEC92Cb->IsSelected())
                    {
                        //����ѡ�е���Ϣ������
                        pIEC92Cb->Selected(false);
                        pParamConfigSMVSend->AddIEC9_1Cb(pIEC92Cb);
                    }
                    else
                    {
                        pParamConfigSMVSend->DeleteIEC9_1Cb(pIEC92Cb->GetAPPID());
                    }
                }
            }
        }
        break;
    case FRAME_TYPE_NET_9_2:
        {
            for (int i = 0; i < m_ctlSmvReport.GetRecordRowCount(); ++i)
            {
                pIEC92Cb = (CIEC9_2Cb*) m_ctlSmvReport.GetRecordRowData(i);
                if (pIEC92Cb->IsSelected())
                {
                    //����ѡ�е���Ϣ������
                    pIEC92Cb->Selected(false);
                    pParamConfigSMVSend->AddIEC9_2Cb(pIEC92Cb);
                }
                else
                {
                    pParamConfigSMVSend->DeleteIEC9_2Cb(pIEC92Cb);
                }
            }
        }
        break;
    case FRAME_TYPE_FT3_NANRUI:
        {
            for (int i = 0; i < m_ctlSmvReport.GetRecordRowCount(); ++i)
            {
                pIEC92Cb = (CIEC9_2Cb*) m_ctlSmvReport.GetRecordRowData(i);
                if (pIEC92Cb->IsSelected())
                {
                    //����ѡ�е���Ϣ������
                    pIEC92Cb->Selected(false);
                    pParamConfigSMVSend->AddFt3Cb(pIEC92Cb);
                }
                else
                {
                    pParamConfigSMVSend->DeleteFt3Cb(pIEC92Cb->GetAPPID());
                }
            }
        }
        break;
    case FRAME_TYPE_FT3_GUOWANG:
        {
            for (int i = 0; i < m_ctlSmvReport.GetRecordRowCount(); ++i)
            {
                pIEC92Cb = (CIEC9_2Cb*) m_ctlSmvReport.GetRecordRowData(i);
                if (pIEC92Cb->IsSelected())
                {
                    //����ѡ�е���Ϣ������
                    pIEC92Cb->Selected(false);
                    pParamConfigSMVSend->AddFt3ExpandCb(pIEC92Cb);
                }
                else
                {
                    pParamConfigSMVSend->DeleteFt3ExpandCb(pIEC92Cb->GetAPPID());
                }
            }
        }
        break;
    }


    //
    //��������
    //
    pParamConfigSMVSend->SaveConfig();


    //
    //���ԭ��ѡ�еĿ��ƿ���Ϣ
    //
    CSCLManager* pSCLManager = CSCLManager::getInstance();
    if (NULL == pSCLManager)
    {
        return ;
    }
    //ȡ��ԭ��ѡ�еĿ��ƿ�
    pSCLManager->UnSelectAllIEC9_2Cb();
}
