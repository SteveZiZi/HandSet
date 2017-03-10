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

#include "StdAfx.h"
#include "GOOSERecvVirtualChannel.h"

///
/// @brief
///     ���캯��
///
CGOOSERecvVirtualChannel::CGOOSERecvVirtualChannel(void)
:m_strName(L"")
,m_pChannel(NULL)
{
}

///
/// @brief
///     �鹹����
///
CGOOSERecvVirtualChannel::~CGOOSERecvVirtualChannel(void)
{
}

///
/// @brief
///     �������ͨ����
///
///
/// @return
///     std::wstring    ����ͨ����
///
std::wstring CGOOSERecvVirtualChannel::GetName()
{
    return m_strName;
}

///
/// @brief
///     ��������ͨ����
///
/// @param[in]  strName ����ͨ����
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
///     ��ð󶨵���ʵͨ��
///
///
/// @return
///     CChannel*    ָ��GOOSE����ͨ����ָ��
///
CChannel* CGOOSERecvVirtualChannel::GetChannel()
{
    return m_pChannel;
}
