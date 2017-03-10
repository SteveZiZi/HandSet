// FindDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "../SCLEditor.h"
#include "../SCLEditorDoc.h"
#include "FindDlg.h"


// CFindDlg 对话框

IMPLEMENT_DYNAMIC(CFindDlg, CDialog)

CFindDlg::CFindDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFindDlg::IDD, pParent)
	, m_iRecordIdx(0)
	, m_strIEDName(_T(""))
	, m_pActiveView(NULL)
	, m_eActiveViewType(VIEW_TYPE_SMV)
{

}

CFindDlg::~CFindDlg()
{
	
}

void CFindDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_IED_NAME, m_strIEDName);
	DDX_Control(pDX, IDC_IED_NAME, m_editFind);
}

BEGIN_MESSAGE_MAP(CFindDlg, CDialog)
	ON_BN_CLICKED(IDC_FIND_NEXT, &CFindDlg::OnBnClickedFind)
	ON_WM_LBUTTONDOWN()
	ON_EN_CHANGE(IDC_IED_NAME, &CFindDlg::OnEnChangeIedName)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CFindDlg 消息处理程序

BOOL CFindDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	//移除边框
	/************************************************************************************/
	/*
	DWORD dwStyle = GetStyle();
	DWORD dwNewStyle = WS_OVERLAPPED | WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	dwNewStyle &= dwStyle;
	SetWindowLong(m_hWnd, GWL_STYLE, dwNewStyle);
	DWORD dwExStyle = GetExStyle();
	DWORD dwNewExStyle = WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR;
	dwNewExStyle &= dwExStyle;
	SetWindowLong(m_hWnd, GWL_EXSTYLE, dwNewExStyle);
	*/
	
	DWORD dwStyle = m_editFind.GetStyle();
	dwStyle = dwStyle | DT_VCENTER | DT_SINGLELINE;
	SetWindowLong(m_editFind,GWL_STYLE,dwStyle);
	


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CFindDlg::OnBnClickedFind()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if ( m_pActiveView == NULL )
		return;

	switch ( m_eActiveViewType )
	{
	case VIEW_TYPE_SMV:
		{
			m_iRecordIdx = ((CSclSmvView *)m_pActiveView)->FindIED(m_strIEDName,m_iRecordIdx);
			if ( m_iRecordIdx == ((CSclSmvView *)m_pActiveView)->GetCtrlCount()-1  ||
				 m_iRecordIdx == -1 )
			{
				m_iRecordIdx = 0;
			}
			else
			{
				m_iRecordIdx++;
			}
		}
		break;
	case VIEW_TYPE_GOOSE:
		{
			m_iRecordIdx = ((CSclGooseView *)m_pActiveView)->FindIED(m_strIEDName, m_iRecordIdx);
			if ( m_iRecordIdx == ((CSclGooseView *)m_pActiveView)->GetCtrlCount()-1 ||
				 m_iRecordIdx == -1 )
			{
				m_iRecordIdx = 0;
			}
			else
			{
				m_iRecordIdx++;
			}
		}
		break;
	}
}

void CFindDlg::SetActiveView(CView *pView, VIEW_TYPE eViewType)
{
	m_pActiveView = pView;
	m_eActiveViewType = eViewType;
}

void CFindDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	// 窗口移动消息
	PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
	
	return;
	//CDialog::OnLButtonDown(nFlags, point);
}

void CFindDlg::OnEnChangeIedName()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void CFindDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
	CRect rc;
	GetClientRect(&rc);
	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);
	CBitmap bmpBackground;
	bmpBackground.LoadBitmap(IDB_FINDDLG);
	BITMAP bitmap;
	bmpBackground.GetBitmap(&bitmap);
	CBitmap *pbmpOld = dcMem.SelectObject(&bmpBackground);
	dc.StretchBlt(0,0,rc.Width(),rc.Height(),&dcMem,0,0,bitmap.bmWidth,bitmap.bmHeight,SRCCOPY);
}
