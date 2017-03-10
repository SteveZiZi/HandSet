/// @file
///     WaveFormGramCtrl.cpp
///
/// @brief
///     波形图控件
///
/// @note
///     该波形图的波形显示为叠加型，即在擦除前所有绘制的波形叠加显示
///     X标尺刻度为数值，从0开始
///     Y标尺中间刻度为0
///     控件标题位于波形显示区域的右上角
///     柱条描述信息位于波形显示区域的左上角
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
///    |2013.05.03     |1.0.503    |修改波形的描述信息位置，从左上角改为右上角标题下方，并默认为2行，用“\r\n”分开|
///    |2013.05.03     |1.0.503    |增加对波形图X方向的每个网格点数的设置，X标尺的分度值对其不影响|
///    |2013.05.06     |1.0.506    |修正当m_fYScaleInterval为0时，波形偏离显示区域的错误          |
///    |2013.05.06     |1.0.506    |修改波形图波形颜色设置接口，变更为一个图对应一个颜色表        |
///    |2013.06.21     |1.0.621    |修正波形数值为NAN时的错误显示的BUG                            |
///    |2013.07.09     |1.0.709    |将网格显示接口分离为水平/垂直网格显示接口                     |
///    |2013.07.09     |1.0.709    |添加波形描述信息的颜色设置接口                                |
///    |2013.07.09     |1.0.709    |修正重置波形图数量时未更新波形描述信息颜色表的Bug             |
///    |2013.07.09     |1.0.709    |添加对水平网格的中间线的可见性的设置                          |
///    |2013.08.07     |1.0.807    |修改字体设置接口中对入参的有效性判断                          |
///    |2013.11.07     |1.0.1107   |添加游标部件及其样式设置和绘制接口                            |
///    |2013.11.07     |1.0.1107   |添加设置组成绘制的波形的点的数量（波形图当前显示的波形的采样点数）的接口|
///    |2013.12.03     |1.0.1203   |优化波形绘制接口的实现，减少内存的申请和释放操作              |
///    |2013.12.03     |1.0.1203   |添加对X标尺的起始点的刻度值的设置                             |
///    |2013.12.06     |1.0.1206   |修改待绘制波形数据的缓存区为数组，并添加设置波形数据和绘制波形分离后的接口|
///
#include "stdafx.h"
#include <float.h>
#include "WaveFormGramCtrl.h"

///
/// @brief
///     控件的背景颜色
///
#define WFG_CTRL_BKGND_COLOR    RGB(0,0,0)            ///< 黑色

///
/// @brief
///     字体的默认设置，包括字体样式以及标题、X-Y标尺刻度、波形描述信息和游标描述信息的字体大小和颜色
///
#define WFG_FONT_STYLE             _T("微软雅黑")
#define WFG_TITLE_FONT_SIZE        20

/**********************颜色修改************************************************************/
#define WFG_TITLE_FONT_COLOR       RGB(255,255,255)      ///< 白色
#define WFG_SCALE_FONT_SIZE        15
#define WFG_SCALE_FONT_COLOR       RGB(170,170,170)      ///< 暖灰色
#define WFG_WAVEDESC_FONT_SIZE     15
#define WFG_CURSORDESC_FONT_SIZE   10
#define WFG_CURSORDESC1_FONT_COLOR RGB(255,140,0)        ///< 深橙色
#define WFG_CURSORDESC2_FONT_COLOR RGB(220,20,60)        ///< 腥红
///
/// @brief
///     波形显示区域的默认设置：边框和内部网格的线条的类型、宽度和颜色
///
#define WFG_WAVE_AREA_BORDER_STYLE PS_SOLID
#define WFG_WAVE_AREA_BORDER_WIDTH 1
#define WFG_WAVE_AREA_GRID_STYLE   PS_DASH
#define WFG_WAVE_AREA_GRID_WIDTH   0
#define WFG_WAVE_AREA_GRID_COLOR   RGB(128,128,105)   ///< 冷灰色

///
/// @brief
///     波形线条的默认设置：波形线条的类型、宽度和前6条的默认颜色(红、黄、森林绿、猩红、橙、草绿)
///
#define WFG_WAVE_LINE_STYLE        PS_SOLID
#define WFG_WAVE_LINE_WIDTH        1
static const COLORREF k_WaveColor[6] = {RGB(255,0,0),  RGB(255,255,0),RGB(34,139,34),
                                        RGB(220,20,60),RGB(255,165,0),RGB(124,252,0)};

///
/// @brief
///     游标的默认设置：颜色、线条样式、线条宽度、游标1的左右矩形和游标2的左右三角形的宽度
///
#define WFG_CURSOR1_COLOR        WFG_CURSORDESC1_FONT_COLOR
#define WFG_CURSOR1_LINE_STYLE   PS_SOLID
#define WFG_CURSOR1_LINE_WIDTH   1
#define WFG_CURSOR1_RECT_WIDTH   3
#define WFG_CURSOR1_RECT_HEIGHT  3
#define WFG_CURSOR2_COLOR        WFG_CURSORDESC2_FONT_COLOR
#define WFG_CURSOR2_LINE_STYLE   PS_SOLID
#define WFG_CURSOR2_LINE_WIDTH   1
#define WFG_CURSOR2_TRIG_WIDTH   3

#define WFG_DRAW_WAVE_DATA_BUF_MAX_SIZE 256*10

IMPLEMENT_DYNAMIC(CWaveFormGramCtrl, CStatic)

///
/// @brief
///     构造函数
///
CWaveFormGramCtrl::CWaveFormGramCtrl()
: m_pOldBkgndBmp(NULL)
, m_pOldForgndBmp(NULL)
, m_clBkgndColor(WFG_CTRL_BKGND_COLOR)
, m_clWaveAreaGridColor(WFG_WAVE_AREA_GRID_COLOR)
, m_clTitleColor(WFG_TITLE_FONT_COLOR)
, m_clScaleValColor(WFG_SCALE_FONT_COLOR)
, m_clCursor1Color(WFG_CURSOR1_COLOR)
, m_clCursor2Color(WFG_CURSOR2_COLOR)
, m_strCursor1Scale(_T("T1"))
, m_strCursor2Scale(_T("T2"))
, m_szWaveDrawCount(SIZE_MAX)
, m_szCursor1Pos(0)
, m_szCursor2Pos(0)
, m_iXHalfGridCount(3)
, m_iYHalfGridCount(4)
, m_iXGramCount(1)
, m_iYGramCount(1)
, m_iLeftMargin(4)
, m_iRightMargin(4)
, m_iTopMargin(4)
, m_iBottomMargin(4)
, m_bShowTitle(FALSE)
, m_bShowWaveDesc(TRUE)
, m_bShowWaveAreaHorGrid(TRUE)
, m_bShowWaveAreaHorCentreGrid(TRUE)
, m_bShowWaveAreaVerGrid(TRUE)
, m_bShowXScaleVal(FALSE)
, m_bShowYScaleVal(TRUE)
, m_bShowCursor1(FALSE)
, m_bShowCursor2(FALSE)
, m_bShowCursor1Desc(FALSE)
, m_bShowCursor2Desc(FALSE)
, m_bShowCursor1Scale(FALSE)
, m_bShowCursor2Scale(FALSE)
, m_iYInterValCharNum(8)
, m_iXGramSpace(0)
, m_iYGramSpace(0)
, m_szDrawWaveDataBufSize(WFG_DRAW_WAVE_DATA_BUF_MAX_SIZE)
{
//	m_pDrawWaveDataBuf.reserve(256*10);
	m_pDrawWaveDataBuf = new POINT*[m_iXGramCount*m_iYGramCount];
	for ( int i = 0; i < m_iXGramCount*m_iYGramCount; i++ )
	{
		m_pDrawWaveDataBuf[i] = new POINT[m_szDrawWaveDataBufSize];
	}
	m_vDrawWaveDataNum.resize(m_iXGramCount*m_iYGramCount,0);
	m_fX0ScaleVal.resize(m_iXGramCount*m_iYGramCount,0.0f);
	m_fXScaleInterval.resize(m_iXGramCount*m_iYGramCount,10.0f);
	m_fYScaleInterval.resize(m_iXGramCount*m_iYGramCount,10.0f);
	m_strYUnit.resize(m_iXGramCount*m_iYGramCount,_T(""));
	m_szCurWaveCount.resize(m_iXGramCount*m_iYGramCount,0);
	m_szXOneGridPointCount.resize(m_iXGramCount*m_iYGramCount,10);
	m_strTitle.resize(m_iXGramCount*m_iYGramCount,_T(""));
	m_clWaveColor.reserve(m_iXGramCount*m_iYGramCount);
	m_clWaveDescColor.reserve(m_iXGramCount*m_iYGramCount);
	m_szCursor1Pos.resize(m_iXGramCount*m_iYGramCount,0);
	m_szCursor2Pos.resize(m_iXGramCount*m_iYGramCount,0);
	m_szWaveDataMaxSize.resize(m_iXGramCount*m_iYGramCount,0);
	for ( int i = 0; i < m_iXGramCount*m_iYGramCount; i++ )
	{
		std::vector<COLORREF> *pColor = new std::vector<COLORREF>;
		pColor->reserve(6);
		for ( int j = 0; j < 6; j++ )
		{
			pColor->push_back(k_WaveColor[j]);
		}
		m_clWaveColor.push_back(pColor);

		pColor = new std::vector<COLORREF>;
		pColor->reserve(6);
		for ( int j = 0; j < 6; j++ )
		{
			pColor->push_back(k_WaveColor[j]);
		}
		m_clWaveDescColor.push_back(pColor);
	}
	m_cTitleFont.CreateFont(WFG_TITLE_FONT_SIZE,0,0,0,FW_NORMAL,FALSE,0,0,0,0,0,0,0,WFG_FONT_STYLE);
	m_cScaleValFont.CreateFont(WFG_SCALE_FONT_SIZE,0,0,0,FW_NORMAL,FALSE,0,0,0,0,0,0,0,WFG_FONT_STYLE);
	m_cWaveDescFont.CreateFont(WFG_WAVEDESC_FONT_SIZE,0,0,0,FW_NORMAL,FALSE,0,0,0,0,0,0,0,WFG_FONT_STYLE);
	m_cCursorDescFont.CreateFont(WFG_CURSORDESC_FONT_SIZE,0,0,0,FW_NORMAL,FALSE,0,0,0,0,0,0,0,WFG_FONT_STYLE);
}

///
/// @brief
///     析构函数
///
CWaveFormGramCtrl::~CWaveFormGramCtrl()
{
	std::vector<std::vector<COLORREF> *>::iterator it;
	for( it = m_clWaveColor.begin(); it != m_clWaveColor.end(); it++ )
	{
		if ( (*it) != NULL )
		{
			delete (*it);
			(*it) = NULL;
		}
	}
	for( it = m_clWaveDescColor.begin(); it != m_clWaveDescColor.end(); it++ )
	{
		if ( (*it) != NULL )
		{
			delete (*it);
			(*it) = NULL;
		}
	}
	
	if ( m_pDrawWaveDataBuf != NULL )
	{
		for ( int i = 0; i < m_iXGramCount*m_iYGramCount; i++ )
		{
			if ( m_pDrawWaveDataBuf[i] != NULL )
			{
				delete[] m_pDrawWaveDataBuf[i];
				m_pDrawWaveDataBuf[i] = NULL;
			}
		}
		delete[] m_pDrawWaveDataBuf;
		m_pDrawWaveDataBuf = NULL;
	}
}

///
/// @brief
///     设置某一波形图的某一波形的颜色，若超过范围则多出的部分为默认颜色
///
/// @param[in]
///     gramIdx - 波形图的顺序号，从0开始（按从左往右，再从上到下的顺序）
/// @param[in]
///     waveIdx - 波形的顺序号，从0开始
/// @param[in]
///     clr     - 颜色值
///
/// @return
///
///
void CWaveFormGramCtrl::SetWaveColor(const size_t gramIdx, const size_t waveIdx, const COLORREF clr)
{
	// 增加新的波形颜色表
	if ( gramIdx >= m_clWaveColor.size() )
	{
		for ( size_t i = m_clWaveColor.size(); i <= gramIdx; i++ )
		{
			std::vector<COLORREF> *pColor = new std::vector<COLORREF>;
			pColor->reserve(6);
			for ( size_t j = 0; j < 6; j++ )
			{
				pColor->push_back(k_WaveColor[j]);
			}
			m_clWaveColor.push_back(pColor);
		}
	}

	// 设置波形颜色表
	std::vector<COLORREF> *pColor = m_clWaveColor.at(gramIdx);
	if ( waveIdx >= pColor->size() )
	{
		for ( size_t i = pColor->size(); i <= waveIdx; i++ )
		{
			if ( i == waveIdx )
			{
				pColor->push_back(clr);
				break;
			}
			if ( i < 6 )
				pColor->push_back(k_WaveColor[i]);
			else
				pColor->push_back(k_WaveColor[0]);
		}
	}
	else
		pColor->at(waveIdx) = clr;
}

///
/// @brief
///     设置某一波形图的某一波形的描述信息的颜色，若超过范围则多出的部分为默认颜色
///
/// @param[in]
///     gramIdx - 波形图的顺序号，从0开始（按从左往右，再从上到下的顺序）
/// @param[in]
///     waveIdx - 波形的顺序号，从0开始
/// @param[in]
///     clr     - 颜色值
///
/// @return
///
///
void CWaveFormGramCtrl::SetWaveDescColor(const size_t gramIdx, const size_t waveIdx, const COLORREF clr)
{
	// 增加新的波形颜色表
	if ( gramIdx >= m_clWaveDescColor.size() )
	{
		for ( size_t i = m_clWaveDescColor.size(); i <= gramIdx; i++ )
		{
			std::vector<COLORREF> *pColor = new std::vector<COLORREF>;
			pColor->reserve(6);
			for ( size_t j = 0; j < 6; j++ )
			{
				pColor->push_back(k_WaveColor[j]);
			}
			m_clWaveDescColor.push_back(pColor);
		}
	}

	// 设置波形颜色表
	std::vector<COLORREF> *pColor = m_clWaveDescColor.at(gramIdx);
	if ( waveIdx >= pColor->size() )
	{
		for ( size_t i = pColor->size(); i <= waveIdx; i++ )
		{
			if ( i == waveIdx )
			{
				pColor->push_back(clr);
				break;
			}
			if ( i < 6 )
				pColor->push_back(k_WaveColor[i]);
			else
				pColor->push_back(k_WaveColor[0]);
		}
	}
	else
		pColor->at(waveIdx) = clr;
}

///
/// @brief
///     设置控件的标题（位于控件波形显示区域的右上角）
///
/// @param[in]
///     idxSeq - 波形图的顺序号（按从左往右，再从上到下的顺序），从0开始
/// @param[in]
///     str    - 控件的标题
///
/// @return
///
///
void CWaveFormGramCtrl::SetTitle(const size_t idxSeq, const CString &str)
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
///     设置控件的X-Y标尺的分度值
///
/// @param[in]
///     idxSeq    - 波形图的顺序号（按从左往右，再从上到下的顺序），从0开始
/// @param[in]
///     xInterval - X标尺的分度值（即每一小格的点数）
/// @param[in]
///     yInterval - Y标尺的分度值（即每一小格对应的数据值）
/// @param[in]
///     yUnit     - Y标尺的单位
///
/// @return
///
///
void CWaveFormGramCtrl::SetScaleInterval(const size_t idxSeq, const float xInterval, const float yInterval, const CString &yUint)
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
///     设置控件的X标尺的起始点X0的刻度值
///
/// @param[in]
///     idxSeq    - 波形图的顺序号（按从左往右，再从上到下的顺序），从0开始
/// @param[in]
///     x0Val     - 起始点X0的刻度值
///
/// @return
///
///
void CWaveFormGramCtrl::SetX0ScaleVal(size_t idxSeq, float x0Val)
{
	if ( idxSeq >= m_fX0ScaleVal.size() )
	{
		for ( size_t i = m_fX0ScaleVal.size(); i <= idxSeq; i++ )
		{
			if ( i == idxSeq )
			{
				m_fX0ScaleVal.push_back(x0Val);
				break;
			}
			// 超出部分为默认设置
			m_fX0ScaleVal.push_back(0.0f);
		}
	}
	else
	{
		m_fX0ScaleVal[idxSeq] = x0Val;
	}
}

///
/// @brief
///     设置控件的各波形图的X标尺方向的每一网格的点数
///
/// @param[in]
///     idxSeq    - 波形图的顺序号（按从左往右，再从上到下的顺序），从0开始
/// @param[in]
///     xPtCount  - 每一小格的点数
///
/// @return
///
///
void CWaveFormGramCtrl::SetXOneGridPointCount(const size_t idxSeq, const size_t xPtCount)
{
	if ( idxSeq >= m_szXOneGridPointCount.size() )
	{
		for ( size_t i = m_szXOneGridPointCount.size(); i <= idxSeq; i++ )
		{
			if ( i == idxSeq )
			{
				m_szXOneGridPointCount.push_back(xPtCount);
				break;
			}
			// 超出部分为默认值
			m_szXOneGridPointCount.push_back(10);
		}
	}
	else
	{
		m_szXOneGridPointCount.at(idxSeq) = xPtCount;
	}
}

///
/// @brief
///     设置控件的波形图的个数
///
/// @param[in]
///     xGramCount - X标尺方向上的波形图个数
/// @param[in]
///     yGramCount - Y标尺方向上的波形图个数
///
/// @return
///
///
void CWaveFormGramCtrl::SetGramCount(const int xGramCount, const int yGramCount)
{
	// 重置待绘制波形数据的缓存区
	if ( m_iXGramCount*m_iYGramCount != xGramCount*yGramCount )
	{
		// 释放待绘制波形数据的缓存区
		if ( m_pDrawWaveDataBuf != NULL )
		{
			for ( int i = 0; i < m_iXGramCount*m_iYGramCount; i++ )
			{
				if ( m_pDrawWaveDataBuf[i] != NULL )
				{
					delete[] m_pDrawWaveDataBuf[i];
					m_pDrawWaveDataBuf[i] = NULL;
				}
			}
			delete[] m_pDrawWaveDataBuf;
			m_pDrawWaveDataBuf = NULL;
		}

		// 创建待绘制波形数据的缓存区
		m_pDrawWaveDataBuf = new POINT*[xGramCount*yGramCount];
		for ( int i = 0; i < xGramCount*yGramCount; i++ )
		{
			m_pDrawWaveDataBuf[i] = new POINT[m_szDrawWaveDataBufSize];
		}
	}
	// 重新初始化配置
	m_iXGramCount = xGramCount;
	m_iYGramCount = yGramCount;
	m_vDrawWaveDataNum.resize(m_iXGramCount*m_iYGramCount,0);
	m_fX0ScaleVal.resize(m_iXGramCount*m_iYGramCount,0.0f);
	m_fXScaleInterval.resize(m_iXGramCount*m_iYGramCount,10.0f);
	m_fYScaleInterval.resize(m_iXGramCount*m_iYGramCount,10.0f);
	m_strYUnit.resize(m_iXGramCount*m_iYGramCount,_T(""));
	m_szCurWaveCount.resize(m_iXGramCount*m_iYGramCount,0);
	m_szXOneGridPointCount.resize(m_iXGramCount*m_iYGramCount,10);
	m_strTitle.resize(m_iXGramCount*m_iYGramCount,_T(""));
	m_szCursor1Pos.resize(m_iXGramCount*m_iYGramCount,0);
	m_szCursor2Pos.resize(m_iXGramCount*m_iYGramCount,0);
	m_szWaveDataMaxSize.resize(m_iXGramCount*m_iYGramCount,0);

	// 重置波形及其描述信息的颜色表信息
	int prevSz = (int)m_clWaveColor.size();
	if ( prevSz < xGramCount*yGramCount )
	{
		m_clWaveColor.resize(m_iXGramCount*m_iYGramCount);
		for ( int i = prevSz; i < xGramCount*yGramCount; i++ )
		{
			std::vector<COLORREF> *pColor = new std::vector<COLORREF>;
			pColor->reserve(6);
			for ( int j = 0; j < 6; j++ )
			{
				pColor->push_back(k_WaveColor[j]);
			}
			m_clWaveColor[i] = pColor;
		}
	}
	prevSz = (int)m_clWaveDescColor.size();
	if ( prevSz < xGramCount*yGramCount )
	{
		m_clWaveDescColor.resize(m_iXGramCount*m_iYGramCount);
		for ( int i = prevSz; i < xGramCount*yGramCount; i++ )
		{
			std::vector<COLORREF> *pColor = new std::vector<COLORREF>;
			pColor->reserve(6);
			for ( int j = 0; j < 6; j++ )
			{
				pColor->push_back(k_WaveColor[j]);
			}
			m_clWaveDescColor[i] = pColor;
		}
	}
}

///
/// @brief
///     重置待绘制波形数据的缓存区的大小
///
/// @param[in]
///     szSize - 待绘制波形数据的缓存区的大小
///
/// @return
///
///
void CWaveFormGramCtrl::SetDrawWaveDataBufSize(size_t szSize)
{
	// 释放待绘制波形数据的缓存区
	if ( m_pDrawWaveDataBuf != NULL )
	{
		for ( int i = 0; i < m_iXGramCount*m_iYGramCount; i++ )
		{
			if ( m_pDrawWaveDataBuf[i] != NULL )
			{
				delete[] m_pDrawWaveDataBuf[i];
				m_pDrawWaveDataBuf[i] = NULL;
			}
		}
		delete[] m_pDrawWaveDataBuf;
		m_pDrawWaveDataBuf = NULL;
	}

	// 创建待绘制波形数据的缓存区
	m_pDrawWaveDataBuf = new POINT*[m_iXGramCount*m_iYGramCount];
	for ( int i = 0; i < m_iXGramCount*m_iYGramCount; i++ )
	{
		m_pDrawWaveDataBuf[i] = new POINT[szSize];
	}
	m_szDrawWaveDataBufSize = szSize;
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
void CWaveFormGramCtrl::SetTitleFont(CFont *pFont)
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
void CWaveFormGramCtrl::SetScaleValFont(CFont *pFont)
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
///     设置控件波形描述信息的字体
///
/// @param[in]
///     pFont - 指向字体的指针
///
/// @return
///
///
void CWaveFormGramCtrl::SetWaveDescFont(CFont *pFont)
{
	if ( pFont == NULL || pFont->GetSafeHandle() == NULL )
		return;
	m_cWaveDescFont.DeleteObject();
	LOGFONT logFont;
	pFont->GetLogFont(&logFont);
	m_cWaveDescFont.CreateFontIndirect(&logFont);
}

///
/// @brief
///     设置控件游标描述信息的字体
///
/// @param[in]
///     pFont - 指向字体的指针
///
/// @return
///
///
void CWaveFormGramCtrl::SetCursorDescFont(CFont *pFont)
{
	if ( pFont == NULL || pFont->GetSafeHandle() == NULL )
		return;
	m_cCursorDescFont.DeleteObject();
	LOGFONT logFont;
	pFont->GetLogFont(&logFont);
	m_cCursorDescFont.CreateFontIndirect(&logFont);
}

///
/// @brief
///     设置波形图当前显示的波形的采样点数
///
/// @param[in]
///     waveDrawCount - 当前显示的波形的采样点数
///
/// @return
///
///
void CWaveFormGramCtrl::SetWaveDrawCount(size_t waveDrawCount)
{
	m_szWaveDrawCount = waveDrawCount;
//	m_pDrawWaveDataBuf.reserve(waveDrawCount);
}

///
/// @brief
///     初始化控件
///
/// @return
///
///
void CWaveFormGramCtrl::Init()
{
	DrawBkgnd();
	ResetForgnd();
}

///
/// @brief
///     擦除波形显示区域的全部波形信息
///
/// @return
///
///
void CWaveFormGramCtrl::EraseWave()
{
	CRect rt;
	GetClientRect(rt);
	m_cForgndDC.BitBlt(0,0,rt.Width(),rt.Height(),&m_cBkgndDC,0,0,SRCCOPY);
	for ( size_t i = 0; i < m_szCurWaveCount.size(); i++ )
	{
		m_szCurWaveCount.at(i) = 0;
		m_szWaveDataMaxSize.at(i) = 0;
	}
}

///
/// @brief
///     设置待绘制波形的采样点数据，与DrawWaveFromBuf()配合使用
///
/// @param[in]
///     idxSeq    - 波形图的顺序号（从左往右，再从上到下），从0开始
/// @param[in]
///     dataBuf   - 保存波形数据的缓存
/// @param[in]
///     bufSize   - 缓存大小
///
/// @return
///     
///
void CWaveFormGramCtrl::SetWaveData(const size_t idxSeq, const float *dataBuf, const size_t bufSize)
{
	if (NULL == dataBuf || 1 >= bufSize || idxSeq >= (size_t)m_iXGramCount*m_iYGramCount )
		return ;

	// 获取特定波形图中各波形的最大点数
	size_t maxSize = m_szWaveDataMaxSize[idxSeq];
	m_szWaveDataMaxSize[idxSeq] = maxSize > bufSize ? maxSize : bufSize;

	// 计算当前要绘制的波形显示区域的矩形域
	int xPt = (int)idxSeq%m_iXGramCount;
	int yPt = (int)idxSeq/m_iXGramCount;
	CRect waveArea(m_rtWaveArea.left   + m_iXGramSpace * xPt,
		m_rtWaveArea.top    + m_iYGramSpace * yPt,
		m_rtWaveArea.right  + m_iXGramSpace * xPt,
		m_rtWaveArea.bottom + m_iYGramSpace * yPt);

	// 计算Y标尺方向的最大高度,X-Y标尺方向的每个点的步长
	float maxXValue   = (float)m_iXHalfGridCount * m_szXOneGridPointCount.at(idxSeq) * 2.0f;
	float maxYValue   = (float)m_iYHalfGridCount * m_fYScaleInterval.at(idxSeq);
	float nStepWidth  = (float)waveArea.Width() / (maxXValue);
	float nStepHeight = (float)waveArea.Height() / (2*maxYValue);
	if ( maxYValue == 0 )
		nStepHeight = 0;
	// 坐标点的Y值在Y标尺方向上的参考值
	float yRefPT = (float)waveArea.top + (float)waveArea.Height()/2.0f;

	// 计算要绘制的波形的实际点数及波形采样点的起始地址
	size_t drawCount = m_szWaveDrawCount;
	if ( drawCount == SIZE_MAX )
		drawCount = bufSize;
	if ( drawCount > (size_t)maxXValue+1 )
		drawCount = (size_t)maxXValue+1;
	if ( m_szCursor1Pos[idxSeq]+drawCount > bufSize )
		drawCount = bufSize - m_szCursor1Pos[idxSeq];
	float *pWaveData = const_cast<float*>(&dataBuf[m_szCursor1Pos[idxSeq]]);

	// 根据分辨率过滤采样点，每个像素点(x)最多绘制2个采样点，超过2个则去最大最小值
	float tmp;
	size_t waveDataNum = 0;
	POINT         maxVal = {};
	POINT         minVal = {};
	BOOL          equal  = FALSE;
	for (size_t i=0;i<drawCount;++i)
	{
		if ( i > maxXValue || waveDataNum >= m_szDrawWaveDataBufSize )
			break;
		POINT pt;
		pt.x = int(waveArea.left+i*nStepWidth);

		// Y值有效性判断
		if ( _finite(pWaveData[i]) == FALSE )
			tmp = 0;
		else if(pWaveData[i] > maxYValue)
			tmp = maxYValue;
		else if (pWaveData[i] < -maxYValue)
			tmp = -maxYValue;
		else 
			tmp = pWaveData[i];

		pt.y = int(yRefPT - tmp * nStepHeight/* + 0.5f*/);    // 4舍5入
		if (0 == i)
		{
			maxVal = pt;
			continue;
		}
		if (equal)
		{
			if (maxVal.x == pt.x)
			{
				if (pt.y > maxVal.y)
					maxVal = pt;
				else if (pt.y < minVal.y)
					minVal = pt;
			}
			else
			{
				equal   = FALSE;
				m_pDrawWaveDataBuf[idxSeq][waveDataNum]   = maxVal;
				m_pDrawWaveDataBuf[idxSeq][waveDataNum+1] = minVal;
				maxVal  = pt;
				waveDataNum += 2;
			}
		}
		else
		{
			if (maxVal.x == pt.x)
			{
				if (pt.y > maxVal.y)
				{
					minVal = maxVal;
					maxVal = pt;
				}
				else
				{
					minVal = pt;
				}
				equal = TRUE;
			}
			else
			{
				if (idxSeq == 1)
				{
					int a = 0;
				}
				m_pDrawWaveDataBuf[idxSeq][waveDataNum] = maxVal;
				maxVal = pt;
				waveDataNum++;
			}
		}
	}
	if ( waveDataNum < m_szDrawWaveDataBufSize )
	{
		m_pDrawWaveDataBuf[idxSeq][waveDataNum] = maxVal;
		waveDataNum++;
	}
	if (equal && waveDataNum < m_szDrawWaveDataBufSize )
	{
		m_pDrawWaveDataBuf[idxSeq][waveDataNum] = minVal;
		waveDataNum++;
	}
	m_vDrawWaveDataNum[idxSeq] = waveDataNum;
}

///
/// @brief
///     根据缓存的波形数据绘制波形，与SetWaveData()配合使用
///
/// @param[in]
///     idxSeq    - 波形图的顺序号（从左往右，再从上到下），从0开始
/// @param[in]
///     waveDesc  - 波形的描述信息，默认为2行，用“\r\n”分开
///
/// @return
///     
///
void CWaveFormGramCtrl::DrawWaveFromBuf(const size_t idxSeq, const CString &waveDesc)
{
	if ( idxSeq >= (size_t)m_iXGramCount*m_iYGramCount )
		return ;

	// 计算当前要绘制的波形显示区域的矩形域
	int xPt = (int)idxSeq%m_iXGramCount;
	int yPt = (int)idxSeq/m_iXGramCount;
	CRect waveArea(m_rtWaveArea.left   + m_iXGramSpace * xPt,
		m_rtWaveArea.top    + m_iYGramSpace * yPt,
		m_rtWaveArea.right  + m_iXGramSpace * xPt,
		m_rtWaveArea.bottom + m_iYGramSpace * yPt);

	// 设置波形线条颜色
	size_t curWaveIdx = m_szCurWaveCount.at(idxSeq);
	COLORREF curWaveColor;
	if ( curWaveIdx >= m_clWaveColor.at(idxSeq)->size() )
		curWaveColor = m_clWaveColor.at(idxSeq)->at(0);
	else
		curWaveColor = m_clWaveColor.at(idxSeq)->at(curWaveIdx);

	// 绘制波形曲线
	CPen pen;
	pen.CreatePen(WFG_WAVE_LINE_STYLE,WFG_WAVE_LINE_WIDTH,curWaveColor);
	CPen *pOldPen = m_cForgndDC.SelectObject(&pen);
	m_cForgndDC.Polyline(&m_pDrawWaveDataBuf[idxSeq][0],m_vDrawWaveDataNum[idxSeq]);
	m_cForgndDC.SelectObject(pOldPen);
	pen.DeleteObject();

	// 绘制波形描述信息，位于波形显示区域的左上角
	if ( m_bShowWaveDesc )
	{
		// 设置波形线条颜色
		COLORREF curWaveDescColor;
		if ( curWaveIdx >= m_clWaveDescColor.at(idxSeq)->size() )
			curWaveDescColor = m_clWaveDescColor.at(idxSeq)->at(0);
		else
			curWaveDescColor = m_clWaveDescColor.at(idxSeq)->at(curWaveIdx);

		CFont *pOldFont = m_cForgndDC.SelectObject(&m_cTitleFont);
		COLORREF oldColor = m_cForgndDC.SetTextColor(curWaveDescColor);
		int oldMode = m_cForgndDC.SetBkMode(TRANSPARENT);
		CSize titleSz = m_cForgndDC.GetTextExtent(_T("0"));

		m_cForgndDC.SelectObject(&m_cWaveDescFont);
		int pos = waveDesc.Find(_T("\r\n"));
		if ( pos == -1)
		{
			CSize descSz = m_cForgndDC.GetTextExtent(waveDesc);
			m_cForgndDC.ExtTextOutW(waveArea.right-descSz.cx-2,int(waveArea.top+4+descSz.cy*2*(curWaveIdx)+titleSz.cy),
				ETO_OPAQUE,NULL,waveDesc,waveDesc.GetLength(),NULL);
		}
		else
		{
			CString str;
			str = waveDesc.Left(pos);
			CSize descSz = m_cForgndDC.GetTextExtent(str);
			m_cForgndDC.ExtTextOutW(waveArea.right-descSz.cx-2,int(waveArea.top+4+descSz.cy*2*(curWaveIdx)+titleSz.cy),
				ETO_OPAQUE,NULL,str,str.GetLength(),NULL);
			str = waveDesc.Right(waveDesc.GetLength()-pos-2);
			descSz = m_cForgndDC.GetTextExtent(str);
			m_cForgndDC.ExtTextOutW(waveArea.right-descSz.cx-2,int(waveArea.top+4+descSz.cy*2*(curWaveIdx)+descSz.cy+titleSz.cy),
				ETO_OPAQUE,NULL,str,str.GetLength(),NULL);
		}

		m_cForgndDC.SetBkMode(oldMode);
		m_cForgndDC.SetTextColor(oldColor);
		m_cForgndDC.SelectObject(pOldFont);
	}

	// 绘制游标2
	DrawCursor2(&m_cForgndDC,idxSeq,waveArea);

	m_szCurWaveCount.at(idxSeq) = curWaveIdx + 1;
}

///
/// @brief
///     绘制波形：覆盖式（与前面绘制的波形一起显示）
///
/// @param[in]
///     idxSeq    - 波形图的顺序号（从左往右，再从上到下），从0开始
/// @param[in]
///     dataBuf   - 保存波形数据的缓存
/// @param[in]
///     bufSize   - 缓存大小
/// @param[in]
///     waveDesc  - 波形的描述信息，默认为2行，用“\r\n”分开
///
/// @return
///     
///
void CWaveFormGramCtrl::DrawWave(const size_t idxSeq, const float *dataBuf, const size_t bufSize, const CString &waveDesc)
{
	if (NULL == dataBuf || 1 >= bufSize || idxSeq >= (size_t)m_iXGramCount*m_iYGramCount )
		return ;

	// 获取特定波形图中各波形的最大点数
	size_t maxSize = m_szWaveDataMaxSize[idxSeq];
	m_szWaveDataMaxSize[idxSeq] = maxSize > bufSize ? maxSize : bufSize;

	// 计算当前要绘制的波形显示区域的矩形域
	int xPt = (int)idxSeq%m_iXGramCount;
	int yPt = (int)idxSeq/m_iXGramCount;
	CRect waveArea(m_rtWaveArea.left   + m_iXGramSpace * xPt,
	               m_rtWaveArea.top    + m_iYGramSpace * yPt,
	               m_rtWaveArea.right  + m_iXGramSpace * xPt,
	               m_rtWaveArea.bottom + m_iYGramSpace * yPt);

	// 计算Y标尺方向的最大高度，X-Y标尺方向的每个点的步长
	float maxXValue   = (float)m_iXHalfGridCount * m_szXOneGridPointCount.at(idxSeq) * 2.0f;
	float maxYValue   = (float)m_iYHalfGridCount * m_fYScaleInterval.at(idxSeq);
	float nStepWidth  = (float)waveArea.Width() / (maxXValue);
//	float nStepWidth  = (float)waveArea.Width() / ((float)bufSize-1) / 2.0f;
	float nStepHeight = (float)waveArea.Height() / (2*maxYValue);
	if ( maxYValue == 0 )
		nStepHeight = 0;

	// 坐标点的Y值在Y标尺方向上的参考值
	float yRefPT = (float)waveArea.top + (float)waveArea.Height()/2.0f;

	// 计算要绘制的波形的实际点数及波形采样点的起始地址
	size_t drawCount = m_szWaveDrawCount;
	if ( drawCount == SIZE_MAX )
		drawCount = bufSize;
	if ( drawCount > (size_t)maxXValue+1 )
		drawCount = (size_t)maxXValue+1;
	if ( m_szCursor1Pos[idxSeq]+drawCount > bufSize )
		drawCount = bufSize - m_szCursor1Pos[idxSeq];
	float *pWaveData = const_cast<float*>(&dataBuf[m_szCursor1Pos[idxSeq]]);

	// 设置波形线条颜色
	size_t curWaveIdx = m_szCurWaveCount.at(idxSeq);
	COLORREF curWaveColor;
	if ( curWaveIdx >= m_clWaveColor.at(idxSeq)->size() )
		curWaveColor = m_clWaveColor.at(idxSeq)->at(0);
	else
		curWaveColor = m_clWaveColor.at(idxSeq)->at(curWaveIdx);

	// 绘制波形曲线
	CPen pen;
	pen.CreatePen(WFG_WAVE_LINE_STYLE,WFG_WAVE_LINE_WIDTH,curWaveColor);
	CPen *pOldPen = m_cForgndDC.SelectObject(&pen);
	// 以下两种绘制方式
	// 第一种，直接点连点绘制，可能较粗糙，操作的dc次数较多
	// 第二种，根据分辨率过滤采样点，每个像素点(x)最多绘制2个采样点，超过2个则去最大最小值
#if 0
	m_cForgndDC.MoveTo(waveArea.left,(int)(yRefPT - pWaveData[0]*nStepHeight));
	for (int i = 1;i<drawCount;++i)
	{
		if ( (int)(i*nStepWidth) > waveArea.Width())
			break;
		m_cForgndDC.LineTo((int)(waveArea.left+i*nStepWidth),(int)(yRefPT - pWaveData[i]*nStepHeight));
	}
#else 
	float tmp;
	size_t waveDataNum = 0;
	POINT         maxVal = {};
	POINT         minVal = {};
	BOOL          equal  = FALSE;
	for (size_t i=0;i<drawCount;++i)
	{
		if ( i > maxXValue || waveDataNum >= m_szDrawWaveDataBufSize )
			break;
		POINT pt;
		pt.x = int(waveArea.left+i*nStepWidth);

		// Y值有效性判断
		if ( _finite(pWaveData[i]) == FALSE )
			tmp = 0;
 
		else if(pWaveData[i] > maxYValue)

			tmp = maxYValue;

		else if (pWaveData[i] < -maxYValue)

			tmp = -maxYValue; 
		else  
			tmp = pWaveData[i];

		pt.y = int(yRefPT - tmp * nStepHeight/* + 0.5f*/);    // 4舍5入
		if (0 == i)
		{
			maxVal = pt;
			continue;
		}
		if (equal)
		{
			if (maxVal.x == pt.x)
			{
				if (pt.y > maxVal.y)
					maxVal = pt;
				else if (pt.y < minVal.y)
					minVal = pt;
			}
			else
			{
				equal   = FALSE;
				m_pDrawWaveDataBuf[idxSeq][waveDataNum]   = maxVal;
				m_pDrawWaveDataBuf[idxSeq][waveDataNum+1] = minVal;
				maxVal  = pt;
				waveDataNum += 2;
			}
		}
		else
		{
			if (maxVal.x == pt.x)
			{
				if (pt.y > maxVal.y)
				{
					minVal = maxVal;
					maxVal = pt;
				}
				else
				{
					minVal = pt;
				}
				equal = TRUE;
			}
			else
			{
				if (idxSeq == 1)
				{
					int a = 0;
				}
				m_pDrawWaveDataBuf[idxSeq][waveDataNum] = maxVal;
				maxVal = pt;
				waveDataNum++;
			}
		}
	}
	if ( waveDataNum < m_szDrawWaveDataBufSize )
	{
		m_pDrawWaveDataBuf[idxSeq][waveDataNum] = maxVal;
		waveDataNum++;
	}
	
	if (equal && waveDataNum < m_szDrawWaveDataBufSize )
	{
		m_pDrawWaveDataBuf[idxSeq][waveDataNum] = minVal;
		waveDataNum++;
	}
	m_vDrawWaveDataNum[idxSeq] = waveDataNum;



	m_cForgndDC.Polyline(&m_pDrawWaveDataBuf[idxSeq][0],waveDataNum);

#if !CHUN_HUA    //通过对最顶和最低端用背景重绘的方法去掉连接
	CPen pen1;
	pen1.CreatePen(WFG_WAVE_LINE_STYLE,WFG_WAVE_LINE_WIDTH,m_clBkgndColor);
	m_cForgndDC.SelectObject(&pen1);
	m_cForgndDC.MoveTo(waveArea.left,waveArea.top);
	m_cForgndDC.LineTo(waveArea.right, waveArea.top);
	m_cForgndDC.MoveTo(waveArea.left,waveArea.bottom);
	m_cForgndDC.LineTo(waveArea.right, waveArea.bottom);
	pen1.DeleteObject();
#endif

#endif
	m_cForgndDC.SelectObject(pOldPen);
	pen.DeleteObject();

	// 绘制波形描述信息，位于波形显示区域的左上角
	if ( m_bShowWaveDesc )
	{
		// 设置波形线条颜色
		COLORREF curWaveDescColor;
		if ( curWaveIdx >= m_clWaveDescColor.at(idxSeq)->size() )
			curWaveDescColor = m_clWaveDescColor.at(idxSeq)->at(0);
		else
			curWaveDescColor = m_clWaveDescColor.at(idxSeq)->at(curWaveIdx);

		CFont *pOldFont = m_cForgndDC.SelectObject(&m_cTitleFont);
		COLORREF oldColor = m_cForgndDC.SetTextColor(curWaveDescColor);
		int oldMode = m_cForgndDC.SetBkMode(TRANSPARENT);
		CSize titleSz = m_cForgndDC.GetTextExtent(_T("0"));

		m_cForgndDC.SelectObject(&m_cWaveDescFont);
		int pos = waveDesc.Find(_T("\r\n"));
		if ( pos == -1)
		{
			CSize descSz = m_cForgndDC.GetTextExtent(waveDesc);
			m_cForgndDC.ExtTextOutW(waveArea.right-descSz.cx-2,int(waveArea.top+4+descSz.cy*2*(curWaveIdx)+titleSz.cy),
				ETO_OPAQUE,NULL,waveDesc,waveDesc.GetLength(),NULL);
		}
		else
		{
			CString str;
			str = waveDesc.Left(pos);
			CSize descSz = m_cForgndDC.GetTextExtent(str);
			m_cForgndDC.ExtTextOutW(waveArea.right-descSz.cx-2,int(waveArea.top+4+descSz.cy*2*(curWaveIdx)+titleSz.cy),
				ETO_OPAQUE,NULL,str,str.GetLength(),NULL);
			str = waveDesc.Right(waveDesc.GetLength()-pos-2);
			descSz = m_cForgndDC.GetTextExtent(str);
			m_cForgndDC.ExtTextOutW(waveArea.right-descSz.cx-2,int(waveArea.top+4+descSz.cy*2*(curWaveIdx)+descSz.cy+titleSz.cy),
				ETO_OPAQUE,NULL,str,str.GetLength(),NULL);
		}
		
		m_cForgndDC.SetBkMode(oldMode);
		m_cForgndDC.SetTextColor(oldColor);
		m_cForgndDC.SelectObject(pOldFont);
	}

	// 绘制游标2
	DrawCursor2(&m_cForgndDC,idxSeq,waveArea);

	m_szCurWaveCount.at(idxSeq) = curWaveIdx + 1;
}

///
/// @brief
///     绘制游标的描述信息波形：叠加式（与前面绘制的部分叠加显示）
///
/// @param[in]
///     cursor1Desc   - 游标1的描述信息
/// @param[in]
///     cursor2Desc   - 游标2的描述信息
///
/// @return
///     
///
void CWaveFormGramCtrl::DrawCursorDesc(const CString &cursor1Desc, const CString &cursor2Desc)
{
	if ( !m_bShowCursor1Desc && !m_bShowCursor2Desc )
		return;

	CRect rt;
	GetClientRect(rt);

	CFont *pOldFont = m_cForgndDC.SelectObject(&m_cCursorDescFont);
	COLORREF oldColor = m_cForgndDC.SetTextColor(m_clCursor1Color);
	int oldMode = m_cForgndDC.SetBkMode(TRANSPARENT);
	CSize cursorDescSize = m_cForgndDC.GetTextExtent(_T("0"));
	
	if ( m_bShowCursor1Desc )
	{
		m_cForgndDC.ExtTextOutW(rt.left+m_iLeftMargin,rt.top+m_iTopMargin,
			ETO_OPAQUE,NULL,cursor1Desc,cursor1Desc.GetLength(),NULL);
	}

	if ( m_bShowCursor2Desc )
	{
		m_cForgndDC.SetTextColor(m_clCursor2Color);
		m_cForgndDC.ExtTextOutW(rt.left+m_iLeftMargin,rt.top+m_iTopMargin+cursorDescSize.cy,
			ETO_OPAQUE,NULL,cursor2Desc,cursor2Desc.GetLength(),NULL);
	}

	m_cForgndDC.SetBkMode(oldMode);
	m_cForgndDC.SetTextColor(oldColor);
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
void CWaveFormGramCtrl::Flush(CDC* pDC)
{
	CRect rt;
	GetClientRect(rt);
	if (NULL == pDC)
	{
		CClientDC clientDc(this);	
		clientDc.BitBlt(0,0,rt.Width(),rt.Height(),&m_cForgndDC,0,0,SRCCOPY);
	}
	else
	{
		pDC->BitBlt(0,0,rt.Width(),rt.Height(),&m_cForgndDC,0,0,SRCCOPY);
	}
}

///
/// @brief
///     获取游标1所在的点相对于波形数据缓存的索引位置
///
int  CWaveFormGramCtrl::GetCursor1Pos(size_t idxSeq)
{
	return m_szCursor1Pos.at(idxSeq);
}


///
/// @brief
///     获取游标2所在的点相对于波形数据缓存的索引位置
///
int  CWaveFormGramCtrl::GetCursor2Pos(size_t idxSeq)
{
	return m_szCursor2Pos.at(idxSeq);
}

///
/// @brief
///     右移游标
///
void CWaveFormGramCtrl::Cursor2MoveRight()
{
	if ( !m_bShowCursor2 )
		return;

	for ( size_t i = 0; i < m_szCursor2Pos.size(); i++ )
	{
		// 判断是否到达波形结尾点
		if ( m_szCursor2Pos[i]+1 < m_szWaveDataMaxSize[i] )
			m_szCursor2Pos[i]++;

		// 判断波形是否翻转到前1截波形
		size_t drawCount = m_iXHalfGridCount * m_szXOneGridPointCount[i] * 2 + 1;
		drawCount = drawCount < m_szWaveDrawCount ? drawCount : m_szWaveDrawCount;
		if ( m_szCursor2Pos[i]-m_szCursor1Pos[i] >= drawCount )
		{
			m_szCursor1Pos[i] += drawCount;
			m_fX0ScaleVal[i] += (float)drawCount*m_fXScaleInterval[i]/(float)m_szXOneGridPointCount[i];
			Init();
		}
	}
}

///
/// @brief
///     左移游标
///
void CWaveFormGramCtrl::Cursor2MoveLeft()
{
	if ( !m_bShowCursor2 )
		return;

	for ( size_t i = 0; i < m_szCursor2Pos.size(); i++ )
	{
		// 判断是否到达波形起始点
		if ( m_szCursor2Pos[i] != 0 )
			m_szCursor2Pos[i]--;

		// 判断是否翻转到前1截波形
		size_t drawCount = m_iXHalfGridCount * m_szXOneGridPointCount[i] * 2 + 1;
		drawCount = drawCount < m_szWaveDrawCount ? drawCount : m_szWaveDrawCount;
		if ( m_szCursor2Pos[i] < m_szCursor1Pos[i] && m_szCursor1Pos[i] != 0 )
		{
			if ( m_szCursor1Pos[i] < drawCount )
				m_szCursor1Pos[i] = 0;
			else
				m_szCursor1Pos[i] -= drawCount;
			m_fX0ScaleVal[i] -= (float)drawCount*m_fXScaleInterval[i]/(float)m_szXOneGridPointCount[i];
			Init();
		}
	}
}

BEGIN_MESSAGE_MAP(CWaveFormGramCtrl, CStatic)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


///
/// @brief
///     擦除背景消息处理函数
///
/// @return
///
///
BOOL CWaveFormGramCtrl::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

///
/// @brief
///     重绘消息处理函数
///
/// @return
///
void CWaveFormGramCtrl::OnPaint()
{
// 	PAINTSTRUCT ps;
// 	CDC* pDC = BeginPaint(&ps);
// 	pDC->BitBlt(0,0,rt.Width(),rt.Height(),m_cForgndDC,0,0,SRCCOPY);
// 	EndPaint(&ps);
	CPaintDC dc(this);
	CRect rt;
	GetClientRect(rt);
	dc.BitBlt(0,0,rt.Width(),rt.Height(),&m_cForgndDC,0,0,SRCCOPY);
}

///
/// @brief
///     释放控件资源消息处理函数
///
/// @return
///
void CWaveFormGramCtrl::OnDestory()
{
	CStatic::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
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
void CWaveFormGramCtrl::CreateDCandBmp(const CRect &rt, CDC &dc, CBitmap &bmp, CBitmap **pPOldBmp)
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
void CWaveFormGramCtrl::DrawBkgnd()
{
	CRect rt;
	GetClientRect(rt);

	// 创建背景DC和位图
	CreateDCandBmp(rt,m_cBkgndDC,m_cBkgndBmp,&m_pOldBkgndBmp);

	// 背景填充
	m_cBkgndDC.FillSolidRect(0,0,rt.Width(),rt.Height(),m_clBkgndColor);

	// 更新第1个波形显示区域(0,0)的矩形域
	UpdateWaveAreaSpace(&m_cBkgndDC);

	// 从左往右，从上到下绘制所有波形图的波形显示区域
	for ( int y = 0; y < m_iYGramCount; y++ )
	{
		for ( int x = 0; x < m_iXGramCount; x++ )
		{
			// 计算当前要绘制的波形显示区域的矩形域
			CRect waveArea(m_rtWaveArea.left   + m_iXGramSpace * x,
				           m_rtWaveArea.top    + m_iYGramSpace * y,
						   m_rtWaveArea.right  + m_iXGramSpace * x,
						   m_rtWaveArea.bottom + m_iYGramSpace * y);

			
			// 绘制波形显示区域的边框
		//	DrawWaveAreaBoard(&m_cBkgndDC,waveArea);

			// 绘制X-Y标尺的刻度
			DrawScale(&m_cBkgndDC,y*m_iXGramCount+x,waveArea);

			// 绘制波形显示区域的内部网格
			DrawWaveAreaGrid(&m_cBkgndDC,waveArea);

			// 绘制右上角的标题
			DrawTitle(&m_cBkgndDC,y*m_iXGramCount+x,waveArea);

			// 绘制游标1
			DrawCursor1(&m_cBkgndDC,y*m_iXGramCount+x,waveArea);
		}
	}
}

///
/// @brief
///     重置控件前景
///
/// @return
///
void CWaveFormGramCtrl::ResetForgnd()
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
void CWaveFormGramCtrl::Release()
{
	if ( m_cBkgndDC.GetSafeHdc() != NULL )
	{
		m_cBkgndDC.SelectObject(m_pOldBkgndBmp);
		m_cBkgndDC.DeleteDC();
		m_cBkgndBmp.DeleteObject();
	}
	if ( m_cForgndDC.GetSafeHdc() != NULL )
	{
		m_cForgndDC.SelectObject(m_pOldForgndBmp);
		m_cForgndDC.DeleteDC();
		m_cForgndBmp.DeleteObject();
	}

	m_cTitleFont.DeleteObject();
	m_cScaleValFont.DeleteObject();
	m_cWaveDescFont.DeleteObject();
}

///
/// @brief
///     计算第1个波形显示区域的矩形域
///
/// @param[in]
///     pDC - 设备环境DC的指针
///
/// @return
///     
///
void CWaveFormGramCtrl::UpdateWaveAreaSpace(CDC *pDC)
{
	CRect rt;
	GetClientRect(rt);

	// 游标1、2描述信息
	if ( m_bShowCursor1Desc || m_bShowCursor2Desc )
	{
		CFont *pOldFont = pDC->SelectObject(&m_cCursorDescFont);
		CSize cursorDescSize = pDC->GetTextExtent(_T("0"));
		pDC->SelectObject(pOldFont);
		if ( m_bShowCursor1Desc )
			rt.top += cursorDescSize.cy;
		if ( m_bShowCursor2Desc )
			rt.top += cursorDescSize.cy;
	}

	// 上下左右边距
	m_iXGramSpace = (rt.Width() - m_iLeftMargin - m_iRightMargin) / m_iXGramCount;
	m_iYGramSpace = (rt.Height() - m_iTopMargin - m_iBottomMargin) / m_iYGramCount;
	rt.left   = rt.left + m_iLeftMargin;
	rt.top    = rt.top  + m_iTopMargin;
	rt.right  = rt.left + m_iXGramSpace;
	rt.bottom = rt.top  + m_iYGramSpace;

	// X-Y轴标尺信息
	CString str(_T(""));
	for ( int i=0; i<m_iYInterValCharNum; i++ )
		str.Append(_T("0"));
	CFont *pOldFont = pDC->SelectObject(&m_cScaleValFont);
	CSize scaleValMaxSz = pDC->GetTextExtent(str);
	pDC->SelectObject(pOldFont);
	if ( m_bShowYScaleVal )
	{
		m_rtWaveArea.left = rt.left + scaleValMaxSz.cx;
		m_rtWaveArea.top  = rt.top  + scaleValMaxSz.cy/2;
		if ( !m_bShowXScaleVal && !m_bShowCursor1Scale && !m_bShowCursor2Scale )
		{
			m_rtWaveArea.right  = rt.right;
			m_rtWaveArea.bottom = rt.bottom - scaleValMaxSz.cy/2;
		}
	}
	if ( m_bShowXScaleVal || m_bShowCursor1Scale || m_bShowCursor2Scale )
	{
		m_rtWaveArea.right  = rt.right/* - scaleValMaxSz.cx/2*/;
		m_rtWaveArea.bottom = rt.bottom - scaleValMaxSz.cy;
		if ( !m_bShowYScaleVal )
		{
			m_rtWaveArea.left = rt.left + scaleValMaxSz.cx/2;
			m_rtWaveArea.top  = rt.top;
		}
	}
	if ( !m_bShowYScaleVal && !m_bShowXScaleVal && !m_bShowCursor1Scale && !m_bShowCursor2Scale )
	{
		m_rtWaveArea.left = rt.left;
		m_rtWaveArea.top  = rt.top;
		m_rtWaveArea.right  = rt.right;
		m_rtWaveArea.bottom = rt.bottom;
	}
}

///
/// @brief
///     绘制波形显示区域的内部网格
///
/// @param[in]
///     pDC      - 设备环境DC的指针
/// @param[in]
///     waveArea - 波形显示区域的矩形域
///
/// @return
///     
///
void CWaveFormGramCtrl::DrawWaveAreaGrid(CDC *pDC, const CRect &waveArea)
{
	if ( m_bShowWaveAreaHorGrid == FALSE && m_bShowWaveAreaVerGrid == FALSE )
		return;

	// 计算单个网格的水平宽度及垂直高度
	float horGridWidth = (float)waveArea.Width() / (float)m_iXHalfGridCount / 2.0f;
	float verGridWidth = (float)waveArea.Height() / (float)m_iYHalfGridCount / 2.0f;

	CPen  pen;
	pen.CreatePen(WFG_WAVE_AREA_GRID_STYLE,WFG_WAVE_AREA_GRID_WIDTH,m_clWaveAreaGridColor);
	CPen *pOldPen = pDC->SelectObject(&pen);

	// 绘制竖直的网格线
	for (int i = 1; i < m_iXHalfGridCount*2 && m_bShowWaveAreaVerGrid; ++i)
	{
		pDC->MoveTo(int(waveArea.left+i*horGridWidth),waveArea.top);
		pDC->LineTo(int(waveArea.left+i*horGridWidth),waveArea.bottom);
	}
	// 绘制水平的网格线，不包括中间线
	for (int i = 1; i < m_iYHalfGridCount*2 && m_bShowWaveAreaHorGrid; ++i)
	{
		if ( i != m_iYHalfGridCount )
		{
			pDC->MoveTo(waveArea.left,int(waveArea.top+verGridWidth*i));
			pDC->LineTo(waveArea.right,int(waveArea.top+verGridWidth*i));
		}
	}

	// 绘制水平的网格线的中间线
	if ( m_bShowWaveAreaHorCentreGrid )
	{
		pDC->MoveTo(waveArea.left,int(waveArea.top+verGridWidth*m_iYHalfGridCount));
		pDC->LineTo(waveArea.right,int(waveArea.top+verGridWidth*m_iYHalfGridCount));
	}
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
}

///
/// @brief
///     绘制波形显示区域的边框
///
/// @param[in]
///     pDC      - 设备环境DC的指针
/// @param[in]
///     waveArea - 波形显示区域的矩形域
///
/// @return
///     
///
void CWaveFormGramCtrl::DrawWaveAreaBoard(CDC *pDC, const CRect &waveArea)
{
	CPen pen;
	pen.CreatePen(WFG_WAVE_AREA_BORDER_STYLE,WFG_WAVE_AREA_BORDER_WIDTH,m_clScaleValColor);
	CPen *pOldPen = pDC->SelectObject(&pen);
// 	// 绘制水平中轴线
// 	float midLineHeight = (float)(m_iWaveAreaTop+(ctrlRt.Height()-m_iWaveAreaTop-m_iWaveAreaBottom)/2);
// 	pDC->MoveTo(m_iWaveAreaLeft,midLineHeight);
// 	pDC->LineTo(ctrlRt.Width() - m_iRightMargin,midLineHeight);
	// 左边框线
	pDC->MoveTo(waveArea.left,waveArea.top);
	pDC->LineTo(waveArea.left,waveArea.bottom);
	// 右边框线
	pDC->MoveTo(waveArea.right,waveArea.top);
	pDC->LineTo(waveArea.right,waveArea.bottom);
	// 上边框线
	pDC->MoveTo(waveArea.left,waveArea.top);
	pDC->LineTo(waveArea.right,waveArea.top);
	// 下边框线
	pDC->MoveTo(waveArea.left,waveArea.bottom);
	pDC->LineTo(waveArea.right,waveArea.bottom);
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
}

///
/// @brief
///     绘制X-Y标尺的刻度
///
/// @param[in]
///     pDC      - 设备环境DC的指针
/// @param[in]
///     idxSeq   - 波形图的顺序号（从左往右，再从上到下），从0开始
/// @param[in]
///     waveArea - 波形显示区域的矩形域
///
/// @return
///     
///
void CWaveFormGramCtrl::DrawScale(CDC *pDC, const size_t idxSeq, const CRect &waveArea)
{
	// 计算X-Y刻度值的间隔
	// 计算单个网格的宽度及高度
	float xSpanWidth = (float)waveArea.Width() / (float)m_iXHalfGridCount / 2.0f;
	float ySpanWidth = (float)waveArea.Height() / (float)m_iYHalfGridCount / 2.0f;

	// 设置X-Y标尺的分度值
	float xInterval;
	float yInterval;
	CString yUnit;
	if ( idxSeq >= m_fXScaleInterval.size() )
	{
		xInterval = m_fXScaleInterval.at(0);
		yInterval = m_fYScaleInterval.at(0);
		yUnit     = m_strYUnit.at(0);
	}
	else
	{
		xInterval = m_fXScaleInterval.at(idxSeq);
		yInterval = m_fYScaleInterval.at(idxSeq);
		yUnit     = m_strYUnit.at(idxSeq);
	}

	CFont *pOldFont = pDC->SelectObject(&m_cScaleValFont);
	COLORREF oldClr = pDC->SetTextColor(m_clScaleValColor);
	int oldMode = pDC->SetBkMode(TRANSPARENT);

	// 绘制X刻度
	if ( m_bShowXScaleVal )
	{
		for (int i=0; i<=m_iXHalfGridCount*2; ++i)
		{
			float val = m_fX0ScaleVal[idxSeq]+i*xInterval;
			CString str;
			str.Format(L"%0.1f", val);
			CSize scaleValSz = pDC->GetTextExtent(str);
			int left;
			if ( i == 0 )
				left = waveArea.left;
			else if ( i == m_iXHalfGridCount*2 )
				left = waveArea.right-scaleValSz.cx;
			else
				left = int(waveArea.left+i*xSpanWidth)-scaleValSz.cx/2;
			CRect rect(left,waveArea.bottom,left+scaleValSz.cx,waveArea.bottom+scaleValSz.cy);
			pDC->DrawText(str, rect, DT_RIGHT);
		}
	}
	// 绘制Y刻度
	if ( m_bShowYScaleVal )
	{
		for (int i=0; i<=m_iYHalfGridCount*2; ++i)
		{
			float val = (m_iYHalfGridCount-i)*yInterval;
			CString str;
			str.Format(L"%0.1f%s",val,yUnit);
			CSize scaleValSz = pDC->GetTextExtent(str);
			int top = int(waveArea.top+ySpanWidth*i)-scaleValSz.cy/2;
			CRect rect(waveArea.left-4-scaleValSz.cx,top,waveArea.left-4,top+scaleValSz.cy);
			pDC->DrawText(str, rect, DT_RIGHT);
		}
	}

#if !SU_FEI
	pDC->FillSolidRect(waveArea.left + 1, waveArea.top + 1, waveArea.Width(), waveArea.Height(), RGB(0, 0, 0));
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
///     pDC      - 设备环境DC的指针
/// @param[in]
///     idxSeq   - 波形图的顺序号（从左往右，再从上到下），从0开始
/// @param[in]
///     waveArea - 波形显示区域的矩形域
///
/// @return
///     
///
void CWaveFormGramCtrl::DrawTitle(CDC *pDC, const size_t idxSeq, const CRect &waveArea)
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
		pDC->ExtTextOutW(waveArea.right - sz.cx - 2,waveArea.top + 2,
			ETO_OPAQUE,NULL,str,str.GetLength(),NULL);
		pDC->SetTextColor(oldClr);
		pDC->SetBkMode(oldMode);
		pDC->SelectObject(pOldFont);
	}
}

///
/// @brief
///     绘制游标1
///
/// @param[in]
///     pDC        - 设备环境DC的指针
/// @param[in]
///     idxSeq     - 柱状图的顺序号（按从左往右，再从上到下的顺序），从0开始
/// @param[in]
///     waveArea   - 柱条显示区域的矩形域
///
/// @return
///     
///
void CWaveFormGramCtrl::DrawCursor1(CDC *pDC, const size_t idxSeq, const CRect &waveArea)
{
	if ( !m_bShowCursor1 )
		return;

	// 设置游标的颜色
	COLORREF cursorClr = m_clCursor1Color;

	// 计算游标的竖线的起点
	POINT pt;
	pt.x = waveArea.left;
	pt.y = waveArea.bottom-1;

	// 绘制游标竖线
	CPen pen;
	pen.CreatePen(WFG_CURSOR1_LINE_STYLE,1,cursorClr);
	CPen* pOldPen = pDC->SelectObject(&pen);
	pDC->MoveTo(pt);
	pDC->LineTo(pt.x,int(waveArea.top-WFG_CURSOR1_RECT_HEIGHT));

	// 绘制游标竖线两端的矩形
	CBrush brush(cursorClr);
	CBrush *pOldBrush = pDC->SelectObject(&brush);
	CRect rt1(0,0,WFG_CURSOR1_RECT_WIDTH,WFG_CURSOR1_RECT_HEIGHT*2);
	rt1.MoveToXY(pt.x-2-WFG_CURSOR1_RECT_WIDTH,waveArea.top);
	pDC->Rectangle(rt1);
	CRect rt2(0,0,WFG_CURSOR1_RECT_WIDTH,WFG_CURSOR1_RECT_HEIGHT*2);
	rt2.MoveToXY(pt.x+2,waveArea.top);
	pDC->Rectangle(rt2);
	pDC->SelectObject(pOldBrush);
	brush.DeleteObject();
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();

	// 绘制游标的标尺
	if ( m_bShowCursor1Scale )
	{
		CFont *pOldFont = pDC->SelectObject(&m_cScaleValFont);
		COLORREF oldClr   = pDC->SetTextColor(cursorClr);
		int oldMode  = pDC->SetBkMode(TRANSPARENT);
		CSize sz = pDC->GetTextExtent(m_strCursor1Scale);
		pDC->ExtTextOutW(pt.x-sz.cx/2,waveArea.bottom,
			ETO_OPAQUE,NULL,m_strCursor1Scale,m_strCursor1Scale.GetLength(),NULL);
		pDC->SetTextColor(oldClr);
		pDC->SetBkMode(oldMode);
		pDC->SelectObject(pOldFont);
	}
}

///
/// @brief
///     绘制游标2
///
/// @param[in]
///     pDC        - 设备环境DC的指针
/// @param[in]
///     idxSeq     - 柱状图的顺序号（按从左往右，再从上到下的顺序），从0开始
/// @param[in]
///     waveArea   - 柱条显示区域的矩形域
///
/// @return
///     
///
void CWaveFormGramCtrl::DrawCursor2(CDC *pDC, const size_t idxSeq, const CRect &waveArea)
{
	if ( !m_bShowCursor2 )
		return;

	// 设置游标的颜色
	COLORREF cursorClr = m_clCursor2Color;

	// 计算X标尺方向的最大长度
	float maxXValue   = (float)m_iXHalfGridCount * m_szXOneGridPointCount.at(idxSeq) * 2.0f;
	// 计算X标尺方向的每个点的步长
	float nStepWidth  = (float)waveArea.Width() / (maxXValue);

	// 计算游标的竖线的起点
	POINT pt;
	pt.x = int(waveArea.left+(m_szCursor2Pos.at(idxSeq)-m_szCursor1Pos.at(idxSeq))*nStepWidth);
	pt.y = waveArea.bottom-1;

	// 绘制游标竖线
	CPen pen;
	pen.CreatePen(WFG_CURSOR2_LINE_STYLE,1,cursorClr);
	CPen* pOldPen = pDC->SelectObject(&pen);
	pDC->MoveTo(pt);
	pDC->LineTo(pt.x,int(waveArea.top-WFG_CURSOR2_TRIG_WIDTH));
	POINT pos[3];
	// 绘制游标竖线两端的三角形
	CBrush brush(cursorClr);
	CBrush *pOldBrush = pDC->SelectObject(&brush);
	pos[0].y = int(waveArea.top);
	pos[1].y = int(waveArea.top+1*WFG_CURSOR2_TRIG_WIDTH);
	pos[2].y = int(waveArea.top+2*WFG_CURSOR2_TRIG_WIDTH);
	pos[0].x = pt.x-WFG_CURSOR2_TRIG_WIDTH;
	pos[1].x = pt.x-2*WFG_CURSOR2_TRIG_WIDTH;
	pos[2].x = pt.x-WFG_CURSOR2_TRIG_WIDTH;
	pDC->Polygon(pos,3);
	pos[0].x = pt.x+WFG_CURSOR2_TRIG_WIDTH;
	pos[1].x = pt.x+2*WFG_CURSOR2_TRIG_WIDTH;
	pos[2].x = pt.x+WFG_CURSOR2_TRIG_WIDTH;
	pDC->Polygon(pos,3);
	pDC->SelectObject(pOldBrush);
	brush.DeleteObject();
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();

	// 绘制游标的标尺
	if ( m_bShowCursor1Scale )
	{
		CFont *pOldFont = pDC->SelectObject(&m_cScaleValFont);
		COLORREF oldClr   = pDC->SetTextColor(cursorClr);
		int oldMode  = pDC->SetBkMode(TRANSPARENT);
		CSize sz = pDC->GetTextExtent(m_strCursor2Scale);
		pDC->ExtTextOutW(pt.x-sz.cx/2,waveArea.bottom,
			ETO_OPAQUE,NULL,m_strCursor2Scale,m_strCursor2Scale.GetLength(),NULL);
		pDC->SetTextColor(oldClr);
		pDC->SetBkMode(oldMode);
		pDC->SelectObject(pOldFont);
	}
}
