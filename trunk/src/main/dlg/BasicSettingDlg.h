#pragma once

#include "src/main/resource.h"

#include "BaseDlg.h"
#include "src/main/control/ButtonEx.h"
#include "src/utils/navicontrol/navimenubar/navitoolbar.h"
#include <vector>
// CBasicSettingDlg dialog

class CBasicSettingDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CBasicSettingDlg)

public:
	CBasicSettingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBasicSettingDlg();

// Dialog Data
	enum { IDD = IDD_BASIC_SETTING_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual void _modifyMenu();
	virtual BOOL OnInitDialog();
	afx_msg void OnItemValueChangedBasic(NMHDR* pNotifyStruct, LRESULT *pResult);
	afx_msg void OnItemSelectChangeBasic(NMHDR* pNotifyStruct, LRESULT *pResult);
	afx_msg void OnItemButtonClickBasic(NMHDR* pNotifyStruct, LRESULT *pResult);
	afx_msg void OnInplaceEditAction(NMHDR * pNotifyStruct, LRESULT* pResult);
	afx_msg LRESULT OnBrightnessChange(WPARAM wParam, LPARAM lParam);

	 virtual BOOL PreTranslateMessage(MSG* pMsg); 
private:
	 /**
		*      创建基本设置页表
		*/
	void _createBasicSettingReport();
	/**
	*		显示基本设置页表
	*/
	void _showBasicSettingReport();
	/**
     *      调整屏幕亮度
     */
    void _modifyBrightness(ELNM_INPLACEBUTTON_ARGS* pItemNotify, LRESULT *pResult);

    /**
     *      修改时间日期
     */
    void _modifyTime(ELNM_INPLACEBUTTON_ARGS* pItemNotify, LRESULT *pResult);
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
	std::vector<stTimeInfo*>  m_vecTimezone;    ///< 时区信息列表


	CReportH			m_ctlBasicSettingReport;		///< 基本设置报表
	CString             m_csDefaultTZS;         ///< 默认时区的注册表项名	
#if !SHENG_ZE
	CReportH            m_reportBase;           ///< 基本设置报表
#endif
};
