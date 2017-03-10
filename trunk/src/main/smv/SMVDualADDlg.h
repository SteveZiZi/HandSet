/**
 * \file
 *      SMVDualADDlg.h
 *
 * \brief
 *      ˫ADҳ��ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/4/22
 */

#pragma once


#include "src/main/smv/smvbasedlg.h"


class CCtrlBlkDataDualAD;


// CSMVDualADDlg �Ի���

class CSMVDualADDlg : public DSM_SMVBASEDLG
{
	DECLARE_DYNAMIC(CSMVDualADDlg)

public:
	CSMVDualADDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSMVDualADDlg();

// �Ի�������
	enum { IDD = IDD_SMV_DUALAD_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnItemSelectChange( NMHDR* pNotifyStruct, LRESULT *pResult);
    afx_msg void OnInplaceButtonDown( NMHDR* pNotifyStruct, LRESULT *pResult);

protected:

    /**
     *      ���ݲ˵�ID�л���ָ���Ŀ��ƿ�
     *
     *  \param[in]
     *      nCmdID          ���ƿ�ӳ�������˵�ID
     */
    virtual bool _switchCtrlBlk(UINT nCmdID);


public:

    /**
     *      ����ҳ��
     */
    virtual void _SavePage();

	virtual void _modifyMenu();
private:

    /**
     *      ����˫AD����
     */
    void _createReport();

    /**
     *      ��ʼ������
     */
    void _resetReport();

    /**
     *      �ָ�ҳ��������Ϣ
     */
    bool _restorePage();

    /**
     *      ����Ĭ����ʾͨ��
     */
    void _adjustChannel();

    /**
     *      Ѱ��Ĭ����ʾͨ��
     *
     *  \param[in]
     *      ���ƿ������ҳ��״̬���ݶ���
     *
     *  \note
     *      ���ڲ�����һ����Ϊһ��
     */
    void _calcChannel(CCtrlBlkDataDualAD* pData);

    /**
     *      ���½�������
     */
    void _update();

    /**
     *      ���ͨ���Ƿ����仯
     */
    bool _checkChannel();

private:

    CReportV                m_reportAD;             ///< ˫AD������ʾ����
    CDataDoubleAD           m_data;                 ///< ˫AD����
};
