/// @file
///     PhasorGramCtrl.cpp
///
/// @brief
///     相量图控件
///
/// @note
///     该相量图为叠加显示
///     控件标题位于控件的右上角
///     相量描述信息位于相量的终点处
///
/// Copyright (c) 2016 广州炫通电气科技有限公司软件部
///
/// 作者：
///    chao  2013.4.12
///
/// 版本：
///    1.0.x.x
/// 
/// 修改历史：
///    ： 时间         : 版本      :  详细信息    
///    :-------------- :-----------:----------------------------------------------------------
///    |2013.05.09     |1.0.0.509  |修改当相量长度小于10像素，则该相量以圆点表示              |
///    |2013.05.09     |1.0.0.509  |修改各相量描述信息的重叠判断和处理                        |
///    |2013.07.05     |1.0.1.705  |将相量长度的游标刻度的绘制移至控件的前景部分              |
///    |2013.07.09     |1.0.1.709  |添加对相量长度的上下限判断                                |
///    |2013.07.16     |1.0.1.716  |将相量长度的游标刻度的绘制改在擦除前景时重新绘制          |
///    |2013.08.07     |1.0.1.807  |修改字体设置接口中对入参的有效性判断                      |
///    |2013.11.19     |1.0.1.1119 |修改各相量描述信息的重叠判断及处理                        |
///
#include "stdafx.h"
#include <math.h>
#include <float.h>
#include <atltypes.h>
#include "PhasorGramCtrl.h"

///
/// @brief
///     控件的背景颜色
///
#define PSG_CTRL_BKGND_COLOR      RGB(0,0,0)           ///< 黑色

///
/// @brief
///     字体的默认设置，包括字体样式以及标题、度数刻度和相量描述信息的字体大小和颜色
///
#define PSG_FONT_STYLE            _T("微软雅黑")
#define PSG_TITLE_FONT_SIZE       20
/***************************************************************************************/
#define PSG_TITLE_FONT_COLOR      RGB(0xbe,0xbd,0xc0)     ///< 白色
#define PSG_SCALE_FONT_SIZE       16

#define PSG_SCALE_FONT_COLOR      RGB(0xbe,0xbd,0xc0)     ///< 暖灰色
//#define PSG_SCALE_FONT_COLOR      RGB(128,128,135)     ///< 暖灰色
#define PSG_PHASORDESC_FONT_SIZE  20

///
/// @brief
///     相量显示区域的默认设置：外圆、内圆和圆半径的线条的类型、宽度和颜色以及圆的间隔
///
/****************************************************************************************/
#define PSG_EXCIRCLE_LINE_STYLE PS_SOLID
#define PSG_EXCIRCLE_LINE_WIDTH 2
#define PSG_EXCIRCLE_LINE_COLOR RGB(0xae,0xae,0xae)         ///< 暖灰色
#define PSG_INCIRCLE_LINE_STYLE PS_SOLID
#define PSG_INCIRCLE_LINE_WIDTH 1
#define PSG_INCIRCLE_LINE_COLOR RGB(0xae,0xae,0xae)         ///< 冷灰色
#define PSG_RADIUS_LINE_STYLE   PS_DASH
#define PSG_RADIUS_LINE_WIDTH   1
#define PSG_RADIUS_LINE_COLOR   RGB(0xae,0xae,0xae)         ///< 冷灰色

///
/// @brief
///     相量显示区域圆的数量
///
#define PSG_CIRCLE_COUNT   4

///
/// @brief
///     相量显示区域半数的扇形（弧度相同）数量
///
#define PSG_HALF_SECTOR_COUNT   6

///
/// @brief
///     相量线条的默认设置：线条的类型、宽度和前6条的默认颜色(红、黄、森林绿、猩红、橙、草绿)
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
///     构造函数
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
///     析构函数
///
CPhasorGramCtrl::~CPhasorGramCtrl()
{
	
}

///
/// @brief
///     设置相量图中某一顺序号的相量的颜色
///
/// @param[in]
///     idxSeq - 波形的顺序号，从0开始
/// @param[in]
///     clr    - 颜色值
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
///     设置控件标题的字体
///
/// @param[in]
///     pFont - 指向字体的指针
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
///     设置控件标尺刻度的字体
///
/// @param[in]
///     pFont - 指向字体的指针
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
///     设置控件相量描述信息的字体
///
/// @param[in]
///     pFont - 指向字体的指针
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
///     设置控件相量的扩展描述信息的字体
///
/// @param[in]
///     pFont - 指向字体的指针
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
///     初始化控件
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
///     擦除相量显示区域的全部相量信息
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

	// 画相量图的标尺
	DrawScale(&m_cForgndDC);
}

///
/// @brief
///     绘制相量：叠加式（与前面绘制的相量叠加显示）
///
/// @param[in]
///     lenBuf    - 保存相量长度值信息的缓存
/// @param[in]
///     degreeBuf - 保存相量角度值信息的缓存
/// @param[in]
///     descBuf   - 保存相量的描述信息的缓存
/// @param[in]
///     bufSize   - 缓存大小
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
	// 计算半径的每个点的步长
	float fStepWidth = (float)m_rtPhasorArea.Width()/2.0f/(float)PSG_CIRCLE_COUNT/m_fLenScaleInterval;

	// 计算相量的端点及绘制相量
	for (size_t i=0; i<bufSize; i++)
	{
		// 相量长度上下限判断
		float phaseLen = lenBuf[i];
		if ( _finite(lenBuf[i]) == FALSE || lenBuf[i] < 0.0f )
			phaseLen = 0;
		if ( lenBuf[i] > (float)PSG_CIRCLE_COUNT*m_fLenScaleInterval )
			phaseLen = (float)PSG_CIRCLE_COUNT*m_fLenScaleInterval;

		// 计算相量终点
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
		{// 相量长度小于10像素，则用圆点表示
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
///     绘制相量的扩展描述信息：叠加式（与前面绘制的内容叠加显示）
///
/// @param[in]
///     leftTop   - 描述信息的绘制区域的左上角坐标
/// @param[in]
///     descClr   - 描述信息的颜色
/// @param[in]
///     descBuf   - 保存相量的扩展描述信息的缓存
/// @param[in]
///     bufSize   - 缓存大小
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
///     绘制相量的扩展描述信息：叠加式（与前面绘制的内容叠加显示）
///
/// @param[in]
///     csDesc      描述信息
/// @param[in]
///     rcRect    - 描述信息的区域
/// @param[in]
///     crColor   - 描述信息的颜色
/// @param[in]
///     nFormat   - 输出格式
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
///     强制刷新：将当前内存设备缓冲中的内容刷新到目标内存设备环境（DC）中
///
/// @param[in]
///     pDC - 指向目标DC的指针；为NULL时，则为默认的DC
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
///     重绘消息处理函数
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
///     释放控件资源消息处理函数
///
/// @return
///
void CPhasorGramCtrl::OnDestroy()
{
	CStatic::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	Release();
}

///
/// @brief
///     擦除背景消息处理函数
///
/// @return
///
///
BOOL CPhasorGramCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return TRUE;
}

///
/// @brief
///     创建内存DC和位图：如果位图已经存在, 则不用重复创建
///
/// @param[in]
///     rt       - 要创建的DC和位图的大小
/// @param[in]
///     dc       - 要创建的DC
/// @param[in]
///     bmp      - 要创建的位图
/// @param[in]
///     pPOldBmp - 指向创建的DC替换出来的原始Bmp的指针的指针
///
void CPhasorGramCtrl::CreateDCandBmp(const CRect &rt, CDC &dc, CBitmap &bmp, CBitmap **pPOldBmp)
{
	if (NULL != dc.GetSafeHdc())
	{
		BITMAP bmpInfo;
		bmp.GetBitmap(&bmpInfo);
		
		if ( rt.Width() != bmpInfo.bmWidth || rt.Height() != bmpInfo.bmHeight )
		{
			// 控件大小发生改变，则释放旧的背景DC和位图
			dc.SelectObject(*pPOldBmp);
			dc.DeleteDC();
			bmp.DeleteObject();
		}
		else
		{
			// 控件大小没改变，则返回
			return;
		}
	}

	// 新建背景DC和位图
	CDC *pDC = GetDC();
	dc.CreateCompatibleDC(pDC);
	bmp.CreateCompatibleBitmap(pDC,rt.Width(),rt.Height());
	*pPOldBmp = dc.SelectObject(&bmp);
	ReleaseDC(pDC);
}

///
/// @brief
///     绘制控件背景
///
/// @return
///
void CPhasorGramCtrl::DrawBkgnd( )
{
	CRect rt;
	GetClientRect(rt);

	// 创建背景DC和位图
	CreateDCandBmp(rt,m_cBkgndDC,m_cBkgndBmp,&m_pOldBkgndBmp);

	// 背景填充
	m_cBkgndDC.FillSolidRect(0,0,rt.Width(),rt.Height(),m_clBkgndColor);

	// 更新相量图的绘图范围
	UpdatePhasorAreaRange(&m_cBkgndDC);

	// 画相量图的圆圈
	DrawCircle(&m_cBkgndDC);

	// 画相量图的半径
	DrawRadius(&m_cBkgndDC);

// 	// 画相量图的标尺
// 	DrawScale(&m_cBkgndDC);

	// 画控件标题
	DrawTitle(&m_cBkgndDC);
}

///
/// @brief
///     重置控件前景
///
/// @return
///
void  CPhasorGramCtrl::ResetForgnd()
{
	CRect rt;
	GetClientRect(rt);

	// 创建前景DC和位图
	CreateDCandBmp(rt,m_cForgndDC,m_cForgndBmp,&m_pOldForgndBmp);

	// 将背景dc拷贝到前景dc
	m_cForgndDC.BitBlt(0,0,rt.Width(),rt.Height(),&m_cBkgndDC,0,0,SRCCOPY);
}

///
/// @brief
///     控件资源释放函数
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
///     更新相量显示区域的矩形域
///
/// @param[in]
///     pDC - 设备环境DC的指针
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
		CSize scaleValMaxSz = pDC->GetTextExtent(L"180.0°");
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
///     绘制相量显示区域的圆圈，需先更新相量显示区域的矩形域
///
/// @param[in]
///     pDC - 设备环境DC的指针
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

	//画外圆
	pen.CreatePen(PSG_EXCIRCLE_LINE_STYLE,PSG_EXCIRCLE_LINE_WIDTH,m_clExCircleColor);
	pOldPen = pDC->SelectObject(&pen);
	pDC->Ellipse(&m_rtPhasorArea);
	pen.DeleteObject();

	//画内圆
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

    //绘制背景
    brush.CreateSolidBrush(m_clBkgndColor);
    pOldBrush = pDC->SelectObject(&brush);

    CBrush bkBrush(PSG_CTRL_BKGND_COLOR);
    pOldBrush = pDC->SelectObject(&bkBrush);
    //画外圆
    pen.CreatePen(PSG_EXCIRCLE_LINE_STYLE,PSG_EXCIRCLE_LINE_WIDTH,m_clExCircleColor);
    pOldPen = pDC->SelectObject(&pen);
    pDC->Ellipse(&m_rtPhasorArea);
    pen.DeleteObject();


    //画内圆
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
///     绘制相量显示区域的半径，需先更新相量显示区域的矩形域
///
/// @param[in]
///     pDC - 设备环境DC的指针
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
///     绘制相量显示区域的标尺刻度，需先更新相量显示区域的矩形域
///
/// @param[in]
///     pDC - 设备环境DC的指针
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
	// 绘制相量角度标尺
	if ( m_bShowDegreeScaleVal )
	{
		for ( int i = 0; i < PSG_HALF_SECTOR_COUNT*2; i++ )
		{
            /**************************************************************/
            float val = float(180/PSG_HALF_SECTOR_COUNT*i);
            CPoint pt(int(circleCentre.x+radiuLen*cos(val*2*PI/360.0)),
                int(circleCentre.y-radiuLen*sin(val*2*PI/360.0)));
            CString str;

            //str.Format(L"%0.1f°", val);
            str.Format(L"%0.0f°", val);

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

	// 绘制相量长度标尺
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

            //计算上下位置
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
///     绘制控件的标题（位于控件的右上角）
///
/// @param[in]
///     pDC - 设备环境DC的指针
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
///     绘制相量的箭头
///
/// @param[in]
///     pDC     - 设备环境DC的指针
/// @param[in]
///     beginPt - 相量的起点
/// @param[in]
///     endPt   - 相量的终点
/// @param[in]
///     idxSeq  - 相量的顺序号
///
/// @return
///     
///
void CPhasorGramCtrl::DrawPhasorArrows(CDC *pDC, const CPoint &beginPt, const CPoint &endPt, const size_t idxSeq)
{
	//画相量线段
	CPen  pen, *pOldPen;
	pen.CreatePen(PSG_PHASOR_LINE_STYLE,PSG_PHASOR_LINE_WIDTH,m_clPhasorColor[idxSeq]);
	pOldPen = pDC->SelectObject(&pen);
	pDC->MoveTo(beginPt);
	pDC->LineTo(endPt);

	// 计算相量末端箭头的端点值
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

	// 画相量末端的箭头
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
///     绘制相量的描述信息（位于相量的终点处）
///
/// @param[in]
///     pDC    - 设备环境DC的指针
/// @param[in]
///     endPt  - 相量的终点
/// @param[in]
///     desc   - 相量的描述信息
/// @param[in]
///     idxSeq - 相量的顺序号
///
/// @return
///     
///
void CPhasorGramCtrl::DrawPhasorDesc(CDC *pDC,const CPoint *endPt, const CString *desc, const size_t bufSize)
{
	CFont *pOldFont = pDC->SelectObject(&m_cPhasorDescFont);
	COLORREF oldClr = pDC->SetTextColor(m_clPhasorColor[0]);
	int oldMode = pDC->SetBkMode(TRANSPARENT);

	// 计算描述信息的矩形域
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

	// 根据矩形域的左上角的点坐标分别按X方向进行排序
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

	// 计算各描述信息矩形域在X方向上靠左且最接近中间的矩形的索引
	int  xMidIdx = (int)bufSize - 1;
	for ( int i = 0; i < (int)bufSize; i++ )
	{
		if ( descRt[xSortIdx[i]].left >= (m_rtPhasorArea.Width()/2+m_rtPhasorArea.left) )
		{
			xMidIdx = i-1;
			break;
		}
	}

	// 左半部分的矩形域的重叠处理
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
	// 右半部分的矩形域的重叠处理
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

	// 绘制描述信息
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
///     判断两矩形是否相交
///
/// @param[in]
///     rt1 - 矩形1
/// @param[in]
///     rt2 - 矩形2
///
/// @return
///     true - 相交； false - 不相交
///
bool CPhasorGramCtrl::IsIntersect(const CRect &rt1,const CRect &rt2)
{
	if(rt1.right <= rt2.left) return false;
	if(rt1.left >= rt2.right ) return false;
	if(rt1.bottom <= rt2.top ) return false;
	if(rt1.top >= rt2.bottom ) return false;

	return true;
}
