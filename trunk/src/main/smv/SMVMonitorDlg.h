/**
 * \file
 *      SMVMonitorDlg.h
 *
 * \brief
 *      ����ҳ��
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/4/21
 */



#pragma once

#include "src/main/smv/smvbasedlg.h"

#include "src/utils/reportcontrol/listctrlel/listctrlel.h"

// CSMVMonitorDlg �Ի���

#if SU_FEI

class CSMVMonitorDlg : public DSM_SMVBASEDLG
{
	DECLARE_DYNAMIC(CSMVMonitorDlg)

public:
	CSMVMonitorDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSMVMonitorDlg();

// �Ի�������
	enum { IDD = IDD_SMV_MONITOR_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual BOOL OnInitDialog();
    afx_msg void OnMenuF1();
    virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
    /**
     *      �޸Ĳ˵�HMENU
     */
    virtual void _modifyMenu() {}

    /**
     *      ��ֹ���ؿ��ƿ鵽F1�˵���
     */
    void _loadCtrlBlk() {}

    /**
     *      ���±���
     */
    virtual void _updateCaption() {}

    /**
     *      ��ʼ����ǰ���ƿ�
     */
    virtual void _initCurrentCtrlBlk() {}


private:
    /**
     *      ����ͨ��״̬����
     */
    void _createReport();

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
    void _initReport();

    /**
     *      ���µ�ǰ��ʾҳ�����Ϣ
     */
    void _updateReport();

    /**
     *      ���µײ�״̬��Ϣ
     */
    void _updateStatus(bool bNull = false);

    /**
     *      ���±���
     */
    void _updateCtrlBlkCaption();

    /**
     *      ���²˵�
     */
    void _updateMenu();

    /**
     *      ��ս����������ʾ��������������Ϊ "--"
     */
    void _updateNullUI();

    /**
     *      ���ͨ�����Ƿ�仯
     */
    bool _checkChannel();

private:
    /**
     *      ҳ��ģʽ,��ͨ���Ǻ���
     */
    enum  MODE_PAGE
    {
        PAGE_NORMAL,            ///< ��ͨҳ�棨��ʾ1�����ƿ飩
        PAGE_DUALPHASE          ///< ������ҳ�棨��ʾ2�����ƿ飩
    };

    /**
     *      ��ǰ��ʾ���ƿ�ı�ʶ
     */
    enum MODE_CTRL
    {
        CTRL_ONE,           ///< ��һ�����ƿ�
        CTRL_TWO            ///< �ڶ������ƿ�
    };



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

    CDataStatusWatch            m_data;                 ///< ״̬��������

    CReportV                    m_reportChannel;        ///< ͨ��״̬����
    CReportV                    m_reportChannel2;       ///< ͨ��״̬����2������ʱ�ã�
    CReportV*                   m_pCurrentReport;       ///< ��ǰ��ʾ�ı���

    CBaseDetectItem*            m_pCurCtrlBlk;          ///< ��ǰ���ƿ�
    CBaseDetectItem*            m_pCtrlBlk1;            ///< smv1
    CBaseDetectItem*            m_pCtrlBlk2;            ///< smv2

    MODE_PAGE                   m_ePage;                ///< ҳ��ģʽ,��ͨ���Ǻ���
    MODE_CTRL                   m_modeCtrl;             ///< ��ǰ��ʾ�Ŀ��ƿ��ʶ
};

#endif