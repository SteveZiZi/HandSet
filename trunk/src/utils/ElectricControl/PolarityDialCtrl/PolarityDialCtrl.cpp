/// @file
///     PolarityDialCtrl.cpp
///
/// @brief
///     极性表盘控件
///
/// @note
///     该表盘指针的初始位置指向12点钟方向
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
#include <float.h>
#include "PolarityDialCtrl.h"

///
/// @brief
///     控件的背景颜色
///
#define PD_CTRL_BKGND_COLOR      RGB(0,0,0)           ///< 黑色

///
/// @brief
///     表盘的默认设置
///
#define PD_HAND_LEFT_COLOR       RGB(255,255,255)     ///< 指针的左半部分颜色,白色
#define PD_HAND_RIGHT_COLOR      RGB(128,128,135)     ///< 指针的右半部分颜色,暖灰色
#define PD_HAND_BTMEDGE_RATIO    3.0f/75.0f           ///< 自绘的三角形指针的底边与实际底边的长度比例

///
/// @brief
///     控件贴图的透明化的默认颜色
///
#define PD_DEFAULT_TRANS_COLOR   RGB(0x01,0x02,0x03)

#ifndef PI
#define PI 3.1415926
#endif

IMPLEMENT_DYNAMIC(CPolarityDialCtrl, CStatic)

///
/// @brief
///     构造函数
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
///     析构函数
///
CPolarityDialCtrl::~CPolarityDialCtrl()
{
	
}

///
/// @brief
///     设置背景bmp
///
/// @param[in]
///     pBmp         - 指向mp的指针
/// @param[in]
///     bTransparent - bmp的白色背景是否透明化
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
///     设置表盘指针，指针须指向12点钟方向
///
/// @param[in]
///     pBmp - 指向Bmp的指针
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
///     初始化极性表盘控件，无表盘
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
///     擦除表盘上的指针
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
///     绘制表盘指针：叠加式（与前面绘制的指针叠加显示）
///     默认为自绘方式，若设置了指针Bmp，则为贴图方式
///
/// @param[in]
///     idxSeq     - 表盘的顺序号（按从左往右，再从上到下的顺序），从0开始
/// @param[in]
///     percentVal - 表盘指针偏转的百分比，最左边为0
///
/// @return
///
///
void CPolarityDialCtrl::DrawHand(const size_t idxSeq,float percentVal)
{
	if ( (int)idxSeq >= m_iXGramCount*m_iYGramCount)
		return;

	// 超过量程则默认为50.f
	if ( _finite(percentVal) == FALSE )
		percentVal = 0;
	if ( percentVal < 0.0f || percentVal > 100.0f )
		percentVal = m_fDialInitState;

	// 计算当前要绘制的表盘的矩形域
	int x = (int)idxSeq%m_iXGramCount;
	int y = (int)idxSeq/m_iXGramCount;
	CRect dialArea(m_rtDialArea.left   + m_iXGramSpace * x,
	               m_rtDialArea.top    + m_iYGramSpace * y,
	               m_rtDialArea.right  + m_iXGramSpace * x,
	               m_rtDialArea.bottom + m_iYGramSpace * y);

	// 计算标准表盘与控件的尺寸比例
	float xRatio = (float)dialArea.Width()/(float)m_rtStdDialRect.Width();
	float yRatio = (float)dialArea.Height()/(float)m_rtStdDialRect.Height();

	// 计算控件中的表盘圆心
	CPoint actCircleCentre(int(dialArea.Width()*x+m_ptStdDialCircleCentre.x*xRatio),
		                   int(dialArea.Height()*y+m_ptStdDialCircleCentre.y*yRatio)-6);

	// 计算控件中的表盘内径长度和指针长度
	//float actInRadii = m_fStdDialInRadii * yRatio;
	//float actHandRadii = m_fStdHandRadii * yRatio;

    // 计算控件中的表盘内径长度和指针长度
    float actInRadii = 15.0;
    float actHandRadii = 51.0;

//	m_cForgndDC.AngleArc(actCircleCentre.x,actCircleCentre.y,actInRadii,0.0f,180.0f);

	// 计算控件中的指针的偏转角度
	float degree = float(((m_fMinDegree+(m_fMaxDegree-m_fMinDegree)/100.0f*percentVal)-90.0f)*2*PI/360.0f);
	
	// 绘制表盘指针
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
///     强制刷新：将当前内存设备缓冲中的内容刷新到目标内存设备环境（DC）中
///
/// @param[in]
///     pDC - 指向目标DC的指针；为NULL时，则为默认的DC
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
///     重绘消息处理函数
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
///     释放控件资源消息处理函数
///
/// @return
///
void CPolarityDialCtrl::OnDestroy()
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
BOOL CPolarityDialCtrl::OnEraseBkgnd(CDC* pDC)
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
void CPolarityDialCtrl::CreateDCandBmp(const CRect &rt, CDC &dc, CBitmap &bmp, CBitmap **pPOldBmp)
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
void CPolarityDialCtrl::DrawBkgnd()
{
	CRect rt;
	GetClientRect(rt);
	
	// 创建背景DC和位图
	CreateDCandBmp(rt,m_cBkgndDC,m_cBkgndBmp,&m_pOldBkgndBmp);

	// 背景填充
	m_cBkgndDC.FillSolidRect(0,0,rt.Width(),rt.Height(),m_clBkgndColor);

	// 计算第1个表盘的矩形域及每个表盘的大小
	UpdateDialArea(&m_cBkgndDC);

	// 绘制表盘Bmp背景
	if ( m_pDialBmp != NULL && m_pDialBmp->GetSafeHandle() != NULL )
	{
		// 初始化缓存DC
		CDC memDC;
		memDC.CreateCompatibleDC(&m_cBkgndDC);
		CBitmap *pOldMemBmp = memDC.SelectObject(m_pDialBmp);

		// 获取表盘Bmp的信息
		BITMAP bmp;
		m_pDialBmp->GetBitmap(&bmp);
	
		// 从左往右，从上到下绘制所有表盘
		for ( int y = 0; y < m_iYGramCount; y++ )
		{
			for ( int x = 0; x < m_iXGramCount; x++ )
			{
				// 计算当前要绘制的表盘的矩形域
				CRect dialArea(m_rtDialArea.left   + m_iXGramSpace * x,
					m_rtDialArea.top    + m_iYGramSpace * y,
					m_rtDialArea.right  + m_iXGramSpace * x,
					m_rtDialArea.bottom + m_iYGramSpace * y);
				if ( m_bDialBmpTransparent )
				{
					// 表盘位图的白色背景透明化	
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
///     重置控件前景
///
/// @return
///
void  CPolarityDialCtrl::ResetForgnd()
{
	CRect rt;
	GetClientRect(rt);

	// 创建前景DC和位图
	CreateDCandBmp(rt,m_cForgndDC,m_cForgndBmp,&m_pOldForgndBmp);

	m_cForgndDC.BitBlt(0,0,rt.Width(),rt.Height(),&m_cBkgndDC,0,0,SRCCOPY);
}

///
/// @brief
///     控件资源释放函数
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
///     计算第1个表盘的矩形域，及每个表盘的大小
///
/// @param[in]
///     pDC - 设备环境DC的指针
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
///     绘制指针：自绘方式
///
/// @param[in]
///     pDC       - 设备环境DC的指针
/// @param[in]
///     axisPt    - 在控件上指针的转轴点坐标
/// @param[in]
///     inRadii   - 在控件上表盘的内径长度
/// @param[in]
///     handRadii - 在控件上指针的半径长度
/// @param[in]
///     degree    - 在控件上指针的偏转弧度，设12点钟方向弧度为0，+为右偏，-为左偏
///
/// @return
///     
///
void CPolarityDialCtrl::DrawDefaultHand(CDC *pDC, const CPoint &axisPt, const float inRadii, const float handRadii, const float degree)
{
#if 0
	// 计算表盘指针的3个端点
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

	// 绘制表盘指针的左半部分
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

	// 绘制表盘指针的右半部分
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
	// 计算表盘指针的端点
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

	// 绘制表盘指针的左半部分
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

	// 绘制表盘指针的右半部分
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
///     绘制指针：贴图方式
///
/// @param[in]
///     pDC       - 设备环境DC的指针
/// @param[in]
///     axisPt    - 在控件上指针的转轴点坐标
/// @param[in]
///     inRadii   - 在控件上表盘的内径长度
/// @param[in]
///     handRadii - 在控件上指针的半径长度
/// @param[in]
///     degree    - 在控件上指针的偏转弧度，设12点钟方向弧度为0，+为右偏，-为左偏
///
/// @return
///     
///
void CPolarityDialCtrl::DrawBmpHand(CDC *pDC, CBitmap *pBmp, const CPoint &axisPt, const float inRadii, const float handRadii, const float degree)
{
	if ( pBmp == NULL || pBmp->GetSafeHandle() == NULL )
		return;

	// 获取源Bmp的信息
	BITMAP srcBmp;
	pBmp->GetBitmap(&srcBmp);

	// 创建DIB位图
	HBITMAP hSrcBmp;
	BYTE *srcLpBits = NULL;
	CreateBmp(hSrcBmp,&srcLpBits,srcBmp.bmWidth,srcBmp.bmHeight,srcBmp.bmBitsPixel);

	// 创建源bmp的缓存DC
	HDC hdcSrc = CreateCompatibleDC(pDC->m_hDC);
	HBITMAP hOldSrcBmp = (HBITMAP)::SelectObject(hdcSrc, hSrcBmp);

	// 获取源bmp的内存数据
	CDC srcBmpDC;
	srcBmpDC.CreateCompatibleDC(pDC);
	CBitmap *pOldSrcBmp = srcBmpDC.SelectObject(pBmp);
	if ( BitBlt(hdcSrc,0,0,srcBmp.bmWidth,srcBmp.bmHeight,srcBmpDC.m_hDC,0,0,SRCCOPY) == FALSE )
		return;
	srcBmpDC.SelectObject(pOldSrcBmp);
	srcBmpDC.DeleteDC();

	// 旋转位图并缓存到DC
	HBITMAP hDstBmp;
	RotateBmp(hDstBmp,srcBmp,srcLpBits,-degree);

	// 创建旋转后的bmp的缓存DC
	HDC hdcDst = CreateCompatibleDC(pDC->m_hDC);
	HBITMAP hOldDstBmp = (HBITMAP)::SelectObject(hdcDst, hDstBmp);

	// 计算旋转后的Bmp的在控件上的矩形域(偏移=偏移后的中心点-偏移前的中心点)	
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
	
	// 绘制指针
	TransparentBlt(pDC->m_hDC,handRt.left,handRt.top-1,handRt.Width(),handRt.Height(),
		hdcDst,0,0,dstBmp.bmWidth,dstBmp.bmHeight,m_clTransColor);

	// 还原设备环境
	::SelectObject(hdcSrc, hOldSrcBmp);
	DeleteDC(hdcSrc);
	DeleteObject(hSrcBmp);
	::SelectObject(hdcDst, hOldDstBmp);
	DeleteDC(hdcDst);
	DeleteObject(hDstBmp);
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
///     旋转位图
///
/// @param[in]
///     hDstBmp   - 保存旋转后的bmp的句柄
/// @param[in]
///     srcBmp    - 源Bmp的信息
/// @param[in]
///     srcLpBits - 指向源Bmp的DIB数据的指针
/// @param[in]
///     degree    - 源Bmp要旋转的弧度，设12点钟方向弧度为0，正则为逆时针方向，负则为顺时针方向
///
/// @return
///     
///
void CPolarityDialCtrl::RotateBmp(HBITMAP &hDstBmp, const BITMAP &srcBmp, BYTE *srcLpBits, const float degree)
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
	BYTE *dstLpBits = NULL;
	CreateBmp(hDstBmp,&dstLpBits,dstBmpWidth,dstBmpHeight,srcBmp.bmBitsPixel);

	// 创建缓存DC
	CDC *pDC = GetDC();
	HDC hdcDst = CreateCompatibleDC(pDC->m_hDC);
	HBITMAP hbmOld = (HBITMAP)::SelectObject(hdcDst, hDstBmp);
	::FillRect(hdcDst,CRect(0,0,dstBmpWidth,dstBmpHeight),CBrush(m_clTransColor));
	ReleaseDC(pDC);

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

	// 还原设备环境
	::SelectObject(hdcDst, hbmOld);
	DeleteDC(hdcDst);
}
