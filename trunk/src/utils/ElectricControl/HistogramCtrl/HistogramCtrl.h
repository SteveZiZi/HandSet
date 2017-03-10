#pragma once

#include <vector>

#define HTG_FEATURE_PILLAR_COUNT 5

///
/// @brief
///     ��д��������Ϣ
///
struct FEATURE_PILLAR_ITEM
{
	CRect rect;		///< �������������ֵľ�������Ĵ�С
	int idx;		///< ��Ҫ��д����������״ͼ�е�˳��ţ��������ң���0��ʼ
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
	///     �ؼ���ʽ���ƺ������������֣����ı����Ҫ����Init����Ч
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
	///     �ؼ���ʽ���ƺ�����ǰ�����֣����ı����Ӧ�������ػ�Ż�仯
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
	///     ��ȡ����
	///
	int  GetCursorPos();
	int  GetSpecialPillarCount();
	int  GetCommonPillarCount();

	///
	/// @brief
	///     ��ʼ����״ͼ�ؼ�
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
	///
	/// @param[in]
	///     idxSeq        - ��״ͼ��˳��ţ����������ң��ٴ��ϵ��µ�˳�򣩣���0��ʼ
	/// @param[in]
	///     cmnPillarData - ���泣������������ֵ�Ļ���
	/// @param[in]
	///     spcPillarData - �������⣨�Զ��壩����������ֵ�Ļ���
	/// @param[in]
	///     curPillarDesc - ��ǰ�α����ڵ�������������Ϣ
	///
	void DrawHistogram(const size_t idxSeq, const float *cmnPillarData,const float *spcPillarData,const CString &curPillarDesc);

	///
	/// @brief
	///     ǿ��ˢ�£�����ǰ�ڴ��豸�����е�����ˢ�µ�Ŀ���ڴ��豸������DC����
	///
	/// @param[in]
	///     pDC - ָ��Ŀ��DC��ָ�룻ΪNULLʱ����ΪĬ�ϵ�DC
	///
	void Flush(CDC *pDC);

	///
	/// @brief
	///     �����α�
	///
	void CursorMoveRight();

	///
	/// @brief
	///     �����α�
	///
	void CursorMoveLeft();

protected:
	///
	/// @brief
	///     ��Ϣ����
	///
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	///
	/// @brief
	///     ��������
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
	CDC      m_cBkgndDC;                    ///< �ؼ������Ļ���DC
	CDC      m_cForgndDC;                   ///< �ؼ�ǰ���Ļ���DC
	CBitmap  m_cBkgndBmp;                   ///< ����λͼ
	CBitmap *m_pOldBkgndBmp;                ///< ָ��ɵı���λͼ��ָ��Ļ���
	CBitmap  m_cForgndBmp;                  ///< ǰ��λͼ
	CBitmap *m_pOldForgndBmp;               ///< ָ��ɵ�ǰ��λͼ��ָ��Ļ���

	CFont    m_cTitleFont;                  ///< ��������
	CFont    m_cScaleValFont;               ///< X-Y��ߵ�����
	CFont    m_cPillarDescFont;             ///< ����������Ϣ������
	CFont    m_cFeatureFont;                ///< ��������д��Ϣ������

	FEATURE_PILLAR_ITEM m_FeaturePillar[HTG_FEATURE_PILLAR_COUNT];
	                                        ///< ���ڱ���5����д��������Ϣ��Ĭ����0��3��5��7��9��
									        ///< �����غ�ʱ������

	COLORREF m_clBkgndColor;                ///< ��״ͼ�ؼ��ı�����ɫ
	COLORREF m_clPillarAreaGridColor;       ///< ������ʾ������ڲ������������ɫ
	COLORREF m_clFeatureInfoColor;          ///< ��������д��Ϣ����ɫ
	COLORREF m_clTitleColor;                ///< ��״ͼ�������ɫ
	COLORREF m_clScaleValColor;             ///< ��״ͼ��X-Y��ߵĿ̶ȵ���ɫ
	COLORREF m_clSpcScaleValColor;          ///< X��������������Ŀ̶ȵ���ɫ
	std::vector<COLORREF> m_clPillarColor;  ///< ��������ɫ
	std::vector<COLORREF> m_clCursorColor;	///< �α����ɫ

	std::vector<CString> m_strTitle;        ///< ����״ͼ����
	int      m_iCommonPillarCount;          ///< ��������������
	int      m_iCursorPos;                  ///< ��ǰ�α�����λ��
	int      m_iXGridCount;                 ///< ������ʾ����X��߷����ϵ�������
	int      m_iYGridCount;                 ///< ������ʾ����Y��߷����ϵ�������
	int      m_iXGramCount;                 ///< X��߷����ϵ���״ͼ����
	int      m_iYGramCount;                 ///< Y��߷����ϵ���״ͼ����
	std::vector<CString> m_strSpcScaleVal;  ///< ����������X����ϵĿ̶�ֵ
	std::vector<float> m_fXScaleInterval;   ///< ����״ͼͼ��X��ߵķֶ�ֵ����ÿһС�����ֵ��
	std::vector<float> m_fYScaleInterval;   ///< ����״ͼ��Y��ߵķֶ�ֵ
	std::vector<CString> m_strYUnit;        ///< Y��ߵĵ�λ

	int      m_iLeftMargin;                 ///< ��״ͼ�ؼ�����߾�
	int      m_iRightMargin;                ///< ��״ͼ�ؼ����ұ߾�
	int      m_iTopMargin;                  ///< ��״ͼ�ؼ����ϱ߾�
	int      m_iBottomMargin;               ///< ��״ͼ�ؼ����±߾�

	BOOL     m_bShowTitle;                  ///< �Ƿ���ʾ��״ͼ�ı���
	BOOL     m_bShowPillarAreaHorGrid;      ///< �Ƿ���ʾ������ʾ������ڲ������ˮƽ����
	BOOL     m_bShowPillarAreaVerGrid;      ///< �Ƿ���ʾ������ʾ������ڲ�����Ĵ�ֱ����
	BOOL     m_bShowXScaleVal;              ///< �Ƿ���ʾX��ߵĿ̶�ֵ
	BOOL     m_bShowYScaleVal;              ///< �Ƿ���ʾY��ߵĿ̶�ֵ
	BOOL     m_bShowCursor;                 ///< �Ƿ���ʾ�α�

	CRect    m_rtPillarArea;                ///< ��1��(0,0)������ʾ����ľ�����
	int      m_iXGramSpace;                 ///< һ����״ͼ��X��߷����ϵĿ��
	int      m_iYGramSpace;                 ///< һ����״ͼ��Y��߷����ϵĿ��
};

///
/// @brief
///     ���ÿؼ��ı�����ɫ
///
inline void CHistogramCtrl::SetBkgndColor(const COLORREF clr)
{
	m_clBkgndColor = clr;
}

///
/// @brief
///     ����������ʾ������ڲ������������ɫ
///
inline void CHistogramCtrl::SetPillarAreaGridColor(const COLORREF clr)
{
	m_clPillarAreaGridColor = clr;
}

///
/// @brief
///     ������д��������д��Ϣ����ɫ
///
inline void CHistogramCtrl::SetFeatureInfoColor(const COLORREF clr)
{
	m_clFeatureInfoColor = clr;
}

///
/// @brief
///     ���ÿؼ��������ɫ
///
inline void CHistogramCtrl::SetTitleColor(const COLORREF clr)
{
	m_clTitleColor = clr;
}

///
/// @brief
///     ����X-Y��ߵĿ̶ȵ���ɫ��X��߳�����������Y��߲��֣�
///
inline void CHistogramCtrl::SetScaleValColor(const COLORREF clr)
{
	m_clScaleValColor = clr;
}

///
/// @brief
///     ����X��������������Ŀ̶ȵ���ɫ
///
inline void CHistogramCtrl::SetSpcXScaleValColor(const COLORREF clr)
{
	m_clSpcScaleValColor = clr;
}

///
/// @brief
///     ���ÿؼ��ı߾���Ϣ
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
///     ���1����������
///
/// @param[in]
///     xScaleStr - ����������X����ϵĿ̶�ֵ
///
inline void CHistogramCtrl::AddASpecialPillar(const CString &xScaleStr)
{
	m_strSpcScaleVal.push_back(xScaleStr);
}

///
/// @brief
///     ����������ʾ����Ĵ�ֱ�������ͳ������������ˮƽ������
///
inline void CHistogramCtrl::SetPillarAreaGird(const int xGridCount, const int yGridCount)
{
	m_iXGridCount = xGridCount;
	m_iYGridCount = yGridCount;
}

///
/// @brief
///     ���ó������������������
///
inline void CHistogramCtrl::SetCommonPillarCount(const int count)
{
	m_iCommonPillarCount = count;
}

///
/// @brief
///     ������д������
///
/// @param[in]
///     itemIdx   - ��д���˳��ţ���0��ʼ
/// @param[in]
///     pillarIdx - ����д����ָ���������˳��ţ�����+���⣩����0��ʼ����������
///
inline void CHistogramCtrl::SetFeaturePillar(const int itemIdx, const int pillarIdx)
{
	if ( itemIdx > 5 )
		return;
	m_FeaturePillar[itemIdx].idx = pillarIdx;
}

///
/// @brief
///     ���ÿؼ�����Ŀɼ���
///
inline void CHistogramCtrl::SetTitleVisible(BOOL bShow /* = TRUE */)
{
	m_bShowTitle = bShow;
}

///
/// @brief
///     ����ˮƽ����Ŀɼ���
///
inline void CHistogramCtrl::SetHorGridVisible(const BOOL bShow /* = TRUE */)
{
	m_bShowPillarAreaHorGrid = bShow;
}

///
/// @brief
///     ���ô�ֱ����Ŀɼ���
///
inline void CHistogramCtrl::SetVerGridVisible(const BOOL bShow /* = TRUE */)
{
	m_bShowPillarAreaVerGrid = bShow;
}

///
/// @brief
///     ����X��߿̶ȵĿɼ���
///
inline void CHistogramCtrl::SetXScaleValVisible(BOOL bShow /* = TRUE */)
{
	m_bShowXScaleVal = bShow;
}

///
/// @brief
///     ����Y��߿̶ȵĿɼ���
///
inline void CHistogramCtrl::SetYScaleValVisible(BOOL bShow /* = TRUE */)
{
	m_bShowYScaleVal = bShow;
}

///
/// @brief
///     �����α�Ŀɼ���
///
inline void CHistogramCtrl::SetCursorVisible(BOOL bShow /* = TRUE */)
{
	m_bShowCursor = bShow;
}

///
/// @brief
///     ��ȡ�α�ĵ�ǰλ�ã���λ�ڵ�N����������0��ʼ����������
///
inline int  CHistogramCtrl::GetCursorPos()
{
	return m_iCursorPos;
}

///
/// @brief
///     ��ȡ��������������
///
inline int  CHistogramCtrl::GetSpecialPillarCount()
{
	return (int)m_strSpcScaleVal.size();
}

///
/// @brief
///     ��ȡ��������������
///
inline int  CHistogramCtrl::GetCommonPillarCount()
{
	return m_iCommonPillarCount;
}

///
/// @brief
///     �����α�
///
inline void CHistogramCtrl::CursorMoveRight()
{
	m_iCursorPos = int((m_iCursorPos+1) % (m_strSpcScaleVal.size()+m_iCommonPillarCount));
}

///
/// @brief
///     �����α�
///
inline void CHistogramCtrl::CursorMoveLeft()
{
	m_iCursorPos = int((m_iCursorPos+m_iCommonPillarCount+m_strSpcScaleVal.size()-1) % (m_strSpcScaleVal.size()+m_iCommonPillarCount));
}
