// ExlvDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ReportControlDemo.h"
#include "ExlvDlg.h"


// CExlvDlg 对话框

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


// CExlvDlg 消息处理程序

BOOL CExlvDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化

    m_ListCtrl.SetColColor(0,RGB(10,150,20)); //设置列背景色
    m_ListCtrl.SetColColor(2,RGB(30,100,90)); //设置列背景色
    m_ListCtrl.SetBkColor(RGB(50,10,10));        //设置背景色
    m_ListCtrl.SetItemColor(1,1,RGB(100,100,10)); //设置指定单元背景色
    m_ListCtrl.SetRowHeigt(25);               //设置行高度
    m_ListCtrl.SetHeaderHeight(1.5);          //设置头部高度
    m_ListCtrl.SetHeaderFontHW(16,0);         //设置头部字体高度,和宽度,0表示缺省，自适应 
    m_ListCtrl.SetHeaderTextColor(RGB(255,200,100)); //设置头部字体颜色
    m_ListCtrl.SetTextColor(RGB(0,255,255));  //设置文本颜色
    m_ListCtrl.SetHeaderBKColor(100,255,100,8); //设置头部背景色
    m_ListCtrl.SetFontHW(15,0);               //设置字体高度，和宽度,0表示缺省宽度
    m_ListCtrl.SetColTextColor(2,RGB(255,255,100)); //设置列文本颜色
    m_ListCtrl.SetItemTextColor(3,1,RGB(255,0,0));  //设置单元格字体颜色


    m_ListCtrl.InsertColumn(0,_T("名字"),LVCFMT_CENTER,55);
    m_ListCtrl.InsertColumn(1,_T("身高"),LVCFMT_CENTER,60);
    m_ListCtrl.InsertColumn(2,_T("体重"),LVCFMT_CENTER,60);
    m_ListCtrl.InsertColumn(3,_T("测量时间"),LVCFMT_CENTER,180);

    for (int i = 0; i < 100; i++)
    {

    
    int  nItem = m_ListCtrl.InsertItem(0, L"张三");
    m_ListCtrl.SetItemText(nItem,1, L"178CM");
    m_ListCtrl.SetItemText(nItem,2, L"70KG");
    m_ListCtrl.SetItemText(nItem,3, L"2009年1月15日23时40分");
    }



    SetWindowLong(m_ListCtrl.m_hWnd ,GWL_EXSTYLE,WS_EX_CLIENTEDGE);
    m_ListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES);                     //设置扩展风格为网格
    ::SendMessage(m_ListCtrl.m_hWnd, LVM_SETEXTENDEDLISTVIEWSTYLE,
        LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    m_ListCtrl.ShowScrollBar(SB_VERT, FALSE);


    SetTimer(1, 300, NULL);


    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void CExlvDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    m_counter++;

    CString  str1;
    CString  str2;
    str1.Format(L"%d.%d", m_counter, m_counter);
    str1.Format(L"%d.%d", m_counter, m_counter);
    
    m_ListCtrl.SetItemText(1, 1, str1);
    m_ListCtrl.SetItemText(2, 3, str1);


    CDialog::OnTimer(nIDEvent);
}
