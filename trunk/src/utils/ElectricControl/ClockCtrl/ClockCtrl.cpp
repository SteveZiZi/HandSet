/// @file
///     ClockCtrl.cpp
///
/// @brief
///     时钟控件
///
/// @note
///     该时钟的时针、分针和秒针的初始位置指向12点钟方向
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
///    |               |           |                                                          |
///
#include "stdafx.h"
#include <math.h>
#include "ClockCtrl.h"

///
/// @brief
///     时钟控件的背景色
///
#define CLOCK_BKGND_COLOR      RGB(0,0,0)        ///< 黑色

///
/// @brief
///     自绘的时钟表盘上的刻度的默认颜色
///
#define CLOCK_DIAL_SCALE_COLOR RGB(0,0,255)      ///< 蓝色

///
/// @brief
///     自绘的时钟指针的默认颜色
///
#define CLOCK_HAND_COLOR       RGB(30,144,255)   ///< 道奇蓝

///
/// @brief
///     控件贴图的透明化的默认颜色
///
#define CLOCK_CTRL_DEFAULT_TRANS_COLOR RGB(0x01,0x02,0x03)

///
/// @brief
///     时钟的时针、分针和秒针与时钟半径的比例
///
#define CLOCK_HOUR_HAND_RATIO  0.5f
#define CLOCK_MIN_HAND_RATIO   0.75f
#define CLOCK_SEC_HAND_RATIO   0.9f

#ifndef PI
#define PI 3.14159265
#endif

///
/// @brief
///     构造函数
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
///     析构函数
///
CClockCtrl::~CClockCtrl()
{
	
}

///
/// @brief
///     设置时钟表盘，表盘的圆心须在Bmp的中心
///
/// @param[in]
///     pBmp - 指向时钟表盘Bmp的指针
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
///     设置时钟指针
///     指针的转轴点须在Bmp的中心，且指针长度应为Bmp的一半，指针指向12点钟方向
///
/// @param[in]
///     pHourBmp - 指向时针Bmp的指针
/// @param[in]
///     pMinBmp  - 指向分针Bmp的指针
/// @param[in]
///     pSecBmp  - 指向秒针Bmp的指针
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
///     初始化控件
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
///     擦除时钟指针
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
///     绘制时间信息
///
/// @param[in]
///     hour - 时
/// @param[in]
///     min  - 分
/// @param[in]
///     sec  - 秒
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
		float fDegree;     ///< 指针旋转的弧度，设12点正的弧度为0
		float fRatio;      ///< 控件上的实际指针与Bmp上的指针的长度的比例
		float fHandLen;    ///< Bmp上的指针长度，默认为bmp高度的一半
		// 绘制时针
		fDegree = float(float(hour*3600+min*60+sec)/3600/12*2*PI);
		m_pHourHandBmp->GetBitmap(&bmp);
		fHandLen = (float)bmp.bmHeight/2;
		fRatio = (CLOCK_HOUR_HAND_RATIO*m_fDialRadius)/fHandLen;
		DrawBmpHand(&m_cForgndDC,m_pHourHandBmp,fDegree,fRatio);

		// 绘制分针
		fDegree = float(float(min*60+sec)/3600*2*PI);
		m_pMinHandBmp->GetBitmap(&bmp);
		fHandLen = (float)bmp.bmHeight/2;
		fRatio  = (CLOCK_MIN_HAND_RATIO*m_fDialRadius)/fHandLen;
		DrawBmpHand(&m_cForgndDC,m_pMinHandBmp,fDegree,fRatio);

		// 绘制秒针
		fDegree = float(float(sec)/60*2*PI);
		m_pSecHandBmp->GetBitmap(&bmp);
		fHandLen = (float)bmp.bmHeight/2;
		fRatio  = (CLOCK_SEC_HAND_RATIO*m_fDialRadius)/fHandLen;
		DrawBmpHand(&m_cForgndDC,m_pSecHandBmp,fDegree,fRatio);	
	}
	else
	{
		// 绘制时针
		DrawDefaultHourHand(&m_cForgndDC,hourTmp,minTmp,secTmp);
		// 绘制分针
		DrawDefaultMinHand(&m_cForgndDC,minTmp,secTmp);
		// 绘制秒针
		DrawDefaultSecHand(&m_cForgndDC,secTmp);
	}
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
///     重绘消息处理函数
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
///     擦除背景消息处理函数
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
///     释放控件资源消息处理函数
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
void CClockCtrl::CreateDCandBmp(const CRect &rt, CDC &dc, CBitmap &bmp, CBitmap **pPOldBmp)
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
void CClockCtrl::DrawBkgnd()
{
	CRect rt;
	GetClientRect(rt);

	// 创建背景DC和位图
	CreateDCandBmp(rt,m_cBkgndDC,m_cBkgndBmp,&m_pOldBkgndBmp);

	// 绘制时钟表盘
	if ( m_pDialBmp != NULL && m_pDialBmp->GetSafeHandle() != NULL )
		DrawBmpDail(&m_cBkgndDC,m_pDialBmp);
	else
		DrawDefaultDial(&m_cBkgndDC);
}

///
/// @brief
///     重置控件前景
///
/// @return
///
void CClockCtrl::ResetForgnd()
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
///     绘制时钟表盘（默认形式，即自绘）
///
/// @param[in]
///     pDC - 设备环境DC的指针
///
/// @return
///     
///
void CClockCtrl::DrawDefaultDial(CDC *pDC)
{
	// 初始化设备环境
	CPen pen(PS_SOLID,0,m_clDialScaleColor);
	CPen *pOldPen = pDC->SelectObject(&pen);
	CBrush brush(m_clDialScaleColor);
	CBrush *pOldBrush = pDC->SelectObject(&brush);

	CRect rt;
	GetClientRect(rt);
	CPoint ptCenter = rt.CenterPoint();
	// 若没设置表盘半径，则默认为Bmp宽度/高度的一半
	if ( m_fDialRadius == 0.0f )
		m_fDialRadius = rt.Width() < rt.Height() ? ((float)rt.Width()/2 - 4.0f) : ((float)rt.Height()/2 - 4.0f);
	float radius = m_fDialRadius;

	// 绘制时钟表盘的刻度
	for ( int i = 0;i < 60;i++ )
	{
		CPoint endPt = ptCenter;
		endPt.Offset(int(radius*sin((i%60)*2*PI/60)+0.5f),
		             int(-radius*cos((i%60)*2*PI/60)+0.5f));
		if ( i%5 == 0 )
		{
			// 绘制表示时钟的矩形刻度
			CRect rcDot(-3,-3,3,3);
			rcDot.OffsetRect(endPt);
			pDC->Rectangle(rcDot);
		}
		else
		{
			// 绘制表示分针和秒针的圆点刻度
			CRect rcDot(-2,-2,2,2);
			rcDot.OffsetRect(endPt);
			pDC->Ellipse(rcDot);
		}
	}

	// 还原设备环境
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	pen.DeleteObject();
	brush.DeleteObject();
}

///
/// @brief
///     绘制时钟时针（默认形式，即自绘）
///
/// @param[in]
///     pDC  - 设备环境DC的指针
/// @param[in]
///     hour - 时
/// @param[in]
///     min  - 分
/// @param[in]
///     sec  - 秒
///
/// @return
///     
///
void CClockCtrl::DrawDefaultHourHand(CDC *pDC,const int hour,const int min, const int sec)
{
	// 初始化设备环境
	CPen pen(PS_SOLID,0,m_clHandColor);
	CPen *pOldPen = pDC->SelectObject(&pen);
	CBrush brush(m_clHandColor);
	CBrush *pOldBrush = pDC->SelectObject(&brush);

	CRect rt;
	GetClientRect(rt);
	CPoint ptCenter = rt.CenterPoint();
	float radius = m_fDialRadius*CLOCK_HOUR_HAND_RATIO;
	// 计算时针的角度
	float fDegree = float(float(hour*3600+min*60+sec)/3600/12*2*PI);

	// 绘制时针
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

	// 还原设备环境
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	pen.DeleteObject();
	brush.DeleteObject();
}

///
/// @brief
///     绘制时钟分针（默认形式，即自绘）
///
/// @param[in]
///     pDC  - 设备环境DC的指针
/// @param[in]
///     min  - 分
/// @param[in]
///     sec  - 秒
///
/// @return
///     
///
void CClockCtrl::DrawDefaultMinHand(CDC *pDC,const int min,const int sec)
{
	// 初始化设备环境
	CPen pen(PS_SOLID,0,m_clHandColor);
	CPen *pOldPen = pDC->SelectObject(&pen);
	CBrush brush(m_clHandColor);
	CBrush *pOldBrush = pDC->SelectObject(&brush);

	CRect rt;
	GetClientRect(rt);
	CPoint ptCenter = rt.CenterPoint();
	float radius = m_fDialRadius*CLOCK_MIN_HAND_RATIO;
	// 计算分针的角度
	float fDegree = float(float(min*60+sec)/3600*2*PI);

	// 绘制分针
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

	// 还原设备环境
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	pen.DeleteObject();
	brush.DeleteObject();
}

///
/// @brief
///     绘制时钟秒针（默认形式，即自绘）
///
/// @param[in]
///     pDC  - 设备环境DC的指针
/// @param[in]
///     sec  - 秒
///
/// @return
///     
///
void CClockCtrl::DrawDefaultSecHand(CDC *pDC,const int sec)
{
	// 初始化设备环境
	CPen pen(PS_SOLID,0,m_clHandColor);
	CPen *pOldPen = pDC->SelectObject(&pen);
	CBrush brush(m_clHandColor);
	CBrush *pOldBrush = pDC->SelectObject(&brush);

	CRect rt;
	GetClientRect(rt);
	CPoint ptCenter = rt.CenterPoint();
	float radius = m_fDialRadius*CLOCK_SEC_HAND_RATIO;
	// 计算秒针的角度
	float fDegree = float(float(sec)/60*2*PI);

	// 绘制秒针
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

	// 还原设备环境
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	pen.DeleteObject();
	brush.DeleteObject();
}

///
/// @brief
///     绘制时钟表盘（贴图形式）
///
/// @param[in]
///     pDC  - 设备环境DC的指针
/// @param[in]
///     pBmp - 指向时钟表盘Bmp的指针
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

	// 初始化缓存DC
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap *pOldBmp = memDC.SelectObject(pBmp);

	// 绘制时钟表盘
	BITMAP bmp;
	pBmp->GetBitmap(&bmp);
	pDC->StretchBlt(0,0,rt.Width(),rt.Height(),&memDC,0,0,bmp.bmWidth,bmp.bmHeight,SRCCOPY);
	memDC.SelectObject(pOldBmp);
	memDC.DeleteDC();

	//  更新控件上的表盘半径，则默认为控件宽度/高度的一半
	if ( m_fDialRadius == 0.0f )
		m_fDialRadius = bmp.bmWidth < bmp.bmHeight ? ((float)rt.Width()/2) : ((float)rt.Height()/2);
}

///
/// @brief
///     绘制时钟指针（贴图形式）
///
/// @param[in]
///     pDC    - 指向设备环境DC的指针
/// @param[in]
///     pBmp   - 指向bmp的指针
/// @param[in]
///     degree - 指针的顺时针方向的旋转弧度，设12点钟方向弧度为0
/// @param[in]
///     ratio  - 实际绘制于控件上的指针与Bmp上的指针的比例
///
/// @return
///     
///
void CClockCtrl::DrawBmpHand(CDC *pDC, CBitmap *pBmp, const float degree, const float ratio)
{
	if ( pBmp == NULL || pBmp->GetSafeHandle() == NULL )
		return;

	// 获取目标位Bmp的信息
	BITMAP bmp;
	pBmp->GetBitmap(&bmp);

	// 创建DIB位图
	HBITMAP hSrcBmp;
	BYTE *srcLpBits = NULL;
	CreateBmp(hSrcBmp,&srcLpBits,bmp.bmWidth,bmp.bmHeight,bmp.bmBitsPixel);
	
	HDC hdcSrc = CreateCompatibleDC(pDC->m_hDC);
	HBITMAP hbmOld = (HBITMAP)::SelectObject(hdcSrc, hSrcBmp);

	// 获取指针位图的内存数据
	CDC srcBmpDC;
	srcBmpDC.CreateCompatibleDC(pDC);
	CBitmap *pOldSrcBmp = srcBmpDC.SelectObject(pBmp);
	if ( BitBlt(hdcSrc,0,0,bmp.bmWidth,bmp.bmHeight,srcBmpDC.m_hDC,0,0,SRCCOPY) == FALSE )
		return;
	srcBmpDC.SelectObject(pOldSrcBmp);
	srcBmpDC.DeleteDC();

	// 旋转位图并缓存到DC
	RotateBmp(pDC,bmp,srcLpBits,-degree,ratio);
	
	// 还原设备环境
	::SelectObject(hdcSrc, hbmOld);
	DeleteDC(hdcSrc);
	DeleteObject(hSrcBmp);

	// 下面代码是通过操作DC来完成位图旋转，效率较慢
#if 0
	// 设左上角坐标为（0，0），并以其为圆心，计算顺时针旋转后的Bmp的矩形域的其余3点
	float sinVal = sin(degree);
	float cosVal = cos(degree);
	float x1     = bmp.bmWidth * cosVal;
	float y1     = bmp.bmWidth * sinVal;
	float x2     = bmp.bmWidth * cosVal - bmp.bmHeight * sinVal;
	float y2     = bmp.bmWidth * sinVal + bmp.bmHeight * cosVal;
	float x3     = -bmp.bmHeight *sinVal;
	float y3     = bmp.bmHeight  *cosVal;

	// 计算顺时针旋转后的Bmp的矩形域的宽度和高度
	float minWidth  = min(min(0,x1),min(x2,x3));
	float minHeight = min(min(0,y1),min(y2,y3));
	float maxWidth  = max(max(0,x1),max(x2,x3));
	float maxHeight = max(max(0,y1),max(y2,y3));
	int dstBmpWidth  = abs(maxWidth - minWidth + 0.5f);
	int dstBmpHeight = abs(maxHeight - minHeight + 0.5f);

	// 初始化原Bmp的缓存DC
	CDC srcBmpDC;
	srcBmpDC.CreateCompatibleDC(pDC);
	CBitmap *pOldSrcBmp = srcBmpDC.SelectObject(pBmp);

	// 初始化旋转后的Bmp的缓存DC
	CDC dstBmpDC;
	CBitmap dstBmp;
	dstBmpDC.CreateCompatibleDC(pDC);
	dstBmp.CreateCompatibleBitmap(pDC,dstBmpWidth,dstBmpHeight);
	CBitmap *pOldDstBmp = dstBmpDC.SelectObject(&dstBmp);
	::FillRect(dstBmpDC,CRect(0,0,dstBmpWidth,dstBmpHeight),CBrush(RGB(255,255,255)));

	// 根据原Bmp和目标Bmp的位置映射进行逐点拷贝
	for( int i = 0; i < dstBmpHeight; i++ )
	{
		for( int j = 0; j  <  dstBmpWidth; j++ )
		{
			// 计算旋转后的Bmp的各点的在原bmp的坐标位置，即逆时针旋转后的位置
			// a为旋转后的角度，b为旋转前的角度
			// x=r*cos(a-b)=r*(cos(a)*cos(b)+sin(a)*sin(b))=X0*cos(b)+Y0*sin(b);
			// y=r*sin(a-b)=r*(sin(a)*cos(b)-cos(a)*sin(b))=Y0*cos(b)-X0*sin(b);
			float srcX = (i + minHeight) * sinVal + (j + minWidth) * cosVal + 0.5f;
			float srcY = (i + minHeight) * cosVal - (j + minWidth) * sinVal + 0.5f;
			// 在原图范围内则拷贝
			if((srcX >= 0) && (srcX < bmp.bmWidth) &&(srcY >=  0) && (srcY < bmp.bmHeight)) 
			{ 
				BitBlt(dstBmpDC, j, i, 1, 1, srcBmpDC.m_hDC, (int)srcX, (int)srcY, SRCCOPY);
			}
		}
	}

	// 计算指针在控件上的实际宽度和高度
	int width  = int(dstBmpWidth*ratio);
	int height = int(dstBmpHeight*ratio);

	// 计算指针在表盘上的实际矩形域
	// 一矩形的4个端点在另一矩形的边上，则中心点相同
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

	// 释放资源
	srcBmpDC.SelectObject(pOldSrcBmp);
	srcBmpDC.DeleteDC();
	dstBmpDC.SelectObject(pOldDstBmp);
	dstBmpDC.DeleteDC();
	dstBmp.DeleteObject();
#endif
}

///
/// @brief
///     创建DIB位图
///
/// @param[in]
///     hBmp        - 用于保存所创建的位图的句柄
/// @param[in]
///     lpBits      - 用于保存一个指向DIB位数据值的指针，其内存空间由系统分配和释放
/// @param[in]
///     bmWidth     - 所要创建的DIB位图的宽度，单位是象素
/// @param[in]
///     bmHeight    - 所要创建的DIB位图的高度，单位是象素
/// @param[in]
///     bmBitsPixel - 所要创建的DIB位图的像素点的位数
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
///     旋转位图并缓存到DC
///
/// @param[in]
///     pDC       - 指向设备环境DC的指针
/// @param[in]
///     srcBmp    - 源Bmp的信息
/// @param[in]
///     srcLpBits - 指向源Bmp的DIB数据的指针
/// @param[in]
///     degree    - 源Bmp要旋转的弧度，设12点钟方向弧度为0，正则为逆时针方向，负则为顺时针方向
/// @param[in]
///     ratio     - 实际绘制于控件上的指针与源Bmp上的指针的比例
///
/// @return
///     
///
void CClockCtrl::RotateBmp(CDC *pDC, const BITMAP &srcBmp, BYTE *srcLpBits, const float degree, const float ratio)
{
	// 设左上角坐标为（0，0），并以其为圆心，计算旋转degree弧度后的Bmp的矩形域的其余3点
	// 注意取得的位图是从下到上，从左到右，即第1个数据是图象最下面一行的左边第一个象素，依次下去
	float sinVal = (float)(sin(degree));
	float cosVal = (float)(cos(degree));
	float x1     = srcBmp.bmWidth * cosVal;
	float y1     = srcBmp.bmWidth * sinVal;
	float x2     = srcBmp.bmWidth * cosVal - srcBmp.bmHeight * sinVal;
	float y2     = srcBmp.bmWidth * sinVal + srcBmp.bmHeight * cosVal;
	float x3     = -srcBmp.bmHeight *sinVal;
	float y3     = srcBmp.bmHeight  *cosVal;

	// 计算旋转后的Bmp的矩形域的宽度和高度
	float minWidth  = min(min(0,x1),min(x2,x3));
	float minHeight = min(min(0,y1),min(y2,y3));
	float maxWidth  = max(max(0,x1),max(x2,x3));
	float maxHeight = max(max(0,y1),max(y2,y3));
	int dstBmpWidth  = abs(int(maxWidth - minWidth + 0.5f));
	int dstBmpHeight = abs(int(maxHeight - minHeight + 0.5f));
	int dstBmpEffWidth = (((srcBmp.bmBitsPixel*dstBmpWidth)+31)/32);

	// 创建记录旋转后的数据的DIB位图
	HBITMAP hDstBmp;
	BYTE *dstLpBits = NULL;
	CreateBmp(hDstBmp,&dstLpBits,dstBmpWidth,dstBmpHeight,srcBmp.bmBitsPixel);

	// 创建缓存DC
	HDC hdcDst = CreateCompatibleDC(pDC->m_hDC);
	HBITMAP hbmOld = (HBITMAP)::SelectObject(hdcDst, hDstBmp);
	::FillRect(hdcDst,CRect(0,0,dstBmpWidth,dstBmpHeight),CBrush(RGB(255,255,255)));

 	// 根据原Bmp和目标Bmp的位置映射进行逐点拷贝
	for( int y = 0; y < dstBmpHeight; y++ )
	{
		for( int x = 0; x  <  dstBmpWidth; x++ )
		{
			// 计算旋转后的Bmp的各点所对应的原bmp的坐标位置
			// a为旋转后的角度，b为旋转前的角度
			// x=r*cos(a-b)=r*(cos(a)*cos(b)+sin(a)*sin(b))=X0*cos(b)+Y0*sin(b);
			// y=r*sin(a-b)=r*(sin(a)*cos(b)-cos(a)*sin(b))=Y0*cos(b)-X0*sin(b);
			int srcX = int((y + minHeight) * sinVal + (x + minWidth) * cosVal + 0.5f);
			int srcY = int((y + minHeight) * cosVal - (x + minWidth) * sinVal + 0.5f);
			// 在原图范围内则拷贝，屏幕色深需大于或等于8位
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

	// 计算指针在控件上的实际宽度和高度
	int width  = int(dstBmpWidth*ratio);
	int height = int(dstBmpHeight*ratio);

	// 计算指针在控件上的实际矩形域
	// 一矩形的4个端点在另一矩形的边上，则中心点相同
	CRect rt;
	GetClientRect(rt);
	CRect handRt;
	handRt.left=rt.CenterPoint().x-int(width/2);
	handRt.right=handRt.left+width;
	handRt.top=rt.CenterPoint().y-height/2;
	handRt.bottom=handRt.top+height;
	TransparentBlt(pDC->m_hDC,handRt.left,handRt.top,handRt.Width(),handRt.Height(),
		hdcDst,0,0,dstBmpWidth,dstBmpHeight,m_clTransColor);

	// 还原设备环境
	::SelectObject(hdcDst, hbmOld);
	DeleteDC(hdcDst);
	DeleteObject(hDstBmp);
}
