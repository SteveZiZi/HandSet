/**
 * \file
 *      SMVMsgMonitorDlg.h
 *
 * \brief
 *      报文检测及录波页面头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/5/11
 */

#pragma once

#include "src/main/smv/smvbasedlg.h"

class CMsgDlg;

// CSMVMsgMonitorDlg 对话框

class CSMVMsgMonitorDlg : public DSM_SMVBASEDLG
{
	DECLARE_DYNAMIC(CSMVMsgMonitorDlg)

public:
	CSMVMsgMonitorDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSMVMsgMonitorDlg();

// 对话框数据
	enum { IDD = IDD_SMV_MSGMONITOR_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnMenuF3();
    afx_msg void OnMenuF5();
    afx_msg void OnMenuF6();
#if	!SU_FEI
	
	afx_msg void OnMenuF4();

#endif
    virtual BOOL DestroyWindow();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg LRESULT OnHandleRecordException(WPARAM wParam, LPARAM lParam);


protected:
    /**
     *      修改菜单HMENU
     */
    virtual void _modifyMenu();

    /**
     *      初始化当前控制块
     */
    virtual void _initCurrentCtrlBlk() {}

    /**
     *      根据菜单ID切换到指定的控制块
     *
     *  \note
     *      子类选择控制块需要重载该方法做一些补充操作
     */
    virtual bool _switchCtrlBlk(UINT nCmdID);

private:

    /**
     *      显示smv帧数据
     */
    void _showFrame();

    /**
     *      更新帧数据
     */
    void _updateFrame(enumFrameType type, const uint8_t* pBuff, int bufflen);

    /**
     *      创建树控件
     */
    void _createTree();

    /**
     *      刷新一帧报文
     */
    void _refreshFrame();

    /**
     *      录波前检查线路与SD状态
     */
    bool _checkforRecordSMV();

    /**
     *      smv录波
     */
    bool _recordSmv();

    /**
     *      smv 9x录波
     */
    bool _recordSmv9x();

    /**
     *      smv ft3录波
     */
    bool _recordSmvft3();

    /**
     *      生成录波文件路径
     */
    void _genRecordFile();

    /**
     *      显示录波状态框
     */
    INT_PTR _showRecordDlg();

    /**
     *      处理录波结束后流程
     */
    bool _handleRecord();


private:

    CTreeV                  m_treeFrame;        ///< 报文帧树控件

    Sav91Msg*               m_pMsg91;           ///< 91协议解码器
    Sav92Msg*               m_pMsg92;           ///< 92协议解码器
    NanRuiFT3Msg*           m_pMsgNanRui;       ///< FT3南瑞协议解码器
    GuoWangFT3Msg*          m_pMsgGuoWang;      ///< FT3国网协议解码器


    CString                 m_csPcapFiles;      ///< 当前分析的pcap文件
    CDataRealTimeMsgData    m_data;             ///< 实时报文数据
    int                     m_nTreeRows;        ///< 树控件一页可显示行数
    CMsgDlg*                m_pMsgDlg;          ///< 录波状态框
    UINT                    m_nRealTimer;       ///< 第一次获取实时报文的定时器，获得一次数据后关闭定时器
    UINT                    m_nRecordTimer;     ///< 录波时检测线路以及SD卡状态定时器

#if !CHUN_HUA
	CString					m_csPcapPath;
#endif
};
