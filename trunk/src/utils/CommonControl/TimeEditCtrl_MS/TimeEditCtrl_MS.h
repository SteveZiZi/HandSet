#ifndef IEC61850_TIME_EDIT_CTRL_H_

#define IEC61850_TIME_EDIT_CTRL_H_

#include "src\include\p_wndusermsg_def.h"

///
/// @brief
///     ֱ�Ӵ�����ʽ�£�ȷ��/ȡ���޸ĵ���Ϣ����
///
#define WM_TIME_EDIT_MS_CANCLE PUBLIC_WND_USERID+130
#define WM_TIME_EDIT_MS_APPLY  PUBLIC_WND_USERID+131

// Ĭ�Ͽ���˫�������
#ifndef IEC61850_TIME_EDIT_DOUBLEBUFFER
#define IEC61850_TIME_EDIT_DOUBLEBUFFER
#endif

class CTimeEditCtrl_MS:public CEdit
{
public:
	//
	//=============================================================================
	// �ؼ���������ʽ
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
	// �ڲ�״̬���Ա���
	//=============================================================================
	//
	CWnd      *m_pParentWnd;       ///< ָ�򸸴��ڵ�ָ��
	CRect      m_rtCtrlArea;       ///< �༭�����ڵľ�����

	COLORREF   m_clrText;          ///< �ı���ɫ
	COLORREF   m_bkColor;          ///< ������ɫ
	COLORREF   m_clrBorder;        ///< �߿���ɫ
	CBrush     m_bkBrush;          ///< ����ɫ��ˢ

	bool       m_bCancel;          ///< �û��Ƿ�ȡ���˱༭
	SYSTEMTIME m_tmPreTime;        ///< ԭʼ��ʱ�䴮
	bool       m_bGoModal;         ///< ʱ��༭���Ƿ�Ϊģ̬����

	CFont     *m_pTextFont;        ///< �ı�����
};

#endif
