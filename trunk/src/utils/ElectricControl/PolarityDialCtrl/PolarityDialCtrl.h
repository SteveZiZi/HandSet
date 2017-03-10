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
	///     �ؼ���ʽ���ƺ������������֣����ı����Ҫ����Init����Ч
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
	///     �ؼ���ʽ���ƺ�����ǰ�����֣����ı����Ӧ�������ػ�Ż�仯
	///
	void SetHandBmp(CBitmap *pBmp);
	void SetDialInitState(const float percentVal);
	void SetHandColor(const COLORREF leftClr, const COLORREF rightClr);
	void SetDialDegree(const float minDegree, float maxDegree);

	///
	/// @brief
	///     ��ʼ�����Ա��̿ؼ����ޱ���
	///
	void Init();

	///
	/// @brief
	///     ���������ϵ�ָ��
	///
	void EraseHand();

	///
	/// @brief
	///     ���Ʊ���ָ�룺����ʽ����ǰ����Ƶ�ָ�������ʾ��
	///     Ĭ��Ϊ�Ի淽ʽ����������ָ��Bmp����Ϊ��ͼ��ʽ
	///
	/// @param[in]
	///     idxSeq     - ���̵�˳��ţ����������ң��ٴ��ϵ��µ�˳�򣩣���0��ʼ
	/// @param[in]
	///     percentVal - ����ָ��ƫת�İٷֱȣ������Ϊ0
	///
	void DrawHand(const size_t idxSeq,const float percentVal);

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
	void UpdateDialArea(CDC *pDC);
	void DrawDefaultHand(CDC *pDC, const CPoint &axisPt, const float inRadii, const float handRadii, const float degree);
	void DrawBmpHand(CDC *pDC, CBitmap *pBmp, const CPoint &axisPt, const float inRadii, const float handRadii, const float degree);

	void CreateBmp(HBITMAP &hBmp, BYTE **lpBits, const int bmWidth, const int bmHeight, const int bmBitsPixel);
	void RotateBmp(HBITMAP &hDstBmp, const BITMAP &srcBmp, BYTE *srcLpBits, const float degree);

private:
	CDC      m_cBkgndDC;                    ///< ���Ա��̱����Ļ���DC
	CDC      m_cForgndDC;                   ///< ���Ա���ǰ���Ļ���DC
	CBitmap  m_cBkgndBmp;                   ///< ����λͼ
	CBitmap *m_pOldBkgndBmp;                ///< ָ��ɵı���λͼ��ָ��Ļ���
	CBitmap  m_cForgndBmp;                  ///< ǰ��λͼ
	CBitmap *m_pOldForgndBmp;               ///< ָ��ɵ�ǰ��λͼ��ָ��Ļ���

	CBitmap *m_pDialBmp;                    ///< ָ�����λͼ��ָ�룬�������������ⲿ�û�ά��
	BOOL     m_bDialBmpTransparent;         ///< ����λͼ�ı����Ƿ�͸����
	CBitmap *m_pHandBmp;

	COLORREF m_clTransColor;                ///< λͼ͸��������ɫ
	COLORREF m_clBkgndColor;		        ///< �ؼ�������ɫ
	COLORREF m_clHandLeftColor;             ///< ����ָ�����벿����ɫ
	COLORREF m_clHandRightColor;            ///< ����ָ����Ұ벿����ɫ

	float    m_fDialInitState;              ///< ����ָ��ĳ�ʼ״ֵ̬
	int      m_iXGramCount;                 ///< X�᷽���ϵı��̸���
	int      m_iYGramCount;                 ///< Y�᷽���ϵı��̸���

	float    m_fStdDialInRadii;             ///< ��׼���̵��ھ�����
	float    m_fStdHandRadii;               ///< ��׼���̵�ָ��İ뾶���ȣ�ĩ�˵�Բ�ģ�
	float    m_fMinDegree;                  ///< ��׼���̵���С�Ƕȣ�˳ʱ�뷽��ˮƽ�ߵ��������Ե�ĽǶ�
	float    m_fMaxDegree;                  ///< ��׼���̵����Ƕȣ�˳ʱ�뷽��ˮƽ�ߵ������ұ�Ե�ĽǶ�
	CPoint   m_ptStdDialCircleCentre;       ///< ��׼���̵�Բ�����꣨���׼���̾���������Ͻ�����Ϊ��0��0����
	CRect    m_rtStdDialRect;               ///< ��׼���̵ľ�����

	CRect    m_rtDialArea;                  ///< �ؼ��е�1��(0,0)���̵ľ�����
	int      m_iXGramSpace;                 ///< һ��������X�᷽���ϵĿ��
	int      m_iYGramSpace;                 ///< һ��������Y�᷽���ϵĿ��

	DECLARE_MESSAGE_MAP()
};

///
/// @brief
///     ���ÿؼ���������ɫ
///
inline void CPolarityDialCtrl::SetBkgndColor(const COLORREF clr)
{
	m_clBkgndColor = clr;
}

///
/// @brief
///     ���ÿؼ���ͼ��͸������ɫ
///
inline void CPolarityDialCtrl::SetBmpTransColor(const COLORREF clr)
{
	m_clTransColor = clr;
}

///
/// @brief
///     ���ñ���ָ�����ɫ��leftClrΪָ����ɫ��rightClrΪָ���ϵ�Բ�����ɫ��
///
inline void CPolarityDialCtrl::SetHandColor(const COLORREF leftClr, const COLORREF rightClr)
{
	m_clHandLeftColor  = leftClr;
	m_clHandRightColor = rightClr;
}

///
/// @brief
///     ���ñ���ָ��ĳ�ʼ״ֵ̬
///
inline void CPolarityDialCtrl::SetDialInitState(const float percentVal)
{
	m_fDialInitState = percentVal;
}

///
/// @brief
///     ���ñ��̵�����
///
inline void CPolarityDialCtrl::SetDialCount(const int xCount, const int yCount)
{
	m_iXGramCount = xCount;
	m_iYGramCount = yCount;
}

///
/// @brief
///     ���ñ�׼���̵��ھ�
///
inline void CPolarityDialCtrl::SetStdDialInRadii(const float radiiLen)
{
	m_fStdDialInRadii = radiiLen;
}

///
/// @brief
///     ���ñ�׼���̵�ָ��뾶
///
inline void CPolarityDialCtrl::SetStdDialHandRadii(const float radiiLen)
{
	m_fStdHandRadii = radiiLen;
}

///
/// @brief
///     ���ñ�׼���̵�Բ�����꣬���׼���̵ľ���������Ͻ�����Ϊ��0��0��
///
inline void CPolarityDialCtrl::SetStdDialCircleCentre(CPoint pt)
{
	m_ptStdDialCircleCentre = pt;
}

///
/// @brief
///     ���ñ�׼���̵ľ�����
///
inline void CPolarityDialCtrl::SetStdDialRect(CRect rt)
{
	m_rtStdDialRect = rt;
}

///
/// @brief
///     ���ñ�׼���̵�ָ���ƫת�Ƕ�
///
inline void CPolarityDialCtrl::SetDialDegree(const float minDegree, const float maxDegree)
{
	m_fMinDegree = minDegree;
	m_fMaxDegree = maxDegree;
}

#endif
