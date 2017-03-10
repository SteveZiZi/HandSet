/// @file
///
/// @brief
///     故障计算 头文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.7.18
///
/// 修改历史：
///

#pragma once
#include "src/main/dlg/basedlg.h"
#include "src/service/datalayer/config/FaultCalc.h"
#include "src/service/datalayer/config/ParamConfigStateSequence.h"
// CStateFaultCalcDlg 对话框
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
///    状态序列状态列表 扩展数据
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
    ///      获取附加数据类绑定的数据页面数据指针
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
    
    eStateFaultSetType      m_eStateFaultSetType;       ///< 标识OnClick要修改哪一项
    int                     m_nFocusRowIndex;           ///< 用于恢复选中的行
    CPageData*              m_pPageData;                ///< 关联的页面数据指针
#endif
};



class CStateFaultCalcDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CStateFaultCalcDlg)

public:
	CStateFaultCalcDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CStateFaultCalcDlg();

// 对话框数据
	enum { IDD = IDD_STATE_FAULT_CALC_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
    ///      创建故障计算设置报表
    ///
    /// @return
    ///      void    
    ///
    void  _createReport();

    ///
    /// @brief
    ///      根据故障类型设置report
    ///
    /// @return
    ///      void    
    ///
    void _resetPartReport();
    
    ///
    /// @brief
    ///      保存页面数据
    ///
    /// @return
    ///      void    
    ///
    void _restorePage();

public:
    ///
    /// @brief
    ///      保存页面数据
    ///
    /// @return
    ///      void    
    ///
    void _SavePage();
#endif
public:
    CReportH       m_ctlFaultCalcReport;       ///< 故障计算设置报表
    CFaultCalc*    m_pFaultCalc;
    CStatus*       m_pStatus;                  ///< 状态值
    CParamConfigStateSequence*   m_pConfigStateSequ;

};
