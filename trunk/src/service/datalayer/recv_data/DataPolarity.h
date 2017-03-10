/// @file
///
/// @brief
///     定义极性数据信息
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.5.18
///
/// 修改历史：
///

#pragma once

#include "src\service\datalayer\recv_data\DataRecvBase.h"

class CDataPolarityChannelInfo : public CDataRecvChannelBase
{
public:
    int32_t     m_nPolarity;                ///< 指示通道极性信息，值大于0为正极性，小于0为反极性

    CDataPolarityChannelInfo(): m_nPolarity(0)
    {
    }
};


class CDataPolarity : public CDataRecvBase
{
public:
    CDataPolarity(void);
    ~CDataPolarity(void);

public:
    ///
    /// @brief
    ///    添加一个通道
    ///
    /// @param[in]  rEffectiveChannelInfo 通道信息
    ///
    /// @return
    ///     bool 添加成功返回true
    ///
    bool AddOneChannelInfo(const CDataPolarityChannelInfo& rChannelInfo);

    ///
    /// @brief
    ///    获取某一个通道的数据
    ///
    /// @param[in]  nIndex 要获取的通道索引
    ///
    /// @return
    ///     CEffectiveChanelInfo* 获取的通道信息指针
    ///
    CDataPolarityChannelInfo* GetOneChannelInfo(uint32_t nIndex);

    ///
    /// @brief
    ///    获取通道的极性信息
    ///
    /// @param[in]      nIndex  通道序号，从0开始
    ///
    /// @return
    ///     int32_t     返回极性信息，大于0表示正极性，小于0表示反极性
    ///
    int32_t GetChannelPolarity(uint32_t nIndex);

    ///
    /// @brief
    ///    清空所有通道信息
    ///
    void ClearChannelInfo();

private:
    ///
    /// @brief
    ///    设置通道的极性信息
    ///
    /// @param[in]      nIndex  通道序号，从0开始
    /// @param[in]      nPorlarity  通道的极性信息
    ///
    /// @return
    ///     int32_t     设置极性信息，设置成功赶回true
    ///
    bool SetChannelPolarity(uint32_t nIndex, int32_t nPorlarity);

    ///
    /// @brief
    ///    判断某一个通道是否存在
    ///
    /// @param[in]  rEffectiveChannelInfo 通道信息
    ///
    /// @return
    ///     bool 已经存在返回true
    ///
    bool _IsChannelInVec(const CDataPolarityChannelInfo& rChannelInfo);

private:
    std::vector<CDataPolarityChannelInfo*>       m_vecChannels;      ///< 保存各个通道的极性信息
};
