/// @file
///
/// @brief
///     一个控制块通道的有效值/相位信息
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.4.22
///
/// 修改历史：
///

#pragma once

#include <string>
#include <vector>
#include "DataRecvBase.h"
#include "src\include\stdint.h"
#include "src\service\datalayer\include\datatypes.h"

///
/// @brief
///    有效值通道信息，对应界面上一行数据
///
class CEffectiveChanelInfo : public CDataRecvChannelBase
{
public:
    float               m_fEffectiveValue;              ///< 有效值
    float               m_fPhase;                       ///< 相位
    bool                m_bIsDcValue;                   ///< 是否是直流

    CEffectiveChanelInfo();
};
inline CEffectiveChanelInfo::CEffectiveChanelInfo() : m_fEffectiveValue(0)
                                                    , m_fPhase(0)
                                                    , m_bIsDcValue(false)
{
}

class CDataEffectiveVal : public CDataRecvBase
{
public:
    CDataEffectiveVal(void);
    ~CDataEffectiveVal(void);

public:

    ///
    /// @brief
    ///    获取某一个通道的数据
    ///
    /// @param[in]  nIndex 要获取的通道索引
    ///
    /// @return
    ///     CEffectiveChanelInfo* 获取的通道信息指针
    ///
     CEffectiveChanelInfo* GetOneChannelInfo(uint32_t nIndex);

     ///
     /// @brief
     ///    添加一个通道
     ///
     /// @param[in]  rEffectiveChannelInfo 通道信息
     ///
     /// @return
     ///     bool 添加成功返回true
     ///
     bool AddOneChannelInfo(const CEffectiveChanelInfo& rEffectiveChannelInfo);

     ///
     /// @brief
     ///    删除一个通道
     ///
     /// @param[in]  rEffectiveChannelInfo 通道信息
     ///
     /// @return
     ///     bool 移除成功返回true
     ///
     bool RemoveOneChannelInfo(const CEffectiveChanelInfo& rEffectiveChannelInfo);

    ///
    /// @brief
    ///    清理通道容器数据
    ///
    void ClearChannelVec();

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
    bool _IsChannelInVec(const CEffectiveChanelInfo& rEffectiveChannelInfo);



protected:
    std::vector<CEffectiveChanelInfo*>      m_vecChannels;                  ///< 通道容器

};
