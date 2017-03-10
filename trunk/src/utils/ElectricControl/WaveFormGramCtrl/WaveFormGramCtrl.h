#pragma once

#include <vector>

class CWaveFormGramCtrl : public CStatic
{
	DECLARE_DYNAMIC(CWaveFormGramCtrl)

public:
	CWaveFormGramCtrl();
	virtual ~CWaveFormGramCtrl();

	///
	/// @brief
	///     �ؼ���ʽ���ƺ������������֣����ı����Ҫ����Init����Ч
	///
	void SetBkgndColor(const COLORREF clr);
	void SetTitleColor(const COLORREF clr);
	void SetWaveAreaGridColor(const COLORREF clr);
	void SetScaleValColor(const COLORREF clr);
	void SetCurSorColor(const COLORREF cursor1Clr, const COLORREF cursor2Clr);

	void SetMargin(const int leftMargin, const int rightMargin, const int topMargin, const int bottomMargin);
	void SetTitle(const size_t idxSeq, const CString &str);
	void SetWaveAreaGrid(const int xHalfGridCount, const int yHalfGridCount);
	void SetYIntervalMaxCharNum(const int num);
	void SetScaleInterval(const size_t idxSeq, const float xInterval, const float yInterval, const CString &yUint);
	void SetX0ScaleVal(size_t idxSeq, float x0Val);
	void SetXOneGridPointCount(const size_t idxSeq, const size_t xPtCount);
	void SetGramCount(const int xGramCount, const int yGramCount);
	void SetCursorScale(const CString &cursor1Scale, const CString &cursor2Scale);
	void SetDrawWaveDataBufSize(size_t szSize);
	
	void SetTitleVisible(BOOL bShow = TRUE);
	void SetHorGridVisible(BOOL bShow = TRUE, BOOL bCentreShow = TRUE);
	void SetVerGridVisible(BOOL bShow = TRUE);
	void SetXScaleValVisible(BOOL bShow = TRUE);
	void SetYScaleValVisible(BOOL bShow = TRUE);
	void SetCursor1Visible(BOOL bShowCursor = TRUE, BOOL bShowDesc = TRUE, BOOL bShowScale = TRUE);
	void SetCursor2Visible(BOOL bShowCursor = TRUE, BOOL bShowDesc = TRUE, BOOL bShowScale = TRUE);
	
	void SetTitleFont(CFont *pFont);
	void SetScaleValFont(CFont *pFont);
	void SetCursorDescFont(CFont *pFont);

	///
	/// @brief
	///     �ؼ���ʽ���ƺ�����ǰ�����֣����ı����Ӧ�������ػ�Ż�仯
	///
	void SetWaveColor(const size_t gramIdx, const size_t waveIdx, const COLORREF clr);
	void SetWaveDescColor(const size_t gramIdx, const size_t waveIdx, const COLORREF clr);
	void SetReduceRatio(const float ratio);
	void SetWaveDescVisible(const BOOL bShow = TRUE);
	void SetWaveDescFont(CFont *pFont);
	void SetWaveDrawCount(size_t waveDrawCount);

	///
	/// @brief
	///     ��ʼ������ͼ�ؼ�
	///
	void Init();

	///
	/// @brief
	///     ����������ʾ�����ȫ��������Ϣ
	///
	void EraseWave();

	///
	/// @brief
	///     ���ô����Ʋ��εĲ��������ݣ���DrawWaveFromBuf()���ʹ��
	///
	/// @param[in]
	///     idxSeq    - ����ͼ��˳��ţ��������ң��ٴ��ϵ��£�����0��ʼ
	/// @param[in]
	///     dataBuf   - ���沨�����ݵĻ���
	/// @param[in]
	///     bufSize   - �����С
	///
	/// @return
	///     
	///
	void SetWaveData(const size_t idxSeq, const float *dataBuf, const size_t bufSize);

	///
	/// @brief
	///     ���ݻ���Ĳ������ݻ��Ʋ��Σ���SetWaveData()���ʹ��
	///
	/// @param[in]
	///     idxSeq    - ����ͼ��˳��ţ��������ң��ٴ��ϵ��£�����0��ʼ
	/// @param[in]
	///     waveDesc  - ���ε�������Ϣ��Ĭ��Ϊ2�У��á�\r\n���ֿ�
	///
	/// @return
	///     
	///
	void DrawWaveFromBuf(const size_t idxSeq, const CString &waveDesc);

	///
	/// @brief
	///     ���Ʋ��Σ�����ʽ����ǰ����ƵĲ��ε�����ʾ��
	///
	/// @param[in]
	///     idxSeq    - ����ͼ��˳��ţ��������ң��ٴ��ϵ��£�����0��ʼ
	/// @param[in]
	///     dataBuf   - ���沨�����ݵĻ���
	/// @param[in]
	///     bufSize   - �����С
	/// @param[in]
	///     waveDesc  - ���ε�������Ϣ��Ĭ��Ϊ2�У��á�\r\n���ֿ�
	///
	void DrawWave(const size_t idxSeq, const float *dataBuf, const size_t bufSize, const CString &waveDesc);

	///
	/// @brief
	///     �����α��������Ϣ���Σ�����ʽ����ǰ����ƵĲ��ֵ�����ʾ��
	///
	/// @param[in]
	///     cursor1Desc   - �α�1��������Ϣ
	/// @param[in]
	///     cursor2Desc   - �α�2��������Ϣ
	///
	void DrawCursorDesc(const CString &cursor1Desc, const CString &cursor2Desc);
	
	///
	/// @brief
	///     ǿ��ˢ�£�����ǰ�ڴ��豸�����е�����ˢ�µ�Ŀ���ڴ��豸������DC����
	///
	/// @param[in]
	///     pDC - ָ��Ŀ��DC��ָ�룻ΪNULLʱ����ΪĬ�ϵ�DC
	///
	void Flush(CDC* pDC);

	///
	/// @brief
	///     ��ȡ�α�1��2���ڵĵ�����ڲ������ݻ��������λ��
	///
	int  GetCursor1Pos(size_t idxSeq);
	int  GetCursor2Pos(size_t idxSeq);

	///
	/// @brief
	///     �����α�
	///
	void Cursor2MoveRight();

	///
	/// @brief
	///     �����α�
	///
	void Cursor2MoveLeft();

protected:
	///
	/// @brief
	///     ��Ϣ����
	///
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnDestory();

	///
	/// @brief
	///     ��������
	///
	void CreateDCandBmp(const CRect &rt, CDC &dc, CBitmap &bmp, CBitmap **pPOldBmp);
	void DrawBkgnd();
	void ResetForgnd();
	void Release();
	void UpdateWaveAreaSpace(CDC *pDC);
	void DrawWaveAreaGrid(CDC *pDC, const CRect &waveArea);
	void DrawWaveAreaBoard(CDC *pDC, const CRect &waveArea);
	void DrawScale(CDC *pDC, const size_t idxSeq, const CRect &waveArea);
	void DrawTitle(CDC *pDC, const size_t idxSeq, const CRect &waveArea);
	void DrawCursor1(CDC *pDC, const size_t idxSeq, const CRect &waveArea);
	void DrawCursor2(CDC *pDC, const size_t idxSeq, const CRect &waveArea);

private:
	CDC      m_cBkgndDC;                    ///< �ؼ������Ļ���DC
	CDC      m_cForgndDC;                   ///< �ؼ�ǰ���Ļ���DC
	CBitmap  m_cBkgndBmp;                   ///< ����λͼ
	CBitmap *m_pOldBkgndBmp;                ///< ָ��ɵı���λͼ��ָ��Ļ���
	CBitmap  m_cForgndBmp;                  ///< ǰ��λͼ
	CBitmap *m_pOldForgndBmp;               ///< ָ��ɵ�ǰ��λͼ��ָ��Ļ���

	CFont    m_cTitleFont;                  ///< ��������
	CFont    m_cScaleValFont;               ///< X-Y��ߵ�����
	CFont    m_cWaveDescFont;               ///< ����������Ϣ������
	CFont    m_cCursorDescFont;

	COLORREF m_clBkgndColor;                ///< ����ͼ�ؼ��ı�����ɫ
	COLORREF m_clWaveAreaGridColor;         ///< �źŲ���ʾ������ڲ��������ɫ
	COLORREF m_clTitleColor;                ///< ����ͼ�������ɫ
	COLORREF m_clScaleValColor;             ///< ����ͼ��X-Y��ߵĿ̶ȵ���ɫ
	std::vector<std::vector<COLORREF> *> m_clWaveColor;    ///< �źŲ�����ɫ
	std::vector<std::vector<COLORREF> *> m_clWaveDescColor;///< �źŲ���������Ϣ����ɫ
	COLORREF m_clCursor1Color;              ///< �α�1����ɫ
	COLORREF m_clCursor2Color;              ///< �α�2����ɫ

	CString  m_strCursor1Scale;             ///< �α�1�ı������
	CString  m_strCursor2Scale;             ///< �α�2�ı������

	std::vector<size_t> m_szWaveDataMaxSize;///< ������ͼ�в��ε�����������
	std::vector<size_t> m_szCursor1Pos;     ///< ������ͼ���α�1���ڵĵ�����ڲ������ݻ��������λ��
	std::vector<size_t> m_szCursor2Pos;     ///< ������ͼ���α�2���ڵĵ�����ڲ������ݻ��������λ��
	size_t   m_szWaveDrawCount;             ///< ��ɻ��ƵĲ��εĵ��������������ͼ��ǰ��ʾ�Ĳ��εĲ�������

	std::vector<CString> m_strTitle;        ///< ������ͼ����
	int      m_iXHalfGridCount;             ///< �źŲ���ʾ����X��߷�����һ���������
	int      m_iYHalfGridCount;             ///< �źŲ���ʾ����Y��߷�����һ���������
	int      m_iXGramCount;                 ///< X��߷����ϵĲ���ͼ����
	int      m_iYGramCount;                 ///< Y��߷����ϵĲ���ͼ����
	std::vector<float> m_fXScaleInterval;   ///< ������ͼ��X��ߵķֶ�ֵ����ÿһС���Ӧ������ֵ��
	std::vector<float> m_fYScaleInterval;   ///< ������ͼ��Y��ߵķֶ�ֵ����ÿһС���Ӧ������ֵ��
	std::vector<CString> m_strYUnit;        ///< Y��ߵĵ�λ
	int      m_iYInterValCharNum;           ///< Y��߷ֶ�ֵ������ַ���
	std::vector<size_t> m_szXOneGridPointCount;///< ������ͼ��X��߷����ÿһ����ĵ���

	int      m_iLeftMargin;                 ///< ����ͼ�ؼ�����߾�
	int      m_iRightMargin;                ///< ����ͼ�ؼ����ұ߾�
	int      m_iTopMargin;                  ///< ����ͼ�ؼ����ϱ߾�
	int      m_iBottomMargin;               ///< ����ͼ�ؼ����±߾�

	BOOL     m_bShowTitle;                  ///< �Ƿ���ʾ����ͼ�ı���
	BOOL     m_bShowWaveDesc;               ///< �Ƿ���ʾ�źŲ���������Ϣ
	BOOL     m_bShowWaveAreaHorGrid;        ///< �Ƿ���ʾ������ʾ������ڲ������ˮƽ����
	BOOL     m_bShowWaveAreaHorCentreGrid;  ///< �Ƿ���ʾ������ʾ������ڲ������ˮƽ������м���
	BOOL     m_bShowWaveAreaVerGrid;        ///< �Ƿ���ʾ������ʾ������ڲ�����Ĵ�ֱ����
	BOOL     m_bShowXScaleVal;              ///< �Ƿ���ʾX��ߵĿ̶�ֵ
	BOOL     m_bShowYScaleVal;              ///< �Ƿ���ʾY��ߵĿ̶�ֵ
	BOOL     m_bShowCursor1;                ///< �Ƿ���ʾ�α�1
	BOOL     m_bShowCursor2;                ///< �Ƿ���ʾ�α�2
	BOOL     m_bShowCursor1Desc;            ///< �Ƿ���ʾ�α�1��������Ϣ
	BOOL     m_bShowCursor2Desc;            ///< �Ƿ���ʾ�α�2��������Ϣ
	BOOL     m_bShowCursor1Scale;           ///< �Ƿ���ʾ�α�1�ı��
	BOOL     m_bShowCursor2Scale;           ///< �Ƿ���ʾ�α�2�ı��

	CRect    m_rtWaveArea;                  ///< ��1��(0,0)������ʾ����ľ�����
	int      m_iXGramSpace;                 ///< һ������ͼ��X��߷����ϵĿ��
	int      m_iYGramSpace;                 ///< һ������ͼ��Y��߷����ϵĿ��
	std::vector<size_t> m_szCurWaveCount;   ///< ������ͼ�ĵ�ǰ���źŲ���

	POINT  **m_pDrawWaveDataBuf;            ///< ָ������Ʋ��ε����ݻ�������ָ��
	size_t m_szDrawWaveDataBufSize;         ///< �����Ʋ��ε����ݻ������Ĵ�С
	std::vector<size_t> m_vDrawWaveDataNum; ///< �����Ʋ��β������ݵ�ʵ�ʴ�С
	std::vector<float> m_fX0ScaleVal;       ///< X��ߵ���ʼ��X0�Ŀ̶�ֵ
	
	DECLARE_MESSAGE_MAP()
};

///
/// @brief
///     ���ÿؼ��ı�����ɫ
///
inline void CWaveFormGramCtrl::SetBkgndColor(const COLORREF clr)
{
	m_clBkgndColor = clr;
}

///
/// @brief
///     ���ÿؼ��������ɫ
///
inline void CWaveFormGramCtrl::SetTitleColor(const COLORREF clr)
{
	m_clTitleColor = clr;
}

///
/// @brief
///     ���ÿؼ�������ʾ������ڲ��������ɫ
///
inline void CWaveFormGramCtrl::SetWaveAreaGridColor(const COLORREF clr)
{
	m_clWaveAreaGridColor = clr;
}

///
/// @brief
///     ���ÿؼ���ߵĿ̶ȵ���ɫ
///
inline void CWaveFormGramCtrl::SetScaleValColor(const COLORREF clr)
{
	m_clScaleValColor = clr;
}

///
/// @brief
///     ���ÿؼ��α����ɫ
///
inline void CWaveFormGramCtrl::SetCurSorColor(const COLORREF cursor1Clr, const COLORREF cursor2Clr)
{
	m_clCursor1Color = cursor1Clr;
	m_clCursor2Color = cursor2Clr;
}

///
/// @brief
///     ���ÿؼ��α�ı��
///
inline void CWaveFormGramCtrl::SetCursorScale(const CString &cursor1Scale, const CString &cursor2Scale)
{
	m_strCursor1Scale = cursor1Scale;
	m_strCursor2Scale = cursor2Scale;
}

///
/// @brief
///     ���ÿؼ��ı߾���Ϣ
///
inline void CWaveFormGramCtrl::SetMargin(const int leftMargin, const int rightMargin, const int topMargin, const int bottomMargin)
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
///     ���ÿؼ����ڲ�����X-Y��߷����ϵ�������
///
inline void CWaveFormGramCtrl::SetWaveAreaGrid(const int xHalfGridCount, const int yHalfGridCount)
{
	m_iXHalfGridCount     = xHalfGridCount;
	m_iYHalfGridCount     = yHalfGridCount;
}

///
/// @brief
///     ���ÿؼ���Y��߷ֶ�ֵ������ַ���
///
inline void CWaveFormGramCtrl::SetYIntervalMaxCharNum(const int num)
{
	m_iYInterValCharNum = num;
}

///
/// @brief
///     ���ñ���Ŀɼ���
///
inline void CWaveFormGramCtrl::SetTitleVisible(BOOL bShow /* = TRUE */)
{
	m_bShowTitle = bShow;
}

///
/// @brief
///     ����ˮƽ����Ŀɼ���
///
inline void CWaveFormGramCtrl::SetHorGridVisible(BOOL bShow /* = TRUE */, BOOL bCentreShow /* = TRUE */)
{
	m_bShowWaveAreaHorGrid = bShow;
	m_bShowWaveAreaHorCentreGrid = bCentreShow;
}

///
/// @brief
///     ���ô�ֱ����Ŀɼ���
///
inline void CWaveFormGramCtrl::SetVerGridVisible(BOOL bShow /* = TRUE */)
{
	m_bShowWaveAreaVerGrid = bShow;
}

///
/// @brief
///     ����X��ߵĿ̶ȵĿɼ���
///
inline void CWaveFormGramCtrl::SetXScaleValVisible(BOOL bShow /* = TRUE */)
{
	m_bShowXScaleVal = bShow;
}

///
/// @brief
///     ����Y��ߵĿ̶ȵĿɼ���
///
inline void CWaveFormGramCtrl::SetYScaleValVisible(BOOL bShow /* = TRUE */)
{
	m_bShowYScaleVal = bShow;
}

///
/// @brief
///     ���ò���������Ϣ��λ�����Ͻǣ��Ŀɼ���
///
inline void CWaveFormGramCtrl::SetWaveDescVisible(const BOOL bShow /* = TRUE */)
{
	m_bShowWaveDesc = bShow;
}

///
/// @brief
///     �����α�1��λ�ù̶�����㴦������������Ϣ�ͱ�ߵĿɼ���
///
inline void CWaveFormGramCtrl::SetCursor1Visible(BOOL bShowCursor /* = TRUE */, BOOL bShowDesc /* = TRUE */, BOOL bShowScale /* = TRUE */)
{
	m_bShowCursor1      = bShowCursor;
	m_bShowCursor1Desc  = bShowDesc;
	m_bShowCursor1Scale = bShowScale;
}

///
/// @brief
///     �����α�2��λ�ÿɱ䣩����������Ϣ�ͱ�ߵĿɼ���
///
inline void CWaveFormGramCtrl::SetCursor2Visible(BOOL bShowCursor /* = TRUE */, BOOL bShowDesc /* = TRUE */, BOOL bShowScale /* = TRUE */)
{
	m_bShowCursor2      = bShowCursor;
	m_bShowCursor2Desc  = bShowDesc;
	m_bShowCursor2Scale = bShowScale;
}
