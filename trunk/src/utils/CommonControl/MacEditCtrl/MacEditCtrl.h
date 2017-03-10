#ifndef MAC_EDIT_CTRL_H_

#define MAC_EDIT_CTRL_H_

#include "src\include\p_wndusermsg_def.h"

///
/// @brief
///     直接创建方式下，确认/取消修改的消息定义
///
#define WM_MAC_EDIT_CANCLE PUBLIC_WND_USERID+110
#define WM_MAC_EDIT_APPLY  PUBLIC_WND_USERID+111

///
/// @brief
///     当控件弹出时, 响应主菜单的命令ID：将Mac值设为全FF
///     注意: 本ID需要与主程序框架定义的ID一致, 否则应该修改
///
#ifndef MAC_EDIT_CMD_SET_TO_ALL_FF
#define MAC_EDIT_CMD_SET_TO_ALL_FF      32798
#endif

// 默认开启双缓冲机制
#ifndef MAC_EDIT_DOUBLEBUFFER
#define MAC_EDIT_DOUBLEBUFFER
#endif

class CMacEditCtrl:public CEdit
{
public:
	//
	//=============================================================================
	// 控件创建及样式
	//=============================================================================
	//
	CMacEditCtrl ();
	~CMacEditCtrl ();

	int   GoModal(CWnd *pParentWnd, const CRect &rt, unsigned char *macArr);
	void  Create (CWnd *pParentWnd, const CRect &rt, unsigned char *macArr);
	void  HideWindow ();

	void  SetBkColor (const COLORREF color);
	void  SetTextColor (const COLORREF color);
	void  SetBorderColor (const COLORREF color);
	void  SetTextFont(CFont* pFont);

	static void  Str2MacAddr(const CString &str, unsigned char *macArr);
	static void  MacAddr2Str(const unsigned char *macArr, CString &str);
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
	afx_msg void  OnBtnSetMacToAllFF ();
	afx_msg void  OnUpdateBtnSetMacToAllFF (CCmdUI *pCmdUI);
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

	int   GetFieldCurPos(const int pos);
	void  CorrectMacStr(CString &str);

	void  KeyBackDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void  KeyTabDown();
	void  KeyEscapeDown();
	void  KeyReturnDown();
	void  KeyHexNumDown(UINT nChar);
	void  KeyMoveLeft();
	void  KeyMoveRight();
	void  KeyMoveUp();
	void  KeyMoveDown();

	void  SetMacToAllFF();

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
	CString       m_strPrevMac;       ///< 初始的Mac地址串
	bool          m_bGoModal;         ///< 编辑器是否为模态窗口

	CFont        *m_pTextFont;        ///< 文本字体
};

#endif
