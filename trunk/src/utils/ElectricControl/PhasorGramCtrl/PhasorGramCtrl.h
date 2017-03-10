#pragma once

#include <vector>

class CPhasorGramCtrl: public CStatic
{
    DECLARE_DYNAMIC(CPhasorGramCtrl);

public:
    CPhasorGramCtrl();
    virtual ~CPhasorGramCtrl();

	///
	/// @brief
	///     控件样式定制函数（背景部分）：改变后需要重新Init才生效
	///
	void SetBkgndColor(const COLORREF clr);
	void SetTitleColor(const COLORREF clr);
	void SetScaleValColor(const COLORREF clr);
	void SetRadiusColor(const COLORREF clr);
	void SetExCircleColor(const COLORREF clr);
	void SetInCircleColor(const COLORREF clr);

	void SetMargin(const int leftMargin, const int rightMargin, const int topMargin, const int bottomMargin);
	void SetCircleCount(const int count);
	void SetHalfSectorCount(const int count);
	void SetLenScaleInterval(const float interval);

	void SetTitleVisible(BOOL bShow = TRUE);
	void SetDegreeScaleValVisible(BOOL bShow = TRUE);
	void SetLenScaleValVisible(BOOL bShow = TRUE);

	void SetTitleFont(CFont *pFont);
	void SetScaleValFont(CFont *pFont);

	///
	/// @brief
	///     控件样式定制函数（前景部分）：改变后相应的项需重绘才会变化
	///
	void SetPhasorColor(const size_t idxSeq, const COLORREF clr);
	void SetPhasorDesc(BOOL bShow = TRUE);
	void SetPhasorDescFont(CFont *pFont);
	void SetPhasorExtDescFont(CFont *pFont);

	///
	/// @brief
	///     初始化相量图控件
	///
    void Init();

	///
	/// @brief
	///     擦除相量显示区域的全部相量信息
	///
	void ErasePhasor();

	///
	/// @brief
	///     绘制相量：叠加式（与前面绘制的相量叠加显示）
	///
	/// @param[in]
	///     lenBuf    - 保存相量长度值信息的缓存
	/// @param[in]
	///     degreeBuf - 保存相量角度值信息的缓存
	/// @param[in]
	///     descBuf   - 保存相量的描述信息的缓存(位于相量的箭头处)
	/// @param[in]
	///     bufSize   - 缓存大小
	///
	void DrawPhasor(const float *lenBuf, const float *degreeBuf, const CString *descBuf, const size_t bufSize);

	///
	/// @brief
	///     绘制相量的扩展描述信息：叠加式（与前面绘制的内容叠加显示）
	///
	/// @param[in]
	///     leftTop   - 描述信息的绘制区域的左上角坐标
	/// @param[in]
	///     descClr   - 描述信息的颜色
	/// @param[in]
	///     descBuf   - 保存相量的扩展描述信息的缓存
	/// @param[in]
	///     bufSize   - 缓存大小
	///
	void DrawExtDesc(const CPoint leftTop, const COLORREF descClr, const CString *descBuf, const size_t bufSize);

    ///
    /// @brief
    ///     绘制相量的扩展描述信息：叠加式（与前面绘制的内容叠加显示）
    ///
    /// @param[in]
    ///     csDesc      描述信息
    /// @param[in]
    ///     rcRect    - 描述信息的区域
    /// @param[in]
    ///     crColor   - 描述信息的颜色
    /// @param[in]
    ///     nFormat   - 输出格式
    ///
    void DrawExtDesc(CString csDesc, CRect rcRect, const COLORREF crColor, UINT nFormat = DT_VCENTER | DT_LEFT | DT_SINGLELINE);

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
	void UpdatePhasorAreaRange(CDC *pDC);
	void DrawCircle(CDC *pDC);
	void DrawRadius(CDC *pDC);
	void DrawScale(CDC *pDC);
	void DrawTitle(CDC *pDC);
	void DrawPhasorArrows(CDC *pDC,const CPoint &beginPt, const CPoint &endPt, const size_t idxSeq);
	void DrawPhasorDesc(CDC *pDC,const CPoint *endPt, const CString *desc, const size_t bufSize);

	bool IsIntersect(const CRect &rt1,const CRect &rt2);

private:
	CDC      m_cBkgndDC;                    ///< 控件背景的缓存DC
	CDC      m_cForgndDC;                   ///< 控件前景的缓存DC
	CBitmap  m_cBkgndBmp;                   ///< 背景位图
	CBitmap *m_pOldBkgndBmp;                ///< 指向旧的背景位图的指针的缓存
	CBitmap  m_cForgndBmp;                  ///< 前景位图
	CBitmap *m_pOldForgndBmp;               ///< 指向旧的前景位图的指针的缓存

	CFont    m_cTitleFont;                  ///< 控件标题的字体
	CFont    m_cScaleValFont;               ///< 控件标尺的字体
	CFont    m_cPhasorDescFont;             ///< 相量描述信息的字体（位于相量箭头处）
	CFont    m_cPhasorExtDescFont;          ///< 相量的扩展描述信息的字体（用户自定义位置）

	COLORREF m_clBkgndColor;		        ///< 控件背景颜色
	COLORREF m_clTitleColor;                ///< 控件标题的颜色
	COLORREF m_clScaleValColor;             ///< 控件标尺的颜色
	COLORREF m_clRadiusColor;		        ///< 雷达图的半径线条颜色
	COLORREF m_clExCircleColor;		        ///< 雷达图的外圆曲线颜色
	COLORREF m_clInCircleColor;		        ///< 雷达图的内圆曲线颜色
	std::vector<COLORREF> m_clPhasorColor;  ///< 相量的颜色

	CString  m_strTitle;
	int      m_iCircleCount;                ///< 雷达图中的圆的数量
	int      m_iHalfSectorCount;            ///< 雷达图中半数的扇面的数量
	float    m_fLenScaleInterval;           ///< 单位圆半径代表的刻度值

	int      m_iLeftMargin;                 ///< 相量图控件的左边距
	int      m_iRightMargin;                ///< 相量图控件的右边距
	int      m_iTopMargin;                  ///< 相量图控件的上边距
	int      m_iBottomMargin;               ///< 相量图控件的下边距

	BOOL     m_bShowTitle;                  ///< 是否显示标题
	BOOL     m_bShowDegreeScaleVal;         ///< 是否显示角度标尺
	BOOL     m_bShowLenScaleVal;            ///< 是否显示长度标尺
	BOOL     m_bShowPhasorDesc;             ///< 是否显示相量描述

	CRect    m_rtPhasorArea;                ///< 相量显示区域（雷达图）的矩形域
	
	DECLARE_MESSAGE_MAP()
};

///
/// @brief
///     设置控件的背景颜色
///
inline void CPhasorGramCtrl::SetBkgndColor(const COLORREF clr)
{
	m_clBkgndColor = clr;
}

///
/// @brief
///     设置控件标题的颜色
///
inline void CPhasorGramCtrl::SetTitleColor(const COLORREF clr)
{
	m_clTitleColor = clr;
}

///
/// @brief
///     设置控件标尺的刻度颜色
///
inline void CPhasorGramCtrl::SetScaleValColor(const COLORREF clr)
{
	m_clScaleValColor = clr;
}

///
/// @brief
///     设置控件中雷达图各半径线条的颜色
///
inline void CPhasorGramCtrl::SetRadiusColor(const COLORREF clr)
{
	m_clRadiusColor = clr;
}

///
/// @brief
///     设置控件中雷达图的外圆线条的颜色
///
inline void CPhasorGramCtrl::SetExCircleColor(const COLORREF clr)
{
	m_clExCircleColor = clr;
}

///
/// @brief
///     设置控件中雷达图的内圆线条的颜色
///
inline void CPhasorGramCtrl::SetInCircleColor(const COLORREF clr)
{
	m_clInCircleColor = clr;
}

///
/// @brief
///     设置控件的边距
///
inline void CPhasorGramCtrl::SetMargin(const int leftMargin, const int rightMargin, const int topMargin, const int bottomMargin)
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
///     设置控件中雷达图的圆圈的数量
///
inline void CPhasorGramCtrl::SetCircleCount(const int count)
{
	m_iCircleCount = count;
}

///
/// @brief
///     设置控件中雷达图的扇区的数量
///
inline void CPhasorGramCtrl::SetHalfSectorCount(const int count)
{
	m_iHalfSectorCount = count;
}

///
/// @brief
///     设置相量图的长度标尺的分度值
///
inline void CPhasorGramCtrl::SetLenScaleInterval(const float interval)
{
	m_fLenScaleInterval = interval;
}

///
/// @brief
///     设置控件标题的可见性
///
inline void CPhasorGramCtrl::SetTitleVisible(BOOL bShow /* = TRUE */)
{
	m_bShowTitle = bShow;
}

///
/// @brief
///     设置相量图的角度标尺的可见性
///
inline void CPhasorGramCtrl::SetDegreeScaleValVisible(BOOL bShow /* = TRUE */)
{
	m_bShowDegreeScaleVal = bShow;
}

///
/// @brief
///     设置相量图的长度标尺的可见性
///
inline void CPhasorGramCtrl::SetLenScaleValVisible(BOOL bShow /* = TRUE */)
{
	m_bShowLenScaleVal = bShow;
}

///
/// @brief
///     设置相量图的相量描述信息的可见性
///
inline void CPhasorGramCtrl::SetPhasorDesc(BOOL bShow /* = TRUE */)
{
	m_bShowPhasorDesc = bShow;
}
