#pragma once

#include "src/service/datalayer/dsmdatalayer.h"
#include "src/utils/reportcontrol/listctrlel/listctrlel.h"
#include "src/utils/ElectricControl/PolarityDialCtrl/PolarityDialCtrl.h"

#include "src/main/resource.h"
#include "src/main/smv/smvbasedlg.h"

// CSmvPolarityDlg 对话框

class CPageDataExtra;



class CSmvPolarityDlg : public DSM_SMVBASEDLG
{
	DECLARE_DYNAMIC(CSmvPolarityDlg)

public:
	CSmvPolarityDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSmvPolarityDlg();

// 对话框数据
	enum { IDD = IDD_SMV_POLARITY_DLG };
    
protected:
    CDataPolarity      m_dataPolarity;                                          ///< 极性数据

    int                m_nChannelCount;                                         ///< 当前控制块中的通道数
    int                m_nCurPageIndex;                                         ///< 当前页索引

    CPolarityDialCtrl  m_polarityDialCtrl0;                                     ///< 极性控件1
    CPolarityDialCtrl  m_polarityDialCtrl1;                                     ///< 极性控件2
    CPolarityDialCtrl  m_polarityDialCtrl2;                                     ///< 极性控件3
    CPolarityDialCtrl  m_polarityDialCtrl3;                                     ///< 极性控件4

    CBitmap            m_bmpDail;                                               ///< 背景图片
    CBitmap            m_bmpPoint;                                              ///< 指针
    
public:
    
    /**
     *      保存页面数据
     */
    void _SavePage();

protected:
    /**
     *      恢复页面显示
     */
    bool _restorePage();

    /**
     *      设置通道值
     */
    BOOL  _setChannelValue(int nChanIndex, CDataPolarityChannelInfo* pPolarityChanInfo);
    /**
     *     更新通道控件
     */
    void  _updateChannelCtrl();

    /**
     *     更新通道值
     */
    void  _updatePolarityValue();
    /**
     *      创建报表
     */
    void  _createReport();

    /**
     *      修改菜单HMENU
     */
    virtual void _modifyMenu();

    /**
     *      检测通道数是否修改
     */
    bool _checkChannel();
    /**
     *      通道切换
     */
    bool _switchCtrlBlk(UINT nCmdID);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg void OnMenuF3();
#if SU_FEI
    afx_msg void OnMenuF5();
#else
	afx_msg void OnMenuF4();

#endif
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL DestroyWindow();
};
