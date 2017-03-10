/// @file
///
/// @brief
///     定义谐波数据信息
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.5.14
///
/// 修改历史：
///

#pragma once

#include "src\service\datalayer\recv_data\DataRecvBase.h"

class CDataHarmonic : public CDataRecvBase
{
public:
    CDataHarmonic(void);
    ~CDataHarmonic(void);

public:
    ///
    /// @brief
    ///    获取通道DC有效值
    ///
    float GetChannelDCValue(uint32_t nIndex);

    ///
    /// @brief
    ///    获取通道dc有效值含有率
    ///
    float GetChannelDcTHD(uint32_t nIndex);

    ///
    /// @brief
    ///    获取某个通道某次谐波含有率，谐波总含有率
    ///
    /// @param[in]  nChannelIndex 通道索引，从0开始
    ///
    /// @param[in]  nHarmType 谐波次数，基波为一次谐波，传递数值1表示一次谐波,传递数值0表示通道总谐波含有率 
    ///
    /// @return
    ///     float 对应通道对应类型谐波含有率
    ///
    float GetChannelHarmTHD(uint32_t nChannelIndex, uint32_t nHarmType = 0);

    ///
    /// @brief
    ///    获取一个通道某次谐波有效值和相位信息
    ///
    /// @param[in]  nChannelIndex 通道索引，从0开始
    ///
    /// @param[in]  nHarmType 谐波次数，基波为一次谐波，传递数值1表示一次谐波
    ///
    /// @param[out] fHarmContent 保存对应次谐波的有效值
    ///
    /// @param[out] fHarmAngle   保存对应次谐波的相位角
    ///
    /// @return
    ///     bool 获取成功返回true
    ///
    bool GetChannelHarmInfo(uint32_t nChannelIndex, uint32_t nHarmType, float& fHarmContent, float& fHarmAngle);

    ///
    /// @brief
    ///    清空谐波信息
    ///
    void ClearHarmonicData();

private:
    friend class CFrameRecv;
    float    m_fHarmContent[MAX_SMV_CHANNELS][MAX_HARM_COUNT];        ///< 各通道谐波有效值
    float    m_fHarmAngle[MAX_SMV_CHANNELS][MAX_HARM_COUNT];          ///< 各通道谐波相位
    float    m_fSumHarmTHD[MAX_SMV_CHANNELS];                         ///< 各通道总谐波含有率
    float    m_fHarmTHD[MAX_SMV_CHANNELS][MAX_HARM_COUNT];            ///< 各通道谐波含有率
    float    m_fDCValue[MAX_SMV_CHANNELS];                            ///< DC有效值
    float    m_fDCTHD[MAX_SMV_CHANNELS];                              ///< 各通道直流含有率
};
