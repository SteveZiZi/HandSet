/// @file
///     HistogramCtrl.cpp
///
/// @brief
///     ��״ͼ�ؼ�
///
/// @note
///     ����״ͼΪ������ʾ��������������������+���⣩
///     ����������X��߿̶�Ϊ��ֵ����0��ʼ
///     �������������û�������Ӳ��Զ���X��߿̶�ֵ���ӳ������������������
///     ����״ͼ��Y����ϲ���������
///     �ؼ�����λ��������ʾ��������Ͻ�
///     ����������Ϣλ��������ʾ��������Ͻ�
///
/// Copyright (c) 2016 ������ͨ�����Ƽ����޹�˾�����
///
/// ���ߣ�
///    chao  2013.4.12
///
/// �汾��
///    1.0.x.x
/// 
/// �޸���ʷ��
///    �� ʱ��         : �汾      :  ��ϸ��Ϣ    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.05.09     |1.0.509    |�޸����������ı����ص��ж�Ϊ��ǰ3����ı������ж�2��     |
///    |2013.05.18     |1.0.518    |�޸��α�������Ӧʱ�α�ƫ�ƵĴ���                         |
///    |2013.05.30     |1.0.530    |�����������ȳ����������޵�Bug                            |
///    |2013.07.09     |1.0.709    |��������ʾ�ӿڷ���Ϊˮƽ/��ֱ������ʾ�ӿ�                |
///    |2013.07.16     |1.0.716    |����ǰ�α�����������������Ϣ���α�Ļ��Ʒ��룬��������Ϣ�ŵ�������|
///    |2013.07.16     |1.0.716    |�޸���д��������д��Ϣ�Ļ���ʵ�֣���д��Ϣ����������ʾ�����򲻻���|
///    |2013.08.07     |1.0.807    |�޸��������ýӿ��ж���ε���Ч���ж�                     |
///
#include "stdafx.h"
#include <float.h>
#include "HistogramCtrl.h"

///
/// @brief
///     �ؼ��ı�����ɫ
///
#define HTG_CTRL_BKGND_COLOR    RGB(0,0,0)            ///< ��ɫ

///
/// @brief
///     �����Ĭ�����ã�����������ʽ�Լ����⡢X-Y��߿̶ȺͲ���������Ϣ�������С����ɫ
///
#define HTG_FONT_STYLE            _T("΢���ź�")
#define HTG_TITLE_FONT_SIZE       20
#define HTG_TITLE_FONT_COLOR      RGB(255,255,255)     ///< ��ɫ
#define HTG_SCALE_FONT_SIZE       15
#define HTG_SCALE_FONT_COLOR      RGB(128,128,135)     ///< ů��ɫ
#define HTG_PILLARDESC_FONT_SIZE  15
#define HTG_PILLARDESC_FONT_COLOR RGB(255,0,0)         ///< ��ɫ
#define HTG_FEATURE_FONT_SIZE     15
#define HTG_FEATURE_FONT_COLOR    RGB(124,252,0)       ///< ����ɫ

///
/// @brief
///     ������ʾ�����Ĭ�����ã��߿���ڲ���������������͡���Ⱥ���ɫ
///
#define HTG_BORDER_LINE_STYLE PS_SOLID
#define HTG_BORDER_LINE_WIDTH 1
#define HTG_GRID_LINE_STYLE   PS_DASH
#define HTG_GRID_LINE_WIDTH   0
#define HTG_GRID_LINE_COLOR   RGB(128,128,105)         ///< ���ɫ

///
/// @brief
///     �������ε�Ĭ�����ã��������εĿ�Ⱥ���ɫ
///
#define HTG_PILLAR_WIDTH 6
#define HTG_PILLAR_COLOR RGB(255,255,0)                ///< ��ɫ

///
/// @brief
///     ��������������ռ�õ������ȼ��ڱ���ϵĿ̶ȵ���ɫ
///
#define HTG_ONE_SPC_PILLAR_GRID_WIDTH 35
#define HTG_SPC_PILLAR_SCALEVAL_COLOR RGB(255,0,0) ///< ��ɫ

///
/// @brief
///     �α��Ĭ�����ã���ɫ��������ʽ��������Ⱥ����������εĿ��
///
#define HTG_CURSOR_COLOR       HTG_PILLARDESC_FONT_COLOR
#define HTG_CURSOR_LINE_STYLE  PS_SOLID
#define HTG_CURSOR_LINE_WIDTH  1
#define HTG_CURSOR_TRIG_WIDTH  3

IMPLEMENT_DYNAMIC(CHistogramCtrl, CStatic)

///
/// @brief
///     ���캯��
///
CHistogramCtrl::CHistogramCtrl()
: m_pOldBkgndBmp(NULL)
, m_pOldForgndBmp(NULL)
, m_clBkgndColor(HTG_CTRL_BKGND_COLOR)
, m_clPillarAreaGridColor(HTG_GRID_LINE_COLOR)
, m_clFeatureInfoColor(HTG_FEATURE_FONT_COLOR)
, m_clTitleColor(HTG_TITLE_FONT_COLOR)
, m_clScaleValColor(HTG_SCALE_FONT_COLOR)
, m_clSpcScaleValColor(HTG_SPC_PILLAR_SCALEVAL_COLOR)
, m_iCommonPillarCount(50)
, m_iCursorPos(0)
, m_iXGridCount(5)
, m_iYGridCount(7)
, m_iXGramCount(1)
, m_iYGramCount(1)
, m_iLeftMargin(4)
, m_iRightMargin(4)
, m_iTopMargin(4)
, m_iBottomMargin(4)
, m_bShowTitle(FALSE)
, m_bShowPillarAreaHorGrid(TRUE)
, m_bShowPillarAreaVerGrid(TRUE)
, m_bShowXScaleVal(TRUE)
, m_bShowYScaleVal(TRUE)
, m_bShowCursor(TRUE)
, m_iXGramSpace(0)
, m_iYGramSpace(0)
{
	m_FeaturePillar[0].idx = 0;
	m_FeaturePillar[1].idx = 3;
	m_FeaturePillar[2].idx = 5;
	m_FeaturePillar[3].idx = 7;
	m_FeaturePillar[4].idx = 9;

	m_fXScaleInterval.reserve(m_iXGramCount*m_iYGramCount);
	m_fYScaleInterval.reserve(m_iXGramCount*m_iYGramCount);
	m_strYUnit.reserve(m_iXGramCount*m_iYGramCount);
	m_clPillarColor.reserve(m_iXGramCount*m_iYGramCount);
	m_clCursorColor.reserve(m_iXGramCount*m_iYGramCount);
	m_strTitle.reserve(m_iXGramCount*m_iYGramCount);
	for ( int i = 0; i < m_iXGramCount*m_iYGramCount; i++ )
	{
		m_fXScaleInterval.push_back(10.f);
		m_fYScaleInterval.push_back(16.667f);
		m_strYUnit.push_back(_T(""));
		m_clPillarColor.push_back(HTG_PILLAR_COLOR);
		m_clCursorColor.push_back(HTG_CURSOR_COLOR);
		m_strTitle.push_back(_T(""));
	}

	m_cTitleFont.CreateFont(HTG_TITLE_FONT_SIZE,0,0,0,FW_NORMAL,FALSE,0,0,0,0,0,0,0,HTG_FONT_STYLE);
	m_cScaleValFont.CreateFont(HTG_SCALE_FONT_SIZE,0,0,0,FW_NORMAL,FALSE,0,0,0,0,0,0,0,HTG_FONT_STYLE);
	m_cPillarDescFont.CreateFont(HTG_PILLARDESC_FONT_SIZE,0,0,0,FW_NORMAL,FALSE,0,0,0,0,0,0,0,HTG_FONT_STYLE);
	m_cFeatureFont.CreateFont(HTG_FEATURE_FONT_SIZE,0,0,0,FW_NORMAL,FALSE,0,0,0,0,0,0,0,HTG_FONT_STYLE);
}

///
/// @brief
///     ��������
///
CHistogramCtrl::~CHistogramCtrl()
{
	
}

///
/// @brief
///     �����ض���״ͼ����������ɫ����˳��ų�����Χ�����Ĳ���ΪĬ����ɫ
///
/// @param[in]
///     idxSeq - ��״ͼ��˳��ţ����������ң��ٴ��ϵ��µ�˳�򣩣���0��ʼ
/// @param[in]
///     clr    - ��ɫֵ
///
/// @return
///
///
void CHistogramCtrl::SetPillarColor(const size_t idxSeq, const COLORREF clr)
{
	if ( idxSeq >= m_clPillarColor.size() )
	{
		for ( size_t i = m_clPillarColor.size(); i <= idxSeq; i++ )
		{
			if ( i == idxSeq )
			{
				m_clPillarColor.push_back(clr);
				break;
			}
			// ��������ΪĬ����ɫ
			m_clPillarColor.push_back(HTG_PILLAR_COLOR);
		}
	}
	else
		m_clPillarColor.at(idxSeq) = clr;
}

///
/// @brief
///     �����α����ɫ����˳��ų�����Χ�����Ĳ���ΪĬ����ɫ
///
/// @param[in]
///     idxSeq - ��״ͼ��˳��ţ����������ң��ٴ��ϵ��µ�˳�򣩣���0��ʼ
/// @param[in]
///     clr    - ��ɫֵ
///
/// @return
///
///
void CHistogramCtrl::SetCursorColor(const size_t idxSeq, const COLORREF clr)
{
	if ( idxSeq >= m_clCursorColor.size() )
	{
		for ( size_t i = m_clCursorColor.size(); i <= idxSeq; i++ )
		{
			if ( i == idxSeq )
			{
				m_clCursorColor.push_back(clr);
				break;
			}
			// ��������ΪĬ����ɫ
			m_clCursorColor.push_back(HTG_CURSOR_COLOR);
		}
	}
	else
		m_clCursorColor.at(idxSeq) = clr;
}

///
/// @brief
///     ���ÿؼ��ı��⣨λ�ڿؼ�������ʾ��������Ͻǣ�,��˳��ų�����Χ�����Ĳ���Ϊ��
///
/// @param[in]
///     idxSeq - ��״ͼ��˳��ţ����������ң��ٴ��ϵ��µ�˳�򣩣���0��ʼ
/// @param[in]
///     str    - �ؼ��ı���
///
/// @return
///
///
void CHistogramCtrl::SetTitle(const size_t idxSeq, const CString &str)
{
	if ( idxSeq >= m_strTitle.size() )
	{
		for ( size_t i = m_strTitle.size(); i <= idxSeq; i++ )
		{
			if ( i == idxSeq )
			{
				m_strTitle.push_back(str);
				break;
			}
			m_strTitle.push_back(_T(""));
		}
	}
	else
		m_strTitle.at(idxSeq) = str;
}

///
/// @brief
///     ���ÿؼ���X-Y��ߵķֶ�ֵ����˳��ų�����Χ�����Ĳ���ΪĬ������
///
/// @param[in]
///     idxSeq    - ��״ͼ��˳��ţ����������ң��ٴ��ϵ��µ�˳�򣩣���0��ʼ
/// @param[in]
///     xInterval - X��ߵķֶ�ֵ
/// @param[in]
///     yInterval - Y��ߵķֶ�ֵ
/// @param[in]
///     yUnit     - Y��ߵĵ�λ
///
/// @return
///
///
void CHistogramCtrl::SetScaleInterval(const size_t idxSeq, const float xInterval, const float yInterval, const CString &yUint)
{
	if ( idxSeq >= m_fXScaleInterval.size() )
	{
		for ( size_t i = m_fXScaleInterval.size(); i <= idxSeq; i++ )
		{
			if ( i == idxSeq )
			{
				m_fXScaleInterval.push_back(xInterval);
				m_fYScaleInterval.push_back(yInterval);
				m_strYUnit.push_back(yUint);
				break;
			}
			// ��������ΪĬ������
			m_fXScaleInterval.push_back(1.0f);
			m_fYScaleInterval.push_back(1.0f);
			m_strYUnit.push_back(_T(""));
		}
	}
	else
	{
		m_fXScaleInterval.at(idxSeq) = xInterval;
		m_fYScaleInterval.at(idxSeq) = yInterval;
		m_strYUnit.at(idxSeq) = yUint;
	}
}

///
/// @brief
///     �Ƴ��ض�����������
///
/// @param[in]
///     idxSeq - ��״ͼ�е�N��������������0��ʼ����������
///
/// @return
///
///
void CHistogramCtrl::RemoveASpecialPillar(const size_t idxSeq)
{
	std::vector<CString>::iterator it;
	size_t i = 0;
	for ( it = m_strSpcScaleVal.begin(); it != m_strSpcScaleVal.end(); it++ )
	{
		if ( i == idxSeq )
		{
			m_strSpcScaleVal.erase(it);
			break;
		}
		i++;
	}
}

///
/// @brief
///     ���ÿؼ�����״ͼ�ĸ���
///
/// @param[in]
///     xGramCount - X��߷����ϵ���״ͼ����
/// @param[in]
///     yGramCount - Y��߷����ϵ���״ͼ����
///
/// @return
///
///
void CHistogramCtrl::SetGramCount(const int xGramCount, const int yGramCount)
{
	m_iXGramCount = xGramCount;
	m_iYGramCount = yGramCount;
}

///
/// @brief
///     ���ÿؼ����������
///
/// @param[in]
///     pFont - ָ�������ָ��
///
/// @return
///
///
void CHistogramCtrl::SetTitleFont(CFont *pFont)
{
	if ( pFont == NULL && pFont->GetSafeHandle() != NULL )
		return;
	m_cTitleFont.DeleteObject();
	LOGFONT logFont;
	pFont->GetLogFont(&logFont);
	m_cTitleFont.CreateFontIndirect(&logFont);
}

///
/// @brief
///     ����X-Y��߿̶ȵ�����
///
/// @param[in]
///     pFont - ָ�������ָ��
///
/// @return
///
///
void CHistogramCtrl::SetScaleValFont(CFont *pFont)
{
	if ( pFont == NULL || pFont->GetSafeHandle() == NULL )
		return;
	m_cScaleValFont.DeleteObject();
	LOGFONT logFont;
	pFont->GetLogFont(&logFont);
	m_cScaleValFont.CreateFontIndirect(&logFont);
}

///
/// @brief
///     ��������������Ϣ������
///
/// @param[in]
///     pFont - ָ�������ָ��
///
/// @return
///
///
void CHistogramCtrl::SetPillarDescFont(CFont *pFont)
{
	if ( pFont == NULL || pFont->GetSafeHandle() == NULL )
		return;
	m_cPillarDescFont.DeleteObject();
	LOGFONT logFont;
	pFont->GetLogFont(&logFont);
	m_cPillarDescFont.CreateFontIndirect(&logFont);
}

///
/// @brief
///     ������д��������д��Ϣ������
///
/// @param[in]
///     pFont - ָ�������ָ��
///
/// @return
///
///
void CHistogramCtrl::SetFeatureInfoFont(CFont *pFont)
{
	if ( pFont == NULL || pFont->GetSafeHandle() == NULL )
		return;
	m_cFeatureFont.DeleteObject();
	LOGFONT logFont;
	pFont->GetLogFont(&logFont);
	m_cFeatureFont.CreateFontIndirect(&logFont);
}

///
/// @brief
///     ��ʼ���ؼ�
///
/// @return
///
///
void CHistogramCtrl::Init()
{
	DrawBkgnd();
	ResetForgnd();
}

///
/// @brief
///     ����������ʾ�����ȫ��������Ϣ
///
/// @return
///
///
void CHistogramCtrl::ErasePillar()
{
	CRect rt;
	GetClientRect(rt);
	m_cForgndDC.BitBlt(0,0,rt.Width(),rt.Height(),&m_cBkgndDC,0,0,SRCCOPY);

}

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
/// @return
///
///
void CHistogramCtrl::DrawHistogram(const size_t idxSeq, const float *cmnPillarData,const float *spcPillarData,const CString &curPillarDesc)
{
	if ( (int)idxSeq >= m_iXGramCount*m_iYGramCount)
		return;

	// ���㵱ǰҪ���Ƶ�������ʾ����ľ�����
	int xPt = (int)idxSeq%m_iXGramCount;
	int yPt = (int)idxSeq/m_iXGramCount;
	CRect pillarArea(m_rtPillarArea.left   + m_iXGramSpace * xPt,
		m_rtPillarArea.top    + m_iYGramSpace * yPt,
		m_rtPillarArea.right  + m_iXGramSpace * xPt,
		m_rtPillarArea.bottom + m_iYGramSpace * yPt);

	DrawCursor(&m_cForgndDC,idxSeq,pillarArea);
	DrawPillar(&m_cForgndDC,idxSeq,pillarArea,cmnPillarData,spcPillarData);
	DrawFeaturePillar(&m_cForgndDC,idxSeq,pillarArea,cmnPillarData,spcPillarData);
	DrawPillarDesc(&m_cForgndDC,idxSeq,pillarArea,curPillarDesc);
}	


///
/// @brief
///     ǿ��ˢ�£�����ǰ�ڴ��豸�����е�����ˢ�µ�Ŀ���ڴ��豸������DC����
///
/// @param[in]
///     pDC - ָ��Ŀ��DC��ָ�룻ΪNULLʱ����ΪĬ�ϵ�DC
///
/// @return
///
///
void CHistogramCtrl::Flush(CDC *pDC)
{
	CRect rt;
	GetClientRect(rt);
	if (NULL == pDC)
	{
		CClientDC cdc(this);
		cdc.BitBlt(0,0,rt.Width(),rt.Height(),&m_cForgndDC,0,0,SRCCOPY);
	}
	else
	{
		pDC->BitBlt(0,0,rt.Width(),rt.Height(),&m_cForgndDC,0,0,SRCCOPY);	
	}
}

BEGIN_MESSAGE_MAP(CHistogramCtrl, CStatic)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_KEYUP()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

///
/// @brief
///     �ػ���Ϣ������
///
/// @return
///
///
void CHistogramCtrl::OnPaint()
{
// 	CRect rt;
// 	GetClientRect(rt);
// 
// 	PAINTSTRUCT ps;
// 	CDC *pDC = BeginPaint(&ps);
// 	pDC->BitBlt(0,0,rt.Width(),rt.Height(),m_cForgndDC,0,0,SRCCOPY);
// 	EndPaint(&ps);
	DWORD st = GetTickCount();
//	RETAILMSG(1,(_T("XieBoTu Draw Begin:%d------------------\r\n"),st));
	CPaintDC dc(this); // device context for painting
	CRect rt;
	GetClientRect(rt);
	dc.BitBlt(0,0,rt.Width(),rt.Height(),&m_cForgndDC,0,0,SRCCOPY);
	DWORD et = GetTickCount();
//	RETAILMSG(1,(_T("XieBoTu Draw End:%d ; %d------------------\r\n"),et,et-st));
}

///
/// @brief
///     �ͷſؼ���Դ��Ϣ������
///
/// @return
///
///
void CHistogramCtrl::OnDestroy()
{
	CStatic::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������
	Release();
}

///
/// @brief
///     ���̰����ͷ���Ϣ������
///
/// @return
///
///
void CHistogramCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	switch (nChar)
	{
	case VK_LEFT:
		CursorMoveLeft();
		break;
	case VK_RIGHT:
		CursorMoveRight();
		break;
	default:
		break;
	}
	CStatic::OnKeyUp(nChar, nRepCnt, nFlags);
}

///
/// @brief
///     ����������Ϣ������
///
/// @return
///
///
BOOL CHistogramCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	return TRUE;
}

///
/// @brief
///     �����ڴ�DC��λͼ�����λͼ�Ѿ�����, �����ظ�����
///
/// @param[in]
///     rt       - Ҫ������DC��λͼ�Ĵ�С
/// @param[in]
///     dc       - Ҫ������DC
/// @param[in]
///     bmp      - Ҫ������λͼ
/// @param[in]
///     pPOldBmp - ָ�򴴽���DC�滻������ԭʼBmp��ָ���ָ��
///
void CHistogramCtrl::CreateDCandBmp(const CRect &rt, CDC &dc, CBitmap &bmp, CBitmap **pPOldBmp)
{
	if (NULL != dc.GetSafeHdc())
	{
		BITMAP bmpInfo;
		bmp.GetBitmap(&bmpInfo);

		if ( rt.Width() != bmpInfo.bmWidth || rt.Height() != bmpInfo.bmHeight )
		{
			// �ؼ���С�����ı䣬���ͷžɵı���DC��λͼ
			dc.SelectObject(*pPOldBmp);
			dc.DeleteDC();
			bmp.DeleteObject();
		}
		else
		{
			// �ؼ���Сû�ı䣬�򷵻�
			return;
		}
	}

	// �½�����DC��λͼ
	CDC *pDC = GetDC();
	dc.CreateCompatibleDC(pDC);
	bmp.CreateCompatibleBitmap(pDC,rt.Width(),rt.Height());
	*pPOldBmp = dc.SelectObject(&bmp);
	ReleaseDC(pDC);
}

///
/// @brief
///     ���ƿؼ�����
///
/// @return
///
void CHistogramCtrl::DrawBkgnd()
{
	CRect rt;
	GetClientRect(rt);

	// ��������DC��λͼ
	CreateDCandBmp(rt,m_cBkgndDC,m_cBkgndBmp,&m_pOldBkgndBmp);

	// �������
	m_cBkgndDC.FillSolidRect(0,0,rt.Width(),rt.Height(),m_clBkgndColor);

	// �����1��������ʾ����ľ����򣬼�ÿ��ͼ�Ĵ�С
	UpdatePillarAreaSpace(&m_cBkgndDC);

	// �������ң����ϵ��»���������״ͼ��������ʾ����
	for ( int y = 0; y < m_iYGramCount; y++ )
	{
		for ( int x = 0; x < m_iXGramCount; x++ )
		{
			// ���㵱ǰҪ���Ƶ�������ʾ����ľ�����
			CRect pillarArea(m_rtPillarArea.left   + m_iXGramSpace * x,
			                 m_rtPillarArea.top    + m_iYGramSpace * y,
			                 m_rtPillarArea.right  + m_iXGramSpace * x,
			                 m_rtPillarArea.bottom + m_iYGramSpace * y);
#if !CHUN_HUA
			m_cBkgndDC.FillSolidRect(pillarArea,RGB(0,0,0));
#endif

			// ����������ʾ������ڲ�����
			DrawPillarAreaGrid(&m_cBkgndDC,pillarArea);

			// ����������ʾ����ı߿�
			DrawPillarAreaBoard(&m_cBkgndDC,pillarArea);

			// ����X-Y��ߵĿ̶�
			DrawScale(&m_cBkgndDC,y*m_iXGramCount+x,pillarArea);

			// �������Ͻǵı���
			DrawTitle(&m_cBkgndDC,y*m_iXGramCount+x,pillarArea);
		}
	}
}

///
/// @brief
///     ���ÿؼ�ǰ��
///
/// @return
///
void CHistogramCtrl::ResetForgnd()
{
	CRect rt;
	GetClientRect(rt);

	// ����ǰ��DC��λͼ
	CreateDCandBmp(rt,m_cForgndDC,m_cForgndBmp,&m_pOldForgndBmp);

	// ������dc������ǰ��dc
	m_cForgndDC.BitBlt(0,0,rt.Width(),rt.Height(),&m_cBkgndDC,0,0,SRCCOPY);
}

///
/// @brief
///     �ؼ���Դ�ͷź���
///
/// @return
///
void CHistogramCtrl::Release()
{
	if (NULL != m_cBkgndDC.GetSafeHdc())
	{
		m_cBkgndDC.SelectObject(m_pOldBkgndBmp);
		m_cBkgndDC.DeleteDC();
		m_cBkgndBmp.DeleteObject();
	}
	if (NULL != m_cForgndDC.GetSafeHdc())
	{
		m_cForgndDC.SelectObject(m_pOldForgndBmp);
		m_cForgndDC.DeleteDC();
		m_cForgndBmp.DeleteObject();
	}
    
	m_cTitleFont.DeleteObject();
	m_cScaleValFont.DeleteObject();
	m_cPillarDescFont.DeleteObject();
	m_cFeatureFont.DeleteObject();
}

///
/// @brief
///     �����1��������ʾ����ľ����򣬼�ÿ��ͼ�Ĵ�С
///
/// @param[in]
///     pDC - �豸����DC��ָ��
///
/// @return
///     
///
void CHistogramCtrl::UpdatePillarAreaSpace(CDC *pDC)
{
	CRect rt;
	GetClientRect(rt);
	m_iXGramSpace = (rt.Width() - m_iLeftMargin - m_iRightMargin) / m_iXGramCount;
	m_iYGramSpace = (rt.Height() - m_iTopMargin - m_iBottomMargin) / m_iYGramCount;
	rt.left   = rt.left + m_iLeftMargin;
	rt.top    = rt.top  + m_iTopMargin;
	rt.right  = rt.left + m_iXGramSpace;
	rt.bottom = rt.top  + m_iYGramSpace;

	CFont *pOldFont = pDC->SelectObject(&m_cScaleValFont);
	CSize scaleValMaxSz = pDC->GetTextExtent(L"100.0%");
	pDC->SelectObject(pOldFont);

	if ( m_bShowYScaleVal )
	{
		m_rtPillarArea.left = rt.left + scaleValMaxSz.cx;
		m_rtPillarArea.top  = rt.top  + scaleValMaxSz.cy/2;
		if ( !m_bShowXScaleVal )
		{
			m_rtPillarArea.right  = rt.right;
			m_rtPillarArea.bottom = rt.bottom - scaleValMaxSz.cy/2;
		}
	}
	if ( m_bShowXScaleVal )
	{
		m_rtPillarArea.right  = rt.right - scaleValMaxSz.cx/2;
		m_rtPillarArea.bottom = rt.bottom - scaleValMaxSz.cy;
		if ( !m_bShowYScaleVal )
		{
			m_rtPillarArea.left = rt.left + scaleValMaxSz.cx/2;
			m_rtPillarArea.top  = rt.top;
		}
	}
	if ( !m_bShowYScaleVal && !m_bShowXScaleVal )
	{
		m_rtPillarArea.left = rt.left;
		m_rtPillarArea.top  = rt.top;
		m_rtPillarArea.right  = rt.right;
		m_rtPillarArea.bottom = rt.bottom;
	}
}

///
/// @brief
///     ����������ʾ������ڲ�����
///
/// @param[in]
///     pDC        - �豸����DC��ָ��
/// @param[in]
///     pillarArea - ������ʾ����ľ�����
///
/// @return
///     
///
void CHistogramCtrl::DrawPillarAreaGrid(CDC *pDC, const CRect &pillarArea)
{
	if ( m_bShowPillarAreaHorGrid == FALSE && m_bShowPillarAreaVerGrid == FALSE )
		return;

	// ���㳣����������������ˮƽ��ȼ���ֱ�߶�
	float horGridWidth = ((float)pillarArea.Width()-(float)m_strSpcScaleVal.size()*HTG_ONE_SPC_PILLAR_GRID_WIDTH)/(float)m_iXGridCount;
	float verGridWidth = (float)pillarArea.Height()/(float)m_iYGridCount;

	CPen  pen;
	pen.CreatePen(HTG_GRID_LINE_STYLE,HTG_GRID_LINE_WIDTH,m_clPillarAreaGridColor);
	CPen *pOldPen = pDC->SelectObject(&pen);
	// �������������������ֱ������
	for ( size_t i = 1; i <= m_strSpcScaleVal.size() && m_bShowPillarAreaVerGrid; i++ )
	{
		pDC->MoveTo(int(pillarArea.left+i*HTG_ONE_SPC_PILLAR_GRID_WIDTH),pillarArea.top);
		pDC->LineTo(int(pillarArea.left+i*HTG_ONE_SPC_PILLAR_GRID_WIDTH),pillarArea.bottom);
	}
	// ���Ƴ��������������ֱ������
	int left = int (pillarArea.left+HTG_ONE_SPC_PILLAR_GRID_WIDTH*m_strSpcScaleVal.size());
	for (int i = 1; i < m_iXGridCount && m_bShowPillarAreaVerGrid; ++i)
	{
		pDC->MoveTo(int(left+i*horGridWidth),pillarArea.top);
		pDC->LineTo(int(left+i*horGridWidth),pillarArea.bottom);
	}
	// ����ˮƽ������
	for (int i = 1; i < m_iYGridCount && m_bShowPillarAreaHorGrid; ++i)
	{
		pDC->MoveTo(pillarArea.left,int(pillarArea.top+verGridWidth*i));
		pDC->LineTo(pillarArea.right,int(pillarArea.top+verGridWidth*i));
	}
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
}

///
/// @brief
///     ����������ʾ����ı߿�
///
/// @param[in]
///     pDC        - �豸����DC��ָ��
/// @param[in]
///     pillarArea - ������ʾ����ľ�����
///
/// @return
///     
///
void CHistogramCtrl::DrawPillarAreaBoard(CDC *pDC, const CRect &pillarArea)
{
	CPen pen;
	pen.CreatePen(HTG_BORDER_LINE_STYLE,HTG_BORDER_LINE_WIDTH,m_clScaleValColor);
	CPen *pOldPen = pDC->SelectObject(&pen);
	// ��߿���
	pDC->MoveTo(pillarArea.left,pillarArea.top);
	pDC->LineTo(pillarArea.left,pillarArea.bottom);
	// �ұ߿���
	pDC->MoveTo(pillarArea.right,pillarArea.top);
	pDC->LineTo(pillarArea.right,pillarArea.bottom);
	// �ϱ߿���
	pDC->MoveTo(pillarArea.left,pillarArea.top);
	pDC->LineTo(pillarArea.right,pillarArea.top);
	// �±߿���
	pDC->MoveTo(pillarArea.left,pillarArea.bottom);
	pDC->LineTo(pillarArea.right,pillarArea.bottom);
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
}

///
/// @brief
///     ����X-Y��ߵĿ̶�
///
/// @param[in]
///     pDC        - �豸����DC��ָ��
/// @param[in]
///     idxSeq     - ��״ͼ��˳��ţ����������ң��ٴ��ϵ��µ�˳�򣩣���0��ʼ
/// @param[in]
///     pillarArea - ������ʾ����ľ�����
///
/// @return
///     
///
void CHistogramCtrl::DrawScale(CDC *pDC, const size_t idxSeq, const CRect &pillarArea)
{
	CFont *pOldFont = pDC->SelectObject(&m_cScaleValFont);
	COLORREF oldClr = pDC->SetTextColor(m_clSpcScaleValColor);
	int oldMode = pDC->SetBkMode(TRANSPARENT);

	// ����X�̶�
	if ( m_bShowXScaleVal )
	{
		// ����������������Ŀ̶�
		for ( size_t i = 0; i < m_strSpcScaleVal.size(); i++ )
		{
			CSize scaleValSz = pDC->GetTextExtent(m_strSpcScaleVal.at(i));
			int left = int(pillarArea.left+(2*i+1)*HTG_ONE_SPC_PILLAR_GRID_WIDTH/2)-scaleValSz.cx/2;
			CRect rect(left,pillarArea.bottom,left+scaleValSz.cx,pillarArea.bottom+scaleValSz.cy);
			pDC->DrawText(m_strSpcScaleVal.at(i), rect, DT_RIGHT);
		}
		// ���㳣�����������X��ߵĿ̶�ֵ�ļ��
		float width = (float)pillarArea.Width()-(float)m_strSpcScaleVal.size()*HTG_ONE_SPC_PILLAR_GRID_WIDTH;
		float xSpanWidth = width / (float)m_iXGridCount;

		// ���ó������������X��ߵķֶ�ֵ
		float xInterval;
		if ( idxSeq >= m_fXScaleInterval.size() )
			xInterval = m_fXScaleInterval.at(0);
		else
			xInterval = m_fXScaleInterval.at(idxSeq);
		// ���Ƴ��������Ŀ̶�,��1���̶Ȳ�����
		pDC->SetTextColor(m_clScaleValColor);
		for (int i=1; i<=m_iXGridCount; ++i)
		{
			float val = i*xInterval;
			CString str;
			str.Format(L"%d", (int)val);
			CSize scaleValSz = pDC->GetTextExtent(str);
			int left;
			if ( i == 0 && m_strSpcScaleVal.size() == 0 )
				left = pillarArea.left;
			else
				left = int(pillarArea.left+i*xSpanWidth+HTG_ONE_SPC_PILLAR_GRID_WIDTH*m_strSpcScaleVal.size())-scaleValSz.cx/2;
			CRect rect(left,pillarArea.bottom,left+scaleValSz.cx,pillarArea.bottom+scaleValSz.cy);
			pDC->DrawText(str, rect, DT_RIGHT);
		}
	}
	
	// ����Y��ߵķֶ�ֵ
	float yInterval;
	CString yUnit;
	if ( idxSeq >= m_fYScaleInterval.size() )
	{
		yInterval = m_fYScaleInterval.at(0);
		yUnit     = m_strYUnit.at(0);
	}
	else
	{
		yInterval = m_fYScaleInterval.at(idxSeq);
		yUnit     = m_strYUnit.at(idxSeq);
	}
	// ����Y�̶�
	if ( m_bShowYScaleVal )
	{
		// ���㳣�����������Y��ߵĿ̶�ֵ�ļ��
		float ySpanWidth = (float)pillarArea.Height() / (float)m_iYGridCount;
		for (int i=0; i<=m_iYGridCount; ++i)
		{
			float val = (m_iYGridCount-i)*yInterval;
			CString str;
			str.Format(L"%0.1f%s", val, yUnit);
			CSize scaleValSz = pDC->GetTextExtent(str);
			int top = int(pillarArea.top+ySpanWidth*i)-scaleValSz.cy/2;
			CRect rect(pillarArea.left-scaleValSz.cx-4,top,pillarArea.left-4,top+scaleValSz.cy);
#if !CHUN_HUA
			if (i != 0)
			{
				pDC->DrawText(str, rect, DT_RIGHT);
			}
#else
			pDC->DrawText(str, rect, DT_RIGHT);
#endif
			
		}
	}
	pDC->SetTextColor(oldClr);
	pDC->SetBkMode(oldMode);
	pDC->SelectObject(pOldFont);
}

///
/// @brief
///     ���ƿؼ��ı��⣨λ��������ʾ��������Ͻǣ�
///
/// @param[in]
///     pDC        - �豸����DC��ָ��
/// @param[in]
///     idxSeq        - ��״ͼ��˳��ţ����������ң��ٴ��ϵ��µ�˳�򣩣���0��ʼ
/// @param[in]
///     pillarArea - ������ʾ����ľ�����
///
/// @return
///     
///
void CHistogramCtrl::DrawTitle(CDC *pDC, const size_t idxSeq, const CRect &pillarArea)
{
	if (m_bShowTitle)
	{
		CString str;
		if ( idxSeq >= m_strTitle.size() )
			str = _T("");
		else
			str = m_strTitle.at(idxSeq);
		CFont *pOldFont = pDC->SelectObject(&m_cTitleFont);
		COLORREF oldClr   = pDC->SetTextColor(m_clTitleColor);
		int oldMode  = pDC->SetBkMode(TRANSPARENT);
		CSize sz = pDC->GetTextExtent(str);
		pDC->ExtTextOutW(pillarArea.right - sz.cx - 2,pillarArea.top + 2,
			ETO_OPAQUE,NULL,str,str.GetLength(),NULL);
		pDC->SetTextColor(oldClr);
		pDC->SetBkMode(oldMode);
		pDC->SelectObject(pOldFont);
	}
}

///
/// @brief
///     ��������
///
/// @param[in]
///     pDC           - �豸����DC��ָ��
/// @param[in]
///     idxSeq        - ��״ͼ��˳��ţ����������ң��ٴ��ϵ��µ�˳�򣩣���0��ʼ
/// @param[in]
///     pillarArea    - ������ʾ����ľ�����
/// @param[in]
///     cmnPillarData - ���泣������������ֵ�Ļ���
/// @param[in]
///     spcPillarData - �������⣨�Զ��壩����������ֵ�Ļ���
///
/// @return
///     
///
void CHistogramCtrl::DrawPillar(CDC *pDC, const size_t idxSeq, const CRect &pillarArea, const float *cmnPillarData, const float *spcPillarData)
{
	ASSERT(NULL != cmnPillarData);
	ASSERT(NULL != spcPillarData);
//	ASSERT(m_strSpcScaleVal.size()!=0);

	// ������������ɫ
	COLORREF pillarClr;
	if ( idxSeq >= m_clPillarColor.size() )
		pillarClr = m_clPillarColor.at(0);
	else
		pillarClr = m_clPillarColor.at(idxSeq);

	// ����Y��ߵķֶ�ֵ
	float yInterval;
	if ( idxSeq >= m_fYScaleInterval.size() )
		yInterval = m_fYScaleInterval.at(0);
	else
		yInterval = m_fYScaleInterval.at(idxSeq);

	// ����Y���1�̶ȵĳ��Ⱥͳ����������򵥸�������ռ�õĿ��
	float OneYScaleLen = (float)pillarArea.Height()/((float)yInterval*(float)m_iYGridCount);
	float cmnAreaWidth = (float)pillarArea.Width()-(float)m_strSpcScaleVal.size()*HTG_ONE_SPC_PILLAR_GRID_WIDTH;
	float OneCPWidth   = (float)cmnAreaWidth/(float)m_iCommonPillarCount;

	CBrush brush;
	brush.CreateSolidBrush(pillarClr);
	// ���������������������
	int minleft = int(pillarArea.left+HTG_ONE_SPC_PILLAR_GRID_WIDTH/2);
	for ( size_t i = 0; i < m_strSpcScaleVal.size(); i++ )
	{
		// �����������ж�
		if ( spcPillarData[i] < 0 )
			break;
		int yScaleVal = (int)spcPillarData[i];
		if ( _finite(yScaleVal) == FALSE )
			yScaleVal = 0;
		if ( yScaleVal > (int)(yInterval*m_iYGridCount) )
			yScaleVal = (int)(yInterval*m_iYGridCount);

		CRect rect(int(minleft+i*HTG_ONE_SPC_PILLAR_GRID_WIDTH-HTG_PILLAR_WIDTH/2),
			       int(pillarArea.bottom-OneYScaleLen*yScaleVal),
			       int(minleft+i*HTG_ONE_SPC_PILLAR_GRID_WIDTH+HTG_PILLAR_WIDTH),
				   int(pillarArea.bottom));
		pDC->FillRect(rect,&brush);
	}
	// ���Ƴ����������������
	minleft = int(pillarArea.left+m_strSpcScaleVal.size()*HTG_ONE_SPC_PILLAR_GRID_WIDTH+OneCPWidth);
	for (int i=0;i<m_iCommonPillarCount;i++)
	{
		// �����������ж�
		if ( cmnPillarData[i] < 0 )
			break;

#if !CHUN_HUA
		float yv = (float)cmnPillarData[i];
		int yScaleVal;
		if (yv-0.009>0 && yv-1.0<0)
		{
			yScaleVal = 2;
		}
		else
			yScaleVal = (int) yv;
		 

#else
	int yScaleVal = (int)cmnPillarData[i];
#endif
		
		if ( _finite(yScaleVal) == FALSE )
			yScaleVal = 0;
		if ( yScaleVal > (int)(yInterval*m_iYGridCount) )
			yScaleVal = (int)(yInterval*m_iYGridCount);


		CRect rect(int(minleft+i*OneCPWidth-HTG_PILLAR_WIDTH/2),
			       int(pillarArea.bottom-OneYScaleLen*yScaleVal),
			       int(minleft+i*OneCPWidth+HTG_PILLAR_WIDTH),
				   int(pillarArea.bottom));
		pDC->FillRect(rect,&brush);
	}
	brush.DeleteObject();
}

///
/// @brief
///     ������д��������д��Ϣ
///
/// @param[in]
///     pDC           - �豸����DC��ָ��
/// @param[in]
///     idxSeq        - ��״ͼ��˳��ţ����������ң��ٴ��ϵ��µ�˳�򣩣���0��ʼ
/// @param[in]
///     pillarArea    - ������ʾ����ľ�����
/// @param[in]
///     cmnPillarData - ���泣������������ֵ�Ļ���
/// @param[in]
///     spcPillarData - �������⣨�Զ��壩����������ֵ�Ļ���
///
/// @return
///     
///
void CHistogramCtrl::DrawFeaturePillar(CDC *pDC, const size_t idxSeq, const CRect &pillarArea, const float *cmnPillarData, const float *spcPillarData)
{
	// ����X��Y��ߵķֶ�ֵ
	float xInterval,yInterval;
	if ( idxSeq >= m_fXScaleInterval.size() )
	{
		xInterval = m_fXScaleInterval.at(0);
		yInterval = m_fYScaleInterval.at(0);
	}
	else
	{
		xInterval = m_fXScaleInterval.at(idxSeq);
		yInterval = m_fYScaleInterval.at(idxSeq);
	}

	// ����Y���1�̶ȵĳ��Ⱥͳ����������򵥸�������ռ�õĿ��
	float OneYScaleLen = (float)pillarArea.Height()/(yInterval*(float)m_iYGridCount);
	float cmnAreaWidth = (float)pillarArea.Width()-(float)m_strSpcScaleVal.size()*HTG_ONE_SPC_PILLAR_GRID_WIDTH;
	float OneCPWidth   = cmnAreaWidth/(float)m_iCommonPillarCount;

	CFont *pOldFont = pDC->SelectObject(&m_cFeatureFont);
	COLORREF oldClr = pDC->SetTextColor(m_clFeatureInfoColor);
	int oldMode  = pDC->SetBkMode(TRANSPARENT);
	CPen pen;
	pen.CreatePen(PS_SOLID,1,m_clFeatureInfoColor);
	CPen* pOldPen = pDC->SelectObject(&pen);

	for ( int i = 0; i < HTG_FEATURE_PILLAR_COUNT; i++ )
	{
		if ( size_t(m_FeaturePillar[i].idx) >= m_strSpcScaleVal.size() )
		{
			// �����������X��߿̶��ı��ľ�����
			pDC->SelectObject(&m_cScaleValFont);
			pDC->SetTextColor(m_clScaleValColor);
			int cmnIdx = int(m_FeaturePillar[i].idx-m_strSpcScaleVal.size());
			float val  = (cmnIdx+1)*(xInterval*m_iXGridCount/m_iCommonPillarCount);
			CString xScaleStr;
			xScaleStr.Format(L"%d", (int)val);
			CSize scaleValSz = pDC->GetTextExtent(xScaleStr);
			int minleft = int(pillarArea.left+m_strSpcScaleVal.size()*HTG_ONE_SPC_PILLAR_GRID_WIDTH+OneCPWidth);
			int left;
			if ( cmnIdx == 0 && m_strSpcScaleVal.size() == 0 )
				left = pillarArea.left;
			else
				left = int(minleft+cmnIdx*OneCPWidth-scaleValSz.cx/2);
			CRect xScaleRct(left,pillarArea.bottom,left+scaleValSz.cx,pillarArea.bottom+scaleValSz.cy);
			// ����X��߿̶�
			pDC->ExtTextOutW(xScaleRct.left,xScaleRct.top,OPAQUE,NULL,xScaleStr,xScaleStr.GetLength(),NULL);
			// ���������ʽ
			pDC->SelectObject(&m_cFeatureFont);
			pDC->SetTextColor(m_clFeatureInfoColor);
		}

		// ���α��ƶ���ĳһ��֮��ʱ��������ʾ����
		if ( m_FeaturePillar[i].idx == m_iCursorPos )
		{
			m_FeaturePillar[i].rect.SetRect(0,0,1,1);
			continue;
		}

		CString yScaleStr;
		CPoint pt;
		if ( size_t(m_FeaturePillar[i].idx) >= m_strSpcScaleVal.size() )
		{
			// �������������ı��ľ�����
			int cmnIdx = int(m_FeaturePillar[i].idx-m_strSpcScaleVal.size());
			float val = cmnPillarData[cmnIdx];
			if ( _finite(val) == FALSE )
				val = 0;
			if ( val < 0 )
				val = 0;
			yScaleStr.Format(L"%0.2f",val);
			CSize sz = pDC->GetTextExtent(yScaleStr);
			int minleft = int(pillarArea.left+m_strSpcScaleVal.size()*HTG_ONE_SPC_PILLAR_GRID_WIDTH+OneCPWidth);

			// ��������
			if ( val > yInterval*m_iYGridCount )
				val = yInterval*m_iYGridCount;
			m_FeaturePillar[i].rect.left = int(minleft+cmnIdx*OneCPWidth-sz.cx/2);
			m_FeaturePillar[i].rect.top = int(pillarArea.bottom-OneYScaleLen*val)-2-sz.cy;
			m_FeaturePillar[i].rect.right = m_FeaturePillar[i].rect.left+sz.cx;
			m_FeaturePillar[i].rect.bottom = m_FeaturePillar[i].rect.top+sz.cy;
			// �����ı�������֮������ߵ����
			pt.x = int(minleft+cmnIdx*OneCPWidth);
//			pt.y = int(pillarArea.bottom-1);
			pt.y = int(pillarArea.bottom-OneYScaleLen*val);
		}
		else
		{
			// �������������ı��ľ�����
			float val = spcPillarData[m_FeaturePillar[i].idx];
			if ( _finite(val) == FALSE )
				val = 0;
			if ( val < 0 )
				val = 0;
			yScaleStr.Format(L"%0.2f",val);
			CSize sz = pDC->GetTextExtent(yScaleStr);
			int minleft = int(pillarArea.left+HTG_ONE_SPC_PILLAR_GRID_WIDTH/2);

			// ��������
			if ( val > yInterval*m_iYGridCount )
				val = yInterval*m_iYGridCount;
			m_FeaturePillar[i].rect.left = minleft+m_FeaturePillar[i].idx*HTG_ONE_SPC_PILLAR_GRID_WIDTH-sz.cx/2;
			m_FeaturePillar[i].rect.top = int(pillarArea.bottom-OneYScaleLen*val)-2-sz.cy;
			m_FeaturePillar[i].rect.right = m_FeaturePillar[i].rect.left+sz.cx;
			m_FeaturePillar[i].rect.bottom = m_FeaturePillar[i].rect.top+sz.cy;
			// �����ı�������֮������ߵ����
			pt.x = minleft+m_FeaturePillar[i].idx*HTG_ONE_SPC_PILLAR_GRID_WIDTH;
//			pt.y = int(pillarArea.bottom-1);
			pt.y = int(pillarArea.bottom-OneYScaleLen*val);
		}

		// �ж����������ı��Ƿ����ǰ3����ı��ص�
		for ( int j = 3; j > 0; j-- )
		{
			if ( i-j >= 0 )
			{
				if ( IsIntersect(m_FeaturePillar[i].rect,m_FeaturePillar[i-j].rect) )
				{
					m_FeaturePillar[i].rect.MoveToY(m_FeaturePillar[i-j].rect.top - m_FeaturePillar[i].rect.Height() - 2);
				}
			}
		}
		for ( int j = 2; j < 4; j++ )
		{
			if ( IsIntersect(m_FeaturePillar[i].rect,m_FeaturePillar[i-j].rect) )
			{
				m_FeaturePillar[i].rect.MoveToY(m_FeaturePillar[i-j].rect.top - m_FeaturePillar[i].rect.Height() - 2);
			}
		}

		// ��ֹ�����������ֵĵײ��������ߵ����
		if (m_FeaturePillar[i].rect.bottom > pt.y)
		{
			m_FeaturePillar[i].rect.MoveToY(m_FeaturePillar[i-1].rect.top - m_FeaturePillar[i].rect.Height() - 5);
		}

		// ��ǰ�������ȵĶ����ı���Ϣ����������ʾ����ʱ��������ʾ�����Ķ����ı���Ϣ
		if ( m_FeaturePillar[i].rect.top < pillarArea.top )
		{
			m_FeaturePillar[i].rect.SetRect(0,0,1,1);
			continue;
		}

		// ���������������ı�
		pDC->ExtTextOutW(m_FeaturePillar[i].rect.left,m_FeaturePillar[i].rect.top,OPAQUE,NULL,yScaleStr,yScaleStr.GetLength(),NULL);

		// ��������
		pDC->MoveTo(pt);
		pDC->LineTo(pt.x,m_FeaturePillar[i].rect.bottom);
	}
	pDC->SetTextColor(oldClr);
	pDC->SetBkMode(oldMode);
	pDC->SelectObject(pOldFont);
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
}

///
/// @brief
///     �����α�
///
/// @param[in]
///     pDC        - �豸����DC��ָ��
/// @param[in]
///     idxSeq     - ��״ͼ��˳��ţ����������ң��ٴ��ϵ��µ�˳�򣩣���0��ʼ
/// @param[in]
///     pillarArea - ������ʾ����ľ�����
///
/// @return
///     
///
void CHistogramCtrl::DrawCursor(CDC *pDC, const size_t idxSeq, const CRect &pillarArea)
{
	if ( !m_bShowCursor )
		return;

	// �����α����ɫ
	COLORREF cursorClr;
	if ( idxSeq >= m_clCursorColor.size() )
		cursorClr = m_clCursorColor.at(0);
	else
		cursorClr = m_clCursorColor.at(idxSeq);

	// ����Y��߷���1����Ŀ�Ⱥͳ����������򵥸�������ռ�õĿ��
	float OneYGridWidth = (float)pillarArea.Height()/(float)m_iYGridCount;
	float cmnAreaWidth  = (float)pillarArea.Width()-(float)m_strSpcScaleVal.size()*HTG_ONE_SPC_PILLAR_GRID_WIDTH;
	float OneCPWidth    = cmnAreaWidth/(float)m_iCommonPillarCount;

	// �����α�����ߵ����
	POINT pt;
	if ( size_t(m_iCursorPos) >= m_strSpcScaleVal.size() )
	{
		int minleft = int(pillarArea.right-cmnAreaWidth+OneCPWidth);
		pt.x = int(minleft+(m_iCursorPos-m_strSpcScaleVal.size())*OneCPWidth);
	}
	else
	{
		int minleft = int(pillarArea.left+HTG_ONE_SPC_PILLAR_GRID_WIDTH/2);
		pt.x = minleft+m_iCursorPos*HTG_ONE_SPC_PILLAR_GRID_WIDTH;
	}
	pt.y = pillarArea.bottom-1;

	// �����α�����
	CPen pen;
	pen.CreatePen(PS_SOLID,1,cursorClr);
	CPen* pOldPen = pDC->SelectObject(&pen);
	pDC->MoveTo(pt);
	pDC->LineTo(pt.x,int(pillarArea.top+OneYGridWidth-HTG_CURSOR_TRIG_WIDTH));
	POINT pos[3];
	// �����α��������˵�������
	CBrush brush(cursorClr);
	CBrush *pOldBrush = pDC->SelectObject(&brush);
	pos[0].y = int(pillarArea.top+OneYGridWidth);
	pos[1].y = int(pillarArea.top+OneYGridWidth+2*HTG_CURSOR_TRIG_WIDTH);
	pos[2].y = int(pillarArea.top+OneYGridWidth+3*HTG_CURSOR_TRIG_WIDTH);
	pos[0].x = pt.x-HTG_CURSOR_TRIG_WIDTH;
	pos[1].x = pt.x-2*HTG_CURSOR_TRIG_WIDTH;
	pos[2].x = pt.x-HTG_CURSOR_TRIG_WIDTH;
	pDC->Polygon(pos,3);
	pos[0].x = pt.x+HTG_CURSOR_TRIG_WIDTH;
	pos[1].x = pt.x+2*HTG_CURSOR_TRIG_WIDTH;
	pos[2].x = pt.x+HTG_CURSOR_TRIG_WIDTH;
	pDC->Polygon(pos,3);
	pDC->SelectObject(pOldBrush);
	brush.DeleteObject();
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
}

///
/// @brief
///     ���Ƶ�ǰ�α����ڵ�������������Ϣ
///
/// @param[in]
///     pDC        - �豸����DC��ָ��
/// @param[in]
///     idxSeq     - ��״ͼ��˳��ţ����������ң��ٴ��ϵ��µ�˳�򣩣���0��ʼ
/// @param[in]
///     pillarArea - ������ʾ����ľ�����
/// @param[in]
///     pillarDesc - ��ǰ�α����ڵ�������������Ϣ
///
/// @return
///     
///
void CHistogramCtrl::DrawPillarDesc(CDC *pDC, const size_t idxSeq, const CRect &pillarArea, const CString &pillarDesc)
{
	if ( !m_bShowCursor )
		return;

	// �����α����ɫ
	COLORREF cursorClr;
	if ( idxSeq >= m_clCursorColor.size() )
		cursorClr = m_clCursorColor.at(0);
	else
		cursorClr = m_clCursorColor.at(idxSeq);

	// �����α����ڵ�������������Ϣ
	CFont *pOldFont = pDC->SelectObject(&m_cPillarDescFont);
	COLORREF oldClr   = pDC->SetTextColor(cursorClr);
	int oldMode  = pDC->SetBkMode(TRANSPARENT);
	CSize sz = pDC->GetTextExtent(pillarDesc,pillarDesc.GetLength());
	pDC->ExtTextOutW(pillarArea.left + 2,pillarArea.top + 2,
		ETO_OPAQUE,NULL,pillarDesc,pillarDesc.GetLength(),NULL);
	pDC->SetTextColor(oldClr);
	pDC->SetBkMode(oldMode);
	pDC->SelectObject(pOldFont);
}

///
/// @brief
///     �ж��������Ƿ��ཻ
///
/// @param[in]
///     rt1 - ����1
/// @param[in]
///     rt2 - ����2
///
/// @return
///     true - �ཻ�� false - ���ཻ
///
bool CHistogramCtrl::IsIntersect(const CRect &rt1,const CRect &rt2)
{
	if(rt1.right <= rt2.left) return false;
	if(rt1.left >= rt2.right ) return false;
	if(rt1.bottom <= rt2.top ) return false;
	if(rt1.top >= rt2.bottom ) return false;

	return true;
}
