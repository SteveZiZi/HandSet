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

#include "StdAfx.h"
#include "GooseCb.h"
#include "GOOSESendVirtualChannel.h"

///
/// @brief
///     构造函数
///
CGOOSESendVirtualChannel::CGOOSESendVirtualChannel(void)
:m_strName(L"")
{
    m_vecChannelGroup.clear();
}

///
/// @brief
///     析构函数
///
CGOOSESendVirtualChannel::~CGOOSESendVirtualChannel(void)
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
std::wstring CGOOSESendVirtualChannel::GetName()
{
    return m_strName;
}

///
/// @brief
///     设置虚拟通道名
///
/// @param[in]  strName 虚拟通道名
///
void CGOOSESendVirtualChannel::SetName( std::wstring strName )
{
    if (m_strName != strName)
    {
        m_strName = strName;
    }
}

///
/// @brief
///    枚举第一个Channel
///
/// @return
///     CChannel*    成功时返回通道指针，失败时返回NULL
///
CChannel* CGOOSESendVirtualChannel::FirstChannel()
{
    std::vector<CChannel*>::iterator    iter = m_vecChannelGroup.begin();
    if (m_vecChannelGroup.end() != iter)
    {
        return *iter;
    }

    return NULL;
}

///
/// @brief
///    枚举下一个Channel
///
/// @param[in]  pCurChannel 当前Channel的指针
///
/// @return
///     CChannel*    成功时返回Channel指针，失败时返回NULL
///
CChannel* CGOOSESendVirtualChannel::NextChannel( CChannel* pCurChannel )
{
    CChannel*                           pChannel   = NULL;
    std::vector<CChannel*>::iterator    iter;


    iter = m_vecChannelGroup.begin();
    while (m_vecChannelGroup.end() != iter)
    {
        pChannel = *iter;

        // 查找当前GOOSE控制块的位置
        if (pChannel == pCurChannel)
        {
            // 获得下一个GOOSE控制块的位置
            iter++;
            if (m_vecChannelGroup.end() == iter)
            {
                return NULL;
            }

            return (*iter);
        }

        iter++;
    }

    return NULL;
}

///
/// @brief
///    是否有真实通道映射到此虚拟通道
///
/// @return
///     bool    有返回true
///
bool CGOOSESendVirtualChannel::IsMaped()
{
    return (m_vecChannelGroup.size() > 0);
}

///
/// @brief
///     设置所有映射到此虚拟通道的真实通道的发送值
///
/// @param[in]  bNewValue   新的发送值
///
/// @return
///     bool    成功返回true
///
bool CGOOSESendVirtualChannel::SetChannelSendValue( bool bNewValue )
{
    CChannel*                           pChannel   = NULL;
    std::vector<CChannel*>::iterator    iter;


    // 遍历虚拟通道映射表，获得每一个GOOSE发送真实通道
    iter = m_vecChannelGroup.begin();
    for (; iter != m_vecChannelGroup.end(); iter++)
    {
        pChannel = *iter;
        if (pChannel)
        {
            // 获得GOOSE通道当前的发送值
            GOOSE_DATA_VARIANT m_stGooseSendValue = pChannel->GetGooseSendValue();

            // 修改单点类型通道的发送值
            if (CHANNEL_TYPE_GOOSE_POINT == pChannel->GetChannelType())
            {
                m_stGooseSendValue.m_stBoolean.boolVal = bNewValue;
                pChannel->SetGooseSendValue(m_stGooseSendValue);
            }
            // 修改双点类型通道的发送值
            else if (CHANNEL_TYPE_GOOSE_DOUBLE_POINT == pChannel->GetChannelType())
            {
                m_stGooseSendValue.m_stBitString.SetString(strDPValue[bNewValue? DP_VALUE_ON: DP_VALUE_OFF]);
                pChannel->SetGooseSendValue(m_stGooseSendValue);
            }
            // 其他
            else
            {
                return false;
            }

            // 修改通道的变位标识
            pChannel->SetSendValueChanged(true);

            // 修改父通道的变位标识
            CChannel* pParentChannel = pChannel->GetParentChannel();
            while(pParentChannel)
            {
                pParentChannel->SetSendValueChanged(true);
                pParentChannel = pParentChannel->GetParentChannel();
            }

            // 修改父控制块的变位标识
            CGooseCb* pGooseCb = (CGooseCb*)pChannel->GetParentBaseCb();
            if (pGooseCb)
            {
                pGooseCb->SetSendValueChanged(true);
            }
        }
    }

    return true;
}
