#ifndef BIT_STRING_EDIT_CTRL_H_

#define BIT_STRING_EDIT_CTRL_H_

#include "src\include\p_wndusermsg_def.h"

///
/// @brief
///     直接创建方式下，确认/取消修改的消息定义
///
#define WM_BIT_STR_EDIT_CANCLE PUBLIC_WND_USERID+120
#define WM_BIT_STR_EDIT_APPLY  PUBLIC_WND_USERID+121

// 默认开启双缓冲机制
#ifndef BIT_STR_EDIT_DOUBLEBUFFER
#define BIT_STR_EDIT_DOUBLEBUFFER
#endif

class CBitStrEditCtrl:public CEdit
{
public:
	//
	//=============================================================================
	// 控件创建及样式
	//=============================================================================
	//
	CBitStrEditCtrl ();
	~CBitStrEditCtrl ();

	int   GoModal(CWnd *pParentWnd, const CRect &rt, CString &bitStr);
	void  Create (CWnd *pParentWnd, const CRect &rt, CString &bitStr);
	void  HideWindow ();

	void  SetBkColor (const COLORREF color);
	void  SetTextColor(const COLORREF color);
	void  SetBorderColor(const COLORREF color);
	void  SetTextFont(CFont *pFont);
	BOOL  SetBitStrLen(int len);

	UINT32 GetBitStrSize();

	static void  Str2BitStrData(const CString &str, UINT8 &padding,UINT32 &strSize,UINT8 *bitStr);                     ///< 指向位串数据的字节型指针);
	static void  BitStrData2Str(const UINT8 padding,const UINT32 strSize,const UINT8 *bitStr, CString &str);
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

	void  CorrectBitStr(CString &str);
	void  FillBitStr(CString &str);

	void  KeyEscapeDown();
	void  KeyReturnDown();
	void  KeyMoveLeft();
	void  KeyMoveRight();
	void  KeyMoveUp();
	void  KeyMoveDown();
	void  KeyBackDown();
	void  KeyTabDown();
	void  KeyNumDown(UINT nChar);

private:
	//
	//=============================================================================
	// 内部状态属性变量
	//=============================================================================
	//
	CWnd         *m_pParentWnd;       ///< 指向父窗口的指针
	CRect         m_rtCtrlArea;       ///< 编辑器窗口的矩形域

	COLORREF      m_clrText;          ///< 文本颜色
	COLORREF      m_bkColor;          ///< 背景颜色
	COLORREF      m_clrBorder;        ///< 边框颜色
	CBrush        m_bkBrush;          ///< 背景色画刷

	bool          m_bCancel;          ///< 用户是否取消了编辑
	CString       m_strPrevBitStr;    ///< 初始的位串值
	bool          m_bGoModal;         ///< 编辑器是否为模态窗口

	int           m_iBitStrLen;       ///< 位串长度
	CFont        *m_pTextFont;        ///< 文本字体
};

#endif
