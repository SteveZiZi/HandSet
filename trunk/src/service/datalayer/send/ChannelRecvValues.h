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

#ifndef __CHANNEL_RECV_VALUES__
#define __CHANNEL_RECV_VALUES__
#include <string>
#include <list>
#include <afxmt.h>
#include "src\utils\MessageCodec\include\goose_types.h"

///
/// @brief
///    通道历史值
///
struct CHistoryData
{
    CEventTime              m_stChangeTime;                             ///< 通道值发生变化的时间
    GOOSE_DATA_VARIANT      m_stHistoryValue;                           ///< 通道历史值

    ///
    /// @brief
    ///    通道值发生变化的时间
    ///
    /// @return
    ///    std::wstring    UTC时间字符串
    ///
    std::wstring GetChangeTimeString()
    {
        int         iMilliSecond;
        wchar_t     cBuffer[128]    = {0};


        // 格式化成时间字符串
        iMilliSecond = m_stChangeTime.ts_s*1000 + m_stChangeTime.ts_ns/1000000;
        swprintf_s(cBuffer, sizeof(cBuffer)/sizeof(cBuffer[0]), L"%d.0", iMilliSecond);
        return cBuffer;
    }

    ///
    /// @brief
    ///    获得指向通道历史值的指针
    ///
    /// @return
    ///     GOOSE_DATA_VARIANT*    指向通道历史值的指针
    ///
    GOOSE_DATA_VARIANT* GetGooseData()
    {
        return &m_stHistoryValue;
    }
};

///
/// @brief
///    存放通道所有接收值的类
///
class CChannelRecvValues
{
public:
    CChannelRecvValues(std::wstring strName);
    ~CChannelRecvValues(void);

    ///
    /// @brief
    ///    清除通道所有接收值
    ///
    void ClearAllRecvValues();

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
    bool SaveRecvValue(CEventTime stChangeTime, GOOSE_DATA_VARIANT& stNewGooseData);

    ///
    /// @brief
    ///    获得虚拟通道名
    ///
    /// @return
    ///     std::wstring    虚拟通道名
    ///
    std::wstring GetChannelName();

    ///
    /// @brief
    ///    获得通道值变位次数
    ///
    /// @return
    ///     int    变位次数
    ///
    int GetChangeTimes();

    ///
    /// @brief
    ///    获得第一个接收值
    ///
    /// @return
    ///     CHistoryData*    指向通道历史值对象的指针，失败时返回NULL
    ///
    CHistoryData* FirstRecvData();

    ///
    /// @brief
    ///    获得下一个接收值
    ///
    /// @param[in]  pCurHistoryData    指向当前通道历史值对象的指针
    ///
    /// @return
    ///     CHistoryData*    指向下一个通道历史值对象的指针，失败时返回NULL
    ///
    CHistoryData* NextHistoryData(CHistoryData* pCurHistoryData);

private:
    std::wstring                    m_strName;                          ///< 虚拟通道名
    int                             m_iChangeTimes;                     ///< 变位次数

    CCriticalSection                m_criticalHistoryData;              ///< 访问容器的临界区，用于互斥对接收值容器的访问
    std::list<CHistoryData*>        m_listHistory;                      ///< 保存通道接收值的容器
};
#endif
