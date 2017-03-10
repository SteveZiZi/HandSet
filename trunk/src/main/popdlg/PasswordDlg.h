#pragma once

#include "src/main/resource.h"
#include "src/main/control/editpassword.h"

typedef bool (* PASSWORDPROC)(CString, DWORD);

// CPasswordDlg �Ի���

class CPasswordDlg : public CDialog
{
	DECLARE_DYNAMIC(CPasswordDlg)

public:
	CPasswordDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPasswordDlg();

// �Ի�������
	enum { IDD = IDD_POPUP_PASSWORD_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnPaint();
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
    virtual void OnOK();

public:

    /**
     *      ���ù�������
     */
    void _SetProc(PASSWORDPROC pProc, DWORD param)
    {
        m_pProc     = pProc;
        m_dwParam   = param;
    }

private:

    PASSWORDPROC            m_pProc;                ///< ��������
    DWORD                   m_dwParam;              ///< ��������

    CRect                   m_rcTitle;              ///< ����

    CRect                   m_rcClient;             ///< ҳ������

    CEditPassword           m_editPassword;         ///< ����ؼ�

    CWnd*                   m_pWndPrev;             ///< ֮ǰ����ָ��

};
