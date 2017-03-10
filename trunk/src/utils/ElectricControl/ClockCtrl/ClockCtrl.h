#ifndef CLOCK_CTRL_H_

#define CLOCK_CTRL_H_

class CClockCtrl:public CStatic
{
public:
	CClockCtrl();
	~CClockCtrl();

	///
	/// @brief
	///     控件样式定制函数（背景部分）：改变后需要重新Init才生效
	///
	void SetBkgndColor(const COLORREF clr);
	void SetDialScaleColor(const COLORREF clr);
	void SetDialBmp(CBitmap *pBmp);
	void SetDialRadius(const float radius);
	void SetBmpTransColor(const COLORREF clr);
	
	///
	/// @brief
	///     控件样式定制函数（前景部分）：改变后相应的项需重绘才会变化
	///
	void SetHandColor(const COLORREF clr);
	void SetHandBmp(CBitmap *pHourBmp, CBitmap *pMinBmp, CBitmap *pSecBmp);
	
	///
	/// @brief
	///     初始化时钟控件
	///
	void Init();

	///
	/// @brief
	///     擦除时钟指针
	///
	void EraseTime();

	///
	/// @brief
	///     绘制时间信息
	///
	/// @param[in]
	///     hour - 时
	/// @param[in]
	///     min  - 分
	/// @param[in]
	///     sec  - 秒
	///
	void DrawTime(const int hour,const int min, const int sec);

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
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();

	///
	/// @brief
	///     辅助操作
	///
	void CreateDCandBmp(const CRect &rt, CDC &dc, CBitmap &bmp, CBitmap **pPOldBmp);
	void DrawBkgnd();
	void ResetForgnd();
	void Release();

	void DrawDefaultDial(CDC *pDC);
	void DrawDefaultHourHand(CDC *pDC,const int hour,const int min, const int sec);
	void DrawDefaultMinHand(CDC *pDC,const int min,const int sec);
	void DrawDefaultSecHand(CDC *pDC,const int sec);
	void DrawBmpDail(CDC *pDC, CBitmap *pBmp);
	void DrawBmpHand(CDC *pDC, CBitmap *pBmp, const float degree, const float ratio);

	void CreateBmp(HBITMAP &hBmp, BYTE **lpBits, const int bmWidth, const int bmHeight, const int bmBitsPixel);
	void RotateBmp(CDC *pDC, const BITMAP &srcBmp, BYTE *srcLpBits, const float degree, const float ratio);

private:
	CDC      m_cBkgndDC;                    ///< 控件背景的缓存DC
	CDC      m_cForgndDC;                   ///< 控件前景的缓存DC
	CBitmap  m_cBkgndBmp;                   ///< 背景位图
	CBitmap *m_pOldBkgndBmp;                ///< 指向旧的背景位图的指针的缓存
	CBitmap  m_cForgndBmp;                  ///< 前景位图
	CBitmap *m_pOldForgndBmp;               ///< 指向旧的前景位图的指针的缓存

	CBitmap *m_pDialBmp;                    ///< 时钟表盘Bmp的缓存DC
	CBitmap *m_pHourHandBmp;                ///< 时针Bmp的缓存DC
	CBitmap *m_pMinHandBmp;                 ///< 分针Bmp的缓存DC
	CBitmap *m_pSecHandBmp;                 ///< 秒针Bmp的缓存DC

	COLORREF m_clTransColor;                ///< 位图透明化的颜色
	COLORREF m_clBkgndColor;                ///< 自绘表盘的背景颜色
	COLORREF m_clDialScaleColor;            ///< 自绘表盘的时刻颜色
	COLORREF m_clHandColor;                 ///< 自绘指针的颜色

	float    m_fDialRadius;                 ///< 时钟表盘的半径
};

///
/// @brief
///     设置自绘表盘的背景颜色
///
inline void CClockCtrl::SetBkgndColor(const COLORREF clr)
{
	m_clBkgndColor = clr;
}

///
/// @brief
///     设置自绘表盘的时刻的颜色
///
inline void CClockCtrl::SetDialScaleColor(const COLORREF clr)
{
	m_clDialScaleColor = clr;
}

///
/// @brief
///     设置自绘指针的颜色
///
inline void CClockCtrl::SetHandColor(const COLORREF clr)
{
	m_clHandColor = clr;
}

///
/// @brief
///     设置控件贴图的透明化颜色
///
inline void CClockCtrl::SetBmpTransColor(const COLORREF clr)
{
	m_clTransColor = clr;
}

///
/// @brief
///     绘制时钟表盘的半径
///
inline void CClockCtrl::SetDialRadius(const float radius)
{
	m_fDialRadius = radius;
}

#endif
