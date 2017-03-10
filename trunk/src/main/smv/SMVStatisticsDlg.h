/**
 * \file
 *      SMVStatisticsDlg.h
 *
 * \brief
 *      ����ͳ������ɢ��ҳ��ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/7/11
 */

#pragma once

#include "src/main/smv/smvbasedlg.h"
#include "src/utils/electriccontrol/horhistogramctrl/horhistogramctrl.h"
#include "src/utils/reportcontrol/listctrlel/listctrlel.h"

#define STATISTICS_PILLARS          6           ///< ���θ���


// CSMVStatisticsDlg �Ի���

class CSMVStatisticsDlg : public DSM_SMVBASEDLG
{
	DECLARE_DYNAMIC(CSMVStatisticsDlg)

public:
	CSMVStatisticsDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSMVStatisticsDlg();

// �Ի�������
	enum { IDD = IDD_SMV_STATISTICS_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

protected:
    //virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnPaint();
    afx_msg void OnMenuF3();
    afx_msg void OnMenuF6();

#if !SU_FEI
	
	afx_msg void OnMenuF4();
	afx_msg void OnMenuF5();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
#endif 
    //virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

public:

    /**
     *      ����ҳ��
     */
    virtual void _SavePage();

protected:

    /**
     *      �޸Ĳ˵�HMENU
     */
    virtual void _modifyMenu();

    /**
     *      ���ݲ˵�ID�л���ָ���Ŀ��ƿ�
     *
     *  /param[in]
     *      nCmdID      ���ƿ�����Ĳ˵�����ID
     */
    virtual bool _switchCtrlBlk(UINT nCmdID);


private:


    /**
     *      ��������
     */
    void _createReport();

    /**
     *      ������״ͼ
     */
    void _createHorHistogram();

    /**
     *      ���±���ͳ����Ϣ
     */
    void _update();

    /**
     *      ������ɢ�ȱ���
     */
    void _resetPillarTitle();

//     /**
//      *      ���ò����ʲ˵�
//      */
//     void _setSampleRateMenu();
// 
//     /**
//      *      ����92������
//      */
//     void _setSampleRate92(UINT nCmdID);

#if !SU_FEI
	///< ���õ�ǰ��ʾģʽ
	enum	SMV_STATISTICS_MODE 
	{
		REPORT_MODE,		///< ����ͳ��ģʽ
		STATISTICS_MODE,	///< ��ɢ��ģʽ
		MONITOR_MODE		///< ���ģʽ
	};


	  /**
     *      ����ͨ��״̬����
     */
    void _createMonitorReport();

    /**
     *      ����92��
     */
    void _insertColumn92();

    /**
     *      ����91��FT3��
     */
    void _insertColumnFT3();

    /**
     *      ������ʾ��ǰ���ƿ�ͨ��״̬��Ϣ
     */
    void _initMonitorReport();
    /**
     *      ���µײ�״̬��Ϣ
     */
    void _updateStatus(bool bNull = false);

    /**
     *      ���µײ�״̬��Ϣ
     */
    void _updateMonitorStatus(bool bNull = false);
    /**
     *      ��ս����������ʾ��������������Ϊ "--"
     */
    void _updateNullUI();

    /**
     *      ���ͨ�����Ƿ�仯
     */
    bool _checkChannel();

private:

	CString                     m_csMUSync;             ///< MU - ͬ��
	CString                     m_csMUNotSync;          ///< MU -��ͬ��
	CString                     m_csMUNULL;             ///< MU -��ͬ��
	CString                     m_csWarnGood;           ///< ���ϱ��� - ����
	CString                     m_csWarnBad;            ///< ���ϱ��� - ά��
	CString                     m_csWarnNULL;           ///< ���ϱ��� - ά��
	CString                     m_csFixConn;            ///< ���� - ��ͨ
	CString                     m_csFixTest;            ///< ���� - ����
	CString                     m_csFixNULL;            ///< ���� - ����
	CString                     m_csWakeValid;          ///< ���� - ������Ч
	CString                     m_csWakeInvalid;        ///< ���� - ������Ч
	CString                     m_csWakeNULL;           ///< ���� - ������Ч

	CRect                       m_rcStatus;             ///< �ײ�״̬����
	CRect                       m_rc92MU;               ///< 92MUͬ������
	CRect                       m_rcFT3MU;              ///< FT3MUͬ������
	CRect                       m_rcFT3Warn;            ///< FT3���ϱ�������
	CRect                       m_rcFT3Wake;            ///< FT3��������
	CRect                       m_rcFT3Fix;             ///< FT3��������

	CBitmap                     m_panelStatus;          ///< ״̬������

	// 92           DSM_SMV_MONITOR_PROTO_92            0x01
	// 91 FT3       DSM_SMV_MONITOR_PROTO_91FT3         0x02
	// δ֪    
	DWORD                       m_dwTag;                ///< Э���־

	int                         m_nBegin;               ///< ��ʾҳ���һ������
	int                         m_nEnd;                 ///< ��ʾҳ�����һ������

	CDataStatusWatch            m_monitordata;                 ///< ״̬��������
	CReportV                    m_reportMonitor;       ///< ��ǰ��ʾ�ı���
#endif




private:

    CRect                   m_rcTop1;               ///< �������
    CRect                   m_rcTop2;               ///< ��ɢͼ����

    CReportV                m_reportStatistics;     ///< ����ͳ�����ݱ���

    float                   m_fdata[STATISTICS_PILLARS];
    MSG_DISP_PERIOD         m_etypes[STATISTICS_PILLARS];

    CHorHistogramCtrl       m_ctrlHistogram;        ///< ������״ͼ
    CDataFrameStatistics    m_data;                 ///< ����ͳ������
    CString                 m_csSampleInter;        ///< �����������
    CString                 m_csSample;             ///< ������
#if !SU_FEI
	
	SMV_STATISTICS_MODE		m_modeCtr;				///< ��ǰ��ʾģʽ

#endif
};
