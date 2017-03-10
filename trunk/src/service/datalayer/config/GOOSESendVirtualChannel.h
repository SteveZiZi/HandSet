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
    ///    ö�ٵ�һ��Channel
    ///
    /// @return
    ///     CChannel*    �ɹ�ʱ����ͨ��ָ�룬ʧ��ʱ����NULL
    ///
    CChannel* FirstChannel();

    ///
    /// @brief
    ///    ö����һ��Channel
    ///
    /// @param[in]  pCurChannel ��ǰChannel��ָ��
    ///
    /// @return
    ///     CChannel*    �ɹ�ʱ����Channelָ�룬ʧ��ʱ����NULL
    ///
    CChannel* NextChannel(CChannel* pCurChannel);

    ///
    /// @brief
    ///    �Ƿ�����ʵͨ��ӳ�䵽������ͨ��
    ///
    /// @return
    ///     bool    �з���true
    ///
    bool IsMaped();

    ///
    /// @brief
    ///     ��������ӳ�䵽������ͨ������ʵͨ���ķ���ֵ
    ///
    /// @param[in]  bNewValue   �µķ���ֵ
    ///
    /// @return
    ///     bool    �ɹ�����true
    ///
    bool SetChannelSendValue(bool bNewValue);

private:
    std::wstring                        m_strName;                      ///< ����ͨ����
    std::vector<CChannel *>             m_vecChannelGroup;              ///< ͨ�����������ڴ��ӳ�䵽������ͨ������ʵͨ����ָ��
};
#endif
