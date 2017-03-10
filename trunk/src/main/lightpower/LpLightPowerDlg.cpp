/// @file
///
/// @brief
///     �⹦�� ʵ���ļ�
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    lqx  2013.7.18
///
/// �޸���ʷ��
///

#include "stdafx.h"
#include "LpLightPowerDlg.h"
#include "src/service/datalayer/OpticalPower/OpticalPowerManager.h"

#if !SU_FEI
#include "src/main/mgr/statusmgr.h"
#include "src/main/dlg/dsmfilesvgsedlg.h"
#include "src/main/dlg/PcapBrowserDlg.h"
#include "src/utils/utility/dsm_utility.h"
#include "src/main/resourcebmp.h"
#endif

#define DSM_LP_INDEX_PORT                     0
#define DSM_LP_INDEX_WAVE_LENGTH              1
#define DSM_LP_INDEX_REAL_VALUE               2
#define DSM_LP_INDEX_MIN_VALUE                3
#define DSM_LP_INDEX_MAX_VALUE                4

#if SU_FEI
#define DSM_LP_PORT_WIDTH_PORT                80
#define DSM_LP_PORT_WIDTH_WAVE_LENGTH         100
#define DSM_LP_PORT_WIDTH_REAL_VALUE          210
#define DSM_LP_PORT_WIDTH_MIN_VALUE           105
#define DSM_LP_PORT_WIDTH_MAX_VALUE           105

#else

#define DSM_LP_PORT_WIDTH_PORT                75
#define DSM_LP_PORT_WIDTH_WAVE_LENGTH         90
#define DSM_LP_PORT_WIDTH_REAL_VALUE          184
#define DSM_LP_PORT_WIDTH_MIN_VALUE           90
#define DSM_LP_PORT_WIDTH_MAX_VALUE           90

#define ID_ASSISTED_TESTING_RET				1120		///< ����welcome�����ID��
#endif


#define DBM_INFINITY 1000000

class CLPNetPortRecord: public CELRecord
{
public:
    CLPNetPortRecord()
    {
        AddItem(new CELRecordItemText(_T("")));
        AddItem(new CELRecordItemText(_T("")));
        AddItem(new CELRecordItemText(_T("")));
        AddItem(new CELRecordItemText(_T("")));
        AddItem(new CELRecordItemText(_T("")));
    }
    ~CLPNetPortRecord()
    {
    };
};


// CLpLightPowerDlg �Ի���

IMPLEMENT_DYNAMIC(CLpLightPowerDlg, CBaseDlg)

CLpLightPowerDlg::CLpLightPowerDlg(CWnd* pParent /*=NULL*/)
	: CBaseDlg(CLpLightPowerDlg::IDD, DSM_STR_LP_LIGHT_POWER_CAPTION, pParent)
    , m_pOptPowMgr(NULL)
{
    m_nMenuID = IDR_MENU_LP_LIGHT_POWER;
}

CLpLightPowerDlg::~CLpLightPowerDlg()
{
    if (m_pOptPowMgr)
    {
        m_pOptPowMgr->StopGatherOpticalPower();
    }
}

void CLpLightPowerDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDlg::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLpLightPowerDlg, CBaseDlg)
    ON_WM_TIMER()
	ON_COMMAND(ID_MENU_LP_LIGHT_POWER_F1, &CLpLightPowerDlg::OnMenuF1)
	ON_COMMAND(ID_MENU_LP_LIGHT_POWER_F2, &CLpLightPowerDlg::OnMenuF2)
	ON_COMMAND(ID_MENU_LP_LIGHT_POWER_F3, &CLpLightPowerDlg::OnMenuF3)
END_MESSAGE_MAP()


// CLpLightPowerDlg ��Ϣ�������

BOOL CLpLightPowerDlg::OnInitDialog()
{
    CBaseDlg::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��

    m_pOptPowMgr = COpticalPowerManager::GetInstance();
    if (m_pOptPowMgr == NULL)
    {
        return FALSE;
    }

    m_pOptPowMgr->StartGatherOpticalPower();
    SetTimer(1, 400, NULL);


    _createPortReport();

    return TRUE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}

void CLpLightPowerDlg::_createPortReport()
{
    CRect rcReport;
    GetClientRect(rcReport);

    rcReport.DeflateRect(DSM_MARGIN_IN_RECT);
    int nRows = (rcReport.Height() - ELT_T1_HDRHEIGHT) / ELT_T1_ROWHEIGHT;
    rcReport.bottom = rcReport.top + ELT_T1_HDRHEIGHT + nRows * ELT_T1_ROWHEIGHT;

    m_ctlPortReport.Create(DSM_WINSTYLE_REPORT_DEFAULT, 
        rcReport, this, IDC_LP_NET_PORT_RET);

    // ��������
    m_ctlPortReport.SetFont(dsm_fontmgr::GetInstance()->GetFont(ELT_T1_TEXTLFHEIGHT));

    m_ctlPortReport.PostInitTemplateStyle();

    // ������Ԫ��

    m_ctlPortReport.InsertColumn(DSM_LP_INDEX_PORT, dsm_stringMgr::Get(DSM_STR_LP_PORT), EL_LEFT, DSM_LP_PORT_WIDTH_PORT);
    m_ctlPortReport.InsertColumn(DSM_LP_INDEX_WAVE_LENGTH, dsm_stringMgr::Get(DSM_STR_LP_WAVE_LENGTH), EL_LEFT, DSM_LP_PORT_WIDTH_WAVE_LENGTH);
    m_ctlPortReport.InsertColumn(DSM_LP_INDEX_REAL_VALUE, dsm_stringMgr::Get(DSM_STR_LP_REAL_VALUE), EL_LEFT, DSM_LP_PORT_WIDTH_REAL_VALUE);
    m_ctlPortReport.InsertColumn(DSM_LP_INDEX_MIN_VALUE, dsm_stringMgr::Get(DSM_STR_LP_MIN_VALUE), EL_LEFT, DSM_LP_PORT_WIDTH_MIN_VALUE);
    m_ctlPortReport.InsertColumn(DSM_LP_INDEX_MAX_VALUE, dsm_stringMgr::Get(DSM_STR_LP_MAX_VALUE), EL_LEFT, DSM_LP_PORT_WIDTH_MAX_VALUE);

    // ����ѡ�񽹵���ʽ
    m_ctlPortReport.SetRowFocusEnable(FALSE);
    m_ctlPortReport.SetRowSelectionVisible(FALSE);
    m_ctlPortReport.SetFocus();


	m_menuMain.HighlightMenuItem(0, TRUE);

    _updatePortReport();
}

void CLpLightPowerDlg::_updatePortReport()
{

    if (m_pOptPowMgr == NULL || !m_ctlPortReport.GetSafeHwnd())
    {
        return;
    }
    
    CLightPort* pLightPort = NULL;

    int nIndex = 0;
    //
    //������1
    //
    pLightPort = m_pOptPowMgr->GetLightPort(NET_PORT_LIGHT_B);
    if (pLightPort)
    {            
        _insertPortToReport(nIndex, pLightPort);
        ++nIndex;
    }
    //
    //������2
    //
    pLightPort = m_pOptPowMgr->GetLightPort(NET_PORT_LIGHT_A);
    if (pLightPort)
    {
        _insertPortToReport(nIndex, pLightPort);
        ++nIndex;
    }
    ////
    ////�⴮��1
    ////
    //pLightPort = m_pOptPowMgr->GetLightPort(FT3_PORT_LIGHT_A);
    //if (pLightPort)
    //{
    //    _insertPortToReport(nIndex, pLightPort);
    //    ++nIndex;
    //}
    ////
    ////�⴮��2
    ////
    //pLightPort = m_pOptPowMgr->GetLightPort(FT3_PORT_LIGHT_B);
    //if (pLightPort)
    //{
    //    _insertPortToReport(nIndex, pLightPort);
    //    ++nIndex;
    //}
    ////
    ////�⴮��
    ////
    //pLightPort = m_pOptPowMgr->GetLightPort(FT3_PORT_LIGHT_C);
    //if (pLightPort)
    //{
    //    _insertPortToReport(nIndex, pLightPort);
    //    ++nIndex;
    //}

    while(nIndex < m_ctlPortReport.GetRecordRowCount())
    {
        //ɾ���������
        m_ctlPortReport.DeleteRecordRow(nIndex);
    }
    
    if (m_ctlPortReport.GetRecordRowCount() > 0)
    {
        m_ctlPortReport.RedrawRowItems(0, m_ctlPortReport.GetRecordRowCount() - 1);
    }
}

void CLpLightPowerDlg::_insertPortToReport(int nIndex, CLightPort* pLightPort)
{
    if (pLightPort == NULL)
    {
        return;
    }

    if (nIndex >= m_ctlPortReport.GetRecordRowCount())
    {
        //�������û��nIndex�У�������һ��
        m_ctlPortReport.InsertRecordRow(nIndex, new CLPNetPortRecord());
    }


    CString strTemp = _T("");
    CELRecordItemText* pItem = NULL;
    pItem = dynamic_cast<CELRecordItemText*>(m_ctlPortReport.GetRecordRow(nIndex)->GetItem(DSM_LP_INDEX_PORT));
    if (pItem)
    {
        //
        //�˿�
        //
        DsmPortName ePortName = pLightPort->GetPortName();
        switch(ePortName)
        {
        case NET_PORT_LIGHT_A:
            {
                strTemp = dsm_stringMgr::Get(DSM_STR_LP_NET_PORT_LIGHT_A);
            }
            break;
        case NET_PORT_LIGHT_B:
            {
                strTemp = dsm_stringMgr::Get(DSM_STR_LP_NET_PORT_LIGHT_B);
            }
            break;
        //case FT3_PORT_LIGHT_A:
        //    {
        //        strTemp = dsm_stringMgr::Get(DSM_STR_LP_FT3_PORT_LIGHT_A);
        //    }
        //    break;
        //case FT3_PORT_LIGHT_B:
        //    {
        //        strTemp = dsm_stringMgr::Get(DSM_STR_LP_FT3_PORT_LIGHT_B);
        //    }
        //    break;
        //case FT3_PORT_LIGHT_C:
        //    {
        //        strTemp = dsm_stringMgr::Get(DSM_STR_LP_FT3_PORT_LIGHT_C);
        //    }
        //    break;
        default:
            break;
        }
        pItem->SetValue(strTemp);
    }
    
    //����
    pItem = dynamic_cast<CELRecordItemText*>(m_ctlPortReport.GetRecordRow(nIndex)->GetItem(DSM_LP_INDEX_WAVE_LENGTH));
    if (pItem)
    {
        strTemp.Format(_T("%d nm"), pLightPort->GetWaveLength());
        pItem->SetValue(strTemp);
    }

    //ʵ��ֵ
    pItem = dynamic_cast<CELRecordItemText*>(m_ctlPortReport.GetRecordRow(nIndex)->GetItem(DSM_LP_INDEX_REAL_VALUE));
    if (pItem)
    {
        float fBDMValue = 0.0f;
        float fUWValue = 0.0f;
        CString strBDMValue = _T("");
        CString strUWValue = _T("");

        pLightPort->GetCurOpticalPower(fBDMValue, fUWValue);

        if (fBDMValue < -DBM_INFINITY)
        {
            strBDMValue = _T("-�� dBm ");
        }
        else
        {
            strBDMValue.Format(_T("%6.02f dBm "), fBDMValue);
        }

        if (fUWValue < -DBM_INFINITY)
        {
            strUWValue = _T("-�� ��W");
        }
        else
        {
            strUWValue.Format(_T("%6.03f ��W"), fUWValue);
        }
        
        strTemp = strBDMValue + strUWValue;

        pItem->SetValue(strTemp);
    }

    //��Сֵ
    pItem = dynamic_cast<CELRecordItemText*>(m_ctlPortReport.GetRecordRow(nIndex)->GetItem(DSM_LP_INDEX_MIN_VALUE));
    if (pItem)
    {
        float   fUWValue = pLightPort->GetMinOpticalPower();
        if (fUWValue < -DBM_INFINITY)
        {
            strTemp = _T("-�� dBm");
        }
        else
        {
            strTemp.Format(_T("%.02f dBm"), fUWValue);
        }
        pItem->SetValue(strTemp);
    }

    //���ֵ
    pItem = dynamic_cast<CELRecordItemText*>(m_ctlPortReport.GetRecordRow(nIndex)->GetItem(DSM_LP_INDEX_MAX_VALUE));
    if (pItem)
    {

        float   fUWValue = pLightPort->GetMaxOpticalPower();
        if (fUWValue < -DBM_INFINITY)
        {
            strTemp = _T("-�� dBm");
        }
        else
        {
            strTemp.Format(_T("%.02f dBm"), fUWValue);
        }
        pItem->SetValue(strTemp);
    }

}

#if SU_FEI
void CLpLightPowerDlg::OnMenuF6()
{
    if (m_pOptPowMgr == NULL || !m_ctlPortReport.GetSafeHwnd())
    {
        return;
    }
    
    m_pOptPowMgr->Reset();
}
#else
void CLpLightPowerDlg::OnMenuF1()
{
	_showLightPower();
	
}
void CLpLightPowerDlg::OnMenuF2()
{
	CEventProc::GetInstance()->ShowPage(IDD_TIME_B_DLG);
}
void CLpLightPowerDlg::OnMenuF3()
{
	if(CStatusMgr::GetSD() == DSM_STATU_DISCONNECT_SD)
	{
		// δ����SD��
		CStatusMgr::SetTips(dsm_stringMgr::Get(DSM_STR_INFO_SD_DISCONNECT));
		return;
	}

	// ����pcap�ļ�
	CString csExt;
	CString csSelectPcapFile = L"";
	csExt.Format(L"*.%s|", DSM_PCAP_FILE_EXTENSION);

	CDsmFileSvGseDlg dlgFile(0, L"\\Storage Card\\Record\\SV", L"", csExt, 0, this, dsm_stringMgr::Get(DSM_STR_FILEBROWSER_TITLE));

	if(!csSelectPcapFile.IsEmpty())
	{
		dlgFile._SetSelectFile(csSelectPcapFile);
	}

	CString csTitle(CStatusMgr::GetTitle());

	HWND hWnd       = ::GetFocus();
	INT_PTR nRet    = dlgFile.DoModal();
	if(nRet == IDOK)
	{
		CString csTitle = CStatusMgr::GetTitle();

		csSelectPcapFile = dlgFile._GetFilePath();

		// ����pcap��
		CPcapBrowserDlg dlg(dlgFile._GetFilePath());
		nRet = dlg.DoModal();
	}

	if(IsWindow(hWnd))
	{
		CStatusMgr::SetTitle(csTitle);
		::SetFocus(hWnd);
	}
	else
	{
		DSM_SetZFocus(CEventProc::GetInstance()->GetCurrentPageHwnd());
	}
}



///< ��ʾ���ʱ��
void CLpLightPowerDlg::_showLightPower()
{	
	if (m_ctlPortReport.GetSafeHwnd())
	{
		m_ctlPortReport.ShowWindow(SW_SHOW);
	}
	else
	{
		_createPortReport();
	}
}
#endif

void CLpLightPowerDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    if (nIDEvent == 1)
    {
        _updatePortReport();
    }
    CBaseDlg::OnTimer(nIDEvent);
}


BOOL CLpLightPowerDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: �ڴ����ר�ô����/����û���
    
    //if (pMsg->message == WM_KEYDOWN)
    //{
    //    switch(pMsg->wParam)
    //    {
    //    case VK_F7:
    //        {
    //            if (m_pOptPowMgr)
    //            {
    //                m_pOptPowMgr->StopGatherOpticalPower();
    //            }
    //        }
    //        break;
    //    case VK_ESCAPE:
    //        {
    //            if (m_pOptPowMgr)
    //            {
    //                m_pOptPowMgr->StopGatherOpticalPower();
    //            }
    //        }
    //        break;
    //    }
    //}
    return CBaseDlg::PreTranslateMessage(pMsg);
}




void CLpLightPowerDlg::_modifyMenu()
{

	m_menuMain.SetMenuItemBitmaps(0, dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_LIGHT_POWER_UNSEL), dsm_bmpmgr::GetCB(DSM_BMP_M_SMV_SCAN_LIGHT_POWER_SEL), NULL);
}