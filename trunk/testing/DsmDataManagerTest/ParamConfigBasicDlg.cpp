// ParamConfigBasic.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DsmDataManagerTest.h"
#include "ParamConfigBasicDlg.h"
#include "src\service\datalayer\DsmDataLayer.h"



// CParamConfigBasicDlg �Ի���

IMPLEMENT_DYNAMIC(CParamConfigBasicDlg, CDialog)

CParamConfigBasicDlg::CParamConfigBasicDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParamConfigBasicDlg::IDD, pParent)
{

}

CParamConfigBasicDlg::~CParamConfigBasicDlg()
{
}

void CParamConfigBasicDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CParamConfigBasicDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_READ, &CParamConfigBasicDlg::OnBnClickedButtonRead)
    ON_BN_CLICKED(IDC_BUTTON_WRITE, &CParamConfigBasicDlg::OnBnClickedButtonWrite)
END_MESSAGE_MAP()


// CParamConfigBasicDlg ��Ϣ�������

void CParamConfigBasicDlg::OnBnClickedButtonRead()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    CParamConfigBasic* pParamConfigBasic = CDsmDataLayer::getInstance()->GetParamConfigBasic();
    if (pParamConfigBasic)
    {
        CString csTmp;
//         pParamConfigBasic->LoadConfig(L"paramconfig_basic.xml");
//        ((CEdit*)GetDlgItem(IDC_EDIT_FREQ))->SetWindowText(pParamConfigBasic->GetRatedFreq().c_str());
//        ((CComboBox*)GetDlgItem(IDC_COMBO_VALUE_TYPE))->SetCurSel((int)pParamConfigBasic->GetFrameValueType() - 1);
//        ((CEdit*)GetDlgItem(IDC_EDIT_VOL1))->SetWindowText(pParamConfigBasic->GetRatedPrimaryVoltage().c_str());
//        ((CEdit*)GetDlgItem(IDC_EDIT_VOL2))->SetWindowText(pParamConfigBasic->GetRatedSecondaryVoltage().c_str());
//        ((CEdit*)GetDlgItem(IDC_EDIT_CUR1))->SetWindowText(pParamConfigBasic->GetRatedPrimaryCurrent().c_str());
//        ((CEdit*)GetDlgItem(IDC_EDIT_CUR2))->SetWindowText(pParamConfigBasic->GetRatedSecondaryCurrent().c_str());
//        ((CEdit*)GetDlgItem(IDC_EDIT_MU_DELAY))->SetWindowText(pParamConfigBasic->GetMuRatedDelay().c_str());
//        ((CComboBox*)GetDlgItem(IDC_COMBO_TIME_SYNC_MODE))->SetCurSel(((int)pParamConfigBasic->GetTimeSyncMode() - 1));
//        ((CEdit*)GetDlgItem(IDC_EDIT_SCL_FILE))->SetWindowText(pParamConfigBasic->GetSclFile().c_str());
    }
}

void CParamConfigBasicDlg::OnBnClickedButtonWrite()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    CParamConfigBasic* pParamConfigBasic = CDsmDataLayer::getInstance()->GetParamConfigBasic();
    if (pParamConfigBasic)
    {
        CString csTemp;
        int nSel = 0;
       (((CEdit*)GetDlgItem(IDC_EDIT_FREQ))->GetWindowText(csTemp));
        pParamConfigBasic->SetRatedFreq(csTemp.GetString());

        nSel = (int)((CComboBox*)GetDlgItem(IDC_COMBO_VALUE_TYPE))->GetCurSel();
        pParamConfigBasic->SetFrameValueType((enumSmvValueType)(nSel+1));

//         ((CEdit*)GetDlgItem(IDC_EDIT_VOL1))->GetWindowText(csTemp);
//         pParamConfigBasic->SetRatedPrimaryVoltage(csTemp.GetString());
//         ((CEdit*)GetDlgItem(IDC_EDIT_VOL2))->GetWindowText(csTemp);
//         pParamConfigBasic->SetRatedSecondaryVoltage(csTemp.GetString());
// 
//         ((CEdit*)GetDlgItem(IDC_EDIT_CUR1))->GetWindowText(csTemp);
//         pParamConfigBasic->SetRatedPrimaryCurrent(csTemp.GetString());
//         ((CEdit*)GetDlgItem(IDC_EDIT_CUR2))->GetWindowText(csTemp);
//         pParamConfigBasic->SetRatedSecondaryCurrent(csTemp.GetString());
// 
//         ((CEdit*)GetDlgItem(IDC_EDIT_MU_DELAY))->GetWindowText(csTemp);
//         pParamConfigBasic->SetMuRatedDelay(csTemp.GetString());
// 
//         ((CEdit*)GetDlgItem(IDC_EDIT_SCL_FILE))->GetWindowText(csTemp);
//         pParamConfigBasic->SetSclFile(csTemp.GetString());

        nSel = (int)((CComboBox*)GetDlgItem(IDC_COMBO_TIME_SYNC_MODE))->GetCurSel();
        pParamConfigBasic->SetTimeSyncMode((FpgaTimeSyncMode)(nSel+1));
        pParamConfigBasic->SaveConfig();

    }
}
