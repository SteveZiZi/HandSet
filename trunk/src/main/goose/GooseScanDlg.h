/**
 * \file
 *      GooseScanDlg.h
 *
 * \brief
 *      ̽��goose���ƿ�ҳ��ͷ�ļ�
 *
 * \copyright
 *      Copyright(c) 2016 ������ͨ�����Ƽ����޹�˾
 *
 * \author
 *      chao      2013/5/8
 */

#pragma once

#include "src/main/dlg/basedlg.h"

class CDsmDataLayer;
class CBaseDetectItem;

// CGooseScanDlg �Ի���

class CGooseScanDlg : public DSM_BASEDLG
{
	DECLARE_DYNAMIC(CGooseScanDlg)

public:
	CGooseScanDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CGooseScanDlg();

// �Ի�������
	enum { IDD = IDD_GOOSE_SCAN_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnNaviMenuFn(UINT nID);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnItemChecked(NMHDR* pNotifyStruct, LRESULT* pResult);


private:
    /**
     *      ����̽��goose����
     */
    void _createReport();

    /**
     *      ����̽��
     */
    void _rescan();

    /**
     *      ���ѡ��
     */
    void _clearSelect();

    /**
     *      ������ѡ���ƿ�
     */
    bool _setSelect();

    /**
     *      ��������
     */
    void _clearRes();

    /**
     *      ����̽��������
     */
    void _insertRecord();


private:

    UINT                    m_nScanTimerID;     ///< ��ʱˢ��̽�����Ķ�ʱ��ID

    CReportV                m_reportGoose;      ///< ̽��goose�������

    typedef CList<CBaseDetectItem*, CBaseDetectItem*>   CListItem;

    CListItem               m_lstDetects;       ///< ̽�⵽����Ϣ�б�
    CListItem               m_lstSelects;       ///< ѡ�е���Ϣ�б�
    CDsmDataLayer*          m_pDataLayer;       ///< ���ݲ�ָ��
};
