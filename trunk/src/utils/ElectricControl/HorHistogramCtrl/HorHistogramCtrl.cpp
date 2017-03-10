/// @file
///     HorHistogramCtrl.cpp
///
/// @brief
///     水平柱状图控件
///
/// @note
///     该柱状图图为叠加显示，支持自绘（默认）和贴图两种方式；
///     自绘方式：柱条为普通矩形，各柱条颜色通过SetPillarColor()设置；
///     贴图方式：柱条为贴图，各柱条样式通过SetPillarStyle()设置；
///     若设置了柱条样式，则表示选择了贴图方式，否则为默认方式；
///     图中各柱条按照由下往上的顺序；
///     控件标题位于控件的右上角；
///     柱条值位于柱条右边；
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
///    |2013.05.28     |1.0.0.528  |添加柱条的垂直高度、间距和Y标尺刻度的长度设置以及柱条长度上限的限制|
///    |2013.08.07     |1.0.1.807  |修改字体设置接口中对入参的有效性判断                     |
///    |2013.08.08     |1.0.2.808  |修改当柱条长度大于0.001小于1像素时，按1像素处理          |
///    |2013.11.05     |1.0.2.1105 |修改柱状图的柱条显示区域的边框的样式设置接口及绘制实现   |
///    |2013.11.05     |1.0.2.1105 |修正计算柱条显示区域的矩形域时Y标尺长度设置错误的Bug     |
///    |2013.11.05     |1.0.2.1105 |修正计算获取Y标尺刻度的最大字符串有误的Bug               |
///
#include "stdafx.h"
#include <float.h>
#include "HorHistogramCtrl.h"

///
/// @brief
///     控件的背景颜色
///
#define HHG_CTRL_BKGND_COLOR      RGB(0,0,0)           ///< 黑色

///
/// @brief
///     字体的默认设置，包括字体样式以及标题、X-Y标尺刻度和波形描述信息的字体大小和颜色
///
#define HHG_FONT_STYLE            _T("微软雅黑")
#define HHG_TITLE_FONT_SIZE       20
#define HHG_TITLE_FONT_COLOR      RGB(255,255,255)     ///< 白色
#define HHG_SCALE_FONT_SIZE       15
#define HHG_SCALE_FONT_COLOR      RGB(128,128,135)     ///< 暖灰色
#define HHG_PILLARDESC_FONT_SIZE  15
#define HHG_PILLARDESC_FONT_COLOR RGB(255,0,0)         ///< 红色

//设置柱状图的Margin
//#ifndef  ZENG_YUN
#define HHG_MARGIN_LEFT			0
#define HHG_MARGIN_RIGHT		0
#define HHG_MARGIN_TOP			0
#define HHG_MARGIN_BOTTOM		0
//#endif
///
/// @brief
///     柱条显示区域的默认设置：边框和内部网格的线条的类型、宽度和颜色
///
#define HHG_BORDER_LINE_STYLE PS_SOLID
#define HHG_BORDER_LINE_WIDTH 1
#define HHG_BORDER_LINE_COLOR RGB(128,128,135)         ///< 暖灰色
#define HHG_GRID_LINE_STYLE   PS_DASH
#define HHG_GRID_LINE_WIDTH   0
#define HHG_GRID_LINE_COLOR   RGB(128,128,105)         ///< 冷灰色

///
/// @brief
///     柱条矩形的默认设置：柱条矩形的间距、垂直高度和前6条柱条的默认颜色(红、黄、森林绿、猩红、橙、草绿)
///
#define HHG_PILLAR_SPACE_BETWEEN 6
#define HHG_PILLAR_HEIGHT        10
static const COLORREF k_PillarColor[6] = {RGB(255,0,0),  RGB(255,255,0),RGB(34,139,34),
                                          RGB(220,20,60),RGB(255,165,0),RGB(124,252,0)};

///
/// @brief
///     控件贴图的透明化的默认颜色
///
#define HHG_DEFAULT_TRANS_COLOR   RGB(0x01,0x02,0x03)


IMPLEMENT_DYNAMIC(CHorHistogramCtrl, CStatic)

///
/// @brief
///     构造函数
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
///     析构函数
///
CHorHistogramCtrl::~CHorHistogramCtrl()
{
	
}

///
/// @brief
///     设置某一柱条的颜色，若顺序号超过范围则多出的部分为默认颜色
///
/// @param[in]
///     idxSeq - 柱条的顺序号（按从下到上的顺序），从0开始
/// @param[in]
///     clr    - 颜色值
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
			// 超出部分为默认颜色
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
///     设置某一柱条的Y标尺刻度，若顺序号超过范围则多出的部分为空串
///
/// @param[in]
///     idxSeq   - 柱条的顺序号（按从下到上的顺序），从0开始
/// @param[in]
///     scaleVal - Y标尺刻度
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
			// 超出部分默认为空串
			m_strYScaleVal.push_back(_T(""));
		}
	}
	else
		m_strYScaleVal.at(idxSeq) = scaleVal;
}

///
/// @brief
///     设置某一柱条的样式，若顺序号超过范围则多出的部分设为空
///
/// @param[in]
///     idxSeq - 柱条的顺序号（按从下到上的顺序），从0开始
/// @param[in]
///     pBmp   - 指向柱条样式Bmp的指针
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
			// 超出部分默认为空
			m_vPillarStyleBmp.push_back(NULL);
		}
	}
	else
	{
		// 更新样式
		m_vPillarStyleBmp.at(idxSeq) = pBmp;
	}
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
///     设置控件上的X、Y标尺的刻度的字体
///
/// @param[in]
///     pFont - 指向字体的指针
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
///     设置柱条值信息的字体
///
/// @param[in]
///     pFont - 指向字体的指针
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
///     初始化控件
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
///     擦除柱条显示区域的全部柱条信息
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
///     绘制柱条：叠加式（与前面绘制的柱条叠加显示）
///     默认为柱条为自绘矩形形式，若设置了柱条样式，则柱条为贴图形式
///
/// @param[in]
///     valBuf  - 保存柱条所表示的值的缓存
/// @param[in]
///     bufSize - 缓存大小
/// @param[in]
///     valUnit - 柱条所表示的值的单位
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
///     强制刷新：将当前内存设备缓冲中的内容刷新到目标内存设备环境（DC）中
///
/// @param[in]
///     pDC - 指向目标DC的指针；为NULL时，则为默认的DC
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
///     重绘消息处理函数
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
///     释放控件资源消息处理函数
///
/// @return
///
void CHorHistogramCtrl::OnDestroy()
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
BOOL CHorHistogramCtrl::OnEraseBkgnd(CDC* pDC)
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
void CHorHistogramCtrl::CreateDCandBmp(const CRect &rt, CDC &dc, CBitmap &bmp, CBitmap **pPOldBmp)
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
void CHorHistogramCtrl::DrawBkgnd( )
{
	CRect rt;
	GetClientRect(rt);

	// 创建背景DC和位图
	CreateDCandBmp(rt,m_cBkgndDC,m_cBkgndBmp,&m_pOldBkgndBmp);

	// 背景填充
    //
	m_cBkgndDC.FillSolidRect(0,0,rt.Width(),rt.Height(),m_clBkgndColor);
   // m_cBkgndDC.FillSolidRect(0,0,rt.Width(),rt.Height(),RGB(255,255,255));

	// 计算柱条显示区域的矩形域
	UpdatePillarAreaSpace(&m_cBkgndDC);

	// 绘制柱条显示区域的内部网格
	DrawPillarAreaGrid(&m_cBkgndDC,m_rtPillarArea);

	// 绘制柱条显示区域的边框
	DrawPillarAreaBorder(&m_cBkgndDC,m_rtPillarArea);

	// 绘制X-Y标尺的刻度
	DrawScale(&m_cBkgndDC,m_rtPillarArea);

	// 绘制右上角的标题
	DrawTitle(&m_cBkgndDC,m_rtPillarArea);
}

///
/// @brief
///     重置控件前景
///
/// @return
///
void  CHorHistogramCtrl::ResetForgnd()
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
///     计算柱条显示区域的矩形域
///
/// @param[in]
///     pDC - 设备环境DC的指针
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
	// 计算Y标尺刻度的最大Size
	CString str;
	CSize xScaleValMaxSz,yScaleValMaxSz;
	str = GetYScaleValMaxStr();
	yScaleValMaxSz = pDC->GetTextExtent(str);
	if ( m_iYScaleValLen != 0 )
		yScaleValMaxSz.cx = m_iYScaleValLen;
	// 计算X标尺刻度的最大Size
	str.Format(_T("%0.1f"),m_fXScaleInterval*m_iXGridCount);
	xScaleValMaxSz = pDC->GetTextExtent(str);
	pDC->SelectObject(pOldFont);

	// 计算柱条显示区域的矩形域
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
///     绘制柱条显示区域的内部网格
///
/// @param[in]
///     pDC        - 设备环境DC的指针
/// @param[in]
///     pillarArea - 柱条显示区域的矩形域
///
/// @return
///     
///
void CHorHistogramCtrl::DrawPillarAreaGrid(CDC *pDC, const CRect &pillarArea)
{
	// 计算常规柱条区域的网格宽度及高度
	float horGridWidth = (float)pillarArea.Width()/(float)m_iXGridCount;
	float verGridWidth = (float)pillarArea.Height()/(float)m_iYGridCount;

	CPen  pen;
	pen.CreatePen(HHG_GRID_LINE_STYLE,HHG_GRID_LINE_WIDTH,m_clPillarAreaGridColor);
	CPen *pOldPen = pDC->SelectObject(&pen);
	// 绘制常规柱条区域的竖直网格线
	if ( m_bShowVerGrid )
	{
		int left = pillarArea.left;
		for (int i = 1; i < m_iXGridCount; ++i)
		{
			pDC->MoveTo(int(left+i*horGridWidth),pillarArea.top);
			pDC->LineTo(int(left+i*horGridWidth),pillarArea.bottom);
		}
	}
	// 绘制水平网格线
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
///     绘制柱条显示区域的边框
///
/// @param[in]
///     pDC        - 设备环境DC的指针
/// @param[in]
///     pillarArea - 柱条显示区域的矩形域
///
/// @return
///     
///
void CHorHistogramCtrl::DrawPillarAreaBorder(CDC *pDC, const CRect &pillarArea)
{
	CPen pen;
	pen.CreatePen(HHG_BORDER_LINE_STYLE,HHG_BORDER_LINE_WIDTH,m_clPillarAreaBorderColor);
	CPen *pOldPen = pDC->SelectObject(&pen);

	// 左边框线
	if ( m_bShowBorder[0] )
	{
		pDC->MoveTo(pillarArea.left-21,pillarArea.top);
		pDC->LineTo(pillarArea.left-21,pillarArea.bottom);
	}
	
	// 右边框线
	if ( m_bShowBorder[1] )
	{
		pDC->MoveTo(pillarArea.right,pillarArea.top);
		pDC->LineTo(pillarArea.right,pillarArea.bottom);
	}

	// 上边框线
	if ( m_bShowBorder[2] )
	{
		pDC->MoveTo(pillarArea.left,pillarArea.top);
		pDC->LineTo(pillarArea.right,pillarArea.top);
	}
	// 下边框线
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
///     绘制X-Y标尺的刻度
///
/// @param[in]
///     pDC        - 设备环境DC的指针
/// @param[in]
///     pillarArea - 柱条显示区域的矩形域
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
	// 绘制X刻度
	if ( m_bShowXScaleVal )
	{
		// 计算X标尺的刻度值的间隔
		float xSpanWidth = (float)pillarArea.Width() / (float)m_iXGridCount;

		// 设置X标尺的分度值
		float xInterval = m_fXScaleInterval;
		// 绘制X标尺的刻度
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

	if ( m_bShowYScaleVal ) 	// 绘制各柱条的Y刻度
	{
		// 计算Y标尺的刻度值的间隔
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
	//绘制Y刻度
	if(m_bShowYScaleVal){
		//计算Y轴刻度间隔
		float ySpanHeight = (float)pillarArea.Height()/2 ;
		float yInterval = 50;
		//绘制刻度
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

#if !CHUN_HUA    //画X轴刻度
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
///     绘制控件的标题（位于波形显示区域的右上角）
///
/// @param[in]
///     pDC        - 设备环境DC的指针
/// @param[in]
///     pillarArea - 柱条显示区域的矩形域
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
///     绘制柱条（自绘形式）
///
/// @param[in]
///     pDC        - 设备环境DC的指针
/// @param[in]
///     pillarArea - 柱条显示区域的矩形域
/// @param[in]
///     valDataBuf - 保存柱条所表示的值的缓存
/// @param[in]
///     bufSize    - 缓存大小
/// @param[in]
///     valUnit    - 柱条所表示的值的单位
///
/// @return
///     
///
void CHorHistogramCtrl::DrawPillar(CDC *pDC, const CRect &pillarArea, const float *valDataBuf, const size_t bufSize, const CString &valUnit)
{
	ASSERT(NULL != valDataBuf);

	/*******************************************************************/
#if ZENG_YUN
	// 计算X标尺1刻度的长度、柱条区域单个柱条的可用的高度和0刻度线所在的位置
	float OneXScaleLen       = (float)pillarArea.Width()/(m_fXScaleInterval*(float)m_iXGridCount);
	float onePillarMaxHeight = (float)pillarArea.Height()/(float)m_iPillarCount;		//柱状条的数目
	int   zeroScalePos       = pillarArea.left+1;

	for (int i=0;i<m_iPillarCount;i++)
	{
		if ( i >= (int)bufSize )
			break;

		// 上下限判断
		float val = valDataBuf[i];
		if ( val < 0.0f || _finite(val) == FALSE )
			val = 0.0f;
		if ( val > m_fXScaleInterval*(float)m_iXGridCount )			// m_fXScaleInterval X刻度值
			val = m_fXScaleInterval*(float)m_iXGridCount;

		// 设置柱条颜色
		COLORREF pillarColor;
		if ( i >= (int)m_clPillarColor.size() )
			pillarColor = m_clPillarColor[0];
		else
			pillarColor = m_clPillarColor[i];

		// 绘制柱条
		CBrush brush(pillarColor);
		int pillarBetween = m_iPillarBetween;
		if ( (int)(onePillarMaxHeight - m_iPillarHeight) > m_iPillarBetween )		//m_iPillarBetween柱状条之间的间距
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

		// 在柱条右边绘制柱条值信息
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
    //计算Y标尺一刻度的长度
    float OneYScaleLen = (float)pillarArea.Height()/(m_fXScaleInterval*(float)m_iXGridCount);
    //柱状图的间距
    float onePillarMaxWidth = (float)pillarArea.Width()/(float)m_iPillarCount;
    //柱状图的其实位置
    int zeroScalePos = pillarArea.bottom -1;

    //开始绘制
    for(int i =0; i<m_iPillarCount; i++){
        if(i >= (int)bufSize)
            break;
        float value = valDataBuf[i];
        if(value < 0.0f || _finite(value) == FALSE)
            value = 0.0f;
        //如果超过刻度，以最大值算
        if(value > m_fXScaleInterval*(float)m_iXGridCount)
            value = m_fXScaleInterval*(float)m_iXGridCount;
        COLORREF   pillarclr;
        if(i >= (int)m_clPillarColor.size())
            pillarclr = m_clPillarColor[0];
        else
            pillarclr = m_clPillarColor[i];
        //绘制柱状图
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

        //绘制柱状图上的文字
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
///     绘制柱条（贴图形式）
///
/// @param[in]
///     pDC        - 设备环境DC的指针
/// @param[in]
///     pillarArea - 柱条显示区域的矩形域
/// @param[in]
///     valDataBuf - 保存柱条所表示的值的缓存
/// @param[in]
///     bufSize    - 缓存大小
/// @param[in]
///     valUnit    - 柱条所表示的值的单位
///
/// @return
///     
///
void CHorHistogramCtrl::DrawPillarWithBmp(CDC *pDC, const CRect &pillarArea, const float *valDataBuf, const size_t bufSize, const CString &valUnit)
{
	ASSERT(NULL != valDataBuf);

	// 计算X标尺1刻度的长度、柱条区域单个柱条的可用的高度和0刻度线所在的位置
	float OneXScaleLen       = (float)pillarArea.Width()/(m_fXScaleInterval*(float)m_iXGridCount);
	float onePillarMaxHeight = (float)pillarArea.Height()/(float)m_iPillarCount;
	int   zeroScalePos       = pillarArea.left+1;

	for (int i=0;i<m_iPillarCount;i++)
	{
		if ( i >= (int)bufSize )
			break;

		// 上下限判断
		int val = (int)valDataBuf[i];
		if ( val < 0 )
			break;
		if ( _finite(val) == FALSE )
			val = 0;
		if ( val > (int)(m_fXScaleInterval*m_iXGridCount) )
			val = (int)(m_fXScaleInterval*m_iXGridCount);

		// 设置柱条样式
		CBitmap *pBmp = NULL;
		if ( i >= (int)m_vPillarStyleBmp.size() )
		{
			if ( m_vPillarStyleBmp[0] != NULL && m_vPillarStyleBmp[0]->GetSafeHandle() != NULL )
				pBmp = m_vPillarStyleBmp[0];
		}
		else
			pBmp = m_vPillarStyleBmp[i];

		// 计算柱条的矩形域
		int pillarBetween = m_iPillarBetween;
		if ( (int)(onePillarMaxHeight - m_iPillarHeight) > m_iPillarBetween )
			pillarBetween = (int)(onePillarMaxHeight - m_iPillarHeight);
		CRect rect(int(zeroScalePos+2),
			int(pillarArea.bottom+pillarBetween/2-(i+1)*onePillarMaxHeight),
			int(zeroScalePos+OneXScaleLen*val),
			int(pillarArea.bottom-pillarBetween/2-i*onePillarMaxHeight));
		// 绘制柱条
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

		// 在柱条上面绘制柱条值信息
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
///     获取Y标尺刻度的最大字符串
///
/// @return
///     Y标尺刻度的最大字符串
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
