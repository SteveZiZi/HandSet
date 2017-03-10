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
	///     �ؼ���ʽ���ƺ������������֣����ı����Ҫ����Init����Ч
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
	///     �ؼ���ʽ���ƺ�����ǰ�����֣����ı����Ӧ�������ػ�Ż�仯
	///
	void SetPhasorColor(const size_t idxSeq, const COLORREF clr);
	void SetPhasorDesc(BOOL bShow = TRUE);
	void SetPhasorDescFont(CFont *pFont);
	void SetPhasorExtDescFont(CFont *pFont);

	///
	/// @brief
	///     ��ʼ������ͼ�ؼ�
	///
    void Init();

	///
	/// @brief
	///     ����������ʾ�����ȫ��������Ϣ
	///
	void ErasePhasor();

	///
	/// @brief
	///     ��������������ʽ����ǰ����Ƶ�����������ʾ��
	///
	/// @param[in]
	///     lenBuf    - ������������ֵ��Ϣ�Ļ���
	/// @param[in]
	///     degreeBuf - ���������Ƕ�ֵ��Ϣ�Ļ���
	/// @param[in]
	///     descBuf   - ����������������Ϣ�Ļ���(λ�������ļ�ͷ��)
	/// @param[in]
	///     bufSize   - �����С
	///
	void DrawPhasor(const float *lenBuf, const float *degreeBuf, const CString *descBuf, const size_t bufSize);

	///
	/// @brief
	///     ������������չ������Ϣ������ʽ����ǰ����Ƶ����ݵ�����ʾ��
	///
	/// @param[in]
	///     leftTop   - ������Ϣ�Ļ�����������Ͻ�����
	/// @param[in]
	///     descClr   - ������Ϣ����ɫ
	/// @param[in]
	///     descBuf   - ������������չ������Ϣ�Ļ���
	/// @param[in]
	///     bufSize   - �����С
	///
	void DrawExtDesc(const CPoint leftTop, const COLORREF descClr, const CString *descBuf, const size_t bufSize);

    ///
    /// @brief
    ///     ������������չ������Ϣ������ʽ����ǰ����Ƶ����ݵ�����ʾ��
    ///
    /// @param[in]
    ///     csDesc      ������Ϣ
    /// @param[in]
    ///     rcRect    - ������Ϣ������
    /// @param[in]
    ///     crColor   - ������Ϣ����ɫ
    /// @param[in]
    ///     nFormat   - �����ʽ
    ///
    void DrawExtDesc(CString csDesc, CRect rcRect, const COLORREF crColor, UINT nFormat = DT_VCENTER | DT_LEFT | DT_SINGLELINE);

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
	void UpdatePhasorAreaRange(CDC *pDC);
	void DrawCircle(CDC *pDC);
	void DrawRadius(CDC *pDC);
	void DrawScale(CDC *pDC);
	void DrawTitle(CDC *pDC);
	void DrawPhasorArrows(CDC *pDC,const CPoint &beginPt, const CPoint &endPt, const size_t idxSeq);
	void DrawPhasorDesc(CDC *pDC,const CPoint *endPt, const CString *desc, const size_t bufSize);

	bool IsIntersect(const CRect &rt1,const CRect &rt2);

private:
	CDC      m_cBkgndDC;                    ///< �ؼ������Ļ���DC
	CDC      m_cForgndDC;                   ///< �ؼ�ǰ���Ļ���DC
	CBitmap  m_cBkgndBmp;                   ///< ����λͼ
	CBitmap *m_pOldBkgndBmp;                ///< ָ��ɵı���λͼ��ָ��Ļ���
	CBitmap  m_cForgndBmp;                  ///< ǰ��λͼ
	CBitmap *m_pOldForgndBmp;               ///< ָ��ɵ�ǰ��λͼ��ָ��Ļ���

	CFont    m_cTitleFont;                  ///< �ؼ����������
	CFont    m_cScaleValFont;               ///< �ؼ���ߵ�����
	CFont    m_cPhasorDescFont;             ///< ����������Ϣ�����壨λ��������ͷ����
	CFont    m_cPhasorExtDescFont;          ///< ��������չ������Ϣ�����壨�û��Զ���λ�ã�

	COLORREF m_clBkgndColor;		        ///< �ؼ�������ɫ
	COLORREF m_clTitleColor;                ///< �ؼ��������ɫ
	COLORREF m_clScaleValColor;             ///< �ؼ���ߵ���ɫ
	COLORREF m_clRadiusColor;		        ///< �״�ͼ�İ뾶������ɫ
	COLORREF m_clExCircleColor;		        ///< �״�ͼ����Բ������ɫ
	COLORREF m_clInCircleColor;		        ///< �״�ͼ����Բ������ɫ
	std::vector<COLORREF> m_clPhasorColor;  ///< ��������ɫ

	CString  m_strTitle;
	int      m_iCircleCount;                ///< �״�ͼ�е�Բ������
	int      m_iHalfSectorCount;            ///< �״�ͼ�а��������������
	float    m_fLenScaleInterval;           ///< ��λԲ�뾶����Ŀ̶�ֵ

	int      m_iLeftMargin;                 ///< ����ͼ�ؼ�����߾�
	int      m_iRightMargin;                ///< ����ͼ�ؼ����ұ߾�
	int      m_iTopMargin;                  ///< ����ͼ�ؼ����ϱ߾�
	int      m_iBottomMargin;               ///< ����ͼ�ؼ����±߾�

	BOOL     m_bShowTitle;                  ///< �Ƿ���ʾ����
	BOOL     m_bShowDegreeScaleVal;         ///< �Ƿ���ʾ�Ƕȱ��
	BOOL     m_bShowLenScaleVal;            ///< �Ƿ���ʾ���ȱ��
	BOOL     m_bShowPhasorDesc;             ///< �Ƿ���ʾ��������

	CRect    m_rtPhasorArea;                ///< ������ʾ�����״�ͼ���ľ�����
	
	DECLARE_MESSAGE_MAP()
};

///
/// @brief
///     ���ÿؼ��ı�����ɫ
///
inline void CPhasorGramCtrl::SetBkgndColor(const COLORREF clr)
{
	m_clBkgndColor = clr;
}

///
/// @brief
///     ���ÿؼ��������ɫ
///
inline void CPhasorGramCtrl::SetTitleColor(const COLORREF clr)
{
	m_clTitleColor = clr;
}

///
/// @brief
///     ���ÿؼ���ߵĿ̶���ɫ
///
inline void CPhasorGramCtrl::SetScaleValColor(const COLORREF clr)
{
	m_clScaleValColor = clr;
}

///
/// @brief
///     ���ÿؼ����״�ͼ���뾶��������ɫ
///
inline void CPhasorGramCtrl::SetRadiusColor(const COLORREF clr)
{
	m_clRadiusColor = clr;
}

///
/// @brief
///     ���ÿؼ����״�ͼ����Բ��������ɫ
///
inline void CPhasorGramCtrl::SetExCircleColor(const COLORREF clr)
{
	m_clExCircleColor = clr;
}

///
/// @brief
///     ���ÿؼ����״�ͼ����Բ��������ɫ
///
inline void CPhasorGramCtrl::SetInCircleColor(const COLORREF clr)
{
	m_clInCircleColor = clr;
}

///
/// @brief
///     ���ÿؼ��ı߾�
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
///     ���ÿؼ����״�ͼ��ԲȦ������
///
inline void CPhasorGramCtrl::SetCircleCount(const int count)
{
	m_iCircleCount = count;
}

///
/// @brief
///     ���ÿؼ����״�ͼ������������
///
inline void CPhasorGramCtrl::SetHalfSectorCount(const int count)
{
	m_iHalfSectorCount = count;
}

///
/// @brief
///     ��������ͼ�ĳ��ȱ�ߵķֶ�ֵ
///
inline void CPhasorGramCtrl::SetLenScaleInterval(const float interval)
{
	m_fLenScaleInterval = interval;
}

///
/// @brief
///     ���ÿؼ�����Ŀɼ���
///
inline void CPhasorGramCtrl::SetTitleVisible(BOOL bShow /* = TRUE */)
{
	m_bShowTitle = bShow;
}

///
/// @brief
///     ��������ͼ�ĽǶȱ�ߵĿɼ���
///
inline void CPhasorGramCtrl::SetDegreeScaleValVisible(BOOL bShow /* = TRUE */)
{
	m_bShowDegreeScaleVal = bShow;
}

///
/// @brief
///     ��������ͼ�ĳ��ȱ�ߵĿɼ���
///
inline void CPhasorGramCtrl::SetLenScaleValVisible(BOOL bShow /* = TRUE */)
{
	m_bShowLenScaleVal = bShow;
}

///
/// @brief
///     ��������ͼ������������Ϣ�Ŀɼ���
///
inline void CPhasorGramCtrl::SetPhasorDesc(BOOL bShow /* = TRUE */)
{
	m_bShowPhasorDesc = bShow;
}
