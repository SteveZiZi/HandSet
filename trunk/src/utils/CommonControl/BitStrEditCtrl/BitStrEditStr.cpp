/// @file
///     BitStrEditCtrl.cpp
///
/// @brief
///     λ���������ݱ༭��Inplace�ؼ�
///
/// @note
///     �ؼ������ִ�����ʽ��
///     ֱ�Ӵ�����Create�� :ȷ��/ȡ���޸�ͨ����Ϣ����
///     ģ̬���ڣ�GoModal��:ֱ�ӷ��ر༭���
///     ע�⣬λ���ĳ������� = �����ؼ�ʱ���ڳ�ʼ�����ַ����ĳ���
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.06.09
///
/// �汾��
///    1.0.x.x
/// 
/// �޸���ʷ��
///    �� ʱ��         : �汾      :  ��ϸ��Ϣ    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.07.03     |1.0.1.703  |�Ż��ַ����봦��ʹ�ؼ���ĳһ������༭������Զ���ת����һ��|
///    |2013.07.03     |1.0.1.703  |������ҷ������BackSpace����Tab������Ϣ����             |
///    |2013.07.03     |1.0.1.703  |�޸�λ���ĳ�������Ĭ��Ϊ13���ַ��������λ���������Ƶ����ýӿ�|
///    |2013.07.05     |1.0.1.705  |�޸�λ���������Ƶ����ýӿڼ�λ�������߼�               |
///
#include "stdafx.h"
#include "BitStrEditCtrl.h"

///
/// @brief
///     ���ؼ�����ʱ, ��Ӧ���˵�������ID: ɾ��һ���ַ�
///     ע��: ��ID��Ҫ���������ܶ����IDһ��, ����Ӧ���޸�
///
#define BIT_STR_EDIT_CMD_BACKSPACE  32799

///
/// @brief
///     CBitStrEditCtrl
///
BEGIN_MESSAGE_MAP(CBitStrEditCtrl, CWnd)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnEnKillfocus)
	ON_WM_KEYDOWN()
	ON_WM_SYSKEYDOWN()
	ON_WM_GETDLGCODE()
	ON_WM_CHAR()
	ON_WM_CTLCOLOR_REFLECT()
	ON_COMMAND(BIT_STR_EDIT_CMD_BACKSPACE, &CBitStrEditCtrl::OnBtnBackspace)
	ON_UPDATE_COMMAND_UI(BIT_STR_EDIT_CMD_BACKSPACE, &CBitStrEditCtrl::OnUpdateBtnBackspace)
	ON_WM_CAPTURECHANGED()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

CBitStrEditCtrl::CBitStrEditCtrl ()
: m_pParentWnd(NULL)
, m_clrText(RGB(0, 0, 0))
, m_bkColor(RGB(255, 255, 255))
, m_clrBorder(RGB(255, 255, 255))
, m_bCancel(false)
, m_bGoModal(false)
, m_strPrevBitStr(_T(""))
, m_iBitStrLen(13)
, m_pTextFont(NULL)
{
	m_bkBrush.CreateSolidBrush(RGB(255, 255, 255));
}


CBitStrEditCtrl::~CBitStrEditCtrl ()
{

}

///
/// @brief
///     ���ñ�����ɫ
///
void  CBitStrEditCtrl::SetBkColor (COLORREF color)
{
	m_bkColor = color;

	m_bkBrush.DeleteObject();
	m_bkBrush.CreateSolidBrush(color);
}

///
/// @brief
///     ����������ɫ
///
void  CBitStrEditCtrl::SetTextColor (COLORREF color)
{
	m_clrText = color;
}

///
/// @brief
///     ���ñ߿���ɫ
///
void  CBitStrEditCtrl::SetBorderColor (COLORREF color)
{
	m_clrBorder = color;
}

///
/// @brief
///     �����������壬Ĭ��Ϊ����������
///
void  CBitStrEditCtrl::SetTextFont(CFont* pFont)
{
	m_pTextFont = pFont;
}

///
/// @brief
///     ����λ���ĳ���
///
/// @return
///     TRUE - �ɹ��� FALSE - ʧ��
///
BOOL  CBitStrEditCtrl::SetBitStrLen(int len)
{
	if ( len <= 0 )
		return FALSE;

	m_iBitStrLen = len;

	// ��������Ѵ���, �����
	if ( m_hWnd )
	{
		// �����ı�
		CString str;
		GetWindowText(str);
		if ( str.GetLength() > m_iBitStrLen )
		{
			SetWindowText(str.Left(m_iBitStrLen));
		}
		else
		{
			FillBitStr(str);
			SetWindowText(str);
		}
		// ��������ַ���
		SetLimitText(m_iBitStrLen);
		// ѡ�е�1���ַ�
		SetSel(0,1);
	}
	return TRUE;
}

///
/// @brief
///     ��λ���ַ���ת��Ϊλ��ֵ
///
/// @param[in]
///     str     - λ���ַ���
/// @param[in]
///     padding - ����λ���Ĳ�λλ��
/// @param[in]
///     strSize - λ����ʵ�ʴ�С��Ϊλ��λ��/8+1
/// @param[in]
///     bitStr  - ����λ�����ݵ�����
///
/// @return
///     
///
void CBitStrEditCtrl::Str2BitStrData(const CString &str, UINT8 &padding,UINT32 &strSize,UINT8 *bitStr)
{
	if ( bitStr == NULL )
		return;

	int len = str.GetLength();
	strSize = (len + 7) / 8;
	padding = strSize * 8 - len;

	memset(bitStr, 0, strSize);
	for ( int i=0 ; i<len ;i++ )
	{
		if ( str.GetAt(i) == '1' )
		{
			bitStr[i/8] |= (1 << (7 - (i%8)));
		}
	}
}

///
/// @brief
///     ��λ��ֵת��Ϊλ���ַ���
///
/// @param[in]
///     padding - λ���Ĳ�λλ��
/// @param[in]
///     strSize - λ���Ĵ�С��Ϊλ��λ��/8+1
/// @param[in]
///     bitStr  - λ�����ݵ�����
/// @param[in]
///     str     - λ���ַ���
///
/// @return
///     
///
void CBitStrEditCtrl::BitStrData2Str(const UINT8 padding,const UINT32 strSize,const UINT8 *bitStr, CString &str)
{
	if ( bitStr == NULL )
		return;

	int len = strSize * 8 - padding;
	str  = _T("");
	UINT8 maskVal;
	for ( int i=0 ; i<len ;i++ )
	{
		if ( i % 8 == 0 )
			maskVal = 0x80;
		else
			maskVal = maskVal >> 1;

		UINT8 tmp = bitStr[i/8] & maskVal;
		if ( tmp != 0 )
			str.Append(_T("1"));
		else
			str.Append(_T("0"));
	}
}

///
/// @brief
///     ����ģ̬���ڱ༭λ������
///
/// @param[in]
///     pParentWnd - ָ�򸸴��ڵ�ָ��
/// @param[in]
///     rt         - ģ̬���ڵľ�����
/// @param[in]
///     bitStr     - ��ʼλ��
///
/// @return
///     
///
int CBitStrEditCtrl::GoModal(CWnd *pParentWnd, const CRect &rt, CString &bitStr)
{
	if (::IsWindow(m_hWnd)) 
	{
		ASSERT(FALSE);
		return FALSE;
	}

	// ��������
	Create(pParentWnd,rt,bitStr);
	m_bGoModal = true;

	SetCapture();

	m_nFlags |= WF_CONTINUEMODAL;
	int nResult = m_nModalResult;
	if (ContinueModal())
	{
		// enter modal loop
		DWORD dwFlags = MLF_SHOWONIDLE;
		if (GetStyle() & DS_NOIDLEMSG)
		{
			dwFlags |= MLF_NOIDLEMSG;
		}
		nResult = RunModalLoop(dwFlags);
	}

	// ��ȡ�༭���
	GetWindowText(bitStr);
	FillBitStr(bitStr);

	ReleaseCapture();
	DestroyWindow();

	m_bGoModal = false;

	return (nResult == IDOK);
}

///
/// @brief
///     ��������ʾλ�����ݱ༭�ؼ�
///
/// @param[in]
///     pParentWnd - ָ�򸸴��ڵ�ָ��
/// @param[in]
///     rt         - ģ̬���ڵľ�����
/// @param[in]
///     bitStr     - ��ʼλ��
///
/// @return
///     
///
void  CBitStrEditCtrl::Create(CWnd *pParentWnd, const CRect &rt, CString &bitStr)
{
	ASSERT(pParentWnd);

	// ������ڻ�û�д���, ���ȴ�������
	if (!m_hWnd)
	{
		DWORD  dwEditStyle = WS_CHILD | WS_VISIBLE | /*WS_BORDER |*/ ES_LEFT | 
			ES_AUTOHSCROLL | ES_MULTILINE;
		CEdit::Create(dwEditStyle, rt, pParentWnd, 0);
	}

	// ��ʾ���ڲ���������λ��
	this->EnableWindow(TRUE);
	this->SetFocus();

	// δ������Ĭ��Ϊ�����ڵ�����
	if ( m_pTextFont != NULL )
		this->SetFont(m_pTextFont);
	else
		this->SetFont(pParentWnd->GetFont());

	// ���ÿؼ��ı�
	CorrectBitStr(bitStr);
	FillBitStr(bitStr);
	this->SetWindowText(bitStr);
	this->SetWindowPos(0, rt.left, rt.top, rt.Width(), rt.Height(), 
		SWP_NOZORDER | SWP_SHOWWINDOW);
	this->SetMargins(0, 0);

	// ��ȡ�ؼ��ı���Ϣ
	CRect  rcWnd;
	this->GetClientRect(&rcWnd);
	CDC*  pDC = this->GetDC();
	CFont*  oldfont = pDC->SelectObject(GetFont());
	TEXTMETRIC  tm;
	pDC->GetTextMetrics(&tm);
	pDC->SelectObject(oldfont);
	this->ReleaseDC(pDC);                                           // �ǵ��ͷ�

	// ʹ���ı���ֱ����, ע��: ����ES_MULTILINE ��ʽ
	int nFontHeight = tm.tmHeight + tm.tmExternalLeading;
	int vMargin = (rcWnd.Height() - nFontHeight) / 2;
	rcWnd.DeflateRect(10, vMargin);                                     // Ĭ�����ñ༭�����ұ߾���10����
	this->SetRectNP(&rcWnd);

	// ��������ַ���
	SetLimitText(m_iBitStrLen);
	// ���ù��λ��
	SetSel(0,1);

	m_pParentWnd = pParentWnd;
	m_rtCtrlArea = rt;
	m_bCancel    = false;                                           // ��λ��־
	m_strPrevBitStr = bitStr;
}

///
/// @brief
///     ����Inplace�༭����
///
void  CBitStrEditCtrl::HideWindow ()
{
	if (m_hWnd)
	{
		this->EnableWindow(FALSE);
		this->ShowWindow(SW_HIDE);
	}
}


///
/// @brief
///     ȷ���û������޸�, ֪ͨ������
///
void  CBitStrEditCtrl::ApplyInput ()
{
	if ( m_bGoModal )
	{
		EndModalLoop(IDOK);
	}
	else
	{
		// �򸸴��ڷ�����Ϣ
		::SendMessage(m_pParentWnd->m_hWnd,WM_BIT_STR_EDIT_APPLY,0,0);
		// ���ٴ���
		DestroyWindow();
	}
}

///
/// @brief
///     �û�ȡ���޸�
///
void  CBitStrEditCtrl::CancelInput ()
{
	if ( m_bGoModal )
	{
		EndModalLoop(IDCANCEL);
	}
	else
	{
		// �򸸴��ڷ�����Ϣ
		::SendMessage(m_pParentWnd->m_hWnd,WM_BIT_STR_EDIT_CANCLE,0,0);
		// ���ٴ���
		DestroyWindow();
	}
}

///
/// @brief
///     ��Inplace���ڶ�ʧ����ʱ, ȷ���û��Ѿ�����޸�
///
void  CBitStrEditCtrl::OnEnKillfocus ()
{
	if (m_bCancel)
	{
		CancelInput();
	}
	else
	{
		ApplyInput();
	}
}

///
/// @brief
///     ʹ�ؼ��ܹ��Լ���������ļ�ͷ����TAB��
///
UINT  CBitStrEditCtrl::OnGetDlgCode ()
{
	return DLGC_WANTALLKEYS;
}

///
/// @brief
///     �ػ�ESC, �س��ȼ�, ȷ���û��޸����
///
void  CBitStrEditCtrl::OnChar (UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case VK_BACK:
		{
			KeyBackDown();
			return;
		}
		break;
	case VK_TAB:
		{
			KeyTabDown();
			return;
		}
		break;
	case VK_ESCAPE:
		{
			KeyEscapeDown();
			return;
		}
		break;
	case VK_RETURN:
		{
			KeyReturnDown();
			return;
		}
		break;
	default:
		break;
	}

	if ( nChar == '0' || nChar == '1' )
		KeyNumDown(nChar);
}

void  CBitStrEditCtrl::OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void  CBitStrEditCtrl::OnSysKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CEdit::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

void CBitStrEditCtrl::OnCaptureChanged(CWnd *pWnd)
{   
	if (m_hWnd)
	{
		m_bCancel = true;
		EndModalLoop(IDCANCEL);
	}

	CEdit::OnCaptureChanged(pWnd);   
}

BOOL CBitStrEditCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	return TRUE;
}

void CBitStrEditCtrl::OnPaint ()
{
#ifdef BIT_STR_EDIT_DOUBLEBUFFER

	CPaintDC                dc(this);           // device context for painting

	CRect                   clientRect;
	CDC                     MenDC;
	CBitmap                 paintMemMap;

	GetClientRect(&clientRect);
	MenDC.CreateCompatibleDC(&dc);
	paintMemMap.CreateCompatibleBitmap(&dc,clientRect.Width(),clientRect.Height());
	//	CreatePaintMemBitmap(&dc, clientRect.Width(), clientRect.Height());
	CBitmap*  oldbmp = MenDC.SelectObject(&paintMemMap);

	MenDC.FillSolidRect(0,0,clientRect.Width(),clientRect.Height(),m_bkColor);

	// �����Զ���ı߿�
	CPen  pen(PS_SOLID, 2, m_clrBorder);
	CPen*  oldpen = MenDC.SelectObject(&pen);
	MenDC.SelectStockObject(NULL_BRUSH);
	{
		CRect  rcFrame = clientRect;
		rcFrame.DeflateRect(1, 1);
		MenDC.Rectangle(rcFrame);
	}
	MenDC.SelectObject(oldpen);

	// ��һ���ǵ���Ĭ�ϵ�OnPaint(),��ͼ�λ����ڴ�DC����
	DefWindowProc(WM_PAINT, (WPARAM)MenDC.m_hDC, (LPARAM)0);

	//���
	dc.BitBlt(0, 0, clientRect.Width(), clientRect.Height(), &MenDC, 0, 0, SRCCOPY);

	MenDC.SelectObject(oldbmp);
	MenDC.DeleteDC();

#else  // MAC_EDIT_DOUBLEBUFFER

	CPaintDC dc(this);
	DefWindowProc(WM_PAINT, (WPARAM)dc.m_hDC, (LPARAM)0);

#endif  // MAC_EDIT_DOUBLEBUFFER
}

///
/// @brief
///     �ػ񰴼���Ϣ, �ø����ڲ����õ�ESC Enter������Ϣ
///
BOOL  CBitStrEditCtrl::PreTranslateMessage (MSG* pMsg)
{
	// ֱ�Ӱѹؼ����������, ����Ҫwindows��Ϣѭ��ȥDispatchMessage 
	if (pMsg->message == WM_KEYDOWN)
	{
		switch ( pMsg->wParam )
		{
		case VK_ESCAPE:
		case VK_RETURN:
		case VK_TAB:
			{
				OnKeyDown(static_cast<UINT>(pMsg->wParam), LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));
				OnChar(static_cast<UINT>(pMsg->wParam), LOWORD(pMsg->lParam), HIWORD(pMsg->lParam));
				return TRUE;
			}
			break;
		case VK_LEFT:
			{
				KeyMoveLeft();
				return TRUE;
			}
			break;
		case VK_RIGHT:
			{
				KeyMoveRight();
				return TRUE;
			}
			break;
		case VK_UP:
			{
				KeyMoveUp();
				return TRUE;
			}
			break;
		case VK_DOWN:
			{
				KeyMoveDown();
				return TRUE;
			}
			break;
		case VK_DELETE:
			{
				// ����DELETE
				return TRUE;
			}
			break;
		default:
			break;
		}
	}

	return CEdit::PreTranslateMessage(pMsg);
}

///
/// @brief
///     �޸Ŀؼ���ɫ
///
HBRUSH  CBitStrEditCtrl::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	// TODO:  �ڴ˸��� DC ���κ�����

	pDC->SetTextColor(m_clrText);               // �ı���ɫ
	pDC->SetBkColor(m_bkColor);                 // �ı�����ɫ
	//pDC->SetBkMode(TRANSPARENT);

	// TODO:  �����Ӧ���ø����Ĵ�������򷵻ط� null ����
	return m_bkBrush;                           // �ؼ����屳��ɫ
}

///
/// @brief
///      ��Ӧ�ⲿ�˵�����, �����˵����ɾ����ťʱ, ɾ��һ���ַ�
///      ��������ϵ�Backspace
///
void CBitStrEditCtrl::OnBtnBackspace()
{
	PostMessage(WM_CHAR, VK_BACK, 0);
}
void CBitStrEditCtrl::OnUpdateBtnBackspace(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(1);
}

///
/// @brief
///     ����λ�����ݣ�����'0'��'1'���ַ���Ϊ0
///
/// @param[in]
///     str - λ��
///
/// @return
///     
///
void CBitStrEditCtrl::CorrectBitStr(CString &str)
{
	for ( int i = 0; i < str.GetLength(); i++ )
	{
		if ( str[i] != '0' && str[i] != '1' )
			str.Replace(str[i],'0');
	}
}

///
/// @brief
///     ���λ�������Ȳ����򲹳�'0'���������ȡ
///
/// @param[in]
///     str - λ��
///
/// @return
///     
///
void CBitStrEditCtrl::FillBitStr(CString &str)
{
	if ( str.GetLength() > m_iBitStrLen )
	{
		CString s = str.Left(m_iBitStrLen);
		str = s;
	}
	else
	{
		for ( int i = str.GetLength(); i < m_iBitStrLen; i++ )
		{
			str.Append(L"0");
		}
	}
}

///
/// @brief
///     ESC������Ӧ������������ֱ�ӷ���
///
void  CBitStrEditCtrl::KeyEscapeDown()
{
	// �û�ȡ���޸�
	m_bCancel = true;
	// �ָ�Ϊԭʼ�ı�
	SetWindowText(m_strPrevBitStr);
	// ʹ��edit�ؼ���ʧ����, ����Killfocus, �ص�������
	GetParent()->SetFocus();
}

///
/// @brief
///     �س���������Ӧ
///
void  CBitStrEditCtrl::KeyReturnDown()
{
	// ���ʣ��λΪ'0'
	CString str;
	GetWindowText(str);
	FillBitStr(str);
	SetWindowText(str);
	// ʹ��edit�ؼ���ʧ����, ����Killfocus, �ص�������
	GetParent()->SetFocus();
}

///
/// @brief
///     �����������Ӧ
///
void  CBitStrEditCtrl::KeyMoveLeft()
{
	int leftPos  = LOWORD(GetSel());  // ��ȡѡ���ı���߽�����ڿؼ���λ��
	int rightPos = HIWORD(GetSel());  // ��ȡѡ���ı��ұ߽�����ڿؼ���λ��

	// ������һ��
	if ( leftPos == 0 )
	{
		leftPos  = m_iBitStrLen-1;
		rightPos = m_iBitStrLen;
	}
	else
	{
		leftPos  = leftPos-1;
		rightPos = leftPos+1;
	}
	SetSel(leftPos,rightPos);
}

///
/// @brief
///     �ҷ����������Ӧ
///
void  CBitStrEditCtrl::KeyMoveRight()
{
	int leftPos  = LOWORD(GetSel());  // ��ȡѡ���ı���߽�����ڿؼ���λ��
	int rightPos = HIWORD(GetSel());  // ��ȡѡ���ı��ұ߽�����ڿؼ���λ��

	// ������һ��
	if ( rightPos == m_iBitStrLen )
	{
		leftPos  = 0;
		rightPos = 1;
	}
	else
	{
		rightPos = rightPos+1;
		leftPos  = rightPos-1;
		
	}
	SetSel(leftPos,rightPos);
}

///
/// @brief
///     �Ϸ����������Ӧ
///
void  CBitStrEditCtrl::KeyMoveUp()
{
	int rightPos = HIWORD(GetSel());  // ��ȡѡ���ı��ұ߹������ڿؼ���λ��

	if ( rightPos == 0 )
		return;

	CString bitStr;
	GetWindowText(bitStr);
	CString str(_T("1"));
	if ( bitStr[rightPos-1] == _T('1') )
		str = _T("0");

	// �滻1���ַ�
	SetSel(rightPos-1,rightPos);
	ReplaceSel(str);
	SetSel(rightPos-1,rightPos);
}

///
/// @brief
///     �·����������Ӧ
///
void  CBitStrEditCtrl::KeyMoveDown()
{
	int rightPos = HIWORD(GetSel());  // ��ȡѡ���ı��ұ߹������ڿؼ���λ��

	if ( rightPos == 0 )
		return;

	CString bitStr;
	GetWindowText(bitStr);
	CString str(_T("1"));
	if ( bitStr[rightPos-1] == _T('1') )
		str = _T("0");

	// �滻1���ַ�
	SetSel(rightPos-1,rightPos);
	ReplaceSel(str);
	SetSel(rightPos-1,rightPos);
}

///
/// @brief
///     Backspace������Ӧ������������ֱ�ӷ���
///
void  CBitStrEditCtrl::KeyBackDown()
{
	int leftPos = LOWORD(GetSel());
	int rightPos = HIWORD(GetSel());  // ��ȡѡ���ı��ұ߹������ڿؼ���λ��

	if ( leftPos == rightPos && leftPos == 0 )
	{
		return;
	}

	// �滻1���ַ�
	SetSel(rightPos-1,rightPos);
	ReplaceSel(_T("0"));
	SetSel(rightPos-1,rightPos);
}

///
/// @brief
///     TAB������Ӧ������������ֱ�ӷ���
///
void  CBitStrEditCtrl::KeyTabDown()
{
	int leftPos  = LOWORD(GetSel());  // ��ȡѡ���ı���߽�����ڿؼ���λ��
	int rightPos = HIWORD(GetSel());  // ��ȡѡ���ı��ұ߽�����ڿؼ���λ��

	// ������һ��
	if ( rightPos == m_iBitStrLen )
	{
		leftPos  = 0;
		rightPos = 1;
	}
	else
	{
		rightPos = rightPos+1;
		leftPos  = rightPos-1;

	}
	SetSel(leftPos,rightPos);
}

///
/// @brief
///     ���ּ�������Ӧ
///
void  CBitStrEditCtrl::KeyNumDown(UINT nChar)
{
	if ( nChar != '0' && nChar != '1' )
		return;

	int leftPos  = LOWORD(GetSel());  // ��ȡѡ���ı���߹������ڿؼ���λ��
	int rightPos = HIWORD(GetSel());  // ��ȡѡ���ı��ұ߹������ڿؼ���λ��

	if ( leftPos == rightPos && leftPos == 0 )
	{
		return;
	}

	// �滻1���ַ�
	SetSel(rightPos-1,rightPos);
	CString str;
	str.Format(_T("%c"),nChar);
	ReplaceSel(str);
	if ( rightPos < m_iBitStrLen )
	{
		SetSel(rightPos,rightPos+1);
	}
	else
	{
		SetSel(rightPos-1,rightPos);
	}
}
