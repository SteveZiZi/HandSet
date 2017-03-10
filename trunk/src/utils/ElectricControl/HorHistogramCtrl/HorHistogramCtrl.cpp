/// @file
///     HorHistogramCtrl.cpp
///
/// @brief
///     ˮƽ��״ͼ�ؼ�
///
/// @note
///     ����״ͼͼΪ������ʾ��֧���Ի棨Ĭ�ϣ�����ͼ���ַ�ʽ��
///     �Ի淽ʽ������Ϊ��ͨ���Σ���������ɫͨ��SetPillarColor()���ã�
///     ��ͼ��ʽ������Ϊ��ͼ����������ʽͨ��SetPillarStyle()���ã�
///     ��������������ʽ�����ʾѡ������ͼ��ʽ������ΪĬ�Ϸ�ʽ��
///     ͼ�и����������������ϵ�˳��
///     �ؼ�����λ�ڿؼ������Ͻǣ�
///     ����ֵλ�������ұߣ�
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
///    |2013.05.28     |1.0.0.528  |��������Ĵ�ֱ�߶ȡ�����Y��߿̶ȵĳ��������Լ������������޵�����|
///    |2013.08.07     |1.0.1.807  |�޸��������ýӿ��ж���ε���Ч���ж�                     |
///    |2013.08.08     |1.0.2.808  |�޸ĵ��������ȴ���0.001С��1����ʱ����1���ش���          |
///    |2013.11.05     |1.0.2.1105 |�޸���״ͼ��������ʾ����ı߿����ʽ���ýӿڼ�����ʵ��   |
///    |2013.11.05     |1.0.2.1105 |��������������ʾ����ľ�����ʱY��߳������ô����Bug     |
///    |2013.11.05     |1.0.2.1105 |���������ȡY��߿̶ȵ�����ַ��������Bug               |
///
#include "stdafx.h"
#include <float.h>
#include "HorHistogramCtrl.h"

///
/// @brief
///     �ؼ��ı�����ɫ
///
#define HHG_CTRL_BKGND_COLOR      RGB(0,0,0)           ///< ��ɫ

///
/// @brief
///     �����Ĭ�����ã�����������ʽ�Լ����⡢X-Y��߿̶ȺͲ���������Ϣ�������С����ɫ
///
#define HHG_FONT_STYLE            _T("΢���ź�")
#define HHG_TITLE_FONT_SIZE       20
#define HHG_TITLE_FONT_COLOR      RGB(255,255,255)     ///< ��ɫ
#define HHG_SCALE_FONT_SIZE       15
#define HHG_SCALE_FONT_COLOR      RGB(128,128,135)     ///< ů��ɫ
#define HHG_PILLARDESC_FONT_SIZE  15
#define HHG_PILLARDESC_FONT_COLOR RGB(255,0,0)         ///< ��ɫ

//������״ͼ��Margin
//#ifndef  ZENG_YUN
#define HHG_MARGIN_LEFT			0
#define HHG_MARGIN_RIGHT		0
#define HHG_MARGIN_TOP			0
#define HHG_MARGIN_BOTTOM		0
//#endif
///
/// @brief
///     ������ʾ�����Ĭ�����ã��߿���ڲ���������������͡���Ⱥ���ɫ
///
#define HHG_BORDER_LINE_STYLE PS_SOLID
#define HHG_BORDER_LINE_WIDTH 1
#define HHG_BORDER_LINE_COLOR RGB(128,128,135)         ///< ů��ɫ
#define HHG_GRID_LINE_STYLE   PS_DASH
#define HHG_GRID_LINE_WIDTH   0
#define HHG_GRID_LINE_COLOR   RGB(128,128,105)         ///< ���ɫ

///
/// @brief
///     �������ε�Ĭ�����ã��������εļ�ࡢ��ֱ�߶Ⱥ�ǰ6��������Ĭ����ɫ(�졢�ơ�ɭ���̡��ɺ졢�ȡ�����)
///
#define HHG_PILLAR_SPACE_BETWEEN 6
#define HHG_PILLAR_HEIGHT        10
static const COLORREF k_PillarColor[6] = {RGB(255,0,0),  RGB(255,255,0),RGB(34,139,34),
                                          RGB(220,20,60),RGB(255,165,0),RGB(124,252,0)};

///
/// @brief
///     �ؼ���ͼ��͸������Ĭ����ɫ
///
#define HHG_DEFAULT_TRANS_COLOR   RGB(0x01,0x02,0x03)


IMPLEMENT_DYNAMIC(CHorHistogramCtrl, CStatic)

///
/// @brief
///     ���캯��
///
CHorHistogramCtrl::CHorHistogramCtrl()
: m_pOldBkgndBmp(NULL)
, m_pOldForgndBmp(NULL)
, m_clTransColor(HHG_DEFAULT_TRANS_COLOR)
, m_clBkgndColor(HHG_CTRL_BKGND_COLOR)
, m_clPillarAreaGridColor(HHG_GRID_LINE_COLOR)
, m_clPillarValColor(HHG_PILLARDESC_FONT_COLOR)
, m_clTitleColor(HHG_TITLE_FONT_COLOR)
, m_clScaleValColor(HHG_SCALE_FONT_COLOR)
, m_clPillarAreaBorderColor(HHG_BORDER_LINE_COLOR)
, m_strTitle(_T(""))
, m_iPillarBetween(HHG_PILLAR_SPACE_BETWEEN)
, m_iPillarHeight(HHG_PILLAR_HEIGHT)
, m_iPillarCount(10)
, m_iXGridCount(5)
, m_iYGridCount(6)
, m_fXScaleInterval(10.0f)
, m_iYScaleValLen(0)
, m_iSpaceFromLeftBorderToPillar(4)
, m_iLeftMargin(HHG_MARGIN_LEFT)
, m_iRightMargin(HHG_MARGIN_RIGHT)
, m_iTopMargin(HHG_MARGIN_TOP)
, m_iBottomMargin(HHG_MARGIN_BOTTOM)
, m_bShowTitle(FALSE)
, m_bShowHorGrid(TRUE)
, m_bShowVerGrid(TRUE)
, m_bShowXScaleVal(TRUE)
, m_bShowYScaleVal(TRUE)
, m_bShowPillarVal(TRUE)
{
	for ( int i = 0; i < 4; i++ )
	{
		m_bShowBorder[i] = FALSE;
	}
	m_clPillarColor.reserve(6);
	for ( int i = 0; i < 6; i++ )
	{
		m_clPillarColor.push_back(k_PillarColor[i]);
	}
	m_cTitleFont.CreateFont(HHG_TITLE_FONT_SIZE,0,0,0,FW_NORMAL,FALSE,0,0,0,0,0,0,0,HHG_FONT_STYLE);
	m_cScaleValFont.CreateFont(HHG_SCALE_FONT_SIZE,0,0,0,FW_NORMAL,FALSE,0,0,0,0,0,0,0,HHG_FONT_STYLE);
	m_cPillarValFont.CreateFont(HHG_PILLARDESC_FONT_SIZE,0,0,0,FW_NORMAL,FALSE,0,0,0,0,0,0,0,HHG_FONT_STYLE);
}

///
/// @brief
///     ��������
///
CHorHistogramCtrl::~CHorHistogramCtrl()
{
	
}

///
/// @brief
///     ����ĳһ��������ɫ����˳��ų�����Χ�����Ĳ���ΪĬ����ɫ
///
/// @param[in]
///     idxSeq - ������˳��ţ������µ��ϵ�˳�򣩣���0��ʼ
/// @param[in]
///     clr    - ��ɫֵ
///
/// @return
///
///
void CHorHistogramCtrl::SetPillarColor(const size_t idxSeq, const COLORREF clr)
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
			if ( i < 6 )
				m_clPillarColor.push_back(k_PillarColor[i]);
			else
				m_clPillarColor.push_back(k_PillarColor[0]);
		}
	}
	else
		m_clPillarColor.at(idxSeq) = clr;
}

///
/// @brief
///     ����ĳһ������Y��߿̶ȣ���˳��ų�����Χ�����Ĳ���Ϊ�մ�
///
/// @param[in]
///     idxSeq   - ������˳��ţ������µ��ϵ�˳�򣩣���0��ʼ
/// @param[in]
///     scaleVal - Y��߿̶�
///
/// @return
///
///
void CHorHistogramCtrl::SetYScaleVal(const size_t idxSeq, const CString &scaleVal)
{
	if ( idxSeq >= m_strYScaleVal.size() )
	{
		for ( size_t i = m_strYScaleVal.size(); i <= idxSeq; i++ )
		{
			if ( i == idxSeq )
			{
				m_strYScaleVal.push_back(scaleVal);
				break;
			}
			// ��������Ĭ��Ϊ�մ�
			m_strYScaleVal.push_back(_T(""));
		}
	}
	else
		m_strYScaleVal.at(idxSeq) = scaleVal;
}

///
/// @brief
///     ����ĳһ��������ʽ����˳��ų�����Χ�����Ĳ�����Ϊ��
///
/// @param[in]
///     idxSeq - ������˳��ţ������µ��ϵ�˳�򣩣���0��ʼ
/// @param[in]
///     pBmp   - ָ��������ʽBmp��ָ��
///
/// @return
///
///
void CHorHistogramCtrl::SetPillarStyle(const size_t idxSeq, CBitmap *pBmp)
{
	if ( idxSeq >= m_vPillarStyleBmp.size() )
	{
		for ( size_t i = m_vPillarStyleBmp.size(); i <= idxSeq; i++ )
		{
			if ( i == idxSeq )
			{
				m_vPillarStyleBmp.push_back(pBmp);
				break;
			}
			// ��������Ĭ��Ϊ��
			m_vPillarStyleBmp.push_back(NULL);
		}
	}
	else
	{
		// ������ʽ
		m_vPillarStyleBmp.at(idxSeq) = pBmp;
	}
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
void CHorHistogramCtrl::SetTitleFont(CFont *pFont)
{
	if ( pFont == NULL || pFont->GetSafeHandle() == NULL )
		return;
	m_cTitleFont.DeleteObject();
	LOGFONT logFont;
	pFont->GetLogFont(&logFont);
	m_cTitleFont.CreateFontIndirect(&logFont);
}

///
/// @brief
///     ���ÿؼ��ϵ�X��Y��ߵĿ̶ȵ�����
///
/// @param[in]
///     pFont - ָ�������ָ��
///
/// @return
///
///
void CHorHistogramCtrl::SetScaleValFont(CFont *pFont)
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
///     ��������ֵ��Ϣ������
///
/// @param[in]
///     pFont - ָ�������ָ��
///
/// @return
///
///
void CHorHistogramCtrl::SetPillarValFont(CFont *pFont)
{
	if ( pFont == NULL || pFont->GetSafeHandle() == NULL )
		return;
	m_cPillarValFont.DeleteObject();
	LOGFONT logFont;
	pFont->GetLogFont(&logFont);
	m_cPillarValFont.CreateFontIndirect(&logFont);
}

///
/// @brief
///     ��ʼ���ؼ�
///
/// @return
///
///
void CHorHistogramCtrl::Init()
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
void CHorHistogramCtrl::ErasePillar()
{
	CRect rt;
	GetClientRect(rt);
	m_cForgndDC.BitBlt(0, 0, rt.Width(), rt.Height(),
		&m_cBkgndDC, 0, 0, SRCCOPY);
}

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
/// @return
///
///
void CHorHistogramCtrl::DrawHorHistoGram(const float *valBuf,const size_t bufSize, const CString &valUnit)
{
	if ( m_vPillarStyleBmp.size() > 0 )
	{
		DrawPillarWithBmp(&m_cForgndDC,m_rtPillarArea,valBuf,bufSize,valUnit);
	}
	else
	{
		DrawPillar(&m_cForgndDC,m_rtPillarArea,valBuf,bufSize,valUnit);
	}
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
void CHorHistogramCtrl::Flush(CDC *pDC)
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

BEGIN_MESSAGE_MAP(CHorHistogramCtrl, CStatic)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

///
/// @brief
///     �ػ���Ϣ������
///
/// @return
///
void CHorHistogramCtrl::OnPaint()
{
// 	PAINTSTRUCT ps;
// 	CDC *pDC = BeginPaint(&ps);
// 	pDC->BitBlt(0, 0, rt.Width(), rt.Height(), m_cForgndDC, 0, 0, SRCCOPY);
// 	EndPaint(&ps);

	CPaintDC dc(this); // device context for painting
	CRect rt;
	GetClientRect(rt);
	dc.BitBlt(0, 0, rt.Width(), rt.Height(), &m_cForgndDC, 0, 0, SRCCOPY);
}

///
/// @brief
///     �ͷſؼ���Դ��Ϣ������
///
/// @return
///
void CHorHistogramCtrl::OnDestroy()
{
	CStatic::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������
	Release();
}

///
/// @brief
///     ����������Ϣ������
///
/// @return
///
///
BOOL CHorHistogramCtrl::OnEraseBkgnd(CDC* pDC)
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
void CHorHistogramCtrl::CreateDCandBmp(const CRect &rt, CDC &dc, CBitmap &bmp, CBitmap **pPOldBmp)
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
void CHorHistogramCtrl::DrawBkgnd( )
{
	CRect rt;
	GetClientRect(rt);

	// ��������DC��λͼ
	CreateDCandBmp(rt,m_cBkgndDC,m_cBkgndBmp,&m_pOldBkgndBmp);

	// �������
    //
	m_cBkgndDC.FillSolidRect(0,0,rt.Width(),rt.Height(),m_clBkgndColor);
   // m_cBkgndDC.FillSolidRect(0,0,rt.Width(),rt.Height(),RGB(255,255,255));

	// ����������ʾ����ľ�����
	UpdatePillarAreaSpace(&m_cBkgndDC);

	// ����������ʾ������ڲ�����
	DrawPillarAreaGrid(&m_cBkgndDC,m_rtPillarArea);

	// ����������ʾ����ı߿�
	DrawPillarAreaBorder(&m_cBkgndDC,m_rtPillarArea);

	// ����X-Y��ߵĿ̶�
	DrawScale(&m_cBkgndDC,m_rtPillarArea);

	// �������Ͻǵı���
	DrawTitle(&m_cBkgndDC,m_rtPillarArea);
}

///
/// @brief
///     ���ÿؼ�ǰ��
///
/// @return
///
void  CHorHistogramCtrl::ResetForgnd()
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
void CHorHistogramCtrl::Release()
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
	m_cPillarValFont.DeleteObject();
}

///
/// @brief
///     ����������ʾ����ľ�����
///
/// @param[in]
///     pDC - �豸����DC��ָ��
///
/// @return
///     
///
void CHorHistogramCtrl::UpdatePillarAreaSpace(CDC *pDC)
{
	CRect rt;
	GetClientRect(rt);
#if ZENG_YUN
	rt.left   = rt.left + m_iLeftMargin;
	rt.top    = rt.top  + m_iTopMargin;
	rt.right  = rt.right - m_iRightMargin;
	rt.bottom = rt.bottom - m_iBottomMargin;
#else
	
#endif

	CFont *pOldFont = pDC->SelectObject(&m_cScaleValFont);
	// ����Y��߿̶ȵ����Size
	CString str;
	CSize xScaleValMaxSz,yScaleValMaxSz;
	str = GetYScaleValMaxStr();
	yScaleValMaxSz = pDC->GetTextExtent(str);
	if ( m_iYScaleValLen != 0 )
		yScaleValMaxSz.cx = m_iYScaleValLen;
	// ����X��߿̶ȵ����Size
	str.Format(_T("%0.1f"),m_fXScaleInterval*m_iXGridCount);
	xScaleValMaxSz = pDC->GetTextExtent(str);
	pDC->SelectObject(pOldFont);

	// ����������ʾ����ľ�����
	if ( m_bShowYScaleVal )
	{
		m_rtPillarArea.left = rt.left + yScaleValMaxSz.cx + 25;
		//m_rtPillarArea.left = rt.left + yScaleValMaxSz.cx + 4 + m_iSpaceFromLeftBorderToPillar;
        m_rtPillarArea.left = rt.left + 85;
		m_rtPillarArea.top  = rt.top  + 35;
		if ( !m_bShowXScaleVal )
		{
			m_rtPillarArea.right  = rt.right;
			m_rtPillarArea.bottom = rt.bottom - yScaleValMaxSz.cy/2;
		}
	}
//	if ( m_bShowXScaleVal )
	{
		m_rtPillarArea.right  = rt.right - xScaleValMaxSz.cx/2;
		m_rtPillarArea.bottom = rt.bottom - xScaleValMaxSz.cy-35;
		if ( !m_bShowYScaleVal )
		{
			m_rtPillarArea.left = rt.left + xScaleValMaxSz.cx/2 + m_iSpaceFromLeftBorderToPillar;
			m_rtPillarArea.top  = rt.top;
		}
	}
	if ( !m_bShowYScaleVal && !m_bShowXScaleVal )
	{
		m_rtPillarArea.left = rt.left + m_iSpaceFromLeftBorderToPillar;
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
void CHorHistogramCtrl::DrawPillarAreaGrid(CDC *pDC, const CRect &pillarArea)
{
	// ���㳣����������������ȼ��߶�
	float horGridWidth = (float)pillarArea.Width()/(float)m_iXGridCount;
	float verGridWidth = (float)pillarArea.Height()/(float)m_iYGridCount;

	CPen  pen;
	pen.CreatePen(HHG_GRID_LINE_STYLE,HHG_GRID_LINE_WIDTH,m_clPillarAreaGridColor);
	CPen *pOldPen = pDC->SelectObject(&pen);
	// ���Ƴ��������������ֱ������
	if ( m_bShowVerGrid )
	{
		int left = pillarArea.left;
		for (int i = 1; i < m_iXGridCount; ++i)
		{
			pDC->MoveTo(int(left+i*horGridWidth),pillarArea.top);
			pDC->LineTo(int(left+i*horGridWidth),pillarArea.bottom);
		}
	}
	// ����ˮƽ������
	if ( m_bShowHorGrid )
	{
		for (int i = 1; i < m_iYGridCount; ++i)
		{
			pDC->MoveTo(pillarArea.left,int(pillarArea.top+verGridWidth*i));
			pDC->LineTo(pillarArea.right,int(pillarArea.top+verGridWidth*i));
		}
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
void CHorHistogramCtrl::DrawPillarAreaBorder(CDC *pDC, const CRect &pillarArea)
{
	CPen pen;
	pen.CreatePen(HHG_BORDER_LINE_STYLE,HHG_BORDER_LINE_WIDTH,m_clPillarAreaBorderColor);
	CPen *pOldPen = pDC->SelectObject(&pen);

	// ��߿���
	if ( m_bShowBorder[0] )
	{
		pDC->MoveTo(pillarArea.left-21,pillarArea.top);
		pDC->LineTo(pillarArea.left-21,pillarArea.bottom);
	}
	
	// �ұ߿���
	if ( m_bShowBorder[1] )
	{
		pDC->MoveTo(pillarArea.right,pillarArea.top);
		pDC->LineTo(pillarArea.right,pillarArea.bottom);
	}

	// �ϱ߿���
	if ( m_bShowBorder[2] )
	{
		pDC->MoveTo(pillarArea.left,pillarArea.top);
		pDC->LineTo(pillarArea.right,pillarArea.top);
	}
	// �±߿���
	//if ( m_bShowBorder[3] )
	{
		pDC->MoveTo(pillarArea.left-20,pillarArea.bottom);
		pDC->LineTo(pillarArea.right-20,pillarArea.bottom);
	}

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
///     pillarArea - ������ʾ����ľ�����
///
/// @return
///     
///
void CHorHistogramCtrl::DrawScale(CDC *pDC, const CRect &pillarArea)
{
	CFont *pOldFont = pDC->SelectObject(&m_cScaleValFont);
	COLORREF oldClr = pDC->SetTextColor(m_clScaleValColor);
	int oldMode = pDC->SetBkMode(TRANSPARENT);
#if  ZENG_YUN
	// ����X�̶�
	if ( m_bShowXScaleVal )
	{
		// ����X��ߵĿ̶�ֵ�ļ��
		float xSpanWidth = (float)pillarArea.Width() / (float)m_iXGridCount;

		// ����X��ߵķֶ�ֵ
		float xInterval = m_fXScaleInterval;
		// ����X��ߵĿ̶�
		for (int i=0; i<=m_iXGridCount; ++i)
		{
			float val = i*xInterval;
			CString str;
			str.Format(L"%d", (int)val);
			CSize scaleValSz = pDC->GetTextExtent(str);
			int left = int(pillarArea.left+i*xSpanWidth)-scaleValSz.cx/2;
			CRect rect(left,pillarArea.bottom,left+scaleValSz.cx,pillarArea.bottom+scaleValSz.cy);
			pDC->DrawText(str, rect, DT_RIGHT);
		}
	}

	if ( m_bShowYScaleVal ) 	// ���Ƹ�������Y�̶�
	{
		// ����Y��ߵĿ̶�ֵ�ļ��
		float ySpanWidth = (float)pillarArea.Height() / (float)m_iPillarCount;
		for (int i=0; i<=m_iPillarCount; ++i)
		{
			if ( i >= (int)m_strYScaleVal.size() )
				break;
			CString str;
			str.Format(L"%s", m_strYScaleVal[i]);
			CSize scaleValSz = pDC->GetTextExtent(str);
			int top = int(pillarArea.bottom-ySpanWidth*i-ySpanWidth/2-scaleValSz.cy/2);
			CRect rect(pillarArea.left-m_iSpaceFromLeftBorderToPillar-scaleValSz.cx-4,top,
				pillarArea.left-m_iSpaceFromLeftBorderToPillar-4,top+scaleValSz.cy);
			pDC->DrawText(str, rect, DT_RIGHT);
		}
	}
#else
	//����Y�̶�
	if(m_bShowYScaleVal){
		//����Y��̶ȼ��
		float ySpanHeight = (float)pillarArea.Height()/2 ;
		float yInterval = 50;
		//���ƿ̶�
		for(int i=0; i < 3; i++){
			float val = i * yInterval;
			CString str;
			str.Format(L"%d",(int)val);
			str = str + _T("%");
			CSize scaleValSz = pDC->GetTextExtent(str);
			int left = pillarArea.left-68;
			int bottom = pillarArea.bottom  - (int)ySpanHeight*i;
			CRect rect(left -44  - scaleValSz.cx  , bottom - scaleValSz.cy/2, left + scaleValSz.cx, bottom + scaleValSz.cy/2);
			pDC->DrawText(str, rect, DT_RIGHT);
		}
	}
	//if(m_bShowXScaleVal)
    {
			float xSpanWidth = (float)pillarArea.Width() / (float)6;
			for(int i = 0; i < 6; i++)
			{
				if(i >= (int)m_strYScaleVal.size())
						break;
				CString str;
				str.Format(L"%s",m_strYScaleVal[i]);
				CSize scalseValSz = pDC->GetTextExtent(str);
				int top = pillarArea.bottom +5;
				int left = pillarArea.left + (int)xSpanWidth * i - 35; 
				CRect rect(left,top,left+scalseValSz.cx, top +scalseValSz.cy);
				pDC->DrawText(str,rect,DT_RIGHT);

#if !CHUN_HUA    //��X��̶�
				pDC->FillSolidRect(left+14,pillarArea.bottom-4,2,4,m_clTitleColor);
#endif
			}
	}

#endif

	pDC->SetTextColor(oldClr);
	pDC->SetBkMode(oldMode);
	pDC->SelectObject(pOldFont);
}

///
/// @brief
///     ���ƿؼ��ı��⣨λ�ڲ�����ʾ��������Ͻǣ�
///
/// @param[in]
///     pDC        - �豸����DC��ָ��
/// @param[in]
///     pillarArea - ������ʾ����ľ�����
///
/// @return
///     
///
void CHorHistogramCtrl::DrawTitle(CDC *pDC, const CRect &pillarArea)
{
	if (m_bShowTitle)
	{
		CFont *pOldFont = pDC->SelectObject(&m_cTitleFont);
		COLORREF oldClr   = pDC->SetTextColor(m_clTitleColor);
		int oldMode  = pDC->SetBkMode(TRANSPARENT);
		CSize sz = pDC->GetTextExtent(m_strTitle);
		pDC->ExtTextOutW(pillarArea.right - sz.cx - 2 + 198,pillarArea.top + 2 +5,
			ETO_OPAQUE,NULL,m_strTitle,m_strTitle.GetLength(),NULL);
		pDC->SetTextColor(oldClr);
		pDC->SetBkMode(oldMode);
		pDC->SelectObject(pOldFont);
	}
}

///
/// @brief
///     �����������Ի���ʽ��
///
/// @param[in]
///     pDC        - �豸����DC��ָ��
/// @param[in]
///     pillarArea - ������ʾ����ľ�����
/// @param[in]
///     valDataBuf - ������������ʾ��ֵ�Ļ���
/// @param[in]
///     bufSize    - �����С
/// @param[in]
///     valUnit    - ��������ʾ��ֵ�ĵ�λ
///
/// @return
///     
///
void CHorHistogramCtrl::DrawPillar(CDC *pDC, const CRect &pillarArea, const float *valDataBuf, const size_t bufSize, const CString &valUnit)
{
	ASSERT(NULL != valDataBuf);

	/*******************************************************************/
#if ZENG_YUN
	// ����X���1�̶ȵĳ��ȡ��������򵥸������Ŀ��õĸ߶Ⱥ�0�̶������ڵ�λ��
	float OneXScaleLen       = (float)pillarArea.Width()/(m_fXScaleInterval*(float)m_iXGridCount);
	float onePillarMaxHeight = (float)pillarArea.Height()/(float)m_iPillarCount;		//��״������Ŀ
	int   zeroScalePos       = pillarArea.left+1;

	for (int i=0;i<m_iPillarCount;i++)
	{
		if ( i >= (int)bufSize )
			break;

		// �������ж�
		float val = valDataBuf[i];
		if ( val < 0.0f || _finite(val) == FALSE )
			val = 0.0f;
		if ( val > m_fXScaleInterval*(float)m_iXGridCount )			// m_fXScaleInterval X�̶�ֵ
			val = m_fXScaleInterval*(float)m_iXGridCount;

		// ����������ɫ
		COLORREF pillarColor;
		if ( i >= (int)m_clPillarColor.size() )
			pillarColor = m_clPillarColor[0];
		else
			pillarColor = m_clPillarColor[i];

		// ��������
		CBrush brush(pillarColor);
		int pillarBetween = m_iPillarBetween;
		if ( (int)(onePillarMaxHeight - m_iPillarHeight) > m_iPillarBetween )		//m_iPillarBetween��״��֮��ļ��
			pillarBetween = (int)(onePillarMaxHeight - m_iPillarHeight);
		float pillarLen = OneXScaleLen*val;
		if ( pillarLen > 0.001f && pillarLen < 1.0f )
			pillarLen = 1.0f;
		CRect rect(int(zeroScalePos),
			       int(pillarArea.bottom+pillarBetween/2-(i+1)*onePillarMaxHeight),
			       int(zeroScalePos+pillarLen),
			       int(pillarArea.bottom-pillarBetween/2-i*onePillarMaxHeight));
		pDC->FillRect(rect,&brush);
		brush.DeleteObject();

		// �������ұ߻�������ֵ��Ϣ
		if ( m_bShowPillarVal )
		{
			CFont *pOldFont = pDC->SelectObject(&m_cPillarValFont);
			COLORREF oldClr   = pDC->SetTextColor(m_clPillarValColor);
			int oldMode  = pDC->SetBkMode(TRANSPARENT);
			CString str;
			str.Format(_T("%0.3f%s"),valDataBuf[i],valUnit);
			CSize sz = pDC->GetTextExtent(str);
			pDC->ExtTextOutW(rect.right + 4,rect.CenterPoint().y - sz.cy/2,
				ETO_OPAQUE,NULL,str,str.GetLength(),NULL);
			pDC->SetTextColor(oldClr);
			pDC->SetBkMode(oldMode);
			pDC->SelectObject(pOldFont);
		}
	}
#else
    int height = pillarArea.Height()/10;
    for(int i = 0; i < 10; i++){
        if(i%2 == 0){
               pDC->FillSolidRect(pillarArea.left-20,pillarArea.top+i*height,pillarArea.Width(),\
                   height,RGB(0x2e,0x2f,0x32));
        }else{
               pDC->FillSolidRect(pillarArea.left-20,pillarArea.top+i*height,pillarArea.Width(),\
                   height,RGB(0x1f,0x1f,0x23));
        }
    }
    
    //pDC->FillSolidRect(pillarArea,RGB(255,255,255));
    //����Y���һ�̶ȵĳ���
    float OneYScaleLen = (float)pillarArea.Height()/(m_fXScaleInterval*(float)m_iXGridCount);
    //��״ͼ�ļ��
    float onePillarMaxWidth = (float)pillarArea.Width()/(float)m_iPillarCount;
    //��״ͼ����ʵλ��
    int zeroScalePos = pillarArea.bottom -1;

    //��ʼ����
    for(int i =0; i<m_iPillarCount; i++){
        if(i >= (int)bufSize)
            break;
        float value = valDataBuf[i];
        if(value < 0.0f || _finite(value) == FALSE)
            value = 0.0f;
        //��������̶ȣ������ֵ��
        if(value > m_fXScaleInterval*(float)m_iXGridCount)
            value = m_fXScaleInterval*(float)m_iXGridCount;
        COLORREF   pillarclr;
        if(i >= (int)m_clPillarColor.size())
            pillarclr = m_clPillarColor[0];
        else
            pillarclr = m_clPillarColor[i];
        //������״ͼ
        CBrush brush(pillarclr);
        int pillarBetween = 64;
        int pillarWidth = 24;
        if((int)(onePillarMaxWidth -pillarWidth) > pillarBetween)
            pillarBetween  = (int)(onePillarMaxWidth - pillarWidth);

        float pillarLen = OneYScaleLen * value;
        if(pillarLen > 0.0001f &&  pillarLen < 1.0f)
            pillarLen = 1.0f;
        CRect rect(int(pillarArea.left + i*onePillarMaxWidth),
                   int(zeroScalePos - pillarLen )-1,
                   int(pillarArea.left + i*onePillarMaxWidth + pillarWidth),
                   int(zeroScalePos ));
        pDC->FillRect(rect,&brush);
        brush.DeleteObject();

        //������״ͼ�ϵ�����
        if(m_bShowPillarVal){
            CPen pPen;
            pPen.CreatePen(PS_SOLID,1,pillarclr);
            CPen *poldPen = pDC->SelectObject(&pPen);
            pDC->MoveTo(rect.CenterPoint().x,rect.top);
            pDC->LineTo(rect.CenterPoint().x,rect.top - 6);
            CFont *pOldFont = pDC->SelectObject(&m_cPillarValFont);
            COLORREF oldClr   = pDC->SetTextColor(pillarclr);
            int oldMode  = pDC->SetBkMode(TRANSPARENT);
            CString str;
            str.Format(_T("%0.3f%s"),valDataBuf[i],valUnit);
            CSize sz = pDC->GetTextExtent(str);
            pDC->ExtTextOutW(rect.right - sz.cx /2 -8 ,rect.top - sz.cy -8,
                ETO_OPAQUE,NULL,str,str.GetLength(),NULL);
            pDC->SelectObject(poldPen);
            pDC->SetTextColor(oldClr);
            pDC->SetBkMode(oldMode);
            pDC->SelectObject(pOldFont);
        }
    }
#endif

}

///
/// @brief
///     ������������ͼ��ʽ��
///
/// @param[in]
///     pDC        - �豸����DC��ָ��
/// @param[in]
///     pillarArea - ������ʾ����ľ�����
/// @param[in]
///     valDataBuf - ������������ʾ��ֵ�Ļ���
/// @param[in]
///     bufSize    - �����С
/// @param[in]
///     valUnit    - ��������ʾ��ֵ�ĵ�λ
///
/// @return
///     
///
void CHorHistogramCtrl::DrawPillarWithBmp(CDC *pDC, const CRect &pillarArea, const float *valDataBuf, const size_t bufSize, const CString &valUnit)
{
	ASSERT(NULL != valDataBuf);

	// ����X���1�̶ȵĳ��ȡ��������򵥸������Ŀ��õĸ߶Ⱥ�0�̶������ڵ�λ��
	float OneXScaleLen       = (float)pillarArea.Width()/(m_fXScaleInterval*(float)m_iXGridCount);
	float onePillarMaxHeight = (float)pillarArea.Height()/(float)m_iPillarCount;
	int   zeroScalePos       = pillarArea.left+1;

	for (int i=0;i<m_iPillarCount;i++)
	{
		if ( i >= (int)bufSize )
			break;

		// �������ж�
		int val = (int)valDataBuf[i];
		if ( val < 0 )
			break;
		if ( _finite(val) == FALSE )
			val = 0;
		if ( val > (int)(m_fXScaleInterval*m_iXGridCount) )
			val = (int)(m_fXScaleInterval*m_iXGridCount);

		// ����������ʽ
		CBitmap *pBmp = NULL;
		if ( i >= (int)m_vPillarStyleBmp.size() )
		{
			if ( m_vPillarStyleBmp[0] != NULL && m_vPillarStyleBmp[0]->GetSafeHandle() != NULL )
				pBmp = m_vPillarStyleBmp[0];
		}
		else
			pBmp = m_vPillarStyleBmp[i];

		// ���������ľ�����
		int pillarBetween = m_iPillarBetween;
		if ( (int)(onePillarMaxHeight - m_iPillarHeight) > m_iPillarBetween )
			pillarBetween = (int)(onePillarMaxHeight - m_iPillarHeight);
		CRect rect(int(zeroScalePos+2),
			int(pillarArea.bottom+pillarBetween/2-(i+1)*onePillarMaxHeight),
			int(zeroScalePos+OneXScaleLen*val),
			int(pillarArea.bottom-pillarBetween/2-i*onePillarMaxHeight));
		// ��������
		if ( pBmp != NULL && pBmp->GetSafeHandle() != NULL )
		{
			CDC memDC;
			memDC.CreateCompatibleDC(pDC);
			CBitmap *pOldBmp = memDC.SelectObject(pBmp);
			BITMAP bmp;
			pBmp->GetBitmap(&bmp);
			TransparentBlt(m_cForgndDC.m_hDC,rect.left,rect.top,rect.Width(),rect.Height(),
				memDC.m_hDC,0,0,bmp.bmWidth,bmp.bmHeight,m_clTransColor);
			memDC.SelectObject(pOldBmp);
			memDC.DeleteDC();
		}

		// �����������������ֵ��Ϣ
		if ( m_bShowPillarVal )
		{
			CFont *pOldFont = pDC->SelectObject(&m_cPillarValFont);
			COLORREF oldClr   = pDC->SetTextColor(m_clPillarValColor);
			int oldMode  = pDC->SetBkMode(TRANSPARENT);
			CString str;
			str.Format(_T("%0.1f%s"),valDataBuf[i],valUnit);
			CSize sz = pDC->GetTextExtent(str);
			pDC->ExtTextOutW(rect.right + 4,rect.CenterPoint().y - sz.cy/2,
				ETO_OPAQUE,NULL,str,str.GetLength(),NULL);
			pDC->SetTextColor(oldClr);
			pDC->SetBkMode(oldMode);
			pDC->SelectObject(pOldFont);
		}
	}
}

///
/// @brief
///     ��ȡY��߿̶ȵ�����ַ���
///
/// @return
///     Y��߿̶ȵ�����ַ���
///
CString CHorHistogramCtrl::GetYScaleValMaxStr()
{
	std::vector<CString>::iterator it;
	CString str(_T(""));
	int sz = 0;
	for( it = m_strYScaleVal.begin(); it != m_strYScaleVal.end(); it++ )
	{
		str = sz > (*it).GetLength() ? str : (*it);
		sz  = str.GetLength();
	}
	return str;
}
