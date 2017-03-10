#pragma once

#include <vector>

class CWaveFormGramCtrl : public CStatic
{
	DECLARE_DYNAMIC(CWaveFormGramCtrl)

public:
	CWaveFormGramCtrl();
	virtual ~CWaveFormGramCtrl();

	///
	/// @brief
	///     控件样式定制函数（背景部分）：改变后需要重新Init才生效
	///
	void SetBkgndColor(const COLORREF clr);
	void SetTitleColor(const COLORREF clr);
	void SetWaveAreaGridColor(const COLORREF clr);
	void SetScaleValColor(const COLORREF clr);
	void SetCurSorColor(const COLORREF cursor1Clr, const COLORREF cursor2Clr);

	void SetMargin(const int leftMargin, const int rightMargin, const int topMargin, const int bottomMargin);
	void SetTitle(const size_t idxSeq, const CString &str);
	void SetWaveAreaGrid(const int xHalfGridCount, const int yHalfGridCount);
	void SetYIntervalMaxCharNum(const int num);
	void SetScaleInterval(const size_t idxSeq, const float xInterval, const float yInterval, const CString &yUint);
	void SetX0ScaleVal(size_t idxSeq, float x0Val);
	void SetXOneGridPointCount(const size_t idxSeq, const size_t xPtCount);
	void SetGramCount(const int xGramCount, const int yGramCount);
	void SetCursorScale(const CString &cursor1Scale, const CString &cursor2Scale);
	void SetDrawWaveDataBufSize(size_t szSize);
	
	void SetTitleVisible(BOOL bShow = TRUE);
	void SetHorGridVisible(BOOL bShow = TRUE, BOOL bCentreShow = TRUE);
	void SetVerGridVisible(BOOL bShow = TRUE);
	void SetXScaleValVisible(BOOL bShow = TRUE);
	void SetYScaleValVisible(BOOL bShow = TRUE);
	void SetCursor1Visible(BOOL bShowCursor = TRUE, BOOL bShowDesc = TRUE, BOOL bShowScale = TRUE);
	void SetCursor2Visible(BOOL bShowCursor = TRUE, BOOL bShowDesc = TRUE, BOOL bShowScale = TRUE);
	
	void SetTitleFont(CFont *pFont);
	void SetScaleValFont(CFont *pFont);
	void SetCursorDescFont(CFont *pFont);

	///
	/// @brief
	///     控件样式定制函数（前景部分）：改变后相应的项需重绘才会变化
	///
	void SetWaveColor(const size_t gramIdx, const size_t waveIdx, const COLORREF clr);
	void SetWaveDescColor(const size_t gramIdx, const size_t waveIdx, const COLORREF clr);
	void SetReduceRatio(const float ratio);
	void SetWaveDescVisible(const BOOL bShow = TRUE);
	void SetWaveDescFont(CFont *pFont);
	void SetWaveDrawCount(size_t waveDrawCount);

	///
	/// @brief
	///     初始化波形图控件
	///
	void Init();

	///
	/// @brief
	///     擦除波形显示区域的全部波形信息
	///
	void EraseWave();

	///
	/// @brief
	///     设置待绘制波形的采样点数据，与DrawWaveFromBuf()配合使用
	///
	/// @param[in]
	///     idxSeq    - 波形图的顺序号（从左往右，再从上到下），从0开始
	/// @param[in]
	///     dataBuf   - 保存波形数据的缓存
	/// @param[in]
	///     bufSize   - 缓存大小
	///
	/// @return
	///     
	///
	void SetWaveData(const size_t idxSeq, const float *dataBuf, const size_t bufSize);

	///
	/// @brief
	///     根据缓存的波形数据绘制波形，与SetWaveData()配合使用
	///
	/// @param[in]
	///     idxSeq    - 波形图的顺序号（从左往右，再从上到下），从0开始
	/// @param[in]
	///     waveDesc  - 波形的描述信息，默认为2行，用“\r\n”分开
	///
	/// @return
	///     
	///
	void DrawWaveFromBuf(const size_t idxSeq, const CString &waveDesc);

	///
	/// @brief
	///     绘制波形：叠加式（与前面绘制的波形叠加显示）
	///
	/// @param[in]
	///     idxSeq    - 波形图的顺序号（从左往右，再从上到下），从0开始
	/// @param[in]
	///     dataBuf   - 保存波形数据的缓存
	/// @param[in]
	///     bufSize   - 缓存大小
	/// @param[in]
	///     waveDesc  - 波形的描述信息，默认为2行，用“\r\n”分开
	///
	void DrawWave(const size_t idxSeq, const float *dataBuf, const size_t bufSize, const CString &waveDesc);

	///
	/// @brief
	///     绘制游标的描述信息波形：叠加式（与前面绘制的部分叠加显示）
	///
	/// @param[in]
	///     cursor1Desc   - 游标1的描述信息
	/// @param[in]
	///     cursor2Desc   - 游标2的描述信息
	///
	void DrawCursorDesc(const CString &cursor1Desc, const CString &cursor2Desc);
	
	///
	/// @brief
	///     强制刷新：将当前内存设备缓冲中的内容刷新到目标内存设备环境（DC）中
	///
	/// @param[in]
	///     pDC - 指向目标DC的指针；为NULL时，则为默认的DC
	///
	void Flush(CDC* pDC);

	///
	/// @brief
	///     获取游标1、2所在的点相对于波形数据缓存的索引位置
	///
	int  GetCursor1Pos(size_t idxSeq);
	int  GetCursor2Pos(size_t idxSeq);

	///
	/// @brief
	///     右移游标
	///
	void Cursor2MoveRight();

	///
	/// @brief
	///     左移游标
	///
	void Cursor2MoveLeft();

protected:
	///
	/// @brief
	///     消息处理
	///
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnDestory();

	///
	/// @brief
	///     辅助操作
	///
	void CreateDCandBmp(const CRect &rt, CDC &dc, CBitmap &bmp, CBitmap **pPOldBmp);
	void DrawBkgnd();
	void ResetForgnd();
	void Release();
	void UpdateWaveAreaSpace(CDC *pDC);
	void DrawWaveAreaGrid(CDC *pDC, const CRect &waveArea);
	void DrawWaveAreaBoard(CDC *pDC, const CRect &waveArea);
	void DrawScale(CDC *pDC, const size_t idxSeq, const CRect &waveArea);
	void DrawTitle(CDC *pDC, const size_t idxSeq, const CRect &waveArea);
	void DrawCursor1(CDC *pDC, const size_t idxSeq, const CRect &waveArea);
	void DrawCursor2(CDC *pDC, const size_t idxSeq, const CRect &waveArea);

private:
	CDC      m_cBkgndDC;                    ///< 控件背景的缓存DC
	CDC      m_cForgndDC;                   ///< 控件前景的缓存DC
	CBitmap  m_cBkgndBmp;                   ///< 背景位图
	CBitmap *m_pOldBkgndBmp;                ///< 指向旧的背景位图的指针的缓存
	CBitmap  m_cForgndBmp;                  ///< 前景位图
	CBitmap *m_pOldForgndBmp;               ///< 指向旧的前景位图的指针的缓存

	CFont    m_cTitleFont;                  ///< 标题字体
	CFont    m_cScaleValFont;               ///< X-Y标尺的字体
	CFont    m_cWaveDescFont;               ///< 波形描述信息的字体
	CFont    m_cCursorDescFont;

	COLORREF m_clBkgndColor;                ///< 波形图控件的背景颜色
	COLORREF m_clWaveAreaGridColor;         ///< 信号波显示区域的内部网格的颜色
	COLORREF m_clTitleColor;                ///< 波形图标题的颜色
	COLORREF m_clScaleValColor;             ///< 波形图的X-Y标尺的刻度的颜色
	std::vector<std::vector<COLORREF> *> m_clWaveColor;    ///< 信号波的颜色
	std::vector<std::vector<COLORREF> *> m_clWaveDescColor;///< 信号波的描述信息的颜色
	COLORREF m_clCursor1Color;              ///< 游标1的颜色
	COLORREF m_clCursor2Color;              ///< 游标2的颜色

	CString  m_strCursor1Scale;             ///< 游标1的标尺描述
	CString  m_strCursor2Scale;             ///< 游标2的标尺描述

	std::vector<size_t> m_szWaveDataMaxSize;///< 各波形图中波形的最大采样点数
	std::vector<size_t> m_szCursor1Pos;     ///< 各波形图中游标1所在的点相对于波形数据缓存的索引位置
	std::vector<size_t> m_szCursor2Pos;     ///< 各波形图中游标2所在的点相对于波形数据缓存的索引位置
	size_t   m_szWaveDrawCount;             ///< 组成绘制的波形的点的数量，即波形图当前显示的波形的采样点数

	std::vector<CString> m_strTitle;        ///< 各波形图标题
	int      m_iXHalfGridCount;             ///< 信号波显示区域X标尺方向上一半的网格数
	int      m_iYHalfGridCount;             ///< 信号波显示区域Y标尺方向上一半的网格数
	int      m_iXGramCount;                 ///< X标尺方向上的波形图个数
	int      m_iYGramCount;                 ///< Y标尺方向上的波形图个数
	std::vector<float> m_fXScaleInterval;   ///< 各波形图的X标尺的分度值（即每一小格对应的数据值）
	std::vector<float> m_fYScaleInterval;   ///< 各波形图的Y标尺的分度值（即每一小格对应的数据值）
	std::vector<CString> m_strYUnit;        ///< Y标尺的单位
	int      m_iYInterValCharNum;           ///< Y标尺分度值的最大字符数
	std::vector<size_t> m_szXOneGridPointCount;///< 各波形图的X标尺方向的每一网格的点数

	int      m_iLeftMargin;                 ///< 波形图控件的左边距
	int      m_iRightMargin;                ///< 波形图控件的右边距
	int      m_iTopMargin;                  ///< 波形图控件的上边距
	int      m_iBottomMargin;               ///< 波形图控件的下边距

	BOOL     m_bShowTitle;                  ///< 是否显示波形图的标题
	BOOL     m_bShowWaveDesc;               ///< 是否显示信号波的描述信息
	BOOL     m_bShowWaveAreaHorGrid;        ///< 是否显示波形显示区域的内部网格的水平网格
	BOOL     m_bShowWaveAreaHorCentreGrid;  ///< 是否显示波形显示区域的内部网格的水平网格的中间线
	BOOL     m_bShowWaveAreaVerGrid;        ///< 是否显示波形显示区域的内部网格的垂直网格
	BOOL     m_bShowXScaleVal;              ///< 是否显示X标尺的刻度值
	BOOL     m_bShowYScaleVal;              ///< 是否显示Y标尺的刻度值
	BOOL     m_bShowCursor1;                ///< 是否显示游标1
	BOOL     m_bShowCursor2;                ///< 是否显示游标2
	BOOL     m_bShowCursor1Desc;            ///< 是否显示游标1的描述信息
	BOOL     m_bShowCursor2Desc;            ///< 是否显示游标2的描述信息
	BOOL     m_bShowCursor1Scale;           ///< 是否显示游标1的标尺
	BOOL     m_bShowCursor2Scale;           ///< 是否显示游标2的标尺

	CRect    m_rtWaveArea;                  ///< 第1个(0,0)波形显示区域的矩形域
	int      m_iXGramSpace;                 ///< 一个波形图在X标尺方向上的宽度
	int      m_iYGramSpace;                 ///< 一个波形图在Y标尺方向上的宽度
	std::vector<size_t> m_szCurWaveCount;   ///< 各波形图的当前的信号波数

	POINT  **m_pDrawWaveDataBuf;            ///< 指向待绘制波形的数据缓存区的指针
	size_t m_szDrawWaveDataBufSize;         ///< 待绘制波形的数据缓存区的大小
	std::vector<size_t> m_vDrawWaveDataNum; ///< 待绘制波形波形数据的实际大小
	std::vector<float> m_fX0ScaleVal;       ///< X标尺的起始点X0的刻度值
	
	DECLARE_MESSAGE_MAP()
};

///
/// @brief
///     设置控件的背景颜色
///
inline void CWaveFormGramCtrl::SetBkgndColor(const COLORREF clr)
{
	m_clBkgndColor = clr;
}

///
/// @brief
///     设置控件标题的颜色
///
inline void CWaveFormGramCtrl::SetTitleColor(const COLORREF clr)
{
	m_clTitleColor = clr;
}

///
/// @brief
///     设置控件波形显示区域的内部网格的颜色
///
inline void CWaveFormGramCtrl::SetWaveAreaGridColor(const COLORREF clr)
{
	m_clWaveAreaGridColor = clr;
}

///
/// @brief
///     设置控件标尺的刻度的颜色
///
inline void CWaveFormGramCtrl::SetScaleValColor(const COLORREF clr)
{
	m_clScaleValColor = clr;
}

///
/// @brief
///     设置控件游标的颜色
///
inline void CWaveFormGramCtrl::SetCurSorColor(const COLORREF cursor1Clr, const COLORREF cursor2Clr)
{
	m_clCursor1Color = cursor1Clr;
	m_clCursor2Color = cursor2Clr;
}

///
/// @brief
///     设置控件游标的标尺
///
inline void CWaveFormGramCtrl::SetCursorScale(const CString &cursor1Scale, const CString &cursor2Scale)
{
	m_strCursor1Scale = cursor1Scale;
	m_strCursor2Scale = cursor2Scale;
}

///
/// @brief
///     设置控件的边距信息
///
inline void CWaveFormGramCtrl::SetMargin(const int leftMargin, const int rightMargin, const int topMargin, const int bottomMargin)
{
	if ( leftMargin >= 0)
		m_iLeftMargin   = leftMargin;
	if ( rightMargin >= 0 )
		m_iRightMargin  = rightMargin;
	if ( topMargin >= 0 )
		m_iTopMargin    = topMargin;
	if ( bottomMargin >= 0 )
		m_iBottomMargin = bottomMargin;
}

///
/// @brief
///     设置控件的内部网格X-Y标尺方向上的网格数
///
inline void CWaveFormGramCtrl::SetWaveAreaGrid(const int xHalfGridCount, const int yHalfGridCount)
{
	m_iXHalfGridCount     = xHalfGridCount;
	m_iYHalfGridCount     = yHalfGridCount;
}

///
/// @brief
///     设置控件的Y标尺分度值的最大字符数
///
inline void CWaveFormGramCtrl::SetYIntervalMaxCharNum(const int num)
{
	m_iYInterValCharNum = num;
}

///
/// @brief
///     设置标题的可见性
///
inline void CWaveFormGramCtrl::SetTitleVisible(BOOL bShow /* = TRUE */)
{
	m_bShowTitle = bShow;
}

///
/// @brief
///     设置水平网格的可见性
///
inline void CWaveFormGramCtrl::SetHorGridVisible(BOOL bShow /* = TRUE */, BOOL bCentreShow /* = TRUE */)
{
	m_bShowWaveAreaHorGrid = bShow;
	m_bShowWaveAreaHorCentreGrid = bCentreShow;
}

///
/// @brief
///     设置垂直网格的可见性
///
inline void CWaveFormGramCtrl::SetVerGridVisible(BOOL bShow /* = TRUE */)
{
	m_bShowWaveAreaVerGrid = bShow;
}

///
/// @brief
///     设置X标尺的刻度的可见性
///
inline void CWaveFormGramCtrl::SetXScaleValVisible(BOOL bShow /* = TRUE */)
{
	m_bShowXScaleVal = bShow;
}

///
/// @brief
///     设置Y标尺的刻度的可见性
///
inline void CWaveFormGramCtrl::SetYScaleValVisible(BOOL bShow /* = TRUE */)
{
	m_bShowYScaleVal = bShow;
}

///
/// @brief
///     设置波形描述信息（位于左上角）的可见性
///
inline void CWaveFormGramCtrl::SetWaveDescVisible(const BOOL bShow /* = TRUE */)
{
	m_bShowWaveDesc = bShow;
}

///
/// @brief
///     设置游标1（位置固定在起点处）及其描述信息和标尺的可见性
///
inline void CWaveFormGramCtrl::SetCursor1Visible(BOOL bShowCursor /* = TRUE */, BOOL bShowDesc /* = TRUE */, BOOL bShowScale /* = TRUE */)
{
	m_bShowCursor1      = bShowCursor;
	m_bShowCursor1Desc  = bShowDesc;
	m_bShowCursor1Scale = bShowScale;
}

///
/// @brief
///     设置游标2（位置可变）及其描述信息和标尺的可见性
///
inline void CWaveFormGramCtrl::SetCursor2Visible(BOOL bShowCursor /* = TRUE */, BOOL bShowDesc /* = TRUE */, BOOL bShowScale /* = TRUE */)
{
	m_bShowCursor2      = bShowCursor;
	m_bShowCursor2Desc  = bShowDesc;
	m_bShowCursor2Scale = bShowScale;
}
