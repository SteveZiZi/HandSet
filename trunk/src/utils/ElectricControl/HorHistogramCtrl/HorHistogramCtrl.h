#ifndef HOR_HISTOGRAM_DIAL_CTRL_H_

#define HOR_HISTOGRAM_DIAL_CTRL_H_

#include <vector>

class CHorHistogramCtrl: public CStatic
{
	DECLARE_DYNAMIC(CHorHistogramCtrl);

public:
	CHorHistogramCtrl();
	virtual ~CHorHistogramCtrl();

	///
	/// @brief
	///     控件样式定制函数（背景部分）：改变后需要重新Init才生效
	///
	void SetBkgndColor(const COLORREF clr);
	void SetPillarAreaGridColor(const COLORREF clr);
	void SetPillarAreaBorderColor(const COLORREF clr);
	void SetTitleColor(const COLORREF clr);
	void SetScaleValColor(const COLORREF clr);
	void SetBmpTransColor(const COLORREF clr);

	void SetMargin(const int leftMargin, const int rightMargin, const int topMargin, const int bottomMargin);
	void SetTitle(const CString &str);
	void SetPillarAreaGrid(const int xCount, const int yCount);
	void SetXScaleInterval(const float interval);
	void SetYScaleVal(const size_t idxSeq, const CString &scaleVal);
	void SetYScaleValLen(const int len);
	void SetSpaceFromLeftBorderToPillar(int space);

	void SetTitleFont(CFont *pFont);
	void SetScaleValFont(CFont *pFont);

	void SetTitleVisible(BOOL bShow);
	void SetHorGridVisible(BOOL bShow);
	void SetVerGridVisible(BOOL bShow);
	void SetBorderVisible(int boardIdx, BOOL bShow);
	void SetXScaleValVisible(BOOL bShow);
	void SetYScaleValVisible(BOOL bShow);
	
	///
	/// @brief
	///     控件样式定制函数（前景部分）：改变后相应的项需重绘才会变化
	///
	void SetPillarValColor(const COLORREF clr);
	void SetPillarColor(const size_t idxSeq, const COLORREF clr);
	void SetPillarStyle(const size_t idxSeq, CBitmap *pBmp);
	void SetPillarBetween(const int spaceBetween);
	void SetPillarHeight(const int height);
	void SetPillarCount(const int count);
	void SetPillarValFont(CFont *pFont);
	void SetPillarValVisible(BOOL bShow);

	///
	/// @brief
	///     初始化相量图控件
	///
	void Init();

	///
	/// @brief
	///     擦除相量显示区域的全部相量信息
	///
	void ErasePillar();

	///
	/// @brief
	///     绘制柱条：叠加式（与前面绘制的柱条叠加显示）
	///     默认为柱条为自绘矩形形式，若设置了柱条样式，则柱条为贴图形式
	///
	/// @param[in]
	///     valBuf  - 保存柱条所表示的值的缓存
	/// @param[in]
	///     bufSize - 缓存大小
	/// @param[in]
	///     valUnit - 柱条所表示的值的单位
	///
	void DrawHorHistoGram(const float *valBuf,const size_t bufSize, const CString &valUnit);

	///
	/// @brief
	///     强制刷新：将当前内存设备缓冲中的内容刷新到目标内存设备环境（DC）中
	///
	/// @param[in]
	///     pDC - 指向目标DC的指针；为NULL时，则为默认的DC
	///
	void Flush(CDC* pDC);

protected:
	///
	/// @brief
	///     消息处理
	///
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	///
	/// @brief
	///     辅助操作
	///
	void CreateDCandBmp(const CRect &rt, CDC &dc, CBitmap &bmp, CBitmap **pPOldBmp);
	void DrawBkgnd(void);
	void ResetForgnd(void);
	void Release();
	void UpdatePillarAreaSpace(CDC *pDC);
	void DrawPillarAreaGrid(CDC *pDC, const CRect &pillarArea);
	void DrawPillarAreaBorder(CDC *pDC, const CRect &pillarArea);
	void DrawScale(CDC *pDC, const CRect &pillarArea);
	void DrawTitle(CDC *pDC, const CRect &pillarArea);
	void DrawPillar(CDC *pDC, const CRect &pillarArea, const float *valDataBuf, const size_t bufSize, const CString &valUnit);
	void DrawPillarWithBmp(CDC *pDC, const CRect &pillarArea, const float *valDataBuf, const size_t bufSize, const CString &valUnit);
	CString GetYScaleValMaxStr();

private:
	CDC      m_cBkgndDC;                    ///< 控件背景的缓存DC
	CDC      m_cForgndDC;                   ///< 控件前景的缓存DC
	CBitmap  m_cBkgndBmp;                   ///< 背景位图
	CBitmap *m_pOldBkgndBmp;                ///< 指向旧的背景位图的指针的缓存
	CBitmap  m_cForgndBmp;                  ///< 前景位图
	CBitmap *m_pOldForgndBmp;               ///< 指向旧的前景位图的指针的缓存
	std::vector<CBitmap *> m_vPillarStyleBmp;///< 柱条Bmp样式的缓存，其内的Bmp的生命周期由外部用户维护

	CFont    m_cTitleFont;                  ///< 标题字体
	CFont    m_cScaleValFont;               ///< X-Y标尺的字体
	CFont    m_cPillarValFont;              ///< 柱条所表示的值的字体

	COLORREF m_clTransColor;                ///< 位图透明化的颜色
	COLORREF m_clBkgndColor;		        ///< 控件背景颜色
	COLORREF m_clPillarAreaGridColor;       ///< 柱条显示区域的内部网格的线条颜色
	COLORREF m_clPillarValColor;            ///< 柱条所表示的值的颜色
	COLORREF m_clTitleColor;                ///< 柱状图标题的颜色
	COLORREF m_clScaleValColor;             ///< 柱状图的X-Y标尺的刻度的颜色
	COLORREF m_clPillarAreaBorderColor;      ///< 柱状图柱条区域边框的颜色
	std::vector<COLORREF> m_clPillarColor;  ///< 柱条的颜色
	int      m_iPillarBetween;              ///< 柱条的间距
	int      m_iPillarHeight;               ///< 柱条的垂直高度大小

	CString  m_strTitle;                    ///< 控件标题
	int      m_iPillarCount;                ///< 柱条的数量
	int      m_iXGridCount;                 ///< 柱条显示区域X标尺方向上一半的网格数
	int      m_iYGridCount;                 ///< 柱条显示区域Y标尺方向上一半的网格数
	float    m_fXScaleInterval;             ///< X标尺的分度值（即每一小格的数值）
	std::vector<CString> m_strYScaleVal;    ///< Y标尺上的刻度值
	int      m_iYScaleValLen;               ///< Y标尺刻度的像素长度
	int      m_iSpaceFromLeftBorderToPillar;///< 左边框到柱条的距离，单位：像素

	int      m_iLeftMargin;                 ///< 柱状图控件的左边距
	int      m_iRightMargin;                ///< 柱状图控件的右边距
	int      m_iTopMargin;                  ///< 柱状图控件的上边距
	int      m_iBottomMargin;               ///< 柱状图控件的下边距

	BOOL     m_bShowTitle;                  ///< 是否显示柱状图的标题
	BOOL     m_bShowHorGrid;                ///< 是否显示柱条显示区域的水平网格线
	BOOL     m_bShowVerGrid;                ///< 是否显示柱条显示区域的垂直网格线
	BOOL     m_bShowBorder[4];              ///< 是否显示柱条显示区域的边框，0：左，1：右，2：上，3：下
	BOOL     m_bShowXScaleVal;              ///< 是否显示X标尺的刻度值
	BOOL     m_bShowYScaleVal;              ///< 是否显示Y标尺的刻度值
	BOOL     m_bShowPillarVal;              ///< 是否显示柱条所表示的值

	CRect    m_rtPillarArea;                ///< 柱条显示区域的矩形域
};

///
/// @brief
///     设置控件背景颜色
///
inline void CHorHistogramCtrl::SetBkgndColor(const COLORREF clr)
{
	m_clBkgndColor = clr;
}

///
/// @brief
///     设置柱条显示区域的内部网格的线条颜色
///
inline void CHorHistogramCtrl::SetPillarAreaGridColor(const COLORREF clr)
{
	m_clPillarAreaGridColor = clr;
}

///
/// @brief
///     设置柱条显示区域的边框的线条颜色
///
inline void CHorHistogramCtrl::SetPillarAreaBorderColor(const COLORREF clr)
{
	m_clPillarAreaBorderColor = clr;
}

inline void CHorHistogramCtrl::SetPillarValColor(const COLORREF clr)
{
	m_clPillarValColor = clr;
}

///
/// @brief
///     设置控件标题的颜色
///
inline void CHorHistogramCtrl::SetTitleColor(const COLORREF clr)
{
	m_clTitleColor = clr;
}

///
/// @brief
///     设置X-Y标尺的刻度的颜色（X标尺常规柱条区域及Y标尺部分）
///
inline void CHorHistogramCtrl::SetScaleValColor(const COLORREF clr)
{
	m_clScaleValColor = clr;
}

///
/// @brief
///     设置控件贴图的透明化颜色
///
inline void CHorHistogramCtrl::SetBmpTransColor(const COLORREF clr)
{
	m_clTransColor = clr;
}

///
/// @brief
///     强制设置Y标尺刻度的像素长度
///
inline void CHorHistogramCtrl::SetYScaleValLen(const int len)
{
	m_iYScaleValLen = len;
}

///
/// @brief
///     设置柱条显示区域的左边框到柱条的距离，单位：像素
///
inline void CHorHistogramCtrl::SetSpaceFromLeftBorderToPillar(int space)
{
	m_iSpaceFromLeftBorderToPillar = space;
}

///
/// @brief
///     设置控件的边距信息
///
inline void CHorHistogramCtrl::SetMargin(const int leftMargin, const int rightMargin, const int topMargin, const int bottomMargin)
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
///     设置控件标题
///
inline void CHorHistogramCtrl::SetTitle(const CString &str)
{
	m_strTitle = str;
}

///
/// @brief
///     设置柱条的边距
///
inline void CHorHistogramCtrl::SetPillarBetween(const int spaceBetween)
{
	m_iPillarBetween = spaceBetween;
}

///
/// @brief
///     设置柱条的高度
///
inline void CHorHistogramCtrl::SetPillarHeight(const int height)
{
	m_iPillarHeight = height;
}

///
/// @brief
///     设置柱条数
///
inline void CHorHistogramCtrl::SetPillarCount(const int count)
{
	m_iPillarCount = count;
}

///
/// @brief
///     设置柱条显示区域的垂直网格数和常规柱条区域的水平网格数
///
inline void CHorHistogramCtrl::SetPillarAreaGrid(const int xCount, const int yCount)
{
	m_iXGridCount = xCount;
	m_iYGridCount = yCount;
}

///
/// @brief
///     设置X标尺的分度值
///
inline void CHorHistogramCtrl::SetXScaleInterval(const float interval)
{
	m_fXScaleInterval = interval;
}

///
/// @brief
///     设置控件标题的可见性
///
inline void CHorHistogramCtrl::SetTitleVisible(BOOL bShow)
{
	m_bShowTitle = bShow;
}

///
/// @brief
///     设置垂直网格的可见性
///
inline void CHorHistogramCtrl::SetHorGridVisible(BOOL bShow)
{
	m_bShowHorGrid = bShow;
}

///
/// @brief
///     设置水平网格的可见性
///
inline void CHorHistogramCtrl::SetVerGridVisible(BOOL bShow)
{
	m_bShowVerGrid = bShow;
}

///
/// @brief
///     设置边框的可见性
///
/// @param[in]
///     boardIdx   - 边框索引，0：左，1：右，2：上，3：下
/// @param[in]
///     bShow      - 是否可见，可见为TRUE
///
inline void CHorHistogramCtrl::SetBorderVisible(int boardIdx, BOOL bShow)
{
	if ( boardIdx >= 4 )
		return;

	m_bShowBorder[boardIdx] = bShow;
}

///
/// @brief
///     设置X标尺刻度的可见性
///
inline void CHorHistogramCtrl::SetXScaleValVisible(BOOL bShow)
{
	m_bShowXScaleVal = bShow;
}

///
/// @brief
///     设置Y标尺刻度的可见性
///
inline void CHorHistogramCtrl::SetYScaleValVisible(BOOL bShow)
{
	m_bShowYScaleVal = bShow;
}

///
/// @brief
///     设置柱条数据值的可见性
///
inline void CHorHistogramCtrl::SetPillarValVisible(BOOL bShow)
{
	m_bShowPillarVal = bShow;
}

#endif
