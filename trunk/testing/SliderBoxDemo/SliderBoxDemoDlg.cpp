// SliderBoxDemoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SliderBoxDemo.h"
#include "SliderBoxDemoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSliderBoxDemoDlg �Ի���

CSliderBoxDemoDlg::CSliderBoxDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSliderBoxDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSliderBoxDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSliderBoxDemoDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CSliderBoxDemoDlg ��Ϣ�������

BOOL CSliderBoxDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

    // ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
    SetWindowPos(NULL, 0, 0, 640, 380, 0);
    CRect rect;
    GetClientRect(&rect);
	m_SliderBox.Create(WS_VISIBLE|WS_CHILD|WS_TABSTOP, rect,this, 
        1000);
    //CString strImgDir = L"\\Storage Card\\images\\";
    CString strImgDir = L"\\FlashDisk2\\images\\";
    BOOL ret = m_SliderBox.LoadImages(strImgDir);
    if(!ret)
    {
        NKDbgPrintfW(L"����ͼƬʧ��!\n");
    }
    //ASSERT(ret);
    m_SliderBox.SetAnimateFreq(12);
    m_SliderBox.SetScrollTimeout(350);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CSliderBoxDemoDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	if (AfxIsDRAEnabled())
	{
		DRA::RelayoutDialog(
			AfxGetResourceHandle(), 
			this->m_hWnd, 
			DRA::GetDisplayMode() != DRA::Portrait ? 
			MAKEINTRESOURCE(IDD_SLIDERBOXDEMO_DIALOG_WIDE) : 
			MAKEINTRESOURCE(IDD_SLIDERBOXDEMO_DIALOG));
	}
}
#endif

