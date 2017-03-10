// ExlvDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ReportControlDemo.h"
#include "ExlvDlg.h"


// CExlvDlg �Ի���

IMPLEMENT_DYNAMIC(CExlvDlg, CDialog)

CExlvDlg::CExlvDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExlvDlg::IDD, pParent)
{
    m_counter = 0;
}

CExlvDlg::~CExlvDlg()
{
}

void CExlvDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_EXLV, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(CExlvDlg, CDialog)
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CExlvDlg ��Ϣ�������

BOOL CExlvDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��

    m_ListCtrl.SetColColor(0,RGB(10,150,20)); //�����б���ɫ
    m_ListCtrl.SetColColor(2,RGB(30,100,90)); //�����б���ɫ
    m_ListCtrl.SetBkColor(RGB(50,10,10));        //���ñ���ɫ
    m_ListCtrl.SetItemColor(1,1,RGB(100,100,10)); //����ָ����Ԫ����ɫ
    m_ListCtrl.SetRowHeigt(25);               //�����и߶�
    m_ListCtrl.SetHeaderHeight(1.5);          //����ͷ���߶�
    m_ListCtrl.SetHeaderFontHW(16,0);         //����ͷ������߶�,�Ϳ��,0��ʾȱʡ������Ӧ 
    m_ListCtrl.SetHeaderTextColor(RGB(255,200,100)); //����ͷ��������ɫ
    m_ListCtrl.SetTextColor(RGB(0,255,255));  //�����ı���ɫ
    m_ListCtrl.SetHeaderBKColor(100,255,100,8); //����ͷ������ɫ
    m_ListCtrl.SetFontHW(15,0);               //��������߶ȣ��Ϳ��,0��ʾȱʡ���
    m_ListCtrl.SetColTextColor(2,RGB(255,255,100)); //�������ı���ɫ
    m_ListCtrl.SetItemTextColor(3,1,RGB(255,0,0));  //���õ�Ԫ��������ɫ


    m_ListCtrl.InsertColumn(0,_T("����"),LVCFMT_CENTER,55);
    m_ListCtrl.InsertColumn(1,_T("���"),LVCFMT_CENTER,60);
    m_ListCtrl.InsertColumn(2,_T("����"),LVCFMT_CENTER,60);
    m_ListCtrl.InsertColumn(3,_T("����ʱ��"),LVCFMT_CENTER,180);

    for (int i = 0; i < 100; i++)
    {

    
    int  nItem = m_ListCtrl.InsertItem(0, L"����");
    m_ListCtrl.SetItemText(nItem,1, L"178CM");
    m_ListCtrl.SetItemText(nItem,2, L"70KG");
    m_ListCtrl.SetItemText(nItem,3, L"2009��1��15��23ʱ40��");
    }



    SetWindowLong(m_ListCtrl.m_hWnd ,GWL_EXSTYLE,WS_EX_CLIENTEDGE);
    m_ListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES);                     //������չ���Ϊ����
    ::SendMessage(m_ListCtrl.m_hWnd, LVM_SETEXTENDEDLISTVIEWSTYLE,
        LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    m_ListCtrl.ShowScrollBar(SB_VERT, FALSE);


    SetTimer(1, 300, NULL);


    return TRUE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}

void CExlvDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

    m_counter++;

    CString  str1;
    CString  str2;
    str1.Format(L"%d.%d", m_counter, m_counter);
    str1.Format(L"%d.%d", m_counter, m_counter);
    
    m_ListCtrl.SetItemText(1, 1, str1);
    m_ListCtrl.SetItemText(2, 3, str1);


    CDialog::OnTimer(nIDEvent);
}
