#pragma once

#include "src/main/resource.h"
#include "src/utils/sliderboxcontrol/sliderbox.h"

class CRNaviMenu;

// CHelpDlg 对话框

class CHelpDlg : public CDialog
{
	DECLARE_DYNAMIC(CHelpDlg)

public:
	CHelpDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CHelpDlg();

// 对话框数据
	enum { IDD = IDD_HELP_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();

private:
    CSliderBox          m_sliderbox;            ///< 图片浏览控件
    CRNaviMenu*         m_pRNaviMenu;           ///< 菜单维护对象 CNaviMenu
};
