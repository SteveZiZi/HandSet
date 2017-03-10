/**
 * \file
 *      SMVPhasorDlg.h
 *
 * \brief
 *      相量
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/4/20
 */

#pragma once

#include "src/main/smv/smvbasedlg.h"
#include "src/utils/reportcontrol/listctrlel/listctrlel.h"
#include "src/utils/electriccontrol/phasorgramctrl/phasorgramctrl.h"



class CCtrlBlkDataPhasor;


// CSMVPhasorDlg 对话框

class CSMVPhasorDlg : public DSM_SMVBASEDLG
{
	DECLARE_DYNAMIC(CSMVPhasorDlg)

public:
	CSMVPhasorDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSMVPhasorDlg();

// 对话框数据
	enum { IDD = IDD_SMV_PHASOR_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnMenuF4();
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnItemSelectChange( NMHDR* pNotifyStruct, LRESULT *pResult);


public:

    /**
     *      保存页面状态信息
     */
    virtual void _SavePage();

protected:

    /**
     *      修改菜单HMENU
     */
    virtual void _modifyMenu();

    /**
     *      根据菜单ID切换到指定的控制块
     *
     *  \param[in]
     *      nCmdID          控制块映射的命令菜单ID
     */
    virtual bool _switchCtrlBlk(UINT nCmdID);


private:

    /**
     *      创建相量报表
     */
    void _createReport();

    /**
     *      重设报表内通道数据
     */
    void _resetReport();

    /**
     *      创建相量图
     */
    void _createPhasorGram();

    /**
     *      刷新界面的值
     */
    void _updatePhasor();

    /**
     *      调整初始选中的3个通道，尽量选中合适的ABC三相
     */
    void _adjustChannel();

    /**
     *      寻找3个合适的默认显示通道名
     *
     *  \note
     *      选中的三个默认通道索引将填入该通道关联的页面数据对象中
     */
    void _calcChannel();

    /**
     *      返回符合条件的通道号
     *
     *  \param[in]
     *      eType           指定的通道类型
     *  \param[in]
     *      ePhase          指定的通道相别
     *  \param[in]
     *      nStart          通道的查询起始位置
     *  \return
     *      uint32_t        返回符合条件的通道号，找不到则返回 UINT_MAX
     */
    uint32_t _findChannel(enumChannelType eType, ChannelPhase ePhase, int nStart);

    /**
     *      使用页面数据设置通道
     */
    bool _restorePage();

    /**
     *      更新所选通道的单位
     */
    void _updateUnit();

    /**
     *      检查通道是否变化
     *
     *  \return
     *      bool        发生变化返回true，没变化返回false
     */
    bool _checkChannel();



private:

    DWORD                       m_dwMode;           ///< F3功能模式

    CReportVM                   m_reportChannel;    ///< 通道报表
    CPhasorGramCtrl             m_ctrlPhasor;       ///< 相量控件
    CDataPhasor                 m_data;             ///< 相量业务数据

    CString                     m_csUnit[4];        ///< 显示数值单位
    float                       m_fScaleInterval;   ///< 相量图刻度值
    int                         m_nTimes;           ///< 缩放倍数

    CCtrlBlkDataPhasor*         m_pCurCtrlBlkData;  ///< 当前控制块关联数据

    // 优化定时器更新数据时数据对象的创建

    float                       m_fLen[4];          ///< 有效值
    float                       m_fValue[4];        ///< 相位
    CString                     m_strDesc[4];       ///< 通道描述

    enumSmvValueType            m_eDispType;        ///< 通道值的显示类型
    enumSmvValueType            m_eFrameType;       ///< 额定通道类型的显示类型
};
