/// @file
///     WaveFormGramCtrl.cpp
///
/// @brief
///     ����ͼ�ؼ�
///
/// @note
///     �ò���ͼ�Ĳ�����ʾΪ�����ͣ����ڲ���ǰ���л��ƵĲ��ε�����ʾ
///     X��߿̶�Ϊ��ֵ����0��ʼ
///     Y����м�̶�Ϊ0
///     �ؼ�����λ�ڲ�����ʾ��������Ͻ�
///     ����������Ϣλ�ڲ�����ʾ��������Ͻ�
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
///    |2013.05.03     |1.0.503    |�޸Ĳ��ε�������Ϣλ�ã������ϽǸ�Ϊ���ϽǱ����·�����Ĭ��Ϊ2�У��á�\r\n���ֿ�|
///    |2013.05.03     |1.0.503    |���ӶԲ���ͼX�����ÿ��������������ã�X��ߵķֶ�ֵ���䲻Ӱ��|
///    |2013.05.06     |1.0.506    |������m_fYScaleIntervalΪ0ʱ������ƫ����ʾ����Ĵ���          |
///    |2013.05.06     |1.0.506    |�޸Ĳ���ͼ������ɫ���ýӿڣ����Ϊһ��ͼ��Ӧһ����ɫ��        |
///    |2013.06.21     |1.0.621    |����������ֵΪNANʱ�Ĵ�����ʾ��BUG                            |
///    |2013.07.09     |1.0.709    |��������ʾ�ӿڷ���Ϊˮƽ/��ֱ������ʾ�ӿ�                     |
///    |2013.07.09     |1.0.709    |��Ӳ���������Ϣ����ɫ���ýӿ�                                |
///    |2013.07.09     |1.0.709    |�������ò���ͼ����ʱδ���²���������Ϣ��ɫ���Bug             |
///    |2013.07.09     |1.0.709    |��Ӷ�ˮƽ������м��ߵĿɼ��Ե�����                          |
///    |2013.08.07     |1.0.807    |�޸��������ýӿ��ж���ε���Ч���ж�                          |
///    |2013.11.07     |1.0.1107   |����α겿��������ʽ���úͻ��ƽӿ�                            |
///    |2013.11.07     |1.0.1107   |���������ɻ��ƵĲ��εĵ������������ͼ��ǰ��ʾ�Ĳ��εĲ����������Ľӿ�|
///    |2013.12.03     |1.0.1203   |�Ż����λ��ƽӿڵ�ʵ�֣������ڴ��������ͷŲ���              |
///    |2013.12.03     |1.0.1203   |��Ӷ�X��ߵ���ʼ��Ŀ̶�ֵ������                             |
///    |2013.12.06     |1.0.1206   |�޸Ĵ����Ʋ������ݵĻ�����Ϊ���飬��������ò������ݺͻ��Ʋ��η����Ľӿ�|
///
#include "stdafx.h"
#include <float.h>
#include "WaveFormGramCtrl.h"

///
/// @brief
///     �ؼ��ı�����ɫ
///
#define WFG_CTRL_BKGND_COLOR    RGB(0,0,0)            ///< ��ɫ

///
/// @brief
///     �����Ĭ�����ã�����������ʽ�Լ����⡢X-Y��߿̶ȡ�����������Ϣ���α�������Ϣ�������С����ɫ
///
#define WFG_FONT_STYLE             _T("΢���ź�")
#define WFG_TITLE_FONT_SIZE        20

/**********************��ɫ�޸�************************************************************/
#define WFG_TITLE_FONT_COLOR       RGB(255,255,255)      ///< ��ɫ
#define WFG_SCALE_FONT_SIZE        15
#define WFG_SCALE_FONT_COLOR       RGB(170,170,170)      ///< ů��ɫ
#define WFG_WAVEDESC_FONT_SIZE     15
#define WFG_CURSORDESC_FONT_SIZE   10
#define WFG_CURSORDESC1_FONT_COLOR RGB(255,140,0)        ///< ���ɫ
#define WFG_CURSORDESC2_FONT_COLOR RGB(220,20,60)        ///< �Ⱥ�
///
/// @brief
///     ������ʾ�����Ĭ�����ã��߿���ڲ���������������͡���Ⱥ���ɫ
///
#define WFG_WAVE_AREA_BORDER_STYLE PS_SOLID
#define WFG_WAVE_AREA_BORDER_WIDTH 1
#define WFG_WAVE_AREA_GRID_STYLE   PS_DASH
#define WFG_WAVE_AREA_GRID_WIDTH   0
#define WFG_WAVE_AREA_GRID_COLOR   RGB(128,128,105)   ///< ���ɫ

///
/// @brief
///     ����������Ĭ�����ã��������������͡���Ⱥ�ǰ6����Ĭ����ɫ(�졢�ơ�ɭ���̡��ɺ졢�ȡ�����)
///
#define WFG_WAVE_LINE_STYLE        PS_SOLID
#define WFG_WAVE_LINE_WIDTH        1
static const COLORREF k_WaveColor[6] = {RGB(255,0,0),  RGB(255,255,0),RGB(34,139,34),
                                        RGB(220,20,60),RGB(255,165,0),RGB(124,252,0)};

///
/// @brief
///     �α��Ĭ�����ã���ɫ��������ʽ��������ȡ��α�1�����Ҿ��κ��α�2�����������εĿ��
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
///     ���캯��
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
///     ��������
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
///     ����ĳһ����ͼ��ĳһ���ε���ɫ����������Χ�����Ĳ���ΪĬ����ɫ
///
/// @param[in]
///     gramIdx - ����ͼ��˳��ţ���0��ʼ�����������ң��ٴ��ϵ��µ�˳��
/// @param[in]
///     waveIdx - ���ε�˳��ţ���0��ʼ
/// @param[in]
///     clr     - ��ɫֵ
///
/// @return
///
///
void CWaveFormGramCtrl::SetWaveColor(const size_t gramIdx, const size_t waveIdx, const COLORREF clr)
{
	// �����µĲ�����ɫ��
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

	// ���ò�����ɫ��
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
///     ����ĳһ����ͼ��ĳһ���ε�������Ϣ����ɫ����������Χ�����Ĳ���ΪĬ����ɫ
///
/// @param[in]
///     gramIdx - ����ͼ��˳��ţ���0��ʼ�����������ң��ٴ��ϵ��µ�˳��
/// @param[in]
///     waveIdx - ���ε�˳��ţ���0��ʼ
/// @param[in]
///     clr     - ��ɫֵ
///
/// @return
///
///
void CWaveFormGramCtrl::SetWaveDescColor(const size_t gramIdx, const size_t waveIdx, const COLORREF clr)
{
	// �����µĲ�����ɫ��
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

	// ���ò�����ɫ��
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
///     ���ÿؼ��ı��⣨λ�ڿؼ�������ʾ��������Ͻǣ�
///
/// @param[in]
///     idxSeq - ����ͼ��˳��ţ����������ң��ٴ��ϵ��µ�˳�򣩣���0��ʼ
/// @param[in]
///     str    - �ؼ��ı���
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
///     ���ÿؼ���X-Y��ߵķֶ�ֵ
///
/// @param[in]
///     idxSeq    - ����ͼ��˳��ţ����������ң��ٴ��ϵ��µ�˳�򣩣���0��ʼ
/// @param[in]
///     xInterval - X��ߵķֶ�ֵ����ÿһС��ĵ�����
/// @param[in]
///     yInterval - Y��ߵķֶ�ֵ����ÿһС���Ӧ������ֵ��
/// @param[in]
///     yUnit     - Y��ߵĵ�λ
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
			// ��������ΪĬ������
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
///     ���ÿؼ���X��ߵ���ʼ��X0�Ŀ̶�ֵ
///
/// @param[in]
///     idxSeq    - ����ͼ��˳��ţ����������ң��ٴ��ϵ��µ�˳�򣩣���0��ʼ
/// @param[in]
///     x0Val     - ��ʼ��X0�Ŀ̶�ֵ
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
			// ��������ΪĬ������
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
///     ���ÿؼ��ĸ�����ͼ��X��߷����ÿһ����ĵ���
///
/// @param[in]
///     idxSeq    - ����ͼ��˳��ţ����������ң��ٴ��ϵ��µ�˳�򣩣���0��ʼ
/// @param[in]
///     xPtCount  - ÿһС��ĵ���
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
			// ��������ΪĬ��ֵ
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
///     ���ÿؼ��Ĳ���ͼ�ĸ���
///
/// @param[in]
///     xGramCount - X��߷����ϵĲ���ͼ����
/// @param[in]
///     yGramCount - Y��߷����ϵĲ���ͼ����
///
/// @return
///
///
void CWaveFormGramCtrl::SetGramCount(const int xGramCount, const int yGramCount)
{
	// ���ô����Ʋ������ݵĻ�����
	if ( m_iXGramCount*m_iYGramCount != xGramCount*yGramCount )
	{
		// �ͷŴ����Ʋ������ݵĻ�����
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

		// ���������Ʋ������ݵĻ�����
		m_pDrawWaveDataBuf = new POINT*[xGramCount*yGramCount];
		for ( int i = 0; i < xGramCount*yGramCount; i++ )
		{
			m_pDrawWaveDataBuf[i] = new POINT[m_szDrawWaveDataBufSize];
		}
	}
	// ���³�ʼ������
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

	// ���ò��μ���������Ϣ����ɫ����Ϣ
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
///     ���ô����Ʋ������ݵĻ������Ĵ�С
///
/// @param[in]
///     szSize - �����Ʋ������ݵĻ������Ĵ�С
///
/// @return
///
///
void CWaveFormGramCtrl::SetDrawWaveDataBufSize(size_t szSize)
{
	// �ͷŴ����Ʋ������ݵĻ�����
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

	// ���������Ʋ������ݵĻ�����
	m_pDrawWaveDataBuf = new POINT*[m_iXGramCount*m_iYGramCount];
	for ( int i = 0; i < m_iXGramCount*m_iYGramCount; i++ )
	{
		m_pDrawWaveDataBuf[i] = new POINT[szSize];
	}
	m_szDrawWaveDataBufSize = szSize;
}

///
/// @brief
///     ���ÿؼ����������
///
/// @param[in]
///     pFont - ָ�������ָ��
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
///     ���ÿؼ���߿̶ȵ�����
///
/// @param[in]
///     pFont - ָ�������ָ��
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
///     ���ÿؼ�����������Ϣ������
///
/// @param[in]
///     pFont - ָ�������ָ��
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
///     ���ÿؼ��α�������Ϣ������
///
/// @param[in]
///     pFont - ָ�������ָ��
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
///     ���ò���ͼ��ǰ��ʾ�Ĳ��εĲ�������
///
/// @param[in]
///     waveDrawCount - ��ǰ��ʾ�Ĳ��εĲ�������
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
///     ��ʼ���ؼ�
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
///     ����������ʾ�����ȫ��������Ϣ
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
///     ���ô����Ʋ��εĲ��������ݣ���DrawWaveFromBuf()���ʹ��
///
/// @param[in]
///     idxSeq    - ����ͼ��˳��ţ��������ң��ٴ��ϵ��£�����0��ʼ
/// @param[in]
///     dataBuf   - ���沨�����ݵĻ���
/// @param[in]
///     bufSize   - �����С
///
/// @return
///     
///
void CWaveFormGramCtrl::SetWaveData(const size_t idxSeq, const float *dataBuf, const size_t bufSize)
{
	if (NULL == dataBuf || 1 >= bufSize || idxSeq >= (size_t)m_iXGramCount*m_iYGramCount )
		return ;

	// ��ȡ�ض�����ͼ�и����ε�������
	size_t maxSize = m_szWaveDataMaxSize[idxSeq];
	m_szWaveDataMaxSize[idxSeq] = maxSize > bufSize ? maxSize : bufSize;

	// ���㵱ǰҪ���ƵĲ�����ʾ����ľ�����
	int xPt = (int)idxSeq%m_iXGramCount;
	int yPt = (int)idxSeq/m_iXGramCount;
	CRect waveArea(m_rtWaveArea.left   + m_iXGramSpace * xPt,
		m_rtWaveArea.top    + m_iYGramSpace * yPt,
		m_rtWaveArea.right  + m_iXGramSpace * xPt,
		m_rtWaveArea.bottom + m_iYGramSpace * yPt);

	// ����Y��߷�������߶�,X-Y��߷����ÿ����Ĳ���
	float maxXValue   = (float)m_iXHalfGridCount * m_szXOneGridPointCount.at(idxSeq) * 2.0f;
	float maxYValue   = (float)m_iYHalfGridCount * m_fYScaleInterval.at(idxSeq);
	float nStepWidth  = (float)waveArea.Width() / (maxXValue);
	float nStepHeight = (float)waveArea.Height() / (2*maxYValue);
	if ( maxYValue == 0 )
		nStepHeight = 0;
	// ������Yֵ��Y��߷����ϵĲο�ֵ
	float yRefPT = (float)waveArea.top + (float)waveArea.Height()/2.0f;

	// ����Ҫ���ƵĲ��ε�ʵ�ʵ��������β��������ʼ��ַ
	size_t drawCount = m_szWaveDrawCount;
	if ( drawCount == SIZE_MAX )
		drawCount = bufSize;
	if ( drawCount > (size_t)maxXValue+1 )
		drawCount = (size_t)maxXValue+1;
	if ( m_szCursor1Pos[idxSeq]+drawCount > bufSize )
		drawCount = bufSize - m_szCursor1Pos[idxSeq];
	float *pWaveData = const_cast<float*>(&dataBuf[m_szCursor1Pos[idxSeq]]);

	// ���ݷֱ��ʹ��˲����㣬ÿ�����ص�(x)������2�������㣬����2����ȥ�����Сֵ
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

		// Yֵ��Ч���ж�
		if ( _finite(pWaveData[i]) == FALSE )
			tmp = 0;
		else if(pWaveData[i] > maxYValue)
			tmp = maxYValue;
		else if (pWaveData[i] < -maxYValue)
			tmp = -maxYValue;
		else 
			tmp = pWaveData[i];

		pt.y = int(yRefPT - tmp * nStepHeight/* + 0.5f*/);    // 4��5��
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
///     ���ݻ���Ĳ������ݻ��Ʋ��Σ���SetWaveData()���ʹ��
///
/// @param[in]
///     idxSeq    - ����ͼ��˳��ţ��������ң��ٴ��ϵ��£�����0��ʼ
/// @param[in]
///     waveDesc  - ���ε�������Ϣ��Ĭ��Ϊ2�У��á�\r\n���ֿ�
///
/// @return
///     
///
void CWaveFormGramCtrl::DrawWaveFromBuf(const size_t idxSeq, const CString &waveDesc)
{
	if ( idxSeq >= (size_t)m_iXGramCount*m_iYGramCount )
		return ;

	// ���㵱ǰҪ���ƵĲ�����ʾ����ľ�����
	int xPt = (int)idxSeq%m_iXGramCount;
	int yPt = (int)idxSeq/m_iXGramCount;
	CRect waveArea(m_rtWaveArea.left   + m_iXGramSpace * xPt,
		m_rtWaveArea.top    + m_iYGramSpace * yPt,
		m_rtWaveArea.right  + m_iXGramSpace * xPt,
		m_rtWaveArea.bottom + m_iYGramSpace * yPt);

	// ���ò���������ɫ
	size_t curWaveIdx = m_szCurWaveCount.at(idxSeq);
	COLORREF curWaveColor;
	if ( curWaveIdx >= m_clWaveColor.at(idxSeq)->size() )
		curWaveColor = m_clWaveColor.at(idxSeq)->at(0);
	else
		curWaveColor = m_clWaveColor.at(idxSeq)->at(curWaveIdx);

	// ���Ʋ�������
	CPen pen;
	pen.CreatePen(WFG_WAVE_LINE_STYLE,WFG_WAVE_LINE_WIDTH,curWaveColor);
	CPen *pOldPen = m_cForgndDC.SelectObject(&pen);
	m_cForgndDC.Polyline(&m_pDrawWaveDataBuf[idxSeq][0],m_vDrawWaveDataNum[idxSeq]);
	m_cForgndDC.SelectObject(pOldPen);
	pen.DeleteObject();

	// ���Ʋ���������Ϣ��λ�ڲ�����ʾ��������Ͻ�
	if ( m_bShowWaveDesc )
	{
		// ���ò���������ɫ
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

	// �����α�2
	DrawCursor2(&m_cForgndDC,idxSeq,waveArea);

	m_szCurWaveCount.at(idxSeq) = curWaveIdx + 1;
}

///
/// @brief
///     ���Ʋ��Σ�����ʽ����ǰ����ƵĲ���һ����ʾ��
///
/// @param[in]
///     idxSeq    - ����ͼ��˳��ţ��������ң��ٴ��ϵ��£�����0��ʼ
/// @param[in]
///     dataBuf   - ���沨�����ݵĻ���
/// @param[in]
///     bufSize   - �����С
/// @param[in]
///     waveDesc  - ���ε�������Ϣ��Ĭ��Ϊ2�У��á�\r\n���ֿ�
///
/// @return
///     
///
void CWaveFormGramCtrl::DrawWave(const size_t idxSeq, const float *dataBuf, const size_t bufSize, const CString &waveDesc)
{
	if (NULL == dataBuf || 1 >= bufSize || idxSeq >= (size_t)m_iXGramCount*m_iYGramCount )
		return ;

	// ��ȡ�ض�����ͼ�и����ε�������
	size_t maxSize = m_szWaveDataMaxSize[idxSeq];
	m_szWaveDataMaxSize[idxSeq] = maxSize > bufSize ? maxSize : bufSize;

	// ���㵱ǰҪ���ƵĲ�����ʾ����ľ�����
	int xPt = (int)idxSeq%m_iXGramCount;
	int yPt = (int)idxSeq/m_iXGramCount;
	CRect waveArea(m_rtWaveArea.left   + m_iXGramSpace * xPt,
	               m_rtWaveArea.top    + m_iYGramSpace * yPt,
	               m_rtWaveArea.right  + m_iXGramSpace * xPt,
	               m_rtWaveArea.bottom + m_iYGramSpace * yPt);

	// ����Y��߷�������߶ȣ�X-Y��߷����ÿ����Ĳ���
	float maxXValue   = (float)m_iXHalfGridCount * m_szXOneGridPointCount.at(idxSeq) * 2.0f;
	float maxYValue   = (float)m_iYHalfGridCount * m_fYScaleInterval.at(idxSeq);
	float nStepWidth  = (float)waveArea.Width() / (maxXValue);
//	float nStepWidth  = (float)waveArea.Width() / ((float)bufSize-1) / 2.0f;
	float nStepHeight = (float)waveArea.Height() / (2*maxYValue);
	if ( maxYValue == 0 )
		nStepHeight = 0;

	// ������Yֵ��Y��߷����ϵĲο�ֵ
	float yRefPT = (float)waveArea.top + (float)waveArea.Height()/2.0f;

	// ����Ҫ���ƵĲ��ε�ʵ�ʵ��������β��������ʼ��ַ
	size_t drawCount = m_szWaveDrawCount;
	if ( drawCount == SIZE_MAX )
		drawCount = bufSize;
	if ( drawCount > (size_t)maxXValue+1 )
		drawCount = (size_t)maxXValue+1;
	if ( m_szCursor1Pos[idxSeq]+drawCount > bufSize )
		drawCount = bufSize - m_szCursor1Pos[idxSeq];
	float *pWaveData = const_cast<float*>(&dataBuf[m_szCursor1Pos[idxSeq]]);

	// ���ò���������ɫ
	size_t curWaveIdx = m_szCurWaveCount.at(idxSeq);
	COLORREF curWaveColor;
	if ( curWaveIdx >= m_clWaveColor.at(idxSeq)->size() )
		curWaveColor = m_clWaveColor.at(idxSeq)->at(0);
	else
		curWaveColor = m_clWaveColor.at(idxSeq)->at(curWaveIdx);

	// ���Ʋ�������
	CPen pen;
	pen.CreatePen(WFG_WAVE_LINE_STYLE,WFG_WAVE_LINE_WIDTH,curWaveColor);
	CPen *pOldPen = m_cForgndDC.SelectObject(&pen);
	// �������ֻ��Ʒ�ʽ
	// ��һ�֣�ֱ�ӵ�������ƣ����ܽϴֲڣ�������dc�����϶�
	// �ڶ��֣����ݷֱ��ʹ��˲����㣬ÿ�����ص�(x)������2�������㣬����2����ȥ�����Сֵ
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

		// Yֵ��Ч���ж�
		if ( _finite(pWaveData[i]) == FALSE )
			tmp = 0;
 
		else if(pWaveData[i] > maxYValue)

			tmp = maxYValue;

		else if (pWaveData[i] < -maxYValue)

			tmp = -maxYValue; 
		else  
			tmp = pWaveData[i];

		pt.y = int(yRefPT - tmp * nStepHeight/* + 0.5f*/);    // 4��5��
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

#if !CHUN_HUA    //ͨ���������Ͷ��ñ����ػ�ķ���ȥ������
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

	// ���Ʋ���������Ϣ��λ�ڲ�����ʾ��������Ͻ�
	if ( m_bShowWaveDesc )
	{
		// ���ò���������ɫ
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

	// �����α�2
	DrawCursor2(&m_cForgndDC,idxSeq,waveArea);

	m_szCurWaveCount.at(idxSeq) = curWaveIdx + 1;
}

///
/// @brief
///     �����α��������Ϣ���Σ�����ʽ����ǰ����ƵĲ��ֵ�����ʾ��
///
/// @param[in]
///     cursor1Desc   - �α�1��������Ϣ
/// @param[in]
///     cursor2Desc   - �α�2��������Ϣ
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
///     ǿ��ˢ�£�����ǰ�ڴ��豸�����е�����ˢ�µ�Ŀ���ڴ��豸������DC����
///
/// @param[in]
///     pDC - ָ��Ŀ��DC��ָ�룻ΪNULLʱ����ΪĬ�ϵ�DC
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
///     ��ȡ�α�1���ڵĵ�����ڲ������ݻ��������λ��
///
int  CWaveFormGramCtrl::GetCursor1Pos(size_t idxSeq)
{
	return m_szCursor1Pos.at(idxSeq);
}


///
/// @brief
///     ��ȡ�α�2���ڵĵ�����ڲ������ݻ��������λ��
///
int  CWaveFormGramCtrl::GetCursor2Pos(size_t idxSeq)
{
	return m_szCursor2Pos.at(idxSeq);
}

///
/// @brief
///     �����α�
///
void CWaveFormGramCtrl::Cursor2MoveRight()
{
	if ( !m_bShowCursor2 )
		return;

	for ( size_t i = 0; i < m_szCursor2Pos.size(); i++ )
	{
		// �ж��Ƿ񵽴ﲨ�ν�β��
		if ( m_szCursor2Pos[i]+1 < m_szWaveDataMaxSize[i] )
			m_szCursor2Pos[i]++;

		// �жϲ����Ƿ�ת��ǰ1�ز���
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
///     �����α�
///
void CWaveFormGramCtrl::Cursor2MoveLeft()
{
	if ( !m_bShowCursor2 )
		return;

	for ( size_t i = 0; i < m_szCursor2Pos.size(); i++ )
	{
		// �ж��Ƿ񵽴ﲨ����ʼ��
		if ( m_szCursor2Pos[i] != 0 )
			m_szCursor2Pos[i]--;

		// �ж��Ƿ�ת��ǰ1�ز���
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
///     ����������Ϣ������
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
///     �ػ���Ϣ������
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
///     �ͷſؼ���Դ��Ϣ������
///
/// @return
///
void CWaveFormGramCtrl::OnDestory()
{
	CStatic::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������
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
void CWaveFormGramCtrl::CreateDCandBmp(const CRect &rt, CDC &dc, CBitmap &bmp, CBitmap **pPOldBmp)
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
void CWaveFormGramCtrl::DrawBkgnd()
{
	CRect rt;
	GetClientRect(rt);

	// ��������DC��λͼ
	CreateDCandBmp(rt,m_cBkgndDC,m_cBkgndBmp,&m_pOldBkgndBmp);

	// �������
	m_cBkgndDC.FillSolidRect(0,0,rt.Width(),rt.Height(),m_clBkgndColor);

	// ���µ�1��������ʾ����(0,0)�ľ�����
	UpdateWaveAreaSpace(&m_cBkgndDC);

	// �������ң����ϵ��»������в���ͼ�Ĳ�����ʾ����
	for ( int y = 0; y < m_iYGramCount; y++ )
	{
		for ( int x = 0; x < m_iXGramCount; x++ )
		{
			// ���㵱ǰҪ���ƵĲ�����ʾ����ľ�����
			CRect waveArea(m_rtWaveArea.left   + m_iXGramSpace * x,
				           m_rtWaveArea.top    + m_iYGramSpace * y,
						   m_rtWaveArea.right  + m_iXGramSpace * x,
						   m_rtWaveArea.bottom + m_iYGramSpace * y);

			
			// ���Ʋ�����ʾ����ı߿�
		//	DrawWaveAreaBoard(&m_cBkgndDC,waveArea);

			// ����X-Y��ߵĿ̶�
			DrawScale(&m_cBkgndDC,y*m_iXGramCount+x,waveArea);

			// ���Ʋ�����ʾ������ڲ�����
			DrawWaveAreaGrid(&m_cBkgndDC,waveArea);

			// �������Ͻǵı���
			DrawTitle(&m_cBkgndDC,y*m_iXGramCount+x,waveArea);

			// �����α�1
			DrawCursor1(&m_cBkgndDC,y*m_iXGramCount+x,waveArea);
		}
	}
}

///
/// @brief
///     ���ÿؼ�ǰ��
///
/// @return
///
void CWaveFormGramCtrl::ResetForgnd()
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
///     �����1��������ʾ����ľ�����
///
/// @param[in]
///     pDC - �豸����DC��ָ��
///
/// @return
///     
///
void CWaveFormGramCtrl::UpdateWaveAreaSpace(CDC *pDC)
{
	CRect rt;
	GetClientRect(rt);

	// �α�1��2������Ϣ
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

	// �������ұ߾�
	m_iXGramSpace = (rt.Width() - m_iLeftMargin - m_iRightMargin) / m_iXGramCount;
	m_iYGramSpace = (rt.Height() - m_iTopMargin - m_iBottomMargin) / m_iYGramCount;
	rt.left   = rt.left + m_iLeftMargin;
	rt.top    = rt.top  + m_iTopMargin;
	rt.right  = rt.left + m_iXGramSpace;
	rt.bottom = rt.top  + m_iYGramSpace;

	// X-Y������Ϣ
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
///     ���Ʋ�����ʾ������ڲ�����
///
/// @param[in]
///     pDC      - �豸����DC��ָ��
/// @param[in]
///     waveArea - ������ʾ����ľ�����
///
/// @return
///     
///
void CWaveFormGramCtrl::DrawWaveAreaGrid(CDC *pDC, const CRect &waveArea)
{
	if ( m_bShowWaveAreaHorGrid == FALSE && m_bShowWaveAreaVerGrid == FALSE )
		return;

	// ���㵥�������ˮƽ��ȼ���ֱ�߶�
	float horGridWidth = (float)waveArea.Width() / (float)m_iXHalfGridCount / 2.0f;
	float verGridWidth = (float)waveArea.Height() / (float)m_iYHalfGridCount / 2.0f;

	CPen  pen;
	pen.CreatePen(WFG_WAVE_AREA_GRID_STYLE,WFG_WAVE_AREA_GRID_WIDTH,m_clWaveAreaGridColor);
	CPen *pOldPen = pDC->SelectObject(&pen);

	// ������ֱ��������
	for (int i = 1; i < m_iXHalfGridCount*2 && m_bShowWaveAreaVerGrid; ++i)
	{
		pDC->MoveTo(int(waveArea.left+i*horGridWidth),waveArea.top);
		pDC->LineTo(int(waveArea.left+i*horGridWidth),waveArea.bottom);
	}
	// ����ˮƽ�������ߣ��������м���
	for (int i = 1; i < m_iYHalfGridCount*2 && m_bShowWaveAreaHorGrid; ++i)
	{
		if ( i != m_iYHalfGridCount )
		{
			pDC->MoveTo(waveArea.left,int(waveArea.top+verGridWidth*i));
			pDC->LineTo(waveArea.right,int(waveArea.top+verGridWidth*i));
		}
	}

	// ����ˮƽ�������ߵ��м���
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
///     ���Ʋ�����ʾ����ı߿�
///
/// @param[in]
///     pDC      - �豸����DC��ָ��
/// @param[in]
///     waveArea - ������ʾ����ľ�����
///
/// @return
///     
///
void CWaveFormGramCtrl::DrawWaveAreaBoard(CDC *pDC, const CRect &waveArea)
{
	CPen pen;
	pen.CreatePen(WFG_WAVE_AREA_BORDER_STYLE,WFG_WAVE_AREA_BORDER_WIDTH,m_clScaleValColor);
	CPen *pOldPen = pDC->SelectObject(&pen);
// 	// ����ˮƽ������
// 	float midLineHeight = (float)(m_iWaveAreaTop+(ctrlRt.Height()-m_iWaveAreaTop-m_iWaveAreaBottom)/2);
// 	pDC->MoveTo(m_iWaveAreaLeft,midLineHeight);
// 	pDC->LineTo(ctrlRt.Width() - m_iRightMargin,midLineHeight);
	// ��߿���
	pDC->MoveTo(waveArea.left,waveArea.top);
	pDC->LineTo(waveArea.left,waveArea.bottom);
	// �ұ߿���
	pDC->MoveTo(waveArea.right,waveArea.top);
	pDC->LineTo(waveArea.right,waveArea.bottom);
	// �ϱ߿���
	pDC->MoveTo(waveArea.left,waveArea.top);
	pDC->LineTo(waveArea.right,waveArea.top);
	// �±߿���
	pDC->MoveTo(waveArea.left,waveArea.bottom);
	pDC->LineTo(waveArea.right,waveArea.bottom);
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
}

///
/// @brief
///     ����X-Y��ߵĿ̶�
///
/// @param[in]
///     pDC      - �豸����DC��ָ��
/// @param[in]
///     idxSeq   - ����ͼ��˳��ţ��������ң��ٴ��ϵ��£�����0��ʼ
/// @param[in]
///     waveArea - ������ʾ����ľ�����
///
/// @return
///     
///
void CWaveFormGramCtrl::DrawScale(CDC *pDC, const size_t idxSeq, const CRect &waveArea)
{
	// ����X-Y�̶�ֵ�ļ��
	// ���㵥������Ŀ�ȼ��߶�
	float xSpanWidth = (float)waveArea.Width() / (float)m_iXHalfGridCount / 2.0f;
	float ySpanWidth = (float)waveArea.Height() / (float)m_iYHalfGridCount / 2.0f;

	// ����X-Y��ߵķֶ�ֵ
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

	// ����X�̶�
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
	// ����Y�̶�
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
///     ���ƿؼ��ı��⣨λ�ڲ�����ʾ��������Ͻǣ�
///
/// @param[in]
///     pDC      - �豸����DC��ָ��
/// @param[in]
///     idxSeq   - ����ͼ��˳��ţ��������ң��ٴ��ϵ��£�����0��ʼ
/// @param[in]
///     waveArea - ������ʾ����ľ�����
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
///     �����α�1
///
/// @param[in]
///     pDC        - �豸����DC��ָ��
/// @param[in]
///     idxSeq     - ��״ͼ��˳��ţ����������ң��ٴ��ϵ��µ�˳�򣩣���0��ʼ
/// @param[in]
///     waveArea   - ������ʾ����ľ�����
///
/// @return
///     
///
void CWaveFormGramCtrl::DrawCursor1(CDC *pDC, const size_t idxSeq, const CRect &waveArea)
{
	if ( !m_bShowCursor1 )
		return;

	// �����α����ɫ
	COLORREF cursorClr = m_clCursor1Color;

	// �����α�����ߵ����
	POINT pt;
	pt.x = waveArea.left;
	pt.y = waveArea.bottom-1;

	// �����α�����
	CPen pen;
	pen.CreatePen(WFG_CURSOR1_LINE_STYLE,1,cursorClr);
	CPen* pOldPen = pDC->SelectObject(&pen);
	pDC->MoveTo(pt);
	pDC->LineTo(pt.x,int(waveArea.top-WFG_CURSOR1_RECT_HEIGHT));

	// �����α��������˵ľ���
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

	// �����α�ı��
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
///     �����α�2
///
/// @param[in]
///     pDC        - �豸����DC��ָ��
/// @param[in]
///     idxSeq     - ��״ͼ��˳��ţ����������ң��ٴ��ϵ��µ�˳�򣩣���0��ʼ
/// @param[in]
///     waveArea   - ������ʾ����ľ�����
///
/// @return
///     
///
void CWaveFormGramCtrl::DrawCursor2(CDC *pDC, const size_t idxSeq, const CRect &waveArea)
{
	if ( !m_bShowCursor2 )
		return;

	// �����α����ɫ
	COLORREF cursorClr = m_clCursor2Color;

	// ����X��߷������󳤶�
	float maxXValue   = (float)m_iXHalfGridCount * m_szXOneGridPointCount.at(idxSeq) * 2.0f;
	// ����X��߷����ÿ����Ĳ���
	float nStepWidth  = (float)waveArea.Width() / (maxXValue);

	// �����α�����ߵ����
	POINT pt;
	pt.x = int(waveArea.left+(m_szCursor2Pos.at(idxSeq)-m_szCursor1Pos.at(idxSeq))*nStepWidth);
	pt.y = waveArea.bottom-1;

	// �����α�����
	CPen pen;
	pen.CreatePen(WFG_CURSOR2_LINE_STYLE,1,cursorClr);
	CPen* pOldPen = pDC->SelectObject(&pen);
	pDC->MoveTo(pt);
	pDC->LineTo(pt.x,int(waveArea.top-WFG_CURSOR2_TRIG_WIDTH));
	POINT pos[3];
	// �����α��������˵�������
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

	// �����α�ı��
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
