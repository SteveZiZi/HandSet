// BmpDisplayDemoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BmpDisplayDemo.h"
#include "BmpDisplayDemoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CBmpDisplayDemoDlg �Ի���

CBmpDisplayDemoDlg::CBmpDisplayDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBmpDisplayDemoDlg::IDD, pParent)
	, m_csBmpPath(_T(""))
	, m_csBmpDir(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBmpDisplayDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBmpDisplayDemoDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	//}}AFX_MSG_MAP
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CBmpDisplayDemoDlg ��Ϣ�������

BOOL CBmpDisplayDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	// �趨�Ի���ȫ��
	int iFullWidth = GetSystemMetrics(SM_CXSCREEN);
	int iFullHeight = GetSystemMetrics(SM_CYSCREEN);
	SetWindowPos(&wndTopMost,  0, 0, iFullWidth, iFullHeight, 
		SWP_NOOWNERZORDER|SWP_SHOWWINDOW);
	
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CBmpDisplayDemoDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	if (AfxIsDRAEnabled())
	{
		DRA::RelayoutDialog(
			AfxGetResourceHandle(), 
			this->m_hWnd, 
			DRA::GetDisplayMode() != DRA::Portrait ? 
			MAKEINTRESOURCE(IDD_BMPDISPLAYDEMO_DIALOG_WIDE) : 
			MAKEINTRESOURCE(IDD_BMPDISPLAYDEMO_DIALOG));
	}
}
#endif

void CBmpDisplayDemoDlg::DrawBitmap(CDC *pDC,CString path)
{

	CBitmap   bitmap; 

	//����BMPͼƬ
#ifdef WINCE
	HBITMAP   hBitmap = (HBITMAP)::SHLoadDIBitmap(path);
#else
	HBITMAP   hBitmap = (HBITMAP)LoadImage(NULL,bmpPath,IMAGE_BITMAP,0,0,LR_DEFAULTSIZE|LR_LOADFROMFILE); 
#endif
	bitmap.Attach(hBitmap);

	CDC    dcComp; 
	dcComp.CreateCompatibleDC(pDC);
	CBitmap* pOldBitMap = dcComp.SelectObject(&bitmap);

	//�õ�BMPͼƬ������Ϣ
	BITMAP   bmInfo; 
	bitmap.GetObject(sizeof(bmInfo),&bmInfo); 
	//�߽�
	CRect rect;
	GetClientRect(&rect);
	//��ʾͼƬ
	pDC->BitBlt(0,0,bmInfo.bmWidth,bmInfo.bmHeight,&dcComp,0,0,SRCCOPY);
	//	pDC->StretchBlt(0,0,rect.Width(),rect.Height(),&dcComp,0,0,bmInfo.bmWidth,bmInfo.bmHeight,SRCCOPY);
	//���
	dcComp.SelectObject(pOldBitMap);
	dcComp.DeleteDC();
	bitmap.DeleteObject();
}

void CBmpDisplayDemoDlg::OpenBmpFile()
{
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT,
		NULL, this);
	// �������ѡ���Ŀ¼
	if ( m_csBmpDir != _T("") )
	{
		dlg.m_ofn.lpstrInitialDir = m_csBmpDir;
	}

	if(dlg.DoModal() == IDOK)
	{
		m_csBmpPath = dlg.GetPathName();
		m_csBmpDir = m_csBmpPath.Left(m_csBmpPath.GetLength()-dlg.GetFileName().GetLength()-1);
		CString extName = dlg.GetFileExt();
		if ( extName.Compare(_T("bmp"))  == 0 || extName.Compare(_T("BMP"))  == 0 )
		{
			Invalidate();
		}
		else
		{
			m_csBmpPath = _T("");
		}
	}
}

void CBmpDisplayDemoDlg::OnPaint()
{
	CPaintDC dc(this);
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CDialog::OnPaint()

	//����ļ����·�����ڣ��������ʾ
	if (!(m_csBmpPath.IsEmpty()))
	{
		DrawBitmap(&dc,m_csBmpPath);
		m_csBmpPath = _T("");
	}
}

BOOL CBmpDisplayDemoDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���

	if ( pMsg->wParam == VK_RETURN)
	{
		if ( pMsg->message == WM_KEYDOWN )
			OpenBmpFile();
		return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}
