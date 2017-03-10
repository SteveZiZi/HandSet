#ifndef POLARITY_DIAL_CTRL_H_

#define POLARITY_DIAL_CTRL_H_

#include <vector>

class CPolarityDialCtrl: public CStatic
{
	DECLARE_DYNAMIC(CPolarityDialCtrl);

public:
	CPolarityDialCtrl();
	virtual ~CPolarityDialCtrl();

	///
	/// @brief
	///     控件样式定制函数（背景部分）：改变后需要重新Init才生效
	///
	void SetBkgndColor(const COLORREF clr);
	void SetBmpTransColor(const COLORREF clr);
	void SetDialCount(const int xCount, const int yCount);
	void SetStdDialInRadii(const float radiiLen);
	void SetStdDialHandRadii(const float radiiLen);
	void SetStdDialCircleCentre(CPoint pt);
	void SetStdDialRect(CRect rt);
	void SetBkgndBmp(CBitmap *pBmp, BOOL bTransparent = FALSE);

	///
	/// @brief
	///     控件样式定制函数（前景部分）：改变后相应的项需重绘才会变化
	///
	void SetHandBmp(CBitmap *pBmp);
	void SetDialInitState(const float percentVal);
	void SetHandColor(const COLORREF leftClr, const COLORREF rightClr);
	void SetDialDegree(const float minDegree, float maxDegree);

	///
	/// @brief
	///     初始化极性表盘控件，无表盘
	///
	void Init();

	///
	/// @brief
	///     擦除表盘上的指针
	///
	void EraseHand();

	///
	/// @brief
	///     绘制表盘指针：叠加式（与前面绘制的指针叠加显示）
	///     默认为自绘方式，若设置了指针Bmp，则为贴图方式
	///
	/// @param[in]
	///     idxSeq     - 表盘的顺序号（按从左往右，再从上到下的顺序），从0开始
	/// @param[in]
	///     percentVal - 表盘指针偏转的百分比，最左边为0
	///
	void DrawHand(const size_t idxSeq,const float percentVal);

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
	void UpdateDialArea(CDC *pDC);
	void DrawDefaultHand(CDC *pDC, const CPoint &axisPt, const float inRadii, const float handRadii, const float degree);
	void DrawBmpHand(CDC *pDC, CBitmap *pBmp, const CPoint &axisPt, const float inRadii, const float handRadii, const float degree);

	void CreateBmp(HBITMAP &hBmp, BYTE **lpBits, const int bmWidth, const int bmHeight, const int bmBitsPixel);
	void RotateBmp(HBITMAP &hDstBmp, const BITMAP &srcBmp, BYTE *srcLpBits, const float degree);

private:
	CDC      m_cBkgndDC;                    ///< 极性表盘背景的缓存DC
	CDC      m_cForgndDC;                   ///< 极性表盘前景的缓存DC
	CBitmap  m_cBkgndBmp;                   ///< 背景位图
	CBitmap *m_pOldBkgndBmp;                ///< 指向旧的背景位图的指针的缓存
	CBitmap  m_cForgndBmp;                  ///< 前景位图
	CBitmap *m_pOldForgndBmp;               ///< 指向旧的前景位图的指针的缓存

	CBitmap *m_pDialBmp;                    ///< 指向表盘位图的指针，其生命周期由外部用户维护
	BOOL     m_bDialBmpTransparent;         ///< 表盘位图的背景是否透明化
	CBitmap *m_pHandBmp;

	COLORREF m_clTransColor;                ///< 位图透明化的颜色
	COLORREF m_clBkgndColor;		        ///< 控件背景颜色
	COLORREF m_clHandLeftColor;             ///< 表盘指针的左半部分颜色
	COLORREF m_clHandRightColor;            ///< 表盘指针的右半部分颜色

	float    m_fDialInitState;              ///< 表盘指针的初始状态值
	int      m_iXGramCount;                 ///< X轴方向上的表盘个数
	int      m_iYGramCount;                 ///< Y轴方向上的表盘个数

	float    m_fStdDialInRadii;             ///< 标准表盘的内径长度
	float    m_fStdHandRadii;               ///< 标准表盘的指针的半径长度（末端到圆心）
	float    m_fMinDegree;                  ///< 标准表盘的最小角度，顺时针方向水平线到表盘左边缘的角度
	float    m_fMaxDegree;                  ///< 标准表盘的最大角度，顺时针方向水平线到表盘右边缘的角度
	CPoint   m_ptStdDialCircleCentre;       ///< 标准表盘的圆心坐标（设标准表盘矩形域的左上角坐标为（0，0））
	CRect    m_rtStdDialRect;               ///< 标准表盘的矩形域

	CRect    m_rtDialArea;                  ///< 控件中第1个(0,0)表盘的矩形域
	int      m_iXGramSpace;                 ///< 一个表盘在X轴方向上的宽度
	int      m_iYGramSpace;                 ///< 一个表盘在Y轴方向上的宽度

	DECLARE_MESSAGE_MAP()
};

///
/// @brief
///     设置控件背景的颜色
///
inline void CPolarityDialCtrl::SetBkgndColor(const COLORREF clr)
{
	m_clBkgndColor = clr;
}

///
/// @brief
///     设置控件贴图的透明化颜色
///
inline void CPolarityDialCtrl::SetBmpTransColor(const COLORREF clr)
{
	m_clTransColor = clr;
}

///
/// @brief
///     设置表盘指针的颜色（leftClr为指针颜色，rightClr为指针上的圆点的颜色）
///
inline void CPolarityDialCtrl::SetHandColor(const COLORREF leftClr, const COLORREF rightClr)
{
	m_clHandLeftColor  = leftClr;
	m_clHandRightColor = rightClr;
}

///
/// @brief
///     设置表盘指针的初始状态值
///
inline void CPolarityDialCtrl::SetDialInitState(const float percentVal)
{
	m_fDialInitState = percentVal;
}

///
/// @brief
///     设置表盘的数量
///
inline void CPolarityDialCtrl::SetDialCount(const int xCount, const int yCount)
{
	m_iXGramCount = xCount;
	m_iYGramCount = yCount;
}

///
/// @brief
///     设置标准表盘的内径
///
inline void CPolarityDialCtrl::SetStdDialInRadii(const float radiiLen)
{
	m_fStdDialInRadii = radiiLen;
}

///
/// @brief
///     设置标准表盘的指针半径
///
inline void CPolarityDialCtrl::SetStdDialHandRadii(const float radiiLen)
{
	m_fStdHandRadii = radiiLen;
}

///
/// @brief
///     设置标准表盘的圆心坐标，设标准表盘的矩形域的左上角坐标为（0，0）
///
inline void CPolarityDialCtrl::SetStdDialCircleCentre(CPoint pt)
{
	m_ptStdDialCircleCentre = pt;
}

///
/// @brief
///     设置标准表盘的矩形域
///
inline void CPolarityDialCtrl::SetStdDialRect(CRect rt)
{
	m_rtStdDialRect = rt;
}

///
/// @brief
///     设置标准表盘的指针的偏转角度
///
inline void CPolarityDialCtrl::SetDialDegree(const float minDegree, const float maxDegree)
{
	m_fMinDegree = minDegree;
	m_fMaxDegree = maxDegree;
}

#endif
