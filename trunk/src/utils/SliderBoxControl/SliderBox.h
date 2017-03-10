#pragma once
#include <vector>
#include <imaging.h>

//
// 类名
#define SLIDERBOX_CLASSNAME         L"SliderBox"
// 调试
#define SB_DEBUGLOG                 NKDbgPrintfW

// 最小图片滑动时间
#define SB_MIN_SCROLL_TIMEOUT       100
// 最大动画频率（Hz）
#define SB_MAX_ANIMATE_FREQ         1000
// CSliderBox

class CSliderBox : public CWnd
{
	DECLARE_DYNAMIC(CSliderBox)

public:
#if !SHENG_ZE
	friend class CBasicAboutDlg;
#endif
    //======================================================================
    // 创建方法
    //====================================================================== 
	CSliderBox();    
	virtual ~CSliderBox();
    virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
        UINT nID, CCreateContext* pContext = NULL);
    virtual BOOL CreateEx(DWORD dwExStyle, DWORD dwStyle, const RECT& rect, 
        CWnd* pParentWnd, UINT nID, LPVOID lpParam = NULL);    

    //======================================================================
    // 公共方法
    //====================================================================== 
    ///
    /// @method:    LoadImages
    /// @access:    public 
    /// @brief:
    ///		载入图片
    /// @param[in]: 
    ///		szImgDir						要载入图片的文件夹
    /// @return:   
    ///		BOOL					        TRUE：成功
    ///                                     FALSE：指定目录不存在或目录中无图片
    ///
    BOOL LoadImages(const CString &szImgDir);

    ///
    /// @method:    SetScrollTimeout
    /// @access:    public 
    /// @brief:
    ///		设置一次滑动的时间
    /// @param[in]: 
    ///		nTimeout						时间（毫秒）
    /// @return:   
    ///		BOOL					        TRUE：成功
    ///                                     FALSE：小于最小滑动时间（SB_MIN_SCROLL_TIMEOUT）
    ///
    BOOL SetScrollTimeout(UINT nTimeout);
    
    ///
    /// @method:    SetAnimateFreq
    /// @access:    public 
    /// @brief:
    ///		设置动画频率
    /// @note
    ///     动画频率即一秒钟页面刷新次数（FPS），动画频率影响动画的平滑程度，频
    ///     率越高，图像越平滑
    /// @param[in]: 
    ///		nFreq						频率值
    /// @return:
    ///		BOOL					    TRUE：成功
    ///                                 FALSE：大于最大频率值（SB_MAX_ANIMATE_FREQ）
    ///
    BOOL SetAnimateFreq(UINT nFreq);


    ///
    /// @method:    GetImgIndex
    /// @access:    public 
    /// @brief:
    ///		取得当前图像索引（指定目录中的第几张图片）    
    /// @return:   
    ///		UINT					    图像索引
    ///
    inline UINT GetImgIndex();
protected:    
    ///
    /// @method:    ScrollLeft
    /// @access:    virtual protected 
    /// @brief:
    ///		图片向左滑动
    /// @param[in]: 
    ///		nTimeout						滑动时间
    /// @return:   
    ///		BOOL					        TRUE：成功
    ///                                     FALSE：如果当前是第一张图片
    ///
    virtual BOOL ScrollLeft(UINT nTimeout);
    ///
    /// @method:    ScrollLeft
    /// @access:    virtual protected 
    /// @brief:
    ///		图片向右滑动
    /// @param[in]: 
    ///		nTimeout						滑动时间
    /// @return:   
    ///		BOOL					        TRUE：成功
    ///                                     FALSE：如果当前是最后一张图片
    ///
    virtual BOOL ScrollRight(UINT nTimeout);  

    //======================================================================
    // 消息处理
    //======================================================================
	DECLARE_MESSAGE_MAP()    
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    enum TIMER_ID{TIMER_SCROLL_LEFT=10000, TIMER_SCROLL_RIGHT};
    afx_msg void OnTimer(UINT_PTR nIDEvent); 
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    
    //======================================================================
    // 辅助
    //======================================================================    
    static BOOL  RegisterWndClass (HINSTANCE hInstance, LPCTSTR lpszClassName,
        UINT style, HICON hIcon = 0, int bgStockBrush = NULL_BRUSH);

    //======================================================================
    // 绘图
    //======================================================================    
    ///
    /// @method:    DrawImgsToMem
    /// @access:    protected 
    /// @brief:
    ///		将前一张，当前，后一张图片载入内存DC缓冲区
    /// @param[in]
    ///     mode                    载入模式，用于控制以何种方式载入图片，初始化
    ///                             模式用于直接从文件载入3张图片，向左滑动和向右
    ///                             滑动模式通过交换句柄并载入滑动后所需的那张图片
    ///                             以优化载入速度
    ///                             DM_INIT:初始化模式
    ///                             DM_MOVE_LEFT:向左移动模式
    ///                             DM_MOVE_RIGHT:向右移动模式    
    /// @return:   
    ///		BOOL					TURE：成功
    ///                             FALSE：图片索引溢出
    ///
    enum DrawMode{DM_INIT, DM_MOVE_LEFT, DM_MOVE_RIGHT};
    BOOL   DrawImgsToMem(DrawMode mode = DM_INIT);


    ///
    /// @method:    DrawArrow
    /// @access:    protected 
    /// @brief:
    ///		画箭头
    /// @param[in]: 
    ///		rect						包围箭头的矩形框大小
    ///		type						箭头类型，上下左右
    ///     nWidth                      箭头线条粗细
    /// @return:   
    ///		VOID					
    ///
#if SHENG_ZE
    enum ARROW_TYPE{ARROW_UP, ARROW_DOWN, ARROW_LEFT, ARROW_RIGHT};    
    VOID DrawArrow(CDC *pDC, ARROW_TYPE type, POINT ptPeak, UINT nIncludedAngle, UINT nLength, UINT nWidth = 3);
#endif

    ///
    /// @method:    DrawIndicators
    /// @access:    protected 
    /// @brief:
    ///		画图片指示器圆点
    /// @param[in]: 
    ///		rect						包围指示器的矩形大小
    ///		nRadius						圆点的半径
    /// @return:   
    ///		BOOL					    TRUE：成功
    ///                                 FALSE：半径太大或者图片太多画不下
    ///
    BOOL DrawIndicators(CDC *pDC, RECT rect, UINT nRadius);
protected:
    UINT                        m_nImgIndex;        ///< 当前图像索引
    CString                     m_szImgDir;         ///< 当前图像文件夹
    ///< 存储所有图像文件名的向量，只支持bmp图像
    std::vector<CString>        m_vImgFileName;
    CDC                         m_memDC;            ///< 内存DC，用于组合图像    
    UINT                        m_nAnimateFreq;     ///< 动画频率（Hz）
    BOOL                        m_fScrolling;       ///< 指示是否正在滑动
    UINT                        m_nScrollTimeout;   ///< 一次滑动时间
private:
    inline CString GetFileTitle(const CString& szFileName);
    inline CString GetFileExt(const CString& szFileName);
    HBITMAP LoadImageFromFile(const TCHAR * pFileImage);
private:
    INT                         m_nScrollPixel;    
    HBITMAP                     m_hBitmap[3];
    CBitmap                     m_bitmap;
    CRect                       m_rtMapMem;
    IImagingFactory             *m_pImgFactory;
};
inline UINT CSliderBox::GetImgIndex()
{
    return m_nImgIndex;    
}

inline CString CSliderBox::GetFileTitle( const CString& szFileName )
{
    CString szFileTitle;    
    int nExtTokenIndex;
    int nFileTitleLength;     
    nExtTokenIndex = szFileName.ReverseFind('.');
    nFileTitleLength = nExtTokenIndex;    
    szFileTitle = szFileName.Left(nFileTitleLength);
    return szFileTitle;
}

inline CString CSliderBox::GetFileExt( const CString& szFileName )
{
    CString szFileExt;
    int nExtTokenIndex;
    int nFileTitleLength;
    int nFileExtLength; 
    nExtTokenIndex = szFileName.ReverseFind('.');
    nFileTitleLength = nExtTokenIndex;
    nFileExtLength = szFileName.GetLength() - nFileTitleLength - 1;
    szFileExt = szFileName.Right(nFileExtLength);
    return szFileExt;
}
