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
	///     �ؼ���ʽ���ƺ������������֣����ı����Ҫ����Init����Ч
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
	///     �ؼ���ʽ���ƺ�����ǰ�����֣����ı����Ӧ�������ػ�Ż�仯
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
	///     ��ʼ������ͼ�ؼ�
	///
	void Init();

	///
	/// @brief
	///     ����������ʾ�����ȫ��������Ϣ
	///
	void ErasePillar();

	///
	/// @brief
	///     ��������������ʽ����ǰ����Ƶ�����������ʾ��
	///     Ĭ��Ϊ����Ϊ�Ի������ʽ����������������ʽ��������Ϊ��ͼ��ʽ
	///
	/// @param[in]
	///     valBuf  - ������������ʾ��ֵ�Ļ���
	/// @param[in]
	///     bufSize - �����С
	/// @param[in]
	///     valUnit - ��������ʾ��ֵ�ĵ�λ
	///
	void DrawHorHistoGram(const float *valBuf,const size_t bufSize, const CString &valUnit);

	///
	/// @brief
	///     ǿ��ˢ�£�����ǰ�ڴ��豸�����е�����ˢ�µ�Ŀ���ڴ��豸������DC����
	///
	/// @param[in]
	///     pDC - ָ��Ŀ��DC��ָ�룻ΪNULLʱ����ΪĬ�ϵ�DC
	///
	void Flush(CDC* pDC);

protected:
	///
	/// @brief
	///     ��Ϣ����
	///
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	///
	/// @brief
	///     ��������
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
	CDC      m_cBkgndDC;                    ///< �ؼ������Ļ���DC
	CDC      m_cForgndDC;                   ///< �ؼ�ǰ���Ļ���DC
	CBitmap  m_cBkgndBmp;                   ///< ����λͼ
	CBitmap *m_pOldBkgndBmp;                ///< ָ��ɵı���λͼ��ָ��Ļ���
	CBitmap  m_cForgndBmp;                  ///< ǰ��λͼ
	CBitmap *m_pOldForgndBmp;               ///< ָ��ɵ�ǰ��λͼ��ָ��Ļ���
	std::vector<CBitmap *> m_vPillarStyleBmp;///< ����Bmp��ʽ�Ļ��棬���ڵ�Bmp�������������ⲿ�û�ά��

	CFont    m_cTitleFont;                  ///< ��������
	CFont    m_cScaleValFont;               ///< X-Y��ߵ�����
	CFont    m_cPillarValFont;              ///< ��������ʾ��ֵ������

	COLORREF m_clTransColor;                ///< λͼ͸��������ɫ
	COLORREF m_clBkgndColor;		        ///< �ؼ�������ɫ
	COLORREF m_clPillarAreaGridColor;       ///< ������ʾ������ڲ������������ɫ
	COLORREF m_clPillarValColor;            ///< ��������ʾ��ֵ����ɫ
	COLORREF m_clTitleColor;                ///< ��״ͼ�������ɫ
	COLORREF m_clScaleValColor;             ///< ��״ͼ��X-Y��ߵĿ̶ȵ���ɫ
	COLORREF m_clPillarAreaBorderColor;      ///< ��״ͼ��������߿����ɫ
	std::vector<COLORREF> m_clPillarColor;  ///< ��������ɫ
	int      m_iPillarBetween;              ///< �����ļ��
	int      m_iPillarHeight;               ///< �����Ĵ�ֱ�߶ȴ�С

	CString  m_strTitle;                    ///< �ؼ�����
	int      m_iPillarCount;                ///< ����������
	int      m_iXGridCount;                 ///< ������ʾ����X��߷�����һ���������
	int      m_iYGridCount;                 ///< ������ʾ����Y��߷�����һ���������
	float    m_fXScaleInterval;             ///< X��ߵķֶ�ֵ����ÿһС�����ֵ��
	std::vector<CString> m_strYScaleVal;    ///< Y����ϵĿ̶�ֵ
	int      m_iYScaleValLen;               ///< Y��߿̶ȵ����س���
	int      m_iSpaceFromLeftBorderToPillar;///< ��߿������ľ��룬��λ������

	int      m_iLeftMargin;                 ///< ��״ͼ�ؼ�����߾�
	int      m_iRightMargin;                ///< ��״ͼ�ؼ����ұ߾�
	int      m_iTopMargin;                  ///< ��״ͼ�ؼ����ϱ߾�
	int      m_iBottomMargin;               ///< ��״ͼ�ؼ����±߾�

	BOOL     m_bShowTitle;                  ///< �Ƿ���ʾ��״ͼ�ı���
	BOOL     m_bShowHorGrid;                ///< �Ƿ���ʾ������ʾ�����ˮƽ������
	BOOL     m_bShowVerGrid;                ///< �Ƿ���ʾ������ʾ����Ĵ�ֱ������
	BOOL     m_bShowBorder[4];              ///< �Ƿ���ʾ������ʾ����ı߿�0����1���ң�2���ϣ�3����
	BOOL     m_bShowXScaleVal;              ///< �Ƿ���ʾX��ߵĿ̶�ֵ
	BOOL     m_bShowYScaleVal;              ///< �Ƿ���ʾY��ߵĿ̶�ֵ
	BOOL     m_bShowPillarVal;              ///< �Ƿ���ʾ��������ʾ��ֵ

	CRect    m_rtPillarArea;                ///< ������ʾ����ľ�����
};

///
/// @brief
///     ���ÿؼ�������ɫ
///
inline void CHorHistogramCtrl::SetBkgndColor(const COLORREF clr)
{
	m_clBkgndColor = clr;
}

///
/// @brief
///     ����������ʾ������ڲ������������ɫ
///
inline void CHorHistogramCtrl::SetPillarAreaGridColor(const COLORREF clr)
{
	m_clPillarAreaGridColor = clr;
}

///
/// @brief
///     ����������ʾ����ı߿��������ɫ
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
///     ���ÿؼ��������ɫ
///
inline void CHorHistogramCtrl::SetTitleColor(const COLORREF clr)
{
	m_clTitleColor = clr;
}

///
/// @brief
///     ����X-Y��ߵĿ̶ȵ���ɫ��X��߳�����������Y��߲��֣�
///
inline void CHorHistogramCtrl::SetScaleValColor(const COLORREF clr)
{
	m_clScaleValColor = clr;
}

///
/// @brief
///     ���ÿؼ���ͼ��͸������ɫ
///
inline void CHorHistogramCtrl::SetBmpTransColor(const COLORREF clr)
{
	m_clTransColor = clr;
}

///
/// @brief
///     ǿ������Y��߿̶ȵ����س���
///
inline void CHorHistogramCtrl::SetYScaleValLen(const int len)
{
	m_iYScaleValLen = len;
}

///
/// @brief
///     ����������ʾ�������߿������ľ��룬��λ������
///
inline void CHorHistogramCtrl::SetSpaceFromLeftBorderToPillar(int space)
{
	m_iSpaceFromLeftBorderToPillar = space;
}

///
/// @brief
///     ���ÿؼ��ı߾���Ϣ
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
///     ���ÿؼ�����
///
inline void CHorHistogramCtrl::SetTitle(const CString &str)
{
	m_strTitle = str;
}

///
/// @brief
///     ���������ı߾�
///
inline void CHorHistogramCtrl::SetPillarBetween(const int spaceBetween)
{
	m_iPillarBetween = spaceBetween;
}

///
/// @brief
///     ���������ĸ߶�
///
inline void CHorHistogramCtrl::SetPillarHeight(const int height)
{
	m_iPillarHeight = height;
}

///
/// @brief
///     ����������
///
inline void CHorHistogramCtrl::SetPillarCount(const int count)
{
	m_iPillarCount = count;
}

///
/// @brief
///     ����������ʾ����Ĵ�ֱ�������ͳ������������ˮƽ������
///
inline void CHorHistogramCtrl::SetPillarAreaGrid(const int xCount, const int yCount)
{
	m_iXGridCount = xCount;
	m_iYGridCount = yCount;
}

///
/// @brief
///     ����X��ߵķֶ�ֵ
///
inline void CHorHistogramCtrl::SetXScaleInterval(const float interval)
{
	m_fXScaleInterval = interval;
}

///
/// @brief
///     ���ÿؼ�����Ŀɼ���
///
inline void CHorHistogramCtrl::SetTitleVisible(BOOL bShow)
{
	m_bShowTitle = bShow;
}

///
/// @brief
///     ���ô�ֱ����Ŀɼ���
///
inline void CHorHistogramCtrl::SetHorGridVisible(BOOL bShow)
{
	m_bShowHorGrid = bShow;
}

///
/// @brief
///     ����ˮƽ����Ŀɼ���
///
inline void CHorHistogramCtrl::SetVerGridVisible(BOOL bShow)
{
	m_bShowVerGrid = bShow;
}

///
/// @brief
///     ���ñ߿�Ŀɼ���
///
/// @param[in]
///     boardIdx   - �߿�������0����1���ң�2���ϣ�3����
/// @param[in]
///     bShow      - �Ƿ�ɼ����ɼ�ΪTRUE
///
inline void CHorHistogramCtrl::SetBorderVisible(int boardIdx, BOOL bShow)
{
	if ( boardIdx >= 4 )
		return;

	m_bShowBorder[boardIdx] = bShow;
}

///
/// @brief
///     ����X��߿̶ȵĿɼ���
///
inline void CHorHistogramCtrl::SetXScaleValVisible(BOOL bShow)
{
	m_bShowXScaleVal = bShow;
}

///
/// @brief
///     ����Y��߿̶ȵĿɼ���
///
inline void CHorHistogramCtrl::SetYScaleValVisible(BOOL bShow)
{
	m_bShowYScaleVal = bShow;
}

///
/// @brief
///     ������������ֵ�Ŀɼ���
///
inline void CHorHistogramCtrl::SetPillarValVisible(BOOL bShow)
{
	m_bShowPillarVal = bShow;
}

#endif
