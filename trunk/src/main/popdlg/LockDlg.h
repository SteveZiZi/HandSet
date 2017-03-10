#pragma once

#include "src/main/resource.h"


class CRNaviMenu;

// CLockDlg �Ի���

class CLockDlg : public CDialog
{
	DECLARE_DYNAMIC(CLockDlg)

public:
	CLockDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLockDlg();

// �Ի�������
	enum { IDD = IDD_POPUP_LOCK_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnPaint();
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
    virtual void OnOK() {}
    virtual void OnCancel() {}

private:

    UINT                m_nLockTimer;       ///< ����ʱ�����ش��ڵĶ�ʱ��
    UINT                m_nUnlockTimer;     ///< ����ʱ����ʾ��������ͣ���Ķ�ʱ��
    bool                m_bLock;            ///< ����true or ����false
    CRect               m_rcClient;         ///< �ͻ�����С
    CRNaviMenu*         m_pRNaviMenu;       ///< �˵���Դ�������
};
