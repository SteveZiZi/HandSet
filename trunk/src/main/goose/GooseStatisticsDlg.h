/**
 * \file
 *      GooseStatisticsDlg.h
 *
 * \brief
 *      goose����ͳ��ҳ��ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/5/10
 */

#pragma once

#include "src/main/goose/goosebasedlg.h"


class CPageDataExtra;

// CGooseStatisticsDlg �Ի���

class CGooseStatisticsDlg : public DSM_GOOSEASEDLG
{
	DECLARE_DYNAMIC(CGooseStatisticsDlg)

public:
	CGooseStatisticsDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CGooseStatisticsDlg();

// �Ի�������
	enum { IDD = IDD_GOOSE_STATISTICS_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnMenuF3();
    afx_msg void OnMenuF6();
    afx_msg void OnTimer(UINT_PTR nIDEvent);

public:

    /**
     *      ����ҳ��
     */
    virtual void _SavePage();

protected:
    /**
     *      �޸ĵײ��˵�
     */
    virtual void _modifyMenu();

private:
    /**
     *      �������ļ��ӱ���
     */
    void _createReport();

    /**
     *      ���±���ͳ����Ϣ
     */
    void _updateData();

private:

    CRect                   m_rcTop1;               ///< �������
    CGooseFrameStatistics   m_data;                 ///< ����ͳ������
    CReportV                m_reportGoose;          ///< ����ͳ�Ʊ���

private:
    CString                 m_csTotal;              ///< �����ܼ���
    CString                 m_csLostFrame;          ///< ��֡����
    CString                 m_csMisOrder;           ///< �������
    CString                 m_csRetry;              ///< �ط�����
    CString                 m_csStLost;             ///< StNum��֡����
    CString                 m_csStMis;              ///< StNum�������
    CString                 m_csLostData;           ///< ���Ķ�ʧ����
    CString                 m_csCommBreak;          ///< ͨ���жϼ���
    CString                 m_csInvalidTTL;         ///< ���ʱ����Ч����
    CString                 m_csTimeNotSync;        ///< ʱ��δͬ������
    CString                 m_csTimeFault;          ///< ʱ�ӹ��ϼ���
};
