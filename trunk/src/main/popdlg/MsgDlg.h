#pragma once

#include "src/main/resource.h"

#include "src/main/control/statusdisp.h"
#include "src/main/control/CountDownWnd.h"


typedef DWORD (WINAPI *MsgTask)(WPARAM wParam, LPARAM lParam, void* arg);


class CRNaviMenu;
class CNaviToolBar;

// CMsgDlg 对话框

class CMsgDlg : public CDialog
{
	DECLARE_DYNAMIC(CMsgDlg)

public:
	CMsgDlg(UINT nMsgID, UINT nBitmapID = 0, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMsgDlg();

// 对话框数据
	enum { IDD = IDD_POPUP_MSG_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:

    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
#if !SHENG_ZE
	afx_msg void OnMenuF1();
#endif
    afx_msg void OnMenuF5();
    afx_msg void OnMenuF6();
    afx_msg void OnClickBtnOk();
    afx_msg void OnClickBtnCancel();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);


protected:

    virtual void OnCancel() {}
    virtual void OnOK() {}
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

public:

    /**
     *      设置信息文字
     */
    void _SetMsg(CString csMsg)
    {
        m_csMsg = csMsg;
    }

    /**
     *      设置信息图片
     */
    void _SetBitmap(HBITMAP hBitmap)
    {
        std::vector<HBITMAP> v;
        v.push_back(hBitmap);
        m_ctlPic._InsertStatus(1, v);
    }

    /**
     *      设置多个图片
     */
    void _SetBitmap(std::vector<HBITMAP>& v)
    {
        m_ctlPic._InsertStatus(1, v);
    }

    /**
     *      获取控件
     */
    CStatusDisp* _GetCtl()
    {
        return &m_ctlPic;
    }

    /**
     *      设置区域大小
     */
    void _SetRect(CRect rc)
    {
        m_rcClient = rc;
    }

    /**
     *      设置图片区域
     */
    void _SetPicRC(CRect rc)
    {
        m_rcPic = rc;
    }

    /**
     *      设置文字区域
     */
    void _SetTextRC(CRect rc)
    {
        m_rcText = rc;
    }

    /**
     *      设置菜单
     */
    void _SetMenu(UINT menu)
    {
        m_nMenuID = menu;
    }

    /**
     *      设置是否显示图片
     */
    void _SetBitmapDisp(bool show)
    {
        m_bBitmap = show;
    }

    /**
     *      设置菜单ID范围
     */
    void _SetCommandRange(UINT begin, UINT end, MsgTask func, void* data = NULL)
    {
        m_nBegin    = begin;
        m_nEnd      = end;
        m_pFunc     = func;
        m_pData     = data;
    }

    /**
     *      启用倒计时控件
     */
    inline void _EnableCountDown()
    {
        m_bEnableCountDown = true;
    }

    inline CCountDownWnd* _GetCountDown()
    {
        return &m_ctlCountDown;
    }

    /**
     *      手动还原菜单
     */
    inline void _ResetMenu();
#if !SHENG_ZE
	/**
     *     设置菜单文字
     */
	void _ResetF1Text(CString text);
#endif
	/**
     *     设置菜单文字
     */
	void _ResetF5Text(CString text);
	/**
     *     设置菜单文字
     */
	void _ResetF6Text(CString text);
private:

    UINT                m_nMenuID;          ///< 菜单ID

    UINT                m_nBegin;
    UINT                m_nEnd;
    MsgTask             m_pFunc;
    void*               m_pData;

    bool                m_bBitmap;          ///< 是否创建图片控件
    bool                m_bEnableCountDown; ///< 启用倒计时控件

    CRect               m_rcClient;         ///< 客户区大小
    CRect               m_rcPic;            ///< 图片区域
    CRect               m_rcText;           ///< 文字区域

    CString             m_csMsg;            ///< 信息文字
#if !SHENG_ZE	
	CString				m_F1Text;
#endif
	CString				m_F5Text;
	CString				m_F6Text;

    CStatusDisp         m_ctlPic;           ///< 图片控件
    CCountDownWnd       m_ctlCountDown;     ///< 倒计时控件


    CNaviToolBar*       m_pNaviBar;         ///< 菜单对象
    CRNaviMenu*         m_pRNaviMenu;       ///< 菜单资源管理对象
};
