/// @file
///
/// @brief
///     SMV���ƿ��б� ͷ�ļ�
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    lqx  2013.7.18
///
/// �޸���ʷ��
///

#pragma once

#include "src/main/dlg/BaseDlg.h"

#include "src/service/datalayer/dsmdatalayer.h"
// CManualSmvCtrlListDlg �Ի���


class CManualSmvCtrlListDataExtra :public IPageDataExtra
{

public:
    CManualSmvCtrlListDataExtra(CPageData* pPageData)
        : IPageDataExtra()
        , m_pPageData(pPageData)
        , m_pBaseCb(NULL)
        , m_nFocusRow(-1)
    {
    }
    virtual ~CManualSmvCtrlListDataExtra()
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
    int                     m_nFocusRow;            ///< ѡ����
    CBaseCb*                m_pBaseCb;
    CPageData*              m_pPageData;            ///< ������ҳ������ָ��
};





///
/// @brief
///      SMV���Ϳ��ƿ��б�
///
class CManualSmvCtrlListDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CManualSmvCtrlListDlg)

public:
	CManualSmvCtrlListDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CManualSmvCtrlListDlg();

// �Ի�������
	enum { IDD = IDD_MANUAL_SMV_CTRL_LIST_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnSmvCtrlOkClick(NMHDR* pNotifyStruct, LRESULT* pResult);


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
    ///      ��������
    ///
    /// @return
    ///      void    
    ///
    void _createReport();

    ///
    /// @brief
    ///      ���¼���ҳ������
    ///
    /// @return
    ///      void    
    ///
    virtual void _restorePage();

protected:

    CReportV                m_ctlSmvCtrlReport;         ///< smv����
    CParamConfigSMVSend*    m_pParamConfigSMVSend;


};
