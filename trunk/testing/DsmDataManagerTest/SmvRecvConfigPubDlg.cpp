// SmvRecvConfigPubDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "DsmDataManagerTest.h"
#include "SmvRecvConfigPubDlg.h"
#include "src\service\datalayer\DsmDataLayer.h"


// CSmvRecvConfigPubDlg 对话框

IMPLEMENT_DYNAMIC(CSmvRecvConfigPubDlg, CDialog)

CSmvRecvConfigPubDlg::CSmvRecvConfigPubDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSmvRecvConfigPubDlg::IDD, pParent)
{

}

CSmvRecvConfigPubDlg::~CSmvRecvConfigPubDlg()
{
}

void CSmvRecvConfigPubDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSmvRecvConfigPubDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_READ, &CSmvRecvConfigPubDlg::OnBnClickedButtonRead)
    ON_BN_CLICKED(IDC_BUTTON_WRITE_SMV_CONFIG, &CSmvRecvConfigPubDlg::OnBnClickedButtonWriteSmvConfig)
END_MESSAGE_MAP()


// CSmvRecvConfigPubDlg 消息处理程序

void CSmvRecvConfigPubDlg::OnBnClickedButtonRead()
{
    // TODO: 在此添加控件通知处理程序代码
    CSmvRecvConfigPublic* pSmvRecvConfigPub = CDsmDataLayer::getInstance()->GetSmvConfigPublic();
    if (pSmvRecvConfigPub)
    {
        pSmvRecvConfigPub->LoadConfigFile(L"smvrecvconfig.xml");
        ((CComboBox*)GetDlgItem(IDC_COMBO_DISP_TYPE))->SetCurSel((int)pSmvRecvConfigPub->GetSmvDispType()-1);
        ((CComboBox*)GetDlgItem(IDC_COMBO_SAMPLE_RATE))->SetCurSel((int)pSmvRecvConfigPub->GetSmv92SampleRate()-1);
        ((CComboBox*)GetDlgItem(IDC_COMBO_PHASE_CHECK_ALIGN))->SetCurSel((int)pSmvRecvConfigPub->GetPhaseCheckAlign()-1);
        ((CButton*)GetDlgItem(IDC_CHECK1))->SetCheck(pSmvRecvConfigPub->GetHarmonicTypeEnabled(HARMONIC_TYPE_THD));
        ((CButton*)GetDlgItem(IDC_CHECK2))->SetCheck(pSmvRecvConfigPub->GetHarmonicTypeEnabled(HARMONIC_TYPE_DC));
        for (int i = 1; i < 22; i++)
        {
            ((CButton*)GetDlgItem(IDC_CHECK2 + i))->SetCheck(pSmvRecvConfigPub->GetHarmonicTypeEnabled((HARMONIC_TYPE)i));
        }
    }
}

void CSmvRecvConfigPubDlg::OnBnClickedButtonWriteSmvConfig()
{
    // TODO: 在此添加控件通知处理程序代码
    CSmvRecvConfigPublic* pSmvRecvConfigPub = CDsmDataLayer::getInstance()->GetSmvConfigPublic();
    if (pSmvRecvConfigPub)
    {
        pSmvRecvConfigPub->SetSmvDispType( (enumSmvValueType)(((CComboBox*)GetDlgItem(IDC_COMBO_DISP_TYPE))->GetCurSel() + 1) );
        pSmvRecvConfigPub->SetSmv92SampleRate( (SMV92_SAMPLE_RATE )( ((CComboBox*)GetDlgItem(IDC_COMBO_SAMPLE_RATE))->GetCurSel() + 1 ) );
        pSmvRecvConfigPub->SetPhaseCheckAlign( (SMV_PHASE_CHECK_ALIGNMENT )( ((CComboBox*)GetDlgItem(IDC_COMBO_PHASE_CHECK_ALIGN))->GetCurSel() + 1 ) );

        pSmvRecvConfigPub->SetHarmonicTypeEnable(HARMONIC_TYPE_THD, ((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck());
        pSmvRecvConfigPub->SetHarmonicTypeEnable(HARMONIC_TYPE_DC, ((CButton*)GetDlgItem(IDC_CHECK2))->GetCheck());

        for (int i = 1; i < 22; i++)
        {
            pSmvRecvConfigPub->SetHarmonicTypeEnable((HARMONIC_TYPE)i, ((CButton*)GetDlgItem(IDC_CHECK2 + i))->GetCheck() );
        }

        pSmvRecvConfigPub->SaveToConfigFile();

    }
}
