#ifndef BIT_STRING_EDIT_CTRL_H_

#define BIT_STRING_EDIT_CTRL_H_

#include "src\include\p_wndusermsg_def.h"

///
/// @brief
///     ֱ�Ӵ�����ʽ�£�ȷ��/ȡ���޸ĵ���Ϣ����
///
#define WM_BIT_STR_EDIT_CANCLE PUBLIC_WND_USERID+120
#define WM_BIT_STR_EDIT_APPLY  PUBLIC_WND_USERID+121

// Ĭ�Ͽ���˫�������
#ifndef BIT_STR_EDIT_DOUBLEBUFFER
#define BIT_STR_EDIT_DOUBLEBUFFER
#endif

class CBitStrEditCtrl:public CEdit
{
public:
	//
	//=============================================================================
	// �ؼ���������ʽ
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

	static void  Str2BitStrData(const CString &str, UINT8 &padding,UINT32 &strSize,UINT8 *bitStr);                     ///< ָ��λ�����ݵ��ֽ���ָ��);
	static void  BitStrData2Str(const UINT8 padding,const UINT32 strSize,const UINT8 *bitStr, CString &str);
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
	CString       m_strPrevBitStr;    ///< ��ʼ��λ��ֵ
	bool          m_bGoModal;         ///< �༭���Ƿ�Ϊģ̬����

	int           m_iBitStrLen;       ///< λ������
	CFont        *m_pTextFont;        ///< �ı�����
};

#endif
