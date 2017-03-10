/// @file
///
/// @brief
///     ״̬����״̬���� ͷ�ļ�
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    lqx  2013.7.18
///
/// �޸���ʷ��
///


#pragma once

#include "src/service/datalayer/config/ParamConfigStateSequence.h"
#include "src/main/dlg/basedlg.h"

#include "src/utils/ReportControl/ListBoxEL/TreeBoxEL.h"
//#include "src/service/datalayer/config/FaultCalc.h"
#include "src/service/datalayer/config/ParamConfigStateSequence.h"

class CStatus;

enum SSSmvFocusColType;
///
/// ������ʾ������
///
#if SU_FEI
enum SSShowReportType
{
    SSShowSetReport = 0,                                ///< ͨ������
    SSShowSmvDataReport                                   ///< ���ƿ���Ϣ����
};
#else
enum SSShowReportType
{
	SSShowNone =0,
	SSShowSetReport,                                ///< ͨ������
	SSShowSmvDataReport,                                  ///< ���ƿ���Ϣ����
	SSShowHarmonicReport,
	SSShowFaultComensateReport,
	SSShowSMVListReport,
	SSShowSmvChanMap
};
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
class CStateSetDataExtra :public IPageDataExtra
{

public:
    CStateSetDataExtra(CPageData* pPageData)
        : IPageDataExtra()
        , m_pPageData(pPageData)
        , m_nFocusRowIndex(-1)
        , m_nFocusColIndex(-1)
        , m_pStatus(NULL)
        , m_nIndex(0)
        , m_eShowReportType(SSShowSetReport)
#if !CHUN_HUA
		, m_eStateFaultSetType(eSFSTOther)
#endif
    {

    }
    virtual ~CStateSetDataExtra()
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
    SSShowReportType        m_eShowReportType;          ///< ��ʾ�ı�������
    int                     m_nFocusRowIndex;           ///< ���ڻָ�ѡ�е���
    int                     m_nFocusColIndex;           ///< ���ڻָ�ѡ�е���
    CStatus*                m_pStatus;                  ///< ״ֵ̬
    int                     m_nIndex;                   ///< ״̬����
    CPageData*              m_pPageData;                ///< ������ҳ������ָ��
#if !CHUN_HUA
	eStateFaultSetType		m_eStateFaultSetType;		///
#endif

};


///
/// @brief
///      ״̬����״̬���� �Ի���
///
class CStateSetDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CStateSetDlg)

public:
	CStateSetDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CStateSetDlg();


// �Ի�������
	enum { IDD = IDD_STATE_SET_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

    DECLARE_MESSAGE_MAP()
    afx_msg void OnNaviMenuChangeState(UINT nID);
    afx_msg void OnNaviMenuStateSet(UINT nID);
    afx_msg void OnStateSetItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnStateSetItemSelectChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnStateSmvDataItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnInplaceEditAction(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnDOItemOkClick(NMHDR* pNotifyStruct, LRESULT* pResult);
#if !CHUN_HUA
	//г������
	afx_msg void OnHarmonicItemValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnHarmonicItemSelectChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnInplaceHarmonicEditAction( NMHDR* pNotifyStruct, LRESULT* pResult );
	afx_msg void OnStateHarmonicContextMenu(UINT nID);

	//��·����
	afx_msg void OnFaultCalcItemSelectChanged( NMHDR* pNotifyStruct, LRESULT* pResult );
	afx_msg void OnFaultCalcItemValueChanged( NMHDR* pNotifyStruct, LRESULT* pResult );
	afx_msg void OnFaultCalcItemOkClick(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnFaultCalcInplaceEditAction( NMHDR* pNotifyStruct, LRESULT* pResult );
#endif
public:
    virtual BOOL OnInitDialog();
    virtual BOOL DestroyWindow();
    virtual BOOL PreTranslateMessage(MSG* pMsg);

	

	afx_msg void OnTimer(UINT_PTR nIDEvent);
#if !CHUN_HUA
	afx_msg void OnTreeBoxClick(NMHDR * pNotifyStruct, LRESULT* pResult);
	void confirmFaultCalExit();
#endif
public:
    ///
    /// @brief
    ///      ��ʾ״̬��������
    ///
    /// @return
    ///      void    
    ///
    void SetStatus(int nIndex, CStatus* pStatus);

    ///
    /// @brief
    ///      ����ҳ������
    ///
    /// @return
    ///      void    
    ///
    virtual void  _SavePage();
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
    ///      �޸Ĳ˵�
    ///
    /// @return
    ///      void    
    ///
    virtual void _modifyMenu();

    ///
    /// @brief
    ///      ����˵���
    ///
    /// @return
    ///      void    
    ///
    void _resetMenu();

    ///
    /// @brief
    ///      �޸�SMV�˵�F6
    ///
    /// @return
    ///      void    
    ///
    void _resetMenuSmvF6(SSSmvFocusColType smvFoucsColType);

    ///
    /// @brief
    ///      �����������ñ���
    ///
    /// @return
    ///      void    
    ///
    void _createSetReport();

#if !SHENG_ZE
	///
	/// @brief
	///      ������������DI����
	///
	/// @return
	///      void    
	///
	void _createSetReportDI();
#endif
    
    ///
    /// @brief
    ///      ����SMV���ݱ���
    ///
    /// @return
    ///      void    
    ///
    void _createSMVDataReport();
    ///
    /// @brief
    ///      �л����ͱ仯ʱ���±�������
    ///
    /// @return
    ///      void    
    ///
    void _updateOnSwitchTypeChange();
    ///
    /// @brief
    ///      ��ʾ״̬��������
    ///
    /// @return
    ///      void    
    ///
    void _showSetReport(bool bIsHide);
    ///
    /// @brief
    ///      ����F2��ѡ�е�״̬
    ///
    /// @return
    ///      void    
    ///
    void _highlightF2(BOOL bHighLight = TRUE);


    ///
    /// @brief
    ///      ��ʾSmv���ݱ���
    ///
    /// @return
    ///      void    
    ///
    void _showSmvDataReport(bool bIsHide);
    ///
    /// @brief
    ///      ״̬��ת
    ///
    /// @return
    ///      void    
    ///
    void _changeStateTo(int nStateIndex);

    ///
    /// @brief
    ///      �������ñ�������
    ///
    /// @return
    ///      void    
    ///
    void _updateStateSetReport();

    ///
    /// @brief
    ///      ����Smv��������
    ///
    /// @return
    ///      void    
    ///
    void _updateSmvDataReport();

	///
	/// @brief
	///      ��ʾ�Ҽ��˵�
	///
	/// @return
	///      void    
	///
	void _showContexMenu();
#if !CHUN_HUA
	///
	/// @brief
	///      ����г������
	///
	/// @return
	///      void    
	///
	void  _createHarmonicReport();
	///
	/// @brief
	///      ��ʾг������
	///
	/// @return
	///      void    
	///
	void  _showHarmonicReport(bool bIsHide);
	///
	/// @brief
	///      ���¼���report
	///
	/// @return
	///      void    
	///
	void _recontentReport();
	///
	/// @brief
	///      ��ʾг������
	///
	/// @return
	///      void    
	///
	void  _showReport(CListCtrlEL report);
	///
	/// @brief
	///      ��ʾг������
	///
	/// @return
	///      void    
	///
	void  _hideReport();
	///
	/// @brief
	///      ��ʾг������
	///
	/// @return
	///      void    
	///
	void _createSmvChanMapTree();
	///
	/// @brief
	///      ��ʾг������
	///
	/// @return
	///      void    
	///
	void _showSmvChanMapTree(bool bIsHide);
	///
	/// @brief
	///      ��ʾг������
	///
	/// @return
	///      void    
	///
	void _setHarmonicContextMenu();
	///
	/// @brief
	///      ��ʾг������
	///
	/// @return
	///      void    
	///
	void _setSmvContextMenu();
	///
	/// @brief
	///      ��ʾг������
	///
	/// @return
	///      void    
	///

	///
	/// @brief
	///      �������ϼ������ñ���
	///
	/// @return
	///      void    
	///
	void  _createFaultCalcReport();

	///
	/// @brief
	///      ���ݹ�����������report
	///
	/// @return
	///      void    
	///
	void _resetFaultCalcPartReport();
	///
	/// @brief
	///      ��ʾ���ϱ���
	///
	/// @return
	///      void    
	///
	void _showFaultCalcReport(BOOL bIsHide);


#endif
protected:

    typedef std::map<UINT, CStatus*>        CUINTStatusMap;
    CUINTStatusMap          m_statusMap;               ///< cmdID��Ӧ��״̬ӳ���
    SSShowReportType        m_eShowReportType;
    CReportH                m_ctlStateSetReport;       ///< ״̬���ñ���
    CReportV                m_ctlStateSMVDataReport;   ///< SMV���ݱ���
    CStatus*                m_pStatus;                 ///< ��ǰ״̬
    int                     m_nIndex;                  ///< ״̬����
    CParamConfigStateSequence*  m_pConfigStateSequ;    ///< ״̬����
#if !CHUN_HUA
	CNaviMenu				m_contextMenu;			   ///�Ҽ��˵�
	CReportV                m_ctlHarmonicReport;       ///< г������
	CListCtrlEL*			m_pCurrentReport;
	CTreeV                  m_ctlSmvChanMapTree;    ///< ͨ��ӳ����

	CParamconfigManualTest* m_pConfigManualTest;    ///< �ֶ�����������ù�����

	CReportH       m_ctlFaultCalcReport;		///< ���ϼ������ñ���
	CFaultCalc*    m_pFaultCalc;
	eStateFaultSetType		m_eStateFaultType;		//��������
#endif
#if !SHENG_ZE
	CReportH                m_ctlStateSetReportDI;       ///< ״̬���ñ���
	bool					m_DIFlag;					 ///< ��־λ
	bool					m_flag;
#endif
};
