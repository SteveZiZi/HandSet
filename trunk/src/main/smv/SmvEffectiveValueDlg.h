#pragma once

#include "src/main/resource.h"
#include "src/main/smv/smvbasedlg.h"
#include "src/service/datalayer/include/datatypes.h"
// CSmvEffectiveValueDlg 对话框

class CPageDataExtra;


class CSmvEffectiveValueDlg : public DSM_SMVBASEDLG
{
	DECLARE_DYNAMIC(CSmvEffectiveValueDlg)

public:
	CSmvEffectiveValueDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSmvEffectiveValueDlg();

// 对话框数据
	enum { IDD = IDD_SMV_EFFECTIVE_VALUE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);

public:

    /**
     *      保存页面数据
     */
    void _SavePage();
	virtual void _modifyMenu();
protected:

    /**
     *      创建报表
     */
    void _createReport();

    /**
     *      重设报表
     */
    void _resetReport();
    /**
     *      插入记录到报表
     */
    void _updateEffectiveValue();
    /**
     *      获取通道名称
     */
    bool _restorePage();
    /**
     *      根据菜单ID切换到指定的控制块
     *
     *  /param[in]
     *      nCmdID      控制块关联的菜单命令ID
     */
    bool _switchCtrlBlk( UINT nCmdID );
    /**
     *      检查通道是否发生变化
     */
    bool _checkChannel();

private:

    CDataEffectiveVal    m_dataEffVal;
    CReportV             m_ctlEffValReport;                     ///< 有效值报表
};
