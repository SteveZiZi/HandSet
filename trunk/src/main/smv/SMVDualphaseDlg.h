/**
 * \file
 *      SMVDualphaseDlg.h
 *
 * \brief
 *      核相页面头文件
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

class CPageDataExtraDualPhase;


/**
 *      页面状态附加数据类
 */
class CPageDataExtraDualPhase : public IPageDataExtra
{
public:
    CPageDataExtraDualPhase(CPageData* pPageData)
        : pPageData(pPageData)
        , nFoucs(0) 
        , nSmv1(UINT_MAX) 
        , nSmv2(UINT_MAX) 
        , bGram(true)
        , bSame(true)
    {
        for(int i = 0; i < 3; i++)
        {
            nChannelA[i]        = UINT_MAX;
            nChannelB[i]        = UINT_MAX;
            bChannelValidA[i]   = false;
            bChannelValidB[i]   = false;
        }

    }
    virtual ~CPageDataExtraDualPhase() {}

public:

    /**
     *      获取附加数据类绑定的数据页面数据指针
     */
    virtual CPageData* GetPageData()
    {
        return pPageData;
    }

public:
    int                 nFoucs;                 ///< 焦点行号

    bool                bGram;                  ///< true 相量图模式，false 表格模式
    bool                bSame;                  ///< true 同相 false 相间

    uint32_t            nSmv1;                  ///< SMV1的控制块ID
    uint32_t            nSmv2;                  ///< SMV2的控制块ID

    uint32_t            nChannelA[3];           ///< SMV1选中通道号
    uint32_t            nChannelB[3];           ///< SMV2选中通道号

    bool                bChannelValidA[3];      ///< SMV1选中通道有效性
    bool                bChannelValidB[3];      ///< SMV2选中通道有效性

private:
    CPageData*          pPageData;              ///< 附件数据关联的页面数据指针
};

// CSMVDualphaseDlg 对话框

class CSMVDualphaseDlg : public DSM_SMVBASEDLG
{
	DECLARE_DYNAMIC(CSMVDualphaseDlg)

public:
	CSMVDualphaseDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSMVDualphaseDlg();

// 对话框数据
	enum { IDD = IDD_SMV_DUALPHASE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnMenuF2();
  //  afx_msg void OnMenuF3();
    afx_msg void OnMenuF4();
    afx_msg void OnMenuF5();
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnItemSelectChange( NMHDR* pNotifyStruct, LRESULT *pResult);
    afx_msg void OnInplaceButtonDown( NMHDR* pNotifyStruct, LRESULT *pResult);


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
     *      禁止加载控制块到F1菜单中
     */
    void _loadCtrlBlk() {}

    /**
     *      初始化当前核相的页面数据
     */
    virtual void _initCurrentCtrlBlk();


private:
    /**
     *      核相业务中组别枚举
     */
    enum ePhaseOrder
    {
        ORDER_PHASE_A,          ///< 第一组
        ORDER_PHASE_B,          ///< 第二组
    };


private:

    /**
     *      初始化核相业务的smv1与smv2控制块以及其选中的ABC三相通道
     */
    void _initPageStatus();


    /**
     *      创建待核相报表
     */
    void _createReport();

    /**
     *      创建有效值表
     */
    void _createReportVal();

    /**
     *      创建相量图
     */
    void _createDualphaseGram();

    /**
     *      初始化报表
     */
    void _initReport();

    /**
     *      根据页面状态信息自动调整smv控制块与选中的报表
     */
    void _adjustChannel();

    /**
     *      根据指定信息设置一组通道，包括设置控制块以及ABC三相通道
     */
    void _setChannel(ePhaseOrder order);

    /**
     *      为指定的控制块寻找合适的ABC三相通道显示
     *
     *  \param[in]
     *      pCtrlBlk         指定的控制块对象
     *  \param[in]
     *      order           smv组序，smv1或者smv2
     */
    void _calcChannel(CBaseDetectItem* pCtrlBlk, ePhaseOrder order);

    /**
     *      返回符合条件的通道号
     *
     *  \param[in]
     *      pCtrlBlk        指定的控制块
     *  \param[in]
     *      eType           指定的通道类型
     *  \param[in]
     *      ePhase          指定的通道相别
     *  \param[in]
     *      nStart          通道的查询起始位置
     *  \return
     *      uint32_t        返回符合条件的通道号，找不到则返回 UINT_MAX
     */
    uint32_t _findChannel(CBaseDetectItem* pCtrlBlk, enumChannelType eType, ChannelPhase ePhase, int nStart);

    /**
     *      F4切换功能，表格与相量图的切换显示
     */
    void _switchF4();

    /**
     *      F5切换功能，在表格模式下，切换显示同相相角差或相间相量差
     */
    void _switchF5();

    /**
     *      更新菜单标题
     */

    void _updateMenuNavi();

    /**
     *      更新选中通道的单位
     *
     *  \param[in]
     *      pCtrlBlk        通道关联的控制块对象
     *  \param[in]
     *      nRow            通道所在报表行号
     *  \param[in]
     *      nChannel        通道号
     */
    void _updateUnit(CBaseDetectItem* pCtrlBlk, int nRow, uint32_t nChannel);

    /**
     *      第一个选中通道若是A相类型则自动为2，3通道选择紧接的BC相类型的通道
     *
     *  \param[in]
     *      eOrder          smv组序，smv1或smv2
     *  \param[in]
     *      pCtrlBlk        选中的控制块对象
     *  \param[in]
     *      nSel            控制块下选中的A相通道索引
     */
    void _adjustABC(ePhaseOrder eOrder, CBaseDetectItem* pCtrlBlk, uint32_t nSel);

    /**
     *      选中新控制块后，调整该控制块下ABC通道
     *
     *  \param[in]
     *      eOrder          smv组序，smv1或smv2
     *  \param[in]
     *      nSel            控制块索引，将会引起ABC三相通道的重新选择
     */
    void _adjustCtrlBlk(ePhaseOrder eOrder, uint32_t nSel);

    /**
     *      刷新页面数据
     */
    void _updateData();

    /**
     *      刷新相量图
     */
    void _updateDataGram();

    /**
     *      刷新报表数据
     */
    void _updateDataReport();

    /**
     *      清空表格数据
     */
    void _clearReport();

    /**
     *      检查通道是否发生变化
     */
    bool _checkChannel();


private:

    CReportVM           m_reportPhase;      ///< 待核相报表
    CReportVM           m_reportValue;      ///< 有效值与相间表
    CPhasorGramCtrl     m_ctrlPhaseGram;    ///< 相量图

    CBaseDetectItem*    m_pCtrlBlkA;        ///< 选中的控制块A
    CBaseDetectItem*    m_pCtrlBlkB;        ///< 选中的控制块B
    CDataPhaseCheck     m_data;             ///< 核相数据接口

    bool                m_bGram;            ///< true 相量图模式 false 表格模式
    bool                m_bSamePhase;       ///< true 同相 false 相间
    bool                m_bLastStatus;      ///< 上一次的数据状态
    float               m_fScaleInterval;   ///< 相量图刻度值

    CPageDataExtraDualPhase*    m_pPageDataExtraDual;       ///< 核相页面关联数据

    CRect                       m_rcAa;                         ///< 相角差显示位置
    CRect                       m_rcBb;                         ///< 相角差显示位置
    CRect                       m_rcCc;                         ///< 相角差显示位置
    CRect                       m_rcF1;                         ///< smv1 频率显示位置
    CRect                       m_rcF2;                         ///< smv2 频率显示位置

    float                       m_fInPhaseAngleDiff[3];         ///< 角差
    CStructPhasor               m_stPhaseVal1[3];               ///< smv1 相量数据
    CStructPhasor               m_stPhaseVal2[3];               ///< smv2 相量数据
    CString                     m_csDesc[6];                    ///< 相量通道的描述
    CString                     m_csUnit[6];                    ///< 通道的单位
    CString                     m_csFmt[6];                     ///< 通道显示数据的格式化字符串

    CString                     m_csFmtEffect;                  ///< 有效值相角的格式化字符串
    CString                     m_csFmtFreq;                    ///< 频率格式化字符串

    CStructPhasor               m_stPhaseData1[3];              ///< smv1 同相/相间差数据
    CStructPhasor               m_stPhaseData2[3];              ///< smv2 同相/相间差数据

    CELRecordItem*              m_arrayItem[14];
};
