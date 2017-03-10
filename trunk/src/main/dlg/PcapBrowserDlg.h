/**
 * \file
 *      PcapBrowserDlg.h
 *
 * \brief
 *      pcap文件浏览器头文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/11/13
 */

#pragma once

#include "vector"
#include "src/main/common.h"
#include "src/main/resource.h"
#include "src/main/dlg/basemodlg.h"

#include "src/service/datalayer/dsmdatalayer.h"

#include "src/utils/electriccontrol/horhistogramctrl/horhistogramctrl.h"
#include "src/utils/electriccontrol/waveformgramctrl/waveformgramctrl.h"

class IPcapFile;
class CNaviToolBar;
class CRNaviMenu;
class CMsgDlg;
class CDsmDataLayer;
class CProgressBarDlg;
class CAppidSampleMapping;

class Sav92Msg;
class Sav91Msg;
class GooseMsg;
class NanRuiFT3Msg;
class GuoWangFT3Msg;


#define DSM_PCAP_WAVEFORM_GRAM_X         1           ///< x轴波形数
#define DSM_PCAP_WAVEFORM_GRAM_Y         3           ///< y轴波形数

#define DSM_PCAP_WAVEFORM_NUMS           DSM_PCAP_WAVEFORM_GRAM_X * DSM_PCAP_WAVEFORM_GRAM_Y


// CPcapBrowserDlg 对话框

class CPcapBrowserDlg : public DSM_BASEMODLG
{
	DECLARE_DYNAMIC(CPcapBrowserDlg)

public:
	CPcapBrowserDlg(const CString csFile = L"", CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPcapBrowserDlg();

// 对话框数据
	enum { IDD = IDD_PCAP_BROWSER_DLG };

private:
    enum ModePage
    {
        MODE_FRAME_LIST,            ///< 报文帧列表模式
        MODE_FRAME_DETAIL,          ///< 报文帧详细信息模式
        MODE_FRAME_WAVE,            ///< 波形分析模式
        MODE_FRAME_STATISTICS,      ///< 报文统计模式
#if !SU_FEI
		MODE_FRAME_HISTOGRAM,		///< 报文离散度模式
#endif	

    };

    enum ModeWave
    {
        MODE_WAVE_ZOOM,             ///< 缩放
        MODE_WAVE_WATCH,            ///< 观看
    };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()


protected:
    virtual void OnOK() {}
    virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

public:
    afx_msg void OnPaint();
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnF2TF4(UINT nID);

#if !CHUN_HUA
	afx_msg void OnDetailF1F2(UINT nID);

	
#endif

#if SU_FEI
    afx_msg void OnNextPreviousException(UINT nID);
#else
	afx_msg void OnMenuF6();
#endif
    afx_msg LRESULT OnLoadPcapComplete(WPARAM, LPARAM);
    afx_msg LRESULT OnUpdateWave(WPARAM, LPARAM);
    afx_msg LRESULT OnInitLoadPcap(WPARAM, LPARAM);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
    afx_msg void OnItemSelFrame(NMHDR* pNotifyStruct, LRESULT *pResult);

#if !SU_FEI
	afx_msg void OnMenuF5();
#endif

private:
    struct PCAP_CTRLBLK
    {
        uint32_t            index;                      ///< 索引
        uint32_t            app_id;                     ///< appid
        size_t              max_frame;                  ///< 最大帧数
        size_t              first_visibility_index;     ///< 当前页第一个可视行的帧索引(控制块内索引)
        int                 focus;                      ///< 当前焦点行
        enumFrameType       type;                       ///< appid类型
        const CAppidSampleMapping* data;                ///< 
        UINT                cmdid;                      ///< 

        struct {
            uint32_t        wfirst_visibility_index;    ///< 波形分析时第一个可见通道索引
            int             wzoomx;                     ///< x轴缩放倍数
            int             wzoomy;                     ///< y轴缩放倍数
            int             wsample;                    ///< 采样率
            uint32_t        wbegin;                     ///< 起始点
        }wave;
    };

public:

    /**
     *      设置待分析的pcap文件路径
     */
    inline void _SetFile(const CString csFile)
    {
        m_csPcapFile = csFile;
    }

    /**
     *      清理窗口关联的资源
     */
    virtual void _ClearResource();


private:

    /**
     *      创建报文帧报表
     */
    void _createReport();

    /**
     *      加载指定pcap文件
     */
    bool _loadPcap();

    /**
     *      显示报文帧信息
     *
     *  param[in]
     *      nIndex          显示的帧索引
     */
    void _showFrameDetail(size_t nIndex);

	void _udateFrameDetail( enumFrameType eType );
	

    /**
     *      根据指定帧索引获取帧数据
     *
     *  \param[in]
     *      nIndex          帧索引
     *  \param[out]
     *      rType           帧类型
     */
    bool _getFrameBuff(size_t nIndex, enumFrameType& rType);

    /**
     *      创建帧信息树控件
     */
    void _createFrameTree();

    /**
     *      切换到某个页面状态时，进行菜单与标题的修改
     *
     *  \param[in]
     *      mode            新的页面模式
     */
    void _switchToPage(ModePage mode);

    /**
     *      处理报文列表翻页操作
     */
    BOOL _preTranslateMessageList(MSG* pMsg);
#if SU_FEI
    /**
     *      更新F5/F6查找异常报文帧的标题信息
     */
    void _updateNaviExcep();
#endif
    /**
     *      处理报文波形分析的上下左右操作
     */
    BOOL _preTranslateMessageWave(MSG* pMsg);

    /**
     *      清理旧状态数据
     */
    void _clearStatus();

    /**
     *      初始化报文中的控制块信息
     */
    bool _initCtrlBlk();

    /**
     *      加载报文中的控制块信息
     */
    bool _loadCtrlBlk();

    /**
     *      生成控制块菜单
     */
    void _initCtrlBlkMenu();

    /**
     *      处理切换控制块消息
     *
     *  \param[in]
     *      nCmdID          选中的菜单ID
     */
    void _switchCtrlBlk(UINT nCmdID);

    /**
     *      获取指定AppID的控制块数据
     *
     *  \param[in]
     *      appid           指定的appid
     *  \param[out]
     *      data            appid对应的数据
     *  \return
     *      bool            获取成功返回true，失败返回false
     */
    bool _getCtrlBlkByAppID(uint32_t appid, PCAP_CTRLBLK** data);

        /**
     *      获取指定AppID的控制块数据
     *
     *  \param[in]
     *      appid           指定的appid
     *  \param[out]
     *      data            appid对应的数据
     *  \return
     *      bool            获取成功返回true，失败返回false
     */
    bool _getCtrlBlkByAppID(uint32_t appid, PCAP_CTRLBLK& data);

    /**
     *      刷新UI数据
     */
    void _updateUI();

    /**
     *      刷新报文帧列表界面
     */
    void _updateUIFrameList();

    /**
     *      刷新报文统计界面
     */
    void _updateUIStatistics();

    /**
     *      刷新sv报文统计界面
     */
    void _updateUIStatisticsSV();

    /**
     *      刷新gs报文统计界面
     */
    void _updateUIStatisticsGS();

    /**
     *      刷新波形分析界面
     */
    void _updateUIWave();

    /**
     *      切换到报文帧列表
     */
    void _switchFrameList();

    /**
     *      切换到详细帧报文
     */
    void _switchFrameDetail();

    /**
     *      切换到异常统计
     */
    void _switchFrameStatistics();

    /**
     *      切换报文波形分析
     */
    void _switchFrameWave();

    /**
     *      获取帧类型描述
     *  \param[in]
     *      type            帧类型
     */
    CString _getFrameTypeDesc(enumFrameType type);

    /**
     *      获取每一帧异常的描述
     *
     *  \param[in]
     *      type            帧类型
     *  \param[in]
     *      status          异常类型
     *  \return
     *      CString         异常的描述信息
     */
    CString _getFrameStatisticsDesc(enumFrameType type, uint16_t status);

    /**
     *      获取帧描述信息
     *
     *  \param[in]
     *      type            帧类型
     *  \param[in]
     *      index           帧索引
     *  \return
     *      CString         返回帧描述信息
     */
    CString _getFrameDesc(enumFrameType type, uint32_t index);

    /**
     *      查找上一个异常帧的位置
     *
     *  \param[out]
     *      first_visible       第一可视行索引
     *  \param[out]
     *      focus               异常帧位置
     *  \return
     *      bool                找到返回true，找不到返回false
     */
 //   bool _lookupPreviousException(size_t& first, int& focus);

    /**
     *      查找下一个异常帧的位置
     *
     *  \param[out]
     *      first_visible       第一可视行索引
     *  \param[out]
     *      focus               异常帧位置
     *  \return
     *      bool                找到返回true，找不到返回false
     */
 //   bool _lookupNextException(size_t& first, int& focus);

    /**
     *      判断是否还有上一个异常帧
     */
  //  bool _hasPrevException();

    /**
     *      判断是否还有下一个异常帧
     */
 //   bool _hasNextException();

    /**
     *      隐藏所有界面控件
     */
    void _hideAllControl();

    /**
     *      更新不同业务对应的菜单与标题
     */
    void _updateMenuNCaption();



    //////////////////////////////////////////////////////////////////////////
    // 报文统计

    /**
     *      创建SMV报文统计报表
     */
    void _createReportSMVStatistics();

    /**
     *      创建离散度柱状图
     */
    void _createHorHistogram();

    /**
     *      创建GOOSE报文统计报表
     */
    void _createReportGSStatistics();

    /**
     *      显示smv报文统计界面
     */
    void _showSMVStatistics();

    /**
     *      修改离散度柱状图控件参数
     */
    void _resetHistogram();
#if !SU_FEI
	void _showSMVRistorgram();
	void _switchFrameHistogram();
#endif
    /**
     *      显示goose报文统计页面
     */
    void _showGOOSEStatistics();

//////////////////////////////////////////////////////////////////////////
// 波形分析

    /**
     *      创建波形控件
     */
    void _createWaveform();

    /**
     *      设置波形控件属性
     */
    void _setWaveform();

    /**
     *      切换波形缩放与观察
     */
    void _switchWaveZoomOrWatch();

    /**
     *      根据类型与通道数获取一个模板对象
     */
    void _getTemplate(enumFrameType type);

    /**
     *      计算幅值
     */
    float _calcAmplitude(uint32_t index);

    /**
     *      上翻页通道
     */
    void _pageupChannel(PCAP_CTRLBLK* data);

    /**
     *      下翻页通道
     */
    void _pagedownChannel(PCAP_CTRLBLK* data);

    /**
     *      左翻页通道
     */
    void _pageleftChannel(PCAP_CTRLBLK* data);

    /**
     *      右翻页通道
     */
    void _pagerightChannel(PCAP_CTRLBLK* data);

    /**
     *      放大波形
     */
    void _zoominWave(PCAP_CTRLBLK* data);

    /**
     *      缩小波形
     */
    void _zoomoutWave(PCAP_CTRLBLK* data);

    /**
     *      放大时间轴
     */
    void _zoominTime(PCAP_CTRLBLK* data);

    /**
     *      缩小时间轴
     */
    void _zoomoutTime(PCAP_CTRLBLK* data);

    /**
     *      获取通道的信息
     */
    void _getChannelInfo(uint32_t index, COLORREF& color);

    /**
     *      重新计算每个格子的采样点数
     */
    inline float _calcXGridCounts(PCAP_CTRLBLK* data)
    {
        return (float)(data->wave.wsample / DSM_WORK_HERTZ_50 / 2  * pow(2, data->wave.wzoomx));
    }

    /**
     *      统计分析pcap后的状态信息
     *
     *  \note
     *      统计是否有异常帧
     */
#if SU_FEI
    void _statisticsLoadedPcap();
#endif

#if !CHUN_HUA
public:
	void _updateFrameDetailCaption() ;
	
#endif


private:
    CTreeV                  m_treeFrame;            ///< 报文帧树控件
    CReportV                m_reportFrame;          ///< 报文监测数据报表
    CReportV                m_reportSMVStatistics;  ///< SMV报文统计报表
    CReportV                m_reportGooseStatistics;///< GOOSE报文统计报表
    CHorHistogramCtrl       m_ctrlHistogram;        ///< 横向柱状图

    CRect                   m_rcTop;                ///< 离散图标题区域
    CString                 m_csSampleInter;        ///< 离散度标题

    int                     m_nMaxRows;             ///< 每页最大行数
    uint8_t*                m_pFrameBuff;           ///< 分析当前帧数据的缓存
    CString                 m_csPcapFile;           ///< 正在分析的pcap文件名
    IPcapFile*              m_pPcap;                ///< pcap文件解析器

    ModePage                m_eModePage;            ///< 当前页面的任务模式
    ModePage                m_eLastMode;            ///< 上一页面模式

    Sav91Msg*               m_pMsg91;               ///< 91协议解码器
    Sav92Msg*               m_pMsg92;               ///< 92协议解码器
    NanRuiFT3Msg*           m_pMsgNanRui;           ///< FT3南瑞协议解码器
    GuoWangFT3Msg*          m_pMsgGuoWang;          ///< FT3国网协议解码器
    GooseMsg*               m_pMsgGoose;            ///< goose解码器

    CNaviToolBar*           m_pNaviBar;             ///< 菜单容器对象
    CRNaviMenu*             m_pRNaviCurrent;        ///< 当前业务页面的菜单对象指针

    CMsgDlg*                m_pMsgDlg;              ///< 显示加载pcap状态窗口指针

    CDsmDataLayer*          m_pDataLayer;           ///< 数据层指针

    typedef std::vector<PCAP_CTRLBLK> vecCtrlBlk;

    vecCtrlBlk              m_vecCtrlBlk;           ///< 控制块表
    uint32_t                m_nCurrentAppID;        ///< 当前选中的控制块APPID
    uint32_t                m_nCurrentChanns;       ///< 当前控制块的通道数
    enumFrameType           m_eCuttentType;         ///< 当前控制块类型
    UINT                    m_nCtrlBlkCmdEnd;       ///< 控制块菜单的ID尾部
    UINT                    m_nCtrlBlkHighLight;    ///< 控制块高亮的菜单ID

    CBrush                  m_brBackground;         ///< 背景画刷
    uint32_t                m_nDetailIndex;         ///< 详细帧内容的索引

    ModeWave                m_eModeWave;            ///< 波形分析模式
    HANDLE                  m_hThread;              ///< 加载pcap线程
    DWORD                   m_dwLoadComplete;       ///< 加载pcap完成
#if SU_FEI
    bool                    m_arrUpdate[3];         ///< 判断各业务是否需要重新刷新数据
#else
	bool					m_arrUpdate[4];			///< 判断各业务是否需要重新刷新数据
#endif

#if SU_FEI
    bool                    m_bHasExceptionFrame;   ///< 判断当前加载的pcap是否有异常数据帧

    DWORD                   m_dwDirectTag;          ///< 判断上下移动的标志 1是向上，2是向下
    size_t                  m_nFirstExcep;          ///< 第一个异常
    size_t                  m_nLastExcep;           ///< 最后一个异常
#endif
    CWaveFormGramCtrl       m_ctrlWaveform;         ///< 波形分析控件
    CDataOscillogram        m_dataWave;             ///< 波形分析结果数据
    SmvChannelTemplate*     m_pChannelTemplate;     ///< 当前控制块通道模板对象指针
#if SHENG_ZE
    CProgressBarDlg*        m_pWaveDlg;             ///< 波形分析进度显示对话框
#else
	CMsgDlg*				m_pWaveDlg;				///< 波形分析进度显示对话框
#endif

#if !CHUN_HUA
	uint32_t				m_nSelectFrame;
	uint32_t				m_nSelectFrameIndex;
#endif
};
