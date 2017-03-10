/// @file
///
/// @brief
///     定义波形图需要的数据
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.4.24
///
/// 修改历史：
///

#pragma once
#include <vector>
#include "src\service\datalayer\recv_data\DataEffectiveVal.h"


typedef std::vector<float>  OscillogramSmv;

class COscillogramChannelInfo : public CEffectiveChanelInfo
{
public:
    OscillogramSmv      m_vecChannelData;       ///< 每个通道保存的采样点数据
};

class CDataOscillogram : public CDataRecvBase
{
public:
    CDataOscillogram(void);
    ~CDataOscillogram(void);

public:
    ///
    /// @brief
    ///    获取一个通道保存的采样点数目
    ///
    uint32_t GetOneChannelSmvCount();

    ///
    /// @brief
    ///    获取某一个通道的数据
    ///
    /// @param[in]  nIndex 要获取的通道索引
    ///
    /// @return
    ///     COscillogramChannelInfo* 获取的通道信息指针
    ///
    COscillogramChannelInfo* GetOneChannelInfo(uint32_t nIndex);

    ///
    /// @brief
    ///    添加一个通道
    ///
    /// @param[in]  rChannelInfo 通道信息
    ///
    /// @return
    ///     bool 添加成功返回true
    ///
    bool AddOneChannelInfo(COscillogramChannelInfo& rChannelInfo);

    ///
    /// @brief
    ///    删除一个通道
    ///
    /// @param[in]  rChannelInfo 通道信息
    ///
    /// @return
    ///     bool 移除成功返回true
    ///
    bool RemoveOneChannelInfo(const COscillogramChannelInfo& rChannelInfo);


    ///
    /// @brief
    ///    清理通道容器数据
    ///
    void ClearChannelVec();

    ///
    /// @brief
    ///    自动生成N个空通道
    ///
    void GenNChannels(uint32_t nChannelCount, uint32_t nVecReserveSize = 0);

private:
    ///
    /// @brief
    ///    判断某一个通道是否存在
    ///
    /// @param[in]  rEffectiveChannelInfo 通道信息
    ///
    /// @return
    ///     bool 已经存在返回true
    ///
    bool _IsChannelInVec(const COscillogramChannelInfo& rChannelInfo);

protected:
    uint32_t                                m_nVecFloatSize;    ///< 定义一个通道保存数据点数
    std::vector<COscillogramChannelInfo*>   m_vecChannels;      ///< 保存各个通道数据容器
};
