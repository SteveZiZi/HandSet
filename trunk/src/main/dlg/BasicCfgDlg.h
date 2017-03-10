/**
 * \file
 *      BasicCfgDlg.h
 *
 * \brief
 *      dsm��������ҳ��ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
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


// CBasicCfgDlg �Ի���

class CBasicCfgDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CBasicCfgDlg)

public:
	CBasicCfgDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CBasicCfgDlg();

// �Ի�������
	enum { IDD = IDD_BASIC_CFG_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
     *      ���������ú���
     */
    void _createReport(CReportH& report, UINT nID);

    /**
     *      ��������ʼ���������ñ���
     */
    void _initReportBase();

    /**
     *      ��������ʼ����ԭ������
     */
    void _initReportRestore();

    /**
     *      ��������ʼ�����ڱ���
     */
    void _initReportAbout();

    /**
     *      ��������ʼ�����ڱ������ϸ��Ϣ
     */
    void _initReportDetail();

    /**
     *      �����洢������
     */
    void _initReportStorage();

    /**
     *      �����������ҳ��
     */
    void _enterBase();

    /**
     *      �������ӹ���ҳ��
     */
    void _enterLinkmgr();

    /**
     *      ����洢��
     */
    void _enterStorage();

    /**
     *      ���뻹ԭ����
     */
    void _enterRestore();

    /**
     *      �������ҳ��
     */
    void _enterAbout();

    /**
     *      ��ʼ�����˵�
     */
    void _initMenuNavi();

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
     *      ������Ļ����
     */
    void _modifyBrightness(ELNM_INPLACEBUTTON_ARGS* pItemNotify, LRESULT *pResult);

    /**
     *      �޸�ʱ������
     */
    void _modifyTime(ELNM_INPLACEBUTTON_ARGS* pItemNotify, LRESULT *pResult);

    /**
     *      ���±���
     */
    void _updateCaption(UINT nCaption);

    /**
     *      �뿪�����޸�ģʽ
     */
    void _leavePWMode();

    /**
     *      ���������޸�ģʽ
     */
    void _enterPWMode();

    /**
     *      ����USB
     */
    void _setLink();

    /**
     *      �Ͽ�����USB
     */
    void _setUnLink();

    /**
     *      ��ʼ��ͼƬ�ؼ�
     */
    void _showPic();

    /**
     *      ����Ĭ��ʱ��
     */
    void _setDefaultTZS();


private:

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

    /**
     *      USB����ģʽ
     */
    enum MODE_USB
    {
        USB_LINKED,             ///< USB��ռ
        USB_UNLINK              ///< USB�Ƕ�ռ
    };

    /**
     *      �豸��Ϣ
     */
    enum MODE_INFO
    {
        INFO_BASE,              ///< ������Ϣ
        INFO_DETAIL,            ///< ��ϸ��Ϣ
    };

private:

    bool                m_bWizard;              ///< ��ģʽ��һ������ģʽ

    bool                m_bPWNull;              ///< ������
    bool                m_bModifyPassword;      ///< �޸�����״̬
    CString             m_csPasswordNew;        ///< ������
    CString             m_csPasswordConfirm;    ///< ȷ��������
    UINT                m_nTimerLinkStatus;     ///< usb����״̬��ʱ��

    CReportH            m_reportBase;           ///< �������ñ���
    CReportH            m_reportRestore;        ///< ��ԭ����
    CReportAbout        m_reportAbout;          ///< ���ڱ���
    CReportH            m_reportDetail;         ///< ���ڱ�����ϸ��Ϣ
    CReportH            m_reportStorage;        ///< �洢������

    CMenuEx             m_menuNavi;             ///< ��ർ���˵�
    CListCtrlEL*        m_pCurReport;           ///< ��ǰ�����ı���ָ��

    MODE_USB            m_eUSBMode;             ///< ��ǰUSBģʽ
    CStatusDisp         m_ctlPic;               ///< ͼƬ�ؼ�

    CString             m_csDefaultTZS;         ///< Ĭ��ʱ����ע�������
    std::vector<stTimeInfo*>  m_vecTimezone;    ///< ʱ����Ϣ�б�

    MODE_INFO           m_eInfo;                ///< ��Ϣ��ʾģʽ
    unsigned int        m_nCountDown;           ///< usb���ӵ�ʱ����
};

