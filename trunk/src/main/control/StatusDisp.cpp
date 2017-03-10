/**
 * \file
 *      StatusDisp.cpp
 *
 * \brief
 *      图片显示小控件实现文件
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/6/27
 */

// StatusDisp.cpp : 实现文件
//

#include "stdafx.h"
#include "StatusDisp.h"

#include <algorithm>

namespace StatusDisp
{
    void deleteElem(std::pair<int, std::vector<HBITMAP>* > val)
    {
        if(val.second)
        {
            delete val.second;
        }
    }
}



// CStatusDisp

IMPLEMENT_DYNAMIC(CStatusDisp, CStatic)

CStatusDisp::CStatusDisp()
: m_nInterval(300)
, m_nStatus(0)
, m_nTimerID(UINT_MAX)
, m_nCount(0)
, m_pValue(NULL)
{

}


CStatusDisp::~CStatusDisp()
{
    for_each(m_mapStatus.begin(), m_mapStatus.end(), StatusDisp::deleteElem);
}


BEGIN_MESSAGE_MAP(CStatusDisp, CStatic)
    ON_WM_TIMER()
    ON_WM_PAINT()
END_MESSAGE_MAP()


// CStatusDisp 消息处理程序

void CStatusDisp::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    Invalidate(TRUE);

    CStatic::OnTimer(nIDEvent);
}

void CStatusDisp::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO: 在此处添加消息处理程序代码
    // 不为绘图消息调用 CStatic::OnPaint()

    ASSERT(m_pValue);

    m_nCount = m_nCount % m_pValue->size();

    CRect rcClient;
    GetClientRect(rcClient);

    CDC dcMem;
    dcMem.CreateCompatibleDC(&dc);
    HGDIOBJ pre = dcMem.SelectObject(m_pValue->at(m_nCount++));
    dc.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &dcMem, 0, 0, SRCCOPY);
    dcMem.SelectObject(pre);
    dcMem.DeleteDC();
}


void CStatusDisp::_InsertStatus( int nStatus, std::vector<HBITMAP>& vbitmap )
{
    std::vector<HBITMAP>* pValue = NULL;
    CMapStatus::iterator it = m_mapStatus.find(nStatus);
    CMapStatus::iterator itend = m_mapStatus.end();

    if(it != itend)
    {
        delete it->second;
        m_mapStatus.erase(nStatus);
    }

    pValue = new std::vector<HBITMAP>;
    pValue->resize(vbitmap.size());
    copy(vbitmap.begin(), vbitmap.end(), pValue->begin());

    m_mapStatus[nStatus] = pValue;
}


void CStatusDisp::_SetStatus( int nStatus )
{
    CMapStatus::iterator it = m_mapStatus.find(nStatus);

    if(it != m_mapStatus.end())
    {
        if(it->second->size() > 1 && IsWindow(m_hWnd))
        {
            m_nTimerID = SetTimer(1, m_nInterval, NULL);
        }
        else
        {
            KillTimer(m_nTimerID);
            m_nTimerID = UINT_MAX;
        }

        m_nStatus   = nStatus;      // 设置状态
        m_nCount    = 0;            // 计数清零
        m_pValue    = it->second;   // 减少查询次数

        Invalidate(TRUE);
    }
}



