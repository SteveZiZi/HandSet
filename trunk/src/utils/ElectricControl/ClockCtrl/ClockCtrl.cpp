/// @file
///     ClockCtrl.cpp
///
/// @brief
///     ʱ�ӿؼ�
///
/// @note
///     ��ʱ�ӵ�ʱ�롢���������ĳ�ʼλ��ָ��12���ӷ���
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
///    |               |           |                                                          |
///
#include "stdafx.h"
#include <math.h>
#include "ClockCtrl.h"

///
/// @brief
///     ʱ�ӿؼ��ı���ɫ
///
#define CLOCK_BKGND_COLOR      RGB(0,0,0)        ///< ��ɫ

///
/// @brief
///     �Ի��ʱ�ӱ����ϵĿ̶ȵ�Ĭ����ɫ
///
#define CLOCK_DIAL_SCALE_COLOR RGB(0,0,255)      ///< ��ɫ

///
/// @brief
///     �Ի��ʱ��ָ���Ĭ����ɫ
///
#define CLOCK_HAND_COLOR       RGB(30,144,255)   ///< ������

///
/// @brief
///     �ؼ���ͼ��͸������Ĭ����ɫ
///
#define CLOCK_CTRL_DEFAULT_TRANS_COLOR RGB(0x01,0x02,0x03)

///
/// @brief
///     ʱ�ӵ�ʱ�롢�����������ʱ�Ӱ뾶�ı���
///
#define CLOCK_HOUR_HAND_RATIO  0.5f
#define CLOCK_MIN_HAND_RATIO   0.75f
#define CLOCK_SEC_HAND_RATIO   0.9f

#ifndef PI
#define PI 3.14159265
#endif

///
/// @brief
///     ���캯��
///
CClockCtrl::CClockCtrl()
: m_pOldBkgndBmp(NULL)
, m_pOldForgndBmp(NULL)
, m_pDialBmp(NULL)
, m_pHourHandBmp(NULL)
, m_pMinHandBmp(NULL)
, m_pSecHandBmp(NULL)
, m_clTransColor(CLOCK_CTRL_DEFAULT_TRANS_COLOR)
, m_clBkgndColor(CLOCK_BKGND_COLOR)
, m_clDialScaleColor(CLOCK_DIAL_SCALE_COLOR)
, m_clHandColor(CLOCK_HAND_COLOR)
, m_fDialRadius(0.0f)
{
}

///
/// @brief
///     ��������
///
CClockCtrl::~CClockCtrl()
{
	
}

///
/// @brief
///     ����ʱ�ӱ��̣����̵�Բ������Bmp������
///
/// @param[in]
///     pBmp - ָ��ʱ�ӱ���Bmp��ָ��
///
/// @return
///
///
void CClockCtrl::SetDialBmp(CBitmap *pBmp)
{
	m_pDialBmp = pBmp;
}

///
/// @brief
///     ����ʱ��ָ��
///     ָ���ת�������Bmp�����ģ���ָ�볤��ӦΪBmp��һ�룬ָ��ָ��12���ӷ���
///
/// @param[in]
///     pHourBmp - ָ��ʱ��Bmp��ָ��
/// @param[in]
///     pMinBmp  - ָ�����Bmp��ָ��
/// @param[in]
///     pSecBmp  - ָ������Bmp��ָ��
///
/// @return
///
///
void CClockCtrl::SetHandBmp(CBitmap *pHourBmp, CBitmap *pMinBmp, CBitmap *pSecBmp)
{
	m_pHourHandBmp = pHourBmp;
	m_pMinHandBmp  = pMinBmp;
	m_pSecHandBmp  = pSecBmp;
}

///
/// @brief
///     ��ʼ���ؼ�
///
/// @return
///
///
void CClockCtrl::Init()
{
	DrawBkgnd();
	ResetForgnd();
}

///
/// @brief
///     ����ʱ��ָ��
///
/// @return
///
///
void CClockCtrl::EraseTime()
{
	CRect rt;
	GetClientRect(rt);
	m_cForgndDC.BitBlt(0, 0, rt.Width(), rt.Height(),
		&m_cBkgndDC, 0, 0, SRCCOPY);
}

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
/// @return
///
///
void CClockCtrl::DrawTime(const int hour,const int min, const int sec)
{
	int hourTmp = hour%12;
	int minTmp  = min%60;
	int secTmp  = sec%60;
	if ( ( m_pHourHandBmp != NULL && m_pHourHandBmp->GetSafeHandle() != NULL ) &&
		 ( m_pMinHandBmp != NULL && m_pMinHandBmp->GetSafeHandle() != NULL ) &&
		 ( m_pSecHandBmp != NULL && m_pSecHandBmp->GetSafeHandle() != NULL ) )
	{
		BITMAP bmp;
		float fDegree;     ///< ָ����ת�Ļ��ȣ���12�����Ļ���Ϊ0
		float fRatio;      ///< �ؼ��ϵ�ʵ��ָ����Bmp�ϵ�ָ��ĳ��ȵı���
		float fHandLen;    ///< Bmp�ϵ�ָ�볤�ȣ�Ĭ��Ϊbmp�߶ȵ�һ��
		// ����ʱ��
		fDegree = float(float(hour*3600+min*60+sec)/3600/12*2*PI);
		m_pHourHandBmp->GetBitmap(&bmp);
		fHandLen = (float)bmp.bmHeight/2;
		fRatio = (CLOCK_HOUR_HAND_RATIO*m_fDialRadius)/fHandLen;
		DrawBmpHand(&m_cForgndDC,m_pHourHandBmp,fDegree,fRatio);

		// ���Ʒ���
		fDegree = float(float(min*60+sec)/3600*2*PI);
		m_pMinHandBmp->GetBitmap(&bmp);
		fHandLen = (float)bmp.bmHeight/2;
		fRatio  = (CLOCK_MIN_HAND_RATIO*m_fDialRadius)/fHandLen;
		DrawBmpHand(&m_cForgndDC,m_pMinHandBmp,fDegree,fRatio);

		// ��������
		fDegree = float(float(sec)/60*2*PI);
		m_pSecHandBmp->GetBitmap(&bmp);
		fHandLen = (float)bmp.bmHeight/2;
		fRatio  = (CLOCK_SEC_HAND_RATIO*m_fDialRadius)/fHandLen;
		DrawBmpHand(&m_cForgndDC,m_pSecHandBmp,fDegree,fRatio);	
	}
	else
	{
		// ����ʱ��
		DrawDefaultHourHand(&m_cForgndDC,hourTmp,minTmp,secTmp);
		// ���Ʒ���
		DrawDefaultMinHand(&m_cForgndDC,minTmp,secTmp);
		// ��������
		DrawDefaultSecHand(&m_cForgndDC,secTmp);
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
///
void CClockCtrl::Flush(CDC* pDC)
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

BEGIN_MESSAGE_MAP(CClockCtrl, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

///
/// @brief
///     �ػ���Ϣ������
///
/// @return
///
void CClockCtrl::OnPaint()
{
// 	CRect rt;
// 	GetClientRect(rt);
// 
// 	PAINTSTRUCT ps;
// 	CDC *pDC = BeginPaint(&ps);
// 	pDC->BitBlt(0,0,rt.Width(),rt.Height(),m_cForgndDC,0,0,SRCCOPY);
// 	EndPaint(&ps);
	CPaintDC dc(this);
	CRect rt;
	GetClientRect(rt);
	dc.BitBlt(0,0,rt.Width(),rt.Height(),&m_cForgndDC,0,0,SRCCOPY);
}

///
/// @brief
///     ����������Ϣ������
///
/// @return
///
///
BOOL CClockCtrl::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

///
/// @brief
///     �ͷſؼ���Դ��Ϣ������
///
/// @return
///
void CClockCtrl::OnDestroy()
{
	CStatic::OnDestroy();

	// TODO: Add your message handler code here
	Release();
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
void CClockCtrl::CreateDCandBmp(const CRect &rt, CDC &dc, CBitmap &bmp, CBitmap **pPOldBmp)
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
void CClockCtrl::DrawBkgnd()
{
	CRect rt;
	GetClientRect(rt);

	// ��������DC��λͼ
	CreateDCandBmp(rt,m_cBkgndDC,m_cBkgndBmp,&m_pOldBkgndBmp);

	// ����ʱ�ӱ���
	if ( m_pDialBmp != NULL && m_pDialBmp->GetSafeHandle() != NULL )
		DrawBmpDail(&m_cBkgndDC,m_pDialBmp);
	else
		DrawDefaultDial(&m_cBkgndDC);
}

///
/// @brief
///     ���ÿؼ�ǰ��
///
/// @return
///
void CClockCtrl::ResetForgnd()
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
void CClockCtrl::Release()
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
}

///
/// @brief
///     ����ʱ�ӱ��̣�Ĭ����ʽ�����Ի棩
///
/// @param[in]
///     pDC - �豸����DC��ָ��
///
/// @return
///     
///
void CClockCtrl::DrawDefaultDial(CDC *pDC)
{
	// ��ʼ���豸����
	CPen pen(PS_SOLID,0,m_clDialScaleColor);
	CPen *pOldPen = pDC->SelectObject(&pen);
	CBrush brush(m_clDialScaleColor);
	CBrush *pOldBrush = pDC->SelectObject(&brush);

	CRect rt;
	GetClientRect(rt);
	CPoint ptCenter = rt.CenterPoint();
	// ��û���ñ��̰뾶����Ĭ��ΪBmp���/�߶ȵ�һ��
	if ( m_fDialRadius == 0.0f )
		m_fDialRadius = rt.Width() < rt.Height() ? ((float)rt.Width()/2 - 4.0f) : ((float)rt.Height()/2 - 4.0f);
	float radius = m_fDialRadius;

	// ����ʱ�ӱ��̵Ŀ̶�
	for ( int i = 0;i < 60;i++ )
	{
		CPoint endPt = ptCenter;
		endPt.Offset(int(radius*sin((i%60)*2*PI/60)+0.5f),
		             int(-radius*cos((i%60)*2*PI/60)+0.5f));
		if ( i%5 == 0 )
		{
			// ���Ʊ�ʾʱ�ӵľ��ο̶�
			CRect rcDot(-3,-3,3,3);
			rcDot.OffsetRect(endPt);
			pDC->Rectangle(rcDot);
		}
		else
		{
			// ���Ʊ�ʾ����������Բ��̶�
			CRect rcDot(-2,-2,2,2);
			rcDot.OffsetRect(endPt);
			pDC->Ellipse(rcDot);
		}
	}

	// ��ԭ�豸����
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	pen.DeleteObject();
	brush.DeleteObject();
}

///
/// @brief
///     ����ʱ��ʱ�루Ĭ����ʽ�����Ի棩
///
/// @param[in]
///     pDC  - �豸����DC��ָ��
/// @param[in]
///     hour - ʱ
/// @param[in]
///     min  - ��
/// @param[in]
///     sec  - ��
///
/// @return
///     
///
void CClockCtrl::DrawDefaultHourHand(CDC *pDC,const int hour,const int min, const int sec)
{
	// ��ʼ���豸����
	CPen pen(PS_SOLID,0,m_clHandColor);
	CPen *pOldPen = pDC->SelectObject(&pen);
	CBrush brush(m_clHandColor);
	CBrush *pOldBrush = pDC->SelectObject(&brush);

	CRect rt;
	GetClientRect(rt);
	CPoint ptCenter = rt.CenterPoint();
	float radius = m_fDialRadius*CLOCK_HOUR_HAND_RATIO;
	// ����ʱ��ĽǶ�
	float fDegree = float(float(hour*3600+min*60+sec)/3600/12*2*PI);

	// ����ʱ��
	CPoint pt[4];
	pt[0] = ptCenter;
	pt[0].Offset(int(radius*sin(fDegree)+0.5f),int(-radius*cos(fDegree)+0.5f));
	pt[1] = ptCenter;
	pt[1].Offset(int(radius/20.0f*sin(fDegree+0.5f*PI)+0.5f),int(-radius/20.0f*cos(fDegree+0.5f*PI)+0.5f));
	pt[2] = ptCenter;
	pt[2].Offset(int(radius/10.0f*sin(fDegree+PI)+0.5f),int(-radius/10.0f*cos(fDegree+PI)+0.5f));
	pt[3] = ptCenter;
	pt[3].Offset(int(radius/20.0f*sin(fDegree+1.5f*PI)+0.5f),int(-radius/20.0f*cos(fDegree+1.5f*PI)+0.5f));
	pDC->Polygon(pt,4);

	// ��ԭ�豸����
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	pen.DeleteObject();
	brush.DeleteObject();
}

///
/// @brief
///     ����ʱ�ӷ��루Ĭ����ʽ�����Ի棩
///
/// @param[in]
///     pDC  - �豸����DC��ָ��
/// @param[in]
///     min  - ��
/// @param[in]
///     sec  - ��
///
/// @return
///     
///
void CClockCtrl::DrawDefaultMinHand(CDC *pDC,const int min,const int sec)
{
	// ��ʼ���豸����
	CPen pen(PS_SOLID,0,m_clHandColor);
	CPen *pOldPen = pDC->SelectObject(&pen);
	CBrush brush(m_clHandColor);
	CBrush *pOldBrush = pDC->SelectObject(&brush);

	CRect rt;
	GetClientRect(rt);
	CPoint ptCenter = rt.CenterPoint();
	float radius = m_fDialRadius*CLOCK_MIN_HAND_RATIO;
	// �������ĽǶ�
	float fDegree = float(float(min*60+sec)/3600*2*PI);

	// ���Ʒ���
	CPoint pt[4];
	pt[0] = ptCenter;
	pt[0].Offset(int(radius*sin(fDegree)+0.5f),int(-radius*cos(fDegree)+0.5f));
	pt[1] = ptCenter;
	pt[1].Offset(int(radius/20.0f*sin(fDegree+0.5f*PI)+0.5f),int(-radius/20.0f*cos(fDegree+0.5f*PI)+0.5f));
	pt[2] = ptCenter;
	pt[2].Offset(int(radius/10.0f*sin(fDegree+PI)+0.5f),int(-radius/10.0f*cos(fDegree+PI)+0.5f));
	pt[3] = ptCenter;
	pt[3].Offset(int(radius/20.0f*sin(fDegree+1.5f*PI)+0.5f),int(-radius/20.0f*cos(fDegree+1.5f*PI)+0.5f));
	pDC->Polygon(pt,4);

	// ��ԭ�豸����
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	pen.DeleteObject();
	brush.DeleteObject();
}

///
/// @brief
///     ����ʱ�����루Ĭ����ʽ�����Ի棩
///
/// @param[in]
///     pDC  - �豸����DC��ָ��
/// @param[in]
///     sec  - ��
///
/// @return
///     
///
void CClockCtrl::DrawDefaultSecHand(CDC *pDC,const int sec)
{
	// ��ʼ���豸����
	CPen pen(PS_SOLID,0,m_clHandColor);
	CPen *pOldPen = pDC->SelectObject(&pen);
	CBrush brush(m_clHandColor);
	CBrush *pOldBrush = pDC->SelectObject(&brush);

	CRect rt;
	GetClientRect(rt);
	CPoint ptCenter = rt.CenterPoint();
	float radius = m_fDialRadius*CLOCK_SEC_HAND_RATIO;
	// ��������ĽǶ�
	float fDegree = float(float(sec)/60*2*PI);

	// ��������
	CPoint pt[4];
	pt[0] = ptCenter;
	pt[0].Offset(int(radius*sin(fDegree)+0.5f),int(-radius*cos(fDegree)+0.5f));
	pt[1] = ptCenter;
	pt[1].Offset(int(radius/50.0f*sin(fDegree+0.5f*PI)+0.5f),int(-radius/50.0f*cos(fDegree+0.5f*PI)+0.5f));
	pt[2] = ptCenter;
	pt[2].Offset(int(radius/5.0f*sin(fDegree+PI)+0.5f),int(-radius/5.0f*cos(fDegree+PI)+0.5f));
	pt[3] = ptCenter;
	pt[3].Offset(int(radius/50.0f*sin(fDegree+1.5f*PI)+0.5f),int(-radius/50.0f*cos(fDegree+1.5f*PI)+0.5f));
	pDC->Polygon(pt,4);

	// ��ԭ�豸����
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	pen.DeleteObject();
	brush.DeleteObject();
}

///
/// @brief
///     ����ʱ�ӱ��̣���ͼ��ʽ��
///
/// @param[in]
///     pDC  - �豸����DC��ָ��
/// @param[in]
///     pBmp - ָ��ʱ�ӱ���Bmp��ָ��
///
/// @return
///     
///
void CClockCtrl::DrawBmpDail(CDC *pDC, CBitmap *pBmp)
{
	if ( pBmp == NULL || pBmp->GetSafeHandle() == NULL )
		return;

	CRect rt;
	GetClientRect(rt);

	// ��ʼ������DC
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap *pOldBmp = memDC.SelectObject(pBmp);

	// ����ʱ�ӱ���
	BITMAP bmp;
	pBmp->GetBitmap(&bmp);
	pDC->StretchBlt(0,0,rt.Width(),rt.Height(),&memDC,0,0,bmp.bmWidth,bmp.bmHeight,SRCCOPY);
	memDC.SelectObject(pOldBmp);
	memDC.DeleteDC();

	//  ���¿ؼ��ϵı��̰뾶����Ĭ��Ϊ�ؼ����/�߶ȵ�һ��
	if ( m_fDialRadius == 0.0f )
		m_fDialRadius = bmp.bmWidth < bmp.bmHeight ? ((float)rt.Width()/2) : ((float)rt.Height()/2);
}

///
/// @brief
///     ����ʱ��ָ�루��ͼ��ʽ��
///
/// @param[in]
///     pDC    - ָ���豸����DC��ָ��
/// @param[in]
///     pBmp   - ָ��bmp��ָ��
/// @param[in]
///     degree - ָ���˳ʱ�뷽�����ת���ȣ���12���ӷ��򻡶�Ϊ0
/// @param[in]
///     ratio  - ʵ�ʻ����ڿؼ��ϵ�ָ����Bmp�ϵ�ָ��ı���
///
/// @return
///     
///
void CClockCtrl::DrawBmpHand(CDC *pDC, CBitmap *pBmp, const float degree, const float ratio)
{
	if ( pBmp == NULL || pBmp->GetSafeHandle() == NULL )
		return;

	// ��ȡĿ��λBmp����Ϣ
	BITMAP bmp;
	pBmp->GetBitmap(&bmp);

	// ����DIBλͼ
	HBITMAP hSrcBmp;
	BYTE *srcLpBits = NULL;
	CreateBmp(hSrcBmp,&srcLpBits,bmp.bmWidth,bmp.bmHeight,bmp.bmBitsPixel);
	
	HDC hdcSrc = CreateCompatibleDC(pDC->m_hDC);
	HBITMAP hbmOld = (HBITMAP)::SelectObject(hdcSrc, hSrcBmp);

	// ��ȡָ��λͼ���ڴ�����
	CDC srcBmpDC;
	srcBmpDC.CreateCompatibleDC(pDC);
	CBitmap *pOldSrcBmp = srcBmpDC.SelectObject(pBmp);
	if ( BitBlt(hdcSrc,0,0,bmp.bmWidth,bmp.bmHeight,srcBmpDC.m_hDC,0,0,SRCCOPY) == FALSE )
		return;
	srcBmpDC.SelectObject(pOldSrcBmp);
	srcBmpDC.DeleteDC();

	// ��תλͼ�����浽DC
	RotateBmp(pDC,bmp,srcLpBits,-degree,ratio);
	
	// ��ԭ�豸����
	::SelectObject(hdcSrc, hbmOld);
	DeleteDC(hdcSrc);
	DeleteObject(hSrcBmp);

	// ���������ͨ������DC�����λͼ��ת��Ч�ʽ���
#if 0
	// �����Ͻ�����Ϊ��0��0����������ΪԲ�ģ�����˳ʱ����ת���Bmp�ľ����������3��
	float sinVal = sin(degree);
	float cosVal = cos(degree);
	float x1     = bmp.bmWidth * cosVal;
	float y1     = bmp.bmWidth * sinVal;
	float x2     = bmp.bmWidth * cosVal - bmp.bmHeight * sinVal;
	float y2     = bmp.bmWidth * sinVal + bmp.bmHeight * cosVal;
	float x3     = -bmp.bmHeight *sinVal;
	float y3     = bmp.bmHeight  *cosVal;

	// ����˳ʱ����ת���Bmp�ľ�����Ŀ�Ⱥ͸߶�
	float minWidth  = min(min(0,x1),min(x2,x3));
	float minHeight = min(min(0,y1),min(y2,y3));
	float maxWidth  = max(max(0,x1),max(x2,x3));
	float maxHeight = max(max(0,y1),max(y2,y3));
	int dstBmpWidth  = abs(maxWidth - minWidth + 0.5f);
	int dstBmpHeight = abs(maxHeight - minHeight + 0.5f);

	// ��ʼ��ԭBmp�Ļ���DC
	CDC srcBmpDC;
	srcBmpDC.CreateCompatibleDC(pDC);
	CBitmap *pOldSrcBmp = srcBmpDC.SelectObject(pBmp);

	// ��ʼ����ת���Bmp�Ļ���DC
	CDC dstBmpDC;
	CBitmap dstBmp;
	dstBmpDC.CreateCompatibleDC(pDC);
	dstBmp.CreateCompatibleBitmap(pDC,dstBmpWidth,dstBmpHeight);
	CBitmap *pOldDstBmp = dstBmpDC.SelectObject(&dstBmp);
	::FillRect(dstBmpDC,CRect(0,0,dstBmpWidth,dstBmpHeight),CBrush(RGB(255,255,255)));

	// ����ԭBmp��Ŀ��Bmp��λ��ӳ�������㿽��
	for( int i = 0; i < dstBmpHeight; i++ )
	{
		for( int j = 0; j  <  dstBmpWidth; j++ )
		{
			// ������ת���Bmp�ĸ������ԭbmp������λ�ã�����ʱ����ת���λ��
			// aΪ��ת��ĽǶȣ�bΪ��תǰ�ĽǶ�
			// x=r*cos(a-b)=r*(cos(a)*cos(b)+sin(a)*sin(b))=X0*cos(b)+Y0*sin(b);
			// y=r*sin(a-b)=r*(sin(a)*cos(b)-cos(a)*sin(b))=Y0*cos(b)-X0*sin(b);
			float srcX = (i + minHeight) * sinVal + (j + minWidth) * cosVal + 0.5f;
			float srcY = (i + minHeight) * cosVal - (j + minWidth) * sinVal + 0.5f;
			// ��ԭͼ��Χ���򿽱�
			if((srcX >= 0) && (srcX < bmp.bmWidth) &&(srcY >=  0) && (srcY < bmp.bmHeight)) 
			{ 
				BitBlt(dstBmpDC, j, i, 1, 1, srcBmpDC.m_hDC, (int)srcX, (int)srcY, SRCCOPY);
			}
		}
	}

	// ����ָ���ڿؼ��ϵ�ʵ�ʿ�Ⱥ͸߶�
	int width  = int(dstBmpWidth*ratio);
	int height = int(dstBmpHeight*ratio);

	// ����ָ���ڱ����ϵ�ʵ�ʾ�����
	// һ���ε�4���˵�����һ���εı��ϣ������ĵ���ͬ
	CRect rt;
	GetClientRect(rt);
	CRect handRt;
	handRt.left=rt.CenterPoint().x-int(width/2);
	handRt.right=handRt.left+width;
	handRt.top=rt.CenterPoint().y-height/2;
	handRt.bottom=handRt.top+height;
	dstBmpDC.SetBkColor(RGB(255,255,255));
	TransparentBlt(pDC->m_hDC,handRt.left,handRt.top,handRt.Width(),handRt.Height(),
		dstBmpDC.m_hDC,0,0,dstBmpWidth,dstBmpHeight,RGB(255,255,255));

	// �ͷ���Դ
	srcBmpDC.SelectObject(pOldSrcBmp);
	srcBmpDC.DeleteDC();
	dstBmpDC.SelectObject(pOldDstBmp);
	dstBmpDC.DeleteDC();
	dstBmp.DeleteObject();
#endif
}

///
/// @brief
///     ����DIBλͼ
///
/// @param[in]
///     hBmp        - ���ڱ�����������λͼ�ľ��
/// @param[in]
///     lpBits      - ���ڱ���һ��ָ��DIBλ����ֵ��ָ�룬���ڴ�ռ���ϵͳ������ͷ�
/// @param[in]
///     bmWidth     - ��Ҫ������DIBλͼ�Ŀ�ȣ���λ������
/// @param[in]
///     bmHeight    - ��Ҫ������DIBλͼ�ĸ߶ȣ���λ������
/// @param[in]
///     bmBitsPixel - ��Ҫ������DIBλͼ�����ص��λ��
///
/// @return
///     
///
void CClockCtrl::CreateBmp(HBITMAP &hBmp, BYTE **lpBits, const int bmWidth, const int bmHeight, const int bmBitsPixel)
{
	BITMAPINFOHEADER bih;
	memset(&bih, 0, sizeof(BITMAPINFOHEADER));
	bih.biSize        = sizeof(BITMAPINFOHEADER);
	bih.biWidth       = bmWidth;
	bih.biHeight      = bmHeight;
	bih.biPlanes      = 1;
	bih.biBitCount    = bmBitsPixel;
	bih.biCompression = BI_RGB;
	bih.biSizeImage   = ((((bmBitsPixel*bmWidth)+31)/32)*4)*bmHeight;

	if ( bmBitsPixel < 24 )
	{
		CDC *pDC = GetDC();
		hBmp = CreateDIBSection(pDC->m_hDC,(BITMAPINFO *)&bih,DIB_PAL_COLORS,(void**)lpBits,NULL,0);
		ReleaseDC(pDC);
	}
	else
		hBmp = CreateDIBSection(NULL,(BITMAPINFO *)&bih,DIB_RGB_COLORS,(void**)lpBits,NULL,0);
}

///
/// @brief
///     ��תλͼ�����浽DC
///
/// @param[in]
///     pDC       - ָ���豸����DC��ָ��
/// @param[in]
///     srcBmp    - ԴBmp����Ϣ
/// @param[in]
///     srcLpBits - ָ��ԴBmp��DIB���ݵ�ָ��
/// @param[in]
///     degree    - ԴBmpҪ��ת�Ļ��ȣ���12���ӷ��򻡶�Ϊ0������Ϊ��ʱ�뷽�򣬸���Ϊ˳ʱ�뷽��
/// @param[in]
///     ratio     - ʵ�ʻ����ڿؼ��ϵ�ָ����ԴBmp�ϵ�ָ��ı���
///
/// @return
///     
///
void CClockCtrl::RotateBmp(CDC *pDC, const BITMAP &srcBmp, BYTE *srcLpBits, const float degree, const float ratio)
{
	// �����Ͻ�����Ϊ��0��0����������ΪԲ�ģ�������תdegree���Ⱥ��Bmp�ľ����������3��
	// ע��ȡ�õ�λͼ�Ǵ��µ��ϣ������ң�����1��������ͼ��������һ�е���ߵ�һ�����أ�������ȥ
	float sinVal = (float)(sin(degree));
	float cosVal = (float)(cos(degree));
	float x1     = srcBmp.bmWidth * cosVal;
	float y1     = srcBmp.bmWidth * sinVal;
	float x2     = srcBmp.bmWidth * cosVal - srcBmp.bmHeight * sinVal;
	float y2     = srcBmp.bmWidth * sinVal + srcBmp.bmHeight * cosVal;
	float x3     = -srcBmp.bmHeight *sinVal;
	float y3     = srcBmp.bmHeight  *cosVal;

	// ������ת���Bmp�ľ�����Ŀ�Ⱥ͸߶�
	float minWidth  = min(min(0,x1),min(x2,x3));
	float minHeight = min(min(0,y1),min(y2,y3));
	float maxWidth  = max(max(0,x1),max(x2,x3));
	float maxHeight = max(max(0,y1),max(y2,y3));
	int dstBmpWidth  = abs(int(maxWidth - minWidth + 0.5f));
	int dstBmpHeight = abs(int(maxHeight - minHeight + 0.5f));
	int dstBmpEffWidth = (((srcBmp.bmBitsPixel*dstBmpWidth)+31)/32);

	// ������¼��ת������ݵ�DIBλͼ
	HBITMAP hDstBmp;
	BYTE *dstLpBits = NULL;
	CreateBmp(hDstBmp,&dstLpBits,dstBmpWidth,dstBmpHeight,srcBmp.bmBitsPixel);

	// ��������DC
	HDC hdcDst = CreateCompatibleDC(pDC->m_hDC);
	HBITMAP hbmOld = (HBITMAP)::SelectObject(hdcDst, hDstBmp);
	::FillRect(hdcDst,CRect(0,0,dstBmpWidth,dstBmpHeight),CBrush(RGB(255,255,255)));

 	// ����ԭBmp��Ŀ��Bmp��λ��ӳ�������㿽��
	for( int y = 0; y < dstBmpHeight; y++ )
	{
		for( int x = 0; x  <  dstBmpWidth; x++ )
		{
			// ������ת���Bmp�ĸ�������Ӧ��ԭbmp������λ��
			// aΪ��ת��ĽǶȣ�bΪ��תǰ�ĽǶ�
			// x=r*cos(a-b)=r*(cos(a)*cos(b)+sin(a)*sin(b))=X0*cos(b)+Y0*sin(b);
			// y=r*sin(a-b)=r*(sin(a)*cos(b)-cos(a)*sin(b))=Y0*cos(b)-X0*sin(b);
			int srcX = int((y + minHeight) * sinVal + (x + minWidth) * cosVal + 0.5f);
			int srcY = int((y + minHeight) * cosVal - (x + minWidth) * sinVal + 0.5f);
			// ��ԭͼ��Χ���򿽱�����Ļɫ������ڻ����8λ
			if ( srcBmp.bmBitsPixel >= 8 )
			{
				int pixelBytes = srcBmp.bmBitsPixel / 8;
				BYTE *pDst = dstLpBits + y*dstBmpEffWidth*4 + x*pixelBytes;
				if((srcX >= 0) && (srcX < srcBmp.bmWidth) &&(srcY >=  0) && (srcY < srcBmp.bmHeight)) 
				{ 
					BYTE *pSrc = srcLpBits + srcY*srcBmp.bmWidthBytes + srcX*pixelBytes;
					for ( int i = 0; i < pixelBytes; i++ )
					{
						*pDst++ = *pSrc++;
					}
				}
			}
		}
	}

	// ����ָ���ڿؼ��ϵ�ʵ�ʿ�Ⱥ͸߶�
	int width  = int(dstBmpWidth*ratio);
	int height = int(dstBmpHeight*ratio);

	// ����ָ���ڿؼ��ϵ�ʵ�ʾ�����
	// һ���ε�4���˵�����һ���εı��ϣ������ĵ���ͬ
	CRect rt;
	GetClientRect(rt);
	CRect handRt;
	handRt.left=rt.CenterPoint().x-int(width/2);
	handRt.right=handRt.left+width;
	handRt.top=rt.CenterPoint().y-height/2;
	handRt.bottom=handRt.top+height;
	TransparentBlt(pDC->m_hDC,handRt.left,handRt.top,handRt.Width(),handRt.Height(),
		hdcDst,0,0,dstBmpWidth,dstBmpHeight,m_clTransColor);

	// ��ԭ�豸����
	::SelectObject(hdcDst, hbmOld);
	DeleteDC(hdcDst);
	DeleteObject(hDstBmp);
}
