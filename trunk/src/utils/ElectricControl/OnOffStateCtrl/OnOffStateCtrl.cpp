/// @file
///     OnOffStateCtrl.cpp
///
/// @brief
///     开关DI状态控件
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
///    |2013.07.08     |1.0.1.708  |将开关状态的绘制接口分离为设置单个状态的样式及一次性绘制两部分|
///    |2013.07.08     |1.0.1.708  |将擦除全部开关DI状态的操作移至绘制接口中，减少外部操作        |
///    |2013.07.09     |1.0.1.709  |添加开关DI状态Bmp的显示区域大小设置，以及Bmp的自绘边框的可见性设置|
///
#include "stdafx.h"
#include "OnOffStateCtrl.h"

///
/// @brief
///     控件的背景颜色
///
#define OOS_CTRL_BKGND_COLOR      RGB(0,0,0)           ///< 黑色

#define OOS_BMP_BORDER_COLOR      RGB(128,128,135)
#define OOS_BMP_BORDER_STYLE      PS_SOLID
#define OOS_BMP_BORDER_WIDTH      1

#define OOS_FONT_STYLE            _T("微软雅黑")
#define OOS_ONOFF_DESC_FONT_COLOR RGB(128,128,135)
#define OOS_ONOFF_DESC_FONT_SIZE  25

#ifndef PI
#define PI 3.1415926
#endif

IMPLEMENT_DYNAMIC(COnOffStateCtrl, CStatic)

///
/// @brief
///     构造函数
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
///     析构函数
///
COnOffStateCtrl::~COnOffStateCtrl()
{

}

///
/// @brief
///     设置控件中开关DI状态的数量
///
void COnOffStateCtrl::SetOnOffDICount(const int xCount, const int yCount)
{
	m_iXOnOffDICount = xCount;
	m_iYOnOffDICount = yCount;

	// 重置开关状态信息
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
///     设置控件上开关DI状态的描述信息的字体
///
/// @param[in]
///     pFont - 指向字体的指针
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
///     设置开关DI状态的样式，若顺序号超过范围则多出的部分设为空
///
/// @param[in]
///     idxSeq - 样式的ID，从0开始
/// @param[in]
///     pBmp   - 指向开关DI状态的样式Bmp的指针
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
			// 超出部分默认为空
			m_vOnOffStyleBmp.push_back(NULL);
		}
	}
	else
	{
		// 更新样式
		m_vOnOffStyleBmp.at(idxSeq) = pBmp;
	}
}

///
/// @brief
///     初始化控件
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
///     擦除全部开关DI状态
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
///     根据选择的样式ID绘制某一开关DI状态：叠加式（与前面绘制的叠加显示）
///
/// @param[in]
///     idxSeq  - 开关DI状态的顺序号（从左往右，再从上到下），从0开始
/// @param[in]
///     styleID - 开关DI状态的样式ID
/// @param[in]
///     desc    - 开关DI状态的描述信息
///
/// @return
///     TRUE - 成功；FALSE - 失败
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
///     根据选择的样式ID绘制某一开关DI状态：叠加式（与前面绘制的叠加显示）
///
/// @param[in]
///     idxSeq  - 开关DI状态的顺序号（从左往右，再从上到下），从0开始
/// @param[in]
///     styleID - 开关DI状态的样式ID
/// @param[in]
///     desc    - 开关DI状态的描述信息
///
/// @return
///
///
void COnOffStateCtrl::DrawOnOffState()
{
	// 擦除前景
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
			// 计算当前要绘制的开关DI状态的矩形域
			CRect diArea = m_rtOnOffDIArea ;
			diArea.OffsetRect(int(m_fXOnOffDISpace*i),int(m_fYOnOffDISpace*j));

			// 绘制描述信息
			CString desc = m_vOnOffStateInfo.at(i+m_iXOnOffDICount*j).csDesc;
			CSize sz = m_cForgndDC.GetTextExtent(desc);
			int left = diArea.CenterPoint().x - sz.cx/2;
			int top  = diArea.bottom - sz.cy;
			m_cForgndDC.ExtTextOut(left,top,ETO_OPAQUE,NULL,desc,desc.GetLength(),NULL);

			// 计算开关DI状态贴图的矩形域
// 			int bmpAreaWidth = diArea.Width() < (diArea.Height()-sz.cy-2) ? diArea.Width() : (diArea.Height()-sz.cy-2);
// 			CRect bmpArea(diArea.CenterPoint().x - bmpAreaWidth/2,
// 				diArea.bottom - bmpAreaWidth - sz.cy - 2,
// 				diArea.CenterPoint().x + bmpAreaWidth/2,
// 				diArea.bottom - sz.cy - 2);
			CRect bmpArea( diArea.CenterPoint().x - m_rtOnOffShowSize.Width()/2,
				           diArea.bottom - m_rtOnOffShowSize.Height() - sz.cy - 2,
						   diArea.CenterPoint().x + m_rtOnOffShowSize.Width()/2,
						   diArea.bottom - sz.cy - 2 );

			// 绘制开关DI状态
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

			// 绘制开关DI状态的边框
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
///     强制刷新：将当前内存设备缓冲中的内容刷新到目标内存设备环境（DC）中
///
/// @param[in]
///     pDC - 指向目标DC的指针；为NULL时，则为默认的DC
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
///     重绘消息处理函数
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
///     释放控件资源消息处理函数
///
/// @return
///
void COnOffStateCtrl::OnDestroy()
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
BOOL COnOffStateCtrl::OnEraseBkgnd(CDC* pDC)
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
void COnOffStateCtrl::CreateDCandBmp(const CRect &rt, CDC &dc, CBitmap &bmp, CBitmap **pPOldBmp)
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
void COnOffStateCtrl::DrawBkgnd( )
{
	CRect rt;
	GetClientRect(rt);

	// 创建背景DC和位图
	CreateDCandBmp(rt,m_cBkgndDC,m_cBkgndBmp,&m_pOldBkgndBmp);

	// 背景填充
	m_cBkgndDC.FillSolidRect(0,0,rt.Width(),rt.Height(),m_clBkgndColor);

	UpdateOnOffBmpArea(&m_cBkgndDC);

}

///
/// @brief
///     重置控件前景
///
/// @return
///
void  COnOffStateCtrl::ResetForgnd()
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
///     计算开关DI状态绘图区域的矩形域
///
/// @param[in]
///     pDC - 设备环境DC的指针
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
