/**
 * \file
 *      StatusDisp.h
 *
 * \brief
 *      图片显示小控件头文件
 *
 *  \note
 *      图片控件可以设置多个状态，每个状态显示不同的图片信息
 *      当某个状态设置了多个图片时，将轮流显示每个图片
 *
 * \copyright
 *      Copyright(c) 2016 广州炫通电气科技有限公司
 *
 * \author
 *      chao      2013/6/27
 */

#pragma once

#include <vector>

// CStatusDisp

class CStatusDisp : public CStatic
{
	DECLARE_DYNAMIC(CStatusDisp)

public:
	CStatusDisp();
	virtual ~CStatusDisp();

protected:
	DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnPaint();


public:


    /**
     *      插入一个状态
     */
    void _InsertStatus(int nStatus, std::vector<HBITMAP>& vbitmap);

    /**
     *      设置定时器时间
     */
    void _SetTimeInterval(UINT interval)
    {
        m_nInterval = interval;
    }

    /**
     *      设置当前状态
     */
    void _SetStatus(int nStatus);

    /**
     *      获取当前状态
     */
    inline int _GetStatus()
    {
        return m_nStatus;
    }


private:

    typedef std::vector<HBITMAP>            CBValue;
    typedef std::map<int, CBValue*>         CMapStatus;
    

    int                 m_nStatus;          ///< 当前状态

    UINT                m_nInterval;        ///< 默认定时器间隔
    UINT                m_nTimerID;         ///< 定时器ID

    int                 m_nCount;           ///< 计数器

    CBValue*            m_pValue;           ///< 当前状态的图片容器
    CMapStatus          m_mapStatus;        ///< 状态图片映射

};


