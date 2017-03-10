#pragma once

#include <vector>

#define HTG_FEATURE_PILLAR_COUNT 5

///
/// @brief
///     特写柱条项信息
///
struct FEATURE_PILLAR_ITEM
{
	CRect rect;		///< 柱条顶部的文字的矩形区域的大小
	int idx;		///< 所要特写的柱条在柱状图中的顺序号，从左往右，从0开始
	FEATURE_PILLAR_ITEM()
		:idx(0)
	{

	}
}; 

class CHistogramCtrl : public CStatic
{
	DECLARE_DYNAMIC(CHistogramCtrl)

public:
	CHistogramCtrl();
	virtual ~CHistogramCtrl();

	///
	/// @brief
	///     控件样式定制函数（背景部分）：改变后需要重新Init才生效
	///
	void SetBkgndColor(const COLORREF clr);
	void SetPillarAreaGridColor(const COLORREF clr);
	void SetTitleColor(const COLORREF clr);
	void SetScaleValColor(const COLORREF clr);
	void SetSpcXScaleValColor(const COLORREF clr);

	void SetMargin(const int leftMargin, const int rightMargin, const int topMargin, const int bottomMargin);
	void SetTitle(const size_t idxSeq, const CString &str);
	void AddASpecialPillar(const CString &xScaleStr);
	void RemoveASpecialPillar(const size_t idxSeq);
	void SetScaleInterval(const size_t idxSeq, const float xInterval, const float yInterval, const CString &yUint);
	void SetPillarAreaGird(const int xGridCount, const int yGridCount);
	void SetCommonPillarCount(const int count);
	void SetGramCount(const int xGramCount, const int yGramCount);

	void SetTitleVisible(BOOL bShow = TRUE);
	void SetHorGridVisible(const BOOL bShow = TRUE);
	void SetVerGridVisible(const BOOL bShow = TRUE);
	void SetXScaleValVisible(BOOL bShow = TRUE);
	void SetYScaleValVisible(BOOL bShow = TRUE);

	void SetTitleFont(CFont *pFont);
	void SetScaleValFont(CFont *pFont);

	///
	/// @brief
	///     控件样式定制函数（前景部分）：改变后相应的项需重绘才会变化
	///
	void SetPillarColor(const size_t idxSeq, const COLORREF clr);
	void SetCursorColor(const size_t idxSeq, const COLORREF clr);
	void SetFeatureInfoColor(const COLORREF clr);
	void SetFeaturePillar(const int itemIdx, const int pillarIdx);
	void SetCursorVisible(BOOL bShow = TRUE);
	void SetPillarDescFont(CFont *pFont);
	void SetFeatureInfoFont(CFont *pFont);

	///
	/// @brief
	///     获取数据
	///
	int  GetCursorPos();
	int  GetSpecialPillarCount();
	int  GetCommonPillarCount();

	///
	/// @brief
	///     初始化柱状图控件
	///
	void Init();

	///
	/// @brief
	///     擦除柱条显示区域的全部柱条信息
	///
	void ErasePillar();

	///
	/// @brief
	///     绘制柱条：叠加式（与前面绘制的柱条叠加显示）
	///
	/// @param[in]
	///     idxSeq        - 柱状图的顺序号（按从左往右，再从上到下的顺序），从0开始
	/// @param[in]
	///     cmnPillarData - 保存常规柱条的数据值的缓存
	/// @param[in]
	///     spcPillarData - 保存特殊（自定义）柱条的数据值的缓存
	/// @param[in]
	///     curPillarDesc - 当前游标所在的柱条的描述信息
	///
	void DrawHistogram(const size_t idxSeq, const float *cmnPillarData,const float *spcPillarData,const CString &curPillarDesc);

	///
	/// @brief
	///     强制刷新：将当前内存设备缓冲中的内容刷新到目标内存设备环境（DC）中
	///
	/// @param[in]
	///     pDC - 指向目标DC的指针；为NULL时，则为默认的DC
	///
	void Flush(CDC *pDC);

	///
	/// @brief
	///     右移游标
	///
	void CursorMoveRight();

	///
	/// @brief
	///     左移游标
	///
	void CursorMoveLeft();

protected:
	///
	/// @brief
	///     消息处理
	///
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	///
	/// @brief
	///     辅助操作
	///
	void CreateDCandBmp(const CRect &rt, CDC &dc, CBitmap &bmp, CBitmap **pPOldBmp);
	void DrawBkgnd();
	void ResetForgnd();
	void Release();
	void UpdatePillarAreaSpace(CDC *pDC);
	void DrawPillarAreaGrid(CDC *pDC, const CRect &pillarArea);
	void DrawPillarAreaBoard(CDC *pDC, const CRect &pillarArea);
	void DrawScale(CDC *pDC, const size_t idxSeq, const CRect &pillarArea);
	void DrawTitle(CDC *pDC, const size_t idxSeq, const CRect &pillarArea);
	void DrawPillar(CDC *pDC, const size_t idxSeq, const CRect &pillarArea, const float *cmnPillarData, const float *spcPillarData);
	void DrawFeaturePillar(CDC *pDC,  const size_t idxSeq, const CRect &pillarArea, const float *cmnPillarData, const float *spcPillarData);
	void DrawCursor(CDC *pDC, const size_t idxSeq, const CRect &pillarArea);
	void DrawPillarDesc(CDC *pDC, const size_t idxSeq, const CRect &pillarArea, const CString &pillarDesc);

	bool IsIntersect(const CRect &rt1,const CRect &rt2);

private:
	CDC      m_cBkgndDC;                    ///< 控件背景的缓存DC
	CDC      m_cForgndDC;                   ///< 控件前景的缓存DC
	CBitmap  m_cBkgndBmp;                   ///< 背景位图
	CBitmap *m_pOldBkgndBmp;                ///< 指向旧的背景位图的指针的缓存
	CBitmap  m_cForgndBmp;                  ///< 前景位图
	CBitmap *m_pOldForgndBmp;               ///< 指向旧的前景位图的指针的缓存

	CFont    m_cTitleFont;                  ///< 标题字体
	CFont    m_cScaleValFont;               ///< X-Y标尺的字体
	CFont    m_cPillarDescFont;             ///< 柱条描述信息的字体
	CFont    m_cFeatureFont;                ///< 柱条的特写信息的字体

	FEATURE_PILLAR_ITEM m_FeaturePillar[HTG_FEATURE_PILLAR_COUNT];
	                                        ///< 用于保存5个特写柱条的信息，默认有0、3、5、7、9次
									        ///< 当有重合时，另算

	COLORREF m_clBkgndColor;                ///< 柱状图控件的背景颜色
	COLORREF m_clPillarAreaGridColor;       ///< 柱条显示区域的内部网格的线条颜色
	COLORREF m_clFeatureInfoColor;          ///< 柱条的特写信息的颜色
	COLORREF m_clTitleColor;                ///< 柱状图标题的颜色
	COLORREF m_clScaleValColor;             ///< 柱状图的X-Y标尺的刻度的颜色
	COLORREF m_clSpcScaleValColor;          ///< X标尺上特殊柱条的刻度的颜色
	std::vector<COLORREF> m_clPillarColor;  ///< 柱条的颜色
	std::vector<COLORREF> m_clCursorColor;	///< 游标的颜色

	std::vector<CString> m_strTitle;        ///< 各柱状图标题
	int      m_iCommonPillarCount;          ///< 常规柱条的数量
	int      m_iCursorPos;                  ///< 当前游标所在位置
	int      m_iXGridCount;                 ///< 柱条显示区域X标尺方向上的网格数
	int      m_iYGridCount;                 ///< 柱条显示区域Y标尺方向上的网格数
	int      m_iXGramCount;                 ///< X标尺方向上的柱状图个数
	int      m_iYGramCount;                 ///< Y标尺方向上的柱状图个数
	std::vector<CString> m_strSpcScaleVal;  ///< 特殊柱条在X标尺上的刻度值
	std::vector<float> m_fXScaleInterval;   ///< 各柱状图图的X标尺的分度值（即每一小格的数值）
	std::vector<float> m_fYScaleInterval;   ///< 各柱状图的Y标尺的分度值
	std::vector<CString> m_strYUnit;        ///< Y标尺的单位

	int      m_iLeftMargin;                 ///< 柱状图控件的左边距
	int      m_iRightMargin;                ///< 柱状图控件的右边距
	int      m_iTopMargin;                  ///< 柱状图控件的上边距
	int      m_iBottomMargin;               ///< 柱状图控件的下边距

	BOOL     m_bShowTitle;                  ///< 是否显示柱状图的标题
	BOOL     m_bShowPillarAreaHorGrid;      ///< 是否显示柱条显示区域的内部网格的水平网格
	BOOL     m_bShowPillarAreaVerGrid;      ///< 是否显示柱条显示区域的内部网格的垂直网格
	BOOL     m_bShowXScaleVal;              ///< 是否显示X标尺的刻度值
	BOOL     m_bShowYScaleVal;              ///< 是否显示Y标尺的刻度值
	BOOL     m_bShowCursor;                 ///< 是否显示游标

	CRect    m_rtPillarArea;                ///< 第1个(0,0)柱条显示区域的矩形域
	int      m_iXGramSpace;                 ///< 一个柱状图在X标尺方向上的宽度
	int      m_iYGramSpace;                 ///< 一个柱状图在Y标尺方向上的宽度
};

///
/// @brief
///     设置控件的背景颜色
///
inline void CHistogramCtrl::SetBkgndColor(const COLORREF clr)
{
	m_clBkgndColor = clr;
}

///
/// @brief
///     设置柱条显示区域的内部网格的线条颜色
///
inline void CHistogramCtrl::SetPillarAreaGridColor(const COLORREF clr)
{
	m_clPillarAreaGridColor = clr;
}

///
/// @brief
///     设置特写柱条的特写信息的颜色
///
inline void CHistogramCtrl::SetFeatureInfoColor(const COLORREF clr)
{
	m_clFeatureInfoColor = clr;
}

///
/// @brief
///     设置控件标题的颜色
///
inline void CHistogramCtrl::SetTitleColor(const COLORREF clr)
{
	m_clTitleColor = clr;
}

///
/// @brief
///     设置X-Y标尺的刻度的颜色（X标尺常规柱条区域及Y标尺部分）
///
inline void CHistogramCtrl::SetScaleValColor(const COLORREF clr)
{
	m_clScaleValColor = clr;
}

///
/// @brief
///     设置X标尺上特殊柱条的刻度的颜色
///
inline void CHistogramCtrl::SetSpcXScaleValColor(const COLORREF clr)
{
	m_clSpcScaleValColor = clr;
}

///
/// @brief
///     设置控件的边距信息
///
inline void CHistogramCtrl::SetMargin(const int leftMargin, const int rightMargin, const int topMargin, const int bottomMargin)
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
///     添加1条特殊柱条
///
/// @param[in]
///     xScaleStr - 特殊柱条在X标尺上的刻度值
///
inline void CHistogramCtrl::AddASpecialPillar(const CString &xScaleStr)
{
	m_strSpcScaleVal.push_back(xScaleStr);
}

///
/// @brief
///     设置柱条显示区域的垂直网格数和常规柱条区域的水平网格数
///
inline void CHistogramCtrl::SetPillarAreaGird(const int xGridCount, const int yGridCount)
{
	m_iXGridCount = xGridCount;
	m_iYGridCount = yGridCount;
}

///
/// @brief
///     设置常规柱条区域的柱条数
///
inline void CHistogramCtrl::SetCommonPillarCount(const int count)
{
	m_iCommonPillarCount = count;
}

///
/// @brief
///     设置特写柱条项
///
/// @param[in]
///     itemIdx   - 特写项的顺序号，从0开始
/// @param[in]
///     pillarIdx - 该特写项所指向的柱条的顺序号（常规+特殊），从0开始，从左向右
///
inline void CHistogramCtrl::SetFeaturePillar(const int itemIdx, const int pillarIdx)
{
	if ( itemIdx > 5 )
		return;
	m_FeaturePillar[itemIdx].idx = pillarIdx;
}

///
/// @brief
///     设置控件标题的可见性
///
inline void CHistogramCtrl::SetTitleVisible(BOOL bShow /* = TRUE */)
{
	m_bShowTitle = bShow;
}

///
/// @brief
///     设置水平网格的可见性
///
inline void CHistogramCtrl::SetHorGridVisible(const BOOL bShow /* = TRUE */)
{
	m_bShowPillarAreaHorGrid = bShow;
}

///
/// @brief
///     设置垂直网格的可见性
///
inline void CHistogramCtrl::SetVerGridVisible(const BOOL bShow /* = TRUE */)
{
	m_bShowPillarAreaVerGrid = bShow;
}

///
/// @brief
///     设置X标尺刻度的可见性
///
inline void CHistogramCtrl::SetXScaleValVisible(BOOL bShow /* = TRUE */)
{
	m_bShowXScaleVal = bShow;
}

///
/// @brief
///     设置Y标尺刻度的可见性
///
inline void CHistogramCtrl::SetYScaleValVisible(BOOL bShow /* = TRUE */)
{
	m_bShowYScaleVal = bShow;
}

///
/// @brief
///     设置游标的可见性
///
inline void CHistogramCtrl::SetCursorVisible(BOOL bShow /* = TRUE */)
{
	m_bShowCursor = bShow;
}

///
/// @brief
///     获取游标的当前位置，即位于第N条柱条，从0开始，从左向右
///
inline int  CHistogramCtrl::GetCursorPos()
{
	return m_iCursorPos;
}

///
/// @brief
///     获取特殊柱条的数量
///
inline int  CHistogramCtrl::GetSpecialPillarCount()
{
	return (int)m_strSpcScaleVal.size();
}

///
/// @brief
///     获取常规柱条的数量
///
inline int  CHistogramCtrl::GetCommonPillarCount()
{
	return m_iCommonPillarCount;
}

///
/// @brief
///     右移游标
///
inline void CHistogramCtrl::CursorMoveRight()
{
	m_iCursorPos = int((m_iCursorPos+1) % (m_strSpcScaleVal.size()+m_iCommonPillarCount));
}

///
/// @brief
///     左移游标
///
inline void CHistogramCtrl::CursorMoveLeft()
{
	m_iCursorPos = int((m_iCursorPos+m_iCommonPillarCount+m_strSpcScaleVal.size()-1) % (m_strSpcScaleVal.size()+m_iCommonPillarCount));
}
