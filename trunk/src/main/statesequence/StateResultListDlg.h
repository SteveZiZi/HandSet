/// @file
///
/// @brief
///     ״̬���в��Խ���б� ͷ�ļ�
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

// CStateResultListDlg �Ի���

///
/// @brief
///      ״̬���в��Խ���б�ҳ�� ���Ӱ�����
///
///
class CStateResultListDataExtra :public IPageDataExtra
{

public:
    CStateResultListDataExtra(CPageData* pPageData)
        : IPageDataExtra()
        , m_pPageData(pPageData)
        , m_nFocusRow(-1)
        , m_pStateTestResult(NULL)
    {
    }
    virtual ~CStateResultListDataExtra()
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
///      ״̬���в��Խ���б�ҳ�� �Ի���
///
///
class CStateResultListDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CStateResultListDlg)

public:
	CStateResultListDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CStateResultListDlg();

// �Ի�������
	enum { IDD = IDD_STATE_RESULT_LIST_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
    afx_msg void OnStateOkClick(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnNaviMenuStateResultList(UINT nID);
public:
    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()

public:
    ///
    /// @brief
    ///      ����ҳ������
    /// 
    /// @return
    ///      void    
    ///
    virtual void _SavePage();

protected:
    ///
    /// @brief
    ///      �ָ�ҳ������
    /// 
    /// @return
    ///      void    
    ///
    void _restorePage();
    ///
    /// @brief
    ///      ��������������
    /// 
    /// @return
    ///      void    
    ///
    void _createReport();

public:
      CReportV            m_ctlResultReport;        ///< �������
};
