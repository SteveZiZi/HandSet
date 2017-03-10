#pragma once

#include "../SCLEditor.h"

typedef DWORD (WINAPI *DSMProgressTask)(void* arg);

// CProgressBarDlg �Ի���

class CProgressBarDlg : public CDialog
{
	DECLARE_DYNAMIC(CProgressBarDlg)

public:
	CProgressBarDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CProgressBarDlg();

// �Ի�������
	enum { IDD = IDD_PROGRESS_BAR_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint();
    afx_msg void OnDestroy();

protected:
    virtual void OnOK();
    virtual void OnCancel();

public:
	inline void _SetTask(DSMProgressTask pTask, LPVOID arg)
	{
		m_pTask = pTask;
		m_pArgs = arg;
	}

    inline void _SetTips(CString csTips)
    {
        m_csTips = csTips;
    }

    inline void _SetTextColor(COLORREF color)
    {
        m_colorText = color;
    }

    inline void _SetBkColor(COLORREF color)
    {
        m_colorBk = color;
    }

    inline void _SetFont(CFont* pFont)
    {
        m_pFont = pFont;
    }



private:
	DSMProgressTask     m_pTask;            ///< �̻߳ص�����

    UINT                m_nTimerID;         ///< ˢ�½���Ķ�ʱ��ID
    CString             m_csTips;           ///< ������ʾ�ַ���
    
    LPVOID              m_pArgs;            ///< �̲߳���
    HANDLE              m_hThread;          ///< �����߳�

    CFont*              m_pFont;            ///< ����
    COLORREF            m_colorBk;          ///< ����ɫ
    COLORREF            m_colorText;        ///< ������ɫ

#ifdef PROGRESS_BAR_ENABLE
	CRect               m_rtProgressBar;         ///< �Ի�������ؼ��Ľ������ľ�����
	CPoint              m_ptProgressBarBorder[5];///< �Ի�������ؼ������Ķ˵�
#endif
};
