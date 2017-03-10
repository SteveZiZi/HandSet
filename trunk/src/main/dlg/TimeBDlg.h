/**
 * \file
 *      TimeBDlg.h
 *
 * \brief
 *      ��ʱҵ���ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/6/13
 */

#pragma once

#include "src/main/dlg/basedlg.h"
#include "src/service/datalayer/dsmdatalayer.h"

#define DSM_TIMETICK_TIMER_ID               0x84        ///< ��ʱ��ID
#define DSM_TIMETICK_TIMER_INTERVAL         400         ///< 300ms��ʱ�����
#if !SHENG_ZE
#define OPTICAL_STATUS(n) ((n)&0x3FF)
#endif
class CRNaviMenu;
class CDsmDataLayer;

// CTimeBDlg �Ի���

class CTimeBDlg : public DSM_BASEDLG
{
	DECLARE_DYNAMIC(CTimeBDlg)

public:
	CTimeBDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTimeBDlg();

// �Ի�������
	enum { IDD = IDD_TIME_B_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnMenuF1();
    afx_msg void OnMenuF2();
    afx_msg void OnMenuF6();
    afx_msg void OnDestroy();
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnSelectClock(NMHDR* pNotifyStruct, LRESULT* pResult);

protected:
    /**
     *      �޸Ĳ˵�HMENU
     */
    virtual void _modifyMenu();

private:

    /**
     *      ����B���ʱ����
     */
    void _createReportB();

    /**
     *      ����1588��ʱ����
     */
    void _createReport1588();

    /**
     *      ����1588ʱ����Ϣ����
     */
    void _createReportTime();

    /**
     *      �л���ʱ��������
     */
    void _switchReport();

    /**
     *      ���²˵��Լ�������ʾ
     */
    void _updateDisp();

    
    /**
     *      ����Ĭ�϶�ʱ��
     */
    void _beginTimer(UINT nTimeout = DSM_TIMETICK_TIMER_INTERVAL);

    /**
     *      ֹͣĬ�϶�ʱ��
     */
    void _stopTimer();

    /**
     *      ���½�������
     */
    void _updateData();

    /**
     *      ˢ��B������
     */
    void _updateIRIGB();

    /**
     *      ˢ��1588̽��ʱ��Դ����
     */
    void _update1588Scan();

    /**
     *      ˢ��1588��ϸʱ����Ϣ
     */
    void _update1588Detail();


private:

    enum TimeMode
    {
        TIME_B,
        TIME_1588,
        TIME_1588_DETAIL
    };

private:

    CReportV                m_reportB;          ///< B�뱨��
    CReportV                m_report1588;       ///< 1588����
    CReportV                m_reportTime;       ///< 1588ʱ����Ϣ

    UINT_PTR                m_nTimerID;         ///< ˢ�����ݶ�ʱ��ID
    TimeMode                m_eTimeMode;        ///< ��ǰʱ���ʱģʽ
    CDsmDataLayer*          m_pDataLayer;       ///< ���ݲ�ָ��
    CDataTimeSyncIRIGB      m_dataIRIGB;        ///< B������
    CRNaviMenu*             m_pRNaviMenu;       ///< �˵��������

    CString                 m_csDetailMac;      ///< 1588��ϸʱ����Ϣʱ��Դmac��ַ
    CString                 m_csDetailID;       ///< 1588��ϸʱ����Ϣʱ��ԴID
    CString                 m_csPort;           ///< 1588ʱ��������صĶ˿ں�
#if !SHENG_ZE
	bool					m_BtimeFlag;        ///< B����ʱ��־
#endif
};
