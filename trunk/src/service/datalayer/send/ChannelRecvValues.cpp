///
/// @brief
///     存放通道所有接收值的类
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.7.1
///
/// 修改历史：
///

#include "StdAfx.h"
#include "ChannelRecvValues.h"

#define MAX_HISTORY_RECV_RECORD         3                               ///< 最大历史接收记录数

///
/// @brief
///    构造函数
///
CChannelRecvValues::CChannelRecvValues(std::wstring strName)
: m_strName(strName)
, m_iChangeTimes(0)
{
    m_listHistory.clear();
}

///
/// @brief
///    析构函数
///
CChannelRecvValues::~CChannelRecvValues(void)
{
    ClearAllRecvValues();
}

///
/// @brief
///    清除通道所有接收值
///
void CChannelRecvValues::ClearAllRecvValues()
{
    // 锁定临界区
    m_criticalHistoryData.Lock();

    // 遍历容器，释放对象
    std::list<CHistoryData*>::iterator iter = m_listHistory.begin();
    while (m_listHistory.end() != iter)
    {
        CHistoryData* pHistoryData = *iter;
        if (pHistoryData)
        {
            delete pHistoryData;
        }

        iter++;
    }

    // 清空子通道
    m_listHistory.clear();

    // 释放临界区互斥锁
    m_criticalHistoryData.Unlock();
}

///
/// @brief
///    添加GOOSE通道历史接收值
///
/// @param[in]  stChangeTime    GOOSE值变位时间（相对于实验开始的时间）
/// @param[in]  stNewGooseData  GOOSE通道值
///
/// @return
///     bool    成功返回true
///
bool CChannelRecvValues::SaveRecvValue( CEventTime stChangeTime, GOOSE_DATA_VARIANT& stNewGooseData )
{
    // new一个CHistoryData对象
    CHistoryData* pHistoryData = new CHistoryData;
    if (NULL == pHistoryData)
    {
        return false;
    }

    // 保存通道值和变位时间
    pHistoryData->m_stChangeTime    = stChangeTime;
    pHistoryData->m_stHistoryValue  = stNewGooseData;

    // 锁定临界区
    m_criticalHistoryData.Lock();

    //
    // 将接收值保存到容器中
    //
    if (m_listHistory.size() >= MAX_HISTORY_RECV_RECORD)
    {
        // 释放最老记录
        delete m_listHistory.front();
        m_listHistory.pop_front();
    }
    m_listHistory.push_back(pHistoryData);
    m_iChangeTimes++;

    // 释放临界区互斥锁
    m_criticalHistoryData.Unlock();
    return true;
}

///
/// @brief
///    获得虚拟通道名
///
/// @return
///     std::wstring    虚拟通道名
///
std::wstring CChannelRecvValues::GetChannelName()
{
    return m_strName;
}

///
/// @brief
///    获得通道值变位次数
///
/// @return
///     int    变位次数
///
int CChannelRecvValues::GetChangeTimes()
{
    return m_iChangeTimes;
}

///
/// @brief
///    获得第一个接收值
///
/// @return
///     CHistoryData*    指向通道历史值对象的指针，失败时返回NULL
///
CHistoryData* CChannelRecvValues::FirstRecvData()
{
    CHistoryData* pHistoryData = NULL;


    // 锁定临界区
    m_criticalHistoryData.Lock();

    // 获得第一条历史记录
    std::list<CHistoryData*>::iterator iter = m_listHistory.begin();
    if (m_listHistory.end() != iter)
    {
        pHistoryData = *iter;
    }

    // 释放临界区互斥锁
    m_criticalHistoryData.Unlock();
    return pHistoryData;
}

///
/// @brief
///    获得下一个接收值
///
/// @param[in]  pCurHistoryData    指向当前通道历史值对象的指针
///
/// @return
///     CHistoryData*    指向下一个通道历史值对象的指针，失败时返回NULL
///
CHistoryData* CChannelRecvValues::NextHistoryData( CHistoryData* pCurHistoryData )
{
    CHistoryData*                       pHistoryData    = NULL;
    std::list<CHistoryData*>::iterator  iter;


    // 锁定临界区
    m_criticalHistoryData.Lock();

    iter = m_listHistory.begin();
    while (m_listHistory.end() != iter)
    {
        // 查找当前历史值对象的位置
        if (*iter == pCurHistoryData)
        {
            // 获得下一个历史值对象的位置
            if (m_listHistory.end() != ++iter)
            {
                pHistoryData = *iter;
            }

            break;
        }

        iter++;
    }

    // 释放临界区互斥锁
    m_criticalHistoryData.Unlock();
    return pHistoryData;
}
