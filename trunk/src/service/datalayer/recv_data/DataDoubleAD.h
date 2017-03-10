/// @file
///
/// @brief
///     定义双ad数据信息
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.4.24
///
/// 修改历史：
///

#pragma once
#include "src\service\datalayer\recv_data\DataRecvBase.h"
#include "src\service\datalayer\recv\FrameRecvStruct.h"

#define DOUBLE_AD_GROUP_COUNT (6)     ///< 定义双AD的比较组数

class CDataDoubleAD : public CDataRecvBase
{
public:
    CDataDoubleAD(void);
    ~CDataDoubleAD(void);

    ///
    /// @brief
    ///    设置AD1对应的通道号
    ///
    /// @param[in] nIndexADGroup 指出第几组AD
    /// @param[in] nIndexChannel 指出要设置的通道号
    ///
    void SetAD1Channel(uint32_t nIndexADGroup, uint32_t nIndexChannel);

    ///
    /// @brief
    ///    设置AD2对应的通道号
    ///
    /// @param[in] nIndexADGroup 指出第几组AD
    /// @param[in] nIndexChannel 指出要设置的通道号
    ///
    void SetAD2Channel(uint32_t nIndexADGroup, uint32_t nIndexChannel);

    ///
    /// @brief
    ///    获取AD1对应的通道号
    ///
    /// @param[in] nIndexADGroup 指出第几组AD
    ///
    /// @return
    ///     uint32_t 返回获取到的通道号，超出通道数目代表无效
    ///
    uint32_t GetAD1Channel(uint32_t nIndexADGroup);

    ///
    /// @brief
    ///    获取AD2对应的通道号
    ///
    /// @param[in] nIndexADGroup 指出第几组AD
    ///
    /// @return
    ///     uint32_t 返回获取到的通道号，超出通道数目代表无效
    ///
    uint32_t GetAD2Channel(uint32_t nIndexADGroup);

    ///
    /// @brief
    ///    获取某一组双AD计算结果
    ///
    /// @param[in]  nIndexADGroup   要获取的双AD组别
    /// @param[out] fVal1           要获取的结果1
    /// @param[out] fVAl2           要获取的结果2
    ///
    /// @return
    ///     bool    获取成功返回true
    ///
    bool GetADGroupResult(uint32_t nIndexADGroup, float& fVal1, float& fVal2);

    ///
    /// @brief
    ///    设置双ad的数据
    ///
    void SetDoubleAdSmvValue(const SMV_VALUE& stSmvValue);

    ///
    /// @brief
    ///    清空双AD保留信息
    ///
    void ClearDoubleADData();

private:
    ///
    /// @brief
    ///    保存双AD比较通道信息
    ///
    /// @note
    ///     数据保存格式为二维数组，通道索引号超出通道数为无效数据
    ///     如下所示，表示通道1、2为一组，5、4为一组，3、6为一组双AD
    ///      1 2
    ///      5 4
    ///      3 6
    uint32_t        m_nADGroupChannels[DOUBLE_AD_GROUP_COUNT][2];
    SMV_VALUE       m_stSmvValue;
};
