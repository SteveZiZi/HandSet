/**
 * \file
 *      SMVPhasesequeDlg.h
 *
 * \brief
 *      序量页面头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/4/21
 */

#pragma once

#include "src/main/smv/smvbasedlg.h"

#include "src/utils/reportcontrol/listctrlel/listctrlel.h"
#include "src/utils/electriccontrol/phasorgramctrl/phasorgramctrl.h"

class CPageDataExtra;
class CCtrlBlkDataPhasaSeq;



// CSMVPhasesequeDlg 对话框

class CSMVPhasesequeDlg : public DSM_SMVBASEDLG
{
	DECLARE_DYNAMIC(CSMVPhasesequeDlg)

public:
	CSMVPhasesequeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSMVPhasesequeDlg();

// 对话框数据
	enum { IDD = IDD_SMV_PHASESEQU_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnMenuF4();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnItemSelectChanged(NMHDR* pNotifyStruct, LRESULT* pResult);
    afx_msg void OnInplaceButtonDown(NMHDR* pNotifyStruct, LRESULT* pResult);


public:

    /**
     *      保存页面
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
     *      nCmdID          命令菜单ID，与控制有一个映射的关系
     */
    virtual bool _switchCtrlBlk(UINT nCmdID);


private:

    /**
     *      创建相位报表
     */
    void _createReport();

    /**
     *      创建序量图
     */
    void _createPhaseGram();

    /**
     *      重设报表内通道数据
     */
    void _resetReport();

    /**
     *      刷新界面数据
     */
    void _updatePhasae();

    /**
     *      寻找3个合适的默认显示通道名
     *
     *  \note
     *      选中的三个默认通道索引将填入该通道关联的页面数据对象中
     */
    void _adjustChannel();

    /**
     *      寻找合适通道值
     *
     */
    void _calcChannel(CCtrlBlkDataPhasaSeq* pData);

    /**
     *      使用页面数据设置通道
     */
    bool _restorePage();

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
     *      更新所选通道的单位
     */
    void _updateUnit();

    /**
     *      检查通道是否发生变化
     */
    bool _checkChannel();


private:

    CDataXuliang                m_data;             ///< 序量业务数据
    CReportVM                   m_reportPhase;      ///< 通道报表
    CPhasorGramCtrl             m_ctrlPhasor;       ///< 序量控件

    DWORD                       m_dwMode;           ///< F3功能模式

    CString                     m_csUnit[4];        ///< 显示数值单位
    int                         m_nTimes;           ///< 缩放倍数
    float                       m_fScaleInterval;   ///< 序量图刻度值

    // 优化频繁更新数据时变量的申请释放
    // 零序、正序、负序
    float                       m_fAngel[3];        ///< 相位角
    float                       m_fValue[3];        ///< 有效值
    CString                     m_strDesc[3];       ///< 通道描述

    enumSmvValueType            m_eDispType;        ///< 通道值的显示类型
    enumSmvValueType            m_eFrameType;       ///< 额定通道类型的显示类型
};
