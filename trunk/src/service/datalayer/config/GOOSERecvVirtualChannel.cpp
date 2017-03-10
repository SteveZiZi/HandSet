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

#include "StdAfx.h"
#include "GOOSERecvVirtualChannel.h"

///
/// @brief
///     构造函数
///
CGOOSERecvVirtualChannel::CGOOSERecvVirtualChannel(void)
:m_strName(L"")
,m_pChannel(NULL)
{
}

///
/// @brief
///     虚构函数
///
CGOOSERecvVirtualChannel::~CGOOSERecvVirtualChannel(void)
{
}

///
/// @brief
///     获得虚拟通道名
///
///
/// @return
///     std::wstring    虚拟通道名
///
std::wstring CGOOSERecvVirtualChannel::GetName()
{
    return m_strName;
}

///
/// @brief
///     设置虚拟通道名
///
/// @param[in]  strName 虚拟通道名
///
void CGOOSERecvVirtualChannel::SetName( std::wstring strName )
{
    if (m_strName != strName)
    {
        m_strName = strName;
    }
}

///
/// @brief
///     获得绑定的真实通道
///
///
/// @return
///     CChannel*    指向GOOSE接收通道的指针
///
CChannel* CGOOSERecvVirtualChannel::GetChannel()
{
    return m_pChannel;
}
