/// @file
///
/// @brief
///     控制块通道品质列表 头文件
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    lqx  2013.10.18
///
/// 修改历史：
///
#pragma once

#include "src/main/dlg/BaseDlg.h"
#include "src/service/datalayer/dsmdatalayer.h"

// CManualSmvChanQualityDlg 对话框


class CManualSmvChanQualityDataExtra :public IPageDataExtra
{

public:
	CManualSmvChanQualityDataExtra(CPageData* pPageData)
		: IPageDataExtra()
		, m_pPageData(pPageData)
		, m_pBaseCb(NULL)
		, m_nFocusRow(-1)
		, m_pChannel(NULL)
	{
	}
	virtual ~CManualSmvChanQualityDataExtra()
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
	int                     m_nFocusRow;            ///< 选中行
	CBaseCb*                m_pBaseCb;
	CChannel*               m_pChannel;
	CPageData*              m_pPageData;            ///< 关联的页面数据指针

#if !CHUN_HUA
	CList<CChannel*>		m_pChannelList;
	CList<int>				m_nSelectIndex;
#endif
};

///
/// @brief
///      控制块通道品质列表
///
class CManualSmvChanQualityDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CManualSmvChanQualityDlg)

public:
	CManualSmvChanQualityDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CManualSmvChanQualityDlg();

	// 对话框数据
	enum { IDD = IDD_MANUAL_SMV_CHAN_QUALITY_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSmvQualityValueChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnInplaceEditAction(NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg void OnNaviMenuSmvChanQuality(UINT nID);
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
	///      更新通道品质
	///
	/// @return
	///      void    
	///
	void _updateChanQualityValue();
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
	///      更新报表
	///
	/// @return
	///      void    
	///
	void _updateReport();
	///
	/// @brief
	///      更新报表
	///
	/// @return
	///      void    
	///
	void _showNoneCtrlTips();

protected:

	CReportV                m_ctlChanQualityReport;         ///< 通道品质
	CIEC9_2Cb*              m_pIEC92Cb;                     ///< 控制块

	typedef std::map<UINT, CBaseCb*>        CUINTCtrlCbMap;
	CUINTCtrlCbMap          m_ctrlCbMap;           ///< cmdID对应的goose控制块映射表
	CBaseCb*			    m_pBaseCb;
};
