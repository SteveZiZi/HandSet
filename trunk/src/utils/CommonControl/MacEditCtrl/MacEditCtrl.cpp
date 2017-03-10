/// @file
///     MacEditCtrl.cpp
///
/// @brief
///     Mac��ַ�༭��Inplace�ؼ�
///
/// @note
///     �ؼ������ִ�����ʽ��
///     ֱ�Ӵ�����Create�� :ȷ��/ȡ���޸�ͨ����Ϣ����
///     ģ̬���ڣ�GoModal��:ֱ�ӷ��ر༭���
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.5.24
///
/// �汾��
///    1.0.x.x
/// 
/// �޸���ʷ��
///    �� ʱ��         : �汾      :  ��ϸ��Ϣ    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.07.02     |1.0.1.702  |�Ż��ַ����봦��ʹ�ؼ���ĳһ������༭������Զ���ת����һ��|
///    |2013.10.15     |1.0.1.1015 |��ӽ�Macֵ��ΪȫFF��������Ϣ                                 |
///
#include "stdafx.h"
#include "MacEditCtrl.h"

///
/// @brief
///     Mac��ַ���ĸ�ʽ���ֶ������޶��ַ���
///
#define MAC_FORMAT_STR _T("%02x-%02x-%02x-%02x-%02x-%02x")
#define MAC_STR_FIELD_NUM 6
#define MAC_STR_SIZE 27

///
/// @brief
///     Mac��ַ���и��ε�ƫ�Ƶ�ַ������
///
const int k_MacPos[6] = {0,3,6,9,12,15};
const int k_MacLen[6] = {2,2,2,2,2,2};

///
/// @brief
///     ���ؼ�����ʱ, ��Ӧ���˵�������ID: ɾ��һ���ַ�
///     ע��: ��ID��Ҫ���������ܶ����IDһ��, ����Ӧ���޸�
///
#define MAC_EDIT_CMD_BACKSPACE  32799

///
/// @brief
///     CMacEditCtrl
///
BEGIN_MESSAGE_MAP(CMacEditCtrl, CWnd)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnEnKillfocus)
	ON_WM_KEYDOWN()
	ON_WM_SYSKEYDOWN()
	ON_WM_GETDLGCODE()
	ON_WM_CHAR()
	ON_WM_CTLCOLOR_REFLECT()
	ON_COMMAND(MAC_EDIT_CMD_BACKSPACE, &CMacEditCtrl::OnBtnBackspace)
	ON_UPDATE_COMMAND_UI(MAC_EDIT_CMD_BACKSPACE, &CMacEditCtrl::OnUpdateBtnBackspace)
	ON_COMMAND(MAC_EDIT_CMD_SET_TO_ALL_FF, &CMacEditCtrl::OnBtnSetMacToAllFF)
	ON_UPDATE_COMMAND_UI(MAC_EDIT_CMD_SET_TO_ALL_FF, &CMacEditCtrl::OnUpdateBtnSetMacToAllFF)
	ON_WM_CAPTURECHANGED()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

CMacEditCtrl::CMacEditCtrl ()
: m_pParentWnd(NULL)
, m_clrText(RGB(0, 0, 0))
, m_bkColor(RGB(255, 255, 255))
, m_clrBorder(RGB(255, 255, 255))
, m_bCancel(false)
, m_bGoModal(false)
, m_pTextFont(NULL)
{
	m_bkBrush.CreateSolidBrush(RGB(255, 255, 255));
}


CMacEditCtrl::~CMacEditCtrl ()
{

}

///
/// @brief
///     ���ñ�����ɫ
///
void  CMacEditCtrl::SetBkColor (COLORREF color)
{
	m_bkColor = color;

	m_bkBrush.DeleteObject();
	m_bkBrush.CreateSolidBrush(color);
}

///
/// @brief
///     ����������ɫ
///
void  CMacEditCtrl::SetTextColor (COLORREF color)
{
	m_clrText = color;
}

///
/// @brief
///     ���ñ߿���ɫ
///
void  CMacEditCtrl::SetBorderColor (COLORREF color)
{
	m_clrBorder = color;
}

///
/// @brief
///     �����������壬Ĭ��Ϊ����������
///
void  CMacEditCtrl::SetTextFont(CFont* pFont)
{
	m_pTextFont = pFont;
}

///
/// @brief
///     ��Mac��ַ��ת��Ϊ����
///
/// @param[in]
///     str    - Mac��ַ��
/// @param[in]
///     macArr - ����Mac��ַ�����飬��С��Ϊ6
///
/// @return
///     
///
void CMacEditCtrl::Str2MacAddr(const CString &str, unsigned char *macArr)
{
	unsigned int k;
	wchar_t       c[3] = {0};

	for ( int i=0 ; i<MAC_STR_FIELD_NUM ;i++ )
	{
		c[0] = str[k_MacPos[i]];
		c[1] = str[k_MacPos[i]+1];
		swscanf_s(c,_T("%x"),&k);
		macArr[i] = (unsigned char)k;
	}
}

///
/// @brief
///     ��Mac��Ϣת��ΪMac��ַ��
///
/// @param[in]
///     macArr - ����Mac��ַ�����飬��С��Ϊ6
/// @param[in]
///     str    - Mac��ַ��
///
/// @return
///     
///
void CMacEditCtrl::MacAddr2Str(const unsigned char *macArr, CString &str)
{
	str.Format(MAC_FORMAT_STR,macArr[0],macArr[1],macArr[2],macArr[3],macArr[4],macArr[5]);
	str.MakeUpper();
}

///
/// @brief
///     ����ģ̬���ڱ༭Mac��ַ
///
/// @param[in]
///     pParentWnd - ָ�򸸴��ڵ�ָ��
/// @param[in]
///     rt         - ģ̬���ڵľ�����
/// @param[in]
///     macArr     - �����ʼMac��ַ�����飬��С��Ϊ6
///
/// @return
///     
///
int CMacEditCtrl::GoModal(CWnd *pParentWnd, const CRect &rt, unsigned char *macArr)
{
	if (::IsWindow(m_hWnd)) 
	{
		ASSERT(FALSE);
		return FALSE;
	}

	// ��������
	Create(pParentWnd,rt,macArr);
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
	CString str;
	GetWindowText(str);
	Str2MacAddr(str,macArr);

	ReleaseCapture();
	DestroyWindow();

	m_bGoModal = false;

	return (nResult == IDOK);
}

///
/// @brief
///     ��������ʾMac��ַ�༭�ؼ�
///
/// @param[in]
///     pParentWnd - ָ�򸸴��ڵ�ָ��
/// @param[in]
///     rt         - ģ̬���ڵľ�����
/// @param[in]
///     macArr     - �����ʼMac��ַ�����飬��С��Ϊ6
///
/// @return
///     
///
void  CMacEditCtrl::Create(CWnd *pParentWnd, const CRect &rt, unsigned char *macArr)
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
	CString str;
	MacAddr2Str(macArr,str);
	this->SetWindowText(str);
	this->SetWindowPos(0, rt.left, rt.top, rt.Width(), rt.Height(), 
		SWP_NOZORDER | SWP_SHOWWINDOW);

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
	this->SetMargins(0, 0);
	int nFontHeight = tm.tmHeight + tm.tmExternalLeading;
	int vMargin = (rcWnd.Height() - nFontHeight) / 2;
	rcWnd.DeflateRect(10, vMargin);
	this->SetRectNP(&rcWnd);

	// ��������ַ���
	SetLimitText(MAC_STR_SIZE);

	// ����ѡ�е��ַ�
	SetSel(k_MacPos[0],k_MacPos[0]+k_MacLen[0]);

	m_pParentWnd = pParentWnd;
	m_rtCtrlArea = rt;
	m_bCancel    = false;                                           // ��λ��־
	m_strPrevMac = str;
}

///
/// @brief
///     ����Inplace�༭����
///
void  CMacEditCtrl::HideWindow ()
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
void  CMacEditCtrl::ApplyInput ()
{
	if ( m_bGoModal )
	{
		EndModalLoop(IDOK);
	}
	else
	{
		// �򸸴��ڷ�����Ϣ
		::SendMessage(m_pParentWnd->m_hWnd,WM_MAC_EDIT_APPLY,0,0);
		// ���ٴ���
		DestroyWindow();
	}
}

///
/// @brief
///     �û�ȡ���޸�
///
void  CMacEditCtrl::CancelInput ()
{
	if ( m_bGoModal )
	{
		EndModalLoop(IDCANCEL);
	}
	else
	{
		// �򸸴��ڷ�����Ϣ
		::SendMessage(m_pParentWnd->m_hWnd,WM_MAC_EDIT_CANCLE,0,0);
		// ���ٴ���
		DestroyWindow();
	}
}

///
/// @brief
///     ��Inplace���ڶ�ʧ����ʱ, ȷ���û��Ѿ�����޸�
///
void  CMacEditCtrl::OnEnKillfocus ()
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
UINT  CMacEditCtrl::OnGetDlgCode ()
{
	return DLGC_WANTALLKEYS;
}

///
/// @brief
///     �ػ�ESC, �س��ȼ�, ȷ���û��޸����
///
void  CMacEditCtrl::OnChar (UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case VK_BACK:
		{
			KeyBackDown(nChar,nRepCnt,nFlags);
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

	KeyHexNumDown(nChar);
}

void  CMacEditCtrl::OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void  CMacEditCtrl::OnSysKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CEdit::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

void CMacEditCtrl::OnCaptureChanged(CWnd *pWnd)
{   
	if (m_hWnd)
	{
		m_bCancel = true;
		EndModalLoop(IDCANCEL);
	}
	
	CEdit::OnCaptureChanged(pWnd);   
}

BOOL CMacEditCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	return TRUE;
}

void CMacEditCtrl::OnPaint ()
{
#ifdef MAC_EDIT_DOUBLEBUFFER

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
BOOL  CMacEditCtrl::PreTranslateMessage (MSG* pMsg)
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
HBRUSH  CMacEditCtrl::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
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
void CMacEditCtrl::OnBtnBackspace()
{
	PostMessage(WM_CHAR, VK_BACK, 0);
}
void CMacEditCtrl::OnUpdateBtnBackspace(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(1);
}

///
/// @brief
///      ��Ӧ�ⲿ�˵�����, �����˵����Mac��ΪȫFF�İ�ťʱ, ��Mac��ΪȫFF
///
void CMacEditCtrl::OnBtnSetMacToAllFF()
{
	SetMacToAllFF();
}
void CMacEditCtrl::OnUpdateBtnSetMacToAllFF(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(1);
}

int CMacEditCtrl::GetFieldCurPos(const int pos)
{
	for ( int i = 1; i < MAC_STR_FIELD_NUM; i++ )
	{
		if ( pos < k_MacPos[i] )
		{
			return i-1;
		}
	}
	return (sizeof(k_MacPos)/sizeof(int)-1);
}

///
/// @brief
///     ����Mac��ַ�����������' '��Ϊ0
///
/// @param[in]
///     str - Mac��ַ��
///
/// @return
///     
///
void CMacEditCtrl::CorrectMacStr(CString &str)
{
	for ( int i = 0; i < MAC_STR_FIELD_NUM; i++ )
	{
		for ( int j = k_MacPos[i]; j < k_MacPos[i]+k_MacLen[i]; j++ )
		{
			if ( str[j] == ' ' )
			{
				CString s = str;
				str.Format(_T("%s%c%s"),s.Left(j),'0',s.Right(s.GetLength()-j-1));
			}
		}
	}
}

///
/// @brief
///     Backspace������Ӧ������������ֱ�ӷ���
///
void  CMacEditCtrl::KeyBackDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	int rightPos = HIWORD(GetSel());  // ��ȡѡ���ı��ұ߹������ڿؼ���λ��
	int fieldIdx = GetFieldCurPos(rightPos);
	if ( rightPos == k_MacPos[fieldIdx] )
		return;
	int leftPos = LOWORD(GetSel());
	CEdit::OnChar(nChar, nRepCnt, nFlags);
	SetSel(k_MacPos[fieldIdx],k_MacPos[fieldIdx]);
	if ( leftPos != rightPos )
	{
		// ɾ������ַ�
		for ( int i = 0; i < k_MacLen[fieldIdx]; i++ )
		{
			ReplaceSel(_T(" "));
		}

	}
	else
	{
		// ɾ��1���ַ�
		ReplaceSel(_T(" "));
	}
	// �ָ����λ��
	SetSel(rightPos,rightPos);
}

///
/// @brief
///     TAB������Ӧ������������ֱ�ӷ���
///
void  CMacEditCtrl::KeyTabDown()
{
	int rightPos = HIWORD(GetSel());  // ��ȡѡ���ı��ұ߹������ڿؼ���λ��
	int fieldIdx = GetFieldCurPos(rightPos);
	// ѡ����һ��ʱ����
	CString str;
	GetWindowText(str);
	fieldIdx++;
	if( fieldIdx > MAC_STR_FIELD_NUM-1 )
		fieldIdx = 0;
	int leftSel = k_MacPos[fieldIdx];
	for ( int i = k_MacPos[fieldIdx]; i < k_MacPos[fieldIdx]+k_MacLen[fieldIdx]; i++ )
	{
		if ( str[i] == ' ' )
			leftSel++;
	}
	SetSel(leftSel,k_MacPos[fieldIdx]+k_MacLen[fieldIdx]);
}

///
/// @brief
///     ESC������Ӧ������������ֱ�ӷ���
///
void  CMacEditCtrl::KeyEscapeDown()
{
	// �û�ȡ���޸�
	m_bCancel = true;
	// �ָ�Ϊԭʼ�ı�
	SetWindowText(m_strPrevMac);
	// ʹ��edit�ؼ���ʧ����, ����Killfocus, �ص�������
	GetParent()->SetFocus();
}

///
/// @brief
///     �س���������Ӧ
///
void  CMacEditCtrl::KeyReturnDown()
{
	// ���������' 'Ϊ'0'
	CString str;
	GetWindowText(str);
	CorrectMacStr(str);
	SetWindowText(str);
	// ʹ��edit�ؼ���ʧ����, ����Killfocus, �ص�������
	GetParent()->SetFocus();
}

///
/// @brief
///     ���ּ�������Ӧ
///
void  CMacEditCtrl::KeyHexNumDown(UINT nChar)
{
	if ( !( ( nChar >= '0' && nChar <= '9' ) || 
		    ( nChar >= 'A' && nChar <= 'F' ) ||
		    ( nChar >= 'a' && nChar <= 'f' ) ) )
		return;

	int leftPos  = LOWORD(GetSel());  // ��ȡѡ���ı���߹������ڿؼ���λ��
	int rightPos = HIWORD(GetSel());  // ��ȡѡ���ı��ұ߹������ڿؼ���λ��
	int fieldIdx = GetFieldCurPos(rightPos);
	CString macStr;
	GetWindowText(macStr);
	if ( leftPos != rightPos )
	{// ѡ�������ַ������
		// ��ѡ�е��ַ��滻Ϊ' '�������ַ������
		CString str1(_T("")),str2;
		for ( int i = 0; i < k_MacLen[fieldIdx]-1; i++ )
			str1.Append(_T(" "));
		str2.Format(_T("%s%c"),str1,nChar);
		str1.Format(_T("%s%s%s"),macStr.Left(k_MacPos[fieldIdx]),str2,
			macStr.Right(macStr.GetLength()-k_MacPos[fieldIdx]-k_MacLen[fieldIdx]));
		// ���½��
		str1.MakeUpper();
		SetWindowText(str1);
		SetSel(rightPos,rightPos);
	}
	else if ( macStr[k_MacPos[fieldIdx]] == ' ' && leftPos != 0 )
	{// δѡ���ַ����ұ༭�������' '�����
		CString str1,str2;
		// ��ȡ�༭���
		str1 = macStr.Mid(k_MacPos[fieldIdx],k_MacLen[fieldIdx]);
		str2.Format(_T("%s%c%s"),str1.Mid(1,leftPos-k_MacPos[fieldIdx]-1),nChar,
			str1.Right(str1.GetLength()-leftPos+k_MacPos[fieldIdx]));
		str1.Format(_T("%s%s%s"),macStr.Left(k_MacPos[fieldIdx]),str2,
			macStr.Right(macStr.GetLength()-k_MacPos[fieldIdx]-k_MacLen[fieldIdx]));
		// ���½��
		str1.MakeUpper();
		SetWindowText(str1);
		if ( str1[k_MacPos[fieldIdx]] != ' ' && fieldIdx < MAC_STR_FIELD_NUM-1 )
		{
			// ��ǰMac��༭��ɣ���������һ��
			fieldIdx++;
			int i = 0;
			while ( str1[k_MacPos[fieldIdx]+i] == ' ' && i < k_MacLen[fieldIdx] )
				i++;
			SetSel(k_MacPos[fieldIdx]+i,k_MacPos[fieldIdx]+k_MacLen[fieldIdx]);
		}
		else
		{
			SetSel(rightPos,rightPos);
		}
	}
	else if ( rightPos == k_MacPos[fieldIdx]+k_MacLen[fieldIdx] &&
		      macStr[k_MacPos[fieldIdx]] != ' ' && fieldIdx < MAC_STR_FIELD_NUM-1 )
	{
		// ��ǰMac��༭��ɣ���������һ��
		fieldIdx++;
		int i = 0;
		while ( macStr[k_MacPos[fieldIdx]+i] == ' ' && i < k_MacLen[fieldIdx] )
			i++;
		SetSel(k_MacPos[fieldIdx]+i,k_MacPos[fieldIdx]+k_MacLen[fieldIdx]);
	}
}

///
/// @brief
///     �����������Ӧ
///
void  CMacEditCtrl::KeyMoveLeft()
{
	int leftPos  = LOWORD(GetSel());  // ��ȡѡ���ı���߽�����ڿؼ���λ��
	int rightPos = HIWORD(GetSel());  // ��ȡѡ���ı��ұ߽�����ڿؼ���λ��
	int fieldIdx = GetFieldCurPos(rightPos);
	CString str;
	GetWindowText(str);
	
	if ( (rightPos != leftPos) ||
		 (rightPos != 0 && str[rightPos-1] == ' ') ||
		 (leftPos == k_MacPos[fieldIdx]) )
	{
		// ����ǰһ���ַ�Ϊ' '��λ��Ϊ��߽��Ϊѡ��״̬����������һ��
		if ( fieldIdx == 0 )
			fieldIdx = 5;
		else
			fieldIdx--;
		int i = 0;
		while ( str[k_MacPos[fieldIdx]+i] == ' ' && i < k_MacLen[fieldIdx] )
			i++;
		SetSel(k_MacPos[fieldIdx]+i,k_MacPos[fieldIdx]+k_MacLen[fieldIdx]);
//		SetSel(k_MacPos[fieldIdx],k_MacPos[fieldIdx]+k_MacLen[fieldIdx]);
	}
	else
	{
		// �������һ��
		rightPos--;
		SetSel(rightPos,rightPos);
	}
}

///
/// @brief
///     �ҷ����������Ӧ
///
void  CMacEditCtrl::KeyMoveRight()
{
	int rightPos = HIWORD(GetSel());  // ��ȡѡ���ı��ұ߹������ڿؼ���λ��
	int fieldIdx = GetFieldCurPos(rightPos);
	CString str;
	GetWindowText(str);

	if ( rightPos == k_MacPos[fieldIdx] + k_MacLen[fieldIdx] )
	{
		// ���Ϊ��ǰʱ������ұ߽磬��������һ��
		if ( fieldIdx == 5 )
			fieldIdx = 0;
		else
			fieldIdx++;
		int i = 0;
		while ( str[k_MacPos[fieldIdx]+i] == ' ' && i < k_MacLen[fieldIdx] )
			i++;
		SetSel(k_MacPos[fieldIdx]+i,k_MacPos[fieldIdx]+k_MacLen[fieldIdx]);
	}
	else
	{
		// �������һ��
		rightPos++;
		SetSel(rightPos,rightPos);
	}
}

///
/// @brief
///     �Ϸ����������Ӧ
///
void  CMacEditCtrl::KeyMoveUp()
{
	int rightPos = HIWORD(GetSel());  // ��ȡѡ���ı��ұ߹������ڿؼ���λ��
	int fieldIdx = GetFieldCurPos(rightPos);
	CString str;
	unsigned char macArr[6];

	// ��ȡ�༭���Mac��ַ������Ӧ�ֶ�ֵ+1
	GetWindowText(str);
	Str2MacAddr(str,macArr);
	macArr[fieldIdx]++;
	MacAddr2Str(macArr,str);
	// ���½��
	str.MakeUpper();
	SetWindowText(str);
	SetSel(k_MacPos[fieldIdx],k_MacPos[fieldIdx]+k_MacLen[fieldIdx]);

}

///
/// @brief
///     �·����������Ӧ
///
void  CMacEditCtrl::KeyMoveDown()
{
	int rightPos = HIWORD(GetSel());  // ��ȡѡ���ı��ұ߹������ڿؼ���λ��
	int fieldIdx = GetFieldCurPos(rightPos);
	CString str;
	unsigned char macArr[6];

	// ��ȡ�༭���Mac��ַ������Ӧ�ֶ�ֵ-1
	GetWindowText(str);
	Str2MacAddr(str,macArr);
	macArr[fieldIdx]--;
	MacAddr2Str(macArr,str);
	// ���½��
	str.MakeUpper();
	SetWindowText(str);
	SetSel(k_MacPos[fieldIdx],k_MacPos[fieldIdx]+k_MacLen[fieldIdx]);
}

///
/// @brief
///     ��Mac��ΪȫFF
///
void CMacEditCtrl::SetMacToAllFF()
{
	// ������ڻ�û�д���, �򷵻�
	if (!m_hWnd)
		return;

	// ���ÿؼ��ı�
	CString str;
	str.Format(MAC_FORMAT_STR,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF);
	str.MakeUpper();
	this->SetWindowText(str);
}
