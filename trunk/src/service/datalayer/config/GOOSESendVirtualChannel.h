///
/// @brief
///     GOOSE发送虚拟通道
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.6.13
///
/// 修改历史：
///

#ifndef __GOOSE_SEND_VIRTUAL_CHANNEL_H__
#define __GOOSE_SEND_VIRTUAL_CHANNEL_H__
#include <string>
#include <vector>
#include "Channel.h"

class CGOOSESendVirtualChannel
{
    friend class CParamConfigGOOSESend;

public:
    CGOOSESendVirtualChannel(void);
    virtual ~CGOOSESendVirtualChannel(void);

    ///
    /// @brief
    ///     获得虚拟通道名
    ///
    ///
    /// @return
    ///     std::wstring    虚拟通道名
    ///
    std::wstring GetName();

    ///
    /// @brief
    ///     设置虚拟通道名
    ///
    /// @param[in]  strName 虚拟通道名
    ///
    void SetName(std::wstring strName);

    ///
    /// @brief
    ///    枚举第一个Channel
    ///
    /// @return
    ///     CChannel*    成功时返回通道指针，失败时返回NULL
    ///
    CChannel* FirstChannel();

    ///
    /// @brief
    ///    枚举下一个Channel
    ///
    /// @param[in]  pCurChannel 当前Channel的指针
    ///
    /// @return
    ///     CChannel*    成功时返回Channel指针，失败时返回NULL
    ///
    CChannel* NextChannel(CChannel* pCurChannel);

    ///
    /// @brief
    ///    是否有真实通道映射到此虚拟通道
    ///
    /// @return
    ///     bool    有返回true
    ///
    bool IsMaped();

    ///
    /// @brief
    ///     设置所有映射到此虚拟通道的真实通道的发送值
    ///
    /// @param[in]  bNewValue   新的发送值
    ///
    /// @return
    ///     bool    成功返回true
    ///
    bool SetChannelSendValue(bool bNewValue);

private:
    std::wstring                        m_strName;                      ///< 虚拟通道名
    std::vector<CChannel *>             m_vecChannelGroup;              ///< 通道容器，用于存放映射到此虚拟通道的真实通道的指针
};
#endif
