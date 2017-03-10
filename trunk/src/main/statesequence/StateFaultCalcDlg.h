/// @file
///
/// @brief
///     ���ϼ��� ͷ�ļ�
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
#include "src/service/datalayer/config/FaultCalc.h"
#include "src/service/datalayer/config/ParamConfigStateSequence.h"
// CStateFaultCalcDlg �Ի���
#if CHUN_HUA
enum eStateFaultSetType
{
    eSFSTOther = 0,
    eSFSTImpedanceZ1,
    eSFSTCompensateK1,
    eSFSTImpedanceZs,
    eSFSTCompensateKs,
};
#endif
///
/// @brief
///    ״̬����״̬�б� ��չ����
///
class CStateFaultCalcDataExtra :public IPageDataExtra
{

public:
    CStateFaultCalcDataExtra(CPageData* pPageData)
#if CHUN_HUA
        : IPageDataExtra()
        , m_pPageData(pPageData)
        , m_nFocusRowIndex(-1)
        , m_eStateFaultSetType(eSFSTOther)
#endif
    {

    }
    virtual ~CStateFaultCalcDataExtra()
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
#if CHUN_HUA
    virtual CPageData* GetPageData()
    {
        return m_pPageData;
    }
public:
    
    eStateFaultSetType      m_eStateFaultSetType;       ///< ��ʶOnClickҪ�޸���һ��
    int                     m_nFocusRowIndex;           ///< ���ڻָ�ѡ�е���
    CPageData*              m_pPageData;                ///< ������ҳ������ָ��
#endif
};



class CStateFaultCalcDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CStateFaultCalcDlg)

public:
	CStateFaultCalcDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CStateFaultCalcDlg();

// �Ի�������
	enum { IDD = IDD_STATE_FAULT_CALC_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
#if SU_FEI
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnItemSelectChanged( NMHDR* pNotifyStruct, LRESULT* pResult );
    afx_msg void OnItemValueChanged( NMHDR* pNotifyStruct, LRESULT* pResult );
    afx_msg void OnItemOkClick(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnInplaceEditAction( NMHDR* pNotifyStruct, LRESULT* pResult );
    virtual BOOL PreTranslateMessage(MSG* pMsg);

public:
    ///
    /// @brief
    ///      �������ϼ������ñ���
    ///
    /// @return
    ///      void    
    ///
    void  _createReport();

    ///
    /// @brief
    ///      ���ݹ�����������report
    ///
    /// @return
    ///      void    
    ///
    void _resetPartReport();
    
    ///
    /// @brief
    ///      ����ҳ������
    ///
    /// @return
    ///      void    
    ///
    void _restorePage();

public:
    ///
    /// @brief
    ///      ����ҳ������
    ///
    /// @return
    ///      void    
    ///
    void _SavePage();
#endif
public:
    CReportH       m_ctlFaultCalcReport;       ///< ���ϼ������ñ���
    CFaultCalc*    m_pFaultCalc;
    CStatus*       m_pStatus;                  ///< ״ֵ̬
    CParamConfigStateSequence*   m_pConfigStateSequ;

};
