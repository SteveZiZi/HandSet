#pragma once

#include "src/main/resource.h"
#include "src/main/common.h"
#include "src/main/resourcestr.h"
#include "src/utils/reportcontrol/listctrlel/listctrlel.h"
#include "src/utils/ElectricControl/HistogramCtrl/HistogramCtrl.h"

#include "src/main/smv/smvbasedlg.h"

// CSmvHarmonicDlg 对话框


class CPageDataExtra;


enum SHShowReportType;

class CSmvHarmonicDlg : public DSM_SMVBASEDLG
{
	DECLARE_DYNAMIC(CSmvHarmonicDlg)

public:
	CSmvHarmonicDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSmvHarmonicDlg();

// 对话框数据
	enum { IDD = IDD_SMV_HARMONIC_DLG };
private:

    SHShowReportType        m_eShowType;                                       ///< 显示方式
    int                     m_nChannelCount;                                    ///< 总通道数

    CReportV                m_ctlHarmonicReport;                                ///< 有效值报表
    int                     m_nHarmRepLeftColIndex;                             ///< 有效值报表最左边列的索引

    CReportV                m_ctlRateReport;                                    ///< 含有率报表
    int                     m_nRateRepLeftColIndex;                             ///< 含有率报表最左边列的索引

    CHistogramCtrl          m_ctlHarmonicHistogram;                             ///< 柱状图控件
    int                     m_nHisCurPageIndex;                                 ///< 柱状图当前页
#if !SU_FEI
	bool					m_bHisFlag;
#endif
    CDataHarmonic           m_dataHarmonic;

protected:

    
    /**
     *      创建含有率控件
     */
    void _createRateReport();
    /**
     *      显示含有率报表
     */
    void _showRateReport();
    /**
     *      重设含有率报表的行数
     */
    void _resetRateReport();
    /**
     *      更新含有率报表数据
     */
    void _updateRateReport();
    /**
     *      更新含有率报表表头
     */
    void _updateRateReportColumnTitle();

    /**
     *      创建有效值控件
     */
    void _createHarmReport();
    /**
     *      重设含有率报表的行数
     */
    void _resetHarmReport();
    
    /**
     *      显示有效值报表
     */
    void _showHarmReport();
    /**
     *      更新有效值报表数据
     */
    void _updateHarmReport();
    /**
     *      更新有效值报表表头
     */
    void _updateHarmReportColumnTitle();


    /**
     *      创建Histogram
     */
    void _createHistogram();
    /**
     *      显示柱状图
     */
    void _showHistogram();
    /**
     *      更行柱状图
     */
    void _updateHistorgram();
    /**
     *      更行柱状图通道描述
     */
    void _updateHistorgramTitle();

    /**
     *      切换显示方式
     */
    void _changeShowType();

    /**
     *      修改菜单HMENU
     */
    virtual void _modifyMenu();


    /**
     *      根据菜单ID切换到指定的控制块
     *
     *  \note
     *      子类选择控制块需要重载该方法做一些补充操作
     */
    virtual bool _switchCtrlBlk(UINT nCmdID);

    /**
     *      检测通道数是否修改
     */
    bool _checkChannel();

    /**
     *      恢复页面状态
     */
    bool _restorePage();

public:
    /**
     *      保存页面数据
     */
    void CSmvHarmonicDlg::_SavePage();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnMenuF4();
#if !SU_FEI
	afx_msg void OnMenuF5();
#endif
};
