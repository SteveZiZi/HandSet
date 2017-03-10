/**
 * \file
 *      GooseMonitorDlg.h
 *
 * \brief
 *      goose报文监测页面头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/5/10
 */

#pragma once

#include "src/main/goose/goosebasedlg.h"

// CGooseMonitorDlg 对话框

class CMsgDlg;
class CRNaviMenu;

class CGooseMonitorDlg : public DSM_GOOSEASEDLG
{
	DECLARE_DYNAMIC(CGooseMonitorDlg)

public:
	CGooseMonitorDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGooseMonitorDlg();

// 对话框数据
	enum { IDD = IDD_GOOSE_MONITOR_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
    //afx_msg void OnMenuF3();
    afx_msg void OnMenuF4();
    afx_msg void OnMenuF3();
    virtual BOOL DestroyWindow();
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg LRESULT OnHandleRecordException(WPARAM wParam, LPARAM lParam);

protected:
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

private:

    /**
     *      创建树控件
     */
    void _createTree();

    /**
     *      显示smv帧数据
     */
    void _showFrame();

    /**
     *      更新帧数据
     *
     *  \param[in]
     *      type            帧类型
     *  \param[in]
     *      pBuff           帧数据缓冲区
     *  \param[in]
     *      bufflen         帧数据缓冲区长度
     */
    void _updateFrame(enumFrameType type, const uint8_t* pBuff, int bufflen);

    /**
     *      goose录波
     */
    bool _recordGoose();

    /**
     *      goose录波设置
     */
    bool _recordGooseEx();

    /**
     *      录波前检查线路与SD状态
     */
    bool _checkforRecordGoose();

    /**
     *      生成录波文件路径
     */
    void _genRecordFileName();

    /**
     *      处理录波结束后流程
     */
    bool _handleRecord();

    /**
     *      显示录波状态框
     */
    INT_PTR _showRecordDlg();

private:

    CTreeV                  m_treeFrame;        ///< 报文帧树控件
    CString                 m_csPcapFiles;      ///< 当前分析的pcap文件
    CDataRealTimeMsgData    m_data;             ///< 实时报文数据
    GooseMsg*               m_pGooseMsg;        ///< goose解码对象
    int                     m_nMaxRows;         ///< 一页最大可显示行数
    UINT                    m_nRealTimer;       ///< 第一次获取实时报文的定时器，获得一次数据后关闭定时器
    UINT                    m_nDetectTimer;     ///< 录波时检测SD卡与网络状态的定时器
    CMsgDlg*                m_pMsgDlg;          ///< 录波状态框
    CRNaviMenu*             m_pRNaviMenu;       ///< 菜单资源管理对象

#if !CHUN_HUA
	CString					m_csRecordPath;
#endif
};
