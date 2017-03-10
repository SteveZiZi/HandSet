/**
 * \file
 *      SMVDualADDlg.h
 *
 * \brief
 *      双AD页面头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/4/22
 */

#pragma once


#include "src/main/smv/smvbasedlg.h"


class CCtrlBlkDataDualAD;


// CSMVDualADDlg 对话框

class CSMVDualADDlg : public DSM_SMVBASEDLG
{
	DECLARE_DYNAMIC(CSMVDualADDlg)

public:
	CSMVDualADDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSMVDualADDlg();

// 对话框数据
	enum { IDD = IDD_SMV_DUALAD_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnItemSelectChange( NMHDR* pNotifyStruct, LRESULT *pResult);
    afx_msg void OnInplaceButtonDown( NMHDR* pNotifyStruct, LRESULT *pResult);

protected:

    /**
     *      根据菜单ID切换到指定的控制块
     *
     *  \param[in]
     *      nCmdID          控制块映射的命令菜单ID
     */
    virtual bool _switchCtrlBlk(UINT nCmdID);


public:

    /**
     *      保存页面
     */
    virtual void _SavePage();

	virtual void _modifyMenu();
private:

    /**
     *      创建双AD报表
     */
    void _createReport();

    /**
     *      初始化报表
     */
    void _resetReport();

    /**
     *      恢复页面数据信息
     */
    bool _restorePage();

    /**
     *      调整默认显示通道
     */
    void _adjustChannel();

    /**
     *      寻找默认显示通道
     *
     *  \param[in]
     *      控制块关联的页面状态数据对象
     *
     *  \note
     *      相邻并相序一样则为一组
     */
    void _calcChannel(CCtrlBlkDataDualAD* pData);

    /**
     *      更新界面数据
     */
    void _update();

    /**
     *      检查通道是否发生变化
     */
    bool _checkChannel();

private:

    CReportV                m_reportAD;             ///< 双AD数据显示报表
    CDataDoubleAD           m_data;                 ///< 双AD数据
};
