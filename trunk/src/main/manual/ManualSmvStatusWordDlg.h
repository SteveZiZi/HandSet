/// @file
///
/// @brief
///     设置状态字 头文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.7.18
///
/// 修改历史：
///

#pragma once


#include "src/main/dlg/BaseDlg.h"
#include "src/service/datalayer/dsmdatalayer.h"

enum MSSStatusWordType
{
    MSSWStatusWordUnknown = 0,                      ///< 未知
    MSSWStatusWord1,                                ///< 状态字1
    MSSWStatusWord2,                                ///< 状态字2
};

///
/// @brief
///    状态序列状态列表 扩展数据
///
class CManualSmvStatusWordDataExtra :public IPageDataExtra
{

public:
    CManualSmvStatusWordDataExtra(CPageData* pPageData)
        : IPageDataExtra()
        , m_pPageData(pPageData)
        , m_nFocusRowIndex(-1)
        , m_pBaseCb(NULL)
        , m_eStatusWordType(MSSWStatusWordUnknown)
    {

    }
    virtual ~CManualSmvStatusWordDataExtra()
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
    virtual CPageData* GetPageData()
    {
        return m_pPageData;
    }
public:
    int                     m_nFocusRowIndex;           ///< 用于恢复选中的行
    CBaseCb*                m_pBaseCb;                  ///< SMV发送控制块
    MSSStatusWordType       m_eStatusWordType;          ///< 状态字类型
    CPageData*              m_pPageData;                ///< 关联的页面数据指针

};


///
/// @brief
///      SMV状态字设置
///
class CManualSmvStatusWordDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CManualSmvStatusWordDlg)

public:
	CManualSmvStatusWordDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CManualSmvStatusWordDlg();

// 对话框数据
	enum { IDD = IDD_MANUAL_SMV_STATUS_WORD_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()


public:
    virtual BOOL OnInitDialog();
    afx_msg void OnNaviMenuSmvStatusWord(UINT nID);
    afx_msg void OnSmvStatusWordValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnInplaceEditAction(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnNaviMenuCtrlSel(UINT nID);
public:
    ///
    /// @brief
    ///      保存页面数据
    ///
    /// @return
    ///      void    
    ///
    virtual void _SavePage();

protected:
    ///
    /// @brief
    ///      创建报表
    ///
    /// @return
    ///      void    
    ///
    void _createReport();

    ///
    /// @brief
    ///      重新加载页面数据
    ///
    /// @return
    ///      void    
    ///
    virtual void _restorePage();
	///
	/// @brief
	///      重新加载菜单据
	///
	/// @return
	///      void    
	///
	void _resetMenu();

	///
	/// @brief
	///      重新加载菜单据
	///
	/// @return
	///      void    
	///
	void _updateReport();
protected:

    CReportH                m_ctlStatusWordReport;         ///< 状态字
    CBaseCb*                m_pBaseCb;
    CParamConfigSMVSend*    m_pParamConfigSMVSend;
    enumFrameType           m_nSmvSendType;                ///< SMV发送的类型
	typedef std::map<UINT, CBaseCb*>        CUINTCtrlCbMap;
	CUINTCtrlCbMap          m_ctrlCbMap;           ///< cmdID对应的goose控制块映射表

};
