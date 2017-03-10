/// @file
///     OnOffStateCtrl.cpp
///
/// @brief
///     ����DI״̬�ؼ�
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
///    |2013.07.08     |1.0.1.708  |������״̬�Ļ��ƽӿڷ���Ϊ���õ���״̬����ʽ��һ���Ի���������|
///    |2013.07.08     |1.0.1.708  |������ȫ������DI״̬�Ĳ����������ƽӿ��У������ⲿ����        |
///    |2013.07.09     |1.0.1.709  |��ӿ���DI״̬Bmp����ʾ�����С���ã��Լ�Bmp���Ի�߿�Ŀɼ�������|
///
#include "stdafx.h"
#include "OnOffStateCtrl.h"

///
/// @brief
///     �ؼ��ı�����ɫ
///
#define OOS_CTRL_BKGND_COLOR      RGB(0,0,0)           ///< ��ɫ

#define OOS_BMP_BORDER_COLOR      RGB(128,128,135)
#define OOS_BMP_BORDER_STYLE      PS_SOLID
#define OOS_BMP_BORDER_WIDTH      1

#define OOS_FONT_STYLE            _T("΢���ź�")
#define OOS_ONOFF_DESC_FONT_COLOR RGB(128,128,135)
#define OOS_ONOFF_DESC_FONT_SIZE  25

#ifndef PI
#define PI 3.1415926
#endif

IMPLEMENT_DYNAMIC(COnOffStateCtrl, CStatic)

///
/// @brief
///     ���캯��
///
COnOffStateCtrl::COnOffStateCtrl()
: m_pOldBkgndBmp(NULL)
, m_pOldForgndBmp(NULL)
, m_clBkgndColor(OOS_CTRL_BKGND_COLOR)
, m_clBorderColor(OOS_BMP_BORDER_COLOR)
, m_clOnOffDescColor(OOS_ONOFF_DESC_FONT_COLOR)
, m_iXOnOffDICount(1)
, m_iYOnOffDICount(1)
, m_iLeftMargin(4)
, m_iRightMargin(4)
, m_iTopMargin(4)
, m_iBottomMargin(5)
, m_fXOnOffDISpace(0.0f)
, m_fYOnOffDISpace(0.0f)
, m_bShowOnOffBoard(TRUE)
{
	m_cOnOffDescFont.CreateFont(OOS_ONOFF_DESC_FONT_SIZE,0,0,0,FW_NORMAL,FALSE,0,0,0,0,0,0,0,OOS_FONT_STYLE);
	m_vOnOffStateInfo.resize(m_iXOnOffDICount*m_iYOnOffDICount);
	ONOFF_STATE_INFO info = {_T(""),0};
	for ( int i = 0; i < m_iXOnOffDICount*m_iYOnOffDICount; i++ )
	{
		m_vOnOffStateInfo.at(i) = info;
	}
}

///
/// @brief
///     ��������
///
COnOffStateCtrl::~COnOffStateCtrl()
{

}

///
/// @brief
///     ���ÿؼ��п���DI״̬������
///
void COnOffStateCtrl::SetOnOffDICount(const int xCount, const int yCount)
{
	m_iXOnOffDICount = xCount;
	m_iYOnOffDICount = yCount;

	// ���ÿ���״̬��Ϣ
	int prevSz = (int)m_vOnOffStateInfo.size();
	if ( prevSz < xCount*yCount )
	{
		m_vOnOffStateInfo.resize(xCount*yCount);
		ONOFF_STATE_INFO info = {_T(""),0};
		for ( int i = prevSz; i < xCount*yCount; i++ )
		{
			m_vOnOffStateInfo[i] = info;
		}
	}
}

///
/// @brief
///     ���ÿؼ��Ͽ���DI״̬��������Ϣ������
///
/// @param[in]
///     pFont - ָ�������ָ��
///
/// @return
///
///
void COnOffStateCtrl::SetOnOffDescFont(CFont *pFont)
{
	if ( pFont == NULL && pFont->GetSafeHandle() != NULL )
		return;
	m_cOnOffDescFont.DeleteObject();
	LOGFONT logFont;
	pFont->GetLogFont(&logFont);
	m_cOnOffDescFont.CreateFontIndirect(&logFont);
}

///
/// @brief
///     ���ÿ���DI״̬����ʽ����˳��ų�����Χ�����Ĳ�����Ϊ��
///
/// @param[in]
///     idxSeq - ��ʽ��ID����0��ʼ
/// @param[in]
///     pBmp   - ָ�򿪹�DI״̬����ʽBmp��ָ��
///
/// @return
///
///
void COnOffStateCtrl::SetOnOffStyle(const size_t idxSeq, CBitmap *pBmp)
{
	if ( idxSeq >= m_vOnOffStyleBmp.size() )
	{
		for ( size_t i = m_vOnOffStyleBmp.size(); i <= idxSeq; i++ )
		{
			if ( i == idxSeq )
			{
				m_vOnOffStyleBmp.push_back(pBmp);
				break;
			}
			// ��������Ĭ��Ϊ��
			m_vOnOffStyleBmp.push_back(NULL);
		}
	}
	else
	{
		// ������ʽ
		m_vOnOffStyleBmp.at(idxSeq) = pBmp;
	}
}

///
/// @brief
///     ��ʼ���ؼ�
///
/// @return
///
///
void COnOffStateCtrl::Init()
{
	DrawBkgnd();
	ResetForgnd();
}

///
/// @brief
///     ����ȫ������DI״̬
///
/// @return
///
///
void COnOffStateCtrl::EraseOnOffState()
{
	CRect rt;
	GetClientRect(rt);
	m_cForgndDC.BitBlt(0, 0, rt.Width(), rt.Height(),
		&m_cBkgndDC, 0, 0, SRCCOPY);
}

///
/// @brief
///     ����ѡ�����ʽID����ĳһ����DI״̬������ʽ����ǰ����Ƶĵ�����ʾ��
///
/// @param[in]
///     idxSeq  - ����DI״̬��˳��ţ��������ң��ٴ��ϵ��£�����0��ʼ
/// @param[in]
///     styleID - ����DI״̬����ʽID
/// @param[in]
///     desc    - ����DI״̬��������Ϣ
///
/// @return
///     TRUE - �ɹ���FALSE - ʧ��
///
BOOL COnOffStateCtrl::SetOnOffState(const size_t idxSeq, const size_t styleID, const CString &desc)
{
	if ( idxSeq >= m_vOnOffStateInfo.size() )
		return FALSE;

	ONOFF_STATE_INFO info = {desc,styleID};
	m_vOnOffStateInfo.at(idxSeq) = info;
	return TRUE;
}

///
/// @brief
///     ����ѡ�����ʽID����ĳһ����DI״̬������ʽ����ǰ����Ƶĵ�����ʾ��
///
/// @param[in]
///     idxSeq  - ����DI״̬��˳��ţ��������ң��ٴ��ϵ��£�����0��ʼ
/// @param[in]
///     styleID - ����DI״̬����ʽID
/// @param[in]
///     desc    - ����DI״̬��������Ϣ
///
/// @return
///
///
void COnOffStateCtrl::DrawOnOffState()
{
	// ����ǰ��
	EraseOnOffState();

	CFont *pOldFont = m_cForgndDC.SelectObject(&m_cOnOffDescFont);
	COLORREF oldColor = m_cForgndDC.SetTextColor(m_clOnOffDescColor);
	int oldMode = m_cForgndDC.SetBkMode(TRANSPARENT);

	CPen pen, *pOldPen;
	pen.CreatePen(OOS_BMP_BORDER_STYLE,OOS_BMP_BORDER_WIDTH,m_clBorderColor);
	pOldPen = m_cForgndDC.SelectObject(&pen);
	
	

	for ( int i = 0; i < m_iXOnOffDICount; i++ )
	{
		for ( int j = 0; j < m_iYOnOffDICount; j++ )
		{
			// ���㵱ǰҪ���ƵĿ���DI״̬�ľ�����
			CRect diArea = m_rtOnOffDIArea ;
			diArea.OffsetRect(int(m_fXOnOffDISpace*i),int(m_fYOnOffDISpace*j));

			// ����������Ϣ
			CString desc = m_vOnOffStateInfo.at(i+m_iXOnOffDICount*j).csDesc;
			CSize sz = m_cForgndDC.GetTextExtent(desc);
			int left = diArea.CenterPoint().x - sz.cx/2;
			int top  = diArea.bottom - sz.cy;
			m_cForgndDC.ExtTextOut(left,top,ETO_OPAQUE,NULL,desc,desc.GetLength(),NULL);

			// ���㿪��DI״̬��ͼ�ľ�����
// 			int bmpAreaWidth = diArea.Width() < (diArea.Height()-sz.cy-2) ? diArea.Width() : (diArea.Height()-sz.cy-2);
// 			CRect bmpArea(diArea.CenterPoint().x - bmpAreaWidth/2,
// 				diArea.bottom - bmpAreaWidth - sz.cy - 2,
// 				diArea.CenterPoint().x + bmpAreaWidth/2,
// 				diArea.bottom - sz.cy - 2);
			CRect bmpArea( diArea.CenterPoint().x - m_rtOnOffShowSize.Width()/2,
				           diArea.bottom - m_rtOnOffShowSize.Height() - sz.cy - 2,
						   diArea.CenterPoint().x + m_rtOnOffShowSize.Width()/2,
						   diArea.bottom - sz.cy - 2 );

			// ���ƿ���DI״̬
			size_t styleID = m_vOnOffStateInfo.at(i+m_iXOnOffDICount*j).styleID;
			if ( styleID < m_vOnOffStyleBmp.size() )
			{
				CBitmap *pBmp = m_vOnOffStyleBmp[styleID];
				if ( pBmp != NULL && pBmp->GetSafeHandle() != NULL )
				{
					CDC memDC;
					memDC.CreateCompatibleDC(&m_cForgndDC);
					CBitmap *pOldBmp = memDC.SelectObject(pBmp);
					BITMAP bmp;
					pBmp->GetBitmap(&bmp);
					m_cForgndDC.StretchBlt(bmpArea.left ,bmpArea.top,bmpArea.Width(),bmpArea.Height(),
						&memDC,0,0,bmp.bmWidth,bmp.bmHeight,SRCCOPY);
					memDC.SelectObject(pOldBmp);
					memDC.DeleteDC();

				}
			}

			// ���ƿ���DI״̬�ı߿�
			if ( m_bShowOnOffBoard )
			{
				m_cForgndDC.MoveTo(bmpArea.TopLeft());
				m_cForgndDC.LineTo(bmpArea.right,bmpArea.top);
				m_cForgndDC.MoveTo(bmpArea.TopLeft());
				m_cForgndDC.LineTo(bmpArea.left,bmpArea.bottom);
				m_cForgndDC.MoveTo(bmpArea.BottomRight());
				m_cForgndDC.LineTo(bmpArea.left,bmpArea.bottom);
				m_cForgndDC.MoveTo(bmpArea.BottomRight());
				m_cForgndDC.LineTo(bmpArea.right,bmpArea.top);
			}
		}
	}

	m_cForgndDC.SetBkMode(oldMode);
	m_cForgndDC.SetTextColor(oldColor);
	m_cForgndDC.SelectObject(pOldFont);

	m_cForgndDC.SelectObject(pOldPen);
	pen.DeleteObject();
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
void COnOffStateCtrl::Flush(CDC *pDC)
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

BEGIN_MESSAGE_MAP(COnOffStateCtrl, CStatic)
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
void COnOffStateCtrl::OnPaint()
{
// 	PAINTSTRUCT ps;
// 	CRect rt;
// 	GetClientRect(rt);
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
void COnOffStateCtrl::OnDestroy()
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
BOOL COnOffStateCtrl::OnEraseBkgnd(CDC* pDC)
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
void COnOffStateCtrl::CreateDCandBmp(const CRect &rt, CDC &dc, CBitmap &bmp, CBitmap **pPOldBmp)
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
void COnOffStateCtrl::DrawBkgnd( )
{
	CRect rt;
	GetClientRect(rt);

	// ��������DC��λͼ
	CreateDCandBmp(rt,m_cBkgndDC,m_cBkgndBmp,&m_pOldBkgndBmp);

	// �������
	m_cBkgndDC.FillSolidRect(0,0,rt.Width(),rt.Height(),m_clBkgndColor);

	UpdateOnOffBmpArea(&m_cBkgndDC);

}

///
/// @brief
///     ���ÿؼ�ǰ��
///
/// @return
///
void  COnOffStateCtrl::ResetForgnd()
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
void COnOffStateCtrl::Release()
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
	m_cOnOffDescFont.DeleteObject();
}

///
/// @brief
///     ���㿪��DI״̬��ͼ����ľ�����
///
/// @param[in]
///     pDC - �豸����DC��ָ��
///
/// @return
///     
///
void COnOffStateCtrl::UpdateOnOffBmpArea(CDC *pDC)
{
	CRect rt;
	GetClientRect(rt);

	m_fXOnOffDISpace = (float)(rt.Width() - m_iLeftMargin)/(float)m_iXOnOffDICount;
	m_fYOnOffDISpace = (float)rt.Height()/(float)m_iYOnOffDICount;

	m_rtOnOffDIArea.SetRect(m_iLeftMargin,m_iTopMargin,
		int(m_fXOnOffDISpace-m_iRightMargin),
		int(m_fYOnOffDISpace-m_iBottomMargin));
}
