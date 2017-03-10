#ifndef IEC61850_TIME_EDIT_CTRL_H_

#define IEC61850_TIME_EDIT_CTRL_H_

#include "src\include\p_wndusermsg_def.h"

///
/// @brief
///     直接创建方式下，确认/取消修改的消息定义
///
#define WM_TIME_EDIT_MS_CANCLE PUBLIC_WND_USERID+130
#define WM_TIME_EDIT_MS_APPLY  PUBLIC_WND_USERID+131

// 默认开启双缓冲机制
#ifndef IEC61850_TIME_EDIT_DOUBLEBUFFER
#define IEC61850_TIME_EDIT_DOUBLEBUFFER
#endif

class CTimeEditCtrl_MS:public CEdit
{
public:
	//
	//=============================================================================
	// 控件创建及样式
	//=============================================================================
	//
	CTimeEditCtrl_MS ();
	~CTimeEditCtrl_MS ();

	int   GoModal(CWnd *pParentWnd, const CRect &rt, SYSTEMTIME &time);
	void  Create (CWnd *pParentWnd, const CRect &rt, const SYSTEMTIME &time);
	void  HideWindow ();

	void  SetBkColor (const COLORREF color);
	void  SetTextColor (const COLORREF color);
	void  SetBorderColor(const COLORREF color);
	void  SetTextFont(CFont *pFont);

	static void  Str2SysTime(const CString &str, SYSTEMTIME &time);
	static void  SysTime2Str(const SYSTEMTIME &time, CString &str);

protected:
	//
	//=============================================================================
	// 消息处理
	//=============================================================================
	//
	DECLARE_MESSAGE_MAP()

	afx_msg BOOL  OnEraseBkgnd(CDC* pDC);
	afx_msg void  OnPaint ();
	afx_msg void  OnEnKillfocus ();
	afx_msg UINT  OnGetDlgCode ();
	afx_msg void  OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void  OnChar (UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void  OnSysKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg HBRUSH  CtlColor(CDC* pDC, UINT /*nCtlColor*/);

	afx_msg void  OnBtnBackspace ();
	afx_msg void  OnUpdateBtnBackspace (CCmdUI *pCmdUI);
	afx_msg void  OnCaptureChanged(CWnd *pWnd);

	virtual BOOL  PreTranslateMessage (MSG* pMsg);


private:
	//
	//=============================================================================
	// 辅助函数
	//=============================================================================
	//
	void  ApplyInput ();
	void  CancelInput ();

	int   GetTimeTypeInCursorPos(const int pos);
	void  TimeStr2IntArr(const CString &timeStr, int *timeArr , const int arrSize);
	BOOL  IsOverFlow(const int timePos, const int *timeArr);
	void  CorrectTimeStr(CString &str);
	void  CorrectYear(CString &str);

	void  KeyBackDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void  KeyTabDown();
	void  KeyEscapeDown();
	void  KeyReturnDown();
	void  KeyNumDown(UINT nChar);
	void  KeyMoveLeft();
	void  KeyMoveRight();
	void  KeyMoveUp();
	void  KeyMoveDown();

private:
	//
	//=============================================================================
	// 内部状态属性变量
	//=============================================================================
	//
	CWnd      *m_pParentWnd;       ///< 指向父窗口的指针
	CRect      m_rtCtrlArea;       ///< 编辑器窗口的矩形域

	COLORREF   m_clrText;          ///< 文本颜色
	COLORREF   m_bkColor;          ///< 背景颜色
	COLORREF   m_clrBorder;        ///< 边框颜色
	CBrush     m_bkBrush;          ///< 背景色画刷

	bool       m_bCancel;          ///< 用户是否取消了编辑
	SYSTEMTIME m_tmPreTime;        ///< 原始的时间串
	bool       m_bGoModal;         ///< 时间编辑器是否为模态窗口

	CFont     *m_pTextFont;        ///< 文本字体
};

#endif
