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
	///     �ؼ���ʽ���ƺ������������֣����ı����Ҫ����Init����Ч
	///
	void SetBkgndColor(const COLORREF clr);
	void SetMargin(const int leftMargin, const int rightMargin, const int topMargin, const int bottomMargin);
	void SetOnOffDICount(const int xCount, const int yCount);
	///
	/// @brief
	///     �ؼ���ʽ���ƺ�����ǰ�����֣����ı����Ӧ�������ػ�Ż�仯
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
	///     ��ʼ���ؼ�
	///
	void Init();

	///
	/// @brief
	///     ����ȫ������DI״̬
	///
	void EraseOnOffState();

	///
	/// @brief
	///     ���ƿ���DI״̬������ʽ����ǰ����Ƶĵ�����ʾ�����ػ����ȵ���EraseOnOffState()
	///
	void DrawOnOffState();

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
	void UpdateOnOffBmpArea(CDC *pDC);

private:
	CDC      m_cBkgndDC;                    ///< �ؼ������Ļ���DC
	CDC      m_cForgndDC;                   ///< �ؼ�ǰ���Ļ���DC
	CBitmap  m_cBkgndBmp;                   ///< ����λͼ
	CBitmap *m_pOldBkgndBmp;                ///< ָ��ɵı���λͼ��ָ��Ļ���
	CBitmap  m_cForgndBmp;                  ///< ǰ��λͼ
	CBitmap *m_pOldForgndBmp;               ///< ָ��ɵ�ǰ��λͼ��ָ��Ļ���
	std::vector<CBitmap *> m_vOnOffStyleBmp;///< ����DI״̬Bmp��ʽ�Ļ��棬���ڵ�Bmp�������������ⲿ�û�ά��

	CFont    m_cOnOffDescFont;              ///< ����DI��������Ϣ������

	COLORREF m_clBkgndColor;		        ///< �ؼ�������ɫ
	COLORREF m_clBorderColor;               ///< ����DI״̬Bmp�ı߿���ɫ
	COLORREF m_clOnOffDescColor;            ///< ����DI��������Ϣ����ɫ

	int      m_iXOnOffDICount;              ///< X�᷽���Ͽ���DI��������������
	int      m_iYOnOffDICount;              ///< Y�᷽���Ͽ���DI��������������

	int      m_iLeftMargin;                 ///< ��������DI״̬����߾�
	int      m_iRightMargin;                ///< ��������DI״̬���ұ߾�
	int      m_iTopMargin;                  ///< ��������DI״̬���ϱ߾�
	int      m_iBottomMargin;               ///< ��������DI״̬���±߾�

	CRect    m_rtOnOffDIArea;               ///< ���õ�һ������DI״̬�ľ�����
	float    m_fXOnOffDISpace;              ///< һ������DI��X�᷽���ϵĿ��
	float    m_fYOnOffDISpace;              ///< һ������DI��Y�᷽���ϵĸ߶�
	CRect    m_rtOnOffShowSize;             ///< ����DI״̬Bmp����ʾ�����С

	BOOL     m_bShowOnOffBoard;             ///< �Ƿ���ʾ����DI״̬Bmp���Ի�߿�

	typedef struct
	{
		CString csDesc;                     ///< ����״̬��������Ϣ
		size_t  styleID;                    ///< ����״̬����ʽID
	}ONOFF_STATE_INFO;
	std::vector<ONOFF_STATE_INFO> m_vOnOffStateInfo;
};

///
/// @brief
///     ���ÿؼ��ı�����ɫ
///
inline void COnOffStateCtrl::SetBkgndColor(const COLORREF clr)
{
	m_clBkgndColor = clr;
}

///
/// @brief
///     ���ÿ���DI״̬Bmp�ı߿����ɫ
///
inline void COnOffStateCtrl::SetBorderColor(const COLORREF clr)
{
	m_clBorderColor = clr;
}

///
/// @brief
///     ���ÿ���DI״̬��������Ϣ��������ɫ
///
inline void COnOffStateCtrl::SetOnOffDescColor(const COLORREF clr)
{
	m_clOnOffDescColor = clr;
}

///
/// @brief
///     ���ÿؼ��е�������DI״̬�ı߾�
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
///     ���ÿ���DI״̬��ͼƬ����ʾ����Ĵ�С
///
inline void COnOffStateCtrl::SetOnOffStyleShowSize(int x, int y)
{
	m_rtOnOffShowSize = CRect(0,0,x,y);
}

///
/// @brief
///     ���ÿ���DI״̬��ͼƬ���Ի�߿�Ŀɼ���
///
inline void COnOffStateCtrl::SetOnOffBoard(BOOL bShow /* = TRUE */)
{
	m_bShowOnOffBoard = bShow;
}

#endif
