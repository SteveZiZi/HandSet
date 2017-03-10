/// @file
///
/// @brief
///     GOOSE发送配置
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.5.27
///
/// 修改历史：
///

#ifndef __PARAM_CONFIG_GOOSE_SEND_H__
#define __PARAM_CONFIG_GOOSE_SEND_H__
#include <map>
#include "ParamConfigGOOSEBase.h"
#include "GOOSESendVirtualChannel.h"

///
/// @brief
///    GOOSE发送参数配置类
///
class CParamConfigGOOSESend : public CParamConfigGOOSEBase
{
    friend class CStateTestResult;

protected:
    CParamConfigGOOSESend(void);
    ~CParamConfigGOOSESend(void);

public:
    ///
    /// @brief
    ///     获取配置实例接口
    ///
    static CParamConfigGOOSESend* getInstance();

    ///
    /// @brief
    ///    加载配置文件
    ///
    bool LoadConfig( std::wstring strConfigFile );

    ///
    /// @brief
    ///    保存配置文件
    ///
    /// @return
    ///     bool    true表示保存成功
    ///
    bool SaveConfig();

    ///
    /// @brief
    ///    获得心跳间隔T0
    ///
    /// @return
    ///     int     心跳间隔(ms)
    ///
    int GetHeartbeatInterval();

    ///
    /// @brief
    ///    设置心跳间隔T0
    ///
    /// @param[in]  iNewInterval 心跳间隔T0(ms)
    ///
    /// @return
    ///     bool    true表示保存成功
    ///
    bool SetHeartbeatInterval(int iNewInterval);

    ///
    /// @brief
    ///    获得最小间隔T1
    ///
    /// @return
    ///     int     最小间隔(ms)，失败时返回-1
    ///
    int GetMinimumInterval();

    ///
    /// @brief
    ///    设置最小间隔T1
    ///
    /// @param[in]  iNewInterval 最小间隔T1(ms)
    ///
    /// @return
    ///     bool    true表示保存成功
    ///
    bool SetMinimumInterval(int iNewInterval);

    ///
    /// @brief
    ///    枚举第一个GOOSE发送虚拟通道
    ///
    ///
    /// @return
    ///     CGOOSESendVirtualChannel*    虚拟通道指针，失败时返回NULL
    ///
    CGOOSESendVirtualChannel* FirstGOOSESendVirtualChannel();

    ///
    /// @brief
    ///    枚举下一个GOOSE发送虚拟通道
    ///
    /// @param[in]  pCurVirtualChannel    指向当前虚拟通道的指针
    ///
    /// @return
    ///     CGOOSESendVirtualChannel*    下一个虚拟通道指针，失败时返回NULL
    ///
    CGOOSESendVirtualChannel* NextGOOSESendVirtualChannel(CGOOSESendVirtualChannel* pCurVirtualChannel);

    ///
    /// @brief
    ///    根据DO名获得GOOSE发送虚拟通道对象
    ///
    /// @param[in]  strChannelName    GOOSE发送虚拟通道的名字
    ///
    /// @return
    ///     CGOOSESendVirtualChannel*    指向GOOSE发送虚拟通道对象的指针
    ///
    CGOOSESendVirtualChannel* GetGOOSESendVirtualChannel(std::wstring strChannelName);

    ///
    /// @brief
    ///    生成所有GOOSE发送虚拟通道的真实通道映射表
    ///    注意：调用FirstGOOSESendVirtualChannel()函数时会自动调用此函数，
    ///           如果需要重新生成映射表，可以手动调用此函数
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool RebuildGOOSESendChannelMap();

    ///
    /// @brief
    ///     更新映射到指定虚拟通道的所有真实通道的值
    ///
    /// @param[in]  mapDoChangedArray   虚拟通道名和值
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool UpdateGooseChannelSendValue(std::map<std::wstring, bool>& mapDoChangedArray);

private:
    int                                 m_iHeartbeatInterval;           ///< 心跳间隔T0
    int                                 m_iMinimumInterval;             ///< 最小间隔T1

    std::map<std::wstring, CGOOSESendVirtualChannel*>   m_mapGOOSESendVirtualChannelGroup;  ///< 虚拟通道容器
};
#endif
