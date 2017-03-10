/**
 * \file
 *      SMVWaveformDlg.h
 *
 * \brief
 *      波形图页面头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/4/19
 */

#pragma once

#include "src/main/smv/smvbasedlg.h"


#include "src/utils/electriccontrol/waveformgramctrl/waveformgramctrl.h"

#define DSM_SMV_WAVEFORM_GRAM_X         1           ///< x轴波形数
#define DSM_SMV_WAVEFORM_GRAM_Y         3           ///< y轴波形数

#define DSM_SMV_WAVEFORM_NUMS           DSM_SMV_WAVEFORM_GRAM_X * DSM_SMV_WAVEFORM_GRAM_Y

class CPageDataExtra;


// CSMVWaveformDlg 对话框

class CSMVWaveformDlg : public DSM_SMVBASEDLG
{
	DECLARE_DYNAMIC(CSMVWaveformDlg)

public:
	CSMVWaveformDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSMVWaveformDlg();

// 对话框数据
	enum { IDD = IDD_SMV_WAVEFORM_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnMenuF4();

public:

    /**
     *      保存页面
     */
    virtual void _SavePage();


protected:

    /**
     *      修改底部菜单
     */
    virtual void _modifyMenu();

    /**
     *      根据菜单ID切换到指定的控制块
     *
     *  \param[in]
     *      nCmdID          菜单ID
     *  \return
     *      bool            切换控制块成功返回true
     */
    virtual bool _switchCtrlBlk(UINT nCmdID);


private:
    /**
     *      创建波形控件
     */ 
    void _createWaveform();

    /**
     *      根据数据层获得的数据刷新波形
     */
    void _updateWaveform();

    /**
     *      上翻通道
     */
    void _pageupChannel();

    /**
     *      下翻通道
     */
    void _pagedownChannel();

    /**
     *      放大波形
     */
    void _zoominWaveform();

    /**
     *      缩小波形
     */
    void _zoomoutWaveform();

    /**
     *      放大时间轴（X轴）
     */
    void _zoominTimeline();

    /**
     *      减小时间轴（X轴）
     */
    void _zoomoutTimeline();

    /**
     *      根据通道的类型设置波形图的属性
     */
    void _setWaveform();

    /**
     *      设置X轴默认刻度
     */
    void _setXScale();

    /**
     *      根据控制块关联的页面状态信息进行设置
     */
    bool _restorePage();

    /**
     *      根据当前页面索引计算显示的通道索引
     */
    void _calcChannel();


private:
    uint32_t                    m_nPage;                ///< 当前页面值

    // DSM_SMV_WAVEFORM_ZOOM    0x01        缩放模式
    // DSM_SMV_WAVEFORM_WATCH   0x02        观察模式
    DWORD                       m_dwWaveMode;           ///< 当前波形图显示模式，F3功能

    int                         m_nSample;              ///< 当前采样率
    int                         m_nSamplePer;           ///< 每个格子采样点数，默认一个格子为一个周波的点数
    int                         m_nSamplePer1;          ///< 实际每个格子的采样点数，12.8k下重采样只有4k的数据量
    float                       m_fVoltage;             ///< 电压额定值，幅值
    float                       m_fAmperage;            ///< 电流额定值，幅值


    int                         m_nWaveformT;           ///< Y轴放大倍数
    int                         m_nTimelineT;           ///< X轴放大倍数

    // 下面数组是保存当前页面所显示通道的信息
    float                       m_arrChannelAmplitude[DSM_SMV_WAVEFORM_NUMS];       ///< 各通道的幅值
    uint32_t                    m_arrChannels[DSM_SMV_WAVEFORM_NUMS];               ///< 当前页面显示的通道索引
    enumChannelType             m_arrChannelType[DSM_SMV_WAVEFORM_NUMS];            ///< 当前页面显示的通道类型
    CString                     m_arrUnits[DSM_SMV_WAVEFORM_NUMS];                  ///< 当前页面显示的通道的数据单位

    CDataOscillogram            m_data;                 ///< 波形数据

    CWaveFormGramCtrl           m_ctrlWaveform;         ///< 波形控件

    enumSmvValueType            m_eDispType;            ///< 通道值的显示类型
    enumSmvValueType            m_eFrameType;           ///< 额定通道类型的显示类型
};
