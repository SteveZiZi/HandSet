/// @file
///     BitStrEditCtrl.cpp
///
/// @brief
///     位串类型数据编辑器Inplace控件
///
/// @note
///     控件有两种创建方式：
///     直接创建（Create） :确认/取消修改通过消息发送
///     模态窗口（GoModal）:直接返回编辑结果
///     注意，位串的长度限制 = 创建控件时用于初始化的字符串的长度
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.06.09
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.07.03     |1.0.1.703  |优化字符输入处理，使控件中某一项参数编辑完成则自动跳转到下一项|
///    |2013.07.03     |1.0.1.703  |添加左右方向键、BackSpace键、Tab键的消息处理             |
///    |2013.07.03     |1.0.1.703  |修改位串的长度限制默认为13个字符，并添加位串长度限制的设置接口|
///    |2013.07.05     |1.0.1.705  |修改位串长度限制的设置接口及位串填充的逻辑               |
///
#include "stdafx.h"
#include "BitStrEditCtrl.h"

///
/// @brief
///     当控件弹出时, 响应主菜单的命令ID: 删除一个字符
///     注意: 本ID需要与主程序框架定义的ID一致, 否则应该修改
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
///     设置背景颜色
///
void  CBitStrEditCtrl::SetBkColor (COLORREF color)
{
	m_bkColor = color;

	m_bkBrush.DeleteObject();
	m_bkBrush.CreateSolidBrush(color);
}

///
/// @brief
///     设置文字颜色
///
void  CBitStrEditCtrl::SetTextColor (COLORREF color)
{
	m_clrText = color;
}

///
/// @brief
///     设置边框颜色
///
void  CBitStrEditCtrl::SetBorderColor (COLORREF color)
{
	m_clrBorder = color;
}

///
/// @brief
///     设置文字字体，默认为父窗口字体
///
void  CBitStrEditCtrl::SetTextFont(CFont* pFont)
{
	m_pTextFont = pFont;
}

///
/// @brief
///     设置位串的长度
///
/// @return
///     TRUE - 成功； FALSE - 失败
///
BOOL  CBitStrEditCtrl::SetBitStrLen(int len)
{
	if ( len <= 0 )
		return FALSE;

	m_iBitStrLen = len;

	// 如果窗口已创建, 则更新
	if ( m_hWnd )
	{
		// 更新文本
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
		// 设置最大字符数
		SetLimitText(m_iBitStrLen);
		// 选中第1个字符
		SetSel(0,1);
	}
	return TRUE;
}

///
/// @brief
///     将位串字符串转换为位串值
///
/// @param[in]
///     str     - 位串字符串
/// @param[in]
///     padding - 保存位串的补位位数
/// @param[in]
///     strSize - 位串的实际大小，为位串位数/8+1
/// @param[in]
///     bitStr  - 保存位串数据的数组
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
///     将位串值转换为位串字符串
///
/// @param[in]
///     padding - 位串的补位位数
/// @param[in]
///     strSize - 位串的大小，为位串位数/8+1
/// @param[in]
///     bitStr  - 位串数据的数组
/// @param[in]
///     str     - 位串字符串
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
///     弹出模态窗口编辑位串数据
///
/// @param[in]
///     pParentWnd - 指向父窗口的指针
/// @param[in]
///     rt         - 模态窗口的矩形域
/// @param[in]
///     bitStr     - 初始位串
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

	// 创建窗口
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

	// 获取编辑结果
	GetWindowText(bitStr);
	FillBitStr(bitStr);

	ReleaseCapture();
	DestroyWindow();

	m_bGoModal = false;

	return (nResult == IDOK);
}

///
/// @brief
///     创建并显示位串数据编辑控件
///
/// @param[in]
///     pParentWnd - 指向父窗口的指针
/// @param[in]
///     rt         - 模态窗口的矩形域
/// @param[in]
///     bitStr     - 初始位串
///
/// @return
///     
///
void  CBitStrEditCtrl::Create(CWnd *pParentWnd, const CRect &rt, CString &bitStr)
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
	CorrectBitStr(bitStr);
	FillBitStr(bitStr);
	this->SetWindowText(bitStr);
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
	rcWnd.DeflateRect(10, vMargin);                                     // 默认设置编辑器左右边距是10像素
	this->SetRectNP(&rcWnd);

	// 设置最大字符数
	SetLimitText(m_iBitStrLen);
	// 设置光标位置
	SetSel(0,1);

	m_pParentWnd = pParentWnd;
	m_rtCtrlArea = rt;
	m_bCancel    = false;                                           // 复位标志
	m_strPrevBitStr = bitStr;
}

///
/// @brief
///     隐藏Inplace编辑窗口
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
///     确认用户输入修改, 通知父窗口
///
void  CBitStrEditCtrl::ApplyInput ()
{
	if ( m_bGoModal )
	{
		EndModalLoop(IDOK);
	}
	else
	{
		// 向父窗口发送消息
		::SendMessage(m_pParentWnd->m_hWnd,WM_BIT_STR_EDIT_APPLY,0,0);
		// 销毁窗口
		DestroyWindow();
	}
}

///
/// @brief
///     用户取消修改
///
void  CBitStrEditCtrl::CancelInput ()
{
	if ( m_bGoModal )
	{
		EndModalLoop(IDCANCEL);
	}
	else
	{
		// 向父窗口发送消息
		::SendMessage(m_pParentWnd->m_hWnd,WM_BIT_STR_EDIT_CANCLE,0,0);
		// 销毁窗口
		DestroyWindow();
	}
}

///
/// @brief
///     当Inplace窗口丢失焦点时, 确认用户已经完成修改
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
///     使控件能够自己处理输入的箭头键和TAB键
///
UINT  CBitStrEditCtrl::OnGetDlgCode ()
{
	return DLGC_WANTALLKEYS;
}

///
/// @brief
///     截获ESC, 回车等键, 确认用户修改完成
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
	// TODO: 在此添加消息处理程序代码和/或调用默认值

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

#else  // MAC_EDIT_DOUBLEBUFFER

	CPaintDC dc(this);
	DefWindowProc(WM_PAINT, (WPARAM)dc.m_hDC, (LPARAM)0);

#endif  // MAC_EDIT_DOUBLEBUFFER
}

///
/// @brief
///     截获按键消息, 让父窗口不能拿到ESC Enter按键消息
///
BOOL  CBitStrEditCtrl::PreTranslateMessage (MSG* pMsg)
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
HBRUSH  CBitStrEditCtrl::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
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
///     修正位串数据，将非'0'或'1'的字符置为0
///
/// @param[in]
///     str - 位串
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
///     填充位串，长度不足则补充'0'，超过则截取
///
/// @param[in]
///     str - 位串
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
///     ESC按键响应，操作结束后直接返回
///
void  CBitStrEditCtrl::KeyEscapeDown()
{
	// 用户取消修改
	m_bCancel = true;
	// 恢复为原始文本
	SetWindowText(m_strPrevBitStr);
	// 使得edit控件丢失焦点, 触发Killfocus, 回到父窗口
	GetParent()->SetFocus();
}

///
/// @brief
///     回车键按键响应
///
void  CBitStrEditCtrl::KeyReturnDown()
{
	// 填充剩余位为'0'
	CString str;
	GetWindowText(str);
	FillBitStr(str);
	SetWindowText(str);
	// 使得edit控件丢失焦点, 触发Killfocus, 回到父窗口
	GetParent()->SetFocus();
}

///
/// @brief
///     左方向键按键响应
///
void  CBitStrEditCtrl::KeyMoveLeft()
{
	int leftPos  = LOWORD(GetSel());  // 获取选中文本左边界相对于控件的位置
	int rightPos = HIWORD(GetSel());  // 获取选中文本右边界相对于控件的位置

	// 跳到上一项
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
///     右方向键按键响应
///
void  CBitStrEditCtrl::KeyMoveRight()
{
	int leftPos  = LOWORD(GetSel());  // 获取选中文本左边界相对于控件的位置
	int rightPos = HIWORD(GetSel());  // 获取选中文本右边界相对于控件的位置

	// 跳到上一项
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
///     上方向键按键响应
///
void  CBitStrEditCtrl::KeyMoveUp()
{
	int rightPos = HIWORD(GetSel());  // 获取选中文本右边光标相对于控件的位置

	if ( rightPos == 0 )
		return;

	CString bitStr;
	GetWindowText(bitStr);
	CString str(_T("1"));
	if ( bitStr[rightPos-1] == _T('1') )
		str = _T("0");

	// 替换1个字符
	SetSel(rightPos-1,rightPos);
	ReplaceSel(str);
	SetSel(rightPos-1,rightPos);
}

///
/// @brief
///     下方向键按键响应
///
void  CBitStrEditCtrl::KeyMoveDown()
{
	int rightPos = HIWORD(GetSel());  // 获取选中文本右边光标相对于控件的位置

	if ( rightPos == 0 )
		return;

	CString bitStr;
	GetWindowText(bitStr);
	CString str(_T("1"));
	if ( bitStr[rightPos-1] == _T('1') )
		str = _T("0");

	// 替换1个字符
	SetSel(rightPos-1,rightPos);
	ReplaceSel(str);
	SetSel(rightPos-1,rightPos);
}

///
/// @brief
///     Backspace按键响应，操作结束后直接返回
///
void  CBitStrEditCtrl::KeyBackDown()
{
	int leftPos = LOWORD(GetSel());
	int rightPos = HIWORD(GetSel());  // 获取选中文本右边光标相对于控件的位置

	if ( leftPos == rightPos && leftPos == 0 )
	{
		return;
	}

	// 替换1个字符
	SetSel(rightPos-1,rightPos);
	ReplaceSel(_T("0"));
	SetSel(rightPos-1,rightPos);
}

///
/// @brief
///     TAB按键响应，操作结束后直接返回
///
void  CBitStrEditCtrl::KeyTabDown()
{
	int leftPos  = LOWORD(GetSel());  // 获取选中文本左边界相对于控件的位置
	int rightPos = HIWORD(GetSel());  // 获取选中文本右边界相对于控件的位置

	// 跳到上一项
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
///     数字键按键响应
///
void  CBitStrEditCtrl::KeyNumDown(UINT nChar)
{
	if ( nChar != '0' && nChar != '1' )
		return;

	int leftPos  = LOWORD(GetSel());  // 获取选中文本左边光标相对于控件的位置
	int rightPos = HIWORD(GetSel());  // 获取选中文本右边光标相对于控件的位置

	if ( leftPos == rightPos && leftPos == 0 )
	{
		return;
	}

	// 替换1个字符
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
