#pragma once


// CFullScreenDlg dialog

class CFullScreenDlg : public CDialog
{
	DECLARE_DYNAMIC(CFullScreenDlg)

public:
	CFullScreenDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFullScreenDlg();

// Dialog Data
	enum { IDD = IDD_FULL_SCREEN_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
    int     m_showLevel;                                //根据其值决定显示的内容
    HDC     m_hMemDC;
    HBITMAP m_hMemBitmap;
    int     m_iImgWidth;                                //测试图片的宽度
    int     m_iImgHeight;                               //测试图片的高度

    BOOL LoadTestImage();

public:

    BOOL    DisplayColorLevel();                        //显示色阶图
    BOOL    DisplayStartInterface();                    //显示开始界面
    BOOL    DisplayTestImage();                         //显示测试图片

    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg void OnDestroy();
};
