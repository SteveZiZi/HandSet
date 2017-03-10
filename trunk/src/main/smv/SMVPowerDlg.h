/**
 * \file
 *      SMVPowerDlg.h
 *
 * \brief
 *      功率页面头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/4/22
 */

#pragma once

#include "src/main/smv/smvbasedlg.h"
#include "src/main/pagedata/pagedata.h"


class CCtrlBlkDataPower;

// CSMVPowerDlg 对话框

class CSMVPowerDlg : public DSM_SMVBASEDLG
{
	DECLARE_DYNAMIC(CSMVPowerDlg)

public:
	CSMVPowerDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSMVPowerDlg();

// 对话框数据
	enum { IDD = IDD_SMV_POWER_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()


public:
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnItemSelectChange( NMHDR* pNotifyStruct, LRESULT *pResult);
    afx_msg void OnInplaceButtonDown( NMHDR* pNotifyStruct, LRESULT *pResult);

public:

    /**
     *      保存当前控制块关联的页面状态数据
     */
    virtual void _SavePage();

	virtual void _modifyMenu();

protected:

    /**
     *      根据菜单ID切换到指定的控制块
     *
     *  /param[in]
     *      nCmdID      控制块关联的菜单命令ID
     */
    virtual bool _switchCtrlBlk(UINT nCmdID);

private:

    /**
     *      创建报表
     */
    void _createReport();

    /**
     *      初始化报表
     */
    void _initReport();

    /**
     *      响应定时器消息，更新报表数据
     */
    void _updateReport();

    /**
     *      根据控制块关联的页面状态数据重设电流电压通道信息
     */
    void _resetChannel();

    /**
     *      为指定行号加载指定通道类型的通道信息
     *
     *  \param[in]
     *      nRow        指定的行号
     *  \param[in]
     *      eType       指定的通道类型
     */
    void _loadItemChannel(int nRow, enumChannelType eType);

    /**
     *      为ABC三相通道选中合适的默认通道
     */
    void _adjustChannel();

    /**
     *      显示合适的默认显示通道名
     *
     *  \param[in]
     *      eType       通道类型
     *  \param[in]
     *      pData       当前控制块关联的数据
     */
    void _calcChannel(enumChannelType eType, CCtrlBlkDataPower* pData);

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
     *      使用控制块的页面状态数据设置报表
     */
    bool _restorePage();

    /**
     *      检查通道是否发生变化
     */
    bool _checkChannel();

    /**
     *      获取基准相位值
     */
    float _getBasePhaseValue();


private:

    CDataPower              m_data;             ///< 功率数据
    CReportV                m_reportPower;      ///< 功率信息显示报表

    // 防止数据频繁更新的影响
    float                   m_fVolt[3];         ///< 电压有效值
    float                   m_fVoltAngle[3];    ///< 电压相位角
    float                   m_fAmper[3];        ///< 电流有效值
    float                   m_fAmperAngle[3];   ///< 电流相位角
    float                   m_fActive[4];       ///< 有功功率
    float                   m_fReactive[4];     ///< 无功功率
    float                   m_fApparent[4];     ///< 视在功率
    float                   m_fFactor[4];       ///< 功率因数

    enumSmvValueType        m_eDispType;        ///< 通道值的显示类型
    enumSmvValueType        m_eFrameType;       ///< 额定通道类型的显示类型

    CCtrlBlkDataPower*      m_pCtrlBlkData;     ///< 当前控制块的页面数据
};
