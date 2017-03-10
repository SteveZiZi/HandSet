/// @file
///     HistogramCtrl.cpp
///
/// @brief
///     柱状图控件
///
/// @note
///     该柱状图为叠加显示，有两类柱形条（常规+特殊）
///     常规柱条：X标尺刻度为数值，从0开始
///     特殊柱条：由用户自行添加并自定义X标尺刻度值，从常规柱条区域的左侧添加
///     该柱状图在Y标尺上不绘制柱条
///     控件标题位于柱条显示区域的右上角
///     柱条描述信息位于柱条显示区域的左上角
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
///    |2013.05.09     |1.0.509    |修改柱条顶部文本的重叠判断为与前3项的文本往返判断2次     |
///    |2013.05.18     |1.0.518    |修改游标左移响应时游标偏移的错误                         |
///    |2013.05.30     |1.0.530    |修正柱条长度超出量程上限的Bug                            |
///    |2013.07.09     |1.0.709    |将网格显示接口分离为水平/垂直网格显示接口                |
///    |2013.07.16     |1.0.716    |将当前游标所在柱条的描述信息与游标的绘制分离，且描述信息放到最后绘制|
///    |2013.07.16     |1.0.716    |修改特写柱条的特写信息的绘制实现，特写信息超出柱条显示区域则不绘制|
///    |2013.08.07     |1.0.807    |修改字体设置接口中对入参的有效性判断                     |
///
#include "stdafx.h"
#include <float.h>
#include "HistogramCtrl.h"

///
/// @brief
///     控件的背景颜色
///
#define HTG_CTRL_BKGND_COLOR    RGB(0,0,0)            ///< 黑色

///
/// @brief
///     字体的默认设置，包括字体样式以及标题、X-Y标尺刻度和波形描述信息的字体大小和颜色
///
#define HTG_FONT_STYLE            _T("微软雅黑")
#define HTG_TITLE_FONT_SIZE       20
#define HTG_TITLE_FONT_COLOR      RGB(255,255,255)     ///< 白色
#define HTG_SCALE_FONT_SIZE       15
#define HTG_SCALE_FONT_COLOR      RGB(128,128,135)     ///< 暖灰色
#define HTG_PILLARDESC_FONT_SIZE  15
#define HTG_PILLARDESC_FONT_COLOR RGB(255,0,0)         ///< 红色
#define HTG_FEATURE_FONT_SIZE     15
#define HTG_FEATURE_FONT_COLOR    RGB(124,252,0)       ///< 草绿色

///
/// @brief
///     柱条显示区域的默认设置：边框和内部网格的线条的类型、宽度和颜色
///
#define HTG_BORDER_LINE_STYLE PS_SOLID
#define HTG_BORDER_LINE_WIDTH 1
#define HTG_GRID_LINE_STYLE   PS_DASH
#define HTG_GRID_LINE_WIDTH   0
#define HTG_GRID_LINE_COLOR   RGB(128,128,105)         ///< 冷灰色

///
/// @brief
///     柱条矩形的默认设置：柱条矩形的宽度和颜色
///
#define HTG_PILLAR_WIDTH 6
#define HTG_PILLAR_COLOR RGB(255,255,0)                ///< 黄色

///
/// @brief
///     单个特殊柱条所占用的网格宽度及在标尺上的刻度的颜色
///
#define HTG_ONE_SPC_PILLAR_GRID_WIDTH 35
#define HTG_SPC_PILLAR_SCALEVAL_COLOR RGB(255,0,0) ///< 红色

///
/// @brief
///     游标的默认设置：颜色、线条样式、线条宽度和左右三角形的宽度
///
#define HTG_CURSOR_COLOR       HTG_PILLARDESC_FONT_COLOR
#define HTG_CURSOR_LINE_STYLE  PS_SOLID
#define HTG_CURSOR_LINE_WIDTH  1
#define HTG_CURSOR_TRIG_WIDTH  3

IMPLEMENT_DYNAMIC(CHistogramCtrl, CStatic)

///
/// @brief
///     构造函数
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
///     析构函数
///
CHistogramCtrl::~CHistogramCtrl()
{
	
}

///
/// @brief
///     设置特定柱状图的柱条的颜色，若顺序号超过范围则多出的部分为默认颜色
///
/// @param[in]
///     idxSeq - 柱状图的顺序号（按从左往右，再从上到下的顺序），从0开始
/// @param[in]
///     clr    - 颜色值
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
			// 超出部分为默认颜色
			m_clPillarColor.push_back(HTG_PILLAR_COLOR);
		}
	}
	else
		m_clPillarColor.at(idxSeq) = clr;
}

///
/// @brief
///     设置游标的颜色，若顺序号超过范围则多出的部分为默认颜色
///
/// @param[in]
///     idxSeq - 柱状图的顺序号（按从左往右，再从上到下的顺序），从0开始
/// @param[in]
///     clr    - 颜色值
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
			// 超出部分为默认颜色
			m_clCursorColor.push_back(HTG_CURSOR_COLOR);
		}
	}
	else
		m_clCursorColor.at(idxSeq) = clr;
}

///
/// @brief
///     设置控件的标题（位于控件波形显示区域的右上角）,若顺序号超过范围则多出的部分为空
///
/// @param[in]
///     idxSeq - 柱状图的顺序号（按从左往右，再从上到下的顺序），从0开始
/// @param[in]
///     str    - 控件的标题
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
///     设置控件的X-Y标尺的分度值，若顺序号超过范围则多出的部分为默认设置
///
/// @param[in]
///     idxSeq    - 柱状图的顺序号（按从左往右，再从上到下的顺序），从0开始
/// @param[in]
///     xInterval - X标尺的分度值
/// @param[in]
///     yInterval - Y标尺的分度值
/// @param[in]
///     yUnit     - Y标尺的单位
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
			// 超出部分为默认设置
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
///     移除特定的特殊柱条
///
/// @param[in]
///     idxSeq - 柱状图中第N条特殊柱条，从0开始，从左向右
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
///     设置控件的柱状图的个数
///
/// @param[in]
///     xGramCount - X标尺方向上的柱状图个数
/// @param[in]
///     yGramCount - Y标尺方向上的柱状图个数
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
///     设置控件标题的字体
///
/// @param[in]
///     pFont - 指向字体的指针
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
///     设置X-Y标尺刻度的字体
///
/// @param[in]
///     pFont - 指向字体的指针
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
///     设置柱条描述信息的字体
///
/// @param[in]
///     pFont - 指向字体的指针
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
///     设置特写柱条的特写信息的字体
///
/// @param[in]
///     pFont - 指向字体的指针
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
///     初始化控件
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
///     擦除柱条显示区域的全部柱条信息
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
///     绘制柱条：叠加式（与前面绘制的柱条叠加显示）
///
/// @param[in]
///     idxSeq        - 柱状图的顺序号（按从左往右，再从上到下的顺序），从0开始
/// @param[in]
///     cmnPillarData - 保存常规柱条的数据值的缓存
/// @param[in]
///     spcPillarData - 保存特殊（自定义）柱条的数据值的缓存
/// @param[in]
///     curPillarDesc - 当前游标所在的柱条的描述信息
///
/// @return
///
///
void CHistogramCtrl::DrawHistogram(const size_t idxSeq, const float *cmnPillarData,const float *spcPillarData,const CString &curPillarDesc)
{
	if ( (int)idxSeq >= m_iXGramCount*m_iYGramCount)
		return;

	// 计算当前要绘制的柱条显示区域的矩形域
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
///     强制刷新：将当前内存设备缓冲中的内容刷新到目标内存设备环境（DC）中
///
/// @param[in]
///     pDC - 指向目标DC的指针；为NULL时，则为默认的DC
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
///     重绘消息处理函数
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
///     释放控件资源消息处理函数
///
/// @return
///
///
void CHistogramCtrl::OnDestroy()
{
	CStatic::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	Release();
}

///
/// @brief
///     键盘按键释放消息处理函数
///
/// @return
///
///
void CHistogramCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
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
///     擦除背景消息处理函数
///
/// @return
///
///
BOOL CHistogramCtrl::OnEraseBkgnd(CDC* pDC)
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
void CHistogramCtrl::CreateDCandBmp(const CRect &rt, CDC &dc, CBitmap &bmp, CBitmap **pPOldBmp)
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
void CHistogramCtrl::DrawBkgnd()
{
	CRect rt;
	GetClientRect(rt);

	// 创建背景DC和位图
	CreateDCandBmp(rt,m_cBkgndDC,m_cBkgndBmp,&m_pOldBkgndBmp);

	// 背景填充
	m_cBkgndDC.FillSolidRect(0,0,rt.Width(),rt.Height(),m_clBkgndColor);

	// 计算第1个柱条显示区域的矩形域，及每个图的大小
	UpdatePillarAreaSpace(&m_cBkgndDC);

	// 从左往右，从上到下绘制所有柱状图的柱条显示区域
	for ( int y = 0; y < m_iYGramCount; y++ )
	{
		for ( int x = 0; x < m_iXGramCount; x++ )
		{
			// 计算当前要绘制的柱条显示区域的矩形域
			CRect pillarArea(m_rtPillarArea.left   + m_iXGramSpace * x,
			                 m_rtPillarArea.top    + m_iYGramSpace * y,
			                 m_rtPillarArea.right  + m_iXGramSpace * x,
			                 m_rtPillarArea.bottom + m_iYGramSpace * y);
#if !CHUN_HUA
			m_cBkgndDC.FillSolidRect(pillarArea,RGB(0,0,0));
#endif

			// 绘制柱条显示区域的内部网格
			DrawPillarAreaGrid(&m_cBkgndDC,pillarArea);

			// 绘制柱条显示区域的边框
			DrawPillarAreaBoard(&m_cBkgndDC,pillarArea);

			// 绘制X-Y标尺的刻度
			DrawScale(&m_cBkgndDC,y*m_iXGramCount+x,pillarArea);

			// 绘制右上角的标题
			DrawTitle(&m_cBkgndDC,y*m_iXGramCount+x,pillarArea);
		}
	}
}

///
/// @brief
///     重置控件前景
///
/// @return
///
void CHistogramCtrl::ResetForgnd()
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
///     计算第1个柱条显示区域的矩形域，及每个图的大小
///
/// @param[in]
///     pDC - 设备环境DC的指针
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
void CHistogramCtrl::DrawPillarAreaGrid(CDC *pDC, const CRect &pillarArea)
{
	if ( m_bShowPillarAreaHorGrid == FALSE && m_bShowPillarAreaVerGrid == FALSE )
		return;

	// 计算常规柱条区域的网格的水平宽度及垂直高度
	float horGridWidth = ((float)pillarArea.Width()-(float)m_strSpcScaleVal.size()*HTG_ONE_SPC_PILLAR_GRID_WIDTH)/(float)m_iXGridCount;
	float verGridWidth = (float)pillarArea.Height()/(float)m_iYGridCount;

	CPen  pen;
	pen.CreatePen(HTG_GRID_LINE_STYLE,HTG_GRID_LINE_WIDTH,m_clPillarAreaGridColor);
	CPen *pOldPen = pDC->SelectObject(&pen);
	// 绘制特殊柱条区域的竖直网格线
	for ( size_t i = 1; i <= m_strSpcScaleVal.size() && m_bShowPillarAreaVerGrid; i++ )
	{
		pDC->MoveTo(int(pillarArea.left+i*HTG_ONE_SPC_PILLAR_GRID_WIDTH),pillarArea.top);
		pDC->LineTo(int(pillarArea.left+i*HTG_ONE_SPC_PILLAR_GRID_WIDTH),pillarArea.bottom);
	}
	// 绘制常规柱条区域的竖直网格线
	int left = int (pillarArea.left+HTG_ONE_SPC_PILLAR_GRID_WIDTH*m_strSpcScaleVal.size());
	for (int i = 1; i < m_iXGridCount && m_bShowPillarAreaVerGrid; ++i)
	{
		pDC->MoveTo(int(left+i*horGridWidth),pillarArea.top);
		pDC->LineTo(int(left+i*horGridWidth),pillarArea.bottom);
	}
	// 绘制水平网格线
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
void CHistogramCtrl::DrawPillarAreaBoard(CDC *pDC, const CRect &pillarArea)
{
	CPen pen;
	pen.CreatePen(HTG_BORDER_LINE_STYLE,HTG_BORDER_LINE_WIDTH,m_clScaleValColor);
	CPen *pOldPen = pDC->SelectObject(&pen);
	// 左边框线
	pDC->MoveTo(pillarArea.left,pillarArea.top);
	pDC->LineTo(pillarArea.left,pillarArea.bottom);
	// 右边框线
	pDC->MoveTo(pillarArea.right,pillarArea.top);
	pDC->LineTo(pillarArea.right,pillarArea.bottom);
	// 上边框线
	pDC->MoveTo(pillarArea.left,pillarArea.top);
	pDC->LineTo(pillarArea.right,pillarArea.top);
	// 下边框线
	pDC->MoveTo(pillarArea.left,pillarArea.bottom);
	pDC->LineTo(pillarArea.right,pillarArea.bottom);
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
///     idxSeq     - 柱状图的顺序号（按从左往右，再从上到下的顺序），从0开始
/// @param[in]
///     pillarArea - 柱条显示区域的矩形域
///
/// @return
///     
///
void CHistogramCtrl::DrawScale(CDC *pDC, const size_t idxSeq, const CRect &pillarArea)
{
	CFont *pOldFont = pDC->SelectObject(&m_cScaleValFont);
	COLORREF oldClr = pDC->SetTextColor(m_clSpcScaleValColor);
	int oldMode = pDC->SetBkMode(TRANSPARENT);

	// 绘制X刻度
	if ( m_bShowXScaleVal )
	{
		// 绘制特殊柱条区域的刻度
		for ( size_t i = 0; i < m_strSpcScaleVal.size(); i++ )
		{
			CSize scaleValSz = pDC->GetTextExtent(m_strSpcScaleVal.at(i));
			int left = int(pillarArea.left+(2*i+1)*HTG_ONE_SPC_PILLAR_GRID_WIDTH/2)-scaleValSz.cx/2;
			CRect rect(left,pillarArea.bottom,left+scaleValSz.cx,pillarArea.bottom+scaleValSz.cy);
			pDC->DrawText(m_strSpcScaleVal.at(i), rect, DT_RIGHT);
		}
		// 计算常规柱条区域的X标尺的刻度值的间隔
		float width = (float)pillarArea.Width()-(float)m_strSpcScaleVal.size()*HTG_ONE_SPC_PILLAR_GRID_WIDTH;
		float xSpanWidth = width / (float)m_iXGridCount;

		// 设置常规柱条区域的X标尺的分度值
		float xInterval;
		if ( idxSeq >= m_fXScaleInterval.size() )
			xInterval = m_fXScaleInterval.at(0);
		else
			xInterval = m_fXScaleInterval.at(idxSeq);
		// 绘制常规柱条的刻度,第1个刻度不绘制
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
	
	// 设置Y标尺的分度值
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
	// 绘制Y刻度
	if ( m_bShowYScaleVal )
	{
		// 计算常规柱条区域的Y标尺的刻度值的间隔
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
///     绘制控件的标题（位于柱条显示区域的右上角）
///
/// @param[in]
///     pDC        - 设备环境DC的指针
/// @param[in]
///     idxSeq        - 柱状图的顺序号（按从左往右，再从上到下的顺序），从0开始
/// @param[in]
///     pillarArea - 柱条显示区域的矩形域
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
///     绘制柱条
///
/// @param[in]
///     pDC           - 设备环境DC的指针
/// @param[in]
///     idxSeq        - 柱状图的顺序号（按从左往右，再从上到下的顺序），从0开始
/// @param[in]
///     pillarArea    - 柱条显示区域的矩形域
/// @param[in]
///     cmnPillarData - 保存常规柱条的数据值的缓存
/// @param[in]
///     spcPillarData - 保存特殊（自定义）柱条的数据值的缓存
///
/// @return
///     
///
void CHistogramCtrl::DrawPillar(CDC *pDC, const size_t idxSeq, const CRect &pillarArea, const float *cmnPillarData, const float *spcPillarData)
{
	ASSERT(NULL != cmnPillarData);
	ASSERT(NULL != spcPillarData);
//	ASSERT(m_strSpcScaleVal.size()!=0);

	// 设置柱条的颜色
	COLORREF pillarClr;
	if ( idxSeq >= m_clPillarColor.size() )
		pillarClr = m_clPillarColor.at(0);
	else
		pillarClr = m_clPillarColor.at(idxSeq);

	// 设置Y标尺的分度值
	float yInterval;
	if ( idxSeq >= m_fYScaleInterval.size() )
		yInterval = m_fYScaleInterval.at(0);
	else
		yInterval = m_fYScaleInterval.at(idxSeq);

	// 计算Y标尺1刻度的长度和常规柱条区域单个柱条所占用的宽度
	float OneYScaleLen = (float)pillarArea.Height()/((float)yInterval*(float)m_iYGridCount);
	float cmnAreaWidth = (float)pillarArea.Width()-(float)m_strSpcScaleVal.size()*HTG_ONE_SPC_PILLAR_GRID_WIDTH;
	float OneCPWidth   = (float)cmnAreaWidth/(float)m_iCommonPillarCount;

	CBrush brush;
	brush.CreateSolidBrush(pillarClr);
	// 绘制特殊柱条区域的柱条
	int minleft = int(pillarArea.left+HTG_ONE_SPC_PILLAR_GRID_WIDTH/2);
	for ( size_t i = 0; i < m_strSpcScaleVal.size(); i++ )
	{
		// 柱条上下限判断
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
	// 绘制常规柱条区域的柱条
	minleft = int(pillarArea.left+m_strSpcScaleVal.size()*HTG_ONE_SPC_PILLAR_GRID_WIDTH+OneCPWidth);
	for (int i=0;i<m_iCommonPillarCount;i++)
	{
		// 柱条上下限判断
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
///     绘制特写柱条的特写信息
///
/// @param[in]
///     pDC           - 设备环境DC的指针
/// @param[in]
///     idxSeq        - 柱状图的顺序号（按从左往右，再从上到下的顺序），从0开始
/// @param[in]
///     pillarArea    - 柱条显示区域的矩形域
/// @param[in]
///     cmnPillarData - 保存常规柱条的数据值的缓存
/// @param[in]
///     spcPillarData - 保存特殊（自定义）柱条的数据值的缓存
///
/// @return
///     
///
void CHistogramCtrl::DrawFeaturePillar(CDC *pDC, const size_t idxSeq, const CRect &pillarArea, const float *cmnPillarData, const float *spcPillarData)
{
	// 设置X、Y标尺的分度值
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

	// 计算Y标尺1刻度的长度和常规柱条区域单个柱条所占用的宽度
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
			// 计算该柱条的X标尺刻度文本的矩形域
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
			// 绘制X标尺刻度
			pDC->ExtTextOutW(xScaleRct.left,xScaleRct.top,OPAQUE,NULL,xScaleStr,xScaleStr.GetLength(),NULL);
			// 重设字体格式
			pDC->SelectObject(&m_cFeatureFont);
			pDC->SetTextColor(m_clFeatureInfoColor);
		}

		// 当游标移动到某一条之上时，则不用显示该条
		if ( m_FeaturePillar[i].idx == m_iCursorPos )
		{
			m_FeaturePillar[i].rect.SetRect(0,0,1,1);
			continue;
		}

		CString yScaleStr;
		CPoint pt;
		if ( size_t(m_FeaturePillar[i].idx) >= m_strSpcScaleVal.size() )
		{
			// 计算柱条顶部文本的矩形域
			int cmnIdx = int(m_FeaturePillar[i].idx-m_strSpcScaleVal.size());
			float val = cmnPillarData[cmnIdx];
			if ( _finite(val) == FALSE )
				val = 0;
			if ( val < 0 )
				val = 0;
			yScaleStr.Format(L"%0.2f",val);
			CSize sz = pDC->GetTextExtent(yScaleStr);
			int minleft = int(pillarArea.left+m_strSpcScaleVal.size()*HTG_ONE_SPC_PILLAR_GRID_WIDTH+OneCPWidth);

			// 柱条上限
			if ( val > yInterval*m_iYGridCount )
				val = yInterval*m_iYGridCount;
			m_FeaturePillar[i].rect.left = int(minleft+cmnIdx*OneCPWidth-sz.cx/2);
			m_FeaturePillar[i].rect.top = int(pillarArea.bottom-OneYScaleLen*val)-2-sz.cy;
			m_FeaturePillar[i].rect.right = m_FeaturePillar[i].rect.left+sz.cx;
			m_FeaturePillar[i].rect.bottom = m_FeaturePillar[i].rect.top+sz.cy;
			// 计算文本与柱条之间的竖线的起点
			pt.x = int(minleft+cmnIdx*OneCPWidth);
//			pt.y = int(pillarArea.bottom-1);
			pt.y = int(pillarArea.bottom-OneYScaleLen*val);
		}
		else
		{
			// 计算柱条顶部文本的矩形域
			float val = spcPillarData[m_FeaturePillar[i].idx];
			if ( _finite(val) == FALSE )
				val = 0;
			if ( val < 0 )
				val = 0;
			yScaleStr.Format(L"%0.2f",val);
			CSize sz = pDC->GetTextExtent(yScaleStr);
			int minleft = int(pillarArea.left+HTG_ONE_SPC_PILLAR_GRID_WIDTH/2);

			// 柱条上限
			if ( val > yInterval*m_iYGridCount )
				val = yInterval*m_iYGridCount;
			m_FeaturePillar[i].rect.left = minleft+m_FeaturePillar[i].idx*HTG_ONE_SPC_PILLAR_GRID_WIDTH-sz.cx/2;
			m_FeaturePillar[i].rect.top = int(pillarArea.bottom-OneYScaleLen*val)-2-sz.cy;
			m_FeaturePillar[i].rect.right = m_FeaturePillar[i].rect.left+sz.cx;
			m_FeaturePillar[i].rect.bottom = m_FeaturePillar[i].rect.top+sz.cy;
			// 计算文本与柱条之间的竖线的起点
			pt.x = minleft+m_FeaturePillar[i].idx*HTG_ONE_SPC_PILLAR_GRID_WIDTH;
//			pt.y = int(pillarArea.bottom-1);
			pt.y = int(pillarArea.bottom-OneYScaleLen*val);
		}

		// 判断柱条顶部文本是否会与前3项的文本重叠
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

		// 防止柱条顶部文字的底部低于竖线的起点
		if (m_FeaturePillar[i].rect.bottom > pt.y)
		{
			m_FeaturePillar[i].rect.MoveToY(m_FeaturePillar[i-1].rect.top - m_FeaturePillar[i].rect.Height() - 5);
		}

		// 当前柱条长度的顶部文本信息超出柱条显示区域时，则不用显示该条的顶部文本信息
		if ( m_FeaturePillar[i].rect.top < pillarArea.top )
		{
			m_FeaturePillar[i].rect.SetRect(0,0,1,1);
			continue;
		}

		// 绘制柱条顶部的文本
		pDC->ExtTextOutW(m_FeaturePillar[i].rect.left,m_FeaturePillar[i].rect.top,OPAQUE,NULL,yScaleStr,yScaleStr.GetLength(),NULL);

		// 绘制竖线
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
///     绘制游标
///
/// @param[in]
///     pDC        - 设备环境DC的指针
/// @param[in]
///     idxSeq     - 柱状图的顺序号（按从左往右，再从上到下的顺序），从0开始
/// @param[in]
///     pillarArea - 柱条显示区域的矩形域
///
/// @return
///     
///
void CHistogramCtrl::DrawCursor(CDC *pDC, const size_t idxSeq, const CRect &pillarArea)
{
	if ( !m_bShowCursor )
		return;

	// 设置游标的颜色
	COLORREF cursorClr;
	if ( idxSeq >= m_clCursorColor.size() )
		cursorClr = m_clCursorColor.at(0);
	else
		cursorClr = m_clCursorColor.at(idxSeq);

	// 计算Y标尺方向1网格的宽度和常规柱条区域单个柱条所占用的宽度
	float OneYGridWidth = (float)pillarArea.Height()/(float)m_iYGridCount;
	float cmnAreaWidth  = (float)pillarArea.Width()-(float)m_strSpcScaleVal.size()*HTG_ONE_SPC_PILLAR_GRID_WIDTH;
	float OneCPWidth    = cmnAreaWidth/(float)m_iCommonPillarCount;

	// 计算游标的竖线的起点
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

	// 绘制游标竖线
	CPen pen;
	pen.CreatePen(PS_SOLID,1,cursorClr);
	CPen* pOldPen = pDC->SelectObject(&pen);
	pDC->MoveTo(pt);
	pDC->LineTo(pt.x,int(pillarArea.top+OneYGridWidth-HTG_CURSOR_TRIG_WIDTH));
	POINT pos[3];
	// 绘制游标竖线两端的三角形
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
///     绘制当前游标所在的柱条的描述信息
///
/// @param[in]
///     pDC        - 设备环境DC的指针
/// @param[in]
///     idxSeq     - 柱状图的顺序号（按从左往右，再从上到下的顺序），从0开始
/// @param[in]
///     pillarArea - 柱条显示区域的矩形域
/// @param[in]
///     pillarDesc - 当前游标所在的柱条的描述信息
///
/// @return
///     
///
void CHistogramCtrl::DrawPillarDesc(CDC *pDC, const size_t idxSeq, const CRect &pillarArea, const CString &pillarDesc)
{
	if ( !m_bShowCursor )
		return;

	// 设置游标的颜色
	COLORREF cursorClr;
	if ( idxSeq >= m_clCursorColor.size() )
		cursorClr = m_clCursorColor.at(0);
	else
		cursorClr = m_clCursorColor.at(idxSeq);

	// 绘制游标所在的柱条的描述信息
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
bool CHistogramCtrl::IsIntersect(const CRect &rt1,const CRect &rt2)
{
	if(rt1.right <= rt2.left) return false;
	if(rt1.left >= rt2.right ) return false;
	if(rt1.bottom <= rt2.top ) return false;
	if(rt1.top >= rt2.bottom ) return false;

	return true;
}
