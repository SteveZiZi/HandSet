/// @file
///
/// @brief
///     B码对时数据
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.6.20
///
/// 修改历史：
///

#pragma once
#include "Winbase.h"

#define IRIGB_GET_LSP(n)                  (n & 0x01)                    ///< 闰秒预告
#define IRIGB_GET_LS(n)                   (n>>1 & 0x01)                 ///< 闰秒标志,0正闰秒，1负闰秒
#define IRIGB_GET_DSP(n)                  (n>>2 & 0x01)                 ///< 夏时制预告
#define IRIGB_GET_DST(n)                  (n>>3 & 0x01)                 ///< 夏时制时置1
#define IRIGB_GET_OFFSET_SIGN(n)          (n>>4 & 0x01)                 ///< 时间偏移符号位，0："+", 1:"-"
#define IRIGB_GET_OFFSET(n)               (n>>5 & 0x0F)                 ///< 时间偏移小时，irigb时间-utc时间
#define IRIGB_GET_OFFSET_0_5(n)           (n>>9 & 0x01)                 ///< 0不增加时间偏移量，1增加时间偏移量0.5小时
#define IRIGB_GET_QUALITY(n)              (n>>10 & 0x0F)                ///< 时间质量

class CDataTimeSyncIRIGB
{
public:
    CDataTimeSyncIRIGB() : m_nLSP(0)
                         , m_nLS(0)
                         , m_nDSP(0)
                         , m_nDST(0)
                         , m_fTimeOffset(0)
                         , m_nTimeQuality(0)
    {
        memset(&m_stSysTime, 0, sizeof(m_stSysTime));
    }
    
    ~CDataTimeSyncIRIGB()
    {
    }

public:
    ///
    /// @brief
    ///    获取时间日期
    ///
    SYSTEMTIME GetDateTime()
    {
        return m_stSysTime;
    }

    ///
    /// @brief
    ///    获取闰秒预告标志
    ///
    uint32_t GetLSP()
    {
        return m_nLSP;
    }

    ///
    /// @brief
    ///    获取闰秒标志
    ///
    uint32_t GetLS()
    {
        return m_nLS;
    }

    ///
    /// @brief
    ///    获取夏时制预告标志
    ///
    uint32_t GetDSP()
    {
        return m_nDSP;
    }

    ///
    /// @brief
    ///    获取夏时制标志
    ///
    uint32_t GetDST()
    {
        return m_nDST;
    }

    ///
    /// @brief
    ///    获取时间偏移（小时）
    ///
    float GetTimeOffset()
    {
        return m_fTimeOffset;
    }

    ///
    /// @brief
    ///    获取时间质量
    ///
    uint32_t GetTimeQuality()
    {
        return m_nTimeQuality;
    }

    ///
    /// @brief
    ///    重置所有数据
    ///
    void ResetData()
    {
        m_nDSP = 0;
        m_nDST = 0;
        m_nLSP = 0;
        m_nLS  = 0;
        m_fTimeOffset = 0;
        m_nTimeQuality = 0;
        memset(&m_stSysTime, 0, sizeof(m_stSysTime));
    }

private:
    friend class CDsmDataLayer;
    ///
    /// @brief
    ///     设置时间日期
    ///
    void SetDateTime(const SYSTEMTIME* pTime)
    {
        if (pTime)
        {
            m_stSysTime = *pTime;
        }
    }

    ///
    /// @brief
    ///    设置闰秒预告标志
    ///
    void SetLSP(uint32_t nLSP)
    {
        m_nLSP = nLSP;
    }

    ///
    /// @brief
    ///    设置闰秒标志
    ///
    void SetLS(uint32_t nLS)
    {
        m_nLS = nLS;
    }

    ///
    /// @brief
    ///    设置夏时制预告标志
    ///
    void SetDSP(uint32_t nDSP)
    {
        m_nDSP = nDSP;
    }

    ///
    /// @brief
    ///    设置夏时制标志
    ///
    void SetDST(uint32_t nDST)
    {
        m_nDST = nDST;
    }

    ///
    /// @brief
    ///    设置时间偏移（小时）
    ///
    void SetTimeOffset(float fTimeOffset)
    {
        m_fTimeOffset = fTimeOffset;
    }

    ///
    /// @brief
    ///    设置时间质量
    ///
    void SetTimeQuality(uint32_t nQuality)
    {
        m_nTimeQuality = nQuality;
    }

private:
    SYSTEMTIME                      m_stSysTime;                        ///< 时间日期信息
    uint32_t                        m_nLSP;                             ///< 闰秒预告标志
    uint32_t                        m_nLS;                              ///< 闰秒标志
    uint32_t                        m_nDSP;                             ///< 夏时制预告标志
    uint32_t                        m_nDST;                             ///< 夏时制标志
    float                           m_fTimeOffset;                      ///< 时间偏移（小时）
/*
时间质量代表的意义
0x0：正常工作状态，时钟同步正常
0x1：时钟同步异常，时间准确度优于1 ns
0x2：时钟同步异常，时间准确度优于10 ns
0x3：时钟同步异常，时间准确度优于100 ns
0x4：时钟同步异常，时间准确度优于1μs
0x5：时钟同步异常，时间准确度优于10μs
0x6：时钟同步异常，时间准确度优于100μs
0x7：时钟同步异常，时间准确度优于1 ms
0x8：时钟同步异常，时间准确度优于10 ms
0x9：时钟同步异常，时间准确度优于100 ms
0xA：时钟同步异常，时间准确度优于1 s
0xB：时钟同步异常，时间准确度优于10 s
0xF：时钟严重故障，时间信息不可信赖
*/
    uint32_t                        m_nTimeQuality;                     ///< 时间质量

};