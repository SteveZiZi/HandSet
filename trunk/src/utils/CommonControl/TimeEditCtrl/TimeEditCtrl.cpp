/// @file
///     TimeEditCtrl.cpp
///
/// @brief
///     ����ʱ��༭��Inplace�ؼ�
///
/// @note
///     �ؼ������ִ�����ʽ��
///     ֱ�Ӵ�����Create�� :ȷ��/ȡ���޸�ͨ����Ϣ����
///     ģ̬���ڣ�GoModal��:ֱ�ӷ��ر༭���
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.5.6
///
/// �汾��
///    1.0.x.x
/// 
/// �޸���ʷ��
///    �� ʱ��         : �汾      :  ��ϸ��Ϣ    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.05.07     |1.0.0.507  |�޸����Ұ�������Ӧʱʱ����ꡢ�¡��յȣ�֮����ת������  |
///    |2013.05.07     |1.0.0.507  |��ȡ����������Ӧ����Ϊ��������                            |
///    |2013.05.09     |1.0.0.509  |����OnPaint()��OnEraseBkgnd(),������˸��Bug               |
///    |2013.05.10     |1.0.0.510  |����ģ̬���ڷ�ʽ��ģʽ��־��λ���������������DestoryWindow��Bug|
///    |2013.05.14     |1.0.0.514  |��ʱ��������ʹ�CString��ΪSYSTEMTIME�����ṩCString��SYSTEMTIME֮���ת���ӿ�|
///    |2013.05.14     |1.0.0.514  |�޸��������Ӧ������ǰһ���ַ�Ϊ' '��λ��Ϊ��߽��Ϊѡ��״̬����������һ��|
///    |2013.05.23     |1.0.0.523  |�޸�OnUpdateBtnBackspace()��EnableΪ1�������ⲿ�˵�       |
///    |2013.07.02     |1.0.1.702  |�Ż��ַ����봦��ʹ�ؼ���ĳһ������༭������Զ���ת����һ��|
///    |2013.07.05     |1.0.1.705  |��Ӷ���ݵ����ƣ�Ӧ>=1970                                    |
///    |2013.07.09     |1.0.1.709  |�������������ɺ�ʼ��Ϊ1970��Bug���жϵ��ַ�������         |
///    |2013.08.23     |1.0.1.823  |�����º��ձ༭Ϊ" 0"���������Լ��س�������û������������Bug|
///
#include "stdafx.h"
#include "TimeEditCtrl.h"

///
/// @brief
///     ʱ�䴮�ĸ�ʽ���޶��ַ���
///
#define TIME_FORMAT_STR _T("%04d - %02d - %02d   %02d : %02d : %02d")
#define TIME_STR_SIZE 29

///
/// @brief
///     ʱ�䴮�и�ʱ��ֵ��ƫ�Ƶ�ַ������
///
const int k_TimePos[6] = {0,7,12,17,22,27};
const int k_TimeLen[6] = {4,2,2,2,2,2};

///
/// @brief
///     ���ؼ�����ʱ, ��Ӧ���˵�������ID: ɾ��һ���ַ�
///     ע��: ��ID��Ҫ���������ܶ����IDһ��, ����Ӧ���޸�
///
#define TIME_EDIT_CMD_BACKSPACE  32799

///
/// @brief
///     CTimeEditCtrl
///
BEGIN_MESSAGE_MAP(CTimeEditCtrl, CWnd)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnEnKillfocus)
	ON_WM_KEYDOWN()
	ON_WM_SYSKEYDOWN()
	ON_WM_GETDLGCODE()
	ON_WM_CHAR()
	ON_WM_CTLCOLOR_REFLECT()
	ON_COMMAND(TIME_EDIT_CMD_BACKSPACE, &CTimeEditCtrl::OnBtnBackspace)
	ON_UPDATE_COMMAND_UI(TIME_EDIT_CMD_BACKSPACE, &CTimeEditCtrl::OnUpdateBtnBackspace)
	ON_WM_CAPTURECHANGED()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

CTimeEditCtrl::CTimeEditCtrl ()
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


CTimeEditCtrl::~CTimeEditCtrl ()
{

}

///
/// @brief
///     ���ñ�����ɫ
///
void  CTimeEditCtrl::SetBkColor (COLORREF color)
{
	m_bkColor = color;

	m_bkBrush.DeleteObject();
	m_bkBrush.CreateSolidBrush(color);
}

///
/// @brief
///     ����������ɫ
///
void  CTimeEditCtrl::SetTextColor (COLORREF color)
{
	m_clrText = color;
}

///
/// @brief
///     ���ñ߿���ɫ
///
void  CTimeEditCtrl::SetBorderColor (COLORREF color)
{
	m_clrBorder = color;
}

///
/// @brief
///     �����������壬Ĭ��Ϊ����������
///
void  CTimeEditCtrl::SetTextFont(CFont* pFont)
{
	m_pTextFont = pFont;
}

///
/// @brief
///     ��ʱ�䴮ת��Ϊʱ����Ϣ
///
/// @param[in]
///     time - ʱ����Ϣ
/// @param[in]
///     str  - ʱ�䴮
///
/// @return
///     
///
void CTimeEditCtrl::Str2SysTime(const CString &str, SYSTEMTIME &time)
{
	time.wYear   = (WORD)_ttoi(str.Mid(k_TimePos[0],k_TimeLen[0]));
	time.wMonth  = (WORD)_ttoi(str.Mid(k_TimePos[1],k_TimeLen[1]));
	time.wDay    = (WORD)_ttoi(str.Mid(k_TimePos[2],k_TimeLen[2]));
	time.wHour   = (WORD)_ttoi(str.Mid(k_TimePos[3],k_TimeLen[3]));
	time.wMinute = (WORD)_ttoi(str.Mid(k_TimePos[4],k_TimeLen[4]));
	time.wSecond = (WORD)_ttoi(str.Mid(k_TimePos[5],k_TimeLen[5]));
	time.wDayOfWeek = -1;
	time.wMilliseconds = 0;
}

///
/// @brief
///     ��ʱ����Ϣת��Ϊʱ�䴮
///
/// @param[in]
///     time - ʱ����Ϣ
/// @param[in]
///     str  - ʱ�䴮
///
/// @return
///     
///
void CTimeEditCtrl::SysTime2Str(const SYSTEMTIME &time, CString &str)
{
	str.Format(TIME_FORMAT_STR,time.wYear,time.wMonth,time.wDay,time.wHour,time.wMinute,time.wSecond);
}

///
/// @brief
///     ����ģ̬���ڵ���ʱ��ֵ
///
/// @param[in]
///     pParentWnd - ָ�򸸴��ڵ�ָ��
/// @param[in]
///     rt         - ģ̬���ڵľ�����
/// @param[in]
///     time       - ʱ����Ϣ���������ó�ʼֵ�����ر༭���
///
/// @return
///     
///
int CTimeEditCtrl::GoModal(CWnd *pParentWnd, const CRect &rt, SYSTEMTIME &time)
{
	if (::IsWindow(m_hWnd)) 
	{
		ASSERT(FALSE);
		return FALSE;
	}

	// ��������
	Create(pParentWnd,rt,time);
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
	Str2SysTime(str,time);

	ReleaseCapture();
	DestroyWindow();

	m_bGoModal = false;

	return (nResult == IDOK);
}

///
/// @brief
///     ��������ʾʱ��༭���ؼ�
///
/// @param[in]
///     pParentWnd - ָ�򸸴��ڵ�ָ��
/// @param[in]
///     rt         - ģ̬���ڵľ�����
/// @param[in]
///     time       - ��ʼʱ��
///
/// @return
///     
///
void  CTimeEditCtrl::Create(CWnd *pParentWnd, const CRect &rt, const SYSTEMTIME &time)
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
	SysTime2Str(time,str);
	this->SetWindowText(str);
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
	rcWnd.DeflateRect(10, vMargin);
	this->SetRectNP(&rcWnd);

	// ��������ַ���
	SetLimitText(TIME_STR_SIZE);

	// ����ѡ�е��ַ�
	SetSel(k_TimePos[0],k_TimePos[0]+k_TimeLen[0]);

	m_pParentWnd = pParentWnd;
	m_rtCtrlArea = rt;
	m_bCancel    = false;                                           // ��λ��־
	m_tmPreTime  = time;
}

///
/// @brief
///     ����Inplace�༭����
///
void  CTimeEditCtrl::HideWindow ()
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
void  CTimeEditCtrl::ApplyInput ()
{
	if ( m_bGoModal )
	{
		EndModalLoop(IDOK);
	}
	else
	{
		// �򸸴��ڷ�����Ϣ
		::SendMessage(m_pParentWnd->m_hWnd,WM_TIME_EDIT_APPLY,0,0);
		// ���ٴ���
		DestroyWindow();
	}
}

///
/// @brief
///     �û�ȡ���޸�
///
void  CTimeEditCtrl::CancelInput ()
{
	if ( m_bGoModal )
	{
		EndModalLoop(IDCANCEL);
	}
	else
	{
		// �򸸴��ڷ�����Ϣ
		::SendMessage(m_pParentWnd->m_hWnd,WM_TIME_EDIT_CANCLE,0,0);
		// ���ٴ���
		DestroyWindow();
	}
}

///
/// @brief
///     ��Inplace���ڶ�ʧ����ʱ, ȷ���û��Ѿ�����޸�
///
void  CTimeEditCtrl::OnEnKillfocus ()
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
UINT  CTimeEditCtrl::OnGetDlgCode ()
{
	return DLGC_WANTALLKEYS;
}

///
/// @brief
///     �ػ�ESC, �س��ȼ�, ȷ���û��޸����
///
void  CTimeEditCtrl::OnChar (UINT nChar, UINT nRepCnt, UINT nFlags)
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

	if ( nChar >= '0' && nChar <= '9' )
	{
		KeyNumDown(nChar);
	}
}

void  CTimeEditCtrl::OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void  CTimeEditCtrl::OnSysKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CEdit::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

void CTimeEditCtrl::OnCaptureChanged(CWnd *pWnd)
{   
	if (m_hWnd)
	{
		m_bCancel = true;
		EndModalLoop(IDCANCEL);
	}

	CEdit::OnCaptureChanged(pWnd);   
}

BOOL CTimeEditCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	return TRUE;
}

void CTimeEditCtrl::OnPaint ()
{
#ifdef TIME_EDIT_DOUBLEBUFFER

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

#else  // TIME_EDIT_DOUBLEBUFFER

	CPaintDC dc(this);
	DefWindowProc(WM_PAINT, (WPARAM)dc.m_hDC, (LPARAM)0);

#endif  // TIME_EDIT_DOUBLEBUFFER
}

///
/// @brief
///     �ػ񰴼���Ϣ, �ø����ڲ����õ�ESC Enter������Ϣ
///
BOOL  CTimeEditCtrl::PreTranslateMessage (MSG* pMsg)
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
HBRUSH  CTimeEditCtrl::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
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
void CTimeEditCtrl::OnBtnBackspace()
{
	PostMessage(WM_CHAR, VK_BACK, 0);
}
void CTimeEditCtrl::OnUpdateBtnBackspace(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(1);
}

///
/// @brief
///     ��ȡ��ǰ������ڵ�ʱ���������
///
/// @param[in]
///     pos - ����ڿؼ��е�λ��
///
/// @return
///     0 - ��
///     1 - ��
///     2 - ��
///     3 - ʱ
///     4 - ��
///     5 - ��
///
int CTimeEditCtrl::GetTimeTypeInCursorPos(const int pos)
{
	for ( int i = 1; i < sizeof(k_TimePos)/sizeof(int); i++ )
	{
		if ( pos < k_TimePos[i] )
		{
			return i-1;
		}
	}
	return (sizeof(k_TimePos)/sizeof(int)-1);
}

///
/// @brief
///     ��ʱ�䴮ת��Ϊʱ������ֵ
///
/// @param[in]
///     timeStr - ʱ�䴮
/// @param[in]
///     timeArr - ����ʱ������ֵ������
/// @param[in]
///     arrSize - �����С
///
/// @return
///     
///
void CTimeEditCtrl::TimeStr2IntArr(const CString &timeStr, int *timeArr , const int arrSize)
{
	CString str;
	for ( int i = 0; i < arrSize; i++ )
	{
		if ( i >= sizeof(k_TimePos)/sizeof(int) )
			break;
		str = timeStr.Mid(k_TimePos[i],k_TimeLen[i]);
		timeArr[i] = _ttoi(str);
	}
}

///
/// @brief
///     �жϵ�ǰ�༭��ʱ�����Ƿ����
///
/// @param[in]
///     timeType - ��ǰ�༭��ʱ���������
/// @param[in]
///     timeArr  - ����ʱ������ֵ������
///
/// @return
///     TRUE - ���; FALSE - û���
///
BOOL CTimeEditCtrl::IsOverFlow(const int timeType, const int *timeArr)
{
	switch(timeType)
	{
	case 0:
		{
			if ( timeArr[0] < 0 || timeArr[0] > 9999 )
				return TRUE;
			else
				return FALSE;
		}
		break;
	case 1:
		{
			if ( timeArr[1] < 1 || timeArr[1] > 12 )
				return TRUE;
			else
				return FALSE;
		}
		break;
	case 2:
		{
			static int days[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
			int maxDay = 31;
			if ( timeArr[1] == 2 )
			{
				if ( ( timeArr[0]%4 == 0 && timeArr[0]%100 != 0 ) ||
					 timeArr[0]%400 == 0 )
					maxDay = 29;
				else
					maxDay = 28;
			}
			else
				maxDay = days[timeArr[1]-1];
			if ( timeArr[2] < 1 || timeArr[2] > maxDay )
				return TRUE;
			else
				return FALSE;
		}
		break;
	case 3:
		{
			if ( timeArr[3] < 0 || timeArr[3] > 23 )
				return TRUE;
			else
				return FALSE;
		}
		break;
	case 4:
		{
			if ( timeArr[4] < 0 || timeArr[4] > 59 )
				return TRUE;
			else
				return FALSE;
		}
		break;
	case 5:
		{
			if ( timeArr[5] < 0 || timeArr[5] > 59 )
				return TRUE;
			else
				return FALSE;
		}
		break;
	default:
		return TRUE;
	}
}

///
/// @brief
///     ����ʱ�䴮���������' '��Ϊ0
///
/// @param[in]
///     str - ʱ�䴮
///
/// @return
///     
///
void CTimeEditCtrl::CorrectTimeStr(CString &str)
{
	for ( int i = 0; i < sizeof(k_TimePos)/sizeof(int); i++ )
	{
		for ( int j = k_TimePos[i]; j < k_TimePos[i]+k_TimeLen[i]; j++ )
		{
			if ( str[j] == ' ' )
			{
				CString s =str;
				str.Format(_T("%s%c%s"),s.Left(j),'0',s.Right(s.GetLength()-j-1));
			}
		}
	}
}

///
/// @brief
///     ������ݣ�2000~2099��������
///
/// @param[in]
///     str - ʱ�䴮
///
/// @return
///     
///
void  CTimeEditCtrl::CorrectYear(CString &str)
{
	// ������ݣ�2000~2099
	int year = _ttoi(str.Mid(k_TimePos[0],k_TimeLen[0]));
	if ( year < 2000 )
	{
		CString s =str;
		str.Format(_T("%s%s"),_T("2000"),s.Right(s.GetLength()-k_TimeLen[0]));
		SetWindowText(str);
	}
	else if ( year > 2099 )
	{
		CString s =str;
		str.Format(_T("%s%s"),_T("2099"),s.Right(s.GetLength()-k_TimeLen[0]));
		SetWindowText(str);
	}
}

///
/// @brief
///     Backspace������Ӧ������������ֱ�ӷ���
///
void  CTimeEditCtrl::KeyBackDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	int rightPos = HIWORD(GetSel());  // ��ȡѡ���ı��ұ߹������ڿؼ���λ��
	int timeType = GetTimeTypeInCursorPos(rightPos);
	if ( rightPos == k_TimePos[timeType] )
		return;
	int leftPos = LOWORD(GetSel());
	CEdit::OnChar(nChar, nRepCnt, nFlags);
	SetSel(k_TimePos[timeType],k_TimePos[timeType]);
	if ( leftPos != rightPos )
	{
		// ɾ������ַ�
		for ( int i = 0; i < k_TimeLen[timeType]; i++ )
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
void  CTimeEditCtrl::KeyTabDown()
{
	int rightPos = HIWORD(GetSel());  // ��ȡѡ���ı��ұ߹������ڿؼ���λ��
	int timeType = GetTimeTypeInCursorPos(rightPos);
	// ѡ����һ��ʱ����
	CString timeStr;
	GetWindowText(timeStr);

	// ������ݣ�2000~2099
	if( timeType == 0 )
	{
		CorrectYear(timeStr);
	}

	timeType++;
	if( timeType >5 )
		timeType = 0;
	int leftSel = k_TimePos[timeType];
	for ( int i = k_TimePos[timeType]; i < k_TimePos[timeType]+k_TimeLen[timeType]; i++ )
	{
		if ( timeStr[i] == ' ' )
			leftSel++;
	}
	SetSel(leftSel,k_TimePos[timeType]+k_TimeLen[timeType]);
}

///
/// @brief
///     ESC������Ӧ������������ֱ�ӷ���
///
void  CTimeEditCtrl::KeyEscapeDown()
{
	// �û�ȡ���޸�
	m_bCancel = true;
	// �ָ�Ϊԭʼ�ı�
	CString str;
	str.Format(TIME_FORMAT_STR,m_tmPreTime.wYear,m_tmPreTime.wMonth,m_tmPreTime.wDay,
		m_tmPreTime.wHour,m_tmPreTime.wMinute,m_tmPreTime.wSecond);
	SetWindowText(str);
	// ʹ��edit�ؼ���ʧ����, ����Killfocus, �ص�������
	GetParent()->SetFocus();
}

///
/// @brief
///     �س���������Ӧ
///
void  CTimeEditCtrl::KeyReturnDown()
{
	// ���������' 'Ϊ'0'
	CString timeStr;
	GetWindowText(timeStr);
	CorrectTimeStr(timeStr);
	CorrectYear(timeStr);

	// �·ݻ����������
	int timeArr[6];
	TimeStr2IntArr(timeStr,timeArr,6);
	for ( int timeType = 1 ; timeType < 3; timeType++ )
	{
		if ( IsOverFlow(timeType,timeArr) )
		{
			CString str = timeStr;
			str.Format(_T("%s%02d%s"),timeStr.Left(k_TimePos[timeType]),1,
				timeStr.Right(timeStr.GetLength()-k_TimePos[timeType]-k_TimeLen[timeType]));
			timeStr = str;
		}
	}

	SetWindowText(timeStr);
	// ʹ��edit�ؼ���ʧ����, ����Killfocus, �ص�������
	GetParent()->SetFocus();
}

///
/// @brief
///     ���ּ�������Ӧ
///
void  CTimeEditCtrl::KeyNumDown(UINT nChar)
{
	if ( nChar < '0' && nChar > '9' )
		return;

	int leftPos  = LOWORD(GetSel());  // ��ȡѡ���ı���߹������ڿؼ���λ��
	int rightPos = HIWORD(GetSel());  // ��ȡѡ���ı��ұ߹������ڿؼ���λ��
	int timeType = GetTimeTypeInCursorPos(rightPos);
	CString timeStr;
	GetWindowText(timeStr);
	if ( leftPos != rightPos )
	{// ѡ�������ַ������
		// ��ѡ�е��ַ��滻Ϊ' '�������ַ������
		CString str1(_T("")),str2;
		for ( int i = 0; i < k_TimeLen[timeType]-1; i++ )
			str1.Append(_T(" "));
		str2.Format(_T("%s%c"),str1,nChar);
 		str1.Format(_T("%s%s%s"),timeStr.Left(k_TimePos[timeType]),str2,
 			timeStr.Right(timeStr.GetLength()-k_TimePos[timeType]-k_TimeLen[timeType]));
		SetWindowText(str1);
		SetSel(rightPos,rightPos);
	}
	else if ( timeStr[k_TimePos[timeType]] == ' ' && leftPos != 0 )
	{// δѡ���ַ�����ʱ��ֵ�༭�������' '�����
		CString str1,str2;
		// ��ȡ�༭���
		str1 = timeStr.Mid(k_TimePos[timeType],k_TimeLen[timeType]);
		str2.Format(_T("%s%c%s"),str1.Mid(1,leftPos-k_TimePos[timeType]-1),nChar,
			str1.Right(str1.GetLength()-leftPos+k_TimePos[timeType]));
		str1.Format(_T("%s%s%s"),timeStr.Left(k_TimePos[timeType]),str2,
			timeStr.Right(timeStr.GetLength()-k_TimePos[timeType]-k_TimeLen[timeType]));
		// �жϱ༭���ʱ��ֵ�Ƿ���������򷵻�
		int timeArr[6];
		TimeStr2IntArr(str1,timeArr,6);
		if ( IsOverFlow(timeType,timeArr) == TRUE )
			return;
		// ����ǰ�༭��Ϊ�·ݣ������ж϶�Ӧ�·ݵ�dayֵ�Ƿ�������������1
		if ( timeType == 1 && IsOverFlow(2,timeArr) == TRUE )
		{
			str2 = str1;
			str1.Format(_T("%s%02d%s"),str2.Left(k_TimePos[2]),1,
				str2.Right(str2.GetLength()-k_TimePos[2]-k_TimeLen[2]));
		}
		// ���½��
		SetWindowText(str1);
		if ( str1[k_TimePos[timeType]] != ' ' && timeType < 5 )
		{
			// ������ݣ�2000~2099
			if( timeType == 0 )
			{
				CorrectYear(str1);
			}
			// ��ǰʱ����༭��ɣ���������һ��
			timeType++;
			int i = 0;
			while ( str1[k_TimePos[timeType]+i] == ' ' && i < k_TimeLen[timeType] )
				i++;
			SetSel(k_TimePos[timeType]+i,k_TimePos[timeType]+k_TimeLen[timeType]);
		}
		else
		{
			SetSel(rightPos,rightPos);
		}
	}
	else if ( rightPos == k_TimePos[timeType]+k_TimeLen[timeType] &&
		      timeStr[k_TimePos[timeType]] != ' ' && timeType < 5 )
	{
		// ������ݣ�2000~2099
		if( timeType == 0 )
		{
			CorrectYear(timeStr);
		}

		// ��ǰʱ����༭��ɣ���������һ��
		timeType++;
		int i = 0;
		while ( timeStr[k_TimePos[timeType]+i] == ' ' && i < k_TimeLen[timeType] )
			i++;
		SetSel(k_TimePos[timeType]+i,k_TimePos[timeType]+k_TimeLen[timeType]);
	}
}

///
/// @brief
///     �����������Ӧ
///
void  CTimeEditCtrl::KeyMoveLeft()
{
	int leftPos  = LOWORD(GetSel());  // ��ȡѡ���ı���߽�����ڿؼ���λ��
	int rightPos = HIWORD(GetSel());  // ��ȡѡ���ı��ұ߽�����ڿؼ���λ��
	int timeType = GetTimeTypeInCursorPos(rightPos);
	CString timeStr;
	GetWindowText(timeStr);
	
	if ( (rightPos != leftPos) ||
		 (rightPos != 0 && timeStr[rightPos-1] == ' ') ||
		 (leftPos == k_TimePos[timeType]) )
	{
		// ����ǰһ���ַ�Ϊ' '��λ��Ϊ��߽��Ϊѡ��״̬����������һ��

		// �·ݻ����������
		if ( timeType == 1 || timeType == 2 )
		{
			int timeArr[6];
			TimeStr2IntArr(timeStr,timeArr,6);
			if ( IsOverFlow(timeType,timeArr) )
			{
				CString str = timeStr;
				str.Format(_T("%s%02d%s"),timeStr.Left(k_TimePos[timeType]),1,
					timeStr.Right(timeStr.GetLength()-k_TimePos[timeType]-k_TimeLen[timeType]));
				SetWindowText(str);
			}
		}
		if ( timeType == 0 )
		{
			// ������ݣ�2000~2099
			CorrectYear(timeStr);
			timeType = 5;
		}
		else
			timeType--;
		int i = 0;
		while ( timeStr[k_TimePos[timeType]+i] == ' ' && i < k_TimeLen[timeType] )
			i++;
		SetSel(k_TimePos[timeType]+i,k_TimePos[timeType]+k_TimeLen[timeType]);
//		SetSel(k_TimePos[timeType],k_TimePos[timeType]+k_TimeLen[timeType]);
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
void  CTimeEditCtrl::KeyMoveRight()
{
	int rightPos = HIWORD(GetSel());  // ��ȡѡ���ı��ұ߹������ڿؼ���λ��
	int timeType = GetTimeTypeInCursorPos(rightPos);
	CString timeStr;
	GetWindowText(timeStr);

	if ( rightPos == k_TimePos[timeType] + k_TimeLen[timeType] )
	{
		// ���Ϊ��ǰʱ������ұ߽磬��������һ��

		// �·ݻ����������
		if ( timeType == 1 || timeType == 2 )
		{
			int timeArr[6];
			TimeStr2IntArr(timeStr,timeArr,6);
			if ( IsOverFlow(timeType,timeArr) )
			{
				CString str = timeStr;
				str.Format(_T("%s%02d%s"),timeStr.Left(k_TimePos[timeType]),1,
					timeStr.Right(timeStr.GetLength()-k_TimePos[timeType]-k_TimeLen[timeType]));
				SetWindowText(str);
			}
		}

		if ( timeType == 5 )
		{
			timeType = 0;
		}
		else
		{
			// ������ݣ�2000~2099
			if( timeType == 0 )
			{
				CorrectYear(timeStr);
			}
			timeType++;
		}
		int i = 0;
		while ( timeStr[k_TimePos[timeType]+i] == ' ' && i < k_TimeLen[timeType] )
			i++;
		SetSel(k_TimePos[timeType]+i,k_TimePos[timeType]+k_TimeLen[timeType]);
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
void  CTimeEditCtrl::KeyMoveUp()
{
	int rightPos = HIWORD(GetSel());  // ��ȡѡ���ı��ұ߹������ڿؼ���λ��
	int timeType = GetTimeTypeInCursorPos(rightPos);
	CString timeStr;
	int timeArr[6];

	// ��ȡ�༭���ʱ��ֵ
	GetWindowText(timeStr);
	TimeStr2IntArr(timeStr,timeArr,6);
	timeArr[timeType]++;
	timeStr.Format(TIME_FORMAT_STR,timeArr[0],timeArr[1],timeArr[2],timeArr[3],timeArr[4],timeArr[5]);
	// �ж����
	if ( IsOverFlow(timeType,timeArr) == TRUE )
		return;
	if ( timeType == 1 && IsOverFlow(2,timeArr) == TRUE )
	{
		CString str = timeStr;
		timeStr.Format(_T("%s%02d%s"),str.Left(k_TimePos[2]),1,
			str.Right(str.GetLength()-k_TimePos[2]-k_TimeLen[2]));
	}
	// ���½��
	SetWindowText(timeStr);
	SetSel(k_TimePos[timeType],k_TimePos[timeType]+k_TimeLen[timeType]);

}

///
/// @brief
///     �·����������Ӧ
///
void  CTimeEditCtrl::KeyMoveDown()
{
	int rightPos = HIWORD(GetSel());  // ��ȡѡ���ı��ұ߹������ڿؼ���λ��
	int timeType = GetTimeTypeInCursorPos(rightPos);
	CString timeStr;
	int timeArr[6];

	// ��ȡ�༭���ʱ��ֵ
	GetWindowText(timeStr);
	TimeStr2IntArr(timeStr,timeArr,6);
	timeArr[timeType]--;
	timeStr.Format(TIME_FORMAT_STR,timeArr[0],timeArr[1],timeArr[2],timeArr[3],timeArr[4],timeArr[5]);
	// �ж����
	if ( IsOverFlow(timeType,timeArr) == TRUE )
		return;
	if ( timeType == 1 && IsOverFlow(2,timeArr) == TRUE )
	{
		CString str = timeStr;
		timeStr.Format(_T("%s%02d%s"),str.Left(k_TimePos[2]),1,
			str.Right(str.GetLength()-k_TimePos[2]-k_TimeLen[2]));
	}
	// ���½��
	SetWindowText(timeStr);
	SetSel(k_TimePos[timeType],k_TimePos[timeType]+k_TimeLen[timeType]);
}
