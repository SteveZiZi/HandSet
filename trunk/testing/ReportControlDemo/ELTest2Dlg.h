#pragma once


#include "src/utils/ReportControl/Template/ListCtrlExt.h"

// CELTest2Dlg �Ի���

class CELTest2Dlg : public CDialog
{
	DECLARE_DYNAMIC(CELTest2Dlg)

    CListCtrlT_VLineMidH        m_ctlListEL;
    CFont                       m_font;

    CListCtrlT_AboutCtrl        m_aboutCtrl;

public:
	CELTest2Dlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CELTest2Dlg();

// �Ի�������
	enum { IDD = IDD_ELTEST2_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
};
