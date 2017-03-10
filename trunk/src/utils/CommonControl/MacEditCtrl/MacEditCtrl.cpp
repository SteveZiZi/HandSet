/// @file
///     MacEditCtrl.cpp
///
/// @brief
///     Mac地址编辑器Inplace控件
///
/// @note
///     控件有两种创建方式：
///     直接创建（Create） :确认/取消修改通过消息发送
///     模态窗口（GoModal）:直接返回编辑结果
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.5.24
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.07.02     |1.0.1.702  |优化字符输入处理，使控件中某一项参数编辑完成则自动跳转到下一项|
///    |2013.10.15     |1.0.1.1015 |添加将Mac值设为全FF的命令消息                                 |
///
#include "stdafx.h"
#include "MacEditCtrl.h"

///
/// @brief
///     Mac地址串的格式、字段数及限定字符数
///
#define MAC_FORMAT_STR _T("%02x-%02x-%02x-%02x-%02x-%02x")
#define MAC_STR_FIELD_NUM 6
#define MAC_STR_SIZE 27

///
/// @brief
///     Mac地址串中各段的偏移地址及长度
///
const int k_MacPos[6] = {0,3,6,9,12,15};
const int k_MacLen[6] = {2,2,2,2,2,2};

///
/// @brief
///     当控件弹出时, 响应主菜单的命令ID: 删除一个字符
///     注意: 本ID需要与主程序框架定义的ID一致, 否则应该修改
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
///     设置背景颜色
///
void  CMacEditCtrl::SetBkColor (COLORREF color)
{
	m_bkColor = color;

	m_bkBrush.DeleteObject();
	m_bkBrush.CreateSolidBrush(color);
}

///
/// @brief
///     设置文字颜色
///
void  CMacEditCtrl::SetTextColor (COLORREF color)
{
	m_clrText = color;
}

///
/// @brief
///     设置边框颜色
///
void  CMacEditCtrl::SetBorderColor (COLORREF color)
{
	m_clrBorder = color;
}

///
/// @brief
///     设置文字字体，默认为父窗口字体
///
void  CMacEditCtrl::SetTextFont(CFont* pFont)
{
	m_pTextFont = pFont;
}

///
/// @brief
///     将Mac地址串转换为数组
///
/// @param[in]
///     str    - Mac地址串
/// @param[in]
///     macArr - 保存Mac地址的数组，大小需为6
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
///     将Mac信息转换为Mac地址串
///
/// @param[in]
///     macArr - 保存Mac地址的数组，大小需为6
/// @param[in]
///     str    - Mac地址串
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
///     弹出模态窗口编辑Mac地址
///
/// @param[in]
///     pParentWnd - 指向父窗口的指针
/// @param[in]
///     rt         - 模态窗口的矩形域
/// @param[in]
///     macArr     - 保存初始Mac地址的数组，大小需为6
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

	// 创建窗口
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

	// 获取编辑结果
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
///     创建并显示Mac地址编辑控件
///
/// @param[in]
///     pParentWnd - 指向父窗口的指针
/// @param[in]
///     rt         - 模态窗口的矩形域
/// @param[in]
///     macArr     - 保存初始Mac地址的数组，大小需为6
///
/// @return
///     
///
void  CMacEditCtrl::Create(CWnd *pParentWnd, const CRect &rt, unsigned char *macArr)
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
	MacAddr2Str(macArr,str);
	this->SetWindowText(str);
	this->SetWindowPos(0, rt.left, rt.top, rt.Width(), rt.Height(), 
		SWP_NOZORDER | SWP_SHOWWINDOW);

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
	this->SetMargins(0, 0);
	int nFontHeight = tm.tmHeight + tm.tmExternalLeading;
	int vMargin = (rcWnd.Height() - nFontHeight) / 2;
	rcWnd.DeflateRect(10, vMargin);
	this->SetRectNP(&rcWnd);

	// 设置最大字符数
	SetLimitText(MAC_STR_SIZE);

	// 设置选中的字符
	SetSel(k_MacPos[0],k_MacPos[0]+k_MacLen[0]);

	m_pParentWnd = pParentWnd;
	m_rtCtrlArea = rt;
	m_bCancel    = false;                                           // 复位标志
	m_strPrevMac = str;
}

///
/// @brief
///     隐藏Inplace编辑窗口
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
///     确认用户输入修改, 通知父窗口
///
void  CMacEditCtrl::ApplyInput ()
{
	if ( m_bGoModal )
	{
		EndModalLoop(IDOK);
	}
	else
	{
		// 向父窗口发送消息
		::SendMessage(m_pParentWnd->m_hWnd,WM_MAC_EDIT_APPLY,0,0);
		// 销毁窗口
		DestroyWindow();
	}
}

///
/// @brief
///     用户取消修改
///
void  CMacEditCtrl::CancelInput ()
{
	if ( m_bGoModal )
	{
		EndModalLoop(IDCANCEL);
	}
	else
	{
		// 向父窗口发送消息
		::SendMessage(m_pParentWnd->m_hWnd,WM_MAC_EDIT_CANCLE,0,0);
		// 销毁窗口
		DestroyWindow();
	}
}

///
/// @brief
///     当Inplace窗口丢失焦点时, 确认用户已经完成修改
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
///     使控件能够自己处理输入的箭头键和TAB键
///
UINT  CMacEditCtrl::OnGetDlgCode ()
{
	return DLGC_WANTALLKEYS;
}

///
/// @brief
///     截获ESC, 回车等键, 确认用户修改完成
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
	// TODO: 在此添加消息处理程序代码和/或调用默认值

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
BOOL  CMacEditCtrl::PreTranslateMessage (MSG* pMsg)
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
HBRUSH  CMacEditCtrl::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
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
///      响应外部菜单命令, 当主菜单点击Mac设为全FF的按钮时, 将Mac设为全FF
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
///     修正Mac地址串，将多余的' '置为0
///
/// @param[in]
///     str - Mac地址串
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
///     Backspace按键响应，操作结束后直接返回
///
void  CMacEditCtrl::KeyBackDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	int rightPos = HIWORD(GetSel());  // 获取选中文本右边光标相对于控件的位置
	int fieldIdx = GetFieldCurPos(rightPos);
	if ( rightPos == k_MacPos[fieldIdx] )
		return;
	int leftPos = LOWORD(GetSel());
	CEdit::OnChar(nChar, nRepCnt, nFlags);
	SetSel(k_MacPos[fieldIdx],k_MacPos[fieldIdx]);
	if ( leftPos != rightPos )
	{
		// 删除多个字符
		for ( int i = 0; i < k_MacLen[fieldIdx]; i++ )
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
void  CMacEditCtrl::KeyTabDown()
{
	int rightPos = HIWORD(GetSel());  // 获取选中文本右边光标相对于控件的位置
	int fieldIdx = GetFieldCurPos(rightPos);
	// 选中下一项时间项
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
///     ESC按键响应，操作结束后直接返回
///
void  CMacEditCtrl::KeyEscapeDown()
{
	// 用户取消修改
	m_bCancel = true;
	// 恢复为原始文本
	SetWindowText(m_strPrevMac);
	// 使得edit控件丢失焦点, 触发Killfocus, 回到父窗口
	GetParent()->SetFocus();
}

///
/// @brief
///     回车键按键响应
///
void  CMacEditCtrl::KeyReturnDown()
{
	// 修正多余的' '为'0'
	CString str;
	GetWindowText(str);
	CorrectMacStr(str);
	SetWindowText(str);
	// 使得edit控件丢失焦点, 触发Killfocus, 回到父窗口
	GetParent()->SetFocus();
}

///
/// @brief
///     数字键按键响应
///
void  CMacEditCtrl::KeyHexNumDown(UINT nChar)
{
	if ( !( ( nChar >= '0' && nChar <= '9' ) || 
		    ( nChar >= 'A' && nChar <= 'F' ) ||
		    ( nChar >= 'a' && nChar <= 'f' ) ) )
		return;

	int leftPos  = LOWORD(GetSel());  // 获取选中文本左边光标相对于控件的位置
	int rightPos = HIWORD(GetSel());  // 获取选中文本右边光标相对于控件的位置
	int fieldIdx = GetFieldCurPos(rightPos);
	CString macStr;
	GetWindowText(macStr);
	if ( leftPos != rightPos )
	{// 选中若干字符的情况
		// 将选中的字符替换为' '和输入字符的组合
		CString str1(_T("")),str2;
		for ( int i = 0; i < k_MacLen[fieldIdx]-1; i++ )
			str1.Append(_T(" "));
		str2.Format(_T("%s%c"),str1,nChar);
		str1.Format(_T("%s%s%s"),macStr.Left(k_MacPos[fieldIdx]),str2,
			macStr.Right(macStr.GetLength()-k_MacPos[fieldIdx]-k_MacLen[fieldIdx]));
		// 更新结果
		str1.MakeUpper();
		SetWindowText(str1);
		SetSel(rightPos,rightPos);
	}
	else if ( macStr[k_MacPos[fieldIdx]] == ' ' && leftPos != 0 )
	{// 未选中字符，且编辑区域存在' '的情况
		CString str1,str2;
		// 获取编辑结果
		str1 = macStr.Mid(k_MacPos[fieldIdx],k_MacLen[fieldIdx]);
		str2.Format(_T("%s%c%s"),str1.Mid(1,leftPos-k_MacPos[fieldIdx]-1),nChar,
			str1.Right(str1.GetLength()-leftPos+k_MacPos[fieldIdx]));
		str1.Format(_T("%s%s%s"),macStr.Left(k_MacPos[fieldIdx]),str2,
			macStr.Right(macStr.GetLength()-k_MacPos[fieldIdx]-k_MacLen[fieldIdx]));
		// 更新结果
		str1.MakeUpper();
		SetWindowText(str1);
		if ( str1[k_MacPos[fieldIdx]] != ' ' && fieldIdx < MAC_STR_FIELD_NUM-1 )
		{
			// 当前Mac项编辑完成，则跳到下一项
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
		// 当前Mac项编辑完成，则跳到下一项
		fieldIdx++;
		int i = 0;
		while ( macStr[k_MacPos[fieldIdx]+i] == ' ' && i < k_MacLen[fieldIdx] )
			i++;
		SetSel(k_MacPos[fieldIdx]+i,k_MacPos[fieldIdx]+k_MacLen[fieldIdx]);
	}
}

///
/// @brief
///     左方向键按键响应
///
void  CMacEditCtrl::KeyMoveLeft()
{
	int leftPos  = LOWORD(GetSel());  // 获取选中文本左边界相对于控件的位置
	int rightPos = HIWORD(GetSel());  // 获取选中文本右边界相对于控件的位置
	int fieldIdx = GetFieldCurPos(rightPos);
	CString str;
	GetWindowText(str);
	
	if ( (rightPos != leftPos) ||
		 (rightPos != 0 && str[rightPos-1] == ' ') ||
		 (leftPos == k_MacPos[fieldIdx]) )
	{
		// 光标的前一个字符为' '、位置为左边界或为选中状态，则跳到上一项
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
		// 光标左移一个
		rightPos--;
		SetSel(rightPos,rightPos);
	}
}

///
/// @brief
///     右方向键按键响应
///
void  CMacEditCtrl::KeyMoveRight()
{
	int rightPos = HIWORD(GetSel());  // 获取选中文本右边光标相对于控件的位置
	int fieldIdx = GetFieldCurPos(rightPos);
	CString str;
	GetWindowText(str);

	if ( rightPos == k_MacPos[fieldIdx] + k_MacLen[fieldIdx] )
	{
		// 光标为当前时间项的右边界，则跳到下一项
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
		// 光标右移一个
		rightPos++;
		SetSel(rightPos,rightPos);
	}
}

///
/// @brief
///     上方向键按键响应
///
void  CMacEditCtrl::KeyMoveUp()
{
	int rightPos = HIWORD(GetSel());  // 获取选中文本右边光标相对于控件的位置
	int fieldIdx = GetFieldCurPos(rightPos);
	CString str;
	unsigned char macArr[6];

	// 获取编辑后的Mac地址串，相应字段值+1
	GetWindowText(str);
	Str2MacAddr(str,macArr);
	macArr[fieldIdx]++;
	MacAddr2Str(macArr,str);
	// 更新结果
	str.MakeUpper();
	SetWindowText(str);
	SetSel(k_MacPos[fieldIdx],k_MacPos[fieldIdx]+k_MacLen[fieldIdx]);

}

///
/// @brief
///     下方向键按键响应
///
void  CMacEditCtrl::KeyMoveDown()
{
	int rightPos = HIWORD(GetSel());  // 获取选中文本右边光标相对于控件的位置
	int fieldIdx = GetFieldCurPos(rightPos);
	CString str;
	unsigned char macArr[6];

	// 获取编辑后的Mac地址串，相应字段值-1
	GetWindowText(str);
	Str2MacAddr(str,macArr);
	macArr[fieldIdx]--;
	MacAddr2Str(macArr,str);
	// 更新结果
	str.MakeUpper();
	SetWindowText(str);
	SetSel(k_MacPos[fieldIdx],k_MacPos[fieldIdx]+k_MacLen[fieldIdx]);
}

///
/// @brief
///     将Mac设为全FF
///
void CMacEditCtrl::SetMacToAllFF()
{
	// 如果窗口还没有创建, 则返回
	if (!m_hWnd)
		return;

	// 设置控件文本
	CString str;
	str.Format(MAC_FORMAT_STR,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF);
	str.MakeUpper();
	this->SetWindowText(str);
}
