/**
 * \file
 *      ScanSmvDlg.h
 *
 * \brief
 *      smv探测
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/4/19
 */

#pragma once

#include "src/main/dlg/BaseDlg.h"
#include "src/service/datalayer/dsmdatalayer.h"
#include "src/utils/reportcontrol/listctrlel/listctrlel.h"



namespace SMVSCAN
{
    enum SMVSCAN_PROTO_TYPE
    {
        PROTO_TYPE_9X,              ///< 探测9x类型
        PROTO_TYPE_FT3,             ///< 探测FT3类型
#if !SU_FEI
		PROTO_TYPE_GOOSE,			///<探测GOOSE类型
#endif
    };
}



// CScanSmvDlg 对话框

class CScanSmvDlg : public CBaseDlg
{
	DECLARE_DYNAMIC(CScanSmvDlg)

public:
	CScanSmvDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CScanSmvDlg();

// 对话框数据
	enum { IDD = IDD_SCAN_SMV_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnNaviMenuFn(UINT nID);
#if SU_FEI
    afx_msg void OnMenuSample(UINT nID);
#endif
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnItemChecked9X(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnItemCheckedFT3(NMHDR* pNotifyStruct, LRESULT* pResult);

#if !SU_FEI
	afx_msg void OnItemCheckGoose(NMHDR* pNotifyStruct, LRESULT* pResult);
#endif

public:
    /**
     *      保存页面状态数据
     */
    virtual void _SavePage();

protected:

    /**
     *      修改菜单HMENU
     */
    virtual void _modifyMenu();

private:
    /**
     *      创建报表
     */
    void _createReport(CReportV* pReport, UINT nID);

    /**
     *      创9x报表
     */
    void _insertColumn9x();

    /**
     *      创建FT3报表
     */
    void _insertColumnFT3();

#if !SU_FEI
	/**
     *      创建goose报表
     */
	void _insertColumnGoose();
	/**
     *      探测goose
     */
    void _scanGoose();
	
#endif

    /**
     *      探测9X
     */
    void _scan9X();

    /**
     *      探测FT3
     */
    void _scanFT3();

    /**
     *      插入9X探测结果到报表
     */
    void _insertRecordTo9X();

    /**
     *      插入FT3探测结果到报表
     */
    void _insertRecordToFT3();

#if !SU_FEI
	 /**
     *      插入GOOSE探测结果到报表
     */
    void _insertRecordToGoose();
#endif

    /**
     *      退出时释放资源
     */
    void _clearRes();

    /**
     *      设置所选控制块数据
     */
    bool _setSelected();

    /**
     *      清空报表的选择
     */
    void _clearSelect(CReportV* pReport);

    /**
     *      检查选中控制块的通道数是否符合限制
     *
     *  \note
     *      92协议12.8k的采样通道数不大于30
     */
    bool _checkChannels();

    /**
     *      更新菜单
     */
    void _updateMenu(SMVSCAN::SMVSCAN_PROTO_TYPE eType);

private:
    typedef CList<CBaseDetectItem*, CBaseDetectItem*>   CListItem;

    SMVSCAN::SMVSCAN_PROTO_TYPE     m_eDetectType;      ///< 区分当前探测协议

    UINT                    m_nScanTimerID;     ///< 探测定时器ID

    CString                 m_cs9XCaption;      ///< 探测9X标题
    CString                 m_csFT3Caption;     ///< 探测FT3标题
	
#if !SU_FEI
	CString					m_csGooseCaption;	///<探测Goose标题
#endif


    CReportV                m_ctl9X;            ///< 显示探测9x信息报表
    CReportV                m_ctlFT3;           ///< 显示探测FT3信息报表

#if !SU_FEI
	CReportV				m_ctlGOOSE;			///<显示探测GOOSE信息表
#endif

    CListItem               m_lstRecords;       ///< 探测到控制块列表

    CListItem               m_lstSelect;        ///< 所选的控制块列表

    CDsmDataLayer*          m_pDataLayer;       ///< 数据层指针
};
