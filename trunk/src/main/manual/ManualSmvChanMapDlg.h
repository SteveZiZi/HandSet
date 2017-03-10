/// @file
///
/// @brief
///     SMVͨ��ӳ�� ͷ�ļ�
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    lqx  2013.7.18
///
/// �޸���ʷ��
///

#pragma once
#include "src/main/dlg/basedlg.h"
#include "src/utils/ReportControl/ListBoxEL/TreeBoxEL.h"
// CManualSmvChanMapDlg �Ի���

class CParamconfigManualTest;


///
/// @brief
///      SMVͨ��ӳ�� �Ի���
///
class CManualSmvChanMapDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CManualSmvChanMapDlg)

public:
	CManualSmvChanMapDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CManualSmvChanMapDlg();

// �Ի�������
	enum { IDD = IDD_MANUAL_SMV_CHAN_MAP_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

    DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnTreeBoxClick(NMHDR * pNotifyStruct, LRESULT* pResult);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual BOOL DestroyWindow();

protected:

    ///
    /// @brief
    ///      ����SMVͨ��ӳ��
    /// 
    /// @return
    ///      void    
    ///
    void _createSmvChanMapTree();
protected:
    
    CTreeV                  m_ctlSmvChanMapTree;    ///< ͨ��ӳ����
    CParamconfigManualTest* m_pConfigManualTest;    ///< �ֶ�����������ù�����
};
