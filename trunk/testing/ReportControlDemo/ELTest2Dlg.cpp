// ELTest2Dlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ReportControlDemo.h"
#include "ELTest2Dlg.h"


// CELTest2Dlg �Ի���

IMPLEMENT_DYNAMIC(CELTest2Dlg, CDialog)

CELTest2Dlg::CELTest2Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CELTest2Dlg::IDD, pParent)
{
    // font   width height
    // L"MSYH", 0, 18, FW_NORMAL
    // L"MSYH", 0, 18, FW_NORMAL
    // L"MSYH", 0, 18, FW_NORMAL
    // hFont = CreateFont(height, width, 0, 0, weight, FALSE, FALSE, 0,
    //    ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
    //    DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, fontName);
    //
    //HDC  hDC = ::GetDC(NULL);
    //LONG  lfHeight = -::MulDiv(11, GetDeviceCaps(hDC, LOGPIXELSY), 72);
    //::ReleaseDC(NULL, hDC);

    m_font.CreateFont(ELT_T1_TEXTLFHEIGHT, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_COMPAT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"΢���ź�");
}

CELTest2Dlg::~CELTest2Dlg()
{
}

void CELTest2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CELTest2Dlg, CDialog)
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CELTest2Dlg ��Ϣ�������

BOOL CELTest2Dlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��

    m_aboutCtrl.Create(
        WS_VISIBLE|WS_CHILD|WS_TABSTOP|LVS_REPORT|LVS_SINGLESEL|LVS_SHOWSELALWAYS,
        CRect(0, 0, 500, 350), this, 200);
    m_aboutCtrl.SetFont(&m_font);
    m_aboutCtrl.PostInitTemplateStyle();

    m_aboutCtrl.SetAboutItemText(CListCtrlT_AboutCtrl::ROW_SW_VER, CListCtrlT_AboutCtrl::COL_DESC, L"V1.5.2");


    // �������ڶ���
    m_ctlListEL.Create(
        WS_VISIBLE|WS_CHILD|WS_TABSTOP|LVS_REPORT|LVS_SINGLESEL|LVS_SHOWSELALWAYS,
        CRect(10, 10, 610, 340), this, 100);
    m_ctlListEL.SetHeaderFont(&m_font);
    m_ctlListEL.SetFont(&m_font);

    m_ctlListEL.PostInitTemplateStyle();


    // ������Ԫ��
    m_ctlListEL.InsertColumn(0,_T("����"), EL_CENTER, 100, FALSE, EL_LEFT, RGB(33, 33, 33));
    m_ctlListEL.InsertColumn(1,_T("���"), EL_LEFT, 100);
    int  col2 = m_ctlListEL.InsertColumn(2,_T("����"), EL_CENTER, 100);
    int  col3 = m_ctlListEL.InsertColumn(3,_T("����ʱ��"), EL_LEFT, 300, FALSE, EL_LEFT);


    // ����ѡ�񽹵���ʽ
    m_ctlListEL.SetFreezeSelectionVisible(0);
    m_ctlListEL.SetFreezeColumnsCount(2);
    m_ctlListEL.SetSubItemFocusEnable(1);
    m_ctlListEL.SetRowSelectionVisible(TRUE);
    m_ctlListEL.SetRowFocusEnable(TRUE);



    // ��������
    for (int i = 0; i < 9; i++)
    {
        CString  str;
        str.Format(L"%d", i);
        int  nItem = m_ctlListEL.InsertRecordRow(i, str);
        m_ctlListEL.SetRowItemText(nItem, 1, L"178CM");
        m_ctlListEL.SetRowItemText(nItem, 2, L"70KG");
        m_ctlListEL.SetRowItemText(nItem, 3, L"2009��1��15��23ʱ40��");
    }

    return TRUE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}

HBRUSH CELTest2Dlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    if (nCtlColor == 4)
    {
        static HBRUSH  brs = ::CreateSolidBrush(RGB(0x1A, 0x21, 0x2D));
        return brs;
    }
    else
    {
        HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

        // TODO:  �ڴ˸��� DC ���κ�����
        // TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
        return hbr;
    }
}
