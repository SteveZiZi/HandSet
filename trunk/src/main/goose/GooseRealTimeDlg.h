/**
 * \file
 *      GooseRealTimeDlg.h
 *
 * \brief
 *      goose实时值页面头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/5/9
 */

#pragma once

#include "src/main/goose/goosebasedlg.h"

#include "src/utils/reportcontrol/listctrlel/listctrlel.h"


class CPageDataExtra;

// CGooseRealTimeDlg 对话框

class CGooseRealTimeDlg : public DSM_GOOSEASEDLG
{
	DECLARE_DYNAMIC(CGooseRealTimeDlg)

public:
	CGooseRealTimeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGooseRealTimeDlg();

// 对话框数据
	enum { IDD = IDD_GOOSE_REALTIME_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
	afx_msg void OnMenuF3();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual BOOL PreTranslateMessage(MSG* pMsg);


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
     *      nCmdID          菜单命令ID
     */
    virtual bool _switchCtrlBlk(UINT nCmdID);


private:
    /**
     *      创建实时值报表
     */
    void _createReport();

    /**
     *      更新实时值报表数据
     */
    void _updateData();

    /**
     *      刷新报表底部的stNum,sqNum,TTL状态
     */
    void _updateStatus(bool bNull = false);

    /**
     *      计算向下翻页在第一行显示的通道序号和子通道序号
     *
     *  \return
     *      bool            发生翻页返回true，否则返回false
     */
    bool _calcDown();

    /**
     *      计算向上翻页在第一行显示的通道序号和子通道序号
     *
     *  \return
     *      bool            发生翻页返回true，否则返回false
     */
    bool _calcUp();

    /**
     *      通道翻页 - 上一页
     */
    void _channelUp();

    /**
     *      通道翻页 - 下一页
     */
    void _channelDown();

    /**
     *      还原页面数据
     */
    bool _restorePage();

    /**
     *      插入一条通道记录
     *
     *  \param[in]
     *      pChannel        通道对象
     *  \param[in]
     *      nChannel        goose数据通道号
     *  \param[in]
     *      nSubChannel     goose数据子通道号
     *  \param[in]
     *      nRow            报表行号
     */
    void _insertRecord(CGooseChannel* pChannel, int nChannel, int nSubChannel, int nRow);

    /**
     *      检查goose通道数量与类型是否变化
     *
     *  \param[in]
     *      bool            通道数量或类型变化返回true，没变化返回false
     */
    bool _isChannelChange();

    /**
     *      重设页面初始状态
     *
     *  \note
     *      当运行时发生通道数量或者类型发生变化后
     *      页面的状态将返回到一个新的初始状态(第一页）
     */
    void _resetPage();


private:

    int                     m_nRows;            ///< 每页可见行数
    int                     m_nChann;           ///< 通道索引，从0计数
    int                     m_nSubChann;        ///< 子通道索引，从0计数
    int                     m_nPage;            ///< 当前页数

    CRect                   m_rcStatus;         ///< stNum状态栏区域
    CBitmap                 m_panel;            ///< 状态栏区域画布

    CReportV                m_reportGoose;      ///< 实时值报表
    CGooseInfo              m_data;             ///< goose实时值数据

    bool                            m_bInitChannelData;         ///< 是否初始化通道数据
    std::vector<CGooseChannel*>     m_vecChannels;              ///< 控制块对应的通道信息
	bool					m_bIsShowDetail;
};
