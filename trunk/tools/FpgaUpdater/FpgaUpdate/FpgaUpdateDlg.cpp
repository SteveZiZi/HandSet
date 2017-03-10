// FpgaUpdateDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FpgaUpdate.h"
#include "FpgaUpdateDlg.h"
#include "FpgaBootOperate.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

///< FPGA�̼����·��
#define FPGA_BOOT_FILE	L"\\Storage Card\\fpgaBoot"

CRITICAL_SECTION g_cs;
// CFpgaUpdateDlg �Ի���

CFpgaUpdateDlg::CFpgaUpdateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFpgaUpdateDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_curRange = 0.0;
    m_curDetailRange = 0.0;
    InitializeCriticalSection(&g_cs);
}

void CFpgaUpdateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFpgaUpdateDlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	//}}AFX_MSG_MAP
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_TIMER()
END_MESSAGE_MAP()


// CFpgaUpdateDlg ��Ϣ�������

BOOL CFpgaUpdateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

    //����λͼ
	m_mainBG.LoadBitmap(IDB_BITMAP_DM8000);
    m_scroll.LoadBitmap(IDB_BITMAP_SCROLL);
    m_scrollBG.LoadBitmap(IDB_BITMAP_SCROLL_BG);
    m_SuccBG.LoadBitmap(IDB_BITMAP_SUCCESS);
    m_FailBG.LoadBitmap(IDB_BITMAP_FAIL);
    m_runScroll.LoadBitmap(IDB_BITMAP_RUN_SCROLL);

    int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

    
    BITMAP bmp;
    m_mainBG.GetBitmap(&bmp);

    SetWindowPos(NULL, 0, 0, bmp.bmWidth, bmp.bmHeight, WS_EX_TOPMOST);
    m_scrollBG.GetBitmap(&bmp);

    m_scrollLeftP.x = (nScreenWidth - bmp.bmWidth) / 2;
    m_scrollLeftP.y = (nScreenHeight - bmp.bmHeight) * 2 / 5;

    m_scrollDetailLeftP.x = m_scrollLeftP.x;
    m_scrollDetailLeftP.y = (nScreenHeight - bmp.bmHeight) * 3 / 5;

    m_textLeftP.x = 0; 
    m_textLeftP.y = m_scrollLeftP.y + 35;

    m_textDetailLeftP.x = 0;
    m_textDetailLeftP.y = m_scrollDetailLeftP.y + 35;

    m_SuccBG.GetBitmap(&bmp);
    m_resultLeftP.x = (nScreenWidth - bmp.bmWidth) / 2;
    m_resultLeftP.y = (nScreenHeight - bmp.bmHeight) * 2 / 5;

    //��������
    m_fntContent.CreateFont(
        20,				        //���߼���λ��ʽָ������ĸ߶�
        0,						//���߼���λ��ʽָ���������ַ���ƽ�����
        0,						//ָ��ƫ�봹�ߺ�X������ʾ���ϵļнǣ���λ��0.1�ȣ�
        0,						//ָ���ַ������ߺ�X��֮��ļнǣ���λ��0.1�ȣ�
        FW_NORMAL,			    //ָ���������
        FALSE,					//�ǲ���б��
        FALSE,					//�Ӳ����»���
        0,						//ָ���Ƿ��������ַ�ͻ��
        ANSI_CHARSET,			//ָ��������ַ���
        OUT_DEFAULT_PRECIS,		//ָ��������������
        CLIP_DEFAULT_PRECIS,	//ָ������ļ�������
        DEFAULT_QUALITY,		//ָʾ������������
        FF_ROMAN,               //ָ������ļ��ͼ���
        _T("����")			    //ָ���������������
        );

    ShowWindow(SW_SHOWMAXIMIZED);

    //���������߳�
    AfxBeginThread(&StartUpdateThread, this);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CFpgaUpdateDlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	if (AfxIsDRAEnabled())
	{
		DRA::RelayoutDialog(
			AfxGetResourceHandle(), 
			this->m_hWnd, 
			DRA::GetDisplayMode() != DRA::Portrait ? 
			MAKEINTRESOURCE(IDD_FPGAUPDATE_DIALOG_WIDE) : 
			MAKEINTRESOURCE(IDD_FPGAUPDATE_DIALOG));
	}
}
#endif


void CFpgaUpdateDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO: �ڴ˴������Ϣ����������
    CRect rectClient;
    GetClientRect(&rectClient);

    CDC dcMem;
    CBitmap bmpMem;
    CBitmap * pOldBitMap;
    dcMem.CreateCompatibleDC(&dc); 
    bmpMem.CreateCompatibleBitmap(&dc, rectClient.Width(), rectClient.Height());
    
    pOldBitMap = dcMem.SelectObject(&bmpMem);

    //���������������dcMem��
    DrawBG(&dcMem);

    dc.BitBlt(0, 0, rectClient.Width(), rectClient.Height(), &dcMem, 0, 0, SRCCOPY);

    dcMem.SelectObject(pOldBitMap);
    dcMem.DeleteDC(); 
    bmpMem.DeleteObject();
}

///
/// @brief
///     ���ڴ�DC�л�������
/// @param[CDC *]
///     pDC - ����ȥ���ڴ�DC
/// @return
///
///
void CFpgaUpdateDlg::DrawBG( CDC * pDC )
{
    if(pDC == NULL || !pDC->GetSafeHdc())
    {
        return ;
    }

    //���ñ���Ϊ͸��
    pDC->SetBkMode(TRANSPARENT);

    CRect rc;
    CFont * pFont;
    BITMAP bm; 
    GetClientRect(&rc);
    m_mainBG.GetBitmap(&bm);

    int nStartX = (GetSystemMetrics(SM_CXSCREEN) - bm.bmWidth) / 2;

    CDC dc;
    dc.CreateCompatibleDC(pDC);
    CBitmap * pOldBmp = dc.SelectObject(&m_mainBG);

    //���Ʊ���λͼ
    pDC->BitBlt(nStartX, 0, bm.bmWidth, bm.bmHeight, &dc, 0, 0, SRCCOPY);
    dc.SelectObject(pOldBmp);
    
    if(m_curRange <= 1.0)
    {
    
        pOldBmp = dc.SelectObject(&m_scrollBG);
        m_scrollBG.GetBitmap(&bm);    
        int nScrollLen = m_scrollLeftP.x + bm.bmWidth;

        //���ƽ��ȹ��������
        pDC->BitBlt(m_scrollLeftP.x, m_scrollLeftP.y, 
            bm.bmWidth, bm.bmHeight, &dc, 0, 0, SRCCOPY);

        //���ƾ�����������
        pDC->BitBlt(m_scrollDetailLeftP.x, m_scrollDetailLeftP.y, 
            bm.bmWidth, bm.bmHeight, &dc, 0, 0, SRCCOPY);

        int nCnt = (int)(m_curRange / 0.01);
        dc.SelectObject(m_scroll);
        m_scroll.GetBitmap(&bm);

        int nX = m_scrollLeftP.x;
        int nY = m_scrollLeftP.y + 2;

        //���ƽ��ȹ��������ı���
        for(int i = 0; i < nCnt; i++)
        {
            pDC->BitBlt(nX, nY, bm.bmWidth, bm.bmHeight, &dc, 0, 0, SRCCOPY);
            nX += bm.bmWidth;
        }

        dc.SelectObject(&m_runScroll);
        m_runScroll.GetBitmap(&bm);

        nCnt = (int)(m_curDetailRange / 0.01);
        nX = m_scrollDetailLeftP.x + nCnt * bm.bmWidth;
        nY = m_scrollDetailLeftP.y + 4;
        int nTimes = nCnt > 0 ? 4 : 2;
        
        if(nTimes == 2)
        {
            nX += 2;
        }

        //���ƾ����������λ��
        for(int i = 0; i < nTimes && m_curRange < 1.0; i++)
        {
            //�ж��Ƿ���˱߽�
            if(nX + bm.bmWidth <= nScrollLen)
            {
                pDC->BitBlt(nX, nY, bm.bmWidth, bm.bmHeight, &dc, 0, 0, SRCCOPY);
            }
            else
            {
                m_curDetailRange = 1.0;
            }
            nX += bm.bmWidth + 4;
        }

        dc.SelectObject(pOldBmp);

        //�����ʵ���ֲ�Ϊ��
        if(m_strText.Compare(L"") != 0 && m_strDetailText.Compare(L"") != 0)
        {
           
            pFont = pDC->SelectObject(&m_fntContent);
            pDC->SetTextColor(RGB(255, 255, 255));
            pDC->SetBkColor(RGB(0, 0, 0));

            CSize size = pDC->GetTextExtent(m_strText);
            RECT rect;
            rect.left = m_textLeftP.x;
            rect.top = m_textLeftP.y;
            rect.bottom = rect.top + size.cy;
            rect.right = rc.Width();

            //��������
            pDC->DrawText(m_strText, m_strText.GetLength(), &rect, 
                DT_SINGLELINE | DT_VCENTER | DT_CENTER);

            size = pDC->GetTextExtent(m_strDetailText);
            rect.left = m_textDetailLeftP.x;
            rect.top = m_textDetailLeftP.y;
            rect.bottom = rect.top + size.cy;

            pDC->DrawText(m_strDetailText, m_strDetailText.GetLength(), &rect, 
                DT_SINGLELINE | DT_VCENTER | DT_CENTER);
        }
    }
    //��������Ѿ�����
    else if(m_curRange > 1.0)
    {
        if(m_bIsSuccess)
        {
            pOldBmp = dc.SelectObject(&m_SuccBG);
            m_SuccBG.GetBitmap(&bm);
        }
        else
        {
            pOldBmp = dc.SelectObject(&m_FailBG);
            m_FailBG.GetBitmap(&bm);
        }
        
        //���ƽ������ʾλͼ
        pDC->BitBlt(m_resultLeftP.x, m_resultLeftP.y, 
            bm.bmWidth, bm.bmHeight, &dc, 0, 0, SRCCOPY);
        dc.SelectObject(pOldBmp);

        if(m_strText.Compare(L"") != 0)
        {
            pFont = pDC->SelectObject(&m_fntContent);
            pDC->SetTextColor(RGB(255, 255, 255));

            CSize size = pDC->GetTextExtent(m_strText);
            RECT rect;
            rect.left = m_resultLeftP.x + 121;
            rect.top = m_resultLeftP.y + 63;
            rect.bottom = rect.top + size.cy;
            rect.right = rect.left + size.cx;

            pDC->DrawText(m_strText, m_strText.GetLength(), &rect, 
               DT_SINGLELINE|DT_VCENTER|DT_LEFT );
        }
    }

    dc.SelectObject(pFont);
    dc.DeleteDC();
}

///
/// @brief
///     ���ý���������ʾ���
/// @param[double]
///     curRange - ��ǰ��������ɽ���
/// @param[CString ]
///     strText - ���ȹ�����Ҫ��ʾ������
/// @param[CString ]
///     strDetailText - ���������Ҫ��ʾ������
/// @param[bool ]
///     bIsSuccess - �Ƿ������ɹ�
/// @return
///
///
void CFpgaUpdateDlg::SetProcessRange( double curRange, CString strText, 
                                     CString strDetailText, bool bIsSuccess )
{
    m_curRange = curRange;
    m_strText = strText;
    m_bIsSuccess = bIsSuccess;
    if(strDetailText.Compare(L"") != 0)
    {
        m_strDetailText = strDetailText;
    }
    Invalidate();
    UpdateWindow();
}

BOOL CFpgaUpdateDlg::OnEraseBkgnd(CDC* pDC)
{
    //ɾ��ȫ����ʱ������CDC
    CDC::DeleteTempMap();

    return TRUE;
}

void CFpgaUpdateDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    if(nIDEvent == 1)
    {
        EnterCriticalSection(&g_cs);
        m_curDetailRange += 0.05;
        if(m_curDetailRange > 1.0)
        {
            m_curDetailRange = 0;
        }
       
        //������������Ѿ�����
        if(m_curRange >= 1.0)
        {
            //������ʱ��
            KillTimer(nIDEvent);
        } 
        LeaveCriticalSection(&g_cs);
        Invalidate();
        UpdateWindow();
        
    }
    CDialog::OnTimer(nIDEvent);
}

///
/// @brief
///     ����FPGA�Ĺ����߳�
/// @param[LPVOID ]
///     pParam - �̲߳���
/// @return
///     �̵߳��˳�ֵ
///
UINT CFpgaUpdateDlg::StartUpdateThread( LPVOID pParam )
{
    CFpgaUpdateDlg * pFpgaUpdateDlg = (CFpgaUpdateDlg *)pParam;

    //����FpgaBootOperate���󣬲����뵱ǰ�Ի����ָ��
    FpgaBootOperate * pFpgaBootOperate = new FpgaBootOperate(pFpgaUpdateDlg);

    //��ʼ����FPGA����
    pFpgaBootOperate->WriteFpgaBoot(FPGA_BOOT_FILE);

    delete pFpgaBootOperate;

    //�رնԻ���
    pFpgaUpdateDlg->OnCancel();
    return 0;
}

CFpgaUpdateDlg::~CFpgaUpdateDlg()
{
    DeleteCriticalSection(&g_cs);
}
