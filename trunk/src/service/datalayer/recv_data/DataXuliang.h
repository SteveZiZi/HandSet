/// @file
///
/// @brief
///     定义序量数据信息
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.4.24
///
/// 修改历史：
///

#pragma once
#include "DataEffectiveVal.h"

class CDataXuliang : public CDataEffectiveVal
{
public:
    CDataXuliang(void);
    ~CDataXuliang(void);

public:
    ///
    /// @brief
    ///    获取零序、正序、负序数值
    ///
    /// @param[out] fZeroPhaseSeqContent   获取零序数值
    /// @param[out] fPosPhaseSeqContent    获取正序数值
    /// @param[out] fNegPhaseSeqContent    获取负序数值
    ///
    void GetPhaseSeqContent(float& fZeroPhaseSeqContent, float& fPosPhaseSeqContent, float& fNegPhaseSeqContent);

    ///
    /// @brief
    ///    获取零序、正序、负序角度
    ///
    /// @param[out] fZeroPhaseSeqAngle   获取零序角度
    /// @param[out] fPosPhaseSeqAngle    获取正序角度
    /// @param[out] fNegPhaseSeqAngle    获取负序角度
    ///
    void GetPhaseSeqAngle(float& fZeroPhaseSeqAngle, float& fPosPhaseSeqAngle, float& fNegPhaseSeqAngle);

    ///
    /// @brief
    ///    获取三相电流通道
    ///
    /// @param[out]  nPhaseA 保存A相通道号
    /// @param[out]  nPhaseB 保存B相通道号
    /// @param[out]  nPhaseC 保存C相通道号
    ///
    /// return
    ///     bool    获取成功返回true
    ///
    bool GetABCChannles(uint32_t& nCurPhaseA, uint32_t& nCurPhaseB, uint32_t& nCurPhaseC);

    ///
    /// @brief
    ///    设置三相电压/电流通道
    ///
    /// @param[in]  nPhaseA A相通道号
    /// @param[in]  nPhaseB B相通道号
    /// @param[in]  nPhaseC C相通道号
    ///
    /// return
    ///     bool    设置成功返回true
    ///
    bool SetABCChannles(uint32_t nVolPhaseA, uint32_t nVolPhaseB, uint32_t nVolPhaseC);

    ///
    /// @brief
    ///    设置当前序量电压电流模式
    ///
    void SetXLMode(enumChannelType enChannelType);

    ///
    /// @brief
    ///    获取当前序量的电压电流模式
    ///
    enumChannelType GetXLMode();

    ///
    /// @brief
    ///    清空序量信息
    ///
    void ClearXLData();

private:
    friend class CFrameRecv;
    ///
    /// @brief
    ///    设置零序、正序、负序数值
    ///
    /// @param[in] enChannelType          指出设置的是电压还是电流的序量值
    /// @param[in] fZeroPhaseSeqContent   设置零序数值
    /// @param[in] fPosPhaseSeqContent    设置正序数值
    /// @param[in] fNegPhaseSeqContent    设置负序数值
    ///
    void SetPhaseSeqContent(enumChannelType enChannelType, float& fZeroPhaseSeqContent, float& fPosPhaseSeqContent, float& fNegPhaseSeqContent);

    ///
    /// @brief
    ///    设置零序、正序、负序角度
    ///
    /// @param[in] enChannelType        指出设置的是电压还是电流的序量角度
    /// @param[in] fZeroPhaseSeqAngle   设置零序角度
    /// @param[in] fPosPhaseSeqAngle    设置正序角度
    /// @param[in] fNegPhaseSeqAngle    设置负序角度
    ///
    void SetPhaseSeqAngle(enumChannelType enChannelType, float& fZeroPhaseSeqAngle, float& fPosPhaseSeqAngle, float& fNegPhaseSeqAngle);

private:
    float           m_fPhaseSeqContent[2][3];                         ///< 零序、正序、负序的数值，分别表示电压，电流的数据
    float           m_fPhaseSeqAngle[2][3];                           ///< 零序、正序、负序的相位角，分别表示电压，电流的数据
    enumChannelType m_enCurrentType;                                  ///< 当前计算序量的模式，是电压还是电流
};
