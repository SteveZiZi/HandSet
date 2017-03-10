#pragma once


// C7290TestDlg dialog

class C7290TestDlg : public CDialog
{
	DECLARE_DYNAMIC(C7290TestDlg)

public:
	C7290TestDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~C7290TestDlg();

// Dialog Data
	enum { IDD = IDD_DLG_7290 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private: 
    HINSTANCE  m_h7290Dll;                 //¿ØÖÆ7290µÄ¶¯Ì¬¿â
   
    int (*epcZlg7290ScanNum)(DWORD dwNumber);   
    int (*epcZlg7290FlashTime)(DWORD dwFlashTime);   
    int (*epcZlg7290ScanNumber)(DWORD dwNumber, DWORD dwData, BOOL bDpEnable, BOOL bFlash);
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnDestroy();
};
