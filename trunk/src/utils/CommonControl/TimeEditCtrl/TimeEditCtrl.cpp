/// @file
///     TimeEditCtrl.cpp
///
/// @brief
///     日期时间编辑器Inplace控件
///
/// @note
///     控件有两种创建方式：
///     直接创建（Create） :确认/取消修改通过消息发送
///     模态窗口（GoModal）:直接返回编辑结果
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.5.6
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.05.07     |1.0.0.507  |修改左右按键的响应时时间项（年、月、日等）之间跳转的条件  |
///    |2013.05.07     |1.0.0.507  |提取各按键的响应操作为单个函数                            |
///    |2013.05.09     |1.0.0.509  |重载OnPaint()和OnEraseBkgnd(),修正闪烁的Bug               |
///    |2013.05.10     |1.0.0.510  |修正模态窗口方式下模式标志复位错误造成连续两次DestoryWindow的Bug|
///    |2013.05.14     |1.0.0.514  |将时间参数类型从CString改为SYSTEMTIME，并提供CString和SYSTEMTIME之间的转换接口|
///    |2013.05.14     |1.0.0.514  |修改左方向键响应：光标的前一个字符为' '、位置为左边界或为选中状态，则跳到上一项|
///    |2013.05.23     |1.0.0.523  |修改OnUpdateBtnBackspace()的Enable为1，避免外部菜单       |
///    |2013.07.02     |1.0.1.702  |优化字符输入处理，使控件中某一项参数编辑完成则自动跳转到下一项|
///    |2013.07.05     |1.0.1.705  |添加对年份的限制，应>=1970                                    |
///    |2013.07.09     |1.0.1.709  |修正年份输入完成后始终为1970的Bug（判断的字符串错误）         |
///    |2013.08.23     |1.0.1.823  |修正月和日编辑为" 0"后，左右移以及回车按键后没有作溢出处理的Bug|
///
#include "stdafx.h"
#include "TimeEditCtrl.h"

///
/// @brief
///     时间串的格式及限定字符数
///
#define TIME_FORMAT_STR _T("%04d - %02d - %02d   %02d : %02d : %02d")
#define TIME_STR_SIZE 29

///
/// @brief
///     时间串中各时间值的偏移地址及长度
///
const int k_TimePos[6] = {0,7,12,17,22,27};
const int k_TimeLen[6] = {4,2,2,2,2,2};

///
/// @brief
///     当控件弹出时, 响应主菜单的命令ID: 删除一个字符
///     注意: 本ID需要与主程序框架定义的ID一致, 否则应该修改
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
///     设置背景颜色
///
void  CTimeEditCtrl::SetBkColor (COLORREF color)
{
	m_bkColor = color;

	m_bkBrush.DeleteObject();
	m_bkBrush.CreateSolidBrush(color);
}

///
/// @brief
///     设置文字颜色
///
void  CTimeEditCtrl::SetTextColor (COLORREF color)
{
	m_clrText = color;
}

///
/// @brief
///     设置边框颜色
///
void  CTimeEditCtrl::SetBorderColor (COLORREF color)
{
	m_clrBorder = color;
}

///
/// @brief
///     设置文字字体，默认为父窗口字体
///
void  CTimeEditCtrl::SetTextFont(CFont* pFont)
{
	m_pTextFont = pFont;
}

///
/// @brief
///     将时间串转换为时间信息
///
/// @param[in]
///     time - 时间信息
/// @param[in]
///     str  - 时间串
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
///     将时间信息转换为时间串
///
/// @param[in]
///     time - 时间信息
/// @param[in]
///     str  - 时间串
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
///     弹出模态窗口调整时间值
///
/// @param[in]
///     pParentWnd - 指向父窗口的指针
/// @param[in]
///     rt         - 模态窗口的矩形域
/// @param[in]
///     time       - 时间信息，用于设置初始值及返回编辑结果
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

	// 创建窗口
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

	// 获取编辑结果
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
///     创建并显示时间编辑器控件
///
/// @param[in]
///     pParentWnd - 指向父窗口的指针
/// @param[in]
///     rt         - 模态窗口的矩形域
/// @param[in]
///     time       - 初始时间
///
/// @return
///     
///
void  CTimeEditCtrl::Create(CWnd *pParentWnd, const CRect &rt, const SYSTEMTIME &time)
{
	ASSERT(pParentWnd);

	// 如果窗口还没有创建, 则先创建窗口
	if (!m_hWnd)
	{
		DWORD  dwEditStyle = WS_CHILD | WS_VISIBLE | /*WS_BORDER |*/ ES_LEFT | 
			ES_AUTOHSCROLL | ES_MULTILINE;
		CEdit::Create(dwEditStyle, rt, pParentWnd, 0);
	}

	// 显示窗口并调整窗口位置
	this->EnableWindow(TRUE);
	this->SetFocus();

	// 未设置则默认为父窗口的字体
	if ( m_pTextFont != NULL )
		this->SetFont(m_pTextFont);
	else
		this->SetFont(pParentWnd->GetFont());

	// 设置控件文本
	CString str;
	SysTime2Str(time,str);
	this->SetWindowText(str);
	this->SetWindowPos(0, rt.left, rt.top, rt.Width(), rt.Height(), 
		SWP_NOZORDER | SWP_SHOWWINDOW);
	this->SetMargins(0, 0);

	// 获取控件文本信息
	CRect  rcWnd;
	this->GetClientRect(&rcWnd);
	CDC*  pDC = this->GetDC();
	CFont*  oldfont = pDC->SelectObject(GetFont());
	TEXTMETRIC  tm;
	pDC->GetTextMetrics(&tm);
	pDC->SelectObject(oldfont);
	this->ReleaseDC(pDC);                                           // 记得释放

	// 使得文本垂直居中, 注意: 必须ES_MULTILINE 样式
	int nFontHeight = tm.tmHeight + tm.tmExternalLeading;
	int vMargin = (rcWnd.Height() - nFontHeight) / 2;
	rcWnd.DeflateRect(10, vMargin);
	this->SetRectNP(&rcWnd);

	// 设置最大字符数
	SetLimitText(TIME_STR_SIZE);

	// 设置选中的字符
	SetSel(k_TimePos[0],k_TimePos[0]+k_TimeLen[0]);

	m_pParentWnd = pParentWnd;
	m_rtCtrlArea = rt;
	m_bCancel    = false;                                           // 复位标志
	m_tmPreTime  = time;
}

///
/// @brief
///     隐藏Inplace编辑窗口
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
///     确认用户输入修改, 通知父窗口
///
void  CTimeEditCtrl::ApplyInput ()
{
	if ( m_bGoModal )
	{
		EndModalLoop(IDOK);
	}
	else
	{
		// 向父窗口发送消息
		::SendMessage(m_pParentWnd->m_hWnd,WM_TIME_EDIT_APPLY,0,0);
		// 销毁窗口
		DestroyWindow();
	}
}

///
/// @brief
///     用户取消修改
///
void  CTimeEditCtrl::CancelInput ()
{
	if ( m_bGoModal )
	{
		EndModalLoop(IDCANCEL);
	}
	else
	{
		// 向父窗口发送消息
		::SendMessage(m_pParentWnd->m_hWnd,WM_TIME_EDIT_CANCLE,0,0);
		// 销毁窗口
		DestroyWindow();
	}
}

///
/// @brief
///     当Inplace窗口丢失焦点时, 确认用户已经完成修改
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
///     使控件能够自己处理输入的箭头键和TAB键
///
UINT  CTimeEditCtrl::OnGetDlgCode ()
{
	return DLGC_WANTALLKEYS;
}

///
/// @brief
///     截获ESC, 回车等键, 确认用户修改完成
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
	// TODO: 在此添加消息处理程序代码和/或调用默认值

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

	// 绘制自定义的边框
	CPen  pen(PS_SOLID, 2, m_clrBorder);
	CPen*  oldpen = MenDC.SelectObject(&pen);
	MenDC.SelectStockObject(NULL_BRUSH);
	{
		CRect  rcFrame = clientRect;
		rcFrame.DeflateRect(1, 1);
		MenDC.Rectangle(rcFrame);
	}
	MenDC.SelectObject(oldpen);

	// 这一句是调用默认的OnPaint(),把图形画在内存DC表上
	DefWindowProc(WM_PAINT, (WPARAM)MenDC.m_hDC, (LPARAM)0);

	//输出
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
///     截获按键消息, 让父窗口不能拿到ESC Enter按键消息
///
BOOL  CTimeEditCtrl::PreTranslateMessage (MSG* pMsg)
{
	// 直接把关键按键处理掉, 不需要windows消息循环去DispatchMessage 
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
				// 禁用DELETE
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
///     修改控件颜色
///
HBRUSH  CTimeEditCtrl::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	// TODO:  在此更改 DC 的任何属性

	pDC->SetTextColor(m_clrText);               // 文本颜色
	pDC->SetBkColor(m_bkColor);                 // 文本背景色
	//pDC->SetBkMode(TRANSPARENT);

	// TODO:  如果不应调用父级的处理程序，则返回非 null 画笔
	return m_bkBrush;                           // 控件整体背景色
}

///
/// @brief
///      响应外部菜单命令, 当主菜单点击删除按钮时, 删除一个字符
///      替代键盘上的Backspace
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
///     获取当前光标所在的时间项的类型
///
/// @param[in]
///     pos - 光标在控件中的位置
///
/// @return
///     0 - 年
///     1 - 月
///     2 - 日
///     3 - 时
///     4 - 分
///     5 - 秒
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
///     将时间串转换为时间整数值
///
/// @param[in]
///     timeStr - 时间串
/// @param[in]
///     timeArr - 保存时间整数值的数组
/// @param[in]
///     arrSize - 数组大小
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
///     判断当前编辑的时间项是否溢出
///
/// @param[in]
///     timeType - 当前编辑的时间项的类型
/// @param[in]
///     timeArr  - 保存时间整数值的数组
///
/// @return
///     TRUE - 溢出; FALSE - 没溢出
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
///     修正时间串，将多余的' '置为0
///
/// @param[in]
///     str - 时间串
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
///     限制年份：2000~2099，并更新
///
/// @param[in]
///     str - 时间串
///
/// @return
///     
///
void  CTimeEditCtrl::CorrectYear(CString &str)
{
	// 限制年份：2000~2099
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
///     Backspace按键响应，操作结束后直接返回
///
void  CTimeEditCtrl::KeyBackDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	int rightPos = HIWORD(GetSel());  // 获取选中文本右边光标相对于控件的位置
	int timeType = GetTimeTypeInCursorPos(rightPos);
	if ( rightPos == k_TimePos[timeType] )
		return;
	int leftPos = LOWORD(GetSel());
	CEdit::OnChar(nChar, nRepCnt, nFlags);
	SetSel(k_TimePos[timeType],k_TimePos[timeType]);
	if ( leftPos != rightPos )
	{
		// 删除多个字符
		for ( int i = 0; i < k_TimeLen[timeType]; i++ )
		{
			ReplaceSel(_T(" "));
		}

	}
	else
	{
		// 删除1个字符
		ReplaceSel(_T(" "));
	}
	// 恢复光标位置
	SetSel(rightPos,rightPos);
}

///
/// @brief
///     TAB按键响应，操作结束后直接返回
///
void  CTimeEditCtrl::KeyTabDown()
{
	int rightPos = HIWORD(GetSel());  // 获取选中文本右边光标相对于控件的位置
	int timeType = GetTimeTypeInCursorPos(rightPos);
	// 选中下一项时间项
	CString timeStr;
	GetWindowText(timeStr);

	// 限制年份：2000~2099
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
///     ESC按键响应，操作结束后直接返回
///
void  CTimeEditCtrl::KeyEscapeDown()
{
	// 用户取消修改
	m_bCancel = true;
	// 恢复为原始文本
	CString str;
	str.Format(TIME_FORMAT_STR,m_tmPreTime.wYear,m_tmPreTime.wMonth,m_tmPreTime.wDay,
		m_tmPreTime.wHour,m_tmPreTime.wMinute,m_tmPreTime.wSecond);
	SetWindowText(str);
	// 使得edit控件丢失焦点, 触发Killfocus, 回到父窗口
	GetParent()->SetFocus();
}

///
/// @brief
///     回车键按键响应
///
void  CTimeEditCtrl::KeyReturnDown()
{
	// 修正多余的' '为'0'
	CString timeStr;
	GetWindowText(timeStr);
	CorrectTimeStr(timeStr);
	CorrectYear(timeStr);

	// 月份或日溢出处理
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
	// 使得edit控件丢失焦点, 触发Killfocus, 回到父窗口
	GetParent()->SetFocus();
}

///
/// @brief
///     数字键按键响应
///
void  CTimeEditCtrl::KeyNumDown(UINT nChar)
{
	if ( nChar < '0' && nChar > '9' )
		return;

	int leftPos  = LOWORD(GetSel());  // 获取选中文本左边光标相对于控件的位置
	int rightPos = HIWORD(GetSel());  // 获取选中文本右边光标相对于控件的位置
	int timeType = GetTimeTypeInCursorPos(rightPos);
	CString timeStr;
	GetWindowText(timeStr);
	if ( leftPos != rightPos )
	{// 选中若干字符的情况
		// 将选中的字符替换为' '和输入字符的组合
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
	{// 未选中字符，且时间值编辑区域存在' '的情况
		CString str1,str2;
		// 获取编辑结果
		str1 = timeStr.Mid(k_TimePos[timeType],k_TimeLen[timeType]);
		str2.Format(_T("%s%c%s"),str1.Mid(1,leftPos-k_TimePos[timeType]-1),nChar,
			str1.Right(str1.GetLength()-leftPos+k_TimePos[timeType]));
		str1.Format(_T("%s%s%s"),timeStr.Left(k_TimePos[timeType]),str2,
			timeStr.Right(timeStr.GetLength()-k_TimePos[timeType]-k_TimeLen[timeType]));
		// 判断编辑后的时间值是否溢出，是则返回
		int timeArr[6];
		TimeStr2IntArr(str1,timeArr,6);
		if ( IsOverFlow(timeType,timeArr) == TRUE )
			return;
		// 若当前编辑项为月份，则需判断对应月份的day值是否会溢出，是则置1
		if ( timeType == 1 && IsOverFlow(2,timeArr) == TRUE )
		{
			str2 = str1;
			str1.Format(_T("%s%02d%s"),str2.Left(k_TimePos[2]),1,
				str2.Right(str2.GetLength()-k_TimePos[2]-k_TimeLen[2]));
		}
		// 更新结果
		SetWindowText(str1);
		if ( str1[k_TimePos[timeType]] != ' ' && timeType < 5 )
		{
			// 限制年份：2000~2099
			if( timeType == 0 )
			{
				CorrectYear(str1);
			}
			// 当前时间项编辑完成，则跳到下一项
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
		// 限制年份：2000~2099
		if( timeType == 0 )
		{
			CorrectYear(timeStr);
		}

		// 当前时间项编辑完成，则跳到下一项
		timeType++;
		int i = 0;
		while ( timeStr[k_TimePos[timeType]+i] == ' ' && i < k_TimeLen[timeType] )
			i++;
		SetSel(k_TimePos[timeType]+i,k_TimePos[timeType]+k_TimeLen[timeType]);
	}
}

///
/// @brief
///     左方向键按键响应
///
void  CTimeEditCtrl::KeyMoveLeft()
{
	int leftPos  = LOWORD(GetSel());  // 获取选中文本左边界相对于控件的位置
	int rightPos = HIWORD(GetSel());  // 获取选中文本右边界相对于控件的位置
	int timeType = GetTimeTypeInCursorPos(rightPos);
	CString timeStr;
	GetWindowText(timeStr);
	
	if ( (rightPos != leftPos) ||
		 (rightPos != 0 && timeStr[rightPos-1] == ' ') ||
		 (leftPos == k_TimePos[timeType]) )
	{
		// 光标的前一个字符为' '、位置为左边界或为选中状态，则跳到上一项

		// 月份或日溢出处理
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
			// 限制年份：2000~2099
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
		// 光标左移一个
		rightPos--;
		SetSel(rightPos,rightPos);
	}
}

///
/// @brief
///     右方向键按键响应
///
void  CTimeEditCtrl::KeyMoveRight()
{
	int rightPos = HIWORD(GetSel());  // 获取选中文本右边光标相对于控件的位置
	int timeType = GetTimeTypeInCursorPos(rightPos);
	CString timeStr;
	GetWindowText(timeStr);

	if ( rightPos == k_TimePos[timeType] + k_TimeLen[timeType] )
	{
		// 光标为当前时间项的右边界，则跳到下一项

		// 月份或日溢出处理
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
			// 限制年份：2000~2099
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
		// 光标右移一个
		rightPos++;
		SetSel(rightPos,rightPos);
	}
}

///
/// @brief
///     上方向键按键响应
///
void  CTimeEditCtrl::KeyMoveUp()
{
	int rightPos = HIWORD(GetSel());  // 获取选中文本右边光标相对于控件的位置
	int timeType = GetTimeTypeInCursorPos(rightPos);
	CString timeStr;
	int timeArr[6];

	// 获取编辑后的时间值
	GetWindowText(timeStr);
	TimeStr2IntArr(timeStr,timeArr,6);
	timeArr[timeType]++;
	timeStr.Format(TIME_FORMAT_STR,timeArr[0],timeArr[1],timeArr[2],timeArr[3],timeArr[4],timeArr[5]);
	// 判断溢出
	if ( IsOverFlow(timeType,timeArr) == TRUE )
		return;
	if ( timeType == 1 && IsOverFlow(2,timeArr) == TRUE )
	{
		CString str = timeStr;
		timeStr.Format(_T("%s%02d%s"),str.Left(k_TimePos[2]),1,
			str.Right(str.GetLength()-k_TimePos[2]-k_TimeLen[2]));
	}
	// 更新结果
	SetWindowText(timeStr);
	SetSel(k_TimePos[timeType],k_TimePos[timeType]+k_TimeLen[timeType]);

}

///
/// @brief
///     下方向键按键响应
///
void  CTimeEditCtrl::KeyMoveDown()
{
	int rightPos = HIWORD(GetSel());  // 获取选中文本右边光标相对于控件的位置
	int timeType = GetTimeTypeInCursorPos(rightPos);
	CString timeStr;
	int timeArr[6];

	// 获取编辑后的时间值
	GetWindowText(timeStr);
	TimeStr2IntArr(timeStr,timeArr,6);
	timeArr[timeType]--;
	timeStr.Format(TIME_FORMAT_STR,timeArr[0],timeArr[1],timeArr[2],timeArr[3],timeArr[4],timeArr[5]);
	// 判断溢出
	if ( IsOverFlow(timeType,timeArr) == TRUE )
		return;
	if ( timeType == 1 && IsOverFlow(2,timeArr) == TRUE )
	{
		CString str = timeStr;
		timeStr.Format(_T("%s%02d%s"),str.Left(k_TimePos[2]),1,
			str.Right(str.GetLength()-k_TimePos[2]-k_TimeLen[2]));
	}
	// 更新结果
	SetWindowText(timeStr);
	SetSel(k_TimePos[timeType],k_TimePos[timeType]+k_TimeLen[timeType]);
}
