#ifndef CLOCK_CTRL_H_

#define CLOCK_CTRL_H_

class CClockCtrl:public CStatic
{
public:
	CClockCtrl();
	~CClockCtrl();

	///
	/// @brief
	///     �ؼ���ʽ���ƺ������������֣����ı����Ҫ����Init����Ч
	///
	void SetBkgndColor(const COLORREF clr);
	void SetDialScaleColor(const COLORREF clr);
	void SetDialBmp(CBitmap *pBmp);
	void SetDialRadius(const float radius);
	void SetBmpTransColor(const COLORREF clr);
	
	///
	/// @brief
	///     �ؼ���ʽ���ƺ�����ǰ�����֣����ı����Ӧ�������ػ�Ż�仯
	///
	void SetHandColor(const COLORREF clr);
	void SetHandBmp(CBitmap *pHourBmp, CBitmap *pMinBmp, CBitmap *pSecBmp);
	
	///
	/// @brief
	///     ��ʼ��ʱ�ӿؼ�
	///
	void Init();

	///
	/// @brief
	///     ����ʱ��ָ��
	///
	void EraseTime();

	///
	/// @brief
	///     ����ʱ����Ϣ
	///
	/// @param[in]
	///     hour - ʱ
	/// @param[in]
	///     min  - ��
	/// @param[in]
	///     sec  - ��
	///
	void DrawTime(const int hour,const int min, const int sec);

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
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();

	///
	/// @brief
	///     ��������
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
	CDC      m_cBkgndDC;                    ///< �ؼ������Ļ���DC
	CDC      m_cForgndDC;                   ///< �ؼ�ǰ���Ļ���DC
	CBitmap  m_cBkgndBmp;                   ///< ����λͼ
	CBitmap *m_pOldBkgndBmp;                ///< ָ��ɵı���λͼ��ָ��Ļ���
	CBitmap  m_cForgndBmp;                  ///< ǰ��λͼ
	CBitmap *m_pOldForgndBmp;               ///< ָ��ɵ�ǰ��λͼ��ָ��Ļ���

	CBitmap *m_pDialBmp;                    ///< ʱ�ӱ���Bmp�Ļ���DC
	CBitmap *m_pHourHandBmp;                ///< ʱ��Bmp�Ļ���DC
	CBitmap *m_pMinHandBmp;                 ///< ����Bmp�Ļ���DC
	CBitmap *m_pSecHandBmp;                 ///< ����Bmp�Ļ���DC

	COLORREF m_clTransColor;                ///< λͼ͸��������ɫ
	COLORREF m_clBkgndColor;                ///< �Ի���̵ı�����ɫ
	COLORREF m_clDialScaleColor;            ///< �Ի���̵�ʱ����ɫ
	COLORREF m_clHandColor;                 ///< �Ի�ָ�����ɫ

	float    m_fDialRadius;                 ///< ʱ�ӱ��̵İ뾶
};

///
/// @brief
///     �����Ի���̵ı�����ɫ
///
inline void CClockCtrl::SetBkgndColor(const COLORREF clr)
{
	m_clBkgndColor = clr;
}

///
/// @brief
///     �����Ի���̵�ʱ�̵���ɫ
///
inline void CClockCtrl::SetDialScaleColor(const COLORREF clr)
{
	m_clDialScaleColor = clr;
}

///
/// @brief
///     �����Ի�ָ�����ɫ
///
inline void CClockCtrl::SetHandColor(const COLORREF clr)
{
	m_clHandColor = clr;
}

///
/// @brief
///     ���ÿؼ���ͼ��͸������ɫ
///
inline void CClockCtrl::SetBmpTransColor(const COLORREF clr)
{
	m_clTransColor = clr;
}

///
/// @brief
///     ����ʱ�ӱ��̵İ뾶
///
inline void CClockCtrl::SetDialRadius(const float radius)
{
	m_fDialRadius = radius;
}

#endif
