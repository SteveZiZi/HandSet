/// @file
///     PolarityDialCtrl.cpp
///
/// @brief
///     ���Ա��̿ؼ�
///
/// @note
///     �ñ���ָ��ĳ�ʼλ��ָ��12���ӷ���
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
#include <float.h>
#include "PolarityDialCtrl.h"

///
/// @brief
///     �ؼ��ı�����ɫ
///
#define PD_CTRL_BKGND_COLOR      RGB(0,0,0)           ///< ��ɫ

///
/// @brief
///     ���̵�Ĭ������
///
#define PD_HAND_LEFT_COLOR       RGB(255,255,255)     ///< ָ�����벿����ɫ,��ɫ
#define PD_HAND_RIGHT_COLOR      RGB(128,128,135)     ///< ָ����Ұ벿����ɫ,ů��ɫ
#define PD_HAND_BTMEDGE_RATIO    3.0f/75.0f           ///< �Ի��������ָ��ĵױ���ʵ�ʵױߵĳ��ȱ���

///
/// @brief
///     �ؼ���ͼ��͸������Ĭ����ɫ
///
#define PD_DEFAULT_TRANS_COLOR   RGB(0x01,0x02,0x03)

#ifndef PI
#define PI 3.1415926
#endif

IMPLEMENT_DYNAMIC(CPolarityDialCtrl, CStatic)

///
/// @brief
///     ���캯��
///
CPolarityDialCtrl::CPolarityDialCtrl()
: m_pOldBkgndBmp(NULL)
, m_pOldForgndBmp(NULL)
, m_pDialBmp(NULL)
, m_pHandBmp(NULL)
, m_bDialBmpTransparent(FALSE)
, m_clTransColor(PD_DEFAULT_TRANS_COLOR)
, m_clBkgndColor(PD_CTRL_BKGND_COLOR)
, m_clHandLeftColor(PD_HAND_LEFT_COLOR)
, m_clHandRightColor(PD_HAND_RIGHT_COLOR)
, m_fDialInitState(0.0f)
, m_iXGramCount(1)
, m_iYGramCount(1)
, m_fStdDialInRadii(0.0f)
, m_fStdHandRadii(0.0f)
, m_fMinDegree(0.0f)
, m_fMaxDegree(0.0f)
, m_iXGramSpace(0)
, m_iYGramSpace(0)
{
	
}

///
/// @brief
///     ��������
///
CPolarityDialCtrl::~CPolarityDialCtrl()
{
	
}

///
/// @brief
///     ���ñ���bmp
///
/// @param[in]
///     pBmp         - ָ��mp��ָ��
/// @param[in]
///     bTransparent - bmp�İ�ɫ�����Ƿ�͸����
///
/// @return
///
///
void CPolarityDialCtrl::SetBkgndBmp(CBitmap *pBmp, BOOL bTransparent /* = FALSE */)
{
	m_pDialBmp = pBmp;
	m_bDialBmpTransparent = bTransparent;
}

///
/// @brief
///     ���ñ���ָ�룬ָ����ָ��12���ӷ���
///
/// @param[in]
///     pBmp - ָ��Bmp��ָ��
///
/// @return
///
///
void CPolarityDialCtrl::SetHandBmp(CBitmap *pBmp)
{
	m_pHandBmp = pBmp;
}

///
/// @brief
///     ��ʼ�����Ա��̿ؼ����ޱ���
///
/// @return
///
///
void CPolarityDialCtrl::Init()
{
	DrawBkgnd();
	ResetForgnd();
}

///
/// @brief
///     ���������ϵ�ָ��
///
/// @return
///
///
void CPolarityDialCtrl::EraseHand()
{
	CRect rt;
	GetClientRect(rt);
	m_cForgndDC.BitBlt(0, 0, rt.Width(), rt.Height(),
		&m_cBkgndDC, 0, 0, SRCCOPY);
}

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
/// @return
///
///
void CPolarityDialCtrl::DrawHand(const size_t idxSeq,float percentVal)
{
	if ( (int)idxSeq >= m_iXGramCount*m_iYGramCount)
		return;

	// ����������Ĭ��Ϊ50.f
	if ( _finite(percentVal) == FALSE )
		percentVal = 0;
	if ( percentVal < 0.0f || percentVal > 100.0f )
		percentVal = m_fDialInitState;

	// ���㵱ǰҪ���Ƶı��̵ľ�����
	int x = (int)idxSeq%m_iXGramCount;
	int y = (int)idxSeq/m_iXGramCount;
	CRect dialArea(m_rtDialArea.left   + m_iXGramSpace * x,
	               m_rtDialArea.top    + m_iYGramSpace * y,
	               m_rtDialArea.right  + m_iXGramSpace * x,
	               m_rtDialArea.bottom + m_iYGramSpace * y);

	// �����׼������ؼ��ĳߴ����
	float xRatio = (float)dialArea.Width()/(float)m_rtStdDialRect.Width();
	float yRatio = (float)dialArea.Height()/(float)m_rtStdDialRect.Height();

	// ����ؼ��еı���Բ��
	CPoint actCircleCentre(int(dialArea.Width()*x+m_ptStdDialCircleCentre.x*xRatio),
		                   int(dialArea.Height()*y+m_ptStdDialCircleCentre.y*yRatio)-6);

	// ����ؼ��еı����ھ����Ⱥ�ָ�볤��
	//float actInRadii = m_fStdDialInRadii * yRatio;
	//float actHandRadii = m_fStdHandRadii * yRatio;

    // ����ؼ��еı����ھ����Ⱥ�ָ�볤��
    float actInRadii = 15.0;
    float actHandRadii = 51.0;

//	m_cForgndDC.AngleArc(actCircleCentre.x,actCircleCentre.y,actInRadii,0.0f,180.0f);

	// ����ؼ��е�ָ���ƫת�Ƕ�
	float degree = float(((m_fMinDegree+(m_fMaxDegree-m_fMinDegree)/100.0f*percentVal)-90.0f)*2*PI/360.0f);
	
	// ���Ʊ���ָ��
	if ( m_pHandBmp != NULL && m_pHandBmp->GetSafeHandle() != NULL )
	{
		DrawBmpHand(&m_cForgndDC,m_pHandBmp,actCircleCentre,actInRadii,actHandRadii,degree);
	}
	else
	{
		DrawDefaultHand(&m_cForgndDC,actCircleCentre,actInRadii,actHandRadii,degree);
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
void CPolarityDialCtrl::Flush(CDC *pDC)
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

BEGIN_MESSAGE_MAP(CPolarityDialCtrl, CStatic)
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
void CPolarityDialCtrl::OnPaint()
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
void CPolarityDialCtrl::OnDestroy()
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
BOOL CPolarityDialCtrl::OnEraseBkgnd(CDC* pDC)
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
void CPolarityDialCtrl::CreateDCandBmp(const CRect &rt, CDC &dc, CBitmap &bmp, CBitmap **pPOldBmp)
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
void CPolarityDialCtrl::DrawBkgnd()
{
	CRect rt;
	GetClientRect(rt);
	
	// ��������DC��λͼ
	CreateDCandBmp(rt,m_cBkgndDC,m_cBkgndBmp,&m_pOldBkgndBmp);

	// �������
	m_cBkgndDC.FillSolidRect(0,0,rt.Width(),rt.Height(),m_clBkgndColor);

	// �����1�����̵ľ�����ÿ�����̵Ĵ�С
	UpdateDialArea(&m_cBkgndDC);

	// ���Ʊ���Bmp����
	if ( m_pDialBmp != NULL && m_pDialBmp->GetSafeHandle() != NULL )
	{
		// ��ʼ������DC
		CDC memDC;
		memDC.CreateCompatibleDC(&m_cBkgndDC);
		CBitmap *pOldMemBmp = memDC.SelectObject(m_pDialBmp);

		// ��ȡ����Bmp����Ϣ
		BITMAP bmp;
		m_pDialBmp->GetBitmap(&bmp);
	
		// �������ң����ϵ��»������б���
		for ( int y = 0; y < m_iYGramCount; y++ )
		{
			for ( int x = 0; x < m_iXGramCount; x++ )
			{
				// ���㵱ǰҪ���Ƶı��̵ľ�����
				CRect dialArea(m_rtDialArea.left   + m_iXGramSpace * x,
					m_rtDialArea.top    + m_iYGramSpace * y,
					m_rtDialArea.right  + m_iXGramSpace * x,
					m_rtDialArea.bottom + m_iYGramSpace * y);
				if ( m_bDialBmpTransparent )
				{
					// ����λͼ�İ�ɫ����͸����	
					TransparentBlt(m_cBkgndDC.m_hDC,dialArea.left,dialArea.top,dialArea.Width(),dialArea.Height(),
						memDC.m_hDC,0,0,bmp.bmWidth,bmp.bmHeight,m_clTransColor);
				}
				else
				{
					m_cBkgndDC.StretchBlt(dialArea.left,dialArea.top,dialArea.Width(),dialArea.Height(),
						&memDC,0,0,bmp.bmWidth,bmp.bmHeight,SRCCOPY);
				}
			}
		}

		memDC.SelectObject(pOldMemBmp);
		memDC.DeleteDC();
	}
}

///
/// @brief
///     ���ÿؼ�ǰ��
///
/// @return
///
void  CPolarityDialCtrl::ResetForgnd()
{
	CRect rt;
	GetClientRect(rt);

	// ����ǰ��DC��λͼ
	CreateDCandBmp(rt,m_cForgndDC,m_cForgndBmp,&m_pOldForgndBmp);

	m_cForgndDC.BitBlt(0,0,rt.Width(),rt.Height(),&m_cBkgndDC,0,0,SRCCOPY);
}

///
/// @brief
///     �ؼ���Դ�ͷź���
///
/// @return
///
void CPolarityDialCtrl::Release()
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
///     �����1�����̵ľ����򣬼�ÿ�����̵Ĵ�С
///
/// @param[in]
///     pDC - �豸����DC��ָ��
///
/// @return
///     
///
void CPolarityDialCtrl::UpdateDialArea(CDC *pDC)
{
	CRect rt;
	GetClientRect(rt);

	m_rtDialArea.SetRect(rt.left,rt.top,rt.Width()/m_iXGramCount,rt.Height()/m_iYGramCount);
	m_iXGramSpace = rt.Width()/m_iXGramCount;
	m_iYGramSpace = rt.Height()/m_iYGramCount;
}

///
/// @brief
///     ����ָ�룺�Ի淽ʽ
///
/// @param[in]
///     pDC       - �豸����DC��ָ��
/// @param[in]
///     axisPt    - �ڿؼ���ָ���ת�������
/// @param[in]
///     inRadii   - �ڿؼ��ϱ��̵��ھ�����
/// @param[in]
///     handRadii - �ڿؼ���ָ��İ뾶����
/// @param[in]
///     degree    - �ڿؼ���ָ���ƫת���ȣ���12���ӷ��򻡶�Ϊ0��+Ϊ��ƫ��-Ϊ��ƫ
///
/// @return
///     
///
void CPolarityDialCtrl::DrawDefaultHand(CDC *pDC, const CPoint &axisPt, const float inRadii, const float handRadii, const float degree)
{
#if 0
	// �������ָ���3���˵�
	float sinVal = (float)(sin(degree));
	float cosVal = (float)(cos(degree));
	CPoint pt[4];
	float  handBtmEdgeHalfLen = (handRadii-inRadii)*(PD_HAND_BTMEDGE_RATIO);
	pt[1] = axisPt;
	pt[1].Offset(int(handRadii*sin(degree)+0.5f),int(-handRadii*cos(degree)-0.5f));
	pt[2] = axisPt;
	pt[2].Offset(int(inRadii*sin(degree)+0.5f),int(-inRadii*cos(degree)-0.5f));
	pt[0] = pt[3] = pt[2];
	pt[3].Offset(int(handBtmEdgeHalfLen*sin(degree+0.5f*PI)+0.5f),int(-handBtmEdgeHalfLen*cos(degree+0.5f*PI)-0.5f));
	pt[0].Offset(int(handBtmEdgeHalfLen*sin(degree+1.5f*PI)+0.5f),int(-handBtmEdgeHalfLen*cos(degree+1.5f*PI)-0.5f));

	// ���Ʊ���ָ�����벿��
	CBrush brush(m_clHandLeftColor);
	CBrush *pOldBrush = pDC->SelectObject(&brush);
	CPen pen;
	pen.CreatePen(PS_SOLID,0,m_clHandLeftColor);
	CPen *pOldPen = pDC->SelectObject(&pen);
	pDC->Polygon(&pt[0],3);
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
	brush.DeleteObject();
	pen.DeleteObject();

	// ���Ʊ���ָ����Ұ벿��
	brush.CreateSolidBrush(m_clHandRightColor);
	pen.CreatePen(PS_SOLID,0,m_clHandRightColor);
	pOldBrush = pDC->SelectObject(&brush);
	pOldPen = pDC->SelectObject(&pen);
	pDC->Polygon(&pt[1],3);
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
	brush.DeleteObject();
	pen.DeleteObject();
#else
	// �������ָ��Ķ˵�
	float sinVal = (float)(sin(degree));
	float cosVal = (float)(cos(degree));
	CPoint pt[4];
	float  handBtmEdgeHalfLen = (handRadii-inRadii)*(PD_HAND_BTMEDGE_RATIO);
	pt[1] = axisPt;
	pt[1].Offset(int(handRadii*sin(degree)+0.5f),int(-handRadii*cos(degree)-0.5f));
	pt[2] = pt[1];
	pt[1].Offset(int(1.0f*sin(degree+0.5f*PI)+0.5f),int(-1.0f*cos(degree+0.5f*PI)-0.5f));
	pt[2].Offset(int(1.0f*sin(degree+1.5f*PI)+0.5f),int(-1.0f*cos(degree+1.5f*PI)-0.5f));
	pt[0] = pt[3] = axisPt;
	pt[3].Offset(int(handBtmEdgeHalfLen*sin(degree+0.5f*PI)+0.5f),int(-handBtmEdgeHalfLen*cos(degree+0.5f*PI)-0.5f));
	pt[0].Offset(int(handBtmEdgeHalfLen*sin(degree+1.5f*PI)+0.5f),int(-handBtmEdgeHalfLen*cos(degree+1.5f*PI)-0.5f));

	// ���Ʊ���ָ�����벿��
	CBrush brush(m_clHandLeftColor);
	CBrush *pOldBrush = pDC->SelectObject(&brush);
	CPen pen;
	pen.CreatePen(PS_SOLID,0,m_clHandLeftColor);
	CPen *pOldPen = pDC->SelectObject(&pen);
	pDC->Polygon(&pt[0],4);
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
	brush.DeleteObject();
	pen.DeleteObject();

	// ���Ʊ���ָ����Ұ벿��
	CRect rt(0,0,(int)(handBtmEdgeHalfLen*2+4),(int)(handBtmEdgeHalfLen*2+4));
	rt.MoveToXY(axisPt.x-rt.Width()/2+1,axisPt.y-rt.Height()/2);
	brush.CreateSolidBrush(m_clHandRightColor);
	pen.CreatePen(PS_SOLID,0,m_clHandRightColor);
	pOldBrush = pDC->SelectObject(&brush);
	pOldPen = pDC->SelectObject(&pen);
	pDC->Ellipse(rt);
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
	brush.DeleteObject();
	pen.DeleteObject();
#endif
}

///
/// @brief
///     ����ָ�룺��ͼ��ʽ
///
/// @param[in]
///     pDC       - �豸����DC��ָ��
/// @param[in]
///     axisPt    - �ڿؼ���ָ���ת�������
/// @param[in]
///     inRadii   - �ڿؼ��ϱ��̵��ھ�����
/// @param[in]
///     handRadii - �ڿؼ���ָ��İ뾶����
/// @param[in]
///     degree    - �ڿؼ���ָ���ƫת���ȣ���12���ӷ��򻡶�Ϊ0��+Ϊ��ƫ��-Ϊ��ƫ
///
/// @return
///     
///
void CPolarityDialCtrl::DrawBmpHand(CDC *pDC, CBitmap *pBmp, const CPoint &axisPt, const float inRadii, const float handRadii, const float degree)
{
	if ( pBmp == NULL || pBmp->GetSafeHandle() == NULL )
		return;

	// ��ȡԴBmp����Ϣ
	BITMAP srcBmp;
	pBmp->GetBitmap(&srcBmp);

	// ����DIBλͼ
	HBITMAP hSrcBmp;
	BYTE *srcLpBits = NULL;
	CreateBmp(hSrcBmp,&srcLpBits,srcBmp.bmWidth,srcBmp.bmHeight,srcBmp.bmBitsPixel);

	// ����Դbmp�Ļ���DC
	HDC hdcSrc = CreateCompatibleDC(pDC->m_hDC);
	HBITMAP hOldSrcBmp = (HBITMAP)::SelectObject(hdcSrc, hSrcBmp);

	// ��ȡԴbmp���ڴ�����
	CDC srcBmpDC;
	srcBmpDC.CreateCompatibleDC(pDC);
	CBitmap *pOldSrcBmp = srcBmpDC.SelectObject(pBmp);
	if ( BitBlt(hdcSrc,0,0,srcBmp.bmWidth,srcBmp.bmHeight,srcBmpDC.m_hDC,0,0,SRCCOPY) == FALSE )
		return;
	srcBmpDC.SelectObject(pOldSrcBmp);
	srcBmpDC.DeleteDC();

	// ��תλͼ�����浽DC
	HBITMAP hDstBmp;
	RotateBmp(hDstBmp,srcBmp,srcLpBits,-degree);

	// ������ת���bmp�Ļ���DC
	HDC hdcDst = CreateCompatibleDC(pDC->m_hDC);
	HBITMAP hOldDstBmp = (HBITMAP)::SelectObject(hdcDst, hDstBmp);

	// ������ת���Bmp���ڿؼ��ϵľ�����(ƫ��=ƫ�ƺ�����ĵ�-ƫ��ǰ�����ĵ�)	
	//float ratio = (handRadii-inRadii)/(float)srcBmp.bmHeight;
    /*************************************************************************/
    float ratio = 1.0;
	BITMAP dstBmp;
	CBitmap::FromHandle(hDstBmp)->GetBitmap(&dstBmp);
	CRect handRt(0,0,int(dstBmp.bmWidth*ratio),int(dstBmp.bmHeight*ratio));
	CPoint pt[3];
	pt[0] = axisPt;
	pt[0].Offset(int(handRadii*sin(degree)+0.5f),int(-handRadii*cos(degree)-0.5f));
	pt[1] = axisPt;
	pt[1].Offset(int(inRadii*sin(degree)+0.5f),int(-inRadii*cos(degree)-0.5f));
	pt[2] = CPoint((pt[1].x+pt[0].x)/2,(pt[1].y+pt[0].y)/2);
	handRt.OffsetRect(pt[2]-handRt.CenterPoint());
	
	// ����ָ��
	TransparentBlt(pDC->m_hDC,handRt.left,handRt.top-1,handRt.Width(),handRt.Height(),
		hdcDst,0,0,dstBmp.bmWidth,dstBmp.bmHeight,m_clTransColor);

	// ��ԭ�豸����
	::SelectObject(hdcSrc, hOldSrcBmp);
	DeleteDC(hdcSrc);
	DeleteObject(hSrcBmp);
	::SelectObject(hdcDst, hOldDstBmp);
	DeleteDC(hdcDst);
	DeleteObject(hDstBmp);
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
void CPolarityDialCtrl::CreateBmp(HBITMAP &hBmp, BYTE **lpBits, const int bmWidth, const int bmHeight, const int bmBitsPixel)
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
///     ��תλͼ
///
/// @param[in]
///     hDstBmp   - ������ת���bmp�ľ��
/// @param[in]
///     srcBmp    - ԴBmp����Ϣ
/// @param[in]
///     srcLpBits - ָ��ԴBmp��DIB���ݵ�ָ��
/// @param[in]
///     degree    - ԴBmpҪ��ת�Ļ��ȣ���12���ӷ��򻡶�Ϊ0������Ϊ��ʱ�뷽�򣬸���Ϊ˳ʱ�뷽��
///
/// @return
///     
///
void CPolarityDialCtrl::RotateBmp(HBITMAP &hDstBmp, const BITMAP &srcBmp, BYTE *srcLpBits, const float degree)
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
	BYTE *dstLpBits = NULL;
	CreateBmp(hDstBmp,&dstLpBits,dstBmpWidth,dstBmpHeight,srcBmp.bmBitsPixel);

	// ��������DC
	CDC *pDC = GetDC();
	HDC hdcDst = CreateCompatibleDC(pDC->m_hDC);
	HBITMAP hbmOld = (HBITMAP)::SelectObject(hdcDst, hDstBmp);
	::FillRect(hdcDst,CRect(0,0,dstBmpWidth,dstBmpHeight),CBrush(m_clTransColor));
	ReleaseDC(pDC);

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

	// ��ԭ�豸����
	::SelectObject(hdcDst, hbmOld);
	DeleteDC(hdcDst);
}
