#ifndef ON_OFF_STATE_CTRL_H_

#define ON_OFF_STATE_CTRL_H_

#include <vector>

class COnOffStateCtrl: public CStatic
{
	DECLARE_DYNAMIC(COnOffStateCtrl);

public:
	COnOffStateCtrl();
	virtual ~COnOffStateCtrl();

	///
	/// @brief
	///     控件样式定制函数（背景部分）：改变后需要重新Init才生效
	///
	void SetBkgndColor(const COLORREF clr);
	void SetMargin(const int leftMargin, const int rightMargin, const int topMargin, const int bottomMargin);
	void SetOnOffDICount(const int xCount, const int yCount);
	///
	/// @brief
	///     控件样式定制函数（前景部分）：改变后相应的项需重绘才会变化
	///
	void SetBorderColor(const COLORREF clr);
	void SetOnOffDescColor(const COLORREF clr);
	void SetOnOffStyleShowSize(int x, int y);
	void SetOnOffStyle(const size_t idxSeq, CBitmap *pBmp);
	BOOL SetOnOffState(const size_t idxSeq, const size_t styleID, const CString &desc);
	void SetOnOffDescFont(CFont *pFont);
	void SetOnOffBoard(BOOL bShow = TRUE);

	///
	/// @brief
	///     初始化控件
	///
	void Init();

	///
	/// @brief
	///     擦除全部开关DI状态
	///
	void EraseOnOffState();

	///
	/// @brief
	///     绘制开关DI状态：叠加式（与前面绘制的叠加显示），重绘需先调用EraseOnOffState()
	///
	void DrawOnOffState();

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
	void UpdateOnOffBmpArea(CDC *pDC);

private:
	CDC      m_cBkgndDC;                    ///< 控件背景的缓存DC
	CDC      m_cForgndDC;                   ///< 控件前景的缓存DC
	CBitmap  m_cBkgndBmp;                   ///< 背景位图
	CBitmap *m_pOldBkgndBmp;                ///< 指向旧的背景位图的指针的缓存
	CBitmap  m_cForgndBmp;                  ///< 前景位图
	CBitmap *m_pOldForgndBmp;               ///< 指向旧的前景位图的指针的缓存
	std::vector<CBitmap *> m_vOnOffStyleBmp;///< 开关DI状态Bmp样式的缓存，其内的Bmp的生命周期由外部用户维护

	CFont    m_cOnOffDescFont;              ///< 开关DI的描述信息的字体

	COLORREF m_clBkgndColor;		        ///< 控件背景颜色
	COLORREF m_clBorderColor;               ///< 开关DI状态Bmp的边框颜色
	COLORREF m_clOnOffDescColor;            ///< 开关DI的描述信息的颜色

	int      m_iXOnOffDICount;              ///< X轴方向上开关DI的数量，即列数
	int      m_iYOnOffDICount;              ///< Y轴方向上开关DI的数量，即行数

	int      m_iLeftMargin;                 ///< 单个开关DI状态的左边距
	int      m_iRightMargin;                ///< 单个开关DI状态的右边距
	int      m_iTopMargin;                  ///< 单个开关DI状态的上边距
	int      m_iBottomMargin;               ///< 单个开关DI状态的下边距

	CRect    m_rtOnOffDIArea;               ///< 放置第一个开关DI状态的矩形域
	float    m_fXOnOffDISpace;              ///< 一个开关DI在X轴方向上的宽度
	float    m_fYOnOffDISpace;              ///< 一个开关DI在Y轴方向上的高度
	CRect    m_rtOnOffShowSize;             ///< 开关DI状态Bmp的显示区域大小

	BOOL     m_bShowOnOffBoard;             ///< 是否显示开关DI状态Bmp的自绘边框

	typedef struct
	{
		CString csDesc;                     ///< 开关状态的描述信息
		size_t  styleID;                    ///< 开关状态的样式ID
	}ONOFF_STATE_INFO;
	std::vector<ONOFF_STATE_INFO> m_vOnOffStateInfo;
};

///
/// @brief
///     设置控件的背景颜色
///
inline void COnOffStateCtrl::SetBkgndColor(const COLORREF clr)
{
	m_clBkgndColor = clr;
}

///
/// @brief
///     设置开关DI状态Bmp的边框的颜色
///
inline void COnOffStateCtrl::SetBorderColor(const COLORREF clr)
{
	m_clBorderColor = clr;
}

///
/// @brief
///     设置开关DI状态的描述信息的字体颜色
///
inline void COnOffStateCtrl::SetOnOffDescColor(const COLORREF clr)
{
	m_clOnOffDescColor = clr;
}

///
/// @brief
///     设置控件中单个开关DI状态的边距
///
inline void COnOffStateCtrl::SetMargin(const int leftMargin, const int rightMargin, const int topMargin, const int bottomMargin)
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
///     设置开关DI状态的图片的显示区域的大小
///
inline void COnOffStateCtrl::SetOnOffStyleShowSize(int x, int y)
{
	m_rtOnOffShowSize = CRect(0,0,x,y);
}

///
/// @brief
///     设置开关DI状态的图片的自绘边框的可见性
///
inline void COnOffStateCtrl::SetOnOffBoard(BOOL bShow /* = TRUE */)
{
	m_bShowOnOffBoard = bShow;
}

#endif
