#ifndef MAC_EDIT_CTRL_H_

#define MAC_EDIT_CTRL_H_

#include "src\include\p_wndusermsg_def.h"

///
/// @brief
///     ֱ�Ӵ�����ʽ�£�ȷ��/ȡ���޸ĵ���Ϣ����
///
#define WM_MAC_EDIT_CANCLE PUBLIC_WND_USERID+110
#define WM_MAC_EDIT_APPLY  PUBLIC_WND_USERID+111

///
/// @brief
///     ���ؼ�����ʱ, ��Ӧ���˵�������ID����Macֵ��ΪȫFF
///     ע��: ��ID��Ҫ���������ܶ����IDһ��, ����Ӧ���޸�
///
#ifndef MAC_EDIT_CMD_SET_TO_ALL_FF
#define MAC_EDIT_CMD_SET_TO_ALL_FF      32798
#endif

// Ĭ�Ͽ���˫�������
#ifndef MAC_EDIT_DOUBLEBUFFER
#define MAC_EDIT_DOUBLEBUFFER
#endif

class CMacEditCtrl:public CEdit
{
public:
	//
	//=============================================================================
	// �ؼ���������ʽ
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
	// ��Ϣ����
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
	// ��������
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
	// �ڲ�״̬���Ա���
	//=============================================================================
	//
	CWnd         *m_pParentWnd;       ///< ָ�򸸴��ڵ�ָ��
	CRect         m_rtCtrlArea;       ///< �༭�����ڵľ�����

	COLORREF      m_clrText;          ///< �ı���ɫ
	COLORREF      m_bkColor;          ///< ������ɫ
	COLORREF      m_clrBorder;        ///< �߿���ɫ
	CBrush        m_bkBrush;          ///< ����ɫ��ˢ

	bool          m_bCancel;          ///< �û��Ƿ�ȡ���˱༭
	CString       m_strPrevMac;       ///< ��ʼ��Mac��ַ��
	bool          m_bGoModal;         ///< �༭���Ƿ�Ϊģ̬����

	CFont        *m_pTextFont;        ///< �ı�����
};

#endif
