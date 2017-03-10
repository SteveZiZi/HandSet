/**
 * \file
 *      BasicCfgDlg.h
 *
 * \brief
 *      dsm基本设置页面头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/4/8
 */

#pragma once

#include "BaseDlg.h"
#include "src/main/resource.h"
#include "src/main/control/menuex.h"
#include "src/main/control/statusdisp.h"
#include "src/utils/ReportControl/ListCtrlEL/ListCtrlEL.h"

#include <vector>


// CBasicCfgDlg 对话框

class CBasicCfgDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CBasicCfgDlg)

public:
	CBasicCfgDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBasicCfgDlg();

// 对话框数据
	enum { IDD = IDD_BASIC_CFG_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnItemValueChangedBasic(NMHDR* pNotifyStruct, LRESULT *pResult);
    afx_msg void OnItemSelectChangeBasic(NMHDR* pNotifyStruct, LRESULT *pResult);
    afx_msg void OnItemButtonClickBasic(NMHDR* pNotifyStruct, LRESULT *pResult);
    afx_msg void OnInplaceEditAction(NMHDR * pNotifyStruct, LRESULT* pResult);
    afx_msg void OnItemRestoreOK(NMHDR* pNotifyStruct, LRESULT *pResult);
    afx_msg void OnItemStorage(NMHDR* pNotifyStruct, LRESULT *pResult);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnNaviMenuWF6();
    afx_msg void OnNaviMenuF6();
    afx_msg void OnMenuChanged();
    afx_msg void OnDestroy();
    afx_msg LRESULT OnBrightnessChange(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnRptBeginEdit(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnExitPasswordCfg(WPARAM wParam, LPARAM lParam);
    afx_msg void OnInplcaseEditEsc(NMHDR * pNotifyStruct, LRESULT* pResult);
    afx_msg LRESULT OnDMTimeTick(WPARAM, LPARAM);


private:

    /**
     *      创建报表共用函数
     */
    void _createReport(CReportH& report, UINT nID);

    /**
     *      创建并初始化基本设置报表
     */
    void _initReportBase();

    /**
     *      创建并初始化还原管理报表
     */
    void _initReportRestore();

    /**
     *      创建并初始化关于报表
     */
    void _initReportAbout();

    /**
     *      创建并初始化关于报表的详细信息
     */
    void _initReportDetail();

    /**
     *      创建存储器报表
     */
    void _initReportStorage();

    /**
     *      进入基本设置页面
     */
    void _enterBase();

    /**
     *      进入连接管理页面
     */
    void _enterLinkmgr();

    /**
     *      进入存储器
     */
    void _enterStorage();

    /**
     *      进入还原设置
     */
    void _enterRestore();

    /**
     *      进入关于页面
     */
    void _enterAbout();

    /**
     *      初始化左侧菜单
     */
    void _initMenuNavi();

    /**
     *      加载时区
     */
    void _loadTimezone();

    /**
     *      调整时区列表顺序
     */
    void _adjustTimezone();

    /**
     *      分割
     */
    int _partition(int nLeft, int nRight);

    /**
     *      调整时区列表算法
     */
    void _qsort(int nleft, int nRight);

    /**
     *      调整屏幕亮度
     */
    void _modifyBrightness(ELNM_INPLACEBUTTON_ARGS* pItemNotify, LRESULT *pResult);

    /**
     *      修改时间日期
     */
    void _modifyTime(ELNM_INPLACEBUTTON_ARGS* pItemNotify, LRESULT *pResult);

    /**
     *      更新标题
     */
    void _updateCaption(UINT nCaption);

    /**
     *      离开密码修改模式
     */
    void _leavePWMode();

    /**
     *      进入密码修改模式
     */
    void _enterPWMode();

    /**
     *      连接USB
     */
    void _setLink();

    /**
     *      断开连接USB
     */
    void _setUnLink();

    /**
     *      初始化图片控件
     */
    void _showPic();

    /**
     *      设置默认时区
     */
    void _setDefaultTZS();


private:

    /**
     *      市区信息
     */
    struct stTimeInfo
    {
        CString csDisp;                     ///< 界面显示字符串
        CString csDlt;                      ///< 夏令时数据
        CString csStd;                      ///< 非夏令时数据
        TIME_ZONE_INFORMATION tzinfo;       ///< 时区信息
        CString csKeyName;                  ///< 时区的注册表项名
    };

    /**
     *      USB连接模式
     */
    enum MODE_USB
    {
        USB_LINKED,             ///< USB独占
        USB_UNLINK              ///< USB非独占
    };

    /**
     *      设备信息
     */
    enum MODE_INFO
    {
        INFO_BASE,              ///< 基本信息
        INFO_DETAIL,            ///< 详细信息
    };

private:

    bool                m_bWizard;              ///< 向导模式与一般设置模式

    bool                m_bPWNull;              ///< 空输入
    bool                m_bModifyPassword;      ///< 修改密码状态
    CString             m_csPasswordNew;        ///< 新密码
    CString             m_csPasswordConfirm;    ///< 确认新密码
    UINT                m_nTimerLinkStatus;     ///< usb连接状态定时器

    CReportH            m_reportBase;           ///< 基本设置报表
    CReportH            m_reportRestore;        ///< 还原设置
    CReportAbout        m_reportAbout;          ///< 关于报表
    CReportH            m_reportDetail;         ///< 关于报表详细信息
    CReportH            m_reportStorage;        ///< 存储器报表

    CMenuEx             m_menuNavi;             ///< 左侧导航菜单
    CListCtrlEL*        m_pCurReport;           ///< 当前工作的报表指针

    MODE_USB            m_eUSBMode;             ///< 当前USB模式
    CStatusDisp         m_ctlPic;               ///< 图片控件

    CString             m_csDefaultTZS;         ///< 默认时区的注册表项名
    std::vector<stTimeInfo*>  m_vecTimezone;    ///< 时区信息列表

    MODE_INFO           m_eInfo;                ///< 信息显示模式
    unsigned int        m_nCountDown;           ///< usb连接倒时计数
};

