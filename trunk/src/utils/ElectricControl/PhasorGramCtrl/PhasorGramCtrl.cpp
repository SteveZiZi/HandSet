/// @file
///     PhasorGramCtrl.cpp
///
/// @brief
///     ����ͼ�ؼ�
///
/// @note
///     ������ͼΪ������ʾ
///     �ؼ�����λ�ڿؼ������Ͻ�
///     ����������Ϣλ���������յ㴦
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
///    |2013.05.09     |1.0.0.509  |�޸ĵ���������С��10���أ����������Բ���ʾ              |
///    |2013.05.09     |1.0.0.509  |�޸ĸ�����������Ϣ���ص��жϺʹ���                        |
///    |2013.07.05     |1.0.1.705  |���������ȵ��α�̶ȵĻ��������ؼ���ǰ������              |
///    |2013.07.09     |1.0.1.709  |��Ӷ��������ȵ��������ж�                                |
///    |2013.07.16     |1.0.1.716  |���������ȵ��α�̶ȵĻ��Ƹ��ڲ���ǰ��ʱ���»���          |
///    |2013.08.07     |1.0.1.807  |�޸��������ýӿ��ж���ε���Ч���ж�                      |
///    |2013.11.19     |1.0.1.1119 |�޸ĸ�����������Ϣ���ص��жϼ�����                        |
///
#include "stdafx.h"
#include <math.h>
#include <float.h>
#include <atltypes.h>
#include "PhasorGramCtrl.h"

///
/// @brief
///     �ؼ��ı�����ɫ
///
#define PSG_CTRL_BKGND_COLOR      RGB(0,0,0)           ///< ��ɫ

///
/// @brief
///     �����Ĭ�����ã�����������ʽ�Լ����⡢�����̶Ⱥ�����������Ϣ�������С����ɫ
///
#define PSG_FONT_STYLE            _T("΢���ź�")
#define PSG_TITLE_FONT_SIZE       20
/***************************************************************************************/
#define PSG_TITLE_FONT_COLOR      RGB(0xbe,0xbd,0xc0)     ///< ��ɫ
#define PSG_SCALE_FONT_SIZE       16

#define PSG_SCALE_FONT_COLOR      RGB(0xbe,0xbd,0xc0)     ///< ů��ɫ
//#define PSG_SCALE_FONT_COLOR      RGB(128,128,135)     ///< ů��ɫ
#define PSG_PHASORDESC_FONT_SIZE  20

///
/// @brief
///     ������ʾ�����Ĭ�����ã���Բ����Բ��Բ�뾶�����������͡���Ⱥ���ɫ�Լ�Բ�ļ��
///
/****************************************************************************************/
#define PSG_EXCIRCLE_LINE_STYLE PS_SOLID
#define PSG_EXCIRCLE_LINE_WIDTH 2
#define PSG_EXCIRCLE_LINE_COLOR RGB(0xae,0xae,0xae)         ///< ů��ɫ
#define PSG_INCIRCLE_LINE_STYLE PS_SOLID
#define PSG_INCIRCLE_LINE_WIDTH 1
#define PSG_INCIRCLE_LINE_COLOR RGB(0xae,0xae,0xae)         ///< ���ɫ
#define PSG_RADIUS_LINE_STYLE   PS_DASH
#define PSG_RADIUS_LINE_WIDTH   1
#define PSG_RADIUS_LINE_COLOR   RGB(0xae,0xae,0xae)         ///< ���ɫ

///
/// @brief
///     ������ʾ����Բ������
///
#define PSG_CIRCLE_COUNT   4

///
/// @brief
///     ������ʾ������������Σ�������ͬ������
///
#define PSG_HALF_SECTOR_COUNT   6

///
/// @brief
///     ����������Ĭ�����ã����������͡���Ⱥ�ǰ6����Ĭ����ɫ(�졢�ơ�ɭ���̡��ɺ졢�ȡ�����)
///
#define PSG_PHASOR_LINE_STYLE   PS_SOLID
#define PSG_PHASOR_LINE_WIDTH   2
static const COLORREF k_PhasorColor[6] = {RGB(0xfc,0x01,0x01),  RGB(0x56,0xe1,0x22),RGB(0xf8,0xe6,0x01),
                                            RGB(220,20,60),RGB(255,165,0),RGB(124,252,0)};

#ifndef PI
#define PI 3.1415926
#endif

IMPLEMENT_DYNAMIC(CPhasorGramCtrl, CStatic)

///
/// @brief
///     ���캯��
///
CPhasorGramCtrl::CPhasorGramCtrl()
: m_pOldBkgndBmp(NULL)
, m_pOldForgndBmp(NULL)
, m_clBkgndColor(PSG_CTRL_BKGND_COLOR)
, m_clTitleColor(PSG_TITLE_FONT_COLOR)
, m_clScaleValColor(PSG_SCALE_FONT_COLOR)
, m_clRadiusColor(PSG_RADIUS_LINE_COLOR)
, m_clExCircleColor(PSG_EXCIRCLE_LINE_COLOR)
, m_clInCircleColor(PSG_INCIRCLE_LINE_COLOR)
, m_strTitle(_T(""))
, m_iCircleCount(PSG_CIRCLE_COUNT)
, m_iHalfSectorCount(PSG_HALF_SECTOR_COUNT)
, m_fLenScaleInterval(10.0f)
, m_iLeftMargin(4)
, m_iRightMargin(4)
, m_iBottomMargin(4)
, m_iTopMargin(4)
, m_bShowTitle(FALSE)
, m_bShowDegreeScaleVal(TRUE)
, m_bShowLenScaleVal(TRUE)
, m_bShowPhasorDesc(TRUE)
{
    m_clPhasorColor.reserve(6);
	for ( int i = 0; i < 6; i++ )
	{
		m_clPhasorColor.push_back(k_PhasorColor[i]);
	}
	m_cTitleFont.CreateFont(PSG_TITLE_FONT_SIZE,0,0,0,FW_NORMAL,FALSE,0,0,0,0,0,0,0,PSG_FONT_STYLE);
	m_cScaleValFont.CreateFont(PSG_SCALE_FONT_SIZE,0,0,0,FW_NORMAL,FALSE,0,0,0,0,0,0,0,PSG_FONT_STYLE);
	m_cPhasorDescFont.CreateFont(PSG_PHASORDESC_FONT_SIZE,0,0,0,FW_NORMAL,FALSE,0,0,0,0,0,0,0,PSG_FONT_STYLE);
	m_cPhasorExtDescFont.CreateFont(PSG_PHASORDESC_FONT_SIZE,0,0,0,FW_NORMAL,FALSE,0,0,0,0,0,0,0,PSG_FONT_STYLE);
}

///
/// @brief
///     ��������
///
CPhasorGramCtrl::~CPhasorGramCtrl()
{
	
}

///
/// @brief
///     ��������ͼ��ĳһ˳��ŵ���������ɫ
///
/// @param[in]
///     idxSeq - ���ε�˳��ţ���0��ʼ
/// @param[in]
///     clr    - ��ɫֵ
///
/// @return
///
///
void CPhasorGramCtrl::SetPhasorColor(const size_t idxSeq, const COLORREF clr)
{
	if ( idxSeq >= m_clPhasorColor.size() )
	{
		for ( size_t i = m_clPhasorColor.size(); i <= idxSeq; i++ )
		{
			if ( i == idxSeq )
			{
				m_clPhasorColor.push_back(clr);
				break;
			}
			if ( i < 6 )
				m_clPhasorColor.push_back(k_PhasorColor[i]);
			else
				m_clPhasorColor.push_back(k_PhasorColor[0]);
		}
	}
	else
		m_clPhasorColor.at(idxSeq) = clr;
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
void CPhasorGramCtrl::SetTitleFont(CFont *pFont)
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
///     ���ÿؼ���߿̶ȵ�����
///
/// @param[in]
///     pFont - ָ�������ָ��
///
/// @return
///
///
void CPhasorGramCtrl::SetScaleValFont(CFont *pFont)
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
///     ���ÿؼ�����������Ϣ������
///
/// @param[in]
///     pFont - ָ�������ָ��
///
/// @return
///
///
void CPhasorGramCtrl::SetPhasorDescFont(CFont *pFont)
{
	if ( pFont == NULL || pFont->GetSafeHandle() == NULL )
		return;
	m_cPhasorDescFont.DeleteObject();
	LOGFONT logFont;
	pFont->GetLogFont(&logFont);
	m_cPhasorDescFont.CreateFontIndirect(&logFont);
}

///
/// @brief
///     ���ÿؼ���������չ������Ϣ������
///
/// @param[in]
///     pFont - ָ�������ָ��
///
/// @return
///
///
void CPhasorGramCtrl::SetPhasorExtDescFont(CFont *pFont)
{
	if ( pFont == NULL || pFont->GetSafeHandle() == NULL )
		return;
	m_cPhasorExtDescFont.DeleteObject();
	LOGFONT logFont;
	pFont->GetLogFont(&logFont);
	m_cPhasorExtDescFont.CreateFontIndirect(&logFont);
}

///
/// @brief
///     ��ʼ���ؼ�
///
/// @return
///
///
void CPhasorGramCtrl::Init()
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
void CPhasorGramCtrl::ErasePhasor()
{
	CRect rt;
	GetClientRect(rt);
	m_cForgndDC.BitBlt(0, 0, rt.Width(), rt.Height(),
		&m_cBkgndDC, 0, 0, SRCCOPY);

	// ������ͼ�ı��
	DrawScale(&m_cForgndDC);
}

///
/// @brief
///     ��������������ʽ����ǰ����Ƶ�����������ʾ��
///
/// @param[in]
///     lenBuf    - ������������ֵ��Ϣ�Ļ���
/// @param[in]
///     degreeBuf - ���������Ƕ�ֵ��Ϣ�Ļ���
/// @param[in]
///     descBuf   - ����������������Ϣ�Ļ���
/// @param[in]
///     bufSize   - �����С
///
/// @return
///
///
void CPhasorGramCtrl::DrawPhasor(const float *lenBuf, const float *degreeBuf, const CString *descBuf, const size_t bufSize)
{
	CPoint circleCentre(m_rtPhasorArea.left + m_rtPhasorArea.Width()/2 ,
		m_rtPhasorArea.top + m_rtPhasorArea.Height()/2);
	CPoint *endPt = new CPoint[bufSize];
//	CPoint endPt[6];
	// ����뾶��ÿ����Ĳ���
	float fStepWidth = (float)m_rtPhasorArea.Width()/2.0f/(float)PSG_CIRCLE_COUNT/m_fLenScaleInterval;

	// ���������Ķ˵㼰��������
	for (size_t i=0; i<bufSize; i++)
	{
		// ���������������ж�
		float phaseLen = lenBuf[i];
		if ( _finite(lenBuf[i]) == FALSE || lenBuf[i] < 0.0f )
			phaseLen = 0;
		if ( lenBuf[i] > (float)PSG_CIRCLE_COUNT*m_fLenScaleInterval )
			phaseLen = (float)PSG_CIRCLE_COUNT*m_fLenScaleInterval;

		// ���������յ�
		float phaseDegree = degreeBuf[i];
		if ( _finite(degreeBuf[i]) == FALSE )
			phaseDegree = 0.0f;
		float x = phaseLen*fStepWidth*(float)cos(phaseDegree*2*PI/360.0);
		float y = phaseLen*fStepWidth*(float)sin(phaseDegree*2*PI/360.0);
		endPt[i].SetPoint(circleCentre.x+(int)x,circleCentre.y-(int)y);

		if ( phaseLen*fStepWidth >= 10.0f )
		{
			DrawPhasorArrows(&m_cForgndDC,circleCentre,endPt[i],i);
		}
		else
		{// ��������С��10���أ�����Բ���ʾ
			CBrush brush,*oldbrush;
			brush.CreateSolidBrush(m_clPhasorColor[i]);
			oldbrush=m_cForgndDC.SelectObject(&brush);
			CRect rect(0,0,10,10);
			rect.OffsetRect(circleCentre.x-5,circleCentre.y-5);
			m_cForgndDC.Ellipse(rect);
			m_cForgndDC.SelectObject(oldbrush);
		}
	}
	DrawPhasorDesc(&m_cForgndDC,endPt,descBuf,bufSize);

	delete[] endPt;
}

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
void CPhasorGramCtrl::DrawExtDesc(const CPoint leftTop, const COLORREF descClr, const CString *descBuf, const size_t bufSize)
{
	CFont *pOldFont = m_cForgndDC.SelectObject(&m_cPhasorExtDescFont);
	COLORREF oldClr = m_cForgndDC.SetTextColor(descClr);
	int oldMode  = m_cForgndDC.SetBkMode(TRANSPARENT);
	CSize sz = m_cForgndDC.GetTextExtent(descBuf[0]);
	for ( size_t i = 0; i < bufSize; i++ )
	{
		m_cForgndDC.ExtTextOutW(leftTop.x,leftTop.y+(int)i*sz.cy,
			ETO_OPAQUE,NULL,descBuf[i],descBuf[i].GetLength(),NULL);
	}
	m_cForgndDC.SetTextColor(oldClr);
	m_cForgndDC.SetBkMode(oldMode);
	m_cForgndDC.SelectObject(pOldFont);
}


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
void CPhasorGramCtrl::DrawExtDesc( CString csDesc, CRect rcRect, const COLORREF crColor, UINT nFormat /* = DT_VCENTER | DT_LEFT | DT_SINGLELINE */)
{
    CFont *pOldFont = m_cForgndDC.SelectObject(&m_cPhasorExtDescFont);
    COLORREF oldClr = m_cForgndDC.SetTextColor(crColor);
    int oldMode     = m_cForgndDC.SetBkMode(TRANSPARENT);

    m_cForgndDC.DrawText(csDesc, &rcRect, nFormat);

    m_cForgndDC.SetTextColor(oldClr);
    m_cForgndDC.SetBkMode(oldMode);
    m_cForgndDC.SelectObject(pOldFont);
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
void CPhasorGramCtrl::Flush(CDC *pDC)
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

BEGIN_MESSAGE_MAP(CPhasorGramCtrl, CStatic)
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
void CPhasorGramCtrl::OnPaint()
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
void CPhasorGramCtrl::OnDestroy()
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
BOOL CPhasorGramCtrl::OnEraseBkgnd(CDC* pDC)
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
void CPhasorGramCtrl::CreateDCandBmp(const CRect &rt, CDC &dc, CBitmap &bmp, CBitmap **pPOldBmp)
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
void CPhasorGramCtrl::DrawBkgnd( )
{
	CRect rt;
	GetClientRect(rt);

	// ��������DC��λͼ
	CreateDCandBmp(rt,m_cBkgndDC,m_cBkgndBmp,&m_pOldBkgndBmp);

	// �������
	m_cBkgndDC.FillSolidRect(0,0,rt.Width(),rt.Height(),m_clBkgndColor);

	// ��������ͼ�Ļ�ͼ��Χ
	UpdatePhasorAreaRange(&m_cBkgndDC);

	// ������ͼ��ԲȦ
	DrawCircle(&m_cBkgndDC);

	// ������ͼ�İ뾶
	DrawRadius(&m_cBkgndDC);

// 	// ������ͼ�ı��
// 	DrawScale(&m_cBkgndDC);

	// ���ؼ�����
	DrawTitle(&m_cBkgndDC);
}

///
/// @brief
///     ���ÿؼ�ǰ��
///
/// @return
///
void  CPhasorGramCtrl::ResetForgnd()
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
void CPhasorGramCtrl::Release()
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
	m_cPhasorDescFont.DeleteObject();
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
void CPhasorGramCtrl::UpdatePhasorAreaRange(CDC *pDC)
{
	CRect rt;
	GetClientRect(rt);
	if ( m_bShowDegreeScaleVal )
	{
		CFont *pOldFont = pDC->SelectObject(&m_cScaleValFont);
		CSize scaleValMaxSz = pDC->GetTextExtent(L"180.0��");
		pDC->SelectObject(pOldFont);

		m_rtPhasorArea.SetRect(
			rt.left + m_iLeftMargin + scaleValMaxSz.cx,
			rt.top + m_iTopMargin + scaleValMaxSz.cy,
			rt.right - m_iRightMargin - scaleValMaxSz.cx,
			rt.bottom - m_iBottomMargin - scaleValMaxSz.cy
			);

		if ( m_rtPhasorArea.Width() > m_rtPhasorArea.Height() )
		{
			int space = ( m_rtPhasorArea.Width() - m_rtPhasorArea.Height() ) / 2;
			m_rtPhasorArea.SetRect(
				m_rtPhasorArea.left + space,
				m_rtPhasorArea.top,
				m_rtPhasorArea.right - space,
				m_rtPhasorArea.bottom);
		}
		else
		{
			int space = ( m_rtPhasorArea.Height() - m_rtPhasorArea.Width() ) / 2;
			m_rtPhasorArea.SetRect(
				m_rtPhasorArea.left,
				m_rtPhasorArea.top + space,
				m_rtPhasorArea.right,
				m_rtPhasorArea.bottom - space);
		}
	}
	else
	{
		m_rtPhasorArea.SetRect(
			rt.left + m_iLeftMargin,
			rt.top + m_iTopMargin,
			rt.right - m_iRightMargin,
			rt.bottom - m_iBottomMargin
			);
	}
}

///
/// @brief
///     ����������ʾ�����ԲȦ�����ȸ���������ʾ����ľ�����
///
/// @param[in]
///     pDC - �豸����DC��ָ��
///
/// @return
///     
///
void CPhasorGramCtrl::DrawCircle(CDC *pDC)
{
/*
	CBrush brush, *pOldBrush;
	CPen pen, *pOldPen;
	brush.CreateSolidBrush(m_clBkgndColor);
	pOldBrush = pDC->SelectObject(&brush);

	//����Բ
	pen.CreatePen(PSG_EXCIRCLE_LINE_STYLE,PSG_EXCIRCLE_LINE_WIDTH,m_clExCircleColor);
	pOldPen = pDC->SelectObject(&pen);
	pDC->Ellipse(&m_rtPhasorArea);
	pen.DeleteObject();

	//����Բ
	pen.CreatePen(PSG_INCIRCLE_LINE_STYLE,PSG_INCIRCLE_LINE_WIDTH,m_clInCircleColor);
	pDC->SelectObject(&pen);
	int circleSpace = m_rtPhasorArea.Width()/PSG_CIRCLE_COUNT/2;
	for ( int i = 1; i < PSG_CIRCLE_COUNT; i++ )
	{
		CRect rt(m_rtPhasorArea.left+circleSpace*i,m_rtPhasorArea.top+circleSpace*i,
			     m_rtPhasorArea.right-circleSpace*i,m_rtPhasorArea.bottom-circleSpace*i);
		pDC->Ellipse(&rt);
	}

	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	pen.DeleteObject();
	brush.DeleteObject();
*/
    CBrush brush, *pOldBrush;
    CPen pen, *pOldPen;

    //���Ʊ���
    brush.CreateSolidBrush(m_clBkgndColor);
    pOldBrush = pDC->SelectObject(&brush);

    CBrush bkBrush(PSG_CTRL_BKGND_COLOR);
    pOldBrush = pDC->SelectObject(&bkBrush);
    //����Բ
    pen.CreatePen(PSG_EXCIRCLE_LINE_STYLE,PSG_EXCIRCLE_LINE_WIDTH,m_clExCircleColor);
    pOldPen = pDC->SelectObject(&pen);
    pDC->Ellipse(&m_rtPhasorArea);
    pen.DeleteObject();


    //����Բ
    pen.CreatePen(PSG_INCIRCLE_LINE_STYLE,PSG_INCIRCLE_LINE_WIDTH,m_clInCircleColor);
    pDC->SelectObject(&pen);
    int circleSpace = m_rtPhasorArea.Width()/PSG_CIRCLE_COUNT/2;
    for ( int i = 1; i < PSG_CIRCLE_COUNT; i++ )
    {
        CRect rt(m_rtPhasorArea.left+circleSpace*i,m_rtPhasorArea.top+circleSpace*i,
            m_rtPhasorArea.right-circleSpace*i,m_rtPhasorArea.bottom-circleSpace*i);
        pDC->Ellipse(&rt);
    }

    pDC->SelectObject(pOldPen);
    pDC->SelectObject(pOldBrush);
    pen.DeleteObject();
    brush.DeleteObject();

}

///
/// @brief
///     ����������ʾ����İ뾶�����ȸ���������ʾ����ľ�����
///
/// @param[in]
///     pDC - �豸����DC��ָ��
///
/// @return
///     
///
void CPhasorGramCtrl::DrawRadius(CDC *pDC)
{
	CPoint circleCentre(m_rtPhasorArea.left + m_rtPhasorArea.Width()/2 ,
		                m_rtPhasorArea.top + m_rtPhasorArea.Height()/2);
	int radiuLen = m_rtPhasorArea.Width()/2;
	CPen pen, *pOldPen;
	pen.CreatePen(PSG_RADIUS_LINE_STYLE,PSG_RADIUS_LINE_WIDTH,m_clRadiusColor);
	pOldPen = pDC->SelectObject(&pen);
	for ( int i = 0; i < PSG_HALF_SECTOR_COUNT*2; i++ )
	{
		CPoint pt(int(circleCentre.x+radiuLen*cos(180/PSG_HALF_SECTOR_COUNT*i*2*PI/360.0)),
			      int(circleCentre.y-radiuLen*sin(180/PSG_HALF_SECTOR_COUNT*i*2*PI/360.0)));
		pDC->MoveTo(circleCentre);
		pDC->LineTo(pt);
	}

	m_cBkgndDC.SelectObject(pOldPen);
	pen.DeleteObject();
}

///
/// @brief
///     ����������ʾ����ı�߿̶ȣ����ȸ���������ʾ����ľ�����
///
/// @param[in]
///     pDC - �豸����DC��ָ��
///
/// @return
///     
///
void CPhasorGramCtrl::DrawScale(CDC *pDC)
{
	CFont *pOldFont = pDC->SelectObject(&m_cScaleValFont);
	COLORREF oldClr = pDC->SetTextColor(m_clScaleValColor);
	int oldMode = pDC->SetBkMode(TRANSPARENT);

	CPoint circleCentre(m_rtPhasorArea.left + m_rtPhasorArea.Width()/2 ,
		m_rtPhasorArea.top + m_rtPhasorArea.Height()/2);
	int radiuLen = m_rtPhasorArea.Width()/2;
	// ���������Ƕȱ��
	if ( m_bShowDegreeScaleVal )
	{
		for ( int i = 0; i < PSG_HALF_SECTOR_COUNT*2; i++ )
		{
            /**************************************************************/
            float val = float(180/PSG_HALF_SECTOR_COUNT*i);
            CPoint pt(int(circleCentre.x+radiuLen*cos(val*2*PI/360.0)),
                int(circleCentre.y-radiuLen*sin(val*2*PI/360.0)));
            CString str;

            //str.Format(L"%0.1f��", val);
            str.Format(L"%0.0f��", val);

            CSize scaleValSz = pDC->GetTextExtent(str);
            int left = 0;
            if ( pt.x == circleCentre.x || abs(pt.x - circleCentre.x) < 2 )
                left = pt.x - scaleValSz.cx/2;
            else if ( pt.x > circleCentre.x )
                //left = pt.x;
                left = pt.x+5;
            else
                //left = pt.x - scaleValSz.cx;
                left = pt.x - scaleValSz.cx-5;
            int top = 0;
            if ( pt.y == circleCentre.y || abs(pt.y - circleCentre.y) < 2 )
                top = pt.y - scaleValSz.cy/2;
            else if ( pt.y > circleCentre.y )
                //top = pt.y;
                top = pt.y+5;
            else
                //top = pt.y - scaleValSz.cy;
                top = pt.y - scaleValSz.cy-5;
            CRect rect(left,top,left+scaleValSz.cx,top+scaleValSz.cy);
            pDC->DrawText(str, rect, DT_RIGHT);
		}
	}

	// �����������ȱ��
	if ( m_bShowLenScaleVal )
	{
		for ( int i = 0; i < PSG_CIRCLE_COUNT; i++ )
		{
            /********************************************************/
            int istrTop;
            float val = float(i*m_fLenScaleInterval);
            CString str;
            str.Format(L"%0.1f", val);
            int left = int(circleCentre.x + i*radiuLen/PSG_CIRCLE_COUNT);
            int top  = circleCentre.y + 1;
            CSize scaleValSz = pDC->GetTextExtent(str);

            pDC->SetTextColor(RGB(255,255,255));

            //��������λ��
            if(i%2){
                istrTop = top;
            }else{
                istrTop = top - 20;
            }

            CRect rect(left,istrTop,left+scaleValSz.cx,top+scaleValSz.cy);

            pDC->DrawText(str, rect, DT_RIGHT);
		}
	}

	pDC->SetTextColor(oldClr);
	pDC->SetBkMode(oldMode);
	pDC->SelectObject(pOldFont);
}

///
/// @brief
///     ���ƿؼ��ı��⣨λ�ڿؼ������Ͻǣ�
///
/// @param[in]
///     pDC - �豸����DC��ָ��
///
/// @return
///     
///
void CPhasorGramCtrl::DrawTitle(CDC *pDC)
{
	CRect rt;
	GetClientRect(rt);

	if (m_bShowTitle)
	{
		CFont *pOldFont = pDC->SelectObject(&m_cTitleFont);
		COLORREF oldClr   = pDC->SetTextColor(m_clTitleColor);
		int oldMode  = pDC->SetBkMode(TRANSPARENT);
		CSize sz = pDC->GetTextExtent(m_strTitle);
		pDC->ExtTextOutW(rt.Width()- sz.cx - m_iRightMargin - 2,m_iTopMargin + 2,
			ETO_OPAQUE,NULL,m_strTitle,m_strTitle.GetLength(),NULL);
		pDC->SetTextColor(oldClr);
		pDC->SetBkMode(oldMode);
		pDC->SelectObject(pOldFont);
	}
}

///
/// @brief
///     ���������ļ�ͷ
///
/// @param[in]
///     pDC     - �豸����DC��ָ��
/// @param[in]
///     beginPt - ���������
/// @param[in]
///     endPt   - �������յ�
/// @param[in]
///     idxSeq  - ������˳���
///
/// @return
///     
///
void CPhasorGramCtrl::DrawPhasorArrows(CDC *pDC, const CPoint &beginPt, const CPoint &endPt, const size_t idxSeq)
{
	//�������߶�
	CPen  pen, *pOldPen;
	pen.CreatePen(PSG_PHASOR_LINE_STYLE,PSG_PHASOR_LINE_WIDTH,m_clPhasorColor[idxSeq]);
	pOldPen = pDC->SelectObject(&pen);
	pDC->MoveTo(beginPt);
	pDC->LineTo(endPt);

	// ��������ĩ�˼�ͷ�Ķ˵�ֵ
	double angle , cosVal , sinVal;
	double arrowsLen = 10.0;
	angle  = atan2( (beginPt.y-endPt.y)*1.0f,(beginPt.x-endPt.x)*1.0f );
	cosVal = cos( angle );
	sinVal = sin( angle );
	CPoint arrowsPt[4];
	arrowsPt[0].SetPoint(endPt.x,endPt.y);
	arrowsPt[1].SetPoint(endPt.x + int( arrowsLen * cosVal - ( arrowsLen/2.0 * sinVal ) ),
		                 endPt.y + int( arrowsLen * sinVal + ( arrowsLen/2.0 * cosVal ) ));
	arrowsPt[2].SetPoint(endPt.x + int( arrowsLen/2.0 * cosVal ), endPt.y + int( arrowsLen/2.0 * sinVal ));
	arrowsPt[3].SetPoint(endPt.x + int( arrowsLen * cosVal + arrowsLen/2.0 * sinVal ),
		                 endPt.y + int( arrowsLen * sinVal - arrowsLen/2.0 * cosVal ));

	// ������ĩ�˵ļ�ͷ
	CBrush brush(m_clPhasorColor[idxSeq]);
	CBrush *pOldBrush = pDC->SelectObject(&brush);
	pDC->Polygon(arrowsPt,4);
	pDC->SelectObject(pOldBrush);
	brush.DeleteObject();

	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
}

///
/// @brief
///     ����������������Ϣ��λ���������յ㴦��
///
/// @param[in]
///     pDC    - �豸����DC��ָ��
/// @param[in]
///     endPt  - �������յ�
/// @param[in]
///     desc   - ������������Ϣ
/// @param[in]
///     idxSeq - ������˳���
///
/// @return
///     
///
void CPhasorGramCtrl::DrawPhasorDesc(CDC *pDC,const CPoint *endPt, const CString *desc, const size_t bufSize)
{
	CFont *pOldFont = pDC->SelectObject(&m_cPhasorDescFont);
	COLORREF oldClr = pDC->SetTextColor(m_clPhasorColor[0]);
	int oldMode = pDC->SetBkMode(TRANSPARENT);

	// ����������Ϣ�ľ�����
	CRect *descRt = new CRect[bufSize];
	for ( size_t i = 0; i < bufSize; i++ )
	{
		CSize sz = pDC->GetTextExtent(desc[i]);
		CPoint pt;
		if ( endPt[i].x > (m_rtPhasorArea.Width()/2+m_rtPhasorArea.left) )
			pt.x = endPt[i].x;
		else
			pt.x = endPt[i].x - sz.cx;
		if ( endPt[i].y > (m_rtPhasorArea.Height()/2+m_rtPhasorArea.top) )
			pt.y = endPt[i].y;
		else
			pt.y = endPt[i].y - sz.cy;
		descRt[i].SetRect(pt.x,pt.y,pt.x+sz.cx,pt.y+sz.cy);
	}

	// ���ݾ���������Ͻǵĵ�����ֱ�X�����������
	size_t *xSortIdx = new size_t[bufSize];
	for ( size_t i = 0; i < bufSize; i++ )
	{
		xSortIdx[i] = i;
	}
	bool flag = true;
	for ( size_t i = 0; i < bufSize; i++ )
	{
		flag = false;
		for ( size_t j = bufSize-1; j > i; j-- )
		{
			if ( descRt[xSortIdx[j-1]].left > descRt[xSortIdx[j]].left )
			{
				std::swap(xSortIdx[j-1],xSortIdx[j]);
				flag = true;
			}
		}
	}

	// �����������Ϣ��������X�����Ͽ�������ӽ��м�ľ��ε�����
	int  xMidIdx = (int)bufSize - 1;
	for ( int i = 0; i < (int)bufSize; i++ )
	{
		if ( descRt[xSortIdx[i]].left >= (m_rtPhasorArea.Width()/2+m_rtPhasorArea.left) )
		{
			xMidIdx = i-1;
			break;
		}
	}

	// ��벿�ֵľ�������ص�����
	for ( int i = xMidIdx-1; i >= 0; i-- )
	{
		for ( int j = xMidIdx; j > i; j-- )
		{
			if ( IsIntersect(descRt[xSortIdx[i]],descRt[xSortIdx[j]]) )
			{
				int x = descRt[xSortIdx[j]].left-descRt[xSortIdx[i]].Width()-2;
				if ( x >= 0 )
					descRt[xSortIdx[i]].MoveToX(x);
				else
					descRt[xSortIdx[i]].MoveToY(descRt[xSortIdx[i+1]].top-descRt[xSortIdx[xMidIdx]].Height());
			}
		}
	}
	// �Ұ벿�ֵľ�������ص�����
	for ( int i = xMidIdx+1; i<(int)bufSize && i>=0; i++ )
	{
		for ( int j = 0; j<=i-1; j++ )
		{
			if ( IsIntersect(descRt[xSortIdx[i]],descRt[xSortIdx[j]]) )
			{
				int x = descRt[xSortIdx[j]].right+2;
				if ( x+descRt[xSortIdx[i]].Width() <= m_rtPhasorArea.right+m_iRightMargin )
					descRt[xSortIdx[i]].MoveToX(x);
				else
					descRt[xSortIdx[i]].MoveToY(descRt[xSortIdx[i-1]].bottom);
			}
		}
	}

	// ����������Ϣ
	for ( size_t i = 0; i < bufSize; i++ )
	{
		pDC->SetTextColor(m_clPhasorColor[i]);
		pDC->ExtTextOut(descRt[i].left,descRt[i].top,ETO_OPAQUE,NULL,desc[i],desc[i].GetLength(),NULL);
	}

	pDC->SetBkMode(oldMode);
	pDC->SetTextColor(oldClr);
	pDC->SelectObject(pOldFont);

	delete[] xSortIdx;
	delete[] descRt;
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
bool CPhasorGramCtrl::IsIntersect(const CRect &rt1,const CRect &rt2)
{
	if(rt1.right <= rt2.left) return false;
	if(rt1.left >= rt2.right ) return false;
	if(rt1.bottom <= rt2.top ) return false;
	if(rt1.top >= rt2.bottom ) return false;

	return true;
}
