///
/// @brief
///     GOOSE接收虚拟通道
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// 作者:
///    chao  2013.6.13
///
/// 修改历史：
///

#ifndef __GOOSE_RECV_VIRTUAL_CHANNEL_H__
#define __GOOSE_RECV_VIRTUAL_CHANNEL_H__
#include <string>
#include "Channel.h"

///
/// @brief
///     虚拟通道名
///
static const std::wstring strGOOSERecvVCName[] =
{
    L"DI1",
    L"DI2",
    L"DI3",
    L"DI4",
    L"DI5",
    L"DI6",
    L"DI7",
    L"DI8"
};

class CGOOSERecvVirtualChannel
{
    friend class CParamConfigGOOSERecv;

public:
    CGOOSERecvVirtualChannel(void);
    virtual ~CGOOSERecvVirtualChannel(void);

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
    ///     获得绑定的真实通道
    ///
    ///
    /// @return
    ///     CChannel*    指向GOOSE接收通道的指针
    ///
    CChannel* GetChannel();

private:
    std::wstring                        m_strName;                      ///< 虚拟通道名
    CChannel*                           m_pChannel;                     ///< 绑定的真实通道
};
#endif
