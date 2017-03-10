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
		*      ������������ҳ��
		*/
	void _createBasicSettingReport();
	/**
	*		��ʾ��������ҳ��
	*/
	void _showBasicSettingReport();
	/**
     *      ������Ļ����
     */
    void _modifyBrightness(ELNM_INPLACEBUTTON_ARGS* pItemNotify, LRESULT *pResult);

    /**
     *      �޸�ʱ������
     */
    void _modifyTime(ELNM_INPLACEBUTTON_ARGS* pItemNotify, LRESULT *pResult);
	/**
     *      ����ʱ��
     */
    void _loadTimezone();

    /**
     *      ����ʱ���б�˳��
     */
    void _adjustTimezone();

    /**
     *      �ָ�
     */
    int _partition(int nLeft, int nRight);

    /**
     *      ����ʱ���б��㷨
     */
    void _qsort(int nleft, int nRight);


	 /**
     *      ������Ϣ
     */
    struct stTimeInfo
    {
        CString csDisp;                     ///< ������ʾ�ַ���
        CString csDlt;                      ///< ����ʱ����
        CString csStd;                      ///< ������ʱ����
        TIME_ZONE_INFORMATION tzinfo;       ///< ʱ����Ϣ
        CString csKeyName;                  ///< ʱ����ע�������
    };
	std::vector<stTimeInfo*>  m_vecTimezone;    ///< ʱ����Ϣ�б�


	CReportH			m_ctlBasicSettingReport;		///< �������ñ���
	CString             m_csDefaultTZS;         ///< Ĭ��ʱ����ע�������	
#if !SHENG_ZE
	CReportH            m_reportBase;           ///< �������ñ���
#endif
};
