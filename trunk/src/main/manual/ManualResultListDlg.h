/// @file
///
/// @brief
///     �������б� ͷ�ļ�
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

///
/// @brief
///      ҳ���������չ
///
///
class CManualResultListDataExtra :public IPageDataExtra
{

public:
    CManualResultListDataExtra(CPageData* pPageData)
        : IPageDataExtra()
        , m_pPageData(pPageData)
        , m_nFocusRow(-1)
        , m_pStateTestResult(NULL)
    {
    }
    virtual ~CManualResultListDataExtra()
    {
    }

public:

    ///
    /// @brief
    ///      ��ȡ����������󶨵�����ҳ������ָ��
    ///
    /// @return
    ///      CPageData*    
    ///
    virtual CPageData* GetPageData()
    {
        return m_pPageData;
    }
public:
    int                     m_nFocusRow;                ///< ѡ�е���
    CStateTestResult*       m_pStateTestResult;         ///< ѡ��״̬�������
    CPageData*              m_pPageData;                ///< ������ҳ������ָ��
};

///
/// @brief
///      �������б�
///
class CManualResultListDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CManualResultListDlg)

public:
	CManualResultListDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CManualResultListDlg();

// �Ի�������
	enum { IDD = IDD_MANUAL_RESULT_LIST_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
    afx_msg void OnStateOkClick(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnNaviMenuManualResultList(UINT nID);

public:
    virtual BOOL OnInitDialog();

public:
    ///
    /// @brief
    ///      ����ҳ������
    ///
    /// @return
    ///      CPageData*    
    ///
    virtual void _SavePage();

protected:
    ///
    /// @brief
    ///      ��������������
    /// 
    /// @return
    ///      void    
    ///
    void _createReport();
    ///
    /// @brief
    ///      �ָ�����
    /// 
    /// @return
    ///      void    
    ///
    void _restorePage();
protected:

    CReportV             m_ctlResultReport;          ///< ���ñ���

};
