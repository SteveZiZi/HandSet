///
/// @brief
///     GOOSE��������ͨ��
///
/// Copyright (c) 2013 IUnknownBase Inc
///
/// ����:
///    chao  2013.6.13
///
/// �޸���ʷ��
///

#ifndef __GOOSE_RECV_VIRTUAL_CHANNEL_H__
#define __GOOSE_RECV_VIRTUAL_CHANNEL_H__
#include <string>
#include "Channel.h"

///
/// @brief
///     ����ͨ����
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
    ///     �������ͨ����
    ///
    ///
    /// @return
    ///     std::wstring    ����ͨ����
    ///
    std::wstring GetName();

    ///
    /// @brief
    ///     ��������ͨ����
    ///
    /// @param[in]  strName ����ͨ����
    ///
    void SetName(std::wstring strName);

    ///
    /// @brief
    ///     ��ð󶨵���ʵͨ��
    ///
    ///
    /// @return
    ///     CChannel*    ָ��GOOSE����ͨ����ָ��
    ///
    CChannel* GetChannel();

private:
    std::wstring                        m_strName;                      ///< ����ͨ����
    CChannel*                           m_pChannel;                     ///< �󶨵���ʵͨ��
};
#endif
