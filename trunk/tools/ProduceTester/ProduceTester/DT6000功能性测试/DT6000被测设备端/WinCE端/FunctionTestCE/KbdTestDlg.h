#pragma once

#include ".\app_src\KBDrvOper.h"
#include ".\app_src\KBKeyMap.h"
// CKbdTestDlg dialog

class CKbdTestDlg : public CDialog
{
	DECLARE_DYNAMIC(CKbdTestDlg)

public:
	CKbdTestDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CKbdTestDlg();

// Dialog Data
	enum { IDD = IDD_KEYBOARD_TEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
    CString m_strPressKey;
    
    

public:
    virtual BOOL OnInitDialog();
    CKBDrvOper * m_pCkbDrv;
    
protected:
    virtual void OnOK();
    virtual void OnCancel();
};
