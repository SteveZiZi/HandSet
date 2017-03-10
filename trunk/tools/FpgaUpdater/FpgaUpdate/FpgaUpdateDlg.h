// FpgaUpdateDlg.h : 头文件
//

#pragma once
#include "resource.h"
// CFpgaUpdateDlg 对话框
class CFpgaUpdateDlg : public CDialog
{
// 构造
public:
	CFpgaUpdateDlg(CWnd* pParent = NULL);	// 标准构造函数
    ~CFpgaUpdateDlg();
// 对话框数据
	enum { IDD = IDD_FPGAUPDATE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;
    CBitmap m_mainBG;
    CBitmap m_SuccBG;
    CBitmap m_FailBG;
    CBitmap m_scroll;
    CBitmap m_scrollBG;
    CBitmap m_runScroll;

    //当前操作的进度情况
    double m_curRange;

    //具体操作的进度情况
    double m_curDetailRange;

    CString m_strText;
    CString m_strDetailText;

    //升级是否成功
    bool m_bIsSuccess;

    //进度滚动条左上角坐标
    POINT m_scrollLeftP;  

    //具体滚动条左上角坐标
    POINT m_scrollDetailLeftP;

    //总体进度的文本显示矩形左上角坐标
    POINT m_textLeftP;    

    //具体操作的文本显示矩形左上角坐标
    POINT m_textDetailLeftP;

    //升级结果图片左上角坐标
    POINT m_resultLeftP;    

    CFont m_fntContent;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
    void DrawBG(CDC * pDC);
    
public:
    void SetProcessRange(double curRange, CString strText, CString strDetailTextbool = L"", bool bIsSuccess = FALSE);

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	afx_msg void OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/);
#endif
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnTimer(UINT_PTR nIDEvent);

    static UINT StartUpdateThread( LPVOID pParam );

};
