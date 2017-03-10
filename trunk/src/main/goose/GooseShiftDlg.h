/**
 * \file
 *      GooseShiftDlg.h
 *
 * \brief
 *      goose变位页面头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/5/10
 */

#pragma once

#include "src/main/goose/goosebasedlg.h"


/**
 *      变位模式
 */
enum ModeTime
{
    TIME_REL,           ///< 相对
    TIME_ABS            ///< 绝对
};

/**
 *      详细信息or列表模式
 */
enum ModePage
{
    PAGE_LIST,          ///< 变位列表
    PAGE_DETAIL         ///< 详细信息
};

/**
 *      全部信息或变位信息
 */
enum ModeShift
{
    SHIFT_ALL,          ///< 全部信息
    SHIFT_ONLY          ///< 单独变位信息
};


class CPageDataExtra;

// CGooseShiftDlg 对话框

class CGooseShiftDlg : public DSM_GOOSEASEDLG
{
	DECLARE_DYNAMIC(CGooseShiftDlg)

public:
	CGooseShiftDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGooseShiftDlg();

// 对话框数据
	enum { IDD = IDD_GOOSE_SHIFT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnMenuF3();
    afx_msg void OnMenuF6();
    afx_msg void OnMenuShiftF6();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnClickOK(NMHDR* pNotifyStruct, LRESULT* pResult);
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

protected:
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

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
     *      nCmdID          菜单的命令ID
     */
    virtual bool _switchCtrlBlk(UINT nCmdID);


private:

    /**
     *      创建变位信息报表
     */
    void _createReport();

    /**
     *      创建详细信息报表
     */
    void _createReportDetail();

    /**
     *      刷新goose变位信息
     */
    void _updateData();

    /**
     *      还原页面设置
     */
    bool _restorePage();

    /**
     *      刷新界面显示控件元素（非数据）
     */
    void _switchMenu();

    /**
     *      重置F6通道菜单
     */
    void _resetF6Channel();

    /**
     *      过滤通道
     *
     *  \param[in]
     *      nItem       通道号对应的菜单ID
     */
    void _filter(int nItem);

    /**
     *      切换显示绝对时间与相对时间
     */
    void _switchAbsoRel();

    /**
     *      显示详细变位信息
     */
    void _showDetail();

    /**
     *      更新详细变位信息
     */
    void _updateDetail();

    /**
     *      插入一条详细变位记录
     *
     *  \param[in]
     *      pChanBef        变位前通道
     *  \param[in]
     *      pChannAft       变位后通道
     *  \param[in]
     *      nChannel        通道号
     *  \param[in]
     *      nSubChannel     子通道号
     *  \param[in]
     *      nRow            报表行号
     */
    bool _insertRecord(CGooseChannel* pChanBef, CGooseChannel* pChannAft, int nChannel, int nSubChannel, int nRow);

    /**
     *      计算向上翻页在第一行显示的通道序号和子通道序号
     *
     *  \return
     *      bool            发生翻页返回true，否则返回false
     */
    bool _calcUp();

    /**
     *      计算向下翻页在第一行显示的通道序号和子通道序号
     *
     *  \return
     *      bool            发生翻页返回true，否则返回false
     */
    bool _calcDown();

    /**
     *      通道翻页 - 上一页
     */
    void _channelDetailUp();

    /**
     *      通道翻页 - 下一页
     */
    void _channelDetailDown();

    /**
     *      计算单独变位信息上翻页
     *
     *  \return
     *      bool        发生翻页返回true，否则返回false
     */
    bool _calcUpOnly();

    /**
     *      计算单独变位信息下翻页
     *
     *  \return
     *      bool        发生翻页返回true，否则返回false
     */
    bool _calcDownOnly();

    /**
     *      显示单独变位信息
     */
    void _updateShiftDetail();

    /**
     *      填充detal报表
     *
     *  \param[in]
     *      csContent       填充字符串内容
     */
    inline void _fillDetailReport(CString csContent);

    /**
     *      获取指定索引的变位数据
     *
     *  \param[in]
     *      nIndex      变位数据索引
     *  \param[out]
     *      change      变位数据
     *  \return
     *      bool        获取数据成功返回true，失败返回false
     */
    bool _getChange(uint32_t nIndex, CGooseOneTimeChange& change);


private:

    ModeTime                m_eTime;            ///< 相对或绝对时间
    ModePage                m_ePage;            ///< 显示变位列表or通道详细信息
    ModeShift               m_eShift;           ///< 全部信息或在单独变位信息

    CReportV                m_reportGoose;      ///< 变位信息报表
    CReportV                m_reportDetail;     ///< 通道详细信息报表

    CDataGooseChangeList    m_data;             ///< goose变位数据

    int                     m_nChannel;         ///< 选中的item，第一个是全部显示，其余是通道索引
    DWORD                   m_dwLast;           ///< F6弹出菜单item数

    uint32_t                m_nDetailIndex;     ///< 显示详细变位信息的索引

    CNaviMenu*              m_pMenuList;        ///< 变位信息列表页面菜单
    CNaviMenu*              m_pMenuDetail;      ///< 详细变位信息页面菜单

    bool                    m_bRecounter;       ///< 重新查询所有变位信息记录

    int                     m_nRows;            ///< 每页可见行数
    int                     m_nParentChann;     ///< 通道索引，从0计数
    int                     m_nSubChann;        ///< 子通道索引，从0计数

    int                     m_nLastFocus;       ///< 切换详细变位信息时保存变位列表的焦点行

    uint32_t                m_nShiftIndex;      ///< 单独变位信息的变位通道索引
    int                     m_nShiftSubIndex;   ///< 单独变位信息的变位通道的子通道索引
};
