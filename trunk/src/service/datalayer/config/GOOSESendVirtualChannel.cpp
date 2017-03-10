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
#include "GooseCb.h"
#include "GOOSESendVirtualChannel.h"

///
/// @brief
///     ���캯��
///
CGOOSESendVirtualChannel::CGOOSESendVirtualChannel(void)
:m_strName(L"")
{
    m_vecChannelGroup.clear();
}

///
/// @brief
///     ��������
///
CGOOSESendVirtualChannel::~CGOOSESendVirtualChannel(void)
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
std::wstring CGOOSESendVirtualChannel::GetName()
{
    return m_strName;
}

///
/// @brief
///     ��������ͨ����
///
/// @param[in]  strName ����ͨ����
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
///    ö�ٵ�һ��Channel
///
/// @return
///     CChannel*    �ɹ�ʱ����ͨ��ָ�룬ʧ��ʱ����NULL
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
///    ö����һ��Channel
///
/// @param[in]  pCurChannel ��ǰChannel��ָ��
///
/// @return
///     CChannel*    �ɹ�ʱ����Channelָ�룬ʧ��ʱ����NULL
///
CChannel* CGOOSESendVirtualChannel::NextChannel( CChannel* pCurChannel )
{
    CChannel*                           pChannel   = NULL;
    std::vector<CChannel*>::iterator    iter;


    iter = m_vecChannelGroup.begin();
    while (m_vecChannelGroup.end() != iter)
    {
        pChannel = *iter;

        // ���ҵ�ǰGOOSE���ƿ��λ��
        if (pChannel == pCurChannel)
        {
            // �����һ��GOOSE���ƿ��λ��
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
///    �Ƿ�����ʵͨ��ӳ�䵽������ͨ��
///
/// @return
///     bool    �з���true
///
bool CGOOSESendVirtualChannel::IsMaped()
{
    return (m_vecChannelGroup.size() > 0);
}

///
/// @brief
///     ��������ӳ�䵽������ͨ������ʵͨ���ķ���ֵ
///
/// @param[in]  bNewValue   �µķ���ֵ
///
/// @return
///     bool    �ɹ�����true
///
bool CGOOSESendVirtualChannel::SetChannelSendValue( bool bNewValue )
{
    CChannel*                           pChannel   = NULL;
    std::vector<CChannel*>::iterator    iter;


    // ��������ͨ��ӳ������ÿһ��GOOSE������ʵͨ��
    iter = m_vecChannelGroup.begin();
    for (; iter != m_vecChannelGroup.end(); iter++)
    {
        pChannel = *iter;
        if (pChannel)
        {
            // ���GOOSEͨ����ǰ�ķ���ֵ
            GOOSE_DATA_VARIANT m_stGooseSendValue = pChannel->GetGooseSendValue();

            // �޸ĵ�������ͨ���ķ���ֵ
            if (CHANNEL_TYPE_GOOSE_POINT == pChannel->GetChannelType())
            {
                m_stGooseSendValue.m_stBoolean.boolVal = bNewValue;
                pChannel->SetGooseSendValue(m_stGooseSendValue);
            }
            // �޸�˫������ͨ���ķ���ֵ
            else if (CHANNEL_TYPE_GOOSE_DOUBLE_POINT == pChannel->GetChannelType())
            {
                m_stGooseSendValue.m_stBitString.SetString(strDPValue[bNewValue? DP_VALUE_ON: DP_VALUE_OFF]);
                pChannel->SetGooseSendValue(m_stGooseSendValue);
            }
            // ����
            else
            {
                return false;
            }

            // �޸�ͨ���ı�λ��ʶ
            pChannel->SetSendValueChanged(true);

            // �޸ĸ�ͨ���ı�λ��ʶ
            CChannel* pParentChannel = pChannel->GetParentChannel();
            while(pParentChannel)
            {
                pParentChannel->SetSendValueChanged(true);
                pParentChannel = pParentChannel->GetParentChannel();
            }

            // �޸ĸ����ƿ�ı�λ��ʶ
            CGooseCb* pGooseCb = (CGooseCb*)pChannel->GetParentBaseCb();
            if (pGooseCb)
            {
                pGooseCb->SetSendValueChanged(true);
            }
        }
    }

    return true;
}
