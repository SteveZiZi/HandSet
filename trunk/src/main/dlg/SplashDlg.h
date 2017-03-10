/**
 * \file
 *      SplashDlg.h
 *
 * \brief
 *      应用程序开始前splash画面头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/4/8
 */


#pragma once

#include "src/main/resource.h"


// CSplashDlg 对话框

class CSplashDlg : public CDialog
{
	DECLARE_DYNAMIC(CSplashDlg)

public:
	CSplashDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSplashDlg();

// 对话框数据
	enum { IDD = IDD_SPLASH_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    /**
     *      显示splash图片对话框
     */
    static void _ShowSplashScreen(CWnd* pParentWnd);

    /**
     *      退出splash图片对话框，清理所有相关资源
     */
    static void _HideSplashScreen(void);

public:
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnPaint();

private:
    int                     m_nWidth;           ///< 屏幕宽度像素值
    int                     m_nHeight;          ///< 屏幕高度像素值
    int                     m_nTimerID;         ///< 定时器ID
    static CSplashDlg*      g_spSplashDlg;      ///< 静态对象指针
};


